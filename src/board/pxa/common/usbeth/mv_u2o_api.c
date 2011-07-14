
//#define DEBUG
//#include <configs/zylonite2.h>
#include <common.h>

#include <linux/types.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <common.h>
#include <config.h>
#include <net.h>
#include <command.h>
#include <malloc.h>

#include "pxa_usb.h"
#include "mv_u2o_ctl.h"
#include "mv_u2o_api.h"
#include "mvUsbCh9.h"
#include "mvUsbDevApi.h"


/*****************************************************************************
 * BSP related functions
 *****************************************************************************/

#if 0
#define U2O_MEM_SIZE 0x20000 // 128KB memory
static unsigned char mem_base[U2O_MEM_SIZE];
static unsigned long mem_ptr = (unsigned long)&mem_base[0];
static unsigned long mem_size = U2O_MEM_SIZE;

static unsigned long dma_base = 0;
static unsigned long dma_ptr = 0;
#endif
static unsigned long dma_size = 0x10000;

static struct usb_request setup_req;

#undef U2O_MALLOC  /* cache flush/invalidate function fail, don't use malloc here */

#ifdef DEBUG
#define PRINTF_BUFFER_LENGTH 1024
static char printf_buffer[PRINTF_BUFFER_LENGTH];
#endif

extern void usb_driver_speed(int speed);
extern void ep2_begin(void);
extern void mvUsbCh9SetConfig(_usb_device_handle handle,
			boolean setup, SETUP_STRUCT *setup_ptr);
extern int mv_usb_ep_enable(struct mv_usb_ep *usb_ep, ep_desc_t *ep_desc);

#if 0
#define __raw_readl(addr) 	(*(volatile u32 *) (addr))
#define readl(addr) 		(*(volatile u32 *) (addr))
#define writel(b, addr)		((*(volatile u32 *) (addr)) = (b))

static void mvOsPrintf(const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(printf_buffer, PRINTF_BUFFER_LENGTH, fmt, args);
	va_end(args);

	SerialOutputString(printf_buffer);
}
#endif

static void *u2o_dma_malloc(void *dev, uint_32 size, uint_32 align,
                                   unsigned long* paddr)
{
	void *ptr;

#if 0
	if (!dma_base) {
		dma_base = dma_ptr = malloc(dma_size);
	}
	if (size & (align-1)) {
		size |= (align-1);
		size += 1;
	}
	ptr = (void *)dma_ptr;
	dma_ptr += size;
	if (dma_ptr > (dma_base + dma_size)) {
		printf("lack of u2o dma mem, dma_base %x dma_ptr %x size %x\n",
			(int)dma_base, (int)dma_ptr, (int)dma_size);
		return NULL;
	}
	*paddr = ptr;
	
	USB_printf("%s ptr %p size %x\n", __func__, ptr, (int)size);
#else
	ptr = malloc(dma_size);
	*paddr = (unsigned long)ptr;
#endif
	return ptr;
}

static void u2o_dma_free(void *pDev, uint_32 size, unsigned long phyAddr, void *pVirtAddr)
{
#if 0
	USB_printf("%s!!! ptr %p\n\n", __func__, ptr);
	dma_ptr = dma_base;
	memset((void*)dma_base, 0, sizeof(dma_base));
#else
	free(pVirtAddr);
#endif
}

void *u2o_malloc(unsigned int size)
{
#ifdef U2O_MALLOC
	void *ptr = (void *)mem_ptr;

	if (size & 0x1f) {
		size |= 0x1f;
		size += 1;
	}
	mem_ptr += size;
	if ((unsigned)mem_ptr > ((unsigned)(&mem_base[0]) + mem_size)) {
		printf("lack of u2o mem\n");
		return NULL;
	}
	printf("%s ptr %p size %x\n", __func__, ptr, (int)size);
	return ptr;
#else
	return malloc(size);
#endif
}

static void u2o_free(void * ptr)
{
#ifdef U2O_MALLOC
	printf("%s not real, ptr %p\n\n", __func__, ptr);
	mem_ptr = (unsigned long)&mem_base[0];
	memset(mem_base, 0, sizeof(mem_base));
#else
//	u2o_dma_free(0);
	free(ptr);
#endif
}

#if 0
static void dma_flush_range(unsigned start, unsigned end)
{
	/* still has problem here, may cause data crupt */
	asm (
	"bic     r0, r0, #31 \n\t"
	"1: \n\t"
        "mcr     p15, 0, r0, c7, c14, 1          @ clean/invalidate L1 D line \n\t"
        "mcr     p15, 1, r0, c7, c11, 1          @ clean L2 line \n\t"
        "mcr     p15, 1, r0, c7, c7, 1           @ invalidate L2 line \n\t"
        "add     r0, r0, #32 \n\t"
        "cmp     r0, r1 \n\t"
        "blo     1b \n\t"
        "mcr     p15, 0, r0, c7, c10, 4          @ data write barrier \n\t");
}
static void dma_inv_range(unsigned start, unsigned end)
{
	/* still has problem here, may cause hang */
	asm (
	"tst     r0, #31 \n\t"
        "bic     r0, r0, #31 \n\t"
        "mcrne   p15, 0, r0, c7, c10, 1          @ clean L1 D line \n\t"
        "mcrne   p15, 1, r0, c7, c11, 1          @ clean L2 line \n\t"
        "tst     r1, #31 \n\t"
        "mcrne   p15, 0, r1, c7, c10, 1          @ clean L1 D line \n\t"
        "mcrne   p15, 1, r1, c7, c11, 1          @ clean L2 line \n\t" 
	"1: \n\t"
	"mcr     p15, 0, r0, c7, c6, 1           @ invalidate L1 D line \n\t"
        "mcr     p15, 1, r0, c7, c7, 1           @ invalidate L2 line \n\t" 
        "add     r0, r0, #32 \n\t"
        "cmp     r0, r1 \n\t"
        "blo     1b \n\t" 
        "mcr     p15, 0, r0, c7, c10, 4          @ data write barrier \n\t");
}
#endif

static unsigned long u2o_cache_flush(void *dev, void *vaddr, int size)
{
#ifdef U2O_MALLOC
	unsigned start = (unsigned)vaddr, end = ((unsigned)vaddr + size);
	USB_printf("%s start %p size %d end %p\n\n", __func__, start, size, end);
	dma_flush_range(start, end);
#endif
	return 0;
}

static unsigned long u2o_cache_invalidate(void *dev, void *vaddr, int size)
{
#ifdef U2O_MALLOC
	unsigned start = (unsigned)vaddr, end = (unsigned)vaddr + size;
	
	USB_printf("%s start %p size %d end %p\n\n", __func__, start, size, end);
	dma_inv_range(start, end);
#endif
	return 0;
}
static unsigned long u2o_virt_to_phys(void* dev, void* vaddr)
{
	return ((unsigned long)vaddr);
}

static uint_32 u2o_get_caps_addr(int dev_num)
{
	struct mv_usb_dev *mv_dev = &the_controller;
	
	return (unsigned int)(mv_dev->regbase + (0x100 >> 2));
}

static void u2o_reset_complete(int dev_num)
{
	struct mv_usb_dev *mv_dev = &the_controller;
	/* Set USB_MODE register */
	u2o_write(mv_dev->regbase, U2xUSBMODE, U2xUSBMODE_CM_DEVICE | 
		U2xUSBMODE_SLOM_OFF | U2xUSBMODE_SDIS_ACTIVE);
	USB_printf("%s end USBMODE %x\n", __func__, u2o_get(mv_dev->regbase, U2xUSBMODE));
}

void mv_usb_ep_cancel_all_req(struct mv_usb_ep *mv_ep)
{
    struct mv_usb_dev*      mv_dev = &the_controller;
    int                     req_cntr, tr_cntr;

    req_cntr = tr_cntr = 0;

    /* Cancel all transfers */
    while(_usb_device_get_transfer_status(mv_dev->mv_usb_handle, mv_ep->num, 
           mv_ep->is_in ? ARC_USB_SEND : ARC_USB_RECV) != ARC_USB_STATUS_IDLE)
   {
        tr_cntr++;
       _usb_device_cancel_transfer(mv_dev->mv_usb_handle, mv_ep->num, 
                           mv_ep->is_in ? ARC_USB_SEND : ARC_USB_RECV);
    }

    if(tr_cntr > 0)
    {
        mvOsPrintf("Cancel ALL transfers: ep=%d-%s, %d transfers\n", 
                        mv_ep->num, mv_ep->is_in ? "in" : "out", tr_cntr);
    }

    if(req_cntr > 0)
    {
        mvOsPrintf("Cancel ALL Requests: ep=%d-%s, %d requests\n", 
                        mv_ep->num, mv_ep->is_in ? "in" : "out", req_cntr);
        _usb_stats(mv_dev->mv_usb_handle);
    }

}

uint_8 mv_usb_start_ep0(struct mv_usb_dev *mv_dev)
{
    DBGMSG("%s: mv_dev=%p, mv_usb_handle=%p, mv_ep=%p\n", 
           __FUNCTION__, mv_dev, mv_dev->mv_usb_handle, &mv_dev->ep[0]);

    /* Init ep0 IN and OUT */
    mv_dev->ep[0].is_enabled = 1;

    mv_dev->ep[0].maxpacket = 64;
    _usb_device_init_endpoint(mv_dev->mv_usb_handle, 0, mv_dev->ep[0].maxpacket, 
                                ARC_USB_SEND,  ARC_USB_CONTROL_ENDPOINT, 0);

    _usb_device_init_endpoint(mv_dev->mv_usb_handle, 0, mv_dev->ep[0].maxpacket, 
                                ARC_USB_RECV, ARC_USB_CONTROL_ENDPOINT, 0);

    return USB_OK;
}

uint_8 mv_usb_reinit (struct mv_usb_dev *usb_dev)
{
    int                 i, num;
    int                 is_in;
    struct mv_usb_ep    *ep;

    DBGMSG("%s: mv_dev=%p, mv_usb_handle=%p\n", 
           __FUNCTION__, usb_dev, usb_dev->mv_usb_handle);

    num = 0;
    for(i=0; i<2*U2O_USED_ENDPOINTS; i++)
    {
        is_in = i % 2;
        ep = &usb_dev->ep[i];

        ep->usb_dev = usb_dev;
        ep->num = num;
        ep->is_in = is_in;
        ep->is_enabled = 0;

        if(is_in)
            num++;
    }
    return USB_OK;
}

void mv_usb_bus_reset_service(void*      handle, 
                               uint_8     type, 
                               boolean    setup,
                               uint_8     direction, 
                               uint_8_ptr buffer,
                               uint_32    length, 
                               uint_8     error)
{
    struct mv_usb_dev       *mv_dev = &the_controller;
    int                     i;
    struct mv_usb_ep        *mv_ep;

    if(setup == 0)
    {
        /* Stop Hardware and cancel all pending requests */
        for (i=0; i<2*(U2O_USED_ENDPOINTS); i++)
        {
            mv_ep = &mv_dev->ep[i];

            if(mv_ep->is_enabled == 0)
                continue;

            mv_usb_ep_cancel_all_req(mv_ep);
        }

       /* Reinit all endpoints */
        mv_usb_reinit(mv_dev);
    }
    else
    {
        USB_printf("device start, ep0 start\n");    
        _usb_device_start(mv_dev->mv_usb_handle);
        /* Restart Control Endpoint #0 */
        mv_usb_start_ep0(mv_dev);
	
        /* setup the ep0 receive buffer */
        setup_req.buf = u2o_dma_malloc(mv_dev, sizeof(usb_dev_request_t), 0x20, (unsigned long *)&i);
        setup_req.length = sizeof(usb_dev_request_t);
        mv_usb_ep_queue(&mv_dev->ep[0], &setup_req);

    }

    usb_connected = 0;
}


void mv_usb_speed_service(void*      handle, 
                           uint_8     type, 
                           boolean    setup,
                           uint_8     direction, 
                           uint_8_ptr buffer,
                           uint_32    length, 
                           uint_8     error)
{
    DBGMSG("Speed = %s\n", (length == ARC_USB_SPEED_HIGH) ? "High" : "Full");

    if(length == ARC_USB_SPEED_HIGH) 
	usb_speed = USB_SPEED_HIGH;
    else
	usb_speed = USB_SPEED_FULL;

    usb_driver_speed(usb_speed);

    return;
}

void mv_usb_suspend_service(void*      handle, 
                            uint_8     type, 
                            boolean    setup,
                            uint_8     direction, 
                            uint_8_ptr buffer,
                            uint_32    length, 
                            uint_8     error)
{
}

void mv_usb_resume_service(void*      handle, 
                            uint_8     type, 
                            boolean    setup,
                            uint_8     direction, 
                            uint_8_ptr buffer,
                            uint_32    length, 
                            uint_8     error)
{
    DBGMSG("%s\n", __FUNCTION__);

    usbctl_next_state_on_event( kEvResume );
    usb_connected = 0;
}

void mv_usb_tr_complete_service(void*      handle, 
                                 uint_8     type, 
                                 boolean    setup,
                                 uint_8     direction, 
                                 uint_8_ptr buffer,
                                 uint_32    length, 
                                 uint_8     error)
{
    struct mv_usb_dev       *mv_dev = &the_controller;
    struct mv_usb_ep        *mv_ep;
    struct usb_request      usb_req;
    int                     ep_num = (type*2) + direction;

    DBGMSG("%s: ep_num=%d, setup=%s, direction=%s, pBuf=0x%x, length=%d, error=0x%x\n", 
             __FUNCTION__, ep_num, setup ? "YES" : "NO", 
             (direction == ARC_USB_RECV) ? "RECV" : "SEND", 
             (unsigned)buffer, (int)length, error);

    mv_ep = &mv_dev->ep[ep_num];

    usb_req.buf = buffer;
    usb_req.length = length;
    
    if (ep_num == 2)
        ep2_int_hndlr(&usb_req);
    else if (ep_num == 3)
        ep1_int_hndlr(&usb_req);  
    else
    	printf("error ep_num %d\n", ep_num);
}

void mv_usb_ep0_complete_service(void*      handle, 
                                 uint_8     type, 
                                 boolean    setup,
                                 uint_8     direction, 
                                 uint_8_ptr buffer,
                                 uint_32    length, 
                                 uint_8     error)
{ /* Body */
    struct mv_usb_dev       *mv_dev = &the_controller;
    struct mv_usb_ep        *mv_ep;
    struct usb_request      usb_req;
    int                     rc;
    boolean                 is_delegate = FALSE;
    static SETUP_STRUCT     mv_ctrl_req;
   
    DBGMSG("%s: EP0(%d), setup=%s, direction=%s, pBuf=0x%x, length=%d, error=0x%x\n", 
                __FUNCTION__, type, setup ? "YES" : "NO", 
                (direction == ARC_USB_RECV) ? "RECV" : "SEND", 
                (unsigned)buffer, (int)length, error);

    mv_ep = &mv_dev->ep[type];

    if (setup) 
    {
        _usb_device_read_setup_data(handle, type, (u8 *)&mv_ctrl_req);

        while(_usb_device_get_transfer_status(handle, mv_ep->num, 
                ARC_USB_SEND) != ARC_USB_STATUS_IDLE)
        {
            _usb_device_cancel_transfer(mv_dev->mv_usb_handle, mv_ep->num, 
                           ARC_USB_SEND);
        }
        while(_usb_device_get_transfer_status(handle, mv_ep->num, 
                ARC_USB_RECV) != ARC_USB_STATUS_IDLE)
        {
            _usb_device_cancel_transfer(mv_dev->mv_usb_handle, mv_ep->num, 
                           ARC_USB_RECV);
        }
	usb_req.buf = &mv_ctrl_req;
	usb_req.length = sizeof(mv_ctrl_req);
    }
    /* Setup request direction */
    mv_ep->is_in = (mv_ctrl_req.REQUESTTYPE & REQ_DIR_IN) != 0;     

    if(setup)
        DBGMSG("*** Setup ***: dir=%s, reqType=0x%x, req=0x%x, value=0x%02x, index=0x%02x, length=0x%02x\n", 
                (direction == ARC_USB_SEND) ? "In" : "Out",
                mv_ctrl_req.REQUESTTYPE, mv_ctrl_req.REQUEST, mv_ctrl_req.VALUE,
                mv_ctrl_req.INDEX, mv_ctrl_req.LENGTH); 

    /* Handle most lowlevel requests;
     * everything else goes uplevel to the gadget code.
     */
    if( (mv_ctrl_req.REQUESTTYPE & REQ_TYPE_MASK) == REQ_TYPE_STANDARD)
    {
        switch (mv_ctrl_req.REQUEST) 
        {
            case REQ_GET_STATUS: 
                mvUsbCh9GetStatus(handle, setup, &mv_ctrl_req);
                break;

            case REQ_CLEAR_FEATURE:
                mvUsbCh9ClearFeature(handle, setup, &mv_ctrl_req);
                break;

            case REQ_SET_FEATURE:
                mvUsbCh9SetFeature(handle, setup, &mv_ctrl_req);
                break;

            case REQ_SET_ADDRESS:
                mvUsbCh9SetAddress(handle, setup, &mv_ctrl_req);
                break;

            case REQ_SET_CONFIGURATION:
		if (setup) {
			desc_t * pdesc = pxa_usb_get_descriptor_ptr();
		        config_desc_t *cfg;
		        intf_desc_t *intf;
		        ep_desc_t *ep;
		
		        cfg = (config_desc_t*) (pdesc->cdb);
			intf = (intf_desc_t *)((config_desc_t *)(cfg + 1));
		        ep = (ep_desc_t *) (intf + 1);

			USB_printf("\t\tSET_CONFIGURATION\n");
			mvUsbCh9SetConfig(handle, setup, &mv_ctrl_req);
			usbctl_next_state_on_event(kEvReset);
			usbctl_next_state_on_event(kEvConfig);
			mv_usb_ep_enable(&mv_dev->ep[2], &ep[1]);
			mv_usb_ep_enable(&mv_dev->ep[3], &ep[0]);
			usb_connected = 1;
			ep2_begin();
		}
		break;

            case REQ_SET_INTERFACE:
		if (setup) {
			USB_printf("\t\tSET_INTERFACE\n");
			/* ack */
      			_usb_device_send_data(handle, 0, 0, 0);
		}
		break;

            default:
                /* All others delegate call up-layer gadget code */
                is_delegate = TRUE;
        }
    }
    else
        is_delegate = TRUE;

    /* delegate call up-layer gadget code */
    if(is_delegate)
    {
        if(setup)
        {
	    mv_ep->ack_recv = 0;
	    mv_ep->ack_sent = 0;
    
	    rc = ep0_int_hndlr(&usb_req);	
            if(rc < 0)
            {
                mvOsPrintf("Setup is failed: rc=%d, req=0x%02x, reqType=0x%x, value=0x%04x, index=0x%04x\n", 
                    rc, mv_ctrl_req.REQUEST, mv_ctrl_req.REQUESTTYPE, 
                    mv_ctrl_req.VALUE, mv_ctrl_req.INDEX);
                _usb_device_stall_endpoint(handle, 0, ARC_USB_RECV);
                return;
            }
            /* Acknowledge  */
            if( mv_ep->is_in ) {
		mv_ep->ack_recv = 1;
                _usb_device_recv_data(handle, 0, NULL, 0);
            } 
            else if( mv_ctrl_req.LENGTH ) {
		mv_ep->ack_sent = 1;
                _usb_device_send_data(handle, 0, NULL, 0);
            }
        }
    }

    if(!setup)
    {
	int is_ack = 0;
	if( mv_ep->ack_sent && (direction == ARC_USB_SEND) ) {
		mv_ep->ack_sent = 0;
		is_ack = 1;
	}
	if( mv_ep->ack_recv && (direction != ARC_USB_SEND) ) {
		mv_ep->ack_recv = 0;
		is_ack = 1;
	}

        //usb_req->complete (&mv_ep->ep, usb_req);
        DBGMSG("Setup complete: dir=%s, is_in=%d, length=%d, is_ack=%d\n", 
                (direction == ARC_USB_SEND) ? "In" : "Out",
                mv_ep->is_in, length, is_ack);
    }
}

int      mv_usb_ep_queue (struct mv_usb_ep *usb_ep, struct usb_request *_req) 
{
    struct mv_usb_dev* usb_dev =  &the_controller;
    uint_8              error;

    DBGMSG("%s: num=%d-%s, _req=%p, buf=%p, length=%d is_enabled %d\n", 
                __FUNCTION__, usb_ep->num, usb_ep->is_in ? "in" : "out", 
                _req, _req->buf, _req->length, usb_ep->is_enabled);

    /* Add request to list */
    if( ((usb_ep->num == 0) && (_req->length == 0)) || (usb_ep->is_in) )
    {
        int     send_size, size;
        uint_8  *send_ptr, *buf_ptr;

        send_ptr = buf_ptr = _req->buf;
        send_size = size = _req->length;

        error = _usb_device_send_data(usb_dev->mv_usb_handle, usb_ep->num, send_ptr, send_size);
        if(error != USB_OK)
        {
            printf("ep_queue: Can't SEND data (err=%d): ep_num=%d, pBuf=0x%x, send_size=%d\n",
                    error, usb_ep->num, (unsigned)_req->buf, _req->length);
        }

        size -= send_size;
        buf_ptr += send_size;
    }
    else
    {
        error = _usb_device_recv_data(usb_dev->mv_usb_handle, usb_ep->num, _req->buf, _req->length);
        if(error != USB_OK)
        {
            printf("mv_usb_ep_queue: Can't RCV data (err=%d): ep_num=%d, pBuf=0x%x, size=%d\n",
                        error, usb_ep->num, (unsigned)_req->buf, _req->length);
        }
    }

    return (int)error;
}

int  mv_usb_ep_enable(struct mv_usb_ep *usb_ep, ep_desc_t *ep_desc)
{
    struct mv_usb_dev* usb_dev = &the_controller;
    __u16             maxSize;
    uint_8              epType; 

    if(usb_ep->is_enabled)
    {
        mvOsPrintf("mv_usb: %d%s Endpoint is already in use\n", 
                    usb_ep->num, usb_ep->is_in ? "In" : "Out");
        return -EINVAL;
    }

    if(usb_ep->num == 0)
    {
        mvOsPrintf("mv_usb: ep0 is reserved\n");
        return -EINVAL;
    }

    /* Max packet size */
    maxSize = ep_desc->wMaxPacketSize;

    epType = (uint_8)ARC_USB_BULK_ENDPOINT;
    usb_ep->is_enabled = 1;

    mvOsPrintf("Enable EP: ep=%d-%s maxSize %d type %d\n", usb_ep->num, 
	usb_ep->is_in ? "in" : "out", maxSize, epType);

    _usb_device_init_endpoint(usb_dev->mv_usb_handle, usb_ep->num, maxSize, 
            usb_ep->is_in ? ARC_USB_SEND : ARC_USB_RECV, epType,
            (epType == ARC_USB_BULK_ENDPOINT) ? ARC_USB_DEVICE_DONT_ZERO_TERMINATE : 0);

    return 0;
}


USB_IMPORT_FUNCS    usbImportFuncs =
{
#ifdef DEBUG
//	.bspPrintf =            mvOsPrintf,
#endif
	.bspUncachedMalloc = 	u2o_dma_malloc,
	.bspUncachedFree = 	u2o_dma_free,
	.bspMalloc =            u2o_malloc,
	.bspFree =              u2o_free,
	.bspMemset =            memset,
	.bspMemcpy =            memcpy,
	.bspCacheFlush = 	u2o_cache_flush,
	.bspCacheInv =		u2o_cache_invalidate,
	.bspVirtToPhys =	u2o_virt_to_phys,
	.bspGetCapRegAddr =     u2o_get_caps_addr,
	.bspResetComplete =     u2o_reset_complete
};

void mv_usb_dump(void)
{
    	struct mv_usb_dev* mv_dev =  &the_controller;
	
        _usb_regs(mv_dev->mv_usb_handle);
        _usb_status(mv_dev->mv_usb_handle);
        _usb_debug_print_trace_log();

	_usb_ep_status(mv_dev->mv_usb_handle, 0, ARC_USB_RECV);
	_usb_ep_status(mv_dev->mv_usb_handle, 0, ARC_USB_SEND);

	_usb_ep_status(mv_dev->mv_usb_handle, 1, ARC_USB_RECV);
	_usb_ep_status(mv_dev->mv_usb_handle, 1, ARC_USB_SEND);
}

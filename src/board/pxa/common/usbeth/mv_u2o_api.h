
#include "mv_u2o_ctl.h"
#include "mvUsbCh9.h"
#include "mvUsbDevApi.h"

//#define DEBUG

#ifdef DEBUG
//#define mvOsPrintf(fmt,args...)	 printf( fmt, ## args)
#define mvOsPrintf	printf
#else
#define mvOsPrintf(frmt, x...)
#endif


#if 0
#define __raw_readl(addr) 	(*(volatile u32 *) (addr))
#define readl(addr) 		(*(volatile u32 *) (addr))
#define writel(b, addr)		((*(volatile u32 *) (addr)) = (b))
#endif

extern unsigned u2o_get(unsigned *base, unsigned offset);
extern void u2o_set(unsigned *base, unsigned offset, unsigned value);
extern void u2o_clear(unsigned *base, unsigned offset, unsigned value);
extern void u2o_write(unsigned *base, unsigned offset, unsigned value);

extern void mv_usb_dump(void);

extern uint_8 mv_usb_start_ep0(struct mv_usb_dev *mv_dev);

extern int mv_usb_ep_queue (struct mv_usb_ep *usb_ep, struct usb_request *_req); 

extern void mv_usb_bus_reset_service(void *handle, uint_8 type, boolean setup,
	uint_8 direction, uint_8_ptr buffer, uint_32 length, uint_8 error);

extern void mv_usb_speed_service(void *handle, uint_8 type, boolean setup,
	uint_8 direction, uint_8_ptr buffer, uint_32 length, uint_8 error);

extern void mv_usb_suspend_service(void *handle, uint_8 type, boolean setup,
	uint_8 direction, uint_8_ptr buffer, uint_32 length, uint_8 error);

extern void mv_usb_resume_service(void *handle, uint_8 type, boolean setup,
	uint_8 direction, uint_8_ptr buffer, uint_32 length, uint_8 error);

extern void mv_usb_ep0_complete_service(void *handle, uint_8 type, boolean setup,
	uint_8 direction, uint_8_ptr buffer, uint_32 length, uint_8 error);

extern void mv_usb_tr_complete_service(void *handle, uint_8 type, boolean setup,
	uint_8 direction, uint_8_ptr buffer, uint_32 length, uint_8 error);

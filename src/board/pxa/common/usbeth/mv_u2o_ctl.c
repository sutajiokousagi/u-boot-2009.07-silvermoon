/*
 *  Copyright (C) Compaq Computer Corporation, 1998, 1999
 *  Copyright (C) Extenex Corporation, 2001
 *  Copyright (C) Intrinsyc, Inc., 2002
 *
 * (C) Copyright 2006 Marvell International Ltd.  
 * All Rights Reserved 
 *
 *  PXA USB controller core driver.
 *
 *  This file provides interrupt routing and overall coordination
 *  of the endpoints.
 *
 *  Please see:
 *    linux/Documentation/arm/SA1100/SA1100_USB 
 *  for more info.
 *
 *  02-May-2002
 *   Frank Becker (Intrinsyc) - derived from sa1100 usb_ctl.c
 *
 */

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
#include "mv_u2o_api.h"
#include "mv_u2o_ctl.h"
#include "mvUsbCh9.h"
#include "mvUsbDevApi.h"
#include <asm/arch/common.h>

#ifdef TAVOR_EVB2
#include "arch/tavor_evb2.h"
#endif

#ifdef CONFIG_CPU_MONAHANS
#include <asm/arch/pxa-regs.h>
#endif

#undef DEBUG 
//#define DEBUG 3
#if DEBUG
static unsigned int usb_debug = DEBUG;
#else
#define usb_debug 0     /* gcc will remove all the debug code for us */
#endif

static int u2o_info(void);

struct mv_usb_dev       the_controller;

extern USB_IMPORT_FUNCS    usbImportFuncs;
void usb_driver_reset(void);
extern void *u2o_malloc(unsigned long size);

//////////////////////////////////////////////////////////////////////////////
// Prototypes
//////////////////////////////////////////////////////////////////////////////

int usbctl_next_state_on_event( int event );
void udc_int_hndlr(int, void *);
static void initialize_descriptors( void );
static void soft_connect_hook( int enable );
static void u2o_phy_init(unsigned *base);
static void u2o_disable(void);
static int u2o_enable(void);

int * ep0_send_buf;
int * ep1_buf;
int * ep2_buf;

/////////////////////////////////////////////////////////////////////////////
int usb_connected = 0;
int usb_set_conf = 0;
int usb_set_intf = 0;
int usb_speed = USB_SPEED_FULL;


//////////////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////////////
static const char pszMe[] = "usbctl: ";
struct usb_info_t usbd_info;  /* global to ep0, usb_recv, usb_send */

/* device descriptors */
static desc_t desc;

#define MAX_STRING_DESC 16
static string_desc_t * string_desc_array[ MAX_STRING_DESC ];
static string_desc_t sd_zero;  /* special sd_zero holds language codes */

// called when configured
static usb_notify_t configured_callback = NULL;

enum {
    kStateZombie		= 0,
    kStateZombieSuspend		= 1,
    kStateDefault		= 2,
    kStateDefaultSuspend	= 3,
    kStateAddr			= 4,
    kStateAddrSuspend		= 5,
    kStateConfig		= 6,
    kStateConfigSuspend		= 7
};

/*
 * FIXME: The PXA UDC handles several host device requests without user 
 * notification/intervention. The table could be collapsed quite a bit...
 */
static int device_state_machine[8][6] = {
//              suspend               reset          resume         adddr       config        deconfig
/* zombie */  { kStateZombieSuspend , kStateDefault, kStateZombie , kError    , kError      , kError },
/* zom sus */ { kStateZombieSuspend , kStateDefault, kStateZombie , kError    , kError      , kError },
/* default */ { kStateDefaultSuspend, kStateDefault, kStateDefault, kStateAddr, kStateConfig, kError },
/* def sus */ { kStateDefaultSuspend, kStateDefault, kStateDefault, kError    , kError      , kError },
/* addr */    { kStateAddrSuspend   , kStateDefault, kStateAddr   , kError    , kStateConfig, kError },
/* addr sus */{ kStateAddrSuspend   , kStateDefault, kStateAddr   , kError    , kError      , kError },
/* config */  { kStateConfigSuspend , kStateDefault, kStateConfig , kError    , kError      , kStateDefault },
/* cfg sus */ { kStateConfigSuspend , kStateDefault, kStateConfig , kError    , kError      , kError }
};

/* "device state" is the usb device framework state, as opposed to the
   "state machine state" which is whatever the driver needs and is much
   more fine grained
*/
static int sm_state_to_device_state[8] = { 
//  zombie            zom suspend       
USB_STATE_POWERED, USB_STATE_SUSPENDED, 
//  default           default sus
USB_STATE_DEFAULT, USB_STATE_SUSPENDED,
//  addr              addr sus         
USB_STATE_ADDRESS, USB_STATE_SUSPENDED, 
//  config            config sus
USB_STATE_CONFIGURED, USB_STATE_SUSPENDED
};

static char * state_names[8] =
{ "zombie", "zombie suspended", 
  "default", "default suspended",
  "address", "address suspended", 
  "configured", "config suspended"
};

static char * event_names[6] =
{ "suspend", "reset", "resume",
  "address assigned", "configure", "de-configure"
};

static char * device_state_names[] =
{ "not attached", "attached", "powered", "default",
  "address", "configured", "suspended" };

static int sm_state = kStateZombie;



//////////////////////////////////////////////////////////////////////////////
// Async
//////////////////////////////////////////////////////////////////////////////

/* The UDCCR reg contains mask and interrupt status bits,
 * so using '|=' isn't safe as it may ack an interrupt.
 */

void u2o_int_hndlr(void)
{
	struct mv_usb_dev *mv_dev = &the_controller;

	/*if (usb_debug) printf("%s U2xSTS=%08x\n",
		pszMe, u2o_get(mv_dev->regbase, U2xUSBSTS));*/

	/* handle ARC USB Device interrupts */
    	_usb_dci_vusb20_isr(mv_dev->mv_usb_handle);

}

/*****************************************************************************
 * The registers read/write routines
 *****************************************************************************/
unsigned u2o_get(unsigned *base, unsigned offset)
{
	return readl(base + (offset>>2));
}

void u2o_set(unsigned *base, unsigned offset, unsigned value)
{
	volatile unsigned int reg;

	if (usb_debug)
		printf("base %p off %x base+off %p read %x\n", base, offset,
			(base + (offset>>2)), *(unsigned *)(base + (offset>>2)));
	reg = readl(base + (offset>>2));
	reg |= value;
	writel(reg, base + (offset>>2));
	readl(base + (offset>>2));
}

void u2o_clear(unsigned *base, unsigned offset, unsigned value)
{
	volatile unsigned int reg;

	reg = readl(base + (offset>>2));
	reg &= ~value;
	writel(reg, base + (offset>>2));
	readl(base + (offset>>2));
}

void u2o_write(unsigned *base, unsigned offset, unsigned value)
{
	writel(value, base + (offset>>2));
	readl(base + (offset>>2));
}

static void u2o_phy_dump(unsigned *base)
{
#ifndef CONFIG_CPU_MONAHANS
	if (cpu_is_pxa910_168()) {
		printf(" UTMI_REVISION   0x%x\n", u2o_get(base,  UTMI_REVISION  ));
		printf(" UTMI_CTRL       0x%x\n", u2o_get(base,  UTMI_CTRL      ));
		printf(" UTMI_PLL        0x%x\n", u2o_get(base,  UTMI_PLL       ));
		printf(" UTMI_TX         0x%x\n", u2o_get(base,  UTMI_TX        ));
		printf(" UTMI_RX         0x%x\n", u2o_get(base,  UTMI_RX        ));
		printf(" UTMI_IVREF      0x%x\n", u2o_get(base,  UTMI_IVREF     ));
		printf(" UTMI_T0         0x%x\n", u2o_get(base,  UTMI_T0        ));
		printf(" UTMI_T1         0x%x\n", u2o_get(base,  UTMI_T1        ));
		printf(" UTMI_T2         0x%x\n", u2o_get(base,  UTMI_T2        ));
		printf(" UTMI_T3         0x%x\n", u2o_get(base,  UTMI_T3        ));
		printf(" UTMI_T4         0x%x\n", u2o_get(base,  UTMI_T4        ));
		printf(" UTMI_T5         0x%x\n", u2o_get(base,  UTMI_T5        ));
		printf(" UTMI_RESERVE    0x%x\n", u2o_get(base,  UTMI_RESERVE   ));
		printf(" UTMI_USB_INT    0x%x\n", u2o_get(base,  UTMI_USB_INT   ));
		printf(" UTMI_DBG_CTL    0x%x\n", u2o_get(base,  UTMI_DBG_CTL   ));
		printf(" UTMI_OTG_ADDON  0x%x\n", u2o_get(base,  UTMI_OTG_ADDON ));
	} else {
		printf(" U2PRSRVD  0x%x\n", u2o_get(base,  U2PRSRVD  ));
		printf(" U2PCTRL   0x%x\n", u2o_get(base,  U2PCTRL   ));
		printf(" U2PPLL    0x%x\n", u2o_get(base,  U2PPLL    ));
		printf(" U2PTX     0x%x\n", u2o_get(base,  U2PTX     ));
		printf(" U2PRX     0x%x\n", u2o_get(base,  U2PRX     ));
		printf(" U2PIVREF  0x%x\n", u2o_get(base,  U2PIVREF  ));
		printf(" U2PT0     0x%x\n", u2o_get(base,  U2PT0     ));
		printf(" U2PT1     0x%x\n", u2o_get(base,  U2PT1     ));
		printf(" U2PT2     0x%x\n", u2o_get(base,  U2PT2     ));
		printf(" U2PID     0x%x\n", u2o_get(base,  U2PID     ));
		printf(" U2PINT    0x%x\n", u2o_get(base,  U2PINT    ));
		printf(" U2PDBGCTL 0x%x\n", u2o_get(base,  U2PDBGCTL ));
		printf(" U2PCTL1   0x%x\n", u2o_get(base,  U2PCTL1   ));
		printf(" U2PT3     0x%x\n", u2o_get(base,  U2PT3     ));
		printf(" U2PT4     0x%x\n", u2o_get(base,  U2PT4     ));
		printf(" U2PT5     0x%x\n", u2o_get(base,  U2PT5     ));
	}
#endif
}

#if CONFIG_CPU_MONAHANS
static void u2o_phy_init(unsigned *base)
{
	int count;

	if (usb_debug)
		printf("init phy\n\n");

	/* check whether U2PPLL[READY] is set */
	count = 100000;
	do {
		count--;
	} while (!(u2o_get(base, U2PPLL) & U2PPLL_READY) && count);
	if (count <= 0) {
		printf("%s TIMEOUT for U2PPLL[READY]\n", __func__);
	}

	/* PLL VCO and TX Impedance Calibration Timing:
	 *
	 * PU             _____|----------------------------------|
	 * VOCAL STAR     ______________|--|______________________|
	 * REG_RCAL_START ___________________________|--|_________|
	 *                     | 200us  |40| 200us   |40| 200us   |USB PHY READY
	 */

	/* SET REG_ARC_DPDM_MODE BIT OF U2PT0 */
	u2o_set(base, U2PT0, U2PT0_REG_ARC_DPDM_MODE);

	/* U2PPLL */
	u2o_clear(base, U2PPLL, (U2PPLL_ICP_MASK | U2PPLL_KVCO_MASK));
	u2o_set(base, U2PPLL, (0x7<<U2PPLL_ICP_SHIFT) 	\
		| (0x7<<U2PPLL_KVCO_SHIFT) | U2PPLL_KVCO_EXT);

	/* U2PRX, program squelch threshold to rise minumum 
	 * sensitivity for receiving
	 */
	u2o_set(base, U2PRX, (0xf << U2PRX_SQ_THRESH_SHIFT));

	/* IVREF */
	u2o_clear(base, U2IVREF, (U2IVREF_BG_VSEL_MASK | U2IVREF_RXVDD18_MASK));
	udelay(200);

	/* U2PTX */
	u2o_set(base, U2PTX, U2PTX_RCAL_START);
	udelay(200);
	u2o_clear(base, U2PTX, U2PTX_RCAL_START);
	udelay(200);
	
	/* Make sure PHY is ready */
	count = 100000;
	do {
		count--;
	} while (!(u2o_get(base, U2PPLL) & U2PPLL_READY) && count);
	if (count <= 0) {
		printf("%s TIMEOUT for U2PPLL[READY]\n", __func__);
	}
	if (usb_debug)
		u2o_phy_dump(base);
}
#else
static void u2o_phy_init(unsigned *base)
{
	int count;

	if (usb_debug)
		printf("init phy\n\n");


	/* Initialize the USB PHY power */
	if (cpu_is_pxa910_910()) {
		u2o_set(base, UTMI_CTRL, (1<<UTMI_CTRL_INPKT_DELAY_SOF_SHIFT)
				| (1<<UTMI_CTRL_PU_REF_SHIFT));
	}

	u2o_set(base, UTMI_CTRL, 1<<UTMI_CTRL_PLL_PWR_UP_SHIFT);
	u2o_set(base, UTMI_CTRL, 1<<UTMI_CTRL_PWR_UP_SHIFT);

	/* UTMI_PLL settings */
	u2o_clear(base, UTMI_PLL, UTMI_PLL_PLLVDD18_MASK
			| UTMI_PLL_PLLVDD12_MASK | UTMI_PLL_PLLCALI12_MASK
			| UTMI_PLL_FBDIV_MASK | UTMI_PLL_REFDIV_MASK
			| UTMI_PLL_ICP_MASK | UTMI_PLL_KVCO_MASK);

	u2o_set(base, UTMI_PLL, 0xee<<UTMI_PLL_FBDIV_SHIFT
			| 0xb<<UTMI_PLL_REFDIV_SHIFT | 3<<UTMI_PLL_PLLVDD18_SHIFT
			| 3<<UTMI_PLL_PLLVDD12_SHIFT | 3<<UTMI_PLL_PLLCALI12_SHIFT
			| 2<<UTMI_PLL_ICP_SHIFT | 3<<UTMI_PLL_KVCO_SHIFT);

	/* UTMI_TX */
	u2o_clear(base, UTMI_TX, UTMI_TX_TXVDD12_MASK
			| UTMI_TX_CK60_PHSEL_MASK | UTMI_TX_IMPCAL_VTH_MASK);
	u2o_set(base, UTMI_TX, 3<<UTMI_TX_TXVDD12_SHIFT
			| 4<<UTMI_TX_CK60_PHSEL_SHIFT | 5<<UTMI_TX_IMPCAL_VTH_SHIFT);

	/* UTMI_RX */
	u2o_clear(base, UTMI_RX, UTMI_RX_SQ_THRESH_MASK
			| UTMI_REG_SQ_LENGTH_MASK);
	if (cpu_is_pxa910_168())
		u2o_set(base, UTMI_RX, 7<<UTMI_RX_SQ_THRESH_SHIFT
				| 2<<UTMI_REG_SQ_LENGTH_SHIFT);
	else
		u2o_set(base, UTMI_RX, 0xa<<UTMI_RX_SQ_THRESH_SHIFT
				| 2<<UTMI_REG_SQ_LENGTH_SHIFT);

	/* UTMI_IVREF */
	if (cpu_is_pxa910_168())
		/* fixing Microsoft Altair board interface with NEC hub issue -
		 * Set UTMI_IVREF from 0x4a3 to 0x4bf */
		u2o_write(base, UTMI_IVREF, 0x4bf);

	/* calibrate */
	count = 10000;
	while(((u2o_get(base, UTMI_PLL) & PLL_READY)==0) && count--);
	if (count <= 0) printf("%s %d: calibrate timeout, UTMI_PLL %x\n",
		__func__, __LINE__, u2o_get(base, UTMI_PLL));

	/* toggle VCOCAL_START bit of UTMI_PLL */
	udelay(200);
	u2o_set(base, UTMI_PLL, VCOCAL_START);
	udelay(40);
	u2o_clear(base, UTMI_PLL, VCOCAL_START);

	/* toggle REG_RCAL_START bit of UTMI_TX */
	udelay(200);
	u2o_set(base, UTMI_TX, REG_RCAL_START);
	udelay(40);
	u2o_clear(base, UTMI_TX, REG_RCAL_START);
	udelay(200);

	/* make sure phy is ready */
	count = 1000;
	while(((u2o_get(base, UTMI_PLL) & PLL_READY)==0) && count--);
	if (count <= 0) printf("%s %d: calibrate timeout, UTMI_PLL %x\n",
		__func__, __LINE__, u2o_get(base, UTMI_PLL));

	if (cpu_is_pxa910_168()) {
		u2o_set(base, UTMI_RESERVE, 1<<5);
		u2o_write(base, UTMI_OTG_ADDON, 1);  /* Turn on UTMI PHY OTG extension */
	}

	if (usb_debug)
		u2o_phy_dump(base);
}
#endif

//////////////////////////////////////////////////////////////////////////////
// Public Interface
//////////////////////////////////////////////////////////////////////////////

/* Open PXA usb core on behalf of a client, but don't start running */

int
pxa_usb_open( const char * client )
{
	if (usb_debug) printf( "pxa_usb_open\n");
	usbd_info.client_name = (char*) client;
	memset(&usbd_info.stats, 0, sizeof(struct usb_stats_t));
	memset(string_desc_array, 0, sizeof(string_desc_array));

	/* hack to start in zombie suspended state */
	sm_state = kStateZombieSuspend;
	usbd_info.state = USB_STATE_SUSPENDED;

	/* create descriptors for enumeration */
	initialize_descriptors();

	ep1_init(0);
	if (usb_debug) printf( "%s%s registered.\n", pszMe, client );
	return 0;
}

/* Start running. Must have called usb_open (above) first */
int
pxa_usb_start( void )
{
	if (usb_debug) printf( "pxa_usb_start\n");

	if ( usbd_info.client_name == NULL ) {
		printf( "%s%s - no client registered\n",
				pszMe, __FUNCTION__ );
		return -EPERM;
	}

	if (usb_debug) u2o_info();

	if (usb_debug) printf( "%sStarted %s\n", pszMe, usbd_info.client_name );
	return 0;
}

/* Stop USB core from running */
int
pxa_usb_stop( void )
{
	if ( usbd_info.client_name == NULL ) {
		printf( "%s%s - no client registered\n",
				pszMe, __FUNCTION__ );
		return -EPERM;
	}

	ep1_reset();
	ep2_reset();

	u2o_disable();
	if( usb_debug) printf( "%sStopped %s\n", pszMe, usbd_info.client_name );
	return 0;
}

/* Tell PXA core client is through using it */
int
pxa_usb_close( void )
{
	 if ( usbd_info.client_name == NULL ) {
		   printf( "%s%s - no client registered\n",
				  pszMe, __FUNCTION__ );
		  return -EPERM;
	 }
	 printf( "%s%s closed.\n", pszMe, (char*)usbd_info.client_name );
	 usbd_info.client_name = NULL;
	 return 0;
}

/* set a proc to be called when device is configured */
usb_notify_t pxa_set_configured_callback( usb_notify_t func )
{
	 usb_notify_t retval = configured_callback;
	 configured_callback = func;
	 return retval;
}

/*====================================================
 * Descriptor Manipulation.
 * Use these between open() and start() above to setup
 * the descriptors for your device.
 *
 */

/* get pointer to static default descriptor */
desc_t *
pxa_usb_get_descriptor_ptr( void ) { return &desc; }

/* optional: set a string descriptor */
int
pxa_usb_set_string_descriptor( int i, string_desc_t * p )
{
	 int retval;
	 if ( i < MAX_STRING_DESC ) {
		  string_desc_array[i] = p;
		  retval = 0;
	 } else {
		  retval = -EINVAL;
	 }
	 return retval;
}

/* optional: get a previously set string descriptor */
string_desc_t *
pxa_usb_get_string_descriptor( int i )
{
	 return ( i < MAX_STRING_DESC )
		    ? string_desc_array[i]
		    : NULL;
}

config_desc_t *
pxa_usb_get_config(int cfgval) 
{
	int i;
	desc_t * pdesc = pxa_usb_get_descriptor_ptr();
	config_desc_t *cfg = (config_desc_t*) (pdesc->cdb);

	for( i=0; i<pdesc->dev.bNumConfigurations; i++) {
		if( cfg->bConfigurationValue == cfgval ) return cfg;
		cfg = (config_desc_t*) ((unsigned char*)cfg + cfg->wTotalLength);
	}

	return NULL;
}

intf_desc_t *
pxa_usb_get_interface( config_desc_t *cfg, int idx)
{
	int i;
	intf_desc_t *intf = (intf_desc_t*) (cfg + 1);
	
	for( i=0; i < cfg->bNumInterfaces; i++) {
		if( idx == intf->bInterfaceNumber) return intf;
		intf++;
	}

	return NULL;
}


ep_desc_t *
pxa_usb_get_endpoint( intf_desc_t *intf, int idx)
{
	int i;
	ep_desc_t *ep = (ep_desc_t *) (intf+1);


	for( i=0; i< intf->bNumEndpoints; i++) {
		if( idx == (ep->bEndpointAddress & 0xF) ) return ep;
		ep++;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////
// Exports to rest of driver
//////////////////////////////////////////////////////////////////////////////

/* called by the int handler here and the two endpoint files when interesting
   .."events" happen */

int
usbctl_next_state_on_event( int event )
{
	int next_state = device_state_machine[ sm_state ][ event ];
	if ( next_state != kError )
	{
		int next_device_state = sm_state_to_device_state[ next_state ];
		if (usb_debug) printf( "%s%s --> [%s] --> %s. Device in %s state.\n",
				pszMe, state_names[ sm_state ], event_names[ event ],
				state_names[ next_state ], device_state_names[ next_device_state ] );

		sm_state = next_state;
		if ( usbd_info.state != next_device_state )
		{
			if ( configured_callback != NULL
				 &&
				 next_device_state == USB_STATE_CONFIGURED
				 &&
				 usbd_info.state != USB_STATE_SUSPENDED
			   ) {
			  configured_callback();
			}
			usbd_info.state = next_device_state;

			ep1_state_change_notify( next_device_state );
			ep2_state_change_notify( next_device_state );
		}
	}
	else
		printf( "%s%s --> [%s] --> ??? is an error.\n",
				pszMe, state_names[ sm_state ], event_names[ event ] );
	return next_state;
}

//////////////////////////////////////////////////////////////////////////////
// Private Helpers
//////////////////////////////////////////////////////////////////////////////

/* setup default descriptors */

static void
initialize_descriptors(void)
{

	desc.dev.bLength               = sizeof( device_desc_t );
	desc.dev.bDescriptorType       = USB_DESC_DEVICE;
	desc.dev.bcdUSB                = 0x100; /* 1.0 */	// HS 0x200 ?????
	desc.dev.bDeviceClass          = 0xFF;	/* vendor specific */
	desc.dev.bDeviceSubClass       = 0;
	desc.dev.bDeviceProtocol       = 0;
	desc.dev.bMaxPacketSize0       = 64;	/* ep0 max fifo size, 16 orig ????? */
	desc.dev.idVendor              = 0;	/* vendor ID undefined */
	desc.dev.idProduct             = 0; 	/* product */
	desc.dev.bcdDevice             = 0; 	/* vendor assigned device release num */
	desc.dev.iManufacturer         = 0;	/* index of manufacturer string */
	desc.dev.iProduct              = 0; 	/* index of product description string */
	desc.dev.iSerialNumber         = 0;	/* index of string holding product s/n */
	desc.dev.bNumConfigurations    = 1;	/* configurations we have */

	desc.qua.bLength               = sizeof( qua_desc_t );
	desc.qua.bDescriptorType       = USB_DESC_QUALIFIER;
	desc.qua.bcdUSB                = 0x0200;
	desc.qua.bDeviceClass          = 0xFF;	/* vendor specific */
	desc.qua.bDeviceSubClass       = 0;
	desc.qua.bDeviceProtocol       = 0;
	desc.qua.bMaxPacketSize0       = 64;	/* ep0 max fifo size, 16 or 64 ????? */
	desc.qua.bNumConfigurations    = 1;
	desc.qua.bRESERVED             = 0;

// FIXME: Add support for all endpoint...
	usb_driver_reset();

	/* set language */
	/* See: http://www.usb.org/developers/data/USB_LANGIDs.pdf */
	sd_zero.bDescriptorType = USB_DESC_STRING;
	sd_zero.bLength         = sizeof( string_desc_t );
	sd_zero.bString[0]      = make_word_c( 0x409 ); /* American English */
	pxa_usb_set_string_descriptor( 0, &sd_zero );
}

/* soft_connect_hook()
 * Some devices have platform-specific circuitry to make USB
 * not seem to be plugged in, even when it is. This allows
 * software to control when a device 'appears' on the USB bus
 * (after Linux has booted and this driver has loaded, for
 * example). If you have such a circuit, control it here.
 */
static void
soft_connect_hook( int enable )
{
	struct mv_usb_dev *mv_dev = &the_controller;
	if (enable) {
		_usb_device_start(mv_dev->mv_usb_handle);
	} else {
		_usb_device_stop(mv_dev->mv_usb_handle);
	}
}

/* disable the UDC at the source */
static void
u2o_disable(void)
{
	struct mv_usb_dev *mv_dev = &the_controller;
	int i;
	soft_connect_hook( 0 );

	if (usb_debug)
		printf("%s\n\n", __func__);
	for (i=0; i<ARC_USB_MAX_ENDPOINTS; i++)
        	_usb_device_unregister_service(mv_dev->mv_usb_handle, i);

	/* Deregister all other services */
	_usb_device_unregister_service(mv_dev->mv_usb_handle, ARC_USB_SERVICE_BUS_RESET);
	_usb_device_unregister_service(mv_dev->mv_usb_handle, ARC_USB_SERVICE_SPEED_DETECTION);
	
	_usb_device_unregister_service(mv_dev->mv_usb_handle, ARC_USB_SERVICE_SUSPEND);

	_usb_device_shutdown(mv_dev->mv_usb_handle);
    	/* Disable clock for USB device */
	writel(0x0, PMUA_USB_CLK_RES_CTRL);
}

/*-------------------------------------------------------------------------*/
#if 0
static void u2o_soft_dis(int enable)
{
	if (usb_debug)
		printf("%s enable %d\n\n", __func__, enable);
}
#endif

/* CHIP ID: 
 * 	Z0: 0x00a0c910
 * 	Y0: 0x00f0c910
 */
#define CHIP_ID    0xd4282c00
int pxa910_is_z0(void)
{
	if ((readl(CHIP_ID) & 0x00f00000) == 0x00a00000)
		return 1;

	return 0;
}


/*  enable the udc at the source */
int u2o_enable(void)
{
	struct mv_usb_dev *mv_dev = &the_controller;
	int error, retval, i;
	u32 tmp;

#ifdef CONFIG_CPU_MONAHANS
	CKENC |= 0x1fe0;
	ACCR1 |= ACCR1_PU_OTG|ACCR1_PU_PLL|ACCR1_PU;
#else
    	/* enable u2o clock */
	writel(0x18, PMUA_USB_CLK_RES_CTRL);
	writel(0x1b, PMUA_USB_CLK_RES_CTRL);

	/* enable the pull up */
	if (!cpu_is_pxa910_168()) {
		if (pxa910_is_z0()) {
	        	writel((1<<20), (0xc0000004));
		} else {
			tmp = readl(0xd4207004);
			tmp |= (1<<20);
	        	writel(tmp, (0xd4207004));
		}
	}
#endif

	mv_dev->regbase = (unsigned *)U2O_REG_BASE; 
	mv_dev->phybase = (unsigned *)U2O_PHY_BASE;
	if (usb_debug)
		printf("%s PMUA_USB_CLK_RES_CTRL 0x%x regbase %p phybase %p\n", __func__,
		  __raw_readl(PMUA_USB_CLK_RES_CTRL), mv_dev->regbase, mv_dev->phybase);
	/* init the UTMI transceiver */
	u2o_phy_init(mv_dev->phybase);

	mv_dev->dev_no = 0;

	/* register bsp functions */
    	_usb_device_set_bsp_funcs(&usbImportFuncs);

	/* Enable ARC USB device */
	retval = (int)_usb_device_init(mv_dev->dev_no, &mv_dev->mv_usb_handle);
	if (retval != USB_OK) 
	{
	    mvOsPrintf("\nUSB Initialization failed. Error: %x", retval);
	    return -EINVAL;
	} /* Endif */
	
	/* Self Power, Remote wakeup disable */
	_usb_device_set_status(mv_dev->mv_usb_handle, ARC_USB_STATUS_DEVICE, (1 << DEVICE_SELF_POWERED));
	
	/* Register all ARC Services */  
	error = _usb_device_register_service(mv_dev->mv_usb_handle, 
	                            ARC_USB_SERVICE_BUS_RESET, mv_usb_bus_reset_service);
	if (error != USB_OK) 
	{
	    mvOsPrintf("\nUSB BUS_RESET Service Registration failed. Error: 0x%x", error);
	    return -EINVAL;
	} /* Endif */
	
	error = _usb_device_register_service(mv_dev->mv_usb_handle, 
	                    ARC_USB_SERVICE_SPEED_DETECTION, mv_usb_speed_service);
	if (error != USB_OK) 
	{
	    mvOsPrintf("\nUSB SPEED_DETECTION Service Registration failed. Error: 0x%x", 
	                    error);
	    return -EINVAL;
	} /* Endif */
	     
	error = _usb_device_register_service(mv_dev->mv_usb_handle, 
	                            ARC_USB_SERVICE_SUSPEND, mv_usb_suspend_service);
	if (error != USB_OK) 
	{
	    mvOsPrintf("\nUSB SUSPEND Service Registration failed. Error: 0x%x", error);
	    return -EINVAL;
	} /* Endif */
	
	error = _usb_device_register_service(mv_dev->mv_usb_handle, 
	              ARC_USB_SERVICE_SLEEP, mv_usb_suspend_service);
	if (error != USB_OK) 
	{
	    mvOsPrintf("\nUSB SUSPEND Service Registration failed. Error: 0x%x", error);
	    return -EINVAL;
	} /* Endif */    
	
	error = _usb_device_register_service(mv_dev->mv_usb_handle, 
	              ARC_USB_SERVICE_RESUME, mv_usb_resume_service);
	if (error != USB_OK) 
	{
	    mvOsPrintf("\nUSB RESUME Service Registration failed. Error: 0x%x", error);
	    return -EINVAL;
	} /* Endif */
	
	error = _usb_device_register_service(mv_dev->mv_usb_handle, 0, 
	              mv_usb_ep0_complete_service);   
	if (error != USB_OK) 
	{
	    mvOsPrintf("\nUSB ep0 TR_COMPLETE Service Registration failed. Error: 0x%x", error);
	    return error;
	} /* Endif */
	
	for (i=1; i<U2O_USED_ENDPOINTS; i++)
	{
	    error = _usb_device_register_service(mv_dev->mv_usb_handle, i, 
	              mv_usb_tr_complete_service);   
	    if (error != USB_OK) 
	    {
	        mvOsPrintf("\nUSB ep0 TR_COMPLETE Service Registration failed. Error: 0x%x", error);
	        return error;
	    } /* Endif */
}

	return 0;
}

void u2o_start(void)
{
	struct mv_usb_dev *mv_dev = &the_controller;
	
	ep0_send_buf = (int *)u2o_malloc(0x1000);
	ep1_buf = (int *)u2o_malloc(0x1000);
	ep2_buf = (int *)u2o_malloc(0x1000);

	if (usb_debug)
		printf("ep1_buf %p ep2_buf %p ep0_send_buf %p\n", 
			ep1_buf, ep2_buf, ep0_send_buf);
	u2o_enable();
	_usb_device_start(mv_dev->mv_usb_handle);
    	mv_usb_start_ep0(mv_dev);

}

static void u2o_dump_info(void)
{
	mv_usb_dump();
}

static int u2o_info(void)
{
	struct mv_usb_dev *mv_dev = &the_controller;

	printf("PMUA_USB_CLK_RES_CTRL %x U2PPLL %x\n", 
		PMUA_USB_CLK_RES_CTRL, u2o_get(mv_dev->phybase, U2PPLL));
	u2o_dump_info();
	return 0;
}


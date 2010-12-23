/*
 *  Copyright (C) Extenex Corporation 2001
 *  Copyright (C) Compaq Computer Corporation, 1998, 1999
 *  Copyright (C) Intrinsyc, Inc., 2002
 *
 * (C) Copyright 2006 Marvell International Ltd.  
 * All Rights Reserved 
 *
 *  PXA USB controller driver - Endpoint zero management
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
#include "mv_u2o_ctl.h"

#undef	DEBUG 
//#define DEBUG 3
#if DEBUG
static unsigned int usb_debug = DEBUG;
#else
#define usb_debug 0     /* gcc will remove all the debug code for us */
#endif

enum { true = 1, false = 0 };
typedef int bool;
#ifndef MIN
#define MIN( a, b ) ((a)<(b)?(a):(b))
#endif

extern int usb_speed;

/***************************************************************************
  Prototypes
 ***************************************************************************/
/* "setup handlers" -- the main functions dispatched to by the
   .. isr. These represent the major "modes" of endpoint 0 operation */
static int sh_setup_begin(struct usb_request*);				/* setup begin (idle) */
static void sh_write( void );      				/* writing data */
static void write_fifo( void );
static void get_descriptor( usb_dev_request_t * pReq );
static void queue_and_start_write( void * p, int req, int act );

/***************************************************************************
  Inline Helpers
 ***************************************************************************/

inline int type_code_from_request( __u8 by ) { return (( by >> 4 ) & 3); }

/* print string descriptor */
static inline void psdesc( string_desc_t * p )
{
	int i;
	int nchars = ( p->bLength - 2 ) / sizeof( __u16 );
	printf( "'" );
	for( i = 0 ; i < nchars ; i++ ) {
		printf( "%c", (char) p->bString[i] );
	}
	printf( "'\n" );
}

#if VERBOSITY
static inline void preq( usb_dev_request_t * pReq )
{
	static char * tnames[] = { "dev", "intf", "ep", "oth" };
	static char * rnames[] = { "std", "class", "vendor", "???" };
	char * psz;
	switch( pReq->bRequest ) {
		case GET_STATUS:        psz = "get stat"; break;
		case CLEAR_FEATURE:     psz = "clr feat"; break;
		case SET_FEATURE:       psz = "set feat"; break;
		case SET_ADDRESS:       psz = "set addr"; break;
		case GET_DESCRIPTOR:    psz = "get desc"; break;
		case SET_DESCRIPTOR:    psz = "set desc"; break;
		case GET_CONFIGURATION: psz = "get cfg"; break;
		case SET_CONFIGURATION: psz = "set cfg"; break;
		case GET_INTERFACE:     psz = "get intf"; break;
		case SET_INTERFACE:     psz = "set intf"; break;
		case SYNCH_FRAME:       psz = "synch frame"; break;
		default:                psz = "unknown"; break;
	}
	printf( "- [%s: %s req to %s. dir=%s]\n", psz,
			rnames[ (pReq->bmRequestType >> 5) & 3 ],
			tnames[ pReq->bmRequestType & 3 ],
			( pReq->bmRequestType & 0x80 ) ? "in" : "out" );
}

#else
static inline void preq( usb_dev_request_t *x){}
#endif

/***************************************************************************
  Globals
 ***************************************************************************/
static const char pszMe[] = "usbep0: ";

/* global write struct to keep write
   ..state around across interrupts */
static struct {
	unsigned char *p;
	int bytes_left;
} wr;

/***************************************************************************
  Public Interface
 ***************************************************************************/

/* reset received from HUB (or controller just went nuts and reset by itself!)
   so udc core has been reset, track this state here  */
void ep0_reset(void)
{
	if (usb_debug) printf( "%sep0_reset\n", pszMe);
	/* reset state machine */
	wr.p = 0;
	wr.bytes_left = 0;
	usbd_info.address=0;
}

/* handle interrupt for endpoint zero */
int ep0_int_hndlr(struct usb_request *usb_req)
{
	int ret;
	if (usb_debug) printf( "%sep0_int_hndlr\n", pszMe);
	ret = sh_setup_begin(usb_req);
	return ret;
}

/***************************************************************************
  Setup Handlers
 ***************************************************************************/
/*
 * sh_setup_begin()
 *
 */
static int sh_setup_begin(struct usb_request *usb_req)
{
	usb_dev_request_t req;
	int request_type, ret = 0;
	unsigned n;

	if (usb_debug) printf( "%s sh_setup_begin\n", pszMe);

	if (usb_req->length != sizeof(usb_dev_request_t)) {
		char *ptr = (char *)usb_req->buf;
		printf("%s error size usb_req.length: %d\n", 	
			__func__, usb_req->length);
		for (n=0; n<usb_req->length;n++) {
			printf(" %x", *ptr++);
		}
		printf("\n");
	}
	/* read the setup request */
	n = memcpy((void*)&req, usb_req->buf, sizeof(usb_dev_request_t));

	/* Is it a standard request? (not vendor or class request) */
	request_type = type_code_from_request( req.bmRequestType );
	if ( request_type != 0 ) {
		printf( "%ssetup begin: unsupported bmRequestType: %d ignored\n",
				pszMe, request_type );
		goto sh_sb_end;
	}


#ifdef DEBUG 
	{
		unsigned char * pdb = (unsigned char *) &req;
		if (usb_debug) printf( "%2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X ",
				pdb[0], pdb[1], pdb[2], pdb[3], pdb[4], pdb[5], pdb[6], pdb[7]
		       );
		preq( &req );
	}
#endif

	/* Handle it */
	switch( req.bRequest ) {
		case GET_DESCRIPTOR:
			if (usb_debug) printf( "%sGET_DESCRIPTOR\n", pszMe );
			get_descriptor( &req );
			break;

		case SET_INTERFACE:
			if (usb_debug) printf( "%sSET_INTERFACE TODO...\n", pszMe);
			break;

		case SET_DESCRIPTOR:
			if (usb_debug) printf( "%sSET_DESCRIPTOR TODO...\n", pszMe );
			break;

		case SET_CONFIGURATION:
			if (usb_debug) printf( "%sSET_CONFIGURATION %d\n", pszMe, req.wValue);
			break;
		default :
			printf("%sunknown request 0x%x\n", pszMe, req.bRequest);
			ret = -1;
			break;
	} /* switch( bRequest ) */

sh_sb_end:
	return ret;
}

/*
 * sh_write()
 * 
 * Due to UDC bugs we push everything into the fifo in one go.
 * Using interrupts just didn't work right...
 * This should be ok, since control request are small.
 */
static void sh_write()
{
	if (usb_debug) printf( "sh_write\n" );
	write_fifo();
}

/***************************************************************************
  Other Private Subroutines
 ***************************************************************************/
/*
 * queue_and_start_write()
 * data == data to send
 * req == bytes host requested
 * act == bytes we actually have
 *
 * Sets up the global "wr"-ite structure and load the outbound FIFO 
 * with data.
 *
 */
static void queue_and_start_write( void * data, int req, int act )
{
	if (usb_debug) printf( "write start: bytes requested=%d actual=%d\n", req, act);

	wr.p = (unsigned char*) data;
	wr.bytes_left = MIN( act, req );

	sh_write();

	return;
}
/*
 * write_fifo()
 * Stick bytes in the endpoint zero FIFO.
 *
 */
#define MAX_SETUP_PACKET_SIZE 256

static void write_fifo( void )
{
	struct mv_usb_dev *mv_usb = &the_controller;
	extern int * ep0_send_buf;
	__u8  *data=(__u8 *)ep0_send_buf;
	struct usb_request usb_req;
	
	memcpy(data, wr.p, wr.bytes_left);
	usb_req.buf = data;
	usb_req.length = wr.bytes_left;

	if (usb_debug) printf("%s  data=0x%p len %d\n", 
		__FUNCTION__, data, usb_req.length);
	if (usb_debug) {
		char *ptr = data;
		int i;
		for (i=0;i<usb_req.length;i++) {
			printf(" %x", *ptr++);
		}
		printf("\n");
	}

	mv_usb_ep_queue(&mv_usb->ep[0], &usb_req);

	wr.p += usb_req.length;
	wr.bytes_left -= usb_req.length;

	usbd_info.stats.ep0_bytes_written += usb_req.length;

	if (usb_debug) 
		printf( "write fifo: bytes sent=%d\n", usb_req.length);
}

/*
 * get_descriptor()
 * Called from sh_setup_begin to handle data return
 * for a GET_DESCRIPTOR setup request.
 *
 * +-----+------------------------------------------------+-----------------+
 * | dev | cfg1 | intf 1 | ep 1..N | intf 2 | ep 1..N |...| cfg2 .......... |
 * +-----+------------------------------------------------+-----------------+
 */
static void get_descriptor( usb_dev_request_t * pReq )
{
	string_desc_t * pString;
	ep_desc_t * pEndpoint = 0;
	config_desc_t *pcfg = 0;

	desc_t * pDesc = pxa_usb_get_descriptor_ptr();
	int type = pReq->wValue >> 8;
	int idx  = pReq->wValue & 0xFF;

	if (usb_debug) printf( "%sget_descriptor for %d\n", pszMe, type );
	switch( type ) {
		case USB_DESC_DEVICE:
			/* return device descritpor */
			queue_and_start_write( &pDesc->dev,
					pReq->wLength,
					pDesc->dev.bLength );
			break;

			// return device qualifier descriptor
		case USB_DESC_QUALIFIER:
			queue_and_start_write( &pDesc->qua,
					pReq->wLength,
					pDesc->qua.bLength );
			break;

			// return config descriptor buffer, cfg, intf 1..N,  ep 1..N
		case USB_DESC_CONFIG:
			{
				int i,len;
				config_desc_t *cfg =(config_desc_t*) (pDesc->cdb);

				len=0;
				for( i=0; i<pDesc->dev.bNumConfigurations; i++) {
					len += __le16_to_cpu(cfg->wTotalLength);
					cfg = (config_desc_t*) ( (unsigned char*) cfg 
							+ __le16_to_cpu(cfg->wTotalLength)) ;
				}
					
				queue_and_start_write( pDesc->cdb,
						pReq->wLength,
						len);
			}
			break;
			
			// return other speed config descriptor buffer
		case USB_DESC_OTHER_SPEED_CONFIG:
			{
				int i,len;
				config_desc_t *cfg =(config_desc_t*) (pDesc->cdb);

				if(usb_speed == USB_SPEED_HIGH) usb_driver_speed(USB_SPEED_FULL);
				else usb_driver_speed(USB_SPEED_HIGH);
				len=0;
				for( i=0; i<pDesc->dev.bNumConfigurations; i++) {
					len += __le16_to_cpu(cfg->wTotalLength);
					cfg = (config_desc_t*) ( (unsigned char*) cfg 
							+ __le16_to_cpu(cfg->wTotalLength)) ;
				}
					
				queue_and_start_write( pDesc->cdb,
						pReq->wLength,
						len);
				usb_driver_speed(usb_speed);
			}
			break;
			
			// not quite right, since doesn't do language code checking
		case USB_DESC_STRING:
			pString = pxa_usb_get_string_descriptor( idx );
			if ( pString ) {
				if ( idx != 0 ) {  // if not language index
					printf( "%sReturn string %d: ", pszMe, idx );
					psdesc( pString );
				}
				queue_and_start_write( pString,
						pReq->wLength,
						pString->bLength );
			}
			else {
				printf("%sunkown string index %d Stall.\n", pszMe, idx );
			}
			break;

			/*
		case USB_DESC_INTERFACE:
			for( i = 0; i < pDesc->intf_num; i++) {
				if ( idx == pDesc->intf[i].bInterfaceNumber ) {
					queue_and_start_write( &pDesc->intf[i],
							pReq->wLength,
							pDesc->intf[i].bLength );
				}
			}
			break;

		case USB_DESC_ENDPOINT: 
			for( i = 0; i < pDesc->ep_num; i++) {
				if ( idx == (0x0F & pDesc->ep[i].bEndpointAddress)) {
					queue_and_start_write( &pDesc->ep[i],
							pReq->wLength,
							pDesc->ep[i].bLength );
				}
			}
			break;
			*/

		default :
			printf("%sunknown descriptor type %d. Stall.\n", pszMe, type );
			break;

	}
}

/* end usb_ep0.c - who needs this comment? */

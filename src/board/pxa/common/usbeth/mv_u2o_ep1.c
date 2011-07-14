/*
 * Generic xmit layer for the PXA USB client function
 *
 * This code was loosely inspired by the original version which was
 * Copyright (c) Compaq Computer Corporation, 1998-1999
 * Copyright (c) 2001 by Nicolas Pitre
 *
 * (C) Copyright 2006 Marvell International Ltd.  
 * All Rights Reserved 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * 02-May-2002
 *   Frank Becker (Intrinsyc) - derived from sa1100 usb_send.c
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
#include "mv_u2o_api.h"

#undef DEBUG
//#define DEBUG 3
#if DEBUG
static unsigned int usb_debug = DEBUG;
#else
#define usb_debug 0     /* gcc will remove all the debug code for us */
#endif

#ifndef MMU_ENABLE
static char  ep1_buf[1024];
#else
extern int *ep1_buf;
#endif
static int   ep1_len;
static int   ep1_remain;
static int   ep1_retries = 0;
static usb_callback_t ep1_callback;

static void ep1_done(int flag);

void dump_buffer(char *buf, unsigned length)
{
        char *c = buf;
        unsigned i;
		
	if (!usb_debug)
		return;
       	printf("%s, buffer:%p, total length:%d\n", __func__, buf, length);
        for (i = 0; i < length; i++) {
                if (0 == i % 10)
                        printf("\n");
                printf (" %2x", c[i]);
        }
        printf("\n");
}

extern int req_pending;
/* device state is changing, async */
void
ep1_state_change_notify( int new_state )
{
}

/* set feature stall executing, async */
void
ep1_stall( void )
{
}

static void ep1_send_packet(void)
{
	struct mv_usb_dev *mv_dev = &the_controller;
	struct usb_request ep1_req;
	
	/* init the params for transfer */
	ep1_req.buf = ep1_buf;
	ep1_req.length = ep1_len;
	
	mv_usb_ep_queue(&mv_dev->ep[3], &ep1_req);
	
	if ((usb_speed == USB_SPEED_FULL) && !(ep1_len%64)) {
		struct usb_request ep1_req_zero;
		ep1_req_zero.buf = ep1_buf;
		ep1_req_zero.length = 0;
		mv_usb_ep_queue(&mv_dev->ep[3], &ep1_req_zero);	
	}

	if (usb_debug) {
		printf("%s: exit, buf %p len %d\n",
			__FUNCTION__, ep1_buf, ep1_len);
	}
}

static void
ep1_start(void)
{
	if (!ep1_len)
		return;

	ep1_send_packet();
}

static void
ep1_done(int flag)
{
	int size = ep1_len - ep1_remain;
	if (ep1_len) {
		ep1_len = 0;
		if (ep1_callback) {
			ep1_callback(flag, size);
		}
	}
}

int
ep1_init(int chn)
{
	if (usb_debug)
		printf("%s\n", __func__);
	return 0;
}

void
ep1_reset(void)
{
	ep1_done(-EINTR);
}

void ep1_int_hndlr(struct usb_request *usb_req)
{
	ep1_remain = ep1_len - usb_req->length;

	if (usb_debug)
		printf("%s send finisned buf %p len %d remain %d\n",
			__func__, usb_req->buf, usb_req->length, ep1_remain);

	if (ep1_remain != 0) {
		/* more data to go */
		ep1_start();
	} else {
		ep1_done(0);
	}

}

int
ep1_send(char *buf, int len, usb_callback_t callback)
{
	if (usb_debug) printf( "\npxa_usb_send: "
		"data len=%d state=%d blen=%d\n", 
		len, usbd_info.state, ep1_len);
	
	if (usbd_info.state != USB_STATE_CONFIGURED) {
		printf("state %d -ENODEV\n", usbd_info.state);
		return -ENODEV;
	}

	if (ep1_len) {
		ep1_retries++;
		if (ep1_retries >3) {
			printf("ep1_len %d -EBUSY\n", ep1_len);
			ep1_retries = 0;
			return -EBUSY;
		}
	}

	ep1_retries = 0;
	memcpy(ep1_buf, buf, len);
	ep1_len = len;
	ep1_callback = callback;
	ep1_remain = len;
	ep1_start();

	return 0;
}

int 
ep1_xmitter_avail( void )
{
	if (usbd_info.state != USB_STATE_CONFIGURED)
		return -ENODEV;
	if (ep1_len)
		return -EBUSY;
	return 0;
}



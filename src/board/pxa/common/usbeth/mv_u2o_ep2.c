/*
 * Generic receive layer for the PXA USB client function
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
 *   Frank Becker (Intrinsyc) - derived from sa1100 usb_recv.c
 * 
 * TODO: Add support for DMA.
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

#undef DEBUG
//#define DEBUG 3
#if DEBUG
static unsigned int usb_debug = DEBUG;
#else
#define usb_debug 0     /* gcc will remove all the debug code for us */
#endif

#ifndef MMU_ENABLE
static char *ep2_buf;
#else
static char *callback_buf;
extern char *ep2_buf;
#endif
static int   ep2_len;
static int   ep2_remain;
static struct usb_request ep2_req;
static usb_callback_t ep2_callback;

static void ep2_start(void)
{
	struct mv_usb_dev *mv_usb = &the_controller;
	struct usb_request ep2_req;

	ep2_req.buf = ep2_buf;
	ep2_req.length = ep2_len;

	if (usb_debug)
		printf("%s queue ep2 buf %p len %x\n", 
			__func__, ep2_buf, ep2_len);
	mv_usb_ep_queue(&mv_usb->ep[2], &ep2_req);
}

void
ep2_done(int flag)
{
	int size = ep2_len - ep2_remain;

	if (usb_debug)
		printf("%s ep2_len %d remain %d size %d\n", 
			__func__, ep2_len, ep2_remain, size);

	if (!ep2_len)
		return;

	ep2_len = 0;
	if (ep2_callback) {
#ifdef MMU_ENABLE
		if (usb_debug)
			printf("ep2_buf %p callback_buf %p size %d\n", 
				ep2_buf, callback_buf, size);
		memcpy(callback_buf, ep2_buf, size);
#endif
		ep2_callback(flag, size);
	}
}

void
ep2_state_change_notify( int new_state )
{
}

void
ep2_stall( void )
{
}

void
ep2_reset(void)
{
	ep2_done(-EINTR);
}

extern int usb_connected;

void ep2_int_hndlr(struct usb_request *usb_req)
{
	unsigned int desc_num, length, i, count;

	ep2_remain = ep2_len - usb_req->length;

	ep2_done(0);
	if( usb_debug) 
		printf("ep2_int_hndlr end: ep2_remain %d buf %d, len %d\n",
			ep2_remain, usb_req->buf, usb_req->length);

	return;
}

int ep2_recv(char *buf, int len, usb_callback_t callback)
{
	int flags;

	if (ep2_len){
		printf("%s, busy, ep2_len %d\n", __FUNCTION__, ep2_len);
		return -EBUSY;
	}

#ifndef MMU_ENABLE
	ep2_buf = buf;
#else
	callback_buf = buf;
#endif

	ep2_len = len;
	ep2_callback = callback;
	ep2_remain = len;
	ep2_start();
	
	return 0;
}


/*
 *  ether-pxausb.c : "eth-over-usb" driver for BLOB
 *
 *  Copyright (c) 2003, Intel Corporation (yu.tang@intel.com)
 *
 * (C) Copyright 2006 Marvell International Ltd.  
 * All Rights Reserved 
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

//#define DEBUG
#ifdef   CONFIG_TAVOREVB
#include <configs/tavorevb.h>
#endif
#ifdef   CONFIG_TTC_DKB
#include <configs/ttc_dkb.h>
#endif
#ifdef	 CONFIG_ZYLONITE2
#include <configs/zylonite2.h>
#endif
#ifdef  CONFIG_ASPENITE
#include <configs/aspenite.h>
#endif
#ifdef  CONFIG_WAYLAND
#include <configs/wayland.h>
#endif

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

static int usb_rsize=64;
static int usb_wsize=64;

/* cable connection */
extern int usb_connected;
extern int usb_speed;

/* receive buffer management */
static char rx_buf[2][ETH_BUF_SIZE];
static int  rx_size = 0;
static int  rx_done = 0 ;
static u16 rxIdx;		/* index of the current RX buffer */

/* transmit buffer management */
static int tx_done = 0 ;

u16 usb_vendor_id = 0x8086;
u16 usb_product_id = 0x07d3;

extern void u2o_start(void);

static void usb_eth_tx_callback(int flag, int size)
{
	tx_done = 1;
}

static void usb_eth_rx_callback(int flag, int size)
{
	int left, i;

#ifdef DEBUG
	printf("%s flag %d size %d left %d rx_done %d rx_size %d\n", 
		__func__, flag, size, left, rx_done, rx_size); 
#endif

	if( flag != 0 ) {
		//printf("%s, flag = %d\n", __FUNCTION__, flag);
		return;
	}

	if (rx_done) { 
		/* printf("overwrite\n"); */
		/* drop */
		rx_done = 0;
		rx_size = 0;
	}

	memcpy(rx_buf[0], rx_buf[1], size);
	rx_size += size;

done:
	rx_done = 1;

#ifdef DEBUG
	printf(" ----------------- rx ------------------\n");
	for (i=0; i<8; i++)
		printf(" %x", rx_buf[0][i]);
		
	printf("\n");
#endif
	NetReceive(rx_buf[0], size);

	/* setup to receive */
	ep2_recv(rx_buf[1], ETH_BUF_SIZE, usb_eth_rx_callback);

	return;
}

void usb_driver_speed(int speed)
{
	desc_t * pdesc = pxa_usb_get_descriptor_ptr();
	config_desc_t *cfg;
	intf_desc_t *intf;
	ep_desc_t *ep;

	cfg = (config_desc_t*) (pdesc->cdb);
	intf = (config_desc_t *)(cfg + 1);
	ep = (ep_desc_t *) (intf + 1);
	
	if( speed == USB_SPEED_HIGH ){
		ep[0].wMaxPacketSize      = make_word( 512 );
		ep[1].wMaxPacketSize      = make_word( 512 );
	} else {
		ep[0].wMaxPacketSize      = make_word( 64 );
		ep[1].wMaxPacketSize      = make_word( 64 );
	}
}

void usb_driver_reset(void)
{
	desc_t * pdesc = pxa_usb_get_descriptor_ptr();
	config_desc_t *cfg;
	intf_desc_t *intf;
	ep_desc_t *ep;

	/* setup device descriptor */
	pdesc->dev.idVendor	= usb_vendor_id;
	pdesc->dev.idProduct    = usb_product_id;
	pdesc->dev.bNumConfigurations = 1;

	cfg = (config_desc_t*) (pdesc->cdb);

	cfg->bLength             = sizeof( config_desc_t );
	cfg->bDescriptorType     = USB_DESC_CONFIG;
	cfg->wTotalLength        = make_word_c( sizeof(config_desc_t) +
						   sizeof(intf_desc_t) * 1+
						   sizeof(ep_desc_t) * 2);
	cfg->bNumInterfaces      = 1;
	cfg->bConfigurationValue = 1;
	cfg->iConfiguration      = 0;
	cfg->bmAttributes        = USB_CONFIG_BUSPOWERED;
	cfg->MaxPower            = USB_POWER( 500 );

	intf = (intf_desc_t *) ( cfg + 1);
	intf->bLength            = sizeof( intf_desc_t );
	intf->bDescriptorType    = USB_DESC_INTERFACE;
	intf->bInterfaceNumber   = 0; 
	intf->bAlternateSetting  = 0;
	intf->bNumEndpoints      = 2;
	intf->bInterfaceClass    = 0xFF; 
	intf->bInterfaceSubClass = 0;
	intf->bInterfaceProtocol = 0;
	intf->iInterface         = 0;

	ep = (ep_desc_t *) (intf + 1);
	ep[0].bLength             = sizeof( ep_desc_t );
	ep[0].bDescriptorType     = USB_DESC_ENDPOINT;
	ep[0].bEndpointAddress    = USB_EP_ADDRESS( 1, USB_IN );
	ep[0].bmAttributes        = USB_EP_BULK;
	if( usb_speed == USB_SPEED_HIGH )
		ep[0].wMaxPacketSize      = make_word( 512 );
	else
		ep[0].wMaxPacketSize      = make_word( 64 );
	ep[0].bInterval           = 0;

	ep[1].bLength             = sizeof( ep_desc_t );
	ep[1].bDescriptorType     = USB_DESC_ENDPOINT;
	ep[1].bEndpointAddress    = USB_EP_ADDRESS( 1, USB_OUT );
	ep[1].bmAttributes        = USB_EP_BULK;
	if( usb_speed == USB_SPEED_HIGH )
		ep[1].wMaxPacketSize      = make_word( 512 );
	else
		ep[1].wMaxPacketSize      = make_word( 64 );
	ep[1].bInterval           = 0;

	/* reset buffer */
	rx_done = 0;
	rx_size = 0;

}

void ep2_begin(void)
{
	/* setup to receive */
	if(usb_connected) {
		ep2_recv(rx_buf[1], ETH_BUF_SIZE, usb_eth_rx_callback);
	}
}

int check_usb_connection(int timeout)
{
	unsigned int cnt = 0;
	unsigned long long start;

	if (usb_connected) return 0;
	else 
		u2o_start();

	/* waiting util connected */
	printf("***** Plug-in USB cable & config usbdnet now ****** \n");
	
	start = get_ticks();
	while (!usb_connected) {
		//if ( __raw_readl(ICU_INT_STATUS_1) & (1<<(44-32)) ) {
			u2o_int_hndlr(0x11, 0);
		//}

		if (timeout && (start + timeout *CONFIG_SYS_HZ) < get_ticks()) {
			printf("wait for USB cable timeout\n");
			return 1;
		}
	}

	while ( cnt < 0x1000) {
		//if ( __raw_readl(ICU_INT_STATUS_1) & (1<<(44-32)) ) {
			u2o_int_hndlr(0x11, 0);
		//	cnt = 0;
		//}
		cnt ++;
	}

	printf("exit check_usb_connection:%d\n", usb_connected);
	return 0;
}

static int usb_eth_init(struct eth_device *dev, bd_t *bd)
{
	int env_size, env_present = 0, reg;
	char *s = NULL, *e, es[] = "11:22:33:44:55:66";
	char s_env_mac[64];
	uchar v_env_mac[6], *v_mac;

	env_size = getenv_r ("ethaddr", s_env_mac, sizeof (s_env_mac));
	if ((env_size > 0) && (env_size < sizeof (es))) {	/* exit if env is bad */
		printf ("\n*** ERROR: ethaddr is not set properly!!\n");
		return (-1);
	}

	if (env_size > 0) {
		env_present = 1;
		s = s_env_mac;
	}

	for (reg = 0; reg < 6; ++reg) { /* turn string into mac value */
		v_env_mac[reg] = s ? simple_strtoul (s, &e, 16) : 0;
		if (s)
			s = (*e) ? e + 1 : e;
	}

	memcpy (bd->bi_enetaddr, v_mac, 6);	/* update global address to match env (allows env changing) */
	memcpy(dev->enetaddr, bd->bi_enetaddr, 6);

	return 0;
}

static int usb_eth_halt(struct eth_device *dev, bd_t *bd)
{
}
static int loop=0;

extern int req_pending;
void dump_buffer(char *buf, unsigned length);

static int usb_eth_rx(struct eth_device *dev)
{
	int i=1000000;

	if(!usb_connected)
		return 0;//check_usb_connection();

	do{
		//if ( __raw_readl(ICU_INT_STATUS_1) & (1<<(44-32)) ) {
			u2o_int_hndlr(0x11, 0);
		//	i=0;
		//}

		DELAY(1);
		if(rx_done) break;
		i--;
	}while (!rx_done && (i>0));

//	memcpy(inbuf, rx_buf, rx_size);

	rx_done = 0;
	rx_size = 0;

	return rx_size;
}

static int usb_eth_tx(struct eth_device *dev, volatile void *packet,
			  int length)
{
	int i,tx_size = length;
	int retry = 3, ret;

	//check_usb_connection();

	if(!usb_connected)
		return 0;//check_usb_connection();

	tx_done = 0;
	do {
		ret = ep1_send(packet, length, usb_eth_tx_callback);
	
		i = 10000;
		do {
			u2o_int_hndlr();
			DELAY(1);
			i--;
		} while (!tx_done && (i>0));

		if (!ret) break;
		printf("%s ret %d retry %d\n", __func__, ret, retry);

	} while (retry--);

	return tx_size;
}

int u2o_eth_initialize(u32 base)
{
	struct eth_device *dev;
	dev = (struct eth_device *)malloc(sizeof(*dev));

	if (dev == NULL)
		hang();


	memset(dev, 0, sizeof(*dev));
	sprintf(dev->name, "u2o-eth");

	dev->iobase = 0;
	dev->priv = 0;
	dev->init = usb_eth_init;
	dev->halt = usb_eth_halt;
	dev->send = usb_eth_tx;
	dev->recv = usb_eth_rx;

	eth_register(dev);

	usb_connected = 0;

	pxa_usb_open("u2o-eth");
	pxa_usb_start();
#ifdef CONFIG_NETCONSOLE
	check_usb_connection(6);
	if(usb_connected) {
		extern int netconsole;
		char *serverip = getenv("serverip");

		/* switch to netconsole */
		setenv("ncip", serverip);
		setenv("stdin","nc");
		setenv("stdout","nc");
		netconsole = 1;
	}
#endif
	return 1;
}


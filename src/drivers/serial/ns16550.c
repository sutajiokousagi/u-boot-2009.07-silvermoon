/*
 * COM1 NS16550 support
 * originally from linux source (arch/ppc/boot/ns16550.c)
 * modified to use CONFIG_SYS_ISA_MEM and new defines
 */

#include <config.h>

#ifdef CONFIG_SYS_NS16550

#include <ns16550.h>

#define LCRVAL LCR_8N1					/* 8 data, 1 stop, no parity */
#define MCRVAL (MCR_DTR | MCR_RTS)			/* RTS/DTR */
#define FCRVAL (FCR_FIFO_EN | FCR_RXSR | FCR_TXSR)	/* Clear & enable FIFOs */

void NS16550_init (NS16550_t com_port, int baud_divisor)
{
#ifdef CONFIG_PXAXXX
       com_port->ier = 0x40;
#else
       com_port->ier = 0x00;
#endif
#ifdef CONFIG_OMAP
	com_port->mdr1 = 0x7;	/* mode select reset TL16C750*/
#endif
#ifndef CONFIG_PXAXXX
	/* The baudrate setting is redundant (set again as required right below)
	On PXA968 the two settings back-to-back result in wrong baudrate */
	com_port->lcr = LCR_BKSE | LCRVAL;
	com_port->dll = 0;
	com_port->dlm = 0;
	com_port->lcr = LCRVAL;
#endif
	com_port->mcr = MCRVAL;
	com_port->fcr = FCRVAL;
	com_port->lcr = LCR_BKSE | LCRVAL;
	com_port->dll = baud_divisor & 0xff;
	com_port->dlm = (baud_divisor >> 8) & 0xff;
	com_port->lcr = LCRVAL;
#if defined(CONFIG_OMAP)
#if defined(CONFIG_APTIX)
	com_port->mdr1 = 3;	/* /13 mode so Aptix 6MHz can hit 115200 */
#else
	com_port->mdr1 = 0;	/* /16 is proper to hit 115200 with 48MHz */
#endif
#endif
}

void NS16550_reinit (NS16550_t com_port, int baud_divisor)
{
#ifdef CONFIG_PXAXXX
       com_port->ier = 0x40;
#else
	com_port->ier = 0x00;
#endif
	com_port->lcr = LCR_BKSE | LCRVAL;
	com_port->dll = 0;
	com_port->dlm = 0;
	com_port->lcr = LCRVAL;
	com_port->mcr = MCRVAL;
	com_port->fcr = FCRVAL;
	com_port->lcr = LCR_BKSE;
	com_port->dll = baud_divisor & 0xff;
	com_port->dlm = (baud_divisor >> 8) & 0xff;
	com_port->lcr = LCRVAL;
}
 
static char __log_buf[1000];
static int __count = 0;

void NS16550_putc (NS16550_t com_port, char c)
{
	if(__count < 1000)
		__log_buf[__count ++] = c;

#if (!defined CONFIG_PXA3XX_SPI && defined CONFIG_ASPENITE) || !defined CONFIG_ASPENITE
	while ((com_port->lsr & LSR_THRE) == 0);
	com_port->thr = c;
#endif
}

char NS16550_getc (NS16550_t com_port)
{
	while ((com_port->lsr & LSR_DR) == 0) {
#ifdef CONFIG_USB_TTY
		extern void usbtty_poll(void);
		usbtty_poll();
#endif
	}
	return (com_port->rbr);
}

int NS16550_tstc (NS16550_t com_port)
{
	return ((com_port->lsr & LSR_DR) != 0);
}

#endif

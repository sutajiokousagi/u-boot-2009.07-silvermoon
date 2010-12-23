/*
 *  linux/include/asm-arm/arch-pxa/pxa-regs.h
 *
 *  Author:	Nicolas Pitre
 *  Created:	Jun 15, 2001
 *  Copyright:	MontaVista Software Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * - 2003/01/20: Robert Schwebel <r.schwebel@pengutronix.de
 *   Original file taken from linux-2.4.19-rmk4-pxa1. Added some definitions.
 *   Added include for hardware.h (for __REG definition)
 */
#ifndef _PXA_REGS_H_
#define _PXA_REGS_H_

#include "bitfield.h"
#include "hardware.h"

/* FIXME hack so that SA-1111.h will work [cb] */

/*
 * DMA Controller
 */
#define DCSR0		__REG(0x40000000)  /* DMA Control / Status Register for Channel 0 */
#define DCSR1		__REG(0x40000004)  /* DMA Control / Status Register for Channel 1 */
#define DCSR2		__REG(0x40000008)  /* DMA Control / Status Register for Channel 2 */
#define DCSR3		__REG(0x4000000c)  /* DMA Control / Status Register for Channel 3 */
#define DCSR4		__REG(0x40000010)  /* DMA Control / Status Register for Channel 4 */
#define DCSR5		__REG(0x40000014)  /* DMA Control / Status Register for Channel 5 */
#define DCSR6		__REG(0x40000018)  /* DMA Control / Status Register for Channel 6 */
#define DCSR7		__REG(0x4000001c)  /* DMA Control / Status Register for Channel 7 */
#define DCSR8		__REG(0x40000020)  /* DMA Control / Status Register for Channel 8 */
#define DCSR9		__REG(0x40000024)  /* DMA Control / Status Register for Channel 9 */
#define DCSR10		__REG(0x40000028)  /* DMA Control / Status Register for Channel 10 */
#define DCSR11		__REG(0x4000002c)  /* DMA Control / Status Register for Channel 11 */
#define DCSR12		__REG(0x40000030)  /* DMA Control / Status Register for Channel 12 */
#define DCSR13		__REG(0x40000034)  /* DMA Control / Status Register for Channel 13 */
#define DCSR14		__REG(0x40000038)  /* DMA Control / Status Register for Channel 14 */
#define DCSR15		__REG(0x4000003c)  /* DMA Control / Status Register for Channel 15 */
#if defined(CONFIG_CPU_MONAHANS) || defined(CONFIG_PXAXXX)
#define DCSR16		__REG(0x40000040)  /* DMA Control / Status Register for Channel 16 */
#define DCSR17		__REG(0x40000044)  /* DMA Control / Status Register for Channel 17 */
#define DCSR18		__REG(0x40000048)  /* DMA Control / Status Register for Channel 18 */
#define DCSR19		__REG(0x4000004c)  /* DMA Control / Status Register for Channel 19 */
#define DCSR20		__REG(0x40000050)  /* DMA Control / Status Register for Channel 20 */
#define DCSR21		__REG(0x40000054)  /* DMA Control / Status Register for Channel 21 */
#define DCSR22		__REG(0x40000058)  /* DMA Control / Status Register for Channel 22 */
#define DCSR23		__REG(0x4000005c)  /* DMA Control / Status Register for Channel 23 */
#define DCSR24		__REG(0x40000060)  /* DMA Control / Status Register for Channel 24 */
#define DCSR25		__REG(0x40000064)  /* DMA Control / Status Register for Channel 25 */
#define DCSR26		__REG(0x40000068)  /* DMA Control / Status Register for Channel 26 */
#define DCSR27		__REG(0x4000006c)  /* DMA Control / Status Register for Channel 27 */
#define DCSR28		__REG(0x40000070)  /* DMA Control / Status Register for Channel 28 */
#define DCSR29		__REG(0x40000074)  /* DMA Control / Status Register for Channel 29 */
#define DCSR30		__REG(0x40000078)  /* DMA Control / Status Register for Channel 30 */
#define DCSR31		__REG(0x4000007c)  /* DMA Control / Status Register for Channel 31 */
#endif /* CONFIG_CPU_MONAHANS */

#define DCSR(x)		__REG2(0x40000000, (x) << 2)

#define DCSR_RUN	(1 << 31)	/* Run Bit (read / write) */
#define DCSR_NODESC	(1 << 30)	/* No-Descriptor Fetch (read / write) */
#define DCSR_STOPIRQEN	(1 << 29)	/* Stop Interrupt Enable (read / write) */

#if defined(CONFIG_PXA27X) || defined (CONFIG_CPU_MONAHANS) || defined(CONFIG_PXAXXX)
#define DCSR_EORIRQEN	(1 << 28)	/* End of Receive Interrupt Enable (R/W) */
#define DCSR_EORJMPEN	(1 << 27)	/* Jump to next descriptor on EOR */
#define DCSR_EORSTOPEN	(1 << 26)	/* STOP on an EOR */
#define DCSR_SETCMPST	(1 << 25)	/* Set Descriptor Compare Status */
#define DCSR_CLRCMPST	(1 << 24)	/* Clear Descriptor Compare Status */
#define DCSR_CMPST	(1 << 10)	/* The Descriptor Compare Status */
#define DCSR_ENRINTR	(1 << 9)	/* The end of Receive */
#endif

#define DCSR_REQPEND	(1 << 8)	/* Request Pending (read-only) */
#define DCSR_STOPSTATE	(1 << 3)	/* Stop State (read-only) */
#define DCSR_ENDINTR	(1 << 2)	/* End Interrupt (read / write) */
#define DCSR_STARTINTR	(1 << 1)	/* Start Interrupt (read / write) */
#define DCSR_BUSERR	(1 << 0)	/* Bus Error Interrupt (read / write) */

#define DINT		__REG(0x400000f0)  /* DMA Interrupt Register */

#define DRCMR0		__REG(0x40000100)  /* Request to Channel Map Register for DREQ 0 */
#define DRCMR1		__REG(0x40000104)  /* Request to Channel Map Register for DREQ 1 */
#define DRCMR2		__REG(0x40000108)  /* Request to Channel Map Register for I2S receive Request */
#define DRCMR3		__REG(0x4000010c)  /* Request to Channel Map Register for I2S transmit Request */
#define DRCMR4		__REG(0x40000110)  /* Request to Channel Map Register for BTUART receive Request */
#define DRCMR5		__REG(0x40000114)  /* Request to Channel Map Register for BTUART transmit Request. */
#define DRCMR6		__REG(0x40000118)  /* Request to Channel Map Register for FFUART receive Request */
#define DRCMR7		__REG(0x4000011c)  /* Request to Channel Map Register for FFUART transmit Request */
#define DRCMR8		__REG(0x40000120)  /* Request to Channel Map Register for AC97 microphone Request */
#define DRCMR9		__REG(0x40000124)  /* Request to Channel Map Register for AC97 modem receive Request */
#define DRCMR10		__REG(0x40000128)  /* Request to Channel Map Register for AC97 modem transmit Request */
#define DRCMR11		__REG(0x4000012c)  /* Request to Channel Map Register for AC97 audio receive Request */
#define DRCMR12		__REG(0x40000130)  /* Request to Channel Map Register for AC97 audio transmit Request */
#define DRCMR13		__REG(0x40000134)  /* Request to Channel Map Register for SSP receive Request */
#define DRCMR14		__REG(0x40000138)  /* Request to Channel Map Register for SSP transmit Request */
#define DRCMR15		__REG(0x4000013c)  /* Reserved */
#define DRCMR16		__REG(0x40000140)  /* Reserved */
#define DRCMR17		__REG(0x40000144)  /* Request to Channel Map Register for ICP receive Request */
#define DRCMR18		__REG(0x40000148)  /* Request to Channel Map Register for ICP transmit Request */
#define DRCMR19		__REG(0x4000014c)  /* Request to Channel Map Register for STUART receive Request */
#define DRCMR20		__REG(0x40000150)  /* Request to Channel Map Register for STUART transmit Request */
#define DRCMR21		__REG(0x40000154)  /* Request to Channel Map Register for MMC receive Request */
#define DRCMR22		__REG(0x40000158)  /* Request to Channel Map Register for MMC transmit Request */
#define DRCMR23		__REG(0x4000015c)  /* Reserved */
#define DRCMR24		__REG(0x40000160)  /* Reserved */
#define DRCMR25		__REG(0x40000164)  /* Request to Channel Map Register for USB endpoint 1 Request */
#define DRCMR26		__REG(0x40000168)  /* Request to Channel Map Register for USB endpoint 2 Request */
#define DRCMR27		__REG(0x4000016C)  /* Request to Channel Map Register for USB endpoint 3 Request */
#define DRCMR28		__REG(0x40000170)  /* Request to Channel Map Register for USB endpoint 4 Request */
#define DRCMR29		__REG(0x40000174)  /* Reserved */
#define DRCMR30		__REG(0x40000178)  /* Request to Channel Map Register for USB endpoint 6 Request */
#define DRCMR31		__REG(0x4000017C)  /* Request to Channel Map Register for USB endpoint 7 Request */
#define DRCMR32		__REG(0x40000180)  /* Request to Channel Map Register for USB endpoint 8 Request */
#define DRCMR33		__REG(0x40000184)  /* Request to Channel Map Register for USB endpoint 9 Request */
#define DRCMR34		__REG(0x40000188)  /* Reserved */
#define DRCMR35		__REG(0x4000018C)  /* Request to Channel Map Register for USB endpoint 11 Request */
#define DRCMR36		__REG(0x40000190)  /* Request to Channel Map Register for USB endpoint 12 Request */
#define DRCMR37		__REG(0x40000194)  /* Request to Channel Map Register for USB endpoint 13 Request */
#define DRCMR38		__REG(0x40000198)  /* Request to Channel Map Register for USB endpoint 14 Request */
#define DRCMR39		__REG(0x4000019C)  /* Reserved */

#define DRCMR68		       __REG(0x40001110)  /* Request to Channel Map Register for Camera FIFO 0 Request */
#define DRCMR69		       __REG(0x40001114)  /* Request to Channel Map Register for Camera FIFO 1 Request */
#define DRCMR70		       __REG(0x40001118)  /* Request to Channel Map Register for Camera FIFO 2 Request */

/*
 * Interrupt Controller
 */
#define ICIP		__REG(0x40D00000)  /* Interrupt Controller IRQ Pending Register */
#define ICMR		__REG(0x40D00004)  /* Interrupt Controller Mask Register */
#define ICLR		__REG(0x40D00008)  /* Interrupt Controller Level Register */
#define ICFP		__REG(0x40D0000C)  /* Interrupt Controller FIQ Pending Register */
#define ICPR		__REG(0x40D00010)  /* Interrupt Controller Pending Register */
#define ICCR		__REG(0x40D00014)  /* Interrupt Controller Control Register */

#if defined(CONFIG_CPU_MONAHANS) || defined(CONFIG_PXAXXX)
#define ICHP		__REG(0x40D00018)  /* Interrupt Controller Highest Priority Register */
/* Missing: 32 Interrupt priority registers
 * These are the same as beneath for PXA27x: maybe can be merged if
 * GPIO Stuff is same too.
 */
#define ICIP2		__REG(0x40D0009C)  /* Interrupt Controller IRQ Pending Register 2 */
#define ICMR2		__REG(0x40D000A0)  /* Interrupt Controller Mask Register 2 */
#define ICLR2		__REG(0x40D000A4)  /* Interrupt Controller Level Register 2 */
#define ICFP2		__REG(0x40D000A8)  /* Interrupt Controller FIQ Pending Register 2 */
#define ICPR2		__REG(0x40D000AC)  /* Interrupt Controller Pending Register 2 */
/* Missing: 2 Interrupt priority registers */
#endif /* CONFIG_CPU_MONAHANS */

#endif	/* _PXA_REGS_H_ */

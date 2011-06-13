/******************************************************************************
 *
 * Name:	PMUC.h
 * Project:	Hermon-2
 * Purpose:	Testing
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  (C)Copyright 2005 - 2008 Marvell. All Rights Reserved.
 *  
 *  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF MARVELL.
 *  The copyright notice above does not evidence any actual or intended 
 *  publication of such source code.
 *  This Module contains Proprietary Information of Marvell and should be
 *  treated as Confidential.
 *  The information in this file is provided for the exclusive use of the 
 *  licensees of Marvell.
 *  Such users have the right to use, modify, and incorporate this code into 
 *  products for purposes authorized by the license agreement provided they 
 *  include this notice and the associated copyright notice with any such
 *  product. 
 *  The information in this file is provided "AS IS" without warranty.
 *
 ******************************************************************************/

/******************************************************************************
 *
 * This file was automatically generated by reg.pl using  *	PMUC.csv
 *
 ******************************************************************************/

/******************************************************************************
 *
 * History:
 *
 ********* PLEASE INSERT THE CVS HISTORY OF THE PREVIOUS VERSION HERE. *********
 *******************************************************************************/

#ifndef	__INC_PMUC_H
#define	__INC_PMUC_H


/*
 *
 *	THE BASE ADDRESSES
 *
 */
#define	PMUC_BASE	0xF00E0000

/*
 *
 *	THE REGISTER DEFINES
 *
 */
#define	PMUC_CCCR	(PMUC_BASE+0x0000)	/* 32 bit	Communication Subsystem
										 *			Clock Configuration
										 *			Register
										 */
#define	PMUC_CCSR	(PMUC_BASE+0x0004)	/* 32 bit	Communication Subsystem
										 *			Clock Status Register
										 */
#define	PMUC_GCCCR	(PMUC_BASE+0x0008)	/* 32 bit	Grayback Core Clock Control
										 *			Register
										 */
#define	PMUC_MPCR	(PMUC_BASE+0x0100)	/* 32 bit	Communication Subsystem
										 *			Power Control Register
										 */
#define	PMUC_CMCR	(PMUC_BASE+0x0114)	/* 32 bit	Communication Subsystem
										 *			Miscellaneous Counters
										 *			Register
										 */
#define	PMUC_CPSR	(PMUC_BASE+0x0118)	/* 32 bit	Communication Subsystem
										 *			Power Status Register
										 */
#define	PMUC_CIER	(PMUC_BASE+0x0130)	/* 32 bit	Communication Subsystem
										 *			Interrupt Enable Register
										 */
#define	PMUC_CGDES	(PMUC_BASE+0x0140)	/* 32 bit	MSA Deep Sleep Entry Status
										 *			Register
										 */
#define	PMUC_CSSR	(PMUC_BASE+0x0200)	/* 32 bit	Communication Subsystem
										 *			Spare Register
										 */

/*
 *
 *	THE BIT DEFINES
 *
 */
/*	PMUC_CCCR	0x0000	Communication Subsystem Clock Configuration Register */
/*		Bit(s) PMUC_CCCR_RSRV_31_17 reserved */
#define	PMUC_CCCR_L2CF					BIT_16			/* L2 Cache clock Force */
/*		Bit(s) PMUC_CCCR_RSRV_15_4 reserved */
/* Communication Subsystem Clock Divider */
#define	PMUC_CCCR_CSCD_MSK				SHIFT1(0x7)		
#define	PMUC_CCCR_CSCD_BASE				1
/*		Bit(s) PMUC_CCCR_RSRV_0 reserved */

/*	PMUC_CCSR	0x0004	Communication Subsystem Clock Status Register */
/*		Bit(s) PMUC_CCSR_RSRV_31_4 reserved */
/* Communication Subsystem Clock Divider Status */
#define	PMUC_CCSR_CSCD_S_MSK			SHIFT1(0x7)			
#define	PMUC_CCSR_CSCD_S_BASE			1
/*		Bit(s) PMUC_CCSR_RSRV_0 reserved */

/*	PMUC_GCCCR	0x0008	Grayback Core Clock Control Register */
/*		Bit(s) PMUC_GCCCR_RSRV_31_1 reserved */
/* Core Frequency Change */
#define	PMUC_GCCCR_F					BIT_0				

/*	PMUC_MPCR	0x0100	Communication Subsystem Power Control Register */
/*		Bit(s) PMUC_MPCR_RSRV_31_23 reserved */
/* WCDMA Modem Power Request */
#define	PMUC_MPCR_WMP_REQ				BIT_22				
/*		Bit(s) PMUC_MPCR_RSRV_21_0 reserved */

/*	PMUC_CMCR	0x0114	Communication Subsystem Miscellaneous Counters Register */
/*		Bit(s) PMUC_CMCR_RSRV_31_7 reserved */
/* Comm Memories Power Down Counter */
#define	PMUC_CMCR_MEMPDC_MSK			SHIFT3(0xf)			
#define	PMUC_CMCR_MEMPDC_BASE			3
#define	PMUC_CMCR_GC_MSK				SHIFT0(0x7)			/* Grid Count */
#define	PMUC_CMCR_GC_BASE				0

/*	PMUC_CPSR	0x0118	Communication Subsystem Power Status Register */
#define	PMUC_CPSR_SWR_MSK				SHIFT28(0xf)		/* SWR */
#define	PMUC_CPSR_SWR_BASE				28
/*		Bit(s) PMUC_CPSR_RSRV_27_6 reserved */
/* WCDMA modem power status */
#define	PMUC_CPSR_WMS					BIT_5				
/*		Bit(s) PMUC_CPSR_RSRV_4_2 reserved */
/* Deep Sleep State Exit Reset */
#define	PMUC_CPSR_LPM					BIT_1				
/*		Bit(s) PMUC_CPSR_RSRV_0 reserved */

/*	PMUC_CIER	0x0130	Communication Subsystem Interrupt Enable Register */
/*		Bit(s) PMUC_CIER_RSRV_31_1 reserved */
/* Frequency Change Interrupt Enable */
#define	PMUC_CIER_FC_IE					BIT_0				

/*	PMUC_CGDES	0x0140	MSA Deep Sleep Entry Status Register */
/*		Bit(s) PMUC_CGDES_RSRV_31_3 reserved */
/* MSA Core Deep Sleep Entry Status */
#define	PMUC_CGDES_GDES_MSK				SHIFT0(0x7)			
#define	PMUC_CGDES_GDES_BASE			0

/*	PMUC_CSSR	0x0200	Communication Subsystem Spare Register */
/*		Bit(s) PMUC_CSSR_RSRV_31_0 reserved */



/* -------------------- */


#endif	/* __INC_PMUC_H */
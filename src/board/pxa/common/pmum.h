/******************************************************************************
 *
 * Name:	PMUM.h
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
 * This file was automatically generated by reg.pl using  *	PMUM.csv
 *
 ******************************************************************************/

/******************************************************************************
 *
 * History:
 *
 ********* PLEASE INSERT THE CVS HISTORY OF THE PREVIOUS VERSION HERE. *********
 *******************************************************************************/

#ifndef	__INC_PMUM_H
#define	__INC_PMUM_H


/*
 *
 *	THE BASE ADDRESSES
 *
 */
#define	PMUM_BASE	0xD4050000

/*
 *
 *	THE REGISTER DEFINES
 *
 */
#define	PMUM_CPCR	(PMUM_BASE+0x0000)	/* 32 bit	Marvell CPU Power Control
										 *			Register
										 */
#define	PMUM_CPSR	(PMUM_BASE+0x0004)	/* 32 bit	Marvell CPU Power Status
										 *			Register
										 */
#define	PMUM_FCCR	(PMUM_BASE+0x0008)	/* 32 bit	Frequency Change Control
										 *			Register
										 */
#define	PMUM_POCR	(PMUM_BASE+0x000C)	/* 32 bit	PLL and Oscillator Control
										 *			Register
										 */
#define	PMUM_POSR	(PMUM_BASE+0x0010)	/* 32 bit	PLL and Oscillator Status
										 *			Register
										 */
#define	PMUM_SUCCR	(PMUM_BASE+0x0014)	/* 29 bit	Slow UART (UART 1) Clock
										 *			Generation Control
										 *			Register
										 */
#define	PMUM_VRCR	(PMUM_BASE+0x0018)	/* 32 bit	VCXO Request Control
										 *			Register
										 */
#define	PMUM_CPRR	(PMUM_BASE+0x0020)	/* 32 bit	Marvell CPU Programmable
										 *			Reset Register
										 */
#define	PMUM_CCGR	(PMUM_BASE+0x0024)	/* 32 bit	Marvell CPU Clock Gating
										 *			Register
										 */
#define	PMUM_CRSR	(PMUM_BASE+0x0028)	/* 32 bit	Marvell CPU Reset Status
										 *			Register
										 */
#define	PMUM_GPCR	(PMUM_BASE+0x0030)	/* 32 bit	General Purpose Clock
										 *			Generation Control
										 *			Register
										 */
#define	PMUM_PLL2CR	(PMUM_BASE+0x0034)	/* 32 bit	PLL2 Control Register */
#define	PMUM_SCCR	(PMUM_BASE+0x0038)	/* 32 bit	Slow Clock Control Register */
#define	PMUM_MCCR	(PMUM_BASE+0x003C)	/* 32 bit	MSA Clock Control Register */
#define	PMUM_ISCCRX	(PMUM_BASE+0x0040)	/* I2S Clock Generation Control Register
										 *			Start
										 */
#define	PMUM_CWUCRS	(PMUM_BASE+0x0048)	/* 32 bit	Wakeup and Clock Resume
										 *			Lines Status Register
										 */
#define	PMUM_CWUCRM	(PMUM_BASE+0x004C)	/* 31 bit	Wakeup and Clock Resume
										 *			Lines Mask Register
										 */
#define	PMUM_DSOC	(PMUM_BASE+0x0100)	/* 32 bit	Debug Signals Output
										 *			Control Register
										 */
#define	PMUM_WDTPCR	(PMUM_BASE+0x0200)	/* 32 bit	WDT (Communication
										 *			Subsystem Timers) Control
										 *			Register
										 */
#define	PMUM_CMPRX	(PMUM_BASE+0x0400)	/* Marvell CPU Module Power Register Start */
#define	PMUM_APCR	(PMUM_BASE+0x1000)	/* 32 bit	Marvell CPU Power Control
										 *			Register
										 */
#define	PMUM_APSR	(PMUM_BASE+0x1004)	/* 32 bit	Marvell CPU Power Status
										 *			Register
										 */
#define	PMUM_APRR	(PMUM_BASE+0x1020)	/* 32 bit	Marvell CPU Programmable
										 *			Reset Register
										 */
#define	PMUM_ACGR	(PMUM_BASE+0x1024)	/* 32 bit	Marvell CPU Clock Gating
										 *			Register
										 */
#define	PMUM_ARSR	(PMUM_BASE+0x1028)	/* 32 bit	Marvell CPU Reset Status
										 *			Register
										 */
#define	PMUM_AWUCRS	(PMUM_BASE+0x1048)	/* 32 bit	Wakeup and Clock Resume
										 *			Lines Status Register
										 */
#define	PMUM_AWUCRM	(PMUM_BASE+0x104C)	/* 31 bit	Wakeup and Clock Resume
										 *			Lines Mask Register
										 */

/*
 *
 *	THE BIT DEFINES
 *
 */
/*	PMUM_CPCR	0x0000	Marvell CPU Power Control Register */
#define	PMUM_CPCR_AXISD					BIT_31			/* AXISD */
#define	PMUM_CPCR_DSPSD					BIT_30			/* DSPSD */
#define	PMUM_CPCR_SLPEN					BIT_29			/* SLPEN */
#define	PMUM_CPCR_DTCMSD				BIT_28			/* DTCMSD */
#define	PMUM_CPCR_DDRCORSD				BIT_27			/* DDRCORSD */
#define	PMUM_CPCR_APBSD					BIT_26			/* APBSD */
#define	PMUM_CPCR_BBSD					BIT_25			/* BBSD */
#define	PMUM_CPCR_INTCLR				BIT_24			/* Interrupt clear register */
/* Sleep mode Wake-up decoder Port 0 disable */
#define	PMUM_CPCR_SLPWP0				BIT_23			
/* Sleep mode Wake-up decoder Port 1 disable */
#define	PMUM_CPCR_SLPWP1				BIT_22			
/* Sleep mode Wake-up decoder Port 2 disable */
#define	PMUM_CPCR_SLPWP2				BIT_21			
/* Sleep mode Wake-up decoder Port 3 disable */
#define	PMUM_CPCR_SLPWP3				BIT_20			
#define	PMUM_CPCR_VCTCXOSD				BIT_19			/* VCTCXOSD */
/* Sleep mode Wake-up decoder Port 4 disable */
#define	PMUM_CPCR_SLPWP4				BIT_18			
/* Sleep mode Wake-up decoder Port 5 disable */
#define	PMUM_CPCR_SLPWP5				BIT_17			
/* Sleep mode Wake-up decoder Port 6 disable */
#define	PMUM_CPCR_SLPWP6				BIT_16			
/* Sleep mode Wake-up decoder Port 7 disable */
#define	PMUM_CPCR_SLPWP7				BIT_15			
#define	PMUM_CPCR_MSASLPEN				BIT_14			/* MSASLPEN */

/*	PMUM_CPSR	0x0004	Marvell CPU Power Status Register */
#define	PMUM_CPSR_DSPIDL				BIT_31			/* DSPIDL */
#define	PMUM_CPSR_APIDL					BIT_30			/* APIDL */
#define	PMUM_CPSR_CPIDL					BIT_29			/* CPIDL */
#define	PMUM_CPSR_DSPAVL				BIT_28			/* DSPAVL */
#define	PMUM_CPSR_AP_FULL_IDLE			BIT_27			/* AP_FULL_IDLE */
#define	PMUM_CPSR_CP_FULL_IDLE			BIT_26			/* CP_FULL_IDLE */
#define	PMUM_CPSR_AP_SS_SLEEP			BIT_25			/* AP_SS_SLEEP */
#define	PMUM_CPSR_CP_SS_SLEEP			BIT_24			/* CP_SS_SLEEP */
/*		Bit(s) PMUM_CPSR_RSRV_23_21 reserved */
/*		Bit(s) PMUM_CPSR_RSRV_20 reserved */
/*		Bit(s) PMUM_CPSR_RSRV_19_17 reserved */
/*		Bit(s) PMUM_CPSR_RSRV_16_0 reserved */

/*	PMUM_FCCR	0x0008	Frequency Change Control Register */
/* Marvell <var Processor: Mohawk> CPU clock selection */
#define	PMUM_FCCR_MOHCLKSEL_MSK			SHIFT29(0x7)	
#define	PMUM_FCCR_MOHCLKSEL_BASE		29
/* Marvell <var Processor: Osprey> CPU clock selection */
#define	PMUM_FCCR_SEAGCLKSEL_MSK		SHIFT26(0x7)	
#define	PMUM_FCCR_SEAGCLKSEL_BASE		26
/* AXI and DDR clock selection */
#define	PMUM_FCCR_AXICLKSEL_MSK			SHIFT23(0x7)	
#define	PMUM_FCCR_AXICLKSEL_BASE		23
/*		Bit(s) PMUM_FCCR_RSRV_22_18 reserved */
/*		Bit(s) PMUM_FCCR_RSRV_17_16 reserved */
#define	PMUM_FCCR_MFC					BIT_15			/* MFC */
#define	PMUM_FCCR_PLL1CEN				BIT_14			/* PLL1 control enable */
#define	PMUM_FCCR_PLL1REFD_MSK			SHIFT9(0x1f)	/* PLL1 REFDIV */
#define	PMUM_FCCR_PLL1REFD_BASE			9
#define	PMUM_FCCR_PLL1FBD_MSK			SHIFT0(0x1ff)	/* PLL1 FBDIV */
#define	PMUM_FCCR_PLL1FBD_BASE			0

/*	PMUM_POCR	0x000C	PLL and Oscillator Control Register */
#define	PMUM_POCR_FORCE					BIT_31			/* FORCE */
/*		Bit(s) PMUM_POCR_RSRV_30_24 reserved */
#define	PMUM_POCR_VCXOST_MSK			SHIFT16(0xff)	/* VCXOST */
#define	PMUM_POCR_VCXOST_BASE			16
/*		Bit(s) PMUM_POCR_RSRV_15_12 reserved */
#define	PMUM_POCR_PLLLOCK_MSK			SHIFT0(0xfff)	/* PLLLOCK */
#define	PMUM_POCR_PLLLOCK_BASE			0

/*	PMUM_POSR	0x0010	PLL and Oscillator Status Register */
/*		Bit(s) PMUM_POSR_RSRV_31_28 reserved */
#define	PMUM_POSR_PLL2REFD_MSK			SHIFT23(0x1f)	/* PLL2REFD */
#define	PMUM_POSR_PLL2REFD_BASE			23
#define	PMUM_POSR_PLL2FBD_MSK			SHIFT14(0x1ff)	/* PLL2FBD */
#define	PMUM_POSR_PLL2FBD_BASE			14
#define	PMUM_POSR_PLL1REFD_MSK			SHIFT9(0x1f)	/* PLL1REFD */
#define	PMUM_POSR_PLL1REFD_BASE			9
#define	PMUM_POSR_PLL1FBD_MSK			SHIFT0(0x1ff)	/* PLL1FBD */
#define	PMUM_POSR_PLL1FBD_BASE			0

/*	PMUM_SUCCR	0x0014	Slow UART (UART 1) Clock Generation Control Register */
#define	PMUM_SUCCR_UARTDIVN_MSK			SHIFT16(0x1fff)	/* UARTDIVN */
#define	PMUM_SUCCR_UARTDIVN_BASE		16
/*		Bit(s) PMUM_SUCCR_RSRV_15_13 reserved */
#define	PMUM_SUCCR_UARTDIVD_MSK			SHIFT0(0x1fff)	/* UARTDIVD */
#define	PMUM_SUCCR_UARTDIVD_BASE		0

/*	PMUM_VRCR	0x0018	VCXO Request Control Register */
/*		Bit(s) PMUM_VRCR_RSRV_31_9 reserved */
#define	PMUM_VRCR_SW_REQ				BIT_8				/* SW Request */
#define	PMUM_VRCR_REQ_POL_MSK			SHIFT4(0xf)			/* HW Request Polarity */
#define	PMUM_VRCR_REQ_POL_BASE			4
#define	PMUM_VRCR_REQ_EN_MSK			SHIFT0(0xf)			/* HW Request Enable */
#define	PMUM_VRCR_REQ_EN_BASE			0

/*	PMUM_CPRR	0x0020	Marvell CPU Programmable Reset Register */
/*		Bit(s) PMUM_CPRR_RSRV_31_7 reserved */
#define	PMUM_CPRR_SLAVE_R				BIT_6				/* SLAVE_R */
#define	PMUM_CPRR_DSRAMINT				BIT_5				/* DSRAMINT */
#define	PMUM_CPRR_WDTR					BIT_4				/* WDTR */
#define	PMUM_CPRR_BBR					BIT_3				/* BBR */
#define	PMUM_CPRR_DSPR					BIT_2				/* DSPR */
#define	PMUM_CPRR_APR					BIT_1				/* APR */
#define	PMUM_CPRR_CPR					BIT_0				/* CPR */

/*	PMUM_CCGR	0x0024	Marvell CPU Clock Gating Register */
#define	PMUM_CCGR_W_208M		BIT_31	/* W_208M */
#define	PMUM_CCGR_W_312M		BIT_30	/* W_312M */
/*		Bit(s) PMUM_CCGR_RSRV_29 reserved */
#define	PMUM_CCGR_G_LP52M		BIT_28	/* G_LP52M */
#define	PMUM_CCGR_G_LP26M		BIT_27	/* G_LP26M */
#define	PMUM_CCGR_G_104M		BIT_26	/* G_104M */
#define	PMUM_CCGR_G_78M			BIT_25	/* G_78M */
#define	PMUM_CCGR_G_52M			BIT_24	/* G_52M */
#define	PMUM_CCGR_G_48MHZ		BIT_23	/* G_48MHz */
#define	PMUM_CCGR_G_TWSI		BIT_22	/* G_TWSI */
#define	PMUM_CCGR_G_HFI2S		BIT_21	/* G_HFI2S */
#define	PMUM_CCGR_G_26M			BIT_20	/* G_26M */
#define	PMUM_CCGR_G_13M			BIT_19	/* G_13M */
#define	PMUM_CCGR_G_6_5M		BIT_18	/* G_6.5M */
#define	PMUM_CCGR_G_SUART		BIT_17	/* G_SUART */
/*		Bit(s) PMUM_CCGR_RSRV_16 reserved */
#define	PMUM_CCGR_APMU_624M		BIT_15	/* APMU_624M */
#define	PMUM_CCGR_APMU_PLL2		BIT_14	/* APMU_PLL2 */
#define	PMUM_CCGR_APMU_312M		BIT_13	/* APMU_312M */
#define	PMUM_CCGR_APMU_104M		BIT_12	/* APMU_104M */
#define	PMUM_CCGR_APMU_52M		BIT_11	/* APMU_52M */
#define	PMUM_CCGR_APMU_48M		BIT_10	/* APMU_48M */
#define	PMUM_CCGR_GPC			BIT_9	/* GPC */
#define	PMUM_CCGR_AP_FUART		BIT_8	/* AP_FUART */
#define	PMUM_CCGR_AP_52M		BIT_7	/* AP_52M */
#define	PMUM_CCGR_AP_HSTWSI		BIT_6	/* AP_HSTWSI */
#define	PMUM_CCGR_AP_HFI2S		BIT_5	/* AP_HFI2S */
#define	PMUM_CCGR_AP_26M		BIT_4	/* AP_26M */
#define	PMUM_CCGR_AP_13M		BIT_3	/* AP_13M */
#define	PMUM_CCGR_AP_6_5M		BIT_2	/* AP_6.5M */
#define	PMUM_CCGR_AP_SUART		BIT_1	/* AP_SUART */
/*		Bit(s) PMUM_CCGR_RSRV_0 reserved */

/*	PMUM_CRSR	0x0028	Marvell CPU Reset Status Register */
/*		Bit(s) PMUM_CRSR_RSRV_31_12 reserved */
#define	PMUM_CRSR_SWR_MSK				SHIFT8(0xf)			/* SWR */
#define	PMUM_CRSR_SWR_BASE				8
/*		Bit(s) PMUM_CRSR_RSRV_7_3 reserved */
#define	PMUM_CRSR_WDTR					BIT_2				/* WDTR */
/*		Bit(s) PMUM_CRSR_RSRV_1 reserved */
#define	PMUM_CRSR_POR					BIT_0				/* POR */

/*	PMUM_GPCR	0x0030	General Purpose Clock Generation Control Register */
#define	PMUM_GPCR_GPCDIVN_MSK		SHIFT16(0xffff)	/* GPCDIVN */
#define	PMUM_GPCR_GPCDIVN_BASE		16
#define	PMUM_GPCR_GPCDIVD_MSK		SHIFT0(0xffff)	/* GPCDIVD */
#define	PMUM_GPCR_GPCDIVD_BASE		0

/*	PMUM_PLL2CR	0x0034	PLL2 Control Register */
/*		Bit(s) PMUM_PLL2CR_RSRV_31_24 reserved */
#define	PMUM_PLL2CR_PLL2REFD_MSK		SHIFT19(0x1f)	/* PLL2 REFDIV */
#define	PMUM_PLL2CR_PLL2REFD_BASE		19
#define	PMUM_PLL2CR_PLL2FBD_MSK			SHIFT10(0x1ff)	/* PLL2 FBDIV */
#define	PMUM_PLL2CR_PLL2FBD_BASE		10
#define	PMUM_PLL2CR_CTRL				BIT_9			/* PLL2 activation control */
#define	PMUM_PLL2CR_EN					BIT_8			/* EN */
/*		Bit(s) PMUM_PLL2CR_RSRV_7_6 reserved */
/*		Bit(s) PMUM_PLL2CR_RSRV_5_0 reserved */

/*	PMUM_SCCR	0x0038	Slow Clock Control Register */
/*		Bit(s) PMUM_SCCR_RSRV_31_2 reserved */
#define	PMUM_SCCR_AFE_CLK_EN			BIT_1				/* AFE_CLK_EN */
#define	PMUM_SCCR_SCS					BIT_0				/* SCS */

/*	PMUM_MCCR	0x003C	MSA Clock Control Register */
/*		Bit(s) PMUM_MCCR_RSRV_31_2 reserved */
/* DSP Core Clock Frequency */
#define	PMUM_MCCR_DSP_FRQ				BIT_1				
/* DSP Clock PLL source select */
#define	PMUM_MCCR_DSP_CLK				BIT_0				

/*	PMUM_ISCCRx	0x0040	I2S Clock Generation Control Register */
#define	PMUM_ISCCRX_SYSCLK_EN				BIT_31			/* SYSCLK_EN */
#define	PMUM_ISCCRX_SYSCLK_BASE				BIT_30			/* SYSCLK_BASE */
#define	PMUM_ISCCRX_BITCLK_EN				BIT_29			/* BITCLK_EN */
#define	PMUM_ISCCRX_BITCLK_DIV_468_MSK		SHIFT27(0x3)	/* BITCLK_DIV_468 */
#define	PMUM_ISCCRX_BITCLK_DIV_468_BASE		27
#define	PMUM_ISCCRX_DENOM_MSK				SHIFT15(0xfff)	/* DENOM */
#define	PMUM_ISCCRX_DENOM_BASE				15
#define	PMUM_ISCCRX_NOM_MSK					SHIFT0(0x7fff)	/* NOM */
#define	PMUM_ISCCRX_NOM_BASE				0

/*	PMUM_CWUCRS	0x0048	Wakeup and Clock Resume Lines Status Register */
#define	PMUM_CWUCRS_BB_DROWSY_EN						BIT_31	/* Bb_drowsy_en */
/* Wakeup 0-7 status */
#define	PMUM_CWUCRS_ALL_WAKEUP							BIT_30	
/* Feis_wanna_wake/Pm_bb_clkres */
#define	PMUM_CWUCRS_FEIS_WANNA_WAKE_PM_BB_CLKRES		BIT_29	
#define	PMUM_CWUCRS_PM_XSC_CLKRES						BIT_28	/* Pm_xsc_clkres */
#define	PMUM_CWUCRS_GSM_WAKEUP							BIT_27	/* GSM Wakeup */
#define	PMUM_CWUCRS_WCDMA_WAKEUP						BIT_26	/* WCDMA Wakeup */
#define	PMUM_CWUCRS_AP_ASYNC_INT						BIT_25	/* AP ASYNC INT */
#define	PMUM_CWUCRS_AP_FULL_IDLE						BIT_24	/* AP Full idle */
#define	PMUM_CWUCRS_SDH1								BIT_23	/* SDH1 */
#define	PMUM_CWUCRS_SDH2								BIT_22	/* SDH2 */
#define	PMUM_CWUCRS_KEYPRESS							BIT_21	/* KeyPress */
#define	PMUM_CWUCRS_TRACKBALL							BIT_20	/* Trackball */
#define	PMUM_CWUCRS_NEWROTARY							BIT_19	/* NewRotary */
#define	PMUM_CWUCRS_WDT									BIT_18	/* WDT */
#define	PMUM_CWUCRS_RTC_ALARM							BIT_17	/* RTC ALARM */
#define	PMUM_CWUCRS_CP_TIMER_3							BIT_16	/* CP Timer 3 */
#define	PMUM_CWUCRS_CP_TIMER_2							BIT_15	/* CP Timer 2 */
#define	PMUM_CWUCRS_CP_TIMER_1							BIT_14	/* CP Timer 1 */
#define	PMUM_CWUCRS_AP2_TIMER_3							BIT_13	/* AP2 Timer 3 */
#define	PMUM_CWUCRS_AP2_TIMER_2							BIT_12	/* AP2 Timer 2 */
#define	PMUM_CWUCRS_AP2_TIMER_1							BIT_11	/* AP2 Timer 1 */
#define	PMUM_CWUCRS_AP1_TIMER_3							BIT_10	/* AP1 Timer 3 */
#define	PMUM_CWUCRS_AP1_TIMER_2							BIT_9	/* AP1 Timer 2 */
#define	PMUM_CWUCRS_AP1_TIMER_1							BIT_8	/* AP1 Timer 1 */
/* Wakeup6 line in status */
#define	PMUM_CWUCRS_WAKEUP6								BIT_6	
/* Wakeup5 line in status */
#define	PMUM_CWUCRS_WAKEUP5								BIT_5	
/* Wakeup4 line in status */
#define	PMUM_CWUCRS_WAKEUP4								BIT_4	
/* Wakeup3 line in status */
#define	PMUM_CWUCRS_WAKEUP3								BIT_3	
/* Wakeup2 line in status */
#define	PMUM_CWUCRS_WAKEUP2								BIT_2	
/* Wakeup1 line in status */
#define	PMUM_CWUCRS_WAKEUP1								BIT_1	
/* Wakeup0 line in status */
#define	PMUM_CWUCRS_WAKEUP0								BIT_0	

/*	PMUM_CWUCRM	0x004C	Wakeup and Clock Resume Lines Mask Register */
/*		Bit(s) PMUM_CWUCRM_RSRV_30_31 reserved */
#define	PMUM_CWUCRM_GSM_WAKEUPWMX		BIT_29		/* GSM WakeupWMX */
#define	PMUM_CWUCRM_WCDMA_WAKEUPX		BIT_28		/* WCDMA WakeupX */
#define	PMUM_CWUCRM_GSM_WAKEUPWM		BIT_27		/* GSM WakeupWM */
#define	PMUM_CWUCRM_WCDMA_WAKEUPWM		BIT_26		/* WCDMA WakeupWM */
#define	PMUM_CWUCRM_AP_ASYNC_INT		BIT_25		/* AP ASYNC INT */
#define	PMUM_CWUCRM_AP_FULL_IDLE		BIT_24		/* AP Full idle */
#define	PMUM_CWUCRM_SDH1				BIT_23		/* SDH1 */
#define	PMUM_CWUCRM_SDH2				BIT_22		/* SDH2 */
#define	PMUM_CWUCRM_KEYPRESS			BIT_21		/* KeyPress */
#define	PMUM_CWUCRM_TRACKBALL			BIT_20		/* Trackball */
#define	PMUM_CWUCRM_NEWROTARY			BIT_19		/* NewRotary */
#define	PMUM_CWUCRM_WDT					BIT_18		/* WDT */
#define	PMUM_CWUCRM_RTC_ALARM			BIT_17		/* RTC ALARM */
#define	PMUM_CWUCRM_CP_TIMER_3			BIT_16		/* CP Timer 3 */
#define	PMUM_CWUCRM_CP_TIMER_2			BIT_15		/* CP Timer 2 */
#define	PMUM_CWUCRM_CP_TIMER_1			BIT_14		/* CP Timer 1 */
#define	PMUM_CWUCRM_AP2_TIMER_3			BIT_13		/* AP2 Timer 3 */
#define	PMUM_CWUCRM_AP2_TIMER_2			BIT_12		/* AP2 Timer 2 */
#define	PMUM_CWUCRM_AP2_TIMER_1			BIT_11		/* AP2 Timer 1 */
#define	PMUM_CWUCRM_AP1_TIMER_3			BIT_10		/* AP1 Timer 3 */
#define	PMUM_CWUCRM_AP1_TIMER_2			BIT_9		/* AP1 Timer 2 */
#define	PMUM_CWUCRM_AP1_TIMER_1			BIT_8		/* AP1 Timer 1 */
#define	PMUM_CWUCRM_WAKEUP7				BIT_7		/* Wakeup7 */
#define	PMUM_CWUCRM_WAKEUP6				BIT_6		/* Wakeup6 */
#define	PMUM_CWUCRM_WAKEUP5				BIT_5		/* Wakeup5 */
#define	PMUM_CWUCRM_WAKEUP4				BIT_4		/* Wakeup4 */
#define	PMUM_CWUCRM_WAKEUP3				BIT_3		/* Wakeup3 */
#define	PMUM_CWUCRM_WAKEUP2				BIT_2		/* Wakeup2 */
#define	PMUM_CWUCRM_WAKEUP1				BIT_1		/* Wakeup1 */
#define	PMUM_CWUCRM_WAKEUP0				BIT_0		/* Wakeup0 */

/*	PMUM_DSOC	0x0100	Debug Signals Output Control Register */
/*		Bit(s) PMUM_DSOC_RSRV_31_9 reserved */
#define	PMUM_DSOC_TPCTL					BIT_8				/* TPCTL */
/*		Bit(s) PMUM_DSOC_RSRV_7 reserved */
#define	PMUM_DSOC_TPSEL_MSK				SHIFT0(0x7f)		/* TPSEL */
#define	PMUM_DSOC_TPSEL_BASE			0

/*	PMUM_WDTPCR	0x0200	WDT (Communication Subsystem Timers) Control Register */
/*		Bit(s) PMUM_WDTPCR_RSRV_31_7 reserved */
#define	PMUM_WDTPCR_FNCLKSEL_MSK		SHIFT4(0x7)			/* FNCLKSEL */
#define	PMUM_WDTPCR_FNCLKSEL_BASE		4
/*		Bit(s) PMUM_WDTPCR_RSRV_3 reserved */
#define	PMUM_WDTPCR_RST					BIT_2				/* RST */
#define	PMUM_WDTPCR_FNCLK				BIT_1				/* FNCLK */
#define	PMUM_WDTPCR_ABCLK				BIT_0				/* ABCLK */

/*	PMUM_CMPRx	0x0400	Marvell CPU Module Power Register */
/*		Bit(s) PMUM_CMPRX_RSRV_31_3 reserved */
#define	PMUM_CMPRX_MSR					BIT_2				/* MSR */
#define	PMUM_CMPRX_AG					BIT_1				/* AG */
#define	PMUM_CMPRX_MCE					BIT_0				/* MCE */

/*	PMUM_APCR	0x1000	Marvell CPU Power Control Register */
#define	PMUM_APCR_AXISD					BIT_31			/* AXISD */
#define	PMUM_APCR_DSPSD					BIT_30			/* DSPSD */
#define	PMUM_APCR_SLPEN					BIT_29			/* SLPEN */
#define	PMUM_APCR_DTCMSD				BIT_28			/* DTCMSD */
#define	PMUM_APCR_DDRCORSD				BIT_27			/* DDRCORSD */
#define	PMUM_APCR_APBSD					BIT_26			/* APBSD */
#define	PMUM_APCR_BBSD					BIT_25			/* BBSD */
#define	PMUM_APCR_INTCLR				BIT_24			/* INTCLR */
#define	PMUM_APCR_SLPWP0				BIT_23			/* SLPWP0 */
#define	PMUM_APCR_SLPWP1				BIT_22			/* SLPWP1 */
#define	PMUM_APCR_SLPWP2				BIT_21			/* SLPWP2 */
#define	PMUM_APCR_SLPWP3				BIT_20			/* SLPWP3 */
#define	PMUM_APCR_VCTCXOSD				BIT_19			/* VCTCXOSD */
#define	PMUM_APCR_SLPWP4				BIT_18			/* SLPWP4 */
#define	PMUM_APCR_SLPWP5				BIT_17			/* SLPWP5 */
#define	PMUM_APCR_SLPWP6				BIT_16			/* SLPWP6 */
#define	PMUM_APCR_SLPWP7				BIT_15			/* SLPWP7 */
#define	PMUM_APCR_MSASLPEN				BIT_14			/* MSASLPEN */

/*	PMUM_APSR	0x1004	Marvell CPU Power Status Register */
#define	PMUM_APSR_DSPIDL				BIT_31			/* DSPIDL */
#define	PMUM_APSR_APIDL					BIT_30			/* APIDL */
#define	PMUM_APSR_CPIDL					BIT_29			/* CPIDL */
#define	PMUM_APSR_DSPAVL				BIT_28			/* DSPAVL */
#define	PMUM_APSR_AP_FULL_IDLE			BIT_27			/* AP_FULL_IDLE */
#define	PMUM_APSR_CP_FULL_IDLE			BIT_26			/* CP_FULL_IDLE */
#define	PMUM_APSR_AP_SS_SLEEP			BIT_25			/* AP_SS_SLEEP */
#define	PMUM_APSR_CP_SS_SLEEP			BIT_24			/* CP_SS_SLEEP */
/*		Bit(s) PMUM_APSR_RSRV_23_21 reserved */
/*		Bit(s) PMUM_APSR_RSRV_20 reserved */
/*		Bit(s) PMUM_APSR_RSRV_19_17 reserved */
/*		Bit(s) PMUM_APSR_RSRV_16_0 reserved */

/*	PMUM_APRR	0x1020	Marvell CPU Programmable Reset Register */
/*		Bit(s) PMUM_APRR_RSRV_31_7 reserved */
#define	PMUM_APRR_SLAVE_R				BIT_6				/* SLAVE_R */
/*		Bit(s) PMUM_APRR_RSRV_5 reserved */
#define	PMUM_APRR_WDTR					BIT_4				/* WDTR */
/*		Bit(s) PMUM_APRR_RSRV_3 reserved */
#define	PMUM_APRR_DSPR					BIT_2				/* DSPR */
#define	PMUM_APRR_APR					BIT_1				/* APR */
#define	PMUM_APRR_CPR					BIT_0				/* CPR */

/*	PMUM_ACGR	0x1024	Marvell CPU Clock Gating Register */
#define	PMUM_ACGR_W_208M		BIT_31	/* W_208M */
#define	PMUM_ACGR_W_312M		BIT_30	/* W_312M */
/*		Bit(s) PMUM_ACGR_RSRV_29 reserved */
#define	PMUM_ACGR_G_LP52M		BIT_28	/* G_LP52M */
#define	PMUM_ACGR_G_LP26M		BIT_27	/* G_LP26M */
#define	PMUM_ACGR_G_104M		BIT_26	/* G_104M */
#define	PMUM_ACGR_G_78M			BIT_25	/* G_78M */
#define	PMUM_ACGR_G_52M			BIT_24	/* G_52M */
#define	PMUM_ACGR_G_48MHZ		BIT_23	/* G_48MHz */
#define	PMUM_ACGR_G_TWSI		BIT_22	/* G_TWSI */
#define	PMUM_ACGR_G_HFI2S		BIT_21	/* G_HFI2S */
#define	PMUM_ACGR_G_26M			BIT_20	/* G_26M */
#define	PMUM_ACGR_G_13M			BIT_19	/* G_13M */
#define	PMUM_ACGR_G_6_5M		BIT_18	/* G_6.5M */
#define	PMUM_ACGR_G_SUART		BIT_17	/* G_SUART */
/*		Bit(s) PMUM_ACGR_RSRV_16 reserved */
#define	PMUM_ACGR_APMU_624M		BIT_15	/* APMU_624M */
#define	PMUM_ACGR_APMU_PLL2		BIT_14	/* APMU_PLL2 */
#define	PMUM_ACGR_APMU_312M		BIT_13	/* APMU_312M */
#define	PMUM_ACGR_APMU_104M		BIT_12	/* APMU_104M */
#define	PMUM_ACGR_APMU_52M		BIT_11	/* APMU_52M */
#define	PMUM_ACGR_APMU_48M		BIT_10	/* APMU_48M */
#define	PMUM_ACGR_GPC			BIT_9	/* GPC */
#define	PMUM_ACGR_AP_FUART		BIT_8	/* AP_FUART */
#define	PMUM_ACGR_AP_52M		BIT_7	/* AP_52M */
#define	PMUM_ACGR_AP_HSTWSI		BIT_6	/* AP_HSTWSI */
#define	PMUM_ACGR_AP_HFI2S		BIT_5	/* AP_HFI2S */
#define	PMUM_ACGR_AP_26M		BIT_4	/* AP_26M */
#define	PMUM_ACGR_AP_13M		BIT_3	/* AP_13M */
#define	PMUM_ACGR_AP_6_5M		BIT_2	/* AP_6.5M */
#define	PMUM_ACGR_AP_SUART		BIT_1	/* AP_SUART */
/*		Bit(s) PMUM_ACGR_RSRV_0 reserved */

/*	PMUM_ARSR	0x1028	Marvell CPU Reset Status Register */
/*		Bit(s) PMUM_ARSR_RSRV_31_12 reserved */
#define	PMUM_ARSR_SWR_MSK				SHIFT8(0xf)			/* SWR */
#define	PMUM_ARSR_SWR_BASE				8
/*		Bit(s) PMUM_ARSR_RSRV_7_3 reserved */
#define	PMUM_ARSR_WDTR					BIT_2				/* WDTR */
/*		Bit(s) PMUM_ARSR_RSRV_1 reserved */
#define	PMUM_ARSR_POR					BIT_0				/* POR */

/*	PMUM_AWUCRS	0x1048	Wakeup and Clock Resume Lines Status Register */
#define	PMUM_AWUCRS_BB_DROWSY_EN		BIT_31	/* Bb_drowsy_en */
#define	PMUM_AWUCRS_ALL_WAKEUP			BIT_30	/* Wakeup 0-7 status */
#define	PMUM_AWUCRS_PM_BB_CLKRES		BIT_29	/* Pm_bb_clkres */
#define	PMUM_AWUCRS_PM_XSC_CLKRES		BIT_28	/* Pm_xsc_clkres */
#define	PMUM_AWUCRS_GSM_WAKEUP			BIT_27	/* GSM Wakeup */
#define	PMUM_AWUCRS_WCDMA_WAKEUP		BIT_26	/* WCDMA Wakeup */
#define	PMUM_AWUCRS_AP_ASYNC_INT		BIT_25	/* AP ASYNC INT */
#define	PMUM_AWUCRS_AP_FULL_IDLE		BIT_24	/* AP Full idle */
#define	PMUM_AWUCRS_SDH1				BIT_23	/* SDH1 */
#define	PMUM_AWUCRS_SDH2				BIT_22	/* SDH2 */
#define	PMUM_AWUCRS_KEYPRESS			BIT_21	/* KeyPress */
#define	PMUM_AWUCRS_TRACKBALL			BIT_20	/* Trackball */
#define	PMUM_AWUCRS_NEWROTARY			BIT_19	/* NewRotary */
#define	PMUM_AWUCRS_WDT					BIT_18	/* WDT */
#define	PMUM_AWUCRS_RTC_ALARM			BIT_17	/* RTC ALARM */
#define	PMUM_AWUCRS_CP_TIMER_3			BIT_16	/* CP Timer 3 */
#define	PMUM_AWUCRS_CP_TIMER_2			BIT_15	/* CP Timer 2 */
#define	PMUM_AWUCRS_CP_TIMER_1			BIT_14	/* CP Timer 1 */
#define	PMUM_AWUCRS_AP2_TIMER_3			BIT_13	/* AP2 Timer 3 */
#define	PMUM_AWUCRS_AP2_TIMER_2			BIT_12	/* AP2 Timer 2 */
#define	PMUM_AWUCRS_AP2_TIMER_1			BIT_11	/* AP2 Timer 1 */
#define	PMUM_AWUCRS_AP1_TIMER_3			BIT_10	/* AP1 Timer 3 */
#define	PMUM_AWUCRS_AP1_TIMER_2			BIT_9	/* AP1 Timer 2 */
#define	PMUM_AWUCRS_AP1_TIMER_1			BIT_8	/* AP1 Timer 1 */
#define	PMUM_AWUCRS_WAKEUP6				BIT_6	/* Wakeup6 line in status */
#define	PMUM_AWUCRS_WAKEUP5				BIT_5	/* Wakeup5 line in status */
#define	PMUM_AWUCRS_WAKEUP4				BIT_4	/* Wakeup4 line in status */
#define	PMUM_AWUCRS_WAKEUP3				BIT_3	/* Wakeup3 line in status */
#define	PMUM_AWUCRS_WAKEUP2				BIT_2	/* Wakeup2 line in status */
#define	PMUM_AWUCRS_WAKEUP1				BIT_1	/* Wakeup1 line in status */
#define	PMUM_AWUCRS_WAKEUP0				BIT_0	/* Wakeup0 line in status */

/*	PMUM_AWUCRM	0x104C	Wakeup and Clock Resume Lines Mask Register */
/*		Bit(s) PMUM_AWUCRM_RSRV_30_31 reserved */
#define	PMUM_AWUCRM_GSM_WAKEUPWMX		BIT_29		/* GSM WakeupWMX */
#define	PMUM_AWUCRM_WCDMA_WAKEUPX		BIT_28		/* WCDMA WakeupX */
#define	PMUM_AWUCRM_GSM_WAKEUPWM		BIT_27		/* GSM WakeupWM */
#define	PMUM_AWUCRM_WCDMA_WAKEUPWM		BIT_26		/* WCDMA WakeupWM */
#define	PMUM_AWUCRM_AP_ASYNC_INT		BIT_25		/* AP ASYNC INT */
#define	PMUM_AWUCRM_AP_FULL_IDLE		BIT_24		/* AP Full idle */
#define	PMUM_AWUCRM_SDH1				BIT_23		/* SDH1 */
#define	PMUM_AWUCRM_SDH2				BIT_22		/* SDH2 */
#define	PMUM_AWUCRM_KEYPRESS			BIT_21		/* KeyPress */
#define	PMUM_AWUCRM_TRACKBALL			BIT_20		/* Trackball */
#define	PMUM_AWUCRM_NEWROTARY			BIT_19		/* NewRotary */
#define	PMUM_AWUCRM_WDT					BIT_18		/* WDT */
#define	PMUM_AWUCRM_RTC_ALARM			BIT_17		/* RTC ALARM */
#define	PMUM_AWUCRM_CP_TIMER_3			BIT_16		/* CP Timer 3 */
#define	PMUM_AWUCRM_CP_TIMER_2			BIT_15		/* CP Timer 2 */
#define	PMUM_AWUCRM_CP_TIMER_1			BIT_14		/* CP Timer 1 */
#define	PMUM_AWUCRM_AP2_TIMER_3			BIT_13		/* AP2 Timer 3 */
#define	PMUM_AWUCRM_AP2_TIMER_2			BIT_12		/* AP2 Timer 2 */
#define	PMUM_AWUCRM_AP2_TIMER_1			BIT_11		/* AP2 Timer 1 */
#define	PMUM_AWUCRM_AP1_TIMER_3			BIT_10		/* AP1 Timer 3 */
#define	PMUM_AWUCRM_AP1_TIMER_2			BIT_9		/* AP1 Timer 2 */
#define	PMUM_AWUCRM_AP1_TIMER_1			BIT_8		/* AP1 Timer 1 */
#define	PMUM_AWUCRM_WAKEUP7				BIT_7		/* Wakeup7 */
#define	PMUM_AWUCRM_WAKEUP6				BIT_6		/* Wakeup6 */
#define	PMUM_AWUCRM_WAKEUP5				BIT_5		/* Wakeup5 */
#define	PMUM_AWUCRM_WAKEUP4				BIT_4		/* Wakeup4 */
#define	PMUM_AWUCRM_WAKEUP3				BIT_3		/* Wakeup3 */
#define	PMUM_AWUCRM_WAKEUP2				BIT_2		/* Wakeup2 */
#define	PMUM_AWUCRM_WAKEUP1				BIT_1		/* Wakeup1 */
#define	PMUM_AWUCRM_WAKEUP0				BIT_0		/* Wakeup0 */



/* -------------------- */


#endif	/* __INC_PMUM_H */

/******************************************************************************
 *
 * Name:	PMUA.h
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
 * This file was automatically generated by reg.pl using  *	PMUA.csv
 *
 ******************************************************************************/

/******************************************************************************
 *
 * History:
 *
 ********* PLEASE INSERT THE CVS HISTORY OF THE PREVIOUS VERSION HERE. *********
 *******************************************************************************/

#ifndef	__INC_PMUA_H
#define	__INC_PMUA_H


/*
 *
 *	THE BASE ADDRESSES
 *
 */
#define	PMUA_BASE	0xD4282800

/*
 *
 *	THE REGISTER DEFINES
 *
 */
#define	PMUA_CC_SEA					(PMUA_BASE+0x0000)	/* 32 bit	<var
														 *			Processor:
														 *			Osprey>
														 *			Clock
														 *			Control
														 *			Register
														 */
#define	PMUA_CC_MOH					(PMUA_BASE+0x0004)	/* 32 bit	<var
														 *			Processor:
														 *			Mohawk>
														 *			Clock
														 *			Control
														 *			Register
														 */
#define	PMUA_DM_CC_SEA				(PMUA_BASE+0x0008)	/* 32 bit	<var
														 *			Processor:
														 *			Osprey>
														 *			Clock
														 *			Control
														 *			Status
														 *			Register
														 */
#define	PMUA_DM_CC_MOH				(PMUA_BASE+0x000C)	/* 32 bit	<var
														 *			Processor:
														 *			Mohawk>
														 *			Clock
														 *			Control
														 *			Status
														 *			Register
														 */
#define	PMUA_FC_TIMER				(PMUA_BASE+0x0010)	/* 32 bit	Frequency
														 *			Change
														 *			Timer
														 *			Register
														 */
#define	PMUA_SEA_IDLE_CFG			(PMUA_BASE+0x0014)	/* 32 bit	<var
														 *			Processor:
														 *			Osprey>
														 *			Idle
														 *			Configuration
														 *			Register
														 */
#define	PMUA_MOH_IDLE_CFG			(PMUA_BASE+0x0018)	/* 32 bit	<var
														 *			Processor:
														 *			Mohawk>
														 *			Idle
														 *			Configuration
														 *			Register
														 */
#define	PMUA_SQU_CLK_GATE_CTRL		(PMUA_BASE+0x001C)	/* 32 bit	SQU Dynamic
														 *			Clock
														 *			Gate
														 *			Control
														 *			Register
														 */
#define	PMUA_IRE_CLK_GATE_CTRL		(PMUA_BASE+0x0020)	/* 32 bit	IRE Dynamic
														 *			Clock
														 *			Gate
														 *			Control
														 *			Register
														 */
#define	PMUA_CCIC_CLK_GATE_CTRL		(PMUA_BASE+0x0028)	/* 32 bit	CCIC
														 *			Dynamic
														 *			Clock
														 *			Gate
														 *			Control
														 *			Register
														 */
#define	PMUA_FBRC1_CLK_GATE_CTRL	(PMUA_BASE+0x002C)	/* 32 bit	Fabric 1
														 *			Dynamic
														 *			Clock
														 *			Gate
														 *			Control
														 *			Register
														 */
#define	PMUA_FBRC2_CLK_GATE_CTRL	(PMUA_BASE+0x0030)	/* 32 bit	Fabric 2
														 *			Dynamic
														 *			Clock
														 *			Gate
														 *			Control
														 *			Register
														 */
#define	PMUA_USB_CLK_GATE_CTRL		(PMUA_BASE+0x0034)	/* 32 bit	USB Dynamic
														 *			Clock
														 *			Gate
														 *			Control
														 *			Register
														 */
#define	PMUA_PMU_CLK_GATE_CTRL		(PMUA_BASE+0x0040)	/* 32 bit	PMU Dynamic
														 *			Clock
														 *			Gate
														 *			Control
														 *			Register
														 */
#define	PMUA_IRE_CLK_RES_CTRL		(PMUA_BASE+0x0048)	/* 32 bit	IRE
														 *			Clock/Reset
														 *			Control
														 *			Register
														 */
#define	PMUA_LCD_CLK_RES_CTRL		(PMUA_BASE+0x004C)	/* 32 bit	LCD
														 *			Clock/Reset
														 *			Control
														 *			Register
														 */
#define	PMUA_CCIC_CLK_RES_CTRL		(PMUA_BASE+0x0050)	/* 32 bit	CCIC
														 *			Clock/Reset
														 *			Control
														 *			Register
														 */
#define	PMUA_SDH0_CLK_RES_CTRL		(PMUA_BASE+0x0054)	/* 32 bit	SDIO Host 0
														 *			Clock/Reset
														 *			Control
														 *			Register
														 */
#define	PMUA_SDH1_CLK_RES_CTRL		(PMUA_BASE+0x0058)	/* 32 bit	SDIO Host 1
														 *			Clock/Reset
														 *			Control
														 *			Register
														 */
#define	PMUA_USB_CLK_RES_CTRL		(PMUA_BASE+0x005C)	/* 32 bit	USB
														 *			Clock/Reset
														 *			Control
														 *			Register
														 */
#define	PMUA_NF_CLK_RES_CTRL		(PMUA_BASE+0x0060)	/* 32 bit	NAND Flash
														 *			Clock/Reset
														 *			Control
														 *			Register
														 */
#define	PMUA_DMA_CLK_RES_CTRL		(PMUA_BASE+0x0064)	/* 32 bit	DMA
														 *			Clock/Reset
														 *			Control
														 *			Register
														 */
#define	PMUA_GEU_CLK_RES_CTRL		(PMUA_BASE+0x0068)	/* 32 bit	GEU
														 *			Clock/Reset
														 *			Control
														 *			Register
														 */
#define	PMUA_BUS_CLK_RES_CTRL		(PMUA_BASE+0x006C)	/* 32 bit	Bus
														 *			Clock/Reset
														 *			Control
														 *			Register
														 */
#define	PMUA_SEA_IMR				(PMUA_BASE+0x0070)	/* 32 bit	PMU <var
														 *			Processor:
														 *			Osprey>
														 *			Interrupt
														 *			Mask
														 *			Register
														 */
#define	PMUA_SEA_IRWC				(PMUA_BASE+0x0074)	/* 32 bit	PMU <var
														 *			Processor:
														 *			Osprey>
														 *			Interrupt
														 *			READ/WRITE
														 *			Clear
														 *			Register
														 */
#define	PMUA_SEA_ISR				(PMUA_BASE+0x0078)	/* 32 bit	PMU <var
														 *			Processor:
														 *			Osprey>
														 *			Interrupt
														 *			Status
														 *			Register
														 */
#define	PMUA_SD_ROT_WAKE_CLR		(PMUA_BASE+0x007C)	/* 32 bit	SDIO/Rotary
														 *			Wake
														 *			Clear
														 *			Register
														 */
#define	PMUA_PWR_STBL_TIMER			(PMUA_BASE+0x0084)	/* 32 bit	Power
														 *			Stable
														 *			Timer
														 *			Register
														 */
#define	PMUA_SRAM_PWR_DWN			(PMUA_BASE+0x008C)	/* 32 bit	Core SRAM
														 *			Power
														 *			Down
														 *			Register
														 */
#define	PMUA_CORE_STATUS			(PMUA_BASE+0x0090)	/* 32 bit	Core Status
														 *			Register
														 */
#define	PMUA_RES_FRM_SLP_CLR		(PMUA_BASE+0x0094)	/* 32 bit	Resume from
														 *			Sleep
														 *			Clear
														 *			Register
														 */
#define	PMUA_MOH_IMR				(PMUA_BASE+0x0098)	/* 32 bit	PMU <var
														 *			Processor:
														 *			Mohawk>
														 *			Interrupt
														 *			Mask
														 *			Register
														 */
#define	PMUA_MOH_IRWC				(PMUA_BASE+0x009C)	/* 32 bit	PMU <var
														 *			Processor:
														 *			Mohawk>
														 *			Interrupt
														 *			READ/WRITE
														 *			Clear
														 *			Register
														 */
#define	PMUA_MOH_ISR				(PMUA_BASE+0x00A0)	/* 32 bit	PMU <var
														 *			Processor:
														 *			Mohawk>
														 *			Interrupt
														 *			Status
														 *			Register
														 */
#define	PMUA_DTC_CLK_RES_CTRL		(PMUA_BASE+0x00AC)	/* 32 bit	DTC
														 *			Clock/Reset
														 *			Control
														 *			Register
														 */
#define	PMUA_MC_HW_SLP_TYPE			(PMUA_BASE+0x00B0)	/* 32 bit	Memory
														 *			Controller
														 *			Hardware
														 *			Sleep
														 *			Type
														 *			Register
														 */
#define	PMUA_MC_SLP_REQ_MOH			(PMUA_BASE+0x00B4)	/* 32 bit	Memory
														 *			Controller
														 *			<var
														 *			Processor:
														 *			Mohawk>
														 *			Sleep
														 *			Request
														 *			Register
														 */
#define	PMUA_MC_SLP_REQ_SEA			(PMUA_BASE+0x00B8)	/* 32 bit	Memory
														 *			Controller
														 *			<var
														 *			Processor:
														 *			Osprey>
														 *			Sleep
														 *			Request
														 *			Register
														 */
#define	PMUA_MC_SLP_REQ_MSA			(PMUA_BASE+0x00BC)	/* 32 bit	Memory
														 *			Controller
														 *			MSA Sleep
														 *			Request
														 *			Register
														 */
#define	PMUA_MC_SW_SLP_TYPE			(PMUA_BASE+0x00C0)	/* 32 bit	Memory
														 *			Controller
														 *			Software
														 *			Sleep
														 *			Type
														 *			Register
														 */
#define	PMUA_PLL_SEL_STATUS			(PMUA_BASE+0x00C4)	/* 32 bit	PLL Clock
														 *			Select
														 *			Status
														 *			Register
														 */
#define	PMUA_GC_CLK_RES_CTRL		(PMUA_BASE+0x00CC)	/* 32 bit	2D/3D
														 *			Graphics
														 *			Controller
														 *			Clock/Reset
														 *			Control
														 *			Register
														 */
#define	PMUA_GC_PWRDWN				(PMUA_BASE+0x00D0)	/* 32 bit	2D/3D
														 *			Graphics
														 *			Controller
														 *			Power
														 *			Down
														 *			Register
														 */
#define	PMUA_SMC_CLK_RES_CTRL		(PMUA_BASE+0x00D4)	/* 32 bit	SMC
														 *			Clock/Reset
														 *			Control
														 *			Register
														 */

/*
 *
 *	THE BIT DEFINES
 *
 */
/*	PMUA_CC_SEA					0x0000	<var Processor: Osprey> Clock Control
 *										Register
 */
#define	PMUA_CC_SEA_SEA_RD_ST_CLEAR			BIT_31			/* RD_ST Clear */
/* Dynamic ACLK Clock Change */
#define	PMUA_CC_SEA_ACLK_DYN_FC				BIT_30			
/* Dynamic DCLK Clock Change */
#define	PMUA_CC_SEA_DCLK_DYN_FC				BIT_29			
/* Dynamic Core Clock Change */
#define	PMUA_CC_SEA_CORE_DYN_FC				BIT_28			
/* <var Processor: Osprey> Speed Change Voting */
#define	PMUA_CC_SEA_SEA_ALLOW_SPD_CHG		BIT_27			
/* ACLK Frequency Change Request */
#define	PMUA_CC_SEA_BUS_FREQ_CHG_REQ		BIT_26			
/* DCLK Frequency Change Request */
#define	PMUA_CC_SEA_DDR_FREQ_CHG_REQ		BIT_25			
/* <var Processor: Osprey> Frequency Change Request */
#define	PMUA_CC_SEA_SEA_FREQ_CHG_REQ		BIT_24			
/* <var Processor: Mohawk> Bus Interface Sync with Bus Interface */
#define	PMUA_CC_SEA_ASYNC5					BIT_23			
/* <var Processor: Osprey> Bus Interface Sync with Bus Interface */
#define	PMUA_CC_SEA_ASYNC4					BIT_22			
/* AXI Bus Interface Sync with DDR Controller */
#define	PMUA_CC_SEA_ASYNC3_1				BIT_21			
/* AXI Bus Interface Sync with DDR Controller */
#define	PMUA_CC_SEA_ASYNC3					BIT_20			
/* <var Processor: Mohawk> Bus Interface Sync with DDR Controller */
#define	PMUA_CC_SEA_ASYNC2					BIT_19			
/* <var Processor: Osprey> Bus Interface Sync with DDR Controller */
#define	PMUA_CC_SEA_ASYNC1					BIT_18			
/* Clock Divider Selection for ACLK */
#define	PMUA_CC_SEA_BUS_CLK_DIV_MSK			SHIFT15(0x7)	
#define	PMUA_CC_SEA_BUS_CLK_DIV_BASE		15
/* Clock Divider Selection for DCLK */
#define	PMUA_CC_SEA_DDR_CLK_DIV_MSK			SHIFT12(0x7)	
#define	PMUA_CC_SEA_DDR_CLK_DIV_BASE		12
/* Clock Divider Selection for XPCLK */
#define	PMUA_CC_SEA_XP_CLK_DIV_MSK			SHIFT9(0x7)		
#define	PMUA_CC_SEA_XP_CLK_DIV_BASE			9
/* Clock Divider Selection for BACLK1 */
#define	PMUA_CC_SEA_BIU_CLK_DIV_MSK			SHIFT6(0x7)		
#define	PMUA_CC_SEA_BIU_CLK_DIV_BASE		6
/* Clock Divider Selection for PDCLK1 */
#define	PMUA_CC_SEA_BUS_MC_CLK_DIV_MSK		SHIFT3(0x7)		
#define	PMUA_CC_SEA_BUS_MC_CLK_DIV_BASE		3
/* Clock Divider Selection for PCLK */
#define	PMUA_CC_SEA_CORE_CLK_DIV_MSK		SHIFT0(0x7)		
#define	PMUA_CC_SEA_CORE_CLK_DIV_BASE		0

/*	PMUA_CC_MOH					0x0004	<var Processor: Mohawk> Clock Control
 *										Register
 */
#define	PMUA_CC_MOH_MOH_RD_ST_CLEAR			BIT_31			/* RD_ST Clear */
/* Dynamic ACLK Clock Change */
#define	PMUA_CC_MOH_ACLK_DYN_FC				BIT_30			
/* Dynamic DCLK Clock Change */
#define	PMUA_CC_MOH_DCLK_DYN_FC				BIT_29			
/* Dynamic Core Clock Change */
#define	PMUA_CC_MOH_CORE_DYN_FC				BIT_28			
/* <var Processor: Mohawk> Speed Change Voting */
#define	PMUA_CC_MOH_MOH_ALLOW_SPD_CHG		BIT_27			
/* ACLK Frequency Change Request */
#define	PMUA_CC_MOH_BUS_FREQ_CHG_REQ		BIT_26			
/* DCLK Frequency Change Request */
#define	PMUA_CC_MOH_DDR_FREQ_CHG_REQ		BIT_25			
/* <var Processor: Mohawk> Frequency Change Request */
#define	PMUA_CC_MOH_MOH_FREQ_CHG_REQ		BIT_24			
/* <var Processor: Mohawk> Bus Interface Sync with Bus Interface */
#define	PMUA_CC_MOH_ASYNC5					BIT_23			
/* <var Processor: Osprey> Bus Interface Sync with Bus Interface */
#define	PMUA_CC_MOH_ASYNC4					BIT_22			
/* AXI Bus Interface Sync with DDR Controller */
#define	PMUA_CC_MOH_ASYNC3_1				BIT_21			
/* AXI Bus Interface Sync with DDR Controller */
#define	PMUA_CC_MOH_ASYNC3					BIT_20			
/* <var Processor: Mohawk> Bus Interface Sync with DDR Controller */
#define	PMUA_CC_MOH_ASYNC2					BIT_19			
/* <var Processor: Osprey> Bus Interface Sync with DDR Controller */
#define	PMUA_CC_MOH_ASYNC1					BIT_18			
/* Clock Divider Selection for ACLK */
#define	PMUA_CC_MOH_BUS_CLK_DIV_MSK			SHIFT15(0x7)	
#define	PMUA_CC_MOH_BUS_CLK_DIV_BASE		15
/* Clock Divider Selection for DCLK */
#define	PMUA_CC_MOH_DDR_CLK_DIV_MSK			SHIFT12(0x7)	
#define	PMUA_CC_MOH_DDR_CLK_DIV_BASE		12
/* Clock Divider Selection for XPCLK */
#define	PMUA_CC_MOH_XP_CLK_DIV_MSK			SHIFT9(0x7)		
#define	PMUA_CC_MOH_XP_CLK_DIV_BASE			9
/* Clock Divider Selection for BACLK1 */
#define	PMUA_CC_MOH_BIU_CLK_DIV_MSK			SHIFT6(0x7)		
#define	PMUA_CC_MOH_BIU_CLK_DIV_BASE		6
/* Clock Divider Selection for PDCLK1 */
#define	PMUA_CC_MOH_BUS_MC_CLK_DIV_MSK		SHIFT3(0x7)		
#define	PMUA_CC_MOH_BUS_MC_CLK_DIV_BASE		3
/* Clock Divider Selection for PCLK */
#define	PMUA_CC_MOH_CORE_CLK_DIV_MSK		SHIFT0(0x7)		
#define	PMUA_CC_MOH_CORE_CLK_DIV_BASE		0

/*	PMUA_DM_CC_SEA				0x0008	<var Processor: Osprey> Clock Control
 *										Status Register
 */
/*		Bit(s) PMUA_DM_CC_SEA_RSRV_31_26 reserved */
/* <var Processor: Mohawk> Read Status */
#define	PMUA_DM_CC_SEA_MOH_RD_STATUS			BIT_25			
/* <var Processor: Osprey> Read Status */
#define	PMUA_DM_CC_SEA_SEA_RD_STATUS			BIT_24			
/* <var Processor: Mohawk> Bus Interface Sync with Bus Interface */
#define	PMUA_DM_CC_SEA_ASYNC5					BIT_23			
/* <var Processor: Osprey> Bus Interface Sync with Bus Interface */
#define	PMUA_DM_CC_SEA_ASYNC4					BIT_22			
/* AXI Bus Interface Sync with DDR Controller */
#define	PMUA_DM_CC_SEA_ASYNC3_1					BIT_21			
/* AXI Bus Interface Sync with DDR Controller */
#define	PMUA_DM_CC_SEA_ASYNC3					BIT_20			
/* <var Processor: Mohawk> Bus Interface Sync with DDR Controller */
#define	PMUA_DM_CC_SEA_ASYNC2					BIT_19			
/* <var Processor: Osprey> Bus Interface Sync with DDR Controller */
#define	PMUA_DM_CC_SEA_ASYNC1					BIT_18			
/* Clock Divider Selection for ACLK */
#define	PMUA_DM_CC_SEA_BUS_CLK_DIV_MSK			SHIFT15(0x7)	
#define	PMUA_DM_CC_SEA_BUS_CLK_DIV_BASE			15
/* Clock Divider Selection for DCLK */
#define	PMUA_DM_CC_SEA_DDR_CLK_DIV_MSK			SHIFT12(0x7)	
#define	PMUA_DM_CC_SEA_DDR_CLK_DIV_BASE			12
/* Clock Divider Selection for XPCLK */
#define	PMUA_DM_CC_SEA_XP_CLK_DIV_MSK			SHIFT9(0x7)		
#define	PMUA_DM_CC_SEA_XP_CLK_DIV_BASE			9
/* Clock Divider Selection for BACLK1 */
#define	PMUA_DM_CC_SEA_BIU_CLK_DIV_MSK			SHIFT6(0x7)		
#define	PMUA_DM_CC_SEA_BIU_CLK_DIV_BASE			6
/* Clock Divider Selection for PDCLK1 */
#define	PMUA_DM_CC_SEA_BUS_MC_CLK_DIV_MSK		SHIFT3(0x7)		
#define	PMUA_DM_CC_SEA_BUS_MC_CLK_DIV_BASE		3
/* Clock Divider Selection for PCLK */
#define	PMUA_DM_CC_SEA_CORE_CLK_DIV_MSK			SHIFT0(0x7)		
#define	PMUA_DM_CC_SEA_CORE_CLK_DIV_BASE		0

/*	PMUA_DM_CC_MOH				0x000C	<var Processor: Mohawk> Clock Control
 *										Status Register
 */
/*		Bit(s) PMUA_DM_CC_MOH_RSRV_31_26 reserved */
/* <var Processor: Mohawk> Read Status */
#define	PMUA_DM_CC_MOH_MOH_RD_STATUS			BIT_25			
/* <var Processor: Osprey> Read Status */
#define	PMUA_DM_CC_MOH_SEA_RD_STATUS			BIT_24			
/* <var Processor: Mohawk> Bus Interface Sync with Bus Interface */
#define	PMUA_DM_CC_MOH_ASYNC5					BIT_23			
/* <var Processor: Osprey> Bus Interface Sync with Bus Interface */
#define	PMUA_DM_CC_MOH_ASYNC4					BIT_22			
/* AXI Bus Interface Sync with DDR Controller */
#define	PMUA_DM_CC_MOH_ASYNC3_1					BIT_21			
/* AXI Bus Interface Sync with DDR Controller */
#define	PMUA_DM_CC_MOH_ASYNC3					BIT_20			
/* <var Processor: Mohawk> Bus Interface Sync with DDR Controller */
#define	PMUA_DM_CC_MOH_ASYNC2					BIT_19			
/* <var Processor: Osprey> Bus Interface Sync with DDR Controller */
#define	PMUA_DM_CC_MOH_ASYNC1					BIT_18			
/* Clock Divider Selection for ACLK */
#define	PMUA_DM_CC_MOH_BUS_CLK_DIV_MSK			SHIFT15(0x7)	
#define	PMUA_DM_CC_MOH_BUS_CLK_DIV_BASE			15
/* Clock Divider Selection for DCLK */
#define	PMUA_DM_CC_MOH_DDR_CLK_DIV_MSK			SHIFT12(0x7)	
#define	PMUA_DM_CC_MOH_DDR_CLK_DIV_BASE			12
/* Clock Divider Selection for XPCLK */
#define	PMUA_DM_CC_MOH_XP_CLK_DIV_MSK			SHIFT9(0x7)		
#define	PMUA_DM_CC_MOH_XP_CLK_DIV_BASE			9
/* Clock Divider Selection for BACLK1 */
#define	PMUA_DM_CC_MOH_BIU_CLK_DIV_MSK			SHIFT6(0x7)		
#define	PMUA_DM_CC_MOH_BIU_CLK_DIV_BASE			6
/* Clock Divider Selection for PDCLK1 */
#define	PMUA_DM_CC_MOH_BUS_MC_CLK_DIV_MSK		SHIFT3(0x7)		
#define	PMUA_DM_CC_MOH_BUS_MC_CLK_DIV_BASE		3
/* Clock Divider Selection for PCLK */
#define	PMUA_DM_CC_MOH_CORE_CLK_DIV_MSK			SHIFT0(0x7)		
#define	PMUA_DM_CC_MOH_CORE_CLK_DIV_BASE		0

/*	PMUA_FC_TIMER				0x0010	Frequency Change Timer Register */
/*		Bit(s) PMUA_FC_TIMER_RSRV_31_24 reserved */
#define	PMUA_FC_TIMER_FC_CLK_PRE_MSK		SHIFT19(0x1f)	/* Clock Prescaler */
#define	PMUA_FC_TIMER_FC_CLK_PRE_BASE		19
/* Frequency Change Timer */
#define	PMUA_FC_TIMER_FC_TIMER_MSK			SHIFT3(0xffff)	
#define	PMUA_FC_TIMER_FC_TIMER_BASE			3
#define	PMUA_FC_TIMER_FC_TIMER_SEL_MSK		SHIFT1(0x3)		/* Timer Select */
#define	PMUA_FC_TIMER_FC_TIMER_SEL_BASE		1
#define	PMUA_FC_TIMER_FC_TIMER_EN			BIT_0			/* Timer Enable */

/*	PMUA_SEA_IDLE_CFG			0x0014	<var Processor: Osprey> Idle
 *										Configuration Register
 */
/*		Bit(s) PMUA_SEA_IDLE_CFG_RSRV_31_22 reserved */
/* <var Processor: Osprey> Disable MC SW REQ */
#define	PMUA_SEA_IDLE_CFG_SEA_DIS_MC_SW_REQ			BIT_21			
/* <var Processor: Osprey> MC Wake Enable */
#define	PMUA_SEA_IDLE_CFG_SEA_MC_WAKE_EN			BIT_20			
/* <var Processor: Osprey> L2 Power Switch */
#define	PMUA_SEA_IDLE_CFG_SEA_L2_PWR_SW_MSK			SHIFT18(0x3)	
#define	PMUA_SEA_IDLE_CFG_SEA_L2_PWR_SW_BASE		18
/* <var Processor: Osprey> Core Power Switch */
#define	PMUA_SEA_IDLE_CFG_SEA_PWR_SW_MSK			SHIFT16(0x3)	
#define	PMUA_SEA_IDLE_CFG_SEA_PWR_SW_BASE			16
/* <var Processor: Osprey> L2 Disable Sleep 2 */
#define	PMUA_SEA_IDLE_CFG_SEA_DIS_L2_SLP2			BIT_15			
/* <var Processor: Osprey> L2 Disable Sleep 1 */
#define	PMUA_SEA_IDLE_CFG_SEA_DIS_L2_SLP1			BIT_14			
/* <var Processor: Osprey> Disable Sleep 2 */
#define	PMUA_SEA_IDLE_CFG_SEA_DIS_SLP2				BIT_13			
/* <var Processor: Osprey> Disable Sleep 1 */
#define	PMUA_SEA_IDLE_CFG_SEA_DIS_SLP1				BIT_12			
/* <var Processor: Osprey> Mask JTAG Idle */
#define	PMUA_SEA_IDLE_CFG_SEA_MASK_JTAG_IDLE		BIT_11			
/* <var Processor: Osprey> L2 Controller Clock Enable/Disable */
#define	PMUA_SEA_IDLE_CFG_SEA_L2_CLK_DIS			BIT_10			
/* <var Processor: Osprey> L2 Controller Reset */
#define	PMUA_SEA_IDLE_CFG_SEA_L2_RESETN				BIT_9			
/* <var Processor: Osprey> L2 Isolation Enable */
#define	PMUA_SEA_IDLE_CFG_SEA_L2_ISOB				BIT_8			
/* <var Processor: Osprey> L2 Controller SRAM Power Off */
#define	PMUA_SEA_IDLE_CFG_SEA_L2_PWR_OFF			BIT_7			
/* <var Processor: Osprey> SRAM Power Down */
#define	PMUA_SEA_IDLE_CFG_SEA_SRAM_PWRDWN			BIT_6			
/* <var Processor: Osprey> Power Down */
#define	PMUA_SEA_IDLE_CFG_SEA_PWRDWN				BIT_5			
/*		Bit(s) PMUA_SEA_IDLE_CFG_RSRV_4_0 reserved */

/*	PMUA_MOH_IDLE_CFG			0x0018	<var Processor: Mohawk> Idle
 *										Configuration Register
 */
/*		Bit(s) PMUA_MOH_IDLE_CFG_RSRV_31_22 reserved */
/* <var Processor: Mohawk> Disable MC SW REQ */
#define	PMUA_MOH_IDLE_CFG_MOH_DIS_MC_SW_REQ			BIT_21			
/* <var Processor: Mohawk> MC Wake Enable */
#define	PMUA_MOH_IDLE_CFG_MOH_MC_WAKE_EN			BIT_20			
/* <var Processor: Mohawk> L2 Power Switch */
#define	PMUA_MOH_IDLE_CFG_MOH_L2_PWR_SW_MSK			SHIFT18(0x3)	
#define	PMUA_MOH_IDLE_CFG_MOH_L2_PWR_SW_BASE		18
/* <var Processor: Mohawk> Core Power Switch */
#define	PMUA_MOH_IDLE_CFG_MOH_PWR_SW_MSK			SHIFT16(0x3)	
#define	PMUA_MOH_IDLE_CFG_MOH_PWR_SW_BASE			16
/* <var Processor: Mohawk> L2 Disable Sleep 2 */
#define	PMUA_MOH_IDLE_CFG_MOH_DIS_L2_SLP2			BIT_15			
/* <var Processor: Mohawk> L2 Disable Sleep 1 */
#define	PMUA_MOH_IDLE_CFG_MOH_DIS_L2_SLP1			BIT_14			
/* <var Processor: Mohawk> Disable Sleep 2 */
#define	PMUA_MOH_IDLE_CFG_MOH_DIS_SLP2				BIT_13			
/* <var Processor: Mohawk> Disable Sleep 1 */
#define	PMUA_MOH_IDLE_CFG_MOH_DIS_SLP1				BIT_12			
/* <var Processor: Mohawk> Mask JTAG Idle */
#define	PMUA_MOH_IDLE_CFG_MOH_MASK_JTAG_IDLE		BIT_11			
/* <var Processor: Mohawk> L2 Clock Enable/Disable */
#define	PMUA_MOH_IDLE_CFG_MOH_L2_CLK_DIS			BIT_10			
/* <var Processor: Mohawk> L2 Controller Reset */
#define	PMUA_MOH_IDLE_CFG_MOH_L2_RESETN				BIT_9			
/* <var Processor: Mohawk> L2 Controller Isolation Enable */
#define	PMUA_MOH_IDLE_CFG_MOH_L2_ISOB				BIT_8			
/* <var Processor: Mohawk> L2 Controller SRAM Power Off */
#define	PMUA_MOH_IDLE_CFG_MOH_L2_PWR_OFF			BIT_7			
/* <var Processor: Mohawk> SRAM Power Down */
#define	PMUA_MOH_IDLE_CFG_MOH_SRAM_PWRDWN			BIT_6			
/* <var Processor: Mohawk> Power Down */
#define	PMUA_MOH_IDLE_CFG_MOH_PWRDWN				BIT_5			
/*		Bit(s) PMUA_MOH_IDLE_CFG_RSRV_4_2 reserved */
/* <var Processor: Mohawk> Idle */
#define	PMUA_MOH_IDLE_CFG_MOH_IDLE					BIT_1			
/*		Bit(s) PMUA_MOH_IDLE_CFG_RSRV_0 reserved */

/*	PMUA_SQU_CLK_GATE_CTRL		0x001C	SQU Dynamic Clock Gate Control Register */
/* SQU Dynamic Clock Gate Control */
#define	PMUA_SQU_CLK_GATE_CTRL_SQU_CLK_GATE_CTRL_MSK		SHIFT0(0xffffffff)	
#define	PMUA_SQU_CLK_GATE_CTRL_SQU_CLK_GATE_CTRL_BASE		0

/*	PMUA_IRE_CLK_GATE_CTRL		0x0020	IRE Dynamic Clock Gate Control Register */
/*		Bit(s) PMUA_IRE_CLK_GATE_CTRL_RSRV_31_8 reserved */
/* IRE Dynamic Clock Gate Control */
#define	PMUA_IRE_CLK_GATE_CTRL_IRE_CLK_GATE_CTRL_MSK		SHIFT0(0xff)		
#define	PMUA_IRE_CLK_GATE_CTRL_IRE_CLK_GATE_CTRL_BASE		0

/*	PMUA_CCIC_CLK_GATE_CTRL		0x0028	CCIC Dynamic Clock Gate Control
 *										Register
 */
/*		Bit(s) PMUA_CCIC_CLK_GATE_CTRL_RSRV_31_16 reserved */
/* CCIC Dynamic Clock Gate Control */
#define	PMUA_CCIC_CLK_GATE_CTRL_CCIC_CLK_GATE_CTRL_MSK		SHIFT0(0xffff)	
#define	PMUA_CCIC_CLK_GATE_CTRL_CCIC_CLK_GATE_CTRL_BASE		0

/*	PMUA_FBRC1_CLK_GATE_CTRL	0x002C	Fabric 1 Dynamic Clock Gate Control
 *										Register
 */
/* Fabric 1 Dynamic Clock Gate Control */
#define	PMUA_FBRC1_CLK_GATE_CTRL_FBRC1_CLK_GATE_CTRL_MSK			SHIFT0(0xffffffff)	
#define	PMUA_FBRC1_CLK_GATE_CTRL_FBRC1_CLK_GATE_CTRL_BASE			0

/*	PMUA_FBRC2_CLK_GATE_CTRL	0x0030	Fabric 2 Dynamic Clock Gate Control
 *										Register
 */
/* Fabric 2 Dynamic Clock Gate Control */
#define	PMUA_FBRC2_CLK_GATE_CTRL_FBRC2_CLK_GATE_CTRL_MSK		SHIFT0(0xffffffff)	
#define	PMUA_FBRC2_CLK_GATE_CTRL_FBRC2_CLK_GATE_CTRL_BASE		0

/*	PMUA_USB_CLK_GATE_CTRL		0x0034	USB Dynamic Clock Gate Control Register */
/*		Bit(s) PMUA_USB_CLK_GATE_CTRL_RSRV_31_16 reserved */
/* USB Dynamic Clock Gate Control */
#define	PMUA_USB_CLK_GATE_CTRL_USB_CLK_GATE_CTRL_MSK		SHIFT0(0xffff)	
#define	PMUA_USB_CLK_GATE_CTRL_USB_CLK_GATE_CTRL_BASE		0

/*	PMUA_PMU_CLK_GATE_CTRL		0x0040	PMU Dynamic Clock Gate Control Register */
/*		Bit(s) PMUA_PMU_CLK_GATE_CTRL_RSRV_31_24 reserved */
/* PMU Dynamic Clock Gate Control */
#define	PMUA_PMU_CLK_GATE_CTRL_PMU_CLK_GATE_CTRL_MSK		SHIFT0(0xffffff)	
#define	PMUA_PMU_CLK_GATE_CTRL_PMU_CLK_GATE_CTRL_BASE		0

/*	PMUA_IRE_CLK_RES_CTRL		0x0048	IRE Clock/Reset Control Register */
/*		Bit(s) PMUA_IRE_CLK_RES_CTRL_RSRV_31_4 reserved */
/* IRE AXI Clock Enable */
#define	PMUA_IRE_CLK_RES_CTRL_IRE_AXICLK_EN			BIT_3				
/*		Bit(s) PMUA_IRE_CLK_RES_CTRL_RSRV_2_1 reserved */
/* IRE AXI Reset */
#define	PMUA_IRE_CLK_RES_CTRL_IRE_AXI_RST			BIT_0				

/*	PMUA_LCD_CLK_RES_CTRL		0x004C	LCD Clock/Reset Control Register */
/*		Bit(s) PMUA_LCD_CLK_RES_CTRL_RSRV_31_7 reserved */
/* LCD Controller Clock Select */
#define	PMUA_LCD_CLK_RES_CTRL_LCD_CLK_SEL			BIT_6				
/*		Bit(s) PMUA_LCD_CLK_RES_CTRL_RSRV_5 reserved */
/* LCD Peripheral Clock Enable */
#define	PMUA_LCD_CLK_RES_CTRL_LCD_CLK_EN			BIT_4				
/* LCD AXI Clock Enable */
#define	PMUA_LCD_CLK_RES_CTRL_LCD_AXICLK_EN			BIT_3				
/*		Bit(s) PMUA_LCD_CLK_RES_CTRL_RSRV_2 reserved */
/* LCD Peripheral Reset */
#define	PMUA_LCD_CLK_RES_CTRL_LCD_RST				BIT_1				
/* LCD AXI Reset */
#define	PMUA_LCD_CLK_RES_CTRL_LCD_AXI_RST			BIT_0				

/*	PMUA_CCIC_CLK_RES_CTRL		0x0050	CCIC Clock/Reset Control Register */
/*		Bit(s) PMUA_CCIC_CLK_RES_CTRL_RSRV_31_15 reserved */
/* CCIC PHY Slow Clock Prescaler */
#define	PMUA_CCIC_CLK_RES_CTRL_CCIC_PHYSLOW_PRER_MSK		SHIFT10(0x1f)		
#define	PMUA_CCIC_CLK_RES_CTRL_CCIC_PHYSLOW_PRER_BASE		10
/* CCIC PHY SLOW Clock Enable */
#define	PMUA_CCIC_CLK_RES_CTRL_CCIC_PHYSLOWCLK_EN			BIT_9				
/* CCIC PHY Slow Reset */
#define	PMUA_CCIC_CLK_RES_CTRL_CCIC_PHYSLOW_RST				BIT_8				
/* CCIC PHY Clock Select */
#define	PMUA_CCIC_CLK_RES_CTRL_CCIC_PHYCLK_SEL				BIT_7				
/* CCIC Clock Select */
#define	PMUA_CCIC_CLK_RES_CTRL_CCIC_CLK_SEL					BIT_6				
/* CCIC PHY Clock Enable */
#define	PMUA_CCIC_CLK_RES_CTRL_CCIC_PHYCLK_EN				BIT_5				
/* CCIC Peripheral Clock Enable */
#define	PMUA_CCIC_CLK_RES_CTRL_CCIC_CLK_EN					BIT_4				
/* CCIC AXI Clock Enable */
#define	PMUA_CCIC_CLK_RES_CTRL_CCIC_AXICLK_EN				BIT_3				
/* CCIC PHY Reset */
#define	PMUA_CCIC_CLK_RES_CTRL_CCIC_PHY_RST					BIT_2				
/* CCIC Peripheral Reset */
#define	PMUA_CCIC_CLK_RES_CTRL_CCIC_RST						BIT_1				
/* CCIC AXI Reset */
#define	PMUA_CCIC_CLK_RES_CTRL_CCIC_AXI_RST					BIT_0				

/*	PMUA_SDH0_CLK_RES_CTRL		0x0054	SDIO Host 0 Clock/Reset Control
 *										Register
 */
/*		Bit(s) PMUA_SDH0_CLK_RES_CTRL_RSRV_31_7 reserved */
/* SDIO Host 0 Clock Select */
#define	PMUA_SDH0_CLK_RES_CTRL_SDH0_CLK_SEL			BIT_6				
/*		Bit(s) PMUA_SDH0_CLK_RES_CTRL_RSRV_5 reserved */
/* SDIO Host 0 Peripheral Clock Enable */
#define	PMUA_SDH0_CLK_RES_CTRL_SDH0_CLK_EN			BIT_4				
/* SDIO Host 0 AXI Clock Enable */
#define	PMUA_SDH0_CLK_RES_CTRL_SDH0_AXICLK_EN		BIT_3				
/*		Bit(s) PMUA_SDH0_CLK_RES_CTRL_RSRV_2 reserved */
/* SDIO Host 0 Peripheral Reset */
#define	PMUA_SDH0_CLK_RES_CTRL_SDH0_RST				BIT_1				
/* SDIO Host 0 AXI Reset */
#define	PMUA_SDH0_CLK_RES_CTRL_SDH0_AXI_RST			BIT_0				

/*	PMUA_SDH1_CLK_RES_CTRL		0x0058	SDIO Host 1 Clock/Reset Control
 *										Register
 */
/*		Bit(s) PMUA_SDH1_CLK_RES_CTRL_RSRV_31_7 reserved */
/* SDIO Host 1 Clock Select */
#define	PMUA_SDH1_CLK_RES_CTRL_SDH1_CLK_SEL			BIT_6				
/*		Bit(s) PMUA_SDH1_CLK_RES_CTRL_RSRV_5 reserved */
/* SDIO Host 1 Peripheral Clock Enable */
#define	PMUA_SDH1_CLK_RES_CTRL_SDH1_CLK_EN			BIT_4				
/* SDIO Host 1 AXI Clock Enable */
#define	PMUA_SDH1_CLK_RES_CTRL_SDH1_AXICLK_EN		BIT_3				
/*		Bit(s) PMUA_SDH1_CLK_RES_CTRL_RSRV_2 reserved */
/* SDIO Host 1 Peripheral Reset */
#define	PMUA_SDH1_CLK_RES_CTRL_SDH1_RST				BIT_1				
/* SDIO Host 1 AXI Reset */
#define	PMUA_SDH1_CLK_RES_CTRL_SDH1_AXI_RST			BIT_0				

/*	PMUA_USB_CLK_RES_CTRL		0x005C	USB Clock/Reset Control Register */
/*		Bit(s) PMUA_USB_CLK_RES_CTRL_RSRV_31_5 reserved */
/* USB SPH AXI Clock Enable */
#define	PMUA_USB_CLK_RES_CTRL_USB_SPH_AXICLK_EN		BIT_4				
/* USB AXI Clock Enable */
#define	PMUA_USB_CLK_RES_CTRL_USB_AXICLK_EN			BIT_3				
/*		Bit(s) PMUA_USB_CLK_RES_CTRL_RSRV_2 reserved */
/* USB SPH AXI Reset */
#define	PMUA_USB_CLK_RES_CTRL_USB_SPH_AXI_RST		BIT_1				
/* USB AXI Reset */
#define	PMUA_USB_CLK_RES_CTRL_USB_AXI_RST			BIT_0				

/*	PMUA_NF_CLK_RES_CTRL		0x0060	NAND Flash Clock/Reset Control Register */
/*		Bit(s) PMUA_NF_CLK_RES_CTRL_RSRV_31_9 reserved */
/* NAND Flash ECC Reset */
#define	PMUA_NF_CLK_RES_CTRL_NF_ECC_RES			BIT_8				
/* NAND Flash ECC Clock Enable */
#define	PMUA_NF_CLK_RES_CTRL_NF_ECC_CLK_EN		BIT_7				
/* NAND Flash Clock Select */
#define	PMUA_NF_CLK_RES_CTRL_NF_CLK_SEL			BIT_6				
/*		Bit(s) PMUA_NF_CLK_RES_CTRL_RSRV_5 reserved */
/* NAND Flash Peripheral Clock Enable */
#define	PMUA_NF_CLK_RES_CTRL_NF_CLK_EN			BIT_4				
/* NAND Flash AXI Clock Enable */
#define	PMUA_NF_CLK_RES_CTRL_NF_AXICLK_EN		BIT_3				
/*		Bit(s) PMUA_NF_CLK_RES_CTRL_RSRV_2 reserved */
/* NAND Flash Peripheral Reset */
#define	PMUA_NF_CLK_RES_CTRL_NF_RST				BIT_1				
/* NAND Flash AXI Reset */
#define	PMUA_NF_CLK_RES_CTRL_NF_AXI_RST			BIT_0				

/*	PMUA_DMA_CLK_RES_CTRL		0x0064	DMA Clock/Reset Control Register */
/*		Bit(s) PMUA_DMA_CLK_RES_CTRL_RSRV_31_4 reserved */
/* DMA AXI Clock Enable */
#define	PMUA_DMA_CLK_RES_CTRL_DMA_AXICLK_EN			BIT_3				
/*		Bit(s) PMUA_DMA_CLK_RES_CTRL_RSRV_2_1 reserved */
/* DMA AXI Reset */
#define	PMUA_DMA_CLK_RES_CTRL_DMA_AXI_RST			BIT_0				

/*	PMUA_GEU_CLK_RES_CTRL		0x0068	GEU Clock/Reset Control Register */
/*		Bit(s) PMUA_GEU_CLK_RES_CTRL_RSRV_31_4 reserved */
/* GEU AXI Clock Enable */
#define	PMUA_GEU_CLK_RES_CTRL_GEU_AXICLK_EN			BIT_3				
/*		Bit(s) PMUA_GEU_CLK_RES_CTRL_RSRV_2_1 reserved */
/* GEU AXI Reset */
#define	PMUA_GEU_CLK_RES_CTRL_GEU_AXI_RST			BIT_0				

/*	PMUA_BUS_CLK_RES_CTRL		0x006C	Bus Clock/Reset Control Register */
/*		Bit(s) PMUA_BUS_CLK_RES_CTRL_RSRV_31_2 reserved */
/* Bus Reset */
#define	PMUA_BUS_CLK_RES_CTRL_BUS_RST				BIT_1				
/* Memory Controller Reset */
#define	PMUA_BUS_CLK_RES_CTRL_MC_RST				BIT_0				

/*	PMUA_SEA_IMR				0x0070	PMU <var Processor: Osprey> Interrupt
 *										Mask Register
 */
/*		Bit(s) PMUA_SEA_IMR_RSRV_31_2 reserved */
/* Frequency Change done by <var Processor: Mohawk> Interrupt Mask */
#define	PMUA_SEA_IMR_MOH_FC_INTR_MASK				BIT_1				
/* Frequency Change done by <var Processor: Osprey> Interrupt Mask */
#define	PMUA_SEA_IMR_SEA_FC_INTR_MASK				BIT_0				

/*	PMUA_SEA_IRWC				0x0074	PMU <var Processor: Osprey> Interrupt
 *										READ/WRITE Clear Register
 */
/*		Bit(s) PMUA_SEA_IRWC_RSRV_31_2 reserved */
/* <var Processor: Mohawk> Frequency Change Interrupt Read Clear */
#define	PMUA_SEA_IRWC_MOH_FC_INTR_IRST				BIT_1				
/* <var Processor: Osprey> Frequency Change Interrupt Read Clear */
#define	PMUA_SEA_IRWC_SEA_FC_INTR_IRST				BIT_0				

/*	PMUA_SEA_ISR				0x0078	PMU <var Processor: Osprey> Interrupt
 *										Status Register
 */
/*		Bit(s) PMUA_SEA_ISR_RSRV_31_2 reserved */
/* <var Processor: Mohawk> Frequency Change Interrupt Status */
#define	PMUA_SEA_ISR_MOH_FC_ISR					BIT_1				
/* <var Processor: Osprey> Frequency Change Interrupt Status */
#define	PMUA_SEA_ISR_SEA_FC_ISR					BIT_0				

/*	PMUA_SD_ROT_WAKE_CLR		0x007C	SDIO/Rotary Wake Clear Register */
/*		Bit(s) PMUA_SD_ROT_WAKE_CLR_RSRV_31_3 reserved */
/* Rotary Wake Clear */
#define	PMUA_SD_ROT_WAKE_CLR_ROT_WK_CLR			BIT_2				
/* SDIO Host 1 Wake Clear */
#define	PMUA_SD_ROT_WAKE_CLR_SDH1_WK_CLR		BIT_1				
/* SDIO Host 0 Wake Clear */
#define	PMUA_SD_ROT_WAKE_CLR_SDH0_WK_CLR		BIT_0				

/*	PMUA_PWR_STBL_TIMER			0x0084	Power Stable Timer Register */
/*		Bit(s) PMUA_PWR_STBL_TIMER_RSRV_31_24 reserved */
/* Clock Prescaler */
#define	PMUA_PWR_STBL_TIMER_PWR_CLK_PRE_MSK				SHIFT16(0xff)	
#define	PMUA_PWR_STBL_TIMER_PWR_CLK_PRE_BASE			16
/* Power up Stable Timer */
#define	PMUA_PWR_STBL_TIMER_PWR_UP_STBL_TIMER_MSK		SHIFT8(0xff)	
#define	PMUA_PWR_STBL_TIMER_PWR_UP_STBL_TIMER_BASE		8
/* Power Down Stable Timer */
#define	PMUA_PWR_STBL_TIMER_PWR_DWN_STBL_TIMER_MSK		SHIFT0(0xff)	
#define	PMUA_PWR_STBL_TIMER_PWR_DWN_STBL_TIMER_BASE		0

/*	PMUA_DEBUG					0x0088	PMU Debug Register */

/*	PMUA_SRAM_PWR_DWN			0x008C	Core SRAM Power Down Register */
/* SRAM Power Down */
#define	PMUA_SRAM_PWR_DWN_SRAM_PWRDWN_MSK		SHIFT0(0xffffffff)	
#define	PMUA_SRAM_PWR_DWN_SRAM_PWRDWN_BASE		0

/*	PMUA_CORE_STATUS			0x0090	Core Status Register */
/*		Bit(s) PMUA_CORE_STATUS_RSRV_31_4 reserved */
/* <var Processor: Mohawk> <q>core idle mode</q> Indication */
#define	PMUA_CORE_STATUS_MOH_IDLE			BIT_3				
/* <var Processor: Mohawk> <q>WFI idle mode</q> Indication */
#define	PMUA_CORE_STATUS_MOH_WFI			BIT_2				
/* <var Processor: Osprey><q>core idle mode</q> Indication */
#define	PMUA_CORE_STATUS_SEA_IDLE			BIT_1				
/* <var Processor: Osprey><q>WFI idle mode</q> Indication */
#define	PMUA_CORE_STATUS_SEA_WFI			BIT_0				

/*	PMUA_RES_FRM_SLP_CLR		0x0094	Resume from Sleep Clear Register */
/*		Bit(s) PMUA_RES_FRM_SLP_CLR_RSRV_31_1 reserved */
/* Clear Resume from Sleep Indication */
#define	PMUA_RES_FRM_SLP_CLR_CLR_RSM_FRM_SLP		BIT_0				

/*	PMUA_MOH_IMR				0x0098	PMU <var Processor: Mohawk> Interrupt
 *										Mask Register
 */
/*		Bit(s) PMUA_MOH_IMR_RSRV_31_2 reserved */
/* Frequency Change done by <var Processor: Mohawk> Interrupt Mask */
#define	PMUA_MOH_IMR_MOH_FC_INTR_MASK				BIT_1				
/* Frequency Change done by <var Processor: Osprey>Interrupt Mask */
#define	PMUA_MOH_IMR_SEA_FC_INTR_MASK				BIT_0				

/*	PMUA_MOH_IRWC				0x009C	PMU <var Processor: Mohawk> Interrupt
 *										READ/WRITE Clear Register
 */
/*		Bit(s) PMUA_MOH_IRWC_RSRV_31_2 reserved */
/* <var Processor: Mohawk> FC Interrupt Read Clear */
#define	PMUA_MOH_IRWC_MOH_FC_INTR_IRST				BIT_1				
/* <var Processor: Osprey>FC Interrupt Read Clear */
#define	PMUA_MOH_IRWC_SEA_FC_INTR_IRST				BIT_0				

/*	PMUA_MOH_ISR				0x00A0	PMU <var Processor: Mohawk> Interrupt
 *										Status Register
 */
/*		Bit(s) PMUA_MOH_ISR_RSRV_31_2 reserved */
/* <var Processor: Mohawk> Frequency Change Interrupt Status */
#define	PMUA_MOH_ISR_MOH_FC_ISR					BIT_1				
/* <var Processor: Osprey> Frequency Change Interrupt Status */
#define	PMUA_MOH_ISR_SEA_FC_ISR					BIT_0				

/*	PMUA_DTC_CLK_RES_CTRL		0x00AC	DTC Clock/Reset Control Register */
/*		Bit(s) PMUA_DTC_CLK_RES_CTRL_RSRV_31_4 reserved */
/* DTC AXI Clock Enable */
#define	PMUA_DTC_CLK_RES_CTRL_DTC_AXICLK_EN			BIT_3				
/*		Bit(s) PMUA_DTC_CLK_RES_CTRL_RSRV_2_1 reserved */
/* DTC AXI Reset */
#define	PMUA_DTC_CLK_RES_CTRL_DTC_AXI_RST			BIT_0				

/*	PMUA_MC_HW_SLP_TYPE			0x00B0	Memory Controller Hardware Sleep Type
 *										Register
 */
/*		Bit(s) PMUA_MC_HW_SLP_TYPE_RSRV_31_3 reserved */
/* Memory Controller Hardware Sleep Type */
#define	PMUA_MC_HW_SLP_TYPE_MC_HW_SLP_TYPE_MSK		SHIFT0(0x7)			
#define	PMUA_MC_HW_SLP_TYPE_MC_HW_SLP_TYPE_BASE		0

/*	PMUA_MC_SLP_REQ_MOH			0x00B4	Memory Controller <var Processor:
 *										Mohawk> Sleep Request Register
 */
/*		Bit(s) PMUA_MC_SLP_REQ_MOH_RSRV_31_2 reserved */
/* Memory Controller Low Power Acknowledge */
#define	PMUA_MC_SLP_REQ_MOH_MC_SLP_ACK			BIT_1				
/* Memory Controller <var Processor: Mohawk> Sleep Request */
#define	PMUA_MC_SLP_REQ_MOH_MC_SLP_REQ_MOH		BIT_0				

/*	PMUA_MC_SLP_REQ_SEA			0x00B8	Memory Controller <var Processor:
 *										Osprey> Sleep Request Register
 */
/*		Bit(s) PMUA_MC_SLP_REQ_SEA_RSRV_31_2 reserved */
/* Memory Controller Low Power Acknowledge */
#define	PMUA_MC_SLP_REQ_SEA_MC_SLP_ACK			BIT_1				
/* Memory Controller <var Processor: Osprey> Sleep Request */
#define	PMUA_MC_SLP_REQ_SEA_MC_SLP_REQ_SEA		BIT_0				

/*	PMUA_MC_SLP_REQ_MSA			0x00BC	Memory Controller MSA Sleep Request
 *										Register
 */
/*		Bit(s) PMUA_MC_SLP_REQ_MSA_RSRV_31_2 reserved */
/* Memory Controller Low Power Acknowledge */
#define	PMUA_MC_SLP_REQ_MSA_MC_SLP_ACK			BIT_1				
/* Memory Controller MSA Sleep Request */
#define	PMUA_MC_SLP_REQ_MSA_MC_SLP_REQ_MSA		BIT_0				

/*	PMUA_MC_SW_SLP_TYPE			0x00C0	Memory Controller Software Sleep Type
 *										Register
 */
/*		Bit(s) PMUA_MC_SW_SLP_TYPE_RSRV_31_3 reserved */
/* Software Controlled Sleep Type for Memory Controller */
#define	PMUA_MC_SW_SLP_TYPE_MC_SW_SLP_TYPE_MSK		SHIFT0(0x7)			
#define	PMUA_MC_SW_SLP_TYPE_MC_SW_SLP_TYPE_BASE		0

/*	PMUA_PLL_SEL_STATUS			0x00C4	PLL Clock Select Status Register */
/*		Bit(s) PMUA_PLL_SEL_STATUS_RSRV_31_8 reserved */
/* BUS Clock PLL Select */
#define	PMUA_PLL_SEL_STATUS_ACLK_PLL_SEL_MSK		SHIFT6(0x3)			
#define	PMUA_PLL_SEL_STATUS_ACLK_PLL_SEL_BASE		6
/* Memory Clock PLL Select */
#define	PMUA_PLL_SEL_STATUS_DCLK_PLL_SEL_MSK		SHIFT4(0x3)			
#define	PMUA_PLL_SEL_STATUS_DCLK_PLL_SEL_BASE		4
/* <var Processor: Mohawk> PLL Select */
#define	PMUA_PLL_SEL_STATUS_MOH_PLL_SEL_MSK			SHIFT2(0x3)			
#define	PMUA_PLL_SEL_STATUS_MOH_PLL_SEL_BASE		2
/* <var Processor: Osprey> PLL Select */
#define	PMUA_PLL_SEL_STATUS_SEA_PLL_SEL_MSK			SHIFT0(0x3)			
#define	PMUA_PLL_SEL_STATUS_SEA_PLL_SEL_BASE		0

/*	PMUA_SYNC_MODE_BYPASS		0x00C8	SYNC MODE BYPASS Register */

/*	PMUA_GC_CLK_RES_CTRL		0x00CC	2D/3D Graphics Controller Clock/Reset
 *										Control Register
 */
/*		Bit(s) PMUA_GC_CLK_RES_CTRL_RSRV_31_7 reserved */
/*		Bit(s) PMUA_GC_CLK_RES_CTRL_RSRV_5 reserved */
/* 2D/3D Graphics Controller Peripheral Clock Enable */
#define	PMUA_GC_CLK_RES_CTRL_GC_CLK_EN				BIT_4				
/* 2D/3D Graphics Controller AXI Clock Enable */
#define	PMUA_GC_CLK_RES_CTRL_GC_AXICLK_EN			BIT_3				
/*		Bit(s) PMUA_GC_CLK_RES_CTRL_RSRV_2 reserved */
/* 2D/3D Graphics Controller Peripheral Reset 1 */
#define	PMUA_GC_CLK_RES_CTRL_GC_RST1				BIT_1				
/* 2D/3D Graphics Controller AXI Reset */
#define	PMUA_GC_CLK_RES_CTRL_GC_AXI_RST				BIT_0				

/*	PMUA_GC_PWRDWN				0x00D0	2D/3D Graphics Controller Power Down
 *										Register
 */
/*		Bit(s) PMUA_GC_PWRDWN_RSRV_31_5 reserved */
/* 2D/3D Graphics Controller Isolation Enable */
#define	PMUA_GC_PWRDWN_GC_ISB				BIT_4				
/* 2D/3D Graphics Controller 3D Power Up */
#define	PMUA_GC_PWRDWN_GC3D_PWRUP_MSK		SHIFT2(0x3)			
#define	PMUA_GC_PWRDWN_GC3D_PWRUP_BASE		2
/* 2D/3D Graphics Controller Power Up */
#define	PMUA_GC_PWRDWN_GC_PWRUP_MSK			SHIFT0(0x3)			
#define	PMUA_GC_PWRDWN_GC_PWRUP_BASE		0

/*	PMUA_SMC_CLK_RES_CTRL		0x00D4	SMC Clock/Reset Control Register */
/*		Bit(s) PMUA_SMC_CLK_RES_CTRL_RSRV_31_7 reserved */
/* SMC Peripheral Clock Select */
#define	PMUA_SMC_CLK_RES_CTRL_SMC_CLK_SEL			BIT_6				
/*		Bit(s) PMUA_SMC_CLK_RES_CTRL_RSRV_5 reserved */
/* SMC Peripheral Clock Enable */
#define	PMUA_SMC_CLK_RES_CTRL_SMC_CLK_EN			BIT_4				
/* SMC AXI Clock Enable */
#define	PMUA_SMC_CLK_RES_CTRL_SMC_AXICLK_EN			BIT_3				
/*		Bit(s) PMUA_SMC_CLK_RES_CTRL_RSRV_2 reserved */
/* SMC Peripheral Reset */
#define	PMUA_SMC_CLK_RES_CTRL_SMC_RST				BIT_1				
/* SMC AXI Reset */
#define	PMUA_SMC_CLK_RES_CTRL_SMC_AXI_RST			BIT_0				



/* -------------------- */


#endif	/* __INC_PMUA_H */

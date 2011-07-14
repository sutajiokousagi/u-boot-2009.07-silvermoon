#ifndef _BU_INTERRUPT_H_
#define _BU_INTERRUPT_H_

#include "common.h"
#include "icu.h"

#define	IPC_PRIORITY 	15
#define	DMA_PRIORITY	15
#define	SD_PRIORITY	15
#define	TIMER_PRIORITY	15

// definition for bit 4 of ICU_INT_0_63_CONF
#define	FIQ_ROUTE	0x0
#define	IRQ_ROUTE	0x1

#define	INT_NUMS	64
#define N_IRQS		INT_NUMS

// ICU Hardware Interrupt Assignment
//Description	INT REQ			APB signal		Comments

#define		INT_AIRQ		0
#define		INT_SSP_2		1
#define		INT_SSP_1		2			//ssp1_int_req
#define		INT_SSP_0		3			//ssp_int_req
#define		INT_PMIC		4			//PAD -- m1_PMIC_INT
#define		INT_RTC_1HZ		5			//rtc_hzclk_int_ndr
#define		INT_RTC_ALARM		6			//rtc_slp_alarm_ndr
#define		INT_I2C_AP		7			//i2c0_int
#define		INT_GPU			8			//reserved for AP
#define		INT_KeyPad		9			//kp_int
#define		INT_Rotary		10
#define		INT_Tackball		11
#define		INT_1WIRE		12
#define		INT_AP_TMR1		13			//timer_1_irq
#define		INT_AP_TMR2		14			//timer_2_irq
#define		INT_AP_TMR3		15			//timer_3_irq

#define		INT_IPC_AP_DATAACK	16			//ipc_946_926_int0
#define		INT_IPC_AP_SET_CMD	17			//ipc_946_926_int1
#define		INT_IPC_AP_SET_MSG	18			//ipc_946_926_int2
#define		INT_IPC_AP_SET_GP	19			//ipc_946_926_int3
#define		INT_IPC_AP_WAKEUP	20			//ipc2pmu_946_wakeup
#define		INT_IPC_CP_DATAACK	21			//ipc_926_946_int0
#define		INT_IPC_CP_SET_CMD	22			//ipc_926_946_int1
#define		INT_IPC_CP_SET_MSG	23			//ipc_926_946_int2
#define		INT_IPC_CP_SET_GP	24			//ipc_926_946_int3
#define		INT_IPC_CP_WAKEUP	25			//ipc2pmu_926_wakeup

#define		INT_DDR			26			//DDR
#define		INT_UART2		27			//ffuart1_int
#define		INT_UART3		28			//ffuart2_int
#define		INT_AP2_TMR1		29			//timer1_1_irq
#define		INT_AP2_TMR2		30			//timer1_2_irq
#define		INT_CP_TMR1		31			//timer_1_irq
#define		INT_CP_TMR2		32			//timer_2_irq
#define		INT_CP_TMR3		33			//timer_3_irq
#define		INT_GSSP 		34			//COMM -- (PCM on MSA)
#define		INT_WDT			35			//wdt_irq
#define		INT_Main_PMU		36			//PMU
#define		INT_CP_FRQ_CHG		37			//PMU
#define		INT_AP_FRQ_CHG		38			//PMU
#define		INT_MMC			39			//sdh2icu_int only 1 int from SD (2)
#define		INT_AEU			40			//aeu_int
#define		INT_LCD_INTF		41			//lcd_irq
#define		INT_CCIC		42			//ipe_irq
#define		INT_IRE			43			//Rotation Enging ire_irq
#define		INT_USB			44			//usb_int
#define		INT_NAND		45			//nd2icu_int
#define		INT_SQU_DMA		46			//squ_int net name dc_int
#define		INT_DMA_CP		47			//dma_int0
#define		INT_DMA_AP		48			//dma_int1
#define		INT_GPIO_AP		49			//gpio_int
#define		INT_AP2_TMR3		50
#define		INT_USB_2		51			//reserved

#define		INT_IPC_SRV0_SG		52
#define		INT_RESERVED_53		53
#define		INT_TWIS		54
#define		INT_GPIO_CP		55
#define		INT_IPC_SRV0_MK		56
#define		INT_RESERVED_57		57
#define		INT_RESERVED_58		58
#define		INT_UART1		59
#define		INT_AP_PMU		60			//pmu_int
#define		INT_Fab0_TO		61			//fabric0_timeout
#define		INT_Fab1_TO		62			//fabric1_timeout
#define		INT_PIN_MUX		63			//From Pin Mux (ND_RDY line)

/* public methord */
void	INT_Enable(BU_U32 int_num, BU_U32 irq_fiq, BU_U32 prio);
void	INT_Disable(BU_U32 int_num );
void	ISR_Connect(BU_U32 int_num, void (*m_func)(void *), void *m_data);

#endif /* _BU_INTERRUPT_H_ */

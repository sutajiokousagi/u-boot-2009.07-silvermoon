/*
 *  U-Boot command for frequency change support
 *
 *  Copyright (C) 2008, 2009 Marvell International Ltd.
 *  All Rights Reserved
 *  Ning Jiang <ning.jiang@marvell.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include "pmua.h"
#include "pmuc.h"
#include "pmud.h"
#include "pmum.h"
#include "predefines.h"

#define CONFIG_AP_ALONE
#define CONFIG_MIPS
#define CONFIG_OPTEST
#define CONFIG_CPUID
#define CONFIG_READ_WRITE
#define CONFIG_SETVOL
#define CONFIG_WFI

#define u32_t	unsigned int
#define u16_t	unsigned short
#define u8_t	unsigned char

#define reg_read(x) (*(volatile u32_t *)(x))
#define reg_write(x,y) ((*(volatile u32_t *)(x)) = y )

void freq_init_sram(u32 sram_code_addr);
void freq_chg_seq(u32 sram_code_addr, u32 sram_stack_addr, u32 op);

void hibernate_init_sram(u32 sram_code_addr);
void hibernate_seq(u32 sram_code_addr, u32 sram_stack_addr, u32 op);

#if (defined(CONFIG_TAVOREVB) || defined(CONFIG_TTC_DKB))
static void PMUcore2_fc_seq (u32_t cp_pdiv, u32_t cp_bdiv, u32_t cp_mdiv, u32_t cp_xpdiv,
		u32_t d_div, u32_t a_div)
{
	volatile u32_t cc_reg,temp,fccr;
	volatile u32_t rdata_core1,rdata_core2,pll_da,pll_ap,pll_cp;
	volatile u32_t temp_ap,temp_cp,temp_a,temp_d;

	cc_reg = 0;

	/* Change the AP freq clk while CP is at reset */
	temp = *(volatile u32_t *)0xd4282888;
	temp |= 0x9;
	*(volatile u32_t *)0xd4282888 = temp;

	/* clear MOH_RD_ST_CLEAR so that reading PMUA_DM_CC_MOH will set MOH_RD_STATUS */
	reg_write(PMUA_CC_MOH, reg_read(PMUA_CC_MOH) & ~0x80000000);

	/* read dummy before sending the command */
	rdata_core2 = reg_read(PMUA_DM_CC_MOH);
	while (rdata_core2 & PMUA_DM_CC_MOH_SEA_RD_STATUS)
		rdata_core2 = reg_read(PMUA_DM_CC_MOH);

	rdata_core1 = reg_read(PMUA_DM_CC_SEA);

	if (cp_pdiv == 26)
		reg_write(PMUM_FCCR, 0x61800000);
	else
		reg_write(PMUM_FCCR, 0x20800000);

	fccr = reg_read(PMUM_FCCR);

	temp = (fccr & PMUM_FCCR_SEAGCLKSEL_MSK) >> PMUM_FCCR_SEAGCLKSEL_BASE;
	if (temp == 0x0)
		pll_cp = 312;
	else if (temp == 0x1)
		pll_cp = 624;
	else if (temp == 0x3)
		pll_cp = 26;

	temp = (fccr & PMUM_FCCR_MOHCLKSEL_MSK) >> PMUM_FCCR_MOHCLKSEL_BASE;
	if (temp == 0x0)
		pll_ap = 312;
	else if (temp == 0x1)
		pll_ap = 624;
	else if (temp == 0x3)
		pll_ap = 26;

	temp = (fccr & PMUM_FCCR_AXICLKSEL_MSK) >> PMUM_FCCR_AXICLKSEL_BASE;
	if ( temp == 0x0 )
		pll_da = 312;
	else if (temp == 0x1)
		pll_da = 624;
	else if (temp == 0x3)
		pll_da = 26;

	/*pclk divider */
	if ( cp_pdiv != 0 )
		cc_reg |= (((pll_ap/cp_pdiv)-1) << PMUA_CC_MOH_CORE_CLK_DIV_BASE);

	/* pdclock divider */
	if ( cp_bdiv != 0 )
		cc_reg |= ((((pll_ap)/cp_bdiv)-1)<< PMUA_CC_MOH_BIU_CLK_DIV_BASE);

	/* xp clock divider */
	if ( cp_xpdiv != 0 )
		cc_reg |= ((((pll_ap)/cp_xpdiv)-1)<< PMUA_CC_MOH_XP_CLK_DIV_BASE);

	/* bus clock divider */
	if ( cp_mdiv != 0 )
		cc_reg |= ((((pll_ap)/cp_mdiv)-1)<< PMUA_CC_MOH_BUS_MC_CLK_DIV_BASE);

	/* D clock divider */
	if ( d_div != 0 )
		cc_reg |= (((pll_da/d_div)-1)<< PMUA_CC_MOH_DDR_CLK_DIV_BASE);

	/* A clock divider */
	if ( a_div != 0 )
		cc_reg |= (((pll_da/a_div)-1)<< PMUA_CC_MOH_BUS_CLK_DIV_BASE);

	/* Async 2 Check : pdclock2 - DDRClk */
	temp_ap = ((((cc_reg & PMUA_CC_MOH_BUS_MC_CLK_DIV_MSK)>>PMUA_CC_MOH_BUS_MC_CLK_DIV_BASE)+1));
	temp_cp = (((cc_reg & PMUA_CC_MOH_DDR_CLK_DIV_MSK) >> PMUA_CC_MOH_DDR_CLK_DIV_BASE)+1) ;
	if ( (unsigned int)(pll_da/(temp_cp*2)) != (unsigned int)(pll_ap/temp_ap) )
		cc_reg |= PMUA_CC_MOH_ASYNC2;
	else
		cc_reg &= ~PMUA_CC_MOH_ASYNC2;

	/* Async 5 Check : baclk2-Aclk */
	temp_ap = ((((cc_reg & PMUA_CC_MOH_BIU_CLK_DIV_MSK)>>PMUA_CC_MOH_BIU_CLK_DIV_BASE)+1));
	temp_cp = ((cc_reg & PMUA_CC_MOH_BUS_CLK_DIV_MSK) >> PMUA_CC_MOH_BUS_CLK_DIV_BASE)+1 ;
	if ( (unsigned int)(pll_da/temp_cp) != (unsigned int)(pll_ap/temp_ap) )
		cc_reg |= PMUA_CC_MOH_ASYNC5;
	else
		cc_reg &= ~PMUA_CC_MOH_ASYNC5;

	/* async 3 Aclk - DDR Clock */
	temp_a = (((cc_reg & PMUA_CC_MOH_BUS_CLK_DIV_MSK)>>PMUA_CC_MOH_BUS_CLK_DIV_BASE)+1) ;
	temp_d = (((cc_reg & PMUA_CC_MOH_DDR_CLK_DIV_MSK)>>PMUA_CC_MOH_DDR_CLK_DIV_BASE)+1);
	if ( (temp_d*2) != temp_a ) {
		cc_reg |= PMUA_CC_MOH_ASYNC3;
		cc_reg |= PMUA_CC_MOH_ASYNC3_1;
	} else {
		cc_reg &= (~PMUA_CC_MOH_ASYNC3_1);
		cc_reg &= (~PMUA_CC_MOH_ASYNC3);
	}

	/* Async 1 Check : pdclock1 - DDRClk */
	temp_ap = ((((rdata_core1 & PMUA_CC_SEA_BUS_MC_CLK_DIV_MSK)>>PMUA_CC_SEA_BUS_MC_CLK_DIV_BASE)+1));
	temp_cp = (((cc_reg & PMUA_CC_MOH_DDR_CLK_DIV_MSK)>>PMUA_CC_MOH_DDR_CLK_DIV_BASE)+1);
	if ( (unsigned int)(pll_da/(temp_cp*2)) != (unsigned int)(pll_ap/temp_ap) )
		cc_reg |= PMUA_CC_MOH_ASYNC1;
	else
		cc_reg &= ~PMUA_CC_MOH_ASYNC1;

	/* Async 4 Check : baclk1-Aclk */
	temp_ap = ((((rdata_core1 & PMUA_CC_SEA_BIU_CLK_DIV_MSK)>> PMUA_CC_SEA_BIU_CLK_DIV_BASE)+1));
	temp_cp = ((cc_reg & PMUA_CC_SEA_BUS_CLK_DIV_MSK) >> PMUA_CC_SEA_BUS_CLK_DIV_BASE)+1 ;
	if ( (unsigned int)(pll_da/temp_cp) != (unsigned int)(pll_ap/temp_ap) )
		cc_reg |= PMUA_CC_MOH_ASYNC4;
	else
		cc_reg &= ~PMUA_CC_MOH_ASYNC4;
#ifdef CONFIG_AP_ALONE
	cc_reg |= (PMUA_CC_MOH_ASYNC4|PMUA_CC_MOH_ASYNC1|PMUA_CC_MOH_ASYNC3|
			PMUA_CC_MOH_ASYNC3_1|PMUA_CC_MOH_ASYNC5|PMUA_CC_MOH_ASYNC2);
#endif
	reg_write(PMUA_CC_MOH,cc_reg);
}

static void core2freqchgcmd(u32_t pclk,u32_t dclk,u32_t aclk)
{
	volatile u32_t freqchg,coremsk;

	coremsk = reg_read(PMUA_MOH_IMR);
	coremsk |= (PMUA_MOH_IMR_MOH_FC_INTR_MASK);
	reg_write(PMUA_MOH_IMR,coremsk);

	freqchg = reg_read(PMUA_CC_MOH);
	freqchg &= ~(PMUA_CC_MOH_MOH_ALLOW_SPD_CHG |
			PMUA_CC_MOH_BUS_FREQ_CHG_REQ |
			PMUA_CC_MOH_DDR_FREQ_CHG_REQ |
			PMUA_CC_MOH_MOH_FREQ_CHG_REQ);

	if ( pclk || dclk || aclk ) {
		freqchg |= (PMUA_CC_MOH_MOH_ALLOW_SPD_CHG);
		reg_write(PMUA_CC_SEA,reg_read(PMUA_CC_SEA)|
				PMUA_CC_SEA_SEA_ALLOW_SPD_CHG);
	}

	if ( aclk )
		freqchg |= (PMUA_CC_SEA_BUS_FREQ_CHG_REQ);
	if ( dclk )
		freqchg |= (PMUA_CC_SEA_DDR_FREQ_CHG_REQ);
	if ( pclk )
		freqchg |= (PMUA_CC_MOH_MOH_FREQ_CHG_REQ);

	reg_write(PMUA_CC_MOH,freqchg);

	/* Chek 4 the cmd 2 go thru */
	while (!(PMUA_MOH_ISR_MOH_FC_ISR & reg_read(PMUA_MOH_ISR)))
		;

	/* Clear the PMU ISR */
	reg_write(PMUA_MOH_ISR,0x0);

	/* write 1 to MOH_RD_ST_CLEAR to clear MOH_RD_STATUS */
	freqchg |= PMUA_CC_MOH_MOH_RD_ST_CLEAR;

	/* Clear the bits */
	freqchg &= ~(	PMUA_CC_MOH_MOH_ALLOW_SPD_CHG |
			PMUA_CC_MOH_BUS_FREQ_CHG_REQ |
			PMUA_CC_MOH_DDR_FREQ_CHG_REQ |
			PMUA_CC_MOH_MOH_FREQ_CHG_REQ);

	/* clear the cmds bit */
	reg_write(PMUA_CC_MOH,freqchg);
}

int do_op(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	ulong op_num;

	if (argc != 2)
		return -1;

	op_num = simple_strtoul(argv[1], NULL, 0);
	if (op_num > 4)
		return -1;

	switch (op_num) {
		case 0:
			PMUcore2_fc_seq(26,26,26,26,13*2,26);
			core2freqchgcmd(1,1,1);
			printf("CPU at 26 MHz DDR at 26 MHz AXI at 26 MHz\r\n");
			break;
		case 1:
			PMUcore2_fc_seq(78,78,78,78,78*2,78);
			core2freqchgcmd(1,1,1);
			printf("CPU at 78 MHz DDR at 78 MHz AXI at 78 MHz\r\n");
			break;
		case 2:
			PMUcore2_fc_seq(156,104,104,104,104*2,104);
			core2freqchgcmd(1,1,1);
			printf("CPU at 156 MHz DDR at 104 MHz AXI at 104 MHz\r\n");
			break;
		case 3:
			PMUcore2_fc_seq(312,156,156,156,156*2,156);
			core2freqchgcmd(1,1,1);
			printf("CPU at 312 MHz DDR at 156 MHz AXI at 156 MHz\r\n");
			break;
		case 4:
			PMUcore2_fc_seq(624,156,156,312,156*2,156);
			core2freqchgcmd(1,1,1);
			printf("CPU at 624 MHz DDR at 156 MHz AXI at 156 MHz\r\n");
			break;
		default :
			break;
	}
	return 0;
}
#elif (defined(CONFIG_ASPENITE) || defined(CONFIG_ZYLONITE2))
static inline void ___delay(unsigned long loops)
{
	__asm__ __volatile__ ("1:\n" "subs %0, %1, #1\n"
			"bne 1b":"=r" (loops):"0"(loops));
}

static int pll2_on(int pclk)
{
	// Disable PLL2
	reg_write(PMUM_PLL2CR, reg_read(PMUM_PLL2CR) | 0x00000200);
	reg_write(PMUM_PLL2CR, reg_read(PMUM_PLL2CR) & 0xFFFFFEFF);

	if (pclk == 800) {
		// Set up PLL2 freq based on 26MHz input clock
		reg_write(PMUM_PLL2CR, (reg_read(PMUM_PLL2CR) & 0xFF0003FF) | 0x00096C00);
	}

	// Enable KVCO 0b0011 (700~900MHz), VCO_VRNG=0b010,
	// post dividers SE VCO_DIV to 0b00 (div 1) and DIFF to 0b00 (div 1)
	reg_write(0xd4050060, 0x90020364);

	// Enable Differential Clock Output
	reg_write(0xd4050064, reg_read(0xd4050064) | 0x00000040);

	// Enable PLL2
	reg_write(PMUM_PLL2CR, reg_read(PMUM_PLL2CR) | 0x00000300);

	return pclk;
}

#define PMUA_CC_MOH_BUS_2_CLK_DIV_BASE 18

void PMUcore2_fc_seq(uint32_t pclk, uint32_t dclk, uint32_t xpclk,
	uint32_t baclk, uint32_t aclk, uint32_t aclk2)
{
	uint32_t cc_reg = 0, temp, fccr, pll_ap = 0, pll_axi = 0;

	if (pclk == 156)
		reg_write(PMUM_FCCR, 0x0000088e);
	else if (pclk == 312)
		reg_write(PMUM_FCCR, 0x0000088e);
	else if (pclk == 624)
		reg_write(PMUM_FCCR, 0x2000088e);
	else if (pclk == 800)
		reg_write(PMUM_FCCR, 0x4000088e);

	fccr = reg_read(PMUM_FCCR);

	temp = (fccr & PMUM_FCCR_MOHCLKSEL_MSK) >> PMUM_FCCR_MOHCLKSEL_BASE;
	if (temp == 0x0)
		pll_ap = 312;
	else if (temp == 0x1)
		pll_ap = 624;
	else if (temp == 0x2)
		pll_ap = pll2_on(pclk);

	temp = (fccr & PMUM_FCCR_AXICLKSEL_MSK) >> PMUM_FCCR_AXICLKSEL_BASE;
	if (temp == 0x0)
		pll_axi = 312;
	else if (temp == 0x1)
		pll_axi = 624;
	else if (temp == 0x2)
		pll_axi = pll2_on(pclk);

	/* pclk divider */
	if (pclk != 0)
		cc_reg |= (((pll_ap / pclk) - 1)
				<< PMUA_CC_MOH_CORE_CLK_DIV_BASE);

	/* D clock divider */
	if (dclk != 0)
		cc_reg |= (((pll_ap / dclk) - 1)
				<< PMUA_CC_MOH_DDR_CLK_DIV_BASE);

	/* xp clock divider */
	if (xpclk != 0)
		cc_reg |= (((pclk / xpclk) - 1)
				<< PMUA_CC_MOH_XP_CLK_DIV_BASE);

	/* bus interface clock divider */
	if (baclk != 0)
		cc_reg |= (((pclk / baclk) - 1)
				<< PMUA_CC_MOH_BIU_CLK_DIV_BASE);

	/* AXI clock clock divider : fabric clock, at present, AXI1 suppose to 156MHz */
	if (aclk != 0)
		cc_reg |= (((pll_axi / 156) - 1)
				<< PMUA_CC_MOH_BUS_CLK_DIV_BASE);

	/* AXI2 clock */
	if (aclk2 != 0)
		cc_reg |= (((pll_axi / aclk2 ) - 1)
				<< PMUA_CC_MOH_BUS_2_CLK_DIV_BASE);

	/* always set BUS_MC_CLK for S0 sillicon, do not set for R0 sillicon */
	cc_reg |= 0x7 << PMUA_CC_MOH_BUS_MC_CLK_DIV_BASE;

	/* The ASYNC3_1 (21) and ASYNC5 (23) bits are supposed to be set to 1 */
	cc_reg |= ((1 << 21) | (1 << 22) | (1 << 23));

	reg_write(PMUA_CC_MOH, cc_reg);
}

void core2freqchgcmd(int pclk, int dclk, int aclk)
{
	unsigned long freqchg,coremsk;

	coremsk = reg_read(PMUA_MOH_IMR);
	coremsk |= (PMUA_MOH_IMR_MOH_FC_INTR_MASK);
	reg_write(PMUA_MOH_IMR, coremsk);

	freqchg = reg_read(PMUA_CC_MOH);

	freqchg &= ~(PMUA_CC_MOH_MOH_ALLOW_SPD_CHG |
			PMUA_CC_MOH_BUS_FREQ_CHG_REQ |
			PMUA_CC_MOH_DDR_FREQ_CHG_REQ |
			PMUA_CC_MOH_MOH_FREQ_CHG_REQ);

	reg_write(PMUA_CC_SEA, reg_read(PMUA_CC_SEA) | PMUA_CC_SEA_SEA_ALLOW_SPD_CHG);

	if ( pclk || dclk || aclk )
		freqchg |= (PMUA_CC_MOH_MOH_ALLOW_SPD_CHG);
	if ( aclk )
		freqchg |= (PMUA_CC_SEA_BUS_FREQ_CHG_REQ);
	if ( dclk )
		freqchg |= (PMUA_CC_SEA_DDR_FREQ_CHG_REQ);
	if ( pclk )
		freqchg |= (PMUA_CC_MOH_MOH_FREQ_CHG_REQ);

	reg_write(PMUA_CC_MOH, freqchg);

	/* Check 4 the cmd 2 go thru */
	while (!(PMUA_MOH_ISR_MOH_FC_ISR & reg_read(PMUA_MOH_ISR)))
		;

	/* Clear the PMU ISR */
	reg_write(PMUA_MOH_ISR, 0x0);

	/* Check status register to make sure frequency has been changed */
	while ((reg_read(PMUA_DM_CC_MOH) & 0x000fffff) != (freqchg & 0x000fffff))
		;

	freqchg |= PMUA_CC_MOH_MOH_RD_ST_CLEAR;

	/* Clear the bits */
	freqchg &= ~(	PMUA_CC_MOH_MOH_ALLOW_SPD_CHG |
			PMUA_CC_MOH_BUS_FREQ_CHG_REQ |
			PMUA_CC_MOH_DDR_FREQ_CHG_REQ |
			PMUA_CC_MOH_MOH_FREQ_CHG_REQ);

	/* clear the cmds bit */
	reg_write(PMUA_CC_MOH, freqchg);
}

static void switch_op(int op)
{
	reg_write(0xd42A0030, reg_read(0xd42A0030) | 0x00000001);
	reg_write(0xd4282C08, reg_read(0xd4282C08) | 0x00000010);

	freq_init_sram(0xd1020000);
	freq_chg_seq(0xd1020000, 0xd1022000, 0);
}

#if 0
static void switch_op(int op)
{
	/* write 0x88.. to PMUA_CC_SEA */
	reg_write(0xd4282800, reg_read(0xd4282800) | 0x88000000);
	reg_write(0xd4050024, 0xffffffff);
	reg_write(0xd4051024, 0xffffffff);

	switch (op) {
	case 0:
		PMUcore2_fc_seq(156,156*2,156,156,156,156);
		core2freqchgcmd(1,1,1);
		printf("op 0 (156MHz)\n");
		break;
	case 1:
		PMUcore2_fc_seq(312,156*2,156,156,156,156);
		core2freqchgcmd(1,1,1);
		printf("op 1 (312MHz)\n");
		break;
	case 2:
		PMUcore2_fc_seq(624,312*2,312,156,156,156);
		core2freqchgcmd(1,1,1);
		printf("op 2 (624MHz)\n");
		break;
	case 3:
		PMUcore2_fc_seq(800,400*2,400,200,156,156);
		core2freqchgcmd(1,1,1);
		printf("op 3 (800MHz)\n");
		break;
	default:
		break;
	}
}

static void switch_op(int op)
{
	reg_write(0xd4282800, reg_read(0xd4282800) | 0x88000000);
	reg_write(0xd4051024, 0xFFFFFFFF);

	if (op == 0) {
		reg_write(0xd4050008, 0x0000888e);
		reg_write(0xd4282898, reg_read(0xd4282898) | 0x0000003A);
		reg_write(0xd4282804, (reg_read(0xd4282804) & 0xFFF00000) | 0x00048039);
		reg_write(0xd4282804, (reg_read(0xd4282804) & 0x0FFFFFFF) | 0xF0000000);
		reg_write(0xd4282804, (reg_read(0xd4282804) & 0xF0FFFFFF) | 0x0F000000);
	}

	if (op == 2) {
		reg_write(0xd4050008, 0x2000888E);
		reg_write(0xd4282898, reg_read(0xd4282898) | 0x0000003A);
		reg_write(0xd4282804, (reg_read(0xd4282804) & 0xFFF00000) | 0x000482F8);
		reg_write(0xd4282804, (reg_read(0xd4282804) & 0x0FFFFFFF) | 0xF0000000);
		reg_write(0xd4282804, (reg_read(0xd4282804) & 0xF0FFFFFF) | 0x0F000000);
	}

	if (op == 3) {
		reg_write(0xd4050008, 0x2000888E);
		reg_write(0xd4282898, reg_read(0xd4282898) | 0x0000003A);
		reg_write(0xd4282804, (reg_read(0xd4282804) & 0xFFF00000) | 0x000482F8);
		reg_write(0xd4282804, (reg_read(0xd4282804) & 0x0FFFFFFF) | 0xB0000000);
		reg_write(0xd4282804, (reg_read(0xd4282804) & 0xF0FFFFFF) | 0x0B000000);

		reg_write(0xd4050034, reg_read(0xd4050034) | 0x00000200);
		reg_write(0xd4050034, reg_read(0xd4050034) & 0xFFFFFEFF);
		reg_write(0xd4050034, (reg_read(0xd4050034) & 0xFF0003FF) | 0x00096C00);
		reg_write(0xd4050060, 0x90020364);
		reg_write(0xd4050064, reg_read(0xd4050064) | 0x00000040);
		reg_write(0xd4050034, reg_read(0xd4050034) | 0x00000300);

		reg_write(0xd4050008, 0x4000888E);
		reg_write(0xd4282898, reg_read(0xd4282898) | 0x0000003A);
		reg_write(0xd4282804, (reg_read(0xd4282804) & 0xFFF00000) | 0x000482F8);
		reg_write(0xd4282804, (reg_read(0xd4282804) & 0x0FFFFFFF) | 0xB0000000);
		reg_write(0xd4282804, (reg_read(0xd4282804) & 0xF0FFFFFF) | 0x0B000000);
	}
}
#endif

int do_op(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int op;

	if (argc != 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	op = simple_strtoul(argv[1], NULL, 0);
	switch_op(op);

	return 0;
}
#else
int do_op(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]) { return 0; }
#endif

U_BOOT_CMD(
		op,	6,	1,	do_op,
		"op	- operating change\n",
		"op_num - set system operating point, opnum between 1 and 5\n"
	  );

#ifdef CONFIG_MIPS

static unsigned long loops_per_sec;

#define READ_TIMER \
		(*(volatile ulong *)(CONFIG_SYS_TIMERBASE+0xa4) = 0x1, \
		*(volatile ulong *)(CONFIG_SYS_TIMERBASE+0xa4))

static inline void __delay(unsigned long loops)
{
	__asm__ __volatile__ ("1:\n" "subs %0, %1, #1\n"
			"bne 1b":"=r" (loops):"0"(loops));
}

int do_calibrate_delay(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	ulong oldtimer, timer, left;

	loops_per_sec = 1;
	printf("Calibrating delay loop.. ");
	while (loops_per_sec <<= 1) {
		oldtimer = READ_TIMER;
		oldtimer = READ_TIMER;
		__delay(loops_per_sec);
		timer = READ_TIMER;
		timer = READ_TIMER;
		timer = timer - oldtimer;
		if (timer >= CONFIG_SYS_HZ) {
			left = loops_per_sec % timer;
			loops_per_sec = loops_per_sec / timer * CONFIG_SYS_HZ;
			while (left > 100000) {
				loops_per_sec += CONFIG_SYS_HZ / (timer / left);
				left = timer % left;
			}
			printf("ok - %lu.%02lu BogoMips\n",
					loops_per_sec/500000,
					(loops_per_sec/5000) % 100);
			return 0;
		}
	}
	printf("failed\n");
	return -1;
}

U_BOOT_CMD(
		mips,	6,	1,	do_calibrate_delay,
		"mips	- calculating BogoMips\n",
		" - calculating BogoMips\n"
	  );
#if 0
int do_read_timer(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	unsigned int value;
	value = READ_TIMER;
	value = READ_TIMER;
	printf("timer %u (%x)\n", value, value);
	return 0;
}

U_BOOT_CMD(
		timer,	6,	1,	do_read_timer,
		"timer	- read timer value\n",
		" - read timer value\n"
	  );
#endif
#endif

#ifdef CONFIG_OPTEST
#if (defined(CONFIG_TAVOREVB) || defined(CONFIG_TTC_DKB))
static void do_single_optest(unsigned int count)
{
	unsigned int start, end, temp;

	temp = count;
	printf("count: %u\n", count);
	start = READ_TIMER;
	start = READ_TIMER;
	printf("start time: 0x%x\n", start);

	while (temp-- > 0) {
		/* 0->1 0->2 0->3 0->4 */
		PMUcore2_fc_seq(26,26,26,26,13*2,26);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(78,78,78,78,78*2,78);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(26,26,26,26,13*2,26);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(156,104,104,104,104*2,104);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(26,26,26,26,13*2,26);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(312,156,156,156,156*2,156);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(26,26,26,26,13*2,26);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(624,156,156,312,156*2,156);
		core2freqchgcmd(1,1,1);

		/* 1->0 1->2 1->3 1->4 */
		PMUcore2_fc_seq(78,78,78,78,78*2,78);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(26,26,26,26,13*2,26);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(78,78,78,78,78*2,78);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(156,104,104,104,104*2,104);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(78,78,78,78,78*2,78);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(312,156,156,156,156*2,156);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(78,78,78,78,78*2,78);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(624,156,156,312,156*2,156);
		core2freqchgcmd(1,1,1);

		/* 2->0 2->1 2->3 2->4 */
		PMUcore2_fc_seq(156,104,104,104,104*2,104);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(26,26,26,26,13*2,26);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(156,104,104,104,104*2,104);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(78,78,78,78,78*2,78);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(156,104,104,104,104*2,104);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(312,156,156,156,156*2,156);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(156,104,104,104,104*2,104);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(624,156,156,312,156*2,156);
		core2freqchgcmd(1,1,1);

		/* 3->0 3->1 3->2 3->4 */
		PMUcore2_fc_seq(312,156,156,156,156*2,156);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(26,26,26,26,13*2,26);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(312,156,156,156,156*2,156);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(78,78,78,78,78*2,78);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(312,156,156,156,156*2,156);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(156,104,104,104,104*2,104);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(312,156,156,156,156*2,156);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(624,156,156,312,156*2,156);
		core2freqchgcmd(1,1,1);

		/* 4->0 4->1 4->2 4->3 */
		PMUcore2_fc_seq(624,156,156,312,156*2,156);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(26,26,26,26,13*2,26);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(624,156,156,312,156*2,156);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(78,78,78,78,78*2,78);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(624,156,156,312,156*2,156);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(156,104,104,104,104*2,104);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(624,156,156,312,156*2,156);
		core2freqchgcmd(1,1,1);

		PMUcore2_fc_seq(312,156,156,156,156*2,156);
		core2freqchgcmd(1,1,1);
	}

	end = READ_TIMER;
	end = READ_TIMER;
	printf("end time: 0x%x\n", end);
	printf("duration: %d sec\n", (end-start)/3250000);
	printf("ticks/op: %d\n", (end-start)/count/5/8);
}

int do_optest(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	ulong count1, count2, temp, loops_per_round;

	if (argc != 2 && argc != 3) {
		printf("usage: optest num\n");
		return 0;
	}

	temp = simple_strtoul(argv[1], NULL, 0);
	loops_per_round = 10000;
	if (argc == 3)
		loops_per_round = simple_strtoul(argv[2], NULL, 0);
	/* loops_per_round needs to be less than 500000, or else timer will loop around */
	loops_per_round %= 500000;
	count1 = temp / loops_per_round;
	count2 = temp % loops_per_round;

	while (count1-- > 0)
		do_single_optest(loops_per_round);

	if (count2 > 0)
		do_single_optest(count2);

	printf("\noptest done!\n\n");
	return 0;
}
#elif (defined(CONFIG_ASPENITE) || defined(CONFIG_ZYLONITE2))
static void do_single_optest(unsigned int count)
{
	unsigned int start, end, temp;

	temp = count;
	printf("count: %u\n", count);
	start = READ_TIMER;
	start = READ_TIMER;
	printf("start time: 0x%x\n", start);

	while (temp-- > 0) {
		switch_op(2);
		switch_op(1);
	}

	end = READ_TIMER;
	end = READ_TIMER;
	printf("end time: 0x%x\n", end);
	printf("duration: %d sec\n", (end-start)/3250000);
	printf("ticks/op: %d\n", (end-start)/count/6);
}

int do_optest(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	ulong count1, count2, temp, loops_per_round;

	if (argc != 2 && argc != 3) {
		printf("usage: optest num\n");
		return 0;
	}

	temp = simple_strtoul(argv[1], NULL, 0);
	loops_per_round = 10000;
	if (argc == 3)
		loops_per_round = simple_strtoul(argv[2], NULL, 0);
	/* loops_per_round needs to be less than 500000, or else timer will loop around */
	loops_per_round %= 500000;
	count1 = temp / loops_per_round;
	count2 = temp % loops_per_round;

	while (count1-- > 0)
		do_single_optest(loops_per_round);

	if (count2 > 0)
		do_single_optest(count2);

	printf("\noptest done!\n\n");
	return 0;
}
#else
int do_optest(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]) { return 0; }
#endif

U_BOOT_CMD(
		optest,	6,	1,	do_optest,
		"optest	- op strength test\n",
		" - op strength test\n"
	  );
#endif

#ifdef CONFIG_READ_WRITE
int do_read(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	ulong addr;
	ulong value, length, start, end;

	if (argc < 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	addr = simple_strtoul(argv[1], NULL, 0);
	if (argc == 3)
		length = simple_strtoul(argv[2], NULL, 0);
	else
		length = 4;
	start = addr;
	end = addr + length;
	for (;start < end;start += 4) {
		value = *(unsigned int*)start;
		printf("0x%08x: 0x%08x\n", (unsigned int)start, (unsigned int)value);
	}

	return 0;
}

U_BOOT_CMD(
		read,	6,	1,	do_read,
		"read	- read address\n",
		" - read address\n"
	  );

int do_write(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
        ulong index, value;

        if (argc != 3)
                return 0;

        index = simple_strtoul(argv[1], NULL, 0);
        value = simple_strtoul(argv[2], NULL, 0);
	reg_write(index, value);
	return 0;
}

U_BOOT_CMD(
		write,	6,	1,	do_write,
		"write	- write address value\n",
		" - write address value\n"
	  );
#endif

#ifdef CONFIG_CPUID

#define __stringify_1(x)	#x
#define __stringify(x)		__stringify_1(x)

#define read_cpuid(reg)						\
	({							\
	 unsigned int __val;					\
	 asm("mrc	p15, 0, %0, c0, c0, " __stringify(reg)	\
		 : "=r" (__val)					\
		 :						\
		 : "cc");					\
	 __val;							\
	 })

int do_cpuid(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	printf("cpu id:		%x\n", read_cpuid(0));
	printf("cache type:	%x\n", read_cpuid(1));
	printf("chip id:	%x\n", *(volatile unsigned int *)0xd4282c00);
	return 0;
}

U_BOOT_CMD(
		cpuid,	6,	1,	do_cpuid,
		"cpuid	- read cpu id\n",
		" - read cpu id\n"
	  );

int do_setid(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	*(volatile unsigned int *)0xD4282c08 = *(volatile unsigned int *)0xD4282c08 | 0x100;
	return 0;
}

U_BOOT_CMD(
		setid,	6,	1,	do_setid,
		"setid	- set SEL_MRVL_ID bit in MOHAWK_CPU_CONF register\n",
		" - set SEL_MRVL_ID bit in MOHAWK_CPU_CONF register\n"
	  );

int do_unsetid(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	*(volatile unsigned int *)0xD4282c08 = *(volatile unsigned int *)0xD4282c08 & ~0x100;
	return 0;
}

U_BOOT_CMD(
		unsetid,	6,	1,	do_unsetid,
		"unsetid	- unset SEL_MRVL_ID bit in MOHAWK_CPU_CONF register\n",
		" - unset SEL_MRVL_ID bit in MOHAWK_CPU_CONF register\n"
	  );
#endif

#ifdef CONFIG_SETVOL

#if (defined(CONFIG_TAVOREVB) || defined(CONFIG_TTC_DKB))
#define MFP_I2C_SCL	0xd401e1b0	/* GPIO53 */
#define MFP_I2C_SDA	0xd401e1b4	/* GPIO54 */
#endif

#define TWSI0_CLK	0xd401502c
#define TWSI1_CLK	0xd401506c

#define I2C_BASE	0xd4011000
#define PI2C_BASE	0xd4025000

#define IBMR		*(volatile u32_t *)(i2c_base+0x00)
#define IDBR		*(volatile u32_t *)(i2c_base+0x08)
#define ICR		*(volatile u32_t *)(i2c_base+0x10)
#define ISR		*(volatile u32_t *)(i2c_base+0x18)
#define ISAR		*(volatile u32_t *)(i2c_base+0x20)
#define ILCR		*(volatile u32_t *)(i2c_base+0x28)

/*  bus monitor register */
#define	IBMR_SDA	BIT_0  /* reflects the status of SDA pin */
#define	IBMR_SCL	BIT_1  /* reflects the status of SCL pin */

/* data buffer regiter mask */
#define	IDBR_ADDR	0xFF;  /* buffer for I2C bus send/receive data */
#define	IDBR_MODE	BIT_0

/* Control Register */
#define	ICR_START	BIT_0  /* 1:send a Start condition to the I2C when in master mode */
#define	ICR_STOP	BIT_1  /* 1:send a Stop condition after next data byte transferred on I2C bus in master mode */
#define	ICR_ACKNACK	BIT_2  /* Ack/Nack control: 1:Nack, 0:Ack (negative or positive pulse) */
#define	ICR_TB		BIT_3  /* 1:send/receive byte, 0:cleared by I2C unit when done */
#define	ICR_MA		BIT_4  /* 1:I2C sends STOP w/out data permission, 0:ICR bit used only */
#define	ICR_SCLEA	BIT_5  /* I2C clock output: 1:Enabled, 0:Disabled. ICCR configured before ! */
#define	ICR_UIE		BIT_6  /* I2C unit: 1:Enabled, 0:Disabled */
#define	ICR_GCD		BIT_7  /* General Call: 1:Disabled, 0:Enabled */
#define	ICR_ITEIE	BIT_8  /* 1: IDBR Transmit Empty Interrupt Enable */
#define	ICR_DRFIE	BIT_9  /* 1: IDBR Receive Full Interrupt Enable */
#define	ICR_BEIE	BIT_10 /* 1: Bus Error Interrupt Enable */
#define	ICR_SSDIE	BIT_11 /* 1: Slave Stop Detected Interrupt Enable */
#define	ICR_ALDIE	BIT_12 /* 1: Arbitration Loss Detected Interrupt Enable */
#define	ICR_SADIE	BIT_13 /* 1: Slave Address Detected Interrupt Enable */
#define	ICR_UR		BIT_14 /* 1: I2C unit reset */
#define ICR_FM   	BIT_15 /* 1: Fast mode - 400 KBit/sec. operation. Default is 100 KBit/sec */

/* Status Register */
#define	ISR_RWM		BIT_0  /* 1: I2C in master receive = slave transmit mode */
#define	ISR_ACKNACK	BIT_1  /* 1: I2C received/sent a Nack, 0: Ack */
#define	ISR_UB		BIT_2  /* 1: Processor's I2C unit busy */
#define	ISR_IBB		BIT_3  /* 1: I2C bus busy. Processor's I2C unit not involved */
#define	ISR_SSD		BIT_4  /* 1: Slave Stop detected (when in slave mode: receive or transmit) */
#define	ISR_ALD		BIT_5  /* 1: Arbitration Loss Detected */
#define	ISR_ITE		BIT_6  /* 1: Transfer finished on I2C bus. If enabled in ICR, interrupt signaled */
#define	ISR_IRF		BIT_7  /* 1: IDBR received new byte from I2C bus. If ICR, interrupt signaled */
#define	ISR_GCAD	BIT_8  /* 1: I2C unit received a General Call address */
#define	ISR_SAD		BIT_9  /* 1: I2C unit detected a 7-bit address matching the general call or ISAR */
#define	ISR_BED		BIT_10 /* Bit set by unit when a Bus Error detected */

/*  slave address mask */
#define	ISAR_ADDR	0x7F;  /* 7-bit(6:0) add to which I2C unit responses to in slave/receive mode*/

#define ICR_INIT_VALUE	(ICR_UIE|ICR_SCLEA)

#define I2C_NO_STOP	0      /* Don't issue stop bit */
#define I2C_STOP	1      /* Issue stop bit */

/* Processor I2C Device ID */
#define I2C_SLAVE_ID	0x4E   /* 0100_111x, The Phillips spec says it must be a value between 0001_000xB and 1110_111xB */

static u32 i2c_base = I2C_BASE;

int i2c_init(unsigned int dev_id) 
{

#if (defined(CONFIG_TAVOREVB) || defined(CONFIG_TTC_DKB))
	/* setup MFPRs for I2C */
	reg_write(MFP_I2C_SCL, 0xa842);
	reg_write(MFP_I2C_SDA, 0xa842);
#elif (defined(CONFIG_ASPENITE) || defined(CONFIG_ZYLONITE2))
	/* MFPRs for Aspenite is set in board_init() */
#endif

	if (i2c_base == I2C_BASE) {
		/* enable i2c clock */
		reg_write(TWSI0_CLK, 0x3);
	} else if (i2c_base == PI2C_BASE) {
		/* enable pwr i2c clock */
		reg_write(TWSI1_CLK, 0x3);
	}

	ICR = 0;
	/* Setup I2C slave address */ 
	ISAR = dev_id;
	ICR = ICR_SCLEA;
	ICR |= ICR_UIE;

	return 0;
}

int i2c_rx_full(int timeout) 
{
	unsigned int temp;
	while (timeout--) {
		temp = ISR;
		if ((temp & ISR_IRF) == ISR_IRF) {
			ISR = temp | ISR_IRF;
			return 0;
		}
		 udelay(200);
	}
	printf("i2c_rx_full timeout\n");
	return -1;
}

int i2c_tx_empty(int timeout) 
{
	unsigned int temp;

	while (timeout--) {
		temp = ISR;
		if ((temp & ISR_ITE) == ISR_ITE) {
			ISR = temp | ISR_ITE;
			if ((temp & ISR_ALD) == ISR_ALD) {
				ISR |= ISR_ALD;
			}
			return 0;
		}
		udelay(200);
	}
	printf("i2c_tx_empty timeout\n");
	return -1;
}

int __i2c_write(unsigned char slave_addr, unsigned char * bytes_buf,
	   unsigned int bytes_count, int stop) 
{
	unsigned int reg;
	
	IDBR = (slave_addr << 1) & ~IDBR_MODE;
	reg = ICR;
	reg |= (ICR_START | ICR_TB);
	reg &= ~(ICR_STOP | ICR_ALDIE);
	ICR = reg;
	if (i2c_tx_empty(20) == -1) {
		return -1;
	}

	/* Send all the bytes */
	while (bytes_count--) {
		IDBR = (unsigned int) (*bytes_buf++);
		reg = ICR;
		reg &= ~ICR_START;
		reg |= (ICR_ALDIE | ICR_TB);
		if ((bytes_count == 0) && stop)
			reg |= ICR_STOP;
		else
			reg &= ~ICR_STOP;
		ICR = reg;
		if (i2c_tx_empty(250) == -1) {
			return -1;
		}
	}

	/* Clear the STOP bit always */
	ICR &= ~ICR_STOP;
	return 0;
}

int __i2c_read(unsigned char slave_addr, unsigned char * bytes_buf,
		unsigned int bytes_count, int stop) 
{
	unsigned int reg;

	IDBR = (slave_addr << 1) | IDBR_MODE;
	reg = ICR;
	reg |= (ICR_START | ICR_TB);
	reg &= ~(ICR_STOP | ICR_ALDIE);
	ICR = reg;
	if (i2c_tx_empty(20) == -1) {
		return -1;
	}
	while (bytes_count--) {
		reg = ICR;
		reg &= ~ICR_START;
		reg |= ICR_ALDIE | ICR_TB;
		if (bytes_count == 0) {
			reg |= ICR_ACKNACK;
			if (stop)
				reg |= ICR_STOP;
			else
				reg &= ~ICR_STOP;
		} else {
			reg &= ~ICR_ACKNACK;
		}
		ICR = reg;
		if (i2c_rx_full(60) == -1) {
			return -1;
		}
		reg = IDBR & 0xFF;
		*bytes_buf++ = (unsigned char) reg;
	}
	ICR &= ~(ICR_STOP | ICR_ACKNACK);
	return 0;
}

int i2c_writeb(unsigned int slave_addr, unsigned char reg, unsigned char val)
{
	unsigned char buffer[2];
	int status;

	buffer[0] = reg;
	buffer[1] = val;
	status = __i2c_write(slave_addr, buffer, 2, 1);

	return status;
}

int i2c_write_addr(unsigned int slave_addr, unsigned char reg)
{
	unsigned char buffer[1];
	int status;

	buffer[0] = reg;
	status = __i2c_write(slave_addr, buffer, 1, 1);

	return status;
}

int i2c_readb(unsigned int slave_addr, unsigned char reg, unsigned char *pval)
{
	unsigned char buffer[1];
	int status;

	buffer[0] = reg;
	status = __i2c_write(slave_addr, buffer, 1, 1);
	if (!status) {
		status = __i2c_read(slave_addr, buffer, 1, 1);
		*pval = buffer[0];
	}

	return status;
}

int do_i2cbase(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	if (argc != 1) {
		i2c_base = simple_strtoul(argv[1], NULL, 0);
		if (i2c_base != I2C_BASE && i2c_base != PI2C_BASE)
			i2c_base = I2C_BASE;
	}
	printf("i2c_base: 0x%x\n", i2c_base);
	return 0;
}

U_BOOT_CMD(
		i2cbase,	6,	1,	do_i2cbase,
		"i2cbase	- i2cbase i2c_base_addr (I2C: 0xd4011000, PI2C: 0xd4025000)\n",
		" - i2cbase i2c_base_addr (I2C: 0xd4011000, PI2C: 0xd4025000)\n"
	  );

int do_i2cinit(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	i2c_init(0);
	printf("%s i2c initialized\n", (i2c_base == I2C_BASE) ? "normal" : "power");
	return 0;
}

U_BOOT_CMD(
		i2cinit,	6,	1,	do_i2cinit,
		"i2cinit	- initialize i2c bus\n",
		" - initialize i2c bus\n"
	  );

#if CONFIG_TAVOREVB
	#define VBUCK1_CNT(x)	((x < 0) ? -1 :			\
				((x < 1525) ? ((x - 725) / 25)	\
				: -1))
#elif CONFIG_TTC_DKB
	#define VBUCK1_CNT(x)	((x < 0) ? -1 :			\
				((x < 800) ? (x / 25 + 0x20) :	\
				((x < 1525) ? ((x - 800) / 25)	\
				: -1)))
#elif (defined(CONFIG_ASPENITE) || defined(CONFIG_ZYLONITE2))
	#define VBUCK1_CNT(x)	((x < 0) ? -1 :			\
				((x < 1825) ? ((x - 725) / 25)	\
				: -1))
#else
	#define VBUCK1_CNT(x)	(-1)
#endif

static int aspen_eco11[][3] = {
	/* cnt,  in,   out */
	{ 0x00,  725,  379 },
	{ 0x01,  750,  408 },
	{ 0x02,  775,  436 },
	{ 0x03,  800,  464 },
	{ 0x04,  825,  493 },
	{ 0x05,  850,  521 },
	{ 0x06,  875,  550 },
	{ 0x07,  900,  578 },
	{ 0x08,  925,  606 },
	{ 0x09,  950,  635 },
	{ 0x0A,  975,  663 },
	{ 0x0B, 1000,  691 },
	{ 0x0C, 1025,  720 },
	{ 0x0D, 1050,  748 },
	{ 0x0E, 1075,  776 },
	{ 0x0F, 1100,  805 },
	{ 0x10, 1125,  833 },
	{ 0x11, 1150,  861 },
	{ 0x12, 1175,  890 },
	{ 0x13, 1200,  918 },
	{ 0x14, 1225,  947 },
	{ 0x15, 1250,  975 },
	{ 0x16, 1275, 1003 },
	{ 0x17, 1300, 1032 },
	{ 0x18, 1325, 1060 },
	{ 0x19, 1350, 1088 },
	{ 0x1A, 1375, 1117 },
	{ 0x1B, 1400, 1145 },
	{ 0x1C, 1425, 1173 },
	{ 0x1D, 1450, 1202 },
	{ 0x1E, 1475, 1230 },
	{ 0x1F, 1500, 1258 },
	{ 0x20, 1525, 1287 },
	{ 0x21, 1550, 1315 },
	{ 0x22, 1575, 1343 },
	{ 0x23, 1600, 1372 },
	{ 0x24, 1625, 1400 },
	{ 0x25, 1650, 1429 },
	{ 0x26, 1675, 1457 },
	{ 0x27, 1700, 1485 },
	{ 0x28, 1725, 1514 },
	{ 0x29, 1750, 1542 },
	{ 0x2A, 1775, 1570 },
	{ 0x2B, 1800, 1599 },
};

#define ASPEN_ECO11_SIZE (sizeof(aspen_eco11)/sizeof(aspen_eco11[0]))

int set_volt(u32 vol)
{
	int cnt = -1 , i, res = 0;
	static int first_time = 1;
	u32 i2c_base_saved;

	i2c_base_saved = i2c_base;

#if (defined(CONFIG_TAVOREVB) || defined(CONFIG_TTC_DKB))
	i2c_base = I2C_BASE;
	cnt = VBUCK1_CNT(vol);
#elif (defined(CONFIG_ASPENITE) || defined(CONFIG_ZYLONITE2))
	i2c_base = PI2C_BASE;
	for (i = 0; i < ASPEN_ECO11_SIZE; i++)
		if (aspen_eco11[i][1] == vol)
			cnt = aspen_eco11[i][0];

	for (i = 0; i < ASPEN_ECO11_SIZE; i++)
		if (aspen_eco11[i][2] == vol)
			cnt = aspen_eco11[i][0];
#endif
	if (cnt < 0) {
		i2c_base = i2c_base_saved;
		return -1;
	}

	if (first_time) {
		i2c_init(0);			/* init I2C */
		first_time = 0;
	}

#if CONFIG_TAVOREVB
	res = i2c_writeb(0x34, 0x10, 0x07);	/* enable LDO2, BUCK1, BUCK2 */
	res = i2c_writeb(0x34, 0x24, cnt);	/* set BUCK1 voltage in ADTV2 */
	res = i2c_writeb(0x34, 0x20, 0x03);	/* select ADTV2 and let it go ramping */
#elif CONFIG_TTC_DKB
	res = i2c_writeb(0x30, 0x40, 0x0);	/* enable GI2C access to BUCK1_SET */
	res = i2c_writeb(0x30, 0x24, cnt);	/* set BUCK1 voltage */
	res = i2c_writeb(0x30, 0x20, 0x01);	/* let it go ramping */
#elif CONFIG_ASPENITE
	res = i2c_writeb(0x34, 0x00, 0x0);	/* dummy write */
	res = i2c_writeb(0x34, 0x23, cnt);	/* set V3 voltage in ADTV1 */
	res = i2c_writeb(0x34, 0x20, 0x01);	/* select ADTV1 and let it go ramping */
#endif
	i2c_base = i2c_base_saved;
	return res;
}

int do_setvol(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	u32 vol;

	if ((argc < 1) || (argc > 2))
		return -1;

	if (argc == 1) {
		printf( "usage: setvol xxxx\n"
			"for tavorevb and ttd_dkb, xxxx can be 725..1500, step 25\n"
			"for aspenite and zylonite2, xxxx can be\n"
			"493  521  550  578  606  635  663  691  720  748  776\n"
			"805  833  861  890  918  947  975  1003 1032 1060 1088\n"
			"1117 1145 1173 1202 1230 1258 1287 1315 1343 1372 1400\n"
			"1429 1457 1485 1514 1542 1570 1599\n");
		return 0;
	}

	vol = simple_strtoul(argv[1], NULL, 0);
	if (set_volt(vol) < 0)
		return -1;
	printf("voltage change successfully\n");
	return 0;
}

U_BOOT_CMD(
		setvol,	6,	1,	do_setvol,
		"setvol	- set VCC_MAIN (725mV - 1500mV, stepping 25mV)\n",
		" - set VCC_MAIN (725mV - 1500mV, stepping 25mV)\n"
	  );
#endif

#ifdef CONFIG_WFI
int do_wfi(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
       __asm__ __volatile__ ("\n\t\
               mov     r0, #0\n\t\
               mrc     p15, 0, r1, c1, c0, 0   @ read control register\n\t\
               mcr     p15, 0, r0, c7, c10, 4  @ Drain write buffer\n\t\
               bic     r2, r1, #1 << 12\n\t\
               mrs     r3, cpsr                @ Disable FIQs while Icache\n\t\
               orr     ip, r3, #0x00000040     @ is disabled\n\t\
               msr     cpsr_c, ip\n\t\
               mcr     p15, 0, r2, c1, c0, 0   @ Disable I cache\n\t\
               mcr     p15, 0, r0, c7, c0, 4   @ Wait for interrupt\n\t\
               mcr     p15, 0, r1, c1, c0, 0   @ Restore ICache enable\n\t\
               msr     cpsr_c, r3              @ Restore FIQ state\n\t"
       );
       return 0;
}

U_BOOT_CMD(
               wfi,    6,      1,      do_wfi,
               "wfi	- Wait For Interrupt\n",
               " - Wait For Interrupt\n"
         );
#endif

int sanremo_ttc_mode(void)
{
	/* set on_key hold bit */
	i2c_writeb(0x30, 0x0b, 0x80);
	/* enter test mode */
	i2c_write_addr(0x30, 0xfa);
	i2c_write_addr(0x30, 0xfb);
	i2c_write_addr(0x30, 0xff);
	/* switch to ttc mode */
	i2c_writeb(0x30, 0xd0, 0x10);
	i2c_writeb(0x30, 0xb0, 0xad);
	i2c_writeb(0x30, 0xb1, 0x05);
	/* exit test mode */
	i2c_write_addr(0x30, 0xfe);
	return 0;
}

int do_i2c_read(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	unsigned char slave_addr;
	unsigned char reg;
	unsigned char val;

	if (argc != 3)
		return -1;

	slave_addr = simple_strtoul(argv[1], NULL, 0);
	reg = simple_strtoul(argv[2], NULL, 0);

	printf("slave_addr: 0x%02x reg: 0x%02x\n", slave_addr, reg);

	if (!i2c_readb(slave_addr, reg, &val))
		printf("i2c_read: success! return 0x%02x\n", val);
	else
		printf("i2c_read: failed!\n");

	return 0;
}

U_BOOT_CMD(
               i2c_read,    6,      1,      do_i2c_read,
               "i2c_read	- read I2C device (byte)\n",
               " - read I2C\n"
         );

int do_i2c_write(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	unsigned char slave_addr;
	unsigned char reg;
	unsigned char val;

	if (argc != 4)
		return -1;

	slave_addr = simple_strtoul(argv[1], NULL, 0);
	reg = simple_strtoul(argv[2], NULL, 0);
	val = simple_strtoul(argv[3], NULL, 0);

	printf("slave_addr: 0x%02x reg: 0x%02x val: 0x%02x\n", slave_addr, reg, val);
	if (!i2c_writeb(slave_addr, reg, val))
		printf("i2c_write: success!\n");
	else
		printf("i2c_write: failed!\n");

	return 0;
}

U_BOOT_CMD(
               i2c_write,    6,      1,      do_i2c_write,
               "i2c_write	- write I2C device (byte)\n",
               " - write I2C\n"
         );

int do_i2c_write_addr(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	unsigned char slave_addr;
	unsigned char reg;

	if (argc != 3)
		return -1;

	slave_addr = simple_strtoul(argv[1], NULL, 0);
	reg = simple_strtoul(argv[2], NULL, 0);

	printf("slave_addr: 0x%02x reg: 0x%02x\n", slave_addr, reg);
	if (!i2c_write_addr(slave_addr, reg))
		printf("i2c_write_addr: success!\n");
	else
		printf("i2c_write_addr: failed!\n");

	return 0;
}

U_BOOT_CMD(
               i2c_write_addr,    6,      1,      do_i2c_write_addr,
               "i2c_write_addr	- write addr to I2C device (byte)\n",
               " - write I2C\n"
         );

static void hibernate(void)
{
	reg_write(0xd42A0030, reg_read(0xd42A0030) | 0x00000001);
	reg_write(0xd4282C08, reg_read(0xd4282C08) | 0x00000010);

	hibernate_init_sram(0xd1020000);
	hibernate_seq(0xd1020000, 0xd1022000, 0);
}

int do_hibernate(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	if (argc != 1) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	hibernate();

	return 0;
}

U_BOOT_CMD(
		hibernate,	6,	1,	do_hibernate,
		"hibernate	- put system into hibernate mode\n",
		"hibernate - put system into hibernate mode\n"
	  );

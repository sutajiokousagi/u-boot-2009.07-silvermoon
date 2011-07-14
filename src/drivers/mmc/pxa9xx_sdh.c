/*
 * Copyright 2009, Marvell Semiconductor Inc.
 * Kevin Wang
 *
 * Base vaguely on linux kernel driver
 *  linux/drivers/mmc/host/pxa.c - PXA MMCI driver
 *
 *  Copyright (C) 2003 Russell King, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <common.h>
#include <malloc.h>
#include <asm/arch/hardware.h>
#include <mmc.h>

//#define DEBUG
#include "pxa9xx_sdh.h"
#ifdef DEBUG
#define dprintf(fmt, args...) printf(fmt, ##args)
#define GPIO_GPLR(x) __REG((x < 3) ? 0x40E00000 + x * 4:0x40E00100 + (x - 3) * 4)
#define GPIO_GPDR(x) __REG((x < 3) ? 0x40E0000C + x * 4:0x40E0010C + (x - 3) * 4)
#define GPIO_GSDR(x) __REG(0x40E00400 + x * 4)
#define GPIO_GCDR(x) __REG(0x40E00420 + x * 4)
#define GPIO_GPSR(x) __REG((x < 3) ? 0x40E00018 + x * 4:0x40E00118 + (x - 3) * 4)
#define GPIO_GPCR(x) __REG((x < 3) ? 0x40E00024 + x * 4:0x40E00124 + (x - 3) * 4)
#else
#define dprintf(...)
#endif

#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)	(x)
#define unlikely(x)	(x)
#endif

struct sdh_host {
	void *reg_base;
	int clk_rate;
};

inline unsigned int sdh_get_reg(struct mmc *mmc, unsigned int offset) {
	return __REG(((struct sdh_host *)(mmc->priv))->reg_base + offset);
}

inline void sdh_set_reg(struct mmc *mmc, unsigned int offset, unsigned int value) {
	__REG(((struct sdh_host *)(mmc->priv))->reg_base + offset) = value;
}

static void sdh_reset(struct mmc *mmc, unsigned int rst_mask)
{
	unsigned int cntl2 = sdh_get_reg(mmc, MMC4_CNTL2);

	sdh_set_reg(mmc, MMC4_CNTL2, cntl2 | rst_mask);
	while (1) {
		udelay(10);
		cntl2 = sdh_get_reg(mmc, MMC4_CNTL2);
		if (!(cntl2 & rst_mask))
			return;
	}
}

static unsigned int sdh_wait_istat_bit(struct mmc *mmc, unsigned int bit_val)
{
	unsigned int istat = 0;
	unsigned long ticks = get_ticks();

	while (get_ticks() < (ticks + CONFIG_SYS_HZ * 1)) { /*at most 1 second*/
		istat = sdh_get_reg(mmc, MMC4_I_STAT);
		if (istat & ERRINT) {
			dprintf("Error detected in MMC4_I_STAT(0x%X)!\n", istat);
			sdh_reset(mmc, CMDSWRST | DATSWRST);
			return istat;
		}
		if (istat & bit_val) {
			dprintf("!!!! istat 0x%x detected. bit_val=0x%x\n", istat, bit_val);
			sdh_set_reg(mmc, MMC4_I_STAT, bit_val);
			return 0;
		}
	}
	dprintf("Timeout while waiting MMC4_I_STAT<0x%X>!, current stat=0x%x\n",
		bit_val, istat);
	sdh_reset(mmc, CMDSWRST | DATSWRST);
	return -1;
}

int pxasdh_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
	unsigned int stat = 0;
	unsigned int xfrmd = 0;
	unsigned int rsp = 0;
	unsigned long ticks = get_ticks();

	while (get_ticks() < ticks + CONFIG_SYS_HZ) { /*at most 1 second*/
		stat = sdh_get_reg(mmc, MMC4_STATE);
		if ((stat & (CCMDINHBT | DCMDINHBT)) == 0)
			break;
	}
	if (get_ticks() >= ticks + CONFIG_SYS_HZ) {
		dprintf("bus is still in busy, cannot start the command!\n");
		sdh_reset(mmc, CMDSWRST | DATSWRST);
		return TIMEOUT;
	}
	sdh_set_reg(mmc, MMC4_I_STAT, sdh_get_reg(mmc, MMC4_I_STAT));

	/*Set Response Type*/
	switch (cmd->resp_type) {
	case MMC_RSP_NONE:
		break;

	case MMC_RSP_R1: /* r1, r5, r6, r7 */
		rsp = CMD_RSP_48BIT;
		xfrmd |= CRCCHKEN | IDXCHKEN;
		break;

	case MMC_RSP_R2: /* r2 */
		rsp = CMD_RSP_136BIT;
		xfrmd |= CRCCHKEN;
		break;

	case MMC_RSP_R3: /* r3, r4*/
		rsp = CMD_RSP_48BIT;
		break;

	case MMC_RSP_R1b: /* r1b */
		rsp = CMD_RSP_48BITB;
		xfrmd |= CRCCHKEN | IDXCHKEN;
		break;

	default:
		BUG_ON(0);
		break;
	}

	/*Set Transfer mode regarding to data flag*/
	if (data)
	{
		xfrmd |= DPSEL | BLKCNTEN;
		if (data->blocks > 1)
			xfrmd |= MS_BLKSEL;

		if (data->flags == MMC_DATA_READ)
			xfrmd |= DXFRDIR;
		else
			xfrmd &= ~DXFRDIR;

		//if (data->blocks > 1)
		//	xfrmd |= AUTOCMD12;

		sdh_set_reg(mmc, MMC4_BLK_CNTL,
			(data->blocks << BLK_CNT_OFFSET) | (0x7 << DMA_BUFSZ_OFFSET)
			| (data->blocksize & XFR_BLKSZ_MASK));
	}

	sdh_set_reg(mmc, MMC4_ARG, cmd->cmdarg);
	xfrmd |= cmd->cmdidx << CMD_IDX_OFFSET | rsp << RES_TYPE_OFFSET;
	dprintf("Sending CMD%d(%s): xfrmd=0x%x, arg=0x%x\n", cmd->cmdidx,
		mmc_get_cmdname(cmd->cmdidx), xfrmd, sdh_get_reg(mmc, MMC4_ARG));
	sdh_set_reg(mmc, MMC4_CMD_XFRMD, xfrmd);

	if (sdh_wait_istat_bit(mmc, CMDCOMP)) {
		if (sdh_get_reg(mmc, MMC4_I_STAT) && CTO)
			return TIMEOUT;
		else
			return -1;
	}

	if (cmd->resp_type == MMC_RSP_R2) {
		cmd->response[0] = sdh_get_reg(mmc, MMC4_RESP3) << 8 |
				sdh_get_reg(mmc, MMC4_RESP2) >> 24;
		cmd->response[1] = sdh_get_reg(mmc, MMC4_RESP2) << 8 |
				sdh_get_reg(mmc, MMC4_RESP1) >> 24;
		cmd->response[2] = sdh_get_reg(mmc, MMC4_RESP1) << 8 |
				sdh_get_reg(mmc, MMC4_RESP0) >> 24;
		cmd->response[3] = sdh_get_reg(mmc, MMC4_RESP0) << 8;
	} else {
		cmd->response[0] = sdh_get_reg(mmc, MMC4_RESP0);
		cmd->response[1] = sdh_get_reg(mmc, MMC4_RESP1);
		cmd->response[2] = sdh_get_reg(mmc, MMC4_RESP2);
		cmd->response[3] = sdh_get_reg(mmc, MMC4_RESP3);
	}

	if (data) {
		unsigned int i, block = 0;
		if (data->flags == MMC_DATA_READ) {
			do {
				if (sdh_wait_istat_bit(mmc, BUFRDRDY))
					return -1;
				for (i = 0; i < data->blocksize; i += 4)
					*(u32 *)(data->dest + block * data->blocksize + i) =
						sdh_get_reg(mmc, MMC4_DP);
				block++;
			} while (data->blocks >= block + 1);
		} else {
			while (data->blocks >= block + 1) {
				if (sdh_wait_istat_bit(mmc, BUFWRRDY))
					return -1;
				for (i = 0; i < data->blocksize; i += 4) {
					 sdh_set_reg(mmc, MMC4_DP,
						*(u32 *)(data->src + block * data->blocksize + i));
				}
				block++;
			}
		}

		if (sdh_wait_istat_bit(mmc, XFRCOMP))
			return -1;

		//sdh_reset(mmc, CMDSWRST | DATSWRST);
	}

	return 0;
}

static int sdh_set_clock(struct mmc *mmc, unsigned int clk_rt)
{
	int i = 0;

	sdh_set_reg(mmc, MMC4_CNTL2,
		(sdh_get_reg(mmc, MMC4_CNTL2) & (~SDFREQ_MASK)) | (clk_rt << SDFREQ_OFFSET));
	sdh_set_reg(mmc, MMC4_CNTL2,
		(sdh_get_reg(mmc, MMC4_CNTL2) | MMC4CLKEN));
	while (!(sdh_get_reg(mmc, MMC4_CNTL2) & MMC4CLKEN)) {
		if (i++ > 1000) {
			eprintf("Failed to enable exteral mmc clock\n");
			return -1;
		}
		udelay(100);
	}

	return 0;
}

void pxasdh_set_ios(struct mmc *mmc)
{
	u32 cntl1 = sdh_get_reg(mmc, MMC4_CNTL1);
	u32 clk, shift, clkrt;
	struct sdh_host *host = (struct sdh_host *)mmc->priv;

	dprintf("%s: bus_width=%d, clock=%d\n",
		__FUNCTION__, mmc->bus_width, mmc->clock);
	switch (mmc->bus_width) {
	case 8:
		dprintf("set as 8-bit bus\n");
		sdh_set_reg(mmc, MMC4_CNTL1,
			(cntl1 & ~_4BITMD) | _8BITMD);
		break;
	case 4:
		dprintf("set as 4-bit bus\n");
		sdh_set_reg(mmc, MMC4_CNTL1,
			(cntl1 & ~_8BITMD) | _4BITMD);
		break;
	default:
		sdh_set_reg(mmc, MMC4_CNTL1,
			(cntl1 & ~_8BITMD & ~_4BITMD));
	}

	if (!mmc->clock)
		return;

	if (mmc->clock >= host->clk_rate) {
		clkrt = 0x00;
	} else {
		clk = host->clk_rate / mmc->clock;
		shift = 0;
		while (clk >= (0x1 << shift))
			shift++;
		clkrt = (0x1 << (shift - 2));
		if ((host->clk_rate / (clkrt * 2)) > mmc->clock)
			clkrt *= 2;
	}
	sdh_set_reg(mmc, MMC4_CNTL2,
		sdh_get_reg(mmc, MMC4_CNTL2) & ~MMC4CLKEN);
	if (mmc->clock > 26000000) {
		dprintf("set as HIGH_SPEED\n");
		sdh_set_reg(mmc, MMC4_CNTL1,
			sdh_get_reg(mmc, MMC4_CNTL1) | HISPEED);
	} else
		sdh_set_reg(mmc, MMC4_CNTL1,
			sdh_get_reg(mmc, MMC4_CNTL1) & ~HISPEED);

	dprintf("set clkrt as 0x%x\n", clkrt);
	if (sdh_set_clock(mmc, clkrt))
		eprintf("Error while setting clock\n");
}

int pxasdh_init(struct mmc *mmc)
{
	unsigned int i = 0;

	//sdh_reset(mmc, MSWRST);

	// enable internal mmc clock
	sdh_set_reg(mmc, MMC4_CNTL2, INTCLKEN);
	while (!(sdh_get_reg(mmc, MMC4_CNTL2) & INTCLKSTB)) {
		if (i++ > 1000) {
			eprintf("Failed to enable interal mmc clock\n");
			return -1;
		}
		udelay(100);
	}
	// set clock as 187.5kHZ(48MHZ/(0x80 * 2))
	if (sdh_set_clock(mmc, 0x80))
		return -1;

	// Eable all state
	sdh_set_reg(mmc, MMC4_I_STAT_EN, STAT_EN_MASK);
	sdh_set_reg(mmc, MMC4_I_SIG_EN, 0);

	// set read response timeout
	sdh_set_reg(mmc, MMC4_CNTL2,
		(sdh_get_reg(mmc, MMC4_CNTL2) | (0xE << DTOCNTR_OFFSET)));

	// set power
	//sdh_set_reg(mmc, MMC4_CNTL1,
	//	sdh_get_reg(mmc, MMC4_CNTL1) | (0x7 << VLTGSEL_OFFSET));
	sdh_set_reg(mmc, MMC4_CNTL1,
		sdh_get_reg(mmc, MMC4_CNTL1) | BUSPWR);

	return 0;
}

static char *PXASDH_NAME = "pxa9xx-sdh";
static int sdh_add_mmc(u32 sdh_id)
{
	struct mmc *mmc;
	struct sdh_host *host = NULL;

	host = malloc(sizeof(struct sdh_host));
	if (!host) {
		eprintf("sdh_host malloc fail!\n");
		return -1;
	}

	mmc = malloc(sizeof(struct mmc));
	if (!mmc) {
		eprintf("mmc malloc fail!\n");
		return -1;
	}

	if (sdh_id == 3)
		host->reg_base = (void *)MMC4_1_REG_BASE_ADDR;
	else if (sdh_id == 4)
		host->reg_base = (void *)MMC4_2_REG_BASE_ADDR;
	else if (sdh_id == 5)
		host->reg_base = (void *)MMC4_3_REG_BASE_ADDR;
	else
		return -1;

	host->clk_rate = MMC4_MAX_SPEED;
	mmc->priv = host;

	sprintf(mmc->name, "%s.%d", PXASDH_NAME, sdh_id);
	mmc->send_cmd = pxasdh_send_cmd;
	mmc->set_ios = pxasdh_set_ios;
	mmc->init = pxasdh_init;

	mmc->f_min = host->clk_rate / 0x100;
	mmc->f_max = host->clk_rate;
	mmc->voltages = MMC_VDD_32_33 | MMC_VDD_33_34;
	mmc->host_caps = MMC_MODE_HS | MMC_MODE_HS_52MHz
		| MMC_MODE_4BIT;// | MMC_MODE_8BIT;

	mmc_register(mmc);

	return mmc_init(mmc);
}

int pxa9xx_mmc_init(bd_t *bis)
{
	if (bis->bi_arch_number == 1828) { //Tavor PV
		/* MMC4 */
		__REG(0x40E1029C) = 0x1805; //MMC4_CMD_GPIO55
		__REG(0x40E102A0) = 0x1805; //MMC4_CLK_GPIO56
		__REG(0x40E10294) = 0x1804; //MMC4_DAT0_GPIO57
		__REG(0x40E10298) = 0x1804; //MMC4_DAT1_GPIO58
		__REG(0x40E102A4) = 0x1801; //MMC4_DAT2_GPIO59
		__REG(0x40E102A8) = 0x1801; //MMC4_DAT3_GPIO60
		return sdh_add_mmc(4);
 	} else if ((bis->bi_arch_number == 2137) || \
 			(bis->bi_arch_number == 2139/*SAARB*/)) { /*Tavor PV2*/
		int ret = 0;
		/* MMC3 */
#define PV2_MMC3_PIN_VAL	0x0000D0C2
		__REG(0x40E1024C) = PV2_MMC3_PIN_VAL;	//MMC3_CMD, DF_ADDR0(GPIO177)
		__REG(0x40E10250) = PV2_MMC3_PIN_VAL;	//MMC3_CLK, DF_ADDR1(GPIO178)
		__REG(0x40E1025C) = PV2_MMC3_PIN_VAL;	//MMC3_DAT0, DF_ADDR2(GPIO179)
		__REG(0x40E10260) = PV2_MMC3_PIN_VAL;	//MMC3_DAT1, DF_ADDR3(GPIO180)
		__REG(0x40E10204) = PV2_MMC3_PIN_VAL;	//MMC3_DAT2, nXCVREN(GPIO181)
		__REG(0x40E10254) = PV2_MMC3_PIN_VAL;	//MMC3_DAT3, nLUA(GPIO182)
		__REG(0x40E10234) = 0x0000D0C4;		//MMC3_DAT4, ND_nWE(GPIO172)
		__REG(0x40E1021C) = PV2_MMC3_PIN_VAL;	//MMC3_DAT5, nBE0(GPIO184)
		__REG(0x40E10220) = PV2_MMC3_PIN_VAL;	//MMC3_DAT6, nBE1(GPIO185)
		__REG(0x40E10238) = 0x0000D0C4;		//MMC3_DAT7, ND_nRE(GPIO173)

		ret = sdh_add_mmc(3);
		if (ret)
			return ret;

#if 0
		/* MMC4 */
#define PV2_MMC4_PIN_VAL	0x0000D0C3
		__REG(0x40E1029C) = PV2_MMC4_PIN_VAL;	//MMC3_CMD, GPIO55
		__REG(0x40E102A0) = PV2_MMC4_PIN_VAL;	//MMC3_CLK, GPIO56
		__REG(0x40E10294) = PV2_MMC4_PIN_VAL;	//MMC3_DAT0, GPIO57
		__REG(0x40E10298) = PV2_MMC4_PIN_VAL;	//MMC3_DAT1, GPIO58
		__REG(0x40E102A4) = PV2_MMC4_PIN_VAL;	//MMC3_DAT2, GPIO59
		__REG(0x40E102A8) = PV2_MMC4_PIN_VAL;	//MMC3_DAT3, GPIO60

		ret = sdh_add_mmc(4);
#endif
		return ret;
	} else {
		eprintf("no pxa9xx-sdh mmc support on board %lu\n", bis->bi_arch_number);
		return -1;
	}
}

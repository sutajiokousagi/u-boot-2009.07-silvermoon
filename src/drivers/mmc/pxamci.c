/*
 * Copyright 2009, Marvell Semiconductor Inc.
 * Lei Wen
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
#include <part.h>
#include <malloc.h>
#include <mmc.h>
#include <asm/io.h>

#include "pxamci.h"

#define NR_SG	32
#define CLKRT_OFF	(~0)
#define PXA_INTERVAL		100
#define PXA_TIMER_TIMEOUT	500

#define mmc_resp_type(cmd)      ((cmd)->resp_type & (MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC|MMC_RSP_BUSY|MMC_RSP_OPCODE))
#define RSP_TYPE(x)	((x) & ~(MMC_RSP_BUSY|MMC_RSP_OPCODE))

struct pxamci_host{
	unsigned int cmdat;
	unsigned int regbase;
	struct mmc_cmd *cmd;
	struct mmc_data *data;
};

static void pxamci_stop_clock(struct mmc *mmc)
{
	struct pxamci_host *host = mmc->priv;

	if (readl(host->regbase + MMC_STAT) & STAT_CLK_EN) {
		unsigned long timeout = 10000;
		unsigned int v;

		writel(STOP_CLOCK, host->regbase + MMC_STRPCL);

		do {
			v = readl(host->regbase + MMC_STAT);
			if (!(v & STAT_CLK_EN))
				break;
			udelay(1);
		} while (timeout--);

		if (v & STAT_CLK_EN)
			printf("unable to stop clock\n");
	}
}

static void pxamci_start_clock(struct mmc *mmc)
{
	unsigned int clock = mmc->clock;
	struct pxamci_host *host = mmc->priv;

	if (!(readl(host->regbase + MMC_STAT) & STAT_CLK_EN)) {
		unsigned long timeout = 10000;
		unsigned int v;

		writel(clock, host->regbase + MMC_CLKRT);
		writel(START_CLOCK, host->regbase + MMC_STRPCL);

		do {
			v = readl(host->regbase + MMC_STAT);
			if (v & STAT_CLK_EN)
				break;
			udelay(1);
		} while (timeout--);

		if (!timeout)
			printf("unable to start clock\n");
	}
}

static void pxamci_setup_data(struct mmc *mmc, struct mmc_data *data)
{
	unsigned int nob = data->blocks;
	unsigned long long clks;
	unsigned int timeout;
	struct pxamci_host *host = mmc->priv;
	u32 dcmd;
	int i;

	host->data = data;
	writel(nob, host->regbase + MMC_NOB);
	writel(data->blocksize, host->regbase + MMC_BLKLEN);

	timeout = mmc->tran_speed / 3;
	writel((timeout + 255) / 256, host->regbase + MMC_RDTO);
}

static int pxamci_cmd_done(struct mmc *mmc)
{
	struct pxamci_host *host = mmc->priv;
	struct mmc_cmd *cmd = host->cmd;;
	int i, status;
	u32 v;

	v = readl(host->regbase + MMC_RES) & 0xffff;
	for (i = 0; i < 4; i++) {
		u32 w1 = readl(host->regbase + MMC_RES) & 0xffff;
		u32 w2 = readl(host->regbase + MMC_RES) & 0xffff;
		cmd->response[i] = v << 24 | w1 << 8 | w2 >> 8;
		v = w2;
	}

	status = readl(host->regbase + MMC_STAT);
	writel(status & ~STAT_END_CMD_RES,host->regbase + MMC_STAT);
	if (status & STAT_TIME_OUT_RESPONSE) {
		printf("cmd timeout!!\n");
		return -1;
	}

	return 0;
}

static int pxamci_read_pio(struct mmc *mmc)
{
	struct pxamci_host *host = mmc->priv;
	struct mmc_data *data = host->data;
	int bytes_to_handle;
	unsigned int value, ofs, ireg, status;

	bytes_to_handle = data->blocks * data->blocksize;
	ofs = 0;

	while (bytes_to_handle > 0) {
		ireg = readl(host->regbase + MMC_I_REG);

		if (ireg & MMC_I_REG_RXFIFO_RD_REQ) {
			int i = min(bytes_to_handle, 64);

			bytes_to_handle -= i;
			for (; i > 0; i --) {
				value = readb(host->regbase + MMC_RXFIFO);
				data->dest[ofs ++] = value;
			}
		}
		else
			break;

		status = readl(host->regbase + MMC_STAT);
		if (status & STAT_ERROR) {
			printf("PXAMCI STAT error %lx\n", status);
			return -1;
		}
	}

	if (bytes_to_handle) {
		printf("Data transfer hasn't finished, %d left"
				", aborted!!\n", bytes_to_handle);
		return -1;
	}
	else
		return 0;
}

static int pxamci_write_pio (struct mmc *mmc)
{
	struct pxamci_host *host = mmc->priv;
	struct mmc_cmd *cmd = host->cmd;
	struct mmc_data *data = host->data;
	unsigned int ofs, ireg, status, error_status;
	int bytes_to_handle;


	bytes_to_handle = data->blocks * data->blocksize;
	ofs = 0;
	while (bytes_to_handle > 0) {
		ireg = readl(host->regbase + MMC_I_REG);

		if (ireg & MMC_I_REG_TXFIFO_WR_REQ) {
			int i, bytes = min(32, bytes_to_handle);

			for (i = 0; i < bytes; i ++)
				writeb(data->src[ofs ++],
						host->regbase + MMC_TXFIFO);

			if (bytes < 32)
				writel(BUF_PART_FULL,
						host->regbase + MMC_PRTBUF);

			bytes_to_handle -= bytes;
		}

		status = readl(host->regbase + MMC_STAT);
		if (status & STAT_ERROR) {
			printf("PXAMCI STAT error %lx\n", status);
			return -1;
		}
	}

	return 0;
}

static int pxamci_start_cmd(struct mmc *mmc, unsigned int cmdat)
{
	struct pxamci_host *host = mmc->priv;
	struct mmc_cmd *cmd = host->cmd;
	struct mmc_data *data = host->data;
	unsigned int ireg, stat;
	int res, intervals = 0, fifo_req;

	if (cmd->resp_type & MMC_RSP_BUSY)
		cmdat |= CMDAT_BUSY;

	switch (RSP_TYPE(mmc_resp_type(cmd))) {
	case RSP_TYPE(MMC_RSP_R1): /* r1, r1b, r6, r7 */
		cmdat |= CMDAT_RESP_SHORT;
		break;
	case RSP_TYPE(MMC_RSP_R3):
		cmdat |= CMDAT_RESP_R3;
		break;
	case RSP_TYPE(MMC_RSP_R2):
		cmdat |= CMDAT_RESP_R2;
		break;
	default:
		break;
	}

	if (data) {
		if (data->flags == MMC_DATA_READ)
			fifo_req = MMC_I_REG_RXFIFO_RD_REQ;
		else
			fifo_req = MMC_I_REG_TXFIFO_WR_REQ;
	}
	else
		fifo_req = 0;

	writel(cmd->cmdidx, host->regbase + MMC_CMD);
	writel(cmd->cmdarg>> 16, host->regbase + MMC_ARGH);
	writel(cmd->cmdarg & 0xffff, host->regbase + MMC_ARGL);
	writel(cmdat, host->regbase + MMC_CMDAT);

//	printf("ready to send command %x, cmddat %x\n", cmd->cmdidx, cmdat);
	//pxamci_start_clock(mmc);

	do {
		ireg = readl(host->regbase + MMC_I_REG);

		if (ireg & END_CMD_RES) {
			res = pxamci_cmd_done(mmc);

			if (res || !fifo_req)
				break;
		}

		if (ireg & fifo_req) {
			if (data->flags == MMC_DATA_READ)
				res = pxamci_read_pio(mmc);
			else
				res = pxamci_write_pio(mmc);

			break;
		}

		udelay(PXA_INTERVAL);

	} while (intervals ++ < PXA_TIMER_TIMEOUT);

	stat = readl(host->regbase + MMC_STAT);
	writel(stat & ~(STAT_END_CMD_RES
				| STAT_PRG_DONE
				| STAT_PRG_DONE),
			host->regbase + MMC_STAT);

	if (intervals >= PXA_TIMER_TIMEOUT) {
		printf("POLLING failed!!\n");
		return -1;
	}
	else
		return res;
}

static int pxamci_request(struct mmc *mmc, struct mmc_cmd *cmd,
		struct mmc_data *data)
{
	unsigned int cmdat;
	struct pxamci_host *host = mmc->priv;

//	pxamci_stop_clock(mmc);

	host->cmd = cmd;
	host->data = NULL;
	cmdat = host->cmdat;
	host->cmdat &= ~CMDAT_INIT;
	if (data) {
		pxamci_setup_data(mmc, data);

		cmdat &= ~CMDAT_BUSY;
		cmdat |= CMDAT_DATAEN;
		if (data->flags & MMC_DATA_WRITE)
			cmdat |= CMDAT_WRITE;
	}

	return pxamci_start_cmd(mmc, cmdat);
}

static void pxamci_set_ios(struct mmc *mmc)
{
	struct pxamci_host *host = mmc->priv;
	int cmdat;

	pxamci_stop_clock(mmc);
	pxamci_start_clock(mmc);
	
	cmdat = readl(host->regbase + MMC_CMDAT);
	if (mmc->bus_width == 4)
		writel(cmdat | CMDAT_SD_4DAT, MMC_CMDAT);

}

static int mci_init(struct mmc *mmc)
{
	struct pxamci_host *host = mmc->priv;
	*(unsigned long *)(host->regbase + MMC_CLKRT) = MMC_CLKRT_0_3125MHZ;
	*(unsigned long *)(host->regbase + MMC_RESTO) = MMC_RES_TO_MAX / 2;
	*(unsigned long *)(host->regbase + MMC_SPI) = MMC_SPI_DISABLE;
	host->cmdat = CMDAT_INIT;

	return 0;
}

int pxamci_init(bd_t *bis)
{
	struct mmc *mmc;
	struct pxamci_host *host;

	mmc = malloc(sizeof(struct mmc));
	if (!mmc) {
		printf("mmc malloc fail!!\n");
		return -1;
	}

	host = malloc(sizeof(struct pxamci_host));
	if (!host) {
		printf("host malloc fail!!!\n");
		return -1;
	}

	host->regbase = CONFIG_SYS_MMC_BASE;

	mmc->priv = host;
	mmc->send_cmd = pxamci_request;
	mmc->set_ios = pxamci_set_ios;
	mmc->init = mci_init;

	/*
	 * Calculate minimum clock rate, rounding up.
	 */
	mmc->f_min = (19500000 + 63) / 64;
#if defined(CONFIG_CPU_MONAHANS_L)	\
	|| defined(CONFIG_CPU_TAVORP)	\
	|| defined(CONFIG_CPU_MONAHANS_LV)
		mmc->f_max = 26000000;
#elif defined(CONFIG_TAVOR_PV)
		mmc->f_max = 19500000;
#else
#error "You should specify the CPU type!!"
#endif
	mmc->voltages = MMC_VDD_32_33|MMC_VDD_33_34;

	mmc_register(mmc);
	return 0;
}

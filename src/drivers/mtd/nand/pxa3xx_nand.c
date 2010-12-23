/*
 * drivers/mtd/nand/pxa3xx_nand.c
 *
 * Copyright © 2005 Intel Corporation
 * Copyright © 2006 Marvell International Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifdef   __KERNEL__
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/io.h>
#include <linux/irq.h>

#include <mach/dma.h>
#include <mach/pxa3xx_nand.h>
#include <mach/nand_supported.h>
#include <mach/pxa3xx_bbm.h>
#endif

/* convert nano-seconds to nand flash controller clock cycles */
#define ns2cycle(ns, clk)	(int)(((ns) * (clk / 1000000) / 1000) + 1)
#define	CHIP_DELAY_TIMEOUT	(500)
#define BCH_THRESHOLD 		(8)
#define PAGE_CHUNK_SIZE		(2048)
#define OOB_CHUNK_SIZE		(64)

static struct nand_ecclayout hw_smallpage_ecclayout = {
	.eccbytes = 6,
	.eccpos = {8, 9, 10, 11, 12, 13 },
	.oobfree = { {2, 6} }
};

static struct nand_ecclayout hw_largepage_ecclayout = {
	.eccbytes = 24,
	.eccpos = {
		40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55,
		56, 57, 58, 59, 60, 61, 62, 63},
	.oobfree = { {2, 38} }
};

static inline int is_buf_blank(uint8_t *buf, size_t len)
{
	for (; len > 0; len--)
		if (*buf++ != 0xff)
			return 0;
	return 1;
}

static void pxa3xx_nand_set_timing(struct pxa3xx_nand_info *info,
		const struct pxa3xx_nand_timing *t)
{
	struct pxa3xx_nand *nand = info->nand_data;
	unsigned long nand_clk = clk_get_rate(nand->clk);
	uint32_t ndtr0, ndtr1, tRP;

	ndtr0 = ndtr1 = 0;
	tRP = (t->tRP > 0xf) ? 0xf : t->tRP;
	if (tRP > 0x7) {
		ndtr0 |= NDTR0_ETRP;
		tRP &= 0x7;
	}

	if (nand->RD_CNT_DEL > 0)
		ndtr0 |= NDTR0_SELCNTR
			| (NDTR0_RD_CNT_DEL(nand->RD_CNT_DEL - 1));

	ndtr0 |= NDTR0_tCH(ns2cycle(t->tCH, nand_clk))		    \
		| NDTR0_tCS(ns2cycle(t->tCS, nand_clk))		    \
		| NDTR0_tWH(ns2cycle(t->tWH, nand_clk))		    \
		| NDTR0_tWP(ns2cycle(t->tWP, nand_clk))		    \
		| NDTR0_tRH(ns2cycle(t->tRH, nand_clk))		    \
		| NDTR0_tRP(ns2cycle(tRP, nand_clk));

	if (nand->wait_mode)
		ndtr1 |= NDTR1_WAIT_MODE;

	ndtr1 |= NDTR1_tR(ns2cycle(t->tR, nand_clk))		    \
		| NDTR1_tWHR(ns2cycle(t->tWHR, nand_clk))	    \
		| NDTR1_tAR(ns2cycle(t->tAR, nand_clk));


	nand_writel(nand, NDTR0CS0, ndtr0);
	nand_writel(nand, NDTR1CS0, ndtr1);
}

static void pxa3xx_set_datasize(struct pxa3xx_nand_info *info)
{
	const struct pxa3xx_nand_flash *flash_info = info->flash_info;

	if (likely(flash_info->page_size >= PAGE_CHUNK_SIZE)) {
		switch (info->use_ecc) {
			case ECC_HAMMIN:
				info->data_size = 2088;
				break;
			case ECC_BCH:
				info->data_size = 2080;
				break;
			default:
				info->data_size = 2112;
				break;
		}
	}
	else {
		switch (info->use_ecc) {
			case ECC_HAMMIN:
				info->data_size = 520;
				break;
			case ECC_BCH:
			default:
				info->data_size = 528;
				break;
		}
	}
}

/* NOTE: it is a must to set ND_RUN firstly, then write 
 * command buffer, otherwise, it does not work
 */
static void pxa3xx_nand_start(struct pxa3xx_nand_info *info)
{
	uint32_t ndcr, ndeccctrl;
	struct pxa3xx_nand *nand = info->nand_data;

	ndcr = info->reg_ndcr;
	ndeccctrl = nand_readl(nand, NDECCCTRL);
	ndeccctrl &=  ~(NDECCCTRL_BCH_EN | NDECCCTRL_ECC_THR_MSK);

	switch (info->use_ecc) {
	case ECC_BCH:
		ndeccctrl |= NDECCCTRL_BCH_EN;
		ndeccctrl |= NDECCCTRL_ECC_THRESH(BCH_THRESHOLD);
	case ECC_HAMMIN:
		ndcr |= NDCR_ECC_EN;
		break;
	default:
		break;
	}

	ndcr |= nand->use_dma ? NDCR_DMA_EN : 0;
	ndcr |= NDCR_ND_RUN;

	/* clear status bits and run */
	nand_writel(nand, NDECCCTRL, ndeccctrl);
	nand_writel(nand, NDSR, NDSR_MASK);
	nand_writel(nand, NDCR, ndcr);
}

static void pxa3xx_nand_stop(struct pxa3xx_nand* nand)
{
	uint32_t ndcr, ndeccctrl;

	/* clear status bits */
	nand_writel(nand, NDSR, NDSR_MASK);

	ndcr = nand_readl(nand, NDCR);

	if (ndcr & NDCR_ND_RUN) {
		ndcr &= ~NDCR_ND_RUN;
		nand_writel(nand, NDCR, ndcr);

		/* wait RUN bit in NDCR become 0 */
		do {
			ndcr = nand_readl(nand, NDCR);
		} while (ndcr & NDCR_ND_RUN);
	}

	/* clear the ECC control register */
	ndeccctrl = nand_readl(nand, NDECCCTRL);
	ndeccctrl &=  ~(NDECCCTRL_BCH_EN | NDECCCTRL_ECC_THR_MSK);
	nand_writel(nand, NDECCCTRL, ndeccctrl);
}

static void start_data_dma(struct pxa3xx_nand *nand, int dir_out)
{
	struct mtd_info *mtd = nand->mtd[nand->chip_select];
	struct pxa3xx_nand_info *info = mtd->priv;
	struct pxa_dma_desc *desc = info->data_desc;
	int dma_len = ALIGN(info->data_size, 32);

	desc->ddadr = DDADR_STOP;
	desc->dcmd = DCMD_ENDIRQEN | DCMD_WIDTH4	\
		     | DCMD_BURST32 | dma_len;

	if (dir_out) {
		desc->dsadr = info->data_buff_phys;
		desc->dtadr = NDDB_DMA_ADDR;

		desc->dcmd |= DCMD_INCSRCADDR 		\
			      | DCMD_FLOWTRG;
	} else {
		desc->dtadr = info->data_buff_phys;
		desc->dsadr = NDDB_DMA_ADDR;

		desc->dcmd |= DCMD_INCTRGADDR 		\
			      | DCMD_FLOWSRC;
	}

	DRCMR(nand->drcmr_dat) = DRCMR_MAPVLD | nand->data_dma_ch;
	DDADR(nand->data_dma_ch) = info->data_desc_addr;
	DCSR(nand->data_dma_ch) |= DCSR_RUN;
}

static void handle_data_pio(struct pxa3xx_nand *nand)
{
	void __iomem * mmio_base = nand->mmio_base;
	struct mtd_info *mtd = nand->mtd[nand->chip_select];
	struct pxa3xx_nand_info *info = mtd->priv;
	int oob_size;

	oob_size = info->data_size - PAGE_CHUNK_SIZE;

	if (nand->is_write) {
		if (oob_size > 0) {
			/* write data part */
			__raw_writesl(mmio_base + NDDB,				\
					info->data_buff	+ info->data_column,	\
					PAGE_CHUNK_SIZE >> 2);

			/* write oob part */
			__raw_writesl(mmio_base + NDDB,				\
					info->oob_buff + info->oob_column,	\
					oob_size >> 2);
		}
		else
			__raw_writesl(mmio_base + NDDB,				\
					info->data_buff	+ info->data_column,	\
					info->data_size >> 2);
	}
	else {
		if (oob_size > 0) {
			/* read data part */
			__raw_readsl(mmio_base + NDDB,				\
					info->data_buff	+ info->data_column,	\
					PAGE_CHUNK_SIZE >> 2);

			/* read oob part */
			__raw_readsl(mmio_base + NDDB,				\
					info->oob_buff + info->oob_column,	\
					oob_size >> 2);
		}
		else
			__raw_readsl(mmio_base + NDDB,				\
					info->data_buff	+ info->data_column,	\
					info->data_size >> 2);

	}

	info->data_column += PAGE_CHUNK_SIZE;
	info->oob_column += OOB_CHUNK_SIZE;
}

static void pxa3xx_nand_data_dma_irq(int channel, void *data)
{
	struct pxa3xx_nand *nand= data;
	struct mtd_info *mtd = nand->mtd[nand->chip_select];
	struct pxa3xx_nand_info *info = mtd->priv;
	uint32_t dcsr, ndcr;

	dcsr = DCSR(channel);
	DCSR(channel) = dcsr;
	if (dcsr & DCSR_BUSERR) {
		nand->errcode |= ERR_DMABUSERR;
	}

	ndcr = nand_readl(nand, NDCR);
	ndcr &= ~NDCR_INT_MASK;
	nand_writel(nand, NDCR, ndcr);
}

static irqreturn_t pxa3xx_nand_irq(int irq, void *devid)
{
	struct pxa3xx_nand *nand = devid;
	struct pxa3xx_nand_info *info;
	struct mtd_info *mtd;
	unsigned int status;
	int chip_select, cmd_done, ready, page_done, badblock_detect;
	int cmd_seqs, ndcb1, ndcb2, ndcr;

	chip_select 	= nand->chip_select;
	ready		= (chip_select) ? NDSR_RDY : NDSR_FLASH_RDY;
	cmd_done	= (chip_select) ? NDSR_CS1_CMDD : NDSR_CS0_CMDD;
	page_done	= (chip_select) ? NDSR_CS1_PAGED : NDSR_CS0_PAGED;
	badblock_detect	= (chip_select) ? NDSR_CS1_BBD : NDSR_CS0_BBD;
	mtd		= nand->mtd[chip_select];
	info		= (struct pxa3xx_nand_info *)(mtd->priv);
	cmd_seqs	= info->current_cmd_seqs;

	status = nand_readl(nand, NDSR);

	if ((status & NDSR_WRDREQ) || (status & NDSR_RDDREQ)) {

		nand->state |= 2;
		/* whether use dma to transfer data */
		if (nand->use_dma) {
			ndcr = nand_readl(nand, NDCR);
			ndcr |= NDCR_INT_MASK;
			nand_writel(nand, NDCR, ndcr);
			start_data_dma(nand, nand->is_write);
		}
		else 
			handle_data_pio(nand);

	}

	if (status & cmd_done) {

		nand->state |= 4;
		/* complete the command cycle when all command
		 * done, and don't wait for ready signal
		 */
		if ((cmd_seqs == info->total_cmds)	\
				&& !(cmd_seqs == info->need_wait_ready)) {

			complete(&nand->cmd_complete);
		}
	}

	if (status & ready) {
		nand->state |= 8;
		/* 
		 * wait for the ready signal, 
		 * then leavl the command cycle
		 */
		if ((cmd_seqs == info->total_cmds) \
				&& (cmd_seqs == info->need_wait_ready)) {

			complete(&nand->cmd_complete);
		}

		nand->is_ready = 1;
	}

	if (status & NDSR_TRUSTVIO) {
		nand->errcode |= ERR_TRUSTVIO;
	}

	if (status & NDSR_ERR_CNT_MASK) {
		nand->bad_count = (status & NDSR_ERR_CNT_MASK) >> 16;
	}

	if (status & NDSR_CORERR) {
		nand->errcode |= ERR_CORERR;
	}

	if (status & NDSR_UNCERR) {
		nand->errcode |= ERR_DBERR;
	}

	if (status & badblock_detect) {
		nand->errcode |= ERR_BBERR;
	}

	if (status & page_done) {
	}

	if (status & NDSR_WRCMDREQ) {
		if (cmd_seqs < info->total_cmds) {

			info->current_cmd_seqs ++;
			if (cmd_seqs == info->need_addressing) {
				ndcb1 = info->ndcb1;
				ndcb2 = info->ndcb2;
			}
			else {
				ndcb1 = 0;
				ndcb2 = 0;
			}

			nand->state |= 1;
			nand_writel(nand, NDCB0, info->ndcb0[cmd_seqs]);
			nand_writel(nand, NDCB0, ndcb1);
			nand_writel(nand, NDCB0, ndcb2);
		}
		else
			complete(&nand->cmd_complete);

	}

	/* clear NDSR to let the controller exit the IRQ */
	nand_writel(nand, NDSR, status);
	return IRQ_HANDLED;
}

static int pxa3xx_nand_dev_ready(struct mtd_info *mtd)
{
	struct pxa3xx_nand_info *info = mtd->priv;
	struct pxa3xx_nand *nand = info->nand_data;
	int ready_mask = (nand->chip_select) 			    \
			 ? NDSR_RDY : NDSR_FLASH_RDY;

	return (nand_readl(nand, NDSR) & ready_mask) ? 1 : 0;
}

static int prepare_command_poll(struct pxa3xx_nand *nand, int command, 
		uint16_t column, int page_addr)
{
	uint16_t cmd;
	int addr_cycle = 0, exec_cmd = 1, ndcb0_csel,  i, use_dma;
	struct mtd_info *mtd = nand->mtd[nand->chip_select];
	struct pxa3xx_nand_info *info = mtd->priv;
	struct nand_chip *chip = mtd->priv;
	const struct pxa3xx_nand_flash *flash_info = info->flash_info;

	/* how many command is to be executed in one cycle */
	info->total_cmds = (info->flash_info->page_size > PAGE_CHUNK_SIZE) \
		   ? 5 : 1;

	ndcb0_csel = (nand->chip_select) ? NDCB0_CSEL : 0;
	/* clear the command buffer */
	for (i = 0; i < CMD_POLL_SIZE; i ++)
		info->ndcb0[i] = ndcb0_csel;

	info->buf_start		= 0;
	info->buf_count		= 0;
	info->current_cmd_seqs	= 0;
	info->need_addressing	= 0;
	info->need_wait_ready	= -1;

	nand->state		= 0;
	nand->is_write		= 0;
	nand->is_ready		= 0;
	nand->errcode		= ERR_NONE;
	nand->bad_count		= 0;

	use_dma = nand->use_dma;
	nand->use_dma = 0;
	addr_cycle = NDCB0_ADDR_CYC(info->row_addr_cycles  	    \
			+ info->col_addr_cycles);

	switch (command) {
	case NAND_CMD_READOOB:
	case NAND_CMD_READ0:

		cmd  = flash_info->cmdset->read1;
		nand->use_dma = use_dma;

		info->buf_count = mtd->writesize + mtd->oobsize;
		memset(info->data_buff, 0xFF, info->buf_count);

		if (command == NAND_CMD_READOOB) {
			info->buf_start = mtd->writesize + column;
			info->use_ecc = ECC_NONE;
		}
		else {
			info->buf_start = column;
			if (chip->ecc.mode == NAND_ECC_HW)
				info->use_ecc = flash_info->ecc_type;
			else
				info->use_ecc = ECC_NONE;
		}

		pxa3xx_set_datasize(info);

		if (flash_info->page_size > PAGE_CHUNK_SIZE) {
			/* need addressing at second command cycle */
			info->need_addressing = 1;
			info->need_wait_ready = 2;
			info->ndcb0[0] |= NDCB0_CMD_TYPE(0x6) 	    \
					  | NDCB0_NC  		    \
					  | addr_cycle		    \
					  | NDCB0_CMD2_MASK;

			info->ndcb0[1] |= NDCB0_CMD_TYPE(0x7) 	    \
					  | NDCB0_NC  		    \
					  | addr_cycle		    \
					  | NDCB0_CMD2_MASK;

			info->ndcb0[2] |= NDCB0_CMD_TYPE(0x6) 	    \
					  | NDCB0_NC  		    \
					  | addr_cycle		    \
					  | NDCB0_CMD2_MASK	    \
					  | ((cmd >> 8) & NDCB0_CMD1_MASK);

			info->ndcb0[3] |= NDCB0_CMD_XTYPE(0x5) 	    \
					  | NDCB0_NC 		    \
					  | addr_cycle		    \
					  | (cmd & NDCB0_CMD2_MASK) \
					  | NDCB0_CMD1_MASK;

			info->ndcb0[4] = info->ndcb0[3];
			info->ndcb0[4] &= ~NDCB0_NC;
		}
		else {
			info->need_wait_ready = 0;
			info->ndcb0[0] |= NDCB0_CMD_TYPE(0)	    \
					  | NDCB0_DBC		    \
					  | addr_cycle		    \
					  | cmd;
		}

		break;

	case NAND_CMD_PAGEPROG:
		cmd = flash_info->cmdset->program;
		nand->use_dma = use_dma;

		nand->is_write = 1;
		pxa3xx_set_datasize(info);
		if (flash_info->page_size > PAGE_CHUNK_SIZE) {
			/* need addressing at second command cycle */
			info->need_addressing = 1;
			info->need_wait_ready = 5;
			info->ndcb0[0] |= NDCB0_CMD_TYPE(0x6) 	    \
					  | NDCB0_NC  		    \
					  | (cmd & NDCB0_CMD1_MASK) \
					  | addr_cycle;

			info->ndcb0[1] |= NDCB0_CMD_TYPE(0xF) 	    \
					  | NDCB0_NC  		    \
					  | (cmd & NDCB0_CMD1_MASK) \
					  | addr_cycle;

			info->ndcb0[2] |= NDCB0_CMD_XTYPE(0x5)	    \
					  | NDCB0_NC 		    \
					  | NDCB0_AUTO_RS 	    \
					  | NDCB0_CMD_TYPE(0x1)	    \
					  | addr_cycle;

			info->ndcb0[3] = info->ndcb0[2];

			info->ndcb0[4] |= NDCB0_CMD_XTYPE(0x3)	    \
					  | NDCB0_ST_ROW_EN	    \
					  | NDCB0_DBC   	    \
					  | NDCB0_CMD_TYPE(0x1)     \
					  | (cmd & NDCB0_CMD2_MASK) \
					  | NDCB0_CMD1_MASK	    \
					  | addr_cycle;
		}
		else {
			info->need_wait_ready = 0;
			info->ndcb0[0] |= NDCB0_CMD_TYPE(1)	    \
					  | NDCB0_AUTO_RS	    \
					  | NDCB0_DBC		    \
					  | cmd			    \
					  | addr_cycle;
		}

		break;

	case NAND_CMD_READID:
		info->total_cmds = 1;
		cmd = flash_info->cmdset->read_id;
		info->data_size = 8;
		info->buf_count = info->read_id_bytes;

		info->ndcb0[0] |= NDCB0_CMD_TYPE(3)		    \
				  | NDCB0_ADDR_CYC(1)		    \
				  | cmd;

		break;

	case NAND_CMD_STATUS:
		info->total_cmds = 1;
		cmd = flash_info->cmdset->read_status;
		info->data_size = 8;
		info->buf_count = 1;

		info->ndcb0[0] |= NDCB0_CMD_TYPE(4)		    \
				  | NDCB0_ADDR_CYC(1)		    \
				  | cmd;

		break;

	case NAND_CMD_ERASE1:
		info->total_cmds = 1;
		cmd = flash_info->cmdset->erase;

		info->ndcb0[0] |= NDCB0_CMD_TYPE(2)		    \
			       | NDCB0_AUTO_RS 	    		    \
			       | NDCB0_ADDR_CYC(3)		    \
			       | NDCB0_DBC			    \
			       | cmd;

		break;
	case NAND_CMD_RESET:
		info->total_cmds = 1;
		cmd = flash_info->cmdset->reset;

		info->ndcb0[0] |= NDCB0_CMD_TYPE(5)		    \
			       | cmd;

		break;

	case NAND_CMD_SEQIN:
		info->buf_count = mtd->writesize + mtd->oobsize;
	case NAND_CMD_ERASE2:
		exec_cmd = 0;
		break;

	default:
		exec_cmd = 0;
		printk(KERN_ERR "non-supported command.\n");
		break;
	}

	nand->use_ecc = info->use_ecc;
	return exec_cmd;
}

static void pxa3xx_nand_cmdfunc(struct mtd_info *mtd, unsigned command,
		int column, int page_addr)
{
	struct pxa3xx_nand_info *info = mtd->priv;
	struct pxa3xx_nand *nand = info->nand_data;
	const struct pxa3xx_nand_flash *flash_info = info->flash_info;
	struct pxa3xx_bbm *pxa3xx_bbm = mtd->bbm;
	int ret, exec_cmd, retry_times = 0, use_dma;
	loff_t addr;

	/* reset timing */
	if (nand->chip_select != info->chip_select) {
		pxa3xx_nand_set_timing(info, flash_info->timing);
		nand->chip_select = info->chip_select;
	}

	/* if this is a x16 device ,then convert the input 
	 * "byte" address into a "word" address appropriate
	 * for indexing a word-oriented device
	 */
	if (flash_info->flash_width == 16)
		column /= 2;

	/* reset data and oob column point to handle data */
	info->data_column = 0;
	info->oob_column = 0;

	addr = page_addr << pxa3xx_bbm->page_shift;
	if (command == NAND_CMD_READOOB || command == NAND_CMD_READ0 || command == NAND_CMD_SEQIN
			|| command == NAND_CMD_ERASE1) {

		addr = pxa3xx_bbm->search(mtd, addr);
		page_addr = addr >> pxa3xx_bbm->page_shift;
	}

	nand->command = command;
	use_dma = nand->use_dma;
RETRY:
	exec_cmd = prepare_command_poll(nand, command, column, page_addr);

	switch (command) {
	case NAND_CMD_READOOB:
	case NAND_CMD_READ0:
	case NAND_CMD_SEQIN:
		/* small page addr setting */
		if (flash_info->page_size < PAGE_CHUNK_SIZE) {
			info->ndcb1 = page_addr * flash_info->page_size \
				      + (column & 0xFFFF);

			info->ndcb2 = 0;
		}
		else {
			info->ndcb1 = ((page_addr & 0xFFFF) << 16)	\
				      | (column & 0xFFFF);

			if (page_addr & 0xFF0000)
				info->ndcb2 = (page_addr & 0xFF0000) >> 16;
			else
				info->ndcb2 = 0;
		}

		break;

	case NAND_CMD_ERASE1:
		info->ndcb1 = page_addr;
		info->ndcb2 = 0;

		break;
	
	case NAND_CMD_PAGEPROG:
		if (is_buf_blank(info->data_buff, (mtd->writesize +
						mtd->oobsize)))
			exec_cmd = 0;
		break;

	default:
		info->ndcb1 = 0;
		info->ndcb2 = 0;
	}

	if (exec_cmd) {
		nand->state |= 16;

		/* prepare for the first command */
		init_completion(&nand->cmd_complete);

		pxa3xx_nand_start(info);

		ret = wait_for_completion_timeout(	    \
				&nand->cmd_complete, 	    \
				CHIP_DELAY_TIMEOUT);

		/* Stop State Machine for next command cycle */
		pxa3xx_nand_stop(nand);

		if (!ret && !nand->errcode) {
			printk(KERN_ERR "IRQ timeout, command %x, ndcb1 %x, \
					\nndcb2 %x, state %x, cmd seqs %x\n", \
					command, info->ndcb1, info->ndcb2,  \
					nand->state, info->current_cmd_seqs);

			printk("ndcr %x , ndsr %x\nndcb0 %x\ntiming0 %x, timing2 %x\nbb1 %x, bb2 %x\ndelay %x\n	ecc control %x\nrnb0 %x\n\n",
					nand_readl(nand, NDCR),
					nand_readl(nand, NDSR),
					nand_readl(nand, NDCB0),
					nand_readl(nand, NDTR0CS0),
					nand_readl(nand, NDTR1CS0),
					nand_readl(nand, NDBBR0),
					nand_readl(nand, NDBBR1),
					nand_readl(nand, NDREDEL),
					nand_readl(nand, NDECCCTRL),
					nand_readl(nand, NDBZCNT));

			/* It is just a workaround way for aspen R0 silicon
			 * should be remove in the future.
			 */
			if (retry_times ++ < 3 && nand->command == NAND_CMD_PAGEPROG)
				goto RETRY;
			else
				nand->errcode |= ERR_SENDCMD;
		}
	}

	/* reset ECC */
	nand->use_dma = use_dma;
	info->use_ecc = ECC_NONE;
	nand->state = 0;
}

static uint8_t pxa3xx_nand_read_byte(struct mtd_info *mtd)
{
	struct pxa3xx_nand_info *info = mtd->priv;
	char retval = 0xFF;

	if (info->buf_start < info->buf_count)
		/* Has just send a new command? */
		retval = info->data_buff[info->buf_start++];

	return retval;
}

static u16 pxa3xx_nand_read_word(struct mtd_info *mtd)
{
	struct pxa3xx_nand_info *info = mtd->priv;
	u16 retval = 0xFFFF;

	if (!(info->buf_start & 0x01)				    \
			&& info->buf_start < info->buf_count) {

		retval = *((u16 *)(info->data_buff+info->buf_start));
		info->buf_start += 2;
	}
	return retval;
}

static void pxa3xx_nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct pxa3xx_nand_info *info = mtd->priv;
	int real_len = min_t(size_t, len, info->buf_count - info->buf_start);

	memcpy(buf, info->data_buff + info->buf_start, real_len);
	info->buf_start += real_len;
}

static void pxa3xx_nand_write_buf(struct mtd_info *mtd,
		const uint8_t *buf, int len)
{
	struct pxa3xx_nand_info *info = mtd->priv;
	int real_len = min_t(size_t, len, info->buf_count - info->buf_start);

	memcpy(info->data_buff + info->buf_start, buf, real_len);
	info->buf_start += real_len;
}

static int pxa3xx_nand_verify_buf(struct mtd_info *mtd,
		const uint8_t *buf, int len)
{
	return 0;
}

static void pxa3xx_nand_select_chip(struct mtd_info *mtd, int chip)
{
	return;
}

/* Error handling expose to MTD level */
static int pxa3xx_nand_waitfunc(struct mtd_info *mtd, struct nand_chip *this)
{
	struct pxa3xx_nand_info *info = mtd->priv;
	struct pxa3xx_nand *nand = info->nand_data;

	if (nand->errcode & (ERR_BBERR | ERR_SENDCMD))
		return NAND_STATUS_FAIL;
	else
		return 0;
}

static int pxa3xx_nand_read_page_hwecc(struct mtd_info *mtd,
			struct nand_chip *chip,	uint8_t *buf)
{
	struct pxa3xx_nand_info *info = mtd->priv;
	struct pxa3xx_nand *nand = info->nand_data;

	chip->read_buf(mtd, buf, mtd->writesize);
	chip->read_buf(mtd, chip->oob_poi, mtd->oobsize);

	if (nand->errcode & ERR_CORERR) {
		switch (nand->use_ecc) {
		case ECC_BCH:
			if (nand->bad_count > BCH_THRESHOLD)
				mtd->ecc_stats.corrected +=
					(nand->bad_count - BCH_THRESHOLD);
			break;

		case ECC_HAMMIN:
			mtd->ecc_stats.corrected ++;
			break;

		case ECC_NONE:
		default:
			break;
		}
	}
	else if (nand->errcode & ERR_DBERR) {
		int buf_blank;

		buf_blank = is_buf_blank(buf, mtd->writesize);

		if (!buf_blank)
			mtd->ecc_stats.failed++;
	}

	return 0;
}

static void pxa3xx_nand_write_page_hwecc(struct mtd_info *mtd,
			struct nand_chip *chip, const uint8_t *buf)
{
	struct pxa3xx_nand_info *info = mtd->priv;
	const struct pxa3xx_nand_flash *flash_info = info->flash_info;

	chip->write_buf(mtd, buf, mtd->writesize);
	chip->write_buf(mtd, chip->oob_poi, mtd->oobsize);

	info->use_ecc = flash_info->ecc_type;
}

static int pxa3xx_nand_config_flash(struct pxa3xx_nand_info *info,
		const struct pxa3xx_nand_flash *f)
{
	/* enable all interrupts */
	uint32_t ndcr = 0;
	struct pxa3xx_nand *nand = info->nand_data;

	/* calculate flash information */
	info->oob_size = (f->page_size >= 2048) ? 64 : 16;
	info->oob_buff = info->data_buff + f->page_size;
	info->read_id_bytes = (f->page_size >= 2048) ? 4 : 2;

	/* calculate addressing information */
	info->col_addr_cycles = (f->page_size >= 2048) ? 2 : 1;

	if (f->num_blocks * f->page_per_block > 65536)
		info->row_addr_cycles = 3;
	else
		info->row_addr_cycles = 2;

	ndcr |= (nand->enable_arbiter) ? NDCR_ND_ARB_EN : 0;
	ndcr |= (info->col_addr_cycles == 2) ? NDCR_RA_START : 0;
	ndcr |= (f->flash_width == 16) ? NDCR_DWIDTH_M : 0;
	ndcr |= (f->dfc_width == 16) ? NDCR_DWIDTH_C : 0;

	switch (f->page_per_block) {
		case 32:
			ndcr |= NDCR_PG_PER_BLK(0x0);
			break;
		case 128:
			ndcr |= NDCR_PG_PER_BLK(0x1);
			break;
		case 256:
			ndcr |= NDCR_PG_PER_BLK(0x3);
			break;
		case 64:
		default:
			ndcr |= NDCR_PG_PER_BLK(0x2);
			break;
	}

	switch (f->page_size) {
		case 512:
			ndcr |= NDCR_PAGE_SZ(0x0);
			break;
		case 2048:
		default:
			ndcr |= NDCR_PAGE_SZ(0x1);
			break;

	}

	ndcr |= NDCR_RD_ID_CNT(info->read_id_bytes);
	ndcr |= NDCR_SPARE_EN; /* enable spare by default */

	info->reg_ndcr = ndcr;

	pxa3xx_nand_set_timing(info, f->timing);
	info->flash_info = f;
	return 0;
}

static void pxa3xx_erase_cmd(struct mtd_info *mtd, int page)
{
	struct nand_chip *chip = mtd->priv;
	/* Send commands to erase a block */
	chip->cmdfunc(mtd, NAND_CMD_ERASE1, -1, page);
}

static int pxa3xx_scan_bbt(struct mtd_info *mtd, loff_t ofs, int getchip)
{
	return 0;
}

static int pxa3xx_block_bad(struct mtd_info *mtd, loff_t ofs, int allowbbt)
{
	struct pxa3xx_bbm *bbm;
	struct reloc_table *table;
	struct mtd_oob_ops ops;
	uint32_t bad_mark;
	loff_t _ofs;

	bbm	= mtd->bbm;
	table	= bbm->table;

	if (bbm->current_slot <= PXA_BEGIN_SLOT
			|| table->total >= bbm->max_reloc_entry
			|| !bbm->table_init) {

		_ofs = bbm->search(mtd, ofs);
		if (_ofs != ofs)
			return 0;

		ops.ooboffs	= 0;
		ops.ooblen	= 2;
		ops.len		= 2;
		ops.datbuf	= NULL;
		ops.oobbuf	= &bad_mark;
		ops.mode	= MTD_OOB_PLACE;

		mtd->read_oob(mtd, ofs, &ops);
		if ((bad_mark & 0xFF) != 0xFF)
			return 1;
	}

	return 0;
}

static void pxa3xx_nand_init_mtd(struct mtd_info *mtd)
{
	struct pxa3xx_nand_info *info = mtd->priv;
	struct nand_chip *this = &info->nand_chip;
	const struct pxa3xx_nand_flash *f = info->flash_info;
	struct pxa3xx_bbm *bbm = mtd->bbm;

	this->options = (f->flash_width == 16) ? NAND_BUSWIDTH_16: 0;

	this->waitfunc		= pxa3xx_nand_waitfunc;
	this->select_chip	= pxa3xx_nand_select_chip;
	this->dev_ready		= pxa3xx_nand_dev_ready;
	this->cmdfunc		= pxa3xx_nand_cmdfunc;
	this->read_word		= pxa3xx_nand_read_word;
	this->read_byte		= pxa3xx_nand_read_byte;
	this->read_buf		= pxa3xx_nand_read_buf;
	this->write_buf		= pxa3xx_nand_write_buf;
	this->verify_buf	= pxa3xx_nand_verify_buf;
	this->erase_cmd		= pxa3xx_erase_cmd;
	this->errstat		= NULL;
	this->write_page	= NULL;

	this->ecc.mode		= NAND_ECC_HW;
	this->ecc.size		= f->page_size;
	this->ecc.read_page	= pxa3xx_nand_read_page_hwecc;
	this->ecc.write_page	= pxa3xx_nand_write_page_hwecc;

	this->chipsize 		= (uint64_t)f->num_blocks 	* \
				  f->page_per_block 		* \
				  f->page_size;

	this->chip_shift 	= ffs(this->chipsize) - 1;
	mtd->size 		= this->chipsize;

	/* Calculate the address shift from the page size */
	this->page_shift = ffs(mtd->writesize) - 1;
	this->pagemask = mtd_div_by_ws(this->chipsize, mtd) - 1;

	this->bbt_erase_shift = this->phys_erase_shift =
		ffs(mtd->erasesize) - 1;

	/* Set the bad block position */
	this->badblockpos = mtd->writesize > 512 ?
		NAND_LARGE_BADBLOCK_POS : NAND_SMALL_BADBLOCK_POS;

	/* Get chip options, preserve non chip based options */
	this->options &= ~NAND_CHIPOPTIONS_MSK;
	this->controller = &this->hwcontrol;
	spin_lock_init(&this->controller->lock);
	init_waitqueue_head(&this->controller->wq);

	/*
	 * Set chip as a default. Board drivers can override it, 
	 * if necessary
	 */
	this->options |= NAND_NO_AUTOINCR;
	this->options |= NAND_NO_READRDY;
	if (f->page_size == 2048)
		this->ecc.layout = &hw_largepage_ecclayout;
	else
		this->ecc.layout = &hw_smallpage_ecclayout;

	this->numchips = 1;
	this->chip_delay = 25;

	this->scan_bbt		= pxa3xx_scan_bbt;
	this->block_bad		= pxa3xx_block_bad;
	this->block_markbad	= bbm->markbad;
}

static int pxa3xx_nand_sensing(struct pxa3xx_nand_info *info, int cs)
{
	struct pxa3xx_nand *nand = info->nand_data;;
	const struct pxa3xx_nand_flash *f = &nand_common;
	struct mtd_info *mtd = nand->mtd[cs];

	nand->wait_mode = 1;
	pxa3xx_nand_config_flash(info, f);
	pxa3xx_nand_cmdfunc(mtd, NAND_CMD_RESET, 0, 0);
	nand->wait_mode = 0;

#ifdef CONFIG_WAYLAND
	return 1;
#else
	if (nand->is_ready)
		return 1;
	else
		return 0;
#endif
}

static int pxa3xx_nand_detect_flash(struct pxa3xx_nand *nand)
{
	struct pxa3xx_nand_flash *f;
	struct nand_chip *chip;
	struct pxa3xx_nand_info *info;
	struct mtd_info *mtd;
	uint32_t id = -1;
	int i, ret, chip_select;

	f = builtin_flash_types[0];
	chip_select = 0;
	for (; chip_select < NUM_CHIP_SELECT; chip_select ++) {
		mtd = nand->mtd[chip_select];
		chip = mtd->priv;
		info = mtd->priv;
		ret = pxa3xx_nand_sensing(info, chip_select);
		if (!ret) {
			kfree (nand->mtd[chip_select]);
			nand->mtd[chip_select] = NULL;
			continue;
		}

		pxa3xx_nand_config_flash(info, f);
		pxa3xx_nand_cmdfunc(mtd, NAND_CMD_READID, 0, 0);

		id = *((uint16_t *)(info->data_buff));

		if (id != 0)
			printk(KERN_INFO "Detect a flash id %x, cs %x\n",
					id, chip_select);
		else {
			kfree(mtd);
			nand->mtd[chip_select] = NULL;
			continue;
		}

		for (i = 1; i < ARRAY_SIZE(builtin_flash_types); i++) {

			f = builtin_flash_types[i];

			/* find the chip in default list */
			if (f->chip_id == id) {
				pxa3xx_nand_config_flash(info, f);
				chip->cellinfo = info->data_buff[2];
				mtd->writesize = f->page_size;
				mtd->oobsize = mtd->writesize / 32;
				mtd->erasesize = f->page_size * f->page_per_block;

				mtd->name = f->name;
				break;
			}
		}

		if (i == ARRAY_SIZE(builtin_flash_types)) {
			kfree(mtd);
			nand->mtd[chip_select] = NULL;
			printk(KERN_ERR "ERROR!! flash not defined!!!\n");
			continue;
		}
	}

	return 0;
}

/* the max buff size should be large than 
 * the largest size of page of NAND flash
 * that currently controller support
 */
#define MAX_BUFF_SIZE	((PAGE_CHUNK_SIZE + OOB_CHUNK_SIZE) * 2) + sizeof(struct pxa_dma_desc)

static struct pxa3xx_nand *alloc_nand_resource(struct platform_device *pdev,
						int use_dma)
{
	struct pxa3xx_nand_info		 *info;
	struct pxa3xx_nand 		 *nand;
	struct pxa3xx_bbm		 *bbm;
	struct mtd_info 		 *mtd;
	struct resource 		 *r;
	int data_desc_offset = MAX_BUFF_SIZE - sizeof(struct pxa_dma_desc);
	int ret, irq, i, chip_select;

	nand = kzalloc(sizeof(struct pxa3xx_nand), GFP_KERNEL);
	if (!nand) {
		dev_err(&pdev->dev, "failed to allocate memory\n");
		return NULL;
	}

	platform_set_drvdata(pdev, nand);
	nand->clk = clk_get(&pdev->dev, "NANDCLK");
	if (IS_ERR(nand->clk)) {
		dev_err(&pdev->dev, "failed to get nand clock\n");
		goto fail_end;
	}
	clk_enable(nand->clk);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(&pdev->dev, "no IRQ resource defined\n");
		goto fail_put_clk;
	}

	ret = request_irq(IRQ_PXA168_NAND, pxa3xx_nand_irq, IRQF_DISABLED,
			pdev->name, nand);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to request IRQ\n");
		goto fail_free_irq;
	}

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (r == NULL) {
		dev_err(&pdev->dev, "no IO memory resource defined\n");
		goto fail_free_irq;
	}

	r = request_mem_region(r->start, r->end - r->start + 1, pdev->name);
	if (r == NULL) {
		dev_err(&pdev->dev, "failed to request memory resource\n");
		goto fail_free_irq;
	}

	nand->mmio_base = ioremap(r->start, r->end - r->start + 1);
	if (nand->mmio_base == NULL) {
		dev_err(&pdev->dev, "ioremap() failed\n");
		goto fail_free_res;
	}

	for (chip_select = 0; chip_select < NUM_CHIP_SELECT; chip_select ++) {
		mtd = kzalloc(sizeof(struct mtd_info)		    \
				+ sizeof(struct pxa3xx_nand_info),  \
				GFP_KERNEL);

		if (!mtd) {
			dev_err(&pdev->dev, "failed to allocate memory\n");
			break;
		}

		info = (struct pxa3xx_nand_info *)(&mtd[1]);
		info->chip_select = chip_select;
		info->nand_data = nand;
		mtd->priv = info;
		nand->mtd[chip_select] = mtd;

		mtd->bbm = alloc_pxa3xx_bbm();
		bbm = mtd->bbm;
		if (!bbm)
			bbm->uninit(mtd);

		if (use_dma == 0) {

			info->data_buff = kmalloc(MAX_BUFF_SIZE, GFP_KERNEL);
			if (info->data_buff == NULL) {
				break;
			}
		}
		else {
			info->data_buff = dma_alloc_coherent(&pdev->dev,    \
					MAX_BUFF_SIZE,			    \
					&info->data_buff_phys, 		    \
					GFP_KERNEL);

			if (info->data_buff == NULL) {
				dev_err(&pdev->dev, "failed to allocate dma \
						buffer\n");

				break;
			}

			info->data_desc = (void *)info->data_buff   \
					  + data_desc_offset;
			r = platform_get_resource(pdev, IORESOURCE_DMA, 0);
			if (r == NULL) {
				dev_err(&pdev->dev, "no resource defined    \
						for data DMA\n");

				goto fail_free_buf;
			}
			nand->drcmr_dat = r->start;

			r = platform_get_resource(pdev, IORESOURCE_DMA, 1);
			if (r == NULL) {
				dev_err(&pdev->dev, "no resource defined    \
						for command DMA\n");

				goto fail_free_buf;
			}
			nand->drcmr_cmd = r->start;
			info->data_desc_addr = info->data_buff_phys	    \
					       + data_desc_offset;

			nand->data_buff_size = MAX_BUFF_SIZE;
			nand->data_dma_ch = pxa_request_dma("nand-data",    \
					DMA_PRIO_LOW,			    \
					pxa3xx_nand_data_dma_irq, nand);

			if (nand->data_dma_ch < 0) {
				dev_err(&pdev->dev, "failed to request data dma\n");
				goto fail_free_dma;
			}
		}
	}

	return nand;
fail_free_dma:
	if (use_dma)
		pxa_free_dma(nand->data_dma_ch);
fail_free_buf:
	for (i = 0; i < NUM_CHIP_SELECT; i ++) {
		mtd = nand->mtd[i];
		info = mtd->priv;

		if (info->data_buff) {
			if (use_dma)
				dma_free_coherent(&pdev->dev, 		\
						nand->data_buff_size,	\
						info->data_buff, 	\
						info->data_buff_phys);
			else
				kfree(info->data_buff);
		}

		if (mtd)
			kfree(mtd);
	}

	iounmap(nand->mmio_base);
fail_free_res:
	release_mem_region(r->start, r->end - r->start + 1);
fail_free_irq:
	free_irq(irq, nand);
fail_put_clk:
	clk_disable(nand->clk);
	clk_put(nand->clk);
fail_end:
	kfree(nand);
	return NULL;
}

static int pxa3xx_nand_scan(struct mtd_info *mtd)
{
	return nand_scan_tail(mtd);
}

static int pxa3xx_nand_probe(struct platform_device *pdev)
{
	struct pxa3xx_nand_platform_data *pdata;
	struct pxa3xx_nand 		 *nand;
	struct pxa3xx_bbm		 *bbm;
	struct mtd_info 		 *mtd;
	int    i;

	pdata = pdev->dev.platform_data;
	if (!pdata) {
		dev_err(&pdev->dev, "no platform data defined\n");
		return -ENODEV;
	}

	nand = alloc_nand_resource(pdev, pdata->use_dma);
	if (!nand)
		return -ENODEV;

	nand->enable_arbiter 	= pdata->enable_arbiter;
	nand->use_dma 		= pdata->use_dma;
	nand->RD_CNT_DEL	= pdata->RD_CNT_DEL;

	pxa3xx_nand_detect_flash(nand);

	for (i = 0; i < NUM_CHIP_SELECT; i ++) {
		mtd = nand->mtd[i];

		if (mtd) {
			bbm = mtd->bbm;
			pxa3xx_nand_init_mtd(mtd);
			if (pxa3xx_nand_scan(mtd)) {
				dev_err(&pdev->dev, "failed to scan nand\n");
			}

			printk("reloc table on cs %x:\n", i);
			bbm->show(mtd);
			printk("\n");

			add_mtd_partitions(mtd, pdata->parts[i],    \
					pdata->nr_parts[i]);
		}
	}


	return 0;

}

static int pxa3xx_nand_remove(struct platform_device *pdev)
{
	struct pxa3xx_nand *nand = platform_get_drvdata(pdev);
	struct mtd_info *mtd = nand->mtd[nand->chip_select];
	struct pxa3xx_nand_info *info = mtd->priv;
	struct pxa3xx_bbm *pxa3xx_bbm = mtd->bbm;
	int i;

	pxa3xx_nand_stop(nand);
	platform_set_drvdata(pdev, NULL);
	free_irq(IRQ_PXA168_NAND, nand);
	if (nand->use_dma) {
		pxa_free_dma(nand->data_dma_ch);
		dma_free_writecombine(&pdev->dev, nand->data_buff_size,
				info->data_buff, info->data_buff_phys);
	} else
		kfree(info->data_buff);

	for (i = 0; i < NUM_CHIP_SELECT; i ++) {
		del_mtd_device(nand->mtd[i]);
		del_mtd_partitions(nand->mtd[i]);
		kfree(nand->mtd[i]);
	}

	pxa3xx_bbm->uninit(mtd);

	return 0;
}

#ifdef CONFIG_PM
static unsigned int ndtr0cs0, ndtr1cs0;

static int pxa3xx_nand_suspend(struct platform_device *pdev,
		pm_message_t state)
{
	struct pxa3xx_nand *nand = platform_get_drvdata(pdev);

	if (nand->state) {
		dev_err(&pdev->dev, "driver busy, state = %d\n", nand->state);
		return -EAGAIN;
	}

	ndtr0cs0 = nand_readl(nand, NDTR0CS0);
	ndtr1cs0 = nand_readl(nand, NDTR1CS0);

	return 0;
}

static int pxa3xx_nand_resume(struct platform_device *pdev)
{
	struct pxa3xx_nand *nand = platform_get_drvdata(pdev);

	nand_writel(nand, NDTR0CS0, ndtr0cs0);
	nand_writel(nand, NDTR1CS0, ndtr1cs0);
	return 0;
}
#else
#define pxa3xx_nand_suspend	NULL
#define pxa3xx_nand_resume	NULL
#endif

static struct platform_driver pxa3xx_nand_driver = {
	.driver = {
		.name	= "pxa3xx-nand",
	},
	.probe		= pxa3xx_nand_probe,
	.remove		= pxa3xx_nand_remove,
	.suspend	= pxa3xx_nand_suspend,
	.resume		= pxa3xx_nand_resume,
};

static int __init pxa3xx_nand_init(void)
{
	return platform_driver_register(&pxa3xx_nand_driver);
}
module_init(pxa3xx_nand_init);

static void __exit pxa3xx_nand_exit(void)
{
	platform_driver_unregister(&pxa3xx_nand_driver);
}
module_exit(pxa3xx_nand_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PXA3xx NAND controller driver");

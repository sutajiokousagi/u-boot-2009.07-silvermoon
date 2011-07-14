/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __SDMMC_H
#define __SDMMC_H

/* MMC4 Register definitions */
#define MMC4_1_REG_BASE_ADDR	0x55000000
#define MMC4_2_REG_BASE_ADDR	0x55100000
#define MMC4_3_REG_BASE_ADDR	0x55200000
#define MMC_REG_BASE_ADDR	MMC4_1_REG_BASE_ADDR
#define MMC4_MAX_SPEED		48000000

#define MMC4_SYSADDR	0x0000	/* MMC4 DMA system address */
#define MMC4_BLK_CNTL	0x0004	/* MMC4 block size control register */
#define MMC4_ARG 	0x0008	/* MMC4 command argument */
#define MMC4_CMD_XFRMD	0x000c	/* MMC4 command and transfer mode */
#define MMC4_RESP0	0x0010	/* MMC4 command response 0 */
#define MMC4_RESP1	0x0014	/* MMC4 command response 1 */
#define MMC4_RESP2	0x0018	/* MMC4 command response 2 */
#define MMC4_RESP3	0x001c	/* MMC4 command response 3 */
#define MMC4_DP		0x0020	/* MMC4 buffer data port */
#define MMC4_STATE	0x0024	/* MMC4 present state */
#define MMC4_CNTL1	0x0028	/* MMC4 host control 1 */
#define MMC4_CNTL2	0x002c	/* MMC4 host control 2 */
#define MMC4_I_STAT	0x0030	/* MMC4 interrupt status */
#define MMC4_I_STAT_EN	0x0034	/* MMC4 interrupt status enable */
#define MMC4_I_SIG_EN	0x0038	/* MMC4 interrupt signal enable */
#define MMC4_ACMD12_ER	0x003c	/* MMC4 auto CMD12 error status */
#define MMC4_CAP0	0x0040	/* MMC4 capabilities 0 */
#define MMC4_CUR_CAP0	0x0048	/* MMC4 maximum current capabilities 0 */
#define MMC4_AMDA_ERSTS	0x0054	/* MMC4 ADMA Error Status */
#define MMC4_AMDA_ADDR1	0x0058	/* MMC4 ADMA Address[31:0] */
#define MMC4_AMDA_ADDR2	0x005C	/* MMC4 ADMA Address[63:32] */
#define MMC4_VER	0x00FC	/* MMC4 controller version status */

/* MMC4_BLK_CNTL */
#define BLK_CNT_OFFSET		16
#define DMA_BUFSZ_OFFSET	12
#define BLK_CNT_MASK		0xffff0000
#define XFR_BLKSZ_MASK		0x00000fff

/* MMC4_CMD_XFRMD */
#define CMD_IDX_OFFSET		24
#define CMD_TYPE_OFFSET		22
#define DPSEL			(1 << 21)
#define IDXCHKEN		(1 << 20)
#define CRCCHKEN		(1 << 19)
#define RES_TYPE_OFFSET		16
#define MS_BLKSEL		(1 << 5)
#define DXFRDIR			(1 << 4)
#define AUTOCMD12		(1 << 2)
#define BLKCNTEN		(1 << 1)
#define DMA_EN			(1 << 0)
/* RES_TYPE */
#define CMD_RSP_NONE	(0x0)
#define CMD_RSP_136BIT	(0x1)
#define CMD_RSP_48BIT	(0x2)
#define CMD_RSP_48BITB 	(0x3)

/* MMC4_STATE */
#define RDACTV		(9 << 0)
#define WRACTV		(9 << 0)
#define DCMDINHBT	(1 << 1)
#define CCMDINHBT	(1 << 0)

/* MMC4_CNTL1 */
#define BGIRQEN		(1 << 19)
#define RDWTCNTL	(1 << 18)
#define CONTREQ		(1 << 17)
#define BGREQSTP	(1 << 16)
#define VLTGSEL_OFFSET	9
#define BUSPWR		(1 << 8)
#define _8BITMD		(1 << 5)
#define DMA_SEL_OFFSET	3
#define DMA_SEL_MASK	(0x3 << DMA_SEL_OFFSET)
#define DMA_SEL_SDMA	(0x0 << DMA_SEL_OFFSET)
#define HISPEED		(1 << 2)
#define _4BITMD		(1 << 1)

/* MMC4_CNTL2 */
#define DATSWRST		(1 << 26)
#define CMDSWRST		(1 << 25)
#define MSWRST			(1 << 24)
#define DTOCNTR_OFFSET		16
#define SDFREQ_OFFSET		8
#define SDFREQ_MASK		(0xff << 8)
#define MMC4CLKEN		(1 << 2)
#define INTCLKSTB		(1 << 1)
#define INTCLKEN		(1 << 0)

/* MMC4_I_STAT, MMC4_I_STAT_EN, MMC4_I_SIG_EN */
#define STAT_EN_MASK	0x33ff00ff
#define AHBT		(1 << 28)
#define ADMA		(1 << 25)	/* AMDA */
#define AC12		(1 << 24)	/* Auto CMD12 */
#define ILMT		(1 << 23)
#define DEND		(1 << 22)	/* Data End Bit */
#define DCRC		(1 << 21)	/* Data CRC */
#define DTO		(1 << 20)	/* Data Timeout */
#define CIDX		(1 << 19)	/* Command Index */
#define CEND		(1 << 18)	/* Command End Bit */
#define CCRC		(1 << 17)	/* Command CRC */
#define CTO		(1 << 16)	/* Command Timeout */
#define ERRINT		(1 << 15)
#define CDINT		(1 << 8)	/* Card Interrupt */
#define CDREM		(1 << 7)
#define CDINS		(1 << 6)
#define BUFRDRDY	(1 << 5)	/* Buffer Read Ready */
#define BUFWRRDY	(1 << 4)	/* Buffer Write Ready */
#define DMAINT		(1 << 3)	/* DMA Interrupt */
#define BGEVNT		(1 << 2)	/* Block Gap Event */
#define XFRCOMP		(1 << 1)	/* Transfer Complete */
#define CMDCOMP		(1 << 0)	/* Command Complete */

inline char* mmc_get_cmdname(unsigned int opcode) {
	switch (opcode) {
		case 0:
			return "GO_IDLE";
		case 1:
			return "SEND_OP_COND";
		case 2:
			return "ALL_SEND_CID";
		case 3:
			return "SET_RELATIVE_ADDRESS";
		case 6:
			return "SWITCH";
		case 7:
			return "SELECT_DESELECT_CARD";
		case 8:
			return "SEND_EXT_CSD";
		case 9:
			return "SEND_CSD";
		case 12:
			return "STOP_TRANSMISSION";
		case 13:
			return "SEND_STATUS";
		case 16:
			return "SET_BLOCKLEN";
		case 14:
			return "BUSTEST_R";
		case 17:
			return "READ_SINGLE_BLOCK";
		case 18:
			return "READ_MULTI_BLOCK";
		case 19:
			return "BUSTEST_W";
		case 24:
			return "WRITE_SINGLE_BLOCK";
		case 25:
			return "WRITE_MULTI_BLOCK";
		case 55:
			return "APP_CMD";
		default:
			return "Unknow CMD";
	}
}

#endif /* __SDMMC_H */

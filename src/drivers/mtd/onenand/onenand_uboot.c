/*
 *  drivers/mtd/onenand/onenand_uboot.c
 *
 *  Copyright (C) 2005-2008 Samsung Electronics
 *  Kyungmin Park <kyungmin.park@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * OneNAND initialization at U-Boot
 */

#include <common.h>
#include <linux/mtd/compat.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/onenand.h>
#include <asm/arch-pxa168/pxa3xx_bbm.h>

struct mtd_info onenand_mtd;
struct onenand_chip onenand_chip;

int onenand_no_bbt(struct mtd_info *mtd)
{
	return 0;
}

void onenand_init(void)
{
	struct pxa3xx_bbm *bbm;

	memset(&onenand_mtd, 0, sizeof(struct mtd_info));
	memset(&onenand_chip, 0, sizeof(struct onenand_chip));

	bbm = alloc_pxa3xx_bbm();
	onenand_mtd.bbm = bbm;

	onenand_chip.base = (void *) CONFIG_SYS_ONENAND_BASE;
	onenand_chip.scan_bbt = onenand_no_bbt;
	onenand_chip.block_markbad = bbm->markbad;
	onenand_mtd.priv = &onenand_chip;

	onenand_scan(&onenand_mtd, 1);

	puts("OneNAND: ");
	print_size(onenand_mtd.size, "\n");
	if (onenand_mtd.size > 0) {
		printf("reloc table on ONENAND:\n");
		bbm->show(&onenand_mtd);
	}
}

int onenand_dump(struct mtd_info *mtd, ulong off)
{
	int i;
	u_char *buf, *p;
	int writesize = mtd->writesize;
	int oobsize = mtd->oobsize;
	struct mtd_oob_ops ops;

	off &= ~(writesize - 1);
	buf = malloc(writesize + oobsize);
	if (!buf) {
		puts("No memory for page buffer\n");
		return 1;
	}

	ops.datbuf 	= (uint8_t *)buf;
	ops.len		= mtd->writesize;
	ops.mode 	= MTD_OOB_PLACE;
	ops.oobbuf 	= (uint8_t *)(buf + mtd->writesize);
	ops.ooblen 	= mtd->oobsize;
	ops.ooboffs 	= off & (mtd->oobsize - 1);

	i = onenand_read_oob(mtd, off & ~(mtd->oobsize - 1), &ops);

	if (i < 0) {
		printf("Error (%d) reading page %08lx\n", i, off);
		free(buf);
		return 1;
	}
	
	printf("Page %08lx dump:\n", off);
	i = mtd->writesize >> 4; p = buf;
	
	while (i--) {
		printf( "\t%02x %02x %02x %02x %02x %02x %02x %02x"
				"  %02x %02x %02x %02x %02x %02x %02x %02x\n",
				p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
				p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
		p += 16;
	}

	puts("OOB:\n");
	i = mtd->oobsize >> 3;
	
	while (i--) {
		printf( "\t%02x %02x %02x %02x %02x %02x %02x %02x\n",
				p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
		p += 8;
	}

	free(buf);
}

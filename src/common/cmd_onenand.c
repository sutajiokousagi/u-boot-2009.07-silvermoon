/*
 *  U-Boot command for OneNAND support
 *
 *  Copyright (C) 2005-2007 Samsung Electronics
 *  Kyungmin Park <kyungmin.park@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <common.h>
#include <command.h>

#include <linux/mtd/compat.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/onenand.h>
#include <jffs2/jffs2.h>

#include <asm/io.h>

extern struct mtd_info onenand_mtd;
extern struct onenand_chip onenand_chip;

int do_onenand(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int ret = 0;

	switch (argc) {
	case 0:
	case 1:
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;

	case 2:
		if (strncmp(argv[1], "open", 4) == 0) {
			onenand_init();
			return 0;
		}
		printf("%s\n", onenand_mtd.name);
		return 0;

	default:
		/* At least 4 args */
		if (strncmp(argv[1], "erase", 5) == 0) {
			struct erase_info instr = {
				.callback	= NULL,
			};
			ulong start, size;
			ulong block;
			char *endtail;
			int clean = 0;
			struct jffs2_unknown_node cleanmarker;
			struct mtd_oob_ops ops = {
				.datbuf = NULL,
				.oobbuf = (uint8_t *)&cleanmarker,
				.ooblen = 8,
				.ooboffs= 0,
				.mode	= MTD_OOB_PLACE,
			};

			if (strncmp(argv[2], "block", 5) == 0) {
				start = simple_strtoul(argv[3], NULL, 10);
				endtail = strchr(argv[3], '-');
				size = simple_strtoul(endtail + 1, NULL, 10) - start;
			}
		        else if (strncmp(argv[2], "clean", 5) == 0) {
				cleanmarker.magic = JFFS2_MAGIC_BITMASK;
				cleanmarker.nodetype = JFFS2_NODETYPE_CLEANMARKER;
				cleanmarker.totlen = 8;
				
				start = simple_strtoul(argv[3], NULL, 10);
				size = simple_strtoul(argv[4], NULL, 10);

				start >>= onenand_chip.erase_shift;
				size >>= onenand_chip.erase_shift;
				/* Don't include the end block */
				size--;

				clean = 1;
				printf("ready to erase with cleanmarker\n");
			} else {
				start = simple_strtoul(argv[2], NULL, 10);
				size = simple_strtoul(argv[3], NULL, 10);

				start >>= onenand_chip.erase_shift;
				size >>= onenand_chip.erase_shift;
				/* Don't include the end block */
				size--;
			}

			if (!size || size < 0)
				size = 0;

			printf("Erase begin with %luth block, erase size is %lu blocks\n", start, size);

			for (block = start; block <= start + size; block++) {
				instr.addr = block << onenand_chip.erase_shift;
				instr.len = 1 << onenand_chip.erase_shift;
				ret = onenand_erase(&onenand_mtd, &instr);
			
				if (ret) {
					printf("erase failed %lu\n", block);
					break;
				}
				
				if (clean == 1) {
					ret = onenand_write_oob(&onenand_mtd, instr.addr, &ops);
					if (ret) {
						printf("write OOB failed %lu\n", block);
						break;
					}
				}
			}

			return 0;
		}

		if (strncmp(argv[1], "dump", 4) == 0) {
			ulong ofs = simple_strtoul(argv[2], NULL, 16);

			onenand_dump(&onenand_mtd, ofs);
			printf("Done\n");

			return 0;
		}

		if (strncmp(argv[1], "read", 4) == 0) {
			ulong addr = simple_strtoul(argv[2], NULL, 16);
			ulong ofs = simple_strtoul(argv[3], NULL, 16);
			size_t len = simple_strtoul(argv[4], NULL, 16);
			int oob = strncmp(argv[1], "read.oob", 8) ? 0 : 1;
			struct mtd_oob_ops ops;

			ops.mode = MTD_OOB_PLACE;

			if (oob) {
				ops.len = 0;
				ops.datbuf = NULL;
				ops.ooblen = len;
				ops.oobbuf = (u_char *) addr;
			} else {
				ops.len = len;
				ops.datbuf = (u_char *) addr;
				ops.ooblen = 0;
				ops.oobbuf = NULL;
			}
			ops.retlen = ops.oobretlen = 0;

			onenand_mtd.read_oob(&onenand_mtd, ofs, &ops);
			printf("Done\n");

			return 0;
		}

		if (strncmp(argv[1], "write", 5) == 0) {
			ulong addr = simple_strtoul(argv[2], NULL, 16);
			ulong ofs = simple_strtoul(argv[3], NULL, 16);
			size_t len = simple_strtoul(argv[4], NULL, 16);
			size_t retlen = 0;
			struct mtd_oob_ops ops = {
				.datbuf = (uint8_t *) addr,
				.len    = len,
				.oobbuf = (uint8_t *) addr,
				.ooblen = len,
				.mode	= MTD_OOB_PLACE,
			};

			onenand_mtd.owner = "UBOOT";
			if (strncmp(argv[1], "write.yaffs", 11) == 0) {
				onenand_write_oob(&onenand_mtd, ofs, &ops);
			}
			else
				onenand_write(&onenand_mtd, ofs, len, &retlen,
						(u_char *) addr);
			printf("Done\n");

			return 0;
		}

		if (strncmp(argv[1], "block", 5) == 0) {
			ulong addr = simple_strtoul(argv[2], NULL, 16);
			ulong block = simple_strtoul(argv[3], NULL, 10);
			ulong page = simple_strtoul(argv[4], NULL, 10);
			size_t len = simple_strtol(argv[5], NULL, 10);
			ulong ofs;
			int oob = strncmp(argv[1], "block.oob", 9) ? 0 : 1;
			struct mtd_oob_ops ops;

			ops.mode = MTD_OOB_PLACE;


			ofs = block << onenand_chip.erase_shift;
			if (page)
				ofs += page << onenand_chip.page_shift;

			if (!len) {
				if (oob)
					ops.ooblen = 64;
				else
					ops.len = 512;
			}

			if (oob) {
				ops.datbuf = NULL;
				ops.oobbuf = (u_char *) addr;
			} else {
				ops.datbuf = (u_char *) addr;
				ops.oobbuf = NULL;
			}
			ops.retlen = ops.oobretlen = 0;

			onenand_read_oob(&onenand_mtd, ofs, &ops);
			return 0;
		}

		break;
	}

	return 0;
}

U_BOOT_CMD(
	onenand,	6,	1,	do_onenand,
	"onenand - OneNAND sub-system\n",
	"info   - show available OneNAND devices\n"
	"onenand read[.oob] addr ofs len - read data at ofs with len to addr\n"
	"onenand dump addr - dump the specified page's content\n"
	"onenand write[.yaffs] addr ofs len - write data at ofs with len from addr\n"
	"onenand erase saddr eaddr - erase block start addr to end addr\n"
	"onenand block[.oob] addr block [page] [len] - "
		"read data with (block [, page]) to addr"
);

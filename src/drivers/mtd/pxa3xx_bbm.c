/*
 * linux/drivers/mtd/pxa3xx_bbm.c
 *
 * Bad Block Table support for PXA3xx.
 * Copyright (C) 2007 Marvell International Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/mtd/mtd.h>
#include <asm/arch-pxa168/pxa3xx_bbm.h>
#include <asm/errno.h>
#include <common.h>

void dump_reloc_table(struct pxa3xx_bbm *bbm)
{
	int i;

	if (bbm->table->total == 0)
		printk(KERN_INFO "The reloc table is empty now\n");
	else
		for (i = 0; i < bbm->table->total; i++) {
			printk(KERN_INFO "block: %8d is relocated to block: %d\n",
					bbm->reloc[i].from, bbm->reloc[i].to);
		}
}

static int pxa3xx_init_reloc_tb(struct mtd_info *mtd)
{
	struct pxa3xx_bbm *bbm = mtd->bbm;
	int size = mtd->writesize + mtd->oobsize;
	int slots, pages;

	bbm->page_shift = ffs(mtd->writesize) - 1;
	bbm->erase_shift = ffs(mtd->erasesize) - 1;

	pages = mtd->erasesize >> bbm->page_shift;
	bbm->reloc_begin = mtd_div_by_eb(mtd->size, mtd);
	slots = (bbm->reloc_begin * 2) / 100;
	slots = (slots < PXA_MAX_RELOC_ENTRY) ? slots : PXA_MAX_RELOC_ENTRY;

	bbm->max_slots		= pages;
	bbm->max_reloc_entry	= slots;
	bbm->reloc_begin	-= bbm->max_reloc_entry;
	bbm->table_init		= 0;

	bbm->data_buf = kzalloc(size, GFP_KERNEL);
	if (!bbm->data_buf) {
		return -ENOMEM;
	}

	bbm->table = (struct reloc_table *)bbm->data_buf;
	memset(bbm->table, 0x0, sizeof(struct reloc_table));

	bbm->reloc = (struct reloc_item *)((uint8_t *)bbm->data_buf +
			sizeof (struct reloc_item));
	memset(bbm->reloc, 0x0,
			sizeof(struct reloc_item) * bbm->max_reloc_entry);

	return 0;
}

static void pxa3xx_uninit_reloc_tb(struct mtd_info *mtd)
{
	struct pxa3xx_bbm *bbm = mtd->bbm;

	if (bbm) {
		kfree(bbm->data_buf);
		kfree(bbm);
		bbm = NULL;
	}
}

/* add the relocation entry into the relocation table
 * It's valid on MOBM V3.
 * If the relocated block is bad, an new entry will be added into the
 * bottom of the relocation table.
 */
int update_reloc_tb(struct mtd_info *mtd, int block)
{
	struct pxa3xx_bbm *bbm = mtd->bbm;
	struct reloc_table *table = bbm->table;
	struct reloc_item *item = bbm->reloc;
	struct erase_info instr;
	int reloc_block, ret, entry_num = -1;
	char rel_dist[PXA_MAX_RELOC_ENTRY];
	int i;

	if (table->total > bbm->max_reloc_entry) {
		printk(KERN_WARNING "Relocation table exceed max num,");
		printk(KERN_WARNING "cannot relocate block 0x%x\n", block);
		return -ENOSPC;
	}

	//identify whether the block has been relocated
	for(i = table->total - 1; i >= 0; i --) {
		if(block == item[i].from)
			entry_num = i;
	}

	memset(rel_dist, 0, PXA_MAX_RELOC_ENTRY);
	//find the available block with the largest number in reservered area
	for (i = 0; i < table->total; i ++) {
		int _rel = (item[i].to != 65535) ? item[i].to : item[i].from;

		rel_dist[_rel - bbm->reloc_begin] = 1;
	}

	while (1) {
		/* Make sure that reloc_block is pointing to a valid block */
		for (reloc_block = bbm->max_reloc_entry - 1; reloc_block >= 0; reloc_block --) {
			if (rel_dist[reloc_block] == 0) {
				break;
			}
		}

		reloc_block = reloc_block + bbm->reloc_begin;
		memset(&instr, 0, sizeof(struct erase_info));
		instr.mtd = mtd;
		instr.addr = (unsigned long long)reloc_block << bbm->erase_shift;
		instr.len = (1 << bbm->erase_shift);

		ret = mtd->erase(mtd, &instr);
		if (ret == 0)
			break;
		else {
			/* skip it if the reloc_block is also a 
			 * bad block
			 */
			if (instr.fail_addr == instr.addr) {
				item[table->total].from = reloc_block;

				item[table->total].to = 65535;
				table->total++;
				rel_dist[reloc_block - bbm->reloc_begin] = 1;;
				continue;
			} else
				return -EINVAL;
		}
	}

	/* Create the relocated block information in the table */
	//when the block is relocated before, blob should modify the original entry to new
	//relocated block and the old relocated block point to 65535. If not the situation, 
	//create a new entry
	if (entry_num != -1) {
		item[table->total].from = item[entry_num].to;
		item[table->total].to = 65535;
		table->total++;
		item[entry_num].to = reloc_block;
	} else {
		item[table->total].from = block;
		item[table->total].to = reloc_block;
		table->total++;
	}

	return 0;
}

/* Write the relocation table back to device, if there's room. */
static int sync_reloc_tb(struct mtd_info *mtd, int *idx)
{
	struct pxa3xx_bbm *bbm = mtd->bbm;
	uint8_t *tmp;
	size_t retlen;
	int len;

	if (*idx < PXA_BEGIN_SLOT) {
		printk(KERN_ERR "Can't write relocation table to device \
				any more.\n");
		return -1;
	}

	if (*idx >= bbm->max_slots) {
		printk(KERN_ERR "Wrong Slot is specified.\n");
		return -1;
	}

	/* should write to the next slot*/
	(*idx) --;

	len = 4;
	len += bbm->table->total << 2;

	tmp = (uint8_t *)bbm->data_buf;
	mtd->write(mtd, (*idx) << bbm->page_shift,
			1 << bbm->page_shift, &retlen, tmp);

	return 0;
}

static int pxa3xx_scan_reloc_tb(struct mtd_info *mtd)
{
	struct pxa3xx_bbm *bbm = mtd->bbm;
	struct reloc_table *table;
	int page, valid = 0;
	size_t retlen;
	int ret;

	if (!(bbm->table_init))
		pxa3xx_init_reloc_tb(mtd);

	table = bbm->table;

	for (page = PXA_BEGIN_SLOT; page < bbm->max_slots; page ++) {
		memset(bbm->data_buf, 0,
				mtd->writesize + mtd->oobsize);
		ret = mtd->read(mtd, (page << bbm->page_shift),
				mtd->writesize, &retlen, bbm->data_buf);

		if (ret == 0) {
			if (table->header != PXA_RELOC_HEADER) {
				continue;
			} else {
				bbm->current_slot = page;	
				valid = 1;
				break;
			}
		}
	}

	if (valid) {
		printk(KERN_INFO "relocation table at page:%d\n", bbm->current_slot);
		bbm->table_init = 1;
	} else {
		/* There should be a valid relocation table slot at least. */
		printk(KERN_ERR "NO VALID reloc table can be recognized\n");
		printk(KERN_ERR "CAUTION: It may cause unpredicated error\n");
		printk(KERN_ERR "Please re-initialize the flash.\n");
		memset((unsigned char *)bbm->table, 0,
				sizeof(struct reloc_table));
		bbm->table_init = 0;
		return -EINVAL;
	}

	return 0;
}

static void show_bbm(struct mtd_info *mtd)
{
	struct pxa3xx_bbm *bbm = mtd->bbm;

	if (pxa3xx_scan_reloc_tb(mtd) == 0) {
		dump_reloc_table(bbm);
	}
}

/* Find the relocated block of the bad one.
 * If it's a good block, return 0. Otherwise, return a relocated one.
 * idx points to the next relocation entry
 * If the relocated block is bad, an new entry will be added into the
 * bottom of the relocation table.
 */
static loff_t pxa3xx_search_reloc_tb(struct mtd_info *mtd, loff_t ofs)
{
	struct pxa3xx_bbm *bbm = mtd->bbm;
	struct reloc_table *table = bbm->table;
	struct reloc_item *item = bbm->reloc;
	int i, max, block, max_allow_relocated;

	block = ofs >> bbm->erase_shift;
	max_allow_relocated = mtd_div_by_eb(mtd->size, mtd)
				- bbm->max_reloc_entry;

	if ((bbm->table_init == 0)
			|| (table->total == 0)
			|| (block >= max_allow_relocated))

		return ofs;

	max = table->total;
	ofs -= block * mtd->erasesize;

	for (i = max-1; i >= 0; i--) {
		if (block == item[i].from) {
			block = item[i].to;
			break;
		}
	}

	ofs += block * mtd->erasesize;

	return ofs;
}

static int pxa3xx_mark_reloc_tb(struct mtd_info *mtd, loff_t ofs)
{
	struct pxa3xx_bbm *bbm = mtd->bbm;
	struct mtd_oob_ops ops;
	uint32_t bad_mark = 0xFFFFFF00;
	int ret = 0, block, _ofs;

	_ofs = bbm->search(mtd, ofs);
	if (_ofs < 0 || _ofs > mtd->size)
		return 0;

	ops.mode	= MTD_OOB_PLACE;
	ops.len		= mtd->oobsize;
	ops.ooblen	= 2;
	ops.ooboffs	= 0;
	ops.oobbuf	= &bad_mark;
	ops.datbuf	= NULL;

	mtd->write_oob(mtd, ofs, &ops);

	if (bbm->table_init) {
		block = ofs >> bbm->erase_shift;
		ret = update_reloc_tb(mtd, block);
		if (ret)
			return ret;

		sync_reloc_tb(mtd, &(bbm->current_slot));
	}

	return 0;
}

struct pxa3xx_bbm* alloc_pxa3xx_bbm(void)
{
	/* FIXME: We don't want to add module_init entry
	 * here to avoid dependency issue.
	 */
	struct pxa3xx_bbm *bbm;

	bbm = kzalloc(sizeof(struct pxa3xx_bbm), GFP_KERNEL);
	if (!bbm)
		return NULL;

	bbm->uninit	= pxa3xx_uninit_reloc_tb;
	bbm->search	= pxa3xx_search_reloc_tb;
	bbm->markbad	= pxa3xx_mark_reloc_tb;
	bbm->scan_bbt	= pxa3xx_scan_reloc_tb;
	bbm->show	= show_bbm;

	return bbm;
}
EXPORT_SYMBOL(alloc_pxa3xx_bbm);

/*
 * linux/arch/arm/mach-pxa/mfp.c
 *
 * PXA3xx Multi-Function Pin Support
 *
 * Copyright (C) 2007 Marvell Internation Ltd.
 *
 * 2007-08-21: eric miao <eric.miao@marvell.com>
 *             initial version
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */
#if 0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#endif

#include <asm/hardware.h>
#include <asm/arch/common.h>
#include <asm/arch/mfp.h>

static void __iomem *mfpr_mmio_base;
static struct pxa3xx_mfp_pin mfp_table[MFP_PIN_MAX];

#define mfpr_readl(off)			\
	__raw_readl(mfpr_mmio_base + (off))

#define mfpr_writel(off, val)		\
	__raw_writel(val, mfpr_mmio_base + (off))

/*
 * perform a read-back of any MFPR register to make sure the
 * previous writings are finished
 */
#define mfpr_sync()	(void)__raw_readl(mfpr_mmio_base + 0x0204)

static inline void __mfp_config(int pin, unsigned long val)
{
	unsigned long off = mfp_table[pin].mfpr_off;

	mfp_table[pin].mfpr_val = val;
	mfpr_writel(off, val);
}

void pxa3xx_mfp_config(mfp_cfg_t *mfp_cfgs, int num)
{
	int i, pin;
	unsigned long val;
	mfp_cfg_t *mfp_cfg = mfp_cfgs;

	for (i = 0; i < num; i++, mfp_cfg++) {
		pin = MFP_CFG_PIN(*mfp_cfg);
		val = MFP_CFG_VAL(*mfp_cfg);

		BUG_ON(pin >= MFP_PIN_MAX);

#if !defined(CONFIG_ASPENITE)
		if (cpu_is_pxa935() || cpu_is_pxa910()) {
			/* pxa935 only, shift the Drive Strength 
			 * from bit[11:10] to bit[12:11] */
			int tmp;
			tmp = (val & MFPR_DRV_MASK) << 1;
			val = (val & ~MFPR_DRV_MASK) | tmp;
		}
		if (cpu_is_pxa910())
			val |= 0x80;
#endif
		__mfp_config(pin, val);
	}

	mfpr_sync();
}

unsigned long pxa3xx_mfp_read(int mfp)
{
	unsigned long val;

	BUG_ON(mfp >= MFP_PIN_MAX);
	val = mfpr_readl(mfp_table[mfp].mfpr_off);
	return val;
}

void pxa3xx_mfp_write(int mfp, unsigned long val)
{

	BUG_ON(mfp >= MFP_PIN_MAX);
	mfpr_writel(mfp_table[mfp].mfpr_off, val);
	mfpr_sync();
}

void pxa3xx_mfp_set_afds(int mfp, int af, int ds)
{
	uint32_t mfpr_off, mfpr_val;

	BUG_ON(mfp >= MFP_PIN_MAX);
	mfpr_off = mfp_table[mfp].mfpr_off;

	mfpr_val = mfpr_readl(mfpr_off);
	mfpr_val &= ~(MFPR_AF_MASK | MFPR_DRV_MASK);
	mfpr_val |= (((af & 0x7) << MFPR_ALT_OFFSET) |
		     ((ds & 0x7) << MFPR_DRV_OFFSET));

	mfpr_writel(mfpr_off, mfpr_val);
	mfpr_sync();
}

void pxa3xx_mfp_set_rdh(int mfp, int rdh)
{
	uint32_t mfpr_off, mfpr_val;

	BUG_ON(mfp >= MFP_PIN_MAX);

	mfpr_off = mfp_table[mfp].mfpr_off;

	mfpr_val = mfpr_readl(mfpr_off);
	mfpr_val &= ~MFPR_RDH_MASK;

	if (likely(rdh))
		mfpr_val |= (1u << MFPR_SS_OFFSET);

	mfpr_writel(mfpr_off, mfpr_val);
	mfpr_sync();
}

void pxa3xx_mfp_set_lpm(int mfp, int lpm)
{
	uint32_t mfpr_off, mfpr_val;

	BUG_ON(mfp >= MFP_PIN_MAX);

	mfpr_off = mfp_table[mfp].mfpr_off;
	mfpr_val = mfpr_readl(mfpr_off);
	mfpr_val &= ~MFPR_LPM_MASK;

	if (lpm & 0x1) mfpr_val |= 1u << MFPR_SON_OFFSET;
	if (lpm & 0x2) mfpr_val |= 1u << MFPR_SD_OFFSET;
	if (lpm & 0x4) mfpr_val |= 1u << MFPR_PU_OFFSET;
	if (lpm & 0x8) mfpr_val |= 1u << MFPR_PD_OFFSET;
	if (lpm &0x10) mfpr_val |= 1u << MFPR_PS_OFFSET;

	mfpr_writel(mfpr_off, mfpr_val);
	mfpr_sync();
}

void pxa3xx_mfp_set_pull(int mfp, int pull)
{
	uint32_t mfpr_off, mfpr_val;

	BUG_ON(mfp >= MFP_PIN_MAX);

	mfpr_off = mfp_table[mfp].mfpr_off;
	mfpr_val = mfpr_readl(mfpr_off);
	mfpr_val &= ~MFPR_PULL_MASK;
	mfpr_val |= ((pull & 0x7u) << MFPR_PD_OFFSET);

	mfpr_writel(mfpr_off, mfpr_val);
	mfpr_sync();
}

void pxa3xx_mfp_set_edge(int mfp, int edge)
{
	uint32_t mfpr_off, mfpr_val;

	BUG_ON(mfp >= MFP_PIN_MAX);

	mfpr_off = mfp_table[mfp].mfpr_off;
	mfpr_val = mfpr_readl(mfpr_off);

	mfpr_val &= ~MFPR_EDGE_MASK;
	mfpr_val |= (edge & 0x3u) << MFPR_ERE_OFFSET;
	mfpr_val |= (!edge & 0x1) << MFPR_EC_OFFSET;

	mfpr_writel(mfpr_off, mfpr_val);
	mfpr_sync();
}

void __init pxa3xx_mfp_init_addr(struct pxa3xx_mfp_addr_map *map)
{
	struct pxa3xx_mfp_addr_map *p;
	unsigned long offset;
	int i;

	for (p = map; p->start != MFP_PIN_INVALID; p++) {
		offset = p->offset;
		i = p->start;

		do {
			mfp_table[i].mfpr_off = offset;
			mfp_table[i].mfpr_val = 0;
			offset += 4; i++;
		} while ((i <= p->end) && (p->end != -1));
	}
}

void __init pxa3xx_init_mfp(void)
{
	if(cpu_is_pxa910())
		mfpr_mmio_base = (void __iomem *)&__REG_PXA910(PXA910_MFPR_BASE);
	else
		mfpr_mmio_base = (void __iomem *)&__REG(MFPR_BASE);
	memset(mfp_table, 0, sizeof(mfp_table));
}

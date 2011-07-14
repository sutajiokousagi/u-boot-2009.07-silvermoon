/*
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <common.h>

#include <linux/types.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <common.h>
#include <config.h>
#include <net.h>
#include <command.h>
#include <malloc.h>

#include <asm/arch/common.h>
#include <asm/arch/pxa168_usb_phy.h>

#define PXA168_USB_SPH_PHY_BASE         CONFIG_USB_SPH_PHY_BASE
#define PMUA_USB_CLK_RES_CTRL   (0xd428285c)

#undef DEBUG

#if DEBUG
static unsigned int usb_debug = DEBUG;
#else
#define usb_debug 0     /* gcc will remove all the debug code for us */
#endif

/*****************************************************************************
 * The registers read/write routines
 ******************************************************************************/
static unsigned usb_sph_get(unsigned *base, unsigned offset)
{
	return readl(base + (offset>>2));
}

static void usb_sph_set(unsigned *base, unsigned offset, unsigned value)
{
	unsigned int reg;

	if (usb_debug)
		printf("base %p off %x base+off %p read %x\n", base, offset,
			(base + (offset>>2)), *(unsigned *)(base + (offset>>2)));
	reg = readl(base + (offset>>2));
	reg |= value;
	writel(reg, base + (offset>>2));
	readl(base + (offset>>2));
}

static void usb_sph_clear(unsigned *base, unsigned offset, unsigned value)
{
	unsigned int reg;

	reg = readl(base + (offset>>2));
	reg &= ~value;
	writel(reg, base + (offset>>2));
	readl(base + (offset>>2));
}

static void usb_sph_write(unsigned *base, unsigned offset, unsigned value)
{
	writel(value, base + (offset>>2));
	readl(base + (offset>>2));
}

static void usb_sph_phy_init(unsigned *base)
{
	if (usb_debug)
		printf("init phy\n\n");

	usb_sph_set(base, U2PCTRL, 1<<UTMI_CTRL_INPKT_DELAY_SOF_SHIFT);
	usb_sph_set(base, U2PCTRL, 1<<UTMI_CTRL_PLL_PWR_UP_SHIFT);
	udelay(1000);
	usb_sph_set(base, U2PCTRL, 1<<UTMI_CTRL_PWR_UP_SHIFT);

	/* aspen specific*/
	if (cpu_is_pxa910_168())
		usb_sph_write(base, 0x3C, 1);  /* Turn on UTMI PHY OTG extension */

	usb_sph_clear(base, U2PPLL, UTMI_PLL_FBDIV_MASK | UTMI_PLL_REFDIV_MASK);
	usb_sph_set(base, U2PPLL, 0xee<<UTMI_PLL_FBDIV_SHIFT | 0xb<<UTMI_PLL_REFDIV_SHIFT);
	usb_sph_set(base, U2PTX, 0x80000);

	/* calibrate */
	while ((usb_sph_get(base, U2PPLL) & PLL_READY) == 0)
		;

	/* toggle VCOCAL_START bit of U2PPLL */
	udelay(200);
	usb_sph_set(base, U2PPLL, VCOCAL_START);
	udelay(40);
	usb_sph_clear(base, U2PPLL, VCOCAL_START);

	/* toggle REG_RCAL_START bit of U2PTX */
	udelay(200);
	usb_sph_set(base, U2PTX, REG_RCAL_START);
	udelay(400);
	usb_sph_clear(base, U2PTX, REG_RCAL_START);

	/* make sure phy is ready */
	while ((usb_sph_get(base, U2PPLL) & PLL_READY) == 0)
		;
}


/* PXA USB SPH init */
/* Initialize USB host controller's UTMI physical interface */
void pxa168_usb_sph_init(void)
{
	u32 tmp;

	/* Turn on Main PMU clocks ACGR */
	writel(0x1EFFFF, 0xD4051024);

	/* USB clk reset */
	writel(0x18, PMUA_USB_CLK_RES_CTRL);
	writel(0x1b, PMUA_USB_CLK_RES_CTRL);

	/* enable the pull up */
	if (!cpu_is_pxa910_168()) {
		if (pxa910_is_z0()) {
			writel((1<<20), (0xc0000004));
		} else {
			tmp = readl(0xd4207004);
			tmp |= (1<<20);
			writel(tmp, (0xd4207004));
		}
	}

	/* init the UTMI transceiver */
	usb_sph_phy_init((unsigned *)PXA168_USB_SPH_PHY_BASE);
}

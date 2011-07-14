/*
 * cpu/88SV331xV5/pxa168/pxa168fb.c -- Marvell PXA168 LCD Controller
 *
 *  Copyright (C) 2009 Marvell International Ltd.
 *  All rights reserved.
 *
 *  2009-10-22  adapted from original version for PXA168
 *              frame buffer driver.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file COPYING in the main directory of this archive for
 * more details.
 */

#include <div64.h>
#include <malloc.h>
#include <linux/types.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <asm/arch/common.h>
#include <asm/arch/pxa168fb.h>
#include <asm/arch/pxa168fb_regs.h>
#include <asm/arch/regs-pxa168.h>

#define DEFAULT_REFRESH		60	/* Hz */

static unsigned int max_fb_size;
struct pxa168fb_info pxa_fb_info;

static void set_pix_fmt(struct fb_var_screeninfo *var, int pix_fmt)
{
	switch (pix_fmt) {
	case PIX_FMT_RGB565:
		var->bits_per_pixel = 16;
		var->red.offset = 11;		var->red.length = 5;
		var->green.offset = 5;		var->green.length = 6;
		var->blue.offset = 0;		var->blue.length = 5;
		var->transp.offset = 0;		var->transp.length = 0;
		var->nonstd &= ~0xff0fffff;
		break;
	case PIX_FMT_BGR565:
		var->bits_per_pixel = 16;
		var->red.offset = 0;		var->red.length = 5;
		var->green.offset = 5;		var->green.length = 6;
		var->blue.offset = 11;		var->blue.length = 5;
		var->transp.offset = 0;		var->transp.length = 0;
		var->nonstd &= ~0xff0fffff;
		break;
	case PIX_FMT_RGB1555:
		var->bits_per_pixel = 16;
		var->red.offset = 10;		var->red.length = 5;
		var->green.offset = 5;		var->green.length = 5;
		var->blue.offset = 0;		var->blue.length = 5;
		var->transp.offset = 15;	var->transp.length = 1;
		var->nonstd &= ~0xff0fffff;
		var->nonstd |= 5 << 20;
		break;
	case PIX_FMT_BGR1555:
		var->bits_per_pixel = 16;
		var->red.offset = 0;		var->red.length = 5;
		var->green.offset = 5;		var->green.length = 5;
		var->blue.offset = 10;		var->blue.length = 5;
		var->transp.offset = 15;	var->transp.length = 1;
		var->nonstd &= ~0xff0fffff;
		var->nonstd |= 5 << 20;
		break;
	case PIX_FMT_RGB888PACK:
		var->bits_per_pixel = 24;
		var->red.offset = 16;		var->red.length = 8;
		var->green.offset = 8;		var->green.length = 8;
		var->blue.offset = 0;		var->blue.length = 8;
		var->transp.offset = 0;		var->transp.length = 0;
		var->nonstd &= ~0xff0fffff;
		var->nonstd |= 6 << 20;
		break;
	case PIX_FMT_BGR888PACK:
		var->bits_per_pixel = 24;
		var->red.offset = 0;		var->red.length = 8;
		var->green.offset = 8;		var->green.length = 8;
		var->blue.offset = 16;		var->blue.length = 8;
		var->transp.offset = 0;		var->transp.length = 0;
		var->nonstd &= ~0xff0fffff;
		var->nonstd |= 6 << 20;
		break;
	case PIX_FMT_RGB888UNPACK:
		var->bits_per_pixel = 32;
		var->red.offset = 16;		var->red.length = 8;
		var->green.offset = 8;		var->green.length = 8;
		var->blue.offset = 0;		var->blue.length = 8;
		var->transp.offset = 0;		var->transp.length = 8;
		var->nonstd &= ~0xff0fffff;
		var->nonstd |= 7 << 20;
		break;
	case PIX_FMT_BGR888UNPACK:
		var->bits_per_pixel = 32;
		var->red.offset = 0;		var->red.length = 8;
		var->green.offset = 8;		var->green.length = 8;
		var->blue.offset = 16;		var->blue.length = 8;
		var->transp.offset = 0;		var->transp.length = 8;
		var->nonstd &= ~0xff0fffff;
		var->nonstd |= 7 << 20;
		break;
	case PIX_FMT_RGBA888:
		var->bits_per_pixel = 32;
		var->red.offset = 16;		var->red.length = 8;
		var->green.offset = 8;		var->green.length = 8;
		var->blue.offset = 0;		var->blue.length = 8;
		var->transp.offset = 24;	var->transp.length = 8;
		var->nonstd &= ~0xff0fffff;
		var->nonstd |= 8 << 20;
		break;
	case PIX_FMT_BGRA888:
		var->bits_per_pixel = 32;
		var->red.offset = 0;		var->red.length = 8;
		var->green.offset = 8;		var->green.length = 8;
		var->blue.offset = 16;		var->blue.length = 8;
		var->transp.offset = 24;	var->transp.length = 8;
		var->nonstd &= ~0xff0fffff;
		var->nonstd |= 8 << 20;
		break;
	case PIX_FMT_YUYV422PACK:
		var->bits_per_pixel = 16;
		var->red.offset = 8;		var->red.length = 16;
		var->green.offset = 4;		var->green.length = 16;
		var->blue.offset = 0;		var->blue.length = 16;
		var->transp.offset = 0;		var->transp.length = 0;
		var->nonstd &= ~0xff0fffff;
		var->nonstd |= 9 << 20;
		break;
	case PIX_FMT_YVU422PACK:
		var->bits_per_pixel = 16;
		var->red.offset = 0;		var->red.length = 16;
		var->green.offset = 8;		var->green.length = 16;
		var->blue.offset = 12;		var->blue.length = 16;
		var->transp.offset = 0;		var->transp.length = 0;
		var->nonstd &= ~0xff0fffff;
		var->nonstd |= 9 << 20;
		break;
	case PIX_FMT_YUV422PACK:
		var->bits_per_pixel = 16;
		var->red.offset = 4;		var->red.length = 16;
		var->green.offset = 12;		var->green.length = 16;
		var->blue.offset = 0;		var->blue.length = 16;
		var->transp.offset = 0;		var->transp.length = 0;
		var->nonstd &= ~0xff0fffff;
		var->nonstd |= 9 << 20;
		break;
	case PIX_FMT_PSEUDOCOLOR:
		var->bits_per_pixel = 8;
		var->red.offset = 0;		var->red.length = 8;
		var->green.offset = 0;		var->green.length = 8;
		var->blue.offset = 0;		var->blue.length = 8;
		var->transp.offset = 0;		var->transp.length = 0;
		break;
	}
}

/*
 * The hardware clock divider has an integer and a fractional
 * stage.
 *
 * clk2 = clk_in / integer_divider
 * clk_out = clk2 * (1 - (fractional_divider >> 12))
 *
 * Calculate integer and fractional divider for given clk_in
 * and clk_out.
 */
static void set_clock_divider(struct pxa168fb_info *fbi,
			const struct fb_videomode *m)
{
	int divider_int;
	int needed_pixclk;
	u64 div_result;
	u32 x = 0;

	/*
	 * Notice: The field pixclock is used by linux fb
	 * is in pixel second. E.g. struct fb_videomode &
	 * struct fb_var_screeninfo
	 */

	/*
	 * Check input values.
	 */
	if (!m || !m->pixclock || !m->refresh) {
		printf("Input refresh or pixclock is wrong.\n");
		return;
	}

	/*
	 * Using PLL/AXI clock.
	 */
	x = 0x80000000;

	/*
	 * Calc divider according to refresh rate.
	 */
	div_result = 1000000000000ll;
	do_div(div_result, m->pixclock);
	needed_pixclk = (u32)div_result;

	divider_int = PXA168_LCD_CLK / needed_pixclk;

	/* check whether divisor is too small. */
	if (divider_int < 2) {
		printf("Warning: clock source is too slow."
				"Try smaller resolution\n");
		divider_int = 2;
	}

	/*
	 * Set setting to reg.
	 */
	x |= divider_int;
	writel(x, fbi->reg_base + LCD_CFG_SCLK_DIV);
}

static void set_dma_control0(struct pxa168fb_info *fbi)
{
	u32 x;

	/*
	 * Set bit to enable graphics DMA.
	 */
	x = readl(fbi->reg_base + LCD_SPU_DMA_CTRL0);
	x |= fbi->active ? 0x00000100 : 0;

	/*
	 * If we are in a pseudo-color mode, we need to enable
	 * palette lookup.
	 */
	if (fbi->pix_fmt == PIX_FMT_PSEUDOCOLOR)
		x |= 0x10000000;

	/*
	 * Configure hardware pixel format.
	 */
	x &= ~(0xF << 16);
	x |= (fbi->pix_fmt >> 1) << 16;

	/*
	 * Check YUV422PACK
	 */
	x &= ~((1 << 9) | (1 << 11) | (1 << 10) | (1 << 12));
	if (((fbi->pix_fmt >> 1) == 5) || (fbi->pix_fmt & 0x1000)) {
		x |= 1 << 9;
		x |= (fbi->panel_rbswap) << 12;
		if (fbi->pix_fmt == 11)
			x |= 1 << 11;
		if (fbi->pix_fmt & 0x1000)
			x |= 1 << 10;
	} else {
		/*
		 * Check red and blue pixel swap.
		 * 1. source data swap. BGR[M:L] rather than RGB[M:L]
		 *	is stored in memeory as source format.
		 * 2. panel output data swap
		 */
		x |= (((fbi->pix_fmt & 1) ^ 1) ^ (fbi->panel_rbswap)) << 12;
	}

	x |= CFG_ARBFAST_ENA(1);

	writel(x, fbi->reg_base + LCD_SPU_DMA_CTRL0);
}

static void set_dma_control1(struct pxa168fb_info *fbi, int sync)
{
	u32 x;

	/*
	 * Configure default bits: vsync triggers DMA, gated clock
	 * enable, power save enable, configure alpha registers to
	 * display 100% graphics, and set pixel command.
	 */
	x = readl(fbi->reg_base + LCD_SPU_DMA_CTRL1);

	/*
	 * We trigger DMA on the falling edge of vsync if vsync is
	 * active low, or on the rising edge if vsync is active high.
	 */
	if (!(sync & FB_SYNC_VERT_HIGH_ACT))
		x |= 0x08000000;

	writel(x, fbi->reg_base + LCD_SPU_DMA_CTRL1);
}

static void set_graphics_start(struct pxa168fb_info *fbi,
				int xoffset, int yoffset)
{
	struct fb_var_screeninfo *var = &fbi->var;
	int pixel_offset;
	unsigned long addr;

	pixel_offset = (yoffset * var->xres_virtual) + xoffset;
	addr = fbi->fb_start_dma + (pixel_offset * (var->bits_per_pixel >> 3));

	/* Enable the interrupt */
	writel(readl(fbi->reg_base+SPU_IRQ_ENA)
		|VSYNC_IRQ_ENA(0x1),
		fbi->reg_base+SPU_IRQ_ENA);
}

static void set_dumb_panel_control(struct pxa168fb_info *fbi,
				struct pxa168fb_mach_info *mi)
{
	u32 x;

	/*
	 * Preserve enable flag.
	 */
	x = readl(fbi->reg_base + LCD_SPU_DUMB_CTRL) & 0x00000001;
	x |= (fbi->is_blanked ? 0x7 : mi->dumb_mode) << 28;
	x |= mi->gpio_output_data << 20;
	x |= mi->gpio_output_mask << 12;
	x |= mi->panel_rgb_reverse_lanes ? 0x00000080 : 0;
	x |= mi->invert_composite_blank ? 0x00000040 : 0;
	x |= (fbi->var.sync & FB_SYNC_COMP_HIGH_ACT) ? 0x00000020 : 0;
	x |= mi->invert_pix_val_ena ? 0x00000010 : 0;
	x |= (fbi->var.sync & FB_SYNC_VERT_HIGH_ACT) ? 0x00000008 : 0;
	x |= (fbi->var.sync & FB_SYNC_HOR_HIGH_ACT) ? 0x00000004 : 0;
	x |= mi->invert_pixclock ? 0x00000002 : 0;
	writel(x, fbi->reg_base + LCD_SPU_DUMB_CTRL);
}

static void set_dumb_screen_dimensions(struct pxa168fb_info *fbi)
{
	struct fb_var_screeninfo *v = &fbi->var;
	int x;
	int y;

	x = v->xres + v->right_margin + v->hsync_len + v->left_margin;
	y = v->yres + v->lower_margin + v->vsync_len + v->upper_margin;

	writel((y << 16) | x, fbi->reg_base + LCD_SPUT_V_H_TOTAL);
}

static int pxa168fb_set_par(struct pxa168fb_info *fbi,
				struct pxa168fb_mach_info *mi)
{
	const struct fb_videomode *mode = mi->modes;
	int pix_fmt;
	u32 x;

	/*
	 * Determine which pixel format we're going to use.
	 */
	pix_fmt = mi->pix_fmt;
	fbi->pix_fmt = pix_fmt;

	set_pix_fmt(&fbi->var, pix_fmt);

	/*
	 * Configure global panel parameters.
	 */
	writel((mode->yres << 16) | mode->xres,
		fbi->reg_base + LCD_SPU_V_H_ACTIVE);

	/*
	 * convet var to video mode
	 */
	mode = mi->modes;

	/* Calculate clock divisor. */
	set_clock_divider(fbi, mode);

	/* Configure dma ctrl regs. */
	set_dma_control0(fbi);
	set_dma_control1(fbi, mode->sync);

	/*
	 * Configure graphics DMA parameters.
	 */
	set_graphics_start(fbi, fbi->var.xoffset, fbi->var.yoffset);
	x = readl(fbi->reg_base + LCD_CFG_GRA_PITCH);
	x = (x & ~0xFFFF) |
		((fbi->var.xres_virtual * fbi->var.bits_per_pixel) >> 3);
	writel(x, fbi->reg_base + LCD_CFG_GRA_PITCH);
	writel((fbi->var.yres << 16) | fbi->var.xres,
		fbi->reg_base + LCD_SPU_GRA_HPXL_VLN);
	writel((fbi->var.yres << 16) | fbi->var.xres,
		fbi->reg_base + LCD_SPU_GZM_HPXL_VLN);

	/*
	 * Configure dumb panel ctrl regs & timings.
	 */
	set_dumb_panel_control(fbi, mi);
	set_dumb_screen_dimensions(fbi);

	writel((fbi->var.left_margin << 16) | fbi->var.right_margin,
		fbi->reg_base + LCD_SPU_H_PORCH);
	writel((fbi->var.upper_margin << 16) | fbi->var.lower_margin,
		fbi->reg_base + LCD_SPU_V_PORCH);

	x = readl(fbi->reg_base + LCD_SPU_DUMB_CTRL);
	if ((x & 1) == 0)
		writel(x | 1, fbi->reg_base + LCD_SPU_DUMB_CTRL);

	return 0;
}

static int pxa168fb_init_mode(struct pxa168fb_info *info,
			struct pxa168fb_mach_info *mi)
{
	struct fb_var_screeninfo *var = &info->var;
	int ret = 0;
	u32 total_w, total_h, refresh;
	u64 div_result;

	/*
	 * Set default value
	 */
	refresh = DEFAULT_REFRESH;

	/*
	 * Initialize mode
	 */
	var->xres = mi->modes->xres;
	var->yres = mi->modes->yres;
	var->xres_virtual = mi->modes->xres;
	var->yres_virtual = mi->modes->yres;
	var->xoffset = 0;
	var->yoffset = 0;
	var->pixclock = mi->modes->pixclock;
	var->left_margin = mi->modes->left_margin;
	var->right_margin = mi->modes->right_margin;
	var->upper_margin = mi->modes->upper_margin;
	var->lower_margin = mi->modes->lower_margin;
	var->hsync_len = mi->modes->hsync_len;
	var->vsync_len = mi->modes->vsync_len;
	var->sync = mi->modes->sync;

	/* Init settings. */
	var->xres_virtual = var->xres;
	var->yres_virtual = var->yres * 2;

	/* correct pixclock. */
	total_w = var->xres + var->left_margin + var->right_margin +
			var->hsync_len;
	total_h = var->yres + var->upper_margin + var->lower_margin +
			var->vsync_len;

	div_result = 1000000000000ll;
	do_div(div_result, total_w * total_h * refresh);
	var->pixclock = (u32)div_result;

	return ret;
}

static void pxa168fb_set_default(struct pxa168fb_info *fbi,
				struct pxa168fb_mach_info *mi)
{
	/*
	 * Configure default register values.
	 */

	/* Disable LCD DMA controller */
	writel(0x00000000, fbi->reg_base + LCD_SPU_DMA_CTRL0);
	/* Set background color when no object is being displayed */
	writel(0x00000000, fbi->reg_base + LCD_SPU_BLANKCOLOR);
	/* Set LCD type */
	writel((0x20 | mi->io_pin_allocation_mode),
			fbi->reg_base + SPU_IOPAD_CONTROL);
	/* Graphic frame1 starting address */
	writel(0x00000000, fbi->reg_base + LCD_CFG_GRA_START_ADDR1);
	/* Set graphic starting point on the screen */
	writel(0x00000000, fbi->reg_base + LCD_SPU_GRA_OVSA_HPXL_VLN);
	writel(0x0, fbi->reg_base + LCD_SPU_SRAM_PARA0);
	writel(CFG_CSB_256x32(0x1)|CFG_CSB_256x24(0x1)|CFG_CSB_256x8(0x1),
		fbi->reg_base + LCD_SPU_SRAM_PARA1);
	/* Configure FIFO depths */
	writel(VIDEO_FIFO(1) | GRAPHIC_FIFO(1),
			fbi->reg_base + LCD_FIFO_DEPTH);

	/* Configure default bits: vsync triggers DMA,
	 * power save enable, configure alpha registers to
	 * display 100% graphics, and set pixel command.
	 */
	writel(0x2000FF00, fbi->reg_base + LCD_SPU_DMA_CTRL1);

	/* Fill in video Contrast, Saturation, and Hue registers */
	writel(0x20004000, fbi->reg_base + LCD_SPU_SATURATION);
	writel(0x00004000, fbi->reg_base + LCD_SPU_CONTRAST);
	writel(0x00004000, fbi->reg_base + LCD_SPU_CBSH_HUE);

	/* Frame buffers start */
	writel(DEFAULT_FB_BASE, fbi->reg_base + LCD_CFG_GRA_START_ADDR0);
}

int pxa168fb_init(struct pxa168fb_mach_info *mi)
{
	struct pxa168fb_info *fbi = &pxa_fb_info;
	unsigned int val;

	/* Enable clock to LCD controller */
	writel(LCD_IRE_AXICLK_EN,
		(APS_PMU_UNIT_PHYSICAL_BASE + PMUA_LCD_CLK_RES_CTRL_offset));

	/* Initialize private data */
	fbi->panel_rbswap = mi->panel_rbswap;
	fbi->is_blanked = 0;
	fbi->active = mi->active;

	/*
	 * Map LCD controller registers.
	 */
	fbi->reg_base = (unsigned int *)PXA168_LCD_REGS_BASE;

	/*
	 * Allocate framebuffer memory.
	 */
	if (!max_fb_size) {
		if (mi->max_fb_size)
			max_fb_size = mi->max_fb_size;
		else
			max_fb_size = DEFAULT_FB_SIZE;
	}
	fbi->fb_size = max_fb_size;

	fbi->fb_start = (unsigned int *)DEFAULT_FB_BASE;
	fbi->fb_start_dma = DEFAULT_FB_BASE;
	memset(fbi->fb_start, 0, fbi->fb_size);

	/*
	 * init video mode data.
	 */
	pxa168fb_init_mode(fbi, mi);

	/*
	 * Fill in sane defaults.
	 */
	pxa168fb_set_default(fbi, mi);
	pxa168fb_set_par(fbi, mi);

	/*  Enable DMAs to start transfer of data to the LCD */
	val = readl(fbi->reg_base + LCD_SPU_DMA_CTRL0);
	val |= (CFG_GRA_ENA(0x1) | CFG_DMA_ENA(0x1));
	writel(val, fbi->reg_base + LCD_SPU_DMA_CTRL0);

	return 0;
}

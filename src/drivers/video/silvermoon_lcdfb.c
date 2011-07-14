/*
 * Driver for AT91/AT32 LCD Controller
 *
 * Copyright (C) 2007 Atmel Corporation
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/arch/common.h>
#include <asm/arch/mfp.h>
#include <asm/arch/mfp-pxa168.h>
#include <lcd.h>

int lcd_line_length;
int lcd_color_fg;
int lcd_color_bg;

void *lcd_base;				/* Start of framebuffer memory	*/
void *lcd_console_address;		/* Start of console buffer	*/

vidinfo_t panel_info = { 1280, 720, LCD_COLOR16 };

short console_col;
short console_row;


#define GPIO_REGS_BASE  (0xD4019000)
#define GPIO2_REGS_BASE (GPIO_REGS_BASE+0x8)
#define GPIO2_PSR_BASE (GPIO2_REGS_BASE+0x18)
#define GPIO2_SDR_BASE  (GPIO2_REGS_BASE+0x54)



#define PXA168_LCD_BASE 0xD420B000
#define LCD_CFG_GRA_START_ADDR0 0xF4
#define LCD_SPU_DMA_CTRL0 0x190
#define LCD_SPU_DUMB_CTRL 0x1B8

#define APBC_GPIO_CLK_RST 0xD4015008

// Frame buffer filled by aspenix
#define SILVERMOON_FB   0xd00000



#define lcdc_readl(reg)       __raw_readl((PXA168_LCD_BASE)+(reg))
#define lcdc_writel(reg, val) __raw_writel((val), (PXA168_LCD_BASE)+(reg))

void lcd_setcolreg(ushort regno, ushort red, ushort green, ushort blue)
{
//	lcdc_writel(panel_info.mmio, ATMEL_LCDC_LUT(regno),
//		    (blue >> 3) | ((green & 0xfc) << 3) | ((red & 0xf8) << 8));
// Ignored, because we run in truecolor mode.
}

struct register_values {
	volatile unsigned long *offset;
	unsigned long           value;
};

void lcd_ctrl_init(void *lcdbase) {
    unsigned int i;
    struct register_values register_values[] = {
	{(volatile unsigned int *) 0xd428284c, 0x0000007f}, // APMU_LCD_CLK_RES_CTRL
	{(volatile unsigned int *) 0xd420b0c0, (unsigned long)lcdbase}, // LCD_SPU_DMA_START_ADDR_Y0
	{(volatile unsigned int *) 0xd420b0c4, (unsigned long)lcdbase}, // LCD_SPU_DMA_START_ADDR_U0
	{(volatile unsigned int *) 0xd420b0c8, (unsigned long)lcdbase}, // LCD_SPU_DMA_START_ADDR_V0
	{(volatile unsigned int *) 0xd420b0cc, 0x00000000}, // LCD_CFG_DMA_START_ADDR_0
	{(volatile unsigned int *) 0xd420b0d0, 0x00000000}, // LCD_SPU_DMA_START_ADDR_Y1
	{(volatile unsigned int *) 0xd420b0d4, 0x00000000}, // LCD_SPU_DMA_START_ADDR_U1
	{(volatile unsigned int *) 0xd420b0d8, 0x00000000}, // LCD_SPU_DMA_START_ADDR_V1
	{(volatile unsigned int *) 0xd420b0dc, 0x00000000}, // LCD_CFG_DMA_START_ADDR_1
	{(volatile unsigned int *) 0xd420b0e0, 0x00000a00}, // LCD_SPU_DMA_PITCH_YC
	{(volatile unsigned int *) 0xd420b0e4, 0x00000000}, // LCD_SPU_DMA_PITCH_UV
	{(volatile unsigned int *) 0xd420b0e8, 0x00000000}, // LCD_SPUT_DMA_OVSA_HPXL_VLN
	{(volatile unsigned int *) 0xd420b0ec, 0x02d00500}, // LCD_SPU_DMA_HPXL_VLN
	{(volatile unsigned int *) 0xd420b0f0, 0x02d00500}, // LCD_SPU_DZM_HPXL_VLN
	{(volatile unsigned int *) 0xd420b0f4, (unsigned long)lcdbase}, // LCD_CFG_GRA_START_ADDR0
	{(volatile unsigned int *) 0xd420b0f8, (unsigned long)lcdbase}, // LCD_CFG_GRA_START_ADDR1
	{(volatile unsigned int *) 0xd420b0fc, 0x00000640}, // LCD_CFG_GRA_PITCH
	{(volatile unsigned int *) 0xd420b100, 0x00000000}, // LCD_SPU_GRA_OVSA_HPXL_VLN
	{(volatile unsigned int *) 0xd420b104, 0x02580320}, // LCD_SPU_GRA_HPXL_VLN
	{(volatile unsigned int *) 0xd420b108, 0x02580320}, // LCD_SPU_GZM_HPXL_VLN
	{(volatile unsigned int *) 0xd420b10c, 0x00000000}, // LCD_SPU_HWC_OVSA_HPXL_VLN
	{(volatile unsigned int *) 0xd420b110, 0x00000000}, // LCD_SPU_HWC_HPXL_VLN
	{(volatile unsigned int *) 0xd420b114, 0x02ee0672}, // LCD_SPUT_V_H_TOTAL
	{(volatile unsigned int *) 0xd420b118, 0x02d00500}, // LCD_SPU_V_H_ACTIVE
	{(volatile unsigned int *) 0xd420b11c, 0x006e00dc}, // LCD_SPU_H_PORCH
	{(volatile unsigned int *) 0xd420b120, 0x00050014}, // LCD_SPU_V_PORCH
	{(volatile unsigned int *) 0xd420b124, 0x00000000}, // LCD_SPU_BLANKCOLOR
	{(volatile unsigned int *) 0xd420b128, 0x00000000}, // LCD_SPU_ALPHA_COLOR1
	{(volatile unsigned int *) 0xd420b12c, 0x00000000}, // LCD_SPU_ALPHA_COLOR2
	{(volatile unsigned int *) 0xd420b130, 0x00000000}, // LCD_SPU_COLORKEY_Y
	{(volatile unsigned int *) 0xd420b134, 0x00000000}, // LCD_SPU_COLORKEY_U
	{(volatile unsigned int *) 0xd420b138, 0x00000000}, // LCD_SPU_COLORKEY_V
	{(volatile unsigned int *) 0xd420b190, 0x08040011}, // LCD_SPU_DMA_CTRL0
	{(volatile unsigned int *) 0xd420b194, 0xa001ff81}, // LCD_SPU_DMA_CTRL1
	{(volatile unsigned int *) 0xd420b1a8, 0x90000001}, // LCD_CFG_SCLK_DIV
	{(volatile unsigned int *) 0xd420b1b8, 0x210ff003}, // LCD_SPU_DUMB_CTRL
	{(volatile unsigned int *) 0xd420b1c8, 0x00000004}, // LCD_MISC_CNTL


	/* Enable power to LCD Panel. Set GPIO 84 as output */
	{(unsigned long *)GPIO2_PSR_BASE, 0x100000},
	{(unsigned long *)GPIO2_SDR_BASE, 0x100000},
    };

    printf("Initializing LCD at address 0x%08x (updating %d registers)\n",
		   (unsigned int)lcdbase,
		   sizeof(register_values)/sizeof(*register_values));

	memset(lcdbase, 0, 1280*720*2);

	/* Initialize the LCD Controller according to the known-good
	 * values taken directly from Linux
	 */
	int t;
	for(i=0; i<sizeof(register_values)/sizeof(*register_values); i++) {
		*register_values[i].offset = register_values[i].value;

		/* Force readback to make value take effect immediately */
		t = __raw_readl(register_values[i].offset);
	}
}

void lcd_enable() {
    /* Enable Dumb LCD panel */
    lcdc_writel(LCD_SPU_DUMB_CTRL, lcdc_readl(LCD_SPU_DUMB_CTRL) | 0x1);
    /* Enable Graphic Transfer enable */
    lcdc_writel(LCD_SPU_DMA_CTRL0, lcdc_readl(LCD_SPU_DMA_CTRL0) | (0x1<<8));
}

ulong calc_fbsize(void) {
    return panel_info.vl_col * panel_info.vl_row * NBITS(panel_info.vl_bpix)/8*2;
}


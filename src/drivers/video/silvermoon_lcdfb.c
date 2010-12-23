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

vidinfo_t panel_info = { 800, 600, LCD_COLOR16 };

short console_col;
short console_row;




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
        {(unsigned long *)0xd420b0c0, (unsigned long)lcdbase},
        {(unsigned long *)0xd420b0c4, (unsigned long)lcdbase},
        {(unsigned long *)0xd420b0c8, (unsigned long)lcdbase},
        {(unsigned long *)0xd420b0f4, (unsigned long)lcdbase},
        {(unsigned long *)0xd420b0f8, (unsigned long)lcdbase},
		/*
        {(unsigned long *)0xd420b0cc, 0x00000000},
        {(unsigned long *)0xd420b0d0, 0x00000000},
        {(unsigned long *)0xd420b0d4, 0x00000000},
        {(unsigned long *)0xd420b0d8, 0x00000000},
        {(unsigned long *)0xd420b0dc, 0x00000000},
		*/
        {(unsigned long *)0xd420b0e0, 0x00000640},
		/*
        {(unsigned long *)0xd420b0e4, 0x00000000},
        {(unsigned long *)0xd420b0e8, 0x00000000},
		*/
        {(unsigned long *)0xd420b0ec, 0x02580320},
        {(unsigned long *)0xd420b0f0, 0x02580320},
        {(unsigned long *)0xd420b0fc, 0x00000640},
        //{(unsigned long *)0xd420b100, 0x00000000},
        {(unsigned long *)0xd420b104, 0x02580320},
        {(unsigned long *)0xd420b108, 0x02580320},
		/*
        {(unsigned long *)0xd420b10c, 0x00000000},
        {(unsigned long *)0xd420b110, 0x00000000},
		*/
        {(unsigned long *)0xd420b114, 0x028c049f},
        {(unsigned long *)0xd420b118, 0x02580320},
        {(unsigned long *)0xd420b11c, 0x00d70028},
        {(unsigned long *)0xd420b120, 0x0022000e},
		/*
        {(unsigned long *)0xd420b124, 0x00000000},
        {(unsigned long *)0xd420b128, 0x00000000},
        {(unsigned long *)0xd420b12c, 0x00000000},
        {(unsigned long *)0xd420b130, 0x00000000},
        {(unsigned long *)0xd420b134, 0x00000000},
        {(unsigned long *)0xd420b138, 0x00000000},
        {(unsigned long *)0xd420b13c, 0x00000000},
        {(unsigned long *)0xd420b140, 0x00000000},
        {(unsigned long *)0xd420b144, 0x00000000},
		*/
        {(unsigned long *)0xd420b148, 0x80000000},
        {(unsigned long *)0xd420b14c, 0x00000640},
        //{(unsigned long *)0xd420b150, 0x00000000},
        {(unsigned long *)0xd420b154, 0x00000640},
        //{(unsigned long *)0xd420b158, 0x00000000},
        {(unsigned long *)0xd420b15c, 0x000000ff},
        //{(unsigned long *)0xd420b160, 0x00000000},
        {(unsigned long *)0xd420b164, 0xe0400000},
        {(unsigned long *)0xd420b168, 0x6028c838},
        //{(unsigned long *)0xd420b16c, 0x00000000},
        {(unsigned long *)0xd420b170, 0x00020000},
        {(unsigned long *)0xd420b174, 0x5119824c},
        {(unsigned long *)0xd420b178, 0x03000000},
        {(unsigned long *)0xd420b17c, 0x00000004},
        {(unsigned long *)0xd420b180, 0x02000f0e},
        {(unsigned long *)0xd420b184, 0x00008808},
		/*
        {(unsigned long *)0xd420b188, 0x00000000},
        {(unsigned long *)0xd420b18c, 0x00000000},
        {(unsigned long *)0xd420b198, 0x00000000},
        {(unsigned long *)0xd420b19c, 0x00000000},
        {(unsigned long *)0xd420b1a0, 0x00000000},
		*/
        {(unsigned long *)0xd420b1a4, 0x0000e000},
        {(unsigned long *)0xd420b1a8, 0x80000007},
        {(unsigned long *)0xd420b1ac, 0x00004000},
        {(unsigned long *)0xd420b1b0, 0x20004000},
        {(unsigned long *)0xd420b1b4, 0x00004000},
        {(unsigned long *)0xd420b1bc, 0x00000002},
        {(unsigned long *)0xd420b1c0, 0x88000000},
        {(unsigned long *)0xd420b1c4, 0x44C0018C},
        {(unsigned long *)0xd420b1c8, 0x00000005},
        {(unsigned long *)0xd420b1cc, 0xD420B000},

    	/* Enable power to LCD Panel. Set GPIO 84 as output */
		{(unsigned long *)GPIO2_PSR_BASE, 0x100000},
		{(unsigned long *)GPIO2_SDR_BASE, 0x100000},

        {(unsigned long *)0xd420b1b8, 0x210ff10f}, // DUMB
        {(unsigned long *)0xd420b190, 0x08001110}, // CTRL0
        {(unsigned long *)0xd420b194, 0x20020081}, // CTRL1
    };


    printf("Initializing LCD at address 0x%08x (updating %d registers)\n",
		   (unsigned int)lcdbase,
		   sizeof(register_values)/sizeof(*register_values));

	memset(lcdbase, 0, 800*600*2);

    /* Initialize the LCD Controller regs to set the
     * LCD to 800x600 RGB666 Dumb panel mode
     */
    for(i=0; i<sizeof(register_values)/sizeof(*register_values); i++) {
		printf("Setting %p -> %p: ", register_values[i].offset, register_values[i].value);
		*register_values[i].offset = register_values[i].value;
		printf("%p\n", *register_values[i].offset);
		//udelay(1000000);
	}
}

void lcd_enable() {
    /* Enable Dumb LCD panel */
    lcdc_writel(LCD_SPU_DUMB_CTRL, lcdc_readl(LCD_SPU_DUMB_CTRL) | 0x1);
    /* Enable Graphic Transfer enable */
    lcdc_writel(LCD_SPU_DMA_CTRL0, lcdc_readl(LCD_SPU_DMA_CTRL0) | (0x1<<8));
}

ulong calc_fbsize(void) {
    return panel_info.vl_col * panel_info.vl_row * NBITS(panel_info.vl_bpix)/8;
}


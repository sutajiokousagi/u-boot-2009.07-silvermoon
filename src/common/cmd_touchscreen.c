/*
 * (C) Copyright 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Misc functions
 */
#include <common.h>
#include <command.h>
#include <asm/arch/common.h>


#define MY_GPIO52_OFFSET  (1 << (52 - 32))  // bit mask offset for GPIO52

#define MY_MFP_52 0x03C
// extra offset added to basic base to select the right bank register
#define MY_GPIO52_BASE    0x4
// bitwise clr of direction register -- makes it an input
#define MY_GPIO_CDR  0x0060
#define MY_GPIO_PLR  0x0000  // pin level readback



static int init_touchscreen(void) {

    // For now just initialize GPIO 52, which is the pen up/down line.
    // Later, if we need to implement position reading, there will be
    // a lot more stuff here

    unsigned long mfp_phys_base = 0xD401E000;
    unsigned long gpio_phys_base = 0xD4019000;

    // set up MFP52, detect rising edge, GPIO function (AF0)
    __raw_writel(0xC090, mfp_phys_base + MY_MFP_52);

    // make GPIO52 an input
    __raw_writel(MY_GPIO52_OFFSET, gpio_phys_base + MY_GPIO52_BASE + MY_GPIO_CDR);


    //printk( "Pin readback of bank 2: *0x%08lx = %08x\n", (unsigned long)my_gpio.mmio_base + MY_GPIO52_BASE + MY_GPIO_PLR, __raw_readl(my_gpio.mmio_base + MY_GPIO52_BASE + MY_GPIO_PLR) );
    //printk( "Dir readback of bank 2: *0x%08lx = %08x\n", (unsigned long)my_gpio.mmio_base + MY_GPIO52_BASE + MY_GPIO_PDR, __raw_readl(my_gpio.mmio_base + MY_GPIO52_BASE + MY_GPIO_PDR) );

    return 1;

}


int do_touchscreen (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    static int ts_initialized = 0;
    unsigned long gpio_phys_base = 0xD4019000;

    if(!ts_initialized) {
        init_touchscreen();
        ts_initialized = 1;
    }

    // read the touchscreen interrupt line, high denotes stylus down
    // TS-touched is GPIO52
    return __raw_readl(gpio_phys_base + MY_GPIO52_BASE + MY_GPIO_PLR) & MY_GPIO52_OFFSET;
}


U_BOOT_CMD(
	touchscreen ,    1,    1,     do_touchscreen,
	"touchscreen - poll the touchscreen to determine if it's being pressed\n",
	"N\n"
	"    - determine if the touchscreen is being pressed\n"
);

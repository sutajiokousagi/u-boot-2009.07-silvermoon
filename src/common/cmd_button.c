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


#define MY_GPIO89_OFFSET  (1 << (89 - 64))  // bit mask offset for GPIO89

#define MY_MFP_89 0x164
// extra offset added to basic base to select the right bank register
#define MY_GPIO89_BASE    0x8
// bitwise clr of direction register -- makes it an input
#define MY_GPIO_CDR  0x0060
#define MY_GPIO_PLR  0x0000  // pin level readback



static int init_button(void) {

    // For now just initialize GPIO 89, which is the pen up/down line.
    // Later, if we need to implement position reading, there will be
    // a lot more stuff here

    unsigned long mfp_phys_base = 0xD401E000;
    unsigned long gpio_phys_base = 0xD4019000;

    // set up MFP89, detect rising edge, GPIO function (AF0)
    __raw_writel(0x0880, mfp_phys_base + MY_MFP_89);

    // make GPIO89 an input (by Clearing its Direction Register)
    __raw_writel(MY_GPIO89_OFFSET, gpio_phys_base + MY_GPIO89_BASE + MY_GPIO_CDR);


    return 1;

}


int do_button (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int value;
    static int ts_initialized = 0;
    unsigned long gpio_phys_base = 0xD4019000;

    if(!ts_initialized) {
        init_button();
        ts_initialized = 1;
    }

    // read the button interrupt line, high denotes stylus down
    // TS-touched is GPIO89
    value = __raw_readl(gpio_phys_base + MY_GPIO89_BASE + MY_GPIO_PLR) & MY_GPIO89_OFFSET;
    return value;
}


U_BOOT_CMD(
	button ,    1,    1,     do_button,
	"button - poll the button to determine if it's being pressed\n",
	"N\n"
	"    - determine if the button is being pressed\n"
);

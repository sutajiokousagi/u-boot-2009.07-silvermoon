/*
 * (C) Copyright 2005
 * Marvell Semiconductors Ltd. <www.marvell.com>
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

#include <common.h>
#include <asm/arch/common.h>
#include <asm/arch/mfp.h>
#include <asm/arch/mfp-pxa168.h>

extern void aspen_init();

void
dummy_delay(unsigned  int delay)
{
    volatile unsigned int i;
    for(i = 0; i < delay; i++);
}

unsigned long initdram (int board_type)
{
    unsigned int mmu_mmap0, area_len;
    mmu_mmap0 = *(unsigned int*)0xb0000100;
    area_len = (mmu_mmap0 >> 16) & 0xf;
    if (area_len < 0x7)
    {
	printf("Unknown DRAM Size\n");
	return 0;
    }
    else
    	return ((0x8 << (area_len-0x7))*1024*1024);
}

int board_init (void)
{
    DECLARE_GLOBAL_DATA_PTR;
    volatile unsigned int reg;

    /* configure for MFP */
    aspen_init();

    /*MFP for EXT_WAKEUP signal to GPIO86 for touch interrupt*/
    *(volatile unsigned int *)0xd401E208 = 0x9886;
    
    /* set SEL_MRVL_ID bit in MOHAWK_CPU_CONF register - Ning */
    *(volatile unsigned int *)0xD4282c08 = *(volatile unsigned int *)0xD4282c08 | 0x100;

    /* ensure L2 cache is not mapped as SRAM */
    *(unsigned int *)0xd4282c08 &= ~(1<<4);

    /* lowering RTC setting could improve Vmin */
    *(unsigned int *)0xd4282c10 = 0x16b5ad6d;
    *(unsigned int *)0xd4282c18 = 0x2d5b56b5;

    /* Enable clocks */
    *(unsigned int *)0xD4051024 = 0xffffffff;
    /* enable UART2 clock */
    *(unsigned int *)0xD4015000 = 0x13;

    /* enable PMU user access */
    __asm__ __volatile__ (
           "mcr     p15, 0, %0, c15, c9, 0\n\t"
           :
           : "r"(0x1)
    );

    gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
    gd->bd->bi_dram[0].size  = initdram(0);
 
    /* arch number of Teton BGA Board */
    gd->bd->bi_arch_number = 2089;  // MACH_TETON_BGA

    /* adress of boot parameters */
    gd->bd->bi_boot_params = 0x00000100;
    gd->baudrate = CONFIG_BAUDRATE;
    icache_enable ();
	
    return 0;
}

int misc_init_r (void)
{
        char *env;
                                                                                                                                               
        /* primary network interface */
        env = getenv("ethprime");
        if(!env)
               setenv("ethprime","eth0");

        /* default usb mode */
	env = getenv("usbMode");
        if(!env)
               setenv("usbMode","host");

        /* linux boot arguments */
   	env = getenv("default_load_addr");
   	if(!env)
      		setenv("default_load_addr",CONFIG_SYS_DEF_LOAD_ADDR);
 
        env = getenv("image_name");
        if(!env)
                setenv("image_name",CONFIG_SYS_IMG_NAME);

        env = getenv("bootfile");
        if(!env)
                setenv("bootfile",CONFIG_SYS_IMG_NAME);
                
   	env = getenv("initrd_name");
       	if(!env)
                setenv("initrd_name",CONFIG_SYS_INITRD_NAME);
 
   	env = getenv("initrd_load_addr");
       	if(!env)
               	setenv("initrd_load_addr",CONFIG_SYS_INITRD_LOAD_ADDR);

   	env = getenv("initrd_size");
        if(!env)
                setenv("initrd_size",CONFIG_SYS_INITRD_SIZE);
 
        env = getenv("standalone_mtd");
        if(!env)
                setenv("standalone_mtd","fsload $(default_load_addr) $(image_name);setenv bootargs $(bootargs) root=/dev/mtdblock0 rw rootfstype=jffs2 ip=$(ipaddr):$(serverip)$(bootargs_end);bootm $(default_load_addr);");

        env = getenv("standalone_initrd");
        if(!env)
                setenv("standalone_initrd","fsload $(default_load_addr) $(image_name);fsload $(initrd_load_addr) $(initrd_name);setenv bootargs $(bootargs) root=/dev/ram0 rw initrd=0x$(initrd_load_addr),0x$(initrd_size) ip=$(ipaddr):$(serverip)$(bootargs_end); bootm $(default_load_addr);");
        
        /* MAC addresses */
        env = getenv("ethaddr");
        if(!env)
                setenv("ethaddr",ETHADDR);

	return 0;
}


int dram_init (void)
{
	return 0;
}

#define CPU_SYS_SOFT_RST_REG         0x90009034
#define CPU_SYS_SOFT_RST_BIT         0x00010000 	/* bit 16 in little endian */
void reset_cpu(ulong addr)
{  
   int i;

   *(volatile unsigned int*)(CPU_SYS_SOFT_RST_REG) = CPU_SYS_SOFT_RST_BIT;
   
   for(i=0; i<0x10000; )
     i++;

   *(volatile unsigned int*)(CPU_SYS_SOFT_RST_REG) = 0x0;
}

int checkboard (void)
{
	return display_marvell_banner();
}

int display_marvell_banner (void)
{
   printf("\n");
   printf(" __  __                      _ _\n");
   printf("|  \\/  | __ _ _ ____   _____| | |\n");
   printf("| |\\/| |/ _` | '__\\ \\ / / _ \\ | |\n");
   printf("| |  | | (_| | |   \\ V /  __/ | |\n");
   printf("|_|  |_|\\__,_|_|    \\_/ \\___|_|_|\n");
   printf(" _   _     ____              _\n");
   printf("| | | |   | __ )  ___   ___ | |_ \n");
   printf("| | | |___|  _ \\ / _ \\ / _ \\| __| \n");
   printf("| |_| |___| |_) | (_) | (_) | |_ \n");
   printf(" \\___/    |____/ \\___/ \\___/ \\__| ");
   printf("\n\nMARVELL PXAXXX series.");
   printf("\nBased on 88SV331xV5 Core with ARM926 LE CPU."); 
   printf("\nCode original base is u-boot-arm 2009.01-rc1.\n\n");

   return 0;
}

void lowlevel_init()
{
	return;
}

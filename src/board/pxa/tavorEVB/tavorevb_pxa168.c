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

int set_volt(u32 vol);

static mfp_cfg_t pxa910_tavorevb_mfp_cfg[] __initdata = {
	/* UART2 */
	GPIO47_UART2_RXD,
	GPIO48_UART2_TXD,

	/* SMC */
	SM_nCS0_nCS0,
	SM_ADV_SM_ADV,
	SM_SCLK_SM_SCLK,
	SM_SCLK_SM_SCLK,
	SM_BE0_SM_BE0,
	SM_BE1_SM_BE1,

	/* DFI */
	PXA910_DF_IO0_ND_IO0,
	PXA910_DF_IO1_ND_IO1,
	PXA910_DF_IO2_ND_IO2,
	PXA910_DF_IO3_ND_IO3,
	PXA910_DF_IO4_ND_IO4,
	PXA910_DF_IO5_ND_IO5,
	PXA910_DF_IO6_ND_IO6,
	PXA910_DF_IO7_ND_IO7,
	PXA910_DF_IO8_ND_IO8,
	PXA910_DF_IO9_ND_IO9,
	PXA910_DF_IO10_ND_IO10,
	PXA910_DF_IO11_ND_IO11,
	PXA910_DF_IO12_ND_IO12,
	PXA910_DF_IO13_ND_IO13,
	PXA910_DF_IO14_ND_IO14,
	PXA910_DF_IO15_ND_IO15,
	DF_nCS0_SM_nCS2_nCS0,
	DF_ALE_SM_WEn_ND_ALE,
	DF_CLE_SM_OEn_ND_CLE,
	DF_WEn_DF_WEn,
	DF_REn_DF_REn,
	DF_RDY0_DF_RDY0,
};

static struct pxa3xx_mfp_addr_map pxa910_910_mfp_addr_map[] __initdata = {

	MFP_ADDR_X(GPIO0, GPIO54, 0xdc),
	MFP_ADDR_X(GPIO67, GPIO98, 0x1b8),
	MFP_ADDR_X(GPIO100, GPIO109, 0x238),

	MFP_ADDR(GPIO123, 0xcc),
	MFP_ADDR(GPIO124, 0xd0),

	MFP_ADDR(DF_IO0, 0x40),
	MFP_ADDR(DF_IO1, 0x3c),
	MFP_ADDR(DF_IO2, 0x38),
	MFP_ADDR(DF_IO3, 0x34),
	MFP_ADDR(DF_IO4, 0x30),
	MFP_ADDR(DF_IO5, 0x2c),
	MFP_ADDR(DF_IO6, 0x28),
	MFP_ADDR(DF_IO7, 0x24),
	MFP_ADDR(DF_IO8, 0x20),
	MFP_ADDR(DF_IO9, 0x1c),
	MFP_ADDR(DF_IO10, 0x18),
	MFP_ADDR(DF_IO11, 0x14),
	MFP_ADDR(DF_IO12, 0x10),
	MFP_ADDR(DF_IO13, 0xc),
	MFP_ADDR(DF_IO14, 0x8),
	MFP_ADDR(DF_IO15, 0x4),

	MFP_ADDR(DF_nCS0_SM_nCS2, 0x44),
	MFP_ADDR(DF_nCS1_SM_nCS3, 0x48),
	MFP_ADDR(SM_nCS0, 0x4c),
	MFP_ADDR(SM_nCS1, 0x50),
	MFP_ADDR(DF_WEn, 0x54),
	MFP_ADDR(DF_REn, 0x58),
	MFP_ADDR(DF_CLE_SM_OEn, 0x5c),
	MFP_ADDR(DF_ALE_SM_WEn, 0x60),
	MFP_ADDR(SM_SCLK, 0x64),
	MFP_ADDR(DF_RDY0, 0x68),
	MFP_ADDR(SM_BE0, 0x6c),
	MFP_ADDR(SM_BE1, 0x70),
	MFP_ADDR(SM_ADV, 0x74),
	MFP_ADDR(DF_RDY1, 0x78),
	MFP_ADDR(SM_ADVMUX, 0x7c),
	MFP_ADDR(SM_RDY, 0x80),


	MFP_ADDR_END,
};

void
dummy_delay(unsigned  int delay)
{
    volatile unsigned int i;
    for(i = 0; i < delay; i++);
}

unsigned long initdram (int board_type)
{
    return (PHYS_SDRAM_SIZE_DEC*1024*1024);
}

int board_init (void)
{
    DECLARE_GLOBAL_DATA_PTR;
    volatile unsigned int reg;

    /* set SEL_MRVL_ID bit in MOHAWK_CPU_CONF register - Ning */
    *(volatile unsigned int *)0xD4282c08 = *(volatile unsigned int *)0xD4282c08 | 0x100;

    /* TODO: Should only enable the bits that make sense at this point */
    /* Turn on clock gating (PMUM_CCGR) */
    *(volatile unsigned int *)0xd4050024 = 0xffffffff;
    /* Turn on clock gating (PMUM_ACGR) */
    *(volatile unsigned int *)0xD4051024 = 0xffffffff; 


    /* temporary work-around to fix DDR 156 issue -- Ning */
    *(volatile unsigned int *)0xb0000230 = 0x20007c04;
    *(volatile unsigned int *)0xb0000e10 = 0x20007c04;
    *(volatile unsigned int *)0xb0000e20 = 0x20007c04;
    *(volatile unsigned int *)0xb0000e30 = 0x20007c04;

    *(volatile unsigned int *)0xd428284c = 0x00000008;
    *(volatile unsigned int *)0xd428284c = 0x00000009;
    *(volatile unsigned int *)0xd428284c = 0x00000019;
    *(volatile unsigned int *)0xd428284c = 0x0000001b;

    /* Below is for IRE */
    *(volatile unsigned int *)0xd4282848 = 0x00000008;
    *(volatile unsigned int *)0xd4282848 = 0x00000009;
    /* UART2 clk */
    *(volatile unsigned int *) 0xD4015000 = 0x13; 

    /* TWSI clk*/
    *(volatile unsigned int *) 0xD401502c = 0x3; 
    dummy_delay(99999);
	
    /*ssp1 reset and clock*/
    *(volatile unsigned int *)0xd405000c=0x80E50FFF;
    *(volatile unsigned int *)0xd4050040=0xf81902b3;
    *(volatile unsigned int *)0xd4050044=0xf81902b3;   
    *(volatile unsigned int *)0xd4015020=0x00000006;
    dummy_delay(99999);
    *(volatile unsigned int *)0xd4015020=0x00000002;

    /* enable GPIO clock */
    *(volatile unsigned int *)0xd4015008=0x03;

    /* configure GPIO80 as input (ether_irq) */
    *(volatile unsigned int *)(0xd4019008 + 0x60)=0x10000;
    /* enable GPIO80 rising edge interrupt */
    *(volatile unsigned int *)(0xd4019008 + 0x6c)=0x10000;
    /* unmask GPIO80 interrupt */
    *(volatile unsigned int *)(0xd4019008 + 0x9c)=0x10000;

    /* Core CPU settings */
    __asm__ __volatile__ ("                     \n\
         @ Enable MMX                           \n\
	 @mrc     p15, 0, r0, c15, c1, 0        \n\
	 @orr     r0,  r0, #3	@ cp0, cp1      \n\
	 @mcr     p15, 0, r0, c15, c1, 0        \n\
                                                \n\
	 @ Allow access of performance counters \n\
         @ (PMU) from user space                \n\
	 mov	r0, #0x1                        \n\
	 mrc	p15, 0, r0, c15, c9, 0          \n\
	 orr	r0, r0, #0x1                    \n\
	 mcr	p15, 0, r0, c15, c9, 0          \n\
                                                \n\
	 @ Ensure branch prediction is          \n\
         @ enabled - BPU (Default)              \n\
	 mrc	p15, 0, r0, c1, c0, 0           \n\
	 orr	r0, r0, #(1<<11)                \n\
	 mcr	p15, 0, r0, c1, c0, 0           \n\
         " : "=r" (reg));

    gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
    gd->bd->bi_dram[0].size  = PHYS_SDRAM_1_SIZE;

    /* arch number of TavorEVB Board */
    gd->bd->bi_arch_number = 1827;  // MACH_TYPE_TAVOREVB
    
    /* adress of boot parameters */
    gd->bd->bi_boot_params = 0x00000100;
    gd->baudrate = CONFIG_BAUDRATE;

    BU_REG_WRITE( APBC_AIB_CLK_RST, APBC_AIB_CLK_RST_FNCLK  |
		    APBC_AIB_CLK_RST_APBCLK );

    BU_REG_WRITE( SMC_CSDFICFGX, 0x51890009 );              // SMC_CSDFICFG0
    BU_REG_WRITE( SMC_CSADRMAPX, 0x10000F00 );              // SMC_CSADRMAP0
    BU_REG_WRITE(0xd4283894, 0x1000008 ); // SMC_CSDFICFG1s, A/D mode

    /*configure for MFP*/
    pxa3xx_init_mfp();
    pxa3xx_mfp_init_addr(pxa910_910_mfp_addr_map);

    pxa3xx_mfp_config(ARRAY_AND_SIZE(pxa910_tavorevb_mfp_cfg));
    pxa3xx_mfp_set_pull(MFP_CFG_PIN(GPIO47_UART2_RXD), MFP_PULL_HIGH);
    pxa3xx_mfp_set_pull(MFP_CFG_PIN(GPIO48_UART2_TXD), MFP_PULL_HIGH);
    pxa3xx_mfp_set_pull(MFP_CFG_PIN(DF_RDY0_DF_RDY0), MFP_PULL_HIGH);

    /* adjust ddr setting for better performance -- Ning */
    *(volatile unsigned int *)0xb0000140=0x20004411; /* MCU_PHY_CONTROL_3 */
    *(volatile unsigned int *)0xb0000100=0x000B0F81; /* MMU_MMAP0 */
    *(volatile unsigned int *)0xb0000050=0x484904C2; /* MCU_SDRAM_TIMING_1 */
    *(volatile unsigned int *)0xb0000060=0x32330122; /* MCU_SDRAM_TIMING_2 */
    *(volatile unsigned int *)0xb00001c0=0x1282a000; /* MCU_SDRAM_TIMING_4 */
    *(volatile unsigned int *)0xb00001d0=0x0ff00331; /* MCU_PHY_CONTROL_7 */
    *(volatile unsigned int *)0xb00001e0=0x07700330; /* MCU_PHY_CONTROL_8 */
    //*(volatile unsigned int *)0xb00001a0=0x0080c011; /* MCU_SDRAM_CONTROL_4 */
    *(volatile unsigned int *)0xb0000090=0x00080000; /* MCU_SDRAM_CONTROL_2 */
    *(volatile unsigned int *)0xb0000650=0x000700A1; /* MCU_SDRAM_TIMING_5 */

    /* Works at 1.203V- 1.23V 312MHz core 156MHz ddr */
    *(volatile unsigned int *)0xb00001f0=0xC0000077; /* MCU_PHY_CONTROL_9 */
    *(volatile unsigned int *)0xb0000200=0x0011210c; /* MCU_PHY_CONTROL_10 */
    *(volatile unsigned int *)0xb0000120=0x00000001; /* MCU_USER_INITIATED_COMMAND */

    /* Following os for 624MHz/156MHz(cpu/ddr) with DLL+ */
    *(volatile unsigned int *)0xb0000230=0x203C7D06; /* MCU_PHY_CONTROL_13 */
    *(volatile unsigned int *)0xb0000e10=0x203C7D06; /* MCU_PHY_DLL_CONTROL_1 */
    *(volatile unsigned int *)0xb0000e20=0x203C7D06; /* MCU_PHY_DLL_CONTROL_2 */
    *(volatile unsigned int *)0xb0000e30=0x203C7D06; /* MCU_PHY_DLL_CONTROL_3 */
    /* *(volatile unsigned int *)0xb0000240=0x20000000; */ /* MCU_PHY_CONTROL_14 */

    /* Allow the freq chng DDR 156MHz CPU 312MHz while cp in reset */
    *(volatile unsigned int *)0xd4050008=0x20800000;
    *(volatile unsigned int *)0xd4282888=0x00060009;
    *(volatile unsigned int *)0xd4282804=0x00fd96d8;
    *(volatile unsigned int *)0xd4282800=0x08fd96d9;
    *(volatile unsigned int *)0xd4282804=0x0ffd96d8;
    reg = *(volatile unsigned int *)0xd4282804;

    return 0;
}

int misc_init_r (void)
{
        char *env;
                                                                                                                                               
	/* set voltage to 1300mV by default */
	set_volt(1300);
	dummy_delay(99999);
	set_volt(1300);
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

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

#include <spartan6.h>

#define PXA168_LCD_BASE 0xD420B000
#define LCD_CFG_GRA_START_ADDR0 0xF4
#define LCD_SPU_DMA_CTRL0 0x190
#define LCD_SPU_DUMB_CTRL 0x1B8

#define APBC_GPIO_CLK_RST 0xD4015008

#define DDR_HACK 1
#define PLL_HACK

extern void aspen_init();
extern void BBU_LCD_init();
extern void BBU_TPO_INIT();
extern void BBU_I2C_Init();
extern void BBU_Timer_init();
extern int reconfigure_plls(void);

/* ------------------------------------------------------------------------- */
extern int fpga_pgm_fn( int assert_pgm, int flush, int cookie );
extern int fpga_init_fn( int cookie );
extern int fpga_done_fn( int cookie );
extern int fpga_clk_fn( int assert_clk, int flush, int cookie );
extern int fpga_wr_fn( int assert_write, int flush, int cookie );
extern int fpga_pre_config_fn( int cookie );
extern int fpga_post_config_fn( int cookie );

Xilinx_Spartan6_Slave_Serial_fns silvermoon_fpga_fns = {
	fpga_pre_config_fn,
	fpga_pgm_fn,
	fpga_clk_fn,
	fpga_init_fn,
	fpga_done_fn,
	fpga_wr_fn,
	fpga_post_config_fn,
};

Xilinx_desc fpga[CONFIG_FPGA_COUNT] = {
	XILINX_XC6SLX9_DESC(
		slave_serial_ssp,
		(void *)&silvermoon_fpga_fns,
		0)
};
/* ------------------------------------------------------------------------- */

static mfp_cfg_t aspenite_mfp_cfg[] __initdata = {
	MFP_CFG(GPIO109, AF0),

	/* Ensure UART is configured */
	MFP_CFG(GPIO108, AF1),
	MFP_CFG(GPIO107, AF1),

	/* Ensure CP UART is configured */
	MFP_CFG(GPIO98, AF2),
	MFP_CFG(GPIO99, AF2),

#if !defined(CHUMBY_CONFIG_platform)
	/* FFUART */
	GPIO108_FFRXD,
	GPIO107_FFTXD,

	/* DFC */
	GPIO0_DFI_D15,
	GPIO1_DFI_D14,
	GPIO2_DFI_D13,
	GPIO3_DFI_D12,
	GPIO4_DFI_D11,
	GPIO5_DFI_D10,
	GPIO6_DFI_D9,
	GPIO7_DFI_D8,
	GPIO8_DFI_D7,
	GPIO9_DFI_D6,
	GPIO10_DFI_D5,
	GPIO11_DFI_D4,
	GPIO12_DFI_D3,
	GPIO13_DFI_D2,
	GPIO14_DFI_D1,
	GPIO15_DFI_D0,

	GPIO16_ND_CS0,
	GPIO17_ND_nWE,
	GPIO19_CF_nCE1,
	GPIO20_CF_nCE2,
	GPIO26_ND_RnB1,

	GPIO28_CF_RDY,
	GPIO29_CF_STSCH,
	GPIO30_CF_nREG,
	GPIO31_CF_nIOIS16,
	GPIO32_CF_nCD1,
	GPIO33_CF_nCD2,

	/* i2c bus */
	GPIO87_PI2C_SDA,
	GPIO88_PI2C_SCL,
	GPIO105_CI2C_SDA,
	GPIO106_CI2C_SCL,
#endif
};

static struct pxa3xx_mfp_addr_map pxa910_168_mfp_addr_map[] __initdata = {

	MFP_ADDR_X(GPIO0, GPIO36, 0x4c),
	MFP_ADDR_X(GPIO37, GPIO55, 0x0),
	MFP_ADDR_X(GPIO56, GPIO123, 0xe0),
	MFP_ADDR_X(GPIO124, GPIO127, 0xf4),

	MFP_ADDR_END,
};

/*---------------------------------------------------------------*/
// GPIO designations:
// 118 - FPGA_CCLK  output
// 119 - RESET_N    output
// 120 - INIT_N     output
// 121 - DIN        output
// 97  - DONE       input

#define FPGA_SM_MFP_118 *((volatile unsigned int *)0xd401e1d8)
#define FPGA_SM_MFP_119 *((volatile unsigned int *)0xd401e1dc)
#define FPGA_SM_MFP_120 *((volatile unsigned int *)0xd401e1e0)
#define FPGA_SM_MFP_121 *((volatile unsigned int *)0xd401e1e4)
#define FPGA_SM_MFP_97  *((volatile unsigned int *)0xd401e184)
#define FPGA_SM_MFP_96  *((volatile unsigned int *)0xd401e180)

// bits 122-96
#define FPGA_SM_GP_96_MSK  (0x1)
#define FPGA_SM_GP_97_MSK  (0x2)
#define FPGA_SM_GP_118_MSK (0x00400000)
#define FPGA_SM_GP_119_MSK (0x00800000)
#define FPGA_SM_GP_120_MSK (0x01000000)
#define FPGA_SM_GP_121_MSK (0x02000000)

#define FPGA_SM_LED_MSK    FPGA_SM_GP_96_MSK
#define FPGA_SM_CCLK_MSK   FPGA_SM_GP_118_MSK
#define FPGA_SM_RESET_N_MSK FPGA_SM_GP_119_MSK
#define FPGA_SM_INIT_N_MSK FPGA_SM_GP_120_MSK
#define FPGA_SM_DIN_MSK    FPGA_SM_GP_121_MSK
#define FPGA_SM_DONE_MSK   FPGA_SM_GP_97_MSK

// we neatly fit within a port config register, so drop the suffix and just
// make our own macro specific to the port
#define FPGA_SM_GPIO_GPDR *((volatile unsigned int *)0xd401910c)

#define FPGA_SM_GPIO_GPSR *((volatile unsigned int *)0xd4019118)
#define FPGA_SM_GPIO_GPCR *((volatile unsigned int *)0xd4019124)
#define FPGA_SM_GPIO_GPLR *((volatile unsigned int *)0xd4019100)


/*
 * Set the FPGA's active-low SelectMap program line to the specified level
 */
int fpga_pgm_fn(int assert, int flush, int cookie)
{
  printf("%s:%d: FPGA PROGRAM ",
	__FUNCTION__, __LINE__);
  
  if (assert) {
    FPGA_SM_GPIO_GPCR = FPGA_SM_RESET_N_MSK;
    printf("asserted\n");
  } else {
    FPGA_SM_GPIO_GPSR = FPGA_SM_RESET_N_MSK;
    printf("deasserted\n");
  }
  return assert;
}


/*
 * Test the state of the active-low FPGA INIT line.  Return 1 on INIT
 * asserted (low).
 */
int fpga_init_fn(int cookie)
{
  if ( FPGA_SM_GPIO_GPLR & FPGA_SM_INIT_N_MSK )
    return 0;
  else
    return 1;
}

/*
 * Test the state of the active-high FPGA DONE pin
 */
int fpga_done_fn(int cookie)
{
  if ( FPGA_SM_GPIO_GPLR & FPGA_SM_DONE_MSK )
    return 1;
  else
    return 0;
}

/*
 * FPGA pre-configuration function. Just make sure that
 * FPGA reset is asserted to keep the FPGA from starting up after
 * configuration.
 */
int fpga_pre_config_fn(int cookie)
{
	printf("%s:%d: FPGA pre-configuration\n", __FUNCTION__, __LINE__);

	// pre-config only valid for bit-bang. SSP pre-config is hard-coded.
	// we'll set up all our GPIOs here
	//	printf( "fpga: setting up MFPs\n" );
	FPGA_SM_MFP_118 &= ~0x7;
	FPGA_SM_MFP_119 &= ~0x7;
	FPGA_SM_MFP_120 &= ~0x7;
	FPGA_SM_MFP_121 &= ~0x7;
	FPGA_SM_MFP_97 &= ~0x7;

	//	printf( "fpga: setting up GPIOs\n" );
	// set inputs
	FPGA_SM_GPIO_GPDR = FPGA_SM_GPIO_GPDR & ~FPGA_SM_DONE_MSK;
	FPGA_SM_GPIO_GPDR = FPGA_SM_GPIO_GPDR & ~FPGA_SM_INIT_N_MSK;
	// set outputs
	FPGA_SM_GPIO_GPDR |= (FPGA_SM_DIN_MSK | FPGA_SM_RESET_N_MSK | FPGA_SM_CCLK_MSK);
	
	//	printf( "fpga: initing GPIOs\n" );
	// set all the outputs to 0
	FPGA_SM_GPIO_GPCR = (FPGA_SM_CCLK_MSK | FPGA_SM_RESET_N_MSK | FPGA_SM_DIN_MSK);
	// so fpga is now in reset, we exit
	return 0;
}


/*
 * FPGA post configuration function. Blip the FPGA reset line and then see if
 * the FPGA appears to be running.
 */
int fpga_post_config_fn(int cookie)
{

	printf("%s:%d: FPGA post configuration\n", __FUNCTION__, __LINE__);
	// nothin man, we are all cool.
	return 0;
}


int fpga_clk_fn(int assert_clk, int flush, int cookie)
{
	if (assert_clk)
	  FPGA_SM_GPIO_GPSR = FPGA_SM_CCLK_MSK;
	else
	  FPGA_SM_GPIO_GPCR = FPGA_SM_CCLK_MSK;

	return assert_clk;
}


int fpga_wr_fn(int assert_write, int flush, int cookie)
{
	if (assert_write)
	  FPGA_SM_GPIO_GPSR = FPGA_SM_DIN_MSK;
	else
	  FPGA_SM_GPIO_GPCR = FPGA_SM_DIN_MSK;

	return assert_write;
}
/*---------------------------------------------------------------*/


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

int cpu_is_aspen_s0(void)
{
	volatile unsigned int reg;
	reg = *(volatile unsigned int *)0xD4282c00;
	if((reg & 0xffff) == 0xa168) {
		return 0;
	}
	else  {
		return 1;
	}
}

#if defined(CHUMBY_CONFIG_platform)

void print_dramconfig_boot(int print) {
  int i;
  volatile unsigned int *addr;
  volatile int dummy;

  if( print )
    printf( "DDR2 registers:\n" );
  for( i = 0; i < 0x290; i += 0x10 ) {
    addr = (unsigned int *) (0xb0000000 + i);
    if( print )
      printf( "%08x: %08x\n", (unsigned int) addr, *addr );
    else
      dummy = (volatile unsigned int *) addr;
  }

  addr = (volatile unsigned int *) (0xb0000000 + 0x650);
  if( print )
    printf( "%08x: %08x\n", (unsigned int) addr, *addr );
  else
    dummy = (volatile unsigned int *) addr;
  addr = (unsigned int *) (0xb0000000 + 0x660);
  if( print )
    printf( "%08x: %08x\n", (unsigned int) addr, *addr );
  else
    dummy = (volatile unsigned int *) addr;
  addr = (unsigned int *) (0xb0000000 + 0x760);
  if( print )
    printf( "%08x: %08x\n", (unsigned int) addr, *addr );
  else
    dummy = (volatile unsigned int *) addr;
  addr = (unsigned int *) (0xb0000000 + 0x770);
  if( print )
    printf( "%08x: %08x\n", (unsigned int) addr, *addr );
  else
    dummy = (volatile unsigned int *) addr;
  addr = (unsigned int *) (0xb0000000 + 0x7d0);
  if( print )
    printf( "%08x: %08x\n", (unsigned int) addr, *addr );
  else
    dummy = (volatile unsigned int *) addr;
  addr = (unsigned int *) (0xb0000000 + 0x7e0);
  if( print )
    printf( "%08x: %08x\n", (unsigned int) addr, *addr );
  else
    dummy = (volatile unsigned int *) addr;
  addr = (unsigned int *) (0xb0000000 + 0xe10);
  if( print )
    printf( "%08x: %08x\n", (unsigned int) addr, *addr );
  else
    dummy = (volatile unsigned int *) addr;
  addr = (unsigned int *) (0xb0000000 + 0xe80);
  if( print )
    printf( "%08x: %08x\n", (unsigned int) addr, *addr );
  else
    dummy = (volatile unsigned int *) addr;

  if( print )
    printf( "DDR PHY DLL delay values: %04x\n", ((*(unsigned int *) 0xb0000240) & 0xFF00) >> 8);
  if( print )
    printf( "==done==\n" );
}

void update_dramconfig_boot() {
  int i;
  volatile unsigned int *addr;
  volatile int dummy;

  *(unsigned long *)0xb0000080 = 0x00000000;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb0000140 = 0x20004433;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb0000210 = 0x00300002;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb0000100 = 0x000B0001;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb0000050 = 0x4CD600C8;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb0000060 = 0x64660332;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb0000190 =  0x20C83752;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb00001c0 = 0x353AA0A0;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb0000650 = 0x00100121;
  print_dramconfig_boot(0);

  // Prioritize perhipherals above memory and data accesses.
  *(unsigned long *)0xb0000280 = 0x0101010e;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb00000f0 = 0xc0000000;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb0000020 = 0x00066330;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb00001a0 = 0x20818004;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb0000090 = 0x00080000;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb0000760 = 0x00000001;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb0000770 = 0x02000002;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb0000240 = 0x80000000;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb0000230 = 0x00000108;
  print_dramconfig_boot(0);
  *(unsigned long *)0xb0000e10 = 0x00000100;
  print_dramconfig_boot(0);
  //      !set value *(unsigned long *)0xb0000230 = 0x004C010a
  //      !set value *(unsigned long *)0xb0000e10 = 0x004C0102

  //set value *(unsigned long *)0xb00001F0 = 0x00000077
  *(unsigned long *)0xb00001F0 = 0x00000011;
  print_dramconfig_boot(0);

  *(unsigned long *)0xb00001D0 = 0x177C2779;
  print_dramconfig_boot(0);
  *(unsigned long *)0xb00001E0 = 0x03300330;
  print_dramconfig_boot(0);


  *(unsigned long *)0xb0000240 = (*(unsigned long *)0xb0000240 | 0x20000000);
  print_dramconfig_boot(0);
  addr = (unsigned long *) 0x0;
  for( i = 0; i < 5120; i ++ ) {
    dummy = *(addr + i);
  }
  
  *(unsigned long *)0xb0000240 = (*(unsigned long *)0xb0000240 & 0xDFFFFFFF);
  print_dramconfig_boot(0);
  addr = (unsigned long *) 0x0;
  for( i = 0; i < 5120; i ++ ) {
    dummy = *(addr + i);
  }

  *(unsigned long *)0xb0000230 = (*(unsigned long *)0xb0000230 & 0xFFFFFFFB);
  print_dramconfig_boot(0);
  addr = (unsigned long *) 0x0;
  for( i = 0; i < 5120; i ++ ) {
    dummy = *(addr + i);
  }


  *(unsigned long *)0xb0000240 = (*(unsigned long *)0xb0000240 | 0x08000000);
  print_dramconfig_boot(0);
  addr = (unsigned long *) 0x0;
  for( i = 0; i < 5120; i ++ ) {
    dummy = *(addr + i);
  }

  *(unsigned long *)0xb0000240 = (*(unsigned long *)0xb0000240 | 0x08000000);
  print_dramconfig_boot(0);
  for( i = 0; i < 5120; i ++ ) {
    dummy = *(addr + i);
  }

  *(unsigned long *)0xb0000120 = 0x00000001;
  print_dramconfig_boot(0);

  addr = (unsigned long *) 0x0;
  for( i = 0; i < 5120; i ++ ) {
    print_dramconfig_boot(0);
    dummy = *(addr + i);
  }
  
  printf( "DDR PHY DLL delay values: %04x\n", ((*(unsigned int *) 0xb0000240) & 0xFF00) >> 8);

}


// Variables initialized by board_init for later informational display
static int _aspen_a0_detected = -1;
static unsigned int _mohawk_cpu_conf_before = 0;
static unsigned int _arm_cpuid_before = 0;
static unsigned int _mohawk_cpu_conf_after = 0;
static unsigned int _arm_cpuid_after = 0;
#endif
int board_init (void)
{
    DECLARE_GLOBAL_DATA_PTR;
    volatile unsigned int reg;
    int i;


#ifdef PLL_HACK
	reconfigure_plls();
#endif

    aspen_init();

    if (!cpu_is_aspen_s0()) {
	*(volatile unsigned int *)0xd401e0f0 = *(volatile unsigned int *)0xd401e0f0 | 0xC00; /* VDD_IO0 = VDD_IO1 = 3.3V*/
	*(unsigned int *)0xd401e0f4 = *(volatile unsigned int *)0xd401e0f4 | 0xC00; /* VDD_IO2 = VDD_IO3 = 3.3V */
	*(unsigned int *)0xd401e0f8 = *(volatile unsigned int *)0xd401e0f8 | 0x800; /* VDD_IO4 = 3.3V */
#if defined(CHUMBY_CONFIG_platform)
	_aspen_a0_detected = 1;
#endif
    }
    else {
	*(volatile unsigned int *)0xd401e0f0 = *(volatile unsigned int *)0xd401e0f0 & ~(0xC00); /* VDD_IO0 = VDD_IO1 = 3.3V*/
        *(unsigned int *)0xd401e0f4 = *(volatile unsigned int *)0xd401e0f4 & ~(0xC00); /* VDD_IO2 = VDD_IO3 = 3.3V */
        *(unsigned int *)0xd401e0f8 = *(volatile unsigned int *)0xd401e0f8 & ~(0x800); /* VDD_IO4 = 3.3V */
#if defined(CHUMBY_CONFIG_platform)
	_aspen_a0_detected = 0;
#endif
    }

    BBU_Timer_init();
    BBU_I2C_Init();
    BBU_LCD_init();
    BBU_TPO_INIT();

	// Marvell CPUID is needed for cpu_is_pxa168() to work properly
	// but causes xdb3.3 to not work
#if defined(CHUMBY_CONFIGNAME_silvermoon_a0) || !defined(CHUMBY_CONFIG_platform)
#if defined(CHUMBY_CONFIG_platform)
	_mohawk_cpu_conf_before = *(volatile unsigned int *)0xD4282c08;
	__asm__ __volatile__ ("mrc        p15, 0, %0, c0, c0, 0"
                    : "=r" (_arm_cpuid_before)
                    :
                    : "cc");
#endif
    /* set SEL_MRVL_ID bit in MOHAWK_CPU_CONF register - Ning */
    *(volatile unsigned int *)0xD4282c08 = *(volatile unsigned int *)0xD4282c08 | 0x100;
#if defined(CHUMBY_CONFIG_platform)
	_mohawk_cpu_conf_after = *(volatile unsigned int *)0xD4282c08;
	__asm__ __volatile__ ("mrc        p15, 0, %0, c0, c0, 0"
                    : "=r" (_arm_cpuid_after)
                    :
                    : "cc");
#endif
#endif

    /* ensure L2 cache is not mapped as SRAM */
    *(unsigned int *)0xd4282c08 &= ~(1<<4);

    /* lowering RTC setting could improve Vmin */
    *(unsigned int *)0xd4282c10 = 0x16b5ad6d;
    *(unsigned int *)0xd4282c18 = 0x2d5b56b5;

    /* Enable clocks */
    *(unsigned int *)0xD4051024 = 0xffffffff;
    /* enable UART2 clock */
    *(unsigned int *)0xD4015000 = 0x13;

	/* Enable L2 cache */
	__asm__ __volatile__ (
			"@ invalidate L2 cache\n\t"
			"mov %0, #0\n\t"
			"mcr p15, 1, %0, c7, c7, 0\n\t"
			"@ L2 - Outer writeback\n\t"
			"@ enable L2 cache\n\t"
			"mrc p15, 0, %0, c1, c0, 0\n\t"
			"orr %0, %0, #(1 << 26)\n\t"
			"mcr p15, 0, %0, c1, c0, 0\n\t"
			"@ invalidate L2 cache\n\t"
			"bic %0, %0, #31\n\t"
			"mcr p15, 1, %0, c7, c7, 0\n\t"
           :
           : "r"(0x1)
    );



    /* enable PMU user access */
    __asm__ __volatile__ (
           "mcr     p15, 0, %0, c15, c9, 0\n\t"
           :
           : "r"(0x1)
    );

    gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
    gd->bd->bi_dram[0].size  = initdram(0);

#ifdef CHUMBY_CONFIG_platform
    /* arch number of Chumby Silvermoon board */
    gd->bd->bi_arch_number = 1829;  // MACH_ASPENITE
    //gd->bd->bi_arch_number = 2222;  // MACH_CHUMBY_SILVERMOON
#else
#ifdef CONFIG_ASPENITE
    /* arch number of Aspenite Board */
    gd->bd->bi_arch_number = 2043;  // MACH_ASPENITE
#else
#ifdef CONFIG_ZYLONITE2
    /* arch number of Zylonite2 Board */
    gd->bd->bi_arch_number = 2042;  // MACH_ZYLONITE2
#endif
#endif
#endif

    /* adress of boot parameters */
    gd->bd->bi_boot_params = 0x00000100;
    gd->baudrate = CONFIG_BAUDRATE;
    icache_enable ();

#if defined(CHUMBY_CONFIG_platform)
    /* configure for MFP */
    pxa3xx_init_mfp();
    pxa3xx_mfp_init_addr(pxa910_168_mfp_addr_map);
    pxa3xx_mfp_config(ARRAY_AND_SIZE(aspenite_mfp_cfg));
#endif

#if defined(CHUMBY_CONFIG_platform)
	//ensure that UART1 input is working...
	//set Pin Direction Set regster (GPIO_SDR) for GPIO109 to Output 
	*(unsigned int *)0xd4019154 =  (1 << (109-96));
#endif


#if defined(CHUMBY_CONFIG_platform)
	/* Disable USB card.  Will be re-enabled by kernel */
	{
		/* GPIO4_PCR: Clear GPIO 101 and set it low */
		*(unsigned int *)0xd4019124 = 0x20;
		/* GPIO4_PDR: Set GPIO 101 to be an output */
		*(unsigned int *)0xd401910c |= 0x20;
	}
#endif

#if DDR_HACK
    printf( "updating DDR config (again)...\n" );
    update_dramconfig_boot(); // really force the NTIM settings in
    printf( "reading back DDR config..." );
    for( i = 0; i < 100; i++ ) {
      print_dramconfig_boot(0);  // read the registers a lot to try and stabilize the settings (some silicon bug...)
    }

#if defined(CONFIG_FPGA)
    printf("%s:%d: Initialize FPGA interface (relocation offset = 0x%.8lx)\n",
	  __FUNCTION__, __LINE__, gd->reloc_off);
    fpga_init(gd->reloc_off);

    // fpga_serialslave_init (); // probably should migrate stuff from spartan6 to here...
    printf("%s:%d: Adding fpga 0\n", __FUNCTION__, __LINE__);
    fpga_add (fpga_xilinx, &fpga[0]);
#endif    

    printf( "done.\n" );
#endif

    return 0;
}

int misc_init_r (void)
{
        char *env;

#if 0
        /* primary network interface */
        env = getenv("ethprime");
        if(!env)
               setenv("ethprime","eth0");

        /* default usb mode */
	env = getenv("usbMode");
        if(!env)
               setenv("usbMode","host");
#endif

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

#if 0
        env = getenv("standalone_mtd");
        if(!env)
                setenv("standalone_mtd","fsload $(default_load_addr) $(image_name);setenv bootargs $(bootargs) root=/dev/mtdblock0 rw rootfstype=jffs2 ip=$(ipaddr):$(serverip)$(bootargs_end);bootm $(default_load_addr);");

        env = getenv("standalone_initrd");
        if(!env)
                setenv("standalone_initrd","fsload $(default_load_addr) $(image_name);fsload $(initrd_load_addr) $(initrd_name);setenv bootargs $(bootargs) root=/dev/ram0 rw initrd=0x$(initrd_load_addr),0x$(initrd_size) ip=$(ipaddr):$(serverip)$(bootargs_end); bootm $(default_load_addr);");
#endif

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
#if defined(CHUMBY_CONFIG_platform)
	// Via http://www.glassgiant.com/ascii/ 120 columns then cut out text
printf("                .:+I$ZOOOZZZZOO~       \n");
printf("             .7OZZZZZZZZZZZZZZZZ?      \n");
printf("            ,ZZZZZZZZZZZZZZZZZZZO      \n");
printf("            ZZZZZZ?ZZZZZZOI$ZZZZO.     \n");
printf("           .OZZZ=?OI~ZZZO Z~?ZZZO.     \n");
printf("            ZZZZ~7O7:OZZO~  ZZZZO      \n");
printf("            IZZZZOOOZZZZZZZZZZZZ$      \n");
printf("            =ZZZZZZZZZZZZZZZZZZZI      \n");
printf("            IZZZZZZZZZZZZZZZZZZZI      \n");
printf("     :IZOOZZZZZZZZZZZZZZZZZZZZZZZZ,    \n");
printf("  ,ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ   \n");
printf(".Z$$7?~,..~OZZZZZZZZZZZZZZZZZ7 =ZZZZO, \n");
printf("         IZZZZ$,OZZZZZZZO+ZZZO.  ,ZZZO \n");
printf("        ZZZZ~ ~ZZZZ ?ZZZ~ ?ZZZZ     .=+\n");
printf("       ,ZZO  .OZZZ  ZZZO   OZZZ        \n");
printf("       :Z$   IZZZ, .OZZZ   .OZZ.       \n");
printf("       .~    ZZO.  .OZZ+     $Z        \n");
printf("             Z+     $ZZ                \n");
printf("                    .?                 \n");

	// From http://www.network-science.de/ascii/ (font=larry3d)
printf("     /\\ \\                        /\\ \\                 \n");
printf("  ___\\ \\ \\___   __  __    ___ ___\\ \\ \\____  __  __    \n");
printf(" /'___\\ \\  _ `\\/\\ \\/\\ \\ /' __` __`\\ \\ '__`\\/\\ \\/\\ \\   \n");
printf("/\\ \\__/\\ \\ \\ \\ \\ \\ \\_\\ \\/\\ \\/\\ \\/\\ \\ \\ \\L\\ \\ \\ \\_\\ \\  \n");
printf("\\ \\____\\\\ \\_\\ \\_\\ \\____/\\ \\_\\ \\_\\ \\_\\ \\_,__/\\/`____ \\ \n");
printf(" \\/____/ \\/_/\\/_/\\/___/  \\/_/\\/_/\\/_/\\/___/  `/___/> \\\n");
printf("                                                /\\___/\n");
printf("[%-40s]      \\/__/ ", CHUMBY_CONFIG_name );
#else
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
#endif
   printf("\n\nMARVELL PXAXXX series.");
   printf("\nBased on 88SV331xV5 Core with ARM926 LE CPU.");
#if defined(CHUMBY_CONFIG_platform)
	printf( "\nStepping %s CPU conf %x->%x ARM CPUID %x->%x",
		_aspen_a0_detected == 1 ? "A0" : "pre-A0",
		_mohawk_cpu_conf_before, _mohawk_cpu_conf_after,
		_arm_cpuid_before, _arm_cpuid_after );
#endif
   printf("\nCode original base is u-boot-arm 2009.01-rc1.\n\n");

#if DDR_HACK
   // and one to grow on.
   print_dramconfig_boot(0);
   printf( "DDR PHY DLL delay values: %04x\n", ((*(unsigned int *) 0xb0000240) & 0xFF00) >> 8);
#endif

   return 0;
}

void lowlevel_init()
{
	return;
}

#ifdef CONFIG_PXASDH
int board_mmc_init(bd_t *bd)
{
	return pxa_sdh_init(bd);
}
#endif



static int (*handler_table[64])(int interrupt);


void register_irq_handler(int irq, int (*handler)(int irq)) {
    handler_table[irq] = handler;
}


void do_irq(void) {
    long *ICU_INT_STATUS_0 = (long *)0xd4282128;
    long *ICU_INT_STATUS_1 = (long *)0xd428212c;
    int i;

    for(i=0; i<32; i++)
        if((*ICU_INT_STATUS_0)&(1<<i) && handler_table[i]) {
			(handler_table[i])(i);
			return;
	}

    for(i=0; i<32; i++)
        if((*ICU_INT_STATUS_1)&(1<<i) && handler_table[i+32]) {
			(handler_table[i+32])(i+32);
			return;
		}
}

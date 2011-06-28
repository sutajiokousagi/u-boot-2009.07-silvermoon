/*
 * (C) Copyright 2002
 * Rich Ireland, Enterasys Networks, rireland@enterasys.com.
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
 *
 */

/*
 * Configuration support for Xilinx Spartan3 devices.  Based
 * on spartan2.c (Rich Ireland, rireland@enterasys.com).
 */

#include <common.h>		/* core U-Boot definitions */
#include <spartan6.h>		/* Spartan-6 device family */
#include <lcd.h>

/* Define FPGA_DEBUG to get debug printf's */
#define FPGA_DEBUG

#ifdef	FPGA_DEBUG
#define PRINTF(fmt,args...)	printf (fmt ,##args)
#else
#define PRINTF(fmt,args...)
#endif

#undef CONFIG_SYS_FPGA_CHECK_BUSY
#undef CONFIG_SYS_FPGA_PROG_FEEDBACK

/* Note: The assumption is that we cannot possibly run fast enough to
 * overrun the device (the Slave Parallel mode can free run at 50MHz).
 * If there is a need to operate slower, define CONFIG_FPGA_DELAY in
 * the board config file to slow things down.
 */
#ifndef CONFIG_FPGA_DELAY
#define CONFIG_FPGA_DELAY()
#endif

#ifndef CONFIG_SYS_FPGA_WAIT
#define CONFIG_SYS_FPGA_WAIT CONFIG_SYS_HZ/100	/* 10 ms */
#endif

static int Spartan6_ss_load( Xilinx_desc *desc, void *buf, size_t bsize );
static int Spartan6_ssp_load( Xilinx_desc *desc, void *buf, size_t bsize );
static int Spartan6_ss_dump( Xilinx_desc *desc, void *buf, size_t bsize );
/* static int Spartan6_ss_info( Xilinx_desc *desc ); */
static int Spartan6_ss_reloc( Xilinx_desc *desc, ulong reloc_offset );
static int Spartan6_ss_ssp_reloc (Xilinx_desc * desc, ulong reloc_offset);


/* ------------------------------------------------------------------------- */
/* Spartan-6 Generic Implementation */
int Spartan6_load (Xilinx_desc * desc, void *buf, size_t bsize)
{
	int ret_val = FPGA_FAIL;

	switch (desc->iface) {
	case slave_serial:
		PRINTF ("%s: Launching Slave Serial Load\n", __FUNCTION__);
		ret_val = Spartan6_ss_load (desc, buf, bsize);
		break;

	case slave_serial_ssp:
	  PRINTF( "%s: Launching slave serial load via SSP\n", __FUNCTION__);
	  ret_val = Spartan6_ssp_load(desc, buf, bsize);
	  break;

	default:
		printf ("%s: Unsupported interface type, %d\n",
				__FUNCTION__, desc->iface);
	}

	return ret_val;
}

int Spartan6_dump (Xilinx_desc * desc, void *buf, size_t bsize)
{
	int ret_val = FPGA_FAIL;

	switch (desc->iface) {
	case slave_serial:
		PRINTF ("%s: Launching Slave Serial Dump\n", __FUNCTION__);
		ret_val = Spartan6_ss_dump (desc, buf, bsize);
		break;

	default:
		printf ("%s: Unsupported interface type, %d\n",
				__FUNCTION__, desc->iface);
	}

	return ret_val;
}

int Spartan6_info( Xilinx_desc *desc )
{
	return FPGA_SUCCESS;
}


int Spartan6_reloc (Xilinx_desc * desc, ulong reloc_offset)
{
	int ret_val = FPGA_FAIL;	/* assume a failure */

	if (desc->family != Xilinx_Spartan6) {
		printf ("%s: Unsupported family type, %d\n",
				__FUNCTION__, desc->family);
		return FPGA_FAIL;
	} else
		switch (desc->iface) {
		case slave_serial:
			ret_val = Spartan6_ss_reloc (desc, reloc_offset);
			break;
		case slave_serial_ssp:
			ret_val = Spartan6_ss_ssp_reloc (desc, reloc_offset);
			break;

		default:
			printf ("%s: Unsupported interface type, %d\n",
					__FUNCTION__, desc->iface);
		}

	return ret_val;
}



/* ------------------------------------------------------------------------- */
#define CONFIG_SYS_FPGA_PROG_FEEDBACK

#define APBC_SSP2_CLK_RST *((volatile unsigned int *)0xd4015820)
#define APBC_SSP3_CLK_RST *((volatile unsigned int *)0xd401584c)

#define SSP2_SSPSP *((volatile unsigned int *)0xd401c02c)
#define SSP2_SSCR1 *((volatile unsigned int *)0xd401c004)
#define SSP2_SSCR0 *((volatile unsigned int *)0xd401c000)
#define SSP2_SSDR  *((volatile unsigned int *)0xd401c010)
#define SSP2_SSSR  *((volatile unsigned int *)0xd401c008)

#define SSP3_SSPSP *((volatile unsigned int *)0xd401f02c)
#define SSP3_SSCR1 *((volatile unsigned int *)0xd401f004)
#define SSP3_SSCR0 *((volatile unsigned int *)0xd401f000)
#define SSP3_SSDR  *((volatile unsigned int *)0xd401f010)

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
#define FPGA_SM_MFP_90  *((volatile unsigned int *)0xd401e168)

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


static int Spartan6_ssp_load( Xilinx_desc *desc, void *buf, size_t bsize) {
  int ret_val = FPGA_FAIL;	/* assume the worst */
  Xilinx_Spartan6_Slave_Serial_fns *fn = desc->iface_fns;
  int i;
  unsigned char val;

  size_t bytecount = 0;
  unsigned char *data = (unsigned char *) buf;
  int cookie = desc->cookie;	/* make a local copy */
  unsigned long ts;		/* timestamp */
  unsigned long ssp_word = 0;
  unsigned int temp;

  if( !fn ) {
    printf ("%s: NULL Interface function table!\n", __FUNCTION__);
    return ret_val;
  }
    
  PRINTF ("%s: Function Table:\n"
	  "ptr:\t0x%p\n"
	  "struct: 0x%p\n"
	  "pgm:\t0x%p\n"
	  "init:\t0x%p\n"
	  "clk:\t0x%p\n"
	  "wr:\t0x%p\n"
	  "done:\t0x%p\n\n",
	  __FUNCTION__, &fn, fn, fn->pgm, fn->init,
	  fn->clk, fn->wr, fn->done);
#ifdef CONFIG_SYS_FPGA_PROG_FEEDBACK
  printf ("Loading FPGA Device %d...\n", cookie);
#endif
  
  printf("SSP2 clock enable (52 MHz)\n");
  APBC_SSP2_CLK_RST = 0x33;
  APBC_SSP2_CLK_RST = 0x37; // reset the unit
  APBC_SSP2_CLK_RST = 0x33;
  
  printf("SSP3 clock enable (26 MHz)\n" );
  APBC_SSP3_CLK_RST = 0x23;
  APBC_SSP3_CLK_RST = 0x27; // reset the unit
  APBC_SSP3_CLK_RST = 0x23;

  // sspsp
  // 0 000 000 0 00 0 011111 0000000 00 000 0 0 00
  // 0x001f0000
  // bit 30:28 - edmystop 000
  // bit 27:26 - edmystrt 000
  // bit 25 fsrt - 0
  // bit 24:23 - dmystop 00
  // bit 21:16 - sfrmwdth 011111
  // bit 15:9 - sfrmdly 0000000
  // bit 8:7 dmystrt 00
  // bit 6:4 strtdly 000
  // bit 3 etds 0
  // bit 2 sfrmp 0
  // bit 1 scmode 00  data driven on falling, sampled on rising, idle state 0
  SSP2_SSPSP = 0x001f0000;

  // sspcr1
  // 0 0 0 1 00 0 0 0 0 0 0 0 0 0 0 0 0 0000 0000 0 0 0 0 0
  // 0x1000000
  // bit 31 ttelp - 0
  // bit 30 tte - 0
  // bit 29 ebcei - 0
  // bit 28 scfr - 1  clk only runs during data transfers
  // bit 25 sclkdir - 0  master mode, ssp port drives clk
  // bit 24 sfrmdir - 0 master mode
  // bit 23 rwot - 0
  // bit 22 trail - 0 
  // bit 21 tsre - 0 tx dma disabled
  // bit 20 rsre - 0 rx dma disabled
  // bit 19 tinte - 0 timeout disabled
  // bit 18 pinte - 0 trail int disabled
  // bit 16 ifs - 0  no inversion of frame
  // bit 15 strf - 0
  // bit 14 efwr - 0
  // bit 13:10 rft - 0
  // bit 9:6 tft - 0  txfifo trigger threshold
  // bit 5 mwds - 0
  // bit 4 sph - 0
  // bit 3 spo - 0
  // bit 2 lbm - 0
  // bit 1 tie - 0 tx fifo interrupt disabled
  // bit 0 rie - 0
  //  __raw_writel(0x10000000, SSP2_SSCR1);
  SSP2_SSCR1 = 0x100000c2;

  // sspcr0
  // 0 0 0 00 000 0 0 0 1 0000 0000 0000 0 0 11 1111
  // 0000 0000 0001 0000 0000 0000 0011 1111  0x0010003f
  // bit 31 mod - 0 normal ssp mode
  // bit 29 fpcke - 0 packing mode disabled
  // bit 26:24 frdc - 0
  // bit 23 tim - 1
  // bit 22 rim - 1
  // bit 20 edss - 1  select the larger size
  // bit 7 sse - 0  -- disable port initially
  // bit 5:4 frf - 11 -- programmable serial protocol
  // bit 3:0 dss 1111  -- 32 bits data
  SSP2_SSCR0 = 0x00d0003f;

  SSP2_SSCR0 = 0x00d000bf; // enable the port

  SSP3_SSPSP = 0x001f0000; // continuous clock on SSP3
  SSP3_SSCR1 = 0x01000000; // this is actually a bizarre configuration
  SSP3_SSCR0 = 0x0010003f; // but it works.
  SSP3_SSCR0 = 0x001000ff; 

  SSP3_SSDR = 0xdeadbeef;  // dummy write to trigger clock
  SSP3_SSDR = 0xdeadbeef;  // dummy write to trigger clock

  // gpios
  FPGA_SM_MFP_97 &= ~0x7; // done
  FPGA_SM_MFP_119 &= ~0x7; // init
  FPGA_SM_MFP_120 &= ~0x7; // reset

  // inputs
  FPGA_SM_GPIO_GPDR = FPGA_SM_GPIO_GPDR & ~FPGA_SM_DONE_MSK;
  FPGA_SM_GPIO_GPDR = FPGA_SM_GPIO_GPDR & ~FPGA_SM_INIT_N_MSK;

  // outputs
  FPGA_SM_GPIO_GPDR |= ( FPGA_SM_RESET_N_MSK );  

  // ssps
  FPGA_SM_MFP_118 &= ~0x7;
  FPGA_SM_MFP_121 &= ~0x7;
  FPGA_SM_MFP_118 |= 1; // ssp clk
  FPGA_SM_MFP_121 |= 1; // ssp txd

  FPGA_SM_MFP_90 &= ~0x7; // source a clock to the FPGA
  FPGA_SM_MFP_90 |= 3;

  //// lcd block -- tied to the FPGA because you don't want to have a mismatched setting
  *((volatile unsigned int *) 0xd428284c) = 0x0000007f;  // APMU_LCD_CLK_RES_CTRL
  //  *((volatile unsigned int *) 0xd420b0c0) = 0x07000000;  // LCD_SPU_DMA_START_ADDR_Y0
  *((volatile unsigned int *) 0xd420b0c0) = 0x01271000;  // LCD_SPU_DMA_START_ADDR_Y0
  //  *((volatile unsigned int *) 0xd420b0c4) = 0x0112d000;  // LCD_SPU_DMA_START_ADDR_U0
  //  *((volatile unsigned int *) 0xd420b0c8) = 0x0112d000;  // LCD_SPU_DMA_START_ADDR_V0
  *((volatile unsigned int *) 0xd420b0c4) = 0x01271000;  // LCD_SPU_DMA_START_ADDR_U0
  *((volatile unsigned int *) 0xd420b0c8) = 0x01271000;  // LCD_SPU_DMA_START_ADDR_V0
  *((volatile unsigned int *) 0xd420b0cc) = 0x00000000;  // LCD_CFG_DMA_START_ADDR_0
  *((volatile unsigned int *) 0xd420b0d0) = 0x00000000;  // LCD_SPU_DMA_START_ADDR_Y1
  *((volatile unsigned int *) 0xd420b0d4) = 0x00000000;  // LCD_SPU_DMA_START_ADDR_U1
  *((volatile unsigned int *) 0xd420b0d8) = 0x00000000;  // LCD_SPU_DMA_START_ADDR_V1
  *((volatile unsigned int *) 0xd420b0dc) = 0x00000000;  // LCD_CFG_DMA_START_ADDR_1
  *((volatile unsigned int *) 0xd420b0e0) = 0x00000a00;  // LCD_SPU_DMA_PITCH_YC
  *((volatile unsigned int *) 0xd420b0e4) = 0x00000000;  // LCD_SPU_DMA_PITCH_UV
  *((volatile unsigned int *) 0xd420b0e8) = 0x00000000;  // LCD_SPUT_DMA_OVSA_HPXL_VLN
  *((volatile unsigned int *) 0xd420b0ec) = 0x02d00500;  // LCD_SPU_DMA_HPXL_VLN
  *((volatile unsigned int *) 0xd420b0f0) = 0x02d00500;  // LCD_SPU_DZM_HPXL_VLN
  //  *((volatile unsigned int *) 0xd420b0f4) = 0x0112d000;  // LCD_CFG_GRA_START_ADDR0
  //  *((volatile unsigned int *) 0xd420b0f8) = 0x0112d000;  // LCD_CFG_GRA_START_ADDR1
  *((volatile unsigned int *) 0xd420b0f4) = 0x01271000;  // LCD_CFG_GRA_START_ADDR0
  *((volatile unsigned int *) 0xd420b0f8) = 0x01271000;  // LCD_CFG_GRA_START_ADDR1
  *((volatile unsigned int *) 0xd420b0fc) = 0x00000640;  // LCD_CFG_GRA_PITCH
  *((volatile unsigned int *) 0xd420b100) = 0x00000000;  // LCD_SPU_GRA_OVSA_HPXL_VLN
  *((volatile unsigned int *) 0xd420b104) = 0x02580320;  // LCD_SPU_GRA_HPXL_VLN
  *((volatile unsigned int *) 0xd420b108) = 0x02580320;  // LCD_SPU_GZM_HPXL_VLN
  *((volatile unsigned int *) 0xd420b10c) = 0x00000000;  // LCD_SPU_HWC_OVSA_HPXL_VLN
  *((volatile unsigned int *) 0xd420b110) = 0x00000000;  // LCD_SPU_HWC_HPXL_VLN
  *((volatile unsigned int *) 0xd420b114) = 0x02ee0672;  // LCD_SPUT_V_H_TOTAL
  *((volatile unsigned int *) 0xd420b118) = 0x02d00500;  // LCD_SPU_V_H_ACTIVE
  *((volatile unsigned int *) 0xd420b11c) = 0x006e00dc;  // LCD_SPU_H_PORCH
  *((volatile unsigned int *) 0xd420b120) = 0x00050014;  // LCD_SPU_V_PORCH
  *((volatile unsigned int *) 0xd420b124) = 0x00000000;  // LCD_SPU_BLANKCOLOR
  *((volatile unsigned int *) 0xd420b128) = 0x00000000;  // LCD_SPU_ALPHA_COLOR1
  *((volatile unsigned int *) 0xd420b12c) = 0x00000000;  // LCD_SPU_ALPHA_COLOR2
  *((volatile unsigned int *) 0xd420b130) = 0x00000000;  // LCD_SPU_COLORKEY_Y
  *((volatile unsigned int *) 0xd420b134) = 0x00000000;  // LCD_SPU_COLORKEY_U
  *((volatile unsigned int *) 0xd420b138) = 0x00000000;  // LCD_SPU_COLORKEY_V
  *((volatile unsigned int *) 0xd420b190) = 0x08041011;  // LCD_SPU_DMA_CTRL0
  *((volatile unsigned int *) 0xd420b194) = 0xa001ff81;  // LCD_SPU_DMA_CTRL1
  *((volatile unsigned int *) 0xd420b1a8) = 0x90000001;  // LCD_CFG_SCLK_DIV
  *((volatile unsigned int *) 0xd420b1b8) = 0x210ff003;  // LCD_SPU_DUMB_CTRL
  *((volatile unsigned int *) 0xd420b1c8) = 0x00000004;  // LCD_MISC_CNTL
  //// end lcd block

  /////// reset the fpga
  FPGA_SM_GPIO_GPCR |= FPGA_SM_RESET_N_MSK; // clear reset pin

  ts = get_timer (0);		/* get current time */
  while( get_timer (ts) < 10 )
    ;
  
  FPGA_SM_GPIO_GPSR |= FPGA_SM_RESET_N_MSK; // set reset pin
  
  /////// wait for fpga to get ready
  ts = get_timer (0);		/* get current time */
  while( get_timer (ts) < 100 )
    ;

  i = 0;
  while( i < bsize ) {
    temp = i + 4*8; // write in 8 words at a time, total fifo depth is 16
    // don't worry about buffer overread, fpga ignores extra stuff
    // skip the test to bum out a few instructions
    while( temp > i ) {
      ssp_word = be32_to_cpu(*((unsigned long *)(data + i)));
      
      i += 4;
      SSP2_SSDR = ssp_word;
    }
    
    /////////////// HERE
    // wait until fully empty
    while( (SSP2_SSSR & 0xF04) != 0x4 ) {

    }
  }
  PRINTF( "wrote %d bytes\n", i );

  ret_val = FPGA_SUCCESS;

  // toggle gpio 96 to indicate power-on at boot
  FPGA_SM_MFP_96 &= ~0x7; // select GPIO
  FPGA_SM_GPIO_GPDR |= FPGA_SM_LED_MSK; // set to output
  FPGA_SM_GPIO_GPCR |= FPGA_SM_LED_MSK; // now drive it low to activate the LED
  
  return ret_val;
}

static int Spartan6_ss_load (Xilinx_desc * desc, void *buf, size_t bsize)
{
	int ret_val = FPGA_FAIL;	/* assume the worst */
	Xilinx_Spartan6_Slave_Serial_fns *fn = desc->iface_fns;
	int i;
	unsigned char val;

	PRINTF ("%s: start with interface functions @ 0x%p\n",
			__FUNCTION__, fn);

	if (fn) {
		size_t bytecount = 0;
		unsigned char *data = (unsigned char *) buf;
		int cookie = desc->cookie;	/* make a local copy */
		unsigned long ts;		/* timestamp */

		PRINTF ("%s: Function Table:\n"
				"ptr:\t0x%p\n"
				"struct: 0x%p\n"
				"pgm:\t0x%p\n"
				"init:\t0x%p\n"
				"clk:\t0x%p\n"
				"wr:\t0x%p\n"
				"done:\t0x%p\n\n",
				__FUNCTION__, &fn, fn, fn->pgm, fn->init,
				fn->clk, fn->wr, fn->done);
#ifdef CONFIG_SYS_FPGA_PROG_FEEDBACK
		printf ("Loading FPGA Device %d...\n", cookie);
#endif

		/*
		 * Run the pre configuration function if there is one.
		 */
		if (*fn->pre) {
			(*fn->pre) (cookie);
		}

		/* Establish the initial state */
		(*fn->pgm) (TRUE, TRUE, cookie);	/* Assert the program, commit */

		/* Wait for INIT state (init low)                            */
		ts = get_timer (0);		/* get current time */
		while( get_timer (ts) < 10 )
		  ;

		/* Get ready for the burn */
		CONFIG_FPGA_DELAY ();
		(*fn->pgm) (FALSE, TRUE, cookie);	/* Deassert the program, commit */

		ts = get_timer (0);		/* get current time */
		while( get_timer (ts) < 100 )
		  ;

		/* Load the data */
		printf( "loading data from %08x\n", (unsigned int) data );
		printf( "First few bytes are:\n" );
		for( i = 0; i < 32; i++ ) {
		  printf( "%02x ", data[i] );
		}
		printf( "\n" );

		while (bytecount < bsize) {

			/* Xilinx detects an error if INIT goes low (active)
			   while DONE is low (inactive) */
			if ((*fn->done) (cookie) == 0 && (*fn->init) (cookie)) {
				puts ("** CRC error during FPGA load.\n");
				return (FPGA_FAIL);
			}
			val = data [bytecount ++];
			i = 8;
			do {
				/* Deassert the clock */
				(*fn->clk) (FALSE, TRUE, cookie);
				CONFIG_FPGA_DELAY ();
				/* Write data */
				(*fn->wr) ((val & 0x80), TRUE, cookie);
				CONFIG_FPGA_DELAY ();
				/* Assert the clock */
				(*fn->clk) (TRUE, TRUE, cookie);
				CONFIG_FPGA_DELAY ();
				val <<= 1;
				i --;
			} while (i > 0);

#ifdef CONFIG_SYS_FPGA_PROG_FEEDBACK
			if (bytecount % (bsize / 40) == 0)
				putc ('.');		/* let them know we are alive */
#endif
		}

		CONFIG_FPGA_DELAY ();

#ifdef CONFIG_SYS_FPGA_PROG_FEEDBACK
		putc ('\n');			/* terminate the dotted line */
#endif

		/* now check for done signal */
		ts = get_timer (0);		/* get current time */
		ret_val = FPGA_SUCCESS;
		(*fn->wr) (TRUE, TRUE, cookie);

		i = 0;
		while (! (*fn->done) (cookie)) {
			/* XXX - we should have a check in here somewhere to
			 * make sure we aren't busy forever... */

			CONFIG_FPGA_DELAY ();
			(*fn->clk) (FALSE, TRUE, cookie);	/* Deassert the clock pin */
			CONFIG_FPGA_DELAY ();
			(*fn->clk) (TRUE, TRUE, cookie);	/* Assert the clock pin */

			if( (i % 10000) == 0 )
			  putc ('*');
			i++;

			if (get_timer (ts) > CONFIG_SYS_FPGA_WAIT) {	/* check the time */
				puts ("** Timeout waiting for DONE to clear.\n");
				ret_val = FPGA_FAIL;
				break;
			}
		}
		putc ('\n');			/* terminate the dotted line */

		/*
		 * Run the post configuration function if there is one.
		 */
		if (*fn->post) {
			(*fn->post) (cookie);
		}

#ifdef CONFIG_SYS_FPGA_PROG_FEEDBACK
		if (ret_val == FPGA_SUCCESS) {
			puts ("Done.\n");
		}
		else {
			puts ("Fail.\n");
		}
#endif

	} else {
		printf ("%s: NULL Interface function table!\n", __FUNCTION__);
	}

	return ret_val;
}

static int Spartan6_ss_dump (Xilinx_desc * desc, void *buf, size_t bsize)
{
	/* Readback is only available through the Slave Parallel and         */
	/* boundary-scan interfaces.                                         */
	printf ("%s: Slave Serial Dumping is unavailable\n",
			__FUNCTION__);
	return FPGA_FAIL;
}

static int Spartan6_ss_reloc (Xilinx_desc * desc, ulong reloc_offset)
{
	int ret_val = FPGA_FAIL;	/* assume the worst */
	Xilinx_Spartan6_Slave_Serial_fns *fn_r, *fn =
			(Xilinx_Spartan6_Slave_Serial_fns *) (desc->iface_fns);

	if (fn) {
		ulong addr;

		/* Get the relocated table address */
		addr = (ulong) fn + reloc_offset;
		fn_r = (Xilinx_Spartan6_Slave_Serial_fns *) addr;

		if (!fn_r->relocated) {

			if (memcmp (fn_r, fn,
						sizeof (Xilinx_Spartan6_Slave_Serial_fns))
				== 0) {
				/* good copy of the table, fix the descriptor pointer */
				desc->iface_fns = fn_r;
			} else {
				PRINTF ("%s: Invalid function table at 0x%p\n",
						__FUNCTION__, fn_r);
				return FPGA_FAIL;
			}

			PRINTF ("%s: Relocating descriptor at 0x%p\n", __FUNCTION__,
					desc);

			if (fn->pre) {
				addr = (ulong) (fn->pre) + reloc_offset;
				fn_r->pre = (Xilinx_pre_fn) addr;
			}

			addr = (ulong) (fn->pgm) + reloc_offset;
			fn_r->pgm = (Xilinx_pgm_fn) addr;

			addr = (ulong) (fn->init) + reloc_offset;
			fn_r->init = (Xilinx_init_fn) addr;

			addr = (ulong) (fn->done) + reloc_offset;
			fn_r->done = (Xilinx_done_fn) addr;

			addr = (ulong) (fn->clk) + reloc_offset;
			fn_r->clk = (Xilinx_clk_fn) addr;

			addr = (ulong) (fn->wr) + reloc_offset;
			fn_r->wr = (Xilinx_wr_fn) addr;

			if (fn->post) {
				addr = (ulong) (fn->post) + reloc_offset;
				fn_r->post = (Xilinx_post_fn) addr;
			}

			fn_r->relocated = TRUE;

		} else {
			/* this table has already been moved */
			/* XXX - should check to see if the descriptor is correct */
			desc->iface_fns = fn_r;
		}

		ret_val = FPGA_SUCCESS;
	} else {
		printf ("%s: NULL Interface function table!\n", __FUNCTION__);
	}

	return ret_val;

}

static int Spartan6_ss_ssp_reloc (Xilinx_desc * desc, ulong reloc_offset)
{
	int ret_val = FPGA_FAIL;	/* assume the worst */
	Xilinx_Spartan6_Slave_Serial_fns *fn_r, *fn =
			(Xilinx_Spartan6_Slave_Serial_fns *) (desc->iface_fns);

	if (fn) {
		ulong addr;

		/* Get the relocated table address */
		addr = (ulong) fn + reloc_offset;
		fn_r = (Xilinx_Spartan6_Slave_Serial_fns *) addr;

		if (!fn_r->relocated) {

			if (memcmp (fn_r, fn,
						sizeof (Xilinx_Spartan6_Slave_Serial_fns))
				== 0) {
				/* good copy of the table, fix the descriptor pointer */
				desc->iface_fns = fn_r;
			} else {
				PRINTF ("%s: Invalid function table at 0x%p\n",
						__FUNCTION__, fn_r);
				return FPGA_FAIL;
			}

			PRINTF ("%s: Relocating descriptor at 0x%p\n", __FUNCTION__,
					desc);

			if (fn->pre) {
				addr = (ulong) (fn->pre) + reloc_offset;
				fn_r->pre = (Xilinx_pre_fn) addr;
			}

			addr = (ulong) (fn->pgm) + reloc_offset;
			fn_r->pgm = (Xilinx_pgm_fn) addr;

			addr = (ulong) (fn->init) + reloc_offset;
			fn_r->init = (Xilinx_init_fn) addr;

			addr = (ulong) (fn->done) + reloc_offset;
			fn_r->done = (Xilinx_done_fn) addr;

			addr = (ulong) (fn->clk) + reloc_offset;
			fn_r->clk = (Xilinx_clk_fn) addr;

			addr = (ulong) (fn->wr) + reloc_offset;
			fn_r->wr = (Xilinx_wr_fn) addr;

			if (fn->post) {
				addr = (ulong) (fn->post) + reloc_offset;
				fn_r->post = (Xilinx_post_fn) addr;
			}

			fn_r->relocated = TRUE;

		} else {
			/* this table has already been moved */
			/* XXX - should check to see if the descriptor is correct */
			desc->iface_fns = fn_r;
		}

		ret_val = FPGA_SUCCESS;
	} else {
		printf ("%s: NULL Interface function table!\n", __FUNCTION__);
	}

	return ret_val;

}

/*---------------------------------------------------------------*/

// $Id$
// ----------------------------------------------------------------------------
// hgroover - derived from stormwind chumbystart
// ----------------------------------------------------------------------------

#define	DEBUG
//#define ALM_DEBUG

#define VER_STR "1.32"

#include <common.h>
#include <exports.h>

// Additional includes needed for LCD
#include <asm/arch/common.h>
#include <asm/arch/mfp.h>
#include <asm/arch/mfp-pxa168.h>

//----------------------------------------------------------------------------
#define MY_MFP_52 0x03C
#define MY_MFP_50 0x03

#define MY_GPIO52_OFFSET  (1 << (52 - 32))  // bit mask offset for GPIO52
#define MY_GPIO52_BASE    0x4        // extra offset added to basic base to select the right bank register
#define MY_GPIO50_OFFSET  (1 << (50 - 32))
#define MY_GPIO50_BASE    0x4

#define MY_GPIO_SDR  0x0054  // bitwise set of direction register -- makes it an output
#define MY_GPIO_CDR  0x0060  // bitwise clr of direction register -- makes it an input
#define MY_GPIO_SRER 0x006C  // bitwise set of GPIO rising edge detect enable
#define MY_GPIO_CRER 0x0078  // bitwise clr of GPIO rising edge detect enable
#define MY_GPIO_SFER 0x0084  // falling set
#define MY_GPIO_CFER 0x0090  // falling clr

#define MY_GPIO_PSR  0x0018  // bitwise GPIO setting
#define MY_GPIO_PCR  0x0024  // bitwise GPIO clearing

#define MY_GPIO_PLR  0x0000  // pin level readback
#define MY_GPIO_PDR  0x000C  // pin direction register

#define MY_GPIO_RER  0x0030  // rising edge det enable register
#define MY_GPIO_FER  0x003C  // rising edge det enable register

#define MY_GPIO_APMASK 0x009C // SHEEVA masking
#define MY_GPIO_CPMASK 0x00A8 // mask of gpio edge detect

#define MY_GPIO_EDR    0x0048 // edge detect status register

#define POLL_DELAY			5000


// Use NON_INIT_FN to force functions to *NOT* be first, since the caller needs to assume
// main is the first function in memory
#define NON_INIT_FN __attribute__(( section(".ztext") ))
#define INIT_FN __attribute__(( section(".text") ))

#define PXA168_LCD_BASE 0xD420B000
#define LCD_CFG_GRA_START_ADDR0 0xF4
#define LCD_SPU_DMA_CTRL0 0x190
#define LCD_SPU_DUMB_CTRL 0x1B8

#define APBC_GPIO_CLK_RST 0xD4015008

// Frame buffer filled by aspenix
#define SILVERMOON_FB	0xd00000

/* Set LCD Controller to 800x600 RGB666 Dumb Panel Mode
 * for bitmap blasting
 */
unsigned int lcd_init_rgb666_800_600 [] = {
	// Extracted from silvermoon
SILVERMOON_FB,     // [0] 0xd420b0c0
SILVERMOON_FB,     // [1] 0xd420b0c4
SILVERMOON_FB,     // [2] 0xd420b0c8
0x00000000,     // [3] 0xd420b0cc

0x00000000,     // [4] 0xd420b0d0
0x00000000,     // [5] 0xd420b0d4
0x00000000,     // [6] 0xd420b0d8
0x00000000,     // [7] 0xd420b0dc

0x00000640,     // [8] 0xd420b0e0
0x00000000,     // [9] 0xd420b0e4
0x00000000,     // [10] 0xd420b0e8
0x02580320,     // [11] 0xd420b0ec

0x02580320,     // [12] 0xd420b0f0
0x07000000,     // [13] 0xd420b0f4
0x00000000,     // [14] 0xd420b0f8
0x00000640,     // [15] 0xd420b0fc

0x00000000,     // [16] 0xd420b100
0x02580320,     // [17] 0xd420b104
0x02580320,     // [18] 0xd420b108
0x00000000,     // [19] 0xd420b10c
0x00000000,     // [20] 0xd420b110
0x028c049f,     // [21] 0xd420b114
0x02580320,     // [22] 0xd420b118
0x00d70028,     // [23] 0xd420b11c
0x0022000e,     // [24] 0xd420b120
0x00000000,     // [25] 0xd420b124
0x00000000,     // [26] 0xd420b128
0x00000000,     // [27] 0xd420b12c
0x00000000,     // [28] 0xd420b130
0x00000000,     // [29] 0xd420b134
0x00000000,     // [30] 0xd420b138
0x00000000,     // [31] 0xd420b13c
0x00000000,     // [32] 0xd420b140
0x00000000,     // [33] 0xd420b144
0x80000000,     // [34] 0xd420b148
0x00000640,     // [35] 0xd420b14c
0x00000000,     // [36] 0xd420b150
0x00000640,     // [37] 0xd420b154
0x00000000,     // [38] 0xd420b158
0x000000ff,     // [39] 0xd420b15c
0x00000000,     // [40] 0xd420b160
0xe0400000,     // [41] 0xd420b164
0x6028c838,     // [42] 0xd420b168
0x00000000,     // [43] 0xd420b16c
0x00020000,     // [44] 0xd420b170
0x5119824c,     // [45] 0xd420b174
0x03000000,     // [46] 0xd420b178
0x00000004,     // [47] 0xd420b17c
0x02000f0e,     // [48] 0xd420b180
0x00008808,     // [49] 0xd420b184
0x00000000,     // [50] 0xd420b188
0x00000000,     // [51] 0xd420b18c
0x08001110,     // [52] 0xd420b190
0x20020081,     // [53] 0xd420b194
0x00000000,     // [54] 0xd420b198
0x00000000,     // [55] 0xd420b19c
0x00000000,     // [56] 0xd420b1a0
0x0000e000,     // [57] 0xd420b1a4
0x80000007,     // [58] 0xd420b1a8
0x00004000,     // [59] 0xd420b1ac
0x20004000,     // [60] 0xd420b1b0
0x00004000,     // [61] 0xd420b1b4
0x210ff10f,     // [62] 0xd420b1b8
0x00000002,     // [63] 0xd420b1bc
/****
0x88000000,     // [64] 0xd420b1c0
0x44c0010c,     // [65] 0xd420b1c4
0x00000005,     // [66] 0xd420b1c8
0x00000000,     // [67] 0xd420b1cc
****/

	0x88000000,	// SPU_IRQ_ENA =...1c0
	0x44C0018C,	// SPU_IRQ_ISR
	0x5,		//
	0xD420B000	//
};

static void NON_INIT_FN lcd_init(void)
{
    unsigned int i, j;

    /* Enable the GPIO Clock */
    *(volatile unsigned int*)(APBC_GPIO_CLK_RST) = 0x7; /*Reset Clock*/
    udelay(10);
    *(volatile unsigned int*)(APBC_GPIO_CLK_RST) = 0x3;

    /* Enable power to LCD Panel. Set GPIO 84 as output */
    *(volatile unsigned int*)(GPIO2_PSR_BASE) = 0x100000;
    *(volatile unsigned int*)(GPIO2_SDR_BASE) = 0x100000;

    /* Initialize the LCD Controller regs to set the
     * LCD to 800x600 RGB666 Dumb panel mode
     */
    for(i=0, j=0xc0; i<sizeof(lcd_init_rgb666_800_600)/sizeof(unsigned int); i+=4, j+=16) {
	*(volatile unsigned int*)(PXA168_LCD_BASE + (j+0x0)) = lcd_init_rgb666_800_600[i+0];
	*(volatile unsigned int*)(PXA168_LCD_BASE + (j+0x4)) = lcd_init_rgb666_800_600[i+1];
	*(volatile unsigned int*)(PXA168_LCD_BASE + (j+0x8)) = lcd_init_rgb666_800_600[i+2];
	*(volatile unsigned int*)(PXA168_LCD_BASE + (j+0xC)) = lcd_init_rgb666_800_600[i+3];
    }
}

static void NON_INIT_FN display_bmp(void)
{
    *(volatile unsigned int *)(PXA168_LCD_BASE + LCD_CFG_GRA_START_ADDR0) = SILVERMOON_FB;

    /* Enable Dumb LCD panel */
    *(volatile unsigned int *)(PXA168_LCD_BASE + LCD_SPU_DUMB_CTRL) |= 0x1;
    /* Enable Graphic Transfer enable */
    *(volatile unsigned int *)(PXA168_LCD_BASE + LCD_SPU_DMA_CTRL0) |= (0x1<<8);

}

static int NON_INIT_FN init_touchscreen(void)
{

	// For now just initialize GPIO 52, which is the pen up/down line.
	// Later, if we need to implement position reading, there will be
	// a lot more stuff here

	unsigned long mfp_phys_base = 0xD401E000;
	unsigned long gpio_phys_base = 0xD4019000;

	__raw_writel(0xC090, mfp_phys_base + MY_MFP_52); // set up MFP52, detect rising edge, GPIO function (AF0)

	__raw_writel(MY_GPIO52_OFFSET, gpio_phys_base + MY_GPIO52_BASE + MY_GPIO_CDR); // make GPIO52 an input


	//printk( "Pin readback of bank 2: *0x%08lx = %08x\n", (unsigned long)my_gpio.mmio_base + MY_GPIO52_BASE + MY_GPIO_PLR, __raw_readl(my_gpio.mmio_base + MY_GPIO52_BASE + MY_GPIO_PLR) );
	//printk( "Dir readback of bank 2: *0x%08lx = %08x\n", (unsigned long)my_gpio.mmio_base + MY_GPIO52_BASE + MY_GPIO_PDR, __raw_readl(my_gpio.mmio_base + MY_GPIO52_BASE + MY_GPIO_PDR) );

	return 1;

}

// ----------------------------------------------------------------------------
static int NON_INIT_FN wait_for_stylus_down( int waittime )
{
	int down = 0;
	int rc;
	int i;

	rc = 1;
	unsigned long gpio_phys_base = 0xD4019000;

	// Wait before we look again.
	udelay( POLL_DELAY );

	// Wait for a stylus event...

	//count down from waittime
	for ( i = (waittime*1000000); i > 0 ; i-= POLL_DELAY)
	{

		// read the touchscreen interrupt line, high denotes stylus down
		// TS-touched is GPIO52
		down = __raw_readl(gpio_phys_base + MY_GPIO52_BASE + MY_GPIO_PLR) & MY_GPIO52_OFFSET;

		// Are we getting an interrupt indication?
		if (down)
		{
			// Return success.
			rc = 0;
			break;
		}

		// Wait before we look again.
		udelay( POLL_DELAY );
    }

    return rc;
}

// Copy string from src to dest. Return bytes copied
static int NON_INIT_FN copy_string( char *dest, const char *src )
{
	int n;
	for (n = 0; ; n++)
	{
		dest[n] = src[n];
		if (src[n] == '\0')
		{
			break;
		}
	}
	return n;
}

// Write hex value using %08x and return bytes written, not including
// terminating null.
static int NON_INIT_FN hex_to_str( char *s, unsigned int v )
{
	int n;
	static const char *hex_chars="0123456789ABCDEF";
	s[8] = '\0';
	for (n = 7; n >= 0; n--)
	{
		s[n] = hex_chars[(v & 0x0f)];
		v >>= 4;
	}
	return 8;
}

// Header on disk for BMP files
#pragma pack(2)
typedef struct _BitmapInfoHeader {
// Offsets are from start of file
// All values are little-endian with low order byte coming first. Use GetWord or GetDword
//        static unsigned short GetWord( const unsigned char *b ) { return b[0] | (b[1]<<8); }
//        static void SetWord( unsigned char *pb, unsigned short w ) { pb[0] = w&0xff; pb[1] = (w&0xff00)>>8; }
//        static unsigned long GetDword( const unsigned char *b ) { return b[0] | (b[1]<<8) | (b[2]<<16) | (b[3]<<24); }
//        static void SetDword( unsigned char *pb, unsigned long dw ) { pb[0] = dw&0xff; pb[1] = (dw&0xff00)>>8; pb[2] = (dw&0xff0000)>>16; pb[3] = (dw&0xff000000)>>24; }
//14 0x0e 4 size of BITMAPINFOHEADER structure which includes the header size, normally 40
        unsigned char dwHeaderSize[4];
//        unsigned long GetHeaderSize() const { return GetDword(dwHeaderSize); }
//        void SetHeaderSize( unsigned long dw ) { SetDword(dwHeaderSize,dw); }
//18 0x12 4 image width in pixels
        unsigned char dwImageWidth[4];
//        unsigned long GetImageWidth() const { return GetDword(dwImageWidth); }
//        void SetImageWidth( unsigned long dw ) { SetDword(dwImageWidth,dw); }
//22 0x16 4 image height in pixels
        unsigned char dwImageHeight[4];
//        unsigned long GetImageHeight() const { return GetDword(dwImageHeight); }
//        void SetImageHeight( unsigned long dw ) { SetDword(dwImageHeight,dw); }
//26 0x1a 2 number of planes in the image, must be 1
        unsigned char wPlanes[2];
//        unsigned short GetPlanes() const { return GetWord(wPlanes); }
//        void SetPlanes( unsigned short w ) { SetWord(wPlanes,w); }
//28 0x1c 2 number of bits per pixel (1, 4, 8, or 24)
        unsigned char wBitsPerPixel[2];
//        unsigned short GetBitsPerPixel() const { return GetWord(wBitsPerPixel); }
//        void SetBitsPerPixel( unsigned short w ) { SetWord(wBitsPerPixel,w); }
//30 0x1e 4 compression type (0=none, 1=RLE-8, 2=RLE-4)
        unsigned char dwCompressionType[4];
//        unsigned long GetCompressionType() const { return GetDword(dwCompressionType); }
//        void SetCompressionType( unsigned long dw ) { SetDword(dwCompressionType,dw); }
//34 0x22 4 size of image data in bytes (including padding)
        unsigned char dwImageDataLength[4];
//        unsigned long GetImageDataLength() const { return GetDword(dwImageDataLength); }
//        void SetImageDataLength( unsigned long dw ) { SetDword(dwImageDataLength,dw); }
//38 0x26 4 horizontal resolution in pixels per meter (unreliable)
        unsigned char dwHorizontalPixPerMeter[4];
//42 0x2a 4 vertical resolution in pixels per meter (unreliable)
        unsigned char dwVerticalPixPerMeter[4];
//46 0x2e 4 number of colors in image, or zero
        unsigned char dwColors[4];
//50 0x32 4 number of important colors, or zero
        unsigned char dwImportantColors[4];
//54 0x36 0-16 undefined
		unsigned char __undef1[16];
//70 0x46 usual start of data
		unsigned char bmpdata[0];
} BitmapInfoHeader_t;

// What to read from start of file to get the image data offset. This is always followed
// immediately by the bitmap info header (Windows) or bitmap core header (OS/2)
typedef struct _BMPFileHeader {
// All values are little-endian with low order byte coming first. Use GetWord or GetDword
//0  0x00 2  signature, must be 'BM'
        unsigned char wSig[2];
//        bool IsBMP() const { return wSig[0]=='B' && wSig[1]=='M'; }
//2  0x02 4 size of BMP file in bytes (unreliable)
        unsigned char dwFilesize[4];
//        void SetFilesize(unsigned long dw) { BitmapInfoHeader_t::SetDword(dwFilesize,dw); }
//6  0x06 2 reserved, must be zero
        unsigned char _rsvd1[2];
//8  0x08 2 reserved, must be zero
        unsigned char _rsvd2[2];
//10 0x0a 4 offset to start of image data in bytes
        unsigned char dwImageDataOffset[4];
//        unsigned long GetImageDataOffset() const { return BitmapInfoHeader_t::GetDword(dwImageDataOffset); }
//        void SetImageDataOffset(unsigned long dw) { BitmapInfoHeader_t::SetDword(dwImageDataOffset,dw); }
} BMPFileHeader_t;

// Structure containing the minimum we should find at start of file
typedef struct _BMPHeader {
        BMPFileHeader_t fileHeader;
        BitmapInfoHeader_t infoHeader;
} BMPHeader_t;
#pragma pack()

// ----------------------------------------------------------------------------
// Chumby start-up application
// ----------------------------------------------------------------------------

// Messages are 240w x 180h for both 800x600 and 800x480
//#include "splashA_600.c"
#define splashA A_600
#define splashB A_600
#define MSG_X	177
#define MSG_Y	80

// Replacement for memcpy
static void NON_INIT_FN __vmemcpy( void *dst, void *src, unsigned int count )
{
#if 0
	int dword_count = count / 4;
	int byte_count = (count & 0x03);
	int n;
	unsigned long *dest = (unsigned long *)dst;
	unsigned long *source = (unsigned long *)source;
	static int spew_count = 0;
	if (spew_count++ < 20)
	{
		printf( "[%d]__vmemcpy(%x,%x,%u) dwords=%d bytes=%d %x%x%x%x\n",
			spew_count-1, (unsigned int)dst, (unsigned int)src, count, dword_count, byte_count,
			source[0], source[1], source[2], source[3] );
	}
	for (n = 0; n < dword_count; n++)
	{
		dest[n] = source[n];
	}
	unsigned char *destc = (unsigned char *)(dword_count * 4 + dest);
	unsigned char *sourcec = (unsigned char *)(dword_count * 4 + source);
	for (n = 0; n < byte_count; n++)
	{
		*destc = *sourcec;
		destc++;
		sourcec++;
	}
#else
	unsigned char *destc = (unsigned char *)dst;
	unsigned char *sourcec = (unsigned char *)src;
	int n;
	for (n = 0; n < count; n++)
	{
		destc[n] = sourcec[n];
	}
#endif
}

// Display bitmap at specified x and y using dimensions in header
static void NON_INIT_FN display_bitmap( unsigned char *bmpdata, unsigned int bmpdata_len, int x, int y )
{
	BMPHeader_t *b = (BMPHeader_t*)bmpdata;
	// FIXME use actual width and height
	unsigned char *dest = (unsigned char *)(SILVERMOON_FB + y * 2 * 800 + x * 2);
	int row;
	printf( "Blasting bitmap to %d, %d starting dest %x source %x source pixels %x\n",
		x, y, (unsigned int)dest, (unsigned int)bmpdata, (unsigned int)&b->infoHeader.bmpdata[0] );
	/****
	printf( "Starting pixels: %02x%02x%02x%02x %02x%02x%02x%02x\n",
		b->infoHeader.bmpdata[0],
		b->infoHeader.bmpdata[1],
		b->infoHeader.bmpdata[2],
		b->infoHeader.bmpdata[3],
		b->infoHeader.bmpdata[4],
		b->infoHeader.bmpdata[5],
		b->infoHeader.bmpdata[6],
		b->infoHeader.bmpdata[7] );
	****/
	// Vertically flip
	for (row = 179; row >= 0; row--, dest += (2 * 800))
	{
		__vmemcpy( dest, &b->infoHeader.bmpdata[row*240*2], 240*2 );
	}
	// Replicate last line
	for (row = 0; row < 20; row++, dest += (2 * 800))
	{
		__vmemcpy( dest, b->infoHeader.bmpdata, 240*2 );
	}
}


#include "splashMsg1.c" // Touch screen to boot diag [A] B
#include "splashMsg2.c" // Booting normally [A] B
//#include "splashMsg3.c" // Touch screen to boot diag A [B]
#include "splashMsg4.c" // Starting diag [A] B
//#include "splashMsg5.c" // Starting diag A [B]
//#include "splashMsg6.c" // Booting normally A [B]
#include "splashMsg7.c" // Update phase 2 [A] B
//#include "splashMsg8.c" // Update phase 2 A [B]

// FIXME uncompress
#define Msg3 Msg1
#define Msg5 Msg4
#define Msg6 Msg2
#define Msg8 Msg7

#include "esd_config_area.h" // from utils-1.0/src

// Try to force this into a section which always comes before .text
// u-boot uses -Ttext <addr> to force the start address of .text, so
// we'll make this the ONLY function in .text
int INIT_FN chumbystart(int argc, char *argv[])
{
	int booting_rfs2 = 0;
	int rfs2_override = 0;
	// Flag array for data from reserved partition
	unsigned char *reserved_data;
	// Config area data (FIXME get this from a common source here and in aspenix)
	config_area *ca = (config_area*)0x8c0000;
	// Additional data from reserved - 1 if phase 1 in rfs2, 2 if phase 2 in rfs1, 0 undefined
	int update_phase = 0;
	//char *bootargs_const = "setenv bootargs root=/dev/mmcblk0p2 rootfstype=ext3 ro rootdelay=2 console=ttyS0,115200 mem=128M uart_dma init=/linuxrc";
	char *bootargs_const = "setenv bootargs root=/dev/mmcblk0p2 rootfstype=ext3 ro rootwait console=ttyS0,115200 mem=128M init=/linuxrc";

	//////////////______////0..3.5.7..0..3.5.7..0..3.5.7..0..3.5.7..0..3.5.7..0..3.5.7..0..3.5.7..0..3.5.7..0..3.5.7..0..3.5.7..
	//////////////______////          1  1 1 1  2  2 2 2  3  3 3 3  4  4 4 4  5  5 5 5  6  6 6 6  7         8         9
#define BOOTARGS_PART_NUM 34 // Index of 2/3
#define BOOTARGS_O_OR_W 53 // Index of o in ro or w in rw
#define BOOTARGS_ROOTDELAY 56 // Index of single-digit rootdelay (seconds)
	char bootargs[256];
	int n;
	int dbg_pass = 0;
	int stylus_wait = 4; // Wait time in seconds for touchscreen
	// Device offsets for krnA and krnB in 512-byte sectors
	unsigned int krnA_device_offset_sectors = 0;
	unsigned int krnA_length_sectors = 0;
	unsigned int krnB_device_offset_sectors = 0;
	unsigned int krnB_length_sectors = 0;

	app_startup(argv);

	if (get_version () != XF_VERSION)
	{
		printf ("Wrong XF_VERSION. Please re-compile with actual u-boot sources\n");
		printf ("Example expects ABI version %d\n", XF_VERSION);
		printf ("Actual U-Boot ABI version %d\n", (int)get_version());
		return 1;
	}

#define TWO_PASS_DEBUG
	// If we've been invoked a second time for debug purposes,
	// check ca->ESD_CONFIG_UNUSED[0]
	if (ca->ESD_CONFIG_UNUSED[0] == 'd' && ca->ESD_CONFIG_UNUSED[1] == 'b' && ca->ESD_CONFIG_UNUSED[2] == 'g')
	{
		dbg_pass = ca->ESD_CONFIG_UNUSED[3];
		ca->ESD_CONFIG_UNUSED[3]++;
	}
	else
	{
		ca->ESD_CONFIG_UNUSED[0] = 'd';
		ca->ESD_CONFIG_UNUSED[1] = 'b';
		ca->ESD_CONFIG_UNUSED[2] = 'g';
		ca->ESD_CONFIG_UNUSED[3] = 1; // Seconds to wait after loading
	}

	// If we're just waiting, don't re-initialize the display
	if (!dbg_pass)
	{
		lcd_init();
		display_bmp();
	}

	init_touchscreen();

	printf( "chumbystart %s %s display %dx%d waiting=%d\n", VER_STR, "$Rev$", CHUMBY_CONFIG_scrwidth, CHUMBY_CONFIG_scrheight, dbg_pass );

	// Copy boot args to writeable copy
	(void)copy_string( bootargs, bootargs_const );

	if (!dbg_pass)
	{
		if (ca->sig[0] == 'C' && ca->sig[1] == 'f' && ca->sig[2] == 'g' && ca->sig[3] == '*')
		{
			printf( "Got config area signature\n" );
			if (ca->active_index[0] != 0)
			{
				display_bitmap( Msg1, sizeof(Msg1), MSG_X, MSG_Y );
				printf( "Active index is %d, booting to rfs B (update in progress == %d) ver=%s\n", ca->active_index[0], ca->updating[0], ca->last_update );
				booting_rfs2 = 1;
			}
			else
			{
				display_bitmap( Msg1, sizeof(Msg1), MSG_X, MSG_Y );
				printf( "Active index is %d, booting to rfs A (update in progress == %d) ver=%s\n", ca->active_index[0], ca->updating[0], ca->last_update );
			}
			// Traverse list to determine device offsets for krnA and krnB
#define KRNA_U32	('k'|('r'<<8)|('n'<<16)|('A'<<24))
#define KRNB_U32	('k'|('r'<<8)|('n'<<16)|('B'<<24))
			for (n = 0; n < 64; n++)
			{
				if (ca->block_table[n].offset == 0xffffffff)
				{
					break;
				}
				if (ca->block_table[n].n.uname == KRNA_U32)
				{
					krnA_device_offset_sectors = (ca->p1_offset + ca->block_table[n].offset) / 512;
					krnA_length_sectors = ca->block_table[n].length / 512;
				}
				else if (ca->block_table[n].n.uname == KRNB_U32)
				{
					krnB_device_offset_sectors = (ca->p1_offset + ca->block_table[n].offset) / 512;
					krnB_length_sectors = ca->block_table[n].length / 512;
				}
			}
		}
		else
		{
			printf( "Did not match config area signature\n" );
		}
	}

	// Get flag for RFS2 which should have been preloaded at
	//   0xc0008000 - our start address
	// + 0x000cc000 - our length
	// = 0xc00d4000 - end of chumbystart code+data
	// NAND offset is 3e80000
	//reserved_data = (unsigned char*)0xc00d4000;
	//printf( "Reserved data at %08x: %02x %02x %02x %02x\n", (unsigned int)reserved_data, reserved_data[0], reserved_data[1], reserved_data[2], reserved_data[3] );

	// Display appropriate bitmap and message
	//video_display_bitmap ((ulong)(/*booting_rfs2 ? splashB : */ splashA), 0,0);
	//video_display_bitmap( (ulong)(booting_rfs2 ? (update_phase==1 ? Msg7 : Msg3) : (update_phase==2 ? Msg8 : Msg1)), MSG_X, MSG_Y );

	// Wait for the pen to go down.
	if (dbg_pass)
	{
		printf( "dbg pass %d - waiting %d seconds for touchscreen\n", dbg_pass, dbg_pass );
		stylus_wait = dbg_pass;
	}
	else
	{
		printf( "Waiting up to %d seconds for touchscreen event\n", stylus_wait );
	}
	if (!wait_for_stylus_down( stylus_wait ))
	{

		if (dbg_pass)
		{
			printf( "Got touchscreen event, proceeding with kernel load\n" );
		}
		else
		{
			char *ptr;

			printf( "Got touchscreen event.  Mounting other rootfs R/W\n" );
			display_bitmap( Msg4, sizeof(Msg4), MSG_X, MSG_Y );

			//ptr = strstr(bootargs, " ro ");
			//strncpy(ptr, " rw ", 4);

			//oops, string library funcs aren't available, do it the hard way.
			ptr = bootargs;
			while(ptr[4] != '\0')
			{
				if (ptr[0] == ' ' && ptr[1] == 'r' && ptr[2] == 'o' && ptr[3] == ' ')
				{
					ptr[2]='w';
					break;
				}
				ptr++;
			}

			booting_rfs2 = !booting_rfs2;
			//rfs2_override = 1;
		}
	}
	else
	{
		printf( "No touchscreen event\n" );
		if (!dbg_pass)
		{
			display_bitmap( Msg2, sizeof(Msg2), MSG_X, MSG_Y );
		}
	}

	if (dbg_pass)
	{
		printf( "dbg_pass = %d, exiting\n", dbg_pass );
		setenv("chumbyboot3", "bootz 0x500000");
		return 0;
	}
	else
	{
		setenv("chumbyboot3", "go 0x8c4000");
	}

	if (!booting_rfs2)
	{
		/******************
		// This no longer follows k1off
		char *k1_boot = "nand read.i c0008000 d80000 200000";
		printf( "Booting rfs1 from %lx using message at %lx\n", k1_load_addr, (ulong)(rfs2_override ? Msg2 : Msg4) );

		if (rfs2_override)
		{
			// We overrode previously requested rfs2 boot
			video_display_bitmap( (ulong)Msg4, MSG_X, MSG_Y );
		}
		else
		{
			video_display_bitmap( (ulong)Msg2, MSG_X, MSG_Y );
		}
		********************/

		char krnA_boot[256];
		int str_offset;
		str_offset = copy_string( krnA_boot, "mmc read 0 0x500000 0x" );
		str_offset += hex_to_str( &krnA_boot[str_offset], krnA_device_offset_sectors );
		str_offset += copy_string( &krnA_boot[str_offset], " 0x" );
		str_offset += hex_to_str( &krnA_boot[str_offset], krnA_length_sectors );
		printf( "Setting environment to load kernel: %s\n", krnA_boot );
		setenv("chumbyboot1", krnA_boot);

		bootargs[BOOTARGS_PART_NUM] = ('2' + booting_rfs2);
		setenv("chumbyboot2", bootargs);
	}
	else
	{
		/*********************
		// This no longer follows k2off
		char *k2_boot = "nand read.i c0008000 300000 200000";

		printf( "Booting rfs2 from %lx using message at %lx\n", k2_load_addr, (ulong)(rfs2_override ? Msg5 : Msg6) );

		if (rfs2_override)
		{
			// We overrode previously requested rfs1 boot
			video_display_bitmap( (ulong)Msg5, MSG_X, MSG_Y );
		}
		else
		{
			video_display_bitmap( (ulong)Msg6, MSG_X, MSG_Y );
		}
		*********************/

		//printf( "Setting environment to load k2: %s\n", k2_boot );

		char krnB_boot[256];
		int str_offset;
		str_offset = copy_string( krnB_boot, "mmc read 0 0x500000 0x" );
		str_offset += hex_to_str( &krnB_boot[str_offset], krnB_device_offset_sectors );
		str_offset += copy_string( &krnB_boot[str_offset], " 0x" );
		str_offset += hex_to_str( &krnB_boot[str_offset], krnB_length_sectors );
		printf( "Setting environment to load kernel: %s\n", krnB_boot );
		setenv("chumbyboot1", krnB_boot);

		bootargs[BOOTARGS_PART_NUM] = ('2' + booting_rfs2);
		setenv("chumbyboot2", bootargs);
		//return (1);
	}

	printf( "active boot flag = %d, boot args = %s\n", booting_rfs2, &bootargs[16] );

	return (0);
}



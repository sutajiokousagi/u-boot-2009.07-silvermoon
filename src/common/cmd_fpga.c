/*
 * (C) Copyright 2000, 2001
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

/*
 *  FPGA support
 */
#include <common.h>
#include <command.h>
#if defined(CONFIG_CMD_NET)
#include <net.h>
#endif
#include <fpga.h>
#include <malloc.h>
#include <asm/arch/common.h>

#if 0
#define	FPGA_DEBUG
#endif

#ifdef	FPGA_DEBUG
#define	PRINTF(fmt,args...)	printf (fmt ,##args)
#else
#define PRINTF(fmt,args...)
#endif

/* Local functions */
static void fpga_usage (cmd_tbl_t * cmdtp);
static int fpga_get_op (char *opstr);

/* Local defines */
#define FPGA_NONE   -1
#define FPGA_INFO   0
#define FPGA_LOAD   1
#define FPGA_LOADB  2
#define FPGA_DUMP   3
#define FPGA_LOADMK 4

/* Convert bitstream data and load into the fpga */
int fpga_loadbitstream(unsigned long dev, char* fpgadata, size_t size)
{
#if defined(CONFIG_FPGA_XILINX)
	unsigned int length;
	unsigned int swapsize;
	char buffer[80];
	unsigned char *dataptr;
	unsigned int i;
	int rc;

	dataptr = (unsigned char *)fpgadata;

	/* skip the first bytes of the bitsteam, their meaning is unknown */
	length = (*dataptr << 8) + *(dataptr+1);
	dataptr+=2;
	dataptr+=length;

	/* get design name (identifier, length, string) */
	length = (*dataptr << 8) + *(dataptr+1);
	dataptr+=2;
	if (*dataptr++ != 0x61) {
		PRINTF ("%s: Design name identifier not recognized in bitstream\n",
			__FUNCTION__ );
		return FPGA_FAIL;
	}

	length = (*dataptr << 8) + *(dataptr+1);
	dataptr+=2;
	for(i=0;i<length;i++)
		buffer[i] = *dataptr++;

	printf("  design filename = \"%s\"\n", buffer);

	/* get part number (identifier, length, string) */
	if (*dataptr++ != 0x62) {
		printf("%s: Part number identifier not recognized in bitstream\n",
			__FUNCTION__ );
		return FPGA_FAIL;
	}

	length = (*dataptr << 8) + *(dataptr+1);
	dataptr+=2;
	for(i=0;i<length;i++)
		buffer[i] = *dataptr++;
	printf("  part number = \"%s\"\n", buffer);

	/* get date (identifier, length, string) */
	if (*dataptr++ != 0x63) {
		printf("%s: Date identifier not recognized in bitstream\n",
		       __FUNCTION__);
		return FPGA_FAIL;
	}

	length = (*dataptr << 8) + *(dataptr+1);
	dataptr+=2;
	for(i=0;i<length;i++)
		buffer[i] = *dataptr++;
	printf("  date = \"%s\"\n", buffer);

	/* get time (identifier, length, string) */
	if (*dataptr++ != 0x64) {
		printf("%s: Time identifier not recognized in bitstream\n",__FUNCTION__);
		return FPGA_FAIL;
	}

	length = (*dataptr << 8) + *(dataptr+1);
	dataptr+=2;
	for(i=0;i<length;i++)
		buffer[i] = *dataptr++;
	printf("  time = \"%s\"\n", buffer);

	/* get fpga data length (identifier, length) */
	if (*dataptr++ != 0x65) {
		printf("%s: Data length identifier not recognized in bitstream\n",
			__FUNCTION__);
		return FPGA_FAIL;
	}
	swapsize = ((unsigned int) *dataptr     <<24) +
	           ((unsigned int) *(dataptr+1) <<16) +
	           ((unsigned int) *(dataptr+2) <<8 ) +
	           ((unsigned int) *(dataptr+3)     ) ;
	dataptr+=4;
	printf("  bytes in bitstream = %d\n", swapsize);

	rc = fpga_load(dev, dataptr, swapsize);
	return rc;
#else
	printf("Bitstream support only for Xilinx devices\n");
	return FPGA_FAIL;
#endif
}

/* ------------------------------------------------------------------------- */
/* command form:
 *   fpga <op> <device number> <data addr> <datasize>
 * where op is 'load', 'dump', or 'info'
 * If there is no device number field, the fpga environment variable is used.
 * If there is no data addr field, the fpgadata environment variable is used.
 * The info command requires no data address field.
 */
int do_fpga (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int op, dev = FPGA_INVALID_DEVICE;
	size_t data_size = 0;
	void *fpga_data = NULL;
	char *devstr = getenv ("fpga");
	char *datastr = getenv ("fpgadata");
	int rc = FPGA_FAIL;
#if defined (CONFIG_FIT)
	const char *fit_uname = NULL;
	ulong fit_addr;
#endif

	if (devstr)
		dev = (int) simple_strtoul (devstr, NULL, 16);
	if (datastr)
		fpga_data = (void *) simple_strtoul (datastr, NULL, 16);

	switch (argc) {
	case 5:		/* fpga <op> <dev> <data> <datasize> */
		data_size = simple_strtoul (argv[4], NULL, 10);

	case 4:		/* fpga <op> <dev> <data> */
#if defined(CONFIG_FIT)
		if (fit_parse_subimage (argv[3], (ulong)fpga_data,
					&fit_addr, &fit_uname)) {
			fpga_data = (void *)fit_addr;
			debug ("*  fpga: subimage '%s' from FIT image at 0x%08lx\n",
					fit_uname, fit_addr);
		} else
#endif
		{
			fpga_data = (void *) simple_strtoul (argv[3], NULL, 16);
			debug ("*  fpga: cmdline image address = 0x%08lx\n", (ulong)fpga_data);
		}
		PRINTF ("%s: fpga_data = 0x%x\n", __FUNCTION__, (uint) fpga_data);

	case 3:		/* fpga <op> <dev | data addr> */
		dev = (int) simple_strtoul (argv[2], NULL, 16);
		PRINTF ("%s: device = %d\n", __FUNCTION__, dev);
		/* FIXME - this is a really weak test */
		if ((argc == 3) && (dev > fpga_count ())) {	/* must be buffer ptr */
			PRINTF ("%s: Assuming buffer pointer in arg 3\n",
				__FUNCTION__);

#if defined(CONFIG_FIT)
			if (fit_parse_subimage (argv[2], (ulong)fpga_data,
						&fit_addr, &fit_uname)) {
				fpga_data = (void *)fit_addr;
				debug ("*  fpga: subimage '%s' from FIT image at 0x%08lx\n",
						fit_uname, fit_addr);
			} else
#endif
			{
				fpga_data = (void *) dev;
				debug ("*  fpga: cmdline image address = 0x%08lx\n", (ulong)fpga_data);
			}

			PRINTF ("%s: fpga_data = 0x%x\n",
				__FUNCTION__, (uint) fpga_data);
			dev = FPGA_INVALID_DEVICE;	/* reset device num */
		}

	case 2:		/* fpga <op> */
		op = (int) fpga_get_op (argv[1]);
		break;

	default:
		PRINTF ("%s: Too many or too few args (%d)\n",
			__FUNCTION__, argc);
		op = FPGA_NONE;	/* force usage display */
		break;
	}

	switch (op) {
	case FPGA_NONE:
		fpga_usage (cmdtp);
		break;

	case FPGA_INFO:
		rc = fpga_info (dev);
		break;

	case FPGA_LOAD:
		rc = fpga_load (dev, fpga_data, data_size);
		break;

	case FPGA_LOADB:
		rc = fpga_loadbitstream(dev, fpga_data, data_size);
		break;
#if 0
	case FPGA_LOADMK:
		switch (genimg_get_format (fpga_data)) {
		case IMAGE_FORMAT_LEGACY:
			{
				image_header_t *hdr = (image_header_t *)fpga_data;
				ulong	data;

				data = (ulong)image_get_data (hdr);
				data_size = image_get_data_size (hdr);
				rc = fpga_load (dev, (void *)data, data_size);
			}
			break;
#if defined(CONFIG_FIT)
		case IMAGE_FORMAT_FIT:
			{
				const void *fit_hdr = (const void *)fpga_data;
				int noffset;
				void *fit_data;

				if (fit_uname == NULL) {
					puts ("No FIT subimage unit name\n");
					return 1;
				}

				if (!fit_check_format (fit_hdr)) {
					puts ("Bad FIT image format\n");
					return 1;
				}

				/* get fpga component image node offset */
				noffset = fit_image_get_node (fit_hdr, fit_uname);
				if (noffset < 0) {
					printf ("Can't find '%s' FIT subimage\n", fit_uname);
					return 1;
				}

				/* verify integrity */
				if (!fit_image_check_hashes (fit_hdr, noffset)) {
					puts ("Bad Data Hash\n");
					return 1;
				}

				/* get fpga subimage data address and length */
				if (fit_image_get_data (fit_hdr, noffset, &fit_data, &data_size)) {
					puts ("Could not find fpga subimage data\n");
					return 1;
				}

				rc = fpga_load (dev, fit_data, data_size);
			}
			break;
#endif
		default:
			puts ("** Unknown image type\n");
			rc = FPGA_FAIL;
			break;
		}
		break;
#endif

	case FPGA_DUMP:
		rc = fpga_dump (dev, fpga_data, data_size);
		break;

	default:
		printf ("Unknown operation\n");
		fpga_usage (cmdtp);
		break;
	}
	return (rc);
}

static void fpga_usage (cmd_tbl_t * cmdtp)
{
	printf ("Usage:\n%s\n", cmdtp->usage);
}

/*
 * Map op to supported operations.  We don't use a table since we
 * would just have to relocate it from flash anyway.
 */
static int fpga_get_op (char *opstr)
{
	int op = FPGA_NONE;

	if (!strcmp ("info", opstr)) {
		op = FPGA_INFO;
	} else if (!strcmp ("loadb", opstr)) {
		op = FPGA_LOADB;
	} else if (!strcmp ("load", opstr)) {
		op = FPGA_LOAD;
	} else if (!strcmp ("loadmk", opstr)) {
		op = FPGA_LOADMK;
	} else if (!strcmp ("dump", opstr)) {
		op = FPGA_DUMP;
	}

	if (op == FPGA_NONE) {
		printf ("Unknown fpga operation \"%s\"\n", opstr);
	}
	return op;
}

U_BOOT_CMD (fpga, 6, 1, do_fpga,
	    "fpga    - loadable FPGA image support\n",
	    "fpga [operation type] [device number] [image address (hex)] [image size (decimal)]\n"
	    "fpga operations:\n"
	    "\tinfo\tlist known device information\n"
	    "\tload\tLoad device from memory buffer\n"
	    "\tloadb\tLoad device from bitstream buffer (Xilinx devices only)\n"
	    //	    "\tloadmk\tLoad device generated with mkimage\n"
	    "\tdump\tLoad device to memory buffer\n"
#if defined(CONFIG_FIT)
	    "\tFor loadmk operating on FIT format uImage address must include\n"
	    "\tsubimage unit name in the form of addr:<subimg_uname>\n"
#endif
);




#define TDO_GPIO 16
#define TMS_GPIO 18
#define TCK_GPIO 20
#define TDI_GPIO 34

#define GPIO_PATH "/sys/class/gpio"
#define EXPORT_PATH GPIO_PATH "/export"
#define UNEXPORT_PATH GPIO_PATH "/unexport"

struct jtag_state {
	int tdi;
	int tms;
	int tck;
	int tdo;
};

static unsigned long mfp_phys_base = 0xD401E000;
static unsigned long gpio_phys_base = 0xD4019000;
static unsigned long gpio_plr = 0x00;
static unsigned long gpio_psr = 0x18;
static unsigned long gpio_pcr = 0x24;
static unsigned long gpio_sdr = 0x54;
static unsigned long gpio_cdr = 0x60;

static int gpio_bases[] = {
	0x0000,
	0x0004,
	0x0008,
	0x0100,
};

static int gpio_export(int pin) {
	unsigned long mfp;

	/* MFP pins have this weird discontinuity between MFP and offset */
	if (pin >= 37 && pin <= 55)
		pin -= 37;
	else
		pin += 19;
	mfp = mfp_phys_base + pin*4;
	__raw_writel(0x880, mfp);;

	return 0;
}

static int gpio_get_value(int pin) {
	unsigned long gpio = gpio_phys_base + gpio_bases[pin>>5];
	return !!(__raw_readl(gpio)&(1<<(pin&0x1f)));
}

static int gpio_set_value(int pin, int value) {
	unsigned long gpio = gpio_phys_base + gpio_bases[pin>>5];
	if (value)
		gpio += gpio_psr;
	else
		gpio += gpio_pcr;
	__raw_writel(1<<(pin&0x1f), gpio);

	return 0;
}

static int gpio_set_direction(int pin, int is_output) {
	unsigned long gpio = gpio_phys_base + gpio_bases[pin>>5];

	if (is_output)
		gpio += gpio_sdr;
	else
		gpio += gpio_cdr;

	__raw_writel(1<<(pin&0x1f), gpio);

	return 0;
}


/* Wiggle the TCK like, moving JTAG one step further along its state machine */
static int jtag_tick(struct jtag_state *state) {
	gpio_set_value(state->tck, 0);
	udelay(100);
	gpio_set_value(state->tck, 1);
	udelay(100);
	gpio_set_value(state->tck, 0);
	udelay(100);
	return 0;
}


/* Send five 1s through JTAG, which will bring it into reset state */
static int jtag_reset(struct jtag_state *state) {
	int i;
	for (i=0; i<5; i++) {
		gpio_set_value(state->tms, 1);
		jtag_tick(state);
	}

	return 0;
}


static int jtag_open(struct jtag_state *state) {

	gpio_export(TDI_GPIO);
	gpio_export(TMS_GPIO);
	gpio_export(TCK_GPIO);
	gpio_export(TDO_GPIO);

	gpio_set_direction(TDI_GPIO, 0);
	gpio_set_direction(TMS_GPIO, 1);
	gpio_set_direction(TCK_GPIO, 1);
	gpio_set_direction(TDO_GPIO, 1);

	gpio_set_value(TDO_GPIO, 0);
	gpio_set_value(TMS_GPIO, 0);
	gpio_set_value(TCK_GPIO, 0);

	state->tdi = TDI_GPIO;
	state->tms = TMS_GPIO;
	state->tck = TCK_GPIO;
	state->tdo = TDO_GPIO;

	jtag_reset(state);

	return 0;
}

/* Reads the ID CODE out of the FPGA
 * When the state machine is reset, the sequence 0, 1, 0, 0 will move
 * it to a point where continually reading the TDO line will yield the
 * ID code.
 *
 * This is because by default, the reset command loads the chip's ID
 * into the data register, so all we have to do is read it out.
 */
static int jtag_idcode(struct jtag_state *state) {
	int i;
	int val = 0;

	/* Reset the state machine */
	jtag_reset(state);

	/* Get into "Run-Test/ Idle" state */
	gpio_set_value(state->tms, 0);
	jtag_tick(state);

	/* Get into "Select DR-Scan" state */
	gpio_set_value(state->tms, 1);
	jtag_tick(state);

	/* Get into "Capture DR" state */
	gpio_set_value(state->tms, 0);
	jtag_tick(state);

	/* Get into "Shift-DR" state */
	gpio_set_value(state->tms, 0);
	jtag_tick(state);

	/* Read the code out */
	for (i=0; i<32; i++) {
		int ret = gpio_get_value(state->tdi);
		val |= (ret<<i);
		jtag_tick(state);
	}

	return val;
}


static int do_fpga_type (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	struct jtag_state state;
	int idcode;

	jtag_open(&state);
	idcode = jtag_idcode(&state);

	if (idcode == 0x24001093)
		setenv("FPGA_TYPE", "LX9");
	else if (idcode == 0x34008093)
		setenv("FPGA_TYPE", "LX45");
	else {
		printf("Unknown FPGA ID code: 0x%08x\n", idcode);
		setenv("FPGA_TYPE", "unknown");
	}
	printf("FPGA type: %s\n", getenv("FPGA_TYPE"));
	return 0;
}


U_BOOT_CMD (fpga_type, 1, 1, do_fpga_type,
	"fpga_type	- Report FPGA type\n",
	"Running \"fpga_type\" prints out FPGA type and sets $FPGA_TYPE\n"
);

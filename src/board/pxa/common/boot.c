/*
 * Boot support
 */
#include <common.h>
#include <watchdog.h>
#include <command.h>
#include <image.h>
#include <malloc.h>
#include <environment.h>
#include <linux/ctype.h>
#include <nand.h>
#include <onenand_uboot.h>

DECLARE_GLOBAL_DATA_PTR;
ulong load_addr = CONFIG_SYS_LOAD_ADDR;	/* Default Load Address */
static bootm_headers_t images;		/* pointers to os/initrd/fdt images */

typedef int boot_os_fn (int flag, int argc, char *argv[],
			bootm_headers_t *images); /* pointers to os/initrd/fdt */

#define CONFIG_BOOTM_LINUX 1
#ifdef CONFIG_BOOTM_LINUX
extern boot_os_fn do_bootm_linux;
#endif

#ifdef CONFIG_CMD_ELF
extern boot_os_fn do_bootm_vxworks;
extern boot_os_fn do_bootm_qnxelf;
#endif

boot_os_fn * boot_os[] = {
#ifdef CONFIG_BOOTM_LINUX
	[IH_OS_LINUX] = do_bootm_linux,
#endif
#ifdef CONFIG_BOOTM_NETBSD
	[IH_OS_NETBSD] = do_bootm_netbsd,
#endif
#ifdef CONFIG_LYNXKDI
	[IH_OS_LYNXOS] = do_bootm_lynxkdi,
#endif
#ifdef CONFIG_BOOTM_RTEMS
	[IH_OS_RTEMS] = do_bootm_rtems,
#endif
#if defined(CONFIG_CMD_ELF)
	[IH_OS_VXWORKS] = do_bootm_vxworks,
	[IH_OS_QNX] = do_bootm_qnxelf,
#endif
#ifdef CONFIG_INTEGRITY
	[IH_OS_INTEGRITY] = do_bootm_integrity,
#endif
};

/*******************************************************************/
/* bootz - boot zImage in memory */
/*******************************************************************/
static int relocated = 0;

int do_bootz (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong		iflag;
	ulong		load_end = 0;
	int		ret;
	boot_os_fn	*boot_fn;
	int		is_zImage, is_autoboot;
	char		cmd_buf[CONFIG_SYS_CBSIZE];

	if(argc < 2)
		images.ep = CONFIG_SYS_LOAD_ADDR;
	else {
		if (isxdigit(*(argv[1])))
			images.ep = simple_strtoul(argv[1], NULL, 16);
		else {
			printf("Input address is not digital, ERROR!!\n");
			return -1;
		}
	}

	is_autoboot = (strncmp(getenv("autoboot"), "boot", 4) == 0) ? 1 : 0;

	/* Autoboot sequence is NAND, OneNAND .. */
	if (is_autoboot) {
		int n, col;
#ifdef CONFIG_CMD_NAND
		if (nand_curr_device >= 0) {
			/* NAND Device Exist, try to boot from NAND */
			printf("read zImage from NAND\n");
			run_command(CONFIG_NANDBOOT, 0);
#ifdef CONFIG_CMD_ONENAND
		} else if (onenand_mtd.size > 0) {
			/* OneNAND Device Exist, try to boot from OneNAND */
			printf("read zImage from OneNAND\n");
			run_command(CONFIG_ONENANDBOOT, 0);
#endif
		} else
#endif
			printf("No flash device, fail to AUTOBOOT!!!\n\n");
	}

	is_zImage = (((ulong *)images.ep)[9] == 0x016f2818) ? 1 : 0;
	/* AUTOBOOT support, check if the sepecified addr contain valid zImage */
	if (is_autoboot && !is_zImage) {
		printf("The address specified contains no valid zImage.\n AutoBOOT failed!!!\n\n");
		return 0;
	}

	images.os.os = IH_OS_LINUX;
	/* relocate boot function table */
	if (!relocated) {
		int i;
		for (i = 0; i < ARRAY_SIZE(boot_os); i++)
			boot_os[i] += gd->reloc_off;
		relocated = 1;
	}

	printf("Ready to boot %s from %x\n\n", is_zImage ? "zImage" : "Image", images.ep);
	boot_fn = boot_os[images.os.os];
	boot_fn(0, argc, argv, &images);

	show_boot_progress (-9);
#ifdef DEBUG
	puts ("\n## Control returned to monitor - resetting...\n");
#endif
	do_reset (cmdtp, flag, argc, argv);

	return 1;
}

U_BOOT_CMD(
        bootz,   CONFIG_SYS_MAXARGS,      1,      do_bootz,
        "bootz   - boot zImge from memory\n",          
        "[addr]"
);

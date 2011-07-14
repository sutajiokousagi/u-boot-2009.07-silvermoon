/*
 * (C) Copyright 2008
 * Marvell Semiconductors Ltd. Shanghai, China.
 *
 * Configuration for Tavor EVB board.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/************/
/* VERSIONS */
/************/
#define CONFIG_IDENT_STRING         "\nMarvell version: 1.1.1.1 PXAxxx"

#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2 "chumby> "

// Enable various nice commands.
#define CONFIG_CMD_MISC
#define CONFIG_CMD_ECHO
#define CONFIG_CMD_IR
#define CONFIG_CMD_RUN
#define CONFIG_CMD_IMI
//#define CONFIG_CMD_ELF
#define CONFIG_CMD_STRINGS
#define CONFIG_CMD_SETEXPR
#define CONFIG_CMD_TOUCHSCREEN
//#define CONFIG_CMD_TERMINAL

#define CONFIG_BOOTM_LINUX

// Add support for LCD.
#define CONFIG_LCD
#define CONFIG_SILVERMOON_LCD
#define CONFIG_CMD_UNZIP
#define CONFIG_CMD_BMP
//#define CONFIG_LZMA
#define CONFIG_VIDEO_BMP_GZIP
#define CONFIG_SYS_VIDEO_LOGO_MAX_SIZE (2<<20)
//#define CONFIG_CONSOLE_MUX // Output data to serial console and LCD.
#define CONFIG_USE_IRQ
#define CONFIG_USE_FIQ

// add support for FPGA
#define CONFIG_CMD_FPGA
#define CONFIG_FPGA_XILINX
#define CONFIG_FPGA
#define CONFIG_FPGA_SPARTAN6
#define CONFIG_FPGA_COUNT  1

// Enable support for useful filesystems.
//#define CONFIG_CMD_FAT
#define CONFIG_CMD_CCONFIG

/* version number passing when loading Kernel */
#define VER_NUM             0x01010101  /* 1.1.1.18 */
/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_CPU_PXA910       1 /* This is an pxa910 core*/
#define CONFIG_CPU_PXA910_168       1 /* pxa168 SOC */
#define CONFIG_PXAXXX           1 /*  pxa family */
#define CONFIG_ASPENITE         1

#if defined(CHUMBY_CONFIG_platform)
#define CONFIG_SYS_BOARD_NAME       "88SV331xV5 based Silvermoon"
#define CONFIG_SYS_VENDOR_NAME      "Chumby"
#else
#define CONFIG_SYS_BOARD_NAME       "88SV331xV5 based PXAxxx"
#define CONFIG_SYS_VENDOR_NAME      "MARVELL"
#endif

#define CONFIG_SYS_MEMTEST_START        0x00100000
#define CONFIG_SYS_MEMTEST_END          0x00F00000

#define CONFIG_SYS_HZ           (3250000)      /* KV - Timer 0 is clocked at 3.25 MHz */
#define CONFIG_SYS_TIMERBASE        0xD4014000
#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_MISC_INIT_R          1   /* call misc_init_r during start up */

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN       (CONFIG_SYS_ENV_SIZE + 4096*1024)
#define CONFIG_SYS_GBL_DATA_SIZE    128   /* size in bytes reserved for initial data */

/*
 *  Configuration
 */
#define CONFIG_AUTO_COMPLETE
#define CONFIG_CONS_INDEX           1
#undef  CONFIG_SERIAL_SOFTWARE_FIFO
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE     -4
#define CONFIG_SYS_NS16550_CLK          14745600
#define CONFIG_BAUDRATE             115200
#define CONFIG_SYS_BAUDRATE_TABLE       { 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_SYS_NS16550_COM1         0xD4017000
#define CONFIG_SYS_NS16550_COM2         0xD4026000

#define CONFIG_SHOW_BOOT_PROGRESS

/*
#if !defined(CHUMBY_CONFIG_platform)
#define CONFIG_CMD_PING
#define CONFIG_CMD_NET
#define CONFIG_NET_MULTI
#define MV_ETH_DEVS             1

#define CONFIG_IPADDR           192.168.1.101

#define CONFIG_SERVERIP         192.168.1.100

#define CONFIG_ETHADDR          "00:00:5A:9F:6D:82"
#endif // !CHUMBY_CONFIG_platform
*/

/* enable passing of ATAGs  */
#define CONFIG_CMDLINE_TAG          1
#define CONFIG_SETUP_MEMORY_TAGS    1
#define CONFIG_SYS_TCLK             0 /* not in use */
#define CONFIG_SYS_BUS_CLK          0 /* not in use */
#define CONFIG_CMD_ENV
#define CONFIG_ENV_SIZE         0x20000
#define CONFIG_ENV_OFFSET       0x80000

/***************************************/
/* LINUX BOOT and other ENV PARAMETERS */
/***************************************/
#define CONFIG_SYS_BOOTARGS_END         ":::ASPENITE:eth0:none"
#define CONFIG_SYS_BOOTARGS_ROOT        "root=/dev/nfs rw init=/linuxrc"
#define CONFIG_SYS_LOAD_ADDR            0x00500000   /* default load address   */
#define CONFIG_SYS_DEF_LOAD_ADDR        "0x00500000"
#define CONFIG_SYS_IMG_NAME     "zImage"
#define CONFIG_SYS_INITRD_NAME          "ramdisk.image.gz"
#define CONFIG_SYS_INITRD_LOAD_ADDR     "800000"
#define CONFIG_SYS_INITRD_SIZE          "400000"
#undef  CONFIG_BOOTARGS


/*-----------------------------------------------------------------------
 * MMC configuration
 */
#if !defined(CHUMBY_CONFIG_platform)
#define CFG_CMD_FAT     1
#define CONFIG_CMD_FAT      1
#endif
#define CONFIG_CMD_MMC      1
#define CONFIG_MMC_MULTBLK_RW_SUPPORT
#define CONFIG_MMC      1
#define CONFIG_MMC3     1
#define CONFIG_GENERIC_MMC  1
#define CONFIG_PXASDH       1
#define CONFIG_DOS_PARTITION    1

#ifndef CONFIG_MMC3
#define CONFIG_SYS_MMC_BASE 0xd4280000
#else
#define CONFIG_SYS_MMC_BASE 0xd427E000
#endif

#ifndef CONFIG_MMC3
#define CONFIG_BOOTDELAY            10
#else
#if defined(CHUMBY_CNPLATFORM_silvermoon)
#define CONFIG_BOOTDELAY            0
#else
#define CONFIG_BOOTDELAY            -1
#endif
#endif


#if (CONFIG_BOOTDELAY >= 0)

/* boot arguments" */
#define CONFIG_CMD_BDI              // Enable bdinfo command
#define CONFIG_CMD_MEMORY       /* md mm nm mw cp cmp crc base loop mtest */
#define CONFIG_CMD_EXT2         /* EXT2 Support                 */

#define CONFIG_BOOTCOMMAND                                      \
    /* Init SD card */                                          \
    "mmc rescan ;"                                              \
    " ;"                                                        \
    /* Test to see which Boot Partition is active */            \
    "PART=mmcblk0p2; PARTNAME=rfsA; KF=2; KB=krnA; "            \
    " ;"                                                        \
    /* Have a pretty, flashy startup screen */                  \
    /* Draw an image to the screen */                           \
    "echo \"Drawing something to the screen...\" ;"             \
    "snow init ${BP} ;"                                         \
    " ;"                                                        \
    /* Load the FPGA, which lets us draw to the screen */       \
    "echo \"Loading FPGA configuration.\" ;"                    \
    "ext2load mmc 0:2 ${default_load_addr} /lib/firmware/hdmi_720p.bin ;" \
    "fpga load 0 ${default_load_addr} 340604 ; "                \
    " ;"                                                        \
    "echo \"Loading boot logo.\" ;"                             \
    "ext2load mmc 0:2 ${default_load_addr} /boot/logo.raw.gz ;" \
    "unzip ${default_load_addr} ${lcdbase} ;"                   \
    " ;"                                                        \
    /* Wait for the user to press Control-C */                  \
    "echo \"Press Control-C to enter a shell.\" ;"              \
    "if sleep 1 ;"                                              \
    "    then echo \"Continuing boot...\" ;"                    \
    "    else echo \"Chumby shell\"; echo \"Type 'run bootcmd' to exit\"; exit ;" \
    "fi ;"                                                      \
    " ;"                                                        \
    /* If the user is pressing the screen, swap partitions */   \
    "echo \"Booting to regular shell...\" ;"                \
    " ;"                                                        \
    /* Set up the Linux command line */                         \
    "setenv bootargs root=/dev/${PART} "                        \
            "rootwait console=ttyS0,115200 mem=128M "           \
            "uart_dma sysrq_always_enabled "                    \
            "brandnum=${brandnum} "                             \
            "rw logo.brand=${brand} ${RECOVERY} ;"              \
    "echo \"Booting to ${PARTNAME}...\" ;"                      \
    " ;"                                                        \
    /* Load the kernel from the config area */                  \
    /*"ext2load mmc 0:${KF} ${default_load_addr} /boot/zImage && cconfigload mmc 0 ${default_load_addr} ${KB};"*/\
    "cconfigload mmc 0 ${default_load_addr} ${KB};"             \
    "bootz ${default_load_addr} ;"                              \
    ""
    //"cconfigload mmc 0 ${default_load_addr} ${KERN} ;"


#define CONFIG_ROOTPATH         /tftpboot/rootfs_arm

#endif /* #if (CONFIG_BOOTDELAY >= 0) */

#define CONFIG_SYS_BARGSIZE         CONFIG_SYS_CBSIZE   /* Boot Argument Buffer Size   */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ        (8<<20)   /* Initial Memory map for Linux */

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP         /* undef to save memory     */
#if defined(CHUMBY_CONFIG_name)
#define CONFIG_SYS_PROMPT           CHUMBY_CONFIG_name ">> "   /* Monitor Command Prompt   */
#else
#define CONFIG_SYS_PROMPT           "Aspenite>> "   /* Monitor Command Prompt   */
#endif

/* Console I/O Buffer Size  */
#define CONFIG_SYS_CBSIZE           512
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE           (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
/* max number of command args   */
#define CONFIG_SYS_MAXARGS          16
/* Boot Argument Buffer Size    */
#define CONFIG_SYS_BARGSIZE         CONFIG_SYS_CBSIZE
#undef  CONFIG_SYS_CLKS_IN_HZ           /* everything, incl board info, in Hz */
#define CONFIG_ENV_OVERWRITE        /* allow to change env parameters */
#undef  CONFIG_INIT_CRITICAL
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_64BIT_VSPRINTF


/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE            (128*1024)   /* regular stack */
#ifdef  CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ        (4*1024)   /* IRQ stack */
#define CONFIG_STACKSIZE_FIQ        (4*1024)   /* FIQ stack */
#endif


/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS        1   /* we have 1 bank of DRAM */
#define PHYS_SDRAM_1                0x00000000   /* SDRAM Bank #1 */
#if defined(CHUMBY_CONFIG_memsize)
#define PHYS_SDRAM_1_SIZE           (CHUMBY_CONFIG_memsize * 0x100000)  /* 128 MB */
#define PHYS_SDRAM_SIZE_DEC         CHUMBY_CONFIG_memsize
#else
#define PHYS_SDRAM_1_SIZE           0x04000000   /* 64 MB */
#define PHYS_SDRAM_SIZE_DEC         64
#endif
#define CONFIG_SYS_ENV_SIZE             0x10000   /* Total Size of Environment Sector */
#define CONFIG_ENV_IS_NOWHERE       1
#define CMD_SAVEENV         1
#define CONFIG_SYS_NO_FLASH     1
#ifndef CONFIG_MMC3
#define CMD_SAVEENV         1
#define CONFIG_BBM          1
#endif

/*-----------------------------------------------------------------------
 * cpuid configuration
 */
#define CONFIG_CMD_CPUID        1

#define CONFIG_USB_ETH
#define CONFIG_U2O_REG_BASE     0xd4208000
#define CONFIG_U2O_PHY_BASE     0xd4207000

#define CONFIG_CMD_PING
#define CONFIG_CMD_NET
#define CONFIG_NET_MULTI
#define MV_ETH_DEVS             1
#define CONFIG_IPADDR           10.0.88.1
#define CONFIG_SERVERIP         10.0.88.4
#define CONFIG_ETHADDR          "00:00:5A:9F:6D:82"

#endif
/* __CONFIG_H */

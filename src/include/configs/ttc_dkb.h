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
#define CONFIG_IDENT_STRING   		"\nMarvell version: 1.1.1.1 Hermon2"

/* version number passing when loading Kernel */
#define VER_NUM 			0x01010101	/* 1.1.1.18 */
/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_CPU_PXA910		1 /* This is an pxa910 core*/
#define CONFIG_PXAXXX      		1 /*  pxa family */
#define CONFIG_TTC_DKB    		1
#define CONFIG_FFUART

#define CONFIG_SYS_BOARD_NAME		"ARM926 based Hermon2"
#define CONFIG_SYS_VENDOR_NAME     	"MARVELL"

#define CONFIG_SYS_MEMTEST_START      	0x00100000
#define CONFIG_SYS_MEMTEST_END        	0x00F00000

#define CONFIG_SYS_HZ   		(3250000)      /* KV - Timer 0 is clocked at 3.25 MHz */
#define CONFIG_SYS_TIMERBASE 		0xD4014000 
#define CONFIG_SYS_CPUSPEED		0x161		/* set core clock to 400/200/100 MHz */
#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_CMDLINE_TAG         	1   /* enable passing of ATAGs  */
#define CONFIG_SETUP_MEMORY_TAGS   	1
#define CONFIG_MISC_INIT_R         	1   /* call misc_init_r during start up */

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN      	(CONFIG_SYS_ENV_SIZE + 512*1024)
#define CONFIG_SYS_GBL_DATA_SIZE   	128   /* size in bytes reserved for initial data */

/*
 *  Configuration
 */
#define CONFIG_AUTO_COMPLETE
#define CONFIG_CONS_INDEX     		1
#undef  CONFIG_SERIAL_SOFTWARE_FIFO
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE   	-4
#define CONFIG_SYS_NS16550_CLK        	14745600
#define CONFIG_BAUDRATE        		115200
#define CONFIG_SYS_BAUDRATE_TABLE     	{ 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_SYS_NS16550_COM1       	0xD4017000 /* configure for PXA910*/

#define CONFIG_SHOW_BOOT_PROGRESS

#define CONFIG_CMD_NET
#define CONFIG_NET_MULTI
#define MV_ETH_DEVS 			1

#define CONFIG_IPADDR      		192.168.1.101

#define CONFIG_SERVERIP    		192.168.1.100

#define CONFIG_ETHADDR			"00:00:5A:9F:6D:82"

/* enable passing of ATAGs  */
#define CONFIG_CMDLINE_TAG       	1
#define CONFIG_SETUP_MEMORY_TAGS 	1
#define CONFIG_SYS_TCLK         	0 /* not in use */
#define CONFIG_SYS_BUS_CLK         	0 /* not in use */
#define CONFIG_ENV_SIZE		 	0x4000
#define CONFIG_ENV_OFFSET        	0x40000

/***************************************/
/* LINUX BOOT and other ENV PARAMETERS */
/***************************************/
#define CONFIG_SYS_BOOTARGS_END     	":::TTC_DKB:eth0:none"
#define CONFIG_SYS_BOOTARGS_ROOT    	"root=/dev/nfs rw init=/linuxrc"
#define CONFIG_ZERO_BOOTDELAY_CHECK
#define CONFIG_SYS_LOAD_ADDR        	0x00500000   /* default load address   */
#define CONFIG_SYS_DEF_LOAD_ADDR    	"0x00500000"
#define CONFIG_SYS_IMG_NAME		"zImage"
#define CONFIG_SYS_INITRD_NAME      	"ramdisk.image.gz"
#define CONFIG_SYS_INITRD_LOAD_ADDR 	"800000"
#define CONFIG_SYS_INITRD_SIZE      	"400000"
#undef  CONFIG_BOOTARGS

#define CONFIG_BOOTDELAY        	10

#if (CONFIG_BOOTDELAY >= 0)

/* boot arguments" */
#define CONFIG_ONENANDBOOT	     	"onenand read 0x500000 0x920000 0x300000"
#define CONFIG_NANDBOOT 	     	"nand read 0x500000 0x920000 0x300000"
#define CONFIG_BOOTCOMMAND      	"setenv autoboot boot; bootz 0x500000; setenv autoboot none"
#define CONFIG_ROOTPATH   		/tftpboot/rootfs_arm

#endif /* #if (CONFIG_BOOTDELAY >= 0) */

#define CONFIG_SYS_BARGSIZE   		CONFIG_SYS_CBSIZE   /* Boot Argument Buffer Size   */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ      	(8<<20)   /* Initial Memory map for Linux */

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP        	/* undef to save memory     */
#define CONFIG_SYS_PROMPT         	"TTC_DKB>> "   /* Monitor Command Prompt   */
#define CONFIG_SYS_PROMPT_HUSH_PS2     	"> "
/* Console I/O Buffer Size  */
#define CONFIG_SYS_CBSIZE         	512
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE         	(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
/* max number of command args   */
#define CONFIG_SYS_MAXARGS        	16
/* Boot Argument Buffer Size    */
#define CONFIG_SYS_BARGSIZE       	CONFIG_SYS_CBSIZE
#undef  CONFIG_SYS_CLKS_IN_HZ         	/* everything, incl board info, in Hz */
#define CONFIG_ENV_OVERWRITE    	/* allow to change env parameters */
#undef  CONFIG_INIT_CRITICAL
#define CONFIG_CMDLINE_EDITING
#define CONFIG_COMMAND_HISTORY
#define CONFIG_COMMAND_EDIT
#define CONFIG_SYS_64BIT_VSPRINTF


/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE       		(128*1024)   /* regular stack */
#ifdef  CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ   		(4*1024)   /* IRQ stack */
#define CONFIG_STACKSIZE_FIQ   		(4*1024)   /* FIQ stack */
#endif

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS    	1   /* we have 1 bank of DRAM */
#define PHYS_SDRAM_1            	0x00000000   /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE       	0x08000000   /* 32 MB */
#define PHYS_SDRAM_SIZE_DEC     	128
#define CONFIG_SYS_ENV_SIZE            	0x10000   /* Total Size of Environment Sector */
#define	CONFIG_ENV_IS_NOWHERE		1
#define CONFIG_SYS_NO_FLASH		1
#define CONFIG_BBM			1

/*-----------------------------------------------------------------------
 * NAND and DFC configuration
 */
#define CONFIG_CMD_NAND 		1
#define CONFIG_SYS_MAX_NAND_DEVICE	1         /* Max number of NAND devices */
#define CONFIG_SYS_NAND_BASE		0xD4283000

/*-----------------------------------------------------------------------
 * ONENAND configuration
 */
#define CONFIG_CMD_ONENAND 		1
#define CONFIG_SYS_ONENAND_BASE 	0x80000000  /* configure for PXA910*/

/*-----------------------------------------------------------------------
 * FREQUENCE configuration
 */
#define CONFIG_CMD_FREQUENCE		1

#undef TURN_ON_ETHERNET
#define TURN_ON_ETHERNET
#ifdef TURN_ON_ETHERNET
#define CONFIG_USB_ETH			1
#define CONFIG_U2O_REG_BASE		0xd4208000
#define CONFIG_U2O_PHY_BASE		0xd4207000
#define CONFIG_DRIVER_SMC91111 		1
#define CONFIG_SMC91111_BASE   		0x90000300 /* PXA910*/
#define CONFIG_SMC_USE_16_BIT
#undef CONFIG_SMC_USE_IOFUNCS          /* just for use with the kernel */
#endif
#define CONFIG_NET_RETRY_COUNT 		10000

#endif
/* __CONFIG_H */

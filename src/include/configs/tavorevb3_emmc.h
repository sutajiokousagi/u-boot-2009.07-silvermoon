/*
 * (C) Copyright 2008
 * Marvell Semiconductors Ltd. Shanghai, China.
 *
 * Configuration for Tavor EVB board with EMMC booting.
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

#ifndef __TAVOREVB_EMMC_CONFIG_H
#define __TAVOREVB_EMMC_CONFIG_H

#include <configs/tavorevb3.h>
#define BOOT_FROM_EMMC
#define CONFIG_MMCBOOT			"mmc read 0x4908 0x17f8 0x80800000"
#define CONFIG_MBR_SECTOR		0x4900
#define CONFIG_MMC_BUSTEST_SUPPORT
#define CONFIG_MMC_EXT_CSD_PART_SUPPORT
#define CONFIG_MMC_MULTBLK_RW_SUPPORT

#endif


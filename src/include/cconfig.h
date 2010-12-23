/*
 * R/O Chumby Config filesystem driver by Sean Cross
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

#ifndef _CCONFIG_H_
#define _CCONFIG_H_

#include <asm/byteorder.h>

#define FS_BLOCK_SIZE 512
#define SECTOR_SIZE FS_BLOCK_SIZE


// Current config area version in array format
#define SD_CONFIG_AREA_VER	1,0,0,1

// Fixed offset within partition 1 for start of config area
#define SD_CONFIG_AREA_PART1_OFFSET	0xc000

// Length of config area
#define SD_CONFIG_AREA_LENGTH			0x4000

// pragma pack() not supported on all platforms, so we make everything
// dword-aligned using arrays
// WARNING: we're being lazy here and assuming that the platform any utility
// using this is running on little-endian! Otherwise you'll need to convert
// u32 values.
typedef union {
	char name[4];
	unsigned int uname;
} block_def_name;


// The first partition may contain multiple blocks, as a sort of
// partition-within-a-partition.  This data structure defines the block's
// structure.
// As a special case, a block definition with an offset of 0xffffffff
// indicates the end of the block definition.
struct block_def {

    // Offset from start of partition 1.
	unsigned int offset;


    // Length of block in bytes.
	unsigned int length;


    // Version of this block data, e.g. 1,0,0,0
	unsigned char block_ver[4];


    // Name of block, e.g. "krnA".
    // Note: not NULL-terminated.
    // Allowed symbols are [a-zA-Z0-9_].
	block_def_name n;


};



// This defines the configuration area of the SD card, which immediately
// follows the stage-1 bootloader.  It's supposed to reside at offset
// 0xc000 from the start of the partition.
struct config_area {

    // Contains the unterminated string "Cfg*".
	char sig[4];


    // Contains the u8 array {1, 0, 0, 0}, defining the config block
    // version.
	unsigned char area_version[4];


    // Element 0 contains the partition number to boot from by default.
	unsigned char active_index;
    unsigned char active_index_padding[3];


    // Element 0 is 1 if update in progress.
	unsigned char updating;
    unsigned char updating_padding[3];


    // NULL-terminated version string of last successful update, e.g. "1.7.1892"
	char last_update[16];


    // Offset in bytes from start of device to start of partition 1
	unsigned int p1_offset;


    // Reserved for future use.
	unsigned char reserved1[220];


    // NULL-terminated CONFIGNAME of current build, e.g. "falconwing".
	char configname[128];


    // Reserved for future use.
	unsigned char reserved2[128];


    // Backup copy of MBR.
	unsigned char mbr_backup[512];


    // Block table entries ending with offset==0xffffffff.
	struct block_def block_table[64];


    // Reserved for future use.
	unsigned char reserved3[0];


};



typedef int	(file_detectfs_func)(struct config_area *ca);
typedef int	(file_ls_func)(void);
typedef long	(file_read_func)(const char *filename, void *buffer,
				 unsigned long maxsize);

struct filesystem {
	file_detectfs_func *detect;
	file_ls_func	   *ls;
	file_read_func	   *read;
	const char	    name[12];
};

/* FAT tables */
file_detectfs_func	file_cconfig_detectfs;
file_ls_func		file_cconfig_ls;
file_read_func		file_cconfig_read;

/* Currently this doesn't check if the dir exists or is valid... */
int file_cconfig_detectfs(struct config_area *ca);
int file_cconfig_ls(void);
long file_cconfig_read(const char *filename, void *buffer, unsigned long maxsize);
int cconfig_register_device(block_dev_desc_t *dev_desc, int part_no);

#endif /* _CCONFIG_H_ */

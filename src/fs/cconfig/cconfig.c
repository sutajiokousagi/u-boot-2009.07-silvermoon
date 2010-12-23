/*
 * cconfig.c
 *
 * R/O Chumby Config partition support by Sean Cross
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
 */

#include <common.h>
#include <config.h>
#include <asm/byteorder.h>
#include <part.h>
#include <cconfig.h>

extern  block_dev_desc_t *cur_dev = NULL;
static unsigned long part_offset = 0;
static int cur_part = 1;

#define DOS_PART_TBL_OFFSET	0x1be
#define DOS_PART_MAGIC_OFFSET	0x1fe
#define DOS_FS_TYPE_OFFSET	0x36

/*
static int disk_read (__u32 startblock, __u32 getsize, __u8 * bufptr)
{
	startblock += part_offset;
	if (cur_dev == NULL)
		return -1;
	if (cur_dev->block_read) {
		return cur_dev->block_read (cur_dev->dev
			, startblock, getsize, (unsigned long *)bufptr);
	}
	return -1;
}
*/


int
cconfig_register_device(block_dev_desc_t *dev_desc, int part_no)
{
	unsigned char buffer[512];
	disk_partition_t info;

    if(!dev_desc) {
        printf("Called cconfig_register_device with a NULL dev_desc\n");
        return -1;
    }

	if (!dev_desc->block_read) {
        printf("No block_read function found\n");
		return -1;
    }

	cur_dev = dev_desc;
	/* check if we have a MBR (on floppies we have only a PBR) */
	if (dev_desc->block_read (dev_desc->dev, 0, 1, (ulong *) buffer) != 1) {
		printf ("** Can't read from device %d **\n", dev_desc->dev);
		return -1;
	}
	if (buffer[DOS_PART_MAGIC_OFFSET] != 0x55 ||
		buffer[DOS_PART_MAGIC_OFFSET + 1] != 0xaa) {
		/* no signature found */
		return -1;
	}

	return 0;
}



int
file_cconfig_detectfs(struct config_area *ca)
{
	if(cur_dev==NULL) {
		printf("No current device\n");
		return 1;
	}

    if(!cur_dev->block_read) {
        printf("No block reader\n");
        return 1;
    }

    //printf("Reading 100 blocks from device\n");
    cur_dev->block_read(cur_dev->dev, 100, sizeof(struct config_area)/512, ca);

    // If it has a valid signature, return true.
    if(ca->sig[0] == 'C' && ca->sig[1] == 'f'
    && ca->sig[2] == 'g' && ca->sig[3] == '*') {
        return 0;
    }

    // Return 1 since it's not a config partition.
    printf("No valid block found\n");
	return 1;
}


int
file_cconfig_ls(void)
{
    struct config_area ca;
    struct block_def  *bd;
    int                block_count = 0;

    if(!cur_dev) {
        printf("No current device\n");
        return -1;
    }

    cur_dev->block_read(cur_dev->dev, 100, sizeof(ca)/512, &ca);

    bd = ca.block_table;
    printf("  Part       Size    Offset Vers    Name\n");
    while(bd->offset != 0xffffffff && block_count < 64) {
        char name[5];
        char vers[5];

        vers[4] = '\0';
        name[4] = '\0';
        memcpy(name, bd->n.name,    4);
        memcpy(vers, bd->block_ver, 4);

        printf("    %2d   %8d  %8d   v%4s  %4s\n",
               block_count, bd->length, bd->offset, vers, name);

        bd++;
        block_count++;
    }

    
	return 0; //do_fat_read(dir, NULL, 0, LS_YES);
}


long
file_cconfig_read(const char *filename, void *buffer, unsigned long maxsize)
{
    struct config_area ca;
    struct block_def  *bd;
    int                block_count = 0;

    if(!cur_dev) {
        printf("No current device\n");
        return -1;
    }


    cur_dev->block_read(cur_dev->dev, 100, sizeof(ca)/512, &ca);
    bd = ca.block_table;
    while(bd->offset != 0xffffffff && block_count < 64) {
        char name[5];
        name[4] = 0;
        memcpy(name, bd->n.name, 4);
        if(!strcmp(name, filename)) {

            printf("Found file of length %d at position %d\n",
                    bd->length, bd->offset);

            // Take the lesser of maxsize or bd->length.
            if(maxsize && maxsize < bd->length)
                bd->length = maxsize;

            // Convert offsets from partition to global.
            bd->offset += 2048;

            cur_dev->block_read(cur_dev->dev,   bd->offset/512,
                                bd->length/512, buffer);


            return bd->length;
        }
        bd++;
        block_count++;
    }
    printf("File \"%s\" not found.\n", filename);

    return 0;
}

/*
 * (C) Copyright 2002
 * Richard Jones, rjones@nexus-tech.net
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

/*
 * Boot support
 */
#include <common.h>
#include <command.h>
#include <s_record.h>
#include <net.h>
#include <ata.h>
#include <part.h>
#include <cconfig.h>


int do_cconfig_fsload (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	long size;
	unsigned long offset;
	unsigned long count;
	char buf [12];
	block_dev_desc_t *dev_desc=NULL;
	int dev=0;
	int part=1;
	char *ep;

	if (argc < 5) {
		printf ("usage: cconfigload <interface> <dev> <addr> <filename> [bytes]\n");
		return 1;
	}
	dev = (int)simple_strtoul (argv[2], &ep, 16);
	dev_desc=get_dev(argv[1],dev);
	if (dev_desc==NULL) {
		puts ("\n** Invalid boot device **\n");
		return 1;
	}
	if (*ep) {
		if (*ep != ':') {
			puts ("\n** Invalid boot device, use `dev' **\n");
			return 1;
		}
		part = (int)simple_strtoul(++ep, NULL, 16);
	}
	if (cconfig_register_device(dev_desc,part)!=0) {
		printf ("\n** Unable to use %s %d:%d for cconfigload **\n",argv[1],dev,part);
		return 1;
	}
	offset = simple_strtoul (argv[3], NULL, 16);
	if (argc == 6)
		count = simple_strtoul (argv[5], NULL, 16);
	else
		count = 0;
	size = file_cconfig_read (argv[4], (unsigned char *) offset, count);

	if(size==-1) {
		printf("\n** Unable to read \"%s\" from %s %d:%d **\n",argv[4],argv[1],dev,part);
		return 1;
	}

	printf ("\n%ld bytes read\n", size);

	sprintf(buf, "%lX", size);
	setenv("filesize", buf);

	return 0;
}


U_BOOT_CMD(
	cconfigload,	6,	0,	do_cconfig_fsload,
	"cconfigload - load binary file from a dos filesystem\n",
	"<interface> <dev>  <addr> <filename> [bytes]\n"
	"    - load binary file 'filename' from 'dev' on 'interface'\n"
	"      to address 'addr' from cconfig filesystem\n"
);

int do_cconfig_ls (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret;
	int dev=0;
	int part=1;
	char *ep;
	block_dev_desc_t *dev_desc=NULL;

	if (argc < 3) {
		printf ("usage: cconfigls <interface> <dev>\n");
		return (0);
	}
	dev = (int)simple_strtoul (argv[2], &ep, 16);
	dev_desc=get_dev(argv[1],dev);
	if (dev_desc==NULL) {
		puts ("\n** Invalid boot device **\n");
		return 1;
	}
	if (*ep) {
		if (*ep != ':') {
			puts ("\n** Invalid boot device, use `dev' **\n");
			return 1;
		}
		part = (int)simple_strtoul(++ep, NULL, 16);
	}
	if (cconfig_register_device(dev_desc,part)!=0) {
		printf ("\n** Unable to use %s %d:%d for cconfigls **\n",argv[1],dev,part);
		return 1;
	}
    ret = file_cconfig_ls();

	if(ret!=0)
		printf("No Chumby config FS detected\n");
	return (ret);
}

U_BOOT_CMD(
	cconfigls,	4,	1,	do_cconfig_ls,
	"cconfigls   - list files in chumby configuration area\n",
	"<interface> <dev> [directory]\n"
	"    - list files from 'dev' on 'interface' in config area\n"
);

int do_cconfig_fsinfo (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int dev=0;
	int part=1;
	char *ep;
    struct config_area ca;
	block_dev_desc_t *dev_desc=NULL;

	if (argc < 4) {
		printf ("usage: cconfiginfo <interface> <dev> <info>\n");
		return (0);
	}
	dev = (int)simple_strtoul (argv[2], &ep, 16);
	dev_desc=get_dev(argv[1],dev);
	if (dev_desc==NULL) {
		puts ("\n** Invalid boot device **\n");
		return 1;
	}
	if (*ep) {
		if (*ep != ':') {
			puts ("\n** Invalid boot device, use `dev' **\n");
			return 1;
		}
		part = (int)simple_strtoul(++ep, NULL, 16);
	}
	if (cconfig_register_device(dev_desc,part)!=0) {
		printf ("\n** Unable to use %s %d:%d for cconfiginfo **\n",argv[1],dev,part);
		return 1;
	}


    if(file_cconfig_detectfs(&ca)) {
        printf("No cconfig partition found\n");
        return 1;
    }

    if(!strcmp(argv[3], "active")) {
        printf("Active partition: %d\n", ca.active_index);
        return ca.active_index;
    }
    else if(!strcmp(argv[3], "updating")) {
        printf("Updating? %d\n", ca.updating);
        return ca.updating;
    }
    else if(!strcmp(argv[3], "offset")) {
        printf("Partition 1 offset: %d\n", ca.p1_offset);
        return ca.p1_offset;
    }
    else if(!strcmp(argv[3], "version")) {
        printf("Config version %d.%d.%d.%d\n",
               ca.area_version[0], ca.area_version[1],
               ca.area_version[2], ca.area_version[3]);
        return ca.area_version[0] << 24 | ca.area_version[1] << 16
             | ca.area_version[2] << 8  | ca.area_version[3];
    }
    else {
        printf("Unrecognized info type '%s'\n", argv[3]);
        return 0;
    }
}

U_BOOT_CMD(
	cconfiginfo,	4,	1,	do_cconfig_fsinfo,
	"cconfiginfo - print information about filesystem\n",
	"<interface> <dev> <info>\n"
	"    - print information about config partition from 'dev' on 'interface'\n"
    "      <info> must be one of active, updating, offset, or version\n"
);



extern int i2c_readb(unsigned int slave_addr, unsigned char reg, unsigned char *pval);
int i2c_init(unsigned int dev_id);
int do_cbrand (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int pos = 0;
	uint8_t hdr[4] = { 's', 'e', 'x', 'i' };
	uint8_t ftr[4] = { 'p', 'u', 's', '!' };
	uint8_t cmp[4];

	setenv("brand", "chumby");
	i2c_init(0);

	// Read in the header and verify it.
	for(pos=0; pos<sizeof(hdr) && pos<768; pos++) {
		if(i2c_readb(0x50, pos, cmp+pos)) {
			printf("I2C read error\n");
			return 1;
		}
	}
	if(memcmp(hdr, cmp, sizeof(hdr))) {
		printf("DCID is uninitialized\n");
		return 1;
	}


	// Continue looking until either (1) we get 'part', or (2) we fall off.
	while(pos<768) {
		uint16_t size;
		uint8_t container;
		int blk_pos;
		for(blk_pos=0; blk_pos<sizeof(cmp) && pos<768; blk_pos++) {
			if(i2c_readb(0x50+((pos>>8)&3), pos&0x00000000ff, cmp+blk_pos)) {
				printf("I2C read error\n");
				return 1;
			}
			pos++;
		}

		if(cmp[0]=='p' && cmp[1]=='u' && cmp[2]=='s' && cmp[3]=='!') {
			return 0;
		}

		// Since it's not the footer, we have another tag to read.  Figure
		// out the size, then copy the rest of the tag title.
		container = (cmp[1] & 0x80);
		((uint8_t *)&size)[1] = cmp[0];
		((uint8_t *)&size)[0] = cmp[1] & 0x7f;
		size -= 6; // Strip the header off the size.
		cmp[0] = cmp[2];
		cmp[1] = cmp[3];
		if(i2c_readb(0x50+(pos>>8), pos&0xff, cmp+2)) {
			printf("I2C read error\n");
			return 1;
		}
		pos++;
		if(i2c_readb(0x50+(pos>>8), pos&0xff, cmp+3)) {
			printf("I2C read error\n");
			return 1;
		}
		pos++;

		if(!container) {
			char data[size+1];
			data[size]='\0';
			for(blk_pos=0; blk_pos<size && pos<768; blk_pos++) {
				i2c_readb(0x50+(pos>>8), pos&0xff, data+blk_pos);
				pos++;
			}
			if(cmp[0]=='p' && cmp[1]=='a' && cmp[2]=='r' && cmp[3]=='t') {
				//int partnum = simple_strtoul(data, NULL, 0);
				uint16_t partnum;
				char partnum_s[16] = "1001";
				// The DCID is stored in a very... special... manner.
				((uint8_t *)&partnum)[0] = data[0];
				((uint8_t *)&partnum)[1] = data[1];
				sprintf(partnum_s, "%02X%02X", data[0], data[1]);

				// Set the brand from a hardcoded lookup table.
				if(partnum==272)
					setenv("brand", "insignia");
				setenv("brandnum", partnum_s);
				return partnum;
			}
		}
	}

	return 0;
}

U_BOOT_CMD(
	cbrand, 1, 1, do_cbrand,
	"cbrand - get information about chumby brand\n",
	"Get information about the brand, and store it in the variable 'brand'\n"
);

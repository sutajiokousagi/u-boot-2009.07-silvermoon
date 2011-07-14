/*
 * (C) Copyright 2003
 * Kyle Harris, kharris@nexus-tech.net
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
#include <command.h>
#include <mmc.h>

#ifndef CONFIG_GENERIC_MMC
static int curr_device = -1;

int do_mmc (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int dev;

	if (argc < 2) {
		cmd_usage(cmdtp);
		return 1;
	}

	if (strcmp(argv[1], "init") == 0) {
		if (argc == 2) {
			if (curr_device < 0)
				dev = 1;
			else
				dev = curr_device;
		} else if (argc == 3) {
			dev = (int)simple_strtoul(argv[2], NULL, 10);
		} else {
			cmd_usage(cmdtp);
			return 1;
		}

		if (mmc_legacy_init(dev) != 0) {
			puts("No MMC card found\n");
			return 1;
		}

		curr_device = dev;
		printf("mmc%d is available\n", curr_device);
	} else if (strcmp(argv[1], "device") == 0) {
		if (argc == 2) {
			if (curr_device < 0) {
				puts("No MMC device available\n");
				return 1;
			}
		} else if (argc == 3) {
			dev = (int)simple_strtoul(argv[2], NULL, 10);

#ifdef CONFIG_SYS_MMC_SET_DEV
			if (mmc_set_dev(dev) != 0)
				return 1;
#endif
			curr_device = dev;
		} else {
			cmd_usage(cmdtp);
			return 1;
		}

		printf("mmc%d is current device\n", curr_device);
	} else {
		cmd_usage(cmdtp);
		return 1;
	}

	return 0;
}

U_BOOT_CMD(
	mmc, 3, 1, do_mmc,
	"MMC sub-system\n",
	"init [dev] - init MMC sub system\n"
	"mmc device [dev] - show or set current device"
);
#else /* !CONFIG_GENERIC_MMC */

static int cur_dev = 0;
static void print_mmcinfo(struct mmc *mmc)
{
	printf("\tDevice Name: %s\n", mmc->name);
	printf("\tManufacturer ID: %x\n", mmc->bcid.manu_id);
	printf("\tOEM ID: %x\n", mmc->bcid.oem_id);
	printf("\tName: %s\n", mmc->bcid.prod_name);
	printf("\tManu Date: %02d,%04d\n\n", mmc->bcid.month, mmc->bcid.year + 1997);

	printf("\tTran Speed: %d MHZ\n", mmc->tran_speed / 1000000);
	printf("\tRd Block Len: %d\n", mmc->read_bl_len);
	printf("\tMax Rd Block Len: %d\n", mmc->max_read_bl_len);

	printf("\t%s version: %d.%d\n", IS_SD(mmc) ? "SD" : "MMC",
			(mmc->version >> 4) & 0xf, mmc->version & 0xf);

	printf("\tHigh Capacity: %s\n", mmc->high_capacity ? "Yes" : "No");
	printf("\tCapacity: %d MB\n", (int)(mmc->capacity / 1024 / 1024));
	printf("\tSectors: 0x%x\n", mmc->sectors);

	printf("\tBus Width: %d-bit\n", mmc->bus_width);
	printf("\tErase group: 0x%x\n", mmc->erase_grp_size);

#ifdef CONFIG_MMC_EXT_CSD_PART_SUPPORT
	if(!IS_SD(mmc)) {
		printf("\n\tMulti-partitions: %s\n",
			(mmc->ext_csd_part_support & 0x1) ? "Yes" : "No");
		printf("\tBoot_part_size: %d KB\n", mmc->ext_csd_boot_size / 1024);
		printf("\tPartition_config: 0x%x\n", mmc->ext_csd_part_config);
		printf("\tBoot_config_prot: 0x%x\n", mmc->ext_csd_boot_config_prot);
		printf("\tBoot_bus_width: 0x%x\n", mmc->ext_csd_boot_bus_width);
		printf("\tErase mem content: %d\n", mmc->ext_csd_erase_mem_content);
	}
#endif
}

int do_mmcinfo (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	struct mmc *mmc;
	int dev_num;

	if (argc < 2)
		dev_num = cur_dev;
	else
		dev_num = simple_strtoul(argv[1], NULL, 0);

	printf("\nSelected device: #%d\n", cur_dev);

	mmc = find_mmc_device(dev_num);

	if (mmc) {
		if (!mmc->block_dev.initialized && mmc_init(mmc)) {
			return 1;
		} else
			mmc->block_dev.initialized = 1;

		if (!IS_SD(mmc))
			printf("\tActive partition: %d\n", mmc->ext_csd_part_config & 0x7);
		print_mmcinfo(mmc);
		printf("\n");
		print_part_dos(&mmc->block_dev);
		printf("\n");
	}

	return 0;
}

U_BOOT_CMD(mmcinfo, 2, 0, do_mmcinfo,
	"mmcinfo - display MMC info\n",
	"mmcinfo <dev num>\n"
);

static int test_mem_range(struct mmc *mmc, u32 end_sct)
{
	char part_access = mmc->ext_csd_part_config & 0x7;

	if(!part_access) {
		if (mmc->sectors && (end_sct >= mmc->sectors)) {
			printf("Error, out of sector range(0x0, 0x%x)\n",
				mmc->sectors - 1);
			return 1;
		}
	} else if(part_access == 1 || part_access == 2) {
		u32 sectors = mmc->ext_csd_boot_size / 512;
		if (end_sct >= sectors) {
			printf("Error, out of sector range(0x0, 0x%x)\n",
				sectors);
			return 1;
		}
	}

	/* just have a try */
	return 0;
}

int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	u32 blk, cnt, num, ret;
	void *addr;

	struct mmc *mmc = find_mmc_device(cur_dev);
	if (!mmc) {
		printf("Error, no MMC device\n");
		return 1;
	}

	switch (argc) {
	case 2:
		if (!strcmp(argv[1], "list")) {
			print_mmc_devices('\n');
		} else if (strcmp(argv[1], "rescan") == 0) {
			if (mmc_init(mmc))
				goto err_mmc_init;
			else
				mmc->block_dev.initialized = 1;
		} else
			goto err_param;
		return 0;
	case 3:
		if (strcmp(argv[1], "sw_dev") == 0) {
			num = simple_strtoul(argv[2], NULL, 0);
			mmc = find_mmc_device(num);
			if (mmc)
				cur_dev = num;

			printf("switch to device #%d, %s\n",
				num, (mmc) ? "OK" : "ERROR");
			return (mmc) ? 0 : 1;
		}
	}

	if (!mmc->block_dev.initialized && mmc_init(mmc))
		goto err_mmc_init;
	else
		mmc->block_dev.initialized = 1;

	switch (argc) {
	case 3:
		num = simple_strtoul(argv[2], NULL, 0);

		if (strcmp(argv[1], "sw_part") == 0) {
			if (num > 3) {
				printf("#part_num should less than 4\n\n");
				goto err_param;
			}

#ifdef CONFIG_MMC_EXT_CSD_PART_SUPPORT
			if (!mmc->block_dev.switch_part) {
				printf("Error, no part_switch func!\n");
				return 1;
			}

			if (!(mmc->ext_csd_part_support & 0x1)) {
				printf("Card doesn't support part_switch\n");
				printf("boot_config has been diabled permanently\n");
				return -1;
			}
#endif

			ret = mmc->block_dev.switch_part(cur_dev, num);

			printf("switch to partions #%d, %s\n",
				num, (!ret) ? "OK" : "ERROR");
			return (!ret) ? 0 : 1;
		} else if (strcmp(argv[1], "sw_boot") == 0) {
			if (strcmp(argv[2], "-1") == 0)
				num = 0;
			else if (num > 2) {
				printf("#part_num should less than 3\n\n");
				goto err_param;
			}

			if (!mmc->block_dev.switch_boot) {
				printf("Error, no switch_boot func!\n");
				return 1;
			}

			if (mmc->ext_csd_boot_config_prot & 0x10) {
				printf("boot_config has been diabled permanently\n");
				return -1;
			}

			ret = mmc->block_dev.switch_boot(cur_dev, num);

			printf("switch boot from partion #%d, %s\n",
				num, (!ret) ? "OK" : "ERROR");
			return (!ret) ? 0 : 1;
		} else
			goto err_param;
		break;

	case 4:
	case 5:
		blk = simple_strtoul(argv[2], NULL, 0);
		cnt = simple_strtoul(argv[3], NULL, 0);

		addr = (void *)simple_strtoul(argv[4], NULL, 0);
#ifdef CONFIG_SYS_LOAD_ADDR
		if (argc == 4)
			addr = (void *)CONFIG_SYS_LOAD_ADDR;
#endif

		if (strcmp(argv[1], "erase") != 0
			&& strcmp(argv[1], "read") != 0
			&& strcmp(argv[1], "write") != 0)
			goto err_param;

		if (test_mem_range(mmc, blk + cnt - 1))
			return 1;

		if (strcmp(argv[1], "erase") == 0) {
			if (argc != 4)
				goto err_param;

			printf("\nMMC erase: dev #%d, block #0x%x, count #0x%x :\n",
				cur_dev, blk, cnt);

			if ((blk % mmc->erase_grp_size) || (cnt % mmc->erase_grp_size)) {
				printf("Error, the range is not erase_group_size(%d) aligned\n",
					mmc->erase_grp_size);
				return 1;
			}

			num = mmc->block_dev.block_erase(cur_dev, blk, cnt);

			printf("\n\t0x%x sectors erased, %s\n",
				num, (num == cnt) ? "OK" : "ERROR");
			return (num == cnt) ? 0 : 1;
		} else if (strcmp(argv[1], "read") == 0) {
#if !defined(CONFIG_SYS_LOAD_ADDR)
			if (argc != 5)
				goto err_param;
#endif
			printf("\nMMC read: dev #%d, block #0x%x, count #0x%x :\n",
				cur_dev, blk, cnt);

			num = mmc->block_dev.block_read(cur_dev, blk, cnt, addr);

			/* flush cache after read */
			flush_cache((ulong)addr, cnt * 512); /* FIXME */

			printf("\n\t0x%x sectors read to 0x%x, %s\n",
				num, (unsigned int) addr, (num == cnt) ? "OK" : "ERROR");
			return (num == cnt) ? 0 : 1;
		} else if (strcmp(argv[1], "write") == 0) {
#if !defined(CONFIG_SYS_LOAD_ADDR)
			if (argc != 5)
				goto err_param;
#endif
			printf("\nMMC write: dev # %d, block # 0x%x, count 0x%x :\n",
				cur_dev, blk, cnt);

			num = mmc->block_dev.block_write(cur_dev, blk, cnt, addr);

			printf("\n\t0x%x sectors written from 0x%x, %s\n",
				num, (unsigned int) addr, (num == cnt) ? "OK" : "ERROR");
			return (num == cnt) ? 0 : 1;
		} else
			goto err_param;
		break;

	default:
		goto err_param;
	}

	return 0;

err_mmc_init:
	mmc->block_dev.initialized = 0;
	return 1;

err_param:
	printf("Usage:\n%s\n", cmdtp->help);
	return 1;
}

U_BOOT_CMD(
	mmc, 6, 1, do_mmcops,
	"mmc     - MMC sub system\n",
	"mmc read #start_sct #sct_cnt [#addr]\n"
	"mmc write #start_sct #sct_cnt [#addr]\n"
	"mmc erase #start_sct #sct_cnt\n"
	"mmc rescan - rescan the current mmc device\n"
	"mmc sw_part #part_num - switch to another eMMC partition\n"
	"mmc sw_boot #part_num - boot from another eMMC partition\n"
	"mmc sw_dev #dev_num - switch to another eMMC device\n"
	"mmc list - lists available devices\n");
#endif

/*
 * Copyright 2008, Freescale Semiconductor, Inc
 * Andy Fleming
 *
 * Based vaguely on the Linux code
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

#include <config.h>
#include <common.h>
#include <command.h>
#include <mmc.h>
#include <part.h>
#include <malloc.h>
#include <linux/list.h>
#include <mmc.h>
#include <div64.h>

#define MMC_RETRY_TIME	3
#define BLKS_PER_READ	0x40
#define BLKS_PER_WRITE	0x40
static struct list_head mmc_devices;
static int cur_dev_num = -1;

int mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
	int retry = 0, res;

	do {
		res = mmc->send_cmd(mmc, cmd, data);
	} while (res && retry ++ < MMC_RETRY_TIME);

	return res;
}

#ifdef CONFIG_MMC_BUSTEST_SUPPORT
static int mmc_bus_test(struct mmc *mmc, int bus_width)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	char bus_test_buf[8], bus_test_ret[8];
	ushort result = 0;
	int err;

	cmd.cmdidx = MMC_CMD_BUSTEST_W;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;
	cmd.flags = 0;
	memset(bus_test_buf, 0, bus_width);
	if (bus_width == 4) {
		bus_test_buf[0] = 0x5A;
	} else if (bus_width == 8) {
		bus_test_buf[0] = 0x55;
		bus_test_buf[1] = 0xAA;
	} else {
		return -1;
	}
	data.dest = bus_test_buf;
	data.blocks = 1;
	data.blocksize = bus_width;
	data.flags = MMC_DATA_WRITE;
	err = mmc_send_cmd(mmc, &cmd, &data);
	if (err)
		return err;

	udelay(1000);
	cmd.cmdidx = MMC_CMD_BUSTEST_R;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;
	cmd.flags = 0;
	memset(bus_test_ret, 0, bus_width);
	data.dest = bus_test_ret;
	data.blocks = 1;
	data.blocksize = bus_width;
	data.flags = MMC_DATA_READ;
	err = mmc_send_cmd(mmc, &cmd, &data);
	if (err)
		return err;

	if (bus_width == 4) {
		result = ~(bus_test_buf[0] ^ bus_test_ret[0]);
		result &= 0xF;
	} else if (bus_width == 8) {
		result = ~(bus_test_buf[0] ^ bus_test_ret[0]);
		result &= 0xF;
		result |= ~(bus_test_buf[1] ^ bus_test_ret[1]) << 8;
	}
	if (result) {
		printf("mmc bus error: in %d-bit-mode bus_test returns: 0x%04x\n",
			bus_width, result);
		return -1;
	}

	return 0;
}
#endif

int mmc_set_blocklen(struct mmc *mmc, int len)
{
	struct mmc_cmd cmd;

	cmd.cmdidx = MMC_CMD_SET_BLOCKLEN;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = len;
	cmd.flags = 0;

	return mmc_send_cmd(mmc, &cmd, NULL);
}

struct mmc *find_mmc_device(int dev_num)
{
	struct mmc *m;
	struct list_head *entry;

	list_for_each(entry, &mmc_devices) {
		m = list_entry(entry, struct mmc, link);

		if (m->block_dev.dev == dev_num)
			return m;
	}

	printf("MMC Device %d not found\n", dev_num);

	return NULL;
}

int mmc_write_block(struct mmc *mmc, void *src, uint blocknum)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	cmd.cmdidx = MMC_CMD_WRITE_SINGLE_BLOCK;

	if (mmc->sect_mode_access)
		cmd.cmdarg = blocknum;
	else
		cmd.cmdarg = blocknum * mmc->write_bl_len;

	cmd.resp_type = MMC_RSP_R1;
	cmd.flags = 0;

	data.src = src;
	data.blocks = 1;
	data.blocksize = mmc->write_bl_len;
	data.flags = MMC_DATA_WRITE;

	return mmc_send_cmd(mmc, &cmd, &data);
}

#ifdef CONFIG_MMC_MULTBLK_RW_SUPPORT
static ulong mmc_write_t(struct mmc *mmc, ulong start, lbaint_t blkcnt, const void *src)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int err;
	lbaint_t blklen = mmc->read_bl_len;

	if (blkcnt >= 1)
		cmd.cmdidx = MMC_CMD_WRITE_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_CMD_WRITE_SINGLE_BLOCK;

	if (mmc->sect_mode_access)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * blklen;

	cmd.resp_type = MMC_RSP_R1;
	cmd.flags = 0;

	data.src = src;
	data.blocks = blkcnt;
	data.blocksize = blklen;
	data.flags = MMC_DATA_WRITE;

	err = mmc_send_cmd(mmc, &cmd, &data);

	if (err) {
		printf("mmc write failed\n\r");
		return err;
	}

	return 0;
}
#endif

static ulong
mmc_bwrite(int dev_num, ulong start, lbaint_t blkcnt, const void *src)
{
	struct mmc_cmd cmd;
	int err;
	struct mmc *mmc = find_mmc_device(dev_num);
	lbaint_t blklen, blk = 0;
#ifdef CONFIG_MMC_MULTBLK_RW_SUPPORT
	int stoperr = 0, blk_r = 0;
#endif

	if (!mmc)
		return -1;

	blklen = mmc->write_bl_len;
	cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
	cmd.cmdarg = 0;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.flags = 0;

	err = mmc_set_blocklen(mmc, mmc->write_bl_len);

	if (err) {
		printf("set write bl len failed\n\r");
		return err;
	}

#ifdef CONFIG_MMC_MULTBLK_RW_SUPPORT
	while (blk < blkcnt) {
		blk_r = ((blkcnt - blk) > BLKS_PER_WRITE) ? BLKS_PER_WRITE : (blkcnt - blk);
		err = mmc_write_t(mmc, start + blk,
			blk_r, src + blk * blklen);
		stoperr = mmc_send_cmd(mmc, &cmd, NULL);

		if (err || stoperr)
			break;

#if 0
		if (blk % (64 * BLKS_PER_WRITE))
			printf(".");
		else {
			if (blk)
				printf("\n.");
			printf("\t.");
		}
#endif
		blk += blk_r;
	}
#else
	for (blk = start; blk < start + blkcnt; blk++, src += blklen) {
		err = mmc_write_block(mmc, (void *) src, blk);

		if (err) {
			printf("block write failed: %d\n", err);
			return blk - start;
		}
	}
	return blk - start;
#endif

	return blk;
}

static ulong mmc_erase_t(struct mmc *mmc, ulong start, lbaint_t blkcnt)
{
	struct mmc_cmd cmd;
	int err;

	cmd.cmdidx = MMC_CMD_ERASE_GRP_START;
	cmd.cmdarg = start;
	cmd.resp_type = MMC_RSP_R1;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err)
		goto err_out;

	cmd.cmdidx = MMC_CMD_ERASE_GRP_END;
	cmd.cmdarg = start + blkcnt - 1;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err)
		goto err_out;

	cmd.cmdidx = MMC_CMD_ERASE;
	cmd.cmdarg = 0x80000000; //Secure erase
	cmd.resp_type = MMC_RSP_R1b;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err)
		goto err_out;

	return 0;

err_out:
	printf("mmc erase failed\n\r");
	return err;
}

static unsigned long mmc_berase(int dev_num, ulong start, lbaint_t blkcnt)
{
	int err = 0;
	struct mmc *mmc = find_mmc_device(dev_num);
	lbaint_t blk = 0, blk_r = 0;

	if (!mmc)
		return -1;

	while (blk < blkcnt) {
		blk_r = ((blkcnt - blk) > mmc->erase_grp_size) ?
			mmc->erase_grp_size : (blkcnt - blk);
		err = mmc_erase_t(mmc, start + blk, blk_r);
		if (err)
			break;

		if (blk % (64 * mmc->erase_grp_size))
			printf(".");
		else {
			if (blk)
				printf("\n");
			printf("\t.");
		}
		blk += blk_r;
	}

	return blk;
}

int mmc_read_block(struct mmc *mmc, void *dst, uint blocknum)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	cmd.cmdidx = MMC_CMD_READ_SINGLE_BLOCK;

	if (mmc->sect_mode_access)
		cmd.cmdarg = blocknum;
	else
		cmd.cmdarg = blocknum * mmc->read_bl_len;

	cmd.resp_type = MMC_RSP_R1;
	cmd.flags = 0;

	data.dest = dst;
	data.blocks = 1;
	data.blocksize = mmc->read_bl_len;
	data.flags = MMC_DATA_READ;

	return mmc_send_cmd(mmc, &cmd, &data);
}

int mmc_read(struct mmc *mmc, u64 src, uchar *dst, int size)
{
	char *buffer;
	int i;
	int blklen = mmc->read_bl_len;
	int startblock = lldiv(src, mmc->read_bl_len);
	int endblock = lldiv(src + size - 1, mmc->read_bl_len);
	int err = 0;

	/* Make a buffer big enough to hold all the blocks we might read */
	buffer = malloc(blklen);

	printf("@@@@@@@@buffer %x\n\n", (unsigned int) buffer);
	while(1);
	if (!buffer) {
		printf("Could not allocate buffer for MMC read!\n");
		return -1;
	}

	/* We always do full block reads from the card */
	err = mmc_set_blocklen(mmc, mmc->read_bl_len);

	if (err)
		return err;

	for (i = startblock; i <= endblock; i++) {
		int segment_size;
		int offset;

		err = mmc_read_block(mmc, buffer, i);

		if (err)
			goto free_buffer;

		/*
		 * The first block may not be aligned, so we
		 * copy from the desired point in the block
		 */
		offset = (src & (blklen - 1));
		segment_size = MIN(blklen - offset, size);

		memcpy(dst, buffer + offset, segment_size);

		dst += segment_size;
		src += segment_size;
		size -= segment_size;
	}

free_buffer:
	free(buffer);

	return err;
}

#ifdef CONFIG_MMC_MULTBLK_RW_SUPPORT
static ulong mmc_read_t(struct mmc *mmc, ulong start, lbaint_t blkcnt, void *dst)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int err;
	lbaint_t blklen = mmc->read_bl_len;

	if (blkcnt >= 1)
		cmd.cmdidx = MMC_CMD_READ_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_CMD_READ_SINGLE_BLOCK;

	if (mmc->sect_mode_access)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * blklen;

	cmd.resp_type = MMC_RSP_R1;
	cmd.flags = 0;

	data.dest = dst;
	data.blocks = blkcnt;
	data.blocksize = blklen;
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);

	if (err) {
		printf("mmc read failed\n\r");
		return err;
	}

	return 0;
}
#endif

static ulong mmc_bread(int dev_num, ulong start, lbaint_t blkcnt, void *dst)
{
	struct mmc_cmd cmd;
	int err;
	struct mmc *mmc = find_mmc_device(dev_num);
	lbaint_t blklen, blk = 0;
#ifdef CONFIG_MMC_MULTBLK_RW_SUPPORT
	int stoperr = 0, blk_r = 0;
#endif

	if (!mmc)
		return -1;

	blklen = mmc->read_bl_len;
	cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
	cmd.cmdarg = 0;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.flags = 0;

	err = mmc_set_blocklen(mmc, mmc->read_bl_len);

	if (err) {
		printf("set read bl len failed\n\r");
		return err;
	}

#ifdef CONFIG_MMC_MULTBLK_RW_SUPPORT
	while (blk < blkcnt) {
		blk_r = ((blkcnt - blk) > BLKS_PER_READ) ? BLKS_PER_READ : (blkcnt - blk);

		err = mmc_read_t(mmc, start + blk,
			blk_r, dst + blk * blklen);
		stoperr = mmc_send_cmd(mmc, &cmd, NULL);

		if (err || stoperr)
			break;

#if 0
		if (blk % (64 * BLKS_PER_READ))
			printf(".");
		else if (blk_r > 1) {
			if (blk)
				printf("\n");
			printf("\t.");
		}
#endif

		blk += blk_r;
	}
#else
	for (blk = start; blk < start + blkcnt; blk++, dst += blklen) {
		err = mmc_read_block(mmc, dst, blk);

		if (err) {
			printf("block read failed: %d\n", err);
			return blk - start;
		}
	}
	return blk - start;
#endif

	return blk;
}

int mmc_go_idle(struct mmc* mmc)
{
	struct mmc_cmd cmd;
	int err;

	udelay(1000);

	cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
	cmd.cmdarg = 0;
	cmd.resp_type = MMC_RSP_NONE;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	udelay(2000);

	return 0;
}

int
sd_send_op_cond(struct mmc *mmc)
{
	int timeout = 1000;
	int err;
	struct mmc_cmd cmd;

	do {
		cmd.cmdidx = MMC_CMD_APP_CMD;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = 0;
		cmd.flags = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;

		cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
		cmd.resp_type = MMC_RSP_R3;
		cmd.cmdarg = MMC_VDD_32_33 | MMC_VDD_33_34;

		if (mmc->version == SD_VERSION_2)
			cmd.cmdarg |= OCR_HCS;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;

		udelay(1000);
	} while ((!(cmd.response[0] & OCR_BUSY)) && timeout--);

	if (timeout <= 0)
		return UNUSABLE_ERR;

	if (mmc->version != SD_VERSION_2)
		mmc->version = SD_VERSION_1_0;

	mmc->ocr = cmd.response[0];

	mmc->sect_mode_access = ((mmc->ocr & OCR_HCS) == OCR_HCS);
	mmc->rca = 0;

	return 0;
}

int mmc_send_op_cond(struct mmc *mmc)
{
	int timeout = 1000;
	struct mmc_cmd cmd;
	int err;

	/* Some cards seem to need this */
	mmc_go_idle(mmc);

	do {
		cmd.cmdidx = MMC_CMD_SEND_OP_COND;
		cmd.resp_type = MMC_RSP_R3;
		cmd.cmdarg = OCR_HCS | mmc->voltages;
		cmd.flags = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;

		udelay(1000);
	} while (!(cmd.response[0] & OCR_BUSY) && timeout--);

	if (timeout <= 0)
		return UNUSABLE_ERR;

	mmc->version = MMC_VERSION_UNKNOWN;
	mmc->ocr = cmd.response[0];

	mmc->sect_mode_access = ((mmc->ocr & OCR_HCS) == OCR_HCS);
	mmc->rca = 0;

	return 0;
}


int mmc_send_ext_csd(struct mmc *mmc, char *ext_csd)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int err;

	/* Get the Card Status Register */
	cmd.cmdidx = MMC_CMD_SEND_EXT_CSD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;
	cmd.flags = 0;

	data.dest = ext_csd;
	data.blocks = 1;
	data.blocksize = 512;
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);

	return err;
}

/* for debugging */
static int mmc_send_status(struct mmc *mmc, u32 *status)
{
	struct mmc_cmd cmd;
	int err;

	cmd.cmdidx = MMC_CMD_SEND_STATUS;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = mmc->rca << 16;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return 1;

	*status = cmd.response[0];
	return 0;
}

int mmc_switch(struct mmc *mmc, u8 set, u8 index, u8 value)
{
	struct mmc_cmd cmd;
	int ret;

	cmd.cmdidx = MMC_CMD_SWITCH;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
		(index << 16) |
		(value << 8);
	cmd.flags = 0;

	ret = mmc_send_cmd(mmc, &cmd, NULL);

	return ret;
}

int mmc_change_freq(struct mmc *mmc)
{
	char ext_csd[512];
	char cardtype;
	int err;

	mmc->card_caps = 0;

	/* Only version 4 supports high-speed */
	if (mmc->version < MMC_VERSION_4)
		return 0;

	/* MMC 4.x (MMC Plus) Cards support both 4 Bit and 8 Bit
	 * bus-width based on Host Controller Capability
	 */
	mmc->card_caps |= (MMC_MODE_4BIT | MMC_MODE_8BIT);

	err = mmc_send_ext_csd(mmc, ext_csd);

	if (err)
		return err;

	if (ext_csd[212] || ext_csd[213] || ext_csd[214] || ext_csd[215]) {
		mmc->high_capacity = 1;
		mmc->sectors = ext_csd[212] | (ext_csd[213] << 8)
			| (ext_csd[214] << 16) | (ext_csd[215] << 24);
		mmc->capacity = mmc->sectors * 512;
	} else
		mmc->high_capacity = 0;

	cardtype = ext_csd[196] & 0xf;

	err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_HS_TIMING, 1);

	if (err)
		return err;

	/* Now check to see that it worked */
	err = mmc_send_ext_csd(mmc, ext_csd);

	if (err)
		return err;

	/* No high-speed support */
	if (!ext_csd[185])
		return 0;

	/* High Speed is set, there are two types: 52MHz and 26MHz */
	if (cardtype & MMC_HS_52MHZ)
		mmc->card_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS;
	else
		mmc->card_caps |= MMC_MODE_HS;

	return 0;
}

static int mmc_get_info(struct mmc *mmc)
{
	char ext_csd[512];
	int err;
	char erase_grp_def, erase_grp_size, erase_grp_mult;

	err = mmc_send_ext_csd(mmc, ext_csd);
	if (err)
		return err;

	mmc->ext_csd_part_support = ext_csd[160];
	mmc->ext_csd_part_config = ext_csd[179];
	mmc->ext_csd_boot_config_prot = ext_csd[178];
	mmc->ext_csd_boot_size = ext_csd[226] * 128 * 1024; // * 128 KB
	mmc->ext_csd_boot_bus_width = ext_csd[177];

	erase_grp_def = ext_csd[176];
	if (erase_grp_def) {
		mmc->erase_grp_size = ext_csd[224] * 512 * 1024;
	 } else {
		erase_grp_size = mmc->bcsd.erase_grp_size;
		erase_grp_mult = mmc->bcsd.erase_grp_mult;
		mmc->erase_grp_size = (erase_grp_size + 1) * (erase_grp_mult + 1);
	}

	mmc->ext_csd_erase_mem_content = ext_csd[181];

	return 0;
}

int sd_switch(struct mmc *mmc, int mode, int group, u8 value, u8 *resp)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	/* Switch the frequency */
	cmd.cmdidx = SD_CMD_SWITCH_FUNC;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = (mode << 31) | 0xffffff;
	cmd.cmdarg &= ~(0xf << (group * 4));
	cmd.cmdarg |= value << (group * 4);
	cmd.flags = 0;

	data.dest = (char *)resp;
	data.blocksize = 64;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	return mmc_send_cmd(mmc, &cmd, &data);
}


int sd_change_freq(struct mmc *mmc)
{
	int err;
	struct mmc_cmd cmd;
	uint scr[2];
	uint switch_status[16];
	struct mmc_data data;
	int timeout;

	mmc->card_caps = 0;

	/* Read the SCR to find out if this card supports higher speeds */
	cmd.cmdidx = MMC_CMD_APP_CMD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = mmc->rca << 16;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	cmd.cmdidx = SD_CMD_APP_SEND_SCR;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;
	cmd.flags = 0;

	timeout = 3;

retry_scr:
	data.dest = (char *)&scr;
	data.blocksize = 8;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);

	if (err) {
		if (timeout--)
			goto retry_scr;

		return err;
	}

	mmc->scr[0] = __be32_to_cpu(scr[0]);
	mmc->scr[1] = __be32_to_cpu(scr[1]);

	switch ((mmc->scr[0] >> 24) & 0xf) {
		case 0:
			mmc->version = SD_VERSION_1_0;
			break;
		case 1:
			mmc->version = SD_VERSION_1_10;
			break;
		case 2:
			mmc->version = SD_VERSION_2;
			break;
		default:
			mmc->version = SD_VERSION_1_0;
			break;
	}

	if (mmc->scr[0] & SD_DATA_4BIT)
		mmc->card_caps |= MMC_MODE_4BIT;

	/* Version 1.0 doesn't support switching */
	if (mmc->version == SD_VERSION_1_0)
		return 0;

	timeout = 4;
	while (timeout--) {
		err = sd_switch(mmc, SD_SWITCH_CHECK, 0, 1,
				(u8 *)&switch_status);

		if (err)
			return err;

		/* The high-speed function is busy.  Try again */
		if (!(__be32_to_cpu(switch_status[7]) & SD_HIGHSPEED_BUSY))
			break;
	}

	/* If high-speed isn't supported, we return */
	if (!(__be32_to_cpu(switch_status[3]) & SD_HIGHSPEED_SUPPORTED))
		return 0;

	err = sd_switch(mmc, SD_SWITCH_SWITCH, 0, 1, (u8 *)&switch_status);

	if (err)
		return err;

	if ((__be32_to_cpu(switch_status[4]) & 0x0f000000) == 0x01000000)
		mmc->card_caps |= MMC_MODE_HS;

	return 0;
}

/* frequency bases */
/* divided by 10 to be nice to platforms without floating point */
int fbase[] = {
	10000,
	100000,
	1000000,
	10000000,
};

/* Multiplier values for TRAN_SPEED.  Multiplied by 10 to be nice
 * to platforms without floating point.
 */
int multipliers[] = {
	0,	/* reserved */
	10,
	12,
	13,
	15,
	20,
	25,
	30,
	35,
	40,
	45,
	50,
	55,
	60,
	70,
	80,
};

void mmc_set_ios(struct mmc *mmc)
{
	mmc->set_ios(mmc);
}

void mmc_set_clock(struct mmc *mmc, uint clock)
{
	if (clock > mmc->f_max)
		clock = mmc->f_max;

	if (clock < mmc->f_min)
		clock = mmc->f_min;

	mmc->clock = clock;

	mmc_set_ios(mmc);
}

void mmc_set_bus_width(struct mmc *mmc, uint width)
{
	mmc->bus_width = width;

	mmc_set_ios(mmc);
}

#define UNSTUFF_BITS(resp,start,size)					\
	({								\
		const int __size = size;				\
		const u32 __mask = (__size < 32 ? 1 << __size : 0) - 1;	\
		const int __off = 3 - ((start) / 32);			\
		const int __shft = (start) & 31;			\
		u32 __res;						\
									\
		__res = resp[__off] >> __shft;				\
		if (__size + __shft > 32)				\
			__res |= resp[__off-1] << ((32 - __shft) % 32);	\
		__res & __mask;						\
	})

static int mmc_decode_cid(struct mmc *mmc)
{
	u32 *resp = &mmc->cid[0];

	/*
	 * The selection of the format here is based upon published
	 * specs from sandisk and from what people have reported.
	 */
	switch (mmc->bcsd.spec_vers) {
	case 0: /* MMC v1.0 - v1.2 */
	case 1: /* MMC v1.4 */
		mmc->bcid.manu_id = UNSTUFF_BITS(resp, 104, 24);
		mmc->bcid.prod_name[0]	= UNSTUFF_BITS(resp, 96, 8);
		mmc->bcid.prod_name[1]	= UNSTUFF_BITS(resp, 88, 8);
		mmc->bcid.prod_name[2]	= UNSTUFF_BITS(resp, 80, 8);
		mmc->bcid.prod_name[3]	= UNSTUFF_BITS(resp, 72, 8);
		mmc->bcid.prod_name[4]	= UNSTUFF_BITS(resp, 64, 8);
		mmc->bcid.prod_name[5]	= UNSTUFF_BITS(resp, 56, 8);
		mmc->bcid.prod_name[6]	= UNSTUFF_BITS(resp, 48, 8);
		mmc->bcid.prod_ver	= UNSTUFF_BITS(resp, 40, 4);
		mmc->bcid.prod_sn	= UNSTUFF_BITS(resp, 16, 24);
		mmc->bcid.month		= UNSTUFF_BITS(resp, 12, 4);
		mmc->bcid.year		= UNSTUFF_BITS(resp, 8, 4) + 1997;
		break;

	case 2: /* MMC v2.0 - v2.2 */
	case 3: /* MMC v3.1 - v3.3 */
	case 4: /* MMC v4 */
		mmc->bcid.manu_id	= UNSTUFF_BITS(resp, 120, 8);
		mmc->bcid.oem_id	= UNSTUFF_BITS(resp, 104, 16);
		mmc->bcid.prod_name[0]	= UNSTUFF_BITS(resp, 96, 8);
		mmc->bcid.prod_name[1]	= UNSTUFF_BITS(resp, 88, 8);
		mmc->bcid.prod_name[2]	= UNSTUFF_BITS(resp, 80, 8);
		mmc->bcid.prod_name[3]	= UNSTUFF_BITS(resp, 72, 8);
		mmc->bcid.prod_name[4]	= UNSTUFF_BITS(resp, 64, 8);
		mmc->bcid.prod_name[5]	= UNSTUFF_BITS(resp, 56, 8);
		mmc->bcid.prod_ver	= UNSTUFF_BITS(resp, 40, 4);
		mmc->bcid.prod_sn	= UNSTUFF_BITS(resp, 16, 32);
		mmc->bcid.month		= UNSTUFF_BITS(resp, 12, 4);
		mmc->bcid.year		= UNSTUFF_BITS(resp, 8, 4) + 1997;
		break;

	default:
		printf("card has unknown MMCA version %d\n",
			mmc->bcsd.spec_vers);
		return -1;
	}

	return 0;
}


/*
 * Given a 128-bit response, decode to our card CSD structure.
 */
static int mmc_decode_csd(struct mmc *mmc)
{
	struct mmc_csd *csd = &mmc->bcsd;
	u32 *resp = &mmc->csd[0];

	memset(csd, 0x0, sizeof(struct mmc_csd));
	csd->csd_structure = UNSTUFF_BITS(resp, 126, 2);
	csd->tran_speed = UNSTUFF_BITS(resp, 96, 8);
	csd->read_bl_len = UNSTUFF_BITS(resp, 80, 4);
	csd->write_bl_len = UNSTUFF_BITS(resp, 22, 4);

	if (IS_SD(mmc)) {
		unsigned int e, m;
		unsigned int eb_en, es_size;

		switch (csd->csd_structure) {
		case 0:
			e = UNSTUFF_BITS(resp, 47, 3);
			m = UNSTUFF_BITS(resp, 62, 12);
			mmc->capacity = (1 + m) << (e + 2);
			mmc->capacity *= (1 << csd->read_bl_len);
			eb_en = UNSTUFF_BITS(resp, 45, 1);
			es_size = UNSTUFF_BITS(resp, 39, 7);
			if (eb_en)
				mmc->erase_grp_size = 1;
			else
				mmc->erase_grp_size = es_size;
			break;
		case 1:
			m = UNSTUFF_BITS(resp, 48, 22);
			mmc->capacity = (1 + m);
			mmc->capacity  = mmc->capacity << 19;
			eb_en = UNSTUFF_BITS(resp, 45, 1);
			mmc->erase_grp_size = 1;
			if (!eb_en)
				printf("Warning: erase_grp_size %d may wrong\n", mmc->erase_grp_size);
			break;
		default:
			printf("unrecognised SD CSD structure version %d\n",
				csd->csd_structure);
			return -1;
		}
		mmc->sectors = mmc->capacity / 512;
	} else {
		/*
		 * We only understand MMC CSD structure v1.1 and v1.2.
		 * v1.2 has extra information in bits 15, 11 and 10.
		 */
		if (csd->csd_structure != 1 && csd->csd_structure != 2) {
			printf("unrecognised MMC CSD structure version %d\n",
				csd->csd_structure);
			return -1;
		}

		csd->spec_vers = UNSTUFF_BITS(resp, 122, 4);
		csd->c_size = UNSTUFF_BITS(resp, 62, 12);
		csd->c_size_mult = UNSTUFF_BITS(resp, 47, 3);
		csd->erase_grp_size = UNSTUFF_BITS(resp, 42, 5);
		csd->erase_grp_mult = UNSTUFF_BITS(resp, 37, 5);
	}

	return 0;
}

int mmc_startup(struct mmc *mmc)
{
	int err;
	uint mult, freq;
	u64 cmult, csize;
	struct mmc_cmd cmd;

	/* Put the Card in Identify Mode */
	cmd.cmdidx = MMC_CMD_ALL_SEND_CID;
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = 0;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	memcpy(mmc->cid, cmd.response, 16);

	/*
	 * For MMC cards, set the Relative Address.
	 * For SD cards, get the Relatvie Address.
	 * This also puts the cards into Standby State
	 */
	cmd.cmdidx = SD_CMD_SEND_RELATIVE_ADDR;
	cmd.cmdarg = mmc->rca << 16;
	cmd.resp_type = MMC_RSP_R6;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	if (IS_SD(mmc))
		mmc->rca = (cmd.response[0] >> 16) & 0xffff;

	/* Get the Card-Specific Data */
	cmd.cmdidx = MMC_CMD_SEND_CSD;
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = mmc->rca << 16;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	mmc->csd[0] = cmd.response[0];
	mmc->csd[1] = cmd.response[1];
	mmc->csd[2] = cmd.response[2];
	mmc->csd[3] = cmd.response[3];
	mmc_decode_csd(mmc);
	mmc_decode_cid(mmc);

	if (mmc->version == MMC_VERSION_UNKNOWN) {
		switch (mmc->bcsd.spec_vers) {
			case 0:
				mmc->version = MMC_VERSION_1_2;
				break;
			case 1:
				mmc->version = MMC_VERSION_1_4;
				break;
			case 2:
				mmc->version = MMC_VERSION_2_2;
				break;
			case 3:
				mmc->version = MMC_VERSION_3;
				break;
			case 4:
				mmc->version = MMC_VERSION_4;
				break;
			default:
				mmc->version = MMC_VERSION_1_2;
				break;
		}
	}

	/* divide frequency by 10, since the mults are 10x bigger */
	freq = fbase[mmc->bcsd.tran_speed & 0x7];
	mult = multipliers[(mmc->bcsd.tran_speed >> 3) & 0xf];
	mmc->tran_speed = freq * mult;

	mmc->max_read_bl_len = 1 << mmc->bcsd.read_bl_len;
	mmc->read_bl_len = 1 << mmc->bcsd.read_bl_len;

	if (IS_SD(mmc))
		mmc->write_bl_len = mmc->read_bl_len;
	else
		mmc->write_bl_len = 1 << (mmc->bcsd.write_bl_len & 0xf);

	if (mmc->read_bl_len > 512)
		mmc->read_bl_len = 512;

	if (mmc->write_bl_len > 512)
		mmc->write_bl_len = 512;

	/* Select the card, and put it into Transfer Mode */
	cmd.cmdidx = MMC_CMD_SELECT_CARD;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = mmc->rca << 16;
	cmd.flags = 0;
	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	if (IS_SD(mmc))
		err = sd_change_freq(mmc);
	else {
		err = mmc_change_freq(mmc);
		if (!mmc->high_capacity) {
			csize = mmc->bcsd.c_size;
			cmult = mmc->bcsd.c_size_mult;

			mmc->sectors =  (csize + 1) << (cmult + 2)
				* (mmc->read_bl_len / 512);
			mmc->capacity = (csize + 1) << (cmult + 2);
			mmc->capacity *= (1 << mmc->bcsd.read_bl_len);
		}
	}

	if (err)
		return err;

	/* Restrict card's capabilities by what the host can do */
	mmc->card_caps &= mmc->host_caps;

	if (IS_SD(mmc)) {
		if (mmc->card_caps & MMC_MODE_4BIT) {
			cmd.cmdidx = MMC_CMD_APP_CMD;
			cmd.resp_type = MMC_RSP_R1;
			cmd.cmdarg = mmc->rca << 16;
			cmd.flags = 0;

			err = mmc_send_cmd(mmc, &cmd, NULL);
			if (err)
				return err;

			cmd.cmdidx = SD_CMD_APP_SET_BUS_WIDTH;
			cmd.resp_type = MMC_RSP_R1;
			cmd.cmdarg = 2;
			cmd.flags = 0;
			err = mmc_send_cmd(mmc, &cmd, NULL);
			if (err)
				return err;

			mmc_set_bus_width(mmc, 4);
		}

		if (mmc->card_caps & MMC_MODE_HS)
			mmc_set_clock(mmc, 50000000);
		else
			mmc_set_clock(mmc, 25000000);
	} else {
		if (mmc->card_caps & MMC_MODE_8BIT) {
			/* Set the card to use 8 bit*/
			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
					EXT_CSD_BUS_WIDTH,
					EXT_CSD_BUS_WIDTH_8);

			if (err)
				return err;

			mmc_set_bus_width(mmc, 8);
		} else if (mmc->card_caps & MMC_MODE_4BIT) {
			/* Set the card to use 4 bit*/
			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
					EXT_CSD_BUS_WIDTH,
					EXT_CSD_BUS_WIDTH_4);

			if (err)
				return err;

			mmc_set_bus_width(mmc, 4);
		} else if (mmc->card_caps & MMC_MODE_1BIT) {
			/* Set the card to use 1 bit*/
			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
					EXT_CSD_BUS_WIDTH,
					EXT_CSD_BUS_WIDTH_1);

			if (err)
				return err;

			mmc_set_bus_width(mmc, 1);
		}

		if (mmc->card_caps & MMC_MODE_HS) {
			if (mmc->card_caps & MMC_MODE_HS_52MHz)
				mmc_set_clock(mmc, 52000000);
			else
				mmc_set_clock(mmc, 26000000);
		} else
			mmc_set_clock(mmc, 20000000);

#ifdef CONFIG_MMC_BUSTEST_SUPPORT
		if (mmc->card_caps & MMC_MODE_8BIT) {
			err = mmc_bus_test(mmc, 8);
			if (err)
				return err;
		} else if (mmc->card_caps & MMC_MODE_4BIT) {
			err = mmc_bus_test(mmc, 4);
			if (err)
				return err;
		}
#endif

		err = mmc_get_info(mmc);
		if (err)
			return err;
	}

	/* fill in device description */
	mmc->block_dev.lun = 0;
	mmc->block_dev.type = 0;
	mmc->block_dev.blksz = mmc->read_bl_len;
	//lldiv seems to has problem
	//mmc->block_dev.lba = lldiv(mmc->capacity, mmc->read_bl_len);
	mmc->block_dev.lba = (mmc->sectors / (mmc->read_bl_len / 512));
	sprintf(mmc->block_dev.vendor, "Man %06x Snr %08x", mmc->bcid.manu_id,
			mmc->bcid.prod_sn);
	sprintf(mmc->block_dev.product, "%s", mmc->bcid.prod_name);
	sprintf(mmc->block_dev.revision, "%d.%d", (mmc->bcid.prod_ver >> 4) & 0xf,
			mmc->bcid.prod_ver & 0xf);
	init_part(&mmc->block_dev);

	return 0;
}

int mmc_send_if_cond(struct mmc *mmc)
{
	struct mmc_cmd cmd;
	int err;

	cmd.cmdidx = SD_CMD_SEND_IF_COND;
	/* We set the bit if the host supports voltages between 2.7 and 3.6 V */
	cmd.cmdarg = ((mmc->voltages & 0xff8000) != 0) << 8 | 0xaa;
	cmd.resp_type = MMC_RSP_R7;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	if ((cmd.response[0] & 0xff) != 0xaa)
		return UNUSABLE_ERR;
	else
		mmc->version = SD_VERSION_2;

	return 0;
}

static unsigned long mmc_switch_part(int dev_num, unsigned int part_num)
{
	int err;
	struct mmc *mmc = find_mmc_device(dev_num);

	if (!mmc)
		return -1;

	err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_PART_CONF,
		(mmc->ext_csd_part_config & ~0x7) | (part_num & 0x7));
	if (err)
		return err;

	err = mmc_get_info(mmc);
	if (err)
		return err;

	return 0;
}

static unsigned long mmc_switch_boot(int dev_num, unsigned int part_num)
{
	int err, boot_sel;
	struct mmc *mmc = find_mmc_device(dev_num);

	if (!mmc)
		return -1;

	part_num &= 0x7;
	boot_sel = (part_num ? part_num : 0x7) << 3;
	err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_PART_CONF,
		(mmc->ext_csd_part_config & ~0x38) | boot_sel);
	if (err)
		return err;

	err = mmc_get_info(mmc);
	if (err)
		return err;

	return 0;
}


int mmc_register(struct mmc *mmc)
{
	/* Setup the universal parts of the block interface just once */
	mmc->block_dev.if_type = IF_TYPE_MMC;
	mmc->block_dev.dev = cur_dev_num++;
	mmc->block_dev.removable = 1;
	mmc->block_dev.block_read = mmc_bread;
	mmc->block_dev.block_write = mmc_bwrite;
	mmc->block_dev.block_erase = mmc_berase;
	mmc->block_dev.switch_part = mmc_switch_part;
	mmc->block_dev.switch_boot = mmc_switch_boot;

	INIT_LIST_HEAD (&mmc->link);

	list_add_tail (&mmc->link, &mmc_devices);

	return 0;
}

block_dev_desc_t *mmc_get_dev(int dev)
{
	struct mmc *mmc = find_mmc_device(dev);

	return mmc ? &mmc->block_dev : NULL;
}

int mmc_init(struct mmc *mmc)
{
	int err, loop=10;

	err = mmc->init(mmc);

	if (err)
		return err;

	mmc_set_bus_width(mmc, 1);
	mmc_set_clock(mmc, 1);

	/* Reset the Card */
	while(loop--)
		err = mmc_go_idle(mmc);

	if (err)
		return err;

	/* Test for SD version 2 */
	err = mmc_send_if_cond(mmc);

	/* Now try to get the SD card's operating condition */
	err = sd_send_op_cond(mmc);

	/* If the command timed out, we check for an MMC card */
	if (err == TIMEOUT) {
		err = mmc_send_op_cond(mmc);

		if (err) {
			printf("Card did not respond to voltage select!\n");
			return UNUSABLE_ERR;
		}
	}

	err = mmc_startup(mmc);
	if (err)
		printf("Error: failed to init mmc\n");

	return err;
}

/*
 * CPU and board-specific MMC initializations.  Aliased function
 * signals caller to move on
 */
static int __def_mmc_init(bd_t *bis)
{
	return -1;
}

int cpu_mmc_init(bd_t *bis) __attribute__((weak, alias("__def_mmc_init")));
int board_mmc_init(bd_t *bis) __attribute__((weak, alias("__def_mmc_init")));

void print_mmc_devices(char separator)
{
	struct mmc *m;
	struct list_head *entry;

	list_for_each(entry, &mmc_devices) {
		m = list_entry(entry, struct mmc, link);

		printf("%s: %d ", m->name, m->block_dev.dev);

		if (entry->next != &mmc_devices)
			printf("%c", separator);
	}

	printf("\n");
}

int mmc_initialize(bd_t *bis)
{
	INIT_LIST_HEAD (&mmc_devices);
	cur_dev_num = 0;

	if (board_mmc_init(bis) < 0)
		cpu_mmc_init(bis);

	print_mmc_devices(',');

	return 0;
}

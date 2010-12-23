/*
#include <command.h>
#include <asm/arch-pxa168/common.h>
#include <asm/arch-pxa168/pxa3xx_bbm.h>
#include <linux/mtd/mtd.h>
#include <malloc.h>

extern int nand_curr_device;
extern struct mtd_info nand_info[];
extern struct mtd_info onenand_mtd;

int do_bbt (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	struct pxa3xx_bbm *bbm;
	struct mtd_info *mtd;
	struct reloc_table *table;
	struct reloc_item *item;
	struct erase_info instr = {
		.callback	= NULL,
	};
	int ntim_s, reloc_s, pages, ofs, block;
	int is_nand = 0, ret, maxsize;
	size_t retlen;
	ulong addr;
	void *buf;

	switch (argc) {
	case 0:
	case 1:
	case 2:
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	case 3:
	case 4:
		if (strcmp(argv[2], "nand") == 0) {

			is_nand = 1;
			mtd = &nand_info[nand_curr_device];
			if (nand_curr_device < 0) {
				printf ("There is no nand device on current CS\n");
				return 0;
			}
		}
#ifdef CONFIG_CMD_ONENAND
		else if (strcmp(argv[2], "onenand") == 0) {
			if (onenand_mtd.size == 0) {
				printf ("There is no onenand device\n");
				return 0;
			}

			mtd = &onenand_mtd;
		}
#endif
		else {
			printf ("Usage:\n%s\n", cmdtp->usage);
			return 1;
		}

		bbm = mtd->bbm;

		if (strcmp (argv[1], "show") == 0)
			bbm->show(mtd);
		else if (strcmp(argv[1], "init") == 0) {
			buf = malloc(mtd->erasesize);
			if (!buf) {
				printf("Malloc fail, exit\n");
				return 0;
			}
			instr.addr = 0;
			instr.len = mtd->erasesize;

			if (is_nand)
				printf("NAND relocation table init... ");
			else
				printf("ONENAND relocation table init... ");

			printf("waiting... ");
			mtd->read(mtd, 0, mtd->erasesize, &retlen, buf);
			ret = mtd->erase(mtd, &instr);
			if (ret) {
				printf("erase block 0 failed\n");
				return 0;
			}

			item	= bbm->reloc;
			table	= bbm->table;
			pages	= bbm->max_slots;
			ntim_s	= mtd->writesize * PXA_BEGIN_SLOT;
			reloc_s	= mtd->writesize * (pages - 1);
			maxsize = mtd->size - (bbm->max_reloc_entry << bbm->erase_shift);

			memset(bbm->data_buf, 0xff, mtd->writesize);
			bbm->current_slot = bbm->max_slots - 1;
			table->header	  = PXA_RELOC_HEADER;
			table->total	  = 0;
			bbm->table_init	  = 0;
			
			for (ofs = 0; ofs < maxsize; ofs += mtd->erasesize)
				if (mtd->block_isbad(mtd, ofs))
					update_reloc_tb(mtd, ofs >> bbm->erase_shift);

			bbm->table_init	  = 1;
			memset(buf + ntim_s, 0xff, mtd->erasesize - ntim_s);
			memcpy(buf + reloc_s, bbm->data_buf, mtd->writesize);
			
			mtd->write(mtd, 0, mtd->erasesize, &retlen, buf);
			printf("init done!!!\n\n");

			free(buf);
		}
		else if (strcmp(argv[1], "mark") == 0) {
			addr = (ulong)simple_strtoul(argv[3], NULL, 16);
			block = addr >> bbm->erase_shift;

			ret = bbm->markbad(mtd, addr);
			if (ret == 0) {
				printf("block %d successfully marked as bad\n",
						block);
				return 0;
			} else {
				printf("block %d NOT marked as bad! ERROR %d\n",
						block, ret);
			}
		}
		else {
			printf ("Usage:\n%s\n", cmdtp->usage);
			return 1;
		}
}

	return 1;
}

U_BOOT_CMD(
	bbt,	4,	1,	do_bbt,
	"bbt\t- Bad Block Management\n",
	"show  [nand | onenand]\t- show nand/onenand relocation table\n"
	"bbt init  [nand | onenand]\t- init relocation table\n"
	"bbt mark  [nand | onenand] addr\t- mark bad block\n"
);
*/

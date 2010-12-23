#ifndef	__PXA3XX_BBT_H__
#define	__PXA3XX_BBT_H__

#define PXA_RELOC_HEADER	0x524e
#define	PXA_MAX_RELOC_ENTRY	100
#define PXA_BEGIN_SLOT		2

struct reloc_item {
	unsigned short from;
	unsigned short to;
};

struct reloc_table {
	unsigned short header;
	unsigned short total;
};

struct pxa3xx_bbm {
	/* NOTES: this field impact the partition table. Please make sure
	 * that this value align with partitions definition.
	 */
	int			reloc_begin;
	int			max_reloc_entry;
	int			max_slots;
	int			current_slot;

	void			*data_buf;

	/* These two fields should be in (one)nand_chip.
	 * Add here to handle onenand_chip and nand_chip
	 * at the same time.
	 */
	int			page_shift;
	int			erase_shift;

	unsigned int		table_init;
	struct reloc_table	*table;
	struct reloc_item	*reloc;

	void	(*uninit)(struct mtd_info *mtd);
	loff_t	(*search)(struct mtd_info *mtd,	loff_t ofs);
	int	(*markbad)(struct mtd_info *mtd, loff_t ofs);
	int	(*scan_bbt)(struct mtd_info *mtd);
	void	(*show)(struct mtd_info *mtd);
};

struct pxa3xx_bbm* alloc_pxa3xx_bbm(void);
#endif


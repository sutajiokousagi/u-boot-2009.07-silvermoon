#include <nand.h>
#include <linux/types.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <asm/arch/common.h>
#include <asm/errno.h>
#ifdef CONFIG_CPU_MONAHANS
#include <asm/arch/pxa-regs.h>
#else
#include <asm/arch/regs-pxa168.h>
#endif

#undef	__KERNEL__
#include <asm/arch-pxa168/pxa3xx_bbm.h>
#include <asm/arch-pxa168/pxa3xx_nand.h>
#include <asm/arch-pxa168/nand_supported.h>
#include <../drivers/mtd/nand/pxa3xx_nand.c>

static int nand_chip_num;
int nand_curr_device = -1;
nand_info_t nand_info[CONFIG_SYS_MAX_NAND_DEVICE];
static u64 pxa3xx_nand_dma_mask = DMA_BIT_MASK(32);
static struct resource pxa168_resources_nand[] = {
	[0] = {
		.start  = CONFIG_SYS_NAND_BASE,
		.end    = CONFIG_SYS_NAND_BASE + 0x200,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = IRQ_PXA168_NAND,
		.end    = IRQ_PXA168_NAND,
		.flags  = IORESOURCE_IRQ,
	},
	[2] = {
		/* DRCMR for Data DMA */
		.start  = 97,
		.end    = 97,
		.flags  = IORESOURCE_DMA,
	},
	[3] = {
		/* DRCMR for Command DMA */
		.start  = 99,
		.end    = 99,
		.flags  = IORESOURCE_DMA,
	},
};

static struct mtd_partition	pxa_mass_partition[] = {
	[0] = {
		.name		= "MassStorage",
		.offset		= 0x0,
		.mask_flags	= MTD_WRITEABLE,
	},
};

static struct pxa3xx_nand_platform_data pxa_nandinfo;

static struct platform_device pxa168_device_nand = {
	.name           = "pxa3xx-nand",
	.id             = -1,
	.dev            = {
		.dma_mask = &pxa3xx_nand_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	},
	.num_resources  = ARRAY_SIZE(pxa168_resources_nand),
	.resource = pxa168_resources_nand,
};


static int nand_polling(struct pxa3xx_nand* nand, unsigned long timeout)
{
	unsigned int i, ret;

	for (i = 0; i < timeout; i ++) {
		pxa3xx_nand_irq(0, nand);
	
		if(nand->cmd_complete.done == 1)
			break;

		udelay(10);
	}
	
	ret = (i == timeout) ? 0 : 1;
	return ret;
}

unsigned long
wait_for_completion_timeout(struct completion *x, unsigned long timeout)
{
	struct pxa3xx_nand *nand;
	nand = container_of(x, struct pxa3xx_nand, cmd_complete);
	return nand_polling(nand, timeout);
}

void nand_init()
{
	struct pxa3xx_nand *nand;
	int chip;

	nand_chip_num = 0;
	pxa_nandinfo.enable_arbiter 	= 1;
	pxa_nandinfo.use_dma 		= 0; /* we only use polling in UBOOT */
	pxa_nandinfo.RD_CNT_DEL		= 0;

	pxa168_device_nand.dev.platform_data = &pxa_nandinfo;
	pxa3xx_nand_probe(&pxa168_device_nand);

	nand = platform_get_drvdata(&pxa168_device_nand);
	for (chip = 0; chip < CONFIG_SYS_MAX_NAND_DEVICE; chip ++) {
		if (nand->mtd[chip]) {
			memcpy(&(nand_info[chip]), nand->mtd[chip], sizeof(struct mtd_info));

			if (nand_curr_device < 0)
				nand_curr_device = chip;
		}
	}

	if (nand_curr_device < 0)
		printf("No NAND dev is found !!!\n\n");
}

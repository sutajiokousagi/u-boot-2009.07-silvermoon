/*
 * (C) Copyright 2002
 * Kyle Harris, Nexus Technologies, Inc. kharris@nexus-tech.net
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
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
#include <asm/arch/pxa-regs.h>

DECLARE_GLOBAL_DATA_PTR;

/**********************************************************************
 * set gpio for nand flash
 */

static void DfcSetGPIO()
{
	*(unsigned long *)0x40E10290=0x00001841;
	*(unsigned long *)0x40E1028C=0x00001841;
	*(unsigned long *)0x40E10288=0x00001841;
	*(unsigned long *)0x40E10284=0x00001841;
	*(unsigned long *)0x40E10280=0x00001841;
	*(unsigned long *)0x40E1027C=0x00001841;
	*(unsigned long *)0x40E10278=0x00001841;
	*(unsigned long *)0x40E10274=0x00001841;
	*(unsigned long *)0x40E10270=0x00001801;
	*(unsigned long *)0x40E1026C=0x00001801;
	*(unsigned long *)0x40E10268=0x00001801;
	*(unsigned long *)0x40E10264=0x00001801;
	*(unsigned long *)0x40E10248=0x00001801;
	*(unsigned long *)0x40E10244=0x00001801;
	*(unsigned long *)0x40E10240=0x00001801;
	*(unsigned long *)0x40E1023C=0x00001801;

	/* nCS0 */
	*(unsigned long *)0x40E1022C=0x00001901;
	/* nCS1 */
	*(unsigned long *)0x40E10230=0x00001903;
	/* nWE */
	*(unsigned long *)0x40E10234=0x00001901;
	/* nRE OE */
	*(unsigned long *)0x40E10238=0x00001901;
	/* nCLE */
	*(unsigned long *)0x40E1020C=0x00001901;
	/* nADV1_ALE */
	*(unsigned long *)0x40E10218=0x00001901;

	/* nADV2_ALE */
	/* Onenand is on nCS2. nADV2 should be configured as nCS2 */
	*(unsigned long *)0x40E10224=0x00001903;
	/* INT_RnB */
	*(unsigned long *)0x40E10228=0x00001901;
	/* DF_SCLK_E */
	*(unsigned long *)0x40E10214=0x00001801;
	/* nLUA */
	*(unsigned long *)0x40E10254=0x00001901;
	/* nLLA */
	*(unsigned long *)0x40E10258=0x00001901;
	/* nBE0 */
	*(unsigned long *)0x40E1021C=0x00001801;
	/* nBE1 */
	*(unsigned long *)0x40E10220=0x00001801;

	/* RDY */
	*(unsigned long *)0x40e10200=0x00001801;
}

/*
 * Miscelaneous platform dependent initialisations
 */

int board_init (void)
{
	/* memory and cpu-speed are setup before relocation */
	/* so we do _nothing_ here */

	/* arch number of tavor evb 3 */
#if defined (CONFIG_TAVOR_PV2)
	gd->bd->bi_arch_number = 2137;
#elif defined (CONFIG_TAVOR_PV)
	gd->bd->bi_arch_number = 1828;
#else
#error "Should specify PV2 or PV"
#endif

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0;

	/* initialize SMC */
	MSC0 = 0x72297229;
	MSC1 = 0x002c0000 | (MSC1 & 0xffff);
	MECR = 0x00000002;
	SXCNFG = 0x00040004;
	CSADRCFG0 = 0x00320919;
	CSADRCFG1 = 0x00320919;
	CSADRCFG2 = 0x00320919;
	CSADRCFG3 = 0x0012c80b;
	OSCC = 0xE502E9E5;

	DfcSetGPIO();

	return 0;
}

#ifdef CONFIG_PXAMCI
int board_mmc_init(bd_t *bd)
{
	return pxamci_init(bd);
}
#endif

int board_late_init(void)
{
	setenv("stdout", "serial");
	setenv("stderr", "serial");
	return 0;
}


int dram_init (void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

	return 0;
}

int checkboard (void)
{
	return display_marvell_banner();
}

int display_marvell_banner (void)
{
   printf("\n");
   printf(" __  __                      _ _\n");
   printf("|  \\/  | __ _ _ ____   _____| | |\n");
   printf("| |\\/| |/ _` | '__\\ \\ / / _ \\ | |\n");
   printf("| |  | | (_| | |   \\ V /  __/ | |\n");
   printf("|_|  |_|\\__,_|_|    \\_/ \\___|_|_|\n");
   printf(" _   _     ____              _\n");
   printf("| | | |   | __ )  ___   ___ | |_ \n");
   printf("| | | |___|  _ \\ / _ \\ / _ \\| __| \n");
   printf("| |_| |___| |_) | (_) | (_) | |_ \n");
   printf(" \\___/    |____/ \\___/ \\___/ \\__| ");
   printf("\n\nMARVELL PXAXXX series.");
   printf("\nBased on 88SV331xV5 Core with ARM926 LE CPU."); 
   printf("\nCode original base is u-boot-arm 2009.01-rc1.\n\n");

   return 0;
}

void lowlevel_init()
{
}

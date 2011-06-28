/*
 * (C) Copyright 2002
 * Rich Ireland, Enterasys Networks, rireland@enterasys.com.
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

#ifndef _SPARTAN6_H_
#define _SPARTAN6_H_

#include <xilinx.h>

extern int Spartan6_load( Xilinx_desc *desc, void *image, size_t size );
extern int Spartan6_dump( Xilinx_desc *desc, void *buf, size_t bsize );
extern int Spartan6_info( Xilinx_desc *desc );
extern int Spartan6_reloc( Xilinx_desc *desc, ulong reloc_off );

/* Slave Parallel Implementation function table */
typedef struct {
	Xilinx_pre_fn	pre;
	Xilinx_pgm_fn	pgm;
	Xilinx_init_fn	init;
	Xilinx_err_fn	err;
	Xilinx_done_fn	done;
	Xilinx_clk_fn	clk;
	Xilinx_cs_fn	cs;
	Xilinx_wr_fn	wr;
	Xilinx_rdata_fn	rdata;
	Xilinx_wdata_fn	wdata;
	Xilinx_busy_fn	busy;
	Xilinx_abort_fn	abort;
	Xilinx_post_fn	post;
	int		relocated;
} Xilinx_Spartan6_Slave_Parallel_fns;

/* Slave Serial Implementation function table */
typedef struct {
	Xilinx_pre_fn	pre;
	Xilinx_pgm_fn	pgm;
	Xilinx_clk_fn	clk;
	Xilinx_init_fn	init;
	Xilinx_done_fn	done;
	Xilinx_wr_fn	wr;
	Xilinx_post_fn	post;
	int		relocated;
} Xilinx_Spartan6_Slave_Serial_fns;

/* Device Image Sizes
 *********************************************************************/
/* Spartan-6 */
#define XILINX_XC6SLX9_SIZE	340604

/* Descriptor Macros
 *********************************************************************/
/* Spartan-6 devices */
#define XILINX_XC6SLX9_DESC(iface, fn_table, cookie) \
{ Xilinx_Spartan6, iface, XILINX_XC6SLX9_SIZE, fn_table, cookie }

#endif /* _SPARTAN3_H_ */

/*
 * Low-level frequency change code
 *
 * Copyright (C) 2009, Marvell Semicondutor.
 *
 * This software program is licensed subject to the GNU General Public License
 * (GPL).Version 2,June 1991, available at http://www.fsf.org/copyleft/gpl.html
 */

	.global freq_init_sram, freq_chg_seq

@******************************************************************************
@
@ freq_init_sram
@
@ Copy frequency change code into ISRAM
@
@ Inputs:
@	r0 = Start address of relocated program
@
@ Outputs:
@	None
@

freq_init_sram:
	stmfd	sp!, {r0 - r12, lr}
	ldr	r3, =freq_sram_start
	ldr	r4, =freq_sram_end
	add	r4, r4, #0x20

rel_ram:
	ldmia	r3!, {r5 - r12}
	stmia	r0!, {r5 - r12}
	cmp	r3, r4
	ble	rel_ram

	ldmfd	sp!, {r0 - r12, pc}

@******************************************************************************
@
@ freq_chg_seq
@
@ frequency change sequence
@
@ Inputs:
@	r0 = Start address of relocated program
@	r1 = Start address of relocated stack
@	r2 = operating points
@
@ Outputs:
@	None
@

#define PMUM_FCCR_OFF	0x0008
#define PMUA_CC_AP_OFF	0x0004

freq_chg_seq:

	@ save registers on stack
	stmfd	sp!, {r3 - r12, lr}

	mov	r12, r0			@ save start address of program in r12
	mov	r11, r1			@ save start address of stack in r11
	mov	r10, sp			@ save sp in r10
	mov	sp, r11			@ set up new stack address
	stmfd	sp!, {lr}		@ store the return address

	ldr	r4, =0xb0000000		@ DMEM base address
	ldr	r5, =0xd4050000		@ PMUM base address
	ldr	r6, =0xd4282800		@ PMUA base address

	mov	pc, r0

freq_sram_start:
	b	1f
	.align  5
1:
	@ frequency change sequence
	ldr	r7, =0x2000088e
	str	r7, [r5, #PMUM_FCCR_OFF]

	ldr	r7, =0xf0e482f8
	str	r7, [r6, #PMUA_CC_AP_OFF]

	@ ddr caliberation

	ldmfd	sp!, {lr}
	mov	sp, r10				@ restore stack address

        ldmfd   sp!, {r3 - r12, pc}

freq_sram_end:
	nop

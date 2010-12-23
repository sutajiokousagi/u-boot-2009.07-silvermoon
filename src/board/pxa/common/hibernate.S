/*
 * Low-level frequency change code
 *
 * Copyright (C) 2009, Marvell Semicondutor.
 *
 * This software program is licensed subject to the GNU General Public License
 * (GPL).Version 2,June 1991, available at http://www.fsf.org/copyleft/gpl.html
 */

	.global hibernate_init_sram, hibernate_seq

	.macro	delay, counter
	mov	\counter, #0x800000	@ Initialize counter
1:	subs	\counter, \counter, #1	@ Decrement counter
	bne	1b			@ No, then loop again
	.endm

	.text
@******************************************************************************
@
@ hibernate_init_sram
@
@ Copy frequency change code into ISRAM
@
@ Inputs:
@	r0 = Start address of relocated program
@
@ Outputs:
@	None
@

hibernate_init_sram:
	stmfd	sp!, {r0 - r12, lr}
	ldr	r3, =hibernate_sram_start
	ldr	r4, =hibernate_sram_end
	add	r4, r4, #0x20

hibernate_rel_ram:
	ldmia	r3!, {r5 - r12}
	stmia	r0!, {r5 - r12}
	cmp	r3, r4
	ble	hibernate_rel_ram

	ldmfd	sp!, {r0 - r12, pc}

@******************************************************************************
@
@ hibernate_seq
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

hibernate_seq:

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

hibernate_sram_start:
	b	1f
	.align  5
1:
	@ return address
	mov	r7, #0xf00000
	mov	r8, #0
	str	r7, [r8, #0x0]

	@ marker
	mov	r7, #10
	mov	r8, #0
	str	r7, [r8, #0x8]

	@ block ddr data request
	mov	r7, #1
	str	r7, [r4, #0x07e0]

	@ ddr self refresh
	ldr	r7, [r4, #0x0120]
	orr	r7, #0x40
	str	r7, [r4, #0x0120]

	@ ddr self refresh
	@mov	r7, #0
	@str	r7, [r6, #0x00c0]
	@mov	r7, #1
	@str	r7, [r6, #0x00b4]

	b	hibernate_pwr_down

	ldmfd	sp!, {lr}
	mov	sp, r10				@ restore stack address

        ldmfd   sp!, {r3 - r12, pc}

@*****************************************************************************
@
@
@ Pull EXP2_SYS_DIS_N low
@
@ Inputs:
@	nONE
@
@ Outputs:
@	None
@
hibernate_pwr_down:
	bl	init_i2c
	mov	r4, #0x20
	mov	r5, #0x06
	bl	readi2c
	mov	r4, #0x20
	mov	r5, #0x06
	and	r6, r0, #0xfd	/* set IO[1] output */
	bl	writei2c
	mov	r4, #0x20
	mov	r5, #0x02
	bl	readi2c
	mov	r4, #0x20
	mov	r5, #0x02
	orr	r6, r0, #0x2	/* set IO[1] high */
	bl	writei2c
	delay	r0
	mov	r4, #0x20
	mov	r5, #0x02
	bl	readi2c
	mov	r4, #0x20
	mov	r5, #0x02
	and	r6, r0, #0xfd	/* set IO[1] low */
	bl	writei2c
__loop:
	nop
	nop
	b	__loop

@*****************************************************************************
@
@
@ Init I2C
@
@ Inputs:
@	nONE
@
@ Outputs:
@	None
@
init_i2c:
	/* MFPRs for Aspenite is set in board_init() */

	/* enable i2c clock */
	ldr             r1, =0xd4051024  /* ACGR */
	ldr             r0, [r1]
	orr             r0, r0, #0x40    /* enable i2c clock */
	str             r0, [r1]
	ldr		r1, =0xd401502c  /* TWSI_CLK */
	mov		r0, #0x3
	str		r0, [r1]

	/* Initialize I2C Unit */
	ldr		r1, =0xd4011000 /* I2C register start address */
	mov		r0, #0x0
	str		r0, [r1, #0x20] /* TWSI_SAR */
	mov		r0, #0x4000     /* reset I2C */
	str		r0, [r1, #0x10] /* TWSI_CR */
	mov		r0, #0x60 	/* set IUE and SCLE */
	str		r0, [r1, #0x10]
	mov		pc, lr

@*****************************************************************************
@
@
@ read I2C
@
@ Inputs:
@	r4 - slave address, r5 - register in slave
@
@ Outputs:
@	None
@
readi2c:
	/* Initialize I2C Unit */
	ldr		r3, =0xd4011000	/* I2C register start address */

	/* Set slave I2C address */
	str		r4, [r3, #0x8]	/* IDBR */

	/* send a start condition */
	ldr		r0, [r3, #0x10]
	orr		r0, r0, #0x9	/* START & TB */
	ldr		r2, =0xffffeffd
	and		r0, r0, r2	/* ~(STOP | ALDIE) */
	str		r0, [r3, #0x10]	/* ICR */

	/* wait until tx buffer empty */
	mov		r8, #0x80000	/* time out */
1:
	ldr		r2, [r3, #0x18]	/* ISR */
	subs		r8, r8, #1
	moveq		pc, lr
	ands		r2, r2, #0x40	/* ITE in ISR */
	beq		1b
	str		r2, [r3, #0x18]	/* clear status */

	/* send first byte(register address) */
	str		r5, [r3, #0x8]	/* IDBR */
	ldr		r0, [r3, #0x10]	/* read ICR */
	ldr		r2, =0x1008
	orr		r0, r0, r2	/* ALDIE, TB */
	ldr		r2, =0xfffffffe
	and		r0, r0, r2	/* ~START */
	orr		r0, r0, #0x2	/* STOP */
	str		r0, [r3, #0x10]

	/* wait until tx buffer empty */
	mov		r8, #0x80000	/* time out */
2:
	ldr		r2, [r3, #0x18]	/* ISR */
	subs		r8, r8, #1
	moveq		pc, lr
	ands		r2, r2, #0x40	/* ITE in ISR */
	beq		2b
	str		r2, [r3, #0x18]	/* clear status */

	/* send second byte (slave read address) */
	orr		r4, r4, #0x1
	str		r4, [r3, #0x8]	/* IDBR */
	ldr		r0, [r3, #0x10]	/* read ICR */
	ldr		r2, =0x1008
	orr		r0, r0, r2	/* ALDIE, TB */
	orr		r0, r0, #0x1	/* START */
	ldr		r2, =0xfffffffd
	and		r0, r0, r2	/* ~STOP */
	str		r0, [r3, #0x10]

	/* wait until tx buffer empty */
	mov		r8, #0x80000	/* time out */
3:
	ldr		r2, [r3, #0x18]	/* ISR */
	subs		r8, r8, #1
	moveq		pc, lr
	ands		r2, r2, #0x40	/* ITE in ISR */
	beq		3b
	str		r2, [r3, #0x18]	/* clear status */

	/* send stop signal and read data */
	ldr		r0, [r3, #0x10]	/* read ICR */
	ldr		r2, =0x1008
	orr		r0, r0, r2	/* ALDIE, TB */
	ldr		r2, =0xfffffffe
	and		r0, r0, r2	/* ~START */
	orr		r0, r0, #0x4	/* ACKNACK */
	orr		r0, r0, #0x2	/* STOP */
	str		r0, [r3, #0x10]

	/* wait until rx buffer full */
	mov		r8, #0x80000	/* time out */
3:
	ldr		r2, [r3, #0x18]	/* ISR */
	subs		r8, r8, #1
	moveq		pc, lr
	ands		r2, r2, #0x80	/* IRF in ISR */
	beq		3b
	str		r2, [r3, #0x18]	/* clear status */

	ldr		r0, [r3, #0x8]

	mov		pc, lr

@*****************************************************************************
@
@
@ write  I2C
@
@ Inputs:
@	nONE
@
@ Outputs:
@	None
@
writei2c:
	/* Initialize I2C Unit */
	ldr		r3, =0xd4011000	/* I2C register start address */

	/* Set slave I2C address */
	str		r4, [r3, #0x8]	/* IDBR */

	/* send a start condition */
	ldr		r0, [r3, #0x10]
	orr		r0, r0, #0x9	/* START & TB */
	ldr		r2, =0xffffeffd
	and		r0, r0, r2	/* ~(STOP | ALDIE) */
	str		r0, [r3, #0x10]	/* ICR */

	/* wait until tx buffer empty */
	mov		r8, #0x80000	/* time out */
1:
	ldr		r2, [r3, #0x18]	/* ISR */
	subs		r8, r8, #1
	moveq		pc, lr
	ands		r2, r2, #0x40	/* ITE in ISR */
	beq		1b
	str		r2, [r3, #0x18]	/* clear status */

	/* send first byte(register address) */
	str		r5, [r3, #0x8]	/* IDBR */
	ldr		r0, [r3, #0x10]	/* read ICR */
	ldr		r2, =0x1008
	orr		r0, r0, r2	/* ALDIE, TB */
	ldr		r2, =0xfffffffe
	and		r0, r0, r2	/* ~START */
	ldr		r2, =0xfffffffd
	and		r0, r0, r2	/* ~STOP */
	str		r0, [r3, #0x10]

	/* wait until tx buffer empty */
	mov		r8, #0x80000	/* time out */
2:
	ldr		r2, [r3, #0x18]	/* ISR */
	subs		r8, r8, #1
	moveq		pc, lr
	ands		r2, r2, #0x40	/* ITE in ISR */
	beq		2b
	str		r2, [r3, #0x18]	/* clear status */

	/* send second byte (register value) */
	str		r6, [r3, #0x8]	/* IDBR */
	ldr		r0, [r3, #0x10]	/* read ICR */
	ldr		r2, =0x1008
	orr		r0, r0, r2	/* ALDIE, TB */
	ldr		r2, =0xfffffffe
	and		r0, r0, r2	/* ~START */
	orr		r0, r0, #0x2	/* STOP */
	str		r0, [r3, #0x10]

	/* wait until tx buffer empty */
	mov		r8, #0x80000	/* time out */
3:
	ldr		r2, [r3, #0x18]	/* ISR */
	subs		r8, r8, #1
	moveq		pc, lr
	ands		r2, r2, #0x40	/* ITE in ISR */
	beq		3b
	str		r2, [r3, #0x18]	/* clear status */

	mov		pc, lr

hibernate_sram_end:
	nop

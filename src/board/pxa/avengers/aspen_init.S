/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

@
@       Standard XLLI setup is to configure pins for the lowest power
@       setting.
@
@       MFPR defs
@
@       The NO_UPDATE value uses RESERVED bit 3 to indicate to the xlli code
@       that this MFPR is NOT to be updated when the MFPRs are configured.
@


#define NO_UPDATE         0x0008  @ Do not update this MFPR
#define PULL_UP           0xC000  @ Enable pull up resistor
#define PULL_DN           0xA000  @ Enable pull down resistor
#define DRV_SLOW          0x0800  @ Use slow drive strength
#define DRV_MED           0x1000  @ Use medium drive strength
#define DRV_FAST          0x1800  @ Use fast drive strength
#define EDGE_DISABLED     0x0040  @ Edge Detect disabled

@
@       Note: Bit 7 should always be set so it is taken care of here
@
#define AF0               0x0080  @ Alternate function 0
#define AF1               0x0081  @ Alternate function 1
#define AF2               0x0082  @ Alternate function 2
#define AF3               0x0083  @ Alternate function 3
#define AF4               0x0084  @ Alternate function 4
#define AF5               0x0085  @ Alternate function 5
#define AF6               0x0086  @ Alternate function 6
#define AF7               0x0087  @ Alternate function 7


#define DRV_FAST__EDGE_DISABLED__AF1 0x18C1
#define DRV_FAST__EDGE_DISABLED__AF2 0x18C2
#define DRV_FAST__EDGE_DISABLED__AF3 0x18C3
#define DRV_FAST__EDGE_DISABLED__AF4 0x18C4
#define DRV_FAST__EDGE_DISABLED__AF5 0x18C5
#define DRV_FAST__EDGE_DISABLED__AF6 0x18C6
#define DRV_FAST__EDGE_DISABLED__AF7 0x18C7

#define DRV_SLOW__EDGE_DISABLED__AF1 0x08C1
#define PULL_UP__DRV_FAST__EDGE_DISABLED__AF2 0xD8C2
#define PULL_DN__DRV_SLOW__EDGE_DISABLED__AF7 0xA8C7
#define DRV_SLOW__EDGE_DISABLED__AF7 0x08C7
#define PULL_DN__DRV_FAST__EDGE_DISABLED__AF1  0xB8C1
#define PULL_DN__DRV_FAST__EDGE_DISABLED__AF2  0xB8C2
#define PULL_DN__DRV_FAST__EDGE_DISABLED__AF6  0xB8C6
 
#define PULL_UP__DRV_FAST__EDGE_DISABLED__AF1  0xD8C1
#define PULL_UP__DRV_FAST__EDGE_DISABLED__AF6  0xD8C6

.text

.macro	delay, counter
	mov	\counter,#(1024*4)	@ Initialize counter
1:	subs	\counter, \counter, #1	@ Decrement counter
	bne	1b			@ No, then loop again
.endm

@@@@ - JMC - Begin
.globl aspen_init
       .type   aspen_init, %function
aspen_init:
	stmfd   sp!,    {r1-r4, lr}             @ Save r1, r2 and link register on the stack

	@ Ensure L2 cache is not mapped as SRAM
	ldr	r0, =0xD4282C08
	ldr	r1, [r0]
	bic	r1, r1, #0x10
	str	r1, [r0]

	@ Initialize MFPR registers
	bl	xlli_MFPR_init

	mov	r2, #0
	ldmfd   sp!,    {r1-r4, pc} @ Restore r1, r2 and return to caller
	mov     pc, lr

@@@@ - JMC - End

@@@ JMC - Begin
	
	.type	xlli_MFPR_init, %function
xlli_MFPR_init:	
@
@ Since MFPR offsets are not in a linear sequence, an offset lookup table (below)
@ is used. An entry of 0xFF indicated an unused MFPR offset and a value of 0xF
@ marks the end of the table.
@
        ldr     r0, =0xD401E000             @ MFPR base address
        add	r2, pc, #(xlli_MFPR_offset-(.+8)) @ Address of MFPR offset table
        add     r4, pc, #(xlli_MFPR_data-(.+8))   @ Address of data for MFPR settings 

xlli_m0:
	ldrh    r3,     [r2],   #2      @ Get next offset value
        cmp     r3,     #0xFF           @ Is this MFPR address UNDEFINED?
        addeq   r4,     r4,     #2      @ Yes - skip entry in MFPR settings table
        beq     xlli_m0			@ ...and get the next table entry
        cmp     r3,     #0xF            @ At end of MFPR offset table?
        beq     xlli_m1                 @ YES - take this exit path
@
@ Code continues here if valid MFPR offset was found. If the MFPR value
@ is equal to NO_UPDATE, the MFPR contents are not altered.
@
        ldrh    r1,     [r4],   #2      @ Fetch MFPR data from user table (below)
        cmp     r1,     #NO_UPDATE      @ Update this MFPR?
        beq     xlli_m0			@ DO NOT UPDATE  - process the next MFPR
        str     r1,     [r0, r3]        @ UPDATE - Write MFPR data to the MFPR offset
        ldr     r1,     [r0, r3]        @ Read back to insure write is complete
        b       xlli_m0                 @ Process the next MFPR        
@
@       Exit Path
@
xlli_m1:
	mov	pc, lr
.ltorg

.align
	.type xlli_MFPR_offset, %object
xlli_MFPR_offset:
@
@       NOTE: MFPRs 0-15, 17, 19, 21-25, 27-33, and  87-88 default to non-GPIO
@             functions at boot time but may be configured as GPIOs by using
@             alternate function 5.
@
@       NOTE: MFPRs 123-127 default to non-GPIO functions at boot time but
@             may be configured as GPIOs by using alternate function 6.
@
@       NOTE: GPIOs 64-67 are listed at 0x2xx offsets in spreadsheet but appear that they
@             should be at 0x1xx offsets (the 0x2xx offsets are defined elsewhere)
@
@                        ***** PXA168 *****     ***** PXA168 *****              @   GPIOs
        .hword     0x04C,  0x050,  0x054,  0x058,  0x05C,  0x060,  0x064,  0x068   @ MFPRs   0->7
        .hword     0x06C,  0x070,  0x074,  0x078,  0x07C,  0x080,  0x084,  0x088   @ MFPRs   8->15
        .hword     0x08C,  0x090,  0x094,  0x098,  0x09C,  0x0A0,  0x0A4,  0x0A8   @ MFPRs  16->23
        .hword     0x0AC,  0x0B0,  0x0B4,  0x0B8,  0x0BC,  0x0C0,  0x0C4,  0x0C8   @ MFPRs  24->31
        .hword     0x0CC,  0x0D0,  0x0D4,  0x0D8,  0x0DC,  0x000,  0x004,  0x008   @ MFPRs  32->39
        .hword     0x00C,  0x010,  0x014,  0x018,  0x01C,  0x020,  0x024,  0x028   @ MFPRs  40->47
        .hword     0x02C,  0x030,  0x034,  0x038,  0x03C,  0x040,  0x044,  0x048   @ MFPRs  48->55
        .hword     0x0E0,  0x0E4,  0x0E8,  0x0EC,  0x0F0,  0x0F4,  0x0F8,  0x0FC   @ MFPRs  56->63
        .hword     0x100,  0x104,  0x108,  0x10C,  0x110,  0x114,  0x118,  0x11C   @ MFPRs  64->71
        .hword     0x120,  0x124,  0x128,  0x12C,  0x130,  0x134,  0x138,  0x13C   @ MFPRs  72->79
        .hword     0x140,  0x144,  0x148,  0x14C,  0x150,  0x154,  0x158,  0x15C   @ MFPRs  80->87
        .hword     0x160,  0x164,  0x168,  0x16C,  0x170,  0x174,  0x178,  0x17C   @ MFPRs  88->95
        .hword     0x180,  0x184,  0x188,  0x18C,  0x190,  0x194,  0x198,  0x19C   @ MFPRs  96->103
        .hword     0x1A0,  0x1A4,  0x1A8,  0x1AC,  0x1B0,  0x1B4,  0x1B8,  0x1BC   @ MFPRs 104->111
        .hword     0x1C0,  0x1C4,  0x1C8,  0x1CC,  0x1D0,  0x1D4,  0x1D8,  0x1DC   @ MFPRs 112->119
        .hword     0x1E0,  0x1E4,  0x1E8,  0x1F4,  0x1F8,  0x1FC,  0x200,  0x204   @ MFPRs 120->127
        .hword     0x00F                           @ Marks the end of the table



.align
	.type xlli_MFPR_data, %object
xlli_MFPR_data:
        .hword     NO_UPDATE       @ GPIO_0   - DFI_D15 (no MFPR update required)
        .hword     NO_UPDATE       @ GPIO_1   - DFI_D14 (no MFPR update required)
        .hword     NO_UPDATE       @ GPIO_2   - DFI_D13 (no MFPR update required)
        .hword     NO_UPDATE       @ GPIO_3   - DFI_D12 (no MFPR update required)
        .hword     NO_UPDATE       @ GPIO_4   - DFI_D11 (no MFPR update required)
        .hword     NO_UPDATE       @ GPIO_5   - DFI_D10 (no MFPR update required)
        .hword     NO_UPDATE       @ GPIO_6   - DFI_D9  (no MFPR update required)
        .hword     NO_UPDATE       @ GPIO_7   - DFI_D8  (no MFPR update required)
        .hword     NO_UPDATE       @ GPIO_8   - DFI_D7  (no MFPR update required)
        .hword     NO_UPDATE       @ GPIO_9   - DFI_D6  (no MFPR update required)
        .hword     NO_UPDATE       @ GPIO_10  - DFI_D5  (no MFPR update required)
        .hword     NO_UPDATE       @ GPIO_11  - DFI_D4  (no MFPR update required)
        .hword     NO_UPDATE       @ GPIO_12  - DFI_D3  (no MFPR update required)
        .hword     NO_UPDATE       @ GPIO_13  - DFI_D2  (no MFPR update required)
        .hword     NO_UPDATE       @ GPIO_14  - DFI_D1  (no MFPR update required)
        .hword     NO_UPDATE       @ GPIO_15  - DFI_D0  (no MFPR update required)
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_16  - ND_nCS0
        .hword     NO_UPDATE       @ GPIO_17  - 
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_18  - ND_nCS1
        .hword     DRV_FAST__EDGE_DISABLED__AF5        @ GPIO_19  - GPIO_19
        .hword     NO_UPDATE       @ GPIO_20  -
        .hword     NO_UPDATE       @ GPIO_21  - 
        .hword     NO_UPDATE       @ GPIO_22  -
        .hword     DRV_FAST__EDGE_DISABLED__AF5        @ GPIO_23  - GPIO_23
        .hword     NO_UPDATE       @ GPIO_24  -
        .hword     DRV_FAST__EDGE_DISABLED__AF5        @ GPIO_25  - GPIO_25
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_26  - ND_RnB1
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_27  - ND_RnB2
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_28  - MMC4_CMD
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_29  - MMC4_CLK
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_30  - MMC4_DAT0
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_31  - MMC4_DAT1
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_32  - MMC4_DAT2
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_33  - MMC4_DAT3
        .hword     NO_UPDATE       @ GPIO_34  - 
        .hword     NO_UPDATE       @ GPIO_35  - 
        .hword     NO_UPDATE       @ GPIO_36  - 
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_37  - MMC1_DAT7
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_38  - MMC1_DAT6
        .hword     NO_UPDATE       @ GPIO_39  - 
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_40  - MMC1_DAT1
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_41  - MMC1_DAT0
        .hword     NO_UPDATE       @ GPIO_42  -
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_43  - MMC1_CLK
        .hword     NO_UPDATE       @ GPIO_44  -
        .hword     NO_UPDATE       @ GPIO_45  -
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_46  - MMC1_WP
        .hword     NO_UPDATE       @ GPIO_47  -
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_48  - MMC1_DAT4
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_49  - MMC1_CMD
        .hword     NO_UPDATE       @ GPIO_50  -
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_51  - MMC1_DAT3
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_52  - MMC1_DAT2
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_53  - MMC1_DETECT
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_54  - MMC1_DAT5
        .hword     NO_UPDATE       @ GPIO_55  - 
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_56 - LCD_FCLK_RD
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_57 - LCD_LCLK_A0
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_58 - LCD_PCLK_WR
        .hword     NO_UPDATE        @ GPIO_59 - LCD_DENA_BIAS
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_60 - LCD_DD0
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_61 - LCD_DD1
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_62 - LCD_DD2
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_63 - LCD_DD3
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_64 - LCD_DD4
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_65 - LCD_DD5
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_66 - LCD_DD6
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_67 - LCD_DD7
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_68 - LCD_DD8
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_69 - LCD_DD9
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_70 - LCD_DD10
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_71 - LCD_DD11
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_72 - LCD_DD12
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_73 - LCD_DD13
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_74 - LCD_DD14
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_75 - LCD_DD15
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_76 - LCD_DD16
        .hword     DRV_FAST__EDGE_DISABLED__AF1        @ GPIO_77 - LCD_DD17
        .hword     NO_UPDATE       @ GPIO_78 -
        .hword     NO_UPDATE       @ GPIO_79 -
        .hword     NO_UPDATE       @ GPIO_80 -
        .hword     NO_UPDATE       @ GPIO_81 -
        .hword     NO_UPDATE       @ GPIO_82 -
        .hword     NO_UPDATE       @ GPIO_83 -
        .hword     DRV_FAST__EDGE_DISABLED__AF4        @ GPIO_84  - PWM1
        .hword     DRV_FAST__EDGE_DISABLED__AF7        @ GPIO_85  - USBHPEN
        .hword     DRV_FAST__EDGE_DISABLED__AF7        @ GPIO_86  - SSP5_RXD
        .hword     DRV_FAST__EDGE_DISABLED__AF7        @ GPIO_87  - SSP5_TXD
        .hword     DRV_FAST__EDGE_DISABLED__AF7        @ GPIO_88  - SSP5_CLK
        .hword     DRV_FAST__EDGE_DISABLED__AF7        @ GPIO_89  - SSP5_FRM
        .hword     NO_UPDATE       @ GPIO_90  -
        .hword     NO_UPDATE       @ GPIO_91  -
        .hword     NO_UPDATE       @ GPIO_92  -
        .hword     NO_UPDATE       @ GPIO_93  -
        .hword     NO_UPDATE       @ GPIO_94  -
        .hword     NO_UPDATE       @ GPIO_95  -
        .hword     DRV_FAST__EDGE_DISABLED__AF1       @ GPIO_96  - PWM3
        .hword     NO_UPDATE       @ GPIO_97  - 
        .hword     DRV_FAST__EDGE_DISABLED__AF2       @ GPIO_98  - UART3_TXD
        .hword     DRV_FAST__EDGE_DISABLED__AF2       @ GPIO_99  - UART3_RXD
        .hword     NO_UPDATE       @ GPIO_100 - 
        .hword     NO_UPDATE       @ GPIO_101 -
        .hword     DRV_FAST__EDGE_DISABLED__AF1       @ GPIO_102 - I2C_SDA
        .hword     DRV_FAST__EDGE_DISABLED__AF1       @ GPIO_103 - I2C_SCL
        .hword     NO_UPDATE       @ GPIO_104 - 
        .hword     NO_UPDATE       @ GPIO_105 -
        .hword     NO_UPDATE       @ GPIO_106 -
        .hword     DRV_FAST__EDGE_DISABLED__AF2       @ GPIO_107 - UART1_RXD
        .hword     DRV_FAST__EDGE_DISABLED__AF2       @ GPIO_108 - UART1_TXD
        .hword     NO_UPDATE       @ GPIO_109 -
        .hword     NO_UPDATE       @ GPIO_110 -
        .hword     NO_UPDATE       @ GPIO_111 -
        .hword     NO_UPDATE       @ GPIO_112 -
        .hword     DRV_FAST__EDGE_DISABLED__AF6       @ GPIO_113 - ?
        .hword     DRV_FAST__EDGE_DISABLED__AF6       @ GPIO_114 - ?
        .hword     DRV_FAST__EDGE_DISABLED__AF6       @ GPIO_115 - ?
        .hword     DRV_FAST__EDGE_DISABLED__AF6       @ GPIO_116 - ?
        .hword     DRV_FAST__EDGE_DISABLED__AF6       @ GPIO_117 - ?
        .hword     NO_UPDATE       @ GPIO_118 - 
        .hword     NO_UPDATE       @ GPIO_119 - 
        .hword     NO_UPDATE       @ GPIO_120 - 
        .hword     NO_UPDATE       @ GPIO_121 -
        .hword     NO_UPDATE       @ GPIO_122 - 
        .hword     NO_UPDATE       @ GPIO_123 - 
        .hword     NO_UPDATE       @ GPIO_124 - 
        .hword     NO_UPDATE       @ GPIO_125 - 
        .hword     NO_UPDATE       @ GPIO_126 - 
        .hword     NO_UPDATE       @ GPIO_127 - 

.ltorg

@@@ JMC - End

@*******************************************************************************
@
@     COPYRIGHT (C) 2006-2009 Marvell International Ltd. All Rights Reserved.
@
@   The information in this file is furnished for informational use only,
@   is subject to change without notice, and should not be construed as
@   a commitment by Marvell. Marvell assumes no responsibility or liability
@   for any errors or inaccuracies that may appear in this software or any
@   documenation that may be provided in association with this software.
@
@*******************************************************************************
@
@                           bbu_LCD.s
@
@               Board Bring Up (bbu) LCD Subroutines
@
@

        .global BBU_LCD_init              @ Init LCD controller
        .global BBU_LCD_SPI               @ Send LCD SPI Command
        .global BBU_TPO_INIT              @ Send TPO 4.3" WVGA SPI Commands for Initialization
        .extern BBU_msWait                @ Milisecond wait routine
        .extern BBU_usWait                @ Microsecond wait routine

        .extern BBU_getI2C                @ Read from I2C bus
        .extern BBU_putI2C                @ Write to I2C bus

@ Misc 
	.equ   APB1_CLOCK_UNIT_PHYSICAL_BASE,0xD4015000      @ APB1 Clock unit registers base address
	.equ   APBC_GPIO_CLK_RST_offset,0x0008          @ GPIO Clock/Reset Register
	.equ   xlli_GPIOREGS_PHYSICAL_BASE,0xD4019000	
	.equ   BBU_GPIO_85,0x00200000      		@ GPIO bit 85
	.equ   xlli_GPDR2_offset,0x0014			@ GPIO set 2 direction register
	.equ   xlli_GPSR2_offset,0x0020			@ GPIO set 2 SET register
	.equ   xlli_GPCR2_offset,0x002C			@ GPIO set 2 CLEAR register
	.equ   APS_PMU_UNIT_PHYSICAL_BASE,0xD4282800      @ Application Subsystem PMU Register base	
	.equ   LCD_IRE_AXICLK_EN,0x007F          @ Bit to enable AXI clock	
	.equ   PMUA_LCD_CLK_RES_CTRL_offset,0x004C          @ LCD Clock/Reset Control register	
	.equ   bbu_BBUART_PHYSICAL_BASE,0xD4017000      @ Address used for BBU UART port (STUART)
	.equ   bbu_UASPR_offset,0x1C    @ UART Scratch Pad Register offset
@
@ LCD CONTROLLER base address and register offsets from the base address
@
        .equ   bbu_LCD_PHYSICAL_BASE,0xD420B000
  	.equ   LCD_SPU_DMA_START_ADDR_YO_offset,0x0C0   @ Video Frame o Y Starting address
        .equ   LCD_SPU_DMA_PITCH_YC_offset,0x0E0   @ Video Y and C Linr lenght Reg
 	.equ   LCD_SPUT_DMA_OVSA_HPXL_VLN_offset,0x0E8   @ Video Starting Point on screen Reg
       	.equ   LCD_SPU_DMA_HPXL_VLN_offset,0x0EC   @ Video Size Register
       	.equ   LCD_spu_DZM_HPXL_VLN_offset,0x0F0   @ Video Zoom Register
    	.equ   LCD_CFG_GRA_START_ADDR0_offset,0x0F4   @ Frame 0 Start Address Reg
    	.equ   LCD_CFG_GRA_START_ADDR1_offset,0x0F8   @ Frame 1 Start Address Reg
        .equ   LCD_CFG_GRA_PITCH_offset,0x0FC   @ Graphic line Length Reg
       	.equ   LCD_SPU_GRA_HPXL_VLN_offset,0x104   @ Graphic Size Register
       	.equ   LCD_SPU_GZM_HPXL_VLN_offset,0x108   @ Graphic Size after Zoom Register
        .equ   LCD_SPUT_V_H_TOTAL_offset,0x114   @ Total Screen Size Reg
        .equ   LCD_SPU_V_H_ACTIVE_offset,0x118   @ Total Active Size Reg
        .equ   LCD_SPU_H_PORCH_offset,0x11C   @ Screen Horizontal Porch Register
        .equ   LCD_SPU_V_PORCH_offset,0x120   @ Screen Vertical Porch Register
        .equ   LCD_SPU_BLANKCOLOR_offset,0x124   @ Screen Blank Color Reg
        .equ   LCD_SPU_SPI_CTRL_offset,0x180   @ SPI Control Register
        .equ   LCD_SPU_SPI_TXDATA_offset,0x184   @ SPI Tx Data Register
        .equ   LCD_SPU_SMPN_CTRL_offset,0x188   @ Smart Panel Control Register
        .equ   LCD_SPU_SLV_PORT_offset,0x18C   @ AHB Slave Path Data Port Register
        .equ   LCD_SPU_DMA_CTRL0_offset,0x190   @ DMA Control Register 0
        .equ   LCD_SPU_DMA_CTRL1_offset,0x194   @ DMA control Register 1
        .equ   LCD_CFG_SCLK_DIV_offset,0x1A8   @ LCD Clock Divider
        .equ   LCD_SPU_CONTRAST_offset,0x1AC   @ Video Contrast Register
        .equ   LCD_SPU_SATURATION_offset,0x1B0   @ Video Saturation Register
        .equ   LCD_SPU_CBSH_HUE_offset,0x1B4   @ Video Hue Adjust Register
        .equ   LCD_SPU_DUMB_CTRL_offset,0x1B8   @ Dumb LCD Panel Control Register
      	.equ   LCD_SPU_IOPAD_CONTROL_offset,0x1BC   @ LCD_I/O Pads Control Register
        .equ   LCD_SPU_IRQ_ISR_offset,0x1C4   @ LCD Interrupt Status Register
@
@       Bit defs
@
        .equ     SLV_IRQ,0x00080000      @ Bit 19 = AHB Slave Path All Cmds done
        .equ     SMPN_RSTB,0x00000008      @ Smart panel reset pin control bit
@
@       Defs used for Toshiba Display
@
       	.equ     LCD_COMMAND_WRITE,0x00000100  @
       	.equ     LCD_DATA_WRITE,0x00008000  @
      	.equ     LCD_WAIT_FOR_VSYNC,0x00000100  @
        .equ     LCD_INT_PROC,0x00000100  @
        .equ     LCD_NO_OP,0x00000100  @

        .equ     LCD_COMMAND,0X0<<8
        .equ     LCD_DATA,0X1<<8
        .equ     LCD_STB_REL,0x1D    @ Standby Release Command


@
@*******************************************************************************
@       Local definitions for LCD functions
@*******************************************************************************
@
@
@
@   **  Values for TPO LCD display  (WVGA mode) **
@
     	.equ     LCD_Width_3,800     @ LCD width in pixels
    	.equ     LCD_Height_3,480     @ LCD height in pixels
    	.equ     LCD_T_vert_3,525     @ Total Vert line count
    	.equ     LCD_T_horz_3,1056    @ Total Horz line count
   	.equ     LCD_horz_fp_3,40      @ Horz Front Porch value
   	.equ     LCD_horz_bp_3,215     @ Horz Back Porch value
   	.equ     LCD_vert_fp_3,10      @ Vert Front Porch value
   	.equ     LCD_vert_bp_3,34      @ Vert Back Porch value
    	.equ     LCD_ColorD_3,24      @ Color depth in bits
     	.equ     LCD_Bytes_3,3       @ Bytes per pixel
      	.equ     LCD_PClk_3,9       @ For 39MHz PCLK (Based on 312Mhz clock)
@
@LCD_T_horz_8    EQU     1688    ; Total Horz line count
@LCD_horz_fp_8   EQU     128     ; Horz Front Porch value
@LCD_horz_bp_8   EQU     200     ; Horz Back Porch value
@LCD_vert_fp_8   EQU     10      ; Vert Front Porch value
@LCD_vert_bp_8   EQU     15      ; Vert Back Porch value

        @NOT SUPPORTED: AREA  "|text|,CODE,READONLY,ALIGN = 5"
@
@*******************************************************************************
@
@       ****************
@       *              *
@       * BBU_LCD_init * Init the LCD display
@       *              *
@       ****************
@
@ This subroutine configures the LCD controller for use
@
@       NOTE:   Video registers refer to YUV control registers.
@             Graphic registers refer to RGB control registers.
@
@ PARAMETERS REQUIRED:
@
@       INPUT:  r0 configures the controller as follows:
@                  0 = VGA  Zylonite Display
@                  1 = QVGA Zylonite Display
@                  2 = WVGA Innolux Display
@                  3 = WVGA TPO Display
@                  4 = TTC/EVB Smart Panel display
@                  5 = HDMI display (VGA mode)
@                  6 = HDMI display (SVGA mode)
@                  7 = HDMI display (XGA mode)
@                  8 = HDMI display (720P mode)
@                  9 = HDMI display (SXGA mode)
@                 10 = HDMI display (UXGA mode)
@                 11 = HDMI display (1080P mode)
@
@
@       OUTPUT:
@               None
@
@       Note: BBU assumes the LCD controller is at POR values when entering this
@             procedure. If a different LCD configuration was set up previously
@             it may be necessary to add code to reset some registers to their
@             POR values. Values may not be optimal at this time.

@NOT SUPPORTED: BBU_LCD_init:  FUNCTION
BBU_LCD_init:

        stmfd   sp!,    {r0-r12, lr}     @ Save registers and link on the stack
	mov	r0,	#0x3		@ 
        ldr     r2,     =LCD_INIT       @ Save user parameter here...
        strb    r0,     [r2]            @ ...for future reference
@
@       Turn on the GPIO clock
@
        ldr     r4,     =APB1_CLOCK_UNIT_PHYSICAL_BASE  @ Address of the APBC_UART0_CLK_RST register
        mov     r5,     #0x3                            @ Set these bits to enable clocks
        str     r5,     [r4, #APBC_GPIO_CLK_RST_offset] @ Write to clock control register
@
@       Set GPIO 85 as output then set/clear the bit depending on LCD mode
@
        ldr     r1,     =xlli_GPIOREGS_PHYSICAL_BASE    @ Get base addres for GPIO registers
        mov     r2,     #BBU_GPIO_85                    @ Used for Zylonite LCD mode select
        ldr     r4,     [r1, #xlli_GPDR2_offset]        @ Get GPDR2 value
        orr     r4,     r4,     r2                      @ Set this bit to make GPIO an output
	str     r4,     [r1, #xlli_GPDR2_offset]        @ Write GPDR2 value back
@        streq     r4,     [r1, #xlli_GPDR2_offset]        @ Write GPDR2 value back
        str     r2,     [r1, #xlli_GPCR2_offset]        @ WVGA - set the GPIO low
@
@       Store LCD parameters for use by LCD subroutines
@
        ldr     r2,     =LCD_HORZ_P     @ Address of LCD pixel width parameter
        ldr     r3,     =LCD_VERT_P     @ Address of LCD pixel height parameter
        ldr     r4,     =LCD_BPP        @ Address of LCD Bytes Per Pixel parameter
        ldr     r6,     =LCD_CD         @ Address of LCD color depth parameter
@
@       LCD type 3 - TPO LCD in WVGA mode
@
1003:   ldr     r1,     =LCD_Width_3    @ Get width
        strh    r1,     [r2]            @ Save width
        ldr     r1,     =LCD_Height_3   @ Get height
        strh    r1,     [r3]            @ Save height
        ldr     r1,     =LCD_Bytes_3    @ Bytes per pixel count
        strb    r1,     [r4]            @ Save Bytes Per Pixel
        ldr     r1,     =LCD_ColorD_3   @ Color depth in bits
        strb    r1,     [r6]            @ Save color depth
        ldr     r2,     =LCD_T_HORZ     @ Total horz lines saved here
        ldr     r3,     =LCD_T_VERT     @ Total vert lines saved here
        ldr     r1,     =LCD_T_horz_3   @ Total vert line count
        strh    r1,     [r2]            @ Save in table
        ldr     r1,     =LCD_T_vert_3   @ Total horz line count
        strh    r1,     [r3]            @ Save in table
        ldr     r2,     =LCD_HORZ_FP    @ Horz Front Porch data saved here
        ldr     r3,     =LCD_HORZ_BP    @ Horz Back Porch saved here
        ldr     r4,     =LCD_VERT_FP    @ Horz Front Porch data saved here
        ldr     r6,     =LCD_VERT_BP    @ Horz Back Porch saved here
        ldr     r1,     =LCD_horz_fp_3  @ Horz Front Porch value
        strb    r1,     [r2]            @ Save in table
        ldr     r1,     =LCD_horz_bp_3  @ Horz Back Porch value
        strb    r1,     [r3]            @ Save in table
        ldr     r1,     =LCD_vert_fp_3  @ Vert Front Porch value
        strb    r1,     [r4]            @ Save in table
        ldr     r1,     =LCD_vert_bp_3  @ Vert Back Porch value
        strb    r1,     [r6]            @ Save in table
        ldr     r2,     =LCD_PCLK       @ Pixel Clock divisor saved here
        ldr     r1,     =LCD_PClk_3     @ Get pixel clock dvider
        strb    r1,     [r2]            @ Save in table
@        b       9                     @ Continue the init process (KV)
@
@///////////////////////////////////////////////////////////////////////////////
@
@       Begin the LCD init process
@
9:      ldr     r4,     =BBU_LCD_TYPE   @ Address where LCD type is kept
        mov     r3,     #0              @ Clear r3
        strb    r3,     [r4]            @ Save this value
@       
@       Enable clock to LCD controller (one bit at a time for now)
@
        ldr     r1,     =APS_PMU_UNIT_PHYSICAL_BASE             @ PMU register base
        mov     r2,     #LCD_IRE_AXICLK_EN                      @ Enable AXI clock
        str     r2,     [r1, #PMUA_LCD_CLK_RES_CTRL_offset]     @ Write to register

@
@       Get physical address of LCD frame buffer
@
@       ldr     r0,     =BBU_LCD_FRAME_BUF      @ LCD Frame Buff addrs (KV)

@       bl      BBU_GetPhysical         @ Get the physical address for the frame buffer (KV)

@
@       **** Set up the LCD control registers ****
@
        ldr     r1,     =bbu_LCD_PHYSICAL_BASE  @ Get base addess for LCD controller
@
@       Disable LCD DMA controller
@
        ldr     r2,     =0x00000000                     @ DMA Control (disable)
        str     r2,     [r1, #LCD_SPU_DMA_CTRL0_offset] @ Write to register

@
@       Set LCD graphics base address
@
@       str     r0,     [r1, #LCD_CFG_GRA_START_ADDR0_offset]   @ Start Address (KV)
@       ldr     r2,     =0x00000000                             @ Area #1 not used in BBU (KV)
@       str     r2,     [r1, #LCD_CFG_GRA_START_ADDR1_offset]   @ Start Address (KV)
@
@       LCD Graphic Display width (pitch)
@
        ldr     r5,     =LCD_HORZ_P     @ LCD pixel width location
        ldrh    r3,     [r5]            @ Fetch LCD width
        ldr     r5,     =LCD_BPP        @ LCD Bytes Per Pixel location
        ldrb    r2,     [r5]            @ Fetch LCD Bytes Per Pixel
        mul     r2,     r2,     r3      @ r2 =line width in bytes
        str     r2,     [r1, #LCD_CFG_GRA_PITCH_offset] @ Update controller reg
@
@       Set up graphic size and size after zoom registers
@
        ldr     r5,     =LCD_VERT_P     @ LCD pixel height location
        ldrh    r2,     [r5]            @ Fetch LCD height
        mov     r2,     r2,     LSL #16 @ Move 16 bits to left
        orr     r0,     r2,     r3      @ OR in the LCD width
        str     r0,     [r1,#LCD_SPU_GRA_HPXL_VLN_offset] @ Graphic size reg
        str     r0,     [r1,#LCD_SPU_GZM_HPXL_VLN_offset] @ Graphic size after zoom
        str     r0,     [r1,#LCD_SPU_V_H_ACTIVE_offset]   @ Set up the active area
@
@       Set up total screen size
@
        ldr     r5,     =LCD_T_HORZ     @ Total horz line count saved here
        ldrh    r0,     [r5]            @ Fetch this value
        ldr     r3,     =LCD_T_VERT     @ Total vert line count saved here
        ldrh    r4,     [r3]            @ Fetch this value
        mov     r4,     r4,     LSL #16 @ Move T-VERT 16 bits to the left
        orr     r4,     r4,     r0                      @ OR in the T_HORZ data
        str     r4,     [r1, #LCD_SPUT_V_H_TOTAL_offset]@ Write the size register

@
@       Set up Horizontal porch register
@
        ldr     r5,     =LCD_HORZ_FP    @ Total horz front porch value saved here
        ldrb    r0,     [r5]            @ Fetch this value
        ldr     r3,     =LCD_HORZ_BP    @ Total horz back porch value saved here
        ldrb    r4,     [r3]            @ Fetch this value
        mov     r4,     r4,     LSL #16 @ Move Back porch 16 bits to the left
        orr     r4,     r4,     r0                      @ OR in the T_HORZ data
        str     r4,     [r1,#LCD_SPU_H_PORCH_offset]    @ Write the register
@
@       Set up Vertical porch register
@
        ldr     r5,     =LCD_VERT_FP    @ Total vert front porch value saved here
        ldrb    r0,     [r5]            @ Fetch this value
        ldr     r3,     =LCD_VERT_BP    @ Total vert back porch value saved here
        ldrb    r4,     [r3]            @ Fetch this value
        mov     r4,     r4,     LSL #16 @ Move Back porch 16 bits to the left
        orr     r4,     r4,     r0                      @ OR in the T_HORZ data
        str     r4,     [r1,#LCD_SPU_V_PORCH_offset]    @ Write the register
@
@       Write DMA control register 1
@
        ldr     r3,     =0x2000FF00                     @ DMA control value
        str     r3,     [r1,#LCD_SPU_DMA_CTRL1_offset]  @ Write control register
@
@       Fill in video Contrast, Saturation, and Hue registers
@
        ldr     r2,     =0x20004000                     @ Default value
        str     r2,     [r1,#LCD_SPU_SATURATION_offset] @ Set up Saturation Register
        ldr     r3,     =0x00004000                     @ Default value
        str     r3,     [r1,#LCD_SPU_CONTRAST_offset]   @ Set up Contrast register
        str     r3,     [r1,#LCD_SPU_CBSH_HUE_offset]   @ Set up the Hue Adj register
@
@       Configure the LCD I/O Pads Control Register
@       Configure as 1K DDR burst boundry & 16-bit panel with GPIO
@
        ldr     r3,     =0x00000024                     @ Config value
        ldr     r5,     =LCD_BPP                        @ LCD Bytes Per Pixel location
        ldrb    r2,     [r5]                            @ Fetch LCD Bytes Per Pixel
        cmp     r2,     #3                              @ Is it 3 BPP?
        bne     F93                            	    	@ No
        bic     r3,     r3,     #0x00000004        	@ Yes - Set for 24Bit
F93:     str     r3,     [r1,#LCD_SPU_IOPAD_CONTROL_offset]  @ Write to register
@
@       Configure the Dumb Panel Control Register
@
        ldr     r3,     =0x010FF10D                     @ Bit 24 turns backlight on
        ldr     r5,     =LCD_BPP                        @ LCD Bytes Per Pixel location
        ldrb    r2,     [r5]                            @ Fetch LCD Bytes Per Pixel
        cmp     r2,     #3                              @ Is it 3 BPP?
        bne     F935                                     @ No
        orr     r3,     r3,     #0x60000000             @ Yes - Set for 24Bit

F935:    ldr     r5,     =LCD_INIT                       @ LCD Type location
        ldrb    r2,     [r5]                            @ LCD Type
        cmp     r2,     #3                              @ Is it TPO LCD?
        bne     F94                                      @ No
        orr     r3,     r3,     #0x00000002             @ Yes - Invert Pixel Clock

F94:     str     r3,     [r1,#LCD_SPU_DUMB_CTRL_offset]  @ Write the register
@
@       Set LCD controller Frequency
@
        ldr     r3,     =LCD_PCLK                       @ Location of clock divisor
        ldrb    r2,     [r3]                            @ Fetch the divisor
        orr     r2,     r2,     #0x80000000             @ OR in value to use 312MHz Clock
        str     r2,     [r1,#LCD_CFG_SCLK_DIV_offset]   @ Write the clock register
@
@       Enable DMAs to start transfer of data to the LCD
@
        ldr     r3,     =0x00000111                     @ DMA control (enable)
        ldr     r5,     =LCD_BPP                        @ LCD Bytes Per Pixel location
        ldrb    r2,     [r5]                            @ Fetch LCD Bytes Per Pixel
        cmp     r2,     #3                              @ Is it 3 BPP?
        bne     F95                                      @ No
        orr     r3,     r3,	#0x00220000             @ Yes - Set for 24Bit
F95:     str     r3,     [r1,#LCD_SPU_DMA_CTRL0_offset]  @ Write to register

@        b       40                    			@ Exit Path (KV)
@
@
@40:     ldr     r3,     =BBU_LCD_BUF            @ Address where buffer used is indicated (KV)
@        ldr     r0,     =BBU_LCD_FRAME_BUF      @ LCD Frame buffer address (KV)
@        str     r0,     [r3]                    @ Save LCD frame buffer address (KV)
@
@       Return to caller
@
        ldmfd   sp!,    {r0-r12, pc}     @ Restore registers and return to caller

     .align 2
BBU_LCD_BUF:     .word     0       @ LCD buffer in use - store physical address here
     .align 2
LCD_DMA_DB:      .word     0,0,0,0 @ 4 word DMA descriptor block
@
@       LCD type
@
@       Bits 3:0
@           0 = Zylonite
@           1 = Toshiba smart panel on EVB platform
@           2-7 = Undefined
@
@       Bits 7:4 may have one of the following values:
@           0 = QVGA screen
@           4 = Wide VGA
@           8 = VGA
@
BBU_LCD_TYPE:    .byte 0       @ LCD Type


LCD_EX_STBY_SIZE:        .byte (LCD_EX_STBY_END - LCD_EX_STBY_CMD)   @ Length
LCD_EX_STBY_CMD:@    ---ADDR---  ---DATA---
                .byte 0x00
                .byte 0x1D
                .byte 0x00
                .byte 0x05      @ Mode setting - Clear Standby and Deep standby states
LCD_EX_STBY_END:

@
@       NOTE: Some setup commands are commented out below - depending on desired setup
@
LCD_P_ON_INI_SIZE:       .byte (LCD_P_ON_INI_END - LCD_P_ON_INI_CMD)   @ Length
LCD_P_ON_INI_CMD:@   ---ADDR---  ---DATA---
                .byte 0x00
                .byte 0x00
                .byte 0x00
                .byte 0x01      @ OSC ON
                .byte 0x00
                .byte 0x01
                .byte 0x01
                .byte 0x27      @ Driver output control (256 x 320 dots)
                .byte 0x00
                .byte 0x02
                .byte 0x02
                .byte 0x00      @ Drive signal setting (Line AC setting)
@               DCB 0x00, 0x03, 0xE1, 0x30      ; IF mode(1) - 8bit smart mode(6-6-6),high speed write mode
                .byte 0x00
                .byte 0x03
                .byte 0x01
                .byte 0x30      @ IF mode(1) - 8bit smart mode(5-6-5) high speed write mode
@               DCB 0x00, 0x03, 0x00, 0x30      ; IF mode(1) - Normal write mode
                .byte 0x07
                .byte 0x00
                .byte 0x00
                .byte 0x03      @ RAM Write Mode (required Mystery command!)
@               DCB 0x00, 0x07, 0x00, 0x00      ; DISPLAY Setting, 262K colors
                .byte 0x00
                .byte 0x07
                .byte 0x40
                .byte 0x00      @ DISPLAY Setting 65K colors

                .byte 0x00
                .byte 0x08
                .byte 0x03
                .byte 0x02      @ BP, FP Seting, BP=2H, FP=3H
                .byte 0x00
                .byte 0x0C
                .byte 0x00
                .byte 0x00      @ Using internal clock & MPU
                .byte 0x00
                .byte 0x0D
                .byte 0x00
                .byte 0x10      @ Frame setting, 1Min. Frequency, 16CLK
                .byte 0x00
                .byte 0x12
                .byte 0x03
                .byte 0x02      @ Timing(1), ASW W=4CLK, ASW ST=1CLK

                .byte 0x00
                .byte 0x13
                .byte 0x01
                .byte 0x02      @ Timing(2), OEV ST=0.5CLK, OEV ED=1CLK
                .byte 0x00
                .byte 0x14
                .byte 0x00
                .byte 0x00      @ Timing(3), ASW HOLD=0.5CLK
                .byte 0x00
                .byte 0x15
                .byte 0x20
                .byte 0x00      @ Timing(4), CKV ST=0CLK, CKV ED=1CLK
                .byte 0x00
                .byte 0x1C
                .byte 0x00
                .byte 0x00      @ AMP POWER SETTING
                .byte 0x03
                .byte 0x00
                .byte 0x04
                .byte 0x03      @ GRAY SCALE SETTING
                .byte 0x03
                .byte 0x01
                .byte 0x03
                .byte 0x04      @ GRAY SCALE SETTING 2
                .byte 0x03
                .byte 0x02
                .byte 0x04
                .byte 0x03      @ GRAY SCALE SETTING 3
                .byte 0x03
                .byte 0x03
                .byte 0x03
                .byte 0x03      @ GRAY SCALE SETTING 4
                .byte 0x03
                .byte 0x04
                .byte 0x01
                .byte 0x01      @ GRAY SCALE SETTING 5
                .byte 0x03
                .byte 0x05
                .byte 0x00
                .byte 0x00      @ BLUE OFFSET SETTING

                .byte 0x04
                .byte 0x02
                .byte 0x00
                .byte 0x00      @ First screen start line = 0
                .byte 0x04
                .byte 0x03
                .byte 0x01
                .byte 0x3F      @ First screen end line = 319
LCD_P_ON_INI_END:

LCD_DISP_DATA_WRITE_SIZE:
                .byte (LCD_DISP_DATA_WRITE_END - LCD_DISP_DATA_WRITE_CMD) @ Length
LCD_DISP_DATA_WRITE_CMD:
@                   ---ADDR---  ---DATA---
                .byte 0x04
                .byte 0x06
                .byte 0x00
                .byte 0x00  @ HADDR, START = 0
                .byte 0x04
                .byte 0x07
                .byte 0x00
                .byte 0xEF  @ HADDR, END = 239
                .byte 0x04
                .byte 0x08
                .byte 0x00
                .byte 0x00  @ VADDR, START
                .byte 0x04
                .byte 0x09
                .byte 0x01
                .byte 0x3F  @ VADDR, END
                .byte 0x02
                .byte 0x00
                .byte 0x00
                .byte 0x00  @ RAM ADDR SETTING
                .byte 0x02
                .byte 0x01
                .byte 0x00
                .byte 0x00  @ RAM ADDR SETTING
LCD_DISP_DATA_WRITE_END:

LCD_DISP_ON_SIZE:
                .byte (LCD_DISP_ON_END - LCD_DISP_ON_CMD)     @ Length
LCD_DISP_ON_CMD:
@                   ---ADDR---  ---DATA---
                .byte 0x01
                .byte 0x02
                .byte 0x07
                .byte 0x7D
                .byte 0x01
                .byte 0x03
                .byte 0x00
                .byte 0x05
                .byte 0x01
                .byte 0x04
                .byte 0x00
                .byte 0x00
                .byte 0x01
                .byte 0x05
                .byte 0x00
                .byte 0x15
                .byte 0x01
                .byte 0x00
                .byte 0xC0
                .byte 0x10
LCD_DISP_ON_END:

LCD_DISP_ON_2_SIZE:
                .byte (LCD_DISP_ON_2_CMD_END - LCD_DISP_ON_2_CMD)     @ Length
LCD_DISP_ON_2_CMD: @ ---ADDR---  ---DATA---
                .byte 0x01
                .byte 0x01
                .byte 0x00
                .byte 0x01
                .byte 0x01
                .byte 0x00
                .byte 0xFF
                .byte 0xFE
LCD_DISP_ON_2_CMD_END:
                .align  2

@NOT SUPPORTED:         ENDFUNC
@
@*******************************************************************************
@
@       ****************
@       *              *
@       * BBU_TPO_INIT * Initialize TPO 4.3" WVGA LCD
@       *              *
@       ****************
@
@ This subroutine sends the TPO 4.3" WVGA LCD the necessary SPI commands
@

@NOT SUPPORTED: BBU_TPO_INIT:  FUNCTION
BBU_TPO_INIT:

        stmfd   sp!,    {r0-r12, lr}    @ Save registers and link on the stack (KV)

@
@       LCD Initialization Routine
@
        ldr     r0,     =0x080F @
@       ldr     r0,     =0x0807 ; Legacy - this version does not invert pixel clock
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x0C5F @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x1017 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x1420 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x1808 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x1c20 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x2020 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x2420 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x2820 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x2c20 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x3020 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x3420 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x3810 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x3c10 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x4010 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x4415 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x48aa @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x4cff @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x5086 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x548d @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x58d4 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x5cfb @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x602e @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x645a @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x6889 @
        bl      BBU_LCD_SPI     @  
        ldr     r0,     =0x6cfe @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x705a @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x749b @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x78c5 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x7cff @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x80f0 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x84f0 @
        bl      BBU_LCD_SPI     @ 
        ldr     r0,     =0x8808 @
        bl      BBU_LCD_SPI     @ 


        ldmfd   sp!,    {r0-r12, pc}  @ Restore registers and return to caller (KV)

@NOT SUPPORTED:         ENDFUNC
@
@       ******************
@       **              **
@       ** LITERAL POOL **     LOCAL DATA STORAGE
@       **              **
@       ******************
@
        .ltorg
@
@
@*******************************************************************************
@
@       ***************
@       *             *
@       * BBU_LCD_SPI * Send SPI Commands using SPI in LCD Controller
@       *             *
@       ***************
@
@ This subroutine is used for sending SPI commands used for the TPO LCD
@
@ PARAMETERS REQUIRED:
@
@       INPUT:  r0 is the command formated at 1 HEX WORD
@
@
@
@       Note: Right now this is only for TX, this LCD can also support RX.
@             RX is not implimented at this time.

@NOT SUPPORTED: BBU_LCD_SPI:   FUNCTION
BBU_LCD_SPI:

        stmfd   sp!,    {r1-r12, lr}     @ Save registers and link on the stack (KV)
        mov     r5,     r0              @ Save Command DATA into r5 (KV)
@
@ Make sure LCD_CS starts out HIGH
@
        mov     r4,     #0x40                      @ I2C address for GPIO Expander
        ldr     r0,	=bbu_BBUART_PHYSICAL_BASE  @ Fetch base address of FFUART
        str     r4,	[r0, #bbu_UASPR_offset]    @ Store I2C address in the scratch pad register

        mov     r0,     #0x03                      @ GPIO Expander Output Port Register
        bl      BBU_getI2C                         @ Read from chip
        orr     r1,     r1,     #0x40              @ Set bit 14
        mov     r0,     #0x03                      @ GPIO Expander Output Port Register
        bl      BBU_putI2C                         @ Write to chip

        mov     r0,     #0x07                      @ GPIO Expander Configuration Register
        bl      BBU_getI2C                         @ Read from chip
        bic     r1,     r1,     #0x40              @ Clear bit 14
        mov     r0,     #0x07                      @ GPIO Expander Configuration Register
        bl      BBU_putI2C                         @ Write to chip
@
@ Grab Current LCD Settings and Store these, then Reconfigure IO PADS for SPI use
@
        ldr     r0,     =bbu_LCD_PHYSICAL_BASE             @ Get base addess for LCD controller
        ldr     r6,     [r0,#LCD_SPU_DUMB_CTRL_offset]     @ Grab Dumb LCD Panel Control Settings, store in r6
        ldr     r1,     =0xF0000000                        @ Disable LCD, Blank Display
        str     r1,     [r0,#LCD_SPU_DUMB_CTRL_offset]     @ 

        ldr     r7,     [r0,#LCD_SPU_IOPAD_CONTROL_offset] @ Grab LCD I/O Pad Settings, store in r7
        ldr     r1,     =0x00000021                        @ Set IO PADS for 18bit+SPI Mode
        str     r1,     [r0,#LCD_SPU_IOPAD_CONTROL_offset] @ 

        ldr     r1,     [r0,#LCD_SPU_DUMB_CTRL_offset]     @ Grab Dumb LCD Panel Control Settings
        orr     r1,     r1,     #0x1                       @ ENABLE LCD
        str     r1,     [r0,#LCD_SPU_DUMB_CTRL_offset]     @ 
@
@ Setup LCD controller SPI Port
@
        ldr     r1,     =0x02000F4E                   	   @ Set LCD Controller SPI for 16bit Transmit
        str     r1,     [r0,#LCD_SPU_SPI_CTRL_offset]      @ 
@
@ Make sure SPI_IRQ bit is cleared
@
        ldr     r1,     [r0,#LCD_SPU_IRQ_ISR_offset]       @ Get LCD Interrupt Status
        bic     r1,     r1,     #0x00040000                @ Clear SPI_IRQ bit
        str     r1,     [r0,#LCD_SPU_IRQ_ISR_offset]       @ 
@
@ Set LCD_CS LOW (START of Window)
@
        mov     r4,     #0x40                  @ I2C address for GPIO Expander
        ldr     r0,	=bbu_BBUART_PHYSICAL_BASE  @ Fetch base address of FFUART
        str     r4,	[r0, #bbu_UASPR_offset]    @ Store I2C address in the scratch pad register

        mov     r0,     #0x03                      @ GPIO Expander Output Port Register
        bl      BBU_getI2C                         @ Read from chip
        bic     r1,     r1,     #0x40              @ Clear bit 14
        mov     r0,     #0x03                      @ GPIO Expander Output Port Register
        bl      BBU_putI2C                         @ Write to chip

        mov     r0,     #2              @ Wait 2mS
        bl      BBU_msWait              @ Wait
@
@
@ START OF COMMAND
@
@
@ Set TX Data (Command Data is in r5) (KV)
@
        ldr     r0,     =bbu_LCD_PHYSICAL_BASE             @ Get base addess for LCD controller
        str     r5,     [r0,#LCD_SPU_SPI_TXDATA_offset]     @ Set SPI Tx Data Register (KV)
@
@ Send TX Data
@
        ldr     r3,     [r0,#LCD_SPU_SPI_CTRL_offset]       @ Get Set LCD Controller settings
        orr     r3,     r3,     #0x1                       @ Set CFG_SPI_START bit
@        bic     r3,     r3,     #0x40                      ; Clear CFG_KEEPXFER bit
@        orr     r3,     r3,     #0x40                      ; Set CFG_KEEPXFER bit
        str     r3,     [r0,#LCD_SPU_SPI_CTRL_offset]       @ SEND COMMAND
@
F10152:   ldr     r1,     [r0,#LCD_SPU_IRQ_ISR_offset]        @ Get LCD Interrupt Status
        ldr     r2,     =0x00040000                         @
        and     r2,     r1,     r2                          @ Check SPI_IRQ bit
        cmp     r2,     #0x00040000                         @
        bne     F10152                                     @ Loop until SPI_IRQ bit is set
        bic     r1,     r1,     #0x00040000                 @ Clear SPI_IRQ bit
        str     r1,     [r0,#LCD_SPU_IRQ_ISR_offset]        @ Clear SPI_IRQ bit
        mov     r0,     #5              @ Wait 5uS
        bl      BBU_usWait              @ Wait
@
@ END OF COMMAND
@
        ldr     r0,     =bbu_LCD_PHYSICAL_BASE             @ Get base addess for LCD controller
        ldr     r1,     [r0,#LCD_SPU_SPI_CTRL_offset]      @ Get SPI Control Settings
@        bic     r3,     r3,     #0x41                      ; Clear CFG_KEEPXFER and CFG_SPI_START bits
        bic     r3,     r3,     #0x40                      @ Clear CFG_KEEPXFER bit
        bic     r3,     r3,     #0x1                       @ Clear CFG_SPI_START bit
        str     r3,     [r2]                               @ Set SPI Control Settings
@
@ Set LCD_CS HIGH (END of Window)
@
        mov     r4,     #0x40                  @ I2C address for GPIO Expander
        ldr     r0,	=bbu_BBUART_PHYSICAL_BASE  @ Fetch base address of FFUART
        str     r4,	[r0, #bbu_UASPR_offset]    @ Store I2C address in the scratch pad register

        mov     r0,     #0x03                      @ GPIO Expander Output Port Register
        bl      BBU_getI2C                         @ Read from chip
        orr     r1,     r1,     #0x40              @ Set bit 14
        mov     r0,     #0x03                      @ GPIO Expander Output Port Register
        bl      BBU_putI2C                         @ Write to chip
@
@ Setup LCD controller for LCD mode (Original Settings)
@
        ldr     r0,     =bbu_LCD_PHYSICAL_BASE             @ Get base addess for LCD controller
        str     r7,     [r0,#LCD_SPU_IOPAD_CONTROL_offset] @ Restore LCD I/O Pad Settings
        str     r6,     [r0,#LCD_SPU_DUMB_CTRL_offset]     @ Restore Dumb LCD Panel Control Settings

        ldmfd   sp!,    {r1-r12, pc} @ Restore registers, lr and return to caller (KV)

@NOT SUPPORTED:         ENDFUNC
@

LCD_DIS_START_SIZE:
                .byte (LCD_DIS_START_END - LCD_DIS_START) @ Length

LCD_DIS_START:   @   ---ADRS---  ---DATA---
                .byte 0x02
                .byte 0x00
                .byte 0x00
                .byte 0x00      @ RAM address setting 1
                .byte 0x02
                .byte 0x01
                .byte 0x00
                .byte 0x00      @ RAM address setting 2
LCD_DIS_START_END:
@
@       LCD PARAMETER STORAGE TO BE RERERENCED BY LCD SUBROUTINES
@       Data filled in by LCD init routine. Dummy non-zero values
@       are put in for now for a standard QVGA screen in 16-bit RGB format.
@
      .align 1
LCD_HORZ_P:      .short     240     @ LCD display width in pixels  (Default is QVGA size)
      .align 1
LCD_VERT_P:      .short     320     @ LCD display height in pixels (Default is QVGA size)
      .align 1
LCD_T_HORZ:      .short     140     @ Total horizontal lines
      .align 1
LCD_T_VERT:      .short     144     @ Total vertical lines

LCD_HORZ_FP:     .byte 69      @ Horz Front Porch value
LCD_HORZ_BP:     .byte 9       @ Horz Back Porch value
LCD_VERT_FP:     .byte 2       @ Vert Front Porch value
LCD_VERT_BP:     .byte 1       @ Vert Back Porch value
LCD_PCLK:        .byte 26      @ Pixel clock divider
LCD_CD:          .byte 16      @ LCD color depth (16, 18, 24 bits - Default is 16)
LCD_BPP:         .byte 2       @ Number of Bytes Per Pixel (Default is 2 bytes)
LCD_O:           .byte 0       @ LCD orientation 0=Landscape, 1 = Portrait
LCD_INIT:        .byte 0       @ User parameter on last LCD init call

        .align   2

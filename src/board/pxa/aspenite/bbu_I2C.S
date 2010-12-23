@*********************************************************************************
@
@     COPYRIGHT (C) 2006 Marvell International Ltd. All Rights Reserved.
@
@   The information in this file is furnished for informational use only,
@   is subject to change without notice, and should not be construed as
@   a commitment by Marvell. Marvell assumes no responsibility or liability
@   for any errors or inaccuracies that may appear in this software or any
@   documenation that may be provided in association with this software.
@
@*********************************************************************************
@
@                                     I2C_CS.s
@                           Subroutines for the I2C bus
@
        .global BBU_I2C_Init      @ Init the seleted I2C bus for use
	.global BBU_Timer_init    @ Init timer(s)
        .global BBU_getI2C        @ Read from I2C bus
        .global BBU_putI2C        @ Write to  I2C bus
        .global BBU_usWait        @ BU_msWait
	.global BBU_msWait	  @ BU_usWait
	.global BBU_tickWait      @ Delay specified # of clock ticks (1 tick = 0.271267 uS)
	.global BBU_Get_Timer     @ Fetch a timer value	
	
@ Misc
	.equ     bbu_UASPR_offset,0x1C    @ UART Scratch Pad Register offset
	.equ     bbu_BBUART_PHYSICAL_BASE,0xD4017000      @ Address used for BBU UART port (STUART)

@
@*******************************************************************************
@
@       TIMER base address, register offsets, and bit defs 
@
@*******************************************************************************
@
       .equ     APPS_TIMERS_PHYSICAL_BASE,0xD4014000      @ Timers registers physical base
       .equ     COMS_TIMERS_PHYSICAL_BASE,0xD403A000      @ Timers registers physical base

       .equ     TMR_T0_M0_offset,0x0004  @ Timer 0 match 0 register
       .equ     TMR_CR0_offset,0x0028  @ Timer 0 count register
       .equ     TMR_CR1_offset,0x002C  @ Timer 0 count register
       .equ     TMR_CR2_offset,0x0030  @ Timer 0 count register
       .equ     TMR_CER_offset,0x0084  @ Timer Count Enable Register
       .equ     TMR_CVWR0_offset,0x00A4  @ Timer Counter 0 Write for Read
       .equ     TMR_CVWR1_offset,0x00A8  @ Timer Counter 1 Write for Read
       .equ     TMR_CVWR2_offset,0x00AC  @ Timer Counter 2 Write for Read
       .equ     TMR_WVR_offset,0x006C  @ Watchdog Timer Value
@
@       Register offsets
@
       .equ     bbu_TMR_CCR_offset,0x000   @ Offset to Timer Clock Control register
       .equ     bbu_TMR_T0_M0_offset,0x004   @ Offset to Timer 0 Match 0
       .equ     bbu_TMR_T0_M1_offset,0x008   @ Offset to Timer 0 Match 1
       .equ     bbu_TMR_T0_M2_offset,0x00C   @ Offset to Timer 0 Match 2
       .equ     bbu_TMR_T1_M0_offset,0x010   @ Offset to Timer 1 Match 0
       .equ     bbu_TMR_T1_M1_offset,0x014   @ Offset to Timer 1 Match 1
       .equ     bbu_TMR_T1_M2_offset,0x018   @ Offset to Timer 1 Match 2
       .equ     bbu_TMR_T2_M0_offset,0x01C   @ Offset to Timer 2 Match 0
       .equ     bbu_TMR_T2_M1_offset,0x020   @ Offset to Timer 2 Match 1
       .equ     bbu_TMR_T2_M2_offset,0x024   @ Offset to Timer 2 Match 2
       .equ     bbu_TMR_PLVR0_offset,0x04C   @ Offset to Timer PreLoad Value Reg 0
       .equ     bbu_TMR_PLVR1_offset,0x050   @ Offset to Timer PreLoad Value Reg 1
       .equ     bbu_TMR_PLVR2_offset,0x054   @ Offset to Timer PreLoad Value Reg 2
       .equ     bbu_TMR_PLCR0_offset,0x058   @ Offset to Timer PreLoad Control Reg 0
       .equ     bbu_TMR_PLCR1_offset,0x05C   @ Offset to Timer PreLoad Control Reg 1
       .equ     bbu_TMR_PLCR2_offset,0x060   @ Offset to Timer PreLoad Control Reg 2
       .equ     bbu_TMR_WMER_offset,0x064   @ Offset to Watchdog Match Enable Reg
       .equ     bbu_TMR_ICR0_offset,0x074   @ Offset to Timer Interrupt clear Reg 0
       .equ     bbu_TMR_ICR1_offset,0x078   @ Offset to Timer Interrupt clear Reg 1
       .equ     bbu_TMR_ICR2_offset,0x07C   @ Offset to Timer Interrupt clear Reg 2
       .equ     bbu_TMR_CER_offset,0x084   @ Offset to Timer Count Enable register
       .equ     bbu_TMR_CMR_offset,0x088   @ Offset to Timer Count Mode register
       .equ     bbu_TMR_WFAR_offset,0x09C   @ Offset to Watchdog First Access Reg
       .equ     bbu_TMR_WSAR_offset,0x0A0   @ Offset to Watchdog Second Access Reg

       .equ     APB1_CLOCK_UNIT_PHYSICAL_BASE,0xD4015000      @ APB1 Clock unit registers base address
       .equ     APB2_CLOCK_UNIT_PHYSICAL_BASE,0xD4015800      @ APB2 Clock unit registers base address

       .equ     APBC_CPK_CLK_RST_offset,0x30            @ Clock/Reset Control Register for Keypad Controller
       .equ     APBC_TIMERS_CLK_RST_offset,0x34            @ Clock/Reset Control Register
       .equ     ISCCR0_offset,0x40            @ I2S Clock Generation Control Register 0
       .equ     ISCCR1_offset,0x44            @ I2S Clock Generation Control Register 1
       .equ     APBCLK,0x1             @ Keypad Controller APB Bus Clock enable/disable
       .equ     FNCLK,0x2             @ Keypad Controller Functional Clock enable/disable

@
@*******************************************************************************
@
@ I2C BUS INTERFACE UNIT base address and register offsets from the base address
@ 
@**************************************************************
  	.equ     bbu_TWSI_PHYSICAL_BASE,0xD4011000
  	.equ     bbu_PI2C_PHYSICAL_BASE,0xD4025000

        .equ     APBC_TWSI_CLK_RST_offset,0x002C  @ Clock/Reset Control register for TWSI

     	.equ     bbu_I2C_IMBR_offset,0X00    @ I2C Bus Monitor Register
     	.equ     bbu_I2C_IDBR_offset,0X08    @ I2C Data Buffer Register
      	.equ     bbu_I2C_ICR_offset,0X10    @ I2C Control Register
        .equ     TWSI_SR_offset,0X18    @ I2C Status Register
     	.equ     bbu_I2C_ISAR_offset,0X20    @ I2C Slave Address Register
     	.equ     bbu_I2C_ICCR_offset,0x28    @ Clock Count Register

        .equ     bbu_ICR_UR,0x4000  @ Unit Reset bit
        .equ     bbu_ICR_IUE,0X0040  @ ICR I2C Unit enable bit
        .equ     bbu_ICR_SCLE,0X0020  @ ICR I2C SCL Enable bit
        .equ     bbu_ICR_TB,0X0008  @ ICR Transfer Byte bit
        .equ     bbu_ICR_ACKNAK,0x0004  @ ICR ACK bit
        .equ     bbu_ICR_STOP,0x0002  @ ICR STOP bit
        .equ     bbu_ICR_START,0X0001  @ ICR START bit
        .equ     bbu_ISR_IRF,0x0080  @ ISR Receive full bit
        .equ     bbu_ISR_ITE,0x0040  @ ISR Transmit empty bit
        .equ     bbu_PI2C_EN,0x0040  @ Power I2C enable bit
        .equ     bbu_ISR_BED,0x0400  @ Bus Error Detect bit

        .equ     BBU_I2C_TimeOut,0x2000  @ I2C bus timeout loop counter value

      	.equ     bbu_PMRCREGS_PHYSICAL_BASE,0x40F50000      @ Power manager base address

        .equ     bbu_PCFR_offset,0x00C   @ Offset to Power Manager general config register
       	.equ     bbu_PI2DBR_offset,0x188   @ Power I2C Data Buffer Register
        .equ     bbu_PI2CR_offset,0x190   @ Power I2C Control Register
        .equ     bbu_PI2SR_offset,0x198   @ Power I2C Status Register
       	.equ     bbu_PI2SAR_offset,0x1A0   @ Power I2C Slave Address Register

        @NOT SUPPORTED: AREA  "|text|,CODE,READONLY"
@
@*********************************************************************************
@
@       ****************
@       *              * 
@       * BBU_I2C_Init * Subroutine
@       *              *
@       ****************
@
@       This subroutine is used to initialize (and/or reset) the regular or the
@       power I2C bus
@
@ PARAMETER PASSING:
@
@       Subroutine looks at the first BBU command switch and will init the power
@       I2C bus if a "P" is found. Otherwise the regular I2C bus is initialized.
@        
@

@NOT SUPPORTED: BBU_I2C_Init:    FUNCTION
BBU_I2C_Init:
        stmfd   sp!,    {r0-r12, lr}     @ Save used registers
@
@       Get command switch setting
@
@        ldr     r3,     =BBU_CMD_SW     @ Base address for user command switches (KV)
@        ldrb    r4,     [r3]            @ Load first switch value in r4 (KV)
        mov    r4,     #0            @ Load first switch value in r4
@
@       Enable I2C controller clock
@
        ldr     r3,     =APB1_CLOCK_UNIT_PHYSICAL_BASE  @ Clock Register base
        mov     r2,     #4
        cmp     r4,     #'P'                            @ Enable Power I2C bus?
        strne   r2,     [r3, #APBC_TWSI_CLK_RST_offset] @ NO  - Enable I2C controller clock
        streq   r2,     [r3, #0x6C]                     @ YES - Enable Power I2C controller clock
@
@       Reset the selected controller
@
        mov     r2,     #7                              @ Set bit 2 (reset bit)
        strne   r2,     [r3, #APBC_TWSI_CLK_RST_offset] @ Enable I2C controller clock
        streq   r2,     [r3, #0x6C]                     @ Enable Power I2C controller clock

        mov     r0,     #5      @ Set up for delay
        bl      BBU_usWait      @ delay

        mov     r2,     #3              @ Clear reset bit
        cmp     r4,     #'P'                            @ Enable Power I2C bus?
        strne   r2,     [r3, #APBC_TWSI_CLK_RST_offset] @ NO  - Enable I2C controller clock
        streq   r2,     [r3, #0x6C]                     @ YES - Enable Power I2C controller clock

        mov     r0,     #20     @ Set up for delay
        bl      BBU_usWait      @ delay
@
@       Load the base address for the I2C bus selected by the user. The regular I2C
@       bus is the default. If the user used /P for the first command switch, the
@       Power I2C bus is selected instead.
@
        cmp     r4,     #'P'                    @ Enable Power I2C bus?
        ldrne   r3,     =bbu_TWSI_PHYSICAL_BASE @ NO - Load TWSI controller base address
        ldreq   r3,     =bbu_PI2C_PHYSICAL_BASE @ YES - Load Power I2C base address
@
@       Reset the controller
@
        mov     r2,     #bbu_ICR_UR                             @ Set the unit reset bit
        orr     r2,     r2,     #(bbu_ICR_IUE | bbu_ICR_SCLE)   @ OR in the IUE and SCLE bits
        str     r2,     [r3, #bbu_I2C_ICR_offset]               @ Reset the unit
@
@       Short delay with reset bit set, then clear the bit.
@
        mov     r1,     #0x10                           @ Loop count
F100:      subs    r1,     r1,     #1                      @ decrement loop count
        bne     F100                                    @ loop until done
        bic     r2,     r2,     #bbu_ICR_UR             @ Clear the unit reset bit  
        str     r2,     [r3, #bbu_I2C_ICR_offset]       @ and write the TWSI Control Register
@
@       Give the controller some time to insure the reset is done.
@
        mov     r0,     #50     @ Set up for delay
        bl      BBU_usWait      @ delay
       
        mov     r1,     #0                              @ Set host controller slave address
        str     r1,     [r3, #bbu_I2C_ISAR_offset]      @ Set host slave address register
        str     r1,     [r3, #bbu_I2C_ICR_offset]       @ Clear interrupts in ICR

        ldmfd   sp!,    {r0-r12, pc}     @ Return to caller
@NOT SUPPORTED:         ENDFUNC
@
@*********************************************************************************
@
@       **************
@       *            * 
@       * BBU_getI2C * Subroutine
@       *            *
@       **************
@
@ This subroutine is used to read data from the Regular or Power I2C bus
@
@ PARAMETER PASSING:
@
@       r0 = address in target device where data is to be read from (preserved)
@       
@       NOTE: If read address equals 0x100, the code just returns after the device
@             address is sent. This can be used to discover if an address is used
@             without reading or writing any data to the device.
@
@             If read address equals 0x200, the code will read data back from
@             certain types of GPIO expanders used on some platforms.
@
@       POWER I2C BUS: This routine checks to see if the user entered a
@       /P= switch for the register to read from to deturmine if the read
@       is to occour from the regular I2C bus or the Power I2C bus.
@
@ RETURNED VALUES:
@
@       r1 = data read from the register pointed to by r0
@       r2 = non zero if device was detected
@       r2 = zero if device not detected (usually a time out issue)  
@
@NOT SUPPORTED: BBU_getI2C:     FUNCTION
BBU_getI2C:
        stmfd   sp!,    {r3-r12, lr}     @ Save used registers
        mov     r7,     r0              @ Save off r0 in r7
@
@       Load the base address for the I2C bus selected by the user. The regular I2C
@       bus is the default. If the user used /P for the first command switch, the
@       Power I2C bus is selected instead.
@
@        ldr     r4,     =BBU_CMD_SW     @ Base address for user command switches (KV)
@        ldrb    r5,     [r4]            @ Get first switch (KV)
	mov	r5,	#0
        cmp     r5,     #'P'                    @ Select the Power I2C bus?
        ldrne   r4,     =bbu_TWSI_PHYSICAL_BASE @ NO - Load TWSI controller base address
        ldreq   r4,     =bbu_PI2C_PHYSICAL_BASE @ YES - Load Power I2C base address
@
@       Get the slave's address (saved in UART scratch pad register)
@
        ldr    r5,  =bbu_BBUART_PHYSICAL_BASE       @ Fetch base address of the BBUART
        ldrb   r6,  [r5, #bbu_UASPR_offset]         @ Get contents of the scratch pad register
        str    r6,  [r4, #bbu_I2C_IDBR_offset]      @ Load Data Buffer Register
@
@       Send 1st byte. BBU uses poling with a loop timeout to monitor when the
@       byte has been sent or there is a controller timeout.
@
        mov     r3,     #(bbu_ICR_IUE | bbu_ICR_SCLE)           @ Set the IUE and SCLE bits
        orr     r3,     r3,     #(bbu_ICR_TB | bbu_ICR_START)   @ Set transfer and start bits
        str     r3,     [r4, #bbu_I2C_ICR_offset]               @ This will start the transfer
        mov     r2,     #BBU_I2C_TimeOut                @ Set up time out value
F200:       ldr     r3,     [r4, #bbu_I2C_ICR_offset]       @ Get Control register contents
        subs    r2,     r2,     #1                      @ Decrement time out value
        beq     F212                            @ Exit path if timed out
        ands    r3,     r3,     #bbu_ICR_TB     @ Has the byte been sent yet?
        bne     F200                             @ No - still being transmitted
@
@       Check I2C bus status
@
        ldr     r3,     [r4, #TWSI_SR_offset]   @ Get status register contents
        ands    r3,     r3,     #bbu_ISR_BED    @ Get status of Bus Error bit
        moveq   r2,     #0xFF                   @ Insure non-zero time out if no error
        movne   r2,     #0      @ Error - assume no ACK from device
        bne     F212            @ ...and take the exit path
        cmp     r7,     #0x100  @ Special case - Just seeing if device is there?
        beq     F212            @ Yes - just exit
        cmp     r7,     #0x200  @ Special case - for reading from certain GPIO expanders?
        beq     F205             @ Yes - jump to this code
@
@       The delay is to make it a lot easier to decode transactions on the
@       bus using a logic analyzer or scope.
@
        mov     r0,     #30     @ Set up for delay
        bl      BBU_usWait      @ delay
@
@       Set up and then 2nd byte (Register Address)
@
        str     r7,     [r4, #bbu_I2C_IDBR_offset]              @ Set register address in the IDBR
        mov     r3,     #(bbu_ICR_TB | bbu_ICR_IUE | bbu_ICR_SCLE | bbu_ICR_STOP)
        str     r3,     [r4, #bbu_I2C_ICR_offset]               @ Set TB bit to start transfer

        mov     r2,     #BBU_I2C_TimeOut                @ Set up time out value
F204:       ldr     r3,     [r4, #bbu_I2C_ICR_offset]       @ Get control register contents
        subs    r2,     r2,     #1                      @ Decrement time out value
        beq     F212                                    @ Exit path if timed out
        ands    r3,     r3,  #bbu_ICR_TB                @ Was the byte sent yet?
        bne     F204                                     @ No - still being transmitted
@
@       Short delay (again, to make it easy to scope/decode the signals)
@
F205:       mov     r0,     #30     @ Set up for delay
        bl      BBU_usWait      @ delay
@
@       Set up and then send 3rd byte - Slave read address
@
        orr    r6,  r6,  #1                               @ Turn the address into a slave address
        str    r6,  [r4, #bbu_I2C_IDBR_offset]            @ Load Data Buffer Register
        mov    r3,  #(bbu_ICR_TB | bbu_ICR_IUE | bbu_ICR_SCLE | bbu_ICR_START)
        str    r3,  [r4, #bbu_I2C_ICR_offset]             @ Set TB bit to start transfer

        mov     r2,     #BBU_I2C_TimeOut        @ Set up time out value
F208:       ldr    r3,  [r4, #bbu_I2C_ICR_offset]   @ Get control register contents
        subs    r2,     r2,     #1              @ Decrement counter
        beq     F212                            @ Exit if zero
        ands   r3,  r3,  #bbu_ICR_TB            @ Was the byte sent yet?
        bne    F208                              @ No - still being transmitted
        str    r3,  [r4, #TWSI_SR_offset]       @ Write the ITE & IRF bits to clear them (sticky)
@
@       Delay (for scope analysis of I2C bus)
@
F209:       mov     r0,     #30     @ Set up for delay
        bl      BBU_usWait      @ delay
@
@       Send STOP signal (this also sends the clock signal needed to fetch the data)
@
        mov    r3,  #(bbu_ICR_TB | bbu_ICR_IUE | bbu_ICR_SCLE | bbu_ICR_STOP | bbu_ICR_ACKNAK)
        str    r3,  [r4, #bbu_I2C_ICR_offset]            @ Set TB bit to start transfer
        mov    r2,  #BBU_I2C_TimeOut                     @ Set up time out value
@
@       Loop to wait for data transfer from slave device
@
        mov     r2,     #BBU_I2C_TimeOut                @ Set up time out value
F210:      ldr     r3,     [r4, #bbu_I2C_ICR_offset]       @ Get control register contents
        subs    r2,     r2,     #1                      @ Decrement time out counter
        moveq   r2,     #1                              @ Reset to 1 if this transfer timed out
        beq     F212                                    @ Exit path if timed out
        ands    r3,     r3,     #bbu_ICR_TB             @ Was the byte received yet?
        bne     F210                                    @ no - keep looping
@
@       Some processors appear to need a little extra delay before reading the
@       the data so the next two lines of code may (or may not) be required.
@
@11      mov     r0,     #30     ; Set up for delay
@        bl      BBU_usWait      ; delay

        ldr     r1,     [r4, #bbu_I2C_IDBR_offset]      @ Fetch data byte from the IDBR
F212:      ldr     r3,     [r4, #TWSI_SR_offset]           @ Fetch status register
        str     r3,     [r4, #TWSI_SR_offset]           @ Clear any sticky bits
        ldmfd   sp!,    {r3-r12, pc}     @ Return to caller

@NOT SUPPORTED:         ENDFUNC
@
@*********************************************************************************
@
@       **************
@       *            * 
@       * BBU_putI2C * Subroutine
@       *            *
@       **************
@
@ This subroutine is used to write data to the Regular or Power I2C bus
@
@ PARAMETER PASSING:
@
@       r0 = address in target device where data is to be sent
@       r1 = data to be loaded into the register pointed to by r0
@
@ RETURNED VALUE:
@
@       r2 = non zero if no I2C bus time out
@       r2 = zero if I2C bus time out 
@
@NOT SUPPORTED: BBU_putI2C:      FUNCTION
BBU_putI2C:
        stmfd   sp!,    {r3-r12, lr}     @ Save used registers
        mov     r7,     r0              @ Save a copy of the slave's register
@
@       Load the base address for the I2C bus selected by the user. The regular I2C
@       bus is the default. If the user used /P for the first command switch, the
@       Power I2C bus is selected.
@
@        ldr     r4,     =BBU_CMD_SW     @ Base address for user command switches (KV)
@        ldrb    r5,     [r4]            @ Get first switch (KV)
        mov    r5,     #0            @ Get first switch (KV)
        cmp     r5,     #'P'                    @ Select the Power I2C bus?
        ldrne   r3,     =bbu_TWSI_PHYSICAL_BASE @ NO - Load TWSI controller base address
        ldreq   r3,     =bbu_PI2C_PHYSICAL_BASE @ YES - Load Power I2C base address

        mov     r2,     #0                              @ Set host controller slave address
        str     r2,     [r3, #bbu_I2C_ISAR_offset]      @ Set slave address register
        str     r2,     [r3, #bbu_I2C_ICR_offset]       @ Clear interrupts in ICR
        mov     r4,     #(bbu_ICR_IUE | bbu_ICR_SCLE)   @ Set IUE and SCLE bits
        str     r4,     [r3, #bbu_I2C_ICR_offset]       @ Enable the I2C in ICR
@
@       Get the slave's address
@
        ldr    r5,  =bbu_BBUART_PHYSICAL_BASE       @ Fetch base address of BBUART
        ldrb   r6,  [r5, #bbu_UASPR_offset]         @ Get contents of the scratch pad register
        str    r6,  [r3, #bbu_I2C_IDBR_offset]      @ Load Data Buffer Register
@
@       Send 1st byte
@
        orr     r4,     r4,  #(bbu_ICR_TB | bbu_ICR_START)
        str     r4,     [r3, #bbu_I2C_ICR_offset]       @ Set TB and START bits (in addition to IUE & SCLE)
        mov     r2,     #BBU_I2C_TimeOut                @ Set up time out loop

F302:       ldr     r4,     [r3, #TWSI_SR_offset]           @ Get status register contents
        subs    r2,     r2,     #1                      @ Decrement time out count
        beq     F308                                     @ Timed out - return to caller
        ands    r4,     r4,     #bbu_ISR_ITE            @ Was the byte sent yet?
        beq     F302                                     @ No - still being transmitted
        str     r4,     [r3, #TWSI_SR_offset]           @ Write the ITE bit to clear it (sticky)
@
@       Set up and then send 2nd byte (ADDRESS)
@
        mov     r0,     #100    @ Set up for delay
        bl      BBU_usWait      @ delay

        str    r7,  [r3, #bbu_I2C_IDBR_offset]            @ Set register address in the IDBR
        mov    r4,  #(bbu_ICR_TB | bbu_ICR_IUE | bbu_ICR_SCLE)
        str    r4,  [r3, #bbu_I2C_ICR_offset]             @ Set TB bit to start transfer
        mov    r2,  #BBU_I2C_TimeOut                      @ Set up time out value

F304:       ldr     r4,  [r3, #TWSI_SR_offset]      @ Get status register contents
        subs    r2,     r2,     #1              @ Decrement time out value
        beq     F308                             @ Return if there was a time out
        ands    r4,     r4,     #bbu_ISR_ITE    @ Was the byte sent yet?
        beq     F304                             @ No - still being transmitted
        str     r4,  [r3, #TWSI_SR_offset]      @ Write the ITE bit to clear it (sticky)
@
@       Set up and then send 3rd byte (DATA) and STOP signal
@
        mov     r0,     #100    @ Set up for delay
        bl      BBU_usWait      @ delay

        str     r1,     [r3, #bbu_I2C_IDBR_offset]      @ Place data byte into the IDBR
        mov     r4,     #(bbu_ICR_TB | bbu_ICR_IUE | bbu_ICR_SCLE | bbu_ICR_STOP)
        str     r4,     [r3, #bbu_I2C_ICR_offset]       @ Set TB bit to start transfer

F306:       ldr     r4,     [r3, #TWSI_SR_offset]           @ Get status register contents
        ands    r4,     r4,  #bbu_ISR_ITE               @ Was the byte sent yet?
        beq     F306                                     @ No - still being transmitted
        str     r4,     [r3, #TWSI_SR_offset]           @ Write the ITE bit to clear it (sticky)

F308:       ldmfd   sp!,    {r3-r12, pc}     @ Return to caller

@NOT SUPPORTED:         ENDFUNC
@*******************************************************************************
@
@       **************
@       *            *
@       * BBU_msWait * Subroutine
@       *            *
@       **************
@
@ This subroutine is used to generate a delay of r0 milliseconds
@
@ PARAMETER PASSING:
@
@       r0 = contains the number of milliseconds to delay (preserved).
@            MAXIMUM DELAY = 1,321,528 ms (1,321 seconds or 22 min & 1 second)
@
@NOT SUPPORTED: BBU_msWait:   FUNCTION
BBU_msWait:

        stmfd   sp!,    {r1-r12, lr}     @ Save r1 registers & link reg on stack
@
@       NOTE: Timer 0 clock frequency is 3.250 MHz
@
        ldr     r1,     =3250           @ Load r1 with value to generate 1 ms delay
        mul     r0,     r0,     r1      @ Calculate the total number of clock ticks
        bl      BBU_tickWait            @ Call the tick wait routine
        ldmfd   sp!,    {r1-r12, pc}     @ Restore used registers and return to caller

@NOT SUPPORTED:         ENDFUNC
@
@*******************************************************************************
@
@       **************
@       *            *
@       * BBU_usWait * Subroutine
@       *            *
@       **************
@
@ This subroutine is used to generate a delay of r0 microseconds
@
@ PARAMETER PASSING:
@
@       r0 = contains the number of microseconds to delay (preserved).
@            (Every 4 uS delay will be 13 ticks using a 3.25 MHz clock)
@
@NOT SUPPORTED: BBU_usWait:   FUNCTION
BBU_usWait:

        stmfd   sp!,    {r1-r12, lr}    @ Save registers & link reg on stack
@
@       NOTE: Clock frequency is 3.250 MHz
@
        bic     r2,     r0,     #3      @ Clear bits 1:0 from what the user requested
        mov     r2,     r2,     LSR #2  @ r2 = number of 4 us increments
        mov     r1,     #13             @ Multiply by 13 to get tick count
        mul     r2,     r2,     r1      @ r2 = tick count for 4 uS increments
        and     r0,     r0,     #3      @ Get the value of bits 1:0
        mov     r1,     #3              @ Multiply by 3
        mul     r0,     r0,     r1      @ Click ticks for sub 4 uS increment
        add     r0,     r0,     r2      @ Add all the clock ticks together
        bl      BBU_tickWait            @ Call the tick wait routine
        ldmfd   sp!,    {r1-r12, pc}     @ Restore used registers and return to caller

@NOT SUPPORTED:         ENDFUNC
@
@*******************************************************************************
@
@       ****************
@       *              *
@       * BBU_tickWait * Subroutine
@       *              *
@       ****************
@
@ This subroutine is used to generate a delay of r0 clock ticks where
@       1 tick = 0.30769 uS
@
@ PARAMETER PASSING:
@
@       r0 = contains the number of ticks to delay - (preserved)
@            MAXIMUM DELAY = 1,321,528 ms (1,321 seconds or 22 min & 1 second)
@
@
@NOT SUPPORTED: BBU_tickWait:   FUNCTION
BBU_tickWait:
@
@ (NOTE: Clock tick frequency is 3.250 MHz)
@
        stmfd   sp!,    {r1-r12, lr}     @ Save registers and link on stack

        mov     r1,     r0              @ Save user parameter in r1
        mov     r0,     #0              @ Request timer 0 value
        bl      BBU_Get_Timer           @ Get the value in timer 0
        adds    r2,     r0,     r1      @ Calculate ending tick count
        bcs     F416                    @ Go here if carry bit was set

F414:      mov     r0,     #0              @ Request timer 0 value
        bl      BBU_Get_Timer           @ Get the value in timer 0
        cmp     r0,     r2              @ compare to tick count value
        bmi     F414                    @ loop until OS timer value equals time out value
        ldmfd   sp!,    {r1-r12, pc}     @ Restore registers, & return to caller

F416:      mov     r0,     #0              @ Request timer 0 value
        bl      BBU_Get_Timer           @ Get the value in timer 0
        cmp     r2,     r0              @ Compare ending value to actual value
        bmi     F416                    @ loop until OS timer value equals time out value
        b       F414                    @ OS timer flipped - change compare loops

@NOT SUPPORTED:         ENDFUNC
@*********************************************************************************
@
@       ******************
@       *                *
@       * BBU_Timer_init * Subroutine
@       *                *
@       ******************
@
@ This subroutine is used to fetch a timer value
@
@ PARAMETER PASSING:
@
@       INPUT  - r0 = 0 for timer 0 value ( 3.250 MHz clock)
@                r0 = 1 for timer 1 value (32.768 KHz clock)
@                r0 = 2 for timer 2 value ( 1.000 KHz clock)
@                r0 = any other value returns watchdog timer value
@
@       OUTPUT - r0 = value curently in the requested timer
@
@NOT SUPPORTED: BBU_Get_Timer:  FUNCTION
BBU_Get_Timer:
        stmfd   sp!,    {r1-r12, lr}     @ Save registers and return link on stack

        ldr     r1,     =APPS_TIMERS_PHYSICAL_BASE      @ documented base
        mov     r2,     #1                              @ Load non-zero value

        cmp     r0,     #0                      @ Timer #0?
        streq   r2,     [r1, #TMR_CVWR0_offset] @ Yes - Write to latch value
        mov     r0,     r0                      @ NOP instruction
        ldreq   r0,     [r1, #TMR_CR0_offset]   @ Yes - get timer 0
        beq     F529                           @ ...and exit

        cmp     r0,     #1                      @ Get value in timer #1?
        streq   r2,     [r1, #TMR_CVWR1_offset] @ Yes - Write to latch value
        mov     r0,     r0                      @ NOP instruction
        ldreq   r0,     [r1, #TMR_CR1_offset]   @ Yes - get timer 1
        beq     F529                           @ ...and exit

        cmp     r0,     #2                      @ Get value in timer #2?
        streq   r2,     [r1, #TMR_CVWR2_offset] @ Yes - Write to latch value
        mov     r0,     r0                      @ NOP instruction
        ldreq   r0,     [r1, #TMR_CR2_offset]   @ Yes - get timer 2
        beq     F529                           @ ...and exit
@
@       If none of the above timers were selected - fetch the watchdog value
@
        ldr     r0,     [r1, #TMR_WVR_offset]   @ Get watchdog timer value

F529:     ldmfd   sp!,    {r1-r12, pc}     @ Restore registers and return to caller
@NOT SUPPORTED:         ENDFUNC
@
@*********************************************************************************
@
@       ******************
@       *                * 
@       * BBU_Timer_init * Subroutine
@       *                *
@       ******************
@
@ This subroutine is used to init the AP timers for use
@
@ PARAMETER PASSING:
@
@       INPUT  - No parameters
@       OUTPUT - No parameters
@
@       NOTE: APPS and COMM timers have to be set up the same way
@
@NOT SUPPORTED: BBU_Timer_init:  FUNCTION
BBU_Timer_init:
        stmfd   sp!,    {r0-r12, lr}     @ Save registers and return link on stack

        ldr     r0,     =APB1_CLOCK_UNIT_PHYSICAL_BASE          @ Documented base
        mov     r1,     #0x3                                    @ Set to turn on clocks
        ldr     r1,     [r0, #APBC_TIMERS_CLK_RST_offset]       @ Turn on clocks

        ldr     r0,     =APPS_TIMERS_PHYSICAL_BASE      @ documented base
        ldr     r3,     =APPS_TIMERS_PHYSICAL_BASE      @ documented base
@
@       Disable the Counters
@
        mov     r1,     #0                              @ Disable the counters
F600:     str     r1,     [r0, #bbu_TMR_CER_offset]       @ Write to disable
        ldr     r2,     [r0, #bbu_TMR_CER_offset]       @ Read status back
        str     r1,     [r3, #bbu_TMR_CER_offset]       @ Write to disable
        ldr     r2,     [r3, #bbu_TMR_CER_offset]       @ Read status back
        tst     r2,     #0x7                            @ Check bits 2:0
        bne     F600                   @ Loop until these bits are clear
@
@       Set timer 0 for 3.25 MHz clock
@       Set timer 1 for 32.768 KHz clock
@       Set timer 2 for 1.0 KHz clock
@
        mov     r1,     #0x24                           @ Select clock sources
        str     r1,     [r0, #bbu_TMR_CCR_offset]       @ Write Clock Control
        ldr     r2,     [r0, #bbu_TMR_CCR_offset]       @ Read back;
        str     r1,     [r3, #bbu_TMR_CCR_offset]       @ Write Clock Control
        ldr     r2,     [r3, #bbu_TMR_CCR_offset]       @ Read back;
@        
@       Short "do nothing" delay loop to let things kick in
@
        mov     r2,     #200            @ Set up delay loop count
F602:     subs    r2,     r2,     #1      @ Decrement count
        bne     F602                   @ Loop until zero
@
@       Set to run in periodic mode (takes care of all 3 timers)
@
        mov     r1,     #0                              @ Value for Count Mode
        str     r1,     [r0, #bbu_TMR_CMR_offset]       @ Write Count Mode Reg
        ldr     r2,     [r0, #bbu_TMR_CMR_offset]       @ Read back
        str     r1,     [r3, #bbu_TMR_CMR_offset]       @ Write Count Mode Reg
        ldr     r2,     [r3, #bbu_TMR_CMR_offset]       @ Read back
@
@       ***** SET UP TIMER 0 ***** - Set up a preload value of zero
@
	mov	r1,     #0                      @ PreLoad value
	str	r1, [r0, #bbu_TMR_PLVR0_offset] @ Write to PreLoad register 0
	ldr	r2, [r0, #bbu_TMR_PLVR0_offset] @ Read back
	str	r1, [r3, #bbu_TMR_PLVR0_offset] @ Write to PreLoad register 0
	ldr	r2, [r3, #bbu_TMR_PLVR0_offset] @ Read back
@
@
@       ***** SET UP TIMER 1 ***** - Set up a preload value of zero
@
	mov	r1,     #0                      @ PreLoad value
	str	r1, [r0, #bbu_TMR_PLVR1_offset] @ Write to PreLoad register 1
	ldr	r2, [r0, #bbu_TMR_PLVR1_offset] @ Read back
	str	r1, [r3, #bbu_TMR_PLVR1_offset] @ Write to PreLoad register 1
	ldr	r2, [r3, #bbu_TMR_PLVR1_offset] @ Read back

@
@       ***** SET UP TIMER 2 ***** - Set up a preload value of zero
@
	mov	r1,     #0                      @ PreLoad value
	str	r1, [r0, #bbu_TMR_PLVR2_offset] @ Write to PreLoad register 2
	ldr	r2, [r0, #bbu_TMR_PLVR2_offset] @ Read back
	str	r1, [r3, #bbu_TMR_PLVR2_offset] @ Write to PreLoad register 2
	ldr	r2, [r3, #bbu_TMR_PLVR2_offset] @ Read back
@			
@       Program TMR_CCR (Timer Clock control Reg)
@       TMR_PLCR[0]:  Set up the preload control register:  Free run
@
        mov     r1,     #0                              @ Set up for "free run"
        str     r1,     [r0, #bbu_TMR_PLCR0_offset]     @ Set into "free run"
        ldr     r2,     [r0, #bbu_TMR_PLCR0_offset]     @ Read back
        str     r1,     [r3, #bbu_TMR_PLCR0_offset]     @ Set into "free run"
        ldr     r2,     [r3, #bbu_TMR_PLCR0_offset]     @ Read back
@			
@       Program TMR_CCR (Timer Clock control Reg)
@       TMR_PLCR[1]:  Set up the preload control register:  Free run
@
        mov     r1,     #0                              @ Set up for "free run"
        str     r1,     [r0, #bbu_TMR_PLCR1_offset]     @ Set into "free run"
        ldr     r2,     [r0, #bbu_TMR_PLCR1_offset]     @ Read back
        str     r1,     [r3, #bbu_TMR_PLCR1_offset]     @ Set into "free run"
        ldr     r2,     [r3, #bbu_TMR_PLCR1_offset]     @ Read back
@			
@       Program TMR_CCR (Timer Clock control Reg)
@       TMR_PLCR[2]:  Set up the preload control register:  Free run
@
        mov     r1,     #0                              @ Set up for "free run"
        str     r1,     [r0, #bbu_TMR_PLCR2_offset]     @ Set into "free run"
        ldr     r2,     [r0, #bbu_TMR_PLCR2_offset]     @ Read back
        str     r1,     [r3, #bbu_TMR_PLCR2_offset]     @ Set into "free run"
        ldr     r2,     [r3, #bbu_TMR_PLCR2_offset]     @ Read back
@
@       TMR_ICR[0]:  Clear any pending match interrupts (sticky bits)
@
        ldr     r1,     =0x7                            @ Write 1's to clear
        str     r1,     [r0, #bbu_TMR_ICR0_offset]      @ Clear the interrupts
        ldr     r2,     [r0, #bbu_TMR_ICR0_offset]      @ Read back
        str     r1,     [r3, #bbu_TMR_ICR0_offset]      @ Clear the interrupts
        ldr     r2,     [r3, #bbu_TMR_ICR0_offset]      @ Read back
@
@       TMR_T0_M0:  Force something in the match, assume 0x100 as the init
@       value of the counter.
@
        ldr     r1,     =0x00000100                     @ Dummy value to load 
        str     r1,     [r0, #bbu_TMR_T0_M0_offset]     @ Load this value
        ldr     r2,     [r0, #bbu_TMR_T0_M0_offset]     @ Read back
        str     r1,     [r3, #bbu_TMR_T0_M0_offset]     @ Load this value
        ldr     r2,     [r3, #bbu_TMR_T0_M0_offset]     @ Read back
@
@       TMR_T0_M1
@
        str     r1,     [r0, #bbu_TMR_T0_M1_offset]     @ Load this value
        ldr     r2,     [r0, #bbu_TMR_T0_M1_offset]     @ Read back
        str     r1,     [r3, #bbu_TMR_T0_M1_offset]     @ Load this value
        ldr     r2,     [r3, #bbu_TMR_T0_M1_offset]     @ Read back
@
@       TMR_T0_M2
@
        str     r1,     [r0, #bbu_TMR_T0_M2_offset]     @ Load this value
        ldr     r2,     [r0, #bbu_TMR_T0_M2_offset]     @ Read back
        str     r1,     [r3, #bbu_TMR_T0_M2_offset]     @ Load this value
        ldr     r2,     [r3, #bbu_TMR_T0_M2_offset]     @ Read back
@
@       Short "do nothing" delay loop to let things kick in
@
        mov     r2,     #F600            @ Set up delay loop count
F604:     subs    r2,     r2,     #1      @ Decrement count
        bne     F604                   @ Loop until zero
@
@       Enable the Counters
@
        mov     r1,     #7                              @ Enable the counters
        str     r1,     [r0, #bbu_TMR_CER_offset]       @ Write to enable
        ldr     r2,     [r0, #bbu_TMR_CER_offset]       @ Read status back
@
@       Return to caller
@
        ldmfd   sp!,    {r0-r12, pc}     @ Restore registers and return to caller
@NOT SUPPORTED:         ENDFUNC

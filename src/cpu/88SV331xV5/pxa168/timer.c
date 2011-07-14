/*
 * (C) Copyright 2003
 * Texas Instruments <www.ti.com>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002-2004
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
 *
 * (C) Copyright 2004
 * Philippe Robin, ARM Ltd. <philippe.robin@arm.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <asm/io.h>
#include <common.h>
#include <asm/arch/interrupt.h>
#include <asm/arch/regs-rtc.h>
#include <asm/arch/rtc.h>


#define READ_TIMER ({volatile int loop=100; \
	         	*(volatile ulong *)(CONFIG_SYS_TIMERBASE+0xa4) = 0x1; \
			while(loop--);\
	       		*(volatile ulong *)(CONFIG_SYS_TIMERBASE+0xa4);})

#define TIMER_LOAD_VAL 0xffffffff

static ulong timestamp;
static ulong lastdec;

#ifdef CONFIG_USE_IRQ
static void timer_isr(void *data)
{
	printf("timer_isr():  called for IRQ %d\n", (int)data);
	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 0x74) = 0x1;
}
#ifdef CONFIG_TIMER2
static void timer2_isr(void *data)
{
	printf("timer2_isr():  called for IRQ %d\n", (int)data);
	*(volatile ulong *)(CONFIG_TIMER2_BASE + 0x74) = 0x1;
}
#endif
#endif

int timer_init(void)
{
	volatile unsigned int i;
	*(volatile unsigned int *)0xD4015034 = 5;
	i = 0x10000;
	while(i --);

	*(volatile unsigned int *)0xD4015034 = 0x33; //KV - Choose 3.25 MHz Clock Frequency for OS Timer
	i = 0x10000;
	while(i --);

	/* Select clock 3.25 MHz */
	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 0) = 0x0; //KV
	/* Select match register */
	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 4) = 0xffffffff;
	/* Preload value. */
	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 0x4c) = 0;
	/* Preload control. */
	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 0x58) = 0x0;

	/* Enable counter 0.  */
	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 0x84) = 0x1;
	/* init the timestamp and lastdec value */
	reset_timer_masked();

#ifdef CONFIG_TIMER2
	*(volatile unsigned int *)0xD4015044 = 5;
	i = 0x10000;
	while(i --);

	*(volatile unsigned int *)0xD4015044 = 0x13; //Choose 32 KHz Clock Frequency
	i = 0x10000;
	while(i --);

	*(volatile ulong *)(CONFIG_TIMER2_BASE + 0) = 0x0; //KV
	/* Select match register */
	*(volatile ulong *)(CONFIG_TIMER2_BASE + 4) = 0xffffffff;
	/* Preload value. */
	*(volatile ulong *)(CONFIG_TIMER2_BASE + 0x4c) = 0;
	/* Preload control. */
	*(volatile ulong *)(CONFIG_TIMER2_BASE + 0x58) = 0x0;

	/* Enable counter 0.  */
	*(volatile ulong *)(CONFIG_TIMER2_BASE + 0x84) = 0x1;
#endif

#ifdef CONFIG_USE_IRQ
	/* enable timer interrupt */
	INT_Enable(INT_AP_TMR1, IRQ_ROUTE, TIMER_PRIORITY);

	/* install interrupt handler for timer */
	ISR_Connect(INT_AP_TMR1, timer_isr, (void *)0);
#ifdef CONFIG_TIMER2
	/* enable timer interrupt */
	INT_Enable(INT_AP2_TMR1, IRQ_ROUTE, TIMER_PRIORITY);

	/* install interrupt handler for timer */
	ISR_Connect(INT_AP2_TMR1, timer2_isr, (void *)0);
#endif
#endif
	RTC_init();

	return 0;
}

void set_timer_match(ulong secs)
{
	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 0x4) =
		(READ_TIMER + secs * CONFIG_SYS_HZ);
	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 0x74) = 0x1;
	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 0x40) = 0x1;
}

#ifdef CONFIG_TIMER2
void set_timer2_match(ulong secs)
{
	unsigned int temp;
	temp = *(volatile ulong *)(CONFIG_TIMER2_BASE + 0x28);
	temp += secs * CONFIG_TIMER2_HZ;
	*(volatile ulong *)(CONFIG_TIMER2_BASE + 0x4) = temp;
	*(volatile ulong *)(CONFIG_TIMER2_BASE + 0x74) = 0x1;
	*(volatile ulong *)(CONFIG_TIMER2_BASE + 0x40) = 0x1;
}
#endif

void reset_timer_masked (void)
{
	/* reset time */
	lastdec = READ_TIMER;  /* capure current decrementer value time */
	timestamp = 0;         /* start "advancing" time stamp from 0 */
}

/*
 * timer without interrupts
 */

void reset_timer (void)
{
	reset_timer_masked ();
}

ulong get_timer (ulong base)
{
	return get_timer_masked () * 3 / 10000 - base;
}

void set_timer (ulong t)
{
	timestamp = t;
}

/* delay x useconds AND perserve advance timstamp value */
void udelay (unsigned long usec)
{
	ulong tmo, tmp;

	if(usec >= 1000){               /* if "big" number, spread normalization to seconds */
		tmo = usec / 1000;      /* start to normalize for usec to ticks per sec */
		tmo *= CONFIG_SYS_HZ;   /* find number of "ticks" to wait to achieve target */
		tmo /= 1000;            /* finish normalize. */
	}else{                          /* else small number, don't kill it prior to HZ multiply */
		tmo = usec * CONFIG_SYS_HZ;
		tmo /= (1000*1000);
	}

	/* tune tmo */
	tmo /= (CONFIG_SYS_HZ / 3000000);
	tmp = get_timer_masked ();	/* get current timestamp */
	if( (tmo + tmp + 1) < tmp ) 	/* if setting this fordward will roll time stamp */
		reset_timer_masked ();	/* reset "advancing" timestamp to 0, set lastdec value */
	else
		tmo += tmp;		/* else, set advancing stamp wake up time */

	while (get_timer_masked () < tmo)/* loop till event */
		/*NOP*/;
}

ulong get_timer_masked (void)
{
	ulong now = READ_TIMER;		/* current tick value */

	if (lastdec <= now) {		/* normal mode (non roll) */
		/* normal mode */
		timestamp += (now - lastdec); /* move stamp fordward with absoulte diff ticks */
	} else {			/* we have overflow of the count down timer */
		/* nts = ts + ld + (TLV - now)
		 * ts=old stamp, ld=time that passed before passing through -1
		 * (TLV-now) amount of time after passing though -1
		 * nts = new "advancing time stamp"...it could also roll and cause problems.
		 */
		timestamp += now + (TIMER_LOAD_VAL - lastdec);
	}
	lastdec = now;

	return timestamp;
}

/* waits specified delay value and resets timestamp */
void udelay_masked (unsigned long usec)
{
	ulong tmo;

	if(usec >= 1000){               /* if "big" number, spread normalization to seconds */
		tmo = usec / 1000;      /* start to normalize for usec to ticks per sec */
		tmo *= CONFIG_SYS_HZ;          /* find number of "ticks" to wait to achieve target */
	}else{                          /* else small number, don't kill it prior to HZ multiply */
		tmo = usec * CONFIG_SYS_HZ;
	}

	reset_timer_masked ();	/* set "advancing" timestamp to 0, set lastdec vaule */

	while (get_timer_masked () < tmo) /* wait for time stamp to overtake tick number.*/
		/*NOP*/;
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return READ_TIMER;
}


/******************************** rtc ***********************************/
void  RTC_set_raw_time(BU_U32 rawt)
{
	BU_REG_WRITE( RTC_CNR, rawt);
}

BU_U32 RTC_get_raw_time()
{
	return BU_REG_READ( RTC_CNR );
}

void RTC_enable_1hz_int()
{
	BU_REG_WRITE( RTC_SR, BU_REG_READ(RTC_SR) | RTC_SR_HZE);
}

void RTC_enable_alarm_int()
{
	BU_REG_WRITE( RTC_SR, BU_REG_READ(RTC_SR) | RTC_SR_ALE);
}

void RTC_disable_1hz_int()
{
	BU_REG_WRITE( RTC_SR, BU_REG_READ(RTC_SR) & ~RTC_SR_HZE);
}

void RTC_disable_alarm_int()
{
	BU_REG_WRITE( RTC_SR, BU_REG_READ(RTC_SR) & ~ RTC_SR_ALE);
}

void RTC_set_alarm( BU_U32 rm )
{
	BU_REG_WRITE( RTC_AR, rm );
	RTC_enable_alarm_int();
}

#ifdef CONFIG_USE_IRQ
void RTC_ISR(void *data)
{
	BU_U32	sr;
	static int sec = 0;

	sr = BU_REG_READ( RTC_SR );
	if ( sr & RTC_SR_ALE && sr & RTC_SR_AL )
	{
		printf("Alarm wakeup at %x!\n", BU_REG_READ(RTC_CNR) );

		BU_REG_WRITE( RTC_SR, ( sr & (RTC_SR_HZE|RTC_SR_ALE )) | RTC_SR_AL );
		BU_REG_WRITE( RTC_AR, 0 );
		return;
	}

	if ( sr & RTC_SR_HZE && sr & RTC_SR_HZ )
	{
		sec++;
		printf("%d", sec);
		BU_REG_WRITE( RTC_SR, ( sr & (RTC_SR_HZE|RTC_SR_ALE )) | RTC_SR_HZ );
		return;
	}

	// else PMU Wake up BUG: in the PMU
	printf("PMU Alarm wakeup at %x!\n", BU_REG_READ(RTC_CNR) );
	BU_REG_WRITE( RTC_SR, sr | (RTC_SR_AL|RTC_SR_HZ) );
	BU_REG_WRITE( RTC_AR, 0 );

	return;
}
#endif

#define APBC_RTC_CLK_RST (0xd4015028)

void RTC_init(void)
{
	volatile int i;
	BU_U32 reg;

	BU_REG_WRITE( APBC_RTC_CLK_RST, 0x5);
	reg=BU_REG_READ( APBC_RTC_CLK_RST);
	i=0x200;
	while(i--);

	BU_REG_WRITE( APBC_RTC_CLK_RST, 3);
	reg=BU_REG_READ( APBC_RTC_CLK_RST);
	i=0x300;
	while(i--);

	BU_REG_WRITE( APBC_RTC_CLK_RST, 0x83);
	reg=BU_REG_READ( APBC_RTC_CLK_RST);
	i=0x300;
	while(i--);

#ifdef CONFIG_USE_IRQ
	INT_Enable( INT_RTC_ALARM, IRQ_ROUTE, RTC_PRIORITY );
	ISR_Connect(INT_RTC_ALARM, RTC_ISR, 0);

	INT_Enable( INT_RTC_1HZ, IRQ_ROUTE, RTC_PRIORITY );
	ISR_Connect(INT_RTC_1HZ, RTC_ISR, 0);
#endif

	RTC_disable_alarm_int();
	RTC_disable_1hz_int();

	BU_REG_WRITE( RTC_CNR, 0 );
	BU_REG_WRITE( RTC_AR,  0 );
}

void rtc_alarm_to_pmu(int alarm_secs)
{
	BU_U32 raw_t;

	printf("Alarm setup for %d seconds\n", alarm_secs);

	raw_t = RTC_get_raw_time();
	raw_t += alarm_secs;
	RTC_set_alarm( raw_t );
}

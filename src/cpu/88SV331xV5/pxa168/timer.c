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

#define READ_TIMER ({volatile int loop=100; \
	         	*(volatile ulong *)(CONFIG_SYS_TIMERBASE+0xa4) = 0x1; \
			while(loop--);\
	       		*(volatile ulong *)(CONFIG_SYS_TIMERBASE+0xa4);})

#define TIMER_LOAD_VAL 0xffffffff

static ulong timestamp;
static ulong lastdec;

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
}

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


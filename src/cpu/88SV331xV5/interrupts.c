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

#include <common.h>
#include <arm926ejs.h>
#include <asm/arch/interrupt.h>

#ifdef CONFIG_INTEGRATOR

	/* Timer functionality supplied by Integrator board (AP or CP) */

#else

#ifdef CONFIG_USE_IRQ
struct _irq_handler {
	void *m_data;
	void (*m_func)( void *data);
};

static struct _irq_handler IRQ_HANDLER[N_IRQS];

static void default_isr(void *data)
{
	printf("default_isr():  called for IRQ %d\n", (int)data);
}

BU_U32 GetIntStatus_IRQ(void)
{
	return BU_REG_READ(ICU_MOHAWK_IRQ_SEL_INT_NUM);
}

void do_irq (struct pt_regs *pt_regs)
{
	BU_U32 int_num;

	/* read interrupt status */
	int_num = GetIntStatus_IRQ();

	/* checking the pending bit */
	if ( int_num &  ICU_MOHAWK_IRQ_SEL_INT_NUM_INT_PENDING ) {
		/* mask out the pending bit to get low 5 bit INT num */
		int_num &= ~ICU_MOHAWK_IRQ_SEL_INT_NUM_INT_PENDING;
		/* get ISR entry from table and call it */
		IRQ_HANDLER[int_num].m_func(IRQ_HANDLER[int_num].m_data);
	}
}

void INT_init(void)
{
	int i;

	BU_REG_WRITE(ICU_MOHAWK_GBL_IRQ_MSK,
		ICU_MOHAWK_GBL_IRQ_MSK_IRQ_MSK
		| ICU_MOHAWK_GBL_IRQ_MSK_FIQ_MSK);
	for (i = 0; i < N_IRQS; i++) {
		IRQ_HANDLER[i].m_data = (void *)i;
		IRQ_HANDLER[i].m_func = default_isr;
		BU_REG_WRITE(ICU_INT_0_TO_63_CONF+i*4,
			(BU_REG_READ(ICU_INT_0_TO_63_CONF+i*4)
			 & ~(ICU_INT_0_TO_63_CONF_MOHAWK_INT
			 |ICU_INT_0_TO_63_CONF_SEAGULL_INT)));
	}
}

void IRQ_Glb_Ena(void)
{
	unsigned int reg;

	reg = BU_REG_READ(ICU_MOHAWK_GBL_IRQ_MSK);
	reg &= (~ICU_MOHAWK_GBL_IRQ_MSK_IRQ_MSK);
	BU_REG_WRITE( ICU_MOHAWK_GBL_IRQ_MSK, reg);
}

void INT_Enable(BU_U32 int_num, BU_U32 irq_fiq, BU_U32 priority )
{
	volatile BU_U32 mask; /* do not touch the int route to other CPU */
	mask = (BU_REG_READ(ICU_INT_0_TO_63_CONF+int_num*4)
		& ICU_INT_0_TO_63_CONF_SEAGULL_INT);
	BU_REG_WRITE(ICU_INT_0_TO_63_CONF+int_num*4,
		ICU_INT_0_TO_63_CONF_MOHAWK_INT | (irq_fiq<<4) | priority | mask);
}

void INT_Disable(BU_U32 int_num )
{
	BU_REG_WRITE(ICU_INT_0_TO_63_CONF+int_num*4, 0);
}

void ISR_Connect(BU_U32 int_num, void (*m_func)( void *data), void *data)
{

	if(NULL == m_func)
		return;

	/* assign the user defined function*/
	IRQ_HANDLER[int_num].m_data = data;
	IRQ_HANDLER[int_num].m_func = m_func;
}

#endif

/* nothing really to do with interrupts, just starts up a counter. */
int interrupt_init (void)
{
	extern void timer_init(void);

#ifdef CONFIG_USE_IRQ
	/* install default interrupt handlers */
	INT_init();

	/* enable irq for AP */
	IRQ_Glb_Ena();

	/* enable interrupts on the CPU side */
	enable_interrupts();
#endif

	timer_init();

	return 0;
}

#endif /* CONFIG_INTEGRATOR */

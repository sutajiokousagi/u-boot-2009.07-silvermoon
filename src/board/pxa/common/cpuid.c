/*
 * cpuid check
 */
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <environment.h>
#include <linux/ctype.h>

#define __stringify_1(x)	#x
#define __stringify(x)		__stringify_1(x)

#define read_cpuid(reg)						\
	({							\
	 unsigned int __val;					\
	 asm("mrc	p15, 0, %0, c0, c0, " __stringify(reg)	\
		 : "=r" (__val)					\
		 :						\
		 : "cc");					\
	 __val;							\
	 })

int do_cpuid(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	printf("cpu id:		%x\n", read_cpuid(0));
	printf("cache type:	%x\n", read_cpuid(1));
	printf("chip id:	%x\n", *(volatile unsigned int *)0xd4282c00);
	return 0;
}

U_BOOT_CMD(
		cpuid,	6,	1,	do_cpuid,
		"cpuid	- read cpu id\n",
		" - read cpu id\n"
	  );

int do_setid(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	*(volatile unsigned int *)0xD4282c08 = *(volatile unsigned int *)0xD4282c08 | 0x100;
	return 0;
}

U_BOOT_CMD(
		setid,	6,	1,	do_setid,
		"setid	- set SEL_MRVL_ID bit in MOHAWK_CPU_CONF register\n",
		" - set SEL_MRVL_ID bit in MOHAWK_CPU_CONF register\n"
	  );

int do_unsetid(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	*(volatile unsigned int *)0xD4282c08 = *(volatile unsigned int *)0xD4282c08 & ~0x100;
	return 0;
}

U_BOOT_CMD(
		unsetid,	6,	1,	do_unsetid,
		"unsetid - unset SEL_MRVL_ID bit in MOHAWK_CPU_CONF register\n",
		" - unset SEL_MRVL_ID bit in MOHAWK_CPU_CONF register\n"
	  );



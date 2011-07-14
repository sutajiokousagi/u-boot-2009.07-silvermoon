#ifndef __PXA_GPIO_H
#define __PXA_GPIO_H


#define GPIO_REGS_VIRT 	(0xD4019000)

#define BANK_OFF(n)     (((n) < 3) ? (n) << 2 : 0x100 + (((n) - 3) << 2))
#define GPIO_REG(x)     (*((volatile u32 *)(GPIO_REGS_VIRT + (x))))

#define NR_BUILTIN_GPIO (128)

#define GPIO_bit(gpio)  (1 << ((gpio) & 0x1f))

#define GPLR(x)         GPIO_REG(BANK_OFF((x) >> 5) + 0x00)
#define GPDR(x)         GPIO_REG(BANK_OFF((x) >> 5) + 0x0c)
#define GPSR(x)         GPIO_REG(BANK_OFF((x) >> 5) + 0x18)
#define GPCR(x)         GPIO_REG(BANK_OFF((x) >> 5) + 0x24)

static inline int gpio_get_value(unsigned gpio)
{
	if (gpio < NR_BUILTIN_GPIO)
		return GPLR(gpio) & GPIO_bit(gpio);
	else
		panic("Invalid GPIO pin %u\n", gpio);
}

static inline void gpio_set_value(unsigned gpio, int value)
{
	if (gpio < NR_BUILTIN_GPIO) {
		if (value)
			GPSR(gpio) = GPIO_bit(gpio);
		else
			GPCR(gpio) = GPIO_bit(gpio);
	} else
		panic("Invalid GPIO pin %u\n", gpio);
}

#endif	/* __PXA_GPIO_H */

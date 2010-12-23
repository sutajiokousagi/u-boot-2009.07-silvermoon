#include <asm/arch/common.h>

void complete(struct completion *completion)
{
	completion->done = 1;
}

inline unsigned long msecs_to_jiffies(int time)
{
	return time * HZ / USEC_PER_SEC;
}


int platform_driver_register(struct platform_device *pdev)
{
	return 1;
}

/*
 * platform_get_resource - get a resource for a device
 * @dev: platform device
 * @type: resource type
 * @num: resource index
 */
struct resource *platform_get_resource(struct platform_device *dev,
		unsigned int type, unsigned int num)
{
	int i;

	for (i = 0; i < dev->num_resources; i++) {
		struct resource *r = &dev->resource[i];

		if ((r->flags & (IORESOURCE_IO|IORESOURCE_MEM|
						IORESOURCE_IRQ|IORESOURCE_DMA)) == type)
			if (num-- == 0)
				return r;
	}
	return NULL;
}

void dev_err(struct device * dev, char *msg)
{
	printf("%s: %s\n", (struct platform_device *)(container_of(dev, struct platform_device, dev))->name, msg);
}

int request_irq(unsigned int irq,
		irq_handler_t handler,
		unsigned long flags, const char *devname, void *dev_id)
{
	return 1;
}

void inline getnstimeofday(struct timespec *tv)
{
	long ticks = get_ticks();
	tv->tv_nsec = ticks / (CONFIG_SYS_HZ / 5000); // KV
	tv->tv_sec = 0;
}

struct clk *clk_get(struct device *dev, const char *id) {return NULL;}
void clk_put(struct clk *clk) {return NULL;}

unsigned long  clk_get_rate(struct clk * clk) 
{
	return 208 * 1000000;
}


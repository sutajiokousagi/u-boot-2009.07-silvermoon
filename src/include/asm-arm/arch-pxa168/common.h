#ifndef _PXA_CONFIG
#define _PXA_CONFIG

#include <common.h>
#include <asm/io.h>
#include <linux/types.h>
#include <linux/err.h>

#define __initdata
#define __exit
#define __init
#define __deprecated
#define module_init(...)
#define module_param(...)
#define module_exit(...)
#define EXPORT_SYMBOL(...)
#define MODULE_DESCRIPTION(...)
#define MODULE_AUTHOR(...)
#define MODULE_LICENSE(...)
#define MODULE_PARM_DESC(...)

#define HZ CONFIG_SYS_HZ
#define IRQ_NONE        (0)
#define IRQ_HANDLED     (1)
#define IRQ_RETVAL(x)   ((x) != 0)
#define IORESOURCE_IO           0x00000100      /* Resource type */
#define IORESOURCE_MEM          0x00000200
#define IORESOURCE_IRQ          0x00000400
#define IORESOURCE_DMA          0x00000800
#define PXA_IRQ(x)      (x)
#define IRQ_PXA168_NAND	PXA_IRQ(45)     /* NAND Controller */
#define IRQF_DISABLED           0x00000020
#define free_irq(x, y)	kfree(y)
#define DFC_BASE_ADDR   0xD4283000
#define IRQF_SHARED	1
#undef	ioremap
#define ioremap(physaddr, size) DFC_BASE_ADDR
#undef	iounmap
#define iounmap(...)
#define ARRAY_AND_SIZE(x)       (x), ARRAY_SIZE(x)
#define msleep(a) udelay(a * 1000)
#define dev_warn(...)

#define __REG_PXA910(x)	(*((volatile u32 *)(x)))
#define BU_U32            unsigned int
#define BU_REG_WRITE(x,y) ((*(volatile BU_U32 *)(x)) = y )

typedef int wait_queue_head_t; 
typedef int irqreturn_t;
typedef irqreturn_t (*irq_handler_t)(int, void *);
typedef u32 resource_size_t;
typedef struct pm_message {
	        int event;
} pm_message_t;
typedef int spinlock_t;
#define clk_enable(...)	
#define clk_disable(...)
#define DEBUG(...)
#define del_mtd_device(...)

static inline int __decalare_return(void)
{
	return 1;
}
#define DECLARE_WAITQUEUE(x, y) __decalare_return()
#define set_current_state(...)		do { } while (0)
#define schedule()			do { } while (0)
#define cond_resched()
#define	spin_lock_irqsave(x,y) y = 0;
#define spin_unlock_irqrestore(x,y)
#define spin_lock_init(...)
#define spin_lock(...)
#define spin_unlock(...)
#define add_wait_queue(...)
#define remove_wait_queue(...)
#define wake_up(...)
#define init_waitqueue_head(...)
#define THIS_MODULE		0
#define wait_for_completion(...)
#define add_mtd_partitions(...)

#define platform_get_irq(...)	0
#define request_mem_region(x,y,z)	(container_of(x, struct resource, start))
#define release_mem_region(...)
/* Parameters used to convert the timespec values: */
#define MSEC_PER_SEC    1000L
#define USEC_PER_MSEC   1000L
#define NSEC_PER_USEC   1000L
#define NSEC_PER_MSEC   1000000L
#define USEC_PER_SEC    1000000L
#define NSEC_PER_SEC    1000000000L
#define FSEC_PER_SEC    1000000000000000L

#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)
#define BIT_3 (1 << 3)
#define BIT_4 (1 << 4)
#define BIT_5 (1 << 5)
#define BIT_6 (1 << 6)
#define BIT_7 (1 << 7)
#define BIT_8 (1 << 8)
#define BIT_9 (1 << 9)
#define BIT_10 (1 << 10)
#define BIT_11 (1 << 11)
#define BIT_12 (1 << 12)
#define BIT_13 (1 << 13)
#define BIT_14 (1 << 14)
#define BIT_15 (1 << 15)
#define BIT_16 (1 << 16)
#define BIT_17 (1 << 17)
#define BIT_18 (1 << 18)
#define BIT_19 (1 << 19)
#define BIT_20 (1 << 20)
#define BIT_21 (1 << 21)
#define BIT_22 (1 << 22)
#define BIT_23 (1 << 23)
#define BIT_24 (1 << 24)
#define BIT_25 (1 << 25)
#define BIT_26 (1 << 26)
#define BIT_27 (1 << 27)
#define BIT_28 (1 << 28)
#define BIT_29 (1 << 29)
#define BIT_30 (1 << 30)
#define BIT_31 ((unsigned)1 << 31)

#define SHIFT0(Val)  (Val)
#define SHIFT1(Val)  ((Val) << 1)
#define SHIFT2(Val)  ((Val) << 2)
#define SHIFT3(Val)  ((Val) << 3)
#define SHIFT4(Val)  ((Val) << 4)
#define SHIFT5(Val)  ((Val) << 5)
#define SHIFT6(Val)  ((Val) << 6)
#define SHIFT7(Val)  ((Val) << 7)
#define SHIFT8(Val)  ((Val) << 8)
#define SHIFT9(Val)  ((Val) << 9)
#define SHIFT10(Val) ((Val) << 10)
#define SHIFT11(Val) ((Val) << 11)
#define SHIFT12(Val) ((Val) << 12)
#define SHIFT13(Val) ((Val) << 13)
#define SHIFT14(Val) ((Val) << 14)
#define SHIFT15(Val) ((Val) << 15)
#define SHIFT16(Val) ((Val) << 16)
#define SHIFT17(Val) ((Val) << 17)
#define SHIFT18(Val) ((Val) << 18)
#define SHIFT19(Val) ((Val) << 19)
#define SHIFT20(Val) ((Val) << 20)
#define SHIFT21(Val) ((Val) << 21)
#define SHIFT22(Val) ((Val) << 22)
#define SHIFT23(Val) ((Val) << 23)
#define SHIFT24(Val) ((Val) << 24)
#define SHIFT25(Val) ((Val) << 25)
#define SHIFT26(Val) ((Val) << 26)
#define SHIFT27(Val) ((Val) << 27)
#define SHIFT28(Val) ((Val) << 28)
#define SHIFT29(Val) ((Val) << 29)
#define SHIFT30(Val) ((Val) << 30)
#define SHIFT31(Val) ((Val) << 31)

/*
 *
 *      THE BASE ADDRESSES
 *
 */
#define APBC_BASE       0xD4015000

#define APBC_AIB_CLK_RST                (APBC_BASE+0x003C)      /* 32 bit       Clock/Reset
								 * Control
								 * Register for
								 * AIB
								 */

#define APBC_AIB_CLK_RST_FNCLK                          (1 << 1)

#define APBC_AIB_CLK_RST_APBCLK                         (1 << 0)

/*
 *
 *      THE BASE ADDRESSES
 *
 */
#define SMC_BASE        0xD4283800

/*
 *
 *      THE REGISTER DEFINES
 *
 */
#define SMC_MSCX                (SMC_BASE+0x0020)       /* Static Memory Control Registers Start */
#define SMC_SXCNFGX             (SMC_BASE+0x0030)       /* Synchronous Static Memory Control
							 * Registers Start
							 */
#define SMC_MEMCLKCFG   (SMC_BASE+0x0068)       /* 32 bit       Clock Configuration
						 * Control Register
						 */
#define SMC_CSDFICFGX   (SMC_BASE+0x0090)       /* DFI Configuration Control
						 * Register for Chip
						 * Selects Start
						 */
#define SMC_CLK_RET_DEL (SMC_BASE+0x00B0)       /* 32 bit       Synchronous NOR Flash
						 * Clock Return Delay
						 * Register
						 */
#define SMC_ADV_RET_DEL (SMC_BASE+0x00B4)       /* 32 bit       Synchronous NOR Flash
						 * nADV Return Delay
						 * Register
						 */
#define SMC_CSADRMAPX   (SMC_BASE+0x00C0)       /* Address Mapping Control Registers Start */
#define SMC_TCVCFGREG   (SMC_BASE+0x00D0)       /* 32 bit       Transceiver
						 * Configuration Control
						 * Register 0
						 */
#define SMC_WE_APX              (SMC_BASE+0x00E0)       /* nWE Address Phase Control
							 * Registers Start
							 */
#define SMC_OE_APX              (SMC_BASE+0x00F0)       /* nOE Address Phase Control
							 * Registers Start
							 */
#define SMC_ADV_APX             (SMC_BASE+0x0100)       /* nADV Address Phase Control
							 * Registers Start
							 */

#define typecheck(type,x) \
	({      type __dummy; \
	         typeof(x) __dummy2; \
	         (void)(&__dummy == &__dummy2); \
	         1; \
	 })

#define time_after(a,b)         \
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)(b) - (long)(a) < 0))

#define time_before(a,b)        time_after(b,a)
#define jiffies			({			\
				long ticks;		\
				ticks = get_ticks();	\
				ticks / 3 / 1000; 	\
				})
#define INIT_COMPLETION(x)      ((x).done = 0)

struct ecc_layout {
	int     offset;
	int     length;
};

struct yaffs_oob_desc {
	int     offset;
	int     size;
};

struct completion {
	        unsigned int done;
};

struct resource {
	resource_size_t start;
	resource_size_t end;
	const char *name;
	unsigned long flags;
	struct resource *parent, *sibling, *child;
};

struct device {
        void            *driver_data;   /* data private to the driver */
	void            *platform_data; /* Platform specific data, device
					   core doesn't touch it */
	u64             *dma_mask;      /* dma mask (if dma'able device) */
	u64             coherent_dma_mask;/* Like dma_mask, but for
					     alloc_coherent mappings as
					     not all hardware supports
					     64 bit addresses for consistent
					     allocations such descriptors. */
};

struct platform_device {
	const char      * name;
	int             id;
        struct device   dev;
	u32             num_resources;
	struct resource * resource;
};

struct device_driver {
	const char              *name;
};

struct platform_driver {
	int (*probe)(struct platform_device *);
	int (*remove)(struct platform_device *);
	void (*shutdown)(struct platform_device *);
	int (*suspend)(struct platform_device *, pm_message_t state);
	int (*resume)(struct platform_device *);
	struct device_driver driver;
};

struct timespec {
	time_t  tv_sec;         /* seconds */
	long    tv_nsec;        /* nanoseconds */
};

void complete(struct completion *completion);
unsigned long wait_for_completion_timeout(struct completion *x, unsigned long timeout);
inline unsigned long msecs_to_jiffies(int time);
void dev_err(struct device * dev, char *msg);
void getnstimeofday(struct timespec *tv);
struct clk *clk_get(struct device *dev, const char *id);
void clk_put(struct clk *clk);
unsigned long clk_get_rate(struct clk * clk);
#define outsl(p,d,l)                   __raw_writesl(__io(p),d,l)
#define CPUID_ID	        0

#define __stringify_1(x)        #x
#define __stringify(x)          __stringify_1(x)

#define read_cpuid(reg)							\
	({								\
		unsigned int __val;					\
		asm("mrc	p15, 0, %0, c0, c0, " __stringify(reg)	\
		    : "=r" (__val)					\
		    :							\
		    : "cc");						\
		__val;							\
	})


#ifdef CONFIG_CPU_PXA930
#define __cpu_is_pxa930(id)                             		\
	({                                              		\
	 unsigned int _id = (id) >> 4 & 0xfff;   			\
	 _id == 0x683;           					\
	 })
#else
#define __cpu_is_pxa930(id)     (0)
#endif

#ifdef CONFIG_CPU_PXA935
#define __cpu_is_pxa935(id)                             		\
	({                                              		\
	 unsigned int _id = (id) >> 4 & 0xfff;   			\
	 _id == 0x693;                           			\
	 })
#else
#define __cpu_is_pxa935(id)     (0)
#endif

#ifdef CONFIG_CPU_PXA910
#define __cpu_is_pxa910(id)                             		\
	({                                              		\
	 unsigned int _id = (id) >> 4 & 0xfff;   			\
	 _id == 0x926 || _id == 0x800 || _id == 0x840;   		\
	 })
#else
#define __cpu_is_pxa910(id)     (0)
#endif

#ifdef CONFIG_CPU_PXA910_910
#define __cpu_is_pxa910_910(id)                         		\
	({                                              		\
	 unsigned int _id = (id) & 0xffff;       			\
	 _id == 0x9262 || _id == 0x8000;         			\
	 })
#else
#define __cpu_is_pxa910_910(id) (0)
#endif

#ifdef CONFIG_CPU_PXA910_168
#define __cpu_is_pxa910_168(id)                         		\
	({                                              		\
	 unsigned int _id = (id) & 0xffff;       			\
	 _id == 0x9263 || _id == 0x8400;         			\
	 })
#else
#define __cpu_is_pxa910_168(id) (0)
#endif

#define cpu_is_pxa930()                                 		\
	({                                              		\
	 unsigned int id = read_cpuid(CPUID_ID); 			\
	 __cpu_is_pxa930(id);                    			\
	 })
#define cpu_is_pxa935()    		                		\
	({                              		                \
	 unsigned int id = read_cpuid(CPUID_ID); 			\
	 __cpu_is_pxa935(id);                    			\
	 })
#define cpu_is_pxa910()                                 		\
	({                                              		\
	 unsigned int id = read_cpuid(CPUID_ID); 			\
	 __cpu_is_pxa910(id);                    			\
	 })
#define cpu_is_pxa910_910()                                     	\
	({                                              		\
	 unsigned int id = read_cpuid(CPUID_ID); 			\
	 __cpu_is_pxa910_910(id);                        		\
	 })
#define cpu_is_pxa910_168()                                     	\
	({                                              		\
	 unsigned int id = read_cpuid(CPUID_ID); 			\
	 __cpu_is_pxa910_168(id);                        		\
	 })

#define DMA_BIT_MASK(n) (((n) == 64) ? ~0ULL : ((1ULL<<(n))-1))

static inline void *dev_get_drvdata(const struct device *dev)
{
	return dev->driver_data;
}       

static inline void dev_set_drvdata(struct device *dev, void *data)
{       
	dev->driver_data = data;
}       

#define platform_get_drvdata(_dev)      dev_get_drvdata(&(_dev)->dev)
#define platform_set_drvdata(_dev,data) dev_set_drvdata(&(_dev)->dev, (data))

static inline void init_completion(struct completion *x)
{
	x->done = 0;
}

/* non use of DMA */
/*
 * Descriptor structure for PXA's DMA engine
 * Note: this structure must always be aligned to a 16-byte boundary.
 */

typedef struct pxa_dma_desc {
	volatile u32 ddadr;     /* Points to the next descriptor + flags */
	volatile u32 dsadr;     /* DSADR value for the current transfer */
	volatile u32 dtadr;     /* DTADR value for the current transfer */
	volatile u32 dcmd;      /* DCMD value for the current transfer */
} pxa_dma_desc;

typedef enum {
	DMA_PRIO_HIGH = 0,
	DMA_PRIO_MEDIUM = 1,
	DMA_PRIO_LOW = 2
} pxa_dma_prio;

static inline int pxa_request_dma (char *name, pxa_dma_prio prio,
		void (*irq_handler)(int, void *),
		void *data)
{
	return 0;
}

static inline void pxa_free_dma (int dma_ch) {}

static inline void
dma_free_coherent(struct device *dev, size_t size, void *cpu_addr,
		dma_addr_t dma_handle) {}

#define DMAC_REGS_VIRT  io_p2v(0x40000000)
#define DMAC_REG(x)     (*((volatile u32 *)(DMAC_REGS_VIRT + (x))))

#define DALGN           DMAC_REG(0x00a0)  /* DMA Alignment Register */
#define DDADR(n)        DMAC_REG(0x0200 + ((n) << 4))
#define DSADR(n)        DMAC_REG(0x0204 + ((n) << 4))
#define DTADR(n)        DMAC_REG(0x0208 + ((n) << 4))
#define DCMD(n)         DMAC_REG(0x020c + ((n) << 4))
#define DRCMR(n)        DMAC_REG((((n) < 64) ? 0x0100 : 0x1100) + \
		                                 (((n) & 0x3f) << 2))

#define DCSR_RUN        (1 << 31)       /* Run Bit (read / write) */
#define DCSR_NODESC     (1 << 30)       /* No-Descriptor Fetch (read / write) */
#define DCSR_STOPIRQEN  (1 << 29)       /* Stop Interrupt Enable (read / write) */
#define DCSR_REQPEND    (1 << 8)        /* Request Pending (read-only) */
#define DCSR_STOPSTATE  (1 << 3)        /* Stop State (read-only) */
#define DCSR_ENDINTR    (1 << 2)        /* End Interrupt (read / write) */
#define DCSR_STARTINTR  (1 << 1)        /* Start Interrupt (read / write) */
#define DCSR_BUSERR     (1 << 0)        /* Bus Error Interrupt (read / write) */

#define DCSR_EORIRQEN   (1 << 28)       /* End of Receive Interrupt Enable (R/W) */
#define DCSR_EORJMPEN   (1 << 27)       /* Jump to next descriptor on EOR */
#define DCSR_EORSTOPEN  (1 << 26)       /* STOP on an EOR */
#define DCSR_SETCMPST   (1 << 25)       /* Set Descriptor Compare Status */
#define DCSR_CLRCMPST   (1 << 24)       /* Clear Descriptor Compare Status */
#define DCSR_CMPST      (1 << 10)       /* The Descriptor Compare Status */
#define DCSR_EORINTR    (1 << 9)        /* The end of Receive */

#define DRCMR_MAPVLD    (1 << 7)        /* Map Valid (read / write) */
#define DRCMR_CHLNUM    0x1f            /* mask for Channel Number (read / write) */

#define DDADR_DESCADDR  0xfffffff0      /* Address of next descriptor (mask) */
#define DDADR_STOP      (1 << 0)        /* Stop (read / write) */

#define DCMD_INCSRCADDR (1 << 31)       /* Source Address Increment Setting. */
#define DCMD_INCTRGADDR (1 << 30)       /* Target Address Increment Setting. */
#define DCMD_FLOWSRC    (1 << 29)       /* Flow Control by the source. */
#define DCMD_FLOWTRG    (1 << 28)       /* Flow Control by the target. */
#define DCMD_STARTIRQEN (1 << 22)       /* Start Interrupt Enable */
#define DCMD_ENDIRQEN   (1 << 21)       /* End Interrupt Enable */
#define DCMD_ENDIAN     (1 << 18)       /* Device Endian-ness. */
#define DCMD_BURST8     (1 << 16)       /* 8 byte burst */
#define DCMD_BURST16    (2 << 16)       /* 16 byte burst */
#define DCMD_BURST32    (3 << 16)       /* 32 byte burst */
#define DCMD_WIDTH1     (1 << 14)       /* 1 byte width */
#define DCMD_WIDTH2     (2 << 14)       /* 2 byte width (HalfWord) */
#define DCMD_WIDTH4     (3 << 14)       /* 4 byte width (Word) */
#define DCMD_LENGTH     0x01fff         /* length mask (max = 8K - 1) */

#define dma_alloc_coherent(...) 	NULL
#endif

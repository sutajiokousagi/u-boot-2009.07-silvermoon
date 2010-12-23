/* MMC_CLKRT */
#define MMC_CLKRT_20MHZ                        (0x0000UL)
#define MMC_CLKRT_10MHZ                        (0x0001UL)
#define MMC_CLKRT_5MHZ                 (0x0002UL)
#define MMC_CLKRT_2_5MHZ               (0x0003UL)
#define MMC_CLKRT_1_25MHZ              (0x0004UL)
#define MMC_CLKRT_0_625MHZ             (0x0005UL)
#define MMC_CLKRT_0_3125MHZ            (0x0006UL)

/* MMC_RESTO */
#define MMC_RES_TO_MAX                 (0x007fUL) /* [6:0] */

/* MMC_SPI */
#define MMC_SPI_DISABLE                        (0x00UL)
#define MMC_SPI_EN                     (0x01UL)
#define MMC_SPI_CS_EN                  (0x01UL << 2)
#define MMC_SPI_CS_ADDRESS             (0x01UL << 3)
#define MMC_SPI_CRC_ON                 (0x01UL << 1)


#define MMC_STRPCL	0x0000
#define STOP_CLOCK		(1 << 0)
#define START_CLOCK		(2 << 0)

#define MMC_STAT	0x0004
#define STAT_END_CMD_RES		(1 << 13)
#define STAT_PRG_DONE			(1 << 12)
#define STAT_DATA_TRAN_DONE		(1 << 11)
#define STAT_CLK_EN			(1 << 8)
#define STAT_RECV_FIFO_FULL		(1 << 7)
#define STAT_XMIT_FIFO_EMPTY		(1 << 6)
#define STAT_RES_CRC_ERR		(1 << 5)
#define STAT_SPI_READ_ERROR_TOKEN	(1 << 4)
#define STAT_CRC_READ_ERROR		(1 << 3)
#define STAT_CRC_WRITE_ERROR		(1 << 2)
#define STAT_TIME_OUT_RESPONSE		(1 << 1)
#define STAT_READ_TIME_OUT		(1 << 0)
#define STAT_ERROR			(0x3f)

#define MMC_CLKRT	0x0008		/* 3 bit */

#define MMC_SPI		0x000c
#define SPI_CS_ADDRESS		(1 << 3)
#define SPI_CS_EN		(1 << 2)
#define CRC_ON			(1 << 1)
#define SPI_EN			(1 << 0)

#define MMC_CMDAT	0x0010
#define CMDAT_SDIO_INT_EN	(1 << 11)
#define CMDAT_SD_4DAT		(1 << 8)
#define CMDAT_DMAEN		(1 << 7)
#define CMDAT_INIT		(1 << 6)
#define CMDAT_BUSY		(1 << 5)
#define CMDAT_STREAM		(1 << 4)	/* 1 = stream */
#define CMDAT_WRITE		(1 << 3)	/* 1 = write */
#define CMDAT_DATAEN		(1 << 2)
#define CMDAT_RESP_NONE		(0 << 0)
#define CMDAT_RESP_SHORT	(1 << 0)
#define CMDAT_RESP_R2		(2 << 0)
#define CMDAT_RESP_R3		(3 << 0)

#define MMC_RESTO	0x0014	/* 7 bit */

#define MMC_RDTO	0x0018	/* 16 bit */

#define MMC_BLKLEN	0x001c	/* 10 bit */

#define MMC_NOB		0x0020	/* 16 bit */

#define MMC_PRTBUF	0x0024
#define BUF_PART_FULL		(1 << 0)

#define MMC_I_MASK	0x0028

/*PXA27x MMC interrupts*/
#define SDIO_SUSPEND_ACK  	(1 << 12)
#define SDIO_INT          	(1 << 11)
#define RD_STALLED        	(1 << 10)
#define RES_ERR           	(1 << 9)
#define DAT_ERR           	(1 << 8)
#define TINT              	(1 << 7)

/*PXA2xx MMC interrupts*/
#define TXFIFO_WR_REQ		(1 << 6)
#define RXFIFO_RD_REQ		(1 << 5)
#define CLK_IS_OFF		(1 << 4)
#define STOP_CMD		(1 << 3)
#define END_CMD_RES		(1 << 2)
#define PRG_DONE		(1 << 1)
#define DATA_TRAN_DONE		(1 << 0)

#if defined(CONFIG_PXA27x) || defined(CONFIG_PXA3xx)
#define MMC_I_MASK_ALL          0x00001fff
#else
#define MMC_I_MASK_ALL          0x0000007f
#endif

#define MMC_I_REG	0x002c
#define MMC_I_REG_TXFIFO_WR_REQ		(0x01UL << 6)
#define MMC_I_REG_RXFIFO_RD_REQ		(0x01UL << 5)
#define MMC_I_REG_CLK_IS_OFF		(0x01UL << 4)
#define MMC_I_REG_STOP_CMD		(0x01UL << 3)
#define MMC_I_REG_END_CMD_RES		(0x01UL << 2)
#define MMC_I_REG_PRG_DONE		(0x01UL << 1)
#define MMC_I_REG_DATA_TRAN_DONE	(0x01UL)
#define MMC_I_REG_ALL			(0x007fUL)
/* same as MMC_I_MASK */

#define MMC_CMD		0x0030

#define MMC_ARGH	0x0034	/* 16 bit */

#define MMC_ARGL	0x0038	/* 16 bit */

#define MMC_RES		0x003c	/* 16 bit */

#define MMC_RXFIFO	0x0040	/* 8 bit */

#define MMC_TXFIFO	0x0044	/* 8 bit */

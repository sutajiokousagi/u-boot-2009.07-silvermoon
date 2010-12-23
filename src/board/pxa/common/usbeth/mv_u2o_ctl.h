/*
 *  Copyright (C) Compaq Computer Corporation, 1998, 1999
 *  Copyright (C) Extenex Corporation 2001
 *  Copyright (C) Intrinsyc, Inc., 2002
 *
 * (C) Copyright 2006 Marvell International Ltd.  
 * All Rights Reserved 
 *
 *  usb_ctl.h
 *
 *  PRIVATE interface used to share info among components of the PXA USB
 *  core: usb_ctl, usb_ep0, usb_recv and usb_send. Clients of the USB core
 *  should use pxa_usb.h.
 *
 *  02-May-2002
 *   Frank Becker (Intrinsyc) - derived from sa1100 usb_ctl.h
 *
 */

#ifndef _USB_CTL_H
#define _USB_CTL_H

#define ICU_INT_STATUS_1	(0xD4282130)

#define PMUA_USB_CLK_GATE_CTRL  (0xd4282834)
#define PMUA_USB_CLK_RES_CTRL   (0xd428285c)

#define U2O_USED_ENDPOINTS 3

#define USB_SPEED_FULL	1
#define USB_SPEED_HIGH	2

#define USB_ENDPOINT_XFER_BULK		2

#ifdef MMU_ENABLE

#define DMA_AREA_START	DDR_UNCACHABLE_BASE
#define DMA_BUF_SIZE	0x20000

#define DMA_MALLOC_START	(DMA_AREA_START + DMA_BUF_SIZE)
#endif

#define ETH_BUF_SIZE	1536

/* registers */
#define U2O_REG_BASE				CONFIG_U2O_REG_BASE
#define U2O_PHY_BASE				CONFIG_U2O_PHY_BASE

#define U2xUSBCMD				(0x140)       /* U2O Command */
#define U2xUSBCMD_RST				(1<<1)      /* Reset */
#define U2xUSBCMD_RS				(1<<0)      /* Run/Stop */

#define U2xUSBSTS				(0x144)       /* U2O Status */
#define U2xUSBINTR				(0x148)       /* U2O Interrupt Enable */

#define U2xPORTSC				(0x184)       /* U2O Port Status */
#define U2xPORTSC_PP                            (1<<12)   		  /* Port Power */
#define U2xPORTSC_PTS_MASK                      (3<<30)   		  /* Parallel Transceiver Select */

#define U2xUSBINTR				(0x148)       /* U2O Interrupt Enable */

#define U2xUSBMODE				(0x1A8)       /* U2O Device Mode */
#define U2xUSBMODE_CM_MASK                      (3<<0)   		  /* U2O Controller Mode */
#define U2xUSBMODE_CM_HOST                      (3<<0)
#define U2xUSBMODE_CM_DEVICE                    (2<<0)
#define U2xUSBMODE_SLOM_OFF                     (1<<3)
#define U2xUSBMODE_SDIS_ACTIVE                  (1<<4)
#define U2xOTGSC				(0x1A4)       /* U2O On-The-Go Status and Control */

/* OTG interrupt enable bit masks */
#define  U2xOTGSC_DPIE                         (0x40000000)   /* Data-line pulsing IE */
#define  U2xOTGSC_1MSIE                        (0x20000000)   /* 1 Millisecond timer IE */
#define  U2xOTGSC_BSEIE                        (0x10000000)   /* B-session end IE */
#define  U2xOTGSC_BSVIE                        (0x08000000)   /* B-session valid IE */
#define  U2xOTGSC_ASVIE                        (0x04000000)   /* A-session valid IE */
#define  U2xOTGSC_AVVIE                        (0x02000000)   /* A-V-bus valid IE */
#define  U2xOTGSC_IDIE                         (0x01000000)   /* OTG ID IE */
#define  U2xOTGSC_IE_MASK   		       (0x7F000000)

/* OTG interrupt status bit masks */
#define  U2xOTGSC_IS_MASK   (0x007F0000)
#define  U2xOTGSC_DPIS                         (0x00400000)   /* Data-line pulsing IS */
#define  U2xOTGSC_1MSIS                        (0x00200000)   /* 1 Millisecond timer IS */
#define  U2xOTGSC_BSEIS                        (0x00100000)   /* B-session end IS */
#define  U2xOTGSC_BSVIS                        (0x00080000)   /* B-session valid IS */
#define  U2xOTGSC_ASVIS                        (0x00040000)   /* A-session valid IS */
#define  U2xOTGSC_AVVIS                        (0x00020000)   /* A-Vbus valid IS */
#define  U2xOTGSC_IDIS                         (0x00010000)   /* OTG ID IS */

/* OTG status bit masks */
#define  U2xOTGSC_DPS                          (0x00004000)   /* Data-line pulsing */
#define  U2xOTGSC_1MST                         (0x00002000)   /* 1 Milliseconf timer toggle */
#define  U2xOTGSC_BSE                          (0x00001000)   /* B-session end */
#define  U2xOTGSC_BSV                          (0x00000800)   /* B-session valid */
#define  U2xOTGSC_ASV                          (0x00000400)   /* A-session valid */
#define  U2xOTGSC_AVV                          (0x00000200)   /* A-Vbus Valid */
#define  U2xOTGSC_ID                           (0x00000100)   /* OTG ID */

/* OTG control bit masks */
#define  U2xOTGSC_CTL_BITS                     (0x2F)
#define  U2xOTGSC_HABA                         (0x00000080)   /* hardware assisted B-Dis to A-Con */
#define  U2xOTGSC_HADP                         (0x00000040)   /* hardware assisted data pulse bits*/
#define  U2xOTGSC_IDPU                         (0x00000020)   /* ID pull-up enable */
#define  U2xOTGSC_DP                           (0x00000010)   /* Data-pulsing */
#define  U2xOTGSC_OT                           (0x00000008)   /* OTG termination */
#define  U2xOTGSC_HAAR                         (0x00000004)   /* Auto reset bit */
#define  U2xOTGSC_VC                           (0x00000002)   /* Vbus charge */
#define  U2xOTGSC_VD                           (0x00000001)   /* Vbus discharge */

/* PHY registers */
#ifdef CONFIG_CPU_MONAHANS
#define U2PPLL		(0x000)       /* U2O PHY PLL Control */
#define U2PTX		(0x004)       /* U2O PHY TX Control */
#define U2PRX		(0x008)       /* U2O PHY RX Control */
#define U2IVREF		(0x00C)       /* U2O PHY IVREF Control */
#define U2PT0		(0x010)       /* U2O PHY Test 0 Control */
#define U2PT1		(0x014)       /* U2O PHY Test 1 Control */
#define U2PT2		(0x018)       /* U2O PHY Test 2 Control */
#define U2PT3		(0x01C)       /* U2O PHY Test 3 Control */
#define U2PT4		(0x020)       /* U2O PHY Test 4 Control */
#define U2PT5		(0x024)       /* U2O PHY Test 5 Control */
#define U2PID		(0x028)       /* U2O PHY ID Register */
#define U2PRS		(0x02C)       /* U2O PHY Reserve Register */
#define U2PMN		(0x030)       /* U2O PHY Monitor Register */
#define U2OCG		(0x108)       /* U2O Clock Gate Register */
#else
#define U2PRSRVD        0x0
#define U2PCTRL         0x4
#define U2PPLL          0x8
#define U2PTX           0xc
#define U2PRX           0x10
#define U2PIVREF        0x14
#define U2PT0           0x18
#define U2PT1           0x1c
#define U2PT2           0x20
#define U2PID           0x24
#define U2PINT          0x28
#define U2PDBGCTL       0x2c
#define U2PCTL1         0x30
#define U2PT3           0x34
#define U2PT4           0x38
#define U2PT5           0x3c

// For UTMICTRL Register
#define UTMI_CTRL_INPKT_DELAY_SHIFT             30
#define UTMI_CTRL_INPKT_DELAY_SOF_SHIFT 	28
#define UTMI_CTRL_ARC_PULLDN_SHIFT              12
#define UTMI_CTRL_PLL_PWR_UP_SHIFT              1
#define UTMI_CTRL_PWR_UP_SHIFT                  0
// For UTMI_PLL Register
#define UTMI_PLL_CLK_BLK_EN_SHIFT               24
#define UTMI_PLL_FBDIV_SHIFT                    4
#define UTMI_PLL_REFDIV_SHIFT                   0
#define UTMI_PLL_FBDIV_MASK                     0x00000FF0
#define UTMI_PLL_REFDIV_MASK                    0x0000000F
// For UTMI_TX Register
#define UTMI_TX_LOW_VDD_EN_SHIFT                11

#define REG_RCAL_START                          0x00001000
#define VCOCAL_START                            0x00200000
#define KVCO_EXT                                0x00400000
#define PLL_READY                               0x00800000
#define CLK_BLK_EN                              0x01000000

#endif

#define U2P480		__REG(0x42404078)         /* U2O PHY 480Mhz Control */

/* U2PPLL */
#define U2PPLL_CTRL_REG		        (0x000)
#define U2PPLL_TSTB			(1<<31)
#define U2PPLL_PLLVDD18_SHIFT		(29)
#define U2PPLL_PLLVDD18_MASK		(3<<29)
#define U2PPLL_PLLVDD12_SHIFT		(27)
#define U2PPLL_PLLVDD12_MASK		(3<<27)
#define U2PPLL_PLLCALI12_SHIFT		(25)
#define U2PPLL_PLLCALI12_MASK		(3<<25)
#define U2PPLL_CLK_BLK_EN		(1<<24)
#define U2PPLL_READY			(1<<23)
#define U2PPLL_KVCO_EXT			(1<<22)
#define U2PPLL_VCOCAL_START		(1<<21)
#define U2PPLL_BGP_VSEL_SHIFT		(19)
#define U2PPLL_BGP_VSEL_MASK		(3<<19)
#define U2PPLL_LOCKDET_ISEL		(1<<18)
#define U2PPLL_KVCO_SHIFT		(15)
#define U2PPLL_KVCO_MASK		(7<<15)
#define U2PPLL_ICP_SHIFT		(12)
#define U2PPLL_ICP_MASK			(7<<12)
#define U2PPLL_FBDIV_SHIFT		(4)
#define U2PPLL_FBDIV_MASK		(0xff<<4)
#define U2PPLL_REFDIV_SHIFT		(0)
#define U2PPLL_REFDIV_MASK		(0xf<<4)

/* U2PTX */
#define U2PTX_CTRL_REG                  (0x004)
#define U2PTX_RCAL_START		(1<<12)

/* U2PRX */
#define U2PRX_EARLY_VOS_ON_EN			(1<<31)
#define U2PRX_RXDATA_BLOCK_EN			(1<<30)
#define U2PRX_RXDATA_BLOCK_LENGTH_SHIFT		(28)
#define U2PRX_RXDATA_BLOCK_LENGTH_MASK		(3<<28)
#define U2PRX_EDGE_DET_SEL_SHIFT		(26)
#define U2PRX_EDGE_DET_SEL_MASK			(3<<26)
#define U2PRX_EDGE_DET_EN			(1<<25)
#define U2PRX_S2TO3_DLY_SEL_SHIFT		(23)
#define U2PRX_S2TO3_DLY_SEL_MASK		(3<<23)
#define U2PRX_CDR_COEF_SEL			(1<<22)
#define U2PRX_CDR_FASTLOCK_EN			(1<<21)
#define U2PRX_PHASE_FREEZE_DLY			(1<<20)
#define U2PRX_REG_USQ_LENGTH			(1<<19)
#define U2PRX_REG_ACQ_LENGTH_SHIFT		(17)
#define U2PRX_REG_ACQ_LENGTH_MASK		(3<<17)
#define U2PRX_REG_SQ_LENGTH_SHIFT		(15)
#define U2PRX_REG_SQ_LENGTH_MASK		(3<<15)
#define U2PRX_DLL_SEL_SHIFT			(13)
#define U2PRX_DLL_SEL_MASK			(3<<13)
#define U2PRX_CAP_SEL_SHIFT			(10)
#define U2PRX_CAP_SEL_MASK			(7<<10)
#define U2PRX_DISCON_THRESH_SHIFT		(8)
#define U2PRX_DISCON_THRESH_MASk		(3<<8)
#define U2PRX_SQ_THRESH_SHIFT			(4)
#define U2PRX_SQ_THRESH_MASK			(0xf<<4)
#define U2PRX_LPF_COEF_SHIFT			(2)
#define U2PRX_LPF_COEF_MASK			(3<<2)
#define U2PRX_INTPI_SHIFT			(0)
#define U2PRX_INTPI_MASK			(3<<0)

/* U2IVREF */
#define U2IVREF_SAMPLER_CTRL           		(1<<31)
#define U2IVREF_RXVDD18_SHIFT          		(29)
#define U2IVREF_RXVDD18_MASK           		(3<<29)
#define U2IVREF_SQ_CM_SEL              		(1<<10)
#define U2IVREF_BG_VSEL_SHIFT          		(8)
#define U2IVREF_BG_VSEL_MASK           		(3<<8)
#define U2IVREF_RXVDD12_SHIFT          		(6)
#define U2IVREF_RXVDD12_MASK           		(3<<6)
#define U2IVREF_FSDRV_EN_SHIFT         		(2)
#define U2IVREF_FSDRV_EN_MASK          		(0xf<<2)
#define U2IVREF_REG_IMP_CAL_DLY_SHIFT  		(0)
#define U2IVREF_REG_IMP_CAL_DLY_MASK   		(3<<0)

/* U2PT0 */
#define U2PT0_REG_ARC_DPDM_MODE			(1<<28)

#ifdef DEBUG
#define DBGMSG printf
#else
#define DBGMSG
#endif

#define DELAY(ns) 	udelay(ns)
/*do { \
		__asm__ volatile (\
		"mov ip, #1000\n"\
		"1:	mov r0, r0\n"\
		"sub ip, ip, #1\n"\
		"cmp ip, #0\n"\
		"bne	1b\n");\
	}while(0);*/

/*================================================
 * USB Protocol Stuff
 */

/* Request Codes   */
enum { 
	GET_STATUS		=0,
	CLEAR_FEATURE		=1,
	/* reserved		=2 */
	SET_FEATURE		=3,
	/* reserved		=4 */
	SET_ADDRESS		=5,        
	GET_DESCRIPTOR		=6,
	SET_DESCRIPTOR		=7,
	GET_CONFIGURATION	=8,
	SET_CONFIGURATION	=9,
	GET_INTERFACE		=10,
	SET_INTERFACE		=11,
	SYNCH_FRAME		=12
};

typedef enum {
	EP0_IDLE,
	EP0_IN_DATA_PHASE,
	EP0_END_XFER,
	EP0_OUT_DATA_PHASE
} EP0_state;

/* USB Device Requests */
typedef struct usb_dev_request {
	__u8 bmRequestType;
	__u8 bRequest;
	__u16 wValue;
	__u16 wIndex;
	__u16 wLength;
} __attribute__ ((packed)) usb_dev_request_t;

/* Data extraction from usb_request_t fields */
enum { 
	kTargetDevice	=0,
	kTargetInterface=1,
	kTargetEndpoint	=2 
};

/*
 * These states correspond to those in the USB specification v1.0
 * in chapter 8, Device Framework.
 */
enum { 
	USB_STATE_NOTATTACHED	=0,
	USB_STATE_ATTACHED	=1,
	USB_STATE_POWERED	=2,
	USB_STATE_DEFAULT	=3,
	USB_STATE_ADDRESS	=4,
	USB_STATE_CONFIGURED	=5,
	USB_STATE_SUSPENDED	=6
};

struct usb_stats_t {
	 unsigned long ep0_fifo_write_failures;
	 unsigned long ep0_bytes_written;
	 unsigned long ep0_fifo_read_failures;
	 unsigned long ep0_bytes_read;
};

struct usb_info_t
{
	 char * client_name;
	 int state;
	 unsigned char address;
	 struct usb_stats_t stats;
};

/* in usb_ctl.c */
extern struct usb_info_t usbd_info;
extern int usb_connected;

/*
 * Function Prototypes
 */
enum { 
	kError		=-1,
	kEvSuspend	=0,
	kEvReset	=1,
	kEvResume	=2,
	kEvAddress	=3,
	kEvConfig	=4,
	kEvDeConfig	=5 
};
int usbctl_next_state_on_event( int event );

struct mv_usb_ep 
{
    struct mv_usb_dev*  usb_dev;
    int			maxpacket;
    unsigned            num : 8,
    			ack_sent : 1,
			ack_recv : 1,
                        is_enabled : 1,
                        is_in : 1;
};

struct usb_request {
        void               *buf;
        int                length;
};

struct mv_usb_dev {
	void			*mv_usb_handle;
	struct mv_usb_ep	ep[2*U2O_USED_ENDPOINTS];
    	int                     dev_no;
	unsigned 		*regbase;
	unsigned 		*phybase;
};

/* in mv_u2o_ctl.c */
extern int usb_speed;
extern struct mv_usb_dev       the_controller;

/* endpoint zero */
void ep0_reset(void);
int ep0_int_hndlr(struct usb_request*);

/* receiver */
void ep2_state_change_notify( int new_state );
int  ep2_recv(char *buf, int len, usb_callback_t callback);
int  ep2_init(int chn);
void ep2_int_hndlr(struct usb_request*);
void ep2_reset(void);
void ep2_stall(void);

/* xmitter */
void ep1_state_change_notify( int new_state );
int  ep1_send(char *buf, int len, usb_callback_t callback);
void ep1_reset(void);
int  ep1_init(int chn);
void ep1_int_hndlr(struct usb_request*);
void ep1_stall(void);




#endif /* _USB_CTL_H */

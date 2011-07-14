#ifndef PXA168_USB_PHY
#define PXA168_USB_PHY

/* ASPEN */
#define UTMI_REVISION           0x0
#define UTMI_CTRL               0x4
#define UTMI_PLL                0x8
#define UTMI_TX                 0xc
#define UTMI_RX                 0x10
#define UTMI_IVREF              0x14
#define UTMI_T0                 0x18
#define UTMI_T1                 0x1c
#define UTMI_T2                 0x20
#define UTMI_T3                 0x24
#define UTMI_T4                 0x28
#define UTMI_T5                 0x2c
#define UTMI_RESERVE            0x30
#define UTMI_USB_INT            0x34
#define UTMI_DBG_CTL            0x38
#define UTMI_OTG_ADDON          0x3c

/* TTC/TD */
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

/* For UTMICTRL Register */
#define UTMI_CTRL_USB_CLK_EN                    (1<<31)
/* pxa168 */
#define UTMI_CTRL_SUSPEND_SET1                  (1<<30)
#define UTMI_CTRL_SUSPEND_SET2                  (1<<29)
#define UTMI_CTRL_RXBUF_PDWN                    (1<<24)
#define UTMI_CTRL_TXBUF_PDWN                    (1<<11)

#define UTMI_CTRL_INPKT_DELAY_SHIFT             30
#define UTMI_CTRL_INPKT_DELAY_SOF_SHIFT 	28
#define UTMI_CTRL_PU_REF_SHIFT			20
#define UTMI_CTRL_ARC_PULLDN_SHIFT              12
#define UTMI_CTRL_PLL_PWR_UP_SHIFT              1
#define UTMI_CTRL_PWR_UP_SHIFT                  0
/* For UTMI_PLL Register */
#define UTMI_PLL_CLK_BLK_EN_SHIFT               24
#define UTMI_PLL_FBDIV_SHIFT                    4
#define UTMI_PLL_REFDIV_SHIFT                   0
#define UTMI_PLL_FBDIV_MASK                     0x00000FF0
#define UTMI_PLL_REFDIV_MASK                    0x0000000F
#define UTMI_PLL_ICP_MASK                       0x00007000
#define UTMI_PLL_KVCO_MASK                      0x00031000
#define UTMI_PLL_PLLCALI12_SHIFT		29
#define UTMI_PLL_PLLCALI12_MASK			(0x3<<29)
#define UTMI_PLL_PLLVDD18_SHIFT			27
#define UTMI_PLL_PLLVDD18_MASK			(0x3<<27)
#define UTMI_PLL_PLLVDD12_SHIFT			25
#define UTMI_PLL_PLLVDD12_MASK			(0x3<<25)
#define UTMI_PLL_KVCO_SHIFT			15
#define UTMI_PLL_ICP_SHIFT			12
/* For UTMI_TX Register */
#define UTMI_TX_LOW_VDD_EN_SHIFT                11
#define UTMI_TX_IMPCAL_VTH_SHIFT                14
#define UTMI_TX_IMPCAL_VTH_MASK                 (0x7<<14)
#define UTMI_TX_CK60_PHSEL_SHIFT                17
#define UTMI_TX_CK60_PHSEL_MASK                 (0xf<<17)
#define UTMI_TX_TXVDD12_SHIFT                   22
#define UTMI_TX_TXVDD12_MASK                    (0x3<<22)
/* For UTMI_RX Register */
#define UTMI_RX_SQ_THRESH_SHIFT                 4
#define UTMI_RX_SQ_THRESH_MASK                  (0xf<<4)
#define UTMI_REG_SQ_LENGTH_SHIFT                15
#define UTMI_REG_SQ_LENGTH_MASK                 (0x3<<15)

#define REG_RCAL_START                          0x00001000
#define VCOCAL_START                            0x00200000
#define KVCO_EXT                                0x00400000
#define PLL_READY                               0x00800000
#define CLK_BLK_EN                              0x01000000
#endif /* PXA168_USB_PHY */

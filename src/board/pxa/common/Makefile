#
# (C) Copyright 2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

ifneq ($(OBJTREE),$(SRCTREE))
$(shell mkdir -p $(obj)board/$(VENDOR)/common)
endif

LIB	= $(obj)lib$(VENDOR).a

SOBJS-y				+= freq_ll.o
SOBJS-y				+= hibernate.o

COBJS-y				+= common.o
COBJS-y				+= boot.o
COBJS-y				+= freq.o
COBJS-${CONFIG_CMD_CPUID}	+= cpuid.o
COBJS-${CONFIG_CMD_NAND}	+= nand.o
COBJS-$(CONFIG_BBM)		+= bbt.o
COBJS-${CONFIG_CMD_NET}		+= tftp.o
COBJS-${CONFIG_USB_ETH}		+= usbeth/ether-mvu2o.o
COBJS-${CONFIG_USB_ETH}		+= usbeth/mv_u2o_ctl.o
COBJS-${CONFIG_USB_ETH}		+= usbeth/mv_u2o_ep0.o
COBJS-${CONFIG_USB_ETH}		+= usbeth/mv_u2o_ep1.o
COBJS-${CONFIG_USB_ETH}		+= usbeth/mv_u2o_ep2.o
COBJS-${CONFIG_USB_ETH}		+= usbeth/mv_u2o_api.o
COBJS-${CONFIG_USB_ETH}		+= usbeth/mvUsbDevCh9.o
COBJS-${CONFIG_USB_ETH}		+= usbeth/mvUsbDevMain.o
COBJS-${CONFIG_USB_ETH}		+= usbeth/mvUsbDevRecv.o
COBJS-${CONFIG_USB_ETH}		+= usbeth/mvUsbDevSend.o
COBJS-${CONFIG_USB_ETH}		+= usbeth/mvUsbDevUtl.o
COBJS-${CONFIG_USB_ETH}		+= usbeth/mvUsbHsDevCncl.o
COBJS-${CONFIG_USB_ETH}		+= usbeth/mvUsbHsDevMain.o
COBJS-${CONFIG_USB_ETH}		+= usbeth/mvUsbHsDevUtl.o

SRCS	:= $(SOBJS-y:.o=.S) $(COBJS-y:.o=.c)
OBJS	:= $(addprefix $(obj),$(SOBJS-y) $(COBJS-y))

$(LIB):	$(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

clean:
	rm -f $(SOBJS) $(OBJS)

distclean:	clean
	rm -f $(LIB) core *.bak $(obj).depend

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################

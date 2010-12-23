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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	:= $(obj)libusbeth.a

ifdef CONFIG_USB_ETH
COBJS-y += ether-mvu2o.o
COBJS-y += mv_u2o_ctl.o
COBJS-y += mv_u2o_ep0.o
COBJS-y += mv_u2o_ep1.o
COBJS-y += mv_u2o_ep2.o
COBJS-y += mv_u2o_api.o
COBJS-y += mvUsbDevCh9.o
COBJS-y += mvUsbDevMain.o
COBJS-y += mvUsbDevRecv.o
COBJS-y += mvUsbDevSend.o
COBJS-y += mvUsbDevUtl.o
COBJS-y += mvUsbHsDevCncl.o
COBJS-y += mvUsbHsDevMain.o
COBJS-y += mvUsbHsDevUtl.o
endif

COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

$(LIB):	$(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################

# $Id: config.mk 36 2008-06-11 06:19:17Z henry $
# config.mk - project-specific configuration details

ifeq (${CNPLATFORM},)
export CNPLATFORM=silvermoon
endif
export ARCH=arm
export TARGET=$(ARCH)-linux
export CROSS_COMPILE=$(TARGET)-

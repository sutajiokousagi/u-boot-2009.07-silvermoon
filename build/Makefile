# $Id: Makefile 4789 2009-03-25 20:07:00Z henry $
# u-boot top-level makefile

include ../config/config.mk

export PLATFORM_TARGET=$(TARGET)-$(CNPLATFORM)

EXPORTS=$(abspath ../exports)
OUTPUT=$(abspath ../output/$(PLATFORM_TARGET))
TARGET_EXPORTS=$(EXPORTS)/$(PLATFORM_TARGET)
CONFIGNAME=$(CNPLATFORM)
# Version of CONFIGNAME to pass via -D
ACONFIGNAME=$(subst -,_,$(CONFIGNAME))

# Set the offset of the u-boot app that will be appended after the main u-boot binary.
# This number may need to be modified if the size of the u-boot binary grows.
# (note: always use the "0x" hex notation for this!)
APP_OFFSET=0x40000

# No config matching needed

UBOOT_CONFIGNAME=aspenite

# File to be used as a touch target to ensure the proper
# level of rebuilding config
DOT_CONFIG_BASE=.config_
DOT_CONFIG=$(DOT_CONFIG_BASE)$(CONFIGNAME)_$(UBOOT_CONFIGNAME)

# Additional platform-specific settings
# These are passed on to all .c and .S sources as CHUMBY_CONFIG_<name>=value
#ifeq ($(findstring :${CONFIGNAME}:,:stormwind: :stormwind2: :stormwind3:),)
export CHUMBY_CONFIG_production=1
#else
#export CHUMBY_CONFIG_production=0
#endif

export CHUMBY_CONFIG_memsize=128

export CHUMBY_CONFIG_scrheight=600
export CHUMBY_CONFIG_scrwidth=800

export CHUMBY_CONFIG_platform=$(CNPLATFORM)

# Dynamically generate offsets in hex
ROM_PARTITION_k1_OFF:=$(shell ../../src/rom-partdata ../../config/rom-partition-${CONFIGNAME} k1)
ROM_PARTITION_k2_OFF:=$(shell ../../src/rom-partdata ../../config/rom-partition-${CONFIGNAME} k2)
ROM_PARTITION_msp_OFF:=$(shell ../../src/rom-partdata ../../config/rom-partition-${CONFIGNAME} msp)

# Dynamically generate filesystem partition identifiers to pass to kernel
ROM_PARTITION_rfs1_NUM:=$(shell ../../src/rom-partserial ../../config/rom-partition-${CONFIGNAME} rfs1)
ROM_PARTITION_rfs2_NUM:=$(shell ../../src/rom-partserial ../../config/rom-partition-${CONFIGNAME} rfs2)

# Define touch target unique to all the partition offsets we might use
PARTITION_SIG=.partsig.$(CONFIGNAME).k1_$(ROM_PARTITION_k1_OFF).k2_$(ROM_PARTITION_k2_OFF).msp_$(ROM_PARTITION_msp_OFF).rfs1_${ROM_PARTITION_rfs1_NUM}.rfs2_${ROM_PARTITION_rfs2_NUM}

# Dynamically generate offsets and lengths in hex for config, Chst and ubfb
# map-util gives us the 0x prefix for free
DOFFSET_config:=$(shell ../../src/map-util --blocks --hex --id config ../../config/map-silvermoon_sd doffset)
LENGTH_config:=$(shell ../../src/map-util --blocks --hex --id config ../../config/map-silvermoon_sd length)
DOFFSET_Chst:=$(shell ../../src/map-util --blocks --hex --id Chst ../../config/map-silvermoon_sd doffset)
LENGTH_Chst:=$(shell ../../src/map-util --blocks --hex --id Chst ../../config/map-silvermoon_sd length)
DOFFSET_ubfb:=$(shell ../../src/map-util --blocks --hex --id ubfb ../../config/map-silvermoon_sd doffset)
LENGTH_ubfb:=$(shell ../../src/map-util --blocks --hex --id ubfb ../../config/map-silvermoon_sd length)

# This may not exist if we're invoked for clean
#PARTITIONS_EXIST:=$(shell test -f ../../output/partitions.mk && echo 1)
#ifeq ($(PARTITIONS_EXIST),1)
#include ../../output/partitions.mk
#endif

CHUMBY_CONFIG_FLAGS= -DCHUMBY_CONFIG_memsize=$(CHUMBY_CONFIG_memsize) \
	-DCHUMBY_CONFIG_scrheight=$(CHUMBY_CONFIG_scrheight) -DCHUMBY_CONFIG_scrwidth=$(CHUMBY_CONFIG_scrwidth) \
	-DCHUMBY_CONFIG_platform=\\\"$(CNPLATFORM)\\\" -DCHUMBY_CONFIG_name=\\\"$(CONFIGNAME)\\\" \
	-DCHUMBY_CNPLATFORM_${CNPLATFORM} -DCHUMBY_CONFIG_${CONFIGNAME} -DCHUMBY_CONFIG_production=$(CHUMBY_CONFIG_production) \
	-DCHUMBY_CONFIGNAME_$(ACONFIGNAME) -DCHUMBY_CONFIG_appoffset=\\\"$(APP_OFFSET)\\\" -DCHUMBY_CONFIG_badblock \
	-DCHUMBY_CONFIG_k1off=0x$(ROM_PARTITION_k1_OFF) -DCHUMBY_CONFIG_k2off=0x$(ROM_PARTITION_k2_OFF) \
	-DCHUMBY_CONFIG_rfs1=${ROM_PARTITION_rfs1_NUM} -DCHUMBY_CONFIG_rfs2=${ROM_PARTITION_rfs2_NUM} \
	-DCHUMBY_CONFIG_mspoff=0x$(ROM_PARTITION_msp_OFF) -DCHUMBY_CONFIG_mspoff_text=\\\"$(ROM_PARTITION_msp_OFF)\\\" \
	-DCHUMBY_DOFFSET_config=\\\"${DOFFSET_config}\\\" -DCHUMBY_LENGTH_config=\\\"${LENGTH_config}\\\" \
	-DCHUMBY_DOFFSET_Chst=\\\"${DOFFSET_Chst}\\\" -DCHUMBY_LENGTH_Chst=\\\"${LENGTH_Chst}\\\" \
	-DCHUMBY_DOFFSET_ubfb=\\\"${DOFFSET_ubfb}\\\" -DCHUMBY_LENGTH_ubfb=\\\"${LENGTH_ubfb}\\\"

# Must be a sibling of this dir
UBOOTDIR=../src
# Must be valid both here and in $(UBOOTDIR)
OBJDIR=../output/$(CONFIGNAME)
EXOUTDIR=$(OBJDIR)/examples
CONFIGFILE=$(UBOOTDIR)/chumby.mk

all:

help:
	@echo "Meta-targets:"
	@echo "help build-all clean-all commit"
	@echo "Build process uses:"
	@echo " make -C build clean-all PRODUCTION=1"
	@echo " make -C build build-all PRODUCTION=1"
	@echo " make -C build commit PRODUCTION=1"
	@echo "Utility targets:"
	@echo "diff trunk-merge bumpbuild"

$(DOT_CONFIG): $(OBJDIR)/include $(PARTITION_SIG)
	echo "# Generated config for $(CONFIGNAME) - do not edit, written by build/Makefile" > $(CONFIGFILE)
	echo "CONFIGNAME=$(CONFIGNAME)" >> $(CONFIGFILE)
	echo "CFLAGS+= $(CHUMBY_CONFIG_FLAGS)" >> $(CONFIGFILE)
	echo "CPPFLAGS+= $(CHUMBY_CONFIG_FLAGS)" >> $(CONFIGFILE)
	echo "AFLAGS+= $(CHUMBY_CONFIG_FLAGS)" >> $(CONFIGFILE)
	echo "OBJCFLAGS+= --remove-section=.framebuffer" >> $(CONFIGFILE)
	echo "# For some reason COMPILE.S is sometimes used in a way that works using AFLAGS, and at other times using ASFLAGS" >> $(CONFIGFILE)
	echo "ASFLAGS=\$$(AFLAGS)" >> $(CONFIGFILE)
	echo "# Required for generating ${OBJDIR}/include/autoconf.mk" >> ${CONFIGFILE}
	echo "HOST_CFLAGS+= ${CHUMBY_CONFIG_FLAGS}" >> ${CONFIGFILE}
	echo "# end of file" >> $(CONFIGFILE)
	@echo "Forcing regeneration of ${OBJDIR}/include/autoconf.mk*"
	-rm -f $(DOT_CONFIG_BASE)* ${OBJDIR}/include/autoconf.*
	touch $@

$(OBJDIR)/include/config.h: $(OBJDIR)/include $(DOT_CONFIG)
	$(MAKE) -C $(UBOOTDIR) O=$(abspath $(OBJDIR)) $(UBOOT_CONFIGNAME)_config

clean-target: $(DOT_CONFIG)
	@echo "Temporary clean of old files from source dir"
	$(MAKE) -C $(UBOOTDIR) clean
	@echo "end temporary clean"
	$(MAKE) -C $(UBOOTDIR) O=$(abspath $(OBJDIR)) clean

exports:

clean-exports:

build-target: uboot-symlinks $(OBJDIR)/u-boot.chumby

# Symlinks are created on the fly in the output copy except for the second level
uboot-symlinks: ../src/include/asm ../src/include/asm-arm/arch ../src/include/asm-arm/proc \
	${OBJDIR}/board/pxa/common/usbeth

../src/include/asm:
	ln -s asm-arm $@

../src/include/asm-arm/arch:
	ln -s arch-pxa168 $@

../src/include/asm-arm/proc:
	ln -s proc-armv $@

${OBJDIR}/board/pxa/common/usbeth:
	mkdir -p $@

UBOOT_BASEDEPS=
$(OBJDIR)/u-boot.chumby: $(OBJDIR)/u-boot.bin $(UBOOT_BASEDEPS)
	cp $(OBJDIR)/u-boot.bin $@

$(OBJDIR)/u-boot.bin: $(DOT_CONFIG) $(OBJDIR) $(OBJDIR)/include/config.h $(PARTITION_SIG)
	$(MAKE) -C $(UBOOTDIR) O=$(abspath $(OBJDIR)) BOOT_SEG=172
	cp $(OBJDIR)/u-boot.bin ../../output/images

$(PARTITION_SIG):
	-rm -f $(EXOUTDIR)/chumbystart*
	touch $@

# CONFIGNAME should already be exported. Previously we built all variants
# here to work around a build dependency problem.
build-all: build-target

clean-all: clean-target
	$(MAKE) clean-exports
	-rm -f $(DOT_CONFIG_BASE)*
	-rm -rf ${OBJDIR}/*

$(OBJDIR) $(OBJDIR)/include:
	-mkdir -p $@

.PHONY: all help clean exports clean-exports \
	build-all clean-all \
	build-target clean-target \
	commit \
	uboot-symlinks


# Makefile - makefile for target/config/pc486
#
# Copyright 1984-2002 Wind River Systems, Inc.
#
# modification history
# --------------------
# 02a,27oct03,fle  SPR#90904 : added POST_HEX_BUILD_RULE calls to .hex rules
# 01z,11dec02,hdn  revived for the I80486 support.
# 01y,30oct01,tpw  Standardize kernel links and bootrom conversions.
# 01x,25sep01,dat  Update for T2.2
# 01w,24mar98,dat  fixed rules for xxx_low, SPR 20837
# 01v,18mar98.dat  removed vxsys.c, SPR 8248
# 01u,03sep96,hdn  added the compression support.
# 01t,08aug96,ms   correct use of TGT_DIR
# 01s,29jul96,hdn  updated romcard_vxWorks_st_512.hex w vxWorks_boot.st.
# 01r,19jul96,hdn  added rules for BOOTABLE vxWorks.
#                  added mkboot.o to RELEASE.
# 01q,14nov95,hdn  added rules for romcard.
# 01p,14jun95,wrs  updated to 5.3 with bspTo5.3
# 01o,02jun95,ms   removed tyCoDrv added sysSerial
# 01n,27may95,jcf  added tyCoDrv.o to MACH_EXTRA
# 01m,07nov94,hdn  added a support for eex32.
# 01l,23may94,hdn  changed TARGET_DIR to pc486.
# 01k,28apr94,hdn  changed ROM_SIZE to 0x7fe00.
#		   added a link rule for romcard.s and romcard.sh.
#		   deleted a link rule for romInit0.s and romInit1.s.
# 01j,29mar94,hdn  changed a link rule.
# 01i,23feb94,hdn  swapped RAM_LOW_ADRS and RAM_HIGH_ADRS.
# 01h,27jan94,hdn  changed RAM_LOW_ADRS 0x10000 to 0x8000.
#                  changed RAM_HIGH_ADRS 0x110000 to 0x108000.
# 01g,23dec93,hdn  added SOURCES to support symbolic link.
# 01f,08dec93,hdn  added RELEASE.
# 01e,17nov93,hdn  changed ROM_SIZE to 0x50000.
# 01d,17jun93,hdn  updated to 5.1.
# 01c,09apr93,jdi  removed value for VENDOR and updated value of BOARD.
# 01b,07apr93,hdn  renamed compaq to pc.
# 01a,21may92,hdn  written.
#
# DESCRIPTION
# This file contains rules for building VxWorks for the
# PC 486.
#
# NOTE
# The constants ROM_TEXT_ADRS and RAM_HIGH_ADRS
# must be identical to those in config.h.
# 
# INCLUDES
#     makeTarget
#*/

CPU		= I80486
TOOL		= gnu

TGT_DIR = $(WIND_BASE)/target

include $(TGT_DIR)/h/make/defs.bsp
#include $(TGT_DIR)/h/make/make.$(CPU)$(TOOL)
#include $(TGT_DIR)/h/make/defs.$(WIND_HOST_TYPE)

## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.


TARGET_DIR = pc486
VENDOR     =
BOARD      = PC 386/486
BOOTCONFIG = bootConfig.c

#
# The constants ROM_TEXT_ADRS, ROM_SIZE, and RAM_HIGH_ADRS are defined
# in config.h, MakeSkel, Makefile, and Makefile.*
# All definitions for these constants must be identical.
#

# ifdef BOOTCODE_IN_RAM
ROM_TEXT_ADRS      = 00010000	# ROM entry address - A: or C:
ROM_SIZE           = 00090000	# number of bytes of ROM space
# else
# ROM_TEXT_ADRS      = fff20000	# ROM entry address - EPROM
# ROM_SIZE           = 0007fe00	# number of bytes of ROM space
# endif

RAM_LOW_ADRS       = 00108000	# VxWorks image entry point
RAM_HIGH_ADRS      = 00010000	# Boot image entry point

EXTRA_DEFINE       = 		# -DFAST_REBOOT

MACH_EXTRA         =


RELEASE += bootrom_uncmp.bin bootrom.bin mkboot.o


## Only redefine make definitions above this point, or the expansion of 
## makefile target dependencies may be incorrect.

include $(TGT_DIR)/h/make/rules.bsp
#include $(TGT_DIR)/h/make/rules.$(WIND_HOST_TYPE)


# Following bootable vxWorks should be placed in bootable diskette
# by "mkboot" or "mkbootFd, mkbootAta" utility.
#
#   vxWorks.st_rom	- bootable vxWorks.st(compressed): upper mem
#   bootrom		- bootrom(compressed):		   upper mem
#   bootrom_uncmp	- bootrom:			   upper mem
#
# The boot image is either bootrom_uncmp or vxWorks.st_rom.
# If the EPROM is 27020 (256 Kbytes)
#	$(BINHEX) -a 3fe00 -b romcard >> $@
# If the EPROM is 27040 (512 Kbytes)
#	$(BINHEX) -a 7fe00 -b romcard >> $@

SED	= sed

romcard_bootrom_512.hex:	depend.$(BSP_NAME) romcard.o bootrom_uncmp
	- @ $(RM) $@
	$(LD) -o romcard -X -N -e _romcard romcard.o
	$(ROMSIZEPROG) -b 7fe00 bootrom_uncmp
	$(BINHEX) -a 0 bootrom_uncmp | $(SED) -e "/S9/d" > $@
	$(BINHEX) -a 7fe00 -b romcard >> $@
	- @ $(RM) romcard.o romcard
	$(POST_HEX_BUILD_RULE)

romcard_bootrom_256.hex:	depend.$(BSP_NAME) romcard.o bootrom_uncmp
	- @ $(RM) $@
	$(LD) -o romcard -X -N -e _romcard romcard.o
	$(ROMSIZEPROG) -b 3fe00 bootrom_uncmp
	$(BINHEX) -a 0 bootrom_uncmp | $(SED) -e "/S9/d" > $@
	$(BINHEX) -a 3fe00 -b romcard >> $@
	- @ $(RM) romcard.o romcard
	$(POST_HEX_BUILD_RULE)

romcard_vxWorks_st_512.hex:	depend.$(BSP_NAME) romcard.o vxWorks.st_rom
	- @ $(RM) $@
	$(LD) -o romcard -X -N -e _romcard romcard.o
	$(ROMSIZEPROG) -b 7fe00 vxWorks.st_rom
	$(BINHEX) -a 0 vxWorks.st_rom | $(SED) -e "/S9/d" > $@
	$(BINHEX) -a 7fe00 -b romcard >> $@
	- @ $(RM) romcard.o romcard
	$(POST_HEX_BUILD_RULE)


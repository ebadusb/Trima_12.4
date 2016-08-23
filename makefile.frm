# $Header: E:/BCT_Development/Trima5.R/Trima/rcs/makefile.frm 1.3 2008/01/23 17:08:19Z dslausb Exp dslausb $
# $Log: makefile.frm $
# Revision 1.3  2008/01/23 17:08:19Z  dslausb
# IT 8435 - VxWorks Checkin
# Initial revision

PRODUCTION_COM_DIR := \\bctquad3\HOME\BCT_Development\Trima5.R\Trima\common_builds
PRODUCTION_COM_DIR_ECHO := $(subst \,/, $(PRODUCTION_COM_DIR))
LCBD := latest_common_build_dir.mk
LRMBD := latest_rmsg_build_dir.mk
LCRMBD := $(patsubst .%,$(strip $(PRODUCTION_COM_DIR_ECHO))%, $(shell cat $(PRODUCTION_COM_DIR_ECHO)/$(LCBD)))/.rmsg

.PHONY: find_rmsg
find_rmsg:
	@rm -f -r .rmsg
	@mkdir .rmsg
	@if exist $(LRMBD) (echo Default Reserved Message directory overridden\n)
	@if exist $(LRMBD) (echo $(patsubst .%,$(shell cmd /c cd)\.%, $(shell cmd /c if exist $(LRMBD) cat $(LRMBD))) > .rmsg\$(LRMBD))
	@if not exist $(LRMBD) (echo Default Reserved Message directory used) $(cat $(LCRMBD)/$(LRMBD))
	@if not exist $(LRMBD) (echo $(patsubst .%,$(strip $(PRODUCTION_COM_DIR_ECHO))%, $(shell cat $(LCRMBD)/$(LRMBD))) > .rmsg\$(LRMBD))

.PHONY: verify_rmsg
verify_rmsg:
	@rm -f .rmsg/status
	@if not exist $(RMSGDIR)\create_rmsg.bat echo File create_rmsg.bat not found > .rmsg\status
	@if not exist $(RMSGDIR)\MessageGenerator.exe echo File MessageGenerator.exe not found > .rmsg\status
	@if exist .rmsg\status cat .rmsg\status
	@if not exist .rmsg\status echo OK

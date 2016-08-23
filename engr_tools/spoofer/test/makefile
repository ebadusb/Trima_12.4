# FILENAME: makefile
# $Header: L:/BCT_Development/Trima/engr_tools/spoofer/test/rcs/makefile 1.2 2000/03/08 20:43:40 TD07711 Exp td07711 $
# PURPOSE: builds spoofer test executables
# CHANGELOG:
# $Log: makefile $
# Revision 1.2  2000/03/08 20:43:40  TD07711
# Revision 1.1  2000/01/13 22:16:52  TD07711
# Initial revision
# 12/30/99 - dyes - initial version


# define include files
TRIMARULES=../../../trimarules.mk
TRIMASHAREDTARGETS=../../../trimasharedtargets.mk

# pull in trima project make environment
include $(TRIMARULES)
# override local_top
LOCAL_TOP=../../..


PROGRAMS=co01 cs01 so01 ss01

SRCS=co01.cpp cs01.cpp so01.cpp ss01.cpp
OBJS=${SRCS:.cpp=.o}

all: $(PROGRAMS)


co01 : co01.o
	$(LD) -o $@ $(LDFLAGS) $@.o $(ROUTERLIB) -l$(LOCAL_TOP)/engr_tools/lib/libtools

cs01 : cs01.o
	$(LD) -o $@ $(LDFLAGS) $@.o $(ROUTERLIB) -l$(LOCAL_TOP)/engr_tools/lib/libtools

so01 : so01.o
	$(LD) -o $@ $(LDFLAGS) $@.o $(ROUTERLIB) -l$(LOCAL_TOP)/engr_tools/lib/libtools

ss01 : ss01.o
	$(LD) -o $@ $(LDFLAGS) $@.o $(ROUTERLIB) -l$(LOCAL_TOP)/engr_tools/lib/libtools


# install/patch defines
TODIR=
INSTALLED=


# includes other targets (clean, install, patch, etc)
# and pulls in compile and link options
include $(TRIMASHAREDTARGETS)
INCLS += $(ID)$(LOCAL_TOP)/engr_tools/include


## dependencies will be put after this line... ##
include dependencies.mk

# FILENAME: makefile
# PURPOSE: builds raw_atod tool
# CHANGELOG:
# $Header$
# $Log$

# define include files
TRIMARULES=../../trimarules.mk
TRIMASHAREDTARGETS=../../trimasharedtargets.mk

# pull in trima project make environment
include $(TRIMARULES)
LOCAL_TOP=../..

SRCS=raw_atod.cpp
OBJS=${SRCS:.cpp=.o}
PROGRAMS = raw_atod

all: $(PROGRAMS)

raw_atod :  raw_atod.o
        $(LD) $(LDFLAGS) -o $@ raw_atod.o \
        $(ROUTERLIB) $(TCOMMON_LIB) \
        -l$(LOCAL_TOP)/engr_tools/lib/libtools \
        -lncurses

# setup trimasharedtargets.mk

# install
TODIR=tools
INSTALLED=$(PROGRAMS)

# include clean & other targets
include $(TRIMASHAREDTARGETS)
INCLS += $(ID)$(LOCAL_TOP)/engr_tools/include $(ID)$(LOCAL_TOP)/low_level


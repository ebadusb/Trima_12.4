# FILENAME: makefile
# PURPOSE: builds latency_test tool
# CHANGELOG:
# $Header$
# $Log$
#

# define include files
TRIMARULES=../../trimarules.mk
TRIMASHAREDTARGETS=../../trimasharedtargets.mk

# pull in trima project make environment
include $(TRIMARULES)
LOCAL_TOP=../..

SRCS=latency_test.cpp
OBJS=${SRCS:.cpp=.o}
PROGRAMS = latency_test

all: $(PROGRAMS)

latency_test :  latency_test.o
        $(LD) $(LDFLAGS) -o $@ latency_test.o \
        $(ROUTERLIB) $(TCOMMON_LIB) \
        -l$(LOCAL_TOP)/engr_tools/lib/libtools


# setup trimasharedtargets.mk

# install
TODIR=tools
INSTALLED=$(PROGRAMS)

# include clean & other targets
include $(TRIMASHAREDTARGETS)
INCLS += $(ID)$(LOCAL_TOP)/engr_tools/include $(ID)$(LOCAL_TOP)/low_level


#
# makefile for guitest
# $Header :$
# NB:Do NOT save & try to run this file with DOS CR/LFs
# Set MKS option Configuration->Personal->Checkout->UNIX
# symptom:cant open ../trimarules.mk
#

# define include files
TRIMARULES=../../trimarules.mk
TRIMASHAREDTARGETS=$(LOCAL_TOP)/trimasharedtargets.mk

# pull in trima project make environment
include $(TRIMARULES)
LOCAL_TOP=../..

INCLS+=$(ID)$(LOCAL_TOP)/gui $(ID)$(LOCAL_TOP)/tcpip $(ID)$(LOCAL_TOP)/service

# we use a non-portable stacksize directive
STACKSIZE=64
include $(LOCAL_TOP)/stacksize.mk

SRCS=xcommmsg.cpp
OBJS=${SRCS:.cpp=.o}

PROGRAMS=xcommmsg

xcommmsg: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ $(ROUTERLIB) $(TCOMMON_LIB) $(GUI_LIB) $(QNXLIBS)

# install
TODIR=tools
INSTALLED=$(PROGRAMS)

# include clean & other targets
include $(TRIMASHAREDTARGETS)


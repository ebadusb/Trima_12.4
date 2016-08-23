# FILENAME: makefile
# PURPOSE: builds timingtool utility
# Changelog:
#   $Header$
#   $Log$
#
# NB:Do NOT save & try to run this file with DOS CR/LFs
# Set MKS option Configuration->Personal->Checkout->UNIX
# symptom:cant open ../trimarules.mk
#

# define include files
TRIMARULES=../../trimarules.mk
TRIMASHAREDTARGETS=../../trimasharedtargets.mk

# pull in trima project make environment
include $(TRIMARULES)

LOCAL_TOP=../..
PROGRAMS = timingtool
SRCS = timingtool.cpp
OBJS=${SRCS:.cpp=.o}

all: $(PROGRAMS)

timingtool: timingtool.o
        $(LD) $(LDFLAGS) -o timingtool timingtool.o
        wstrip timingtool timingtool timingtool.sym

# setup trimasharedtargets.mk

# install
TODIR=tools
INSTALLED=$(PROGRAMS) 

# include clean & other targets
include $(TRIMASHAREDTARGETS)


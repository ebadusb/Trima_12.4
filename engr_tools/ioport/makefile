# FILENAME: makefile
# PURPOSE: builds ioport utility
# Changelog:
#   $Header$
#   $Log$
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
PROGRAMS = ioport
SRCS = ioport.c
OBJS=${SRCS:.c=.o}

all: $(PROGRAMS)

ioport: ioport.o
        $(LD) $(LDFLAGS) -o ioport ioport.o
        wstrip ioport ioport ioport.sym

# setup trimasharedtargets.mk

# install
TODIR=tools
INSTALLED=$(PROGRAMS) 

# include clean & other targets
include $(TRIMASHAREDTARGETS)


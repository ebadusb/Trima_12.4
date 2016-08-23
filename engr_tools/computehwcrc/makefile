# FILENAME: makefile
# PURPOSE: builds computehwcrc utility
# Changelog:
#   $Header: K:/BCT_Development/Trima/engr_tools/computehwcrc/rcs/makefile 1.1 2001/07/24 19:48:54 jl11312 Exp jl11312 $
#   $Log: makefile $
#   Revision 1.1  2001/07/24 19:48:54  jl11312
#   Initial revision
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
PROGRAMS = computehwcrc
SRCS = computehwcrc.cpp
OBJS=${SRCS:.cpp=.o}

all: $(PROGRAMS)

computehwcrc: computehwcrc.o $(LIBDIR)/$(CRC_LIB)
        $(LD) $(LDFLAGS) -o computehwcrc computehwcrc.o $(CRC_LIB)
        wstrip computehwcrc computehwcrc computehwcrc.sym

# setup trimasharedtargets.mk

# install
TODIR=tools
INSTALLED=$(PROGRAMS) 

# include clean & other targets
include $(TRIMASHAREDTARGETS)


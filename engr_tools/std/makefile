# FILENAME: makefile
# $Header: J:/BCT_Development/Trima/engr_tools/std/rcs/makefile 1.4 2000/03/09 21:52:38 TD07711 Exp hr10414 $
# PURPOSE: builds std (simple test driver)
# CHANGELOG:
# $Log: makefile $
# Revision 1.4  2000/03/09 21:52:38  TD07711
# IT4580
# Revision 1.4  2000/01/13 21:23:05  TD07711
#   01/12/2000 - dyes - update .o dependencies
#   11/7/99 - dyes - initial version
#   12/2/99 - dyes - moved to tools subproject

PRODUCT = std
INSTALL_PATH = ../bin


LIB_PATH = ../lib
INCLUDE_PATH = ../include
CC = wpp386
LD = cc
INCLUDES = -I/usr/include -I./ -I$(INCLUDE_PATH) -I../../stl
CFLAGS = -zq -w3 -d2 -mf -g $(INCLUDES)
LDFLAGS = -b -g -M


OBJS = main.o \
	suite.o \
	test.o

LIBS = -l$(LIB_PATH)/libtools


all: $(PRODUCT)

clean:
    touch foo.o foo.map foo.sym 
    rm -f $(PRODUCT) *.o *.map *.sym

install:
    rm -f $(INSTALL_PATH)/$(PRODUCT)
    cp -p $(PRODUCT) $(INSTALL_PATH)	

$(PRODUCT): $(OBJS)
    $(LD) -o $(PRODUCT) $(LDFLAGS) $(OBJS) $(LIBS)

# generic compile rule
.cpp.o:
    $(CC) $(CFLAGS) $*.cpp

#
# dependencies
#
main.o: main.cpp version.h
suite.o: suite.cpp suite.hpp version.h test.hpp
test.o: test.cpp test.hpp suite.hpp







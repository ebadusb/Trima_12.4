/*
 * Copyright (c) 1995-1999 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      msginfo.hpp, definition of structures in shared memory
 *             used to communicate between the driver's messaging task
 *             and the driver.
 *             these structures can be used in both the safety and
 *             control drivers.
 *
 * ABSTRACT:   Define message identification information in shared memory
 *             message areas.
 *
 */

#ifndef MSGINFO_HPP
#define MSGINFO_HPP

#include "fastdata.h"


// structures for fastdata packets
const int NUM_FASTPACK = 10;

typedef struct
{
   FASTDATA      data;
   unsigned long crc;
}FASTEntry;

typedef struct
{
   FASTEntry msg[NUM_FASTPACK];
   int       count;
   int       NextIn;
   int       NextOut;
   BOOL      FIFOIsActive;
}FASTPAK;


// structures for fastdatalong packets
const int NUM_FASTPACKLONG = 10;

typedef struct
{
   FASTDATALONG  data;
   unsigned long crc;
}FASTLONGEntry;

typedef struct
{
   FASTLONGEntry msg[NUM_FASTPACKLONG];
   int           count;
   int           NextIn;
   int           NextOut;
   BOOL          FIFOIsActive;
}FASTPAKLONG;

#endif   // MSGINFO_HPP

/* FORMAT HASH 3919806abb4622a4185c644451d441cf */

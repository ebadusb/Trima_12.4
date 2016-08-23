/*
 * Copyright (c) 1995-1999 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: K:/BCT_Development/vxWorks/Trima/low_level/safe_drv/rcs/smsginfo.hpp 1.5 2002/10/31 18:28:26Z pn02526 Exp jl11312 $
 * $Log: smsginfo.hpp $
 * Revision 1.5  2002/10/31 18:28:26Z  pn02526
 * Massive checkin for integration testing.  Changes BOOL to bool, and forces build to be for 486, only.
 * Revision 1.4  2002/08/13 17:38:34  pn02526
 * Use message system's definitions of pid (process a.k.a. task Id) and nid (node Id).
 * Revision 1.3  2001/04/05 14:16:19  jl11312
 * - internal timer handling changes required for versa logic CPU board
 * Revision 1.2  2000/08/07 23:00:45  bs04481
 * Allow visibility to 10ms data used/created by the drivers.
 * See IT 4741
 * Revision 1.1  1999/09/30 04:14:02  BS04481
 * Initial revision
 *
 * TITLE:      smsginfo.hpp, definition of structures in shared memory
 *             used to communicate between the safety messaging task
 *             and the safety driver.
 *
 * ABSTRACT:   Define message identification information in shared memory
 *             message areas.
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 * PUBLIC FUNCTIONS:
 *
 *
 */

#ifndef SMSGINFO_HPP
#define SMSGINFO_HPP

#include "shw.hpp"
#include "basin.hpp"

// structure for basin temp data
const int NUM_BASIN = 5;

typedef struct
{
   basinTemp     data;
   unsigned long crc;
}BASEntry;

typedef struct
{
   BASEntry msg[NUM_BASIN];
   int      NextIn;
   int      NextOut;
}BAS;


// structure for command packets
const int NUM_SHWORDERS = 10;

typedef struct
{
   SHwOrders       orders;
   unsigned long   nid;
   unsigned long   pid;
   struct timespec time;
   unsigned long   crc;
}SHOEntry;

typedef struct
{
   SHOEntry msg[NUM_SHWORDERS];
   int      NextIn;
   int      NextOut;
}SHO;


// structure for request status packets
const int NUM_SREQUEST = 10;

typedef struct
{
   int counter[NUM_SREQUEST];
   int NextIn;
   int NextOut;
}RS;


// structure for status packets
const int NUM_SHWSTATUS = 20;

typedef struct
{
   SHwStates     status;
   unsigned long crc;
}SHSEntry;

typedef struct
{
   SHSEntry msg[NUM_SHWSTATUS];
   int      NextIn;
   int      NextOut;

   //
   // Set by safe_drv to indicate that a timing error has been detected.
   // It should be cleared by safe_msg when the associated A2 alarm
   // has been set.
   //
   bool timingError;
}SHS;


#endif // SMSGINFO_HPP

/* FORMAT HASH d3120b667fc9ac6bd67c38b027232afc */

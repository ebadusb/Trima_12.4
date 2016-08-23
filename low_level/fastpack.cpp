/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      fastpack.hpp, Fast data from aps, pumps, etc
 *
 * ABSTRACT:   Data collection for fata data types.  Data is collected
 *             in shared memory and is available for sending by
 *             the high priority message task
 *
 */

#include <vxWorks.h>
#include <string.h>

#include "crcgen.h"
#include "fastpack.hpp"
#include "sh_mem.h"
#include "trima_assert.h"
#include "trima_datalog.h"

// SPECIFICATION:    used to collect access pressure data,
//                   and ultrasonic data.
//                   fastpack constructor creates shared memory
//
// ERROR HANDLING:   none.


fastpack::fastpack(DATA_SOURCE src, const char* sharename)
{
   TRIMA_ASSERT(sharename);

   startTime.sec = 0; startTime.nanosec = 0;

   // map the share memory objects
   _FPtr = (FASTPAK*)shmem_create(sharename, sizeof(FASTPAK));
   if ( !_FPtr )
   {
      DataLog_Critical critical;

      DataLog(critical) << sharename << " shmem_create failed" << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "shmem_create");
   }

   // initialize the data area
   memset(_FPtr, 0, sizeof(FASTPAK));

   // set the source field
   for (int link = 0; link<NUM_FASTPACK; link++)
      _FPtr->msg[link].data.source = src;

   // initialize the indices
   _FPtr->NextIn  = 0;
   _FPtr->NextOut = 0;
   _FPtr->count   = 0;

   // default is to not send a message
   _FPtr->FIFOIsActive = FALSE;


}

// SPECIFICATION:    fastpack destructor
//
//
// ERROR HANDLING:   none.

fastpack::~fastpack()
{
   _FPtr = NULL;
}

// SPECIFICATION:    fastpack data collector to
//                   shared memory
//
//
// ERROR HANDLING:   none.

void fastpack::dataPoint (short data)
{
   // initialize just-in-time to align start of time series as
   // close to zero as possible
   if ( (startTime.sec == 0) && (startTime.nanosec == 0) )
      osTime::snapshotRawTime(startTime);

   _FPtr->msg[_FPtr->NextIn].data.raw[_FPtr->count] = data;

   // SEP in order to time-scale and analyze the data, we need
   // a time spec associated with each reading
   _FPtr->msg[_FPtr->NextIn].data.time[_FPtr->count] =
      osTime::howLongMilliSecAndUpdate(startTime);

   _FPtr->count++;

   if ( _FPtr->count >= FASTDATALENGTH )
   {
      _FPtr->msg[_FPtr->NextIn].crc = 0;
      if ( crcgen32(&_FPtr->msg[_FPtr->NextIn].crc, (unsigned char*)&_FPtr->msg[_FPtr->NextIn], sizeof(FASTDATA)) != 0 )
      {
         _FATAL_ERROR(__FILE__, __LINE__, "crcgen32");
      }
      else
      {
         // test for lapping NextOut
         // this is optional data so only test the lap
         // if someone is retrieving the data
         if ( (_FPtr->NextIn + 1 == _FPtr->NextOut) &&
              (_FPtr->FIFOIsActive == TRUE) )
            _FATAL_ERROR(__FILE__, __LINE__, "Fastpack NextIn lapping NextOut!");

         // increment NextIn index
         if (_FPtr->NextIn == NUM_FASTPACK - 1)
            _FPtr->NextIn = 0;
         else
            _FPtr->NextIn++;
         _FPtr->count = 0;
      }
   }
}

// SPECIFICATION:    used to collect pump 10ms data
//                   fastpacklong constructor creates shared memory
//
//
// ERROR HANDLING:   none.

fastpacklong::fastpacklong(DATA_SOURCE src, const char* sharename)
{
   TRIMA_ASSERT(sharename);

   startTime.sec = 0; startTime.nanosec = 0;

   // map the share memory objects
   _FPtr = (FASTPAKLONG*)shmem_create(sharename, sizeof(FASTPAKLONG));
   if ( !_FPtr )
   {
      DataLog_Critical critical;

      DataLog(critical) << sharename << " shmem_create failed" << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "shmem_create");
   }

   // SEP: re-arranged initialization code.  Previous version performed memset after
   // source type was set, effectively removing the source header for our payloads

   // initialize the data area
   memset(_FPtr, 0, sizeof(FASTPAKLONG));

   // initialize the indices
   _FPtr->NextIn  = 0;
   _FPtr->NextOut = 0;
   _FPtr->count   = 0;

   // default is to not send a message
   _FPtr->FIFOIsActive = FALSE;

   // set the source type
   for (int link = 0; link<NUM_FASTPACKLONG; link++)
      _FPtr->msg[link].data.source = src;
}

// SPECIFICATION:    fastpacklong destructor
//
//
// ERROR HANDLING:   none.

fastpacklong::~fastpacklong()
{
   _FPtr = NULL;
}


// SPECIFICATION:    fastpacklong data collector to
//                   shared memory
//
//
// ERROR HANDLING:   none.

void fastpacklong::dataPoint (long data)
{
   // initialize just-in-time to align start of time series as
   // close to zero as possible
   if ( (startTime.sec == 0) && (startTime.nanosec == 0) )
      osTime::snapshotRawTime(startTime);

   _FPtr->msg[_FPtr->NextIn].data.raw[_FPtr->count] = data;

   // SEP in order to time-scale and analyze the data, we need
   // a time spec associated with each reading
   _FPtr->msg[_FPtr->NextIn].data.time[_FPtr->count] =
      osTime::howLongMilliSecAndUpdate(startTime);

   _FPtr->count++;

   if ( _FPtr->count >= FASTDATALENGTHFORLONGS )
   {
      _FPtr->msg[_FPtr->NextIn].crc = 0;
      if ( crcgen32(&_FPtr->msg[_FPtr->NextIn].crc, (unsigned char*)&_FPtr->msg[_FPtr->NextIn], sizeof(FASTDATALONG)) != 0 )
      {
         _FATAL_ERROR(__FILE__, __LINE__, "crcgen32");
      }
      else
      {
         // test for lapping NextOut
         // this is optional data so only test the lap
         // if someone is retrieving the data
         if ( (_FPtr->NextIn + 1 == _FPtr->NextOut) &&
              (_FPtr->FIFOIsActive == TRUE) )
            _FATAL_ERROR(__FILE__, __LINE__, "Fastpacklong NextIn lapping NextOut!");

         // increment NextIn index
         if (_FPtr->NextIn == NUM_FASTPACKLONG - 1)
            _FPtr->NextIn = 0;
         else
            _FPtr->NextIn++;
         _FPtr->count = 0;
      }
   }
}

/* FORMAT HASH 7ca3597e3d44a6394dddfa99171e1a3c */

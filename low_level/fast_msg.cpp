/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      fast_msg.cpp  classes for optional and low-priority
 *             fast data messages used by the control and safety drivers
 *
 */

#include <vxWorks.h>

#include "crcgen.h"
#include "fast_msg.hpp"
#include "sh_mem.h"
#include "trima_assert.h"
#include "trima_datalog.h"

// SPECIFICATION:    fastdataMessage constructor
//
// ERROR HANDLING:   None.

fastdataMessage::fastdataMessage(const char* sharename)
   : _msgInitialized(false)
{
   int retry = 0;
   TRIMA_ASSERT(sharename);

   // open shared memory for communication to the driver task
   _FPtr = NULL;
   while ( !_FPtr && retry < 100 )
   {
      _FPtr = (FASTPAK*)shmem_open(sharename);
      if ( !_FPtr )
      {
         taskDelay(5);
         retry += 1;
      }
   }

   if ( !_FPtr )
   {
      DataLog_Critical critical;

      DataLog(critical) << sharename << " shmem_open failed" << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "shmem_open");
   }

}


// SPECIFICATION:    fastdataMessage destructor
//
// ERROR HANDLING:   None.

fastdataMessage::~fastdataMessage()
{
   _FPtr = NULL;
}

// SPECIFICATION:    fastdataMessage update
//                   Sends the fastdataMessageif the
//                   indices indicate new data has been
//                   set by the driver
//
// ERROR HANDLING:   None.

void fastdataMessage::update ()
{
   if ( (_FPtr->NextIn != _FPtr->NextOut) &&
        (_FPtr->FIFOIsActive == TRUE) )
   {
      unsigned long checksum = 0;

      if ( crcgen32(&checksum, (unsigned char*)&_FPtr->msg[_FPtr->NextOut], sizeof(FASTDATA)) != 0 )
      {
         _FATAL_ERROR(__FILE__, __LINE__, "crcgen32");
      }
      else if ( checksum != _FPtr->msg[_FPtr->NextOut].crc )
      {
         DataLog_Critical critical;

         DataLog(critical) << "fastdata CRC mismatch: expected=" << checksum <<
            " received=" << _FPtr->msg[_FPtr->NextOut].crc << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "fastdata CRC");
      }
      else
      {
         _msg.send(_FPtr->msg[_FPtr->NextOut].data);
      }

      if ( _FPtr->NextOut >= NUM_FASTPACK - 1 )
         _FPtr->NextOut = 0;
      else
         _FPtr->NextOut++;
   }
}


// SPECIFICATION:    fastdataMessage activate
//                   Enables the sending of the fastdataMessage.
//                   The data is always put in shared memory
//                   but it is not picked up and sent unless enabled.
//
// ERROR HANDLING:   None.

void fastdataMessage::activate ()
{
   if ( !_msgInitialized )
   {
      _msg.init(MessageBase::SEND_LOCAL);
      _msgInitialized = true;
   }

   //
   // Clear any existing data and activate
   //
   _FPtr->NextOut      = _FPtr->NextIn;
   _FPtr->FIFOIsActive = TRUE;

   DataLog_Default << "enable fastdata message" << endmsg;
}

// SPECIFICATION:    fastdataMessage deactivate
//                   Stops the sending of the fastdataMessage.
//                   The data is always put in shared memory
//                   but it is not picked up and sent unless enabled.
//
// ERROR HANDLING:   None.

void fastdataMessage::deactivate ()
{
   // stop this object from sending messages
   _FPtr->FIFOIsActive = FALSE;

   DataLog_Default << "disable fastdata message" << endmsg;
}

// SPECIFICATION:    fastdataMessage toggle
//                   If activated, then deactivate.
//                   If deactivated, then activate.
//
// ERROR HANDLING:   None.

void fastdataMessage::toggle ()
{
   if ( _FPtr->FIFOIsActive == FALSE )
      activate();
   else
      deactivate();
}


// SPECIFICATION:    fastdatalongMessage constructor
//
// ERROR HANDLING:   None.

fastdatalongMessage::fastdatalongMessage(const char* sharename)
   : _msgInitialized(false)
{
   int retry = 0;
   TRIMA_ASSERT(sharename);

   // open shared memory for communication to the driver task
   _FPtr = NULL;
   while ( !_FPtr && retry < 100 )
   {
      _FPtr = (FASTPAKLONG*)shmem_open(sharename);
      if ( !_FPtr )
      {
         taskDelay(5);
         retry += 1;
      }
   }

   if ( !_FPtr )
   {
      DataLog_Critical critical;

      DataLog(critical) << sharename << " shmem_open failed" << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "shmem_open");
   }
}


// SPECIFICATION:    fastdatalongMessage destructor
//
// ERROR HANDLING:   None.

fastdatalongMessage::~fastdatalongMessage()
{
   _FPtr = NULL;
}

// SPECIFICATION:    fastdatalongMessage update
//                   Sends the fastdataMessageif the
//                   indices indicate new data has been
//                   set by the driver
//
// ERROR HANDLING:   None.

void fastdatalongMessage::update ()
{
   if ( (_FPtr->NextIn != _FPtr->NextOut) &&
        (_FPtr->FIFOIsActive == TRUE) )
   {
      unsigned long checksum = 0;

      if ( crcgen32(&checksum, (unsigned char*)&_FPtr->msg[_FPtr->NextOut], sizeof(FASTDATALONG)) != 0 )
      {
         _FATAL_ERROR(__FILE__, __LINE__, "crcgen32");
      }
      else if ( checksum != _FPtr->msg[_FPtr->NextOut].crc )
      {
         DataLog_Critical critical;

         DataLog(critical) << "fastdatalong CRC mismatch: expected=" << checksum <<
            " received=" << _FPtr->msg[_FPtr->NextOut].crc << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "fastdata CRC");
      }
      else
      {
         _msg.send(_FPtr->msg[_FPtr->NextOut].data);
      }

      if ( _FPtr->NextOut >= NUM_FASTPACKLONG - 1 )
         _FPtr->NextOut = 0;
      else
         _FPtr->NextOut++;
   }
}

// SPECIFICATION:    fastdatalongMessage activate
//                   Enables the sending of the fastdatalongMessage.
//                   The data is always put in shared memory
//                   but it is not picked up and sent unless enabled.
//
// ERROR HANDLING:   None.

void fastdatalongMessage::activate ()
{
   if ( !_msgInitialized )
   {
      _msg.init(MessageBase::SEND_LOCAL);
      _msgInitialized = true;
   }

   //
   // Clear any existing data and activate
   //
   _FPtr->NextOut      = _FPtr->NextIn;
   _FPtr->FIFOIsActive = TRUE;

   DataLog_Default << "enable fastdatalong message" << endmsg;
}

// SPECIFICATION:    fastdatalongMessage deactivate
//                   Stops the sending of the fastdatalongMessage.
//                   The data is always put in shared memory
//                   but it is not picked up and sent unless enabled.
//
// ERROR HANDLING:   None.

void fastdatalongMessage::deactivate ()
{
   // stop this object from sending messages
   _FPtr->FIFOIsActive = FALSE;

   DataLog_Default << "disable fastdatalong message" << endmsg;
}


// SPECIFICATION:    fastdatalongMessage toggle
//                   If activated, then deactivate.
//                   If deactivated, then activate.
//
// ERROR HANDLING:   None.

void fastdatalongMessage::toggle ()
{
   if ( _FPtr->FIFOIsActive == FALSE )
      activate();
   else
      deactivate();
}

/* FORMAT HASH 19800e40f85d027ee6fc2a688459dc52 */

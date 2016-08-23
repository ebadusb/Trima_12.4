/*
 * Copyright (c) 1995-1999 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/low_level/safe_drv/rcs/basindata.cpp 1.18 2003/06/19 19:44:52Z jl11312 Exp dslausb $
 * $Log: basindata.cpp $
 * Revision 1.18  2003/06/19 19:44:52Z  jl11312
 * - moved hardware interface to separate modules so that it is available earlier in the boot process
 * Revision 1.17  2003/04/29 17:02:55Z  jl11312
 * - put shmem_create calls in driver and shmem_open calls in other modules to remove timing dependencies on startup
 * Revision 1.16  2003/03/28 19:48:12Z  jl11312
 * - modified to use new data log levels
 * Revision 1.15  2002/12/26 21:51:02Z  pn02526
 * Runnable safety driver with CRC checking of the code and FATAL timing errors disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.  Compiling with ENABLE_FATAL_TIMING_ERRORS defined will enable timing errors to shut the system down.
 * Revision 1.14  2002/12/17 09:25:57  pn02526
 * Checkins for first operating safety driver under vxWorks
 * Revision 1.13  2002/10/31 18:28:26Z  pn02526
 * Massive checkin for integration testing.  Changes BOOL to bool, and forces build to be for 486, only.
 * Revision 1.12  2002/08/13 13:36:55  pn02526
 * Remove extraneous #includes of numerous system files.
 * Revise logging to use the DataLog_Level class through the DataLog_Default macro.
 * Revise FATAL_ERROR calls to use the new one.
 * Change ASSERT() macro calls to trima_assert().
 * Substitute the temporary shared memory emulation for the QNX one.
 * Remove _fd_* members, since they are not used by the shared memory emulation.
 * Insert shmem_close calls into destructor functions for classes that open shared memory.
 * To keep GNU compiler happy for production builds:
 * + Change the type of the buffer pointer argument iin all crcgen32 calls to const unsigned char *.
 * + Change the order of member initialization in basinTimer::basinTimer.
 * Add basinTimer ::_myTimer to use the new timer message class definition,
 *    rather than deriving from a timer message class.
 * Remove the check for time reversing as a result of time-of-day changes,
 *   since it is unnecessary on vxWorks.
 * Revision 1.11  2000/05/05 15:38:01  BS04481
 * Change log_error function
 * Revision 1.10  2000/03/17 16:41:08  BS04481
 * Non-essential issues from 3.3 code review
 * Revision 1.9  1999/10/08 18:34:37  BS04481
 * Reference the variable that controls the watchdogs during time
 * setting via a function instead of directly.
 * Revision 1.8  1999/09/30 04:07:22  BS04481
 * Remove message send and receive functions from the driver
 * service loop.
 * Revision 1.7  1999/09/17 06:38:44  BS04481
 * Removed kernel calls to clock_****, qnx_****, dev_****, write
 * and delay functions in an effort to get better determinism in the
 * driver service loops
 * Revision 1.6  1999/08/13 14:24:40  BS04481
 * Remove logging
 * Revision 1.5  1999/08/06 14:56:14  BS04481
 * Go back to divide by 16 instead of divide by 2 because hardware
 * has been fixed to match.
 * Revision 1.4  1999/08/03 18:45:31  BS04481
 * Increase idle time between packets to at least 500ms.  Remove
 * idle time between bytes.   Force resync at start and when data
 * errors are detected.  Enable error detection for bad start bit, bad
 * stop bit and bad CRC on the packet.
 * Revision 1.3  1999/07/30 17:01:39  BS04481
 * Fixed problem where if communication stopped in an error state,
 * the watchdogs for "no data" did not set the message.
 * Revision 1.2  1999/07/29 03:28:11  BS04481
 * Changes to basin temp code as a result of QA review:  Driver will
 * notify upper level code of corrupt data received from microprocessor.
 * Driver will always listen at port without regard to cal.dat file.
 * Revision 1.1  1999/07/23 01:41:54  BS04481
 * Initial revision
 *
 * TITLE:      basinData.cpp, part of Focussed System basin temperature monitoring
 *
 * ABSTRACT:   Takes basin data from the getByte object.  Validates and
 *             sends to higher level code.
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 * PUBLIC FUNCTIONS:
 *
 *
 */
#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "callback.h"
#include "sh_mem.h"

#include "trima_assert.h"
#include "trima_datalog.h"
#include "crcgen.h"
#include "basindata.hpp"




basinData::basinData(class getbyte* acquisition)
   : _acquisition(acquisition)
{
   trima_assert(_acquisition);

   // create shared memory for communication to the safty message task
   _BPtr = (BAS*)shmem_create("basinData", sizeof(BAS));
   if ( !_BPtr )
      _FATAL_ERROR(__FILE__, __LINE__, "basinData shmem_create failed");

   memset(_BPtr, 0, sizeof(BAS));

   // initialize to start
   _state          = BASIN_LENGTH_ACQUIRE;

   _receivedLength = 0;
   _receivedData   = 0;
   _receivedCRC    = 0x0;
   _dataByteCount  = 0;
   _temp.dataType  = NO_BASIN_DATA;
   _temp.temp      = 0.0;

   // initialize to no error
   _valid    = true;

   _watchdog = new basinTimer();

   _crcCheck = new crc8Bit();

}

basinData::~basinData()
{
   delete _watchdog;
   delete _crcCheck;
   _watchdog    = NULL;
   _crcCheck    = NULL;
   _acquisition = NULL;
   shmem_close("basinData");
   _BPtr        = (BAS*)NULL;
}

void basinData::dataToGo ()
{
   // copy the data into shmem
   memcpy(&_BPtr->msg[_BPtr->NextIn].data, &_temp, sizeof(basinTemp));
   // generate a crc
   _BPtr->msg[_BPtr->NextIn].crc = 0;
   if (crcgen32(&_BPtr->msg[_BPtr->NextIn].crc, (const unsigned char*)&_BPtr->msg[_BPtr->NextIn].data,
                sizeof(basinTemp)) != 0)
      _FATAL_ERROR(__FILE__, __LINE__, "CRC generation error");
   else
   {
      // test for lapping NextOut
      if (_BPtr->NextIn + 1 == _BPtr->NextOut)
         _FATAL_ERROR(__FILE__, __LINE__, "basinData NextIn lapping NextOut!");
      // increment NextIn index
      if (_BPtr->NextIn == NUM_BASIN - 1)
         _BPtr->NextIn = 0;
      else
         _BPtr->NextIn++;
   }
}


void basinData::update ()
{
   GETBYTE_RESULT result;
   char           data;

   _watchdog->notify();  // pulse the watchdog.

   // if we haven't heard from the basin temp sensor
   // in 60 seconds, send a "no data" message
   if (_watchdog->getCommError() == HW_FAULT)
   {
      _temp.dataType = NO_BASIN_DATA;
      _temp.temp     = 0.0f;
      dataToGo();
      // clear flag and listen for another 60 seconds
      _watchdog->clearCommError();

   }
   else if (_watchdog->getDataError() == HW_FAULT)
   {
      // if we have have bad data, send a 'bad data' message
      _temp.dataType = BASIN_DATA_CORRUPT;
      _temp.temp     = 0.0f;
      dataToGo();
      // clear flag and listen for another 2 seconds
      _watchdog->clearDataError();
   }


   result = _acquisition->_dataValid;

   if (result == DATA_ERR)
   {
      _valid = false;
      // it may be bad data but it is data
      _watchdog->petTheDog(_valid);
   }
   else if (result == DATA_OK)
   {
      _valid = true;
      // we've got data
      _watchdog->petTheDog(_valid);

      data                     = _acquisition->assembleByte();
      _acquisition->_dataValid = NOT_DETERMINED;

      switch (_state)
      {
         case BASIN_LENGTH_ACQUIRE :
            // always 4 bytes for this release
            if (data != 4)
            {
               _watchdog->setDataError(HW_FAULT);
               _acquisition->setForceIdle(true);
            }
            else
            {
               _receivedLength = data;
               _watchdog->setDataError(HW_NO_FAULT);
               _dataByteCount  = _receivedLength - 2;
               _receivedData   = 0;
               _state          = BASIN_DATA_ACQUIRE;
            }
            break;
         case BASIN_DATA_ACQUIRE :
            // get the data bytes
            _receivedData |= (data << ( (2 - _dataByteCount) * 8) );
            if (--_dataByteCount <= 0)
               _state = BASIN_CRC_ACQUIRE;
            break;
         case BASIN_CRC_ACQUIRE :
            // get the CRC byte
            _receivedCRC = data;
            result       = checkCRC();
            if (result == DATA_OK)
            {
               _watchdog->setDataError(HW_NO_FAULT);
               calculateTemp();
               dataToGo();
            }
            else
            {
               _watchdog->setDataError(HW_FAULT);

            }
            _state = BASIN_LENGTH_ACQUIRE;
            break;
         default :
            DataLog(log_level_safe_drv_error) << _state << " Unknown basinTemp state" << endmsg;
            _watchdog->setDataError(HW_FAULT);
            break;
      }

   }

}

GETBYTE_RESULT basinData::checkCRC ()
{
   unsigned char myCRC;

   _crcCheck->clearCRC();
   _crcCheck->calculateCRC((char*)&_receivedLength, 1);
   _crcCheck->calculateCRC((char*)&_receivedData, 2);
   myCRC = _crcCheck->getCRC();

   if (myCRC == _receivedCRC)
      return(DATA_OK);
   else
   {
      DataLog(log_level_safe_drv_error) << "Received CRC: 0x" << hex << _receivedCRC
                                        << ", Calculated CRC: 0x" << myCRC << dec << endmsg;
      return(DATA_ERR);
   }
}

void basinData::calculateTemp ()
{
   _temp.dataType = BASIN_TEMP;
   _temp.temp     = (float)(_receivedData) / 16.0f;
}


basinTimer::basinTimer()
{

   // initialize the timer
   osTime::snapshotRawTime(_lastTime);

   // initialize flags to "no error" state
   _commErrorFlag = HW_NO_FAULT;
   _dataErrorFlag = HW_NO_FAULT;
   _validData     = true;
}

basinTimer::~basinTimer()
{}

void basinTimer::notify ()
{
   // check the time of the last basin data against the
   // present time.  If it has been more than the maximum time
   // data type to no data
   int dt;

   dt = osTime::howLongMilliSec(_lastTime);

   // time the messages
   if ( ( dt < 0) )
   {
      // Time has gone backward or wrapped. Start the timers again.
      osTime::snapshotRawTime(_lastTime);
   }
   else if ( ( dt > 3000) && ( dt < 60000 ) )
   {
      // after three seconds set the error if the data is not
      // valid.  This will cause the message sender to send
      // a 'bad data' message on its next pass through the loop
      if ( ( !_validData /* == false */)
           ||(_dataErrorFlag == HW_FAULT) )
         _dataErrorFlag = HW_FAULT;
      else
         _dataErrorFlag = HW_NO_FAULT;

   }
   else if ( dt > 60000)
   {
      // 60 seconds have gone by without receiving new
      // basin data, so set the error.
      _commErrorFlag = HW_FAULT;
      // reset the timer so we don't tell anyone for another 60 seconds
      osTime::snapshotRawTime(_lastTime);
   }
   else
   {
      _commErrorFlag = HW_NO_FAULT;
   }
}

HW_ERROR basinTimer::getCommError ()
{
   return(_commErrorFlag);
}

void basinTimer::clearCommError ()
{
   _commErrorFlag = HW_NO_FAULT;
}

HW_ERROR basinTimer::getDataError ()
{
   return(_dataErrorFlag);
}

void basinTimer::setDataError (HW_ERROR status)
{
   _dataErrorFlag = status;
}

void basinTimer::clearDataError ()
{
   _validData     = true;
   _dataErrorFlag = HW_NO_FAULT;
}

void basinTimer::petTheDog (bool valid)
{
   osTime::snapshotRawTime(_lastTime);
   _validData = valid;
}

/* FORMAT HASH 52e3564d52ac95d2394611445cb8f194 */

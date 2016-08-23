/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      c_rbc.cpp, Focussed System control rbc detctor interface
 *
 * ABSTRACT:   Interface to the rbcport task through shared memory which,
 *             in turn, interfaces to the rbc detector microcontroller.
 *             The rbcDetector class allows the setting of the read and green
 *             driver values and the updating of the red and green sensed values.
 *
 */

#include <vxWorks.h>

#include "c_rbc.hpp"
#include "crc_util.h"
#include "error.h"
#include "sh_mem.h"
#include "trima_assert.h"
#include "trima_datalog.h"

const int RBC_COMM_TIMEOUT = 10000;    // ten seconds


// SPECIFICATION:    rbcDetector constructor
//
// ERROR HANDLING:   _FATAL_ERROR

rbcDetector::rbcDetector(short& red,
                         short& green,
                         unsigned long& newReflectanceCounts,
                         unsigned long& newDriveCounts)
   : _red(red),
     _green(green),
     _newReflectanceCounts(newReflectanceCounts),
     _newDriveCounts(newDriveCounts),
     _watchdog()
{
   //
   // create shared memory for communication to the rbcPort task
   //
   _writeDrive = (writeDrive*)shmem_create("writeDrive", sizeof(writeDrive));

   if (!_writeDrive)
      _FATAL_ERROR(__FILE__, __LINE__, "writeDrive shmem_create failed");

   //
   // initialize the writeDrive area
   //
   memset((void*)_writeDrive, 0, sizeof(writeDrive));
   CRCGEN32_STRUCT(&_writeDrive->crc, _writeDrive);

   _readReflectance = (readReflectance*)shmem_create("readReflectance", sizeof(readReflectance));

   if (!_readReflectance)
      _FATAL_ERROR(__FILE__, __LINE__, "readReflectance shmem_create failed");

   //
   // initialize the readReflectance area
   //
   memset((void*)_readReflectance, 0, sizeof(readReflectance));
   _readReflectance->errors = HW_NO_FAULT;
   CRCGEN32_STRUCT(&_readReflectance->crc, _readReflectance);

   // initialize timer
   osTime::snapshotRawTime(_writeDriveTimer);

   _lastPortReadCount    = 0;

   _newDriveCounts       = 0;

   _newReflectanceCounts = 0;
}



// SPECIFICATION:    rbdDetector destructor
//
// ERROR HANDLING:   none.

rbcDetector::~rbcDetector()
{
   _writeDrive      = NULL;
   _readReflectance = NULL;
}



// SPECIFICATION:    sets drive values for red and green leds
//
// ERROR HANDLING:   none.
void rbcDetector::newDrive (unsigned char red, unsigned char green)
{
   // no write pending and the drive values have changed
   if ( (_writeDrive->writeCount == 0) &&
        ( (_redDrive != red) || (_greenDrive != green) )
        )
   {
      // create command string, write to port
      memset((void*)_writeDrive->newDriveValues, 0, sizeof(_writeDrive->newDriveValues));
      sprintf(_writeDrive->newDriveValues, "%d,%d\r", red, green);

      // generate a crc on the drive values
      _writeDrive->crc = 0;
      crcgen32(&_writeDrive->crc, (unsigned char*)_writeDrive, writeDrive::DriveValuesLen + sizeof(int));

      _writeDrive->writeCount++;

      // watchdog the time needed to complete the write
      osTime::snapshotRawTime(_writeDriveTimer);
   }
   else
   {
      if (_writeDrive->writeCount != 0)
      {
         // either we have a write pending already or the last was unsuccessful
         // check write result because it could be an unsucessful write
         // log error and quit.  we'll get a recoverable alarm during calibration.
         if (_writeDrive->newDriveResult == -1)
            _FATAL_ERROR(__FILE__, __LINE__, "-write()");

         // otherwise, do the new value write but log that the old has not completed
         DataLog_Default << "New drive values with write in progress: writeCount=" << _writeDrive->writeCount << endmsg;

         // create command string, write to port
         memset((void*)_writeDrive->newDriveValues, 0, sizeof(_writeDrive->newDriveValues));
         sprintf(_writeDrive->newDriveValues, "%d,%d\r", red, green);

         // generate a crc on the drive values
         _writeDrive->crc = 0;
         crcgen32(&_writeDrive->crc, (unsigned char*)_writeDrive, writeDrive::DriveValuesLen + sizeof(int));
         _writeDrive->writeCount--;

         // watchdog the time needed to complete the write
         osTime::snapshotRawTime(_writeDriveTimer);
      }
   }

   // keep values
   _redDriveInProgress   = red;
   _greenDriveInProgress = green;
}



void rbcDetector::newDriveResult ()
{
   int dt;

   if (_writeDrive->writeCount == 0)
   {
      // check the crc
      unsigned long checksum = 0;
      crcgen32(&checksum, (unsigned char*)_writeDrive, writeDrive::DriveValuesLen + sizeof(int));

      if (checksum == _writeDrive->crc)
      {
         // check write result
         if (_writeDrive->newDriveResult == -1)
            _FATAL_ERROR(__FILE__, __LINE__, "-write()");

         // finally, save the drive values
         _redDrive   = _redDriveInProgress;
         _greenDrive = _greenDriveInProgress;

         _newDriveCounts++;
      }
      else
      {
         DataLog(log_level_ctl_drv_error) << hex << "CRC check error: generated=0x"
                                          << checksum << " expected=0x" << _writeDrive->crc << dec << endmsg;
      }
   }
   else
   {
      // how long has it been since we heard
      dt = osTime::howLongMilliSec(_writeDriveTimer);

      if (dt > RBC_COMM_TIMEOUT)
         _watchdog.setCommError(HW_FAULT);
   }
}



// SPECIFICATION:    read message from sensor
//
// ERROR HANDLING:   none.
void rbcDetector::read (HW_ERROR& errors)
{
   // do we have new read data?
   if (_readReflectance->portReadCount > _lastPortReadCount)
   {
      // check the crc
      unsigned long checksum = 0;
      crcgen32(&checksum, (unsigned char*)_readReflectance, sizeof(short) + sizeof(short) + sizeof(HW_ERROR));
      if (checksum == _readReflectance->crc)
      {
         _red               = _readReflectance->red;
         _green             = _readReflectance->green;
         _lastPortReadCount = _readReflectance->portReadCount;

         _watchdog.petTheDog();

         // check the error from shared memory
         errors = _readReflectance->errors;
      }
      else
      {
         _red               = 0;
         _green             = 0;
         _lastPortReadCount = _readReflectance->portReadCount;

         DataLog(log_level_ctl_drv_error) << hex << "CRC check error: generated=0x"
                                          << checksum << " expected=0x" << _readReflectance->crc << dec << endmsg;
      }
   }
   else
   {
      // take care of wrap on counter
      if (_readReflectance->portReadCount < _lastPortReadCount)
      {
         DataLog_Default << "Wrap on port read counter: last port read count="
                         << _lastPortReadCount << " port read count="
                         << _readReflectance->portReadCount << endmsg;

         _lastPortReadCount = _readReflectance->portReadCount;
      }
   }

   // check the error from the serial port watchdog
   if (errors == HW_NO_FAULT)
   {
      errors = _watchdog.getCommError();

      _newReflectanceCounts++;
   }
   else
   {  // if error, set reflection to 0

      _red   = 0;
      _green = 0;
   }
}



// SPECIFICATION:    rbcPortMonitor class, this class causes the driver
//                   to log an error if the serial port does not deliver
//                   data for 10 seconds
//
// ERROR HANDLING:   none.
rbcPortMonitor::rbcPortMonitor(void)
{
   // initialize the timer to 0 because
   // we won't start timing until it goes non-zero
   _lastTime.sec     = 0;
   _lastTime.nanosec = 0;

   osTime::snapshotRawTime(_lastPollTime);
   _commErrorFlag = HW_NO_FAULT;
}



rbcPortMonitor::~rbcPortMonitor()
{}



void rbcPortMonitor::monitorCommunications (void)
{
   // check the time of the last serial port data against the
   // present time.  If it has been more than the maximum time
   // set the communications error flag
   int dt;

   dt = osTime::howLongMilliSec(_lastTime);

   // if we have started getting messages, it is OK to time them.
   if (_lastTime.sec != 0)
   {
      if (dt > 10000)
      {
         // 10 seconds have gone by without receiving new
         // serial port data, so set the error.
         if (_commErrorFlag == HW_NO_FAULT)
            DataLog(log_level_ctl_drv_error) << "Serial port watchdog: dt=" << (int)(dt / 1000) << endmsg;

         _commErrorFlag = HW_FAULT;
      }
      else
      {
         _commErrorFlag = HW_NO_FAULT;
      }
   }
}



HW_ERROR rbcPortMonitor::getCommError ()
{
   if (osTime::howLongMilliSec(_lastPollTime) > 2000)
   {
      monitorCommunications();
   }

   return _commErrorFlag;
}



void rbcPortMonitor::setCommError (HW_ERROR error)
{
   _commErrorFlag = error;
}



void rbcPortMonitor::petTheDog ()
{
   osTime::snapshotRawTime(_lastTime);
}

/* FORMAT HASH 0fc10f458ef3ff0b95a507dba5cb577d */

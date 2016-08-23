/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>

#include "ctl_hw_private.h"
#include "c_temp.hpp"
#include "chw.hpp"
#include "trima_assert.h"
#include "trima_datalog.h"

// The sensor has a 500 mv offset such the 0 deg C translates to 500mv.
// After that, each change of 10 mv represents a change of 1 deg C.
static const int EMI_TEMP_LIMIT             = ((10 * 50) + 500); // Issue warning temp. (50 deg C)
static const int EMI_TEMP_FATAL_LIMIT       = ((10 * 60) + 500); // Shutdown temp.  (60 deg C)
static const int EMI_TEMP_TIME_LIMIT        = 60000;             // Shutdown after this time limit (60 sec)
static const int FAN_FAILURE_TIME_TOLERANCE = 500;               // debounce fan failure for 500ms

inline int emiToCelsius (int emiTemp) { return (emiTemp - 500) / 10; }


// SPECIFICATION:    EMI periodic temperature monitoring
//
// ERROR HANDLING:   none.

void tempSense::emiMonitor (void)
{
   int dt;             // delta t, milliseconds

   dt = osTime::howLongMilliSec(_emiMonitorTime);

   // check timer
   if ((dt >= EMI_TEMP_TIME_LIMIT) && (_overTempInProgress == HW_FAULT))
   {
      DataLog_Critical critical;

      DataLog(critical) << "emiTemp=" << emiToCelsius(_emiTemp) << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "EMI over temp timeout");
   }
   else if (_emiTemp >= EMI_TEMP_FATAL_LIMIT)
   {
      DataLog_Critical critical;

      DataLog(critical) << "emiTemp=" << emiToCelsius(_emiTemp) << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "EMI over fatal limit");
   }
}


// SPECIFICATION:   Temperature sensing.
//                  Available for 2.6 hardware only
// ERROR HANDLING:  None

tempSense::tempSense(HW_ERROR& tempFailure,
                     short& emiTemp,
                     HW_ERROR& fan0,
                     HW_ERROR& fan1,
                     HW_ERROR& fan2)
   : _tempFail(tempFailure), _emiTemp(emiTemp),
     _fan0(fan0), _fan1(fan1), _fan2(fan2),
     _fanSampleCount(0), _fan0BadCount(0), _fan1BadCount(0), _fan2BadCount(0)
{
   _overTempInProgress = HW_NO_FAULT;
   _tempFail           = HW_NO_FAULT;
   _fan0               = HW_NO_FAULT;
   _fan1               = HW_NO_FAULT;
   _fan2               = HW_NO_FAULT;

   _emiTemp            = 0;
}

tempSense::~tempSense()
{}

void tempSense::update ()
{
   // test for over temp warning
   if (_emiTemp >= EMI_TEMP_LIMIT)                 // overtemp exists
   {
      if (_overTempInProgress == HW_NO_FAULT)      // and we just noticed
      {
         DataLog_Default << "EMI over temp warning: EMI Temp=" << emiToCelsius(_emiTemp)
                         << "C.  Limit=" << emiToCelsius(EMI_TEMP_LIMIT) << endmsg;

         _tempFail           = HW_FAULT;
         _overTempInProgress = HW_FAULT;
         osTime::snapshotRawTime(_emiMonitorTime);
      }

      emiMonitor();
   }
   else if ((_overTempInProgress == HW_FAULT) && (_emiTemp < EMI_TEMP_LIMIT))
   {
      DataLog_Default << "EMI over temp cleared: emiTemp=" << emiToCelsius(_emiTemp) << endmsg;

      _tempFail           = HW_NO_FAULT;
      _overTempInProgress = HW_NO_FAULT;
   }

   unsigned short fanStatus = chw_fanGetStatus();
   _fanSampleCount += 1;

   _fan0            = (fanStatus & chw_fan0Fault) ? HW_FAULT : HW_NO_FAULT;
   if (_fan0 == HW_FAULT) _fan0BadCount += 1;

   _fan1 = (fanStatus & chw_fan1Fault) ? HW_FAULT : HW_NO_FAULT;
   if (_fan1 == HW_FAULT) _fan1BadCount += 1;

   _fan2 = (fanStatus & chw_fan2Fault) ? HW_FAULT : HW_NO_FAULT;
   if (_fan2 == HW_FAULT) _fan2BadCount += 1;

   if (_fanSampleCount >= FanSampleSize)
   {
      if (_fan0BadCount > 0)
      {
         DataLog_Default << "Fan 0 fault: fan0BadCount=" << _fan0BadCount << endmsg;
         if (_fan0BadCount >= MaxFanBadSamples)
         {
            _FATAL_ERROR(__FILE__, __LINE__, "Fan 0 failure shutdown");
         }
      }

      if (_fan1BadCount > 0)
      {
         DataLog_Default << "Fan 1 fault: fan1BadCount=" << _fan1BadCount << endmsg;
         if (_fan1BadCount >= MaxFanBadSamples)
         {
            _FATAL_ERROR(__FILE__, __LINE__, "Fan 1 failure shutdown");
         }
      }

      if (_fan2BadCount > 0)
      {
         DataLog_Default << "Fan 2 fault: fan2BadCount=" << _fan2BadCount << endmsg;
         if (_fan2BadCount >= MaxFanBadSamples)
         {
            _FATAL_ERROR(__FILE__, __LINE__, "Fan 2 failure shutdown");
         }
      }

      _fanSampleCount = 0;
      _fan0BadCount   = _fan1BadCount = _fan2BadCount = 0;
   }
}

/* FORMAT HASH a47b51107af9806fcb9a9a94eacf936c */

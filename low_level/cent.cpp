/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      c_cent.cpp, Focussed System control hardware centrifuge driver
 *
 * ABSTRACT:   Interface to the control hardware centrifuge
 *
 */

#include <vxWorks.h>
#include <intLib.h>

#include "cent.hpp"
#include "common_hw_private.h"
#include "hw.h"
#include "trima_assert.h"
#include "trima_datalog.h"

// constants
const int MIN_UPDATE_INTERVAL = 95000;      // minimum interval allowed between centrifuge speed updates (micro-seconds)

// SPECIFICATION:    centrifugeRpm constructor
//
// ERROR HANDLING:   none.

centrifugeRpm::centrifugeRpm(void)
   : _fastRawEncoders(CENT_RAW_ENCODER, "centRawEncoders"),
     _fastAvgEncoders(CENT_AVG_ENCODER, "centAvgEncoders"),
     _encoderRate(0),
     _rawEncoderRate(0),
     _lastErrorStatus(0),
     _rejectCount(0),
     _windowCount(0)
{
   osTime::snapshotRawTime(_lastMeasurementTime);
   _lastMeasurement = hw_centGetCommutationCount();

   memset(&_lastRawRate[0], 0, sizeof(long) * ROLLING_AVG_SIZE); // clear averaging array
   _rollingIndex = 0;
}

// SPECIFICATION:    centrifugeRpm destructor
//
// ERROR HANDLING:   none.

centrifugeRpm::~centrifugeRpm()
{}



float centrifugeRpm::rpm () const
{
   return _encoderRate * (60.0 / (float)centCountsPerRev);
}


// SPECIFICATION:    centrifugeRpm actuals
//
// ERROR HANDLING:   none.

void centrifugeRpm::errorStatus (bool powerOn, unsigned short& errors)
{
   // error if any of the fault bits or the reverse bit is set
   // do not debounce this fault
   bool errorDetected = false;

   if (powerOn)
   {
      const unsigned short statusData = hw_centGetStatus();
      clearCommStatus();

      const bool HW_ERROR = (statusData & hw_centFault);

      // Only set the reverse bit if we are at an operating speed. The reverse bit test was put in place
      // to catch a centrifuge that is miswired.  Hence it will always run backwards at all speeds.
      // We need to add cent speed to the test to prevent false alarms from operator spinning the Cent
      // or chance backwards movement.
      // 200 RPM is the min commanded cent speed from cobe_config
      const float RPM         = rpm();
      const bool  REVERSE_BIT = (statusData & hw_centReverse) && (RPM >= 200.0f);

      if (HW_ERROR || REVERSE_BIT)
      {
         if (_lastErrorStatus != statusData)
         {
            _lastErrorStatus = statusData;

            // Provide specific fault logging so GTS guys don't have to decode the numbers
            if (HW_ERROR)
            {
               errors |= HW_CENT_HARDWARE_ERROR;
               datalog_PrintToDefault(__FILE__, __LINE__, "centrifuge error active %x.  commuatation fault", statusData);
            }
            else if (REVERSE_BIT)
            {
               errors |= HW_CENT_REVERSE_BIT;
               datalog_PrintToDefault(__FILE__, __LINE__, "centrifuge error active %x.  reverse fault.  RPM is %f", statusData, RPM);
            }
            else
            {
               datalog_PrintToDefault(__FILE__, __LINE__, "centrifuge error active %x.  unexpected fault.", statusData);
            }
         }

         errorDetected = true;
      }
   }

   if (!errorDetected && errors)
   {
      errors           = HW_CENT_NO_FAULT;
      _lastErrorStatus = 0;
      datalog_PrintToDefault(__FILE__, __LINE__, "centrifuge error inactive");
   }
}


// SPECIFICATION:    centrifugeRpm update
//
// ERROR HANDLING:   none.

void centrifugeRpm::update ()
{
   // 3150 RPM is the over speed trigger
   static const int CENT_OVERSPEED = 3150 * centCountsPerRev / 60;

   int              intervalMicroSec;

   intervalMicroSec = osTime::howLongMicroSec(_lastMeasurementTime);
   if (intervalMicroSec > MIN_UPDATE_INTERVAL)
   {
      // interrupt lock ensures encoder and time interval acquistion are
      // treated as a single atomic operation
      //
      const int level   = intLock();
      long      encoder = hw_centGetCommutationCount();
      intervalMicroSec = osTime::howLongMicroSecAndUpdate(_lastMeasurementTime);
      intUnlock(level);

      float tempRate = encoder - _lastMeasurement;
      _lastMeasurement = encoder;

      if (tempRate < 0.0f)
      {
         tempRate += centCountLimit;
      }

      // compute raw encoder rate (counts/sec)
      tempRate       *= (1000000.0 / intervalMicroSec);
      _rawEncoderRate = (long)tempRate;

      // check for anomalous readings.  If we are 250 RPM higher or lower than
      // the average, then we assume that a hardware glitch (perhaps noise on the
      // commutation lines) has caused an incorrect reading and reject it.  Note
      // however that the number of readings allowed to be rejected is limited.
      //
      long rateDiff = (_rawEncoderRate > _encoderRate) ? _rawEncoderRate - _encoderRate : _encoderRate - _rawEncoderRate;

      if ( (rateDiff < 250 * centCountsPerRev / 60) || (_rejectCount >= 2) )
      {
         // calculate the average encoder rate
         if (++_rollingIndex >= ROLLING_AVG_SIZE)
            _rollingIndex = 0;
         _lastRawRate[_rollingIndex] = _rawEncoderRate;

         long sum = 0;
         for (int i = 0; i<ROLLING_AVG_SIZE; i++)
            sum += _lastRawRate[i];

         _encoderRate = sum / ROLLING_AVG_SIZE;

         if (_encoderRate > CENT_OVERSPEED)
         {
            datalog_PrintToDefault(__FILE__, __LINE__, "centrifuge overspeed warning: %d %ld %ld %ld %ld %ld",
                                   _rollingIndex, _lastRawRate[0], _lastRawRate[1], _lastRawRate[2], _lastRawRate[3], _lastRawRate[4]);
         }

         _fastRawEncoders.dataPoint(_rawEncoderRate);
         _fastAvgEncoders.dataPoint(_encoderRate);
      }
      else
      {
         _rejectCount += 1;
         datalog_PrintToDefault(__FILE__, __LINE__, "rejected centrifuge rate measurement(%d %ld): %d %ld %ld %ld %ld %ld",
                                _rejectCount, _rawEncoderRate, _rollingIndex, _lastRawRate[0], _lastRawRate[1], _lastRawRate[2], _lastRawRate[3], _lastRawRate[4]);
      }

      _windowCount += 1;

      if (_windowCount >= 10)
      {
         _windowCount = 0;
         _rejectCount = 0;
      }
   }
}

/* FORMAT HASH e5750072ee66bedbd03b340c6be59732 */

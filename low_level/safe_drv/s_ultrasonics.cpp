/*
 * Copyright (c) 1995, 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: H:/BCT_Development/Trima5.R/Trima/low_level/safe_drv/rcs/s_ultrasonics.cpp 1.39 2008/01/23 17:30:04Z dslausb Exp spriusb $
 * $Log: s_ultrasonics.cpp $
 * Revision 1.39  2008/01/23 17:30:04Z  dslausb
 * IT 8435 - VxWorks 5.5 Checkin
 * Revision 1.38  2003/06/25 15:46:56Z  jl11312
 * - additional logging for IT 6158 (safety air to donor shutdown)
 * Revision 1.37  2003/06/19 19:46:03Z  jl11312
 * - moved hardware interface to separate modules so that it is available earlier in the boot process
 * Revision 1.36  2003/03/28 19:48:13Z  jl11312
 * - modified to use new data log levels
 * Revision 1.35  2003/02/06 20:44:11Z  jl11312
 * - changed sample interval for ultrasonics timing from 1 to 2 seconds
 * Revision 1.34  2003/01/22 11:54:30  jl11312
 * - enabled timing error and removed debug code
 * Revision 1.33  2003/01/09 20:17:04  pn02526
 * Runnable safety driver with CRC checking of the code and FATAL timing errors disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.  Compiling with ENABLE_FATAL_TIMING_ERRORS defined will enable timing errors to shut the system down.
 * Revision 1.32  2002/12/11 08:33:55  pn02526
 * Checkins for first operating safety driver under vxWorks
 * Revision 1.31  2002/10/31 18:28:26Z  pn02526
 * Massive checkin for integration testing.  Changes BOOL to bool, and forces build to be for 486, only.
 * Revision 1.30  2002/08/13 13:50:23  pn02526
 * Remove extraneous #includes of system files.
 * Substitute vxWorks sysInByte() calls for QNX inp() calls.
 * Revise logging to use the DataLog_Level class through the DataLog_Default macro.
 * Revision 1.29  2002/05/13 13:46:12  pn02526
 * Name change: c_ultra.hpp -> s_ultrasonics.hpp
 * Revision 1.28  2002/05/13 09:48:58  pn02526
 * Name change: c_ultra -> s_ultrasonics
 * Revision 1.27  2001/06/28 09:20:43  jl11312
 * - added logging for timing errors exceeding 1/2 of allowed margin (IT 5182)
 * Revision 1.26  2001/05/30 22:40:30  ms10234
 * 4480 - Changed the interval between CPU and Ultrasonic timing verification
 * to 1 second.
 * Revision 1.25  2001/04/18 20:01:37  jl11312
 * - added initialization for _timingError member
 * Revision 1.24  2001/04/05 20:15:59  jl11312
 * - internal timer handling changes required for versa logic CPU board
 * Revision 1.23  2000/08/07 23:00:21  bs04481
 * Allow visibility to 10ms data used/created by the drivers.
 * See IT 4741
 * Revision 1.22  1999/11/03 15:28:48  BS04481
 * Implement logging of min/max/average total counts from the
 * ultrasonic hardware at the end of each return cycle
 * Revision 1.21  1999/09/30 04:07:44  BS04481
 * Remove message send and receive functions from the driver
 * service loop.
 * Revision 1.20  1997/07/30 17:39:01  bs04481
 * Comment only change to emphasize how air2donor monitoring
 * in the safety driver is protected from the failure of the ultrasonics
 * hardware.
 * Revision 1.19  1996/07/30 18:48:41  SS03309
 * Control driver changes, include files
 * Revision 1.18  1996/07/24 19:49:13  SS03309
 * fix MKS
 * Revision 1.17  1996/07/19 18:47:30  SS03309
 * lint changes
 * Revision 1.16  1996/07/02 16:18:26  SS03309
 * added log and header comments
 *
 * TITLE:      Focussed System control hardware ultrasonics
 *
 * ABSTRACT:   Reads ultrasonic sensor.  The ultrasonics measure the number
 *             of air and fluid echos and are stored in 8 bit roll over
 *             counters.  This module reads the counters at a 10 ms rate,
 *             and totals the last US_SAMPLES.  US_FLUID_RATIO is used
 *             to set the air/fluid detection ratio.
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 * PUBLIC FUNCTIONS:
 *    class ultrasonics
 *
 * NOTES:
 */

// #includes
#include <vxWorks.h>

#include "trima_assert.h"
#include "trima_datalog.h"
#include "ostime.hpp"
#include "error.h"
#include "s_ultrasonics.hpp"
#include "safe_hw_private.h"

// constants

#define MAX_COUNTS 256



// SPECIFICATION:    ultrasonics constructor
//
// ERROR HANDLING:   none.

ultrasonics::ultrasonics(short ratio)
   : _spoofingEmpty(false), _fluidRatio(ratio), _actual(0), _errorCount(0),
     _accTotal(0), _accInterval(0), _startupDelay(1000),
     _timingError(false), _maxTimingError(0),
     _historyIdx(0), _historyLogHoldoff(0), _historyLogRequested(0)
{
// set initial counter values

   unsigned short raw_data = shw_ultrasonicSensorGetCounter();
   _lastAir                 = (unsigned char)(raw_data & 0x00ff) ;
   _lastFluid               = (unsigned char)((raw_data & 0xff00) >> 8);

   _cycleStats.sampleCount  = 1;
   _cycleStats.runningTotal = (unsigned long)_lastAir + (unsigned long)_lastFluid;
   _cycleStats.averageTotal = (float)_cycleStats.runningTotal;
   _cycleStats.totalMin     = 0xffffffff;
   _cycleStats.totalMax     = 0L;
   _usair                   = new fastpack(SAFETY_LOWER_US_AIR, "safetyLSAir");
   _usfluid                 = new fastpack(SAFETY_LOWER_US_FLUID, "safetyLSFluid");

   memset(_historyData, 0, sizeof(_historyData));
}

// SPECIFICATION:    ultrasonics destructor
//
// ERROR HANDLING:   none.

ultrasonics::~ultrasonics()
{
   if (_usair != NULL)
      delete _usair;
   _usair = NULL;
   if (_usfluid != NULL)
      delete _usfluid;
   _usfluid = NULL;
}

// SPECIFICATION:    ultrasonics actuals
//                   returns 1 if fluid, 0 otherwise
//                   US_FLUID_RATIO is used to determine threshold
//
// ERROR HANDLING:   none.

bool ultrasonics::isFluid ()
{
   return (_actual > (US_SAMPLES / 2));
}


// SPECIFICATION:    ultrasonics update
//
// ERROR HANDLING:   none.

void ultrasonics::update ()
{
// read counters from gate array

   unsigned short raw_data = shw_ultrasonicSensorGetCounter();
   unsigned char  air      = (unsigned char)(raw_data & 0x00ff) ;
   unsigned char  fluid    = (unsigned char)((raw_data & 0xff00) >> 8);

// get air counts

   long airCounts = air - _lastAir;
   if ( airCounts < 0)
   {
      airCounts += MAX_COUNTS;
   }
   _lastAir = air;

// get fluid counts

   long fluidCounts = fluid - _lastFluid;
   if ( fluidCounts < 0)
   {
      fluidCounts += MAX_COUNTS;
   }
   _lastFluid = fluid;

   _usair->dataPoint((short)airCounts);
   _usfluid->dataPoint((short)fluidCounts);

// gather stats
   long total = airCounts + fluidCounts;

   _cycleStats.runningTotal += total;
   _cycleStats.sampleCount++;
   if (_cycleStats.totalMin > total)
      _cycleStats.totalMin = total;
   if (_cycleStats.totalMax < total)
      _cycleStats.totalMax = total;

//
// Check that the ultrasonics counts are consistent with the QNX internal timing
//
   checkCounts(total);

// Note that hardware failure which results in the counters not counting will
// cause this code to indicate air after a maximum of 11 samples.

   if ( (fluidCounts * _fluidRatio) > airCounts)
   {
      _actual++;                                  // bump and limit fluid
      if (_actual > US_SAMPLES)
      {
         _actual = US_SAMPLES;
      }

      _historyData[_historyIdx] = fluidCounts * _fluidRatio - airCounts;
      if ( _historyData[_historyIdx] > 0x3f ) _historyData[_historyIdx] = 0x3f;
   }
   else
   {
      _actual--;                                  // decrement and limit air
      if (_actual < 0)
      {
         _actual = 0;
      }

      _historyData[_historyIdx]  = airCounts - fluidCounts * _fluidRatio;
      if ( _historyData[_historyIdx] > 0x3f ) _historyData[_historyIdx] = 0x3f;
      _historyData[_historyIdx] |= 0x40;
   }

   if ( shw_pumpDirection(hw_returnPump) == shw_pumpReturn ) _historyData[_historyIdx] |= 0x80;
   _historyIdx = (_historyIdx + 1) % HistoryLogSize;

   if ( _historyLogHoldoff > 0 )
   {
      _historyLogHoldoff -= 1;
      if ( _historyLogHoldoff == 0 && _historyLogRequested )
      {
         logHistory();
         _historyLogRequested = 0;
      }
   }

   if (_spoofingEmpty) _actual = 0;
}

// SPECIFICATION:    ultrasonics statistics
//
// ERROR HANDLING:   none.

void ultrasonics::getStats (ultraStats* stats)
{

   stats->runningTotal      = _cycleStats.runningTotal;
   stats->sampleCount       = _cycleStats.sampleCount;
   stats->averageTotal      = (float)stats->runningTotal / (float)stats->sampleCount;
   stats->totalMin          = _cycleStats.totalMin;
   stats->totalMax          = _cycleStats.totalMax;

   _cycleStats.runningTotal = 0L;
   _cycleStats.sampleCount  = 0L;
   _cycleStats.averageTotal = 0.0f;
   _cycleStats.totalMin     = 0xffffffff;
   _cycleStats.totalMax     = 0L;

}

bool ultrasonics::checkTimingError (void)
{
   bool retValue = _timingError;
   _timingError = false;
   return retValue;
}

void ultrasonics::logHistory (void)
{
   if ( _historyLogHoldoff <= 0 )
   {
      DataLog_Stream& outStream = log_level_safe_drv_info(__FILE__, __LINE__);
      outStream << "actual=" << (int)_actual << " data (new to old)=" << hex;
      for ( int i = 0; i<HistoryLogSize; i++ )
      {
         int idx = (_historyIdx + HistoryLogSize - i - 1) % HistoryLogSize;
         outStream << (int)_historyData[idx] << " ";
      }

      outStream << dec << endmsg;
      _historyLogHoldoff = HistoryLogSize;
   }
   else
   {
      _historyLogRequested = 1;
   }
}

void ultrasonics::checkCounts (long total     // total number of counts for the current 10 msec period
                               )
{
   if (_startupDelay > 0)
   {
      _startupDelay -= 1;
   }
   else
   {
      _accTotal    += total;
      _accInterval += 1;

      //
      // This interval must be at least as long as the timing interval used
      // in the safe_drv monitorTiming routine.  Otherwise, a timing error
      // set in this routine might not be detected by the monitorTiming routine
      // soon enough, and the fatal error for an unackowledged timing error
      // may occur.
      //
      if (_accInterval >= 200)
      {
         //
         // If a timing error was detected in the last cycle, it should
         // have been handled by now.  If not, report a fatal error.
         //
         if (_timingError)
         {
            _FATAL_ERROR(__FILE__, __LINE__, "Unacknowledged timing error");
            _timingError = false;
         }

         enum { MinReadingsPerSecond = 3867 };  // 99% of nominal (256 uSec/reading)
         enum { NomReadingsPerSecond = 3906 };  // nominal (256 uSec/reading)
         enum { MaxReadingsPerSecond = 3945 };  // 101% of nominal (256 uSec/reading)

         unsigned long intervalMSec       = _accInterval * 10;
         unsigned long minAllowedReadings = (MinReadingsPerSecond * intervalMSec) / 1000;
         unsigned long maxAllowedReadings = (MaxReadingsPerSecond * intervalMSec) / 1000;

         if (_accTotal < minAllowedReadings || _accTotal > maxAllowedReadings)
         {
            _timingError = true;
            DataLog(log_level_safety_alarm_detail) << "timing error: _accTotal=" << _accTotal
                                                   << " intervalMSec=" << intervalMSec
                                                   << " minAllowedReadings=" << minAllowedReadings
                                                   << " maxAllowedReadings=" << maxAllowedReadings
                                                   << endmsg;
         }
         else
         {
            int currTimingError = abs(_accTotal - (NomReadingsPerSecond * intervalMSec) / 1000);
            if (currTimingError > _maxTimingError)
            {
               _maxTimingError = currTimingError;
               if (_maxTimingError > (maxAllowedReadings - minAllowedReadings) / 4)
               {

                  DataLog_Default << "timing warning: _accTotal=" << _accTotal
                                  << " intervalMSec=" << intervalMSec
                                  << " minAllowedReadings=" << minAllowedReadings
                                  << " maxAllowedReadings=" << maxAllowedReadings
                                  << endmsg;
               }
            }
         }

         _accInterval = 0;
         _accTotal    = 0;
      }
   }
}

/* FORMAT HASH 5a5bd3a19cbd28771fa88c8e57516506 */

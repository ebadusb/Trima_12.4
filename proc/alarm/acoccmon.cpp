/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      AC Occlusion Monitoring
 *
 * ABSTRACT:   This program reads the high speed ac data from the control driver,
 *             thresholds and processes the data to determine AC occlusion.
 *
 */

#include "acoccmon.h"
#include "trima_datalog.h"
#include "procdata.h"
#include "CatalogChecker.h"

#include <math.h>
#include <stdlib.h>


//
// defines
//
#define MIN_DATA         (700)         // min data needed for analysis
#define MIN_NOISE        (20)          // min estimate of noise
#define NOISE_ADJUSTMENT (10)          // noise threshold adjustment
#define CENT_RPM         (2100)        // min centrifuge rpm
#define THRESHOLD        (1.35f)       // detection threshold
#define LONG_SAMPLES     (40)          // samples in running average
#define FIRST_SAMPLE     (295 + 200)   // first samples (~3secs) after return pump pressure bleed down
                                       //   plus 4 seconds to account for AC pump restart after recirc blood

DEFINE_OBJ(ACOccMon);

ACOccMon::ACOccMon()
   : _APSFastDataMsg(),
     _ACOcclusionAlarm(AC_OCCLUSION_DETECTED),
     _CycleCount(0),
     _AverageDistance(0),
     _AverageNoise(0),
     _DataCount(0),
     _MonitorCycle(false),
     _ReturnCycle(false),
     _SkipCycle(false),
     _isRasSet(false),
     _RawData()
{}

ACOccMon::~ACOccMon()
{}

void ACOccMon::Monitor ()
{
   ProcData pd;

   // Disable AC occlusion monitor if RAS
   if ( _isRasSet )
   {
      return;
   }

   if ( pd.TrimaSet().CassetteState.Get() >= ProcTrimaSet_CDS::BLOOD &&
        pd.TrimaSet().CassetteState.Get() < ProcTrimaSet_CDS::USED )
   {
      _ReturnCycle  = ( pd.Status().ReturnPump.ActRPM() != 0.0f );

      _MonitorCycle = ( (pd.Status().CentrifugeRPM() > CENT_RPM) && !_ReturnCycle );

      if ( _MonitorCycle && (pd.Status().InletPump.ActRPM() == 0.0f) )
      {
         _SkipCycle = true;
      }

      if (_CycleCount >= 3)            // three in a row
      {
         // Log result
         DataLog(log_level_proc_alarm_monitor_info) << "AC Occlusion alarm triggered.  Cycle count="
                                                    << _CycleCount << ".  Return=" << _ReturnCycle << ".  Monitor="
                                                    << _MonitorCycle << endmsg;

         _ACOcclusionAlarm.setAlarm(); // set alarm
         _CycleCount = 0;              // re-enable for next time
      }
   }
}

void ACOccMon::enable ()
{
   ProcData pd;

   _isRasSet = CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::MSS_RBC);

   // Disable AC occlusion monitor if RAS
   if ( _isRasSet )
   {
      DataLog(log_level_proc_alarm_monitor_info) << "AC OCCLUSION MONITOR is N/A for RAS set" << endmsg;
   }
   else
   {
      // Register for APS updates
      _APSFastDataMsg.init(Callback<ACOccMon>(this, &ACOccMon::newAPSData) );
   }
}

void ACOccMon::disable ()
{
   if ( _isRasSet )
   {
      return;
   }
   else
   {
      _APSFastDataMsg.deregisterMsg();
   }
}

void ACOccMon::newAPSData ()
{
   //
   // Get the message data ...
   FASTDATA fastdata = _APSFastDataMsg.getData();

   // Check the message source to see if this
   //  data comes from the Access Pressure Sensor ...
   if ( fastdata.source == CONTROL_ACCESS_PRESSURE )
   {
      //
      // if we should monitor this cycle,
      //          and we have enough room to add data to our array
      //
      if ( _MonitorCycle && (_DataCount < (MAX_HISTORY - FASTDATALENGTH)) )
      {
         //
         // Put this data into the array at the end ...
         memcpy(&_RawData[_DataCount], fastdata.raw, sizeof(short) * FASTDATALENGTH);
         // Increment the array capacity counter ...
         _DataCount += FASTDATALENGTH;

      }
      //
      // if the Array is full or close enough to full ... OR
      //   we have started in a return cycle and have a minimum of data ...
      //
      else if ( (_DataCount >= MAX_HISTORY - FASTDATALENGTH) ||
                ( _ReturnCycle && (_DataCount > MIN_DATA) )
                )
      {
         if ( _SkipCycle )
         {
            //
            // We did trigger a calculation.  Set the return cycle flag
            //  to remember that we did try a calculation, but skip the
            //  calculation due to bad data.
            //
            _ReturnCycle = true;

            DataLog(log_level_proc_alarm_monitor_info) << "SKIP THIS CYCLE due to inlet pump stopping; numPts=" << _DataCount << endmsg;

         }
         else
         {
            // Run the data through the analysor
            const float ratio = analyzeData();

            if (ratio > THRESHOLD)
            {
               _CycleCount++;
               DataLog(log_level_proc_alarm_monitor_info) << "OCC " << ratio << " cycle " << _CycleCount
                                                          << ".  Trigger is " << THRESHOLD
                                                          << "  [numPts=" << _DataCount
                                                          << " avgDist=" << _AverageDistance
                                                          << " avgNoise=" << _AverageNoise
                                                          << "]" << endmsg;
            }
            else
            {
               _CycleCount = 0;
               DataLog(log_level_proc_alarm_monitor_info) << "NO OCC " << ratio
                                                          << "  [numPts=" << _DataCount
                                                          << " avgDist=" << _AverageDistance
                                                          << " avgNoise=" << _AverageNoise
                                                          << "]" << endmsg;

            }

            //
            //  Not a return cycle ...
            //
            if ( !_ReturnCycle )
            {
               //
               // Save the last half of the data for the next cycle to complete
               //   our rolling window ...
               //
               // Set the data size to one half full ...
               _DataCount = (MAX_HISTORY / 2) - 1;
               //
               //  Copy the last half of the data to the first half of the array ...
               memcpy(&_RawData[0], &_RawData[_DataCount + 1], sizeof(short) * (MAX_HISTORY / 2) );

               DataLog(log_level_proc_alarm_monitor_info) << "Roll the Two-minute window ..." << endmsg;
            }

         }

      }

      //
      //  Is it a return cycle ...
      //
      if ( _ReturnCycle )
      {
         //
         //  It is the a return cycle, start over ...
         //
         if ( _DataCount != 0 )
         {
            _DataCount = 0;

            DataLog(log_level_proc_alarm_monitor_info) << "Reset the data window ..." << endmsg;
         }

         //
         // We went through a cycle, so we can reset the
         //  flag ...
         //
         _SkipCycle = false;

      }

   }
}


float ACOccMon::analyzeData ()
{
   unsigned long k;                            // loop counter
   long          initialMean;                  // mean of FIRST_SAMPLE number of data points
   long          meanLong;                     // long tc mean
   long          p3                     = 0;   // current raw APS sample
   long          posAverage             = 0;   // positive peak average
   long          peakAverage            = 0;   // average of peaks which cross the average-threshold
   long          peakThresholdCrossings = 0;   // Number of samples that cross the average-threshold
   float         ratio                  = 0.0; // ratio of the
                                               //  ( average of peaks which cross the average-threshold ) to
                                               //  ( average of the variation from the sample average )

   // initialize running mean and history

   meanLong = _RawData[0];
   for (k = 1; k<FIRST_SAMPLE; k++)         // initialize running average, Yi
   {
      meanLong = ( meanLong * (LONG_SAMPLES - 1) + _RawData[k]) / LONG_SAMPLES;
   }
   // Save the initial mean ...
   initialMean = meanLong;

   // compute and record the deviation from Yi (noise band B)

   for (k = FIRST_SAMPLE + 1; k<_DataCount; k++)
   {

      // compute running first order filter

      p3       = _RawData[k];
      meanLong = ( meanLong * (LONG_SAMPLES - 1) + p3) / LONG_SAMPLES;

      // compute [integrated] noise threshold value, B

      posAverage += labs(p3 - meanLong);
   }

   // make threshold value an average by dividing by number of samples

   posAverage /= (_DataCount - FIRST_SAMPLE);
   posAverage += NOISE_ADJUSTMENT;
   if (posAverage < MIN_NOISE)
   {
      posAverage = MIN_NOISE;
   }

   // this is the average noise band
   _AverageNoise = posAverage;

   // pass through data second time,
   // capture process peaks below threshold

   // Restore the initial mean ...
   meanLong = initialMean;

   // integrate those peaks below threshold value

   for (k = FIRST_SAMPLE + 1; k<_DataCount; k++)
   {

      // compute running first order filter

      p3       = _RawData[k];
      meanLong = ( meanLong * (LONG_SAMPLES - 1) + p3) / LONG_SAMPLES;

      // threshold detection: look for the samples Xi below the (Yi - B) band

      if (p3 < (meanLong - _AverageNoise))
      {
         peakAverage += (p3 - meanLong);
         peakThresholdCrossings++;
      }
   }

   if (peakThresholdCrossings > 0)
   {
      // compute the average peak-to-noise ratio
      peakAverage /= peakThresholdCrossings;
      ratio        = (float)peakAverage / (float)_AverageNoise;

      // average "distance" between crossings (a measure of how noisy the data is)
      _AverageDistance = _DataCount / peakThresholdCrossings;

      // reject noisy data or level shifts
      if ((_AverageDistance < 10) || (_AverageDistance > 35))
      {
         ratio = 0.0;
      }
   }
   else // no peaks crossed the noise band
   {
      _AverageDistance = 0;
   }

   // return positive ratio (_AveragePeak is negative)
   return fabs(ratio);
}

/* FORMAT HASH 947d30794d256d120284f2c3ab8c86da */

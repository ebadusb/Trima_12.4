/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeRbcMonitor.cpp
 *
 */

#include "PrimeRBC_Monitor.h"
#include "cobeconfig.h"
#include "trima_datalog.h"
#include "procdata.h"
#include <string.h>
#include <strstream>
#include <string>
using namespace std;




// Local constants
const float VERY_LARGE_RATIO = 1e6;


const float startPrimeVentVolm = CobeConfig::data().VinPrimeChannel4;
const float endPrimeVentVolm   = CobeConfig::data().VinPrimeVent;
const float percentRise        = CobeConfig::data().RbcPrimeSpilloverRisePercent / 100.0;
const float theshold           = CobeConfig::data().PrimeSpilloverSampleThreshold;

string      rampCent           =  "RampCentrifuge";
string      PrimeChannel4      =  "PrimeChannel4";

DEFINE_OBJ(PrimeRBC_Monitor);

PrimeRBC_Monitor::PrimeRBC_Monitor()
   : _avgRatio(0.0),
     _sumRatios(0.0),
     _numRatios(0.0),
     _avgComputed(false),
     _detected(false),
     _hits(0.0),
     _logOnce(false)
{}

PrimeRBC_Monitor::~PrimeRBC_Monitor()
{}

void PrimeRBC_Monitor::Monitor ()
{
   ProcData pd;

   if (pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC)
      return;

   float red;
   float green;
   float ratio;

   red   = (float)pd.Status().RedValue();
   green = (float)pd.Status().GreenValue();

   //////// CALC RG RATIO //////////////////////////////////////////
   // Ensure green value is not zero.
   if (green != 0.0f)
   {
      ratio = red / green;
   }
   // If both green and red are zero then ratio is one.
   else if (red == 0.0f)
   {
      ratio = 1.0f;
   }
   // Other wise the ratio is very large.
   else
   {
      ratio = VERY_LARGE_RATIO;
   }


   ////////COLLECT AVERAGE RG RATIO //////////////////////////////
   string currState =  pd.SubstateName();

   if ( PrimeChannel4 == currState )
   {
      // do the Ratio average;
      average(ratio);
      if (!_logOnce)
      {
         DataLog(log_level_proc_info) << "Starting Prime spillover baseline" << endmsg;
         _logOnce = true;
      }
   }



   //////// CHECK FOR SPIKE RG RATIO //////////////////////////////////////////
   if ( rampCent == currState )
   {
      // check for 10% rise
      check(ratio);

   }
}

void PrimeRBC_Monitor::check (const float& ratio)
{
   ProcData pd;

   // compute the average
   if ( (_numRatios > 0) && (!_avgComputed) )
   {
      _avgRatio    =  _sumRatios / _numRatios;
      _avgComputed = true;
      DataLog(log_level_proc_info) << "Samples of RG Ratio from PrimeChannel4 -> "  << _numRatios  << endmsg;
      DataLog(log_level_proc_info) << "SUM of RG Ratio from PrimeChannel4     -> "  << _sumRatios  << endmsg;
      DataLog(log_level_proc_info) << "Average RG Ratio from PrimeChannel4    -> "  << _avgRatio   << endmsg;
   }


   static float riseLimit = _avgRatio + _avgRatio * percentRise;

   // if not cal the cant do it.
   if (!pd.TrimaSet().RBCDetectorCalibrated.Get()) return;

   if ( ratio >= riseLimit)
   {

      _hits++;


      if ((_hits >= theshold) && !_detected)
      {
         DataLog(log_level_proc_info) << "PLSO: Ratio Exceeds threshold -> "  <<  ratio
                                      << "; Limit -> " << riseLimit << "; hits so far ->" << _hits <<  endmsg;


         DataLog(_pqLog) << (int)SPILL_DETECTION << " COUNT PLATELET WBC because of Spillover Detected during Blood Prime" << endmsg;
         LogIt2(SPILL_DETECTION, VERIFY_PLATELET_WBCS);
         _pqi.AddReasonToFlag(SPILL_DETECTION, VERIFY_PLATELET_WBCS);


         // IT 11037   added alarm
         pd.RequestObtrusiveWbcAlarm(__FILE__, __LINE__, "WBC event alarm is needed");

         _detected = true;
      }

   }
   else if  ( (ratio < riseLimit) && !_detected)
   {
      _hits = 0.0;
   }

}

void PrimeRBC_Monitor::average (const float& ratio)
{
   ProcData pd;

   // do the average
   _sumRatios = _sumRatios + ratio;
   _numRatios++;
}




// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void PrimeRBC_Monitor::enable ()
{
   MonitorBase::enableMonitoring(1);
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void PrimeRBC_Monitor::disable ()
{

   ProcData pd;

   MonitorBase::enableMonitoring(0);
}

/* FORMAT HASH 67ffff67b2ecaea9dfbb921c208f65eb */

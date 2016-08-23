/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      primeHemolysis_alarm.cpp
 *
 */

#include "primeHemolysis_alarm.h"
#include "rbcdat.h"
#include "trima_datalog.h"
#include "procdata.h"
#include "cobeconfig.h"


// Local constants
const float VERY_LARGE_RATIO = 1e6;
const float MIN_CENT_RPM     = 199.0f;

DEFINE_OBJ(PrimeHemolysisAlarm);

PrimeHemolysisAlarm::PrimeHemolysisAlarm()
   : _WeSawGT50(false),
     _HemolysisAlarm(HEMOLYSIS_DETECTED_ALARM)
{}

PrimeHemolysisAlarm::~PrimeHemolysisAlarm()
{}

void PrimeHemolysisAlarm::Monitor ()
{
   ProcData pd;

   // only check in first  return.
   if (pd.Run().FirstCycleComplete.Get())
      disable();


   // no go if not calibrated....
   if (!pd.TrimaSet().RBCDetectorCalibrated.Get())
      return;

   // on-off switch in RBC.dat
   if ( RbcDat::data().CheckForPrimeHemolysis )
   {
      // dont look prior to PRIMED
      if (pd.TrimaSet().CassetteState.Get() >= ProcTrimaSet_CDS::PRIMED)
      {
         // stuff floats up the line if the cent is slow
         if ( pd.Status().CmdCentrifugeRPM() > MIN_CENT_RPM)
         {
            // ok now check
            checkForHemolysis(makeRatio() );
         }
      }
   }
   else
   {
      // if it's off disable the monitor.
      disable();
   }

}

void PrimeHemolysisAlarm::checkForHemolysis (const float& ratio)
{

   ///////////////////////////////////////////////
   /*   the Plan....
     Run  only for prime states. end at entry to Collect
     Ignore signals when cent is commanded <= 199 because of the
     fluid backing up through the channel lines.
     watch for signal of >= 50 then if we have seen that
     at first return watch for
     a signal ratio >= 1.1 and green <= 1000....
     if all conditions fit then raise the alarm and end run.
     able to turn on/off via RBC.dat
   */
   //////////////////////////////////////////////

   ProcData pd;
   if (    ( !_WeSawGT50 )
           && ( !pd.Run().FirstDrawComplete.Get() )
           && ( ratio >= CobeConfig::data().PrimeHemolysisRGTrigger )
           )
   {

      DataLog(log_level_proc_info) << "Checking for Prime Hemolysis will occure in first return. Ratio: "
                                   << ratio
                                   << endmsg;
      _WeSawGT50 = true;
   }


   if (    _WeSawGT50
           &&  pd.Run().FirstDrawComplete.Get()
           && !pd.Run().FirstCycleComplete.Get()  )
   {

      if (   ( ratio >= CobeConfig::data().PrimeHemolysisRGReturnLevel )
             && ( CobeConfig::data().PrimeHemolysisGreenReturnLimit  >= (float)pd.Status().GreenValue() )
             )
      {
         DataLog(log_level_proc_info) << "ALARM:: Prime Hemolysis during first return! Ratio: "
                                      << ratio
                                      << ", Green value: "
                                      << pd.Status().GreenValue()
                                      << endmsg;
         _HemolysisAlarm.setAlarm();
         disable();    // no sense going on....
      }


   }

}

float PrimeHemolysisAlarm::makeRatio ()
{

   ProcData pd;
   float    red;
   float    green;
   float    ratio;

   red   = (float)pd.Status().RedValue();
   green = (float)pd.Status().GreenValue();

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
   return ratio;
}



// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void PrimeHemolysisAlarm::enable ()
{
   MonitorBase::enableMonitoring(1);
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void PrimeHemolysisAlarm::disable ()
{


   MonitorBase::enableMonitoring(0);
}

/* FORMAT HASH cedf73319769f7d95657fb433f0636ff */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      rbc_alarm.cpp
 *
 */

#include "rbc_alarm.h"
#include "rbcdat.h"
#include "trima_datalog.h"
#include "procdata.h"


// Local constants
const float  VERY_LARGE_RATIO       = 1e6;

const string RBCAlarmrampCent       =  "RampCentrifuge";
const string RBCAlarmprimeVent      =  "PrimeVent";
const string RBCAlarmprimeChannel   =  "PrimeChannel";
const string RBCAlarmprimeChannel2A =  "PrimeChannel2A";
const string RBCAlarmprimeChannel2  =   "PrimeChannel2";
const string RBCAlarmprimeChannel3  =   "PrimeChannel3";
const string RBCAlarmprimeChannel4  =   "PrimeChannel4";
const string RBCAlarmprimeAirout1   =   "PrimeAirout1";
const string RBCAlarmprimeAirout2   =   "PrimeAirout2";





DEFINE_OBJ(RBCAlarm);

RBCAlarm::RBCAlarm()
   : _UpdateCounter(0),
     _SpilloverDetected(0),
     _spilloverAlarm(SPILLOVER_DETECTED),
     _alreadyShowedSpilloverAlarm(false)
{}

RBCAlarm::~RBCAlarm()
{}

void RBCAlarm::Monitor ()
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

   //
   // platelet sets ...
   //
   if ( pd.Status().CassIsPltPlsRBC() )
   {
      //
      // Platelet collection sets only ...
      //
      DataLog(log_level_proc_debug) << "Check Ratio for spillover on Plt/Pls/RBC set -> " << ratio << endmsg;
      checkForSpillover(ratio);

   }

   //
   // If we have detected a spillover ...
   //
   if ( _SpilloverDetected )
   {
      State_names currentState   = pd.Run().Substate.Get();

      bool        excludedStates =   (
         (currentState == SS_RP_RBC)                                            // RBC collection states (black stamp)
         || (currentState == SS_RP_RBC_DOUBLE)
         || (currentState == SS_RP_RBC_PTF_SETUP_1)
         || (currentState == SS_RP_RBC_PTF_SETUP_2)
         || (currentState == SS_RP_RBC_PTF_SETUP_1_DOUBLE)
         || (currentState == SS_RP_RBC_PTF_SETUP_2_DOUBLE)
         || (currentState == SS_RBC)                                           // RBC collection states (white stamp)
         || (currentState == SS_RBC_PTF_SETUP_1)
         || (currentState == SS_RBC_PTF_SETUP_2)
         || (currentState == SS_RBC_PTF_PRIME)

         ) ;

      // Fix for IT7177:  original code explicitly checked for condition of collect and plasma
      // valves to determine if we were collecting platelets or plasma.  Valve expected states
      // have changed, breaking this code.  We now use substates to filter spillover detection
      // if we're in one of the RBC collection substates.
      if ( !excludedStates )     // RBC collection (dRBC)
      {
         if ( _UpdateCounter == RbcDat::data().SpilloverTriggerSamples )
            DataLog(log_level_proc_info) << "SPILLOVER detected" << endmsg;

         SpilloverDetectedMsg spilloverDetected(MessageBase::SEND_LOCAL);
         spilloverDetected.send(1);

         if (!_alreadyShowedSpilloverAlarm)
         {
            _alreadyShowedSpilloverAlarm = true;


            // IT 11037   make the alarms diff per prime||run
            if ( inPrime() )
            {
               pd.RequestObtrusiveWbcAlarm(__FILE__, __LINE__, "WBC event alarm is needed");
            }
            else
            {
               _spilloverAlarm.setAlarm();
            }

         }

      }

   }

}

void RBCAlarm::checkForSpillover (const float& ratio)
{
   ProcData pd;

   //
   // If the RBC detector is calibrated then check for spillover ratio detected
   //

   if ( pd.TrimaSet().RBCDetectorCalibrated.Get() &&
        ratio > RbcDat::data().SpilloverTriggerRatio )
   {
      if ( !_SpilloverDetected )
      {

         DataLog(log_level_proc_info) << "Spillover Trigger Ratio detected -> " << ratio
                                      << ";  number of consecutive detections -> "  << _UpdateCounter << endmsg;

      }
      if ( ++_UpdateCounter >= RbcDat::data().SpilloverTriggerSamples )
      {

         _SpilloverDetected = 1;
      }

   }
   else
   {
      // get me the log of the one that resets
      if (_SpilloverDetected)
      {
         DataLog(log_level_proc_info) << "NON-Spillover Trigger Ratio detected (resetting)->"  << ratio << endmsg;
      }

      _SpilloverDetected = 0;

      // Reset the counter.
      _UpdateCounter = 0;
   }


}


bool RBCAlarm::inPrime ()  const
{
   ProcData pd;
   bool     answer    = false;
   string   currState =  pd.SubstateName();

   if (    RBCAlarmprimeVent == currState
           ||  RBCAlarmrampCent  == currState
           ||  RBCAlarmprimeChannel   == currState
           ||  RBCAlarmprimeChannel2A == currState
           ||  RBCAlarmprimeChannel2  == currState
           ||  RBCAlarmprimeChannel3  == currState
           ||  RBCAlarmprimeChannel4  == currState
           ||  RBCAlarmprimeAirout1   == currState
           ||  RBCAlarmprimeAirout2   == currState
           )
   {
      DataLog(log_level_proc_info) << "RBC_Alarm for spillover detected in the priming state: "  << currState << endmsg;
      answer = true;
   }

   return answer;


}

/* FORMAT HASH 1c605a100cde9d355d1be9f521dc0c68 */

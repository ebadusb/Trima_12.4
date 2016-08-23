/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      aplimiter.cpp
 *
 */

#include <vxworks.h>
#include "aplimiter.h"
#include "caldat.h"
#include "an_alarm.h"
#include "procdata.h"
#include "software_cds.h"

const float MINIMUM_PRESSURE = -350.0f; // mmHg
const float MAXIMUM_PRESSURE = 400.0f;  // mmHg
const float SWITCH_HOLD_TIME = 2.1f;    // seconds

APLimiter::APLimiter()
   : _LastState(),
     _LastRecovery(),
     _StateChangeTime(),
     _LowConfigLimit(0.0f),
     _HighConfigLimit(0.0f),
     _LastLowLimit(0.0f),
     _LastHighLimit(0.0f),
     _FirstTimeInDRBCCollectReturn(true),
     _FirstReturnLimiterActive(false),
     _SpecialBlackStampNeedle(false)
{}


APLimiter::~APLimiter()
{}

int APLimiter::init (float lowLimit, float highLimit)
{
   //
   // Set the limits ...
   //
   _LowConfigLimit  = lowLimit;
   _LastLowLimit    = lowLimit;
   _HighConfigLimit = highLimit;
   _LastHighLimit   = highLimit;

   _StateChangeTime.activate();

   //
   //
   //   Log configuration and calibration info once upon startup.
   //
   DataLog(log_level_proc_info).flags(ios::showpoint | ios::fixed);
   DataLog(log_level_proc_info).precision(2);
   DataLog(log_level_proc_info) << "APS cal"
                                << " +:" << CalDat::data().APSRawPositive << "=" << CalDat::data().APSmmHgPositive
                                << " 0:" << CalDat::data().APSRawReference << "=" << CalDat::data().APSmmHgReference
                                << " -:" << CalDat::data().APSRawNegative << "=" << CalDat::data().APSmmHgNegative
                                << endmsg;

   DataLog(log_level_proc_info) << "APS config limits: "
                                << _LowConfigLimit << " " << _HighConfigLimit << endmsg;

   //
   // Initialize the limits ...
   //
   ProcData pd;
   pd.Orders().APSLow(_LowConfigLimit);
   pd.Orders().APSHigh(_HighConfigLimit);

   _SpecialBlackStampNeedle = Software_CDS::GetInstance().getFeature(SpecialBlackStampNeedle);

   DataLog(log_level_proc_info) << "Special black stamp needle override is "
                                << (_SpecialBlackStampNeedle ? "ON" : "OFF") << endmsg;

   return 1;
}

void APLimiter::setLimits (const string& currentState)
{
   ProcData pd;

   //
   // Set up variables for pressure equations based on the current state of
   //  the system.
   //
   float             Hn, Hin;
   const float       R           = pd.Cycle().ACRatio.Get();
   const bool        isDrawCycle = pd.Run().DrawCycle.Get();
   const State_names runSubState = pd.Run().Substate.Get();

   if ( pd.SystemState() == BLOOD_RUN &&
        isDrawCycle )
   {
      Hn = pd.Donor().Hct.Get();             // Hematocrit needle
      if ( R > 0.0f )
      {
         Hin = Hn * (1 - 1 / R);  // Hematocrit inlet
      }
      else
      {
         Hin = Hn;
      }
   }
   // If we are in DRBC PTF Setup states, fix the crit to 80.
   else if ( pd.SystemState() == BLOOD_RUN &&
             ((runSubState == SS_RP_RBC_PTF_SETUP_1_DOUBLE) ||
              (runSubState == SS_RP_RBC_PTF_SETUP_2_DOUBLE)) )
   {
      Hn = Hin = pd.RunTargets().RbcCritTarget.Get();
      DataLog(log_level_proc_debug) << "RBC Setup states.  Adjust res crit" << endmsg;
   }
   // If we are in DRBC Double collect, fix crit for first return cycle.
   else if ( pd.SystemState() == BLOOD_RUN &&
             runSubState == SS_RP_RBC_DOUBLE &&
             _FirstTimeInDRBCCollectReturn )
   {
      _FirstReturnLimiterActive = true;

      Hn = Hin = pd.RunTargets().RbcCritTarget.Get();

      DataLog(log_level_proc_debug) << "First return in DRBC Collect, adjust res crit" << endmsg;
   }
   else // Default behavior for all return cycles
   {
      //
      // Hematocrit needle, Hematocrit inlet line
      //
      Hn = Hin = pd.Cycle().ResHct.Get();
   }

   // If we have seen our first return and we switch to a draw, end condition.
   if (_FirstReturnLimiterActive && isDrawCycle)
   {
      _FirstReturnLimiterActive     = false;
      _FirstTimeInDRBCCollectReturn = false;
      DataLog(log_level_proc_debug) << "Turning off return limiter" << endmsg;
   }

   const float Qin = pd.Orders().Qin(); // Inlet speed
   const float Qrp = pd.Orders().Qrp(); // Return speed
   const float Qac = pd.Orders().Qac(); // AC speed
   const float Qn  = Qin - Qrp - Qac;   // Flow at the needle

   //
   //
   //   Set the pressure limits based upon set type
   //
   float      lowLimit;
   float      highLimit;
   float      lowVeinPressure  = _LowConfigLimit + 75.0;
   float      highVeinPressure = _HighConfigLimit - 50.0;

   const bool WhiteStampKit    = pd.Status().CassIsPltPlsRBC();

   // Use the 17 guage needle coefficients if it is a white stamp or the special black stamp
   if ( WhiteStampKit || _SpecialBlackStampNeedle )
   {
      lowLimit  = lowVeinPressure  - ( 0.3309f * Qin / ( 1 - Hin ) )
                  - ( 0.3026f * Qn / ( 1 - Hn ) );
      highLimit = highVeinPressure - ( 0.3309f * Qin / ( 1 - Hin ) )
                  - ( 0.3026f * Qn / ( 1 - Hn ) );
   }
   else
   {
      lowLimit  = lowVeinPressure  - ( 0.3309f * Qin / ( 1 - Hin ) )
                  - ( 0.5602f * Qn / ( 1 - Hn ) );
      highLimit = highVeinPressure - ( 0.3309f * Qin / ( 1 - Hin ) )
                  - ( 0.5602f * Qn / ( 1 - Hn ) );

      if ( runSubState == SS_RP_PLASMA_ONLY ) // override for this substate only
      {
         highLimit = 400.0f;
      }
   }

   //
   // The low limit is constant during Blood Prime Inlet line
   //
   if ( currentState.compare("BloodPrimeInlet") == 0 )
   {
      lowLimit = -100.0f;
   }

   //
   // Add some hysteresis for cycle changes and state changes
   //  (pump speed changes could mess up pressure equations)
   //

   //
   // If the cycle switch has just happened, keep the current
   //  limits in place for a while ...
   //
   if  ( pd.CurrentCycleTime().getSecs() < SWITCH_HOLD_TIME )
   {
      //
      // Keep the opposing cycle's limit in place ...
      //
      if ( isDrawCycle )
      {
         highLimit = _LastHighLimit;
      }
      else
      {
         lowLimit = _LastLowLimit;
      }
   }
   else
   {
      if ( isDrawCycle &&
           pd.TrimaSet().CassetteState < ProcTrimaSet_CDS::RINSEBACK ) // Disallow low/high limit changes induced
      {                                                                //  by cycle switches during RINSEBACK
         highLimit = _HighConfigLimit + 90.0;
      }
      else
      {
         lowLimit = _LowConfigLimit - 75.0;
      }
   }

   bool stateChanged = checkStateChange(currentState);
   //
   // If a state change just happened, keep the current limits
   //  in place for a while ...
   //
   if ( _StateChangeTime.getSecs() < SWITCH_HOLD_TIME )
   {
      //
      // Keep the current cycle's limit in place if it's less
      // restrictive than the new limit ...
      //
      if ( isDrawCycle &&
           lowLimit > _LastLowLimit )
      {
         lowLimit = _LastLowLimit;
      }
      else if ( !isDrawCycle &&
                highLimit < _LastHighLimit )
      {
         highLimit = _LastHighLimit;
      }

   }

   // Log pre-cap values on cycle switch or state change to test limiter
   if (pd.Run().CycleSwitch.Get() || stateChanged )
   {
      DataLog(log_level_proc_info) << "APS: Pre-cap limits: low/high=[ " << lowLimit << " " << highLimit
                                   << " ] ; Last low/high=[ " << _LastLowLimit << " " << _LastHighLimit << " ]"
                                   << " isDraw=" << isDrawCycle << endmsg;
   }

   //
   // Don't go too low on the high limit or we may lock ourselves up ...
   //  (Stop at 50mmHg)
   //
   if ( highLimit < 50.0f )
      highLimit = 50.0f;

   //
   // Keep a history
   //
   _LastLowLimit  = lowLimit;
   _LastHighLimit = highLimit;

   //
   // Always make sure the limits don't drop below, or above, the
   //  hard maximum limits ...
   //
   if ( lowLimit < MINIMUM_PRESSURE )
      lowLimit = MINIMUM_PRESSURE;
   if ( highLimit > MAXIMUM_PRESSURE )
      highLimit = MAXIMUM_PRESSURE;

   //
   //   Set the limits
   //
   pd.Orders().APSLow(lowLimit);
   pd.Orders().APSHigh(highLimit);
}

bool APLimiter::checkStateChange (const string& currentState)
{
   ProcData pd;
   //
   // Here we use parent state's currentState() and Proc's current recovery
   // instead of using the ProcState message notification because it occurs
   // *after* the state machine process(), such that setLimits() will miss
   // the transition's first update. (IT 12139)
   //
   const bool changed = ( (currentState != _LastState) ||
                          (pd.RecoveryName() != _LastRecovery) );
   if ( changed )
   {
      //
      // The state of the system has changed ...
      //
      _StateChangeTime.init();            // restart the timer for new state
      _LastState    = currentState;       // save this sub-state
      _LastRecovery = pd.RecoveryName();  // save this recovery state

      DataLog(log_level_proc_debug) << "APS: substate/recovery changed to "
                                    << currentState << "/" << pd.RecoveryName() << endmsg;
   }

   return changed;
}

/* FORMAT HASH 92c1697280c8cd34ea43d270215dedbb */

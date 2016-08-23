/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      EvacuateSetValves.cpp
 *
 */

#include "evacuatesetvalves.h"
#include "procdata.h"
#include "cobeconfig.h"

DEFINE_STATE(EvacuateSetValves);

EvacuateSetValves::EvacuateSetValves()
   : StateAbs(),
     _ValveTimer(0, Callback<EvacuateSetValves>(this, &EvacuateSetValves::valvesTimedOut), TimerMessage::DISARMED),
     _DelayTimer(0, Callback<EvacuateSetValves>(this, &EvacuateSetValves::delayComplete), TimerMessage::DISARMED),
     _DelayComplete(0),
     _RBCAlarm(RBC_VALVE_FAILURE),
     _PlasmaAlarm(PLASMA_VALVE_FAILURE),
     _CollectAlarm(COLLECT_VALVE_FAILURE)
{}

EvacuateSetValves::EvacuateSetValves(const EvacuateSetValves& state)
   : StateAbs(state),
     _ValveTimer(0, Callback<EvacuateSetValves>(this, &EvacuateSetValves::valvesTimedOut), TimerMessage::DISARMED),
     _DelayTimer(0, Callback<EvacuateSetValves>(this, &EvacuateSetValves::delayComplete), TimerMessage::DISARMED),
     _DelayComplete(_DelayComplete),
     _RBCAlarm(RBC_VALVE_FAILURE),
     _PlasmaAlarm(PLASMA_VALVE_FAILURE),
     _CollectAlarm(COLLECT_VALVE_FAILURE)
{
   copyOver(state);
}

EvacuateSetValves::~EvacuateSetValves()
{
   cleanup();
}

int EvacuateSetValves::transitionStatus ()
{
   ProcData pd;

   if (Software_CDS::GetInstance().getFeature(AiroutMitigation))
      return 1;

   //
   // If evacuation volume is set to zero, we just skip the evacuate bags state.
   // Otherwise, we must check to see if all the valves have moved to the correct
   // position, and go to the next state as soon as they have.
   //
   if (CobeConfig::data().EvacuationVolume <= 0)
   {
      return 2;
   }
   else if ( checkRBCValve() &&
             pd.Status().PlasmaValve.IsOpen() &&
             pd.Status().CollectValve.IsOpen() &&
             _DelayComplete )
   {
      return 1;
   }

   return NO_TRANSITION;
}

void EvacuateSetValves::valvesTimedOut ()
{
   ProcData pd;
   //
   // Determine which valve(s) FAILED and set appropriate alarm(s)
   //
   if ( !checkRBCValve() )
   {
      _RBCAlarm.setAlarm();
   }
   if ( !pd.Status().PlasmaValve.IsOpen() )
   {
      _PlasmaAlarm.setAlarm();
   }
   if ( !pd.Status().CollectValve.IsOpen() )
   {
      _CollectAlarm.setAlarm();
   }
}

void EvacuateSetValves::delayComplete ()
{
   _DelayComplete = 1;
}

int EvacuateSetValves::preProcess ()
{
   //
   // Recommand the valves just in case
   //
   commandValves();

   return NORMAL;
}

int EvacuateSetValves::postProcess ()
{
   return NORMAL;
}

int EvacuateSetValves::preEnter ()
{
   //
   // Command all valves to the desired position
   //
   commandValves();

   //
   // Start the timers
   //
   _ValveTimer.interval(10000);   // 10 seconds ( 10000 msecs )

   if ( CobeConfig::data().EvacuationStartupDelay > 0 )
   {
      _DelayTimer.interval( (int)(1000.0 * CobeConfig::data().EvacuationStartupDelay) );
      _DelayComplete = 0;
   }
   else
   {
      _DelayComplete = 1;
   }

   return NORMAL;
}

int EvacuateSetValves::postExit ()
{
   //
   // Delete my messages ...
   //
   cleanup();

   //
   // Stop the timers
   //
   _ValveTimer.interval(0);
   _DelayTimer.interval(0);

   return NORMAL;
}

void EvacuateSetValves::commandValves ()
{
   ProcData pd;
   //
   // Command all valves.  Note that we only do this is evacuation volume
   // is greater than 0.
   //
   if (CobeConfig::data().EvacuationVolume > 0)
   {
      setRBCValve();
      pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_OPEN);
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_OPEN);
   }
}

//
// set RBC valve appropriately for the currently loaded cassette type
//
void EvacuateSetValves::setRBCValve ()
{
   ProcData pd;
   //
   // For standard sets (PltPlsRBC), the RBC valve should be set to the return position,
   // otherwise it should be set to collect (see IT 4689 for details).
   //
   // IT 5558 - RBC valve should always be in return position during evacuation
   //
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
}

//
// check if RBC valve has reached correct final position for currently
// loaded cassette type
//
bool EvacuateSetValves::checkRBCValve ()
{
   ProcData pd;
   //
   // IT 5558 - RBC valve should always be in return position during evacuation
   //
   return pd.Status().RBCValve.IsReturning();
}

void EvacuateSetValves::copyOver (const EvacuateSetValves&)
{}

void EvacuateSetValves::cleanup ()
{}

/* FORMAT HASH 6a01ba6642b684ae939957e2a9771138 */

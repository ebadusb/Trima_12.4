/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      EvacEndSetValves.cpp
 *
 */

#include "evacendsetvalves.h"
#include "cobeconfig.h"
#include "procdata.h"
#include "software_cds.h"     // Get Optional sw.dat settings

DEFINE_STATE(EvacEndSetValves);

EvacEndSetValves::EvacEndSetValves()
   : StateAbs(),
     _ValveTimer(0, Callback<EvacEndSetValves>(this, &EvacEndSetValves::valvesTimedOut), TimerMessage::DISARMED),
     _ValveDwellTimer(0, Callback<EvacEndSetValves>(this, &EvacEndSetValves::valveDwellTimerCompleted), TimerMessage::DISARMED),
     _ValveDwellTimerStarted(0),
     _ValveDwellCompleted(0),
     _RBCAlarm(RBC_VALVE_FAILURE),
     _PlasmaAlarm(PLASMA_VALVE_FAILURE),
     _CollectAlarm(COLLECT_VALVE_FAILURE)
{}

EvacEndSetValves::EvacEndSetValves(const EvacEndSetValves& state)
   : StateAbs(state),
     _ValveTimer(0, Callback<EvacEndSetValves>(this, &EvacEndSetValves::valvesTimedOut), TimerMessage::DISARMED),
     _ValveDwellTimer(0, Callback<EvacEndSetValves>(this, &EvacEndSetValves::valveDwellTimerCompleted), TimerMessage::DISARMED),
     _ValveDwellTimerStarted(state._ValveDwellTimerStarted),
     _ValveDwellCompleted(state._ValveDwellCompleted),
     _RBCAlarm(RBC_VALVE_FAILURE),
     _PlasmaAlarm(PLASMA_VALVE_FAILURE),
     _CollectAlarm(COLLECT_VALVE_FAILURE)
{
   copyOver(state);
}

EvacEndSetValves::~EvacEndSetValves()
{
   cleanup();
}

int EvacEndSetValves::transitionStatus ()
{
   ProcData pd;


   if (Software_CDS::GetInstance().getFeature(AiroutMitigation))
   {
      DataLog(log_level_proc_disposable_test_info) << " Air mitigation on -- skipping this state" << endmsg;
      return 1;
   }


   //
   // If evacuation is disabled, just transition to the next state immediately
   //
   if (    CobeConfig::data().EvacuationVolume <= 0
           && pd.Status().CentrifugeRPM() >= CobeConfig::data().RPMBloodPrime )

   {
      return 1;
   }

   //
   // Otherwise, check and see if ALL valves are in the proper position
   //
   else if ( pd.Status().RBCValve.IsOpen() &&
             pd.Status().PlasmaValve.IsOpen() &&
             pd.Status().CollectValve.IsOpen() )
   {
      //
      // If valves are in the right position, start the
      // valve open dwell timer.  Once they have been open
      // long enough, transition to the next state.
      //
      if ( !_ValveDwellTimerStarted )
      {
         _ValveDwellTimer.interval( (int)( CobeConfig::data().EvacuationValveOpenDwell * 1000) );
         _ValveDwellTimerStarted = 1;
      }

      if (    _ValveDwellCompleted
              && pd.Status().CentrifugeRPM() >= CobeConfig::data().RPMBloodPrime )
      {
         return 1;
      }
   }
   //
   // Otherwise, we are not in the right position.  Stop the
   // valve open dwell timer if it has been started.
   //
   else if ( _ValveDwellTimerStarted )
   {
      _ValveDwellTimer.interval(0);
      _ValveDwellTimerStarted = 0;
   }

   return NO_TRANSITION;
}

void EvacEndSetValves::valvesTimedOut ()
{
   ProcData pd;

   //
   // Determine which valve(s) FAILED and set appropriate alarm(s)
   //
   if ( !pd.Status().RBCValve.IsOpen() )
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

void EvacEndSetValves::valveDwellTimerCompleted ()
{
   _ValveDwellCompleted = 1;
}

int EvacEndSetValves::preProcess ()
{
   //
   // Recommand the valves just in case
   //
   commandValves();

   //
   // Command the centrifuge ...
   //
   ProcData pd;
   pd.Orders().RPM(CobeConfig::data().RPMBloodPrime);
   DataLog(log_level_proc_debug) << "Centrifuge command: " << pd.Orders().RPM() << endmsg;

   return NORMAL;
}

int EvacEndSetValves::postProcess ()
{
   return NORMAL;
}

int EvacEndSetValves::preEnter ()
{
   //
   // Command all valves to the desired position
   //
   commandValves();

   //
   // Setup timers
   //
   _ValveTimer.interval(10000);   // 10 seconds ( 10000 msecs )

   return NORMAL;
}

int EvacEndSetValves::postExit ()
{
   //
   // Delete my messages ...
   //
   cleanup();

   //
   // Stop the timer ...
   //
   _ValveTimer.interval(0);

   return NORMAL;
}

void EvacEndSetValves::commandValves ()
{


   if ( CobeConfig::data().EvacuationVolume > 0 )
   {
      //
      // Command all valves
      //
      ProcData pd;
      pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_OPEN);
      pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_OPEN);
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_OPEN);
   }
}

void EvacEndSetValves::copyOver (const EvacEndSetValves&)
{}

void EvacEndSetValves::cleanup ()
{}

/* FORMAT HASH 5305c629c170cf1233fdd948f0d6e0d3 */

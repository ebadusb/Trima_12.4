/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      EvacEndSetValves.cpp
 *
 */

#include "evacendsetvalvesAirMitigationBlack.h"
#include "cobeconfig.h"
#include "procdata.h"

DEFINE_STATE(EvacEndSetValvesARMBlack);

EvacEndSetValvesARMBlack::EvacEndSetValvesARMBlack()
   : StateAbs(),
     _ValveTimer(0, Callback<EvacEndSetValvesARMBlack>(this, &EvacEndSetValvesARMBlack::valvesTimedOut), TimerMessage::DISARMED),
     _ValveDwellTimer(0, Callback<EvacEndSetValvesARMBlack>(this, &EvacEndSetValvesARMBlack::valveDwellTimerCompleted), TimerMessage::DISARMED),
     _ValveDwellTimerStarted(0),
     _ValveDwellCompleted(0),
     _RBCAlarm(RBC_VALVE_FAILURE),
     _PlasmaAlarm(PLASMA_VALVE_FAILURE),
     _CollectAlarm(COLLECT_VALVE_FAILURE),
     _plsSet(false),
     _plsValveDwellTimer(0, Callback<EvacEndSetValvesARMBlack>(this, &EvacEndSetValvesARMBlack::plsValveDwellTimerCompleted), TimerMessage::DISARMED),
     _plsValveDwellTimerStarted(false),
     _plsValveDwellCompleted(false),
     _isBalckStamp(false)

{}

EvacEndSetValvesARMBlack::EvacEndSetValvesARMBlack(const EvacEndSetValvesARMBlack& state)
   : StateAbs(state),
     _ValveTimer(0, Callback<EvacEndSetValvesARMBlack>(this, &EvacEndSetValvesARMBlack::valvesTimedOut), TimerMessage::DISARMED),
     _ValveDwellTimer(0, Callback<EvacEndSetValvesARMBlack>(this, &EvacEndSetValvesARMBlack::valveDwellTimerCompleted), TimerMessage::DISARMED),
     _ValveDwellTimerStarted(state._ValveDwellTimerStarted),
     _ValveDwellCompleted(state._ValveDwellCompleted),
     _RBCAlarm(RBC_VALVE_FAILURE),
     _PlasmaAlarm(PLASMA_VALVE_FAILURE),
     _CollectAlarm(COLLECT_VALVE_FAILURE),
     _plsSet(false),
     _plsValveDwellTimer(0, Callback<EvacEndSetValvesARMBlack>(this, &EvacEndSetValvesARMBlack::plsValveDwellTimerCompleted), TimerMessage::DISARMED),
     _plsValveDwellTimerStarted(false),
     _plsValveDwellCompleted(false),
     _isBalckStamp(false)

{
   copyOver(state);
}

EvacEndSetValvesARMBlack::~EvacEndSetValvesARMBlack()
{
   cleanup();
}

int EvacEndSetValvesARMBlack::transitionStatus ()
{

   ProcData pd;

   if (!Software_CDS::GetInstance().getFeature(AiroutMitigation))
   {
      DataLog(log_level_proc_disposable_test_info) << " Air mitigation off -- skipping this state" << endmsg;
      return 1;
   }

   if  (pd.TrimaSet().Cassette != cassette::RBCPLS)
   {
      DataLog(log_level_proc_disposable_test_info) << " EvacEndSetValvesARMBlack -- skipping this state for white stamp" << endmsg;
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
             pd.Status().CollectValve.IsOpen()   &&
             _plsSet
             )
   {
      //
      // If valves are in the right position, start the
      // valve open dwell timer.  Once they have been open
      // long enough, transition to the next state.
      //
      if ( !_ValveDwellTimerStarted )
      {
         _ValveDwellTimer.interval( (int)( 3.0 * 1000) );
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

void EvacEndSetValvesARMBlack::valvesTimedOut ()
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

void EvacEndSetValvesARMBlack::valveDwellTimerCompleted ()
{
   _ValveDwellCompleted = 1;
}
void EvacEndSetValvesARMBlack::plsValveDwellTimerCompleted ()
{
   _plsValveDwellCompleted = 1;
}

int EvacEndSetValvesARMBlack::preProcess ()
{
   //
   // Recommand the valves just in case
   //
   // commandValves();

   //
   // Command the centrifuge ...
   //

   return NORMAL;
}

int EvacEndSetValvesARMBlack::postProcess ()
{
   ProcData pd;

   if ( pd.Status().RBCValve.IsOpen() &&
        pd.Status().PlasmaValve.IsOpen() &&
        pd.Status().CollectValve.IsOpen())
   {


      //
      if ( !_plsValveDwellTimerStarted )
      {
         _plsValveDwellTimer.interval(2000);
         _plsValveDwellTimerStarted = 1;
      }

      if ( _plsValveDwellCompleted  )
      {
         pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);
      }

   }


   if ( pd.Status().RBCValve.IsOpen() &&
        pd.Status().PlasmaValve.IsReturning() &&
        pd.Status().CollectValve.IsOpen()
        )
   {
      _plsSet = true;
   }


   return NORMAL;
}


int EvacEndSetValvesARMBlack::preEnter ()
{
   //
   // Command all valves to the desired position
   //
   commandValves();

   //
   // Setup timers
   //
   _ValveTimer.interval(10000);   // 10 seconds ( 10000 msecs )
                                  //

   ProcData pd;
   pd.Orders().RPM(CobeConfig::data().RPMBloodPrime);
   DataLog(log_level_proc_debug) << "Centrifuge command: " << pd.Orders().RPM() << endmsg;


   return NORMAL;
}

int EvacEndSetValvesARMBlack::postExit ()
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

void EvacEndSetValvesARMBlack::commandValves ()
{
   //
   // Command all valves
   //
   if ( CobeConfig::data().EvacuationVolume > 0 )
   {
      ProcData pd;

      pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_OPEN);
      pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_OPEN);
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_OPEN);
   }


}

void EvacEndSetValvesARMBlack::copyOver (const EvacEndSetValvesARMBlack&)
{}

void EvacEndSetValvesARMBlack::cleanup ()
{}

/* FORMAT HASH 7245b5799152cd76dc4d7379107e3e62 */

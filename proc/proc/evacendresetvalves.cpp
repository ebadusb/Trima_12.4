/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      EvacEndResetValves.cpp
 *
 */

#include "cobeconfig.h"
#include "evacendresetvalves.h"
#include "states.h"
#include "procdata.h"

#include "MssCommon.h"

DEFINE_STATE(EvacEndResetValves);

EvacEndResetValves::EvacEndResetValves()
   : StateAbs(),
     _ValveTimer(0, Callback<EvacEndResetValves>(this, &EvacEndResetValves::valvesTimedOut), TimerMessage::DISARMED),
     _RBCAlarm(RBC_VALVE_FAILURE),
     _PlasmaAlarm(PLASMA_VALVE_FAILURE),
     _CollectAlarm(COLLECT_VALVE_FAILURE)
{}

EvacEndResetValves::EvacEndResetValves(const EvacEndResetValves& state)
   : StateAbs(state),
     _ValveTimer(0, Callback<EvacEndResetValves>(this, &EvacEndResetValves::valvesTimedOut), TimerMessage::DISARMED),
     _RBCAlarm(RBC_VALVE_FAILURE),
     _PlasmaAlarm(PLASMA_VALVE_FAILURE),
     _CollectAlarm(COLLECT_VALVE_FAILURE)
{}

EvacEndResetValves::~EvacEndResetValves()
{}

int EvacEndResetValves::transitionStatus ()
{
   ProcData pd;

   //
   // Check and see if ALL valves are in the proper position
   //

   if (pd.Status().CentrifugeRPM() >= CobeConfig::data().RPMBloodPrime)
   {

      if (    pd.Status().RBCValve.IsReturning()
              && pd.Status().PlasmaValve.IsReturning()
              && pd.Status().CollectValve.IsReturning() )
      {
         return 1;
      }



   }

   return NO_TRANSITION;
}

void EvacEndResetValves::valvesTimedOut ()
{
   ProcData pd;

   //
   // Determine which valve(s) FAILED and set appropriate alarm(s)
   //
   if ( !pd.Status().RBCValve.IsReturning() )
   {
      _RBCAlarm.setAlarm();
   }
   if ( !pd.Status().PlasmaValve.IsReturning() )
   {
      _PlasmaAlarm.setAlarm();
   }


   if ( !pd.Status().CollectValve.IsReturning() )
   {
      _CollectAlarm.setAlarm();
   }


}

int EvacEndResetValves::preProcess ()
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

int EvacEndResetValves::postProcess ()
{
   return NORMAL;
}

int EvacEndResetValves::preEnter ()
{
   //
   // Command all valves to the desired position
   //
   commandValves();

   //
   // Start the timer ...
   //
   _ValveTimer.interval(10000);   // 10 seconds ( 10000 msecs )

   return NORMAL;
}

int EvacEndResetValves::postExit ()
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

void EvacEndResetValves::commandValves ()
{
   ProcData pd;

   //
   // Command all valves
   //
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);

}

void EvacEndResetValves::copyOver (const EvacEndResetValves&)
{}

void EvacEndResetValves::cleanup ()
{}

/* FORMAT HASH 9f2a25e2ea5095266dffe37941235cb0 */

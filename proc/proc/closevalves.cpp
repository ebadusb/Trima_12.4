/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      closevalves.cpp
 *
 */

#include "closevalves.h"
#include "procdata.h"

DEFINE_STATE(CloseValves);

CloseValves::CloseValves()
   : StateAbs(),
     _ValveTimer(0, Callback<CloseValves>(this, &CloseValves::valvesTimedOut), TimerMessage::DISARMED),
     _RBCAlarm(RBC_VALVE_FAILURE),
     _PlasmaAlarm(PLASMA_VALVE_FAILURE),
     _CollectAlarm(COLLECT_VALVE_FAILURE)
{}

CloseValves::CloseValves(const CloseValves& state)
   : StateAbs(state),
     _ValveTimer(0, Callback<CloseValves>(this, &CloseValves::valvesTimedOut), TimerMessage::DISARMED),
     _RBCAlarm(RBC_VALVE_FAILURE),
     _PlasmaAlarm(PLASMA_VALVE_FAILURE),
     _CollectAlarm(COLLECT_VALVE_FAILURE)
{}

CloseValves::~CloseValves()
{}

int CloseValves::transitionStatus ()
{
   ProcData pd;
   //
   // Check and see if ALL valves are return
   //
   if ( pd.Status().RBCValve.IsReturning() &&
        pd.Status().PlasmaValve.IsReturning() &&
        pd.Status().CollectValve.IsReturning() )
   {
      return 1;
   }


   return NO_TRANSITION;
}

void CloseValves::valvesTimedOut ()
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

int CloseValves::preProcess ()
{
   //
   // Recommand the valves to open ...
   //
   commandToReturn();

   return NORMAL;
}

int CloseValves::postProcess ()
{
   return NORMAL;
}

int CloseValves::preEnter ()
{
   //
   // Command all valves to the return position
   //
   commandToReturn();

   //
   // Start the timer ...
   //
   _ValveTimer.interval(10000);   // 10 seconds ( 10000 msecs )

   return NORMAL;
}

int CloseValves::postExit ()
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

void CloseValves::commandToReturn ()
{
   ProcData pd;

   //
   // Command all valves to the return position
   //
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);     //
                                                            //

}

void CloseValves::copyOver (const CloseValves&)
{}

void CloseValves::cleanup ()
{}

/* FORMAT HASH b7b96a2154f148ff34471b8ea3596370 */

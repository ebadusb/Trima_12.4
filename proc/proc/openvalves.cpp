/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      openvalves.cpp
 *
 */

#include "openvalves.h"
#include "procdata.h"

DEFINE_STATE(OpenValves);

OpenValves::OpenValves()
   : StateAbs(),
     _ValveTimer(0, Callback<OpenValves>(this, &OpenValves::valvesTimedOut), TimerMessage::DISARMED),
     _RBCAlarm(RBC_VALVE_FAILURE),
     _PlasmaAlarm(PLASMA_VALVE_FAILURE),
     _CollectAlarm(COLLECT_VALVE_FAILURE),
     _ValvesSeenOpen(0)
{
   maintainHistory(0);
}

OpenValves::OpenValves(const OpenValves& state)
   : StateAbs(state),
     _ValveTimer(0, Callback<OpenValves>(this, &OpenValves::valvesTimedOut), TimerMessage::DISARMED),
     _RBCAlarm(RBC_VALVE_FAILURE),
     _PlasmaAlarm(PLASMA_VALVE_FAILURE),
     _CollectAlarm(COLLECT_VALVE_FAILURE),
     _ValvesSeenOpen(state._ValvesSeenOpen)
{
   copyOver(state);
}

OpenValves::~OpenValves()
{
   cleanup();
}

int OpenValves::transitionStatus ()
{
   //
   // Check and see if the valves are open
   //
   if ( _ValvesSeenOpen )
   {
      return 1;
   }
   return NO_TRANSITION;
}

void OpenValves::valvesTimedOut ()
{
   ProcData pd;
   //
   // Determine which valve(s) FAILED and set appropriate alarm(s)
   //
   if ( !pd.Status().RBCValve.IsOpen() )
   {
      _RBCAlarm.setAlarm();
      DataLog(log_level_proc_debug) << "Set RBC valve alarm " << RBC_VALVE_FAILURE << endmsg;
   }
   if ( !pd.Status().PlasmaValve.IsOpen() )
   {
      _PlasmaAlarm.setAlarm();
      DataLog(log_level_proc_debug) << "Set Plasma valve alarm " << PLASMA_VALVE_FAILURE << endmsg;
   }
   if ( !pd.Status().CollectValve.IsOpen() )
   {
      _CollectAlarm.setAlarm();
      DataLog(log_level_proc_debug) << "Set Collect valve alarm " << COLLECT_VALVE_FAILURE << endmsg;
   }

}

int OpenValves::preProcess ()
{
   //
   // Recommand the valves to open ...
   //
   commandToOpen();

   return NORMAL;
}

int OpenValves::postProcess ()
{
   ProcData pd;
   //
   // Check and see if ALL valves are open
   //
   if ( pd.Status().RBCValve.IsOpen() &&
        pd.Status().PlasmaValve.IsOpen() &&
        pd.Status().CollectValve.IsOpen() )
   {
      //
      // The valves are open ...
      //
      _ValvesSeenOpen = 1;
      DataLog(log_level_proc_debug) << "All valves in open position" << endmsg;
   }
   return NORMAL;
}

int OpenValves::preEnter ()
{
   //
   // Start the timer ...
   //
   _ValveTimer.interval(10000);   // 10 seconds ( 10000 msecs )
   DataLog(log_level_proc_debug) << "Start valve timer" << endmsg;

   return NORMAL;
}

int OpenValves::postExit ()
{
   //
   // Delete my messages ...
   //
   cleanup();

   //
   // Stop the timer ...
   //
   _ValveTimer.interval(0);
   DataLog(log_level_proc_debug) << "Stop valve timer" << endmsg;

   return NORMAL;
}

void OpenValves::commandToOpen ()
{
   ProcData pd;
   //
   // Command all valves to open ...
   //
   pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_OPEN);
   pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_OPEN);
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_OPEN);
   DataLog(log_level_proc_debug) << "Command valves to open position " << HAL_VALVE_OPEN << endmsg;
}

void OpenValves::reset ()
{
   //
   // Reset the flags ...
   //
   _ValvesSeenOpen = 0;
}

void OpenValves::copyOver (const OpenValves&)
{}

void OpenValves::cleanup ()
{}

/* FORMAT HASH c6eb8daccf07ee328f980fa77ab9c524 */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      InletPressTest2.cpp
 *
 */

#include "InletPressTest2.h"
#include "procdata.h"

DEFINE_STATE(InletPressTest2);

InletPressTest2::InletPressTest2()
   : StateAbs(),
     _PressureCheckTimer(0, Callback<InletPressTest2>(this, &InletPressTest2::pressureCheck), TimerMessage::DISARMED),
     _DecayAlarm(FAILURE_TO_PRESSURIZE_INLET),
     _InitialAPS(0.0f),
     _PressureDecayed(0),
     _PressureChecked(0)
{}

InletPressTest2::InletPressTest2(const InletPressTest2& state)
   : StateAbs(state),
     _PressureCheckTimer(0, Callback<InletPressTest2>(this, &InletPressTest2::pressureCheck), TimerMessage::DISARMED),
     _DecayAlarm(FAILURE_TO_PRESSURIZE_INLET),
     _InitialAPS(state._InitialAPS),
     _PressureDecayed(state._PressureDecayed),
     _PressureChecked(state._PressureChecked)
{}

InletPressTest2::~InletPressTest2()
{}

int InletPressTest2::transitionStatus ()
{
   //
   // If the pressure was found to decay ...
   //
   if ( _PressureDecayed )
   {
      DataLog(log_level_proc_disposable_test_error) << "Pressure decayed" << endmsg;
      return 1;
   }
   else if ( _PressureChecked )
   {
      DataLog(log_level_proc_disposable_test_info) << "Pressure check passed" << endmsg;
      return 2;
   }
   return NO_TRANSITION;
}

void InletPressTest2::pressureCheck ()
{
   ProcData pd;

   //
   // Check for access pressure decay condition
   //
   float diff = _InitialAPS - pd.Status().APS();
   DataLog(log_level_proc_debug) << "Pressure difference = " << diff << "mmHg" << endmsg;
   if (  diff >= 50.0f )
   {
      //
      // APS decayed - set alarm
      //
      _DecayAlarm.setAlarm();
      DataLog(log_level_proc_debug) << "Set the pressure decay alarm" << ACCESS_PRESSURE_DECAY_FAILURE << endmsg;

      //
      // Set the pressure decayed flag ...
      //
      _PressureDecayed = 1;
   }

   //
   // Set the flag to indicate the timer has expired ...
   //
   _PressureChecked = 1;
}

int InletPressTest2::preProcess ()
{
   return NORMAL;
}

int InletPressTest2::postProcess ()
{
   return NORMAL;
}

int InletPressTest2::preEnter ()
{
   ProcData pd;

   //
   // Get the initial APS reading ...
   //
   _InitialAPS = pd.Status().APS();
   DataLog(log_level_proc_debug) << "Initial APS = " << _InitialAPS << "mmHg" << endmsg;

   //
   // Make sure all pumps are stopped ...
   //
   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qrp(0.0f);
   DataLog(log_level_proc_debug) << "Stop all pumps" << endmsg;


   //
   // Set up the timer ...
   //
   _PressureCheckTimer.interval(3000);   // 3 seconds ( 3000 msecs )
   DataLog(log_level_proc_debug) << "Start the pressure check timer" << endmsg;

   return NORMAL;
}

int InletPressTest2::postExit ()
{
   //
   // Stop the timer ...
   //
   _PressureCheckTimer.interval(0);
   DataLog(log_level_proc_debug) << "Stop the pressure check timer" << endmsg;

   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void InletPressTest2::reset ()
{
   //
   // Reset the flags ...
   //
   _PressureDecayed = 0;
   _PressureChecked = 0;
}

void InletPressTest2::copyOver (const InletPressTest2&)
{}

void InletPressTest2::cleanup ()
{}

/* FORMAT HASH 01f5a90f3df6e45d4fecbe20109506fd */

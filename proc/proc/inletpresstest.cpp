/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      inletpresstest.cpp
 *
 */

#include "inletpresstest.h"
#include "procdata.h"

DEFINE_STATE(InletPressTest);

InletPressTest::InletPressTest()
   : StateAbs(),
     _PressureCheckTimer(0, Callback<InletPressTest>(this, &InletPressTest::pressureCheck), TimerMessage::DISARMED),
     _DecayAlarm(ACCESS_PRESSURE_DECAY_FAILURE),
     _InitialAPS(0.0f),
     _PressureDecayed(0),
     _PressureChecked(0)
{}

InletPressTest::InletPressTest(const InletPressTest& state)
   : StateAbs(state),
     _PressureCheckTimer(0, Callback<InletPressTest>(this, &InletPressTest::pressureCheck), TimerMessage::DISARMED),
     _DecayAlarm(ACCESS_PRESSURE_DECAY_FAILURE),
     _InitialAPS(state._InitialAPS),
     _PressureDecayed(state._PressureDecayed),
     _PressureChecked(state._PressureChecked)
{
   copyOver(state);
}

InletPressTest::~InletPressTest()
{
   cleanup();
}

int InletPressTest::transitionStatus ()
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

void InletPressTest::pressureCheck ()
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

int InletPressTest::preProcess ()
{
   return NORMAL;
}

int InletPressTest::postProcess ()
{
   return NORMAL;
}

int InletPressTest::preEnter ()
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
   // Check to see there is a valid pressure for the test at the APS.
   //
   if ( _InitialAPS < 375.0f )
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
   // Set up the timer ...
   //
   _PressureCheckTimer.interval(3000);   // 3 seconds ( 3000 msecs )
   DataLog(log_level_proc_debug) << "Start the pressure check timer" << endmsg;

   return NORMAL;
}

int InletPressTest::postExit ()
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

void InletPressTest::reset ()
{
   //
   // Reset the flags ...
   //
   _PressureDecayed = 0;
   _PressureChecked = 0;
}

void InletPressTest::copyOver (const InletPressTest&)
{}

void InletPressTest::cleanup ()
{}

/* FORMAT HASH 38533b65a42d93325b70d1e797db26e2 */

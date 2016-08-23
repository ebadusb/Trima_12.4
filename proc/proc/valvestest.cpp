/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      valvestest.cpp
 *
 */


#include "valvestest.h"
#include "procdata.h"

DEFINE_STATE(ValvesTest) ValvesTest::ValvesTest()
   : StateAbs(),
     _ValveTimer(0, Callback<ValvesTest>(this, &ValvesTest::valvesTooSlow), TimerMessage::DISARMED),
     _ValveAlarm(VALVE_TEST_FAILURE),
     _TestFailed(false),
     _AllValvesSeenReturn(false),
     _AllValvesSeenCollect(false),
     _AllValvesSeenOpen(false),
     _TimerStarted(false)
{}

ValvesTest::ValvesTest(const ValvesTest& state)
   : StateAbs(state),
     _ValveTimer(0, Callback<ValvesTest>(this, &ValvesTest::valvesTooSlow), TimerMessage::DISARMED),
     _ValveAlarm(VALVE_TEST_FAILURE),
     _TestFailed(false),
     _AllValvesSeenReturn(false),
     _AllValvesSeenCollect(false),
     _AllValvesSeenOpen(false),
     _TimerStarted(false)
{}

ValvesTest::~ValvesTest()
{}

int ValvesTest::transitionStatus ()
{
   //
   // Check and see if ALL valves have passed all tests
   //
   if ( _AllValvesSeenOpen && _AllValvesSeenCollect && _AllValvesSeenReturn )
   {
      //
      // Test passed ...
      //
      DataLog(log_level_proc_startup_test_info) << "Valves test passed" << endmsg;
      return 1;
   }

   return NO_TRANSITION;
}

void ValvesTest::valvesTooSlow ()
{
   //
   // Send alarm message indicating valve tests failed
   //
   _ValveAlarm.setAlarm();
   DataLog(log_level_proc_startup_test_error) << "Set valve test failure alarm " << VALVE_TEST_FAILURE << endmsg;

   //
   // Test failed ...
   //
   _TestFailed = 1;
}

int ValvesTest::preProcess ()
{
   ProcData pd;
   if ( !_AllValvesSeenReturn )
   {
      // Perform Return valve movements
      orderValvesToReturn();
      if ( pd.Status().RBCValve.IsReturning() &&
           pd.Status().PlasmaValve.IsReturning() &&
           pd.Status().CollectValve.IsReturning() )
      {
         _AllValvesSeenReturn = true;
         _TimerStarted        = false;
         DataLog(log_level_proc_startup_test_info) << "All valves seen return" << endmsg;
      }
   }
   else if ( !_AllValvesSeenCollect )
   {
      // Perform Collect valve movements
      orderValvesToCollect();
      if ( pd.Status().RBCValve.IsCollecting() &&
           pd.Status().PlasmaValve.IsCollecting() &&
           pd.Status().CollectValve.IsCollecting() )
      {
         _AllValvesSeenCollect = true;
         _TimerStarted         = false;
         DataLog(log_level_proc_startup_test_info) << "All valves seen collecting" << endmsg;
      }
   }
   else if ( !_AllValvesSeenOpen )
   {
      // Perform Open valve movements
      orderValvesToOpen();
      if ( pd.Status().RBCValve.IsOpen() &&
           pd.Status().PlasmaValve.IsOpen() &&
           pd.Status().CollectValve.IsOpen() )
      {
         _AllValvesSeenOpen = true;
         _TimerStarted      = false;
         DataLog(log_level_proc_startup_test_info) << "All valves seen open" << endmsg;
      }
   }
   return NORMAL;
}

int ValvesTest::postProcess ()
{
   return NORMAL;
}

int ValvesTest::preEnter ()
{
   //
   // Reset the test failed flag ...
   //
   _TestFailed = 0;

   //
   // Reset the position flags ...
   //
   _AllValvesSeenReturn  = 0;
   _AllValvesSeenCollect = 0;
   _AllValvesSeenOpen    = 0;


   //
   // Set up the valve timer ...
   //
   _TimerStarted = false;

   return NORMAL;
}

int ValvesTest::postExit ()
{
   //
   // Stop the timer ...
   //
   _ValveTimer.interval(0);
   DataLog(log_level_proc_debug) << "Stop valves timer" << endmsg;

   return NORMAL;
}

void ValvesTest::orderValvesToReturn ()
{
   ProcData pd;
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   DataLog(log_level_proc_debug) << "Set RBC valve to return " << HAL_VALVE_RETURN << endmsg;
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);
   DataLog(log_level_proc_debug) << "Set Plasma valve to return " << HAL_VALVE_RETURN << endmsg;
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   DataLog(log_level_proc_debug) << "Set Collect valve to return " << HAL_VALVE_RETURN << endmsg;
   if ( !_TimerStarted )
   {
      _ValveTimer.interval(10000);
      _TimerStarted = true;
      DataLog(log_level_proc_debug) << "Set valves timer to 10000" << endmsg;
   }
}

void ValvesTest::orderValvesToCollect ()
{
   ProcData pd;
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_COLLECT);
   DataLog(log_level_proc_debug) << "Set RBC valve to collect " << HAL_VALVE_COLLECT << endmsg;
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_COLLECT);
   DataLog(log_level_proc_debug) << "Set Plasma valve to collect " << HAL_VALVE_COLLECT << endmsg;
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_COLLECT);
   DataLog(log_level_proc_debug) << "Set Collect valve to collect " << HAL_VALVE_COLLECT << endmsg;
   if ( !_TimerStarted )
   {
      _ValveTimer.interval(10000);
      _TimerStarted = true;
      DataLog(log_level_proc_debug) << "Set valves timer to 10000" << endmsg;
   }
}

void ValvesTest::orderValvesToOpen ()
{
   ProcData pd;
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_OPEN);
   DataLog(log_level_proc_debug) << "Set RBC valve to open " << HAL_VALVE_OPEN << endmsg;
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_OPEN);
   DataLog(log_level_proc_debug) << "Set Plasma valve to open " << HAL_VALVE_OPEN << endmsg;
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_OPEN);
   DataLog(log_level_proc_debug) << "Set Collect valve to open " << HAL_VALVE_OPEN << endmsg;
   if ( !_TimerStarted )
   {
      _ValveTimer.interval(10000);
      _TimerStarted = true;
      DataLog(log_level_proc_debug) << "Set valves timer to 10000" << endmsg;
   }
}

/* FORMAT HASH bed919947bc7ffe67101aa09bda4654b */

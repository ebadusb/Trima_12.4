/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      inletdecaytest2.cpp
 *
 */

#include "inletDecayTest2.h"
#include <math.h>
#include "procdata.h"
#include "cobeconfig.h"

//
// IT 3424
//  The volume accrual must be greater than this number to trigger
//   a problem
//
const float deltaVolume = 0.002; // mls


DEFINE_STATE(InletDecayTest2);

InletDecayTest2::InletDecayTest2()
   : StateAbs(),
     _PressureTimer(0, Callback<InletDecayTest2>(this, &InletDecayTest2::pressureCheck), TimerMessage::DISARMED),
     _PumpTimer(0, Callback<InletDecayTest2>(this, &InletDecayTest2::pumpProblem), TimerMessage::DISARMED),
     _DecayAlarm(ACCESS_PRESSURE_DECAY_FAILURE),
     _InitialAPS(0.0f),
     _InletVolume(0.0f),
     _ACVolume(0.0f),
     _ReturnVolume(0.0f),
     _TestStarted(0),
     _TestPassed(0),
     _TestFailed(0),
     _TestPassedCount(0)
{}

InletDecayTest2::InletDecayTest2(const InletDecayTest2& state)
   : StateAbs(state),
     _PressureTimer(0, Callback<InletDecayTest2>(this, &InletDecayTest2::pressureCheck), TimerMessage::DISARMED),
     _PumpTimer(0, Callback<InletDecayTest2>(this, &InletDecayTest2::pumpProblem), TimerMessage::DISARMED),
     _DecayAlarm(ACCESS_PRESSURE_DECAY_FAILURE),
     _InitialAPS(state._InitialAPS),
     _InletVolume(state._InletVolume),
     _ACVolume(state._ACVolume),
     _ReturnVolume(state._ReturnVolume),
     _TestStarted(state._TestStarted),
     _TestPassed(state._TestPassed),
     _TestFailed(state._TestFailed),
     _TestPassedCount(state._TestPassedCount)
{}

InletDecayTest2::~InletDecayTest2()
{}

int InletDecayTest2::transitionStatus ()
{
   //
   // The test failed ...
   //
   if ( _TestFailed == 1 )
   {
      DataLog(log_level_proc_disposable_test_error) << "Inlet decay test failed" << endmsg;
      return 1;
   }

   //
   // The test passed ...
   //
   if ( _TestPassed == 1 )
   {
      DataLog(log_level_proc_disposable_test_info) << "Inlet decay test passed" << endmsg;
      return 2;
   }

   return NO_TRANSITION;
}

void InletDecayTest2::pressureCheck ()
{
   ProcData pd;

   //
   // Reset the test started flag ...
   //
   _TestStarted = 0;

   //
   // Check for access pressure decay condition
   //
   float diff = pd.Status().APS() - _InitialAPS;
   // current APS-initialAPS to get a positive number for diff


   DataLog(log_level_proc_info) << "APS decayed " << diff << "mmHg" << endmsg;
   if ( diff < CobeConfig::data().DispTestDeltaAPS )
   {

      //
      // Increment the count ...
      //
      _TestPassedCount++;
      DataLog(log_level_proc_info) << "Test passed count " << _TestPassedCount << endmsg;
      if ( _TestPassedCount >1 )
      {
         //
         // NO decay ...
         //
         _TestPassed = 1;
      }
      else
      {
         //
         // Run the test again ...
         //
         setUpTest();
      }

      //
      // Stop the timer ...
      //
      _PressureTimer.interval(0);
      DataLog(log_level_proc_debug) << "Stop the pressure timer" << endmsg;

   }
   else
   {

      //
      // Decay ...
      //
      _TestFailed = 1;

      //
      // APS decayed - set alarm
      //
      _DecayAlarm.setAlarm();
      DataLog(log_level_proc_debug) << "Set the pressure decay alarm " << ACCESS_PRESSURE_DECAY_FAILURE << endmsg;


   }
}

void InletDecayTest2::pumpProblem ()
{
   ProcData pd;

   //
   // Log the reason for failure
   //
   DataLog(log_level_proc_disposable_test_error) << "Access Pressure Test: Inlet change = " << _InletVolume << endmsg;
   DataLog(log_level_proc_disposable_test_error) << "Access Pressure Test: AC change = " << _ACVolume << endmsg;
   DataLog(log_level_proc_disposable_test_error) << "Access Pressure Test: Return change = " << _ReturnVolume << endmsg;
   DataLog(log_level_proc_disposable_test_error) << "Access Pressure Test: Actual Inlet: " << pd.Status().InletPump.ActFlow() << endmsg;
   DataLog(log_level_proc_disposable_test_error) << "Access Pressure Test: Actual AC: " << pd.Status().ACPump.ActFlow() << endmsg;
   DataLog(log_level_proc_disposable_test_error) << "Access Pressure Test: Actual Return: " << pd.Status().ReturnPump.ActFlow() << endmsg;

   //
   // Set the alarm
   //
   _DecayAlarm.setAlarm();
   DataLog(log_level_proc_debug) << "Set the pressure decay alarm " << ACCESS_PRESSURE_DECAY_FAILURE << endmsg;

   //
   // Set the test failed flag
   //
   _TestFailed = 1;
}

int InletDecayTest2::preProcess ()
{
   ProcData pd;

   //
   // The pumps have already been stopped and we are
   //  now waiting for the timer to finish the test ...
   //
   if ( _TestStarted == 1 )
   {
      return NORMAL;
   }

   //
   // Accrue volumes ...
   //
   _InletVolume  += pd.Status().InletPump.dV();
   _ACVolume     += pd.Status().ACPump.dV();
   _ReturnVolume += pd.Status().ReturnPump.dV();

   int inletStopped  = ( pd.Orders().Qin() == 0.0f );
   int acStopped     = ( pd.Orders().Qac() == 0.0f );
   int returnStopped = ( pd.Orders().Qrp() == 0.0f );

   //
   // Turn pumps approximately 1/3 revolution
   //
   if ( !inletStopped &&
        _InletVolume >= 0.28f)
   {
      pd.Orders().Qin(0.0f);
      DataLog(log_level_proc_debug) << "Command inlet pump off" << endmsg;
   }
   else if ( !inletStopped )
   {
      pd.Orders().Qin(20.0f);
   }

   if ( !acStopped &&
        _ACVolume >= 0.28f)
   {
      pd.Orders().Qac(0.0f);
      DataLog(log_level_proc_debug) << "Command ac pump off" << endmsg;
   }
   else if ( !acStopped )
   {
      pd.Orders().Qac(20.0f);
   }

   if ( !returnStopped &&
        _ReturnVolume <= -0.9f)
   {
      pd.Orders().Qrp(0.0f);
      DataLog(log_level_proc_debug) << "Command return pump off" << endmsg;
   }
   else if ( !returnStopped )
   {
      pd.Orders().Qrp(-40.0f);
   }

   //
   //  Instead of checking for volumes to be exactly zero, we will
   //   allow a volume of 'deltaVolume' to be able to continue.
   //
   if ( inletStopped && acStopped && returnStopped &&
        ( fabs(pd.Status().InletPump.dV() )  <= deltaVolume ) &&
        ( fabs(pd.Status().ACPump.dV() )     <= deltaVolume ) &&
        ( fabs(pd.Status().ReturnPump.dV() ) <= deltaVolume ) )
   {
      //
      // Signify the test is starting ...
      //
      DataLog(log_level_proc_debug) << "Test started, pumps stopped " << endmsg;
      _TestStarted = 1;

      //
      // Start the pressure timer
      //
      _PressureTimer.interval(5000);   // 5 seconds ( 5000 msecs )
      DataLog(log_level_proc_debug) << "Start the pressure timer" << endmsg;

      //
      // Volume check worked, stop the pump timer
      //
      _PumpTimer.interval(0);
      DataLog(log_level_proc_debug) << "Stop the pump timer" << endmsg;
   }

   return NORMAL;
}

int InletDecayTest2::postProcess ()
{
   return NORMAL;
}

int InletDecayTest2::preEnter ()
{
   //
   // Set up the test ...
   //
   setUpTest();

   return NORMAL;
}

int InletDecayTest2::postExit ()
{
   //
   // Stop the pumps
   //
   stopPumps();

   //
   // Turn off the timers
   //
   _PressureTimer.interval(0);
   DataLog(log_level_proc_debug) << "Stop the pressure timer" << endmsg;
   _PumpTimer.interval(0);
   DataLog(log_level_proc_debug) << "Stop the pump timer" << endmsg;

   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void InletDecayTest2::setUpTest ()
{
   ProcData pd;

   //
   // Check to see that a valid pressure for the test is at the APS.
   //
   if ( pd.Status().APS() > -325.0f )
   {
      DataLog(log_level_proc_disposable_test_error) << "APS reading ( " << pd.Status().APS() << "mmHg) needed -325 or less" << endmsg;

      //
      // Set the alarm
      //
      _DecayAlarm.setAlarm();
      DataLog(log_level_proc_debug) << "Set the pressure decay alarm" << ACCESS_PRESSURE_DECAY_FAILURE << endmsg;

      //
      // Set the test failed flag
      //
      _TestFailed = 1;

      return;
   }

   //
   // Save initial values
   //
   saveValues();

   //
   // Command pumps
   //
   pd.Orders().Qin(20.0f);
   pd.Orders().Qac(20.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qrp(-40.0f);
   DataLog(log_level_proc_debug) << "Command pumps: " <<
      "inlet=20.0, ac=20.0, plasma=0.0, collect=0.0, return=-40.0" << endmsg;

   //
   // Start the pump timer
   //
   _PumpTimer.interval(5000);   // 5 seconds ( 5000 msecs )
   DataLog(log_level_proc_debug) << "Start the pump timer" << endmsg;

}

void InletDecayTest2::stopPumps ()
{
   ProcData pd;

   //
   // Turn the pumps off
   //
   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qrp(0.0f);
   DataLog(log_level_proc_debug) << "Command pumps: " <<
      "inlet=0.0, ac=0.0, plasma=0.0, collect=0.0, return=0.0" << endmsg;

}

void InletDecayTest2::saveValues ()
{
   ProcData pd;

   //
   // Save the initial APS reading
   //
   _InitialAPS = pd.Status().APS();
   DataLog(log_level_proc_info) << "Initial APS = " << _InitialAPS << "mmHg" << endmsg;

   //
   // Initialize volumes
   //
   _InletVolume  = 0.0;
   _ACVolume     = 0.0;
   _ReturnVolume = 0.0;
   DataLog(log_level_proc_debug) << "Initialize Volumes" << endmsg;

}

void InletDecayTest2::reset ()
{
   //
   // Reset the test started flag
   //
   _TestStarted = 0;

   //
   // Reset the test passed and failed flags
   //
   _TestPassed      = 0;
   _TestFailed      = 0;
   _TestPassedCount = 0;

}

void InletDecayTest2::copyOver (const InletDecayTest2&)
{}

void InletDecayTest2::cleanup ()
{}

/* FORMAT HASH 5a1527b09e75abb8b379fb7ad2d1d08e */

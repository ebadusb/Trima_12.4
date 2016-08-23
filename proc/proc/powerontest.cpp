/*                                    -
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      powerontest.cpp
 *
 */


#include "powerontest.h"
#include <math.h>
#include "run_defs.h"
#include "safproc.h"
#include "procdata.h"
#include "cobeconfig.h"

DEFINE_STATE(PowerOnTest);

const float FIRST_VOLUME_CHECK  = 0.1;        // Constants defined in IT 3529
const float SECOND_VOLUME_CHECK = 0.005;      //
const int   VOLTAGE_RISE_TIME   = 6000;       // msec allowed for voltage rise

PowerOnTest::PowerOnTest()
   : StateAbs(),
     _RequestSafetyTestMsg(),
     _PowerTestAlarm(POWER_TEST_FAILURE),
     _TestPassed(0),
     _FirstTestPassed(0),
     _SecondTestPassed(0),
     _PowerEnabled(0),
     _VoltageRiseTooSlowTimer(0, Callback<PowerOnTest>(this, &PowerOnTest::voltageRiseTooSlow), TimerMessage::DISARMED)
{}

PowerOnTest::PowerOnTest(const PowerOnTest& state)
   : StateAbs(state),
     _RequestSafetyTestMsg(),
     _PowerTestAlarm(POWER_TEST_FAILURE),
     _TestPassed(0),
     _FirstTestPassed(0),
     _SecondTestPassed(0),
     _PowerEnabled(0),
     _VoltageRiseTooSlowTimer(0, Callback<PowerOnTest>(this, &PowerOnTest::voltageRiseTooSlow), TimerMessage::DISARMED)
{}

PowerOnTest::~PowerOnTest()
{
   PowerOnTest::cleanup();
}

int PowerOnTest::transitionStatus ()
{
   ProcData pd;
   if ( _TestPassed )
   {
      DataLog(log_level_proc_startup_test_info) << "Power On Test passed" << endmsg;
      return 1;
   }
   return NO_TRANSITION;
}

int PowerOnTest::preProcess ()
{
   ProcData pd;
   DataLog(log_level_proc_debug) << "Current voltages - twenty four volt: " << pd.Status().PS24V.mV()
                                 << " twenty four sw volt: " << pd.Status().PS24Vsw.mV()
                                 << endmsg;

   //
   // Enable power if necessary
   //
   if ( !pd.AlarmActive() &&
        !_PowerEnabled )
   {
      //
      // Request power from safety
      //
      _RequestSafetyTestMsg.send(ENABLE_24V_POWER);
      DataLog(log_level_proc_startup_test_info) << "Request 24V Power On" << endmsg;

      _VoltageRiseTooSlowTimer.interval(VOLTAGE_RISE_TIME);

      _PowerEnabled = 1;
   }

   //
   // Check pumps ...
   //
   if ( _PowerEnabled &&
        voltageAndCurrentOK() )
   {
      //
      // We have 24V Power
      //
      // now check for pump volume accrual.
      //
      if ( pumpVolumesTest() == NORMAL )
      {
         if ( _FirstTestPassed && _SecondTestPassed )
         {
            // TEST PASSED
            _TestPassed = 1;
            _VoltageRiseTooSlowTimer.interval(0);
            DataLog(log_level_proc_startup_test_info) << "Volumes Test Passed" << endmsg;
         }
      }
      else
      {
         failTest("Power test fail: Volume accumulated", 0);
      }
   }

   return NORMAL;
}

int PowerOnTest::postProcess ()
{
   return NORMAL;
}

int PowerOnTest::preEnter ()
{
   //
   // Initialize passing flags for pumps ...
   //
   initTest();

   //
   // Create safety test message ...
   //
   _RequestSafetyTestMsg.init(MessageBase::SEND_GLOBAL);
   DataLog(log_level_proc_debug) << "Create Request Safety Power Message " << endmsg;

   return NORMAL;
}

int PowerOnTest::postExit ()
{
   // Ensure that the timer is turned off.
   _VoltageRiseTooSlowTimer.interval(0);

   //
   // Delete all messages ...
   //
   cleanup();

   return NORMAL;
}

void PowerOnTest::voltageRiseTooSlow (void)
{
   //
   // Check if voltages are acceptable
   //
   if ( !voltageAndCurrentOK() )
   {
      failTest("Power test fail: Voltages out of range", 0);
   }
}

int PowerOnTest::voltageAndCurrentOK (void)
{
   ProcData pd;
   return ( pd.Status().PS24V.Acceptable() &&
            pd.Status().PS24Vsw.Acceptable() ) ? 1 : 0;
}

void PowerOnTest::failTest (const char* msg, int userCode)
{
   ProcData pd;
   //
   // Log the voltages/current at the time of the failure
   //
   DataLog(log_level_proc_startup_test_info) << "24V=" << pd.Status().PS24V.mV()
                                             << " sw24V=" << pd.Status().PS24Vsw.mV()
                                             << endmsg;

   //
   // Make sure power is disabled in case we have a run-away pump
   //
   _RequestSafetyTestMsg.send(DISABLE_24V_POWER);
   DataLog(log_level_proc_startup_test_info) << "Request 24V Power Off" << endmsg;
   _PowerEnabled = 0;
   _VoltageRiseTooSlowTimer.interval(0);

   //
   //  Send alarm message indicating test failed.
   //
   _PowerTestAlarm.setAlarm();
   DataLog(log_level_proc_debug) << "Set Power Test Alarm " << POWER_TEST_FAILURE << endmsg;

   //
   // Restart the test
   //
   initTest();

   //
   // Log the failure
   //
   DataLog(log_level_proc_startup_test_info) << msg << " " <<  userCode << endmsg;
}

void PowerOnTest::initTest ()
{
   ProcData pd;
   //
   // Reset the passing flags ...
   //
   _FirstTestPassed = _SecondTestPassed = 0;

   //
   // Stop the pumps if they were moving.
   //
   pd.Orders().Qac(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qplasma(0.0f);
   DataLog(log_level_proc_debug) << "Stop all pumps" << endmsg;
}

int PowerOnTest::pumpVolumesTest ()
{
   if ( !_FirstTestPassed )
   {
      //
      // Check for volume greater than the FIRST_VOLUME_CHECK constant
      //
      if ( pumpVolumesLessThan(FIRST_VOLUME_CHECK) == NORMAL )
      {
         //
         // FIRST TEST PASSED ...
         //
         _FirstTestPassed = 1;
         DataLog(log_level_proc_startup_test_info) << "First Volume Check passed" << endmsg;
      }
      else
      {
         DataLog(log_level_proc_startup_test_info) << "First Volume Check failed" << endmsg;
         return PROCESSING_ERROR;
      }
   }
   else
   {
      //
      // Check for volume greater than the SECOND_VOLUME_CHECK constant.
      // Give a little bit more buffer in the second volume check
      //
      if ( pumpVolumesLessThan(SECOND_VOLUME_CHECK) == NORMAL )
      {
         //
         // SECOND TEST PASSED
         //
         _SecondTestPassed = 1;
         DataLog(log_level_proc_startup_test_info) << "Second Volume Check passed" << endmsg;
      }
      else
      {
         DataLog(log_level_proc_startup_test_info) << "Second Volume Check failed" << endmsg;
         return PROCESSING_ERROR;
      }
   }
   return NORMAL;
}


int PowerOnTest::pumpVolumesLessThan (const float limit)
{
   ProcData pd;
   //
   // Check pumps against allowed limit - note that the return pump can rotate
   // in either direction.
   //
   if ( pd.Status().ACPump.dV()      >= limit ||
        pd.Status().InletPump.dV()   >= limit ||
        pd.Status().CollectPump.dV() >= limit ||
        pd.Status().PlasmaPump.dV()  >= limit ||
        pd.Status().ReturnPump.dV()  >= limit ||
        pd.Status().ReturnPump.dV()  <= -limit )
   {
      //
      // Log the pump values so that the erroneous pump can be identified
      //
      DataLog(log_level_proc_startup_test_info) << "PowTest-AC : Delta volume: " << pd.Status().ACPump.dV() << endmsg;
      DataLog(log_level_proc_startup_test_info) << "PowTest-Inlet : Delta volume: " << pd.Status().InletPump.dV() << endmsg;
      DataLog(log_level_proc_startup_test_info) << "PowTest-Return : Delta volume: " << pd.Status().ReturnPump.dV() << endmsg;
      DataLog(log_level_proc_startup_test_info) << "PowTest-Collect : Delta volume: " << pd.Status().CollectPump.dV() << endmsg;
      DataLog(log_level_proc_startup_test_info) << "PowTest-Plasma : Delta volume: " << pd.Status().PlasmaPump.dV() << endmsg;

      return PROCESSING_ERROR;
   }
   return NORMAL;
}


void PowerOnTest::reset ()
{
   //
   // Reset the termination flag
   //
   _TestPassed = 0;
}

void PowerOnTest::cleanup ()
{
   //
   // Deregister all messages
   //
   _RequestSafetyTestMsg.deregisterMsg();
}

/* FORMAT HASH 6ba096490e66a28098cb050d47cc7c2d */

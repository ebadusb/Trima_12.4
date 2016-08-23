/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      centshutdowntest.cpp
 *
 */


#include "centshutdowntest.h"
#include "safproc.h"
#include "cobeconfig.h"
#include "procdata.h"

DEFINE_STATE(CentShutdownTest)

const float VOLTAGE_RISE_TIME = 6.0;      // seconds allowed for voltage rise before centrifuge shutdown test
const float VOLTAGE_STABLE_TIME = 0.5;    // delay after initial voltage rise for voltages to be stable

CentShutdownTest::CentShutdownTest()
   : StateAbs(),
     _TestStartTime(0),
     _VoltageOKTime(0),
     _VoltageCheckComplete(false),
     _ShutdownCheckComplete(false),
     _TestFailed(false),
     _TestStarted(false)
{}

CentShutdownTest::CentShutdownTest(const CentShutdownTest& state)
   : StateAbs(state),
     _TestStartTime(0),
     _VoltageOKTime(0),
     _VoltageCheckComplete(false),
     _ShutdownCheckComplete(false),
     _TestFailed(false),
     _TestStarted(false)
{}

CentShutdownTest::~CentShutdownTest()
{
   CentShutdownTest::cleanup();
}

int CentShutdownTest::transitionStatus ()
{
   return _TransitionStatus;
}

int CentShutdownTest::preProcess ()
{
   if ( !_TestStarted )
   {
      ProcData pd;
      if (    !pd.Status().DoorIsOpen()
              && !pd.AlarmActive()
              && pd.Status().CentrifugeRPM() < CobeConfig::data().CentrifugeOpenSpeed )
      {
         pd.Orders().LockDoor();
         if ( pd.Status().DoorIsLocked() )
            startTest();
      }
      else
      {
         //
         // Enable the door alarms to alert the operator to close the door
         //  so we can perform the test ...
         //
         pd.Orders().RPM(1.0f);
      }
   }

   return NORMAL;
}

int CentShutdownTest::postProcess ()
{
   ProcData pd;

   if (    _TestStarted
           && !pd.AlarmActive()
           && pd.Status().CentrifugeRPM() < CobeConfig::data().CentrifugeOpenSpeed )
   {
      float totalTestTime    = pd.AbsTime().getSecs() - _TestStartTime;
      float shutdownTestTime = (_VoltageCheckComplete) ? (pd.AbsTime().getSecs() - _VoltageOKTime) : 0.0;

      if ( pd.Status().CentrifugeRPM() > 0.1f )
      {
         DataLog(log_level_proc_startup_test_info) << "Detected centrifuge motion of " << pd.Status().CentrifugeRPM() << endmsg;
         failTest("Centrifuge motion detected during shutdown test");
         return NORMAL;
      }

      if ( _VoltageCheckComplete &&
           !_ShutdownCheckComplete &&
           pd.Status().CentI() > CobeConfig::data().MaxCentStopCurrent )
      {
         failTest("Excessive centrifuge motor current during shutdown test");
         return NORMAL;
      }

      if ( _VoltageCheckComplete &&
           !_ShutdownCheckComplete &&
           shutdownTestTime > CobeConfig::data().MaxCentErrorTime )
      {
         //
         // Centrifuge error bit should be off by now.
         // IT 10898.  Ignore reverse bit for this check
         //
         if ( pd.Status().CentrifugeErrorNoReverse() )
         {
            failTest("Centrifuge error detected during shutdown test");
            return NORMAL;
         }
      }

      if ( _VoltageCheckComplete &&
           !_ShutdownCheckComplete &&
           shutdownTestTime > VOLTAGE_STABLE_TIME )
      {
         //
         // Voltage should be stable at this point, any dips
         // might indicate excessive current draw by the centrifuge
         // motor controller
         //
         if ( !pd.Status().PS64V.Acceptable() ||
              !pd.Status().PS64Vsw.Acceptable() )
         {
            failTest("Voltages out of range after initial rise for centrifuge shutdown test");
            return NORMAL;
         }
      }

      if ( !_VoltageCheckComplete )
      {
         if ( pd.Status().PS64V.Acceptable() &&
              pd.Status().PS64Vsw.Acceptable() &&
              pd.Status().CentI() <= CobeConfig::data().MaxCentStopCurrent )
         {
            _VoltageCheckComplete = true;
            _VoltageOKTime        = pd.AbsTime().getSecs();

            DataLog(log_level_proc_startup_test_info) << "Voltages/current OK for centrifuge shutdown test " << (int)totalTestTime * 1000 << " ms" << endmsg;
            logVoltages();

            pd.Orders().RPM(0.0);
            DataLog(log_level_proc_debug) << "Cleared centrifuge inhibit bit" << endmsg;
         }
         else if ( totalTestTime > VOLTAGE_RISE_TIME )
         {
            failTest("Voltages/current out of range during centrifuge shutdown test");
            return NORMAL;
         }
      }

      if ( _VoltageCheckComplete &&
           shutdownTestTime >= CobeConfig::data().CentShutdownTestDuration )
      {
         _ShutdownCheckComplete = true;
      }

      //
      // If all checks are OK, then pass test
      //
      if ( _VoltageCheckComplete &&
           _ShutdownCheckComplete )
      {
         float currentTestTime = pd.AbsTime().getSecs() - _TestStartTime;
         DataLog(log_level_proc_startup_test_info) << "Centrifuge shutdown (zero speed) test passed. Test time: "
                                                   << (int)currentTestTime * 1000 << " ms" << endmsg;
         _TransitionStatus =  1;
      }
   }
   else
   {
      _TestStarted = false;
   }

   return NORMAL;
}

int CentShutdownTest::preEnter ()
{
   return NORMAL;
}

int CentShutdownTest::postExit ()
{
   //
   // Disable 64V power at end of test
   //
   RequestSafetyPowerMsg safetyTestMsg(MessageBase::SEND_GLOBAL);
   safetyTestMsg.send(DISABLE_64V_POWER);
   DataLog(log_level_proc_startup_test_info) << "Request 64V Power Off " << DISABLE_64V_POWER << endmsg;

   //
   // If the test completed successfully ...
   //
   if ( _TransitionStatus == 1 )
   {
      //
      // Allow the door to be unlocked ...
      //
      ProcData pd;
      pd.Orders().RPM(0.0);
   }
   else
   {
      //
      // Reset the test started flag and status for
      //  another try ...
      //
      _TransitionStatus = NO_TRANSITION;
      _TestStarted      = false;
   }

   //
   // Delete the safety message ...
   //
   cleanup();

   return NORMAL;
}

void CentShutdownTest::cleanup ()
{}

void CentShutdownTest::startTest ()
{
   ProcData pd;
   //
   // Mark the time at the start of the test
   //
   _TestStartTime         = pd.AbsTime().getSecs();
   _VoltageCheckComplete  = false;
   _ShutdownCheckComplete = false;
   _TestFailed            = false;

   //
   // Set the centrifuge inhibit bit until voltages are stable
   //
   pd.Orders().RPM(-1);
   DataLog(log_level_proc_startup_test_info) << "Start centrifuge shutdown (zero speed) test. Set centrifuge inhibit bit" << endmsg;

   //
   // Request that safety turn on the 64V for cent test.
   //
   RequestSafetyPowerMsg safetyTestMsg(MessageBase::SEND_GLOBAL);
   safetyTestMsg.send(ENABLE_64V_POWER);
   DataLog(log_level_proc_startup_test_info) << "Request 64V Power On " << ENABLE_64V_POWER << endmsg;

   _TestStarted = true;
}

void CentShutdownTest::failTest (const char* msg)
{
   ProcData pd;
   //
   // Disable 64V power and set centrifuge inhibit bit
   //
   RequestSafetyPowerMsg safetyTestMsg(MessageBase::SEND_GLOBAL);
   safetyTestMsg.send(DISABLE_64V_POWER);
   DataLog(log_level_proc_startup_test_info) << "Request 64V Power Off " << DISABLE_64V_POWER << endmsg;
   pd.Orders().RPM(-1);

   //
   // Set alarm
   //
   PendingCassetteAlarmMsg delayedAlarm(MessageBase::SEND_LOCAL);
   delayedAlarm.send(CENTRIFUGE_SHUTDOWN_FAILURE);

   //
   // Log cause of alarm
   //
   float totalTestTime = pd.AbsTime().getSecs() - _TestStartTime;
   DataLog(log_level_proc_startup_test_error) << msg << " for " << (int)(1000 * totalTestTime) << "ms" << endmsg;
   logVoltages();

   //
   // Mark test as failed
   //
   _TestFailed = true;
}

void CentShutdownTest::logVoltages (void)
{
   ProcData pd;
   DataLog(log_level_proc_startup_test_info) << dec << "64V=" << pd.Status().PS64V.mV() << " mV sw64V=" << pd.Status().PS64Vsw.mV() << "mV centI=" << pd.Status().CentI() << " A" << endmsg;
}

/* FORMAT HASH 6528c49f82b8aa04052d067590194b2e */

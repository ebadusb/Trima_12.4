/*******************************************************************************
 *
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.28  2007/04/12 20:22:27Z  spriusb
 * 24V power control now calls new request method for safety donor disconnect test
 * Revision 1.27  2005/07/05 16:22:50Z  rm70006
 * IT 7025.  Fix logging statement.
 * Revision 1.26  2003/11/14 20:14:24Z  ms10234
 * 5316 - rework 64v power tests and fix logging level
 * Revision 1.25  2003/07/21 22:12:37Z  td07711
 * IT6157 - no need to alarm when going fatal
 * Revision 1.24  2003/07/01 22:35:00Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.23  2003/04/04 16:04:48Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.22  2003/01/08 17:13:13Z  pn02526
 * Runnable safety executive with CRC checking of the code disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.
 * Revision 1.21  2002/10/11 09:41:29  pn02526
 * Massive checkin of fixed modules for integration checking
 * Revision 1.20  2002/08/08 11:51:25  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * Add SafetyPowerControlTimer ::_myTimer to use the new timer message class definition, rather than deriving from a timer message class.
 * Use the new buffered message base class definition to send the SafetyTestDoneMsg, rather than deriving class aSafetyTestDoneMsg_sndr from an INT32 message base class.
 * Add aRequestSafetyPowerMsg_rcvr :: _theRequestSafetyPowerMsg to use the new buffered message class definition, rather than deriving from an INT32 message base class.
 * Revision 1.19  2002/07/23 16:52:27  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.18  2002/04/23 15:09:34  pn02526
 * Change type of T1Status to clear up compile warnings.
 * Revision 1.17  2002/04/11 17:17:18  pn02526
 * Revise logging to use DataLogLevel class.  Revise FATAL_ERROR calls to use the new one.
 * Revision 1.16  2001/08/01 15:25:57Z  jl11312
 * - corrected problem which prevent retry of door latch power off test
 * Revision 1.15  2000/08/17 16:34:20  pn02526
 * Added logic to check whether Donor Disconnect Test is running, and whether it expects a Power Request.  If not expected, a Software Fault alarm is issued.
 * Revision 1.14  2000/07/26 22:07:09  bs04481
 * Relocate code to control solenoid power from the power contol
 * timer to the door object because the door object normally
 * controls the solenoid power based on centrifuge speed.  Also,
 * delay 1 second before sending a response for solenoid power
 * disable.
 * Revision 1.13  2000/07/10 16:55:54  bs04481
 * Remove changes in previous revision.  See it 3524
 * Revision 1.12  2000/07/10 15:21:55  bs04481
 * Alarm if safety receives a request for 64v power with the door
 * during blood prime
 * Revision 1.11  2000/07/07 23:19:03  bs04481
 * Lost the kick-off for the disconnect test when I put in the
 * power request responses.
 * Revision 1.10  2000/07/06 01:20:36  bs04481
 * Add handling for solenoid power to RequestSafetyPower message.
 * Respond to all RequestSafetyPower messages, not just test
 * requests.
 * Revision 1.9  2000/06/08 21:54:00  bs04481
 * Respond to power requests along with test requests
 * Revision 1.8  2000/05/26 17:12:18  BS04481
 * More maintanence changes:  This set contains changes for
 * 1. Moving valve object from container to bag
 * 2. Removing SafetyTimer->SafetySoftTimer and letting the
 *      switch and disconnect tests have their own timers.
 * 3.  All message types are changed to NO_BOUNCE to cut down
 *      the on unnecessary stuff in the message que.
 * Revision 1.7  2000/03/15 18:23:09  BS04481
 * Move power test and power test timer objects from this file
 * to pwr_test
 * Revision 1.6  1999/08/01 20:53:18  BS04481
 * Non-critical code review changes identified at the 3.2 release.
 * Revision 1.5  1998/11/05 19:21:08  bs04481
 * Remove unnecessary reset of the A/D conversion multiplier.
 * Code review change.
 * Revision 1.4  1998/10/16 16:41:21  bs04481
 * After power test fails, try to disable power.
 * Revision 1.3  1998/10/09 05:55:47  bs04481
 * Block 64v power enable with door open for test and enable both
 * case in addition to enable 64v case.
 * Revision 1.2  1998/09/29 19:54:39  bs04481
 * Change to respond with a "test complete" message if power
 * has already been tested instead of enabling the power.
 * Revision 1.1  1998/09/28 21:50:40  bs04481
 * Initial revision
 *
 *
 */

#include "trima_assert.h"
#include "trima_datalog.h"
#include "s_power.hpp"
#include "s_exec.hpp"


/*******************************************************************************
*
*  SafetyPowerControlTimer CLASS METHOD DEFINITION
*
******************************************************************************/
// SPECIFICATION:    Safety Power Test Timer constructor
//
//
// ERROR HANDLING:   none.

SafetyPowerControlTimer::SafetyPowerControlTimer(aDoor_Safety* paDoor
                                                 , aCentrifuge_Safety* paCentrifuge
                                                 , aSafety24vPowerTest* paSafety24vPowerTest
                                                 , aSafety64vPowerTest* paSafety64vPowerTest
                                                 , aDonorDisconnectTest* paDonorDCTest)
   : _myTimer(500ul, Callback<SafetyPowerControlTimer>(this, &SafetyPowerControlTimer::notify), TimerMessage::ARMED)
{
   trima_assert(paDoor);
   trima_assert(paCentrifuge);
   trima_assert(paSafety24vPowerTest);
   trima_assert(paSafety64vPowerTest);
   trima_assert(paDonorDCTest);
   _paDoor                   = paDoor;
   _paCentrifuge             = paCentrifuge;
   _p24vPowerTest            = paSafety24vPowerTest;
   _p64vPowerTest            = paSafety64vPowerTest;
   _paDonorDCTest            = paDonorDCTest;
   _bPowerTestInProgress     = FALSE;
   _bTimeSolenoid            = FALSE;
   _bSolenoidResponseSent    = FALSE;
   _solenoidTimeout.tv_sec   = 0;
   _solenoidTimeout.tv_nsec  = 0;
   _lastTime.tv_sec          = 0;
   _lastTime.tv_nsec         = 0;
   DataLog(log_level_safe_exec_debug) << "SafetyPowerControlTimer::SafetyPowerControlTimer() constructing SafetyTestDoneMsg" << endmsg;
   _paSafetyTestDoneMsg_sndr = new SafetyTestDoneMsg(MessageBase::SEND_GLOBAL);
   DataLog(log_level_safe_exec_debug) << "SafetyPowerControlTimer::SafetyPowerControlTimer() SafetyTestDoneMsg constructed" << endmsg;
}

// SPECIFICATION:    Safety Power Test Timer destructor
//
//
// ERROR HANDLING:   none.

SafetyPowerControlTimer::~SafetyPowerControlTimer()
{
   delete _paSafetyTestDoneMsg_sndr;
   _paSafetyTestDoneMsg_sndr = NULL;
   _paDoor                   = NULL;
   _paCentrifuge             = NULL;
   _paDonorDCTest            = NULL;
   _p24vPowerTest            = NULL;
   _p64vPowerTest            = NULL;
}

// SPECIFICATION:    Safety Power Control Timer method
//                   Starts the 24v test if one is required.
//                   If a test is not enabled and a response is
//                   requested, responds with status.
//                   Otherwise returns response to called.
//
//
// ERROR HANDLING:   none.

long SafetyPowerControlTimer::PowerControl24v (HW_ORDERS order, bool doResponse, bool forceTest)
{
   long     response = PWR_CONTROL_FAILED;
   char     cDonorDisconnectTest;
   T1STATUS T1Status;
   T1Status = _p24vPowerTest->GetT1Status();
   DataLog(log_level_safe_exec_info) << "PowerControl24v: T1Status = " << (int)T1Status << endmsg;

   // if we have power blocked, ignore all requests
   if (paSafetyHardwareCommandsExec->isPowerBlocked() == TRUE)
   {
      DataLog(log_level_safe_exec_info) << "Ignore 24v request with blocked power." << endmsg;
      response = PWR_CONTROL_24V_IGNORED;
   }
   else if (order == HW_ENABLE)
   {
      DataLog(log_level_safe_exec_info) << "PowerControl24v: HW_ENABLE ordered." << endmsg;
      // if they just want it tested, test the 24v
      // or if we haven't tested it successfully before, test the 24v
      // or if we are running the disconnect test, we always test the 24v
      cDonorDisconnectTest = _paDonorDCTest->GetDonorDisconnectTest();

      if ( (forceTest == TRUE)
           ||(T1Status == PWR_CONTROL_NOT_TESTED)
           ||(T1Status == PWR_CONTROL_FAILED)
           ||(cDonorDisconnectTest == DISCONNECT_TEST_IN_PROGRESS)
           ||(cDonorDisconnectTest == DISCONNECT_TEST_FAILED_HOLDING_LOW) )
      {
         // If Donor Disconnect Test is running...
         if ( (cDonorDisconnectTest == DISCONNECT_TEST_IN_PROGRESS)
              ||(cDonorDisconnectTest == DISCONNECT_TEST_FAILED_HOLDING_LOW) )
         {
            DataLog(log_level_safety_donor_disconnect_info) << "PowerControl24v: Donor Disconnect Test in progress." << endmsg;
            // Make sure Donor Disconnect Test is expecting a T1 power test.
            if (_paDonorDCTest->GetDonorDisconnectTestState() == WAITING_FOR_POWER_REQUEST )
            {
               // OK, start it...
               DataLog(log_level_safety_donor_disconnect_info) << "PowerControl24v: Donor Disconnect Test is waiting for Power Request." << endmsg;
               _p24vPowerTest->SetPowerTest(RUNNING_24V_PWR_TEST_ENAB);
               _bPowerTestInProgress = TRUE;
               response              = RUNNING_24V_PWR_TEST_ENAB;
               //             ... and kick Donor Disconnect Test.
               _paDonorDCTest->request();
            }
            else
            {
               // Software Fault Alarm: Donor Disconnect Test running and is NOT expecting a T1 power test.
               DataLog(log_level_safety_alarm_detail) << "SW_FAULT: extraneous Power Request during Donor Disconnect Test." << endmsg;
               response = PWR_CONTROL_24V_IGNORED;
               paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SW_FAULT);
            }
         }
         else  // Donor Disconnect test not running.
         {
            DataLog(log_level_safety_donor_disconnect_info) << "PowerControl24v: Donor Disconnect Test NOT in progress." << endmsg;
            _p24vPowerTest->SetPowerTest(RUNNING_24V_PWR_TEST_ENAB);
            _bPowerTestInProgress = TRUE;
            response              = RUNNING_24V_PWR_TEST_ENAB;
         }
      }
      else if (T1Status != RUNNING_24V_PWR_TEST_ENAB)
      {
         DataLog(log_level_safe_exec_info) << "PowerControl24v: Executing HW_ENABLE." << endmsg;
         // if it is not in the middle of being tested, just go ahead
         // and enable it and respond
         paSafetyHardwareCommandsExec->SetPumpPower(HW_ENABLE);
         response = PWR_CONTROL_24V_NOMINAL;
      }
      else
      {
         // ignore the request if we are already running the test
         DataLog(log_level_safe_exec_info) << "Ignore redundant 24v request." << endmsg;
         response = PWR_CONTROL_24V_IGNORED;
      }
   }
   else if (order == HW_DISABLE)
   {
      DataLog(log_level_safe_exec_info) << "PowerControl24v: HW_DISABLE ordered & being executed." << endmsg;
      // if they want it off, turn it off and respond
      paSafetyHardwareCommandsExec->SetPumpPower(HW_DISABLE);
      response = PWR_CONTROL_24V_NOMINAL;
   }
   else
   {
      // ignore the request if it is not enable or disable
      response = PWR_CONTROL_24V_IGNORED;
      DataLog(log_level_safe_exec_info) << "Order: " << (int)order << "  Ignore unknown 24v request." << endmsg;
   }

   // if we're waiting for this test to complete, don't respond.
   // if we're waiting to consolidate this result with another request, don't respond
   // but return the response.  otherwise, respond and return the response.
   if (response == RUNNING_24V_PWR_TEST_ENAB)
      return(0);
   else
   {
      if (doResponse == TRUE)
      {
         DataLog(log_level_safe_exec_info) << "SafetyPowerControlTimer::PowerControl24v(" << (int)order << " " << doResponse << " " << forceTest << ") sending SafetyTestDoneMsg(" << response << ")" << endmsg;
         _paSafetyTestDoneMsg_sndr->send(response);
      }
      return(response);
   }


}

// SPECIFICATION:    Safety Power Control Timer method
//                   Starts the 64v test if one is required.
//                   If a test is not enabled and a response is
//                   requested, responds with status.
//                   Otherwise returns response to called.
//
//
// ERROR HANDLING:   none.

long SafetyPowerControlTimer::PowerControl64v (HW_ORDERS order, bool doResponse, bool forceTest)
{
   long           response = PWR_CONTROL_FAILED;
   T1STATUS       T1Status;
   HW_DOOR_STATES locks;

   T1Status = _p64vPowerTest->GetT1Status();
   locks    = _paDoor->GetDoor();

   // if we have power blocked, ignore all requests
   if (paSafetyHardwareCommandsExec->isPowerBlocked() == TRUE)
   {
      DataLog(log_level_safe_exec_info) << "Ignore 64v request with blocked power." << endmsg;
      response = PWR_CONTROL_64V_IGNORED;
   }
   else if (order == HW_ENABLE)
   {
      if (   (locks == HW_DOOR_OPEN)
             ||(locks == HW_DOOR_LOCKED_STATE) )
      {
         // if the door is open, make sure the
         // power is off and ignore the request if it was to enable
         paSafetyHardwareCommandsExec->SetCentrifugePower(HW_DISABLE);
         DataLog(log_level_safe_exec_info) << "System State: "
                                           << SafetyExec::instance()->SystemState()
                                           << "  Ignore 64v request with door open."
                                           << endmsg;
         response = PWR_CONTROL_64V_IGNORED;
      }
      else if ( (forceTest == TRUE)
                ||(T1Status == PWR_CONTROL_NOT_TESTED)
                ||(T1Status == PWR_CONTROL_FAILED) )
      {
         // if they just want it tested, test the 64v
         // or if we haven't tested it successfully before, test the 64v
         _p64vPowerTest->SetPowerTest(RUNNING_64V_PWR_TEST_ENAB);
         _bPowerTestInProgress = TRUE;
         response              = RUNNING_64V_PWR_TEST_ENAB;
      }
      else if (T1Status != RUNNING_64V_PWR_TEST_ENAB)
      {
         // if it is not in the middle of being tested, just go ahead
         // and enable it and respond
         paSafetyHardwareCommandsExec->SetCentrifugePower(HW_ENABLE);
         response = PWR_CONTROL_64V_NOMINAL;
      }
      else
      {
         // ignore the request if we are already running the test
         DataLog(log_level_safe_exec_info) << "Ignore redundant 64v request." << endmsg;
         response = PWR_CONTROL_64V_IGNORED;
      }
   }
   else if (order == HW_DISABLE)
   {
      // if they want it off, turn it off
      paSafetyHardwareCommandsExec->SetCentrifugePower(HW_DISABLE);
      response = PWR_CONTROL_64V_NOMINAL;
   }
   else
   {
      // ignore the request if it is not enable or disable
      response = PWR_CONTROL_64V_IGNORED;
      DataLog(log_level_safe_exec_info) << "Order: " << (int)order << "  Ignore unknown 64v request." << endmsg;
   }

   // if we're waiting for this test to complete, don't respond.
   // if we're waiting to consolidate this result with another request, don't respond
   // but return the response.  otherwise, respond and return the response.
   if (response == RUNNING_64V_PWR_TEST_ENAB)
      return(0);
   else
   {
      if (doResponse == TRUE)
      {
         DataLog(log_level_safe_exec_info) << "SafetyPowerControlTimer::PowerControl64v(" << (int)order << " " << doResponse << " " << forceTest << ") sending SafetyTestDoneMsg(" << response << ")" << endmsg;
         _paSafetyTestDoneMsg_sndr->send(response);
      }
      return(response);
   }

}

// SPECIFICATION:    Safety Power Control Timer method
//                   Controls solenoid for testing.
//                   Always responds.
//
//
// ERROR HANDLING:   none.

void SafetyPowerControlTimer::PowerControlDoor (HW_ORDERS order)
{
   long            response = PWR_CONTROL_FAILED;
   long            dTest, dSample;
   struct timespec now;

   // if we are timing a disable but have received a new order
   // to enable, discontinue timing and allow solenoid control
   // based on centrifuge speed
   if ( (_bTimeSolenoid == TRUE)
        &&(order == HW_ENABLE) )
   {
      _bTimeSolenoid          = FALSE;
      _paDoor->SetForceDisable(FALSE);
      _solenoidTimeout.tv_sec = 0;
   }

   // if we have disabled the solenoid power on command, we need
   // to time-out the command and let it be controlled on the
   // centrifuge speed.
   // respond after a bit under the assumption that the power
   // should be drained.
   // control based on centrifuge speed after 15 seconds
   if (_bTimeSolenoid == TRUE)
   {
      clock_gettime(CLOCK_REALTIME, &now);
      dTest            = now.tv_sec - _solenoidTimeout.tv_sec;
      dSample          = now.tv_sec - _lastTime.tv_sec;
      _lastTime.tv_sec = now.tv_sec;
      if ( (dTest < 0)
           ||( dSample > 2 ) )
      {
         // somebody reset the clock so reset the time
         clock_gettime(CLOCK_REALTIME, &_solenoidTimeout);
         _lastTime.tv_sec = _solenoidTimeout.tv_sec;
      }
      else if ( (dTest > RESPOND_SOLENOID_OFF_TIME)
                &&(_bSolenoidResponseSent == FALSE) )
      {
         // make sure the door did the disable
         if (_paDoor->IsSolenoidDisabled() == TRUE)
         {
            // send a response
            DataLog(log_level_safe_exec_info) << "SafetyPowerControlTimer::PowerControlDoor(" << (int)order << ") sending SafetyTestDoneMsg(PWR_CONTROL_SOLENOID_NOMINAL)" << endmsg;
            _paSafetyTestDoneMsg_sndr->send(PWR_CONTROL_SOLENOID_NOMINAL);
            _bSolenoidResponseSent = TRUE;
         }
      }
      else if (dTest > MAX_SOLENOID_OFF_TIME)
      {
         // if it has been greater than 20 sec, stop timing
         // and allow solenoid control based on centrifuge speed
         _bTimeSolenoid = FALSE;
         _paDoor->SetForceDisable(FALSE);
      }

   }
   else
   {
      // if we haven't completed the 24v power test, ignore the request
      if (_p24vPowerTest->GetT1Status() != PWR_CONTROL_24V_NOMINAL)
      {
         DataLog(log_level_safe_exec_info) << "Ignore solenoid request with untested 24v." << endmsg;
         response = PWR_CONTROL_SOLENOID_IGNORED;
      }
      else if (_paCentrifuge->WhatIsCurrentRPM() >= 60.0)
      {
         // if the centrifuge is spinning, ignore the request
         DataLog(log_level_safe_exec_info) << "Ignore solenoid request with spinning centrifuge." << endmsg;
         response = PWR_CONTROL_SOLENOID_IGNORED;
      }
      else
      {
         if (order == HW_DISABLE)
         {
            // if they want it off, tell the door object to turn it off
            _paDoor->SetForceDisable(TRUE);
            // this is the abnormal case for a machine with a stopped
            // centrifuge so time-out this case.
            clock_gettime(CLOCK_REALTIME, &_solenoidTimeout);
            _lastTime.tv_sec       = _solenoidTimeout.tv_sec;
            _bTimeSolenoid         = TRUE;
            _bSolenoidResponseSent = FALSE;
         }
         else if (order == HW_ENABLE)
         {
            // 'normal' case is enabled if the centrifuge is not spinning
            // so we won't bother to time this
            response       = PWR_CONTROL_SOLENOID_NOMINAL;
            _bTimeSolenoid = FALSE;
         }
         else
         {
            // ignore the request if it is not enable or disable
            response       = PWR_CONTROL_SOLENOID_IGNORED;
            DataLog(log_level_safe_exec_info) << "Order: " << (int)order << "Ignore unknown solenoid request." << endmsg;
            _bTimeSolenoid = FALSE;
         }
      }

      // send a response unless you are waiting for the solenoid to drain
      if (_bTimeSolenoid == FALSE)
      {
         DataLog(log_level_safe_exec_info) << "SafetyPowerControlTimer::PowerControlDoor(" << (int)order << ") sending SafetyTestDoneMsg(" << response << ")" << endmsg;
         _paSafetyTestDoneMsg_sndr->send(response);
      }

   }

}


// SPECIFICATION:    Safety Power Control Timer method
//                   Handles combined requests.
//                   Responds if a response is available.
//
//
// ERROR HANDLING:   none.

void SafetyPowerControlTimer::PowerControlBoth (HW_ORDERS order, bool forceTest)
{
   long response = 0;
   response |= PowerControl24v(order, FALSE, forceTest);
   response |= PowerControl64v(order, FALSE, forceTest);

   // if either responded, send it
   if (response != 0)
   {
      DataLog(log_level_safe_exec_info) << "SafetyPowerControlTimer::PowerControlBoth(" << order << " " << forceTest << ") sending SafetyTestDoneMsg(" << response << ")" << endmsg;
      _paSafetyTestDoneMsg_sndr->send(response);
   }

}
// SPECIFICATION:    execute power tests for T1
//                   responds with test results or alarms
//
//
// ERROR HANDLING:   PWR_SWITCH_BOTH_FAILED,
//                   PWR_SWITCH_24V_FAILED
//                   PWR_SWITCH_64V_FAILED

void SafetyPowerControlTimer::notify ()
{
   static unsigned long notify_count = 0;
   char                 status24v, status64v;

   DataLog(log_level_safe_exec_debug) << "SafetyPowerControlTimer::notify: count=" << ++notify_count << endmsg;

   // get the current status from the power test objects
   status24v = _p24vPowerTest->GetT1Status();
   status64v = _p64vPowerTest->GetT1Status();

   // if we are in test, update
   if (status24v == RUNNING_24V_PWR_TEST_ENAB)
      _p24vPowerTest->Update();
   if (status64v == RUNNING_64V_PWR_TEST_ENAB)
      _p64vPowerTest->Update();

   // if we are not testing anymore, clear the flag
   if ( (status24v != RUNNING_24V_PWR_TEST_ENAB)
        &&(status64v != RUNNING_64V_PWR_TEST_ENAB)
        &&(_bPowerTestInProgress == TRUE) )
   {
      _bPowerTestInProgress = FALSE;

      // if the tests failed, take appropriate action
      if ( ( status24v == PWR_CONTROL_FAILED)
           &&( status64v == PWR_CONTROL_FAILED) )
      {
         DataLog(log_level_safe_exec_info) << "SafetyPowerControlTimer :: notify() sending SafetyTestDoneMsg(PWR_CONTROL_BOTH_FAILED)" << endmsg;
         _paSafetyTestDoneMsg_sndr->send( (long)PWR_CONTROL_BOTH_FAILED);
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, (ALARM_VALUES)PWR_SWITCH_BOTH_FAILED);
      }
      else if ( status24v == PWR_CONTROL_FAILED )
      {
         DataLog(log_level_safe_exec_info) << "SafetyPowerControlTimer :: notify() sending SafetyTestDoneMsg(PWR_CONTROL_24V_FAILED)" << endmsg;
         _paSafetyTestDoneMsg_sndr->send( (long)PWR_CONTROL_24V_FAILED);
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, (ALARM_VALUES)PWR_SWITCH_24V_FAILED);
      }
      else if ( status64v == PWR_CONTROL_FAILED )
      {
         DataLog(log_level_safe_exec_info) << "SafetyPowerControlTimer :: notify() sending SafetyTestDoneMsg(PWR_CONTROL_64V_FAILED)" << endmsg;
         _paSafetyTestDoneMsg_sndr->send( (long)PWR_CONTROL_64V_FAILED);
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, (ALARM_VALUES)PWR_SWITCH_64V_FAILED);
      }

      // if the tests passed, just tell Procedure
      if ( ( status24v == PWR_CONTROL_NOMINAL)
           &&( status64v == PWR_CONTROL_NOMINAL) )
      {
         DataLog(log_level_safe_exec_info) << "SafetyPowerControlTimer :: notify() sending SafetyTestDoneMsg(PWR_CONTROL_BOTH_NOMINAL)" << endmsg;
         _paSafetyTestDoneMsg_sndr->send( (long)PWR_CONTROL_BOTH_NOMINAL);
      }
      else if ( status24v == PWR_CONTROL_NOMINAL )
      {
         DataLog(log_level_safe_exec_info) << "SafetyPowerControlTimer :: notify() sending SafetyTestDoneMsg(PWR_CONTROL_24V_NOMINAL)" << endmsg;
         _paSafetyTestDoneMsg_sndr->send( (long)PWR_CONTROL_24V_NOMINAL);
      }
      else if ( status64v == PWR_CONTROL_NOMINAL )
      {
         DataLog(log_level_safe_exec_info) << "SafetyPowerControlTimer :: notify() sending SafetyTestDoneMsg(PWR_CONTROL_64V_NOMINAL)" << endmsg;
         _paSafetyTestDoneMsg_sndr->send( (long)PWR_CONTROL_64V_NOMINAL);
      }

   }

   // if we are timing out a solenoid power disable command, check the timer
   if (_bTimeSolenoid == TRUE)
      PowerControlDoor(HW_DISABLE);
}

/*******************************************************************************
*
*  aRequestSafetyPowerMsg_rcvr CLASS METHOD DEFINITION
*
******************************************************************************/
// SPECIFICATION:    Safety Power Request Message Receiver constructor
//
//
// ERROR HANDLING:   none.

aRequestSafetyPowerMsg_rcvr::aRequestSafetyPowerMsg_rcvr(aDoor_Safety* paDoor
                                                         , aCentrifuge_Safety* paCentrifuge
                                                         , aSafety24vPowerTest* paSafety24vPowerTest
                                                         , aSafety64vPowerTest* paSafety64vPowerTest
                                                         , aDonorDisconnectTest* paDonorDCTest)
   : _theRequestSafetyPowerMsg(Callback<aRequestSafetyPowerMsg_rcvr>(this, &aRequestSafetyPowerMsg_rcvr::notify),
                               MessageBase::SNDRCV_RECEIVE_ONLY)
{
   trima_assert(paDoor);
   trima_assert(paCentrifuge);
   trima_assert(paSafety24vPowerTest);
   trima_assert(paSafety64vPowerTest);
   trima_assert(paDonorDCTest);

   _pa24vPowerTest          = paSafety24vPowerTest;
   _pa64vPowerTest          = paSafety64vPowerTest;
   DataLog(log_level_safe_exec_debug) << "aRequestSafetyPowerMsg_Rcvr::aRequestSafetyPowerMsg_rcvr() constructing SafetyPowerControlTimer" << endmsg;
   _SafetyPowerControlTimer = new SafetyPowerControlTimer(paDoor
                                                          , paCentrifuge
                                                          , _pa24vPowerTest
                                                          , _pa64vPowerTest
                                                          , paDonorDCTest);
   DataLog(log_level_safe_exec_debug) << "aRequestSafetyPowerMsg_Rcvr::aRequestSafetyPowerMsg_rcvr() SafetyPowerControlTimer constructed" << endmsg;
}

// SPECIFICATION:    Safety Power Request Message Receiver destructor
//
//
// ERROR HANDLING:   none.

aRequestSafetyPowerMsg_rcvr::~aRequestSafetyPowerMsg_rcvr()
{
   delete _SafetyPowerControlTimer;
   _SafetyPowerControlTimer = NULL;
   _pa24vPowerTest          = NULL;
   _pa64vPowerTest          = NULL;

}


// SPECIFICATION:    Safety Power Request Message Receiver notify
//                   Starts the power test, if required, and
//                   controls power switched
//
//
// ERROR HANDLING:   FATAL ERROR.

void aRequestSafetyPowerMsg_rcvr::notify ()
{
   long lPowerRequest;

   lPowerRequest = _theRequestSafetyPowerMsg.getData();
   DataLog(log_level_safe_exec_info) << "Power Request Received: " << (int)lPowerRequest << "=";
   switch ( lPowerRequest )
   {
      case ENABLE_24V_POWER : { DataLog(log_level_safe_exec_info) << "ENABLE_24V_POWER"; break; }
      case ENABLE_64V_POWER : { DataLog(log_level_safe_exec_info) << "ENABLE_64V_POWER"; break; }
      case ENABLE_BOTH_POWER : { DataLog(log_level_safe_exec_info) << "ENABLE_BOTH_POWER"; break; }
      case ENABLE_SOLENOID_POWER : { DataLog(log_level_safe_exec_info) << "ENABLE_SOLENOID_POWER"; break; }
      case DISABLE_24V_POWER : { DataLog(log_level_safe_exec_info) << "DISABLE_24V_POWER"; break; }
      case DISABLE_64V_POWER : { DataLog(log_level_safe_exec_info) << "DISABLE_64V_POWER"; break; }
      case DISABLE_BOTH_POWER : { DataLog(log_level_safe_exec_info) << "DISABLE_BOTH_POWER"; break; }
      case DISABLE_SOLENOID_POWER : { DataLog(log_level_safe_exec_info) << "DISABLE_SOLENOID_POWER"; break; }
      case TEST_BOTH_POWER : { DataLog(log_level_safe_exec_info) << "TEST_BOTH_POWER"; break; }
      default : { DataLog(log_level_safe_exec_info) << "???"; break; }
   }
   DataLog(log_level_safe_exec_info) << endmsg;

   switch ( lPowerRequest )
   {
      case TEST_BOTH_POWER :
         // enable, force test
         _SafetyPowerControlTimer->PowerControlBoth(HW_ENABLE, TRUE);
         break;

      case ENABLE_24V_POWER :
         // enable, respond, don't force test
         _SafetyPowerControlTimer->PowerControl24v(HW_ENABLE, TRUE, FALSE);
         break;

      case ENABLE_64V_POWER :
         // enable, respond, don't force test
         _SafetyPowerControlTimer->PowerControl64v(HW_ENABLE, TRUE, FALSE);
         break;

      case ENABLE_BOTH_POWER :
         // enable, don't force test
         _SafetyPowerControlTimer->PowerControlBoth(HW_ENABLE, FALSE);
         break;

      case ENABLE_SOLENOID_POWER :
         // enable
         _SafetyPowerControlTimer->PowerControlDoor(HW_ENABLE);
         break;

      case DISABLE_24V_POWER :
         // disable, respond, don't force test
         _SafetyPowerControlTimer->PowerControl24v(HW_DISABLE, TRUE, FALSE);
         break;

      case DISABLE_64V_POWER :
         // disable, respond, don't force test
         _SafetyPowerControlTimer->PowerControl64v(HW_DISABLE, TRUE, FALSE);
         break;

      case DISABLE_BOTH_POWER :
         // disable, don't force test
         _SafetyPowerControlTimer->PowerControlBoth(HW_DISABLE, FALSE);
         break;

      case DISABLE_SOLENOID_POWER :
         // disable
         _SafetyPowerControlTimer->PowerControlDoor(HW_DISABLE);
         break;
      default :
      {
         DataLog(log_level_critical) << "Power Request: "
                                     << (int)lPowerRequest
                                     << " causes forbidden case switch default"
                                     << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "invalid PowerRequest");
      }

      break;

   } // end switch( lPowerRequest )

} // end void aRequestSafetyPowerMsg_rcvr :: notify()




// SPECIFICATION:    Safety Power Request Message Receiver
//                   Responds with TRUE/FALSE when asked
//                   if the power T1's have passed
//
//
// ERROR HANDLING:   none.

bool aRequestSafetyPowerMsg_rcvr::PowerT1OK (void)
{
   bool result = FALSE;
   char status24v, status64v;

   // get the current status from the power test objects
   status24v = _pa24vPowerTest->GetT1Status();
   status64v = _pa64vPowerTest->GetT1Status();

   // debug
   DataLog(log_level_safe_exec_info) << "Power T1, 24v : " << (int)status24v << ", 64v : " << (int)status64v << endmsg;

   if ( (status24v == PWR_CONTROL_NOMINAL)
        &&(status64v == PWR_CONTROL_NOMINAL) )
      result = TRUE;

   return(result);

}

/* FORMAT HASH 01f6fc04aa4421e6e65deff784e7b9b6 */

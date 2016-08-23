/*******************************************************************************
 *
 * Copyright (c) 1996-1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

// SPECIFICATION:    Donor disconnect test class.
//
// NOTES :
//
// ERROR HANDLING:   none.

#include "trima_assert.h"
#include "trima_datalog.h"
#include "s_disc.hpp"

#include "trimamessages.h"
/*******************************************************************************
 *
 *  aDonorDisconnectTest Methods
 *
 ******************************************************************************/


// SPECIFICATION:    Safety Disconnect Test destructor
//
// ERROR HANDLING:   none.

aDonorDisconnectTest::~aDonorDisconnectTest()
{}

// SPECIFICATION:    Safety Disconnect Test method
//                   really only used to start or restart
//                   the test.
//
//                   Note: This method was created as a fix
//                   for IT7771, decoupling test initiating
//                   behavior originally placed in method
//                   SetDonorDisconnectTestState.
//
// ERROR HANDLING:   none.
void aDonorDisconnectTest::request ()
{
   DataLog(log_level_safety_donor_disconnect_info) << "Safety donor disconnect test requested." << endmsg;

   // don't run the test if the cassette is not down
   const HW_CASSETTE_STATES position = _paCassette->GetCassettePosition();

   if (position != HW_CASSETTE_DOWN)
   {
      DataLog(log_level_safety_donor_disconnect_info)
         << "Cassette position incorrect for Disconnect Test: position="
         << (int)position << "="
         << ( (position == HW_CASSETTE_UNKNOWN) ? "HW_CASSETTE_UNKNOWN" :
           (position == HW_CASSETTE_DOWN) ? "HW_CASSETTE_DOWN" :
           (position == HW_CASSETTE_UP) ? "HW_CASSETTE_UP" : "?" )
         << endmsg;

      return;
   }

   // constrain cassette to not move
   _paCassette->SetDeviceConstraint(CASSETTE_MUST_REMAIN_DOWN);

   // indicate donor is connected
   _paDonor->SetDonorConnectedState(DONOR_IS_CONNECTED);

   // ensure monitoring is enabled
   _paReservoir->SetCycleMon(MON_DISCONNECT);

   _current = VALID;

   ResetTimer(0);   // Start the timer and state machine

   // now, ask for the state change...
   SetDonorDisconnectTestState(GOING_LOW);
}



// SPECIFICATION:    Safety Disconnect method (private) to
//                   change test state member.  This should
//                   only be called directly for states other
//                   than GOING_LOW, which is used to start/restart
//                   a test.  Request to start/restart a test - e.g.
//                   from 24V power control - should use the
//                   public request method instead.
//
// ERROR HANDLING:   none.

void aDonorDisconnectTest::SetDonorDisconnectTestState (char cDonorDisconnectTestState)
{
   DataLog(log_level_safety_donor_disconnect_info)
      << "aDonorDisconnectTest::SetDonorDisconnectTestState("
      << (int)cDonorDisconnectTestState << "="
      << ( (cDonorDisconnectTestState == DISCONNECT_TEST_NOT_DONE) ? "DISCONNECT_TEST_NOT_DONE" :
        (cDonorDisconnectTestState == WAITING_FOR_POWER_REQUEST) ? "WAITING_FOR_POWER_REQUEST" :
        (cDonorDisconnectTestState == GOING_LOW) ? "GOING_LOW" : "?" )
      << ")"
      << endmsg;

   _cDonorDisconnectTestState = cDonorDisconnectTestState;
}



// SPECIFICATION:    Safety Disconnect Test constructor
//
// ERROR HANDLING:   none.

aDonorDisconnectTest::aDonorDisconnectTest(short tickLimit
                                           , aDonor* paDonor
                                           , aCassette_Safety* paCassette
                                           , aReservoir_Safety*  paReservoir)
   : aSoftTimer_Safety(tickLimit, "Disconnect")
{
   trima_assert(paDonor);
   trima_assert(paCassette);
   trima_assert(paReservoir);

   _cDonorDisconnectTest = DISCONNECT_TEST_NOT_DONE;
   SetDonorDisconnectTestState(DISCONNECT_TEST_NOT_DONE);

   _current = NONE_DISC;

   _fDonorDisconnectPressure       = 0.0f;
   _fStabilizeThresholdLimit       = 0.0f;
   _fFinalThresholdLimit           = 0.0f;
   _lInletAccum                    = 0L;
   _lastInletAccum                 = 0L;
   _lDisconnectTestMessageCount    = 0L;
   _lastDisconnectTestMessageCount = 0L;

   _cDonorDisconnectTestCount      = 0;

   _paDonor                        = paDonor;
   _paCassette                     = paCassette;
   _paReservoir                    = paReservoir;

}

// SPECIFICATION:    Safety Disconnect Test method to record
//                   the test passed and release cassette to
//                   move.  Also releases all donor related
//                   monitoring.
//
// ERROR HANDLING:   none.

void aDonorDisconnectTest::SetDisconnectTestPassed ()
{
   DataLog(log_level_safety_donor_disconnect_info) << "aDonorDisconnectTest::SetDisconnectTestPassed()" << endmsg;

   _cDonorDisconnectTest = DISCONNECT_TEST_NOMINAL;
   // allow cassette to move
   _paCassette->SetDeviceConstraint(CASSETTE_MAY_BE_MOVED);

   // indicate donor is not connected
   _paDonor->SetDonorConnectedState(DONOR_IS_NOT_CONNECTED);

   // turn off monitoring
   _paReservoir->SetCycleMon(MON_INACTIVE);

   _paReservoir->LogProcedureTotals();
}

// SPECIFICATION:    Safety Disconnect Test method to ensure
//                   a new status message has been received.
//                   The timer evaluates the test every 100ms
//                   but new data is received only every 500ms.
//
// ERROR HANDLING:   none.

bool aDonorDisconnectTest::ensureNewData ()
{
   bool result = false;

   if (_lastDisconnectTestMessageCount < _lDisconnectTestMessageCount)
   {
      _lastDisconnectTestMessageCount = _lDisconnectTestMessageCount;
      result = true;
   }

   return result;
}



// SPECIFICATION:    Safety Disconnect Test method.
//                   This is the implementation of the state
//                   machine where the test actually runs.
//                   It is called with _current set to VALID
//                   to start the timer that drives the state
//                   machine.
//
// ERROR HANDLING:   none.

void aDonorDisconnectTest::notify (void)
{
   static unsigned long notify_count = 0;
   DisconnectStates     newState;
   bool                 newData      = false;

   notify_count++;

   static DisconnectStates Disconnect[DISCONNECT_STATES][3] =
   {
      // this is the state transition table for the disconnect test
      // the current state is where we are now
      // the transition state is were we want to go
      // the wait state is where we may be held up for a while
      //
      // CURRENT   TRANSITION     WAIT
      { VALID,     PUMPDOWN,    NONE_DISC},
      { PUMPDOWN,  STABILIZE,   PUMPDOWN},
      { STABILIZE, READP1,      STABILIZE},
      { READP1,    HOLD,        NONE_DISC},
      { HOLD,      READP2,      HOLD},
      { READP2,    FINAL,       NONE_DISC},
      { FINAL,     NONE_DISC,   NONE_DISC},
      { PASS_DISC, PASS_DISC,   PASS_DISC},
      { FAIL_DISC, FAIL_DISC,   FAIL_DISC},
      { NONE_DISC, NONE_DISC,   NONE_DISC}
   };

   // Call base class notify
   aSoftTimer_Safety::notify();

   // Determine if new pressure data has been received.
   newData = ensureNewData();

   // Initialize transition state to indicate "no transition"
   newState = _current;

   // switch on the current state and see if we can attain the transition state
   switch (Disconnect[_current][CURRENT])
   {
      case VALID :
         // Transition from VALID to PUMPDOWN
         // stabilization threshold is lower of -90mmHg or current-5mmHg
         _fStabilizeThresholdLimit = PRESSURE_LIMIT1;

         if ( (_fDonorDisconnectPressure + OFFSET1) < _fStabilizeThresholdLimit)
            _fStabilizeThresholdLimit = _fDonorDisconnectPressure + OFFSET1;

         // if we are not below the limit already, start the test
         if (_fDonorDisconnectPressure > STARTING_PRESSURE_LIMIT)
         {
            newState            = Disconnect[_current][TRANSITION];
            _pumpHasBeenStarted = false;
            _lastInletAccum     = _lInletAccum;

            // start the timer
            ResetTimer(GOING_LOW_TIMEOUT);
         }
         else
            newState = FAIL_DISC;

         DataLog(log_level_safety_donor_disconnect_info) << "Start Pressure = "
                                                         << _fDonorDisconnectPressure << ", Stabilization Threshold = "
                                                         << _fStabilizeThresholdLimit << endmsg;

         break;

      case PUMPDOWN :
         // Transition from PUMPDOWN to STABILIZE
         if (newData)
         {
            if (labs(_lInletAccum - _lastInletAccum) >= 10 * PUMPS_STOPPED_TOLERANCE &&
                !_pumpHasBeenStarted)
            {
               DataLog(log_level_safety_donor_disconnect_info) << "Detected pump start: " << labs(_lInletAccum - _lastInletAccum) << endmsg;
               _pumpHasBeenStarted = true;
            }
            else if (labs(_lInletAccum - _lastInletAccum) <= PUMPS_STOPPED_TOLERANCE)
            {
               if (_fDonorDisconnectPressure <= _fStabilizeThresholdLimit)
               {
                  newState = Disconnect[_current][TRANSITION];

                  // clear and restart the timer for the stabilization time
                  ResetTimer(STABILIZE_TIMEOUT);
                  DataLog(log_level_safety_donor_disconnect_info) << "Pressure at pump stop = " << _fDonorDisconnectPressure << endmsg;
               }
               else if (_pumpHasBeenStarted)
               {
                  _pumpHasBeenStarted = false;
                  ResetTimer(GOING_LOW_STOPPED_TIMEOUT);
                  DataLog(log_level_safety_donor_disconnect_info) << "Delaying after pump stop - pressure = " << _fDonorDisconnectPressure << endmsg;
               }
            }

            _lastInletAccum = _lInletAccum;
         }

         break;

      case STABILIZE :
         // Transition from STABILIZE to READP1
         if (GetTimerStatus() == TIMER_LIMIT_EXPIRED)
         {
            newState = Disconnect[_current][TRANSITION];
            ResetTimer(0); // Let timer free-run.
         }

         break;

      case READP1 :
         // Transition from READP1 to HOLD
         if (newData)
         {
            // be sure we get new status data after the stabilization time before taking
            // the stabilization pressure reading
            newState = Disconnect[_current][TRANSITION];

            // clear and restart the timer for the hold time
            ResetTimer(HOLD_TIMEOUT);

            if (_fDonorDisconnectPressure <= _fStabilizeThresholdLimit)
               // final threshold is 22mmHg above the reading after stabilization
               _fFinalThresholdLimit = _fDonorDisconnectPressure + LEAKAGE_ALLOWANCE;
            else
               newState = FAIL_DISC;

            DataLog(log_level_safety_donor_disconnect_info) << "Pressure after stabilization = "
                                                            << _fDonorDisconnectPressure << ", Hold threshold = "
                                                            << _fFinalThresholdLimit << endmsg;
         }

         break;

      case HOLD :
         // Transition from HOLD to READP2
         if (GetTimerStatus() == TIMER_LIMIT_EXPIRED)
         {
            newState = Disconnect[_current][TRANSITION];
            ResetTimer(0); // Let timer free-run.
         }

         break;

      case READP2 :
         // Final test
         // be sure we got new status data after the hold time before taking
         // the final pressure reading
         if (newData)
         {
            if (_fDonorDisconnectPressure <= _fFinalThresholdLimit)
               newState = PASS_DISC;
            else
               newState = FAIL_DISC;

            DataLog(log_level_safety_donor_disconnect_info) << "Pressure after hold = " << _fDonorDisconnectPressure << endmsg;

            // READP2 is the last timed state, so...
            StopTimer();
            // ... and let AreWeFinished() take care of the rest.
         }

         break;

      default :
         break;
   }

   // if we didn't transition, maybe we are in a wait state
   if (newState == _current)
   {
      switch (Disconnect[_current][WAIT])
      {
         case PUMPDOWN :
            if (GetTimerStatus() == TIMER_LIMIT_EXPIRED)
            {
               newState = FAIL_DISC;
               StopTimer(); // AreWeFinished() will handle the retry, if any
               DataLog(log_level_safety_donor_disconnect_info) << "Pumpdown Timer expired" << endmsg;
            }
            break;

         case STABILIZE :
         case HOLD :
            newState = Disconnect[_current][WAIT];
            break;

         default :
            break;
      }
   }
   else
   {
      DataLog(log_level_safety_donor_disconnect_info) << "aDonorDisconnectTest::notify: count=" << notify_count << "; going from ";
      switch (_current)
      {
         case VALID : { DataLog(log_level_safety_donor_disconnect_info) << "VALID";  break; }
         case PUMPDOWN : { DataLog(log_level_safety_donor_disconnect_info) << "PUMPDOWN";  break; }
         case STABILIZE : { DataLog(log_level_safety_donor_disconnect_info) << "STABILIZE";  break; }
         case READP1 : { DataLog(log_level_safety_donor_disconnect_info) << "READP1";  break; }
         case HOLD : { DataLog(log_level_safety_donor_disconnect_info) << "HOLD";  break; }
         case READP2 : { DataLog(log_level_safety_donor_disconnect_info) << "READP2";  break; }
         case FINAL : { DataLog(log_level_safety_donor_disconnect_info) << "FINAL";  break; }
         case PASS_DISC : { DataLog(log_level_safety_donor_disconnect_info) << "PASS_DISC";  break; }
         case FAIL_DISC : { DataLog(log_level_safety_donor_disconnect_info) << "FAIL_DISC";  break; }
         case NONE_DISC : { DataLog(log_level_safety_donor_disconnect_info) << "NONE_DISC";  break; }
      }

      DataLog(log_level_safety_donor_disconnect_info) << " to ";

      switch (newState)
      {
         case VALID : { DataLog(log_level_safety_donor_disconnect_info) << "VALID";  break; }
         case PUMPDOWN : { DataLog(log_level_safety_donor_disconnect_info) << "PUMPDOWN";  break; }
         case STABILIZE : { DataLog(log_level_safety_donor_disconnect_info) << "STABILIZE";  break; }
         case READP1 : { DataLog(log_level_safety_donor_disconnect_info) << "READP1";  break; }
         case HOLD : { DataLog(log_level_safety_donor_disconnect_info) << "HOLD";  break; }
         case READP2 : { DataLog(log_level_safety_donor_disconnect_info) << "READP2";  break; }
         case FINAL : { DataLog(log_level_safety_donor_disconnect_info) << "FINAL";  break; }
         case PASS_DISC : { DataLog(log_level_safety_donor_disconnect_info) << "PASS_DISC";  break; }
         case FAIL_DISC : { DataLog(log_level_safety_donor_disconnect_info) << "FAIL_DISC";  break; }
         case NONE_DISC : { DataLog(log_level_safety_donor_disconnect_info) << "NONE_DISC";  break; }
      }

      DataLog(log_level_safety_donor_disconnect_info) << endmsg;
   }

   _current = newState;
   AreWeFinished();
}

// SPECIFICATION:    Safety Disconnect Test method.
//                   Decides if the test is successful or not.
//
// ERROR HANDLING:   none.

void aDonorDisconnectTest::AreWeFinished ()
{
   // tell proc if we pass or not
   SafetyDisconnectStatMsg tellProc(MessageBase::SEND_GLOBAL);

   // see if we passed or failed.
   if (_current == PASS_DISC)
   {
      SetDisconnectTestPassed();
      _current = NONE_DISC;
      StopTimer();
      DataLog(log_level_safety_donor_disconnect_info) << "Disconnect Test Passed" << endmsg;


      DataLog(log_level_safety_donor_disconnect_info) << "Safety confirming donor disconnect to proc" << endmsg;
      tellProc.send(1); // tellProc we passed (1)
   }

   if (_current == FAIL_DISC)
   {
      // Count this test.  Fix for IT7771: Now that we've made the execution pathway for requesting a test
      // (from 24v power control) vs. any internal change to test state more explicit, we're able to
      // move this line here knowing that it will only be called by the test runner following a test failure.
      _cDonorDisconnectTestCount++;

      if (_cDonorDisconnectTestCount >= THREE_STRIKES_YER_OUT)
      {
         // three strikes pass of test.
         SetDisconnectTestPassed();

         _current = NONE_DISC;

         StopTimer();

         DataLog(log_level_safety_donor_disconnect_info) << "Disconnect Test "
                                                         << (int)_cDonorDisconnectTestCount << " Failures.  Unload OK." << endmsg;

         DataLog(log_level_safety_donor_disconnect_info) << "Safety confirming donor disconnect to proc" << endmsg;

         tellProc.send(1); // tellProc we passed (1)
      }
      else
      {
         _cDonorDisconnectTest = DISCONNECT_TEST_FAILED_HOLDING_LOW;

         SetDonorDisconnectTestState(WAITING_FOR_POWER_REQUEST);

         _current = NONE_DISC;

         StopTimer();

         DataLog(log_level_safety_donor_disconnect_info) << "Disconnect Test Failure "
                                                         << (int)_cDonorDisconnectTestCount << ".  Retry." << endmsg;

         DataLog(log_level_safety_donor_disconnect_info) << "Safety NOT confirming donor disconnect to proc" << endmsg;

         tellProc.send(0); // tellProc we failed (0)
      }
   }
}

// SPECIFICATION:    Safety Disconnect Test method.
//                   Prepares safety to run the disconnect test.
//
// ERROR HANDLING:   none.

void aDonorDisconnectTest::initializeForState (long newState)
{
   switch (newState)
   {
      case DONOR_DISCONNECT :
         DataLog(log_level_safety_donor_disconnect_info) << "Donor Disconnect detected by saf_exec" << endmsg;

         _cDonorDisconnectTest = DISCONNECT_TEST_IN_PROGRESS;

         SetDonorDisconnectTestState(WAITING_FOR_POWER_REQUEST);
         break;

      default :
         break;
   }
}

/* FORMAT HASH e932ec30cd1be84683aa6d18502da946 */

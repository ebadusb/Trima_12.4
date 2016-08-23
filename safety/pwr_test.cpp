/*******************************************************************************
 *
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.8  2003/11/14 20:14:19Z  ms10234
 * 5316 - rework 64v power tests and fix logging level
 * Revision 1.7  2003/07/01 22:34:05Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.6  2003/04/03 23:05:23Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.5  2002/08/07 17:53:54Z  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * To keep GNU compiler happy for production builds, add "default: break;" to "switch (_currentState)" in aSafetyPowerTest :: Update().
 * Revision 1.4  2002/07/23 16:52:15  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.3  2002/04/09 12:18:01  pn02526
 * "Revise logging to use DataLogLevel class."
 * Revision 1.2  2000/07/06 01:18:49Z  bs04481
 * Variable name change
 * Revision 1.1  2000/03/15 18:28:43  BS04481
 * Initial revision
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

#include "trima_datalog.h"
#include "pwr_test.hpp"


/*******************************************************************************
*
*  aSafetyPowerTest BASE CLASS METHODS DEFINITION
*
******************************************************************************/
// SPECIFICATION:    Power T1 Test object constructor
//
//
// ERROR HANDLING:   none.

aSafetyPowerTest::aSafetyPowerTest(short onLimit, short onTime,
                                   short offLimit, short offTime,
                                   short conversionMultiplier)
   : _highLimitVoltage(onLimit), _highLimitTime(onTime),
     _lowLimitVoltage(offLimit), _lowLimitTime(offTime),
     _ADCConversion(conversionMultiplier)
{
   _cPowerTest            = PWR_CONTROL_NOT_TESTED;
   _currentState          = NONE_POWER;
   _currentVoltage        = 0;
   _PowerTestTime.tv_sec  = 0L;
   _PowerTestTime.tv_nsec = 0L;
   _lastTestTime.tv_sec   = 0L;
   _lastTestTime.tv_nsec  = 0L;

}

// SPECIFICATION:    Power T1 Test object destructor
//
//
// ERROR HANDLING:   none.

aSafetyPowerTest::~aSafetyPowerTest()
{}

// SPECIFICATION:    Sets the appropriate voltage
//                   depending on which is under test
//                   in this object
//
//
// ERROR HANDLING:   none.

void aSafetyPowerTest::SetVoltage (HW_ORDERS newOrder)
{
   if (_cPowerTest == RUNNING_24V_PWR_TEST_ENAB)
      paSafetyHardwareCommandsExec->SetPumpPower(newOrder);
   else if (_cPowerTest == RUNNING_64V_PWR_TEST_ENAB)
      paSafetyHardwareCommandsExec->SetCentrifugePower(newOrder);
}


// SPECIFICATION:    Allows control status to push voltage
//                   to this object
//
// ERROR HANDLING:   none.
void aSafetyPowerTest::SetCurrentVoltage (short voltage)
{
   _currentVoltage = voltage;
}


// SPECIFICATION:    Sets up to start the power test
//
//
// ERROR HANDLING:   none.

void aSafetyPowerTest::SetPowerTest (char PowerTest)
{
   _cPowerTest            = PowerTest;
   _currentState          = NONE_POWER;
   _PowerTestTime.tv_sec  = 0L;
   _PowerTestTime.tv_nsec = 0L;
   _lastTestTime.tv_sec   = 0L;
   _lastTestTime.tv_nsec  = 0L;
}

// SPECIFICATION:    Performs the Power T1 test for
//                   the given supply.  The generic state
//                   transition order is:
//                      1. None
//                      2. Order switched supply on
//                      3. Wait for supply to come up
//                      4. Order switched supply off
//                      5. Wait for supply to go down
//                   If the supply is on when the test starts,
//                   we start at step 4.  If the supply is
//                   off when the test starts, we start at
//                   step 1.  The important part of the test
//                   if step 4 and 5.
//
// ERROR HANDLING:   none.

void aSafetyPowerTest::Update ()
{
   struct timespec now;
   long            dt;
   int             loggingVoltage, loggingLowLimitVoltage, loggingHighLimitVoltage;
   PowerTestStates newState;

   // assume we are staying where we are
   newState                = _currentState;

   loggingVoltage          = (int)((_currentVoltage * 100) / _ADCConversion);
   loggingLowLimitVoltage  = (int)((_lowLimitVoltage * 100) / _ADCConversion);
   loggingHighLimitVoltage = (int)((_highLimitVoltage * 100) / _ADCConversion);

   // check the current state and see if we can advance
   switch (_currentState)
   {
      case NONE_POWER :
         // if the power is on, order it off
         // otherwise, order it on
         if (_currentVoltage > _highLimitVoltage)
            newState = ORDER_OFF;
         else
            newState = ORDER_ON;
         DataLog(log_level_safe_exec_info) << std::dec << "NewState: " << newState << " - New Test" << endmsg;

         break;
      case ORDER_OFF :
         // order power off and start timing
         SetVoltage(HW_DISABLE);
         clock_gettime(CLOCK_REALTIME, &_PowerTestTime);
         _lastTestTime.tv_sec = _PowerTestTime.tv_sec;
         newState             = WAIT_OFF;
         DataLog(log_level_safe_exec_info) << std::dec << "NewState: "
                                           << newState
                                           << " - Ordered Off, starting voltage = "
                                           << loggingVoltage
                                           << " v, Goal = "
                                           << loggingLowLimitVoltage
                                           << " v"
                                           << endmsg;
         break;
      case WAIT_OFF :
         // see if power has turned off
         if (_currentVoltage < _lowLimitVoltage)
            newState = PASS_POWER;
         DataLog(log_level_safe_exec_info) << std::dec << "NewState: "
                                           << newState
                                           << " - Wait Off = "
                                           << loggingVoltage
                                           << " v, Goal = "
                                           << loggingLowLimitVoltage
                                           << " v"
                                           << endmsg;
         break;
      case ORDER_ON :
         // order power on and start timing
         SetVoltage(HW_ENABLE);
         clock_gettime(CLOCK_REALTIME, &_PowerTestTime);
         _lastTestTime.tv_sec = _PowerTestTime.tv_sec;
         newState             = WAIT_ON;
         DataLog(log_level_safe_exec_info) << std::dec << "NewState: "
                                           << newState
                                           << " - Ordered On, starting voltage = "
                                           << loggingVoltage
                                           << " v, Goal= "
                                           << loggingHighLimitVoltage
                                           << " v"
                                           << endmsg;
         break;
      case WAIT_ON :
         // see if power has turned on
         if (_currentVoltage > _highLimitVoltage)
            newState = ORDER_OFF;
         DataLog(log_level_safe_exec_info) << std::dec << "NewState: "
                                           << newState
                                           << " - Wait On = "
                                           << loggingVoltage
                                           << " v, Goal = "
                                           << loggingHighLimitVoltage
                                           << " v"
                                           << endmsg;
         break;
      default :
         // Why are we here if we have already passed,
         // failed or are not running?
         DataLog(log_level_safe_exec_info) << "Test Not in Progress" << endmsg;
   }

   // if we didn't make a transition, check the time
   if (newState == _currentState)
   {
      switch (_currentState)
      {
         case WAIT_OFF :
            // check the clock but only worry about the seconds
            clock_gettime(CLOCK_REALTIME, &now);
            dt = now.tv_sec - _PowerTestTime.tv_sec;
            if ( (dt < 0)
                 ||( (now.tv_sec - _lastTestTime.tv_sec) > 2) )
            {
               // somebody reset the clock so reset the time
               clock_gettime(CLOCK_REALTIME, &_PowerTestTime);
               _lastTestTime.tv_sec = _PowerTestTime.tv_sec;
            }
            else if (dt > _lowLimitTime)
            {
               // time's up
               newState = FAIL_POWER;
               DataLog(log_level_safe_exec_info) << "PowerTest: "  << _cPowerTest << " - Power Test Timeout Waiting for Off" << endmsg;
            }
            else
               _lastTestTime.tv_sec = now.tv_sec;
            break;
         case WAIT_ON :
            // check the clock but only worry about the seconds
            clock_gettime(CLOCK_REALTIME, &now);
            dt = now.tv_sec - _PowerTestTime.tv_sec;
            if ( (dt < 0)
                 ||( (now.tv_sec - _lastTestTime.tv_sec) > 2) )
            {
               // somebody reset the clock so reset the time
               clock_gettime(CLOCK_REALTIME, &_PowerTestTime);
               _lastTestTime.tv_sec = _PowerTestTime.tv_sec;
            }
            else if (dt > _highLimitTime)
            {
               // time's up
               newState = FAIL_POWER;
               DataLog(log_level_safe_exec_info) << "PowerTest: "  << _cPowerTest << " - Power Test Timeout Waiting for On" << endmsg;
            }
            else
               _lastTestTime.tv_sec = now.tv_sec;
            break;
         default :
            break;
      }
   }

   // check if it passed or failed
   if (newState == PASS_POWER)
   {
      // turn it on if it passed
      SetVoltage(HW_ENABLE);
      DataLog(log_level_safe_exec_info) << "Power Test Passed "
                                        << loggingVoltage
                                        << " v"
                                        << endmsg;
      _cPowerTest = PWR_CONTROL_NOMINAL;
   }
   else if (newState == FAIL_POWER)
   {
      // turn it off if it failed
      SetVoltage(HW_DISABLE);
      DataLog(log_level_safe_exec_error) << "Power Test Failed "
                                         << loggingVoltage
                                         << " v"
                                         << endmsg;
      _cPowerTest = PWR_CONTROL_FAILED;
   }

   // save the state
   _currentState = newState;
}

/* FORMAT HASH 3fa04110a83588f0ccacff2c6b746f4c */

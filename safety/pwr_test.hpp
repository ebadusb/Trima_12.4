/*******************************************************************************
 *
 * Copyright (c) 1996-1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.3  2002/04/23 21:09:33Z  pn02526
 * Change type of T1Status to clear up compile warnings.
 * Revision 1.2  2000/07/05 19:18:50  bs04481
 * Variable name change
 * Revision 1.1  2000/03/15 18:28:44  BS04481
 * Initial revision
 * Revision 1.3  1999/08/01 20:53:20  BS04481
 * Non-critical code review changes identified at the 3.2 release.
 * Revision 1.2  1998/11/05 16:23:05  bs04481
 * Timeouts should be in sec not in msec
 * Revision 1.1  1998/09/28 21:50:42  bs04481
 * Initial revision
 *
 *
 * TITLE:      pwr_test.hpp
 *             Safety 24v and 64v power test class definitions.
 *
 *
 * ABSTRACT:   This file contains the class definitions for the
 *             object that performs the 24v and 64v power tests
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 ******************************************************************************/

#ifndef PWR_TEST_HPP  //  Prevents multiple compilations.
#define PWR_TEST_HPP

#include "error.h"       // 07/26/96 _FATAL_ERROR, _LOG_ERROR
#include "hw.h"
#include "s_glob.hpp"

enum PowerTestStates
{
   ORDER_OFF = 0,
   WAIT_OFF,
   ORDER_ON,
   WAIT_ON,
   PASS_POWER,
   FAIL_POWER,
   NONE_POWER
};

#define POWER_TEST_STATES 7

// 03/12/96 first cut acceptable adc readings from control status
// during power switch tests
// using scovill gains from pump....
// to use scovills scaling eqns : v = raw * mult/div/100
// where for 24V, mult = 10000, div = 9091, for 64V, mult = 10000, div = 1883
//
#define MULT_24V                         9091                                // 24V ADC Gain
#define MULT_64V                         1883                                // 64V ADC Gain
// make 24V cutoff 12V 06/14/96 due to cramer falling so slowly - old value 5.
#define CUTOFF_24V                       12                              // make 24V OFF voltage 12V
// change 64V cutoff voltage from 5V to 8V to make it hard to get safety alarm when spining centrifuge without power
#define CUTOFF_64V                        8
#define TURNON_24V                       22                              // make 24v ON voltage 22v
#define TURNON_64V                       62                              // make 64v ON voltage 62
/// lint fix 07/26/96 parentheses
#define ACCEPT_24V_OFF_RAW_ADC           ( CUTOFF_24V * (MULT_24V / 100) ) // 24V sw OFF limin in RAW ADCC
#define ACCEPT_24V_ON_RAW_ADC            ( TURNON_24V * (MULT_24V / 100) ) // 24V sw OFF limit in RAW ADCC
#define ACCEPT_64V_OFF_RAW_ADC           ( CUTOFF_64V * (MULT_64V / 100) ) // 64V sw on limit in RAW ADC
#define ACCEPT_64V_ON_RAW_ADC            ( TURNON_64V * (MULT_64V / 100) ) // 64V sw on limit in RAW ADC

// power test state indicators
#define PWR_CONTROL_NOT_TESTED             0x0                      // T1 test for power control has not been performed ( init value )
#define PWR_CONTROL_NOMINAL                0x1                      // T1 test for power control passed
#define PWR_CONTROL_FAILED                 0x2                      // T1 test for power control failed
#define RUNNING_24V_PWR_TEST_ENAB          0x4                      // 24v T1 Test in progress
#define RUNNING_64V_PWR_TEST_ENAB          0x8                      // 64v T1 Test in progress

// allow 15 seconds for high to low transition
#define PWR_LOW_TEST_NOTIFICATION_TOLERANCE      15              // 15 sec
// allow 5 seconds for low to high transition
#define PWR_HIGH_TEST_NOTIFICATION_TOLERANCE     5               // 5 sec


/*******************************************************************************
*
*  aSafetyPowerTest BASE CLASS DEFINITION
*
******************************************************************************/
class aSafetyPowerTest
{
private:
   char _cPowerTest;                         // PWR_CONTROL_NOT_TESTED, PWR_CONTROL_NOMINAL,
                                             // PWR_CONTROL_FAILED, RUNNING_24V_PWR_TEST_ENAB,
                                             // RUNNING_64V_PWR_TEST_ENAB
   PowerTestStates _currentState;            // current state of test
   struct timespec _PowerTestTime;           // timer for wait states
   struct timespec _lastTestTime;            // last sample time.  accommodate clock reset

   short           _currentVoltage;          // voltage measurement now
   short           _highLimitVoltage;        // minimum on voltage required
   short           _lowLimitVoltage;         // maximum off voltage allowed
   short           _highLimitTime;           // max time to achieve on voltage
   short           _lowLimitTime;            // max time to achieve off voltage
   short           _ADCConversion;           // conversion constant for ADC

   aSafetyPowerTest();                                     // not implemented
   aSafetyPowerTest(aSafetyPowerTest const&);              // not implemented
   aSafetyPowerTest& operator = (aSafetyPowerTest const&); // not implemented
public:
   aSafetyPowerTest(short onLimit, short onTime,
                    short offLimit, short offTime,
                    short conversionMultiplier);
   virtual ~aSafetyPowerTest();
   void Update ();
   void SetVoltage (HW_ORDERS newOrder);
   void SetCurrentVoltage (short voltage);
   void SetPowerTest (char PowerTest);

   T1STATUS GetT1Status (void)
   {
      return( _cPowerTest );
   }
};

/*******************************************************************************
*
*  aSafety24vPowerTest DERIVED CLASS DEFINITION
*
******************************************************************************/
class aSafety24vPowerTest
   : public aSafetyPowerTest
{
public:
   aSafety24vPowerTest()
      : aSafetyPowerTest(ACCEPT_24V_ON_RAW_ADC,
                         PWR_HIGH_TEST_NOTIFICATION_TOLERANCE,
                         ACCEPT_24V_OFF_RAW_ADC,
                         PWR_LOW_TEST_NOTIFICATION_TOLERANCE,
                         MULT_24V){}
};

/*******************************************************************************
*
*  aSafety64vPowerTest DERIVED CLASS DEFINITION
*
******************************************************************************/
class aSafety64vPowerTest
   : public aSafetyPowerTest
{
public:
   aSafety64vPowerTest()
      : aSafetyPowerTest(ACCEPT_64V_ON_RAW_ADC,
                         PWR_HIGH_TEST_NOTIFICATION_TOLERANCE,
                         ACCEPT_64V_OFF_RAW_ADC,
                         PWR_LOW_TEST_NOTIFICATION_TOLERANCE,
                         MULT_64V){}

};


#endif /* PWR_TEST_HPP */

/* FORMAT HASH 1c5fabe3052ddccd7fbfb78a46c429b2 */

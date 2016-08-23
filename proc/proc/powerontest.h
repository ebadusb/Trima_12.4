/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      powerontest.h
 *
 * ABSTRACT:   This header file defines the global data and classes for the
 *                  Startup Tests - POWER TESTS.
 *
 */


#ifndef POWERONTEST_H  //  Prevents multiple compilations.
#define POWERONTEST_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"
#include "trimamessages.h"

class PowerOnTest
   : public StateAbs
{
   DECLARE_STATE(PowerOnTest);
public:

   // Constructor
   PowerOnTest();

   // Copy constructor
   PowerOnTest(const PowerOnTest& state);

   // Destructor
   virtual ~PowerOnTest();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback function when voltage rise fails to complete in time ...
   //
   virtual void voltageRiseTooSlow (void);
   virtual int  voltageAndCurrentOK (void);

   //
   // Create a duplicate of myself
   //
   virtual StateAbs* clone () const { return ( new PowerOnTest(*this)); }

protected:

   //
   // Main processing function
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // Functions called one time at the entrance and exit
   //  transitions to and from this state
   //
   virtual int preEnter ();
   virtual int postExit ();

   //
   // Common handler for test failures
   //
   void failTest (const char* msg, int userCode);

   //
   // Function used to set up the test ...
   //
   void initTest ();

   //
   // Tests the volume accrual of all pumps
   //
   int pumpVolumesTest ();

   //
   // Tests the volume accrual of all pumps against the
   //  limit.  Logs the volume that is over if any are over
   //
   int pumpVolumesLessThan (const float limit);

   //
   // Reset the state upon termination
   //
   virtual void reset ();

   //
   // Delete any dynamically allocated memory here
   //
   virtual void cleanup ();

protected:

   //
   // Message to request enabling and disabling of power from safety ...
   //
   RequestSafetyPowerMsg _RequestSafetyTestMsg;

   //
   // Alarm for power test failure ...
   //
   anAlarmMsg _PowerTestAlarm;

   //
   // Timer that will produce an error if voltage rise
   //  doesn't complete in the allotted time ...
   //
   TimerMessage _VoltageRiseTooSlowTimer;

   //
   // Test passing flags ...
   //
   int _TestPassed;
   int _FirstTestPassed;
   int _SecondTestPassed;
   int _PowerEnabled;
};


#endif

/* FORMAT HASH 4b42a5a48318cc969ebdecec44f8c3e6 */

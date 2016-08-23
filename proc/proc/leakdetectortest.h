/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      leakdetectortest.h
 *
 * ABSTRACT:   This header file defines the global data and classes for the
 *                  Startup Tests - LEAK TEST.
 *
 */


#ifndef LEAKDETECTORTEST_H  //  Prevents multiple compilations.
#define LEAKDETECTORTEST_H

#include <an_alarm.h>
#include <stateabs.h>

class LeakDetectorTest
   : public StateAbs
{
   DECLARE_STATE(LeakDetectorTest);
public:

   // Constructor
   LeakDetectorTest();

   // Copy Constructor
   LeakDetectorTest(const LeakDetectorTest& state);

   // Destructor
   virtual ~LeakDetectorTest();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Create a duplicate of myself
   //
   virtual StateAbs* clone () const { return ( new LeakDetectorTest(*this)); }

protected:

   //
   // Main processing function
   //
   virtual int preProcess ();
   virtual int postProcess ();

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
   // Alarm for test failure
   //
   anAlarmMsg _LeakDetectorAlarm;

   //
   // Flag to indicate test completion
   //
   int _TestPassed;

};

#endif

/* FORMAT HASH d876f26235aeb690068ed5bac70d3940 */

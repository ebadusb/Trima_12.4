/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      valvestest.h
 *
 * ABSTRACT:   This header file defines the global data and classes for the
 *                  Startup Tests - VALVES TEST.
 *
 */


#ifndef VALVESTEST_H  //  Prevents multiple compilations.
#define VALVESTEST_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"

class ValvesTest
   : public StateAbs
{
   DECLARE_STATE(ValvesTest);
public:

   // Constructor
   ValvesTest();

   // Copy Constructor
   ValvesTest(const ValvesTest& state);

   // Destructor
   virtual ~ValvesTest();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback for the valve timer ...
   //
   virtual void valvesTooSlow ();

   //
   // Create a duplicate of myself
   //
   virtual StateAbs* clone () const { return ( new ValvesTest(*this)); }

protected:

   //
   // Main processing function
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // One time per entry and exit functions
   //
   virtual int preEnter ();
   virtual int postExit ();

   //
   // Function to command the valves
   //
   void orderValvesToReturn ();
   void orderValvesToCollect ();
   void orderValvesToOpen ();

protected:

   //
   // Timer to check valve movement
   //
   TimerMessage _ValveTimer;

   //
   // Alarm for slow valve movement
   //
   anAlarmMsg _ValveAlarm;

   //
   // Flag to signify the test has failed
   //
   bool _TestFailed;

   //
   // Flags used to show which positions the valves have
   //  been seen to move into
   //
   bool _AllValvesSeenReturn;
   bool _AllValvesSeenCollect;
   bool _AllValvesSeenOpen;

   //
   // Flag to show the timer has been started
   //
   bool _TimerStarted;

};

#endif

/* FORMAT HASH 35d4ad9a13dbf5b15c920c97bd2d1194 */

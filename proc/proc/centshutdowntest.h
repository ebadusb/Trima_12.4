/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      centshutdowntest.h
 *
 * ABSTRACT:   This header file defines the global data and classes for the
 *                  Startup Tests - CENTRIFUG SHUTDOWN TEST.
 *
 */


#ifndef CENTSHUTDOWNTEST_H  //  Prevents multiple compilations.
#define CENTSHUTDOWNTEST_H

#include "an_alarm.h"
#include "stateabs.h"
#include "trimamessages.h"


class CentShutdownTest
   : public StateAbs
{
   DECLARE_STATE(CentShutdownTest);
public:

   // Constructor
   CentShutdownTest();

   // Copy Constructor
   CentShutdownTest(const CentShutdownTest& state);

   // Destructor
   virtual ~CentShutdownTest();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Create a duplicate of myself
   //
   virtual StateAbs* clone () const { return ( new CentShutdownTest(*this)); }

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
   // Delete any dynamically allocated memory here
   //
   virtual void cleanup ();

   //
   // Helper functions
   //
   void startTest ();
   void failTest (const char* msg);
   void logVoltages (void);

protected:

   //
   // Status variables
   //
   float _TestStartTime;
   float _VoltageOKTime;
   bool  _VoltageCheckComplete;
   bool  _ShutdownCheckComplete;
   bool  _TestFailed;
   bool  _TestStarted;
};

#endif

/* FORMAT HASH 7364ca0f0079d5847cbbff408c977714 */

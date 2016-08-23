/*
 * Copyright (c) 2009 by CaridianBCT BCT, Inc.  All rights reserved.
 *
 * TITLE:      centrifugeSpinTest.h
 *
 *     This state performs the centrifuge spin test.
 */


#ifndef CENTRIFUGE_SPIN_TEST__H  //  Prevents multiple inclusions.
#define CENTRIFUGE_SPIN_TEST__H

#include <stateabs.h>

#include "trimamessages.h"
#include "timermessage.h"


// Class Declaration
class CentrifugeSpinTest
   : public StateAbs
{
   DECLARE_STATE(CentrifugeSpinTest);
public:

   // Default constructor
   CentrifugeSpinTest();

   // Destructor
   virtual ~CentrifugeSpinTest();

   //
   // Functions to do this states processing ...
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // Function called upon first entrance to this state
   //
   virtual int preEnter ();

   //
   // Function called upon transition from this state
   //
   virtual int postExit ();

   //
   // Function called to determine if ready to transition from this state
   //
   virtual int transitionStatus ();

private:

   // TimerMessage callback
   void stopTest ();

   int          _runTime;
   TimerMessage _centSpinTimer;
   bool         _transitionNow;
   bool         _thisTestIsComplete;
};


#endif

/* FORMAT HASH 727d562865af074ca20c6704dafc2d77 */

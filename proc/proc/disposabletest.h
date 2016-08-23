/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      disposabletest.h
 *
 */


#ifndef DISPOSABLETEST_H  //  Prevents multiple inclusions.
#define DISPOSABLETEST_H

#include "stateabs.h"
#include "centrifugeSpinTest.h"


// Class Declaration
class DisposableTest
   : public StateAbs
{
   DECLARE_STATE(DisposableTest);
public:

   // Default constructor
   DisposableTest();

   // Destructor
   virtual ~DisposableTest();

   //
   // Function to initialize the parallel, CentrifugeTests, state.  This
   //  is overridden from the stateabs class which must be called after
   //  this function does its processing.
   //
   virtual int init ();

   virtual const char* currentState () const;

protected:

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

protected:
   CentrifugeSpinTest _centSpin;
   int                _centSpinStatus;

};


#endif

/* FORMAT HASH 5b30825a91dc5e5527bd1f907724fb9a */

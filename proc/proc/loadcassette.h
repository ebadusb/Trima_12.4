/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      loadcassette.h
 *
 */


#ifndef LOADCASSETTE_H  //  Prevents multiple inclusions.
#define LOADCASSETTE_H

#include <stateabs.h>
#include "centrifugetests.h"
#include "an_alarm.h"


// Class Declaration
class LoadCassette
   : public StateAbs
{
   DECLARE_STATE(LoadCassette);
public:

   // Default constructor
   LoadCassette();

   // Default destructor
   virtual ~LoadCassette();

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

   CentrifugeTests _CentrifugeTests;
   int             _CentrifugeTestsStatus;


};


#endif

/* FORMAT HASH 5ae86b4092c51dac62233a3d61fd3fde */

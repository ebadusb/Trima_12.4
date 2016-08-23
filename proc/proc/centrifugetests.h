/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      centrifugetests.h
 *
 * ABSTRACT:   This header file defines the global data and classes for the
 *                 various centrifuge tests.
 *
 */


#ifndef CENTRIFUGE_TESTS_H  //  Prevents multiple compilations.
#define CENTRIFUGE_TESTS_H

#include <stateabs.h>

class CentrifugeTests
   : public StateAbs
{
   DECLARE_STATE(CentrifugeTests);
public:

   // Constructor
   CentrifugeTests();

   // Destructor
   virtual ~CentrifugeTests();

protected:

   //
   // Main processing function
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // One time per entrance/exit functions ...
   //
   virtual int preEnter ();
   virtual int postExit ();

protected:

   bool _DoorWasLocked;

};

#endif

/* FORMAT HASH 97abd74e71c67e2a2a390e3a2e0d0300 */

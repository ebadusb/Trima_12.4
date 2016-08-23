/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      startuptest.h
 *
 */


#ifndef STARTUPTEST_H  //  Prevents multiple inclusions.
#define STARTUPTEST_H

#include <stateabs.h>

// Class Declaration
class StartupTest
   : public StateAbs
{
   DECLARE_STATE(StartupTest);
public:

   // Default constructor
   StartupTest();

   // Copy constructor
   StartupTest(const StartupTest& state);

   // Default destructor
   virtual ~StartupTest();


   virtual StateAbs* clone () const { return ( new StartupTest(*this)); }

protected:

   //
   // Function called upon first entrance to this state
   //
   virtual int preEnter ();
   //
   // Functions to do this states processing ...
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const StartupTest& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();


protected:
};


#endif

/* FORMAT HASH acb1485899c6528ab5c73c3c7425c691 */

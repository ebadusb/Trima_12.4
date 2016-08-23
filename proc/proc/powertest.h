/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      powertest.h
 *
 * ABSTRACT:   This header file defines the global data and classes for the
 *                  Startup Tests - POWER TESTS.
 *
 */


#ifndef POWERTEST_H
#define POWERTEST_H

#include <stateabs.h>

class PowerTest
   : public StateAbs
{
   DECLARE_STATE(PowerTest);
public:

   // Constructor
   PowerTest();

   // Destructor
   virtual ~PowerTest();

   //
   // Create a duplicate of myself
   //
   virtual StateAbs* clone () const { return ( new PowerTest(*this)); }

protected:

   //
   // Main processing function
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // One time per entrance function ...
   //
   virtual int preEnter ();

   //
   // Delete any dynamically allocated memory here
   //
   virtual void cleanup ();

protected:

};

#endif

/* FORMAT HASH cf06d3d4819d801bfe438bcdeb15c365 */

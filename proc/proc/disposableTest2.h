/*
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      disposableTest2.h
 *
 *
 *
 */

/*
$Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/proc/proc/rcs/disposableTest2.h 1.1 2007/02/20 22:10:10Z jheiusb Exp jheiusb $
$Log: disposableTest2.h $
*/

#ifndef DISP_TEST_2_H  //  Prevents multiple inclusions.
#define DISP_TEST_2_H

#include <stateabs.h>


// Class Declaration
class DisposableTest2
   : public StateAbs
{
   DECLARE_STATE(DisposableTest2);
public:

   // Default constructor
   DisposableTest2();

   // Copy constructor
   DisposableTest2(const DisposableTest2& state);

   // Default destructor
   virtual ~DisposableTest2();

   virtual StateAbs* clone () const { return ( new DisposableTest2(*this)); }

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


   virtual int transitionStatus ();

   //
   // Function called upon transition from this state
   //
   virtual int postExit ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const DisposableTest2& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

private:
   bool useAirMitigation;
   bool whiteStamp;

};





#endif

/* FORMAT HASH 97091294033d0b5a7d1a4c815439f30b */

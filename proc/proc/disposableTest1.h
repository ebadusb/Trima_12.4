/*
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      disposableTest1.h
 *
 *
 *
 */

/*
$Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/proc/proc/rcs/disposableTest1.h 1.1 2007/02/20 22:10:10Z jheiusb Exp jheiusb $
$Log: disposableTest1.h $
*/

#ifndef DISP_TEST_1_H  //  Prevents multiple inclusions.
#define DISP_TEST_1_H

#include <stateabs.h>


// Class Declaration
class DisposableTest1
   : public StateAbs
{
   DECLARE_STATE(DisposableTest1);
public:

   // Default constructor
   DisposableTest1();

   // Copy constructor
   DisposableTest1(const DisposableTest1& state);

   // Default destructor
   virtual ~DisposableTest1();

   virtual StateAbs* clone () const { return ( new DisposableTest1(*this)); }

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
   virtual void copyOver (const DisposableTest1& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

private:
   bool useAirMitigation;
   bool whiteStamp;

};





#endif

/* FORMAT HASH 5399da3062c7dc8320bf1f623bbddd25 */

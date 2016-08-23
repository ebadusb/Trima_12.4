/*
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:       closeCrossoverClamp
 *
 *
 */

/*
$Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/proc/proc/rcs/closeCrossoverClamp.h 1.1 2007/02/20 22:10:55Z jheiusb Exp jheiusb $
$Log: closeCrossoverClamp.h $
*/

#ifndef CLOSE_CROSS_OVER_CLAMP_H  //  Prevents multiple inclusions.
#define CLOSE_CROSS_OVER_CLAMP_H

#include <stateabs.h>


// Class Declaration
class CloseCrossoverClamp
   : public StateAbs
{
   DECLARE_STATE(CloseCrossoverClamp);
public:

   // Default constructor
   CloseCrossoverClamp();

   // Copy constructor
   CloseCrossoverClamp(const CloseCrossoverClamp& state);

   // Default destructor
   virtual ~CloseCrossoverClamp();

   virtual StateAbs* clone () const { return ( new CloseCrossoverClamp(*this)); }

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
   virtual void copyOver (const CloseCrossoverClamp& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();




};





#endif

/* FORMAT HASH eda7131d7fba5755203f5f62f1617cc2 */

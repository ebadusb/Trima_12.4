/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      acprime.h
 *
 */


#ifndef ACPRIME_H  //  Prevents multiple inclusions.
#define ACPRIME_H

#include <stateabs.h>


// Class Declaration
class ACPrime
   : public StateAbs
{
   DECLARE_STATE(ACPrime);
public:

   // Default constructor
   ACPrime();

   // Copy constructor
   ACPrime(const ACPrime& state);

   // Default destructor
   virtual ~ACPrime();

   virtual StateAbs* clone () const { return ( new ACPrime(*this)); }

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

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const ACPrime& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

};


#endif

/* FORMAT HASH 58a3aaee9774ebbef7bdb756f4cfef36 */

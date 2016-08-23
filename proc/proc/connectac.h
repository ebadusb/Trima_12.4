/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      connectac.h
 *
 */


#ifndef _CONNECTAC_H_  //  Prevents multiple inclusions.
#define _CONNECTAC_H_

#include <stateabs.h>


// Class Declaration
class ConnectAC
   : public StateAbs
{
   DECLARE_STATE(ConnectAC);
public:

   // Default constructor
   ConnectAC();

   // Copy constructor
   ConnectAC(const ConnectAC& state);

   // Default destructor
   virtual ~ConnectAC();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   virtual StateAbs* clone () const { return ( new ConnectAC(*this)); }

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
   virtual void copyOver (const ConnectAC& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

};


#endif

/* FORMAT HASH 26240a92d9743c48a0b96b97b237e285 */

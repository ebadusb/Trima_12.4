/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      connectdonor.h
 *
 * ABSTRACT:   This state sets the donor connection status.
 *
 */


#ifndef CONNECTDONOR_H  //  Prevents multiple inclusions.
#define CONNECTDONOR_H

#include "stateabs.h"
#include "runinfomsg.h"

class ConnectDonor
   : public StateAbs
{
   DECLARE_STATE(ConnectDonor);
public:

   // Default constructor
   ConnectDonor();

   // Copy constructor
   ConnectDonor(const ConnectDonor& state);

   // Default destructor
   virtual ~ConnectDonor();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new ConnectDonor(*this)); }

protected:

   //
   // Functions to do this state's processing ...
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
   // Reset the state upon termination
   //
   virtual void reset ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const ConnectDonor& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:
   //
   // Run information for GUI ...
   //
   CRunInfoMsg _RunInfo;

};


#endif

/* FORMAT HASH 1663b27776b68fa9d75bace807c1dabf */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      donorconnected.h
 *
 * ABSTRACT:   This is the controling state when the donor is connected.
 *
 */


#ifndef DONORCONNECTED_H  //  Prevents multiple inclusions.
#define DONORCONNECTED_H

#include "runinfomsg.h"
#include "stateabs.h"
#include "aplimiter.h"
#include "trimamessages.h"

class DonorConnected
   : public StateAbs
{
   DECLARE_STATE(DonorConnected);
public:

   // Default constructor
   DonorConnected();

   // Destructor
   virtual ~DonorConnected();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

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

private:
   //
   // Function which will set the Adjust Control Button status
   //
   void setButtonStatus ();

   //
   // Message to request power...
   //
   RequestSafetyPowerMsg _RequestPowerMsg;

   //
   // Run information for GUI ...
   //
   CRunInfoMsg _RunInfo;

   //
   // Access Pressure limit setting object
   //
   APLimiter _AccessPressureLimiter;

};


#endif

/* FORMAT HASH 10951da80d748a1b01b4619b3c8bef32 */

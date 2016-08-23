/*
 * Copyright (c) 2013 by Terumo BCT, Inc.  All rights reserved.
 *
 * TITLE:      closevalvesanddisconnect.h
 *
 */


#ifndef CLOSEVALVESANDDISCONNECT_H  //  Prevents multiple inclusions.
#define CLOSEVALVESANDDISCONNECT_H

#include "closevalves.h"
#include "an_alarm.h"
#include "stateabs.h"
#include "trimamessages.h"


// Class Declaration
class CloseValvesAndDisconnect
   : CloseValves
{
   DECLARE_STATE(CloseValvesAndDisconnect);
public:

   // Default constructor
   CloseValvesAndDisconnect();

   // Copy constructor
   CloseValvesAndDisconnect(const CloseValvesAndDisconnect& state);

   // Default destructor
   virtual ~CloseValvesAndDisconnect();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // callback for the power request taking too long
   void powerTooSlow ();

   //
   // Callback for the power request response
   //
   void powerRequestResponse ();

   //
   // Callback function for valve timer ...
   //
   virtual void valvesTimedOut ();

protected:

   //
   // Message to request power...
   //
   RequestSafetyPowerMsg _RequestPowerMsg;

   //
   // Function called upon first entrance to this state
   //
   virtual int preEnter ();

   //
   // Function called upon transition from this state
   //
   virtual int postExit ();

   //
   // Function to ask for 24V Power
   //
   void request24VPower ();


   //
   // Message that receives the response from the
   //  safety power request
   //
   SafetyTestDoneMsg _PowerRequestResponseMsg;

   //
   // Flag to save whether or not we registered for the power response yet ...
   bool _PowerRespRegistered;

   //
   // Timer for the power
   //
   TimerMessage _PowerTimer;

   //
   // valves commanded yet
   //
   bool _ValvesCommanded;

protected:

   TimerMessage _LocalValveTimer;

};


#endif

/* FORMAT HASH cbe4715796f7bc35b31dd325ed2c4e76 */

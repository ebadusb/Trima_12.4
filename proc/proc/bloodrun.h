/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      bloodrun.h
 *
 * ABSTRACT:   This state controls the trima actions during the collection phases
 *             of the run.
 *
 */


#ifndef BLOODRUN_H  //  Prevents multiple inclusions.
#define BLOODRUN_H

#include "stateabs.h"
#include "rbcdetectoryield.h"
#include "timermessage.h"
#include "trimamessages.h"

class BloodRun
   : public StateAbs
{
   DECLARE_STATE(BloodRun);
public:

   // Default constructor
   BloodRun();

   // Copy constructor
   BloodRun(const BloodRun& state);

   // Default destructor
   virtual ~BloodRun();

   //
   // Send the blood run state message
   void bloodRunState ();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback function for rinseback status
   //
   void rinsebackStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new BloodRun(*this)); }

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
   // Function which delays the AC pump at the return
   //  to draw cycle switches
   //
   void controlACPump ();

   //
   // Callback function which starts the AC pump after the
   //  timeout
   //
   void startACPump ();

   //
   // Reset the state upon termination
   //
   virtual void reset ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const BloodRun& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Periodic state notification message
   //
   TimerMessage _PeriodicStateMsg;

   //
   // Rinseback status message ...
   //
   RinsebackStatusMsg _RinsebackStatusMsg;

   //
   // Request for the orders to be sent
   //
   SendOrdersMsg _SendOrdersRequestMsg;

   //
   // Timer to delay the AC pump on cycle switches
   //
   TimerMessage _ACPumpDelayTimer;

   //
   // RBC Detector Yield integrator object
   //
   RBCDetectorYield _RBCDetectorYield;

   //
   // Previous cycle enum
   //
   int _PrevCycle;

   //
   // Saved value for the AC Commanded speed
   //
   float _SavedACPumpCommand;


};


#endif

/* FORMAT HASH 6403abedd77fb1b377ac969fd15ca364 */

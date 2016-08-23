/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      EvacuateSetValves.h
 *
 */


#ifndef EVACUATESETVALVES_H  //  Prevents multiple inclusions.
#define EVACUATESETVALVES_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"


// Class Declaration
class EvacuateSetValves
   : public StateAbs
{
   DECLARE_STATE(EvacuateSetValves);
public:

   // Default constructor
   EvacuateSetValves();

   // Copy constructor
   EvacuateSetValves(const EvacuateSetValves& state);

   // Default destructor
   virtual ~EvacuateSetValves();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback function for valve timer ...
   //
   virtual void valvesTimedOut ();

   //
   // Callback function for startup delay timer
   //
   virtual void delayComplete ();

   virtual StateAbs* clone () const { return ( new EvacuateSetValves(*this)); }

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
   // Command all valves to the desired position ...
   //
   void commandValves ();
   void setRBCValve ();
   bool checkRBCValve ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const EvacuateSetValves& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Timer to ensure the valves move within a specified time
   //
   TimerMessage _ValveTimer;

   //
   // Timer to provide pressure equalization delay (set by evacuation_startup_delay)
   //
   TimerMessage _DelayTimer;
   int          _DelayComplete;

   //
   // Valve Alarms
   //
   anAlarmMsg _RBCAlarm;
   anAlarmMsg _PlasmaAlarm;
   anAlarmMsg _CollectAlarm;
};


#endif

/* FORMAT HASH 7cb7d3a003c234b6de6a0571a9e0dae2 */

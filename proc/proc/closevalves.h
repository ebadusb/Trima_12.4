/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      closevalves.h
 *
 */


#ifndef CLOSEVALVES_H  //  Prevents multiple inclusions.
#define CLOSEVALVES_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"


// Class Declaration
class CloseValves
   : public StateAbs
{
   DECLARE_STATE(CloseValves);
public:

   // Default constructor
   CloseValves();

   // Copy constructor
   CloseValves(const CloseValves& state);

   // Default destructor
   virtual ~CloseValves();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback function for valve timer ...
   //
   virtual void valvesTimedOut ();

   virtual StateAbs* clone () const { return ( new CloseValves(*this)); }

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
   // Command all valves to return ...
   //
   void commandToReturn ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const CloseValves& s);

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
   // Valve Alarms
   //
   anAlarmMsg _RBCAlarm;
   anAlarmMsg _PlasmaAlarm;
   anAlarmMsg _CollectAlarm;
};


#endif

/* FORMAT HASH 8d606b5010c4d6b513446de9986373c0 */

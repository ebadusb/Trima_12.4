/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      openvalves.h
 *
 */


#ifndef OPENVALVES_H  //  Prevents multiple inclusions.
#define OPENVALVES_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"


// Class Declaration
class OpenValves
   : public StateAbs
{
   DECLARE_STATE(OpenValves);
public:

   // Default constructor
   OpenValves();

   // Copy constructor
   OpenValves(const OpenValves& state);

   // Default destructor
   virtual ~OpenValves();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback function for valve timer ...
   //
   virtual void valvesTimedOut ();

   virtual StateAbs* clone () const { return ( new OpenValves(*this)); }

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
   // Command all valves to open ...
   //
   void commandToOpen ();

   //
   // Function to reset the flags
   //
   virtual void reset ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const OpenValves& s);

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

   //
   // Flag to signify the valves are open
   //
   int _ValvesSeenOpen;
};


#endif

/* FORMAT HASH 3caefcfb4ef23bb8bf72add0d55bd093 */

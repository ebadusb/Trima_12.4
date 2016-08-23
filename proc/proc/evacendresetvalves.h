/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      EvacEndResetValves.h
 *
 */


#ifndef EVACENDRESETVALVES_H  //  Prevents multiple inclusions.
#define EVACENDRESETVALVES_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"


// Class Declaration
class EvacEndResetValves
   : public StateAbs
{
   DECLARE_STATE(EvacEndResetValves);
public:

   // Default constructor
   EvacEndResetValves();

   // Copy constructor
   EvacEndResetValves(const EvacEndResetValves& state);

   // Default destructor
   virtual ~EvacEndResetValves();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback function for valve timer ...
   //
   virtual void valvesTimedOut ();

   virtual StateAbs* clone () const { return ( new EvacEndResetValves(*this)); }

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

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const EvacEndResetValves& s);

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


#endif // ifndef EVACENDRESETVALVES_H

/* FORMAT HASH dfc485c491c03d781721a113bfdd2422 */

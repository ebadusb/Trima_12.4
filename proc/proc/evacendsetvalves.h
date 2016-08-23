/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      EvacEndSetValves.h
 *
 */


#ifndef EVACENDSETVALVES_H  //  Prevents multiple inclusions.
#define EVACENDSETVALVES_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"


// Class Declaration
class EvacEndSetValves
   : public StateAbs
{
   DECLARE_STATE(EvacEndSetValves);
public:

   // Default constructor
   EvacEndSetValves();

   // Copy constructor
   EvacEndSetValves(const EvacEndSetValves& state);

   // Default destructor
   virtual ~EvacEndSetValves();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Timer callback functions
   //
   virtual void valvesTimedOut ();
   virtual void valveDwellTimerCompleted ();

   virtual StateAbs* clone () const { return ( new EvacEndSetValves(*this)); }

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
   virtual void copyOver (const EvacEndSetValves& s);

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
   // Timer to control valve open time (set in cobe_config)
   //
   TimerMessage _ValveDwellTimer;
   int          _ValveDwellTimerStarted;
   int          _ValveDwellCompleted;

   //
   // Valve Alarms
   //
   anAlarmMsg _RBCAlarm;
   anAlarmMsg _PlasmaAlarm;
   anAlarmMsg _CollectAlarm;
};


#endif // ifndef EVACENDSETVALVES_H

/* FORMAT HASH 443d3095b3b8a0cd805adb4f6a678e01 */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      EvacEndSetValves.h
 *
 */


#ifndef EVACENDSETVALVESAIRMITIGATION_H  //  Prevents multiple inclusions.
#define EVACENDSETVALVESAIRMITIGATION_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"


// Class Declaration
class EvacEndSetValvesARM
   : public StateAbs
{
   DECLARE_STATE(EvacEndSetValvesARM);
public:

   // Default constructor
   EvacEndSetValvesARM();

   // Copy constructor
   EvacEndSetValvesARM(const EvacEndSetValvesARM& state);

   // Default destructor
   virtual ~EvacEndSetValvesARM();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Timer callback functions
   //
   virtual void valvesTimedOut ();
   virtual void valveDwellTimerCompleted ();
   virtual void collectValveDwellTimerCompleted ();

   virtual StateAbs* clone () const { return ( new EvacEndSetValvesARM(*this)); }

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
   virtual void copyOver (const EvacEndSetValvesARM& s);

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

   TimerMessage _CollectValveDwellTimer;
   int          _CollectValveDwellTimerStarted;
   int          _CollectValveDwellCompleted;

   //
   // Valve Alarms
   //
   anAlarmMsg _RBCAlarm;
   anAlarmMsg _PlasmaAlarm;
   anAlarmMsg _CollectAlarm;

   bool       _collectSet;


};


#endif // ifndef EVACENDSETVALVES_H

/* FORMAT HASH 82d984cfe8561dce6624e2a129a86fe9 */

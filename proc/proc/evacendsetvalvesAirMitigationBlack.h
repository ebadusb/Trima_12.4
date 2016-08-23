/*
 *
 */


#ifndef EVACENDSETVALVESAIRMITIGATIONBLACK_H  //  Prevents multiple inclusions.
#define EVACENDSETVALVESAIRMITIGATIONBLACK_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"


// Class Declaration
class EvacEndSetValvesARMBlack
   : public StateAbs
{
   DECLARE_STATE(EvacEndSetValvesARMBlack);
public:

   // Default constructor
   EvacEndSetValvesARMBlack();

   // Copy constructor
   EvacEndSetValvesARMBlack(const EvacEndSetValvesARMBlack& state);

   // Default destructor
   virtual ~EvacEndSetValvesARMBlack();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Timer callback functions
   //
   virtual void valvesTimedOut ();
   virtual void valveDwellTimerCompleted ();
   virtual void plsValveDwellTimerCompleted ();

   virtual StateAbs* clone () const { return ( new EvacEndSetValvesARMBlack(*this)); }

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
   virtual void copyOver (const EvacEndSetValvesARMBlack& s);

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

   TimerMessage _plsValveDwellTimer;
   int          _plsValveDwellTimerStarted;
   int          _plsValveDwellCompleted;

   //
   // Valve Alarms
   //
   anAlarmMsg _RBCAlarm;
   anAlarmMsg _PlasmaAlarm;
   anAlarmMsg _CollectAlarm;

   bool       _plsSet;
   bool       _isBalckStamp;

};


#endif // ifndef EVACENDSETVALVES_H

/* FORMAT HASH 812967bf8b022ec58a3c3526abe3c886 */

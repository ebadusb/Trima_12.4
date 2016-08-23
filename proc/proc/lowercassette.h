/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      lowercassette.h
 *
 */


#ifndef LOWERCASSETTE_H  //  Prevents multiple inclusions.
#define LOWERCASSETTE_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"

// Class Declaration
class LowerCassette
   : public StateAbs
{
   DECLARE_STATE(LowerCassette);
public:

   // Default constructor
   LowerCassette();

   // Default destructor
   virtual ~LowerCassette();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback for the cassette taking too long
   //  to lower
   //
   void lowerTooLong ();

   //
   // Callback to ensure the cassette was completely
   //  lowered
   //
   void completelyLowered ();

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
   // Function which sends the commands needed to finish
   //  this state
   //
   void startCommand ();

   //
   // Reset the state upon termination
   //
   virtual void reset ();

protected:

   //
   // Time at which the lower cassette command was given
   //
   TimerMessage _LowerTooLongTimer;

   //
   // Timer to ensure the cassette completely lowers
   //
   TimerMessage _CompletelyLoweredTimer;

   //
   // Flag to show the cassette has taken too long to
   //  get to the lower position
   //
   bool _LowerTooLong;

   //
   // Flag to show the cassette has been seen in
   //  the lower position
   //
   bool _CassetteLowered;

   //
   // Flag to show the cassette has completely
   //  lowered
   //
   bool _CassetteCompletelyLowered;

   //
   // Flag to save the state of the timer
   //
   bool _TimerStarted;

   //
   // Flag to signify the state has failed ...
   //
   bool _CassetteLowerFailure;

private:

   long _apsStartReadDelay;         // used for delay before starting APS readings - insures
                                    // any previous auto-zero has been reset before collecting readings

   bool         _apsAutoZeroComplete; // true if APS auto calibration has been completed
   long         _apsAverageCount;     // current count of APS readings for auto-cal
   float        _apsAverageTotal;     // current total of APS readings for auto-cal
   float        _apsMin;              // minimum APS reading during auto-cal
   float        _apsMax;              // maximum APS reading during auto-cal
   float        _apsZeroOffset;       // computed offset (from _apsAverageCount and _apsAverageTotal)

   float        _apsLastZeroOffset; // zero offset used for last APS auto calibration

   ALARM_VALUES _apsAlarm;
};


#endif

/* FORMAT HASH 45e2e2da0a597ed32327b881904d4c82 */

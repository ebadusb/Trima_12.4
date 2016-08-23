/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      raisecassette.h
 *
 */


#ifndef RAISECASSETTE_H  //  Prevents multiple inclusions.
#define RAISECASSETTE_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"


// Class Declaration
class RaiseCassette
   : public StateAbs
{
   DECLARE_STATE(RaiseCassette);
public:

   // Default constructor
   RaiseCassette();

   // Copy constructor
   RaiseCassette(const RaiseCassette& state);

   // Default destructor
   virtual ~RaiseCassette();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback for the raise too long timer
   //
   void raiseTooLong ();

   //
   // Callback for the cassette up timer
   //
   void cassetteClear ();

   virtual StateAbs* clone () const { return ( new RaiseCassette(*this)); }

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
   // Function which commands the cassette to raise
   //
   void startCommand ();

   //
   // Resets the state upon termination
   //
   virtual void reset ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const RaiseCassette& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Alarm set when the cassette fails to raise in time
   //
   anAlarmMsg _CassetteRaiseAlarm;

   //
   // Timer to set an alarm if the cassette takes too long to
   //  reach the raised position
   //
   TimerMessage _RaiseTooLongTimer;

   //
   // Timer to ensure the set is completely clear of the pumps ...
   //
   TimerMessage _CassetteUpTimer;

   //
   // Flag to signify the cassette is up ...
   //
   int _CassetteUpFlag;

   //
   // Flag to signify the cassette is clear ...
   //
   int _CassetteClearFlag;

   //
   // Flag to signify the state has failed ...
   //
   int _CassetteRaiseFailure;

   //
   // Flag to signify the timer was started ...
   //
   int _TimerStarted;
};


#endif

/* FORMAT HASH e0a19588633ab73a56a891b5d1fd5a4d */

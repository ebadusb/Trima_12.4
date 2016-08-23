/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      startpumps.h
 *
 */


#ifndef STARTPUMPS_H  //  Prevents multiple inclusions.
#define STARTPUMPS_H

#include "stateabs.h"
#include "timermessage.h"


// Class Declaration
class StartPumps
   : public StateAbs
{
   DECLARE_STATE(StartPumps);
public:

   // Default constructor
   StartPumps();

   // Copy constructor
   StartPumps(const StartPumps& state);

   // Default destructor
   virtual ~StartPumps();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback for the start pumps timer ...
   //
   void pumpsNotStarted ();

   virtual StateAbs* clone () const { return ( new StartPumps(*this)); }

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
   // Function to command the pumps to move
   //
   virtual void startPumps ();

   //
   // Function to stop the pumps
   //
   virtual void stopPumps ();

   //
   // Reset the state upon termination
   //
   virtual void reset ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const StartPumps& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Flag to signify we are finished
   //
   bool _Finished;

   //
   // Timer to sense a failure to start the pumps
   //
   TimerMessage _StartPumpsTimer;

   //
   // Flag to signify the timer is running
   //
   int _TimerStarted;

   //
   //
   //  Number of pumps started
   //
   int _nPumps;

};


#endif

/* FORMAT HASH de33d047db43fbbd42695d40b26b9a00 */

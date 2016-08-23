/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      stoppumps.h
 *
 */


#ifndef STOPPUMPS_H  //  Prevents multiple inclusions.
#define STOPPUMPS_H

#include "stateabs.h"
#include "timermessage.h"

#include "an_alarm.h"

// Class Declaration
class StopPumps
   : public StateAbs
{
   DECLARE_STATE(StopPumps);
public:

   // Default constructor
   StopPumps();

   // Copy constructor
   StopPumps(const StopPumps& state);

   // Default destructor
   virtual ~StopPumps();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback for the stop pumps timer ...
   //
   void pumpsNotStopped ();

   virtual StateAbs* clone () const { return ( new StopPumps(*this)); }

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
   // Reset the state upon termination
   //
   virtual void reset ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const StopPumps& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Flag for the finished condition
   //
   int _Finished;

   //
   // Timer to sense a failure to stop the pumps
   //
   TimerMessage _StopPumpsTimer;

   anAlarmMsg   _CPSZeroOutsideRange;
};


#endif

/* FORMAT HASH 8cc6da24f1fe4bfd471ebbe5e7df7df0 */

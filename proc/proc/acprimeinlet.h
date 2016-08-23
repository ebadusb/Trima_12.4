/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      acprimeinlet.h
 *
 */


#ifndef ACPRIMEINLET_H  //  Prevents multiple inclusions.
#define ACPRIMEINLET_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timekeeper.h"


// Class Declaration
class ACPrimeInlet
   : public StateAbs
{
   DECLARE_STATE(ACPrimeInlet);
public:

   // Default constructor
   ACPrimeInlet();

   // Copy constructor
   ACPrimeInlet(const ACPrimeInlet& state);

   // Default destructor
   virtual ~ACPrimeInlet();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback for the inlet prime timer
   //
   virtual void primeTooLong ();

   virtual StateAbs* clone () const { return ( new ACPrimeInlet(*this)); }

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
   // Function to reset the accumulated volumes
   //
   virtual void resetVolumes ();

   //
   // Function to save the initial volumes
   //
   virtual void saveValues ();

   //
   // Function to command the pump movement
   //
   virtual void startPumps ();
   virtual void startPumpsAt30 ();
   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const ACPrimeInlet& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Time accumulator for when the timer is running
   //
   TimeKeeper _AccumulatedTime;

   TimeKeeper _ClearLineTimer;
   //
   // AC Prime Alarms
   //
   anAlarmMsg _PrimeFailureAlarm;
   anAlarmMsg _ACLevelAlarm;

   //
   // Volumes accumulated throughout the state
   //
   float _ACVolume;

   //
   // Flag to save the first occurrence of AC seen at the
   //  sensor
   //
   int _ACSeen;

   //
   // Counter to accumulate the number of prime attempts
   //  taken to prime the set ...
   //
   int  _PrimeAttempts;

   bool runPauseTimer;
   bool waitDone;


};


#endif

/* FORMAT HASH 9acaef4e697f7bacb280d964faf0672c */

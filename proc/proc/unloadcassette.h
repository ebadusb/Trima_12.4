/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      unloadcassette.h
 *
 */


#ifndef UNLOADCASSETTE_H  //  Prevents multiple inclusions.
#define UNLOADCASSETTE_H

#include "stateabs.h"
#include "trimamessages.h"
#include "alarms.h"

class AlarmMonitorBase;

// Class Declaration
class UnloadCassette
   : public StateAbs
{
   DECLARE_STATE(UnloadCassette);
public:

   // Default constructor
   UnloadCassette();

   // Copy constructor
   UnloadCassette(const UnloadCassette& state);

   // Default destructor
   virtual ~UnloadCassette();

   virtual StateAbs* clone () const { return ( new UnloadCassette(*this)); }

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
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const UnloadCassette& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

   //
   //   Callback for pending alarm message
   //
   void setPendingAlarm ();

protected:
   //
   //
   //   Alarm to display after cassette raise complete
   //
   PendingCassetteAlarmMsg _PendingAlarmMsg;
   ALARM_VALUES            _PendingAlarm;

};


#endif

/* FORMAT HASH 0d8409ae410355de890ea66f918fb3f7 */

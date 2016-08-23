/*
 * Copyright (c) 2012 by Terumo BCT, Inc.  All rights reserved.
 *
 * TITLE:      WBCevent_Monitor.h
 *
 */


#ifndef WBCEVENT_MONITOR_H
#define WBCEVENT_MONITOR_H

#include "aPQMonitor.h"
#include "states.h"
#include "objdictionary.h"

class WBCEvent_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(WBCEvent_Monitor);

public:

   // Constructor, called at the time the "states" file is read in.
   WBCEvent_Monitor();

   // Destructor
   virtual ~WBCEvent_Monitor();

   // Monitor function, called AFTER hardware status is received, the values
   // are processed by the methods of the state to which the monitor is attached
   // in the "states" file and all of its substates (if any),
   // and no transition out of the state results.
   virtual void Monitor ();

   // Initialization, called by the state machine AFTER the Initialization()
   // method of the state to which the monitor is attached in the "states" file.
   virtual void Initialize ();

   // Function to enable monitoring, called by the state machine AFTER the preEnter()
   // method of the state to which the monitor is attached in the "states" file.
   virtual void enable ();

   // Function to disable monitoring, called by the state machine BEFORE the postExit()
   // method of the state to which the monitor is attached in the "states" file.
   virtual void disable ();

private:


};


#endif

/* FORMAT HASH 1fc8deb4e592677b7f308cffded714eb */

/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PIR_Monitor.h
 *
 * ABSTRACT:  This is the include file for the PIR monitor.
 *
 */


#ifndef PIR_MONITOR_H
#define PIR_MONITOR_H

#include "aPQMonitor.h"
#include "states.h"
#include "objdictionary.h"

class PIR_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(PIR_Monitor);

public:

   // Constructor, called at the time the "states" file is read in.
   PIR_Monitor();

   // Destructor
   virtual ~PIR_Monitor();

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

   State_names _previousSubstate;

};


#endif

/* FORMAT HASH f1ade305aa0e810118370341bc9e802a */

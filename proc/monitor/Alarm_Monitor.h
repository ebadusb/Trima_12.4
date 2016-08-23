/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      Alarm_Monitor.h
 *
 * ABSTRACT:  This is the include file for the Alarm monitor.
 *
 */


#ifndef Alarm_MONITOR_H
#define Alarm_MONITOR_H

#include "aPQMonitor.h"
#include "objdictionary.h"
#include "trimamessages.h"
#include "alarmentry.h"          // Include class for alarm msgs

class Alarm_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(Alarm_Monitor);

public:

   // Constructor, called at the time the "states" file is read in.
   Alarm_Monitor();

   // Destructor
   virtual ~Alarm_Monitor();

   // Monitor function, called AFTER hardware status is received, the values
   // are processed by the methods of the state to which the monitor is attached
   // in the "states" file and all of its substates (if any),
   // and no transition out of the state results.
   virtual void Monitor ();

   // Notify function, called whenever a AlarmActiveMessage is received.
   void Notify ();

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

   AlarmActiveMessage _AlarmMsg;
};


#endif

/* FORMAT HASH b6eaf630942a3cdc8cbe8a5cac0feee6 */

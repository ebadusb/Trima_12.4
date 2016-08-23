/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      SetID_Monitor.h
 *
 * ABSTRACT:  This is the include file for the Set ID monitor.
 *
 */


#ifndef SetID_MONITOR_H
#define SetID_MONITOR_H

#include "aPQMonitor.h"
#include "objdictionary.h"
#include "trimamessages.h"

class SetID_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(SetID_Monitor);

public:

   // Constructor
   SetID_Monitor();

   // Destructor
   virtual ~SetID_Monitor();

   // Monitor function, called AFTER hardware status is received, the values
   // are processed by the methods of the state to which the monitor is attached
   // in the "states" file and all of its substates (if any),
   // and no transition out of the state results.
   virtual void Monitor ();

   // Notify function, called whenever a MSG_TYPE is received.
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

   //
   // Message used to return the set identified
   //  from GUI
   //
   OperatorSetIdentificationMsg _IdentifySetMsg;
};


#endif

/* FORMAT HASH 4ab44c2216d9df3b8e90a9360cbdc6ad */

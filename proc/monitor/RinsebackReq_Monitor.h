/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      RinsebackReq_Monitor.h
 *
 * ABSTRACT:  This is the include file for the Rinseback Request monitor.
 *
 */


#ifndef RinsebackReq_MONITOR_H
#define RinsebackReq_MONITOR_H

#include "aPQMonitor.h"
#include "objdictionary.h"
#include "trimamessages.h"

class RinsebackReq_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(RinsebackReq_Monitor);

public:

   // Constructor, called at the time the "states" file is read in.
   RinsebackReq_Monitor();

   // Destructor
   virtual ~RinsebackReq_Monitor();

   // Monitor function, called AFTER hardware status is received, the values
   // are processed by the methods of the state to which the monitor is attached
   // in the "states" file and all of its substates (if any),
   // and no transition out of the state results.
   virtual void Monitor ();

   // Notify function, called whenever a RinsebackRequestMsg is received from GUI,
   // indicating that the operator has pressed the button requesting Rinseback.
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

   RinsebackRequestMsg _RinsebackRequestMsg;
};


#endif

/* FORMAT HASH b6b3e81979d132e2c11f6ef78d629492 */

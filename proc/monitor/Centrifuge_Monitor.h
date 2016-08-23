/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      Centrifuge_Monitor.h
 *
 * ABSTRACT:  This is the include file for the Centrifuge monitor.
 *
 */


#ifndef Centrifuge_MONITOR_H
#define Centrifuge_MONITOR_H

#include "aPQMonitor.h"
#include "states.h"
#include "objdictionary.h"
#include "trimamessages.h"

class Centrifuge_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(Centrifuge_Monitor);

public:

   // Constructor, called at the time the "states" file is read in.
   Centrifuge_Monitor();

   // Destructor
   virtual ~Centrifuge_Monitor();

   // Monitor function, called AFTER hardware status is received, the values
   // are processed by the methods of the state to which the monitor is attached
   // in the "states" file and all of its substates (if any),
   // and no transition out of the state results.
   virtual void Monitor ();

   // Notify function, called whenever a CentrifugeStopMsg is received.
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

   CentrifugeStopMsg _CentrifugeStopMsg;

   float             _CentrifugeTrigger;

   bool              _alreadyFlaggedCentrifugeStop;
};


#endif

/* FORMAT HASH 0643c7afccdc6a54b6d15b5f320e826c */

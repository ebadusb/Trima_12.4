/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      SpillReq_Monitor.h
 *
 * ABSTRACT:  This is the include file for the Spillover recovery Request monitor.
 *
 */


#ifndef SpillReq_MONITOR_H
#define SpillReq_MONITOR_H

#include "aPQMonitor.h"
#include "objdictionary.h"
#include "trimamessages.h"

class SpillReq_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(SpillReq_Monitor);

public:

   // Constructor, called at the time the "states" file is read in.
   SpillReq_Monitor();

   // Destructor
   virtual ~SpillReq_Monitor();

   // Monitor function, called AFTER hardware status is received, the values
   // are processed by the methods of the state to which the monitor is attached
   // in the "states" file and all of its substates (if any),
   // and no transition out of the state results.
   virtual void Monitor ();

   // Notify function, called whenever a SpilloverRecoveryMessage is received from GUI,
   // indicating that the operator has pressed the "Spillover Recovery" button.
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

   SpilloverRecoveryMsg _SpillReqMsg;

   bool                 _alreadyLoggedSpilloverRecovery,
                        _alreadyLoggedPltContamination;
};


#endif

/* FORMAT HASH 044be92f9155d81b56b0df0b91232035 */

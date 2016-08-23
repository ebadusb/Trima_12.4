/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      DeltaHCT_Monitor.h
 *
 * ABSTRACT:  This is the include file for the Donor Delta HCT monitor.
 *
 */


#ifndef DELTA_HCT_MONITOR_H
#define DELTA_HCT_MONITOR_H

#include "aPQMonitor.h"
#include "objdictionary.h"
#include "cobeconfig.h"

class DeltaHCT_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(DeltaHCT_Monitor);

public:

   // Constructor, called at the time the "states" file is read in.
   DeltaHCT_Monitor();

   // Destructor
   virtual ~DeltaHCT_Monitor();

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

   float _previousDonorHCT;
   float _hctTrigger;
};


#endif

/* FORMAT HASH 622ad2583c8f4fe18328152840a82170 */

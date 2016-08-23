/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      RBCDetCal_Monitor.h
 *
 * ABSTRACT:  This is the include file for the RCB Detector Calibration monitor.
 *
 */


#ifndef RBCDETCAL_MONITOR_H
#define RBCDETCAL_MONITOR_H

#include "aPQMonitor.h"
#include "objdictionary.h"

class RBCDetCal_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(RBCDetCal_Monitor);

public:

   // Constructor
   RBCDetCal_Monitor();

   // Destructor
   virtual ~RBCDetCal_Monitor();

   // Monitor function, called AFTER hardware status is received, the values
   // are processed by the methods of the state to which the monitor is attached
   // in the "states" file and all of its substates (if any),
   // and no transition out of the state results.
   virtual void Monitor ();

   // Function to enable monitoring, called by the state machine AFTER the preEnter()
   // method of the state to which the monitor is attached in the "states" file.
   virtual void enable ();

   // Function to disable monitoring, called by the state machine BEFORE the postExit()
   // method of the state to which the monitor is attached in the "states" file.
   virtual void disable ();

private:

};


#endif

/* FORMAT HASH 2cb09f31bb5b330982ad517069d2e00c */

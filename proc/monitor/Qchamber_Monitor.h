/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      Qchamber_Monitor.h
 *
 * ABSTRACT:  This is the include file for the LRS chamber flow monitor.
 *
 */


#ifndef Qchamber_MONITOR_H
#define Qchamber_MONITOR_H

#include "aPQMonitor.h"
#include "objdictionary.h"

class Qchamber_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(Qchamber_Monitor);

public:

   // Constructor
   Qchamber_Monitor();

   // Destructor
   virtual ~Qchamber_Monitor();

   // Monitor function, called AFTER hardware status is received, the values
   // are processed by the methods of the state to which the monitor is attached
   // in the "states" file and all of its substates (if any),
   // and no transition out of the state results.
   virtual void Monitor ();

   float Qchamber_Monitor::scaleQch1 (const float Qch1);
};


#endif

/* FORMAT HASH 1006f77c22dd4d3ab228745127b347e4 */

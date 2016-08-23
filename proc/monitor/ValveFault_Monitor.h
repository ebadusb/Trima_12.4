/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ValveFault_Monitor.h
 *
 * ABSTRACT:  This is the include file for the valve fault monitor.
 *
 */


#ifndef ValveFault_MONITOR_H
#define ValveFault_MONITOR_H

#include "aPQMonitor.h"
#include "objdictionary.h"

class ValveFault_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(ValveFault_Monitor);

public:

   // Constructor
   ValveFault_Monitor();

   // Destructor
   virtual ~ValveFault_Monitor();

   // Monitor function, called AFTER hardware status is received, the values
   // are processed by the methods of the state to which the monitor is attached
   // in the "states" file and all of its substates (if any),
   // and no transition out of the state results.
   virtual void Monitor ();

private:

};


#endif

/* FORMAT HASH 40547f7b32165fe39c9d02be8bd494af */

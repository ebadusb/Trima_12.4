/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      Purge_Monitor.h
 *
 * ABSTRACT:  This is the include file for the monitor template.
 *
 */


#ifndef PURGE_MONITOR_H
#define PURGE_MONITOR_H

#include "aPQMonitor.h"
#include "objdictionary.h"
#include "cobeconfig.h"

class Purge_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(Purge_Monitor);

public:

   // Constructor
   Purge_Monitor();

   // Destructor
   virtual ~Purge_Monitor();

   // Monitor function, called AFTER hardware status is received, the values
   // are processed by the methods of the state to which the monitor is attached
   // in the "states" file and all of its substates (if any),
   // and no transition out of the state results.
   virtual void Monitor ();
};


#endif

/* FORMAT HASH 4369bcc46c64374600d048e4119d9603 */

/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      Collection_Monitor.h
 *
 * ABSTRACT:  This is the include file for the Collection monitor.
 *
 */


#ifndef Collection_MONITOR_H
#define Collection_MONITOR_H

#include "aPQMonitor.h"
#include "objdictionary.h"

class Collection_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(Collection_Monitor);

public:

   // Constructor
   Collection_Monitor();

   // Destructor
   virtual ~Collection_Monitor();

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

};


#endif

/* FORMAT HASH d046ab4315513fced3574d9626a9fc41 */

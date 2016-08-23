/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      Purge_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the monitor template.
 *
 */

#include "Purge_Monitor.h"
#include "procdata.h"

// Define this class to the object dictionary
DEFINE_OBJ(Purge_Monitor);

// Constructor
Purge_Monitor::Purge_Monitor()
{}

// Destructor
Purge_Monitor::~Purge_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void Purge_Monitor::Monitor ()
{
   ProcData pd;

   // PURGE MONITORING CODE HERE
}

/* FORMAT HASH d57b60504d92ae3e7f2eb0705bc8165d */

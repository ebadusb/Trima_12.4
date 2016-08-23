/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      DonorHCT_Monitor.h
 *
 * ABSTRACT:  This is the include file for the Donor HCT monitor.
 *
 */


#ifndef DONOR_HCT_MONITOR_H
#define DONOR_HCT_MONITOR_H

#include "aPQMonitor.h"
#include "objdictionary.h"
#include "cobeconfig.h"

class DonorHCT_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(DonorHCT_Monitor);

public:

   // Constructor
   DonorHCT_Monitor();

   // Destructor
   virtual ~DonorHCT_Monitor();

   // Monitor function, called AFTER hardware status is received, the values
   // are processed by the methods of the state to which the monitor is attached
   // in the "states" file and all of its substates (if any),
   // and no transition out of the state results.
   void Monitor ();
};


#endif

/* FORMAT HASH 017ee58b50c22af8b4a8b02beea9cc9a */

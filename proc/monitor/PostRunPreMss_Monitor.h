/*
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PostRunPreMss_Monitor.h
 *
 * ABSTRACT:  This is the include file for the Post Run monitor.
 *             this monitor is run for pre-MSS flag evaluation.
 *
 */


#ifndef PostRunPreMss_Monitor_H
#define PostRunPreMss_Monitor_H

#include "aPQMonitor.h"
#include "objdictionary.h"


class PostRunPreMss_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(PostRunPreMss_Monitor);

public:

   // Constructor
   PostRunPreMss_Monitor();

   // Destructor
   virtual ~PostRunPreMss_Monitor();

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

   // Flags
   bool _plateletsWereCollected;
   bool _rbcsWereCollected;
   bool _pasSet;
   bool _rasSet;

   // Product totals
   float              _VplateletTarget; // Plt Target volume before solution addn
   float              _VcolPlateletBag; // Plt Bag volume before solution addn
   float              _PLTfactor;

   ProcData           _pd;
   ProductQualityData _pqData;

   // Private method that performs product verification checks for the end of platelet collection.
   void SetPlateletFlags ();
   void ClearPlateletFlags ();

   void SetRbcFlags ();
   void ClearRbcFlags ();

   // Sets Run Ended Early flags
   void SetRunEndedEarlyFlags ();
};


#endif

/* FORMAT HASH ff95f7dee55ebea00fc6db2e35bd9447 */

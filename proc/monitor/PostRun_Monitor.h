/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PostRun_Monitor.h
 *
 * ABSTRACT:  This is the include file for the Post Run monitor.
 *
 */


#ifndef PostRun_MONITOR_H
#define PostRun_MONITOR_H

#include "aPQMonitor.h"
#include "objdictionary.h"


class PostRun_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(PostRun_Monitor);

public:

   // Constructor
   PostRun_Monitor();

   // Destructor
   virtual ~PostRun_Monitor();

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
   bool _plasmaWasCollected;
   bool _rbcsWereCollected;

   // Product totals
   float _VplateletTarget;           // Plt Target volume before solution addn
   float _VcolPlateletBag;           // Plt Bag volume before solution addn
   float _VcolPltBagWithSoln;        // Plt Bag volume after solution addn

   float _VplasmaTarget;
   float _VcolPlasmaBag;

   float _PLTfactor;

   float _VrbcTarget;
   float _VcolRBCBag;


   ProcData           _pd;
   ProductQualityData _pqData;

   // Private method that performs product verification checks for the end of platelet collection.
   void SetPlateletFlags ();
   void ClearPlateletFlags ();

   // Private method that performs product verification checks for the end of plasma collection.
   void SetPlasmaFlags ();
   void ClearPlasmaFlags ();

   // Private method that performs product verification checks for the end of RBC collection.
   void SetRbcFlags ();
   void ClearRbcFlags ();

   // Sets Run Ended Early flags
   void SetRunEndedEarlyFlags ();

   // Function to fill the Stats with the current run info
   void updateStats ();

};


#endif

/* FORMAT HASH 2be70bdf9a1216d95d49d9ecb8f3748d */

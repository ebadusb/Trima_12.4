/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      EndOfRun_Monitor.h
 *
 * ABSTRACT:  This is the include file for the End of Run "monitor".
 *
 */


#ifndef EndOfRun_MONITOR_H
#define EndOfRun_MONITOR_H

#include "aPQMonitor.h"
#include "objdictionary.h"


class EndOfRun_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(EndOfRun_Monitor);

public:

   // Constructor
   EndOfRun_Monitor();

   // Destructor
   virtual ~EndOfRun_Monitor();

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
   float    _VplateletTarget;        // Plt Target volume before solution addn
   float    _VcolPlateletBag;        // Plt Bag volume before solution addn

   float    _VplasmaTarget;
   float    _VcolPlasmaBag;

   float    _PLTfactor;

   float    _VrbcTarget;
   float    _VcolRBCBag;

   ProcData _pd;

private:
   void SetPlateletFlags ();

   void SetPlasmaFlags ();

   void SetRBCFlags ();
};


#endif

/* FORMAT HASH 2b656cd60d8fe7c62c9f8df8047c0888 */

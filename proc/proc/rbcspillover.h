/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      rbcspillover.h
 *
 */

#ifndef _RBCSPILLOVER_H_
#define _RBCSPILLOVER_H_

#include "recoveryabs.h"
#include "trimamessages.h"

class RBCSpillover
   : public RecoveryAbs
{
   DECLARE_OBJ(RBCSpillover);
public:

   //
   // Constructor
   RBCSpillover();

   //
   // Copy constructor
   RBCSpillover(const RBCSpillover& rec);

   //
   // Destructor
   virtual ~RBCSpillover();

   //
   // Initialize function
   //
   virtual int init ();

   //
   // function which does the recovery, or at least a step of
   //  the recovery.  When the recovery is complete, the
   //  recoveryNeeded function will reflect the information.
   //
   virtual int doRecovery ();

   //
   // function to control whether this recovery may be performed
   //  given the current system state and data
   //
   virtual int mayEnter () const;

   //
   // Function to reset the spillover if it gets preempted by another
   //  recovery
   //
   virtual void reset ();

   //
   // Callback for the spillover request message
   //
   void spilloverRequest ();

   //
   // Callback for the spillover detected message
   //
   void spilloverDetected ();

   //
   // Callback for the line cleared message
   //
   void lineCleared ();

protected:

   //
   // Function which will control whether this recovery has detected
   //  that it is needed give the current system state
   //
   virtual int shouldRecover ();

   //
   // Function which sends the spillover request message
   //
   void requestSpillover ();

   //
   // Function to copy the dynamic data for the object
   //
   void copyOver (const RBCSpillover& rec);

   //
   // Function to delete the objects dynamic memory
   //
   void cleanup ();

protected:

   //
   // Spillover request message
   //
   SpilloverRecoveryMsg _SpilloverReqMsg;

   //
   // Spillover detected message
   //
   SpilloverDetectedMsg _SpilloverDetMsg;

   //
   // Line cleared message
   //
   ClearLineCompleteMsg _LineClearedMsg;

   //
   // Flag to signify a spillover recovery has been requested
   //
   int _SpilloverRequested;

   //
   // Flag to signify a spillover has been detected
   //
   int _SpilloverDetected;

   //
   // Flag to signify a spillover recovery has been started
   //
   int _SpilloverStarted;

   //
   // Flag to signify the line clear recovery has finished
   //
   int _LineCleared;

};

#endif

/* FORMAT HASH 6797fdb00a202e34b35266b650396948 */

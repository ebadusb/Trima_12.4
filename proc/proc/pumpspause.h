/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pumpspause.h
 *
 */

#ifndef _PUMPSPAUSE_H_
#define _PUMPSPAUSE_H_

#include "recoveryabs.h"
#include "timermessage.h"

class PumpsPause
   : public RecoveryAbs
{
   DECLARE_OBJ(PumpsPause);
public:

   //
   // Constructor
   PumpsPause();

   //
   // Copy constructor
   PumpsPause(const PumpsPause& rec);

   //
   // Destructor
   virtual ~PumpsPause();

   //
   // Initialize function
   //
   virtual int init ();

   //
   // Callback function for the recovery trigger timer
   //
   void pumpsPaused ();

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
   // Function to reset the flags
   //
   virtual void reset ();

protected:

   //
   // Function which will control whether this recovery has detected
   //  that it is needed give the current system state
   //
   virtual int shouldRecover ();

   //
   // Function to copy the dynamic data for the object
   //
   void copyOver (const PumpsPause& rec);

   //
   // Function to delete the objects dynamic memory
   //
   void cleanup ();

protected:

   //
   // Timer to trigger the recovery
   //
   TimerMessage _RecoveryTimer;
   //
   // Flag to signify we have already started the timer
   //
   int _TimerStarted;

   //
   // Flag to signify the recovery should be performed
   //
   int _PerformRecovery;

   //
   // Flag to signify the interface has been reestablished ...
   //
   int _InterfaceEstablished;

   //
   // Total inlet volume processed during this recovery ...
   //
   float _InletVolume;

};

#endif

/* FORMAT HASH cb95ea2b05613d6d817e0bc6a136cc30 */

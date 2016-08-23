/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      SalineDrawRecovery.h
 *
 */

#ifndef _SALINEDRAWRECOVERY_H_
#define _SALINEDRAWRECOVERY_H_

#include "recoveryabs.h"
#include "trimamessages.h"

class SalineDrawRecovery
   : public RecoveryAbs
{
   DECLARE_OBJ(SalineDrawRecovery);
public:

   //
   // Constructor
   SalineDrawRecovery();

   //
   // Copy constructor
   SalineDrawRecovery(const SalineDrawRecovery& rec);

   //
   // Destructor
   virtual ~SalineDrawRecovery();

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
   // Callback for the recovery request message
   //
   virtual void recoveryRequest ();

protected:

   //
   // Function which will control whether this recovery has detected
   //  that it is needed give the current system state
   //
   virtual int shouldRecover ();

   //
   // Function to reset the recovery
   //
   virtual void reset ();

   //
   // Function to copy the dynamic data for the object
   //
   virtual void copyOver (const SalineDrawRecovery& rec);

   //
   // Function to delete the objects dynamic memory
   //
   virtual void cleanup ();

protected:

   //
   // Message which will start the recovery ...
   //
   SalineDrawRecoveryMsg _RecoveryReqMsg;

   //
   // Flag to signify the recovery has been requested
   //  but not yet started
   //
   int _RecoveryRequested;

   //
   // Flag to signify the recovery has been completed
   //
   int _RecoveryComplete;

   //
   // Volume delivered during recovery
   //
   float _RecoveryVolume;

};

#endif

/* FORMAT HASH a2fdb48924297d973bf4e5452e399996 */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      salinebolus.h
 *
 */

#ifndef _SALINEBOLUS_H_
#define _SALINEBOLUS_H_

#include "recoveryabs.h"
#include "trimamessages.h"

class SalineBolus
   : public RecoveryAbs
{
   DECLARE_OBJ(SalineBolus);
public:

   //
   // Constructor
   SalineBolus();

   //
   // Copy constructor
   SalineBolus(const SalineBolus& rec);

   //
   // Destructor
   virtual ~SalineBolus();

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
   void recoveryRequest ();

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
   void copyOver (const SalineBolus& rec);

   //
   // Function to delete the objects dynamic memory
   //
   void cleanup ();

protected:

   //
   // Message which will start the recovery ...
   //
   SalineBolusMsg _RecoveryReqMsg;

   //
   // Flag to signify the recovery has been requested
   //  but not yet started
   //
   int _RecoveryRequested;

   //
   // Flag to signify the recovery has been completed
   //
   int _BolusComplete;

   //
   // Volume delivered for the Bolus
   //
   float _BolusVolume;

};

#endif

/* FORMAT HASH de14ef6add3b4b5a5d63bd1406969545 */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      salineprime.h
 *
 */

#ifndef _SALINEPRIME_H_
#define _SALINEPRIME_H_

#include "recoveryabs.h"
#include "trimamessages.h"

class SalinePrime
   : public RecoveryAbs
{
   DECLARE_OBJ(SalinePrime);
public:

   //
   // Constructor
   SalinePrime();

   //
   // Copy constructor
   SalinePrime(const SalinePrime& rec);

   //
   // Destructor
   virtual ~SalinePrime();

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
   // Callback function for the rep fluid connection message
   //
   void repFluidConnected ();

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
   void copyOver (const SalinePrime& rec);

   //
   // Function to delete the objects dynamic memory
   //
   void cleanup ();

protected:

   //
   // Replacement fluid connection message status ...
   //
   RepFluidConnectStatusMsg _RepFluidConMsg;

   //
   // Flag to signify the recovery completed ...
   //
   // moved to Run cds for PFR reasons.....

   //
   // Initial Volume of Rep Fluid ...
   //
   float _ReplaceVolume;
   float _BolusVolume;
   float _CollectVolume;
   float _ReservoirVolume;
   float _CollectLineVolume;

   //
   // Volume primed so far
   //
   float _PrimeVolume;

};

#endif

/* FORMAT HASH 7f2538a07a15cf8264273050fa23a31e */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      airblock.h
 *
 */

#ifndef _AIRBLOCK_H_
#define _AIRBLOCK_H_

#include "recoveryabs.h"
#include "trimamessages.h"

class AirBlock
   : public RecoveryAbs
{
   DECLARE_OBJ(AirBlock);
public:

   //
   // Constructor
   AirBlock();

   //
   // Copy constructor
   AirBlock(const AirBlock& rec);

   //
   // Destructor
   virtual ~AirBlock();

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
   // Calback function for the air block recovery request message
   //
   void requestReceived ();

protected:

   //
   // Function which will control whether this recovery has detected
   //  that it is needed give the current system state
   //
   virtual int shouldRecover ();

   //
   // Function to copy the dynamic data for the object
   //
   void copyOver (const AirBlock& rec);

   //
   // Function to delete the objects dynamic memory
   //
   void cleanup ();

protected:

   //
   // Message to request an air block recovery
   //
   AirInPlasmaLineMsg _AirBlockRecoveryRequest;

   //
   // Flag to store that a request was made, but not
   //  yet satisfied
   //
   int _RequestPending;

   //
   // Flag to signify that we have completed the air
   //  block recovery protocol
   //
   int _AirBlockCleared;

   //
   // Volume of plasma processed during the air block
   //  recovery
   //
   float _PlasmaProcessed;

};

#endif

/* FORMAT HASH 6c3de41728ef1a9b51ec5c6d4a6b2b76 */

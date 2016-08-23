/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      rbcchamberpurge.h
 *
 */

#ifndef _RBCCHAMBERPURGE_H_
#define _RBCCHAMBERPURGE_H_

#include "recoveryabs.h"
#include "trimamessages.h"

class RBCChamberPurge
   : public RecoveryAbs
{
   DECLARE_OBJ(RBCChamberPurge);
public:

   //
   // Constructor
   RBCChamberPurge();

   //
   // Copy constructor
   RBCChamberPurge(const RBCChamberPurge& rec);

   //
   // Destructor
   virtual ~RBCChamberPurge();

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
   // The first recovery is expected during any run
   virtual RecoveryTypes::InRecovery recoveryExpected ();

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
   // Function to copy the dynamic data for the object
   //
   void copyOver (const RBCChamberPurge& rec);

   //
   // Function to delete the objects dynamic memory
   //
   void cleanup ();

protected:

   //
   // Message to start the recovery
   //
   RBCChamberPurgeMsg _RecoveryReqMsg;

   //
   // Flag to signify this recovery was requested
   //
   int _RecoveryRequested;

   //
   // Flag to signify the recovery has completed
   //
   int _ChamberPurged;

   //
   // Volume of plasma processed through this recovery
   //
   float _PlasmaVolume;

};

#endif

/* FORMAT HASH ff36a8f68dd6891edccb4aa48dc448d4 */

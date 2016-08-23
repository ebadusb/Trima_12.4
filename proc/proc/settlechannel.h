/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      settlechannel.h
 *
 */

#ifndef _SETTLECHANNEL_H_
#define _SETTLECHANNEL_H_

#include "recoveryabs.h"
#include "trimamessages.h"

class SettleChannel
   : public RecoveryAbs
{
   DECLARE_OBJ(SettleChannel);
public:

   //
   // Constructor
   SettleChannel();

   //
   // Copy constructor
   SettleChannel(const SettleChannel& rec);

   //
   // Destructor
   virtual ~SettleChannel();

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

protected:

   //
   // Function which will control whether this recovery has detected
   //  that it is needed give the current system state
   //
   virtual int shouldRecover ();

   //
   // Function to reset the flags
   //
   virtual void reset ();

   //
   // Callback for the settle channel request message
   //
   void settleChannelRequest ();

   //
   // Function to copy the dynamic data for the object
   //
   void copyOver (const SettleChannel& rec);

   //
   // Function to delete the objects dynamic memory
   //
   void cleanup ();

protected:

   //
   // Settle channel request message
   //
   SettleChannelMsg _SettleChannelReqMsg;

   //
   // Flag to signify the recovery is needed
   //
   int _RecoveryRequested;

   //
   // Flag to signify the recovery has completed
   //
   int _ChannelSettled;

   //
   // Volume of plasma processed through this recovery
   //
   float _InletVolume;

};

#endif

/* FORMAT HASH 91003dd23c274d772eb969398638be9e */

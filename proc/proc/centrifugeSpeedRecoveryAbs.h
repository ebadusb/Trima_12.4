/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      centrifugeSpeedRecoveryAbs.h
 *
 */

#ifndef __CENTRIFUGE_SPEED_RECOVERY_ABS__
#define __CENTRIFUGE_SPEED_RECOVERY_ABS__

#include "recoveryabs.h"
#include "timermessage.h"
#include "procdata.h"

//////////////////////////////////////////////////////////////
//
// Abstract base class for all centrifuge speed recoveries
//
class CentrifugeSpeedRecoveryAbs
   : public RecoveryAbs
{
public:

   //
   // Constructor
   CentrifugeSpeedRecoveryAbs();

   //
   // Copy constructor
   CentrifugeSpeedRecoveryAbs(const CentrifugeSpeedRecoveryAbs& rec);

   //
   // Destructor
   virtual ~CentrifugeSpeedRecoveryAbs();

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
   // Callback for the interface setup timer
   //
   virtual void interfaceEstablished ();

   //
   // Function which may be overridden if the recovery is an exception which
   //  is expected and needed during the run ...
   virtual RecoveryTypes::InRecovery recoveryExpected ();

protected:

   //
   // Function which will control whether this recovery has detected
   //  that it is needed give the current system state
   //
   virtual int shouldRecover () = 0;

   virtual void returnCentrifugeToSpeed (ProcData& pd)     = 0;

   virtual bool centrifugeIsUpToSpeed (ProcData& pd) const = 0;

   virtual int interfaceSetupTimerInterval ()              = 0;

   virtual float numRecoveriesExpected ()                  = 0;

   //
   // Function to copy the dynamic data for the object
   //
   virtual void copyOver (const CentrifugeSpeedRecoveryAbs& rec);

   //
   // Function to delete the objects dynamic memory
   //
   virtual void cleanup ();

protected:

   //
   // Timer to track the amount of time the centrifuge is up
   //  to speed
   //
   TimerMessage _InterfaceSetupTimer;

   //
   // Flag to signify the centrifuge is back up to speed and
   //  the recovery is over
   //
   int _InterfaceEstablished;

   //
   // Flag to signify the timer has been started
   //
   int _TimerStarted;

};

//////////////////////////////////////////////////////////////
//
// Abstract base class for the centrifuge slow recoveries
//
class CentrifugeSlowRecoveryAbs
   : public CentrifugeSpeedRecoveryAbs
{
public:

   //
   // Constructor
   CentrifugeSlowRecoveryAbs();

   //
   // Copy constructor
   CentrifugeSlowRecoveryAbs(const CentrifugeSlowRecoveryAbs& rec);

protected:

   //
   // Function which will control whether this recovery has detected
   //  that it is needed give the current system state
   //
   virtual int interfaceSetupTimerInterval ();

   virtual float numRecoveriesExpected ();
};

//////////////////////////////////////////////////////////////
//
// Abstract base class for the centrifuge slow recoveries
//
class CentrifugeStopRecoveryAbs
   : public CentrifugeSpeedRecoveryAbs
{
public:

   //
   // Constructor
   CentrifugeStopRecoveryAbs();

   //
   // Copy constructor
   CentrifugeStopRecoveryAbs(const CentrifugeStopRecoveryAbs& rec);

   virtual int doRecovery ();

   //
   // Callback for the recovery request message
   //
   void recoveryRequest ();

protected:

   //
   // Function which will control whether this recovery has detected
   //  that it is needed give the current system state
   //
   virtual int interfaceSetupTimerInterval ();

   virtual float numRecoveriesExpected ();

   //
   // Flag to signify that a recovery has been requested
   //
   bool _RecoveryRequested;

   //
   // Message to request this recovery be performed
   //
   CentrifugeStopMsg _RecoveryReqMsg;
};


#endif

/* FORMAT HASH e5a165d9b076176fdb6a825d8c41c15a */

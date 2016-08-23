/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plasmavalvemotion.h
 *
 */

#ifndef _PLASMAVALVEMOTION_H_
#define _PLASMAVALVEMOTION_H_

#include "recoveryabs.h"
#include "timermessage.h"
#include "haldefs.h"

class PlasmaValveMotion
   : public RecoveryAbs
{
   DECLARE_OBJ(PlasmaValveMotion);
public:

   //
   // Constructor
   PlasmaValveMotion();

   //
   // Copy constructor
   PlasmaValveMotion(const PlasmaValveMotion& rec);

   //
   // Destructor
   virtual ~PlasmaValveMotion();

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
   // Callback function for the wait timer
   //
   void valveCanMove ();

   //
   // function to control whether this recovery may be performed
   //  given the current system state and data
   //
   virtual int mayEnter () const;

protected:

   //
   // Function which will control whether this recovery has detected
   //  that it is needed give the current system state
   //
   virtual int shouldRecover ();

   //
   // Function to detect if the valve has been commanded to move
   //
   int valveCommandedToMove ();

   //
   // Function to stop all pumps
   //
   void stopPumps ();

   //
   // Function to copy the dynamic data for the object
   //
   void copyOver (const PlasmaValveMotion& rec);

   //
   // Function to delete the objects dynamic memory
   //
   void cleanup ();

protected:

   //
   // Timer to wait for the pumps to stop before continuing
   //
   TimerMessage _WaitTimer;

   //
   // Flag to signify the timer is running
   //
   int _TimerStarted;

   //
   // Flag to signify the valve can now move
   //
   int _ValveCanMove;

   //
   // Flag to signify the valve is in its new position
   //
   int _ValveInPosition;

   //
   // Valve order
   //
   HAL_VALVESTATE _NewOrder;
};

#endif

/* FORMAT HASH b16cc8355d6e8cec88c61d07f481e663 */

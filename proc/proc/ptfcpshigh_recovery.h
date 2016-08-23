/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 *
 */

#ifndef PtfCpsHighRecovery_H_
#define PtfCpsHighRecovery_H_

#include "recoveryabs.h"
#include "trimamessages.h"
#include "aPQMonitor.h"


class PtfCpsHighRecovery
   : public RecoveryAbs
{
   DECLARE_OBJ(PtfCpsHighRecovery);
public:

   //
   // Constructor
   PtfCpsHighRecovery();

   //
   // Copy constructor
   PtfCpsHighRecovery(const PtfCpsHighRecovery& rec);

   //
   // Destructor
   virtual ~PtfCpsHighRecovery();

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


protected:

   // calculate the AC and inlet with the proper ratio
   void command_AC_Inlet (float Qac, float Qin);


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
   void copyOver (const PtfCpsHighRecovery& rec);

   //
   // Function to delete the objects dynamic memory
   //
   void cleanup ();

   void alarmListener ();

protected:


   float              _clearInletVolume;

   float              _bagLineInletVolume;

   bool               _centrifugeAtAltSpeed;

   AlarmActiveMessage _AlarmMsg;

};

#endif

/* FORMAT HASH a40bf711d71e0432a214c87278495d57 */

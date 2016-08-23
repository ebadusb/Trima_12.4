/*
* Copyright (c) 2009 by Caridian BCT, Inc.  All rights reserved.
*
* TITLE:
*
*/

#ifndef _CENTRIFUGERBCRECOVERY_H_
#define _CENTRIFUGERBCRECOVERY_H_


#include "an_alarm.h"
#include "recoveryabs.h"
#include "trimamessages.h"

class CentrifugeRBCRecovery
   : public RecoveryAbs
{
   DECLARE_OBJ(CentrifugeRBCRecovery);
public:

   //
   // Constructor
   CentrifugeRBCRecovery();

   //
   // Copy constructor
   CentrifugeRBCRecovery(const CentrifugeRBCRecovery& rec);

   //
   // Destructor
   virtual ~CentrifugeRBCRecovery();

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

   // my version of disabling this beast.
   virtual bool disabled () const;

   // how i disable this recovery after it is used...
   void disable (bool disableThis)  {_imDisabled = disableThis; }

   //
   // Function which will control whether this recovery has detected
   //  that it is needed give the current system state
   //
   virtual int shouldRecover ();


   // centrifuge within 1% of target speed (2970rpms usually)
   virtual bool centrifugeIsUpToSpeed () const;


private:

   bool       _imDisabled;
   anAlarmMsg _CentErrAlarm;
   float      _inletVolume;
};

#endif

/* FORMAT HASH 22cb1ccac3cbdb5f2625f4402c60e330 */

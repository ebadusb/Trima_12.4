/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      centrifugestopscaled.h
 *
 */

#ifndef __CENTRIFUGE_STOP_SCALED_H__
#define __CENTRIFUGE_STOP_SCALED_H__

#include "centrifugeSpeedRecoveryAbs.h"

class CentrifugeStopScaled
   : public CentrifugeStopRecoveryAbs
{
   DECLARE_OBJ(CentrifugeStopScaled);
public:

   //
   // Constructor
   CentrifugeStopScaled();

   //
   // Copy constructor
   CentrifugeStopScaled(const CentrifugeStopScaled& rec);

protected:

   //
   // Function which will control whether this recovery has detected
   //  that it is needed give the current system state
   //
   virtual int shouldRecover ();

   virtual void returnCentrifugeToSpeed (ProcData& pd);

   virtual bool centrifugeIsUpToSpeed (ProcData& pd) const;

   virtual bool disabled () const;
};

#endif

/* FORMAT HASH 91067cb4b722c5f04fc61614f79ffb14 */

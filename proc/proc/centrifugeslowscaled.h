/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      centrifugeslowscaled.h
 *
 */

#ifndef __CENTRIFUGE_SLOW_SCALED_H__
#define __CENTRIFUGE_SLOW_SCALED_H__

#include "centrifugeSpeedRecoveryAbs.h"

class CentrifugeSlowScaled
   : public CentrifugeSlowRecoveryAbs
{
   DECLARE_OBJ(CentrifugeSlowScaled);
public:

   //
   // Constructor
   CentrifugeSlowScaled();

   //
   // Copy constructor
   CentrifugeSlowScaled(const CentrifugeSlowScaled& rec);

protected:

   //
   // Function which will control whether this recovery has detected
   //  that it is needed give the current system state
   //
   virtual int shouldRecover ();

   virtual void returnCentrifugeToSpeed (ProcData& pd);

   virtual bool centrifugeIsUpToSpeed (ProcData& pd) const;

   virtual bool disabled () const;

   virtual float timeElapsedWithPumpsPaused (ProcData& pd) const;
};

#endif

/* FORMAT HASH 35b972c5405e22af814f756e29f376b2 */

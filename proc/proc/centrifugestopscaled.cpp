/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      centrifugestopscaled.cpp
 *
 */

#include "centrifugestopscaled.h"
#include "substatemap.h"
#include "cobeconfig.h"
#include "trima_datalog.h"

DEFINE_OBJ(CentrifugeStopScaled);

CentrifugeStopScaled::CentrifugeStopScaled()
   : CentrifugeStopRecoveryAbs()
{}

CentrifugeStopScaled::CentrifugeStopScaled(const CentrifugeStopScaled& rec)
   : CentrifugeStopRecoveryAbs(rec)
{}

int CentrifugeStopScaled::shouldRecover ()
{
   // If this is disabled, then we should not recover.
   if (disabled()) return 0;

   ProcData pd;

   float    stopTrigger = CobeConfig::data().CentStopRPMTriggerScaled;

   if (_RecoveryRequested
       || pd.Status().CentrifugeRPM() <= stopTrigger
       || ( active() && !_InterfaceEstablished ) )
   {
      // Only log the first one.
      if (!active())
         DataLog (log_level_proc_info) << "shouldRecover(). [request="
                                       << _RecoveryRequested << "; rpm="
                                       << pd.Status().CentrifugeRPM() << "; trigger="
                                       << stopTrigger << "; active()="
                                       << active() << "; interface=" << _InterfaceEstablished
                                       << "]" << endmsg;

      //
      // Continue the recovery ...
      return 1;
   }

   //
   // Reset the flag for the next time the
   //  recovery is performed ...
   _InterfaceEstablished = 0;

   return 0;
}

void CentrifugeStopScaled::returnCentrifugeToSpeed (ProcData& pd)
{
   pd.Orders().scaleRPM();
}

bool CentrifugeStopScaled::centrifugeIsUpToSpeed (ProcData& pd) const
{
   float centTarget = pd.Orders().getScaledRPM() - CobeConfig::data().CentStopRPMTargetScaledDelta;
   return (pd.Status().CentrifugeRPM() > centTarget);
}

bool CentrifugeStopScaled::disabled () const
{
   return getCentAlgType() != SCALED;
}

/* FORMAT HASH a0385f8a08e72bd0208f59760ccf0a80 */

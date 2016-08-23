/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      centrifugeslowscaled.cpp
 *
 */

#include "centrifugeslowscaled.h"
#include "substatemap.h"
#include "cobeconfig.h"
#include "procdata.h"
#include "trima_datalog.h"

DEFINE_OBJ(CentrifugeSlowScaled);

CentrifugeSlowScaled::CentrifugeSlowScaled()
   : CentrifugeSlowRecoveryAbs()
{
   // fprintf(stderr, "CentrifugeSlowScaled constructed (default)");
}

CentrifugeSlowScaled::CentrifugeSlowScaled(const CentrifugeSlowScaled& rec)
   : CentrifugeSlowRecoveryAbs(rec)
{
   // fprintf(stderr, "CentrifugeSlowScaled constructed (copy)");
}


int CentrifugeSlowScaled::shouldRecover ()
{
   // If this is disabled, then we should not recover.
   if (disabled()) return 0;

   ProcData pd;

   float    slowingTrigger         = pd.Orders().getScaledRPM() - CobeConfig::data().CentSlowRPMTriggerScaledDelta;
   float    pumpsPausedTimeElapsed = timeElapsedWithPumpsPaused(pd);

   if (((pumpsPausedTimeElapsed > 60000) && (pd.Status().CentrifugeRPM() <= slowingTrigger)) ||
       ( active() && !_InterfaceEstablished ))
   {
      // Only log if we just switched.
      if (!active())
         DataLog (log_level_proc_info) << "shouldRecover(). [pauseTimeElapsed="
                                       << pumpsPausedTimeElapsed << "; rpm="
                                       << pd.Status().CentrifugeRPM() << "; trigger="
                                       << slowingTrigger << "; active()="
                                       << active() << "; interface=" << _InterfaceEstablished
                                       << "]" << endmsg;

      return 1;
   }

   //
   // Reset the interface established flag ...
   //
   _InterfaceEstablished = 0;

   return 0;
}


void CentrifugeSlowScaled::returnCentrifugeToSpeed (ProcData& pd)
{
   //
   // Command the centrifuge to return to
   //  speed ...
   //
   pd.Orders().scaleRPM();
   pd.Orders().RampUp(CobeConfig::data().StopRecoveryLowCentrifugeRampUpRate);
}

bool CentrifugeSlowScaled::centrifugeIsUpToSpeed (ProcData& pd) const
{
   float centTarget = pd.Orders().getScaledRPM() - CobeConfig::data().CentSlowRPMTargetScaledDelta;
   return (pd.Status().CentrifugeRPM() > centTarget);
}

bool CentrifugeSlowScaled::disabled () const
{
   return getCentAlgType() != SCALED;
}

float CentrifugeSlowScaled::timeElapsedWithPumpsPaused (ProcData& pd) const
{
   unsigned        elapsed = 0;
   struct timespec current;
   clock_gettime(CLOCK_REALTIME, &current);

   if (pd.Run().StartPauseTime.Get().tv_sec == 0.0f ||
       current.tv_sec <= pd.Run().StartPauseTime.Get().tv_sec)
      return 0.0f;

   return 1000 * ( current.tv_sec - pd.Run().StartPauseTime.Get().tv_sec );
}

/* FORMAT HASH 83fef003c5f7bef03eb43c32cee705d1 */

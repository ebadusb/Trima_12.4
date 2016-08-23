/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeChannel2A.cpp
 *
 */

#include "primechannel2a.h"
#include "procdata.h"
#include "cobeconfig.h"


DEFINE_STATE(PrimeChannel2A);

PrimeChannel2A::PrimeChannel2A()
   : StateAbs()
{}

PrimeChannel2A::PrimeChannel2A(const PrimeChannel2A& state)
   : StateAbs(state)
{}

PrimeChannel2A::~PrimeChannel2A()
{}

int PrimeChannel2A::transitionStatus ()
{
   ProcData pd;

   if (    !pd.Status().CassIsPltPlsRBC()
           ||  (pd.Donor().Hct.Get() * 100.0f) < CobeConfig::data().HctTriggerChannel2A )
   {
      DataLog(log_level_proc_blood_run_info) << "Skipping Prime Channel 2A" << endmsg;
      return 2;
   }

   float vol = pd.Volumes().VinTotal.Get();
   if ( vol > CobeConfig::data().VinPrimeChannel2A )
   {
      DataLog(log_level_proc_debug) << "VinTotal ( " << vol << " ) > "
                                    << CobeConfig::data().VinPrimeChannel2A  << endmsg;

      return 1;
   }
   DataLog(log_level_proc_debug) << "VinTotal ( " << vol << " ) <= "
                                 << CobeConfig::data().VinPrimeChannel2A  << endmsg;

   return NO_TRANSITION;
}

int PrimeChannel2A::preProcess ()
{
   ProcData pd;

   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);

   //
   //
   //  Calculate max inlet flow as lesser of Cobe config or adjustment value
   //
   float qin          = pd.Adjustments().Draw.CurrentCap.Get();
   if (qin > CobeConfig::data().QinPrimeChannel2A) qin = CobeConfig::data().QinPrimeChannel2A;
   float adjustFactor = qin / CobeConfig::data().QinPrimeChannel2A;

   float ratio        = pd.Config().Procedure.Get().key_custom_ratio;

   float qcol         = CobeConfig::data().QcolPrimeChannel2A;
   if (pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC) qcol = 0.0f;

   pd.Orders().Qin(qin);
   pd.Orders().Qac(qin / ratio);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qplasma(CobeConfig::data().QpPrimeChannel2A * adjustFactor);
   pd.Orders().Qcollect(qcol * adjustFactor);
   pd.Orders().RPM(CobeConfig::data().RPMPrimeChannel2A);

   //
   // Set the centrifuge pressure limit ...
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CentPresLimit.Set(CobeConfig::data().CentPresLimitHigh);

   return NORMAL;
}

int PrimeChannel2A::postProcess ()
{
   return NORMAL;
}

int PrimeChannel2A::preEnter ()
{
   return NORMAL;
}

int PrimeChannel2A::postExit ()
{
   //
   // Place action here that will occur after the state decides to transition ...
   //
   return NORMAL;
}

void PrimeChannel2A::copyOver (const PrimeChannel2A&)
{}

void PrimeChannel2A::cleanup ()
{
   //
   // Delete the dynamic memory ...
   //
}

/* FORMAT HASH 77fe57b9f07bde3685189ac4ecdf9521 */

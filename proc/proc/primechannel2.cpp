/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeChannel2.cpp
 *
 */

#include "primechannel2.h"
#include "procdata.h"
#include "cobeconfig.h"


DEFINE_STATE(PrimeChannel2);

PrimeChannel2::PrimeChannel2()
   : StateAbs()
{}

PrimeChannel2::PrimeChannel2(const PrimeChannel2& state)
   : StateAbs(state)
{}

PrimeChannel2::~PrimeChannel2()
{}

int PrimeChannel2::transitionStatus ()
{
   ProcData pd;

   float    vol = pd.Volumes().VinTotal.Get();
   if ( vol > CobeConfig::data().VinPrimeChannel2 )
   {
      DataLog(log_level_proc_debug) << "VinTotal ( " << vol << " ) > "
                                    << CobeConfig::data().VinPrimeChannel2  << endmsg;

      return 1;
   }
   DataLog(log_level_proc_debug) << "VinTotal ( " << vol << " ) <= "
                                 << CobeConfig::data().VinPrimeChannel2  << endmsg;

   return NO_TRANSITION;
}

int PrimeChannel2::preProcess ()
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
   if (qin > CobeConfig::data().QinPrimeChannel2) qin = CobeConfig::data().QinPrimeChannel2;
   float adjustFactor = qin / CobeConfig::data().QinPrimeChannel2;

   float ratio        = pd.Config().Procedure.Get().key_custom_ratio;

   float qcol         = CobeConfig::data().QcolPrimeChannel2;
   if (pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC) qcol = 0.0f;

   pd.Orders().Qin(qin);
   pd.Orders().Qac(qin / ratio);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qplasma(CobeConfig::data().QpPrimeChannel2 * adjustFactor);
   pd.Orders().Qcollect(qcol * adjustFactor);
   pd.Orders().RPM(CobeConfig::data().RPMPrimeChannel2);

   //
   // Set the centrifuge pressure limit ...
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CentPresLimit.Set(CobeConfig::data().CentPresLimitHigh);

   return NORMAL;
}

int PrimeChannel2::postProcess ()
{
   return NORMAL;
}

int PrimeChannel2::preEnter ()
{
   return NORMAL;
}

int PrimeChannel2::postExit ()
{
   //
   // Place action here that will occur after the state decides to transition ...
   //
   return NORMAL;
}

void PrimeChannel2::copyOver (const PrimeChannel2&)
{}

void PrimeChannel2::cleanup ()
{
   //
   // Delete the dynamic memory ...
   //
}

/* FORMAT HASH 2067c4feb11bc658a3bb5269d0d1ba57 */

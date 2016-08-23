/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeChannel3.cpp
 *
 */

#include "primechannel3.h"
#include "procdata.h"
#include "cobeconfig.h"


DEFINE_STATE(PrimeChannel3);

PrimeChannel3::PrimeChannel3()
   : StateAbs()
{}

PrimeChannel3::PrimeChannel3(const PrimeChannel3& state)
   : StateAbs(state)
{}

PrimeChannel3::~PrimeChannel3()
{}

int PrimeChannel3::transitionStatus ()
{
   ProcData pd;

   float    vol = pd.Volumes().VinTotal.Get();
   if ( vol > CobeConfig::data().VinPrimeChannel3 )
   {
      DataLog(log_level_proc_debug) << "VinTotal ( " << vol << " ) > "
                                    << CobeConfig::data().VinPrimeChannel3  << endmsg;

      return 1;
   }
   DataLog(log_level_proc_debug) << "VinTotal ( " << vol << " ) <= "
                                 << CobeConfig::data().VinPrimeChannel3  << endmsg;

   return NO_TRANSITION;
}

int PrimeChannel3::preProcess ()
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
   if (qin > CobeConfig::data().QinPrimeChannel3) qin = CobeConfig::data().QinPrimeChannel3;
   float adjustFactor = qin / CobeConfig::data().QinPrimeChannel3;

   float ratio        = pd.Config().Procedure.Get().key_custom_ratio;

   float qcol         = CobeConfig::data().QcolPrimeChannel3;
   if (pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC) qcol = 0.0f;

   pd.Orders().Qin(qin);
   pd.Orders().Qac(qin / ratio);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qplasma(CobeConfig::data().QpPrimeChannel3 * adjustFactor);
   pd.Orders().Qcollect(qcol * adjustFactor);
   pd.Orders().RPM(CobeConfig::data().RPMPrimeChannel3);

   //
   // Set the centrifuge pressure limit ...
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CentPresLimit.Set(CobeConfig::data().CentPresLimitHigh);

   return NORMAL;
}

int PrimeChannel3::postProcess ()
{
   return NORMAL;
}

int PrimeChannel3::preEnter ()
{
   return NORMAL;
}

int PrimeChannel3::postExit ()
{
   //
   // Place action here that will occur after the state decides to transition ...
   //
   return NORMAL;
}

void PrimeChannel3::copyOver (const PrimeChannel3&)
{}

void PrimeChannel3::cleanup ()
{
   //
   // Delete the dynamic memory ...
   //
}

/* FORMAT HASH 2ff8fb0a577c2df6721fe9ab0e26fbd0 */

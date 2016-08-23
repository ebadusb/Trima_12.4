/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeChannel4.cpp
 *
 */

#include "primechannel4.h"
#include "procdata.h"
#include "cobeconfig.h"

DEFINE_STATE(PrimeChannel4);

PrimeChannel4::PrimeChannel4()
   : StateAbs()
{}

PrimeChannel4::PrimeChannel4(const PrimeChannel4& state)
   : StateAbs(state)
{}

PrimeChannel4::~PrimeChannel4()
{}

int PrimeChannel4::transitionStatus ()
{
   ProcData pd;

   float    vol = pd.Volumes().VinTotal.Get();
   if ( vol > CobeConfig::data().VinPrimeChannel4 )
   {
      DataLog(log_level_proc_debug) << "VinTotal ( " << vol << " ) > "
                                    << CobeConfig::data().VinPrimeChannel4  << endmsg;

      return 1;
   }
   DataLog(log_level_proc_debug) << "VinTotal ( " << vol << " ) <= "
                                 << CobeConfig::data().VinPrimeChannel4  << endmsg;

   return NO_TRANSITION;
}

int PrimeChannel4::preProcess ()
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
   if (qin > CobeConfig::data().QinPrimeChannel4) qin = CobeConfig::data().QinPrimeChannel4;
   float adjustFactor = qin / CobeConfig::data().QinPrimeChannel4;

   float ratio        = pd.Config().Procedure.Get().key_custom_ratio;

   float qcol         = CobeConfig::data().QcolPrimeChannel4;
   if (pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC) qcol = 0.0f;

   pd.Orders().Qin(qin);
   pd.Orders().Qac(qin / ratio);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qplasma(CobeConfig::data().QpPrimeChannel4 * adjustFactor);
   pd.Orders().Qcollect(qcol * adjustFactor);
   pd.Orders().RPM(CobeConfig::data().RPMPrimeChannel4);

   return NORMAL;
}

int PrimeChannel4::postProcess ()
{
   return NORMAL;
}

int PrimeChannel4::preEnter ()
{
   return NORMAL;
}

int PrimeChannel4::postExit ()
{
   //
   // Place action here that will occur after the state decides to transition ...
   //
   return NORMAL;
}

void PrimeChannel4::copyOver (const PrimeChannel4&)
{}

void PrimeChannel4::cleanup ()
{
   //
   // Delete the dynamic memory ...
   //
}

/* FORMAT HASH f78f8f4b212f5d46135e818cb950d6d2 */

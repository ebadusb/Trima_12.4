/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeVent.cpp
 *
 */

#include "primevent.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "volumeacc.h"

DEFINE_STATE(PrimeVent);

PrimeVent::PrimeVent()
   : StateAbs()
{}

PrimeVent::PrimeVent(const PrimeVent& state)
   : StateAbs(state)
{
   copyOver(state);
}

PrimeVent::~PrimeVent()
{
   cleanup();
}

int PrimeVent::transitionStatus ()
{
   ProcData pd;

   if (pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC) return 1;

   float vol = pd.Volumes().VinTotal.Get();
   if (    vol > CobeConfig::data().VinPrimeVent
           && pd.Status().CollectValve.IsReturning() )
   {
      DataLog(log_level_proc_debug) << "VinTotal ( " << vol << " ) > "
                                    << CobeConfig::data().VinPrimeVent  << endmsg;

      return 1;
   }
   DataLog(log_level_proc_debug) << "VinTotal ( " << vol << " ) <= "
                                 << CobeConfig::data().VinPrimeVent  << endmsg;

   return NO_TRANSITION;
}

int PrimeVent::preProcess ()
{
   ProcData pd;

   //
   //
   //  Calculate max inlet flow as lesser of Cobe config or adjustment value
   //
   float qin          = pd.Adjustments().Draw.CurrentCap.Get();
   if (qin > CobeConfig::data().QinPrimeVent) qin = CobeConfig::data().QinPrimeVent;
   float adjustFactor = qin / CobeConfig::data().QinPrimeVent;

   float ratio        = pd.Config().Procedure.Get().key_custom_ratio;

   float qcol         = CobeConfig::data().QcolPrimeVent;

   pd.Orders().Qin(qin);
   pd.Orders().Qac(qin / ratio);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qplasma(CobeConfig::data().QpPrimeVent * adjustFactor);
   pd.Orders().Qcollect(qcol * adjustFactor);
   pd.Orders().RPM(CobeConfig::data().RPMPrimeVent);

   return NORMAL;
}

int PrimeVent::postProcess ()
{
   ProcData pd;

   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);

   //
   // Close the valve after we processed enough volume ...
   if ( pd.Volumes().VinTotal.Get() > CobeConfig::data().VinPrimeVent )
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   else
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_COLLECT);

   return NORMAL;
}

int PrimeVent::preEnter ()
{
   return NORMAL;
}

int PrimeVent::postExit ()
{
   //
   // Clear the platelet bag volumes ...
   VolumeAcc::ClearPlateletBag();

   //
   // Place action here that will occur after the state decides to transition ...
   //
   return NORMAL;
}

void PrimeVent::copyOver (const PrimeVent&)
{}

void PrimeVent::cleanup ()
{
   //
   // Delete the dynamic memory ...
   //
}

/* FORMAT HASH aaef8ef5105c25f560e7fe4e72152312 */

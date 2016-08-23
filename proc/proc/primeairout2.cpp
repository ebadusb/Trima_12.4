/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeAirout2.cpp
 *
 */

#include "primeairout2.h"
#include "substatemap.h"
#include "procdata.h"
#include "cobeconfig.h"


DEFINE_STATE(PrimeAirout2);

PrimeAirout2::PrimeAirout2()
   : StateAbs(),
     _VinAtStart(0.0f)
{}

PrimeAirout2::PrimeAirout2(const PrimeAirout2& state)
   : StateAbs(state),
     _VinAtStart(0.0f)
{
   copyOver(state);
}

PrimeAirout2::~PrimeAirout2()
{
   cleanup();
}

int PrimeAirout2::transitionStatus ()
{
   ProcData pd;

   //
   // This state should only occur after the first return cycle...
   //
   if ( pd.Run().NumReturn.Get() > 1 )
   {
      return 2;
   }

   return _TransitionStatus;
}

int PrimeAirout2::preProcess ()
{
   ProcData pd;






   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);
   //
   //
   //  Calculate max inlet flow as lesser of Cobe config or adjustment cap.  Per IT12734
   //  this should not violate AC infusion.
   //
   float qin    = CobeConfig::data().QinPrimeAirout2;
   float qincap = pd.Adjustments().Draw.CurrentCap.Get();
   if (qin > qincap) qin = qincap;

   float ratio = pd.Config().Procedure.Get().key_custom_ratio;

   float qcol  = CobeConfig::data().QcolPrimeAirout2;
   if (pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC) qcol = 0.0f;

   float qrp = qin + 60.0f; // IT12734

   if (pd.Run().DrawCycle.Get())
   {
      pd.Orders().Qac(qin / ratio);
      pd.Orders().Qrp(0.0f);
   }
   else
   {
      pd.Orders().Qac(0.0f);
      pd.Orders().Qrp(qrp);

      static bool logDone = false;
      if (!logDone)
      {
         DataLog(log_level_proc_info) << "First Return Qrp commanded to " << qrp << endmsg;
         logDone = true;
      }

   }

   pd.Orders().Qin(qin);
   pd.Orders().Qplasma(CobeConfig::data().QpPrimeAirout2 /* *adjustFactor */);
   pd.Orders().Qcollect(qcol /* *adjustFactor */); // removed adjustFactor per IT5284
   pd.Orders().RPM(CobeConfig::data().RPMPrimeAirout2);

   return NORMAL;
}

int PrimeAirout2::postProcess ()
{
   ProcData pd;

   float    deltaVin = pd.Volumes().VinTotal.Get() - _VinAtStart;

   if ( deltaVin > CobeConfig::data().DeltaVinPrimeAirout2 )
   {
      _TransitionStatus = 1;
   }

   return NORMAL;
}

int PrimeAirout2::preEnter ()
{
   ProcData pd;

   _VinAtStart = pd.Volumes().VinTotal.Get();

   return NORMAL;
}

int PrimeAirout2::postExit ()
{
   //
   // Place action here that will occur after the state decides to transition ...
   //
   _TransitionStatus = NO_TRANSITION; // in case of re-entry
   cleanup();
   return NORMAL;
}

void PrimeAirout2::copyOver (const PrimeAirout2&)
{}

void PrimeAirout2::cleanup ()
{}

/* FORMAT HASH 48ab8d76b7a0f1740daaeefc6075be22 */

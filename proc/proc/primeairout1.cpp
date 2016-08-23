/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeAirout1.cpp
 *
 */

#include "primeairout1.h"
#include "substatemap.h"
#include "procdata.h"
#include "cobeconfig.h"

DEFINE_STATE(PrimeAirout1);

PrimeAirout1::PrimeAirout1()
   : StateAbs()
{}

PrimeAirout1::PrimeAirout1(const PrimeAirout1& state)
   : StateAbs(state)
{
   copyOver(state);
}

PrimeAirout1::~PrimeAirout1()
{
   cleanup();
}

int PrimeAirout1::transitionStatus ()
{
   ProcData pd;

   if ( pd.Status().CentrifugeRPM() <= CobeConfig::data().RPMPrimeAirout1 )
   {
      return 1;
   }
   DataLog(log_level_proc_debug) << "RPM ( " << pd.Status().CentrifugeRPM() << " ) > "
                                 << CobeConfig::data().RPMPrimeAirout1  << endmsg;

   return NO_TRANSITION;
}

int PrimeAirout1::preProcess ()
{
   ProcData pd;

   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);

   //
   //
   //  Calculate max inlet flow as lesser of Cobe config or nocollect flows
   //  (nocollect flows are limited by IR and adjustments)
   //
   float qin          = CobeConfig::data().QinPrimeAirout1;
   // float nocollectQin =  pd.Predict().NoCollectFirstReturnOnly.Get().QinD;
   float nocollectQin = pd.Predict().Qin(1, SubstateMap::alg(NOCOLLECT_STATE));


   if (qin > nocollectQin) qin = nocollectQin;

   //
   //
   //  If Qin is limited for some reason then scale other flows accordingly
   //
   float adjustFactor = 1.0f;
   if (qin < CobeConfig::data().QinPrimeAirout1)
   {
      adjustFactor = qin / CobeConfig::data().QinPrimeAirout1;
   }

   float ratio = pd.Config().Procedure.Get().key_custom_ratio;

   float qcol  = CobeConfig::data().QcolPrimeAirout1;
   if (pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC) qcol = 0.0f;

   if (pd.Run().DrawCycle.Get())
   {
      pd.Orders().Qac(qin / ratio);
      pd.Orders().Qrp(0.0f);
   }
   else
   {
      pd.Orders().Qac(0.0f);
      pd.Orders().Qrp(pd.Predict().NoCollectFirstReturnOnly.Get().Qrp);

      static bool logDone = false;
      if (!logDone)
      {
         DataLog(log_level_proc_info) << "First Return Qrp commanded to "
                                      << pd.Predict().NoCollectFirstReturnOnly.Get().Qrp
                                      << endmsg;
         logDone = true;
      }
   }

   pd.Orders().Qin(qin);
   pd.Orders().Qplasma(CobeConfig::data().QpPrimeAirout1);
   pd.Orders().Qcollect(qcol);
   pd.Orders().RPM(CobeConfig::data().RPMPrimeAirout1);

   return NORMAL;
}

int PrimeAirout1::postProcess ()
{
   return NORMAL;
}

int PrimeAirout1::preEnter ()
{
   return NORMAL;
}

int PrimeAirout1::postExit ()
{
   //
   // Place action here that will occur after the state decides to transition ...
   //
   cleanup();
   return NORMAL;
}

void PrimeAirout1::copyOver (const PrimeAirout1&)
{}

void PrimeAirout1::cleanup ()
{}

/* FORMAT HASH c1bf53c01bcec7ac9183ec3136e2c219 */

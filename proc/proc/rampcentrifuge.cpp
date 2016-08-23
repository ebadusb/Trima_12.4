/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      RampCentrifuge.cpp
 *
 */

#include "rampcentrifuge.h"
#include "procdata.h"
#include "cobeconfig.h"

DEFINE_STATE(RampCentrifuge);

RampCentrifuge::RampCentrifuge()
   : StateAbs()
{}

RampCentrifuge::RampCentrifuge(const RampCentrifuge& state)
   : StateAbs(state)
{}

RampCentrifuge::~RampCentrifuge()
{}

int RampCentrifuge::transitionStatus ()
{
   ProcData pd;
   //
   //   Transition to return cycle or, we have
   //    already done this state once (ie. past first return )...
   //
   if (   !pd.Run().DrawCycle.Get()
          || pd.Run().NumDraw.Get() > 1
          || pd.Run().NumReturn.Get() > 0 )
   {
      return 1;
   }

   return NO_TRANSITION;
}

int RampCentrifuge::preProcess ()
{
   ProcData pd;

   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);     //
   //
   //  Calculate max inlet flow as lesser of Cobe config or adjustment value
   //
   float qin          = pd.Adjustments().Draw.CurrentCap.Get();
   float adjustFactor = 1.0f;
   if (qin >= CobeConfig::data().QinRampCentrifuge)
   {
      qin = CobeConfig::data().QinRampCentrifuge;
   }
   else     // qin adjusted less than cobe config
   {
      adjustFactor = qin / CobeConfig::data().QinRampCentrifuge;
   }

   float ratio = pd.Config().Procedure.Get().key_custom_ratio;

   float qcol  = CobeConfig::data().QcolRampCentrifuge;
   if (pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC) qcol = 0.0f;


   pd.Orders().Qin(qin);
   pd.Orders().Qac(qin / ratio);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qplasma(CobeConfig::data().QpRampCentrifuge * adjustFactor);
   pd.Orders().Qcollect(qcol * adjustFactor);
   pd.Orders().RPM(CobeConfig::data().RPMRampCentrifuge);

   return NORMAL;
}

int RampCentrifuge::postProcess ()
{
   return NORMAL;
}

int RampCentrifuge::preEnter ()
{

   return NORMAL;
}

int RampCentrifuge::postExit ()
{
   return NORMAL;
}

void RampCentrifuge::copyOver (const RampCentrifuge&)
{}

void RampCentrifuge::cleanup ()
{
   //
   // Delete the dynamic memory ...
   //
}

/* FORMAT HASH 594308ae6ea5e37d01bd8e7aa5f56f7e */

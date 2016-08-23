/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      RinsebackReservoir.cpp
 *
 */

#include "rinsebackreservoir.h"
#include "cobeconfig.h"
#include "procdata.h"

DEFINE_STATE(RinsebackReservoir);

RinsebackReservoir::RinsebackReservoir()
   : StateAbs()
{}

RinsebackReservoir::RinsebackReservoir(const RinsebackReservoir& state)
   : StateAbs(state)
{
   RinsebackReservoir::copyOver(state);
}

RinsebackReservoir::~RinsebackReservoir()
{
   RinsebackReservoir::cleanup();
}

int RinsebackReservoir::transitionStatus ()
{
   ProcData pd;

   float    deltaVp = pd.Volumes().Vp.Get() - pd.Volumes().VRinseRes.Get();
   if (( deltaVp > 10.0f ) || (pd.Status().ReservoirIsFull()) )
      return 1;

   return NO_TRANSITION;
}

int RinsebackReservoir::preProcess ()
{
   ProcData pd;

   //
   //  Check to see if we have processed enough volume.  Before this point
   //  we always return assuming a ratio of 5 to avoid safety shutdowns.
   //
   float ratio = ( pd.Volumes().VinTotal.Get() > 325.0f) ? pd.Run().LastRunRatio.Get() : 5.0f;
   if (ratio < 5.0f) ratio = 5.0f;

   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);

   float Hin  = pd.Run().AdjustedHct.Get() * (1.0f - 1.0f / ratio);
   float qcol = CobeConfig::data().RinsebackQinRecirculate * (1.0f - Hin);
   float qp   = qcol - CobeConfig::data().RinsebackQchamberRecirculate;

   //
   //
   //  Do not run collect / replacement pump for RBC/Plasma set
   //
   if ( pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC) qcol = 0.0f;

   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qplasma(qp);
   pd.Orders().Qcollect(qcol);
   pd.Orders().RPM(0.0f);


   return NORMAL;
}

int RinsebackReservoir::postProcess ()
{
   return NORMAL;
}

int RinsebackReservoir::preEnter ()
{
   ProcData pd;

   if ( pd.Volumes().VRinseRes.Get() <= 0.1f )
   {
      pd.MakeVolumesWritable();
      pd.Volumes().VRinseRes.Set(pd.Volumes().Vp.Get() );
   }

   return NORMAL;
}


int RinsebackReservoir::postExit ()
{
   return NORMAL;
}

void RinsebackReservoir::copyOver (const RinsebackReservoir&)
{}

void RinsebackReservoir::cleanup ()
{
   //
   // Delete the dynamic memory ...
   //
}

/* FORMAT HASH 4a0beccf7ef9549dcc9a108da36b59b0 */

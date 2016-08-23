/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      RinsebackRecirculate.cpp
 *
 */

#include "rinsebackrecirculate.h"
#include <math.h>
#include "cobeconfig.h"
#include "procdata.h"

DEFINE_STATE(RinsebackRecirculate);

RinsebackRecirculate::RinsebackRecirculate()
   : StateAbs(),
     _VpAtStart(0.0f)
{}

RinsebackRecirculate::RinsebackRecirculate(const RinsebackRecirculate& state)
   : StateAbs(state),
     _VpAtStart(state._VpAtStart)
{
   RinsebackRecirculate::copyOver(state);
}

RinsebackRecirculate::~RinsebackRecirculate()
{
   RinsebackRecirculate::cleanup();
}

int RinsebackRecirculate::transitionStatus ()
{
   ProcData pd;

   //
   //
   //   This state is skipped for RBC/Plasma collections
   //
   if (pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC)
      return 1;

   if ( pd.Volumes().VRinseRecirc.Get() > CobeConfig::data().RinsebackVpRecirculate )
      return 1;

   return NO_TRANSITION;
}

int RinsebackRecirculate::preProcess ()
{
   ProcData pd;

   //
   //  Check to see if we have processed enough volume.  Before this point
   //  we always return assuming a ratio of 5 to avoid safety shutdowns.
   //
   float ratio = (pd.Volumes().VinTotal.Get() > 325.0f) ? pd.Run().LastRunRatio.Get() : 5.0f;
   if (ratio < 5.0f) ratio = 5.0f;

   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);

   float Hin  = pd.Run().AdjustedHct.Get() * (1.0f - 1.0f / ratio);
   float qcol = CobeConfig::data().RinsebackQinRecirculate * (1.0f - Hin);
   pd.Orders().Qin(CobeConfig::data().RinsebackQinRecirculate);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qrp(CobeConfig::data().RinsebackQinRecirculate);
   pd.Orders().Qplasma(qcol - CobeConfig::data().RinsebackQchamberRecirculate);
   pd.Orders().Qcollect(qcol);




   static bool logDone = false;
   if (!logDone)
   {
      DataLog(log_level_proc_rinseback_info) << "RinsebackRecirculate Pumps started at..."
                                             << "  Qrp = "   << CobeConfig::data().RinsebackQinRecirculate
                                             << ", Qin = "   << CobeConfig::data().RinsebackQinRecirculate
                                             << ", Qac = "   << 0.0
                                             << ", Qp  = "   << (qcol - CobeConfig::data().RinsebackQchamberRecirculate)
                                             << ", Qc  = "   << qcol
                                             << ", Hin was " << Hin
                                             << endmsg;
      logDone = true;
   }



   return NORMAL;
}

int RinsebackRecirculate::postProcess ()
{
   ProcData pd;
   pd.MakeVolumesWritable();
   pd.Volumes().VRinseRecirc.Set(pd.Volumes().Vp.Get() - _VpAtStart);
   return NORMAL;
}

int RinsebackRecirculate::preEnter ()
{
   ProcData pd;
   _VpAtStart = pd.Volumes().Vp.Get();
   return NORMAL;
}

int RinsebackRecirculate::postExit ()
{
   return NORMAL;
}

void RinsebackRecirculate::copyOver (const RinsebackRecirculate&)
{}

void RinsebackRecirculate::cleanup ()
{
   //
   // Delete the dynamic memory ...
   //
}

/* FORMAT HASH 60e83f76b1d74f4ca4494195e80906a5 */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinsebackreservoir.cpp
 *
 */

#include "cobeconfig.h"
#include "procdata.h"
#include "plsrinsebackreservoir.h"

DEFINE_STATE(PlsRinsebackReservoir);

PlsRinsebackReservoir::PlsRinsebackReservoir()
   : StateAbs()
{}

PlsRinsebackReservoir::~PlsRinsebackReservoir()
{}

int PlsRinsebackReservoir::transitionStatus ()
{
   ProcData pd;

   if ( pd.PlasmaRinseback().Status.Get() & ProcPlsRinseback_CDS::Reservoir )
   {
      return 1;
   }

   if ( pd.TrimaSet().Cassette.Get() != cassette::RBCPLS )
      return 2;

   if (    ( pd.Volumes().VRinseRes > 10.0f )
           || ( pd.Status().ReservoirIsFull() )
           )
   {
      ProcPlsRinseback_CDS::PlsRinsebackSubstateComplete status = pd.PlasmaRinseback().Status.Get();
      pd.PlasmaRinseback().Status.Set( (ProcPlsRinseback_CDS::PlsRinsebackSubstateComplete)(status | ProcPlsRinseback_CDS::Reservoir) );

      DataLog(log_level_proc_rinseback_info) << "PlsRinsebackReservoir ended at a VRinseRes volume of 10.0f or full reservoir" << endmsg;
      return 1;
   }

   return NO_TRANSITION;
}

int PlsRinsebackReservoir::preProcess ()
{
   ProcData pd;

   //
   //  Check to see if we have processed enough volume.  Before this point
   //  we always return assuming a ratio of 5 to avoid safety shutdowns.
   //
   float ratio = ( pd.Volumes().VinTotal.Get() > 325.0f ) ? pd.Run().LastRunRatio.Get() : 5.0f;
   if ( ratio < 5.0f ) ratio = 5.0f;

   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);

   float Hin = pd.Run().AdjustedHct.Get() * ( 1.0f - 1.0f / ratio );
   float qp  = CobeConfig::data().RinsebackQinrb * ( 1.0f - Hin );

   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qplasma(qp);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().RPM(0.0f);


   return NORMAL;
}

int PlsRinsebackReservoir::postProcess ()
{
   ProcData pd;

   //
   // Accumulate volume for the plasma pump
   //
   pd.MakeVolumesWritable();
   pd.Volumes().VRinseRes.Set(pd.Volumes().VRinseRes.Get() + pd.Status().PlasmaPump.dV() );
   pd.MakeVolumesReadable();

   return NORMAL;
}

int PlsRinsebackReservoir::preEnter ()
{
   ProcData pd;
   DataLog(log_level_proc_rinseback_info) << "PlsRinsebackReservoir started PlasmaVolume=" << pd.Volumes().PlasmaVolume.Get() << endmsg;

   return NORMAL;
}


int PlsRinsebackReservoir::postExit ()
{
   return NORMAL;
}

/* FORMAT HASH a853ba5a0ebd00a76619c477f718a2fc */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinsebackpurge.cpp
 *
 */

#include "cobeconfig.h"
#include "procdata.h"
#include "plsrinsebackpurge.h"

DEFINE_STATE(PlsRinsebackPurge);

PlsRinsebackPurge::PlsRinsebackPurge()
   : StateAbs(),
     _Finished(false)
{}

PlsRinsebackPurge::~PlsRinsebackPurge()
{}

int PlsRinsebackPurge::transitionStatus ()
{
   ProcData pd;

   if ( pd.PlasmaRinseback().Status.Get() & ProcPlsRinseback_CDS::Purge )
   {
      return 1;
   }

   if (    pd.TrimaSet().Cassette.Get() == cassette::PLTPLSRBC
           && pd.Volumes().PlasmaVolume <= 0.0f )
      return 2;

   if ( pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC )
      return 3;

   return _TransitionStatus;
}

int PlsRinsebackPurge::preProcess ()
{
   ProcData pd;

   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);

   pd.Orders().Qac(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qrp(0.0f);
   pd.Orders().RPM(0.0);

   if ( !_Finished )
   {
      pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_OPEN);

      if ( pd.Status().PlasmaValve.IsOpen() )
      {
         float deltaV = CobeConfig::data().RinsebackVcpurge - CobeConfig::data().RinsebackVplsdecel;
         if ( deltaV <= 0.0f )
            deltaV = CobeConfig::data().RinsebackVcpurge;

         if ( pd.Volumes().VRinsePurge.Get() < deltaV )
            pd.Orders().Qcollect(CobeConfig::data().RinsebackQcpurge);
         else
            pd.Orders().Qcollect(CobeConfig::data().RinsebackQcdecel);
      }

      pd.Orders().RPM(CobeConfig::data().RPMRinsebackSpillAndPurge);
   }
   else
   {
      if ( pd.Status().PlasmaValve.IsReturning() )
      {
         _TransitionStatus = 1;

         ProcPlsRinseback_CDS::PlsRinsebackSubstateComplete status = pd.PlasmaRinseback().Status.Get();
         pd.PlasmaRinseback().Status.Set( (ProcPlsRinseback_CDS::PlsRinsebackSubstateComplete)(status | ProcPlsRinseback_CDS::Purge) );
      }
   }

   return NORMAL;
}

int PlsRinsebackPurge::postProcess ()
{
   ProcData pd;

   //
   // Accumulate plasma volume
   pd.MakeVolumesWritable();
   pd.Volumes().VRinsePurge.Set(pd.Volumes().VRinsePurge.Get() + pd.Status().CollectPump.dV() );
   pd.MakeVolumesReadable();

   //
   // Accumulate to the set
   //
   float totalVolume = pd.PlasmaRinseback().Vtotal.Get();
   totalVolume += pd.Status().CollectPump.dV();
   pd.PlasmaRinseback().Vtotal.Set(totalVolume);
   float acVolume = pd.PlasmaRinseback().Vac.Get();
   acVolume += ( pd.PlasmaRinseback().Facb.Get() * pd.Status().CollectPump.dV() );
   pd.PlasmaRinseback().Vac.Set(acVolume);

   if ( pd.Volumes().VRinsePurge.Get() > CobeConfig::data().RinsebackVcpurge )
   {
      DataLog(log_level_proc_rinseback_info) << "PlsRinsebackPurge ended at max volume of " << pd.Volumes().VRinsePurge.Get() << endmsg;
      _Finished = true;
   }

   // Check for full reservoir
   if ( pd.Status().ReservoirIsFull() )
   {
      DataLog(log_level_proc_rinseback_info) << "PlsRinsebackPurge ended with reservoir full" << endmsg;
      _Finished = true;
   }

   return NORMAL;
}

int PlsRinsebackPurge::preEnter ()
{
   ProcData pd;

   //
   //   Log pertinant information once
   //
   DataLog(log_level_proc_rinseback_info) << "PlsRinsebackPurge started at PlasmaBag Volume = "
                                          << pd.Volumes().PlasmaVolume << endmsg;

   return NORMAL;
}

int PlsRinsebackPurge::postExit ()
{
   return NORMAL;
}

/* FORMAT HASH c3fc1143f22f6a4bfa50ebabd5071d7e */

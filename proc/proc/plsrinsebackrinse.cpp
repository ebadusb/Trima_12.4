/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinsebackrinse.cpp
 *
 */

#include "cobeconfig.h"
#include "procdata.h"
#include "plsrinsebackrinse.h"
#include "volumeacc.h"

DEFINE_STATE(PlsRinsebackRinse);

PlsRinsebackRinse::PlsRinsebackRinse()
   : StateAbs(),
     _Finished(false)
{}

PlsRinsebackRinse::~PlsRinsebackRinse()
{}

int PlsRinsebackRinse::transitionStatus ()
{
   ProcData pd;

   if ( pd.PlasmaRinseback().Status.Get() & ProcPlsRinseback_CDS::Rinse )
   {
      return 1;
   }

   if ( pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC )
   {
      return 2;
   }
   if ( pd.Volumes().PlasmaVolume.Get() <= 0.0f )
   {
      return 3;
   }

   return _TransitionStatus;
}

int PlsRinsebackRinse::preProcess ()
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
   pd.Orders().RPM(0.0f);

   if ( !_Finished )
   {
      pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_OPEN);

      if ( pd.Status().PlasmaValve.IsOpen() )
      {
         float deltaV = CobeConfig::data().RinsebackVcrinse2 - CobeConfig::data().RinsebackVplsdecel;
         if ( deltaV <= 0.0f )
            deltaV = CobeConfig::data().RinsebackVcrinse2;

         if ( pd.Volumes().Vcrinse.Get() < deltaV )
            pd.Orders().Qcollect(CobeConfig::data().RinsebackQcrinse);
         else
            pd.Orders().Qcollect(CobeConfig::data().RinsebackQcdecel);
      }
   }
   else
   {
      if ( pd.Status().PlasmaValve.IsReturning() )
      {
         _TransitionStatus = 1;

         ProcPlsRinseback_CDS::PlsRinsebackSubstateComplete status = pd.PlasmaRinseback().Status.Get();
         pd.PlasmaRinseback().Status.Set( (ProcPlsRinseback_CDS::PlsRinsebackSubstateComplete)(status | ProcPlsRinseback_CDS::Rinse) );

      }
   }

   return NORMAL;
}

int PlsRinsebackRinse::postProcess ()
{
   ProcData pd;

   //
   // Accumulate plasma volume
   pd.MakeVolumesWritable();
   pd.Volumes().Vcrinse.Set(pd.Volumes().Vcrinse.Get() + pd.Status().CollectPump.dV() );
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

   if ( pd.Volumes().RBCVolume.Get() > 0.0f )
   {
      if ( pd.Volumes().Vcrinse.Get() >= CobeConfig::data().RinsebackVcrinse2 )
      {
         DataLog(log_level_proc_rinseback_info) << "PlsRinsebackRinse ended at max volume of " << pd.Volumes().Vcrinse.Get() << endmsg;
         _Finished = true;
      }
   }
   else
   {
      if ( pd.Volumes().Vcrinse.Get() >= CobeConfig::data().RinsebackVcrinse1 )
      {
         DataLog(log_level_proc_rinseback_info) << "PlsRinsebackRinse ended at max volume of " << pd.Volumes().Vcrinse.Get() << endmsg;
         _Finished = true;
      }
   }

   // Check for a full reservoir
   if ( pd.Status().ReservoirIsFull() )
   {
      DataLog(log_level_proc_rinseback_info) << "PlsRinsebackRinse ended with reservoir full" << endmsg;
      _Finished = true;
   }

   return NORMAL;
}

int PlsRinsebackRinse::preEnter ()
{
   ProcData pd;

   //
   //   Log pertinant information once
   //
   DataLog(log_level_proc_rinseback_info) << "PlsRinsebackRinse started at PlasmaBag Volume=" << pd.Volumes().PlasmaVolume.Get() << endmsg;

   //
   //   Zero out the return volume.
   //
   VolumeAcc::ClearReturn();

   return NORMAL;
}

int PlsRinsebackRinse::postExit ()
{
   ProcData pd;

   //
   // Adjust the volume to account for the difference between collected volumes, the pumped volumes and
   //  the experimentally derived flow during plasma rinseback.
   //
   float deltaVol = CobeConfig::data().RinsebackVplrb -
                    ( CobeConfig::data().RinsebackVcrinse1 + CobeConfig::data().RinsebackVcpurge );

   //
   // Accumulate to the set
   //
   float totalVolume = pd.PlasmaRinseback().Vtotal.Get();
   totalVolume += deltaVol;
   pd.PlasmaRinseback().Vtotal.Set(totalVolume);
   float acVolume = pd.PlasmaRinseback().Vac.Get();
   acVolume += ( pd.PlasmaRinseback().Facb.Get() * deltaVol );
   pd.PlasmaRinseback().Vac.Set(acVolume);

   pd.MakeVolumesWritable();
   pd.Volumes().PlasmaVolume.Set(pd.Volumes().PlasmaVolume.Get() - deltaVol);
   pd.MakeVolumesReadable();

   return NORMAL;
}

/* FORMAT HASH 1d47ae9b389bfe7d711db0b080a6afa9 */

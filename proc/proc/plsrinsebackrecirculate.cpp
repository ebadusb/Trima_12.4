/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinsebackrecirculate.cpp
 *
 */

#include "cobeconfig.h"
#include "procdata.h"
#include "plsrinsebackrecirculate.h"
#include "volumeacc.h"

DEFINE_STATE(PlsRinsebackRecirculate);

PlsRinsebackRecirculate::PlsRinsebackRecirculate()
   : StateAbs()
{}

PlsRinsebackRecirculate::~PlsRinsebackRecirculate()
{}

int PlsRinsebackRecirculate::transitionStatus ()
{
   ProcData pd;

   if ( pd.PlasmaRinseback().Status.Get() & ProcPlsRinseback_CDS::Recirculate )
   {
      return 1;
   }

   //
   //
   //   This state is skipped for RBC/Plasma collections
   //
   if ( pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC )
      return 3;

   return _TransitionStatus;
}

int PlsRinsebackRecirculate::preProcess ()
{
   ProcData pd;

   if ( pd.Volumes().RBCVolume.Get() > 0.0f )
      pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   else
      pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_COLLECT);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);

   float Qch = ( CobeConfig::data().RinsebackQrrec + CobeConfig::data().RinsebackQpm ) / 2.0f;
   if (Qch > CobeConfig::data().RinsebackQchrb2) Qch = CobeConfig::data().RinsebackQchrb2;

   float Qc;
   if ( pd.Status().ReservoirIsFull() )
      Qc = CobeConfig::data().RinsebackQrrec;
   else
      Qc = CobeConfig::data().RinsebackQrrec + CobeConfig::data().RinsebackQpm;

   pd.Orders().Qcollect(Qc);
   //
   // Keep the plasma pump running at a reasonable speed
   if ( Qc - Qch >= CobeConfig::data().MinimumPumpRPM )
      pd.Orders().Qplasma(Qc - Qch);
   //
   // ... if not, then command it to 0.0
   else
      pd.Orders().Qplasma(0.0f);

   pd.Orders().Qin(CobeConfig::data().RinsebackQinrec);
   pd.Orders().Qac(0.0f);
   if ( pd.SafetyControlReservoirIsEmpty() )
      pd.Orders().Qrp(0.0f);
   else
      pd.Orders().Qrp(CobeConfig::data().RinsebackQrrec);

   return NORMAL;
}

int PlsRinsebackRecirculate::postProcess ()
{
   ProcData pd;

   pd.MakeVolumesWritable();
   pd.Volumes().VRinseRecirc.Set(pd.Volumes().VRinseRecirc.Get() + pd.Status().InletPump.dV() );
   pd.MakeVolumesReadable();

   bool finished = false;
   if ( pd.SafetyControlReservoirIsEmpty() )
   {
      DataLog(log_level_proc_rinseback_info) << "PlsRinsebackRecirculate ended with an empty reservoir" << endmsg;
      _TransitionStatus = 2;
      finished          = true;
   }
   else if ( pd.Volumes().VRinseRecirc.Get() > CobeConfig::data().RinsebackVinrec )
   {
      DataLog(log_level_proc_rinseback_info) << "PlsRinsebackRecirculate ended at a Vin volume of " << pd.Volumes().VRinseRecirc.Get() << endmsg;
      _TransitionStatus = 1;
      finished          = true;
   }

   if ( finished )
   {
      ProcPlsRinseback_CDS::PlsRinsebackSubstateComplete status = pd.PlasmaRinseback().Status.Get();
      pd.PlasmaRinseback().Status.Set( (ProcPlsRinseback_CDS::PlsRinsebackSubstateComplete)(status | ProcPlsRinseback_CDS::Recirculate) );
   }

   return NORMAL;
}

int PlsRinsebackRecirculate::preEnter ()
{
   ProcData pd;

   //
   //   Zero out the return volume.
   //
   VolumeAcc::ClearReturn();

   //
   //   Log pertinant information once
   //
   DataLog(log_level_proc_rinseback_info) << "PlsRinsebackRecirculate started at VRetRinseback=" << pd.Volumes().VRetRinseback.Get() << endmsg;

   return NORMAL;
}

int PlsRinsebackRecirculate::postExit ()
{
   return NORMAL;
}

/* FORMAT HASH 0e6e37e1f83da5fdfe87f98dde1c857c */

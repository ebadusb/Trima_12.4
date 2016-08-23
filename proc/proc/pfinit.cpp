/*
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pfinit.cpp
 *
 * The PFInit state handles initialization of the power fail data
 * within the Proc class.  The ProcPFRHandler class provides the
 * interface to the power fail save file.
 *
 */

#include <vxworks.h>
#include "pfinit.h"
#include "states.h"
#include "an_alarm.h"
#include "procdata.h"
#include "guiproc.h"
#include "cobeconfig.h"

DEFINE_STATE(PFInit);

PFInit::PFInit()
   : StateAbs(),
     _alreadySentCobeConfigValues(false)
{}

PFInit::PFInit(const PFInit& state)
   : StateAbs(state),
     _alreadySentCobeConfigValues(false)
{}

PFInit::~PFInit()
{
   cleanup();
}

int PFInit::transitionStatus ()
{
   //
   // We need to skip this state if the power-fail restore check
   // (see the preEnter() method) has already been completed, so
   // that we don't restore data twice.
   //
   ProcData pd;
   return (pd.PFRHandler().IsRestoreCheckDone()) ? 1 : NO_TRANSITION;
}

int PFInit::preProcess ()
{
   return NORMAL;
}

int PFInit::postProcess ()
{
   return NORMAL;
}

int PFInit::preEnter ()
{
   ProcData pd;

   /////////////////////////////////////////////////////////////////////////
   // GUI doesn't read the cobe_config.dat file (only proc does), but there
   // is some information that GUI needs from that file.  So, at this point,
   // proc sends over all the cobe_config info that GUI needs.
   //   It is done here because it must happen before the operator has the
   // opportunity to enter the config screens, meaning before GUI allocates
   // the two-button screen.  It didn't seem worthy of a separate substate
   // just to send this one message, and PFR intialization is the first
   // place that made some sense.
   //   Note that this happens unconditionally, whether or not a PFR
   // actually occurs. --D.S. (x4664)
   if (!_alreadySentCobeConfigValues)
   {
      _alreadySentCobeConfigValues = true;

      DataLog (log_level_proc_info) << "Proc sending GUI cobe config struct" << endmsg;

      GUI_COBE_CONFIG_STRUCT cobeConfigStruct;


      cobeConfigStruct.PtfFilterLoss       = CobeConfig::data().mssPtfFilterLoss;
      cobeConfigStruct.pltWbcThresholdConc = CobeConfig::data().ConcMax;
      cobeConfigStruct.rasBagEmptyPercent  = 1.0 - (CobeConfig::data().MssPltSSPercent / 100.0);
      cobeConfigStruct.pasBagEmptyPercent  = 1.0 - (CobeConfig::data().MssPltSSPercent / 100.0);
      cobeConfigStruct.ptfFilterVol        = CobeConfig::data().MssPtfVolume;

      cobeConfigStruct.flowRateMin         = CobeConfig::data().QinLimitMin;
      cobeConfigStruct.flowRateMax         = CobeConfig::data().QinLimitMax;

      cobeConfigStruct.pasPrimeVol         =
         CobeConfig::data().MssPltSubstateMaxPrimeVol +
         CobeConfig::data().MssPLTPrimeFinalVol +
         ( CobeConfig::data().MssVacuumCycles *
           (  CobeConfig::data().MssVac1PlasmaVol +
              CobeConfig::data().MssVac2PlasmaVol +
              CobeConfig::data().MssVac3PlasmaVol ));

      cobeConfigStruct.pasVolBiasPercent     = 1.0f - CobeConfig::data().MssPltSSPercent / 100.0f;
      cobeConfigStruct.pasNoRinsebackPenalty = CobeConfig::data().MssPltSubstateMaxPrimeVol;

      cobeConfigStruct.minPlsRinseVol        = CobeConfig::data().RinsebackVplrb;

      Message<GUI_COBE_CONFIG_STRUCT> guiCobeConfigStruct(MessageBase::SEND_LOCAL);
      guiCobeConfigStruct.send(cobeConfigStruct);
   }
   /////////////////////////////////////////////////////////////////////////

   pd.MakeTrimaSetWritable();
   if ( !pd.Status().CassIsDown() && !pd.Status().CassIsUp() )
   {
      pd.TrimaSet().Cassette.Set(cassette::UNKNOWN);
   }
   else if ( pd.Status().CassIsUp() )
   {
      pd.TrimaSet().Cassette.Set(cassette::NOT_LOADED);
   }
   pd.MakeTrimaSetReadable();

   //
   // Restore power fail data if possible.
   //
   if ( pd.PFRHandler().IsPFDataAvailable() )
   {
      pd.TrimaSet().RestorePfData();
      pd.Orders().CassetteId(pd.TrimaSet().Cassette.Get() );
      if ( pd.TrimaSet().CassetteState.Get() > ProcTrimaSet_CDS::ACPRIMED )
      {
         DataLog(log_level_proc_pfr_info) << "Restore Donor/Vital PFR data" << endmsg;
         pd.PFRHandler().RestoreVitalData();
      }
      else
      {
         DataLog(log_level_proc_pfr_info) << "Don't Restore Donor/Vital PFR data" << endmsg;
         pd.PFRHandler().ForgetVitalData();
      }
      pd.PFRHandler().RestoreAllData();
      pd.Config().SendUpdateMsg();
   }

   //
   // Mark the restore check done, so we don't try to restore the data
   // a second time.  This is only necessary to handle the PFInit substate
   // within donor disconnect.  If the operator presses pause/stop and
   // then end run before reaching the PFInit substate within PFR, then
   // we need to perform these operations within donor disconnect.
   // Otherwise they should be skipped.
   //
   pd.PFRHandler().MarkRestoreCheckDone();
   return NORMAL;
}

int PFInit::postExit ()
{
   return NORMAL;
}

void PFInit::cleanup ()
{}

/* FORMAT HASH 4c19ec28cb0364989c7d46beb91692ac */

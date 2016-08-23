/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      VacPurgeFinal.cpp
 *
 */

#include "pasFinalFlowVerification.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"



DEFINE_STATE(PasFinalFlowVerification);


const float StopVol      =  CobeConfig::data().MssPostPurgeFillPlasmaVolume;
const float PLTPumpSpeed =  CobeConfig::data().MssPostPurgeFillCollectSpeed;
const float PLSPumpSpeed =  CobeConfig::data().MssPostPurgeFillPlasmaSpeed;


PasFinalFlowVerification::PasFinalFlowVerification()
   : StateAbs(),
     transitionNow(false),
     plasmaVol(0.0f),
     initPlasmaPumpVolume(0.0f),
     fluidTooLateAlert(MSS_PRIME_DELAY_PLT_ALERT)
{}

PasFinalFlowVerification::PasFinalFlowVerification(const PasFinalFlowVerification& state)
   : StateAbs(state),
     transitionNow(false),
     plasmaVol(0.0f),
     initPlasmaPumpVolume(0.0f),
     fluidTooLateAlert(MSS_PRIME_DELAY_PLT_ALERT)
{}

PasFinalFlowVerification::~PasFinalFlowVerification()
{}

int PasFinalFlowVerification::preProcess ()
{

   ProcData pd;
   pd.Orders().Qplasma (PLSPumpSpeed);
   pd.Orders().Qcollect(PLTPumpSpeed);

   return NORMAL;
}

int PasFinalFlowVerification::postProcess ()
{

   ProcData pd;

   float    plasmaVolPumped = pd.Volumes().Vp - initPlasmaPumpVolume;

   if (pd.Status().ReservoirIsFull())
   {
      transitionNow = true;

      DataLog(log_level_proc_info) << "Reservoir filled for PAS final flow test"  << endmsg;
      DataLog(log_level_proc_info) << " fluid pumped to fill reservoir = "  << plasmaVolPumped << "mL" << endmsg;
   }
   else
   {

      if (plasmaVolPumped > StopVol)
      {
         DataLog(log_level_proc_info) << "PAS Final Flow Verification FAILED!"  << endmsg;
         DataLog(log_level_proc_info) << "-- raise alarm and flag for mss volume error"  << endmsg;

         pd.MakeRunWritable();
         pd.Run().flagMSSPASVolumeError.Set(true);
         pd.MakeRunReadable();

         fluidTooLateAlert.setAlarm();

         // alarms a one shot so just transition now
         transitionNow = true;
      }
   }

   return NORMAL;
}

int PasFinalFlowVerification::transitionStatus ()
{
   ProcData pd;

   // if we're not doing MSS for PLT then get the heck out of here
   if (!(pd.RunTargets().MeterPlatelets && pd.Run().MeterPlatelets_OK))
   {
      return 1;
   }

   // if we did'nt do rinseback skip this is salvage case
   // just move the valve at system speed.
   if (!pd.Run().rinsebackVolumeSuccessMss.Get() )
   {
      DataLog(log_level_proc_info) << "Skipping out because rinseback not entered!"   << endmsg;
      return 1;
   }

   // check if PFR and we need to do this yet!
   if (pd.Run().mssInPFR)  // if we're in PFR
   {
      if ( (pd.Run().currentMSSSubState ==  SS_MSSPARALLELPROC )
           && ( pd.Run().currentPLTMSSSubState >  SS_PASFINALFLOWVERIFICATION  )  )
      {
         return 1;
      }
   }

   // exit when we've completed
   if (transitionNow)
   {
      return 1;
   }

   return NO_TRANSITION;
}

int PasFinalFlowVerification::preEnter ()
{
   ProcData pd;

   if (!pd.Run().mssInPFR)  // if we're in PFR
   {
      pd.MakeRunWritable();
      pd.Run().currentPLTMSSSubState = SS_PASFINALFLOWVERIFICATION;
      pd.MakeRunReadable();

   }
   else
   {
      if (pd.Run().currentPLTMSSSubState.Get() == SS_PASFINALFLOWVERIFICATION)
      {
         DataLog(log_level_proc_info) << "Back to PFR point, continuing as non-PFR" << endmsg;
         // back to where we PFR'ed reset flag and carry on
         pd.MakeRunWritable();
         pd.Run().mssInPFR.Set(false);
         pd.MakeRunReadable();

      }

   }


   if (pd.Status().ReservoirIsFull())
   {
      transitionNow = true;

      pd.MakeRunWritable();
      pd.Run().SkipPauseValveInMSSPurge.Set(true);
      pd.MakeRunReadable();

      DataLog(log_level_proc_info) << "Reservoir fill at start of PAS final flow test - no collect valve pause"  << endmsg;
   }

   // set the starting volume number for diff calc
   initPlasmaPumpVolume = pd.Volumes().Vp;

   return NORMAL;
}

int PasFinalFlowVerification::postExit ()
{

   ProcData pd;

   pd.Orders().Qplasma (0.0);
   pd.Orders().Qcollect(0.0);
   pd.Orders().Qin(0.0);
   pd.Orders().Qrp(0.0);

   transitionNow = false ;

   return NORMAL;
}

void PasFinalFlowVerification::copyOver (const PasFinalFlowVerification&)
{}

void PasFinalFlowVerification::cleanup ()
{}

/* FORMAT HASH c3d682772155ba58fd9d64b236d51384 */

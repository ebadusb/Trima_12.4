/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      VacPurgeFinal.cpp
 *
 */

#include "vacPurgeFinal.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"


// time valva hangs in the open position
const int VALVE_SWING_TIME = int(1000.0f * CobeConfig::data().MssPLTValveTimer); // in millisecs of course (



DEFINE_STATE(VacPurgeFinal);

VacPurgeFinal::VacPurgeFinal()
   : StateAbs(),
     transitionNow(false),
     plateletVol(0.0f),
     plasmaVol(0.0f),
     valveTimer(0, Callback<VacPurgeFinal>(this, &VacPurgeFinal::moveValve), TimerMessage::DISARMED)
{}

VacPurgeFinal::VacPurgeFinal(const VacPurgeFinal& state)
   : StateAbs(state),
     transitionNow(false),
     plateletVol(0.0f),
     plasmaVol(0.0f),
     valveTimer(0, Callback<VacPurgeFinal>(this, &VacPurgeFinal::moveValve), TimerMessage::DISARMED)
{
   copyOver(state);
}

VacPurgeFinal::~VacPurgeFinal()
{
   cleanup();
}

int VacPurgeFinal::preProcess ()
{
   return NORMAL;
}

int VacPurgeFinal::postProcess ()
{

   ProcData pd;

   DataLog(log_level_proc_info) << "Moving Valve to OPEN"   << endmsg;
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_OPEN);

   pd.MakeRunWritable();
   pd.Run().currentPLTMSSSubState = INVALID_SUBSTATE;    // if PFR now we must scrap the MSS run
   pd.MakeRunReadable();


   if (notArmed)
   {

      notArmed = false;

      // when the time goes off move the valve to collect
      valveTimer.interval(VALVE_SWING_TIME);
   }

   return NORMAL;
}

int VacPurgeFinal::transitionStatus ()
{

   ProcData pd;


   // if we're not doing MSS for PLT then get the heck out of here
   if (!(pd.RunTargets().MeterPlatelets && pd.Run().MeterPlatelets_OK))
   {
      return 1;
   }



   // if we did'nt do rinseback skip this is salvage case
   // just move the valve at system speed.
   if (!pd.Run().rinsebackVolumeSuccessMss.Get() || pd.Run().SkipPauseValveInMSSPurge.Get() )
   {

      DataLog(log_level_proc_info) << "Moving Valve to COLLECT"   << endmsg;
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_COLLECT);

      // Send a message to GUI to tell it that we have completed Vac Purge.
      MSSVacPurgeCompleteMsg _completeVP(MessageBase::SEND_GLOBAL);

      _completeVP.send(0);

      if (pd.Run().SkipPauseValveInMSSPurge.Get())
         DataLog(log_level_proc_info) << "Skipping out because res is full!"   << endmsg;
      else
         DataLog(log_level_proc_info) << "Skipping out because rinseback not entered!"   << endmsg;

      return 1;
   }

   // check if PFR and we need to do this yet!
   if (pd.Run().mssInPFR)   // if we're in PFR
   {
      if ( (pd.Run().currentMSSSubState ==  SS_MSSPARALLELPROC )
           && ( pd.Run().currentPLTMSSSubState >  SS_MSSVACPURGEFINAL  )  )
      {

         // Send a message to GUI to tell it that we have completed Vac Purge.
         MSSVacPurgeCompleteMsg _completeVP(MessageBase::SEND_GLOBAL);
         _completeVP.send(0);
         return 1;
      }
   }

   // exit when we've completed
   if (transitionNow)
   {

      pd.Orders().Qplasma (0.0f);
      pd.Orders().Qcollect(0.0f);

      // Send a message to GUI to tell it that we have completed Vac Purge.
      MSSVacPurgeCompleteMsg _completeVP(MessageBase::SEND_GLOBAL);
      _completeVP.send(0);

      DataLog(log_level_proc_info) << "Storage Solution Remaining in Bag =>"  << pd.Volumes().VpltStorageRemaining << endmsg;
      DataLog(log_level_proc_info) << "Purge Final Platelet Volm Pumped =>"  << pd.Volumes().Vc - InitialPlateletVol << endmsg;
      DataLog(log_level_proc_info) << "Purge Final Plasma Volm Pumped   =>"  << pd.Volumes().Vp - InitialPlasmaVol << endmsg;

      return 1;
   }

   return NO_TRANSITION;
}

int VacPurgeFinal::preEnter ()
{
   ProcData pd;

   if (!pd.Run().mssInPFR)  // if we're in PFR
   {

      pd.MakeRunWritable();
      pd.Run().currentPLTMSSSubState = SS_MSSVACPURGEFINAL;
      pd.MakeRunReadable();

   }
   else
   {
      if (pd.Run().currentPLTMSSSubState.Get() == SS_MSSVACPURGEFINAL)
      {
         DataLog(log_level_proc_info) << "Back to PFR point, continuing as non-PFR" << endmsg;
         // back to where we PFR'ed reset flag and carry on
         pd.MakeRunWritable();
         pd.Run().mssInPFR.Set(false);
         pd.MakeRunReadable();
      }
   }

   notArmed = true;
   return NORMAL;
}

int VacPurgeFinal::postExit ()
{

   ProcData pd;


   valveTimer.interval(0);   // disable timer

   pd.MakeRunWritable();
   pd.Run().currentPLTMSSSubState = SS_MSSVACPURGEFINAL;   // set this for next state
   pd.MakeRunReadable();

   transitionNow = false ;

   return NORMAL;
}

void VacPurgeFinal::copyOver (const VacPurgeFinal&)

{}

void VacPurgeFinal::cleanup ()
{}

void VacPurgeFinal::moveValve ()
{

   ProcData pd;

   // set to collect to recieve SS next

   DataLog(log_level_proc_info) << "Moving Valve to COLLECT"   << endmsg;
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_COLLECT);

   transitionNow = true;


}

/* FORMAT HASH ad14af98e436ac2cbae6825c96d79245 */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PreRinseback.cpp
 *
 */

#include <vxworks.h>
#include "cobeconfig.h"
#include "prerinseback.h"
#include "procdata.h"
#include "MssCommon.h"

DEFINE_STATE(PreRinseback);

PreRinseback::PreRinseback()
   : StateAbs(),
     _Timer(0, Callback<PreRinseback>(this, &PreRinseback::timeout), TimerMessage::DISARMED),
     _TimerExpired(false)
{}

PreRinseback::PreRinseback(const PreRinseback& state)
   : StateAbs(state),
     _Timer(0, Callback<PreRinseback>(this, &PreRinseback::timeout), TimerMessage::DISARMED),
     _TimerExpired(false)
{
   PreRinseback::copyOver(state);
}

PreRinseback::~PreRinseback()
{
   PreRinseback::cleanup();
}

int PreRinseback::transitionStatus ()
{
   if ( _TimerExpired )
   {
      ProcData pd;

      // SALINE RINSEBACK
      if (   ( pd.Config().Procedure.Get().key_saline_rinseback == 1 )
             && ( pd.TrimaSet().Cassette.Get() == cassette::RBCPLS )
             )
      {

         DataLog(log_level_proc_rinseback_info) << "Saline Rinseback" <<   endmsg;
         return 3;
      }

      // PLASMA RINSEBACK
      else if ( pd.Config().Procedure.Get().key_plasma_rinseback == 1                   // Plasma rinseback configured
                && pd.Volumes().PlasmaVolume.Get() >= CobeConfig::data().RinsebackVplrb // Volume of plasma required for plasma rinseback
                && pd.TrimaSet().Cassette.Get() == cassette::PLTPLSRBC                  // Only on platelet collection sets
                )
      {

         DataLog(log_level_proc_rinseback_info) << "Plasma Rinseback" <<   endmsg;
         return 2;
      }

      // NORMAL RINSEBACK
      else
      {

         DataLog(log_level_proc_rinseback_info) << "Normal Rinseback" <<   endmsg;
         return 1;
      }

   }

   return NO_TRANSITION;
}

int PreRinseback::preProcess ()
{
   return NORMAL;
}

int PreRinseback::postProcess ()
{
   return NORMAL;
}

int PreRinseback::preEnter ()
{
   ProcData pd;

   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(0.0f);

   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);

   if (pd.Volumes().RBCDose.Get() > 0.0)
   {
      DataLog(log_level_proc_rinseback_info) << "RBC Product Info::::" <<   endmsg;
      DataLog(log_level_proc_rinseback_info) << "Total RBC Dose     ->" << pd.Volumes().RBCDose.Get() << endmsg;
      DataLog(log_level_proc_rinseback_info) << "Total RBC Volume   ->" << pd.Volumes().RBCVolume.Get() << endmsg;
      DataLog(log_level_proc_rinseback_info) << "Total RBC AC Volm  ->" << pd.Volumes().VacRBCBag.Get() << endmsg;

      DataLog(log_level_proc_rinseback_info) << "Prod 1 RBC Dose    ->" << pd.Volumes().RBCProduct_A_FinalDose.Get() << endmsg;
      DataLog(log_level_proc_rinseback_info) << "Prod 1 RBC Volume  ->" << pd.Volumes().RBCProduct_A_FinalVolume.Get() << endmsg;
      DataLog(log_level_proc_rinseback_info) << "Prod 1 RBC AC Volm ->" << pd.Volumes().RBCProduct_A_FinalVac.Get() << endmsg;

      DataLog(log_level_proc_rinseback_info) << "Prod 2 RBC Dose    ->" << pd.Volumes().RBCProduct_B_FinalDose.Get() << endmsg;
      DataLog(log_level_proc_rinseback_info) << "Prod 2 RBC Volume  ->" << pd.Volumes().RBCProduct_B_FinalVolume.Get() << endmsg;
      DataLog(log_level_proc_rinseback_info) << "Prod 2 RBC AC Volm ->" << pd.Volumes().RBCProduct_B_FinalVac.Get() << endmsg;
   }
   //
   // Set up the timer ...
   //
   _TimerExpired = false ;
   _Timer.interval(3000);   // 3 seconds ( 3000 msecs )

   //
   //
   //  If we managed to get here before rinseback should really be
   //  available then we should go to donor disconnect instead.
   //  Request the transition (which should happen while we are
   //  waiting for the pumps to stop...)
   //
   if (!pd.Run().FirstDrawComplete.Get())
   {
      DataLog(log_level_proc_rinseback_info) << "Rinseback disallowed before first draw complete" << endmsg;
      CancelProcedureMsg procHint(MessageBase::SEND_LOCAL);
      procHint.send(1);
   }

   return NORMAL;
}

int PreRinseback::postExit ()
{
   //
   // Stop the timer ...
   //
   _Timer.interval(0);
   _TimerExpired = false;
   cleanup();
   return NORMAL;
}

void PreRinseback::copyOver (const PreRinseback&)
{}

void PreRinseback::cleanup ()
{
   //
   // Delete the dynamic memory ...
   //
}

void PreRinseback::timeout ()
{
   _TimerExpired = true;
}

/* FORMAT HASH eb0ea830f8a78f812e5800fa3777e7c9 */

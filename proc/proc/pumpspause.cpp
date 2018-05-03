/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pumpspause.cpp
 *
 */

#include <periodic_log.h>
#include "pumpspause.h"
#include "cobeconfig.h"
#include "procdata.h"

DEFINE_OBJ(PumpsPause);

const float inletSpeedForce = 40.0;

PumpsPause::PumpsPause()
   : RecoveryAbs(),
     _RecoveryTimer(0, Callback<PumpsPause>(this, &PumpsPause::pumpsPaused), TimerMessage::DISARMED),
     _TimerStarted(0),
     _PerformRecovery(0),
     _InterfaceEstablished(0),
     _InletVolume(0.0f)
{}

PumpsPause::PumpsPause(const PumpsPause& rec)
   : RecoveryAbs(rec),
     _RecoveryTimer(0, Callback<PumpsPause>(this, &PumpsPause::pumpsPaused), TimerMessage::DISARMED),
     _TimerStarted(0),
     _PerformRecovery(0),
     _InterfaceEstablished(0),
     _InletVolume(0.0f)
{
   copyOver(rec);
}

PumpsPause::~PumpsPause()
{
   cleanup();
}

int PumpsPause::init ()
{
   ProcData pd;
   pd.MakeRunWritable();
   pd.Run().inPPR.Set(false);
   pd.MakeRunReadable();

   return 1;
}

void PumpsPause::pumpsPaused ()
{
   ProcData pd;

   DataLog(log_level_proc_info) << "PumpsPause: Timer Ended with PPR to start " << pd.GetAbsTimeNowinMinutes() << endmsg;
   _PerformRecovery = 1;
   _RecoveryTimer.interval(0);
   _TimerStarted    = 0;

   pd.MakeRunWritable();
   pd.Run().inPPR.Set(true);

}

int PumpsPause::doRecovery ()
{
   ProcData pd;

   //
   // We are doing the recovery so reset the flag ...
   //
   _PerformRecovery = 0;

   //
   // Keep the valves from moving ...
   //
   pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_RETURN);
   //
   //
   //   Set the other valves to the current position.  In case the valve is in the unknown position (IT10906)
   //   choose the return position (it's either coming from return or moving to return, so either way
   //   that is an appropriate choice)
   //
   HAL_VALVESTATE collectcmd = pd.Status().CollectValve.State();
   if ( collectcmd == HAL_VALVE_UNKNOWN )
   {
      DataLog(log_level_proc_info) << "PumpsPause: collect valve position overridden from unknown to return" << endmsg;
      collectcmd = HAL_VALVE_RETURN;
   }
   pd.Orders().ValveCmd(HAL_VCOLLECT, collectcmd);


   HAL_VALVESTATE plasmacmd = pd.Status().PlasmaValve.State();
   if ( plasmacmd == HAL_VALVE_UNKNOWN )
   {
      DataLog(log_level_proc_info) << "PumpsPause: plasma valve position overridden from unknown to return" << endmsg;
      plasmacmd = HAL_VALVE_RETURN;
   }
   pd.Orders().ValveCmd(HAL_VPLASMA, plasmacmd);


   if ( pd.Status().InletPump.ActRPM() < CobeConfig::data().MinimumPumpRPM )
   {
      //
      // Start volume processed over ...
      _InletVolume = 0.0f;
   }
   else
   {
      _InletVolume += pd.Status().InletPump.dV();
   }

   //
   // Keep the collect and plasma pumps paused ...
   //
   pd.Orders().Qcollect(CobeConfig::data().PauseQCollect);
   pd.Orders().Qplasma(CobeConfig::data().PauseQPlasma);

   // no collect data
   float nc_Qin = pd.Predict().Qin(/* DrawCycle == */ false, SubstateMap::alg(NOCOLLECT_STATE));
   float nc_Qrp = pd.Predict().Qrp(/* DrawCycle == */ false, SubstateMap::alg(NOCOLLECT_STATE));
   float nc_Qac = pd.Predict().Qac(/* DrawCycle == */ false, SubstateMap::alg(NOCOLLECT_STATE));

   // no collect needle flow
   float nc_Qn = nc_Qrp - nc_Qin;
   float Qret  = nc_Qn + inletSpeedForce;

   // dont exceed no collect infusion rate (Qret)
   if (Qret > nc_Qrp)
   {
      Qret = nc_Qrp;
   }

    DataLog(log_level_proc_info) << "PumpsPause: Qn set to "      << nc_Qn
                                 << ",  PumpsPause: Qret set to " << Qret
                                 << ",  PumpsPause: Qin set to "  << inletSpeedForce << endmsg;


   if (Qret > nc_Qrp)
   {
      Qret = nc_Qrp;
   }

   if (pd.Run().DrawCycle.Get())
   {
      float ratio = pd.Config().Procedure.Get().key_custom_ratio;
      pd.Orders().Qin(inletSpeedForce);

      if (ratio != 0.0)
      {
         pd.Orders().Qac(inletSpeedForce / ratio);
      }
   }

   else /* return */
   {
      pd.Orders().Qin(inletSpeedForce);
      pd.Orders().Qrp(Qret);
   }

   PeriodicLog::forceOutput();

   //
   // If we have processed enough volume ...
   //
   if ( _InletVolume >= CobeConfig::data().PauseVinProcessedTarget )
   {
      _InterfaceEstablished = 1;

      DataLog(log_level_proc_info) << "PumpsPause: end " << pd.GetAbsTimeNowinMinutes() << endmsg;

      pd.MakeRunWritable();
      pd.Run().inPPR.Set(false);
   }

   return 1;
}

int PumpsPause::mayEnter () const
{
   ProcData pd;

   if ( pd.Status().CassIsPltPlsRBC() &&
        pd.Run().Substate.Get() < SS_RBC_PTF_SETUP_1 &&
        pd.Status().RBCValve.IsReturning() )
   {
      return 1;
   }
   return 0;
}

int PumpsPause::shouldRecover ()
{
   if ( _PerformRecovery ||
        ( active() && !_InterfaceEstablished )
        )
   {
      return 1;
   }

   ProcData pd;
   if ( pd.Status().InletPump.ActRPM() < CobeConfig::data().MinimumPumpRPM )
   {
      if (!_TimerStarted )
      {
         //
         // Maximum pause time (seconds to milliseconds)
         DataLog(log_level_proc_info) << "PumpsPause: Timer Started " << pd.GetAbsTimeNowinMinutes() << endmsg;
         _RecoveryTimer.interval( (CobeConfig::data().ApsPauseTimeBeforeAlarm * 1000) );
         _TimerStarted = 1;
      }
   }
   else  // Pump speed is above minimum RPM
   {
      //
      // Stop the timer ...
      //
      if (_TimerStarted)
         DataLog(log_level_proc_info) << "PumpsPause: Timer Ended " << pd.GetAbsTimeNowinMinutes() << endmsg;

      _RecoveryTimer.interval(0);
      _TimerStarted = 0;
   }

   //
   // Reset the interface established flag and
   //  the volume processed ...
   //
   _InterfaceEstablished = 0;
   _InletVolume          = 0.0f;

   return 0;
}

void PumpsPause::reset ()
{
   ProcData pd;
   _RecoveryTimer.interval(0);
   _TimerStarted         = 0;
   _PerformRecovery      = 0;
   _InterfaceEstablished = 0;
   _InletVolume          = 0;

   pd.MakeRunWritable();
   pd.Run().inPPR.Set(false);
   pd.MakeRunReadable();
}

void PumpsPause::copyOver (const PumpsPause&)
{}

void PumpsPause::cleanup ()
{
   ProcData pd;
   pd.MakeRunWritable();
   pd.Run().inPPR.Set(false);
   pd.MakeRunReadable();
}

/* FORMAT HASH c71186bcafd788b3b6a245e73e488b25 */

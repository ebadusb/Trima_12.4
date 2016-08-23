/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      startpumps.cpp
 *
 */

#include <math.h>
#include "cobeconfig.h"
#include "procdata.h"
#include "startpumps.h"

const float DELTA_SPEED = 5.0f;

DEFINE_STATE(StartPumps);

StartPumps::StartPumps()
   : StateAbs(),
     _Finished(false),
     _StartPumpsTimer(0, Callback<StartPumps>(this, &StartPumps::pumpsNotStarted), TimerMessage::DISARMED),
     _TimerStarted(0),
     _nPumps(0)
{}

StartPumps::StartPumps(const StartPumps& state)
   : StateAbs(state),
     _Finished(false),
     _StartPumpsTimer(0, Callback<StartPumps>(this, &StartPumps::pumpsNotStarted), TimerMessage::DISARMED),
     _TimerStarted(0),
     _nPumps(0)
{
   copyOver(state);
}

StartPumps::~StartPumps()
{
   cleanup();
}

int StartPumps::transitionStatus ()
{
   if ( _Finished )
   {
      DataLog(log_level_proc_debug) << "Finished" << endmsg;
      return 1;
   }
   return NO_TRANSITION;
}

void StartPumps::pumpsNotStarted ()
{
   //
   // Set the pump alarm
   //
   anAlarmMsg pumpAlarm(PUMP_RPM_COMMAND_FAILURE);
   pumpAlarm.setAlarm();
   DataLog(log_level_proc_debug) << "Set the pump command failure alarm " << PUMP_RPM_COMMAND_FAILURE << endmsg;

   //
   // Stop the timer ...
   //
   _StartPumpsTimer.interval(0);
   DataLog(log_level_proc_debug) << "Stop the start pumps timer" << endmsg;

   reset();
}

int StartPumps::preProcess ()
{
   ProcData pd;
   if (    !pd.AlarmActive()
           && pd.Status().CentrifugeRPM() < CobeConfig::data().CentrifugeOpenSpeed )
   {
      //
      // Command all the pumps to turn ...
      //
      startPumps();
   }
   else
   {
      reset();
   }

   return NORMAL;
}

int StartPumps::postProcess ()
{
   ProcData pd;
   //
   // When all pumps have started we are through ...
   //
   if (    (_nPumps > 3)
           && ( pd.Orders().Qin()         > 0.0f && fabs(pd.Status().InletPump.ActFlow()  - pd.Orders().Qin() )      <= DELTA_SPEED )
           && ( pd.Orders().Qac()         > 0.0f && fabs(pd.Status().ACPump.ActFlow()     - pd.Orders().Qac() )      <= DELTA_SPEED )
           && ( pd.Orders().Qplasma()     > 0.0f && fabs(pd.Status().PlasmaPump.ActFlow() - pd.Orders().Qplasma() )  <= DELTA_SPEED )
           && ( pd.Orders().Qcollect()    > 0.0f && fabs(pd.Status().CollectPump.ActFlow() - pd.Orders().Qcollect() ) <= DELTA_SPEED )
           && ( fabs(pd.Orders().Qrp() ) > 0.0f && fabs(pd.Status().ReturnPump.ActFlow() - pd.Orders().Qrp() )      <= DELTA_SPEED )
           )
   {
      DataLog(log_level_proc_debug) << "Pumps up to speed - AC: " << pd.Status().ACPump.ActFlow() <<
         " Inlet: " << pd.Status().InletPump.ActFlow() <<
         " Return: " << pd.Status().ReturnPump.ActFlow() <<
         " Collect: " << pd.Status().CollectPump.ActFlow() <<
         " Plasma: " << pd.Status().PlasmaPump.ActFlow() << endmsg;
      _Finished = true;
   }
   return NORMAL;
}

int StartPumps::preEnter ()
{

   return NORMAL;
}

int StartPumps::postExit ()
{
   //
   // Stop the timer ...
   //
   _StartPumpsTimer.interval(0);
   DataLog(log_level_proc_debug) << "Stop the start pumps timer" << endmsg;

   //
   // If not finished with the pump sequence then stop them all ...
   //
   if ( !_Finished )
      stopPumps();

   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void StartPumps::startPumps ()
{
   ProcData pd;

   if ( !_TimerStarted )
   {
      _StartPumpsTimer.interval(10000);   // 10 seconds
      DataLog(log_level_proc_debug) << "Start the start pumps timer" << endmsg;
      _TimerStarted = 1;
      _nPumps       = 0;
   }

   stopPumps();



   const float QrpLoad    =  110.0f;
   const float QrpUnload  =  150.0f;
   const float QinLoad    =  140.0f;
   const float QinUnload  =   60.0f;

   float       currentQrp = 60.0f;
   float       currentQin = 60.0f;

   // after DonorDisco
   if (pd.Run().DonorDisconnectTime.Get() > 0.0f)
   {
      currentQrp =  QrpUnload;
      currentQin =  QinUnload;
   }
   else
   {
      currentQrp = QrpLoad;
      currentQin = QinLoad;
   }

   //
   // Command all the pumps to turn ...
   //
   if ( pd.TrimaSet().CassetteState.Get() == ProcTrimaSet_CDS::DRY ||
        pd.TrimaSet().CassetteState.Get() == ProcTrimaSet_CDS::TESTED)
   {
      pd.Orders().Qrp(currentQrp);
   }
   else
   {
      pd.Orders().Qrp(-currentQrp);
   }
   if (_nPumps > 0) pd.Orders().Qac(60.0f);
   if (_nPumps > 1) pd.Orders().Qin(currentQin);
   if (_nPumps > 2) pd.Orders().Qplasma(60.0f);
   if (_nPumps > 3) pd.Orders().Qcollect(60.0f);

   ++_nPumps; // can't overflow before timeout

   DataLog(log_level_proc_debug) << "Command pumps - AC: " << pd.Orders().Qac() <<
      " Inlet: " << pd.Orders().Qin() <<
      " Return: " << pd.Orders().Qrp() <<
      " Collect: " << pd.Orders().Qcollect() <<
      " Plasma: " << pd.Orders().Qplasma() << endmsg;
}

void StartPumps::stopPumps ()
{
   ProcData pd;
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(0.0f);

}

void StartPumps::reset ()
{
   //
   // Reset the flags ...
   //
   _Finished     = false;
   _TimerStarted = 0;
   _nPumps       = 0;

   //
   // reset the timer
   //
   _StartPumpsTimer.interval(0);
}

void StartPumps::copyOver (const StartPumps&)
{}

void StartPumps::cleanup ()
{}

/* FORMAT HASH d2e1c77fb5cf9c6b3e2bc788a7636d30 */

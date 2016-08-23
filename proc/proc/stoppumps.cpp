/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      stoppumps.cpp
 *
 */

#include "stoppumps.h"
#include "procdata.h"
#include "cobeconfig.h"

DEFINE_STATE(StopPumps);

StopPumps::StopPumps()
   : StateAbs(),
     _Finished(0),
     _StopPumpsTimer(0, Callback<StopPumps>(this, &StopPumps::pumpsNotStopped), TimerMessage::DISARMED),
     _CPSZeroOutsideRange(CPS_OUT_OF_RANGE_AT_SET_LOWER)
{}

StopPumps::StopPumps(const StopPumps& state)
   : StateAbs(state),
     _Finished(state._Finished),
     _StopPumpsTimer(0, Callback<StopPumps>(this, &StopPumps::pumpsNotStopped), TimerMessage::DISARMED),
     _CPSZeroOutsideRange(CPS_OUT_OF_RANGE_AT_SET_LOWER)
{}

StopPumps::~StopPumps()
{}

int StopPumps::transitionStatus ()
{
   if ( _Finished )
   {
      DataLog(log_level_proc_debug) << "Finished" << endmsg;
      return 1;
   }
   return NO_TRANSITION;
}

void StopPumps::pumpsNotStopped ()
{
   //
   // Set the pump alarm
   //
   anAlarmMsg pumpAlarm(PUMP_RPM_COMMAND_FAILURE);
   pumpAlarm.setAlarm();
   DataLog(log_level_proc_debug) << "Set the pump command failure alarm " << PUMP_RPM_COMMAND_FAILURE << endmsg;
}

int StopPumps::preProcess ()
{
   //
   // Command all the pumps to stop ...
   //
   ProcData pd;
   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(0.0f);
   DataLog(log_level_proc_debug) << "Stop all pumps" << endmsg;

   return NORMAL;
}

int StopPumps::postProcess ()
{
   ProcData pd;
   //
   // When all pumps have stopped we are through ...
   //
   if ( pd.Status().InletPump.ActFlow()   == 0.0f &&
        pd.Status().ACPump.ActFlow()      == 0.0f &&
        pd.Status().ReturnPump.ActFlow()  == 0.0f &&
        pd.Status().PlasmaPump.ActFlow()  == 0.0f &&
        pd.Status().CollectPump.ActFlow() == 0.0f )
   {
      DataLog(log_level_proc_debug) << "Pumps stopped" << endmsg;
      _Finished = 1;
   }
   return NORMAL;
}

int StopPumps::preEnter ()
{
   //
   // Set up the timer callback ...
   //
   _StopPumpsTimer.interval(10000);   // 10 seconds
   DataLog(log_level_proc_debug) << "Start the stop pumps timer" << endmsg;

   return NORMAL;
}

int StopPumps::postExit ()
{

   ProcData pd;
   //
   // Stop the timer ...
   //
   _StopPumpsTimer.interval(0);
   DataLog(log_level_proc_debug) << "Stop the stop pumps timer" << endmsg;

   // check pressure only when the cassette is down... otherwise causes trouble
   if (pd.Status()._cassettePos.Get() == HAL_CASSETTE_DOWN)
   {
      // autozero moved from load Cassette state IT8239

      // If the set is filtered then use the CPS pressure at set load as the 'zero' value.
      // Note: IT7345  we will do this here all the time but only use it if this is a PTF run.
      //       At this point we don't know yet if this is a PTF or not....
      const float CPS = pd.Status().CPS();

      DataLog(log_level_proc_load_cassette_info) << "CPS Trima Set Zero = " << CPS << "mmHg" << endmsg;

      pd.MakeTrimaSetWritable();
      pd.TrimaSet().CPSZero.Set(CPS);
      pd.MakeTrimaSetReadable();

      if (   (CPS > CobeConfig::data().PTFCPSLoadHigh)
             || (CPS < CobeConfig::data().PTFCPSLoadLow) )
      {

         DataLog(log_level_proc_load_cassette_info) << "Alarm: CPS Trima Set Zero too far out of range" << endmsg;
         _CPSZeroOutsideRange.setAlarm();

      }

   }
   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void StopPumps::reset ()
{
   //
   // Reset the test passed flag ...
   //
   _Finished = 0;
}

void StopPumps::copyOver (const StopPumps&)
{}

void StopPumps::cleanup ()
{}

/* FORMAT HASH 2d626bbf645bca41dc4aa9fb575631f2 */

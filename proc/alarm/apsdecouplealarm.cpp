/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      apsdecouplealarm.h
 *
 */


#include "apsdecouplealarm.h"
#include "trima_datalog.h"
#include "procdata.h"
#include "cobeconfig.h"

const float DECOUPLING_MIN_TIME      = 5.0f / 60.0f; // minutes
const short MIN_DECOUPLING_THRESHOLD = 40;

DEFINE_OBJ(APSDecoupleAlarm);

APSDecoupleAlarm::APSDecoupleAlarm()
   : _APSFastDataMsg(),
     _APSDecoupledPause(APS_DECOUPLING_PAUSE),
     _APSDecoupledAlarm(APS_DECOUPLING),
     _AbsoluteTime(0.0f),
     _DecouplingTime(0.0f),
     _PumpsPausedTime(0.0f),
     _DecouplingEnabled(0),
     _PauseCount(0)
{}

APSDecoupleAlarm::~APSDecoupleAlarm()
{}

void APSDecoupleAlarm::Monitor ()
{
   ProcData pd;

   if ( pd.Run().CycleSwitch.Get() )
   {
      _PauseCount = 0;
   }

   //
   // Get the current time ...
   //
   _AbsoluteTime = pd.AbsTime().getMins();

   if ( pd.TrimaSet().CassetteState.Get() >= ProcTrimaSet_CDS::BLOOD &&
        pd.TrimaSet().CassetteState.Get() < ProcTrimaSet_CDS::USED )
   {
      if ( _APSDecoupledPause.getState() == ACTIVE )
      {
         if ( _DecouplingTime > 0.0f &&
              ( _AbsoluteTime - _DecouplingTime ) > DECOUPLING_MIN_TIME )
         {
            _DecouplingTime = 0.0f;

            _APSDecoupledPause.clearAlarm();

            APSDecoupledPauseMsg decoupleMsg(MessageBase::SEND_LOCAL);
            decoupleMsg.send((int)false);
         }

      }

      //
      //  Check if the pumps are paused ...
      //
      if ( pd.Status().InletPump.ActRPM() < CobeConfig::data().MinimumPumpRPM )
      {
         _PumpsPausedTime = _AbsoluteTime;
      }

      //
      //
      //  To avoid spurious alarms, ensure that the pump is moving and we
      //  didn't just restart from a pause.  We use LowLevelDetected to
      //  ensure that we have gone far enough into prime to guarantee that
      //  fluid covers the sensor, since the sensor does not respond to
      //  pressure changes using air very well.  This means that we may
      //  possibly miss a sample at the return->draw switch when the sensor
      //  becomes uncovered but we are really only interested in the period
      //  of time where we are within a draw cycle anyway (return cycles will
      //  cause positive pressure so decoupling should not occur).
      //
      _DecouplingEnabled = 0;
      if ( ( _AbsoluteTime - _PumpsPausedTime > 2.0f / 60.0f ) &&
           ( pd.Status().InletPump.ActFlow() > 30.0f ) &&
           ( pd.Status().ReservoirLowSensorCovered() )
           )
      {
         _DecouplingEnabled = 1;
      }
   }
}

void APSDecoupleAlarm::enable ()
{
   MonitorBase::enable();

   //
   // Register for the message ...
   //
   _APSFastDataMsg.init(Callback<APSDecoupleAlarm>(this, &APSDecoupleAlarm::fastAPSData) );
}

void APSDecoupleAlarm::disable ()
{
   MonitorBase::disable();

   _APSFastDataMsg.deregisterMsg();

   //
   // Disable checking ...
   //
   _DecouplingEnabled = 0;

   //
   // Disable status bar message ...
   APSDecoupledPauseMsg decoupleMsg(MessageBase::SEND_LOCAL);
   decoupleMsg.send( (int)false);

}

void APSDecoupleAlarm::fastAPSData ()
{
   //
   //
   //  The decoupling threshold is an empirical value, determined via analysis
   //  of actual APS data in decoupling circumstances.  It is assumed that all
   //  normal operations of the pumps at any speed exceed this threshold.
   //
   FASTDATA fastdata;

   short    minvalue;
   short    maxvalue;
   short    value;

   if ( _DecouplingEnabled )
   {
      fastdata = _APSFastDataMsg.getData();

      if ( fastdata.source == CONTROL_ACCESS_PRESSURE)
      {
         minvalue = maxvalue = fastdata.raw[0];

         for ( int i = 0 ; i<FASTDATALENGTH ; i++ )
         {
            value = fastdata.raw[i];
            if (value < minvalue)
               minvalue = value;
            if (value > maxvalue)
               maxvalue = value;
         }

         if ( ( maxvalue - minvalue ) < MIN_DECOUPLING_THRESHOLD )
         {
            _DecouplingTime = _AbsoluteTime;
            _PauseCount++;
            if ( _PauseCount <= 5 )
            {
               _APSDecoupledPause.setAlarm();
               APSDecoupledPauseMsg decoupleMsg(MessageBase::SEND_LOCAL);
               decoupleMsg.send((int)true);
            }
            else
            {
               _APSDecoupledAlarm.setAlarm();
            }

            DataLog(log_level_proc_alarm_monitor_info) << "APS decoupling detected min="
                                                       << minvalue << " max=" << maxvalue
                                                       << " diff " << (maxvalue - minvalue) << " pause count " << _PauseCount << endmsg;
         }
      }
   }
}

/* FORMAT HASH 3ea80691d37e16318dd8002a4985fbfa */

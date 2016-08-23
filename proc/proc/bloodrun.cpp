/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      bloodrun.cpp
 *
 */

#include "bloodrun.h"
#include "guiproc.h"
#include "recoveryman.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"

DEFINE_STATE(BloodRun);

BloodRun::BloodRun()
   : StateAbs(),
     _PeriodicStateMsg(0, Callback<BloodRun>(this, &BloodRun::bloodRunState), TimerMessage::DISARMED),
     _RinsebackStatusMsg(),
     _SendOrdersRequestMsg(),
     _ACPumpDelayTimer(0, Callback<BloodRun>(this, &BloodRun::startACPump), TimerMessage::DISARMED),
     _RBCDetectorYield(),
     _PrevCycle(0),
     _SavedACPumpCommand(0.0f)
{}

BloodRun::BloodRun(const BloodRun& state)
   : StateAbs(state),
     _PeriodicStateMsg(0, Callback<BloodRun>(this, &BloodRun::bloodRunState), TimerMessage::DISARMED),
     _RinsebackStatusMsg(),
     _SendOrdersRequestMsg(),
     _ACPumpDelayTimer(0, Callback<BloodRun>(this, &BloodRun::startACPump), TimerMessage::DISARMED),
     _RBCDetectorYield(),
     _PrevCycle(0),
     _SavedACPumpCommand(0.0f)
{
   BloodRun::copyOver(state);
}

BloodRun::~BloodRun()
{
   BloodRun::cleanup();
}

int BloodRun::transitionStatus ()
{
   ProcData pd;
   if ( pd.TrimaSet().CassetteState.Get() >= ProcTrimaSet_CDS::RINSEBACK )
   {
      return 1;
   }
   return NO_TRANSITION;
}

void BloodRun::bloodRunState ()
{
   //
   // Set the new system state ...
   //
   ProcData pd;
   pd.SystemState(BLOOD_RUN, false);

   //
   // Stop the timer
   _PeriodicStateMsg.interval(0);
}

void BloodRun::rinsebackStatus ()
{
   ProcData pd;
   if ( _RinsebackStatusMsg.getData() == REQUEST_RINSEBACK_STATUS)
   {
      RINSEBACK_STATUS status = ( pd.Run().FirstDrawComplete.Get() ? RINSEBACK_OK : RINSEBACK_NOT_OK );
      _RinsebackStatusMsg.send(status);
   }

}

int BloodRun::preProcess ()
{
   ProcData pd;
   //
   // Accumulate volumes ...
   //
   if (    RecoveryMan::inRecovery() == RecoveryTypes::CountNone
           || RecoveryMan::inRecovery() == RecoveryTypes::CountTime )
   {
      //
      // We are in recovery, treat the volumes as such ...
      //
      pd.Update(ProcData::RECOVERY);
   }
   else
   {
      pd.Update(ProcData::BLOODRUN);
   }

   return NORMAL;
}

int BloodRun::postProcess ()
{
   ProcData pd;
   //
   // Activate the timers ...
   //
   pd.SysRunTime().activate();
   if (    !pd.AlarmActive()
           && (    RecoveryMan::inRecovery() == RecoveryTypes::NoRecovery
                   || RecoveryMan::inRecovery() == RecoveryTypes::CountAll
                   || RecoveryMan::inRecovery() == RecoveryTypes::CountTime )
           )
      pd.ProcRunTime().activate();
   else
      pd.ProcRunTime().inactivate();

   //
   // Control the AC PUmp during cycle switches ...
   //
   controlACPump();

   //
   // Save the current cycle flag ...
   //
   _PrevCycle = pd.Run().DrawCycle.Get();

   //
   //
   //   Calculate and save off last run ratio.  Note we have to verify
   //   both commands since otherwise pauses at the return/draw point
   //   can cause invalid ratios to be calculated (IT4892).
   //
   if (pd.Run().DrawCycle.Get())
   {
      float ACFlow    = pd.Status().ACPump.CmdFlow();
      float InletFlow = pd.Status().InletPump.CmdFlow();
      if ((ACFlow > 0.0f) && (InletFlow > 0.0f))
      {
         pd.MakeRunWritable();
         pd.Run().LastRunRatio.Set(InletFlow / ACFlow);
         pd.MakeRunReadable();
      }
   }

   //
   //   Sample RBC Detector for Yield Integration purposes
   //
   _RBCDetectorYield.ProcessHalStatus();

   return NORMAL;
}

int BloodRun::preEnter ()
{
   ProcData pd;
   //
   // Set the new system state ...
   //
   pd.SystemState(BLOOD_RUN);

   //
   // Set up the periodic state message
   //
   _PeriodicStateMsg.interval(5000);   // 5 seconds

   //
   // Register for the request for rinseback status ...
   //
   _RinsebackStatusMsg.init(Callback<BloodRun>(this, &BloodRun::rinsebackStatus), MessageBase::SNDRCV_LOCAL);

   //
   // Create the request send orders message ...
   //
   _SendOrdersRequestMsg.init(MessageBase::SEND_LOCAL);

   //
   // Get the current cycle ...
   //
   _PrevCycle = pd.Run().DrawCycle.Get();

   //
   // Initialize the RBC Detector's Platelet Yield Integrator ...
   //
   _RBCDetectorYield.Initialize();

   return NORMAL;
}

int BloodRun::postExit ()
{
   ProcData pd;

   //
   // Stop the timers ...
   //
   _ACPumpDelayTimer.interval(0);

   //
   // Compute final Integrated Yield and turn off the RBC Detector's Platelet Yield Integrator
   //
   _RBCDetectorYield.Finalize();

   //
   // The cassette can now be considered finished with blood run ...
   //
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CassetteState.Set(ProcTrimaSet_CDS::RINSEBACK);
   pd.MakeTrimaSetReadable();

   //
   // Stop the timer
   _PeriodicStateMsg.interval(0);

   //
   // Cleanup the memory ...
   //
   cleanup();

   return NORMAL;
}

void BloodRun::controlACPump ()
{
   ProcData pd;

   //
   // If we have just had a switch from return to draw ...
   //
   if ( pd.Run().DrawCycle.Get() &&
        _PrevCycle != pd.Run().DrawCycle.Get() )
   {

      //
      // Save the AC commanded speed ...
      //
      _SavedACPumpCommand = pd.Orders().Qac();

      //
      // Start a timer with a duration of ...
      //
      //    _CobeConfig.RecirculateSnDrawVol mls /
      //              ( pd.Orders().Qin() mls/min / ( 60 secs * 1000 msecs ) )
      //
      int timeout = (int)( CobeConfig::data().RecirculateSnDrawVol / ( pd.Orders().Qin() / 60000.0f ));
      _ACPumpDelayTimer.interval(timeout);
      DataLog(log_level_proc_debug) << "Started AC Pump Delay Timer " << timeout << endmsg;

   }

   //
   // Stop the AC pump ...
   //
   if ( _SavedACPumpCommand != 0.0f )
   {
      //
      // Set the AC commanded speed to 0.0f ...
      //
      pd.Orders().Qac(0.0f);
   }

}

void BloodRun::startACPump ()
{
   ProcData pd;

   //
   // Command the AC pump to the saved speed ...
   //
   pd.Orders().Qac(_SavedACPumpCommand);
   _SavedACPumpCommand = 0.0f;

   //
   // Stop the timer ...
   //
   _ACPumpDelayTimer.interval(0);

   //
   // Request the orders be sent ...
   //
   _SendOrdersRequestMsg.send(1);

}

void BloodRun::reset ()
{
   //
   // Reset the flags ...
   //
   _PrevCycle          = 0;
   _SavedACPumpCommand = 0.0f;
}

void BloodRun::copyOver (const BloodRun&)
{}

void BloodRun::cleanup ()
{
   //
   // Deregister the messages ...
   //
   _RinsebackStatusMsg.deregisterMsg();
   _SendOrdersRequestMsg.deregisterMsg();
}

/* FORMAT HASH 900b79d75005372fcd60dc17e590b6ec */

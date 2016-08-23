/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      normalrinseback.cpp
 *
 */

#include "normalrinseback.h"
#include "states.h"
#include "cobeconfig.h"
#include "procdata.h"

DEFINE_STATE(NormalRinseback);

NormalRinseback::NormalRinseback()
   : StateAbs(),
     _PeriodicStateMsg(0, Callback<NormalRinseback>(this, &NormalRinseback::rinsebackState), TimerMessage::DISARMED)
{}

NormalRinseback::NormalRinseback(const NormalRinseback& state)
   : StateAbs(state),
     _PeriodicStateMsg(0, Callback<NormalRinseback>(this, &NormalRinseback::rinsebackState), TimerMessage::DISARMED)
{
   NormalRinseback::copyOver(state);
}

NormalRinseback::~NormalRinseback()
{
   NormalRinseback::cleanup();
}

int NormalRinseback::transitionStatus ()
{
   return NO_TRANSITION;
}

void NormalRinseback::rinsebackState ()
{
   ProcData pd;

   //
   // Set the new system state notification.  Make sure we don't set
   // it if we have already gotten to disconnect prompt.
   //
   if ( pd.TrimaSet().CassetteState.Get() == ProcTrimaSet_CDS::RINSEBACK )
   {
      pd.SystemState(BLOOD_RINSEBACK, false);
   }

   //
   // Stop the timer...
   _PeriodicStateMsg.interval(0);
}

int NormalRinseback::preProcess ()
{
   ProcData pd;

   //
   // Deactivate the timers ...
   //
   pd.SysRunTime().inactivate();
   pd.ProcRunTime().inactivate();

   //
   // Accumulate volumes ...
   //
   pd.Update(ProcData::RINSEBACK);

   //
   //
   //   Calculate new commanded centrifuge speed per IT3080.  We use +/- 1% of
   //   3000RPM (per spec) to determine if the centrifuge is at speed.
   //
   int   centrifugeAtSpeed = 1;
   float commandedRPM      = 0.0f;
   float speedDelta        = (float)fabs(pd.Status().CentrifugeRPM() - pd.Status().CmdCentrifugeRPM());
   if (speedDelta > 30.0f) centrifugeAtSpeed = 0;

   if (pd.Status().CentrifugeRPM() < CobeConfig::data().RPMRinsebackThreshold)
   {
      commandedRPM = 0.0f;
   }
   else if (centrifugeAtSpeed)
   {
      commandedRPM = pd.Status().CmdCentrifugeRPM();
      //
      //
      //   The centrifuge is not at speed but the current speed is above the
      //   threshold speed.  The desire is to maintain the current centrifuge
      //   speed.  We can't simply copy the current speed into the command
      //   since we would then have the command attempting to seek to the current
      //   speed as the current speed changes.  If for some reason we enter this
      //   state right at 2000 and then the centrifuge speed drops below the
      //   threshold, the above logic will set the command to zero (and it will
      //   stay there from then on).
      //
   }
   else
   {
      if (commandedRPM < CobeConfig::data().RPMRinsebackThreshold)
      {
         commandedRPM = pd.Status().CentrifugeRPM(); // which we know is >= threshold
      }
   }
   //
   //
   //  For robustness limit to collect RPM speed (this condition should never
   //  actually occur unless our centrifuge is outside spec).
   //
   if (commandedRPM > CobeConfig::data().RPMCollect) commandedRPM = CobeConfig::data().RPMCollect;

   pd.Orders().RPM(commandedRPM);

   return NORMAL;
}

int NormalRinseback::postProcess ()
{
   return NORMAL;
}

int NormalRinseback::preEnter ()
{
   //
   // Notify the system we are in rinseback state
   ProcData pd;
   pd.SystemState(BLOOD_RINSEBACK);

   pd.MakeRunWritable();
   // rinseback considered started for MSS purposes.. not a slavage case.
   pd.Run().rinsebackEnteredMss.Set(true);
   pd.MakeRunReadable();

   //
   // Set up the state message
   _PeriodicStateMsg.interval(5000);    // 5 seconds

   return NORMAL;
}

int NormalRinseback::postExit ()
{
   //
   // Turn off the timer
   _PeriodicStateMsg.interval(0);

   return NORMAL;
}

void NormalRinseback::reset ()
{}

void NormalRinseback::copyOver (const NormalRinseback&)
{}

void NormalRinseback::cleanup ()
{}

/* FORMAT HASH 2e2bae30d122800f4ab501a0fc76ecc0 */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      bloodprime.cpp
 *
 */

#include "bloodprime.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"

DEFINE_STATE(BloodPrime);

BloodPrime::BloodPrime()
   : StateAbs(),
     _PeriodicStateMsg(0, Callback<BloodPrime>(this, &BloodPrime::primeState), TimerMessage::DISARMED)
{}

BloodPrime::BloodPrime(const BloodPrime& state)
   : StateAbs(state),
     _PeriodicStateMsg(0, Callback<BloodPrime>(this, &BloodPrime::primeState), TimerMessage::DISARMED)
{}

BloodPrime::~BloodPrime()
{}

int BloodPrime::transitionStatus ()
{
   ProcData pd;
   if ( pd.TrimaSet().CassetteState.Get() >= ProcTrimaSet_CDS::PRIMED )
   {
      //
      // Already been primed ...
      //
      return 1;
   }
   return NO_TRANSITION;
}

void BloodPrime::primeState ()
{
   ProcData pd;
   pd.SystemState(BLOOD_PRIME, false);

   //
   // Stop the timer
   _PeriodicStateMsg.interval(0);
}

int BloodPrime::preProcess ()
{
   ProcData pd;
   //
   // Activate the timers ...
   //
   pd.SysRunTime().activate();
   if ( !pd.AlarmActive() )
      pd.ProcRunTime().activate();
   else
      pd.ProcRunTime().inactivate();

   //
   // Accumulate volumes ...
   //
   pd.Update(ProcData::PRIME);

   return NORMAL;
}

int BloodPrime::postProcess ()
{
   return NORMAL;
}

int BloodPrime::preEnter ()
{
   ProcData pd;
   pd.SystemState(BLOOD_PRIME);


   // zero the start time NOW!
   pd.MakeRunWritable();
   _RunInfo.SetStartTime(pd.Run().procedureStartTime);
   pd.MakeRunReadable();

   //
   // Set up the periodic state message
   //
   _PeriodicStateMsg.interval(5000);   // 5 seconds

   return NORMAL;
}

int BloodPrime::postExit ()
{
   ProcData pd;
   //
   // The cassette can now be considered primed with blood ...
   //
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CassetteState.Set(ProcTrimaSet_CDS::PRIMED);
   pd.MakeTrimaSetReadable();

   //
   // Stop the timer
   _PeriodicStateMsg.interval(0);

   return NORMAL;
}

void BloodPrime::reset ()
{}

void BloodPrime::copyOver (const BloodPrime&)
{}

void BloodPrime::cleanup ()
{}

/* FORMAT HASH 29abe808b57a298885a50c76494a4ab7 */

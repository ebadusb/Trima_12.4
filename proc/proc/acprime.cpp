/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      acprime.cpp
 *
 */

#include "acprime.h"
#include "states.h"
#include "procdata.h"
#include "cycleacc.h"
#include "volumeacc.h"

DEFINE_STATE(ACPrime);

ACPrime::ACPrime()
   : StateAbs()
{}

ACPrime::ACPrime(const ACPrime& state)
   : StateAbs(state)
{
   ACPrime::copyOver(state);
}

ACPrime::~ACPrime()
{
   ACPrime::cleanup();
}

int ACPrime::preProcess ()
{
   ProcData pd;

   //
   // Accumulate all the pump volumes ...
   //
   pd.Update(ProcData::ACPRIME);

   return NORMAL;
}

int ACPrime::postProcess ()
{
   return NORMAL;
}

int ACPrime::preEnter ()
{
   ProcData pd;

   //
   // Set the system state to AC Prime
   //
   pd.SystemState(AC_PRIME);

   //
   // Clear the volumes accrued in the set thus far...
   //
   VolumeAcc::Clear();

   //
   // Allow save of power fail data once AC prime has started.
   //
   pd.PFRHandler().EnablePFSave();

   return NORMAL;
}

int ACPrime::postExit ()
{
   ProcData pd;

   //
   // Send the AC prime complete message ...
   //
   ACPrimeCompletedMsg acPrimeComplete(MessageBase::SEND_LOCAL);
   acPrimeComplete.send(1);
   DataLog(log_level_proc_ac_prime_info) << "Sent ACPrimeCompleteMsg Message " << endmsg;

   //
   // The cassette can now been primed with AC
   //
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CassetteState.Set(ProcTrimaSet_CDS::ACPRIMED);
   pd.MakeTrimaSetReadable();

   //
   // Reset the inlet volumes ...
   //
   VolumeAcc::ClearVin();
   VolumeAcc::ClearVinTotal();

   //
   // Reset the reservoir fill volume ...
   //
   CycleAcc::ClearFillVolume();

   //
   // Reset the return volumes ...
   //
   VolumeAcc::ClearReturn();

   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void ACPrime::copyOver (const ACPrime&)
{}

void ACPrime::cleanup ()
{}

/* FORMAT HASH 1d8fbc13eced9fae5406bf0a1be39f9d */

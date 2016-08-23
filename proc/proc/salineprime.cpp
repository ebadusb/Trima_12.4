/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      salineprime.cpp
 *
 */

#include "salineprime.h"
#include "guiproc.h"
#include "cobeconfig.h"
#include "procdata.h"


const float SPRIME_SPEED  = CobeConfig::data().SalinePrimeSpeed;
const float SPRIME_VOLUME = CobeConfig::data().SalinePrimevolume;
const float SPRIME_START  = CobeConfig::data().SalinePrimeVinPoint;

DEFINE_OBJ(SalinePrime);

SalinePrime::SalinePrime()
   : RecoveryAbs(),
     _RepFluidConMsg(Callback<SalinePrime>(this, &SalinePrime::repFluidConnected) ),
     _ReplaceVolume(0.0f),
     _BolusVolume(0.0f),
     _CollectVolume(0.0f),
     _ReservoirVolume(0.0f),
     _CollectLineVolume(0.0f),
     _PrimeVolume(0.0f)
{}

SalinePrime::SalinePrime(const SalinePrime& rec)
   : RecoveryAbs(rec),
     _RepFluidConMsg(Callback<SalinePrime>(this, &SalinePrime::repFluidConnected) ),
     _ReplaceVolume(0.0f),
     _BolusVolume(0.0f),
     _CollectVolume(0.0f),
     _ReservoirVolume(0.0f),
     _CollectLineVolume(0.0f),
     _PrimeVolume(0.0f)
{
   copyOver(rec);
}

SalinePrime::~SalinePrime()
{
   cleanup();
}

int SalinePrime::init ()
{
   return 1;
}

int SalinePrime::doRecovery ()
{
   ProcData pd;


   //
   // Update prime volume ...
   //
   _PrimeVolume += pd.Status().CollectPump.dV();

   if ( _PrimeVolume < SPRIME_VOLUME )
   {
      //
      // Command the replacement pump ...
      //
      pd.Orders().Qcollect(SPRIME_SPEED);

      if ( _PrimeVolume == 0.0f )
      {
         //
         // Save the current volume ...
         //
         _ReplaceVolume     = pd.Volumes().VReplacement.Get();
         _BolusVolume       = pd.Volumes().VSalineBolus.Get();
         _CollectVolume     = pd.Volumes().Vc.Get();
         _CollectLineVolume = pd.Cycle().Vplt.Get();

      }
   }
   else
   {
      //
      // Reset the volume ...
      //
      pd.MakeVolumesWritable();
      pd.Volumes().VReplacement.Set(_ReplaceVolume);
      pd.Volumes().VSalineBolus.Set(_BolusVolume);
      pd.Volumes().Vc.Set(_CollectVolume);
      pd.MakeCycleWritable();

      pd.Cycle().Vplt.Set(_CollectLineVolume);

      //
      // We are done the line is primed ...
      //
      pd.MakeRunWritable();
      pd.Run().SalinePrimed.Set(true);
      pd.MakeRunReadable();

      _PrimeVolume = 0.0f;
   }


   return 1;
}

int SalinePrime::mayEnter () const
{
   ProcData pd;

   if ( pd.Status().CassIsPltPlsRBC() )
   {
      return 0;
   }
   return 1;
}

void SalinePrime::repFluidConnected ()
{

   int status = _RepFluidConMsg.getData();

   if ( status == RF_CONNECTED )
   {

      ProcData pd;
      pd.MakeRunWritable();
      pd.Run().SalinePrimed.Set(false);    // redo prime if we replace the bag....
      pd.MakeRunReadable();
   }
}

int SalinePrime::shouldRecover ()
{
   ProcData pd;

   //
   // If we are using a set with replacement fluid and the replacement
   //  fluid is trying to be pumped without the line being primed, or
   //  we are not finished with the recovery ...
   //
   if (
      pd.Run().SalineConnected.Get()
      && (    (    !pd.Status().CassIsPltPlsRBC()
                   // IT 8131 make the saline prime happen before rbc setups/collect
                   && pd.Run().FirstDrawComplete.Get()  && (pd.Volumes().Vin >= SPRIME_START)
                   && !pd.Run().SalinePrimed.Get() )
              || ( active() && !pd.Run().SalinePrimed.Get() )
              )
      )
   {
      return 1;
   }
   return 0;
}

void SalinePrime::reset ()
{
   _ReplaceVolume     = 0.0f;
   _BolusVolume       = 0.0f;
   _PrimeVolume       = 0.0f;
   _CollectVolume     = 0.0f;
   _ReservoirVolume   = 0.0f;
   _CollectLineVolume = 0.0f;
}

void SalinePrime::copyOver (const SalinePrime&)
{}

void SalinePrime::cleanup ()
{}

/* FORMAT HASH 908b2437ad4ab30943396ef75bd6ca2a */

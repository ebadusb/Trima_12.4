/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      purgecalcs.cpp
 *
 */

#include "purgecalcs.h"
#include "cobeconfig.h"
#include <math.h>

#ifdef __WIN32__
#define min(a, b) ((a<b) ? a : b)
#define max(a, b) ((a>b) ? a : b)
#endif

int PurgeCalcs::calculateNumCycles (bool hyperconcentration, float volumeTarget, float VPurgeTrigger)
{

   int   MinNumOfPurgeCycles = 0;
   float VplasmaPurgeCycle   = 0;
   if (!hyperconcentration)
   {
      VplasmaPurgeCycle   = CobeConfig::data().VplasmaPurgeCycle1Std;
      MinNumOfPurgeCycles = CobeConfig::data().MinNumOfPurgeCyclesStd;
   }
   else
   {
      VplasmaPurgeCycle   = CobeConfig::data().VplasmaPurgeCycle1High;
      MinNumOfPurgeCycles = CobeConfig::data().MinNumOfPurgeCyclesHigh;
   }


   //
   // PurgeScaleVar
   //
   float purgeScale = CobeConfig::data().PurgeScaleVariable;

   //
   // VolumeRemaining (litres) = ( TargetVolume - VPurgeTrigger ) / 1000.0;
   //
   float volumeRemaining = ( volumeTarget - VPurgeTrigger ) / 1000.0;
   // DataLog(log_level_proc_recovery_info) << "Volume remaining after purge: " << volumeRemaining << " liters" << endmsg;

   //
   // PurgeVolume = minimum( pvMax, PurgeScaleVar * VolumeRemaining );
   //
   float pvScaled    = ( purgeScale * volumeRemaining );
   float pvMax       = CobeConfig::data().PurgeMaxTotalVolume;
   float purgeVolume = min(pvMax, pvScaled);
   // DataLog(log_level_proc_recovery_info) << "Purge Volume: " << purgeVolume << endmsg;

   //
   // NumCycles = PurgeVolume / VPurgeCycle
   //
   float numCycles = purgeVolume / VplasmaPurgeCycle;
   return (max( (int)ceil(numCycles), MinNumOfPurgeCycles));

}

/* FORMAT HASH 06d424966316d683881e9156cb437b68 */

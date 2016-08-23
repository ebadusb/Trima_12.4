/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      reslowalarm.cpp
 *
 */

#include "reslowalarm.h"
#include "procdata.h"

DEFINE_OBJ(ResLowLevelAlarm);

ResLowLevelAlarm::ResLowLevelAlarm()
   : _LowLevelAlarm(LOW_LEVEL_DETECTED_PRIOR_TO_EXPECTED)
{}

ResLowLevelAlarm::~ResLowLevelAlarm()
{}

void ResLowLevelAlarm::Monitor ()
{
   ProcData pd;
   if ( pd.TrimaSet().CassetteState.Get() < ProcTrimaSet_CDS::USED )
   {
      if ( pd.Status().ReservoirLowSensorCovered() ||
           pd.Status().ReservoirIsFull() )
      {
         _LowLevelAlarm.setAlarm();
      }
   }
}

/* FORMAT HASH 99f1e57d30457267d0864b4f207b5e29 */

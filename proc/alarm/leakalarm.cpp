/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      leakalarm.cpp
 *
 */

#include "leakalarm.h"
#include "alarms.h"
#include "procdata.h"

DEFINE_OBJ(LeakAlarm);

LeakAlarm::LeakAlarm()
   : _LeakAlarmMsg(LEAK_DETECTED_ALERT),
     _LeakErrorAlarm(LEAK_DETECTOR_ERROR_FAILURE),
     _DetectorInitialized(0)
{}

LeakAlarm::~LeakAlarm()
{}

void LeakAlarm::Monitor ()
{
   ProcData pd;

   if ( !_DetectorInitialized &&
        pd.Status().LeakValue() > 20 )
   {
      _DetectorInitialized = 1;
   }
   else if (_DetectorInitialized &&
            (   // Don't worry about leak detector during disconnect or mss disposables removal.
               (pd.TrimaSet().CassetteState.Get() != ProcTrimaSet_CDS::USED)
               && (pd.TrimaSet().CassetteState.Get() != ProcTrimaSet_CDS::STORAGE_COMPLETE)
            ))
   {
      if ( pd.Status().LeakDetected() )
      {
         if (_LeakAlarmMsg.getState() == CLEARED)
            _LeakAlarmMsg.setAlarm();
      }

      if ( pd.Status().LeakError() )
      {

         if (_LeakErrorAlarm.getState() == CLEARED)
            _LeakErrorAlarm.setAlarm();
      }
   }
}

/* FORMAT HASH fa9121f02a23f5e1669fe542d242a1c2 */

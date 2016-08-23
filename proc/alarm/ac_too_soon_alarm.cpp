/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ac_too_soon_alarm.cpp
 *
 */

#include "ac_too_soon_alarm.h"
#include "procdata.h"

DEFINE_OBJ(ACTooSoonAlarm);

ACTooSoonAlarm::ACTooSoonAlarm()
   : _ACTooSoonAlarm(AC_DETECTED_PRIOR_TO_AC_PRIME_FAILURE)
{}

ACTooSoonAlarm::~ACTooSoonAlarm()
{}

void ACTooSoonAlarm::Monitor ()
{
   ProcData pd;
   if (    pd.TrimaSet().CassetteState.Get() < ProcTrimaSet_CDS::AC
           && pd.Status().ACDetectFluid() )
   {
      _ACTooSoonAlarm.setAlarm();
   }

}

/* FORMAT HASH 37a18135663e282bc4978072cb94c968 */

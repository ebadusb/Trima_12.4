/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      drbcsplitalarm.cpp
 *
 */

#include "drbcsplitalarm.h"
#include "guiproc.h"
#include "procdata.h"

#include "CatalogChecker.h"

DEFINE_OBJ(DRBCSplitAlarm);

DRBCSplitAlarm::DRBCSplitAlarm()
   : _DRBCSplitAlarm(DRBC_SPLIT_ALERT),
     _SplitNotificationSent(0)
{}

DRBCSplitAlarm::~DRBCSplitAlarm()
{}

void DRBCSplitAlarm::Monitor ()
{
   ProcData pd;

   // if this is PTF then this logic is in collect.
   if ( pd.RunTargets().MeterRbcs.Get() )
   {
      disable();
      return;
   }


   // if not DBRC then never do a split.. duhh!
   if (!CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(),
                                 CatalogChecker::DRBC_BAG))
   {
      disable();
      return;
   }

   //
   // Determine if split notification has been set, or even
   //  should be set ...
   //
   if ( // We haven't already sent a split notification
      (!_SplitNotificationSent) &&
      // and this is a DRBC set
      pd.Status().CassIsRBCPls() &&
      // and this is configured as a prompted split
      // IT 8306 - Always determine whether this is prompted split based on configured
      // volumes with no scaling or adjustments
      pd.Config().isPromptedDrbcSplit(pd.RunTargets().ProcNumber.Get()) &&
      // and we've accumulted have the targeted volume
      (pd.Volumes().RBCDose.Get() * 2.0f >= pd.RunTargets().RbcDoseTarget.Get()))
   {
      // Set the split alarm.
      _DRBCSplitAlarm.setAlarm();

      //
      // Only show the split alarm once.
      _SplitNotificationSent = 1;
   }
}

/* FORMAT HASH 79768ae87086b33ac52ac52c02aa0a31 */

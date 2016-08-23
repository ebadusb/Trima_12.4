/*
 * Copyright (C) 2003 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: //bctquad3/HOME/BCT_Development/vxWorks/Trima/cds/rcs/procplsrinseback_cds.cpp 1.2 2003/11/26 23:59:40Z ms10234 Exp ms10234 $
 * $Log: procplsrinseback_cds.cpp $
 * Revision 1.2  2003/11/26 23:59:40Z  ms10234
 * 6026 - plasma rinseback infusion calculation additions.
 * Revision 1.1  2003/11/26 01:18:00Z  ms10234
 * Initial revision
 *
 */

#include <vxWorks.h>

#ifndef WIN32
#pragma implementation "procplsrinseback_cds.h"
#endif

#include "procplsrinseback_cds.h"

ProcPlsRinseback_CDS::ProcPlsRinseback_CDS(Role role)
   : SingleWriteDataStore ("ProcPlsRinseback_CDS", role)
{
   Status.Register               (this, PFR_RECOVER, ProcPlsRinseback_CDS::None);
   VPlasmaTarget.Register        (this, PFR_RECOVER, 0.0f);
   Facb.Register                 (this, PFR_RECOVER, 0.0f);
   Vtotal.Register               (this, PFR_RECOVER, 0.0f);
   Vac.Register                  (this, PFR_RECOVER, 0.0f);
   IRMax.Register                (this, PFR_RECOVER, 0.0f);
   QReturnMax.Register           (this, PFR_RECOVER, 0.0f);
   Ratio.Register                (this, PFR_RECOVER, 0.0f);

   struct timespec timeSpec; timeSpec.tv_nsec = 0; timeSpec.tv_sec = 0;
   PlasmaValveOpenAccumulatedTime.Register (this, PFR_RECOVER, timeSpec);
   PlasmaValveOpenLastSeenTime.Register       (this, PFR_RECOVER, timeSpec);
}

ProcPlsRinseback_CDS::~ProcPlsRinseback_CDS()
{}




float ProcPlsRinseback_CDS::PlasmaRinsebackTarget (float RinsebackPlasmaTarget,
                                                   float RunTargetPlasmaTarget,
                                                   float CurrentVolume,
                                                   ProcTrimaSet_CDS::CassetteStateType cassetteState)
{
   float plasmaTarget;

   if (cassetteState >= ProcTrimaSet_CDS::RINSEBACK &&
       RinsebackPlasmaTarget != 0.0f )
   {
      if (    RinsebackPlasmaTarget < RunTargetPlasmaTarget
              && RinsebackPlasmaTarget < CurrentVolume )
      {
         plasmaTarget = CurrentVolume;
      }
      else if (    RinsebackPlasmaTarget < RunTargetPlasmaTarget
                   && RinsebackPlasmaTarget > CurrentVolume )
      {
         plasmaTarget = RinsebackPlasmaTarget;
      }
      else
      {
         plasmaTarget = RunTargetPlasmaTarget;
      }
   }
   else
   {
      plasmaTarget = RunTargetPlasmaTarget;
   }

   return plasmaTarget;
}

/* FORMAT HASH a3a7ea03750fba92d034d6da6bb00b41 */

/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Log: procruntargets_cds.cpp $
 * Revision 1.9  2008/07/28 15:50:15Z  ssunusb
 * IT8773 - Predictionator does not compile because of conflicting C++ include files
 * Revision 1.8  2008/02/21 20:42:02Z  jheiusb
 *  -- removed PTFRbcProduct flag as redundant to MSSRbc flag
 * Revision 1.7  2008/02/07 21:52:21Z  jheiusb
 * 8012 -- fix concentration for PAS
 * Revision 1.6  2008/01/23 17:09:54Z  dslausb
 * IT 8435 - VxWorks Checkin
 * Revision 1.5  2006/04/27 20:28:14Z  jheiusb
 * 7468 -- change % carryover
 * Revision 1.4  2006/03/29 17:40:44Z  rm70006
 * IT 7262.  Add helper functions for platelet concentration checks.
 * Revision 1.3  2006/03/03 17:26:41Z  jheiusb
 * initial merge
 * Revision 1.2  2006/01/18 15:26:52Z  rm70006
 * IT 7237.  Turn off purge for PPC runs.
 *
 */

#include <vxWorks.h>

#ifndef WIN32
#pragma implementation "procruntargets_cds.h"
#include "../os/trima_options.h"
#endif

#include "procruntargets_cds.h"
#include "software_cds.h"

ProcRunTargets_CDS::ProcRunTargets_CDS (Role role)
   : SingleWriteDataStore ("ProcRunTargets_CDS", role)
{
   //
   // Procedure specific targets
   //
   ProcNumber.Register              (this, PFR_RECOVER, 0);
   ProcTimeTarget.Register          (this, PFR_RECOVER, 0.0f);

   PlateletYieldTarget.Register     (this, PFR_RECOVER, 0.0f);
   PlateletVolumeTarget.Register    (this, PFR_RECOVER, 0.0f);
   PlateletConcenTarget.Register    (this, PFR_RECOVER, 0.0f);
   PlateletAmapSizeTarget.Register  (this, PFR_RECOVER, AMAP_PLT_NONE);



   PlasmaTarget.Register            (this, PFR_RECOVER, 0.0f);
   RbcVolumeTarget.Register         (this, PFR_RECOVER, 0.0f);
   RbcDoseTarget.Register           (this, PFR_RECOVER, 0.0f);
   RbcVsvnTarget.Register           (this, PFR_RECOVER, 0.0f);
   RbcCritTarget.Register           (this, PFR_RECOVER, 0.0f);
   InfusionRateTarget.Register      (this, PFR_RECOVER, 0.0f);

   SelectedPlateletYield.Register   (this, PFR_RECOVER, 0.0f);
   SelectedPlateletVolume.Register  (this, PFR_RECOVER, 0.0f);
   SelectedPlateletConcen.Register  (this, PFR_RECOVER, 0.0f);
   SelectedPlasmaVolume.Register    (this, PFR_RECOVER, 0.0f);
   SelectedRbcVolume.Register       (this, PFR_RECOVER, 0.0f);

   RequiresRepFluid.Register        (this, PFR_RECOVER, false);

   RBCSolutionTarget.Register       (this, PFR_RECOVER, 0.0f);
   PltSolutionTarget.Register       (this, PFR_RECOVER, 0.0f);
   PltPctCarryover.Register         (this, PFR_RECOVER, 0.0f);

   OriginalYeild.Register           (this, PFR_RECOVER, 0.0f);
   OriginalVolume.Register          (this, PFR_RECOVER, 0.0f);

   MeterPlatelets.Register          (this, PFR_RECOVER, false);
   MeterRbcs.Register               (this, PFR_RECOVER, false);

}

ProcRunTargets_CDS::~ProcRunTargets_CDS()
{}

/* FORMAT HASH 98c746d056706063954cd7df66b52c49 */

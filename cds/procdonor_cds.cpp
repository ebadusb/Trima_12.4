/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: I:/BCT_Development/vxWorks/Trima/cds/rcs/procdonor_cds.cpp 1.5 2003/12/09 17:08:18Z rm70006 Exp rm70006 $
 * $Log: procdonor_cds.cpp $
 * Revision 1.5  2003/12/09 17:08:18Z  rm70006
 * IT 5858.  Add new variables for end of run record processing.
 * Revision 1.4  2003/11/04 22:39:04Z  ms10234
 * 6446 - added donor BMI
 * Revision 1.3  2003/01/24 16:54:45Z  ms10234
 * Initialized the donor values
 * Revision 1.2  2002/12/11 18:35:05Z  sb07663
 * conditional non-win32 compilation of pragma interface directives
 * Revision 1.1  2002/11/18 18:30:42Z  jl11312
 * Initial revision
 *
 */

#include <vxWorks.h>

#ifndef WIN32
#pragma implementation "procdonor_cds.h"
#endif

#include "procdonor_cds.h"

ProcDonor_CDS::ProcDonor_CDS (Role role)
   : SingleWriteDataStore ("ProcDonor_CDS", role)
{
   Female.Register       (this, PFR_RECOVER, 0);
   Height.Register       (this, PFR_RECOVER, 0.0f);
   Weight.Register       (this, PFR_RECOVER, 0.0f);
   BMI.Register          (this, PFR_RECOVER, 0.0f);
   TBV.Register          (this, PFR_RECOVER, 0.0f);
   TBVOgawa.Register     (this, PFR_RECOVER, 0.0f);
   PreCnt.Register       (this, PFR_RECOVER, 0.0f);
   Hct.Register          (this, PFR_RECOVER, 0.0f);
   BloodType.Register    (this, PFR_RECOVER, 0);
   SampleVolume.Register (this, PFR_RECOVER, 0.0f);
   DownloadStatus.Register(this, PFR_RECOVER, 0);
}

ProcDonor_CDS::~ProcDonor_CDS()
{}

/* FORMAT HASH 4bcbeb9f545074c15d6a74a96c189d19 */

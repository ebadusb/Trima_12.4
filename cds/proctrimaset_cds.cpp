/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/cds/rcs/proctrimaset_cds.cpp 1.6 2003/08/11 15:28:03Z ms10234 Exp jheiusb $
 * $Log: proctrimaset_cds.cpp $
 * Revision 1.6  2003/08/11 15:28:03Z  ms10234
 * 5838 - changes for the chamber saturation algorithm
 * Revision 1.5  2003/07/07 20:57:32Z  jl11312
 * - added CDS element to save APS auto-zero setting
 * Revision 1.4  2003/06/11 14:48:16Z  jl11312
 * - removed comments for temporary fix for IT6109, since it should be permanent
 * Revision 1.3  2003/06/03 14:03:51Z  rm70006
 * IT 6109.
 * Temporarily change trima set type to not-loaded  until PFR is working.
 * Revision 1.2  2002/12/11 18:35:07Z  sb07663
 * conditional non-win32 compilation of pragma interface directives
 * Revision 1.1  2002/11/18 18:30:44Z  jl11312
 * Initial revision
 *
 */

#include <vxWorks.h>

#ifndef WIN32
#pragma implementation "proctrimaset_cds.h"
#endif

#include "proctrimaset_cds.h"

ProcTrimaSet_CDS::ProcTrimaSet_CDS(Role role)
   : SingleWriteDataStore ("ProcTrimaSet_CDS", role)
{
   Cassette.Register              (this, PFR_RECOVER, cassette::NOT_LOADED);
   CassetteState.Register         (this, PFR_RECOVER, DRY);
   CentPresLimit.Register         (this, PFR_RECOVER, 0.0f);
   RBCRedDriveValue.Register      (this, PFR_RECOVER, 0);
   RBCGreenDriveValue.Register    (this, PFR_RECOVER, 0);
   RBCDetectorCalibrated.Register (this, PFR_RECOVER, false);
   APSZero.Register               (this, PFR_RECOVER, 0.0f);
   CmdQch.Register                (this, NO_PFR, 0.0f);
   ActQch.Register                (this, NO_PFR, 0.0f);


   CPSZero.Register                   (this, PFR_RECOVER, 0.0f);
   PTFCPSTestPassed.Register          (this, PFR_RECOVER, false);
}

ProcTrimaSet_CDS::~ProcTrimaSet_CDS()
{}

/* FORMAT HASH 430db2a1b01c4446ae1b384d7eb1479d */

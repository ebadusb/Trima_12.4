/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/cds/RCS/procadjust_cds.cpp 1.3 2003/05/09 12:52:05 ms10234 Exp $
 * $Log: procadjust_cds.cpp $
 * Revision 1.3  2003/05/09 12:52:05  ms10234
 * 5829 - Use adjust CDS instead of adjust message
 * Revision 1.2  2002/12/11 18:35:05Z  sb07663
 * conditional non-win32 compilation of pragma interface directives
 * Revision 1.1  2002/11/18 18:30:41Z  jl11312
 * Initial revision
 *
 */

#include <vxWorks.h>

#ifndef WIN32
#pragma implementation "procadjust_cds.h"
#endif

#include "procadjust_cds.h"

ProcAdjustBar_CDS::ProcAdjustBar_CDS(const char* name, Role role)
   : MultWriteDataStore (name, role)
{
   Maximum.Register     (this, PFR_RECOVER, 0.0f);
   Minimum.Register     (this, PFR_RECOVER, 0.0f);
   CurrentCap.Register  (this, PFR_RECOVER, 0.0f);
   CurrentValue.Register(this, PFR_RECOVER, 0.0f);
   Active.Register      (this, PFR_RECOVER, 0.0f);
}

ProcAdjustBar_CDS::~ProcAdjustBar_CDS()
{}

ProcAdjust_CDS::ProcAdjust_CDS(Role role)
   : MultWriteDataStore ("ProcAdjust_CDS", role),
     Draw     ("DrawAdjustment", role),
     Return   ("ReturnAdjustment", role),
     Tingling ("TinglingAdjustment", role),
     Clumping ("ClumpingAdjustment", role)
{
   Spillover.Register  (this, PFR_RECOVER, BUTTON_INVISIBLE);
   AirBlock.Register   (this, PFR_RECOVER, BUTTON_INVISIBLE);
   SalineBolus.Register(this, PFR_RECOVER, BUTTON_INVISIBLE);
   MaxQin.Register     (this, PFR_RECOVER, 0.0f);
}

ProcAdjust_CDS::~ProcAdjust_CDS()
{}

/* FORMAT HASH cc30b3f5c042f32ebb90badbab0326d1 */

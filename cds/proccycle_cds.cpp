/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/cds/rcs/proccycle_cds.cpp 1.3 2003/06/19 18:40:31Z ms10234 Exp spriusb $
 * $Log: proccycle_cds.cpp $
 * Revision 1.3  2003/06/19 18:40:31Z  ms10234
 * 5829 - changes for PFR
 * Revision 1.2  2003/04/17 17:10:24Z  ms10234
 * 6004 - Added new reservoir data members
 * Revision 1.1  2002/11/18 18:30:41Z  jl11312
 * Initial revision
 *
 */

#include <vxWorks.h>

#pragma implementation "proccycle_cds.h"
#include "proccycle_cds.h"

ProcCycle_CDS::ProcCycle_CDS (Role role)
   : SingleWriteDataStore ("ProcCycle_CDS", role)
{
   Vin.Register            (this, PFR_RECOVER, 0.0f);
   Vac.Register             (this, PFR_RECOVER, 0.0f);
   Vret.Register            (this, PFR_RECOVER, 0.0f);
   Vpls.Register            (this, PFR_RECOVER, 0.0f);
   Vplt.Register            (this, PFR_RECOVER, 0.0f);
   Vrbc.Register            (this, PFR_RECOVER, 0.0f);
   VrbcRes.Register         (this, PFR_RECOVER, 0.0f);
   Vres.Register            (this, PFR_RECOVER, 0.0f);
   VresDraw.Register        (this, PFR_RECOVER, 0.0f);
   VresReturn.Register      (this, PFR_RECOVER, 0.0f);
   VresLast.Register        (this, PFR_RECOVER, 0.0f);
   VAccumOverdraw.Register  (this, NO_PFR, 0.0f);
   ACRatio.Register         (this, PFR_RECOVER, 5.0f);
   Hrbc.Register            (this, PFR_RECOVER, 1.0f);
   Facb.Register            (this, PFR_RECOVER, 1.0f);
   ResHct.Register          (this, PFR_RECOVER, 0.0f);
   ResRBCs.Register         (this, PFR_RECOVER, 0.0f);
}

ProcCycle_CDS::~ProcCycle_CDS()
{}

/* FORMAT HASH b51dbfb2b61a0923a49b752c3944ae7e */

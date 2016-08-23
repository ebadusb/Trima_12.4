/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/cds/rcs/proctime_cds.cpp 1.4 2004/01/05 18:12:34Z rm70006 Exp jheiusb $
 * $Log: proctime_cds.cpp $
 * Revision 1.4  2004/01/05 18:12:34Z  rm70006
 * IT 5858.  Change where procedure start time gets stored.
 * Revision 1.3  2003/12/09 17:08:28Z  rm70006
 * IT 5858.  Add new variables for end of run record processing.
 * Revision 1.2  2002/12/11 18:35:06Z  sb07663
 * conditional non-win32 compilation of pragma interface directives
 * Revision 1.1  2002/11/18 18:30:44Z  jl11312
 * Initial revision
 *
 */

#include <vxWorks.h>

#ifndef WIN32
#pragma implementation "proctime_cds.h"
#endif
#include "proctime_cds.h"

ProcTime_CDS::ProcTime_CDS (Role role)
   : SingleWriteDataStore ("ProcTime_CDS", role)
{
   const struct timespec zerotime = {0, 0};

   tmAbsolute.Register  (this, PFR_RECOVER, zerotime);
   tmProcTime.Register  (this, PFR_RECOVER, zerotime);
   tmSysRun.Register    (this, PFR_RECOVER, zerotime);
   tmProcRun.Register   (this, PFR_RECOVER, zerotime);
   tmMSSRun.Register    (this, PFR_RECOVER, zerotime);

}

ProcTime_CDS::~ProcTime_CDS()
{}

float ProcTime_CDS::ConvertToMinutes (const timespec& t)
{
   return (float)(((double)t.tv_sec / (double)60 ) +
                  ((double)t.tv_nsec / (double)6.0E10 ));
}

/* FORMAT HASH 5d2085cd11e0a36853acdc075b261b21 */

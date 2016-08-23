/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/trima_common/rcs/trima_datalog.h 1.14 2006/03/03 17:35:34Z jheiusb Exp dslausb $
 * $Log: trima_datalog.h $
 * Revision 1.14  2006/03/03 17:35:34Z  jheiusb
 * INITIAL MERGE
 * Revision 1.13  2005/12/16 23:01:02Z  rm70006
 * IT 7237.  Fix files to compile with predictionator.
 * Revision 1.12  2003/04/30 20:06:10Z  jl11312
 * - removed old datalog levels
 * Revision 1.11  2003/03/27 16:29:09Z  jl11312
 * - added support for new datalog levels
 */
#ifndef WIN32
#include "error.h"
#endif


#ifndef TRIMA_DATALOG_H  /*  Prevents multiple compilations. */
#define TRIMA_DATALOG_H

#include "datalog.h"
#include "trima_datalog_levels.h"

DataLog_Result trima_datalogInit ();

#endif /* ifndef TRIMA_DATALOG_H */

/* FORMAT HASH 802cfbde8bcb3f2d029247bdaab5813a */

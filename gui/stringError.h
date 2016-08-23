/*******************************************************************************
 *
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      stringError.h
 *             String Error Reporter
 *
 * ABSTRACT:   I was going nuts trying to track down which string was causing the
 *		system to reboot, so I decided to create this big function which will tell
 *		you what the actual ID of the string is in the enumeration (not just the
 *		integer value).
 *
 * HISTORY:    $Log: stringError.h $
 * HISTORY:    Revision 1.4  2006/04/12 22:16:26Z  dslausb
 * HISTORY:    - Changed and deleted some strings for MSS
 * HISTORY:    - Fixed a glitch with the status bar on syscassette
 * HISTORY:    Revision 1.3  2006/03/30 21:35:12Z  dslausb
 * HISTORY:    IT 7148 - Reformatted leukoreduction labeling screen
 * HISTORY:    Revision 1.2  2006/03/06 23:48:02Z  dslausb
 * HISTORY:    IT 7434 - Rearranged the MSS config screen
 *
 *******************************************************************************/

#if (CPU==SIMNT)
#ifndef __STRINGERROR_H__
#define __STRINGERROR_H__

#include "datalog.h"

void                     printStringErrorInfo (TEXT_STRING_ID string_id);
const char*              convertStringIdToString (TEXT_STRING_ID string_id);
DataLog_Stream& operator << (DataLog_Stream& os, TEXT_STRING_ID id);

#endif
#endif

/* FORMAT HASH e5e783320cf79d4bd235b9748c2e1d8c */

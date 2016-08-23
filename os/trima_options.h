/*
 * Copyright (C) 2005 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/os/rcs/trima_options.h 1.1 2007/11/07 22:45:09Z dslausb Exp $
 * $Log: trima_options.h $
 * Revision 1.1  2007/11/07 22:45:09Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:38:46Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:34:19Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:30:50Z  dslausb
 * Initial revision
 * Revision 1.1  2007/05/22 18:20:19Z  dslausb
 * Initial revision
 *
 */

#ifndef _TRIMA_OPTIONS_INCLUDE
#define _TRIMA_OPTIONS_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

const char* trimaSysGetHardwareSetting (const char* optionName);

int trimaSysLoadOptionFiles (void);

#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _TRIMA_OPTIONS_INCLUDE */

/* FORMAT HASH 5010c582359f91d55e3981a640571db4 */

/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/vxworks_target/control_ampro/rcs/envfile.h 1.1 2007/11/08 00:59:25Z dslausb Exp $
 * $Log: envfile.h $
 * Revision 1.1  2007/11/08 00:59:25Z  dslausb
 * Initial revision
 * Revision 1.1  2007/08/28 18:18:00Z  rhecusb
 * Initial revision
 * Revision 1.1  2002/09/19 22:16:19Z  jl11312
 * Initial revision
 *
 */

#ifndef _ENVFILE_INCLUDE
#define _ENVFILE_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

STATUS readEnvironmentFile(const char * fileName);

#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _ENVFILE_INCLUDE */


/*
 * Copyright (C) 2003 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/os/rcs/load_driver.h 1.1 2007/11/07 22:38:38Z dslausb Exp $
 * $Log: load_driver.h $
 * Revision 1.1  2007/11/07 22:38:38Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:34:11Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:28:52Z  dslausb
 * Initial revision
 * Revision 1.1  2007/05/22 18:20:11Z  dslausb
 * Initial revision
 * Revision 1.1  2003/11/13 14:38:11Z  jl11312
 * Initial revision
 *
 */

#ifndef _LOAD_DRIVER_INCLUDE
#define _LOAD_DRIVER_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

void        parseDriverCommandLine (const char* driverCmd, char** driverFileName, char** initFuncName);
const char* initializeDriver (const char* driverCmd, const char* driverFileName, const char* initFuncName);
STATUS      loadDriverFromMemory (const char* fileName, char* memStart, size_t memLength);

#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _LOAD_DRIVER_INCLUDE */

/* FORMAT HASH 829e2d992bdeda06337bdb7193af4002 */

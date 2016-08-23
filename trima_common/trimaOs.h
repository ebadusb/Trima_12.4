/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 *	Trima specific functions provided directly in the vxWorks operating
 * system image.
 *
 * $Header: E:/BCT_Development/TrimaVxPort/Trima/trima_common/rcs/trimaOs.h 1.3 2007/09/17 21:53:26Z rhecusb Exp $
 * $Log: trimaOs.h $
 * Revision 1.3  2007/09/17 21:53:26Z  rhecusb
 * Added prototype for trimaSysStartDiskSync().
 *
 */

#ifndef _TRIMA_OPSYS_INCLUDE
#define _TRIMA_OPSYS_INCLUDE

#include <vxWorks.h>
#include <setjmp.h>
#include <drv/hdisk/ataDrv.h>
#include "common_hw.h"
#include "filenames.h"
#include "portnumbers.h"
#include "trima_datalog.h"

#include "../config/file_system.h"

/*
 *	Make sure this file uses only constructs valid in both C and C++.  Although
 * it is used mostly from Trima .cpp files, the operating system image build
 * uses it from a .c file.
 */

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

void           trimaSysStartDiskSync (void);
int            trimaSysGetPartitionData (TrimaPartitionID id, TrimaPartitionData* result);
void           trimaSysClockInit (void);
void           trimaSysClock (void);
unsigned short trimaSysClockConnect (VOIDFUNCPTR func, unsigned short intervalMSec);
void           trimaSysClockDisconnect (VOIDFUNCPTR func);

#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */


#endif /* ifndef _TRIMA_OPSYS_INCLUDE */

/* FORMAT HASH fe121403fcf93e16067fe290e4f9f3e2 */

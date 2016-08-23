/*
 * Copyright (C) 2003 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: H:/BCT_Development/Trima5.R/Trima/os/rcs/power_fail.h 1.3 2008/03/06 17:00:21Z jheiusb Exp jheiusb $
 * $Log: power_fail.h $
 * Revision 1.3  2008/03/06 17:00:21Z  jheiusb
 *  -- c++ style comment moved
 * Revision 1.2  2008/03/06 16:56:05Z  jheiusb
 *  -- fix PFR safety write for vx5.5
 * Revision 1.1  2007/11/07 22:45:07Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:38:42Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:34:15Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:30:27Z  dslausb
 * Initial revision
 * Revision 1.2  2007/07/06 21:44:50Z  dslausb
 * Got rid of references to trimaOpSys stuff, because it's now obsolete
 * Revision 1.1  2007/05/22 18:20:16Z  dslausb
 * Initial revision
 *
 */

#ifndef _POWER_FAIL_INCLUDE
#define _POWER_FAIL_INCLUDE

#include <drv/hdisk/ataDrv.h>



#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

void trimaSysSaveSafetyPFData (char* data, unsigned int dataLength); /* safety-side only function */
void trimaSysClearAllPFData (void);                                  /* control-side only function */

/* public functions needed in platform_init.c: */
void trimaSysWriteSafetyPFData (int);  /* control-side only function */
void trimaSysClearSafetyPFData (void); /* control-side only function */

void trimaSysReceiveTimeSync (int serverSocket); /* in safety */
void trimaSysSendSafetyTimeSync (void);          /* in control */
void trimaSysRequestSafetytimeSync (void);       /* in safety */
/*
 *	Configuration information for power fail save.  Power fail data is
 * written to absolute disk blocks rather than through the file system
 * because of the limited time available to perform the save operation.
 */
extern ATA_RAW trimaSysControlPFSaveArea;
extern ATA_RAW trimaSysSafetyPFSaveArea;

#ifdef OS_BUILD


ATA_RAW trimaSysControlPFSaveArea =
{
   0,       /* cylinder */
   0,       /* head */
   0x10,    /* sector */
   NULL,    /* pBuf */
   0x14,    /* nSecs (maximum sectors available) */
   1        /* direction (1=write) */
};


/* The Safety image backs up to the Control one.  Make sure they don't overlap */
ATA_RAW trimaSysSafetyPFSaveArea =
{
   0,       /* cylinder */
   0,       /* head */
   0x24,    /* sector */
   NULL,    /* pBuf */
   0x6,     /* nSecs (maximum sectors available) */
   1        /* direction (1=write) */
};

#endif /* ifdef OS_BUILD */

#ifdef __cplusplus
}  // extern "C"
#endif /* ifdef __cplusplus */


#endif

/* FORMAT HASH d5ed21f728eeb3f38d96251de59bc01a */

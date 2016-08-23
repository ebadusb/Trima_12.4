/*
 * Copyright (C) 2013 Terumo BCT, Inc.  All rights reserved.
 *
 * API for saving data on Safety for warm boot situations.
 *
 *
 *
 */

#ifndef _SAFETY_WARM_RAM_INCLUDE_
#define _SAFETY_WARM_RAM_INCLUDE_

#include <vxWorks.h>

#include "auxclock.h"


/*
 * Make sure this file uses only constructs valid in both C and C++.  Although
 * it is used mostly from Trima .cpp files, the operating system image build
 * uses it from a .c file.
 */

// Typedef for C struct containing information to save for the Safe Exec task
typedef struct
{
   unsigned int sentinel;           // Always a fixed value
   unsigned int counter;

   unsigned int here;          // Magic numbers sprinkled through out the code
} safetyRamSafetyApplicationData;

enum { TimingHistorySize = 3 };

// Typedef for C struct containing information to save for the Safety Driver task
typedef struct
{
   unsigned int sentinel;      // Always a fixed value;
   unsigned int counter;

   unsigned int here;          // Magic numbers sprinkled through out the code

   rawTime      lastStartTime[TimingHistorySize];
   rawTime      lastEndTime[TimingHistorySize];
   int          lastTimeIndex;
} safetyRamSafetyDriverData;


#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

/* Accessor functions for accessing/updating/printing data */
safetyRamSafetyApplicationData* safetyRamAppData (void);
safetyRamSafetyDriverData*      safetyRamDriverData (void);
void                            safetyRamPrintData (void);


/* functions for kernel level operations */
void safetyRamInit (void);


#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */


#endif

/* FORMAT HASH 66874dc6a1e9a3cbd6bd765d6496f686 */

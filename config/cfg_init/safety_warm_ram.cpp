/*
 * Copyright (C) 2013 Terumo BCT, Inc.  All rights reserved.
 *
 * $Header: //depot/main/embedded/Trima/Main/config/cfg_init/safety_warm_ram.cpp#1 $
 *
 */

#include <vxWorks.h>
#include <stdio.h>
#include <ioLib.h>
#include <string.h>
#include <sysLib.h>

#include "trima_assert.h"
#include "safety_warm_ram.h"


static safetyRamSafetyApplicationData* safetyAppData    = NULL;
static safetyRamSafetyDriverData*      safetyDriverData = NULL;


const size_t AppDataSize    = sizeof(safetyRamSafetyApplicationData);
const size_t DriverDataSize = sizeof(safetyRamSafetyDriverData);

/* Accessor functions for accessing/updating data */
safetyRamSafetyApplicationData* safetyRamAppData (void)
{
   return safetyAppData;
}



safetyRamSafetyDriverData* safetyRamDriverData (void)
{
   return safetyDriverData;
}


/* functions for kernel level operations */
void safetyRamInit (void)
{
   /* Relies on kernel's USER_RESERVED_MEM (config.h) that will persist a warm boot */
   const char*  pUserBeg = sysMemTop();
   const char*  pUserEnd = sysPhysMemTop();
   const size_t offset   = 0x100;

   /* put app data at an offset from start of user reserved memory */
   safetyAppData = (safetyRamSafetyApplicationData*)(pUserBeg + offset);

   /* put driver data a little after the app data */
   safetyDriverData = (safetyRamSafetyDriverData*)(pUserBeg + offset + offset);

   /* Guard against overflowing the memory areas */
   char* pEnd1 = (char*)safetyAppData + AppDataSize;
   char* pEnd2 = (char*)safetyDriverData + DriverDataSize;
   trima_assert(pEnd1 < (char*)safetyDriverData);
   trima_assert(pEnd2 < pUserEnd);

   /* print off the data */
   safetyRamPrintData();

   /* zero it for the new run */
   memset(safetyAppData, 0, AppDataSize);
   memset(safetyDriverData, 0, DriverDataSize);
}



void safetyRamPrintData (void)
{
   printf("SAFETY RAM APP DUMP\n");
   printf("sentinel=%X\n", safetyAppData->sentinel);
   printf("counter=%X\n", safetyAppData->counter);
   printf("here=%d\n", safetyAppData->here);

   printf("SAFETY RAM DRIVER DUMP\n");
   printf("sentinel=%X\n", safetyDriverData->sentinel);
   printf("counter=%X\n", safetyDriverData->counter);
   printf("here=%d\n", safetyDriverData->here);

   printf("Time Index=%d\n", safetyDriverData->lastTimeIndex);

   for (int i = 0; i < TimingHistorySize; i++)
   {
      const int deltaTime = ( (safetyDriverData->lastEndTime[i].sec - safetyDriverData->lastStartTime[i].sec) * 1000) + ( (safetyDriverData->lastEndTime[i].nanosec - safetyDriverData->lastStartTime[i].nanosec) / 1000000);

      printf("Driver execution time %d Start:%d.%d End:%d.%d Delta %d\n",
             i,
             safetyDriverData->lastStartTime[i].sec, safetyDriverData->lastStartTime[i].nanosec,
             safetyDriverData->lastEndTime[i].sec, safetyDriverData->lastEndTime[i].nanosec,
             deltaTime);
   }
}

/* FORMAT HASH cf9b9681b8164183fdebff52d45dda8b */

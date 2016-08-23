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

#include "trima_assert.h"
#include "safety_warm_ram.h"


static safetyRamSafetyApplicationData* safetyAppData    = NULL;
static safetyRamSafetyDriverData*      safetyDriverData = NULL;


// Need to have a small offset from the base of the area
#define SAFETY_WARM_RAM_BASE 0x03E00100

// The protected region is 0x200000 minus the offset
#define SAFETY_WARM_RAM_SIZE 0x001FFF00

const unsigned int AppDataSize    = sizeof(safetyRamSafetyApplicationData);
const unsigned int DriverDataSize = sizeof(safetyRamSafetyDriverData);




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
   // Guard against overflowing the protected area.  For now, give them half each
   const int memorySize = AppDataSize + DriverDataSize;
   trima_assert(AppDataSize <= SAFETY_WARM_RAM_SIZE / 2);
   trima_assert(DriverDataSize <= SAFETY_WARM_RAM_SIZE / 2);

   /* start off the app data at the beginning */
   safetyAppData = (safetyRamSafetyApplicationData*)SAFETY_WARM_RAM_BASE;

   /* start off the driver data at the mid point (why not) */
   safetyDriverData = (safetyRamSafetyDriverData*)(SAFETY_WARM_RAM_BASE + 0x100000);

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

/* FORMAT HASH 91ef84bb42889e6497372a3678715a7b */

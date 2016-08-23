/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 *	Trima specific functions provided directly in the vxWorks operating
 * system image.
 *
 * $Header: E:/BCT_Development/TrimaVxPort/Trima/trima_common/rcs/trimaOs.cpp 1.2 2007/09/17 21:52:37Z rhecusb Exp $
 * $Log: trimaOs.cpp $
 * Revision 1.2  2007/09/17 21:52:37Z  rhecusb
 * Removed reference to userAppInit.h and config.h
 * Revision 1.4  2007/07/10 22:03:32Z  dslausb
 * Fixed sysClkRate stuff
 * Revision 1.3  2007/07/10 20:49:16Z  dslausb
 * Fixed cpuSpeedTest
 * Revision 1.2  2007/07/10 16:35:30Z  dslausb
 * Added a comment regarding the sysClkRate function.
 * Revision 1.1  2007/07/09 22:13:25Z  dslausb
 * Initial revision
 *
 */

#include <vxWorks.h>
#include <fcntl.h>
#include <in.h>
#include <inetLib.h>
#include <ioLib.h>
#include <sockLib.h>
#include <symLib.h>
#include <sysSymTbl.h>
#include <taskLib.h>
#include <tickLib.h>
#include <ftpdLib.h>

#include "load_driver.h"
#include "trimaOs.h"

extern unsigned long      cpuSpeedTestResult;

IMPORT TrimaPartitionData trimaPartitionData[EndOfPartitionEntries];

int sysClkRateGet (void);

int trimaSysGetPartitionData (TrimaPartitionID id, TrimaPartitionData* result)
{
   int retVal = -1;

   if ( id >= FirstPartition && id < EndOfPartitionEntries )
   {
      memcpy(result, &trimaPartitionData[id], sizeof(TrimaPartitionData));
      retVal = 0;
   }

   return retVal;
}

#ifdef CONTROL_BUILD

struct ClockConnectData
{
   VOIDFUNCPTR              func;
   unsigned short           tick;
   unsigned short           tickReload;
   struct ClockConnectData* next;
};

static struct ClockConnectData* connectList = NULL;
static SEM_ID                   connectSem  = NULL;

void trimaSysClockInit (void)
{
   connectSem = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE);
}

unsigned short trimaSysClockConnect (VOIDFUNCPTR func, unsigned short msecInterval)
{
   struct ClockConnectData* data = (struct ClockConnectData*)malloc(sizeof(struct ClockConnectData));
   int tick = (msecInterval * sysClkRateGet()) / 1000;

   if (func == NULL)
   {
      return 0;
   }

   if ( tick < 1 )
   {
      tick = 1;
   }

   data->func = func;
   data->tick = data->tickReload = tick;

   /*
    * The new function must be added to the list in a single atomic operation, since
    * the actual clock tick interrupt could occur at any time.  The semaphore lock
    * is to protect concurrent calls to the connect and disconnect functions.
    */
   semTake(connectSem, WAIT_FOREVER);
   data->next  = connectList;
   connectList = data;
   semGive(connectSem);

   return (tick * 1000) / sysClkRateGet();
}

void trimaSysClockDisconnect (VOIDFUNCPTR func)
{
   struct ClockConnectData* data = NULL;

   semTake(connectSem, WAIT_FOREVER);
   if ( connectList && connectList->func == func )
   {
      data        = connectList;
      connectList = data->next;
   }
   else
   {
      struct ClockConnectData* item = connectList;
      while ( item && item->next )
      {
         if ( item->next->func == func )
         {
            data       = item->next;
            item->next = data->next;
            break;
         }

         item = item->next;
      }
   }

   semGive(connectSem);
   if ( data )
   {
      free(data);
   }
}

void trimaSysClock (void)
{
   struct ClockConnectData* item = connectList;
   tickAnnounce();

   while ( item )
   {
      item->tick -= 1;
      if ( item->tick == 0 )
      {
         item->tick = item->tickReload;
         (*item->func)();
      }

      item = item->next;
   }
}

#endif /* ifdef CONTROL_BUILD */

void trimaSysStartDiskSync (void)
{
   int id;

   for ( id = FirstPartition; id < EndOfPartitionEntries; id += 1 )
   {
      if ( !trimaPartitionData[id].readOnly )
      {
         int fd = open(trimaPartitionData[id].name, O_RDONLY, 0);
         ioctl(fd, FIOSYNC, 0);
         close(fd);
      }
   }
}

/* FORMAT HASH 9392c6dbb59abce611ac3b881d559ca0 */

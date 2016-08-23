/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysLib.h>
#include <taskLib.h>
#include <ioLib.h>
#include <usrLib.h>

#define TRIMA_SYSINIT_BUILD
#include "file_system.h"
#include "node_data.h"
#include "trimaOs.h"
#include "trima_tasks.h"
#include "os_comm.h"

static bool taskDataInitialized = false;
static void initializeTaskData (void);

int trimaGetTaskInfo (TrimaTaskID id, TrimaTaskInfo* result)
{
   if (!taskDataInitialized)
   {
      initializeTaskData();
   }

   int retVal = -1;
   if (NULL == result)
   {
      // do nothing
   }
   else if (id >= TTID_FirstTask && id < TTID_EndOfTasks)
   {
      retVal  = 0;
      *result = trimaTaskInfo[id];
   }
   else
   {
      // initialize to null info
      *result = TrimaTaskInfo();
   }

   return retVal;
}

void trimaGetTaskStartupList (const TrimaTaskID** taskList, size_t* taskCount)
{
   if (sysGetLocalNodeID() == ControlNode)
   {
      *taskList  = trimaControlStartupOrder;
      *taskCount = sizeof(trimaControlStartupOrder) / sizeof(trimaControlStartupOrder[0]);
   }
   else
   {
      *taskList  = trimaSafetyStartupOrder;
      *taskCount = sizeof(trimaSafetyStartupOrder) / sizeof(trimaSafetyStartupOrder[0]);
   }
}

int trimaGetTaskID (TrimaTaskID id)
{
   if (!taskDataInitialized)
   {
      initializeTaskData();
   }

   int retVal = -1;
   if (id >= TTID_FirstTask && id < TTID_EndOfTasks)
   {
      retVal = trimaTaskInfo[id].tid;
   }

   return retVal;
}

void trimaSetTaskID (TrimaTaskID id, int tid)
{
   if (!taskDataInitialized)
   {
      initializeTaskData();
   }

   if (id >= TTID_FirstTask && id < TTID_EndOfTasks)
   {
      trimaTaskInfo[id].tid = tid;
   }
}

void trimaSetRouterQ (int tid, void* routerQ)
{
   if (!taskDataInitialized)
   {
      initializeTaskData();
   }

   for (int id = (int)TTID_FirstTask; id < (int)TTID_EndOfTasks; id++)
   {
      if (trimaTaskInfo[id].tid == tid)
      {
         trimaTaskInfo[id].routerQ = routerQ;
         return;
      }
   }

   DataLog(log_level_trima_library_error) << "Could not find ID " << hex << tid << dec
                                          << " in list for task name " << taskName(taskIdSelf()) << endmsg;
}

static SEM_ID        shutdownTriggerSem = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
static ShutdownData* data               = 0;
static size_t        count              = 0;


static void trimaShutdownTask ()
{
   size_t index = 0;
   int    msec  = 0;

   semTake(shutdownTriggerSem, WAIT_FOREVER);

   while (index < count)
   {
      if (msec >= data[index].msec)
      {
         TrimaTaskInfo taskInfo;
         trimaGetTaskInfo(data[index].taskID, &taskInfo);

         if (taskInfo.tid &&
             taskIdVerify(taskInfo.tid) == OK)
         {
            if ((data[index].signal & TS_TrimaSignalMask) != 0)
            {
               switch (data[index].signal & TS_TrimaSignalMask)
               {
                  case TS_Suspend :
                     taskSuspend(taskInfo.tid);
                     break;

                  case TS_ChangePriority :
                     taskPrioritySet(taskInfo.tid, data[index].signal & 255);
                     break;
               }
            }
            else
            {
               // If the kill crashes, use this to figure out who blew their stack.
               // printf("Killing task %s, id=%x, signal=%x, index=%d\n", taskInfo.taskName, taskInfo.tid, data[index].signal, index);
               // checkStack(taskInfo.tid);
               kill(taskInfo.tid, data[index].signal);
            }
         }

         index += 1;
      }
      else
      {
         taskDelay(1); // parasoft-suppress BD-TRS-TSHL "Intentional block in critical section"
         msec += 1000 / sysClkRateGet();
      }
   }
} // parasoft-suppress BD-TRS-LOCK "Unreleased lock of shutdownTriggerSem is intentional"

static SEM_ID shutdownSem = semBCreate(SEM_Q_PRIORITY, SEM_FULL);
void trimaStartShutdown (TrimaShutdownEvent event)
{
   //
   // Only allow one shutdown
   //
   if (semTake(shutdownSem, NO_WAIT) == OK)
   {
      if (sysGetLocalNodeID() == ControlNode)
      {
         trimaSysStartDiskSync();

         switch (event)
         {
            case TTE_NextProcedure :
               data  = trimaControlNextProcedureShutdown;
               count = sizeof(trimaControlNextProcedureShutdown) / sizeof(ShutdownData);
               osCommSend(OSComm_NextProcedure);
               break;

            case TTE_PowerFail :
               data  = trimaControlPowerFailShutdown;
               count = sizeof(trimaControlPowerFailShutdown) / sizeof(ShutdownData);
               break;

            default :
               data  = trimaControlFatalErrorShutdown;
               count = sizeof(trimaControlFatalErrorShutdown) / sizeof(ShutdownData);
               osCommSend(OSComm_Fatal);
               break;
         }
      }
      else
      {
         switch (event)
         {
            case TTE_NextProcedure :
               data  = trimaSafetyNextProcedureShutdown;
               count = sizeof(trimaSafetyNextProcedureShutdown) / sizeof(ShutdownData);
               osCommSend(OSComm_NextProcedure);
               break;

            case TTE_PowerFail :
               data  = trimaSafetyPowerFailShutdown;
               count = sizeof(trimaSafetyPowerFailShutdown) / sizeof(ShutdownData);
               break;

            default :
               data  = trimaSafetyFatalErrorShutdown;
               count = sizeof(trimaSafetyFatalErrorShutdown) / sizeof(ShutdownData);
               osCommSend(OSComm_Fatal);
               break;
         }
      }

      semGive(shutdownTriggerSem);
   }
} // parasoft-suppress BD-TRS-LOCK "Unreleased lock on shutdownSem is Intentional"

static void initializeTaskData (void)
{
   //
   // Since task ID values are guaranteed by design to be consecutive integers
   // starting at 0, we can use a fast linear time sort.  This sort also
   // insures that there are no duplicate or missing task ID values.
   //
   int currentSortPos = 0;

   while (currentSortPos < TTID_EndOfTasks)
   {
      int currentID = trimaTaskInfo[currentSortPos].id;
      if (currentID == currentSortPos)
      {
         //
         // Current task info block is already in the correct location
         //
         currentSortPos += 1;
      }
      else if (currentID >= TTID_FirstTask && currentID < TTID_EndOfTasks &&            // ID is valid
               currentID > currentSortPos && currentID != trimaTaskInfo[currentID].id)  // ID is not duplicated
      {
         //
         // Swap current record into the correct location
         //
         TrimaTaskInfo tempInfo(trimaTaskInfo[currentSortPos]);
         trimaTaskInfo[currentSortPos] = trimaTaskInfo[currentID];
         trimaTaskInfo[currentID]      = tempInfo;
      }
      else
      {
         printf("Internal software error - bad Trima task info array\n");
         exit(1);
      }
   }

   taskDataInitialized = true;

   taskSpawn("shutdown", // parasoft-suppress PB-27 "Ignore for VxWorks API"
             10, 0, 4000, (FUNCPTR)trimaShutdownTask, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

/* FORMAT HASH 6de8f519f984dfbaf379a9b2dfe77ccc */

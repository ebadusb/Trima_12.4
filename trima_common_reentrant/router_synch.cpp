/*
 * Copyright (c) 2011 by TerumoBCT, Inc.  All rights reserved.
 *
 * File: router_synch.cpp
 *
 * The purpose of this module is to:
 * 1.  Start the message system for the sysinit task.
 * 2.  Wait for the router to come up so we can continue starting the system.
 *     By not doing this, sometimes the alarm_handler starts before the router
 *     is ready and causes an error
 */

#include "messagesystem.h"
#include "router.h"
#include "trima_tasks.h"
#include "trima_datalog_levels.h"


// Points to the Node Router
static MessageSystem* ms = NULL;



extern "C" void wait_for_message_system ()
{
   // Has to be a new to keep the message system around after this function quits
   ms = new MessageSystem();

   ms->initNonBlock("sysinit");

   printf("Wait for message system to start...\n");

   while (MessageSystem::MsgSystem() == NULL)
   {
      taskDelay(sysClkRateGet() / 2);    /* delay 0.5 seconds */

      printf("still waiting...\n");
   }

   printf("Continuing...\n");
}



extern "C" void sysinit_cleanup ()
{
   printf("Deleting sysinit instance of the message system\n");

   delete ms;
   ms = NULL;
}



extern "C" int trimaNodeRouterQueueSize (void)
{
   int value = -1;

   if (Router::globalRouter() != NULL)
   {
      value = Router::globalRouter()->messageRemaining();
   }

   return value;
}



extern "C" void trimaReportTaskQueueSizes (void)
{
   const TrimaTaskID* taskList = NULL;
   size_t             taskListSize;

   trimaGetTaskStartupList(&taskList, &taskListSize);

   for (int task = 0; task < taskListSize; task++)
   {
      TrimaTaskInfo info;
      trimaGetTaskInfo(taskList[task], &info);

      if (info.routerQ != 0)
      {
         MessageSystem* ms = (MessageSystem*)info.routerQ;
         DataLog(log_level_trima_library_info) << "Task " << info.taskName << " queue size is " << ms->dispatcher().messageRemaining() << endmsg;
      }
   }
}

/* FORMAT HASH 6d1066bd1778da64f27955965ec32ac3 */

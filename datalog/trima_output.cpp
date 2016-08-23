/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: K:/BCT_Development/vxWorks/Trima/datalog/rcs/trima_output.cpp 1.7 2003/06/06 22:12:52Z jl11312 Exp jl11312 $
 * $Log: trima_output.cpp $
 * Revision 1.7  2003/06/06 22:12:52Z  jl11312
 * - add handling for system shutdown
 * Revision 1.5  2003/04/29 16:59:00Z  jl11312
 * save task ID numbers from spawning data log tasks
 * Revision 1.4  2003/02/25 20:43:35Z  jl11312
 * - added support for logging platform specific information in log header
 * Revision 1.3  2003/02/06 20:38:57  jl11312
 * - added support for symbolic node names
 * Revision 1.2  2002/08/22 20:17:23  jl11312
 * - added network support
 * Revision 1.1  2002/07/18 21:23:07  jl11312
 * Initial revision
 *
 */

#include <vxWorks.h>
#include "datalog.h"
#include "task_start.h"
#include "trima_tasks.h"

static DataLog_OutputTask* task = NULL;

static void tdatalog_local_output (const char* platformName, const char* nodeName, const char* platformInfo);
static void tdatalog_network_output (long connectTimeout, const char* nodeName);
static void tdatalog_outputSignal (int signal);

void datalog_StartLocalOutputTask (const char* platformName, const char* nodeName, const char* platformInfo)
{
   TrimaTaskInfo info;

   trimaGetTaskInfo(TTID_DataLogOutput, &info);
   info.tid = taskStart(info.taskName, info.priority, info.stackSize, (FUNCPTR)tdatalog_local_output, (int)platformName, (int)nodeName, (int)platformInfo, 0, 0, 0, 0, 0, 0, 0);
   trimaSetTaskID(TTID_DataLogOutput, info.tid);
}

void datalog_StartNetworkOutputTask (long connectTimeout, const char* nodeName)
{
   TrimaTaskInfo info;

   trimaGetTaskInfo(TTID_DataLogOutput, &info);
   info.tid = taskStart(info.taskName, info.priority, info.stackSize, (FUNCPTR)tdatalog_network_output, (int)connectTimeout, (int)nodeName, 0, 0, 0, 0, 0, 0, 0, 0);
   trimaSetTaskID(TTID_DataLogOutput, info.tid);
}

static void tdatalog_local_output (const char* platformName, const char* nodeName, const char* platformInfo)
{
   signal(SIGINT, tdatalog_outputSignal);
   signal(SIGQUIT, tdatalog_outputSignal);

   task = new DataLog_LocalOutputTask(platformName, nodeName, platformInfo);
   task->main();

   delete task;
}

static void tdatalog_network_output (long connectTimeout, const char* nodeName)
{
   signal(SIGINT, tdatalog_outputSignal);

   task = new DataLog_NetworkOutputTask(connectTimeout, nodeName);
   task->main();

   delete task;
}

static void tdatalog_outputSignal (int signal)
{
   if ( task )
   {
      if ( signal == SIGINT )
      {
         task->exit();
      }
      else if ( signal == SIGQUIT )
      {
         task->exitImmediately();
      }
   }
}

/* FORMAT HASH d12613f0fa41a1707eac06786905c3d7 */

/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: K:/BCT_Development/vxWorks/Trima/datalog/rcs/trima_network.cpp 1.5 2003/06/06 22:12:47Z jl11312 Exp jl11312 $
 * $Log: trima_network.cpp $
 * Revision 1.5  2003/06/06 22:12:47Z  jl11312
 * - add handling for system shutdown
 * Revision 1.4  2003/05/21 19:53:48Z  jl11312
 * - enhanced memory protection (IT 6091)
 * Revision 1.3  2003/04/29 16:58:56Z  jl11312
 * save task ID numbers from spawning data log tasks
 * Revision 1.2  2003/02/25 16:12:42Z  jl11312
 * - modified buffering scheme to help prevent buffer overruns
 * Revision 1.1  2002/08/22 20:17:07  jl11312
 * Initial revision
 *
 */

#include <vxWorks.h>

#include "datalog.h"
#include "list.h"
#include "portnumbers.h"
#include "task_start.h"
#include "trima_tasks.h"

#ifndef DATALOG_NETWORK_SUPPORT
#error "Trima requires datalog network support"
#endif /* ifndef DATALOG_NETWORK_SUPPORT */

static void tdatalog_network (int port);
static void tdatalog_networkClient (int clientSocket, struct sockaddr_in* clientAddr);

void datalog_StartNetworkTask (void)
{
   TrimaTaskInfo info;

   trimaGetTaskInfo(TTID_DataLogNetwork, &info);
   info.tid = taskStart(info.taskName, info.priority, info.stackSize, (FUNCPTR)tdatalog_network, Trima_DataLogPort, 0, 0, 0, 0, 0, 0, 0, 0, 0);
   trimaSetTaskID(TTID_DataLogNetwork, info.tid);
}

static void tdatalog_network (int port)
{
   DataLog_NetworkTask networkTask(port);
   networkTask.main();
}

void datalog_StartNetworkClientTask (int clientSocket, struct sockaddr_in* clientAddr)
{
   TrimaTaskInfo info;

   trimaGetTaskInfo(TTID_DataLogNetworkClient, &info);
   taskStart(info.taskName, info.priority, info.stackSize, (FUNCPTR)tdatalog_networkClient, clientSocket, (int)clientAddr, 0, 0, 0, 0, 0, 0, 0, 0);
}

static void tdatalog_networkClient (int clientSocket, struct sockaddr_in* clientAddr)
{
   DataLog_NetworkClientTask task(clientSocket, clientAddr);
   task.main();
}

/* FORMAT HASH ee670724343d69e4a636d456ee0f4c88 */

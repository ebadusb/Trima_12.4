/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/datalog/rcs/trima_periodic.cpp 1.2 2003/05/21 19:53:41Z jl11312 Exp dslausb $
 * $Log: trima_periodic.cpp $
 * Revision 1.2  2003/05/21 19:53:41Z  jl11312
 * - enhanced memory protection (IT 6091)
 * Revision 1.1  2002/08/15 21:07:38Z  jl11312
 * Initial revision
 *
 */

#include "datalog.h"
#include "task_start.h"
#include "trima_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

static void tdatalog_periodic (DataLog_SetHandle set);

void datalog_StartPeriodicLogTask (DataLog_SetHandle set)
{
   TrimaTaskInfo info;

   trimaGetTaskInfo(TTID_DataLogPeriodic, &info);
   taskStart(info.taskName, info.priority, info.stackSize, (FUNCPTR)tdatalog_periodic, (int)set, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

static void tdatalog_periodic (DataLog_SetHandle set)
{
   DataLog_PeriodicTask task(set);
   task.main();
}

#ifdef __cplusplus
}  // extern "C"
#endif /* ifdef __cplusplus */

/* FORMAT HASH ae1319fd8c5cb3a14926e7f4553c45f8 */

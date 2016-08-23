/*
 * Copyright (C) 2003 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/os/rcs/task_start.h 1.1 2007/11/07 22:38:45Z dslausb Exp $
 * $Log: task_start.h $
 * Revision 1.1  2007/11/07 22:38:45Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:34:18Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:30:44Z  dslausb
 * Initial revision
 * Revision 1.1  2007/05/22 18:20:18Z  dslausb
 * Initial revision
 * Revision 1.1  2003/05/21 20:01:56Z  jl11312
 * Initial revision
 *
 */

#ifndef _TASK_START_INCLUDE
#define _TASK_START_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

int taskStart (const char* taskName, int priority, unsigned long stackSize, FUNCPTR entryPoint, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10);

#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _TASK_START_INCLUDE */

/* FORMAT HASH 5688f578dbec02a3098eb44764ef4914 */

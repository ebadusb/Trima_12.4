/**
 * Copyright (C) 2012 Terumo BCT, Inc.  All rights reserved.
 *
 * @file   hangman.c
 *
 * @author mra1usb
 * @date   Sep 7, 2012
 *
 * @brief  Provides utilities that can be run from a WindSh to suspend tasks
 *         for a programmed interval, and then resume.
 *
 * Compile this module. Launch a WindShell (or telnet) terminal, load the
 * object file, et voila!
 *
 * Example:
 *   $ windsh TS-T1006@USBLR9R3EFL
 *   -> ld 1, 0, "hangman.o"
 *   -> showem 1
 *   -> hangem 5, "dlog_per barcode_reader 0x2823984"
 */

/* vxworks includes */
#include <vxWorks.h>
#include <taskLib.h>
#include <sysLib.h>
#include <usrLib.h>
#include <errnoLib.h>
#include <stdio.h>
#include <string.h>

int showem( int show_tid );
int hangem( int secsToSuspend, const char *taskNames, int runForeground );
int hangit( int tid, char* tname, int secsToSuspend );

#define MAX_TASKS   99

/**
 * Print all of the active task names, and optionally their task ids.
 *
 * @param  show_tid - if true, also print the task ids
 * @return the number of active tasks
 */
int showem( int show_tid )
{
   int idList[MAX_TASKS] =
                           { 0 };
   int cntTasks = taskIdListGet( idList, MAX_TASKS );
   int idx;

   for ( idx = 0; idx < cntTasks; idx++ )
   {
      int tid = idList[idx];

      if ( show_tid )
      {
         printf( "tid[%02d]:  %#08x   %s\n", idx, tid, taskName( tid ) );
      }
      else
      {
         printf( "%s\n", taskName( tid ) );
      }
   }

   printf( "### cntTasks=%d\n", cntTasks );
   return cntTasks;
}

/**
 * Runs hangit() for each task in the list of task names.
 * The caller is blocked until all tasks have been processed.
 * Use showem() to print the list of active tasks.
 *
 * @param secsToSuspend - number of seconds to suspend each task.
 * @param taskNames     - string of space/comma-separated task names
 *                        and/or hexadecimal task ids
 * @param runForeground - if true, run hangit() in the foreground; otherwise,
 *                        runs in the background so that printf() output goes
 *                        to DLOG
 *
 * @return the number of tasks suspended
 */
int hangem( int secsToSuspend, const char *taskNames, int runForeground )
{
   int okayCnt = 0;
   int rval = ERROR;
   int taskPriority = 100;
   int taskOptions = 0;
   int taskStackSize = 100000;

   const size_t MAX_STRLEN = 512;
   char *clone = calloc( MAX_STRLEN, sizeof(char) );
   char *pNext = NULL;
   char *aTask = NULL;
   char separators[] = " ,";
   int  tid = 0;

   if ( taskNames == NULL ) return 0;

   /* copy the input string */
   strncpy( clone, taskNames, MAX_STRLEN - 1 );

   /* For each task name ... */
   for ( aTask = strtok_r( clone, separators, &pNext );
         aTask != NULL;
         aTask = strtok_r( NULL, separators, &pNext ) )
   {
      /* check for hex task id */
      if ( aTask[0] == '0' )
      {
         tid = (int) strtoul( aTask, NULL, 16 );
      }
      else
      {
         tid = taskNameToId( aTask );
      }

      /* Check that this is a valid task */
      if ( tid <= 0 || taskIdVerify( tid ) == ERROR )
      {
         printf( "%s(): Skipping invalid task name/id: '%s')\n",
                 __FUNCTION__, aTask );
         continue;
      }

      if ( runForeground )
      {
         rval = hangit( tid, 0, secsToSuspend );
      }
      else
      {
         /* run hangit() in the background so that its output will go to DLOG */
         printf( "%s(): Hanging task %08#x '%s' for %d seconds ... ",
                 __FUNCTION__, tid, taskName( tid ), secsToSuspend );

         rval = taskSpawn( 0, taskPriority, taskOptions, taskStackSize,
                           hangit, tid, 0, secsToSuspend,
                           0, 0, 0, 0, 0, 0, 0 );
         if ( rval == ERROR )
         {
            printf( "\ntaskSpawn failed errno=%d\n", errno );
            printErrno( errnoGet() );
         }
         else
         {
            /* sleep long enough for hangit() to do its job */
            taskDelay( sysClkRateGet() * (secsToSuspend + 2) );
            okayCnt++;
            printf( "okayCnt=%d\n", okayCnt );
         }
      }

   } /* for each task ... */

   /* free the allocated clone */
   if ( clone ) free( clone );

   return okayCnt;
}

/**
 * Suspends the given task for a period of time, and then resumes it.
 * The calling task is blocked until the period expires.
 *
 * @param  tid    - the task ID to suspend
 * @param  tname  - optional task name to suspend (used if tid is zero)
 * @param  secsToSuspend - seconds to suspend. Range is [1, 60]
 * @return OK on success; otherwise, ERROR
 */
int hangit( int tid, char *tname, int secsToSuspend )
{
   /* ==================== check preconditions */

   if ( tid == 0 && tname != NULL )
   {
      tid = taskNameToId( tname );
   }

   if ( tid <= 0 || taskIdVerify( tid ) == ERROR )
   {
      printf( "%s(): Invalid task! %d '%s'\n", __FUNCTION__, tid, tname );
      return ERROR;
   }

   if ( secsToSuspend < 1 || secsToSuspend > 60 )
   {
      printf( "%s(): Invalid secsToSuspend=%d\n", __FUNCTION__, secsToSuspend );
      return ERROR;
   }

   /* ==================== preconditions okay */

   tname = taskName( tid );

   printf( "%s(): SUSPENDING task %08#x -> '%s'\n", __FUNCTION__, tid, tname );

   if ( taskSuspend( tid ) == ERROR )
   {
      printf( "%s(): taskSuspend( %08#x ) failed!\n", __FUNCTION__, tid );
      return ERROR;
   }

   while ( secsToSuspend > 0 )
   {
      printf( "%s(): resuming task %08#x -> '%s' in %d seconds\n", __FUNCTION__,
              tid, tname, secsToSuspend );

      /* Sleep for 1 second */
      taskDelay( sysClkRateGet() /* ticks per second */);
      --secsToSuspend;
   }

   if ( taskResume( tid ) == ERROR )
   {
      printf( "%s(): taskResume( %08#x ) failed!\n", __FUNCTION__, tid );
      return ERROR;
   }
   else
   {
      printf( "%s(): RESUMED task %08#x -> '%s'\n", __FUNCTION__, tid, tname );
   }

   return OK;
}

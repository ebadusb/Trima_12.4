/*
 * $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/os/rcs/task_start.c 1.1 2007/11/07 22:45:08Z dslausb Exp $
 *
 *	Start task with stack overrun protection
 *
 * $Log: task_start.c $
 * Revision 1.1  2007/11/07 22:45:08Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:38:44Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:34:18Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:30:41Z  dslausb
 * Initial revision
 * Revision 1.1  2007/05/22 18:20:18Z  dslausb
 * Initial revision
 * Revision 1.1  2005/01/17 18:24:11Z  ms10234
 * Initial revision
 * Revision 1.2  2004/06/08 22:18:29Z  jl11312
 * - no vm... functions in simulator
 * Revision 1.1  2004/03/04 17:55:46Z  jl11312
 * Initial revision
 */

#include <vxWorks.h>
#include <taskLib.h>
#include <vmLib.h>

#include "task_start.h"

static void preTaskEntryFunction (FUNCPTR entryPt, int arg2, int arg3, int arg4, int arg5,
                                  int arg6, int arg7, int arg8, int arg9, int arg10);

static void preTaskEntryFunction (FUNCPTR entryPt, int arg2, int arg3, int arg4, int arg5,
                                  int arg6, int arg7, int arg8, int arg9, int arg10)
{
   __asm__ volatile ( "fninit" );

   if ( entryPt ) (* entryPt)(arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

int taskStart (const char* taskName, int priority, unsigned long stackSize,
               FUNCPTR entryPoint,
               int arg1, int arg2, int arg3, int arg4, int arg5,
               int arg6, int arg7, int arg8, int arg9, int arg10)
{
   WIND_TCB*     tid             = (WIND_TCB*)malloc(sizeof(WIND_TCB));
   unsigned long paddedStackSize = (stackSize + 0x0fff) & 0xfffff000;

#if CPU==SIMNT
   void* stack = malloc(paddedStackSize + 0x1000);
#else
   void* stack = valloc(paddedStackSize + 0x1000);
#endif /* CPU==SIMNT */

   int result = ERROR;

   if ( tid && stack )
   {
      unsigned long stackBaseAddr = ((unsigned long)stack) + paddedStackSize + 0x1000;

#if CPU!=SIMNT
      vmBaseStateSet(NULL, stack, 0x1000, VM_STATE_MASK_VALID, VM_STATE_VALID_NOT);
#endif /* CPU!=SIMNT */

      if ( (taskInit(tid, (char*)taskName, priority, VX_FP_TASK,
                     (char*)stackBaseAddr, paddedStackSize, (FUNCPTR)&preTaskEntryFunction,
                     (int)entryPoint, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) == OK) &&
           (taskActivate((int)tid) == OK) )
      {
         result = (int)tid;
      }
      else
      {
         free(tid);
         free(stack);
      }
   }

   return result;
}

/* FORMAT HASH 521a05ddd6ba64d25e99bd1d691a26ee */

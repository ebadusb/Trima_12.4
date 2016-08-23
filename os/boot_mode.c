/*
 *
 * $Header$
 *
 *  boot-mode functions
 *
*/


#include <vxWorks.h>
#include <sysLib.h>

#include "boot_mode.h"
#include "node_data.h"

int bootSingleStepEnabled (void)
{
   return(trimaSingleStepBootMode);
}

int bootNormalModeEnabled (void)
{
   return(trimaNormalBootMode && !getenv("SERVICE_MODE"));
}


int bootServiceModeEnabled (void)
{
   return(trimaNormalBootMode && getenv("SERVICE_MODE"));
}

int bootAltModeEnabled (void)
{
   return(!trimaNormalBootMode);
}

int bootKeyboardAttached (void)
{
   return(trimaKeyboardAttached);
}

int bootSafetyBootFailed (void)
{
   return(safetyBootFailed);
}

int bootSafetyIsAlive (void)
{
   return(safetyAlive);
}



#ifdef CONTROL_BUILD
enum { BASE                 = 0x0B00 };    /* lower I/O base address for control */
enum { IN_BUTTONS           = BASE + 0x14 };
enum { NOT_STOP_SWITCH_BIT  = 0x02 };      /* stop switch bit for control */
enum { NOT_PAUSE_SWITCH_BIT = 0x01 };      /* pause switch bit for control */
#endif /* CONTROL_BUILD */

#ifdef SAFETY_BUILD
enum { BASE                 = 0x0700 };    /* I/O base address for safety */
enum { IN_BUTTONS           = BASE + 0x06 };
enum { NOT_STOP_SWITCH_BIT  = 0x08 };      /* stop switch bit for safety */
enum { NOT_PAUSE_SWITCH_BIT = 0x04 };      /* pause switch bit for safety */
#endif /* SAFETY_BUILD */

int bootPauseButtonPressed (void)
{
   unsigned char value  = sysInByte(IN_BUTTONS);
   int           result = 0;

   if (!(value & NOT_PAUSE_SWITCH_BIT)) result = 1;

   return result;
}

int bootStopButtonPressed (void)
{
   unsigned char value  = sysInByte(IN_BUTTONS);
   int           result = 0;

   if (!(value & NOT_STOP_SWITCH_BIT)) result = 1;

   return result;
}

TRIMAOSSingleStepStatus bootPromptSingleStep (const char* msg)
{
   if (trimaSingleStepBootMode)
   {
      printf("press and release pause button to %s\r", msg);
      while (!bootPauseButtonPressed())
      {
         taskDelay(1);
      }

      if (bootStopButtonPressed())
      {
         return SingleStep_Abort;
      }

      taskDelay(2);
      while (bootPauseButtonPressed())
      {
         taskDelay(1);
      }

      printf("\n");
   }
   return SingleStep_Continue;
}

/* FORMAT HASH 5b932a23048af1d207cd752c8dfec56c */

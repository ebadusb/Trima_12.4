/*
 * $Header$
 *
 * platform initialization (common to all nodes)
 *
 */

#include <vxWorks.h>
#include <fcntl.h>
#include <ioLib.h>
#include <inetLib.h>
#include <shellLib.h>
#include <sockLib.h>
#include <setjmp.h>
#include <stdio.h>
#include <sysLib.h>
#include <sysSymTbl.h>
#include <taskLib.h>
#include <tickLib.h>
#include <unistd.h>
#include <bootLib.h>
#include <rebootLib.h>
#include <time.h>

#include "log_trap.h"
#include "task_start.h"
#include "filenames.h"
#include "boot_mode.h"
#include "os_comm.h"
#include "power_fail.h"
#include "trima_driver.h"

#include "config.h"


void osCommTask(void);


#define OS_BUILD
#include "node_data.h"
#undef OS_BUILD


/* Entry point for platform initialization */
void usrCommonInitSetError(const char *msg);
void usrCommonInitSetFatalError(const char *msg);
void usrStartSysInit(const char *msg);
void usrShellInit(void);
void platformInit(void);
void resetSystem(void);

extern void nodeInit(void);
extern void trimaSysWriteSafetyPFData(int);  
extern void trimaSysClearSafetyPFData(void);
extern void trimaSysReceiveTimeSync(int serverSocket);    /* in safety */
extern void trimaSysSendSafetyTimeSync(void); /* in control */
extern void trimaSysRequestSafetytimeSync(void);         /* in safety */

#include "crc.c"
#include "load_driver.c"
#include "load_module.c"
#include "log_trap.c"
#include "task_start.c"

#include "boot_mode.c"
#include "option_file.c"
#include "trima_options.c"
#include "power_fail.c"
#include "cpu_speed.c"
#include "trima_driver.c"

#ifdef CONTROL_BUILD
#include "discover_hw.c"
#endif

/*
 * Routines for handling errors during OS initialization
 */
static int initErrorDetected = 0;
jmp_buf usrInitAbort;

BOOL trimaSingleStepBootMode = FALSE;
BOOL trimaNormalBootMode     = TRUE;
BOOL trimaKeyboardAttached   = FALSE;
BOOL safetyBootFailed        = TRUE;
BOOL safetyBootInProgress    = TRUE;    // Default to true to make sure we don't deny safety from booting
BOOL safetyAlive             = FALSE;


void usrCommonInitSetError(const char * msg)
{
   /*
    * Error detected.  System initialization will continue to end of usrAppInit()
    * and then drop to target shell prompt.  A delay is used after displaying the
    * error message to insure that it can be read before subsequent messages are
    * displayed.
    */
   printf("%s\n", msg);
   initErrorDetected = 1;

   printf("attempting to perform remaining initialization steps\n");
   taskDelay(2*sysClkRateGet());
}



void usrCommonInitSetFatalError(const char * msg)
{
   /*
    * Fatal error detected (i.e. initialization can not be allowed to proceed).
    * The system will drop to the target shell prompt without performing any
    * further initialization.  A delay is used after displaying the
    * error message to insure that it can be read.
    */
   printf("%s\n", msg);
   printf("aborting system startup\n");
   initErrorDetected = 1;
   usrShellInit();

   taskDelay(2*sysClkRateGet()); // parasoft-suppress BD-TRS-TSHL "Intentional block in critical section"

   longjmp(usrInitAbort, 1);
}



void usrStartSysInit(const char * fileName)
{
   LoadModuleInfo info;
   SYM_TYPE       symType;
   void (*sysInitFunc)(void) = NULL;

#ifdef CONTROL_BUILD
   if (bootPromptSingleStep("load sysinit module") == SingleStep_Abort)
   {
      longjmp( usrInitAbort, 1 );
   }
#endif /* CONTROL_BUILD */

   if (loadModuleFromFile(fileName, &info) != OK)
   {
      usrCommonInitSetFatalError("failed to load system initialization module");
   }

   if (symFindByName(sysSymTbl, "sysinit_main", (char **)&sysInitFunc, &symType) != OK)
   {
      perror("sysinit_main");
      usrCommonInitSetFatalError("Failed to locate system initialization function");
   }

#ifdef CONTROL_BUILD
   if (bootPromptSingleStep("start Trima system initialization") == SingleStep_Abort)
   {
      longjmp( usrInitAbort, 1 );
   }
#endif /* CONTROL_BUILD */

   taskStart("sysinit", 100, 40000, (FUNCPTR)sysInitFunc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}



extern int shellTaskPriority;

void usrShellInit()
{
   if (trimaKeyboardAttached)
   {
      shellTaskPriority = 100;
      shellInit(60000, TRUE);
   }
}


/*
 * Main entry point
 */
void platformInit(void)
{
   BOOT_PARAMS params;
/* Recover memory for safety from the bootrom space */

#ifndef CONTROL_BUILD

   if ( memPartAddToPool( memSysPartId, (char *)(0x00010000), 0x00090000 ) != 0 )
   {
      printf("Unable to Add to Memory Pool\n");
   }

#endif

   if (!setjmp(usrInitAbort))
   {
	   bootStringToStruct(BOOT_LINE_ADRS,&params);
      /*
       * Normal initialization
       */

      /* Trap stdin/stdout to a file */
      logTrapInit(PIPE_LOGTRAP, PIPE_LOGTRAP_BUFFER, 12000);

	  if (params.flags & SYSFLG_VENDOR_0)
      {
		 trimaNormalBootMode = FALSE;
	  }

      if (params.flags & SYSFLG_VENDOR_1)
      {
		  printf("Keyboard Detected\n");
		  trimaKeyboardAttached = TRUE;
	  }

      nodeInit();

      taskStart("os_comm", 10, 4000, (FUNCPTR)osCommTask, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); 

      usrStartSysInit( sysGetLocalNodeInitModule() );
   }
   else
   {
      /*
       * Initialization error detected - drop to target shell prompt
       */
      usrShellInit();
   }
}

static OSCommCallback *osCommCallback = NULL;

void osCommTask(void)
{
   struct sockaddr_in  addr;
   unsigned int    osMsg;
   int rcvBytes;
   int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
   memset(&addr, 0, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = INADDR_ANY;
   addr.sin_port = htons(Trima_OSPort);
   bind(serverSocket, (struct sockaddr *)&addr, sizeof(addr));


#ifdef SAFETY_BUILD
    // assumtion: control comes up first
   trimaSysRequestSafetytimeSync();
#endif

   while (1)
   {
      struct sockaddr_in  clientAddr;
      int     clientAddrSize = sizeof(clientAddr);

      rcvBytes = recvfrom(serverSocket, (char *)&osMsg, sizeof(osMsg), 0, (struct sockaddr *)&clientAddr, &clientAddrSize);

      if (rcvBytes > 0)
      {
         switch (osMsg)
         {
         case OSComm_Reset:
             resetSystem();
             break; 

#ifdef CONTROL_BUILD
         case OSComm_WriteSafetyPFData:
			 trimaSysWriteSafetyPFData(serverSocket);
		     break;

         case OSComm_RequestTimeData:
			 trimaSysSendSafetyTimeSync();
			 if (safetyBootInProgress)
			 {
				 safetyBootInProgress = FALSE;   // This identifies that Safety is booted (enough)
				 printf("Safety started successfully!\n");
			 }
             safetyBootFailed     = FALSE;
		     break;

		 case OSComm_ClearSafetyPFData:
		     trimaSysClearSafetyPFData();
		     break;
#endif /* ifdef CONTROL_BUILD */

#ifdef SAFETY_BUILD
         case OSComm_SendTimeData:
              trimaSysReceiveTimeSync(serverSocket);
              break;
#endif /* ifdef SAFETY_BUILD */

#ifdef CONTROL_BUILD
            case OSComm_SafetyAlive:
               safetyAlive = TRUE;
               break;
#endif /* ifdef CONTROL_BUILD */

         default:
            if (osCommCallback)
            {
               (*osCommCallback)((OSCommData)osMsg);
            }
            break;
         }
      }
      else if (rcvBytes < 0)
      {
         taskDelay(30);
      }
   }
}

void osCommSetCallback(OSCommCallback * callback)
{
   osCommCallback = callback;
}

void osCommSend(OSCommData data)
{
   struct sockaddr_in  addr;
   unsigned int    buffer = data;
   int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

   memset(&addr, 0, sizeof(addr));
   addr.sin_family = AF_INET;

   switch (sysGetLocalNodeID())
   {
   case ControlNode:
      addr.sin_addr.s_addr = nodeData[ControlNode].ipAddress[SafetyNode];
      break;
   case SafetyNode:
      addr.sin_addr.s_addr = nodeData[SafetyNode].ipAddress[ControlNode];
      break;
   default:
      usrCommonInitSetFatalError( "unknown node attempting OS comm send" );
   }

   addr.sin_port = htons(Trima_OSPort);

   sendto(clientSocket, (char *)&buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, sizeof(addr));
   close(clientSocket);
}



void resetSystem(void)
{
   SYM_TYPE symType;
   void (*hwResetFunc)(void) = NULL;

   if (symFindByName(sysSymTbl, "hw_forceReset", (char **)&hwResetFunc, &symType) == OK)
   {
      (*hwResetFunc)();
   }
   else
   {
      printf("failed to locate trima hardware reset function\n");
      reboot(BOOT_CLEAR);
   }

   abort();
}


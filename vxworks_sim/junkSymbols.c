/*
* Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
*
* TITLE:      junkSymbols.c
*
* DESCRIPTIONS: This file exists to be included from usrAppInit.c in order to match dependencies
* in the simulator.  So, basically this is a collection of global functions and variables needed
* to get the software to compile.
*
* $Header$
*
*/

#include "node_data.h"
#include "file_system.h"
#include "node_data.c"
#include "trima_options.c"
#include "power_fail.c"
#include "option_file.c"
#include "version.c"
#include "winSio.c"
#include "ntEnd.c"
#include "load_module.c"
#include "crc.c"

static int  initErrorDetected = 0;
jmp_buf usrInitAbort;

BOOL safetyBootFailed = FALSE;
BOOL safetyBootInProgress = FALSE;

unsigned char isAmpro(void)		{ return 0; }
int bootSingleStepEnabled(void) { return 0; }
int bootNormalModeEnabled(void) { return 1; }
int bootServiceModeEnabled(void){ return 0; }
int bootAltModeEnabled(void)    { return 0; }
int bootPauseButtonPressed(void){ return 0; }
int bootStopButtonPressed(void) { return 0; }

int extraAuxClockRateGet(void)	{ return 1; }
STATUS extraAuxClockConnect(FUNCPTR routine, int arg) { return OK; }

void arpShow( void ) {}

STATUS ataRawio(int ctrl,
                int drive,
                ATA_RAW *pAtaRaw)
{
	fprintf(stderr, "Somebody is calling ataRawio for ctrl %d, drive %d.  Not cool.\n", ctrl, drive);
	return 0;
}

SysNodeID sysGetLocalNodeID(void)
{
   return ControlNode;
}

int taskStart(const char * taskName, int priority, unsigned long stackSize, FUNCPTR entryPoint, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10)
{
	WIND_TCB	* tid = (WIND_TCB *)malloc(sizeof(WIND_TCB));
	unsigned long	paddedStackSize = (stackSize + 0x0fff) & 0xfffff000;

#if CPU==SIMNT
	void * stack = malloc(paddedStackSize+0x1000);
#else /* if CPU==SIMNT */
	void * stack = valloc(paddedStackSize+0x1000);
#endif /* if CPU==SIMNT */

	int  result = ERROR; 

	if ( tid && stack )
	{
		unsigned long stackBaseAddr = ((unsigned long)stack) + paddedStackSize + 0x1000;

#if CPU!=SIMNT
		vmBaseStateSet(NULL, stack, 0x1000, VM_STATE_MASK_VALID, VM_STATE_VALID_NOT);
#endif /* if CPU!=SIMNT */

		if ( taskInit(tid, (char *)taskName, priority, VX_FP_TASK, (char *)stackBaseAddr, paddedStackSize, entryPoint, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) == OK )
		{
			taskActivate((int)tid);
			result = (int)tid;
		}
	}

	if ( result == ERROR )
	{
		if ( tid ) free(tid);
		if ( stack ) free(stack);
	}

	return result;
}

void usrCommonInitSetFatalError(const char * msg)
{
   /*
    * Fatal error detected (i.e. initialization can not be allowed to proceed).
    * The system will drop to the target shell prompt without performing any
    * further initialization.  A delay is used after displaying the
    * error message to insure that it can be read.
    */
   fprintf(stderr, "%s\n", msg);
   fprintf(stderr, "aborting system startup\n");
   initErrorDetected = 1;
   usrShellInit();

   taskDelay(2*sysClkRateGet());
   longjmp(usrInitAbort, 1);
}

void resetSystem(void)
{
   SYM_TYPE symType;
   void (*hwResetFunc)(void);

   if (symFindByName(sysSymTbl, "forceReset__6trimaHW", (char **)&hwResetFunc, &symType) == OK)
   {
      (*hwResetFunc)();
   }
   else
   {
      fprintf(stderr, "failed to locate trima hardware reset function\n");
      reboot(BOOT_CLEAR);
   }

   abort();
}

static OSCommCallback   * osCommCallback = NULL;
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

unsigned short hw_hardwareVersion(void)
{
	fprintf(stderr, "Somebody is calling hw_hardwareVersion.  Not cool.\n");
	return 0;
}


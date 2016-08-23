/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 */

#include <vxWorks.h>
#include <ctype.h>
#include <envLib.h>
#include <fcntl.h>
#include <ioLib.h>
#include <loadLib.h>
#include <stdio.h>
#include <sysLib.h>
#include <sysSymTbl.h>
#include <ioctl.h>
#include <netShow.h>
#include <arpLib.h>
#include <usrLib.h>
// Force definition of alarm table
#define ALARM_LIST
#include "alarms.h"

#include "failure_debug.h"
#include "load_module.h"
#include "log_trap.h"
#include "sysinit.h"
#include "task_start.h"
#include "datalog.h"

#include "trima_datalog.h"
#include "node_data.h"
#include "filenames.h"
#include "boot_mode.h"
#include "os_comm.h"

#include "trima_options.h"
#include "common_hw.h"
#include "node_data.h"
#include "zlib.h"

#if CPU==SIMNT // The simulator has no need for arplib/netshow, so just fake a few symbols here.
#define arpShow() { printf("This would normally be an arpShow on Trima."); }
#define routeShow() { printf("This would normally be an routeShow on Trima."); }
#define ifShow(__junk__) { printf("This would normally be an ifShow(%s) on Trima.", __junk__); }
#define routestatShow() { printf("This would normally be an routestatShow on Trima."); }
#endif

static bool datalogInitOK          = false;
static bool serviceModeAcknowledge = false;


// Extern definitions
SEM_ID   taskSynchronizationSem = NULL;    // used for task synchronization during startup
jmp_buf  sysinit_abort;
BootMode sysinit_boot_mode      = NormalBootMode;

// Forward declarations
void usrSeedArpTable (void);


//
// Utility routine for getting stack frame pointer (BP register) - used
// to provide stack dump information.  Note that this function needs to
// have a valid frame pointer and should never be declared as inline.
//
static unsigned int* GetStackFramePointer (void)
{
   //
   // Get frame pointer for this function
   //
   unsigned int* result;  // parasoft-suppress INIT-04 "initialized by ASM"
   __asm__ volatile ("movl %%ebp, %%eax" : : : "eax"     );
   __asm__ volatile ("movl %%eax, %0"    : "=m" (result) );

   //
   // Need to go back one stack level to get the frame pointer for the calling function
   //
   result = (unsigned int*)(result[0]);
   return result;
}

//
// Each task can register a fatal error handler
//
struct HandlerEntry
{
   FatalErrorHandler* _handler;
   void*              _arg;
};

static map<int, HandlerEntry*> registeredHandler;

void _FATAL_ERROR (const char* file, int line, const char* eString)  // parasoft-suppress CODSTA-93 "Intentional use of reserved name"
{
   typedef void (* DumpDataFunction)(void);

   //
   // If a handler has been registered, call it
   //
   bool hasHandler = false;
   if (registeredHandler.find(taskIdSelf()) != registeredHandler.end())
   {
      HandlerEntry* entry = registeredHandler[taskIdSelf()];
      (*entry->_handler)(entry->_arg);

      hasHandler = true;
   }

   //
   // Log fatal error
   //
   if (datalogInitOK)
   {
      log_level_critical(file, line) << eString << endmsg;
   }
   else
   {
      printf("%s(%d): <FATAL ERROR> %s\n", file, line, eString);
   }

   //
   // Notify any interested tasks that a fatal error has been detected
   //
   trimaStartShutdown(TTE_Fatal);

   //
   // Record system state information in log
   //
   DBG_DumpData();

   // This dump can take too much time and prevent the system from saving off
   // other more important information.  Only allow it if the machine has the
   // improved watchdog circuitry
   #if CPU!=SIMNT // Disable this in the simulator, since we don't know about the FPGA.
   if (hw_mxFpgaInstalled())
   {
      DumpDataFunction function = NULL;
      STATUS           status   = OK;
      SYM_TYPE         symType;

      status = symFindByName(sysSymTbl, "msgDump", (char**)&function, &symType);  // parasoft-suppress PB-27 "Ignore for OS functions"

      if (status == OK)
      {
         function();
      }
   }
   #endif // #if CPU!=SIMNT

   //
   // If a fatal error handler was registered, simply return.  Otherwise suspend
   // the task.
   //
   if (!hasHandler)
   {
      taskSuspend(0);
   }
}

void _CHAIN_FATAL_ERROR_HANDLER (FatalErrorHandler* func, void* arg)  // parasoft-suppress CODSTA-93 "Intentional use of reserved name"
{
   HandlerEntry* entry = new HandlerEntry;

   entry->_handler                 = func;
   entry->_arg                     = arg;
   registeredHandler[taskIdSelf()] = entry;
}

void sysinit_handleOSComm (OSCommData data)
{
   switch (data)
   {
      case OSComm_NextProcedure :
         if (datalogInitOK)
            DataLog(log_level_critical) << "shutdown for next procedure" << endmsg;

         trimaStartShutdown(TTE_NextProcedure);
         break;

      case OSComm_Fatal :
         if (datalogInitOK)
            DataLog(log_level_critical) << "shutdown for fatal error" << endmsg;

         trimaStartShutdown(TTE_Fatal);
         DBG_DumpData();
         break;

      case OSComm_SwitchToService :
         if (datalogInitOK)
            DataLog(log_level_critical) << "service mode acknowledgement received" << endmsg;

         serviceModeAcknowledge = true;
         break;

      default :
         if (datalogInitOK)
            DataLog(log_level_critical) << "unknown OS comm code " << hex << (unsigned int)data << dec << endmsg;

         break;
   }
}

#if (CPU!=SIMNT)
void sysinit_logTrapWrite (void)
{
   char buffer[PIPE_LOGTRAP_BUFFER + 1] = {0};
   int  idx = 0;

   while (1)
   {
      bool readDone = false;
      logTrapWaitData();

      //
      // May have had multiple writes before this task becomes active, so continue
      // reading until no more data is available.
      //
      while (!readDone)
      {
         int readSize = logTrapRead(&buffer[idx], PIPE_LOGTRAP_BUFFER - idx);
         idx += readSize;

         if (!readSize)
         {
            readDone = true;
         }

         //
         // Break data in buffer at newlines and log as separate messages
         //
         int endOfString;
         while ((endOfString = strcspn(buffer, "\n\r")) < idx)
         {
            if (endOfString > 0)
            {
               buffer[endOfString] = 0;
               DataLog(log_level_vxworks_info) << buffer << endmsg;
            }

            endOfString += 1;
            while (endOfString < idx &&
                   ( buffer[endOfString] == '\n' || buffer[endOfString] == '\r' ))
            {
               endOfString += 1;
            }

            if (endOfString < idx)
            {
               idx -= endOfString;
               memmove(&buffer[0], &buffer[endOfString], idx);
            }
            else
            {
               buffer[0] = 0;
               idx       = 0;
            }
         }

         //
         // Handle case where the buffer is full but does not contain any newline characters
         //
         if (idx >= PIPE_LOGTRAP_BUFFER)
         {
            DataLog(log_level_vxworks_info) << buffer << endmsg;
            idx = 0;
         }
      }
   }
}
#endif /* #if (CPU!=SIMNT) */

void sysinit_datalog (void)
{
   /*
    * Initialize the data log
    */
   if (trima_datalogInit() != DataLog_OK)
   {
      safetyBootFailed = TRUE;
      printf("error initializing data log\n");
      printf("aborting system startup\n");
      longjmp(sysinit_abort, 1);
   }

   log_level_startup_info.consoleOutput(DataLog_ConsoleEnabled);
   log_level_startup_error.consoleOutput(DataLog_ConsoleEnabled);

   DBG_EnableTaskSwitchLogging(750);
   DBG_EnableMessageLogging(150);

   DataLog(log_level_startup_detail) << "data log initialization completed" << endmsg;
   datalogInitOK = true;
}

void sysinit_waitServiceModeAcknowledge (void)
{
   int delay = 0;

   putenv("SERVICE_MODE=1"); // parasoft-suppress PB-27 "Ignore for OS functions"
   sysinit_boot_mode = ServiceBootMode;

   // Both sides need to wait for the other to send the message stating that they are
   // in Service mode
   while (delay < 90 * sysClkRateGet() &&
          !serviceModeAcknowledge)
   {
      taskDelay(1);
      delay += 1;
   }

   if (!serviceModeAcknowledge)
   {
      safetyBootFailed = TRUE;
      DataLog(log_level_startup_error) << "failed to receive service mode acknowledge" << endmsg;
      longjmp(sysinit_abort, 1);
   }
   else
   {
      if (sysGetLocalNodeID() == ControlNode)
      {
         // Now that Safety is up, send Control a message that Safety is in Service mode
         osCommSend(OSComm_SwitchToService);
      }

      printf("\nservice mode acknowledged - release the buttons to continue\n");

      while (bootStopButtonPressed() || bootPauseButtonPressed())
      {
         taskDelay(1);
      }
   }
}

#if (CPU!=SIMNT)
static bool handleLoadModule (const char* fileName, unsigned int properties)
{
   STATUS status = OK;

   if (fileName)
   {
      LoadModuleInfo info;

      status = loadModuleFromFile(fileName, &info);

      if (status != OK)
      {
         if (!(properties & TTP_NotRequired))
         {
            DataLog(log_level_startup_error) << "failed to open module " << fileName << ": " << errnoMsg << endmsg;
         }
      }
      else
      {
         DataLog(log_level_startup_info) << "module " << fileName << " loaded" << endmsg;
         datalog_Print(log_handle_startup_detail, __FILE__, __LINE__, "%s T=%lx:%lx D=%lx:%lx B=%lx:%lx",
                       fileName, info.textAddr, info.textSize, info.dataAddr, info.dataSize, info.bssAddr, info.bssSize);
      }
   }

   return(status == OK);
}
#endif /* if (CPU!=SIMNT) */

#if (CPU!=SIMNT)
typedef void EntryPoint (const char* commandLine);
static EntryPoint* findEntryPoint (const char* commandLine, unsigned int properties)
{
   SYM_TYPE    symType;
   EntryPoint* result = NULL;

   if (commandLine)
   {
      size_t len = strcspn(commandLine, " \t");
      if (len > 0)
      {
         //
         // Entry point names may simply be specified with the function name
         // if they have been given extern "C" linkage.  Otherwise, the mangled
         // function name must be used.  Note that this function handles inserting
         // a leading underscore.
         //
         char* buff = new char[len + 1];

         strncpy(buff, commandLine, len);
         buff[len] = '\0';

         if (symFindByName(sysSymTbl, buff, (char**)&result, &symType) != OK)
         {
            if (!(properties & TTP_NotRequired))
            {
               DataLog(log_level_startup_error) << "failed to find task entry point "
                                                << buff << ": " << errnoMsg << endmsg;
            }

            result = NULL;
         }

         delete[] buff;
      }
   }

   return result;
}
#endif /* if (CPU!=SIMNT) */

#if (CPU!=SIMNT)
const char* envSub (const char** str)
{
   const char* parseChar    = *str;
   bool        acceptDollar = true;
   bool        subPresent   = false;

   int         tmpStringLen = strlen(*str) * 2;
   int         tmpStringIdx = 0;
   char*       tmpString    = (char*)malloc(tmpStringLen);
   int         envIdx       = -1;

   bool        done         = false;
   while (!done && tmpString)
   {
      if (acceptDollar && *parseChar == '$')
      {
         envIdx       = tmpStringIdx;
         acceptDollar = false;
         ++parseChar;
      }
      else if (envIdx >= 0 &&
               ( isspace(*parseChar) || *parseChar == '\0' ))
      {
         // First try to get variable from hardware options (hw.dat), then
         // from environment.
         //
         tmpString[tmpStringIdx] = '\0';
         const char* envValue = trimaSysGetHardwareSetting(&tmpString[envIdx]);
         if (!envValue)
         {
            envValue = getenv(&tmpString[envIdx]);
         }

         if (envValue)
         {
            if (strlen(envValue) >= tmpStringLen - envIdx)
            {
               tmpStringLen += strlen(envValue);
               tmpString     = (char*)realloc(tmpString, tmpStringLen);
               if ( !tmpString ) continue;
            }

            strcpy(&tmpString[envIdx], envValue);
            tmpStringIdx = envIdx + strlen(envValue);
         }
         else
         {
            tmpStringIdx = envIdx;
         }

         subPresent = true;
         envIdx     = -1;
      }
      else
      {
         tmpString[tmpStringIdx++] = *parseChar;
         if (*parseChar == '\0')
         {
            done = true;
         }
         else
         {
            acceptDollar = isspace(*parseChar);
            ++parseChar;

            if (tmpStringIdx >= tmpStringLen)
            {
               tmpStringLen += 20;
               tmpString     = (char*)realloc(tmpString, tmpStringLen);
            }
         }
      }
   }

   if (subPresent)
   {
      *str = tmpString;
   }
   else if (tmpString)
   {
      free(tmpString);
   }

   return *str;
}
#endif /* if (CPU!=SIMNT) */



#if (CPU!=SIMNT)

static void sysinit_delay (unsigned int properties)
{
   if (properties & TTP_Delay_1)
      taskDelay(sysClkRateGet() * 1);
   else if (properties & TTP_Delay_5)
      taskDelay(sysClkRateGet() * 5);
   else if (properties & TTP_Delay_10)
      taskDelay(sysClkRateGet() * 10);
   else if (properties & TTP_Delay_20)
      taskDelay(sysClkRateGet() * 20);
}



static void sysinit_startup (void)
{
   const TrimaTaskID* taskList = NULL;
   size_t             taskListSize;
   LoadModuleInfo     info;
   SYM_TYPE           symType;
   int                (* routerQueueSizeFunc)(void) = NULL;


   DataLog(log_level_startup_detail) << "CHECKPOINT: Pre Trima task startup (sysinit_startup)" << endmsg;

   // Create the binary semaphore used for task synchronization.
   // It starts out empty.
   taskSynchronizationSem = semBCreate(SEM_Q_FIFO, SEM_EMPTY);

   trimaGetTaskStartupList(&taskList, &taskListSize);

   for (int task = 0; task < taskListSize; task++)
   {
      TrimaTaskInfo info;
      trimaGetTaskInfo(taskList[task], &info);

      //
      // Perform any necessary environment variable substitutions (these will
      // generally come from settings in hw.dat).
      //
      if (info.fileName)
      {
         const char* sub = envSub(&info.fileName);

         if (sub != info.fileName &&
             strlen(sub) == 0)
         {
            safetyBootFailed = TRUE;
            DataLog(log_level_startup_error) << "missing environment variable for " << info.taskName << " task filename \"" << info.fileName << "\"" << endmsg;
            longjmp(sysinit_abort, 1);
         }

         info.fileName = sub;
      }

      if (info.commandLine)
      {
         const char* sub = envSub(&info.commandLine);
         if (sub != info.commandLine &&
             strlen(sub) == 0)
         {
            safetyBootFailed = TRUE;
            DataLog(log_level_startup_error) << "missing environment variable for " << info.taskName << " task command line \"" << info.commandLine << "\"" << endmsg;
            longjmp(sysinit_abort, 1);
         }

         info.commandLine = sub;
      }

      //
      // Check if task is enabled to startup in the current boot mode
      //
      if ((info.properties & sysinit_boot_mode) != 0)
      {
         if (!handleLoadModule(info.fileName, info.properties))
         {
            if (!(info.properties & TTP_NotRequired))
            {
               safetyBootFailed = TRUE;
               DataLog(log_level_startup_info) << "Cannot load module for " << info.fileName << endmsg;

               longjmp(sysinit_abort, 1);
            }
         }
         else if (info.commandLine)
         {
            EntryPoint* entryPoint = findEntryPoint(info.commandLine, info.properties);

            if (!entryPoint)
            {
               if (!(info.properties & TTP_NotRequired))
               {
                  safetyBootFailed = TRUE;
                  DataLog(log_level_startup_info) << "Cannot find entry point for " << info.taskName << endmsg;

                  longjmp(sysinit_abort, 1);
               }
            }
            else if (info.properties & TTP_RunInSysinit)
            {
               DataLog(log_level_startup_info) << "executing function " << info.taskName << endmsg;
               (* entryPoint)(info.commandLine);

               // If the delay attribute is set then delay after the function was executed the specified time
               sysinit_delay(info.properties);
            }
            else
            {
               if (routerQueueSizeFunc == NULL)
               {
                  // If the router has been loaded, attach to the function that tells us how big the
                  // router queue size is. Have to do the late binding look up because the router
                  // object file is loaded in the task startup list
                  symFindByName(sysSymTbl, "trimaNodeRouterQueueSize", (char**)&routerQueueSizeFunc, &symType);
               }

               //
               //
               //   After each task is started, give it a chance to initialize before starting the next task.  This adds robustness
               //   to the startup process.
               //
               if (info.properties & TTP_SafetyTask)
                  taskDelay(sysClkRateGet() * 5);

               // Report amount of memory the task needs
               const unsigned long paddedStackSize = ((info.stackSize + 0x0fff) & 0xfffff000) + 0x1000;
               const int           largestBlock    = memFindMax();

               info.tid = taskStart(info.taskName, info.priority, info.stackSize, (FUNCPTR)entryPoint, (int)info.commandLine, 0, 0, 0, 0, 0, 0, 0, 0, 0);

               DataLog(log_level_startup_info) << "task name " << info.taskName << " stack needed " << paddedStackSize
                                               << ".  Largest available memory block " << largestBlock
                                               << ".  After task started largest available memory block " << memFindMax() << endmsg;

               if (info.tid == ERROR)
               {
                  safetyBootFailed = TRUE;
                  DataLog(log_level_startup_error) << "failed to start " << info.taskName << " task" << endmsg;
                  longjmp(sysinit_abort, 1);
               }

               trimaSetTaskID(taskList[task], info.tid);
               DataLog(log_level_startup_info) << "started task " << info.taskName << endmsg;

               // If the delay attribute is set then delay after the task was started the specified time
               sysinit_delay(info.properties);

               // If the SyncWithSem option is set, then block on a semaphore.
               // After spawning the task, the loop does a blocking take.
               // The spawned task does the give after it completes its initialization.
               // NOTE:  Make sure to put the give in the tasks you add this option to.
               if (info.properties & TTP_SyncWithSem)
               {
                  // Take the sem with a 20 second timeout
                  const STATUS result = WAIT_ON_TASK(20);

                  if (result != OK)
                  {
                     if (errno == S_objLib_OBJ_TIMEOUT)
                        DataLog(log_level_startup_error) << "Task " << info.taskName << " sync semaphore timed out!" << endmsg;
                     else
                        DataLog(log_level_startup_error) << "Task " << info.taskName << " semTake failed with status " << errnoGet() << endmsg;
                  }
                  else
                  {
                     DataLog(log_level_startup_info) << "Task " << info.taskName << " synchronization complete";

                     if (routerQueueSizeFunc != NULL)
                        DataLog(log_level_startup_info) << ".  Router queue size is " << routerQueueSizeFunc() << endmsg;
                     else
                        DataLog(log_level_startup_info) << "." << endmsg;
                  }
               }

               int delayCount = 0;

               //
               // Don't start the next task until the Router's queue is below 10%
               //
               while ( (routerQueueSizeFunc != NULL) && (routerQueueSizeFunc() > 10) )
               {
                  DataLog(log_level_startup_info) << "Startup delayed for task " << info.taskName
                                                  << ".  Router queue size is " << routerQueueSizeFunc() << endmsg;

                  sysinit_delay(TTP_Delay_1);

                  //
                  // Bail out after 10 seconds of waiting.
                  //
                  if (delayCount++ > 10)
                  {
                     break;
                  }
               }
            }
         }
      }
   }

   DataLog(log_level_startup_detail) << "CHECKPOINT: Exiting sysinit_startup" << endmsg;
}
#endif /* if (CPU!=SIMNT) */


extern FUNCPTR memPartBlockErrorRtn;
extern FUNCPTR memPartAllocErrorRtn;

static void sysinit_memBlockError (void)
{
   _FATAL_ERROR(__FILE__, __LINE__, "memory block error");
}

static void sysinit_memAllocError (unsigned long partID, unsigned long size)
{
   const int largestBlock = memFindMax();

   DataLog(log_level_critical) << "attempted allocation size = " << size << ".  Largest is " << largestBlock << endmsg;

   _FATAL_ERROR(__FILE__, __LINE__, "memory allocation failed");
}

int unzipFile (const char* from, const char* to)
{
   gzFile fromFD = gzopen(from, "r");
   int    toFD   = open(to, O_CREAT | O_RDWR, 0644);

   if ( fromFD >= 0 && toFD >= 0 )
   {
      int  bytesRead;
      int  bytesWritten  = 0;
      int  kBytesWritten = 0;
      char buffer[10 * 1024];

      while ( (bytesRead = gzread(fromFD, buffer, 10 * 1024)) > 0 )
      {
         bytesWritten += write(toFD, buffer, bytesRead);
         kBytesWritten = bytesWritten / 1024;
      }

      printf("\tUncompression complete\n");
      gzclose(fromFD);
      close(toFD);

      return 1;
   }
   else
   {
      if ( fromFD < 0 )
      {
         perror(from);
      }
      else
      {
         gzclose(fromFD);
      }

      if ( toFD < 0 )
      {
         perror(to);
      }
      else
      {
         close(toFD);
      }
   }

   return 0;
}

void sysinit_log_configuration (void)
{
   extern char** ppGlobalEnviron;
   int           idx = 0;

   while (ppGlobalEnviron && ppGlobalEnviron[idx])
   {
      DataLog(log_level_startup_detail) << "env: " << ppGlobalEnviron[idx] << endmsg;
      idx += 1;
   }

   if (sysGetLocalNodeID() == ControlNode)
   {
      const char* fileNames[] = { PNAME_HWDAT, PNAME_CALDAT, PNAME_RBCDAT, PNAME_METERDAT, TRAP_OVERRIDE_FILE, PNAME_COBECONFIGDAT, NULL};
      idx = 0;

      while (fileNames[idx])
      {
         FILE* fp = fopen(fileNames[idx], "r");
         if (fp)
         {
            DataLog(log_level_startup_detail) << "config file: " << fileNames[idx] << endmsg;

            enum
            {
               BuffSize = 256
            };
            char buff[BuffSize];
            while (fgets(buff, BuffSize, fp))
            {
               char* token = buff + strspn(buff, " \t\r\n");
               token[strcspn(token, "\r\n")] = '\0';
               if (token[0] != '\0' && token[0] != '#')
               {
                  DataLog(log_level_startup_detail) << "config data: " << token << endmsg;
               }
            }

            fclose(fp);
         }

         idx += 1;
      }
   }
}

void sysinit_main (void)
{
   //
   // Setup vxWorks to check all memory allocation/deallocation
   //
   memOptionsSet(MEM_ALLOC_ERROR_LOG_FLAG | MEM_ALLOC_ERROR_SUSPEND_FLAG | MEM_BLOCK_ERROR_LOG_FLAG | MEM_BLOCK_ERROR_SUSPEND_FLAG | MEM_BLOCK_CHECK);
   memPartBlockErrorRtn = (FUNCPTR)sysinit_memBlockError;
   memPartAllocErrorRtn = (FUNCPTR)sysinit_memAllocError;

   //
   // Provide handler for OS communication port
   //
   osCommSetCallback(&sysinit_handleOSComm);

   if (!setjmp(sysinit_abort))
   {
      if (bootAltModeEnabled())
      {
         sysinit_boot_mode = AltBootMode;
      }

      sysinit_setup_environment_variables();
      sysinit_datalog();
      sysinit_log_configuration();

#if (CPU!=SIMNT)
      sysinit_prestart_node();
      taskDelay(1);

      // Begin starting up tasks
      sysinit_startup();

      // Set Control and Safety ARP entries permanent.
      usrSeedArpTable();

#else /* if (CPU!=SIMNT) */
      usrShellInit();
#endif /* if (CPU!=SIMNT) */
   }
   else
   {
      if (!datalogInitOK)
         printf("System initialization is aborted...\n\n");
      else
         DataLog(log_level_critical) << "System initialization is aborted...\n\n" << endmsg;

      safetyBootFailed = TRUE;
      usrShellInit();
   }
}

//
// Override new/delete.  The Trima specific versions of new zero
// allocated memory.
//
extern "C"
void* __builtin_new (size_t size) // parasoft-suppress CODSTA-93 "Intentional use of reserved name"
{
   return calloc(size, 1);
}

extern "C"
void __builtin_delete (void* ptr) // parasoft-suppress CODSTA-93 "Intentional use of reserved name"
{
   free(ptr);
}

extern "C"
void* __builtin_vec_new (size_t size) // parasoft-suppress CODSTA-93 "Intentional use of reserved name"
{
   return calloc(size, 1);
}

extern "C"
void __builtin_vec_delete (void* ptr) // parasoft-suppress CODSTA-93 "Intentional use of reserved name"
{
   free(ptr);
}

void usrSeedArpTable (void)
{
   struct arpreq       arpEntry;
   char                arpString[18];
   struct in_addr      inetAddress;
   int                 status;

   struct sockaddr_in* sin       = (struct sockaddr_in*)&arpEntry.arp_pa; /* point this to the network address in the ARP entry */
   const char*         arpPtr    = (char*)&arpEntry.arp_ha.sa_data;       /* points the the MAC addr */

   const SysNodeID     otherNode = (sysGetLocalNodeID() == ControlNode) ? SafetyNode : ControlNode;
   char                otherAddr[INET_ADDR_LEN];
   SysNodeData         nodeData;

   // Get the Sys Node Data.
   sysGetNodeData(sysGetLocalNodeID(), &nodeData);

   // Set the IP address of the other node.
   inetAddress.s_addr = nodeData.ipAddress[otherNode];
   inet_ntoa_b(inetAddress, otherAddr);

   // Initialize the ARP entry
   memset ((caddr_t)&arpEntry, 0, sizeof (struct arpreq));
   arpEntry.arp_pa.sa_family = AF_INET;
   ((struct sockaddr_in*)&(arpEntry.arp_pa))->sin_addr.s_addr = inetAddress.s_addr;
   arpEntry.arp_ha.sa_family = AF_UNSPEC;

   /* Initalize arp entry with other addr */
   const int s = socket(AF_INET, SOCK_RAW, 0);
   sin->sin_family = AF_INET;
   memcpy(&sin->sin_addr, (char*)&inetAddress, sizeof(struct in_addr));
   in_socktrim(sin);  /* set the length right */

   // Set Control ARP entry to be permanent.
   if (ioctl(s, SIOCGARP, (int)&arpEntry) != ERROR)
   {
      sprintf(arpString, "%02x:%02x:%02x:%02x:%02x:%02x",
              arpPtr[0], arpPtr[1], arpPtr[2], arpPtr[3], arpPtr[4], arpPtr[5]);

      status = arpAdd(otherAddr, arpString, ATF_PERM);

      // printf("ARP table entry for %s/%s set as permanent.  Status = %d\n", arpString, otherAddr, status);
   }
   else
   {
      perror("ARP ioctl lookup failed");
      printf("Cannot find ARP entry for node %s(%X)\n", otherAddr, inetAddress.s_addr);
      arpShow();
   }

   close (s);
}

/* FORMAT HASH de8d5117c4e08bd3abaca67d5409d40f */

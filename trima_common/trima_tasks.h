/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * Trima task names
 *
 * $Header$
 *
 */

#ifndef _TRIMA_TASKS_INCLUDE
#define _TRIMA_TASKS_INCLUDE

#include <vxWorks.h>
#include <sigLib.h>
#include <signal.h>

#include "filenames.h"

/*
 * Make sure this file uses only constructs valid in both C and C++.
 */
#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */


/* define task synchronization semaphore and methods */
extern SEM_ID taskSynchronizationSem;
#define WAIT_ON_TASK(delay) semTake(taskSynchronizationSem, sysClkRateGet() * delay);
#define TASK_DONE    semGive(taskSynchronizationSem);


/*
 * List of tasks for the Trima system
 */
typedef enum
{
   TTID_FirstTask    = 0,     /* must be zero */

   TTID_AlarmHandler = TTID_FirstTask,
   TTID_AlarmHandlerLoad,
   TTID_Barcode,
   TTID_CDSLoad,
   TTID_DatFileReaderLoad,
   TTID_DatFileReader,
   TTID_Connector,
   TTID_ControlCommon,
   TTID_CorruptMain,
   TTID_CtlDrv,
   TTID_CtlDrvLoad,
   TTID_CtlFastMsg,
   TTID_CtlMsg,
   TTID_DataLogNetwork,
   TTID_DataLogNetworkClient,
   TTID_DataLogOutput,
   TTID_DataLogPeriodic,
   TTID_FontdefLoad,
   TTID_Gateway,
   TTID_GoodDog,
   TTID_GuiGraphicsLoad,
   TTID_GuiLoad,
   TTID_Gui,
   TTID_HogControl,
   TTID_HogControlLoad,
   TTID_HogSafety,
   TTID_HogSafetyLoad,
   TTID_LanguageLoad,
   TTID_LogTrapWrite,
   TTID_MakeBigFileLoad,
   TTID_MessageSystemTimer,
   TTID_Meter,
   TTID_Predict,
   TTID_PredictLoad,
   TTID_Proc,
   TTID_ProcLoad,
   TTID_RBCCmd,
   TTID_RBCData,
   TTID_RetOccMon,
   TTID_RetOccMonLoad,
   TTID_Router,
   TTID_SafeDrv,
   TTID_SafeDrvLoad,
   TTID_SafeExec,
   TTID_SafeExecLoad,
   TTID_SafeMsg,
   TTID_SafeMsgLoad,
   TTID_SafeFastMsg,
   TTID_SafeFastMsgLoad,
   TTID_SafetyCommon,
   TTID_ServiceGraphicsLoad,
   TTID_ServiceLoad,
   TTID_Service,
   TTID_Shell,
   TTID_SoftCRC_SafetyFiles,
   TTID_SoftCRC_TrimaFiles,
   TTID_SoftCRC_MachineFiles,
   TTID_SoftCRC_GuiFiles,
   TTID_SoftCRC_ProcFiles,
   TTID_Sounds,
   TTID_SpoofMainLoad,
   TTID_SpoofServerControl,
   TTID_SpoofServerControlLoad,
   TTID_SpoofServerSafety,
   TTID_SpoofServerSafetyLoad,
   TTID_SysinitCleanup,
   TTID_SysinitSyncRouter,
   TTID_SysIDLog,
   TTID_Ted,
   TTID_TFTP,
   TTID_TrapClient,
   TTID_TrimaBroadcast,
   TTID_UGLLoad,
   TTID_VipCmd,
   TTID_VipSts,
   TTID_LogNetworkStats,
   TTID_StackUsageMonitor,
   TTID_WinApp,

   TTID_EndOfTasks            /* must be last entry */
} TrimaTaskID;

/*
 * Task properties are stored as a bitwise or of one or more of the following
 * values.
 */
typedef enum
{
   /*
    * Node(s) on which task is run.  Sysinit and shutdown use this information
    * to determine if a task should be started/signaled on a particular node.
    */
   TTP_ControlTask = 0x01,
   TTP_SafetyTask  = 0x02,
   TTP_AllNodes    = TTP_ControlTask | TTP_SafetyTask,

   /*
    * Flags to determine in which modes tasks run
    */
   TTP_NormalBoot   = 0x08,
   TTP_ServiceBoot  = 0x10,
   TTP_AltBoot      = 0x20,
   TTP_AllBootModes = TTP_NormalBoot | TTP_ServiceBoot | TTP_AltBoot,

   /*
    * Flag to determine if task should be spawned or if it runs within the
    * sysinit task context.
    */
   TTP_RunInSysinit = 0x100,

   /*
    *  Flag set indicates sysinit should not abort if load or spawn failure
    */
   TTP_NotRequired = 0x200,

   /*
    *  Flag set indicates sysinit should wait for a synchronization semaphore
    */
   TTP_SyncWithSem = 0x400,

   /*
    * Task delay options for after a task has started to give them more time to get loaded
    */
   TTP_Delay_1  = 0x1000,
   TTP_Delay_5  = 0x2000,
   TTP_Delay_10 = 0x4000,
   TTP_Delay_20 = 0x8000,

} TrimaTaskProperties;

/*
 * The following lists the special mask values allowed for the endSignal, pfwSignal,
 * and fatalSignal members of TrimaTaskInfo.  These members can also be set
 * to one of the standard POSIX signal values.  Trima specific signal values
 * listed here are in the high order bits so as not to clash with POSIX signals.
 */
typedef enum
{
   TS_TrimaSignalMask = 0xff000000, /* Bits used to denote trima specific signal values */
   TS_Suspend         = 0x01000000, /* suspend task */
   TS_ChangePriority  = 0x02000000, /* change priority of task */
} TrimaSignals;

/*
 * Struct for storing information about each task
 */
typedef struct
{
   TrimaTaskID  id;           /* ID from TrimaTaskID list */
   const char*  fileName;     /* file name to be loaded for task - NULL if no file load needed */
   const char*  taskName;     /* ASCII name to be used for task */
   const char*  commandLine;  /* command line for task */
   unsigned int stackSize;    /* stack size (bytes) for task - can be 0 if not spawned */
   unsigned int priority;     /* priority for task - can be 0 if not spawned */
   unsigned int properties;   /* bitwise-or of values from TrimaTaskProperties */
   int          tid;          /* task ID from spawn command - set to 0 if not spawned */
   void*        routerQ;      /* Router Queue handle - set to 0 if not set */
} TrimaTaskInfo;

/*
 * Get various information about tasks
 */
int  trimaGetTaskInfo (TrimaTaskID id, TrimaTaskInfo* result);
int  trimaGetTaskID (TrimaTaskID id);
void trimaGetTaskStartupList (const TrimaTaskID** taskList, size_t* taskCount);
void trimaSetTaskID (TrimaTaskID id, int tid);
void trimaSetRouterQ (int tid, void* routerQ);

/* function stored in router_synch because of loading dependency issues */
void trimaReportTaskQueueSizes (void);
int  trimaNodeRouterQueueSize (void);             /* only available on Control */



/*
 * List of possible task events
 */
typedef enum
{
   TTE_NextProcedure,      /* normal procedure end */
   TTE_PowerFail,          /* power-fail detected, completing shutdown */
   TTE_Fatal               /* fatal error detected */
} TrimaShutdownEvent;

struct ShutdownData
{
   unsigned short msec;
   int            signal;
   TrimaTaskID    taskID;
};

#ifdef TRIMA_SYSINIT_BUILD

/*
 * Shutdown sequences for normal procedure end
 */
ShutdownData trimaControlNextProcedureShutdown[] =
{
/*    time   action                             task */
   {     0,  SIGQUIT,              TTID_StackUsageMonitor },   // Can hang system
   {     0,  SIGQUIT,              TTID_Gui },
   {     0,  SIGQUIT,              TTID_Barcode },
   {     0,  TS_Suspend,           TTID_DataLogNetwork },
   {     0,  SIGQUIT,              TTID_TrapClient },
   {     0,  SIGQUIT,              TTID_TrimaBroadcast },
   {     0,  SIGQUIT,              TTID_VipCmd },
   {     0,  SIGQUIT,              TTID_VipSts },

   {  1000,  SIGINT,               TTID_DataLogOutput },
   {  5000,  SIGQUIT,              TTID_DataLogOutput },
   {  6000,  TS_Suspend,           TTID_CtlDrv }
};

ShutdownData trimaSafetyNextProcedureShutdown[] =
{
/*    time   action                             task */
   {  1000,  SIGINT,               TTID_DataLogOutput },
   {  5000,  SIGQUIT,              TTID_DataLogOutput },
   {  6000,  TS_Suspend,           TTID_SafeDrv }
};

/*
 * Shutdown sequences for fatal error
 */
ShutdownData trimaControlFatalErrorShutdown[] =
{
/*    time   action                                       task */
   {     0,  SIGQUIT,                    TTID_CtlDrv },
   {     0,  TS_ChangePriority | 200, TTID_AlarmHandler },
   {     0,  SIGQUIT,                 TTID_StackUsageMonitor },   // Can hang system
   {     0,  SIGQUIT,                    TTID_Barcode },
   {     0,  TS_ChangePriority | 200, TTID_CtlFastMsg },
   {     0,  TS_ChangePriority | 200, TTID_CtlMsg },
   {     0,  TS_ChangePriority | 200, TTID_Gateway },
   {     0,  TS_ChangePriority | 200, TTID_Gui },
   {     0,  TS_ChangePriority | 200, TTID_MessageSystemTimer },
   {     0,  TS_ChangePriority | 200, TTID_Predict },
   {     0,  TS_ChangePriority | 200, TTID_Proc },
   {     0,  TS_ChangePriority | 200, TTID_RBCCmd },
   {     0,  TS_ChangePriority | 200, TTID_RBCData },
   {     0,  TS_ChangePriority | 200, TTID_RetOccMon },
   {     0,  TS_ChangePriority | 200, TTID_Router },
   {     0,  SIGQUIT,                 TTID_TrapClient },
   {     0,  TS_ChangePriority | 200, TTID_TrimaBroadcast },
   {     0,  SIGQUIT,                 TTID_VipCmd },
   {     0,  SIGQUIT,                 TTID_VipSts },

#ifdef DEBUG_EXTEND_SHUTDOWN
   { 10000,  TS_ChangePriority | 5,      TTID_DataLogOutput },

   { 60000,  SIGINT,                         TTID_DataLogOutput },
   { 65000,  SIGQUIT,                    TTID_DataLogOutput }
#else /* ifdef DEBUG_EXTEND_SHUTDOWN */
   {  1000,  TS_ChangePriority | 5,      TTID_DataLogOutput },

   {  3000,  SIGINT,                         TTID_DataLogOutput },
   {  4000,  SIGQUIT,                    TTID_DataLogOutput }
#endif /* ifdef DEBUG_EXTEND_SHUTDOWN */
};

ShutdownData trimaSafetyFatalErrorShutdown[] =
{
/*    time   action                                       task */
   {     0,  SIGQUIT,                    TTID_SafeDrv },
   {     0,  TS_ChangePriority | 200, TTID_MessageSystemTimer },
   {     0,  TS_ChangePriority | 200, TTID_Router },
   {     0,  TS_ChangePriority | 200, TTID_SafeExec },
   {     0,  TS_ChangePriority | 200, TTID_SafeFastMsg },

#ifdef DEBUG_EXTEND_SHUTDOWN
   { 10000,  TS_ChangePriority | 5,      TTID_DataLogOutput },

   { 50000,  SIGINT,                         TTID_DataLogOutput },
   { 55000,  SIGQUIT,                    TTID_DataLogOutput }
#else /* ifdef DEBUG_EXTEND_SHUTDOWN */
   {   500,  TS_ChangePriority | 5,      TTID_DataLogOutput },

   {  1500,  SIGINT,                     TTID_DataLogOutput },
   {  2500,  SIGQUIT,                    TTID_DataLogOutput }
#endif /* ifdef DEBUG_EXTEND_SHUTDOWN */
};

/*
 * Shutdown sequences for power fail
 */
ShutdownData trimaControlPowerFailShutdown[] =
{
/*    time   action                                   task */
   {     0,  TS_ChangePriority | 15,   TTID_Proc },

   {     0,  SIGHUP,                           TTID_Proc },

   {     0,  SIGQUIT,                  TTID_StackUsageMonitor },   // Can hang system
   {     0,  TS_ChangePriority | 200,  TTID_AlarmHandler },
   {     0,  TS_ChangePriority | 200,  TTID_Barcode },
   {     0,  TS_ChangePriority | 200,  TTID_CtlFastMsg },
   {     0,  TS_ChangePriority | 200,  TTID_CtlMsg },
   {     0,  TS_ChangePriority | 200,  TTID_Gateway },
   {     0,  TS_ChangePriority | 200,  TTID_Gui },
   {     0,  TS_ChangePriority | 200,  TTID_MessageSystemTimer },
   {     0,  TS_ChangePriority | 200,  TTID_RBCData },
   {     0,  TS_ChangePriority | 200,  TTID_Router },
   {     0,  TS_ChangePriority | 200,  TTID_TrapClient },
   {     0,  TS_ChangePriority | 200,  TTID_VipCmd },
   {     0,  TS_ChangePriority | 200,  TTID_VipSts },

   {   200,  SIGINT,                           TTID_DataLogOutput },
   {   350,  SIGQUIT,                      TTID_DataLogOutput },

   {   500,  SIGQUIT,                      TTID_CtlDrv }
};

ShutdownData trimaSafetyPowerFailShutdown[] =
{
/*    time   action                                   task */
   {     0,  TS_ChangePriority | 15,   TTID_SafeExec },

   {     0,  SIGHUP,                         TTID_SafeExec },

   {     0,  TS_ChangePriority | 200,  TTID_MessageSystemTimer },
   {     0,  TS_ChangePriority | 200,  TTID_Router },
   {     0,  TS_ChangePriority | 200,  TTID_SafeFastMsg },

   {   100,  SIGINT,                     TTID_DataLogOutput },
   {   300,  SIGQUIT,                    TTID_DataLogOutput },

   {   400,  SIGQUIT,                    TTID_SafeDrv }
};

#endif /* ifdef TRIMA_SYSINIT_BUILD */

void trimaStartShutdown (TrimaShutdownEvent event);

/*
 * TRIMA_SYSINIT_BUILD is defined by one source module in the Trima OS build,
 * and should not be defined in any other modules.  The header file is
 * organized in this way to allow all task related data to be held in a single
 * file.
 */
#ifdef TRIMA_SYSINIT_BUILD
TrimaTaskInfo trimaTaskInfo[TTID_EndOfTasks] =
{
   /*
    * These are sorted by task ID at system initialization, and can be listed in any order.
    * However, there must be one entry for each task ID defined in the TrimaTaskID enumeration
    * Proper number of entries is enforced at compile time.  Missing or duplicate entries
    * for a particular task ID is checked during system initialization.
    */
   { TTID_AlarmHandler,
     /* fileName */ NULL,
     /* taskName */ "alarm_handler",
     /* commandLine */ "AlarmHandler_main",
     /* stackSize */ 16000,
     /* priority */ 65,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_SyncWithSem },

   { TTID_AlarmHandlerLoad,
     /* fileName */ TRIMA_PATH "/proc/alarmhandler.out",
     /* taskName */ "alarm handler loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot },

   { TTID_Barcode,
     /* fileName */ TRIMA_PATH "/gui/barcode_reader.out",
     /* taskName */ "barcode_reader",
     /* commandLine */ "barcode_reader /tyCo/1",
     /* stackSize */ 5000,
     /* priority */ 90,
     /* properties */ TTP_ControlTask | TTP_NormalBoot },

   { TTID_DatFileReader,
     /* fileName */ NULL,
     /* taskName */ "Dat File Reader",
     /* commandLine */ "readDatFiles",
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_RunInSysinit },

   { TTID_DatFileReaderLoad,
     /* fileName */ TRIMA_PATH "/gui/datfile_reader.out",
     /* taskName */ "Dat File Reader Load",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_RunInSysinit },

   { TTID_CDSLoad,
     /* fileName */ TRIMA_PATH "/cds/cds.out",
     /* taskName */ "CDS Load",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_RunInSysinit },

   { TTID_GuiGraphicsLoad,
     /* fileName */ PNAME_GUI_GRAPHICS,
     /* taskName */ "GUI Graphics Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot },

   { TTID_CorruptMain,  /* corrupts various data files at boot */
     /* fileName */ TOOLS_PATH "/corrupt_main.out",
     /* taskName */ "corrupt_main",
     /* commandLine */ "corrupt_main",
     /* stackSize */ 10000,
     /* priority */ 20,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_NotRequired },

   { TTID_Connector,
     /* fileName */ NULL,           /* routine is included with common.out module */
     /* taskName */ "Connector",
     /* commandLine */ NULL,
     /* stackSize */ 4000,
     /* priority */ 90,
     /* properties */ TTP_AllNodes | TTP_AllBootModes },

   { TTID_ControlCommon,
     /* fileName */ TRIMA_PATH "/common/control_lib.out",
     /* taskName */ "Control lib loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_CtlDrv,
     /* fileName */ NULL,
     /* taskName */ "ctl_drv",
     /* commandLine */ "ctl_drv $pfw_ridethru",
     /* stackSize */ 10000,
     /* priority */ 1,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_CtlDrvLoad,
     /* fileName */ TRIMA_PATH "/low_level/ctl_drv.out",
     /* taskName */ "Control Driver Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_CtlFastMsg,
     /* fileName */ NULL,
     /* taskName */ "cfastmsg",
     /* commandLine */ "cfastmsg",
     /* stackSize */ 7000,
     /* priority */ 30,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_CtlMsg,
     /* fileName */ NULL,
     /* taskName */ "ctl_msg",
     /* commandLine */ "ctl_msg",
     /* stackSize */ 7000,
     /* priority */ 30,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_DataLogNetwork,
     /* fileName */ NULL,           /* routine is included with system initialization module */
     /* taskName */ "dlog_net",
     /* commandLine */ NULL,        /* task is started automatically by datalog initialization routine */
     /* stackSize */ 4000,
     /* priority */ 82,
     /* properties */ TTP_AllNodes | TTP_AllBootModes },

   { TTID_DataLogNetworkClient,
     /* fileName */ NULL,           /* routine is included with system initialization module */
     /* taskName */ "dlog_netc",
     /* commandLine */ NULL,        /* task is started automatically by datalog initialization routine */
     /* stackSize */ 3000,
     /* priority */ 81,
     /* properties */ TTP_AllNodes | TTP_AllBootModes },

   { TTID_DataLogOutput,
     /* fileName */ NULL,           /* routine is included with system initialization module */
     /* taskName */ "dlog_out",
     /* commandLine */ NULL,        /* task is started automatically by datalog initialization routine */
     /* stackSize */ 6000,
     /* priority */ 80,
     /* properties */ TTP_AllNodes | TTP_AllBootModes },

   { TTID_DataLogPeriodic,
     /* fileName */ NULL,           /* routine is included with system initialization module */
     /* taskName */ "dlog_per",
     /* commandLine */ NULL,        /* task is started through datalog API */
     /* stackSize */ 5000,
     /* priority */ 81,
     /* properties */ TTP_AllNodes | TTP_AllBootModes },

   { TTID_FontdefLoad,
     /* fileName */ TRIMA_PATH "/gui/fontdefs.o",
     /* taskName */ "Font Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_Gateway,
     /* fileName */ NULL,           /* routine is included with common.out module */
     /* taskName */ "Gateway",
     /* commandLine */ NULL,
     /* stackSize */ 5000,
     /* priority */ 40,
     /* properties */ TTP_AllNodes | TTP_AllBootModes },

   { TTID_GoodDog,
     /* fileName */ TRIMA_PATH "/service/gooddog.out",
     /* taskName */ "gooddog",
     /* commandLine */ "gooddog",
     /* stackSize */ 10000,
     /* priority */ 45,
     /* properties */ TTP_SafetyTask | TTP_ServiceBoot },

   { TTID_GuiLoad,
     /* fileName */ TRIMA_PATH "/gui/gui.out",
     /* taskName */ "GUI Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot },


   /*
   NOTE: This task now runs within sysinit, because it merely starts up the winApp UGL process,
   which actually does all the heavy lifting now.
   */
   { TTID_Gui,
     /* fileName */ NULL,
     /* taskName */ "gui_startup",
     /* commandLine */ "guimain",
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_RunInSysinit | TTP_Delay_5 },

   { TTID_HogControl,  /* hogs 10% cpu and 1MB of memory */
     /* fileName */ NULL,
     /* taskName */ "hog",
     /* commandLine */ "hog_main -p 2 -m 1000 -t 0 -step 1",
     /* stackSize */ 5000,
     /* priority */ 2,        /* priority overridden in by hog */
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_NotRequired },

   { TTID_HogControlLoad,
     /* fileName */ TOOLS_PATH "/hog_control.out",
     /* taskName */ "Hog Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_NotRequired },

   { TTID_HogSafety,  /* hogs 10% cpu and 100K of memory */
     /* fileName */ NULL,
     /* taskName */ "hog",
     /* commandLine */ "hog_main -p 2 -m 100 -t 0 -step 1",
     /* stackSize */ 5000,
     /* priority */ 2,        /* priority overridden in by hog */
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_NotRequired },

   { TTID_HogSafetyLoad,
     /* fileName */ TOOLS_PATH "/hog_safety.out",
     /* taskName */ "Hog Safety Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_SafetyTask | TTP_NormalBoot | TTP_NotRequired },

   { TTID_LanguageLoad,
     /* fileName */ TRIMA_PATH "/gui/language_loader.out",
     /* taskName */ "Language Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_LogTrapWrite,
     /* fileName */ NULL,           /* routine is included with sysinit module */
     /* taskName */ "stdout_write",
     /* commandLine */ "sysinit_logTrapWrite",
     /* stackSize */ 4000,
     /* priority */ 250,
     /* properties */ TTP_AllNodes | TTP_AllBootModes },

   { TTID_MakeBigFileLoad,  /* tool for making bogus rundata files */
     /* fileName */ TOOLS_PATH "/makebigfile_main.out",
     /* taskName */ "Make Big File Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_NotRequired },

   { TTID_MessageSystemTimer,
     /* fileName */ NULL,           /* routine is included with common.out module */
     /* taskName */ "msgsystmr",
     /* commandLine */ "MsgSysTimer_main__11MsgSysTimer",
     /* stackSize */ 4000,
     /* priority */ 25,
     /* properties */ TTP_AllNodes | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_Meter,
     /* fileName */ TRIMA_PATH "/service/meter.out",
     /* taskName */ "meter",
     /* commandLine */ "meter",
     /* stackSize */ 9000,
     /* priority */ 150,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_SyncWithSem },

   { TTID_Predict,
     /* fileName */ NULL,
     /* taskName */ "predict",
     /* commandLine */ "PredictTask_main",
     /* stackSize */ 26000,
     /* priority */ 254,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_SyncWithSem },

   { TTID_PredictLoad,
     /* fileName */ TRIMA_PATH "/proc/predict.out",
     /* taskName */ "Predict Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot },

   { TTID_Proc,
     /* fileName */ NULL,
     /* taskName */ "proc",
     /* commandLine */ "Proc_main",
     /* stackSize */ 25000,
     /* priority */ 70,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_SyncWithSem },

   { TTID_ProcLoad,
     /* fileName */ TRIMA_PATH "/proc/proc.out",
     /* taskName */ "Proc Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot },

   { TTID_RBCCmd,
     /* fileName */ NULL,
     /* taskName */ "rbc_cmd",
     /* commandLine */ "rbcCmd /tyCo/3",
     /* stackSize */ 5000,
     /* priority */ 45,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_RBCData,
     /* fileName */ NULL,
     /* taskName */ "rbc_data",
     /* commandLine */ "rbcData /tyCo/3",
     /* stackSize */ 2000,
     /* priority */ 45,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_RetOccMon,
     /* fileName */ NULL,
     /* taskName */ "retoccmon",
     /* commandLine */ "ReturnOccMon_main",
     /* stackSize */ 14000,
     /* priority */ 100,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_SyncWithSem },

   { TTID_RetOccMonLoad,
     /* fileName */ TRIMA_PATH "/proc/retoccmon.out",
     /* taskName */ "Return Occlusion Monitor Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot },

   { TTID_Router,
     /* fileName */ NULL,           /* routine is included with common.out module */
     /* taskName */ "router",
     /* commandLine */ "Router_main__6Router",
     /* stackSize */ 8000,
     /* priority */ 30,
     /* properties */ TTP_AllNodes | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_SafeDrv,
     /* fileName */ NULL,
     /* taskName */ "safe_drv",
     /* commandLine */ "safe_drv__FPCc $pfw_ridethru",
     /* stackSize */ 8000,
     /* priority */ 1,
     /* properties */ TTP_SafetyTask | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_SafeDrvLoad,
     /* fileName */ TRIMA_PATH "/low_level/safe_drv.out",
     /* taskName */ "Safety Driver Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_SafetyTask | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_SafeExec,
     /* fileName */ NULL,
     /* taskName */ "saf_exec",
     /* commandLine */ "saf_exec",
     /* stackSize */ 16000,
     /* priority */ 45,        // must preempt netTask running at 50
     /* properties */ TTP_SafetyTask | TTP_NormalBoot },

   { TTID_SafeExecLoad,
     /* fileName */ TRIMA_PATH "/safety/saf_exec.out",
     /* taskName */ "Safety Executive Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_SafetyTask | TTP_NormalBoot },

   { TTID_SafeFastMsg,             /* this task not required for Safety, just debug */
     /* fileName */ NULL,
     /* taskName */ "sfastmsg",
     /* commandLine */ "sfastmsg",
     /* stackSize */ 8000,
     /* priority */ 75,
     /* properties */ TTP_SafetyTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_NotRequired },

   { TTID_SafeFastMsgLoad,
     /* fileName */ TRIMA_PATH "/low_level/sfastmsg.out",
     /* taskName */ "Safety Fast Message Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_SafetyTask | TTP_NormalBoot | TTP_NotRequired },

   { TTID_SafeMsg,
     /* fileName */ NULL,
     /* taskName */ "safe_msg",
     /* commandLine */ "safe_msg",
     /* stackSize */ 8000,
     /* priority */ 30,
     /* properties */ TTP_SafetyTask | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_SafeMsgLoad,
     /* fileName */ TRIMA_PATH "/low_level/safe_msg.out",
     /* taskName */ "Safety Msg Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_SafetyTask | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_SafetyCommon,
     /* fileName */ TRIMA_PATH "/common/safety_lib.out",
     /* taskName */ "Safety Lib Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_SafetyTask | TTP_AllBootModes },

   { TTID_ServiceGraphicsLoad,
     /* fileName */ PNAME_SERVICE_GRAPHICS,
     /* taskName */ "Service Graphics Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_ServiceBoot },

   { TTID_ServiceLoad,
     /* fileName */ TRIMA_PATH "/service/service.out",
     /* taskName */ "Service Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_ServiceBoot },

   { TTID_Service,
     /* fileName */ NULL,
     /* taskName */ "smagent",
     /* commandLine */ "smagent",
     /* stackSize */ 15000,
     /* priority */ 160,
     /* properties */ TTP_ControlTask | TTP_ServiceBoot  },

   { TTID_Shell,
     /* fileName */ NULL,           /* routine is included with system initialization module */
     /* taskName */ "Shell",
     /* commandLine */ "usrShellInit",
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_AllBootModes | TTP_RunInSysinit },

   { TTID_SoftCRC_SafetyFiles,
     /* fileName */ NULL,
     /* taskName */ "SafetyCRC",
     /* commandLine */ "doSafetyFilesCrcCheck",
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_RunInSysinit },

   { TTID_SoftCRC_TrimaFiles,
     /* fileName */ NULL,
     /* taskName */ "TrimaCRC",
     /* commandLine */ "doTrimaFileCrcCheck",
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_RunInSysinit | TTP_Delay_5 },

   { TTID_SoftCRC_MachineFiles,
     /* fileName */ NULL,
     /* taskName */ "MachineCRC",
     /* commandLine */ "doMachineFileCrcCheck",
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_RunInSysinit },

   { TTID_SoftCRC_GuiFiles,
     /* fileName */ NULL,
     /* taskName */ "GuiCRC",
     /* commandLine */ "doGuiFileCrcCheck",
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_RunInSysinit | TTP_Delay_5 },

   { TTID_SoftCRC_ProcFiles,
     /* fileName */ NULL,
     /* taskName */ "ProcCRC",
     /* commandLine */ "doProcFileCrcCheck",
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_RunInSysinit | TTP_Delay_5 },

   { TTID_Sounds,
     /* fileName */ NULL,
     /* taskName */ "sounds",
     /* commandLine */ "sounds",
     /* stackSize */ 8000,
     /* priority */ 55,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_SpoofMainLoad,
     /* fileName */ TOOLS_PATH "/spoof_main.out",
     /* taskName */ "Spoof Main Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_NotRequired },

   { TTID_SpoofServerControl,
     /* fileName */ NULL,
     /* taskName */ "spoof_server",
     /* commandLine */ "spoof_server -priority 30 -name CONTROL",
     /* stackSize */ 60000,    /* increased from 15000 for vxWorks 5.5 */
     /* priority */ 30,        // priority overridden in command line
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_NotRequired },

   { TTID_SpoofServerControlLoad,
     /* fileName */ TOOLS_PATH "/spoof_server_control.out",
     /* taskName */ "Spoof Server Control Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_NotRequired },

   { TTID_SpoofServerSafety,
     /* fileName */ NULL,
     /* taskName */ "spoof_server",
     /* commandLine */ "spoof_server -priority 30 -name SAFETY",
     /* stackSize */ 60000,    /* increased from 15000 for vxWorks 5.5 */
     /* priority */ 30,        // priority overridden in command line
     /* properties */ TTP_SafetyTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_NotRequired },

   { TTID_SpoofServerSafetyLoad,
     /* fileName */ TOOLS_PATH "/spoof_server_safety.out",
     /* taskName */ "Spoof Server Safety Loader",
     /* commandLine */ NULL,
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_SafetyTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_NotRequired },

   { TTID_SysinitCleanup,
     /* fileName */ NULL,
     /* taskName */ "Cleaning up router",
     /* commandLine */ "sysinit_cleanup",
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_RunInSysinit },

   { TTID_SysinitSyncRouter,
     /* fileName */ NULL,
     /* taskName */ "Waiting for Router",
     /* commandLine */ "wait_for_message_system",
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_RunInSysinit },

   { TTID_SysIDLog,
     /* fileName */ TRIMA_PATH "/low_level/sysidlog.out",
     /* taskName */ "sysidlog",
     /* commandLine */ "sysidlog",
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_AllNodes | TTP_NormalBoot | TTP_RunInSysinit | TTP_ServiceBoot },

   { TTID_Ted,
     /* fileName */ TRIMA_PATH "/xif/ted.out",
     /* taskName */ "ted",
     /* commandLine */ "ted",
     /* stackSize */ 24000,
     /* priority */ 160,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot },

   { TTID_TFTP,
     /* fileName */ NULL,           /* routine is included with system initialization module */
     /* taskName */ "tftpd",
     /* commandLine */ NULL,        /* task is started automatically as needed during system startup */
     /* stackSize */ 5000,
     /* priority */ 2,
     /* properties */ TTP_AllNodes | TTP_AllBootModes },

   { TTID_TrapClient,
     /* fileName */ TRIMA_PATH "/xif/trap_client.out",
     /* taskName */ "trap_client",
     /* commandLine */ "trap_client",
     /* stackSize */ 15000,
     /* priority */ 160,
     /* properties */ TTP_ControlTask | TTP_NormalBoot },

   { TTID_TrimaBroadcast,
     /* fileName */ TRIMA_PATH "/xif/trima_broadcast.out",
     /* taskName */ "trima_broadcast",
     /* commandLine */ "trima_broadcast",
     /* stackSize */ 15000,
     /* priority */ 100,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_SyncWithSem },

   { TTID_UGLLoad,
     /* fileName */ NULL,
     /* taskName */ "UGL Loader",
     /* commandLine */ "loadUglModule",
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_RunInSysinit },

   { TTID_VipCmd,
     /* fileName */ TRIMA_PATH "/xif/vip_cmd.out",
     /* taskName */ "vip_cmd",
     /* commandLine */ "vip_cmd",
     /* stackSize */ 25000,
     /* priority */ 75,
     /* properties */ TTP_ControlTask | TTP_NormalBoot },

   { TTID_VipSts,
     /* fileName */ TRIMA_PATH "/xif/vip_sts.out",
     /* taskName */ "vip_sts",
     /* commandLine */ "vip_sts",
     /* stackSize */ 15000,
     /* priority */ 70,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_SyncWithSem },

   { TTID_LogNetworkStats,
     /* fileName */ TOOLS_PATH "/log_network_stats.out",
     /* taskName */ "log_network_stats",
     /* commandLine */ "logNetworkStats",
     /* stackSize */ 0,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_SafetyTask | TTP_NormalBoot | TTP_ServiceBoot | TTP_NotRequired },

   { TTID_StackUsageMonitor,
     /* fileName */ TRIMA_PATH "/service/sumo.out",
     /* taskName */ "stack_usage_monitor",
     /* commandLine */ "stackmon",
     /* stackSize */ 5000,
     /* priority */ 250,
     /* properties */ TTP_ControlTask | TTP_SafetyTask | TTP_NormalBoot },

   { TTID_WinApp,
     /* fileName */ NULL,        /* started by UGL call in osgraphics.cpp */
     /* taskName */ "winApp",
     /* commandLine */ NULL,
     /* stackSize */ 64000,
     /* priority */ 0,
     /* properties */ TTP_ControlTask | TTP_NormalBoot | TTP_ServiceBoot},

};

/*
 * Order of task startup on control side.  Note that the properties value also
 * has an impact on when tasks are started - see the TrimaTaskProperties definition.
 *
 * Control Priority Order
 *
 *    CtlDrv               1
 *    MessageSystemTimer   25
 *    CtlFastMsg           30
 *    CtlMsg               30
 *    Router               35
 *    RBCData              45
 *    RBCCmd               45
 *    Sounds               55
 *    AlarmHandler         65   -- Make a bit above Proc.  When Proc starts, it can swamp this with messages
 *    VipSts               70
 *    AlarmHandler         70
 *    Proc                 70
 *    VipCmd               75
 *    Barcode              90
 *    Broadcast            100
 *    RetOccMon            100
 *    Meter                150
 *    Ted                  160
 *    Service              160
 *    TrapClient           160
 *    LogTrapWrite         250
 *    StackUsagaMonitor    250
 *    Predict              254
 *
 */
const TrimaTaskID trimaControlStartupOrder[] =
{
   TTID_LogTrapWrite,
   TTID_ControlCommon,
   TTID_SysIDLog,
   TTID_CorruptMain,         /* run prior to safety boot */
   TTID_Router,
   TTID_MessageSystemTimer,
   TTID_SysinitSyncRouter,   /* waits for Router to become ready and then continues */

   /* Begin Start Loading all of the modules */
   TTID_CDSLoad,
   TTID_AlarmHandlerLoad,
   TTID_AlarmHandler,
   TTID_FontdefLoad,         /* needed by UGLLoad */
   TTID_UGLLoad,             /* needed by LanguageLoad */
   TTID_LanguageLoad,        /* needed by DatFileReaderLoad */
   TTID_DatFileReaderLoad,
   TTID_DatFileReader,
   TTID_GuiGraphicsLoad,
   TTID_GuiLoad,
   TTID_ServiceGraphicsLoad,
   TTID_ServiceLoad,
   TTID_RetOccMonLoad,
   TTID_ProcLoad,
   TTID_CtlDrvLoad,
   TTID_PredictLoad,
   TTID_SpoofServerControlLoad,
   TTID_SpoofMainLoad,
   TTID_HogControlLoad,
   TTID_MakeBigFileLoad,
   /* End Start Loading all of the modules */

   TTID_SoftCRC_SafetyFiles,   // These 2 files are checked by Safety and are short so do them early so Safety doesn't have to wait
   TTID_SoftCRC_MachineFiles,
   TTID_StackUsageMonitor,     // Start early because Proc sends it a message
   TTID_Sounds,
   TTID_Barcode,
   TTID_Gui,                   // 5 second delay for GUI to complete loading all files
   TTID_Service,

   TTID_CtlFastMsg,            // Provides APS raw data; others can be activate()'d for debug/testing
   TTID_CtlMsg,
   TTID_CtlDrv,

   /* Proc dependants */
   TTID_RetOccMon,
   TTID_Predict,
   TTID_RBCCmd,
   TTID_RBCData,
   TTID_Proc,                  // Proc State file initialization and GUI overwhelm the system.  Start them far enough apart to keep them from interfering

   TTID_Meter,
   TTID_Ted,
   TTID_VipSts,
   TTID_VipCmd,
   TTID_TrapClient,
   TTID_TrimaBroadcast,

   TTID_SoftCRC_TrimaFiles,    // Checked by Safety but takes a while
   TTID_SoftCRC_GuiFiles,      // 5 second delay for system rubustness
   TTID_SoftCRC_ProcFiles,     // 5 second delay for system rubustness
   TTID_Shell,
   TTID_SpoofServerControl,
   TTID_HogControl,
   TTID_LogNetworkStats,
   TTID_SysinitCleanup         // This should be the last task in the list since it deletes the router.
};

/*
 * Order of task startup on safety side.  Note that the properties value also
 * has an impact on when tasks are started - see the TrimaTaskProperties definition.
 */
const TrimaTaskID trimaSafetyStartupOrder[] =
{
   TTID_LogTrapWrite,
   TTID_SafetyCommon,
   TTID_SysIDLog,
   TTID_SafeExecLoad,
   TTID_SafeDrvLoad,
   TTID_SafeMsgLoad,
   // TTID_SafeFastMsgLoad,   /* not needed for production build */
   TTID_SpoofServerSafetyLoad,
   TTID_HogSafetyLoad,
   TTID_Router,
   TTID_MessageSystemTimer,
   TTID_SafeMsg,
   TTID_SafeExec,
   // TTID_SafeFastMsg,     /* not required for production build; see FAST_DATA in safe_drv.{hpp,cpp} */
   TTID_SafeDrv,
   TTID_GoodDog,
   TTID_StackUsageMonitor,
   // TTID_Shell,  /* not needed for production build */
   TTID_SpoofServerSafety,
   TTID_HogSafety,
   TTID_LogNetworkStats,

};

#endif /* ifdef TRIMA_SYSINIT_BUILD */

#ifdef __cplusplus
}  // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _TRIMA_TASKS_INCLUDE */

/* FORMAT HASH d5abc55869fb7ab42b39170ad02f856d */

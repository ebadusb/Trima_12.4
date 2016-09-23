
/*
 * $Header$
 *
 * Common Control board functions
 * Common file used on all target platforms.  Purpose is to keep
 * local copies of node_init.c small.
 *
 */

#include <vxWorks.h>
#include <errnoLib.h>
#include <time.h>
#include <drv/hdisk/ataDrv.h>
#include <dcacheCbio.h>
#include <fcntl.h>
#include <ioLib.h>
#include <dpartCbio.h>
#include <usrFdiskPartLib.h>
#include <stat.h>
#include <usrLib.h>
#include <mountLib.h>
#include <inetLib.h>
#include <routeLib.h>
#include <setjmp.h>
#include <ftpdLib.h>
#include <sysSymTbl.h>
#include <envLib.h>

#include "config.h"
#include "rtc.h"
#include "load_module.h"
#include "envfile.h"

#define OS_BUILD
#include "file_system.h"
#include "nfs_data.h"
#include "power_fail.h"
#undef OS_BUILD

#include "filesort.c"
#include "serial_drv.c"

#include "regcomp.c"
#include "regexec.c"
#include "regfree.c"
#include "regerror.c"
#include "readdir_stat.c"

#include "filenames.h"
#include "option_file.h"
#include "node_data.h"
#include "task_start.h"

#include "password.h"
#include "common_hw.h"
#include "trima_options.h"
#include "boot_mode.h"
#include "trima_driver.h"

/* external definitions from platform.c */
extern void usrCommonInitSetError( const char *msg );
extern void usrCommonInitSetFatalError( const char *msg );


/* Local Functions */
static void usrSetTime(void);
static void usrSetupDisk(void);
static void checkForUpdate(void);
static void checkForSingleStepMode(void);
static void usrCleanDisk(void);
static void usrSetupNFS(void);
static void usrHandleSafetyPFSaveFile(void);
static void usrSetupGlobvars(void);
static void usrInitGateway(const char *baseIP);
static void usrSetupSerialPort(void);
static STATUS internalFTPAuthorize(int checkPassword, const char * user, const char * password);


/* main entry point */
void nodeInit(void);

/* Used by FTP */
extern int ftpsMaxClients;
extern int ftpdTaskPriority;
extern int ftpdWorkTaskPriority;

extern int shellTaskPriority;

extern jmp_buf usrInitAbort;

 

void nodeInit(void)
{
   char *externalIP;

   usrSetTime();
   usrSetupDisk();

   /* load option files (hw.dat, features.bin) needed early in the boot sequence */
   if (trimaSysLoadOptionFiles() < 0)
   {
      usrCommonInitSetError("failed to load system option files");
   }

   /* load any hardware drivers defined in hw.dat */
   loadTrimaHWDriver("control_hardware_driver");

   checkForUpdate();

   /* Check for single step first in case stop button is stuck */
   checkForSingleStepMode();

   if (trimaNormalBootMode)
   {
       /*
        * If safety saved power fail recovery info the last time the system was booted,
        * make it accessible in the safety power fail save file
        */
      usrHandleSafetyPFSaveFile();
   }
   else /* alt boot mode */
   {
       /*
        * Clear any existing power fail information, since power fail recovery is not
        * allowed after alt-boot.
        */
      trimaSysClearAllPFData();
   }

   if (bootPromptSingleStep("perform disk cleanup") == SingleStep_Abort)
   {
      longjmp( usrInitAbort, 1 ); 
   }

   usrCleanDisk();


   if (bootPromptSingleStep("read machine setup data") == SingleStep_Abort)
   {
      longjmp( usrInitAbort, 1 ); 
   }

   usrSetupGlobvars();

   if (bootPromptSingleStep("initialize network devices") == SingleStep_Abort)
   {
      longjmp( usrInitAbort, 1 );
   }

   usrInitNetworkDevices();  /* network cards */

   externalIP = getenv("EXTERNALIP");

   if (externalIP != NULL)
      routeAdd("0.0.0.0", externalIP);
   else
   {
      printf("error adding default route  -- EXTERNALIP not found in globvars file");
   }

   if (bootPromptSingleStep("start FTP") == SingleStep_Abort)
   {
      longjmp( usrInitAbort, 1 );
   }

   /*
    * Setup FTP access
   */
   ftpdTaskPriority = 240;
   ftpdWorkTaskPriority = 241;
   ftpsMaxClients = 2;
   ftpdInit((FUNCPTR)&internalFTPAuthorize, 0);

   if (bootPromptSingleStep("start NFS") == SingleStep_Abort)
   {
      longjmp( usrInitAbort, 1 );
   }

   usrSetupNetworkApps();    /* Start BOOTP / PXE server */

    /*
     * Export NFS mount points
     */
   usrSetupNFS();

   if (bootPromptSingleStep("initialize serial port driver") == SingleStep_Abort)
   {
      longjmp( usrInitAbort, 1 );
   }

   /*
    * Setup the serial port for use
    */
   usrSetupSerialPort();
}


/*
 * Set time from battery backed RTC
 */
static void usrSetTime(void)
{
   struct timespec clockTime;

   if (getCurrentTimeFromRTC(&clockTime) == OK)
   {
      clock_settime(CLOCK_REALTIME, &clockTime);
   }
   else
   {
      printf("Error setting RTC.  Error = %d\n", errnoGet());
   }
}



/*
 *  Disk partition consistency check
 */
static STATUS usrCHKPartition(const char * partitionName)
{
   int  fileSystem;
   int  chkStatus;

   fileSystem = open(partitionName, O_RDONLY, 0);
   chkStatus = ioctl(fileSystem, FIOCHKDSK, DOS_CHK_ONLY | DOS_CHK_VERB_SILENT);

   if (chkStatus != OK)
   {
      ioctl(fileSystem, FIOCHKDSK, DOS_CHK_REPAIR | DOS_CHK_VERB_1);
   }

   close(fileSystem);
   return chkStatus;
}



/*
 *  Check/mount disk partitions
 */
static void usrSetupDisk(void)
{
   BLK_DEV *   hdDevRO;
   BLK_DEV *   hdDevRW;
   CBIO_DEV_ID  dcacheCbioRO, dcacheCbioRW;
   CBIO_DEV_ID partDevRO, partDevRW;
   int          partNum;
   PART_TABLE_ENTRY partData[EndOfPartitionEntries];

    /*
     * Create a device for reading/writing entire disk
     */
   hdDevRO = ataDevCreate(TRIMA_ATA_CTRL_NUM, 0, 0, 0);
   hdDevRW = ataDevCreate(TRIMA_ATA_CTRL_NUM, 0, 0, 0);
   dcacheCbioRO = dcacheDevCreate((CBIO_DEV_ID)hdDevRO, 0, 4096*1024, "hdro");
   dcacheCbioRW = dcacheDevCreate((CBIO_DEV_ID)hdDevRW, 0, TOTAL_CACHE_BLOCKS*512, "hdrw");
   dcacheDevTune(dcacheCbioRW, WRITE_CACHE_BLOCKS, 0, 0, 1);

    /*
     * Create a CBIO device for referencing the partition table
     */
   partDevRO = dpartDevCreate(dcacheCbioRO, (int)EndOfPartitionEntries, usrFdiskPartRead);
   partDevRW = dpartDevCreate(dcacheCbioRW, (int)EndOfPartitionEntries, usrFdiskPartRead);

    /*
     * Mount the partitions and check/repair file systems
     */
   for (partNum=FirstPartition; partNum<EndOfPartitionEntries; partNum+=1)
   {
      CBIO_DEV_ID cbioDev;

#ifdef MOUNT_READONLY
      if (trimaPartitionData[partNum].readOnly &&
          trimaNormalBootMode)
      {
         cbioDev = dpartPartGet(partDevRO, partNum);
      }
      else
#endif /* ifdef MOUNT_READONLY */
      {
         cbioDev = dpartPartGet(partDevRW, partNum);
      }

      dosFsDevCreate((char *)trimaPartitionData[partNum].name, cbioDev, trimaPartitionData[partNum].maxOpenFiles, NONE);
      usrCHKPartition(trimaPartitionData[partNum].name);
   }

   cbioModeSet(dcacheCbioRO, O_RDONLY);

   if (usrFdiskPartRead(dcacheCbioRO, partData, EndOfPartitionEntries) == OK)
   {
      for (partNum=FirstPartition; partNum<EndOfPartitionEntries; partNum+=1)
      {
         trimaPartitionData[partNum].bytesTotal = (long long)partData[partNum].nBlocks * hdDevRO->bd_bytesPerBlk;
         trimaPartitionData[partNum].bytesOffset = (long long)partData[partNum].offset * hdDevRO->bd_bytesPerBlk;
      }
   }
}


static FileCallBackStatus usrCleanTmp(const char * fullPathName)
{
   struct stat fileStat;

   if (stat((char *)fullPathName, &fileStat) == OK &&
       (fileStat.st_mode & S_IFDIR) != 0)
   {
        /*
         *  Recurse through sub-directories to make sure all files are removed
         */
      fileSort(fullPathName, FILE_SORT_BY_DATE_ASCENDING, usrCleanTmp);
   }

   attrib(fullPathName, "-R");
   remove(fullPathName);
   return FILE_CALLBACK_CONTINUE;
}



static FileCallBackStatus usrCleanRdOnly(const char * fullPathName)
{
   struct stat fileStat;

   if (stat((char *)fullPathName, &fileStat) == OK &&
       (fileStat.st_mode & S_IFDIR) != 0)
   {
        /*
         *  Recurse through sub-directories to make sure all files processed
         */
      fileSort(fullPathName, FILE_SORT_BY_DATE_ASCENDING, usrCleanRdOnly);
   }

   attrib(fullPathName, "-R");
   return FILE_CALLBACK_CONTINUE;
}



/*
 * Checks to see that the /machine partitiion has at least 500 MB of free space
 *
 */
static int usrCheckMachinePartitionFreeSpace(void)
{
   struct statfs   machinePartitionStat;
   size_t  minSpace = MIN_MACHINE_PARTITION_SPACE*1024*1024;
   int     result = 1;

   if (statfs(MACHINE_PARTITION_NAME, &machinePartitionStat) == OK)
   {
      if (machinePartitionStat.f_bavail < minSpace/machinePartitionStat.f_bsize)
      {
         result = 0;
      }
   }

   return result;
}

static void readLog(const char *logName)
{
   FILE * logfp = NULL;
   char    buffer[256];

   if ((logfp = fopen(logName, "r")) != NULL )
   {
      while ( fgets(buffer, sizeof(buffer), logfp) )
      {
         if (strlen(buffer) > 1)
         {
            printf("%s", buffer);
         }
      }
   }

   return;
}

/* This looks for a recent install log and reads/writes it to the dlog */
static int usrCheckInstallLogs(void)
{
   static const int MAX_INSTALL_LOGS = 1;

   int   result = 1;
   int   remaining_install_logs = 0;

	DIR * sortDir = opendir(INSTALL_LOG_PATH);
   char * fileBuffer = NULL;

   attrib(INSTALL_LOG_PATH, "-R");

	if ( sortDir )
	{
		struct dirent * dirEntry;
		struct stat	  	 fileStat;

		while ( (dirEntry = readdir(sortDir)) != NULL )
		{
			/*
			 *	Skip . and .. directory entries and anything that doesn't start with install_
			 */
			if ( strcmp(dirEntry->d_name, ".") != 0 &&
				  strcmp(dirEntry->d_name, "..") != 0 &&
              strncmp(dirEntry->d_name, FILE_INSTALL_LOG, sizeof(FILE_INSTALL_LOG)) == 0)
			{
            remaining_install_logs++;
//            if (remaining_install_logs == 1)
//            {
               fileBuffer = malloc(sizeof(INSTALL_LOG_PATH) + 1 + strlen(dirEntry->d_name) +1);
               if (fileBuffer)
               {
                  sprintf(fileBuffer, "%s/%s", INSTALL_LOG_PATH, dirEntry->d_name);
                  printf("Reading Latest Installation Log %s\n", fileBuffer);
                  readLog(fileBuffer);
                  printf("Done reading Installation Log\n");
//                  mv( fileBuffer, "*" fileBuffer );
                  free(fileBuffer);
//                  break;
               }
//            }
			}
		}
	}
	closedir(sortDir);

   if (remaining_install_logs > MAX_INSTALL_LOGS)
   {
      result = 0;
   }
   return result;
}

static FileCallBackStatus usrCleanInstallLogs(const char * fullPathName)
{
   attrib(fullPathName, "-R");
   remove(fullPathName);
   return(usrCheckInstallLogs()) ? FILE_CALLBACK_END : FILE_CALLBACK_CONTINUE;
}


static FileCallBackStatus usrCleanLog(const char * fullPathName)
{
   attrib(fullPathName, "-R");
   remove(fullPathName);
   return(usrCheckMachinePartitionFreeSpace()) ? FILE_CALLBACK_END : FILE_CALLBACK_CONTINUE;
}



static void usrCleanDisk(void)
{
   printf("performing disk cleanup\n");

    /*
     *  Make sure the temporary directory exists and is empty
     */
   mkdir(TEMP_PATH);
   fileSort(TEMP_PATH, FILE_SORT_BY_DATE_ASCENDING, usrCleanTmp);

    /*
     *  Make sure the update directory exists and is empty
     */
   mkdir(UPDATE_PATH);
   fileSort(UPDATE_PATH, FILE_SORT_BY_DATE_ASCENDING, usrCleanTmp);

    /*
     * Make sure the save data directory exists and that all files are writable
     */
   mkdir(SAVEDATA_PATH);
   fileSort(SAVEDATA_PATH, FILE_SORT_BY_DATE_ASCENDING, usrCleanRdOnly);

    /*
     *  Check space on machine partition.  Do a file cleanup in the log directory
     * if necessary to make enough space
     */
   if (!usrCheckMachinePartitionFreeSpace())
   {
      fileSort(LOG_PATH, FILE_SORT_BY_DATE_ASCENDING, usrCleanLog);
   }

    /*
     *  Check number of install log files.  Clean if too many files exist.
     */  
   mkdir(INSTALL_LOG_PATH);

   if (!usrCheckInstallLogs())
   {
      fileSort(INSTALL_LOG_PATH, FILE_SORT_BY_DATE_ASCENDING, usrCleanInstallLogs);
   }
}



static void usrSetupNFS(void)
{
   int mount;

   for (mount = 0; mount < NFS_MOUNT_POINTS; mount++)
   {
      if (nfsExport((char *)nfsMountData[mount]._mountPath, 0, nfsMountData[mount]._mountType, 0) != OK)
         perror("nfsExport");
   }
}



static void checkForSingleStepMode()
{
   if (!bootStopButtonPressed() &&
       bootPauseButtonPressed())
   {
      int retry = 5*sysClkRateGet();

      printf("\n\npause button detected - release the pause button to enable single step mode\n");
      taskDelay(2);

      while (bootPauseButtonPressed() &&
             retry-- > 0)
      {
         taskDelay(1);
      }

      if (retry > 0)
      {
         printf("single step mode enabled\n");
         trimaSingleStepBootMode = TRUE;
      }
      else
      {
         usrCommonInitSetError("pause button release not detected");
      }
   }
}



static void checkForUpdate()
{
   if ( bootKeyboardAttached() || /* if keyboard connected, install the script */
        (bootStopButtonPressed() && bootPauseButtonPressed())
      )
   {
        /* Check for install script and attempt to execute it if present */
      if (loadModuleFromFile(TRIMA_INSTALL_SCRIPT, NULL) == OK)
      {
         SYM_TYPE symType;
         int (*INSTALL_ENTRY_POINT)(void);

         /* The format of power fail save information may change between software versions */
         trimaSysClearAllPFData();

         printf("\n\ninstallation script found - release both buttons to execute\n");

         while (bootStopButtonPressed() ||
                bootPauseButtonPressed())
         {
            taskDelay(1);
         }

         if (symFindByName(sysSymTbl, "updateTrima", (char **)&INSTALL_ENTRY_POINT, &symType) != OK)
         {
            usrCommonInitSetFatalError("bad install script: can't locate updateTrima entry point");
         }

         printf("starting installation\n");

         if ((*INSTALL_ENTRY_POINT)() != 0)
         {
            usrCommonInitSetFatalError("installation failed");
         }
         else
         {
            remove( TRIMA_INSTALL_SCRIPT );
            printf("\ninstallation complete - cycle power to restart");

            /* Wait forever for power to be cycled */
            while (1)
               taskDelay(1);
         }
      }
   }
}


static void usrHandleSafetyPFSaveFile(void)
{
   char    * data = (char *)malloc(trimaSysSafetyPFSaveArea.nSecs * 512);
   unsigned int    * dataLength  = (unsigned int *)data;

   trimaSysSafetyPFSaveArea.pBuf = data;
   trimaSysSafetyPFSaveArea.direction = 0;
   ataRawio(TRIMA_ATA_CTRL_NUM, 0, &trimaSysSafetyPFSaveArea);

   if (*dataLength > 0 && *dataLength < trimaSysSafetyPFSaveArea.nSecs * 512)
   {
      FILE * fp = fopen(SAFETY_PFSAVE_FILE, "wb");
      if (fp)
      {
         fwrite(&data[sizeof(*dataLength)], 1, *dataLength, fp);
         fclose(fp);
      }
   }
}


static void usrSetupGlobvars(void)
{
   printf("reading machine setup data\n");

    /*
     *  Try to read machine specific file.  If it isn't available, read the default file
     * but report the error.
     */
   if (readEnvironmentFile(GLOBVARS_FILE) != OK)
   {
      usrCommonInitSetError("error reading globvars data\n");
      perror(GLOBVARS_FILE);

      if (readEnvironmentFile(GLOBVARS_DEFAULT_FILE) != OK);
      {
         perror(GLOBVARS_DEFAULT_FILE);
      }
   }
}


static void usrInitGateway(const char *baseIP)
{
   struct in_addr addrStruct;
   unsigned long addr;
   char gatewayIP[20];

   // convert the address to a number
   addr = inet_addr((char *)baseIP);

   // Set the last 2 octets to be the gateway address (254.254)
   addr &= 0x0000FFFF;
   addr |= 0xFEFE0000;

   // Copy over the address
   addrStruct.s_addr = addr;

   inet_ntoa_b(addrStruct, gatewayIP);
   if (mRouteAdd("0.0.0.0", gatewayIP, 0, 0, 0) != OK)
   {
      printf("Failed to add Gateway route to %s", gatewayIP);
   }
}



static void usrSetupSerialPort(void)
{
   const char *serialPortOption;
   const int serialPort[4] = { 0x3f8, 0x2f8, 0x3e8, 0x2e8};
   const int serialInt[4]  = { 4, 3, 11, 7};
   int port;

   // Pull serial port interrupt mapping from config/hw.dat settings
   serialPortOption = trimaSysGetHardwareSetting("control_serial_driver");

   if (serialPortOption)
   {
      if (sscanf(serialPortOption, "%x:%d %x:%d %x:%d %x:%d", 
                 &serialPort[0], &serialInt[0], 
                 &serialPort[1], &serialInt[1], 
                 &serialPort[2], &serialInt[2], 
                 &serialPort[3], &serialInt[3]) != 8)
      {
         printf("invalid serial port option \"%s\"\n", serialPortOption);
         usrCommonInitSetFatalError("failed to initialize serial port driver");
      }
   }

   // Already initialized in usrConfig.c when NUM_TTY > 0
   if (NUM_TTY == 0) ttyDrv();

   // Initialize remaining ports not already initialized in usrConfig.c
   for (port = NUM_TTY; port < 4; port++)
   {
      setupSerialPort(serialPort[port], serialInt[port], port);
   }
}





/****************************/
/* Check for internal user. */
/****************************/
static STATUS checkInternalFtpPword(const char * user, const char * password)
{
   /* Guard against null pointers */
   if (user == NULL || password == NULL)
   {
      // This is an odd case, so log it.
      printf("FTP access denied - null username or password\n");
      return ERROR;
   }

#if (!defined(INTFTPUSER) || !defined(INTFTPPASS))
#error "No internal FTP user/password defined"
#endif

   if (strcmp(INTFTPUSER,user) == 0 && strcmp(INTFTPPASS,password) == 0) return OK;

   return ERROR;
}



static STATUS checkExternalFtpPword(const char * user, const char * password)
{
   char *xu=NULL, *xp=NULL;

   /* Guard against null pointers */
   if (user == NULL || password == NULL)
   {
      printf("FTP access denied - null username or password\n");
      return ERROR;
   }

   /****************************/
   /*** Check for R&D user.  ***/
   /****************************/
   #if (defined(RDFTPUSER) && defined(RDFTPPASS))
   if (strcmp(RDFTPUSER,user) == 0 && strcmp(RDFTPPASS,password) == 0) return OK;
   #endif

   /****************************/
   /* Check for external user. */
   /****************************/
   if (NULL != (xu=getenv("EXTERNALUSER")) && NULL != (xp=getenv("EXTERNALPASS")))
   {
      if (*xu != '\0' && *xp != '\0')
      {
         if (strcmp(xu,user) == 0 && strcmp(xp,password) == 0)
         {
            /* Allow only one login with this username/password pair */
            putenv("EXTERNALUSER=");
            putenv("EXTERNALPASS=");
            return OK;
         }
      }
   }

   /****************************/
   /**** No others allowed. ****/
   /****************************/
   return ERROR;
}



static STATUS checkFtpCassetteState()
{
   SYM_TYPE symType;
   static unsigned short (*hwCassetteFunc)(void) = NULL;
   static unsigned int firstTimeLoggingFtpSwDat = 0;

   // An additional check for cassette not lowered may be necessary
   if (bootNormalModeEnabled() &&
       !bootSingleStepEnabled())
   {
      unsigned short cassetteStatus = 0;

      if (!hwCassetteFunc)
         symFindByName(sysSymTbl, "hw_cassetteGetStatus", (char **)&hwCassetteFunc, &symType);

      if (hwCassetteFunc)
      {
         cassetteStatus = (*hwCassetteFunc)();

         if (!cassetteStatus)
         {
            // May have caught the safety blink test
            taskDelay(1);
            cassetteStatus = (*hwCassetteFunc)();
         }

          if (!(cassetteStatus & hw_cassetteRaised))
          {
             printf("FTP access denied - unable to verify cassette state\n");
             return ERROR;
          }
      }
      else
      {
         printf("FTP access denied - cassette state function not found\n");
         return ERROR;
      }
   }

   return OK;
}


/*
 * Authorize FTP user
 */
static STATUS internalFTPAuthorize(int checkPassword, const char * user, const char * password)
{
   static int log911ModeOnce = FALSE;
   static int logFtpGrantedOnce = FALSE;
   static int logFtpDeniedOnce = FALSE;


   // The FTP Server code calls this function for all sorts of (stupid) things.  
   // If the checkPassord var is not set, then don't test the user/password as it is bogus
   // But we need to check if the checkPassword is not set so that you can cancel a 
   // connection midstream.


   //
   // Super secret password always works and bypasses all checks.
   // External user password works if:
   //    cassette is up (config file settable)
   //    cassette is unknown but safety failed to boot.
   //

   // If request is made when Safety is booting, always allow.  
   // Makes safety boot up faster.
   //
   if (safetyBootInProgress)
   {
      return OK;   // Silent logging
   }

   // Internal (super secret) password bypasses all checks.
   // Works for Safety and the very priveledged
   if (checkPassword && (checkInternalFtpPword(user, password) == OK))
   {
      return OK;   // Silent logging
   }

   // If External password check (for STS) fails, you are done.
   if (checkPassword && (checkExternalFtpPword(user, password) != OK))
   {
      printf("FTP access denied - invalid username + password [%s,%s]\n", user, password);
      return ERROR;
   }

   // If Cassette Test passes or Safety Failed to boot, then FTP is OK.
   if (bootSafetyBootFailed())
   {
      if (!log911ModeOnce)
      {
         printf("FTP access granted.  Safety failed to boot.  911 mode\n");
         log911ModeOnce = TRUE;
      }

      return OK;
   }
   else 
   {
      struct stat buffer;   
      if ( (stat(FTP_ALLOWED, &buffer) == 0) ||
           bootKeyboardAttached()
         )
      {
         if (!logFtpGrantedOnce)
         {
            printf("FTP access granted. Config file found\n");
            logFtpGrantedOnce = TRUE;
         }
         return OK;
      }
      else if (checkFtpCassetteState() == OK)
      {
         if (!logFtpGrantedOnce)
         {
            printf("FTP access granted.  Cassette in up position\n");
            logFtpGrantedOnce = TRUE;
         }

         return OK;
      }
      else
      {
         if (!logFtpDeniedOnce)
         {
            printf("FTP access denied - Cassette in down position\n");
            logFtpDeniedOnce = TRUE;
         }

         return ERROR;
      }
   }
}

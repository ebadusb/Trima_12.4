/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>
#include <envLib.h>
#include <pingLib.h>
#include <sysLib.h>

#define CONTROL_BUILD

#include "crcgen.h"
#include "filenames.h"
#include "sysinit.h"
#include "common_hw.h"
#include "trima_datalog.h"
#include "node_data.h"

#include "os_comm.h"
#include "power_fail.h"
#include "boot_mode.h"

#include "discover_hw.h"

#include "tftpd.h"

#if CPU!=SIMNT // The bootSafetyIsAlive and pxeServer symbols don't exist in the simulator
#include "boot_mode.h"

#define SERVEICE_MODE_OVERRIDE 0

/* PXE Server forward declaration function (also not present in simulator */
extern "C" STATUS pxeServer (void);

#endif // #if CPU!=SIMNT

extern "C" STATUS shellInit (int, int);
extern "C" STATUS telnetdParserSet (FUNCPTR pParserCtrlRtn);
extern "C" STATUS shellParserControl (...);
extern "C" STATUS telnetdInit (int, BOOL);
extern "C" STATUS telnetdStart (int);

extern int shellTaskPriority;

enum TFTPD_State
{
   TFTPD_Idle,
   TFTPD_BaseOSTransferStarted,
   TFTPD_BaseOSTransferCompleted
};

const int          safetyTimeOutSeconds = 180;
static TFTPD_State tftpdState           = TFTPD_Idle;

static bool sysinit_tftpd_callback (TFTPD_CallBackStatus status, struct in_addr& clientAddr, const char* fileName)
{
   if (status == TFTPD_FileTransferRequest)
   {
      SysNodeData safetyData;

      sysGetNodeData(LocalNode, &safetyData);

      if (clientAddr.s_addr != safetyData.ipAddress[SafetyNode])
      {
         char    client_ipAddress[INET_ADDR_LEN];
         char    safety_ipAddress[INET_ADDR_LEN];
         in_addr safetyAddr;

         safetyAddr.s_addr = safetyData.ipAddress[SafetyNode];
         inet_ntoa_b(clientAddr, client_ipAddress);
         inet_ntoa_b(safetyAddr, safety_ipAddress);

         DataLog(log_level_critical) << "TFTP transfer from " << client_ipAddress << " not permitted.  Expected "
                                     << safety_ipAddress << endmsg;

         return false;
      }
      else
      {
         DataLog(log_level_startup_info) << "transferring Safety OS image " << fileName << endmsg;

#define HACK 1
#if HACK
         // Some Versalogic network cards get socket timeouts when receiving the
         // bootrom.  Delay a bit after the request to make the problem go away
         const int clkRatePerSec = sysClkRateGet();
         const int quarterSecond = clkRatePerSec / 4;
         taskDelay(quarterSecond);
#endif
      }
   }
   else if (status == TFTPD_FileTransferCompleted)
   {
      tftpdState = TFTPD_BaseOSTransferCompleted;

      if (safetyBootInProgress)
         DataLog(log_level_startup_info) << "\nSafety OS image transferred successfully." << endmsg;
   }

   return true;
}


void sysinit_setup_environment_variables (void)
{
   unsigned long trimaCRC  = 0;
   unsigned long safetyCRC = 0;

   bool crcReadOK = false;
   FILE* fp       = fopen(TRIMA_CRC_FILE, "r");

   if (fp)
   {
      crcReadOK = ( fscanf(fp, "%lx", &trimaCRC) == 1 );
      fclose(fp);
   }

   if (!crcReadOK)
   {
      safetyBootFailed = TRUE;
      perror("Trima CRC read back failed");
      longjmp(sysinit_abort, 1);
   }

   char envVarSet[40];
   sprintf(envVarSet, "TRIMA_CRC=0x%08lx", trimaCRC);
   putenv(envVarSet);

   crcReadOK = false;
   fp        = fopen(SAFETY_CRC_FILE, "r");

   if (fp)
   {
      crcReadOK = ( fscanf(fp, "%lx", &safetyCRC) == 1 );
      fclose(fp);
   }

   if (!crcReadOK)
   {
      safetyBootFailed = TRUE;
      perror("Safety CRC read back failed");
      longjmp(sysinit_abort, 1);
   }

   sprintf(envVarSet, "SAFETY_CRC=0x%08lx", safetyCRC);
   putenv(envVarSet);

   unsigned short version;
   version = hw_hardwareVersion();
   sprintf(envVarSet, "HW_REVISION=%d.%d", (version & 0xff00) >> 8, version & 0xff);
   putenv(envVarSet);

   const int LineSize = 60;

   char line[LineSize] = "\0";
   char rev[LineSize];

   fp = fopen(TRIMA_PROJECT_REVISION_FILE, "r");
   bool revReadOK = false;

   if (fp &&
       fgets(line, 60, fp) &&
       sscanf(line, "$ProjectRevision: %s ", rev) == 1 )
   {
      sprintf(envVarSet, "TRIMA_BUILD=%s", rev);
      putenv(envVarSet);
      revReadOK = true;
   }
   else if (fp)
   {
      printf("revision data: %s\n", line);
   }

   if (fp)
   {
      fclose(fp);
   }

   if (!revReadOK)
   {
      safetyBootFailed = TRUE;
      perror("Trima revision read back failed");
      longjmp(sysinit_abort, 1);
   }
}

void sysinit_prestart_node (void)
{
    #if CPU!=SIMNT // Fix some undefined symbols in the simulator.

   const int clkRatePerSec = sysClkRateGet();
   const int halfSecond    = clkRatePerSec / 2;

   bool serviceModeButtonsPressed = false;
   bool timedOut                  = FALSE;
   bool serviceModeOverride       = false;


   // This setting is redundant but left here for brevity.  This flag works fine in all machines
   // but old Ampro's with original parts.  The idea was to make the window for anyone to get in
   // using FTP as small as possible but it causes boot up failures in old Ampro machines.

#if SERVEICE_MODE_OVERRIDE // Block to always boot machine in Service Mode
   const int serviceModeFile = open(SERVICE_MODE_ALWAYS, O_RDONLY, 0644);

   if (serviceModeFile != ERROR)
   {
      printf("Found service_mode file\n");
      close(serviceModeFile);
      serviceModeOverride = true;
   }
#endif

   // Poor service guy.  Aaron wants your thumbs to hurt.  Keep prompt here for backwards
   // compatibility.  Seems like a good idea to put this before starting TFTP to prevent Safety
   // starting before Control notices the buttons being pushed.
   //
   if (serviceModeOverride || (bootStopButtonPressed() && bootPauseButtonPressed()))
   {
      printf("\n\nstarting service mode - hold buttons until instructed to release them\n\n");
      serviceModeButtonsPressed = true;
   }
   else
   {
      // Let service guy know that he missed the window as soon as possible.
      printf("...starting Normal mode...\n\n");
   }

   //
   // Take the Sem before starting tftp task.
   //
   semTake(bootSyncSem, WAIT_FOREVER);

   //
   // Start the TFTP task.  TFTP is used to transfer the Safety boot image.
   // Make sure this starts before the PXE Server
   // Task will give bootSyncSem back when started.
   //
   startTFTPD(sysinit_tftpd_callback);

   //
   // Wait 5 seconds for task to start.  If sem times out, boot will probably fail
   //
   if (semTake(bootSyncSem, 5 * sysClkRateGet()) == ERROR)
   {
      printf("TFTPD took too long to start.  Boot may not succeed.\n");
   }

   //
   // Spawn the PXE boot service to allow safety to network boot using PXE or BOOTP.
   //
   printf("spawning PXE task\n");
   taskSpawn("tPxeTask", // parasoft-suppress PB-27 "Ignore for OS functions"
             20, 0, 5000, (FUNCPTR)pxeServer, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

   printf("waiting for Safety startup\n");

   unsigned int delay = 0;

   while (delay < safetyTimeOutSeconds)   // Delay up to N seconds waiting for safety to start
   {
      if (bootSafetyIsAlive())
      {
         printf("Safety communication established.  Continuing boot sequence\n");
         break;
      }

      // Wait a second
      taskDelay(1 * sysClkRateGet()); // parasoft-suppress BD-TRS-TSHL "Intentional block in critical section"
      delay += 1;
   }

   if ( (tftpdState < TFTPD_BaseOSTransferStarted) || !bootSafetyIsAlive())
   {
      DataLog(log_level_critical) << "\n\nSafety failed to start within " << safetyTimeOutSeconds << " seconds timeout period..." << endmsg;

      safetyBootFailed     = TRUE;
      safetyBootInProgress = FALSE;   // This identifies that Safety is timeouted
      longjmp(sysinit_abort, 1);
   }

   /* Don't need to wait for safety twice.
    * Either wait for Service mode to start or wait for Safety to boot in Normal mode.
    */
   if (serviceModeButtonsPressed)
   {
      osCommSend(OSComm_SwitchToService);
      trimaSysClearAllPFData();

      // Control sees the buttons pushed first, then it waits for Safety to see the buttons pushed
      // When Safety sees the buttons pushed, it sends the message to Control acknowledging Service Mode.
      // When Control sees the mssage from Safety, the handshake is complete and the message to the service guy gets displayed
      sysinit_waitServiceModeAcknowledge();
   }

   #endif // #if CPU!=SIMNT

   sysinit_telnet_configuration();
} // parasoft-suppress BD-TRS-LOCK "Unreleased lock on bootSyncSem is okay"

void sysinit_telnet_configuration (void)
{
   const int spoofer     = open(PNAME_SPOOF_SERVER_CONTROL_FILE, O_RDONLY, DEFAULT_FILE_PERM);
   const int telnet_file = open(TELNET_ON, O_RDONLY, DEFAULT_FILE_PERM);

   // If there are either:
   //    spoofer files present,
   //    start telnet special file
   //    keyboard attached
   // then start the telnet server
   if ( (spoofer != ERROR) || (telnet_file != ERROR) || bootKeyboardAttached())
   {
      DataLog(log_level_startup_detail) << "starting telnet server" << endmsg;

      if ((taskNameToId ("tShell")) == ERROR) // parasoft-suppress PB-27 "Ignore for VxWorks API"
      {
         /* Shell not started yet. */
         DataLog(log_level_startup_detail) << "Shell not started yet" << endmsg;
         shellTaskPriority = 100;
         shellInit(60000, TRUE);
      }

      /* Added by Michel S. Chaffin Oct 2011 - There is a difference in how
       *        Trima 5.x (VxWorks 5.4.2 ) and Trima 6.x (VxWorks 5.5.1) start Telnet.
       *        This is specific to VxWorks versions */
      if ( ERROR == telnetdParserSet(shellParserControl) )
      {
         DataLog(log_level_startup_detail) << "telnetdParserSet( ) Failed" << endmsg;
      }
      else if ( ERROR == telnetdInit(1, FALSE) )
      {
         DataLog(log_level_startup_detail) << "telnetdInit( ) Failed" << endmsg;
      }
      /* If telnetInit reports good, attempt to start the telnetd
       * Server; at default port */
      else if ( ERROR == telnetdStart(Telnet_ServerPort) )
      {
         DataLog(log_level_startup_detail) << "TelnetdStart( ) Failed" << endmsg;
      }
      else
      {
         DataLog(log_level_startup_detail) << "Telnet Server started" << endmsg;
      }
   }
   else
   {
      DataLog(log_level_startup_detail) << "File spoof_server_control.out not present.  Errno=" << errnoMsg << endmsg;
   }

   // close the files
   if (spoofer != ERROR)
      close(spoofer);

   if (telnet_file != ERROR)
      close(telnet_file);
}

#if CPU==SIMNT
int bootKeyboardAttached (void){ return 0; }
#endif

/* FORMAT HASH 6205caaf972d77a73d6aea0a29e369ef */

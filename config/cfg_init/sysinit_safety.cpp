/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>
#include <stdio.h>
#include <ioLib.h>

#include "sysinit.h"
#include "boot_mode.h"
#include "os_comm.h"
#include "filenames.h"
#include "safety_warm_ram.h"


void sysinit_setup_environment_variables (void)
{
   // Set up the protected RAM area on Safety for use
   safetyRamInit();
}

extern BOOL trimaServiceBootMode;

void sysinit_prestart_node (void)
{
   bool serviceModeOverride = false;

   printf("\n\nSending Safety alive message - notifying Control\n\n");
   osCommSend(OSComm_SafetyAlive);  // Notify control that Safety has booted

#if 0
   const int serviceModeFile = open(SERVICE_MODE_ALWAYS, O_RDONLY, 0644);

   if (serviceModeFile != ERROR)
   {
      printf("Found service_mode file\n");
      close(serviceModeFile);
      serviceModeOverride = true;
   }
#endif

   if (serviceModeOverride || (bootStopButtonPressed() && bootPauseButtonPressed()))
   {
      printf("\n\nstarting service mode - notifying Control\n\n");
      osCommSend(OSComm_SwitchToService);

      sysinit_waitServiceModeAcknowledge();
   }
}

/* FORMAT HASH 641667ae96adf17dc3499c47b0833e76 */

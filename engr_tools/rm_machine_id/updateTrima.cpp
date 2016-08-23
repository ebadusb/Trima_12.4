/*
 * Copyright (C) 2012 Terumo BCT, Inc.  All rights reserved.
 *
 * Update program for R&D/QA to remove the Machine ID file and recompute CRCs.
 * The Machine ID file is part of Trima_6.3. This utility can be used to
 * restore the machine's config directory to a pre-6.3 configuration.
 *
 * A developer/tester can remove /config/machine.id in two ways:
 * 1) FTP bin/updateTrima to machine's /update directory and do service reboot
 * 2) Run via command-line using telnet or WindShell terminal:
 *
 *    -> ld 1, 0, "rm_machine_id.out"
 *    -> rm_machine_id
 */
#include <vxWorks.h>
#include <taskLib.h>
#include <usrLib.h>
#include <stat.h>
#include <stdio.h>

#include "crcgen.h"
#include "filenames.h"

extern "C" void updateTrima();
extern "C" int rm_machine_id();

/**
 * Service-mode entry-point for updating files.
 */
void updateTrima()
{
   STATUS status = rm_machine_id();

   // Regardless of status, delete the update script
   // so that it doesn't run on subsequent reboot.
   //
   printf( "Removing update script.\n" );
   rm( UPDATE_PATH "/updateTrima" );
   fflush( stdout );
}

/**
 * Removes the Machine ID file and then recomputes the associated CRC files.
 */
int rm_machine_id()
{
   // Check if the file exists
   struct stat fileStat = {0};
   STATUS status = stat( PNAME_MACHINE_ID, &fileStat );
   if ( status != OK )
   {
      // Nothing to do here
      printf( "Machine ID does not exist: %s\n", PNAME_MACHINE_ID );
      return OK;
   }

   //
   // Make sure we don't interrupt anybody else who is running ...
   //
   taskPrioritySet( taskIdSelf(), 250 );

   // Unlock the parent directory
   attrib( CONFIG_PATH, "-R" );

   // Remove the machine ID file
   printf( "Removing the machine ID file: %s\n", PNAME_MACHINE_ID );
   fflush( stdout );
   attrib( PNAME_MACHINE_ID, "-R" );
   rm( PNAME_MACHINE_ID );
   attrib( PNAME_MACHINE_ID, "+R" );

   // Update the associated CRC values
   //
   attrib( PNAME_MACHINE_CRC, "-R" );
   status = softcrc( "-filelist " MACHINE_CRC_LIST_FILES " -update " PNAME_MACHINE_CRC );
   attrib( PNAME_MACHINE_CRC, "+R" );

   // Lock the parent directory
   attrib( CONFIG_PATH, "+R" );

   // Verify the installation CRC values
   //
   status = softcrc( "-filelist " MACHINE_CRC_LIST_FILES " -verify " PNAME_MACHINE_CRC );
   if ( OK != status )
   {
      printf( "Machine CRC check failed!\n" );
   }
   else
   {
      printf( "Machine ID removal complete.\n" );
   }
   fflush( stdout );
   return status;
}

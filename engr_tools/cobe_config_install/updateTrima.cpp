
/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * Install program for the recirculate change to Cobe_config.dat
 *
 * 
 */
#include <vxWorks.h>
#include <cbioLib.h>
#include <dosFsLib.h>
#include <private/dosFsLibP.h>
#include <bootLib.h>
#include <sysLib.h>
#include <taskLib.h>
#include <unistd.h>
#include <usrLib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#include "filenames.h"

#ifdef __cplusplus
extern "C" { 
#endif

int cp(const char * from, const char * to);
void updateTrima();

int softcrc(const char * options);

#ifdef __cplusplus
};
#endif

void updateTrima()
{
   //
   // Make sure we don't interrupt anybody else who is running ...
   //
   taskPrioritySet( taskIdSelf(), 250 );

   //
   // Make the /trima partition is writable
   DOS_VOLUME_DESC_ID  pVolDesc;
   pVolDesc = dosFsVolDescGet((void*)TRIMA_PATH, NULL);

   if ( !pVolDesc )
   {
        perror(TRIMA_PATH);
        return;
   }

   cbioModeSet(pVolDesc->pCbio, O_RDWR);   

   attrib(PNAME_COBECONFIGDAT,"-R");
   remove(PNAME_COBECONFIGDAT);
   printf("Copying the cobe_config.dat\n");
   fflush( stdout );
   cp(UPDATE_PATH "/cobe_config.dat",  PNAME_COBECONFIGDAT);
   attrib(PNAME_COBECONFIGDAT, "+R");


   //
   // Update configuration CRC values
   attrib(TRIMA_CRC_FILE,"-R");
   softcrc("-filelist " TRIMA_CRC_LIST_FILES " -update " TRIMA_CRC_FILE);
   attrib(TRIMA_CRC_FILE, "+R");  // set attribute back so no one messes with it
   attrib(TRIMA_PATH, "+R");

   //
   // Verify the installation CRC values
   if (softcrc("-filelist " TRIMA_CRC_LIST_FILES " -verify " TRIMA_CRC_FILE) != 0)
   {
      printf("Trima CRC check failed\n");
      return;
   }

   printf("Trima cobe_config update complete.\n" );

   // Delete the update script so that it doesn't run again on the subsequent boot if the GTS guy
   // is still holding down the buttons.
   printf("removing update script.\n");
   remove( UPDATE_PATH "/updatetrima" );
   remove( UPDATE_PATH "/cobe_config.dat" );

   fflush( stdout );

}

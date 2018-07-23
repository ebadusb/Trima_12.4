/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      ProjectRev.h
 *             Project Revision file header
 *
 * ABSTRACT:   This file reads the Project Revision MKS variable.
 *
 *
 ******************************************************************************/

#ifndef PROJECTREV_H  //  Prevents multiple inclusions.
#define PROJECTREV_H

#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REV_SIZE 512
static char rev[ REV_SIZE ] = "Trima Build";
static int  bInitialized    = 0;

#define CAT_SIZE(__str__) ( REV_SIZE - 1 - strlen(__str__)>0 ? REV_SIZE - 1 - strlen(__str__) : 0 )


// Remove any copies of the specified character from the string in place
//
static int removeChar (char* strToChange, char ch)
{
   unsigned int   destIdx, srcIdx;
   for (destIdx = 0, srcIdx = 0; strToChange[srcIdx] != '\0'; ++srcIdx)
   {
      if (strToChange[srcIdx] != ch)
      {
         strToChange[destIdx] = strToChange[srcIdx];
         ++destIdx;
      }
   }

   strToChange[destIdx] = '\0';
   return OK;
}

static const char* getProjectRevision ()
{
   if ( !bInitialized )
   {
      char* revString = NULL;
      char* crcString = NULL;
      FILE* fp        = NULL;

      revString = getenv("TRIMA_BUILD");
      strncat(rev, "  BUILD REVISION: ", CAT_SIZE(rev) );
      strncat(rev, (revString) ? revString : "ERROR", CAT_SIZE(rev) );

      crcString = getenv("TRIMA_CRC");
      strncat(rev, "  CRC: ", CAT_SIZE(rev) );
      strncat(rev, (crcString) ? crcString : "ERROR", CAT_SIZE(rev) );

      fp = fopen("/trima/build_info", "r");
      if ( fp != NULL )
      {
         char  buffer[REV_SIZE - 1];
         char* startStr = NULL;
         strncat(rev, "  BUILD DATE: ", CAT_SIZE(rev) );
         if ( fgets(buffer, CAT_SIZE(rev), fp) != NULL ) strncat(rev, buffer, CAT_SIZE(rev) );
         strncat(rev, " ", CAT_SIZE(rev) );
         if ( fgets(buffer, CAT_SIZE(rev), fp) != NULL ) strncat(rev, buffer, CAT_SIZE(rev) );
         strncat(rev, "  BUILD DIR: ", CAT_SIZE(rev) );
         if ( fgets(buffer, CAT_SIZE(rev), fp) != NULL ) strncat(rev, buffer, CAT_SIZE(rev) );
         strncat(rev, "  BUILD COMPUTER: ", CAT_SIZE(rev) );
         startStr = NULL;
         if ( fgets(buffer, REV_SIZE - 1, fp) != NULL ) startStr = strrchr(buffer, ' ');
         if ( startStr != NULL ) strncat(rev, startStr, CAT_SIZE(rev) );
         strncat(rev, "  BUILD USER: ", CAT_SIZE(rev) );
         startStr = NULL;
         if ( fgets(buffer, REV_SIZE - 1, fp) != NULL ) startStr = strrchr(buffer, ' ');
         if ( startStr != NULL ) strncat(rev, startStr, CAT_SIZE(rev) );
         rev[ REV_SIZE - 1 ] = 0;
         fclose(fp);
         bInitialized        = 1;
      }

      removeChar(rev, 0x0A);
      removeChar(rev, 0x0D);
   }

   return rev;
}

#endif  // end conditional compilation

/* FORMAT HASH ff860d7a43ec50233915d013c3fcd283 */

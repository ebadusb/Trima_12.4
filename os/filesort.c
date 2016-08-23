/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/os/rcs/filesort.c 1.2 2009/06/25 13:51:47Z sb07663 Exp rm70006 $
 * $Log: filesort.c $
 * Revision 1.2  2009/06/25 13:51:47Z  sb07663
 * Fixed stat call for proper sorting of files
 * Revision 1.1  2007/11/07 22:38:37Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:34:10Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:28:39Z  dslausb
 * Initial revision
 * Revision 1.1  2007/05/22 18:20:09Z  dslausb
 * Initial revision
 * Revision 1.2  2005/01/20 22:30:14Z  ms10234
 * - removed readdir_stat function
 * --- Added comments ---  pn02526 [2005/04/14 18:16:26Z]
 * - reverted to 1.2 to remove my changes.
 * Revision 1.1  2005/01/20 00:49:29Z  ms10234
 * Initial revision
 * Revision 1.4  2003/09/05 21:38:07Z  jl11312
 * - modified to use readdir_stat
 * Revision 1.3  2003/05/13 15:01:00Z  jl11312
 * - fileSort no longer performs final callback with NULL file name
 * Revision 1.2  2002/12/03 19:20:43Z  jl11312
 * - corrected logic for storing file names
 * Revision 1.1  2002/09/19 22:15:43  jl11312
 * Initial revision
 *
 */

#include <vxworks.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stat.h>
#include <string.h>



#include "filesort.h"

typedef struct
{
   size_t fileNameIndex;
   TIME   lastModifyTime;
} FileSortEntry;

static int fileCmpByDateAscending (const FileSortEntry* e1, const FileSortEntry* e2)
{
   int retVal;

   if ( e1->lastModifyTime > e2->lastModifyTime ) retVal = 1;
   else if ( e1->lastModifyTime < e2->lastModifyTime )
      retVal = -1;
   else retVal = 0;

   return retVal;
}

static int fileCmpByDateDescending (const FileSortEntry* e1, const FileSortEntry* e2)
{
   int retVal;

   if ( e1->lastModifyTime < e2->lastModifyTime ) retVal = 1;
   else if ( e1->lastModifyTime > e2->lastModifyTime )
      retVal = -1;
   else retVal = 0;

   return retVal;
}

static void buildFullPathName (char* result, const char* dirName, const char* fileName)
{
   strcpy(result, dirName);
   if ( dirName[strlen(dirName) - 1] != '/' )
   {
      strcat(result, "/");
   }

   strcat(result, fileName);
}

STATUS fileSort (const char* dirName, FileSortType sortType, fileSortCallBack* callBack)
{
   STATUS retVal  = ERROR;
   DIR*   sortDir = NULL;

   typedef int FileSortCmp (const void* e1, const void* e2);
   FileSortCmp* fileSortCmp;

   switch ( sortType )
   {
      case FILE_SORT_BY_DATE_ASCENDING :
         fileSortCmp = (FileSortCmp*)fileCmpByDateAscending;
         break;

      case FILE_SORT_BY_DATE_DESCENDING :
         fileSortCmp = (FileSortCmp*)fileCmpByDateDescending;
         break;

      default :
         fileSortCmp = NULL;
         break;
   }

   if ( fileSortCmp )
   {
      sortDir = opendir((char*)dirName);
   }

   if ( sortDir )
   {
      struct dirent*     dirEntry;
      struct stat        fileStat;
      char               fullPathName[NAME_MAX + 1];

      size_t             fileCount          = 0;
      size_t             fileListSize       = 512;
      FileSortEntry*     fileList           = malloc(fileListSize * sizeof(FileSortEntry));

      size_t             fileNameBufferSize = fileListSize * 32;
      size_t             fileNameBufferPos  = 0;
      char*              fileNameBuffer     = malloc(fileNameBufferSize * sizeof(char));

      FileCallBackStatus callBackStatus     = FILE_CALLBACK_CONTINUE;

      /*
       *	Scan directory
       */
      while ( (dirEntry = readdir(sortDir)) != NULL )
      {
         /*
          *	Skip . and .. directory entries
          */
         if ( strcmp(dirEntry->d_name, ".") != 0 &&
              strcmp(dirEntry->d_name, "..") != 0 )
         {
            int fileNameLen = strlen(dirEntry->d_name);

            if ( fileCount >= fileListSize )
            {
               /*
                *	Double file list size or increase by 4096 entries, whichever
                * is smaller.
                */
               fileListSize += (fileListSize < 4096) ? fileListSize : 4096;
               fileList      = realloc(fileList, fileListSize * sizeof(FileSortEntry));
            }

            if ( fileNameBufferPos + fileNameLen + 1 >= fileNameBufferSize )
            {
               /*
                *	Look at average file name size so far and increase buffer
                * size according to current file list size.
                */
               size_t avgFileNameLen = fileNameBufferPos / fileCount + 1;

               fileNameBufferSize = avgFileNameLen * fileListSize + fileNameLen + 1;
               fileNameBuffer     = realloc(fileNameBuffer, fileNameBufferSize * sizeof(char));
            }

            /*
             *	Get file information and add it to the file list
             */
            buildFullPathName(fullPathName, dirName, dirEntry->d_name);
            stat(fullPathName, &fileStat);
            strcpy(&fileNameBuffer[fileNameBufferPos], dirEntry->d_name);
            fileList[fileCount].fileNameIndex  = fileNameBufferPos;
            fileList[fileCount].lastModifyTime = fileStat.st_mtime;

            fileNameBufferPos                 += fileNameLen + 1;
            fileCount += 1;
         }
      }

      /*
       *	Send sorted list of files to call back function
       */
      if ( fileCount > 0 )
      {
         int file = 0;

         /*
          *	Sort file entries
          */
         qsort(fileList, fileCount, sizeof(FileSortEntry), fileSortCmp);
         while ( file < fileCount &&
                 callBackStatus == FILE_CALLBACK_CONTINUE )
         {
            buildFullPathName(fullPathName, dirName, &fileNameBuffer[fileList[file].fileNameIndex]);
            callBackStatus = (* callBack)(fullPathName);
            file          += 1;
         }
      }

      /*
       *	Clean up
       */
      closedir(sortDir);
      free(fileNameBuffer);
      free(fileList);
      retVal = OK;
   }

   return retVal;
}

/* FORMAT HASH 87b3775546e76912fe36eb58f749905e */

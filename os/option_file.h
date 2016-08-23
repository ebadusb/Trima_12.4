/*
 * Copyright (C) 2003 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/os/rcs/option_file.h 1.1 2007/11/07 22:38:42Z dslausb Exp $
 * $Log: option_file.h $
 * Revision 1.1  2007/11/07 22:38:42Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:34:13Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:30:17Z  dslausb
 * Initial revision
 * Revision 1.1  2007/05/22 18:20:14Z  dslausb
 * Initial revision
 * Revision 1.1  2003/06/26 22:35:21Z  jl11312
 * Initial revision
 *
 */

#ifndef _OPTION_FILE_INCLUDE
#define _OPTION_FILE_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

typedef struct OptionFileDataStruct
{
   char* name;
   char* setting;

   struct OptionFileDataStruct* next;
} OptionFileData;

STATUS readOptionFile (const char* fileName, const char* sectionName, OptionFileData** data, int crcRequired);
STATUS getOption (const char* optionName, const char** option, OptionFileData* data);

#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _OPTION_FILE_INCLUDE */

/* FORMAT HASH e2b5c5285b7137cd61df2801f85f3ffd */

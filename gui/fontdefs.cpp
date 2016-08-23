/*
 *  Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>
#include <ctype.h>
#include <ioLib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>

#include "fontdefs.h"
#include "filenames.h"
#include "trima_datalog.h"

/****************************************************************/
/* Enter the fonts available at init time into the table below.
* The included entries are examples ... please change them.
* The last entry in the table must be {0,0,0,0} as shown.
*
* name   == the name of the font, as in the argument to FS_set_font()
* path   == if a DISK font, the fully qualified pathname, 0 otherwise
* memptr == if a ROM/RAM font, the address of the font, 0 otherwise
* index  == the position of the font in a TrueType collection, 0 otherwise
*
* if the available fonts change dynamically, you can use the functions
* FS_add_font() and FS_delete_font() to change the available fonts list.
*/

MY_FONT my_fonts[MaxMyFonts] =
{
/*  {"name",                      "path",                                        memptr,    index }   */
   {"Lucida Sans BOLD",        FONT_DIRECTORY "/lsansd.ttf",               NULL,        0}, // Latin I Languages
#if CPU==SIMNT
   {"Albany AMT Custom",       FONT_DIRECTORY "/albanybd_custom.ttf",      NULL,        1},
   {"Albany AMT Bold",         FONT_DIRECTORY "/albwb.ttf",                NULL,        2},
   {"Sans 2312",               FONT_DIRECTORY "/san2312.ccc",              NULL,        3},
   {"Sans MT 932",             FONT_DIRECTORY "/san_932.stf",              NULL,        4},
#endif
   {NULL,                      NULL,                           NULL,        0},
};

void dumpFontTable (void)
{
   DataLog (log_level_gui_info) << "TableIndex; Name; Path; MemPtr; FontIndex" << endmsg
                                << "-----------------------------------------------------------------------" << endmsg;

   for (int i = 0; i<MaxMyFonts; i++)
      if (my_fonts[i].name && my_fonts[i].path)
         DataLog (log_level_gui_info) << "TableIndex " << i << ": " << my_fonts[i].name << "; "
                                      << my_fonts[i].path << "; " << my_fonts[i].memptr << "; " << my_fonts[i].index << endmsg;
}



const char* getFontFamilyName (const int fontId)
{
   if ((fontId > MaxMyFonts) || (fontId < 0))
   {
      DataLog(log_level_gui_info) << "Bad font id " << fontId << " used." << endmsg;
      dumpFontTable();
      // _FATAL_ERROR(__FILE__, __LINE__, "Bad font index");
      return NULL;
   }

   return my_fonts[fontId].name;
}

void initializeFontTable ()
{
   int  i;
   DIR* fontDataDirectory = NULL;
   char directoryPath[MAX_FILENAME_LENGTH];

   DataLog(log_level_gui_startup_debug) << "Entering initializeFontTable()" << endmsg;

   // Start loop at one since Lucida Sans has a zero index
   //  and already fills that position in the fontTable array.
   // Initialize table.
   for (i = 1; i < MaxMyFonts; i++)
   {
      my_fonts[i].name   = NULL;
      my_fonts[i].path   = NULL;
      my_fonts[i].memptr = NULL;
      my_fonts[i].index  = 0;
   }

   // Font CRCs are done in guimain, FYI.

   // Look for and read fontData.info files.
   strcpy(directoryPath, DROP_IN_FONTS_DIR);
   fontDataDirectory = opendir(directoryPath);

   if (fontDataDirectory == NULL)
      DataLog (log_level_gui_error) << "No add-on font data directory found " << directoryPath << endmsg;
   else
   {
      struct dirent* directoryData = NULL;

      while (( directoryData = readdir(fontDataDirectory)) != NULL)
      {
         if (strcmp(directoryData->d_name, ".") != 0 &&
             strcmp(directoryData->d_name, "..") != 0)
         {
            int         i;
            size_t      fileNameLength = strlen(directoryData->d_name);
            char        fileNameBuffer[MAX_FILENAME_LENGTH] = {0};

            const char* fileSuffixString                    = fileNameBuffer;

            if (fileNameLength > 14)  // must hold at least "_fontdata.info"
               fileSuffixString = &fileNameBuffer[ fileNameLength - 14 ];

            // Copy the file name to a temporary buffer
            //   and convert to lower case.
            //
            for (i = 0; i < fileNameLength; i++)
               fileNameBuffer[i] = tolower(directoryData->d_name[i]);

            fileNameBuffer[ fileNameLength ] = '\0';

            // Is this a "_fontdata.info file"?
            if (strcmp(fileSuffixString, "_fontdata.info") == 0)
            {
               //  Process fontdata file.
               //
               char fileName[MAX_FILENAME_LENGTH];
               memset(fileName, 0, MAX_FILENAME_LENGTH);

               // CRC check files.
               strcpy(fileName, directoryPath);
               strcat(fileName, "/");
               strcat(fileName, directoryData->d_name);

               enum { LineBufferSize = MAX_FILENAME_LENGTH * 2 };

               char lineBuffer[ LineBufferSize ];

               // Open fontdata file
               FILE* fp = fopen(fileName, "r");

               if (!fp)
               {
                  DataLog (log_level_gui_error) << "Unable to open fontdata file " << fileName << endmsg;
                  continue;
               }

               while (fgets(lineBuffer, LineBufferSize, fp) != NULL)
               {
                  if (lineBuffer[0] == '"')
                  {
                     char*   p     = NULL;
                     char*   token = NULL;
                     size_t  stringLength;

                     bool    fontAlreadyLoaded = false;

                     MY_FONT newFont;

                     newFont.memptr = 0;  // Always set to zero since font file is not in ROM/RAM memory. No need to update.
                     newFont.index  =  0; // Initialize this to something out of range

                     token          = strtok_r(lineBuffer, "\"", &p); // get font name, should match name in string.info file
                     newFont.name   = token;

                     token          = strtok_r(NULL, " \t\n\r", &p);
                     newFont.path   = token;

                     //
                     // Find the next available font array position...
                     //
                     for (newFont.index = 1;
                          ( (newFont.name != NULL)                   &&
                            (my_fonts[ newFont.index ].name != NULL) &&
                            (newFont.index < MaxMyFonts)
                          );
                          newFont.index++)
                     {
                        if (strcmp(my_fonts[newFont.index].name, newFont.name) == 0)
                        {
                           DataLog (log_level_gui_error) << "That font was already loaded" << endmsg;
                           fontAlreadyLoaded = true;
                        }
                     }

                     if (newFont.name                 &&
                         newFont.path                 &&
                         (newFont.index < MaxMyFonts) &&
                         !fontAlreadyLoaded)
                     {
                        memset(fileName, 0, MAX_FILENAME_LENGTH);

                        strcpy(fileName, DROP_IN_FONTS_DIR);
                        strcat(fileName, "/");
                        strcat(fileName, newFont.path);

                        stringLength                   = strlen(newFont.name);
                        my_fonts[ newFont.index ].name = (char*)malloc(stringLength + 1);
                        strcpy(my_fonts[ newFont.index ].name, newFont.name);

                        stringLength                   = strlen(fileName);

                        my_fonts[ newFont.index ].path = (char*)malloc(stringLength + 1);
                        sprintf(my_fonts[ newFont.index ].path, "%s", fileName);

                        my_fonts[ newFont.index ].memptr = newFont.memptr;
                        my_fonts[ newFont.index ].index  = newFont.index;

                        DataLog (log_level_gui_info) << "Added font \"" << my_fonts[ newFont.index ].name
                                                     << "\" with path " << my_fonts[ newFont.index ].path << " at index "
                                                     << my_fonts[ newFont.index ].index << endmsg;
                     }
                     else if (newFont.index >= MaxMyFonts && newFont.name)
                        DataLog (log_level_gui_error) << "my_fonts full. - " << newFont.name << endmsg;
                  }
               }

               fclose(fp);

            } /* Process "_fontdata.info" file. */
         }
      }     /*while directory entry */

      closedir(fontDataDirectory);
   }   /* if valid directory path */

   // dumpFontTable();
}

void loadFontFileToMemory (const char* fontName)
{
   bool somethingWasLoaded = false;

   if (fontName)
   {
      int index;

      for (index = 0;
           index < MaxMyFonts;
           index++)
      {
         if (!my_fonts[index].name)
            continue;

         if (!strcmp(my_fonts[index ].name, fontName))
            break;
      }

      if (index == MaxMyFonts)
      {
         DataLog (log_level_gui_error) << "Suitable font not found." << endmsg;
         dumpFontTable();
         _FATAL_ERROR(__FILE__, __LINE__, "Suitable font not found.");
         return;
      }

      if (my_fonts[ index ].memptr)
      {
         DataLog (log_level_gui_error) << "Font " << my_fonts[index ].name << " already exists." << endmsg;
         dumpFontTable();
         return;
      }

      if (index < MaxMyFonts && my_fonts[ index ].name && my_fonts[ index ].path)
      {
         struct stat statData;

         if (stat(my_fonts[ index ].path, &statData) == 0)
         {
            FILE* fp = fopen(my_fonts[ index ].path, "rb");

            if (fp)
            {
               DataLog (log_level_gui_info) << "Allocating memory chunk of size " << (int)statData.st_size
                                            << " bytes for font \"" << fontName << "\" (fontStructIndex " << index << ")." << endmsg;

               unsigned char* buffer = ( unsigned char* )malloc(statData.st_size);

               if (buffer && fread(buffer, sizeof( unsigned char ), statData.st_size, fp) == statData.st_size)
               {
                  my_fonts[ index ].memptr = buffer;
                  somethingWasLoaded       = true;
               }
               else
               {
                  free(buffer); buffer = 0;
                  DataLog (log_level_gui_error) << "fread failed for " << my_fonts[ index ].path << endmsg;
                  perror(my_fonts[ index ].path);
               }

               fclose(fp);
            }
            else
            {
               DataLog (log_level_gui_error) << "fopen failed for " << my_fonts[ index ].path << endmsg;
               perror(my_fonts[ index ].path);
            }
         }
         else
         {
            DataLog (log_level_gui_error) << "stat failed for " << my_fonts[index].path << endmsg;
            perror(my_fonts[index].path);
         }
      }
      else
      {
         DataLog (log_level_gui_error) << "Something is wrong with your font table." << endmsg;
         dumpFontTable();
         _FATAL_ERROR(__FILE__, __LINE__, "Stray pointer");
      }
   }
   else
   {
      DataLog (log_level_gui_error) << "Font name passed to loadFontFileToMemory is NULL" << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "Stray pointer");
   }

   if (!somethingWasLoaded)
   {
      DataLog (log_level_gui_error) << "Nothing was unloaded." << endmsg;
      dumpFontTable();
   }
}

void unloadFontFileFromMemory (int index)
{
   bool somethingWasUnloaded = false;

   if (index >= 0 && index < MaxMyFonts && my_fonts[ index ].memptr)
   {
      DataLog (log_level_gui_info) << "Freeing memory for fontStructIndex " << index << endmsg;
      free(my_fonts[ index ].memptr);
      my_fonts[ index ].memptr = 0;
      somethingWasUnloaded     = true;
   }

   if (!somethingWasUnloaded)
   {
      DataLog (log_level_gui_error) << "Nothing was unloaded." << endmsg;
      dumpFontTable();
   }
}

int getFontIndex (const char* fontName)
{
   int i;
   int returnIndex =  -1;

   if (fontName != NULL)
   {
      for (i = 0; i < MaxMyFonts; i++)
      {
         if (my_fonts[ i ].name != NULL &&
             !strcmp(fontName, my_fonts[i].name))
         {
            returnIndex = (int)my_fonts[i].index;
            break;
         }
      }

      if (returnIndex == -1)
         DataLog (log_level_gui_error) << "ERROR: unable to locate font \"" << fontName << "\"." << endmsg;
   }
   else
      DataLog (log_level_gui_error) << "ERROR: passed font name is NULL." << endmsg;

   return returnIndex;
}

int getFontIndexFromTableIndex (int tableIndex)
{
   if (!my_fonts[tableIndex].name || !my_fonts[tableIndex].path)
      return -1;

   return my_fonts[tableIndex].index;
}

const MY_FONT* getFontStruct (int tableIndex)
{
   if (!my_fonts[tableIndex].name || !my_fonts[tableIndex].path)
      return NULL;

   return &my_fonts[tableIndex];

}

/* FORMAT HASH 45d5aba25af59bcb51311b3997225ea8 */

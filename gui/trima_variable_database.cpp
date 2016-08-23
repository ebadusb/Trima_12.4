/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 *
 *
 */

#include <vxworks.h>

#include "trima_variable_database.h"
#include "filenames.h"
#include "cgui_pool_allocator.hpp"


TrimaVariableDatabase::VARIABLE_DICTIONARY_MAP TrimaVariableDatabase::_variableDictionary;


TrimaVariableDatabase::TrimaVariableDatabase()
{}

TrimaVariableDatabase::~TrimaVariableDatabase()
{
   _variableDictionary.clear();
}

const TrimaStringChar* TrimaVariableDatabase::variableLookUp (const string& name)
{
   const TrimaStringChar* result = NULL;
   TrimaVariableDatabase::VARIABLE_DICTIONARY_MAP_ITER iter;

   iter = _variableDictionary.find(name);

   if (iter!=_variableDictionary.end())
   {
      TrimaDataItem* dataItem = iter->second;

      result = dataItem->convertToString();
   }

   return result;
}

void TrimaVariableDatabase::addDataItem (const char* name, TrimaDataItem* dataItem)
{
   _variableDictionary[name] = dataItem;
}

void TrimaVariableDatabase::deleteDataItem (const char* name)
{
   _variableDictionary.erase(name);
}


TrimaDataItem* TrimaVariableDatabase::getDataItem (const char* name)
{
   TrimaDataItem* result = NULL;
   TrimaVariableDatabase::VARIABLE_DICTIONARY_MAP_ITER iter;

   iter = _variableDictionary.find(name);

   if (iter!=_variableDictionary.end())
   {
      result = iter->second;
   }

   return result;
}



bool TrimaVariableDatabase::read ()
{
   enum { LineBufferSize = 128 };

   char lineBuffer[LineBufferSize + 1];

   struct DataItemEntry
   {
      char* id;
      char* type;
      char* initialize;
      char* precision;
   };

   // The variable database uses TrimaTextItem which uses TrimaUnicodeString.
   // TrimaUnicodeString uses the GUI memory pool, so make sure it's initialized.
   cguiPoolAllocatorInitialize();

   bool  readingFileTable = false;
   bool  retVal           = true;
   int   line             = 0;
   char* p                = NULL;

   FILE* dataItemInfo     = fopen(DEFAULT_DATA_PATH, "r");

   if (dataItemInfo)
   {
      while (fgets(lineBuffer, LineBufferSize, dataItemInfo) != NULL)
      {
         line += 1;
         lineBuffer[LineBufferSize] = 0; // Make sure the buffer is null terminated

         //
         // Get first token. (space, tab, newline, return)
         char* firstToken = strtok_r(lineBuffer, " \t\n\r", &p);

         if (!firstToken || firstToken[0] == '#')
         {
            continue;
         }

         if (strcmp(firstToken, "DATA_ITEM_FILE_TABLE_START") == 0)
         {
            if (readingFileTable)
            {
               DataLog(log_level_gui_error) << "Error in file at line " << line << ": unexpected DATA_ITEM_FILE_TABLE_START - " << DEFAULT_DATA_PATH << endmsg;
               retVal = false;
            }

            readingFileTable = true;
            continue;
         }

         if (strcmp(firstToken, "DATA_ITEM_FILE_TABLE_END") == 0)
         {
            if (!readingFileTable)
            {
               DataLog(log_level_gui_error) << "Error in file at line " << line << ": unexpected DATA_ITEM_FILE_TABLE_END - " << DEFAULT_DATA_PATH << endmsg;
               retVal = false;
            }

            readingFileTable = false;
            continue;
         }

         if (readingFileTable)
         {
            DataItemEntry entry;

            //
            // Parse entry from file.
            //
            entry.id         = firstToken;
            entry.type       = strtok_r(NULL, " \t\n\r", &p);
            entry.initialize = strtok_r(NULL, " \t\n\r", &p);
            entry.precision  = NULL;

            if (entry.type)
            {
               if (strcmp(entry.type, "TextItem") == 0)
               {
                  // Declare variables for string - text item mapping.
                  if (entry.initialize)
                  {
                     TrimaTextItem* pTextItem = TrimaTextItem::getTextItem(entry.initialize);

                     if (pTextItem)
                     {
                        TrimaDataItemTextItem* dataItemTextItem = new TrimaDataItemTextItem(pTextItem);
                        addDataItem(entry.id, dataItemTextItem);
                     }
                     else
                     {
                        DataLog(log_level_gui_error) << "Error in file at line " << line << ": unknown Text Item DATA ITEM - " << DEFAULT_DATA_PATH << endmsg;
                        retVal = false;
                     }
                  }
                  else
                  {
                     DataLog(log_level_gui_error) << "Error in file at line " << line << ": cannot initialize Text Item DATA ITEM - " << DEFAULT_DATA_PATH << endmsg;
                     retVal = false;
                  }
               }
               else if (strcmp(entry.type, "Text") == 0)
               {
                  // Declare variables for string - text.
                  if (entry.initialize)
                  {
                     TrimaDataItemText* dataItemText = new TrimaDataItemText(entry.initialize);
                     addDataItem(entry.id, dataItemText);
                  }
                  else
                  {
                     DataLog(log_level_gui_error) << "Error in file at line " << line << ": bad entry in string info file - " << DEFAULT_DATA_PATH << endmsg;
                     retVal = false;
                  }
               }
               else if (strcmp(entry.type, "Double") == 0)
               {
                  entry.precision = strtok_r(NULL, " \t\n\r", &p);

                  if (entry.precision && entry.initialize)
                  {
                     TrimaDataItemDouble* dataItemDouble = new TrimaDataItemDouble(atof(entry.initialize), atoi(entry.precision));
                     addDataItem(entry.id, dataItemDouble);
                  }
                  else
                  {
                     DataLog(log_level_gui_error) << "Error in file at line " << line << ": bad entry in string info file - " << DEFAULT_DATA_PATH << endmsg;
                     retVal = false;
                  }
               }
               else if (strcmp(entry.type, "Integer") == 0)
               {
                  if (entry.initialize)
                  {
                     TrimaDataItemInteger* dataItemInteger = new TrimaDataItemInteger(atoi(entry.initialize));
                     addDataItem(entry.id, dataItemInteger);
                  }
                  else
                  {
                     DataLog(log_level_gui_error) << "Error in file at line " << line << ": bad entry in string info file - " << DEFAULT_DATA_PATH << endmsg;
                     retVal = false;
                  }
               }
               else if (strcmp(entry.type, "Clock") == 0)
               {
                  if (entry.initialize)
                  {
                     TrimaDataItemClock* dataItemClock = new TrimaDataItemClock(atoi(entry.initialize));
                     addDataItem(entry.id, dataItemClock);
                  }
                  else
                  {
                     DataLog(log_level_gui_error) << "Error in file at line " << line << ": unknown Clock DATA ITEM - " << DEFAULT_DATA_PATH << endmsg;
                     retVal = false;
                  }
               }
               else
               {
                  DataLog(log_level_gui_error) << "Error in file at line " << line << ": unknown DATA ITEM TYPE - " << DEFAULT_DATA_PATH << endmsg;
                  retVal = false;
               }
            }
            else
            {
               DataLog(log_level_gui_error) << "Error in file at line " << line << ": bad entry in string info file - " << DEFAULT_DATA_PATH << endmsg;
               retVal = false;
            }
         }
      }

      // Close file.
      fclose(dataItemInfo);
   }
   else
   {
      DataLog(log_level_gui_error) << "Could not open DATA ITEM file: " << DEFAULT_DATA_PATH << endmsg;
      retVal = false;
   }

   DataLog(log_level_gui_info) << "Trima Variable Dictionary size is " << _variableDictionary.size() << endmsg;

   return retVal;
}



void TrimaVariableDatabase::dumpDatabase ()
{
   DataLog(log_level_gui_info) << "Contents of Trima Variable Database" << endmsg;

   int i = 0;

   for (VARIABLE_DICTIONARY_MAP_ITER iter = _variableDictionary.begin();
        iter != _variableDictionary.end();
        ++iter)
   {
      const string&  name = iter->first;
      TrimaDataItem* item = iter->second;

      DataLog(log_level_gui_info) << "Entry: " << i++ << " " << name.c_str() << " value " << item->convertToString() << endmsg;
   }
}

/* FORMAT HASH 6a45bf7fe4292f4afcf28b8918885155 */

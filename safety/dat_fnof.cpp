/*
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      dat_fnof.cpp
 *
 * AUTHOR:     Tate Moore
 *
 * ABSTRACT:   This file implements the calibration reader for Beta 2.2
 *             For pump, this has been modified to eliminate fatal errors
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 */

#include <vxworks.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dat_fnof.hpp"
#include "trima_datalog.h"

// SPECIFICATION:    aDat_File_Reader_no_fault::aDat_File_Reader_no_fault()
//                      parameter None
//
//                      Constructor for the calibration reader.
//
//
aDat_File_Reader_no_fault::aDat_File_Reader_no_fault(const char* fileName)
{
   memset(buffer, 0, sizeof(buffer));    // clear the buffer

   int retryCount = 5;  // arbitrarily chosen

   while (retryCount > 0)
   {
      file = fopen(fileName, "r");

      if (!file)
      {
         DataLog(log_level_safe_exec_error) << "Unable to open " << fileName << ", System status: " << errnoMsg(errno) << endmsg;
         taskDelay (sysClkRateGet () / 2);      /* pause a 1/2 second */
      }
      else
      {
         DataLog(log_level_safe_exec_info) << fileName << " is opened." << endmsg;
         break;
      }

      retryCount--;
   }
}



// SPECIFICATION:    aDat_File_Reader_no_fault::~aDat_File_Reader_no_fault()
//                      parameter None
//
//                      Destructor for the calibration reader.
//
// ERROR HANDLING:  None
//
aDat_File_Reader_no_fault::~aDat_File_Reader_no_fault()
{
   if (file != NULL)
      fclose(file);
}



// SPECIFICATION:    aDat_File_Reader_no_fault::find_name( name, value )
//                      parameters:
//                         name = the name to find in this section.
//                         value = pointer into buffer where value is located.
//
//                      returns:
//                         TRUE = the name was found
//                         Error values otherwise.
//                         FALSE = if there is not a matching name
//
//
bool aDat_File_Reader_no_fault::find_name (const char* name)
{
   long  save_position;
   char* temp = NULL;
   char  temp_buffer[255];

   if (file != NULL)
   {
      // Save the current position in the file.
      save_position = ftell(file);

      // In this "correct" section check to see if there is a matching
      // name.
      while (fgets(temp_buffer, sizeof(temp_buffer), file) != NULL)
      {
         // If you made it to another '[' before finding the value
         // there is not a matching value.
         if (temp_buffer[0] != '[')
         {
            // Parse the name = value line.
            temp = strtok(temp_buffer, "=");

            if (temp != NULL)
            {
               // If the name matches....
               if (strcmp(temp, name) == 0)
               {
                  // Get the value.
                  temp = strtok(NULL, "=");

                  if (temp != NULL)
                  {
                     // Otherwise std::string copy the value into buffer for this
                     // object.
                     strcpy(buffer, temp);
                     return true;
                  }
                  // Bad parse of line
                  else
                  {
                     DataLog(log_level_safe_exec_info) << "value not found for " << name << endmsg;
                     return false;
                  }
               }
            }
            // Bad parse of line
            else
            {
               long fposition = ftell(file);
               DataLog(log_level_safe_exec_info) << "invalid name at position " << fposition << endmsg;
               return false;
            }
         }
         // Brace found no matching name in this section.
         else
         {
            DataLog(log_level_safe_exec_info) << "Name " << name << " not found in file." << endmsg;

            // Set the file back to the last read line.
            fseek(file, save_position, SEEK_SET);

            return false;
         }

         // Save current position in the file.
         save_position = ftell(file);
      }

      // Set the file back to the last read line.
      fseek(file, save_position, SEEK_SET);
   }

   return false;
}



// SPECIFICATION:    aDat_File_Reader_no_fault::find_section( section )
//                      parameters:
//                         section = the section to find.
//
//                      returns:
//                         TRUE = if the section is found
//                         Error values otherwise.
//                         FALSE = if there is not a matching section
//
//

bool aDat_File_Reader_no_fault::find_section (const char* section)
{
   char* temp = NULL;
   char  temp_buffer[255];

   // Read each line in the file
   while (file != NULL && fgets(temp_buffer, sizeof( temp_buffer), file) != NULL)
   {
      // See if this is the beginning of a section.
      // [SECTION_NAME]
      if (temp_buffer[0] == '[')
      {
         // Strip of the braces..
         temp = strtok(temp_buffer, "[]");

         if (temp != NULL)
         {
            // See if this is the correct section.
            if (strcmp(temp, section) == 0)
            {
               return true;
            }
         }
         // Bad parse of line.
         else
         {
            long fposition = ftell(file);
            DataLog(log_level_safe_exec_info) << "Bad section token at location " << fposition << endmsg;
            return false;
         }
      }
   }

   DataLog(log_level_safe_exec_info) << "Section " << section << " not found." << endmsg;

   return false;
}



// SPECIFICATION:    aDat_File_Reader_no_fault::find( section, name, return_value,
//                                               return_length )
//                      parameters:
//                         section = the section to find.
//                         name = the name to find.
//                         return_value = char array to place found value
//                         return_length = length of the return_value array
//
//                      returns:
//                         TRUE = if the section and name have a value
//                         Error values otherwise.
//                         FALSE = if there is not a matching section, name
//                         and value
//
//                      Reads a line of input from the calibration file
//                      and puts it in the buffer.
//
//
// *****************************************************************************
// The file format shold be the following,
//   with NO SPACES allowed....
//
// [SECTION_NAME0]
// name0=value0
// name1=value1
//
// [SECTION_NAME1]
// name0=value0
//
// [SECTION_NAME2]
// ...
//
// *****************************************************************************

bool aDat_File_Reader_no_fault::find (const char* section,
                                      const char* name,
                                      char* return_value,
                                      unsigned int return_length)
{
   // Check to see that all pointers are somewhat valid.
   if ( (file == NULL)         ||
        (return_value == NULL) ||
        (section == NULL)      ||
        (name == NULL)
        )
   {
      return false;
   }

   // Move to the start of the file.
   fseek(file, 0, SEEK_SET);

   // Search the file for the section specified.
   // find_section -> leaves the file position at the next line in the file.
   while (find_section(section))
   {
      // Inside the specified section search for the name.
      // ***** The file position is always left at the beginning of the line
      // ***** where the name was found or the next section or end of file.
      while (find_name(name))
      {
         // The value is actually in the private buffer 'buffer[]'
         if (strlen(buffer) >= return_length)
         {
            DataLog(log_level_safe_exec_info) << "invalid value length for field " << name << " in section " << section << endmsg;
            return false;
         }

         strncpy(return_value, buffer, return_length);
         return true;
      }
   }

   return false;
}



// SPECIFICATION:    aDat_File_Reader_no_fault::get_float( section, name,
//                                                low_value, high_value )
//                      parameters:
//                         section = the section to find.
//                         name = the name to find.
//                         low_value = the minimum value allowed for this float
//                         high_value = the maximum value allowed for this float
//
//                      returns:
//                         a floating point value between low and high value.
//
//
// *****************************************************************************
bool aDat_File_Reader_no_fault::get_float (const char* section,
                                           const char* name,
                                           float low_value,
                                           float high_value,
                                           float* found_value)
{
   char value[100];
   *found_value = 0.0f;                // float to store found values in.

   // Call the find to see if the section and name has a specified value.
   if (find(section, name, value, 99))
   {
      // Value is in the file, now see if the value is valid and between
      // the specified limits.
      *found_value = atof(value);

      if ( ( *found_value < low_value ) || ( *found_value > high_value ) )
      {
         DataLog(log_level_safe_exec_info) << "Field " << name << " in section "
                                           << section << " out of range: " << *found_value << " " << low_value
                                           << " " << high_value << endmsg;

         return false;
      }
   }
   else
   {
      return false;
   }

   return true;
}

/* FORMAT HASH 5985b4707e5d41ccdace80cd719158c2 */

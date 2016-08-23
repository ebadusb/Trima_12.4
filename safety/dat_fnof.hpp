/*
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      dat_fnof.hpp
 *
 * AUTHOR:     Tate Moore
 *
 * ABSTRACT:   This file implements the class definitions for the
 *             calibration reader for Beta 2.2
 *             For pump, this has been modified to eliminate fatal errors
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 */

#ifndef DAT_FNOF_HPP  //  Prevents multiple compilations.
#define DAT_FNOF_HPP

#include "s_type.h"

class aDat_File_Reader_no_fault
{
public:
   // Constructor
   aDat_File_Reader_no_fault(const char* fileName);

   // Destructor
   virtual ~aDat_File_Reader_no_fault();

   // Non-critical Methods.
   bool find (const char* section, const char* name,
              char* return_data, unsigned int return_length);

   bool get_float (const char* section, const char* name,
                   float low_value, float high_value, float* found_value);

private:
   // Methods
   bool find_section (const char* section);
   bool find_name (const char* name);

private:
   char  buffer[255];              // Buffer used to hold each line of the file.
   FILE* file;                     // File pointer for this object.
};

#endif

/* FORMAT HASH c7abada592d3c498d549efbbfffc16a4 */

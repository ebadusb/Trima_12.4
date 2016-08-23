/*
 * Copyright (C) 2012 Gambro BCT, Inc.  All rights reserved.
 *
 *  autocal.dat access class
 *
 * $Header: //depot/main/embedded/Trima/Main/trima_common_reentrant/autocaldat.cpp#1 $
 *
 */

#include <vxworks.h>
#include <usrLib.h>
#include <stdio.h>

#include "crcgen.h"
#include "filenames.h"
#include "trima_datalog_levels.h"
#include "caldat.h"

#include "autocaldat.h"

//
//  My singleton data instance
//
float AutoCalDat::_rawReference   = 0;
float AutoCalDat::_lastZeroOffset = 0;
bool  AutoCalDat::_dataOK         = false;

//
// -----------------------------------------------------------------------------
//                      Constructor, destructor
//
//
AutoCalDat::AutoCalDat()
{}

AutoCalDat::~AutoCalDat()
{}

//
// -----------------------------------------------------------------------------
//
void AutoCalDat::read ()
{
   FILE*         autocalFile = NULL;
   FILE*         autocalCRCFile = NULL;

   unsigned long crcFromData;
   unsigned long crcFromFile;

   float         autocalData[2]; // [0] = raw ref (from cal.dat), [1] = last zero offset

   autocalFile = fopen(AUTOCAL_FILE, "rb");

   if (autocalFile)
   {
      autocalCRCFile = fopen(AUTOCAL_FILE_CRC, "r");

      if (autocalCRCFile)
      {
         fscanf(autocalCRCFile, "%lx", &crcFromFile);
         fclose(autocalCRCFile);

         // Read the 2 floats from the file
         fread(autocalData, sizeof(autocalData), 2, autocalFile);

         // Check the CRC against the actual values
         crcFromData = INITCRC_DEFAULT;
         crcgen32(&crcFromData, (unsigned char*)autocalData, sizeof(autocalData));

         if (crcFromFile != crcFromData)
         {
            DataLog(log_level_datfile_error) << "CRC mismatch for autocal file \"" << AUTOCAL_FILE << "\"" << endmsg;
         }
         else if (autocalData[0] != CalDat::data().APSRawReference)
         {
            DataLog(log_level_datfile_error) << "Autocal file \"" << AUTOCAL_FILE << "\" invalid: cal.dat changed" << endmsg;
         }
         else
         {
            // Store them in the member variables
            _rawReference   = autocalData[0];
            _lastZeroOffset = autocalData[1];

            _dataOK         = true;
         }
      }
      else
      {
         DataLog(log_level_datfile_error) << "Error opening autocal CRC file \"" << AUTOCAL_FILE_CRC << "\" : " << errnoMsg << endmsg;
      }

      fclose(autocalFile);
   }
   else
   {
      DataLog(log_level_datfile_error) << "Error opening autocal file \"" << AUTOCAL_FILE << "\" : " << errnoMsg << endmsg;
   }
}



//
// update
//
bool AutoCalDat::update (float apsRaw, float apsZeroOffset)
{
   DataLog(log_level_datfile_info) << "Saving autocal file..." << endmsg;

   attrib(AUTOCAL_FILE, "-R");
   attrib(AUTOCAL_FILE_CRC, "-R");

   float autocalData[2] = { apsRaw, apsZeroOffset};

   FILE* fp             = fopen(AUTOCAL_FILE, "wb");

   if (fp)
   {
      fwrite(autocalData, sizeof(float), 2, fp);
      fclose(fp);
   }
   else
   {
      DataLog(log_level_datfile_error) << "Unable to open autocal file: " << AUTOCAL_FILE << endmsg;
      return false;
   }

   unsigned long crcFromData = INITCRC_DEFAULT;
   crcgen32(&crcFromData, (unsigned char*)autocalData, 2 * sizeof(float));

   fp = fopen(AUTOCAL_FILE_CRC, "w");

   if (fp)
   {
      fprintf(fp, "%lx", crcFromData);
      fclose(fp);
   }
   else
   {
      DataLog(log_level_datfile_error) << "Unable to open autocal crc file: " << AUTOCAL_FILE_CRC << endmsg;
      return false;
   }

   attrib(AUTOCAL_FILE, "+R");
   attrib(AUTOCAL_FILE_CRC, "+R");

   DataLog(log_level_datfile_info) << "autocal file saved" << endmsg;

   return true;
}

/* FORMAT HASH 0e76ab60b4ef3b4a961aca0d6f15ab21 */

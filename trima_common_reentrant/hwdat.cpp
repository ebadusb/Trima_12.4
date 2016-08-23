#include <vxworks.h>
#include <stdio.h>

#include "hwdat.h"
#include "datfile.h"
#include "error.h"
#include "filenames.h"
#include "trima_datalog.h"

static const char PATHNAME[] = PNAME_HWDAT; // the one and only

//
// -----------------------------------------------------------------------------
//                      Constructor, destructor
//
//
HWDat::HWDat()
{}

HWDat::~HWDat()
{}

//
// -----------------------------------------------------------------------------
//
//   Return a constant reference to my data
//
const HWDat::HWDatStruct& HWDat::data ()
{
   return _data;
}


//
// -----------------------------------------------------------------------------
//
ALARM_VALUES HWDat::initialize (bool logError)
{
   ALARM_VALUES status = NULL_ALARM; // default

   //
   //
   // Create the dat file reader to retrieve the Cobe configuration data.
   //
   CDatFileReader config;
   if (config.Initialize(PATHNAME, logError) != DFR_OK)
   {
      return HW_DAT_BAD_FILE;
   }

   // [TEMPERATURE]
   _data.MaximumBasinTemperature = config.get_float("TEMPERATURE", "maximum_basin_temperature", 0.0, 50.0);

   //
   // Get the valid HW CRC values for control and safety
   //
   CDatFileReader::FindIter iter;
   const char*              crcString = config.FindFirst("HW_CRC", "control", iter);
   while ( crcString )
   {
      unsigned int crc;

      if ( sscanf(crcString, "%x", &crc) == 1 )
      {
         _data.ValidControlHWCRC.push_back(crc);
      }

      crcString = config.FindNext(iter);
   }

   crcString = config.FindFirst("HW_CRC", "safety", iter);
   while ( crcString )
   {
      unsigned int crc;

      if ( sscanf(crcString, "%x", &crc) == 1 )
      {
         _data.ValidSafetyHWCRC.push_back(crc);
      }

      crcString = config.FindNext(iter);
   }

   //
   //  Final status check to ensure that all values were read in properly.
   //
   DFR_ERROR configStatus = config.Error();
   if (configStatus == DFR_NOTFOUND)
   {
      status = HW_DAT_VALUE_NOT_FOUND;
   }
   else if (configStatus == DFR_VALUE)
   {
      status = HW_DAT_VALUE_OUT_OF_RANGE;
   }
   else if (configStatus == DFR_PARSE)
   {
      status = HW_DAT_PARSE_ERROR;
   }
   return status;
}

int HWDat::VerifyControlHWCRC (unsigned long crc)
{
   int retVal = 0;

   list<unsigned long>::iterator entry;
   for ( entry = _data.ValidControlHWCRC.begin();
         entry != _data.ValidControlHWCRC.end();
         entry++ )
   {
      if ( crc == *entry )
      {
         retVal = 1;
         break;
      }
   }

   return retVal;
}

int HWDat::VerifySafetyHWCRC (unsigned long crc)
{
   int retVal = 0;

   list<unsigned long>::iterator entry;
   for ( entry = _data.ValidSafetyHWCRC.begin();
         entry != _data.ValidSafetyHWCRC.end();
         entry++ )
   {
      if ( crc == *entry )
      {
         retVal = 1;
         break;
      }
   }

   return retVal;
}

void HWDat::LogValidControlHWCRC (void)
{
   list<unsigned long>::iterator entry;
   for ( entry = _data.ValidControlHWCRC.begin();
         entry != _data.ValidControlHWCRC.end();
         entry++ )
   {
      DataLog(log_level_datfile_info) << "Valid control HW CRC (from " << PATHNAME << ")= " << hex << *entry << dec << endmsg;
   }
}

void HWDat::LogValidSafetyHWCRC (void)
{
   list<unsigned long>::iterator entry;
   for ( entry = _data.ValidSafetyHWCRC.begin();
         entry != _data.ValidSafetyHWCRC.end();
         entry++ )
   {
      DataLog(log_level_datfile_info) << "Valid safety HW CRC (from " << PATHNAME << ")= " << hex << *entry << dec << endmsg;
   }
}

//
//
//  My singleton data instance
//
HWDat::HWDatStruct HWDat::_data;

/* FORMAT HASH 8092ebabd94b28484bdff1120be45fdc */

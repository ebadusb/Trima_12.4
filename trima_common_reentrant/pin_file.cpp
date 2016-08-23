#include <fstream>
#include <usrLib.h>

#include "pin_file.h"

#include "datalog_periodic.h"

#include "trima_datalog.h"    // Datalog stuff

#include "filenames.h"        // Get Trima filename locations.
#include "alarms.h"           // Get alarm defns
#include "an_alarm.h"         // Get proc alarm defn.


static PIN_LIST_TYPE _pinList;


DataLog_SetHandle PinDat::_set     = 0;
bool              PinDat::_logPins = false;


//
// Base class definitions
//
PinDat::PinDat()
{}


PinDat::~PinDat()
{}



//
// begin
//
PIN_LIST_ITERATOR PinDat::begin ()
{
   return _pinList.begin();
}



//
// end
//
PIN_LIST_ITERATOR PinDat::end ()
{
   return _pinList.end();
}



//
// erase
//
void PinDat::erase (const PIN_LIST_ITERATOR& it)
{
   _pinList.erase(it);
}



//
// size
//
unsigned int PinDat::size ()
{
   return _pinList.size();
}



//
// clear
//
void PinDat::clear ()
{
   _pinList.clear();
}



//
// addPin
//
void PinDat::addPin (const char* badge, const char* pin)
{
   _pinList[badge] = pin;
}



//
// find
//
PIN_LIST_ITERATOR PinDat::find (const char* badge)
{
   return _pinList.find(badge);
}



//
// read
//
void PinDat::read ()
{
   unsigned long fileCrc;
   unsigned int  numberOfEntries = 0;

   char          badge[81];
   char          pin[7];

   // Create the PIN file dlog set used for writing the file to the drive
   datalog_CreatePeriodicSet("pin_file", &_set);
   datalog_SetPeriodicOutputInterval(_set, 0);
   datalog_AddFunc(_set, writeDataToFile, "PINwriteDataToFile", "PIN Data written", "%d");

   //
   // Open the PIN CRC file
   //
   ifstream crcFile;
   crcFile.open(BIO_PIN_CRC_FILE, ios::in | ios::binary);

   if (!crcFile.good())
   {
      DataLog(log_level_datfile_error) << "File " << BIO_PIN_CRC_FILE << " doesn't exist." << endmsg;
      return;
   }

   // get the crc for the pin file
   crcFile >> fileCrc;

   crcFile.close();

   //
   // Compare the CRC's
   //
   unsigned long calculatedCrc = 0;

   if (file_crcgen32(BIO_PIN_FILE, &calculatedCrc) == -1)
   {
      DataLog(log_level_datfile_error) << "File " << BIO_PIN_FILE << " doesn't exist." << endmsg;
      return;
   }
   else if (calculatedCrc != fileCrc)
   {
      DataLog(log_level_datfile_error) << "Calculated CRC(" << hex << calculatedCrc
                                       << ") doesn't match file value of " << fileCrc << dec << ".  File not read." << endmsg;
      return;
   }

   //
   // Open Data file
   //
   ifstream inFile;
   inFile.open(BIO_PIN_FILE, ios::in | ios::binary);

   if (!inFile.good())
   {
      DataLog(log_level_datfile_error) << "File " << BIO_PIN_FILE << " doesn't exist." << endmsg;
      return;
   }

   // Read file and populate _pinList;
   for (;; )
   {
      bool badEntry    = true;
      int  badgeLength = 0;
      int  pinLength   = 0;
      badge[0] = '\0';
      pin[0]   = '\0';

      // Grab entries
      while (true)
      {
         inFile.read(&badgeLength, sizeof(badgeLength));

         if ((badgeLength == 0) || (badgeLength > 80))
         {
            break;
         }

         inFile.read(&badge, badgeLength);
         inFile.read(&pinLength, sizeof(pinLength));

         if ((pinLength == 0) || (pinLength > 6))
         {
            break;
         }

         inFile.read(&pin, pinLength);

         ++numberOfEntries;

         badEntry = false;
         break;
      }

      if (inFile.eof())
      {
         break;
      }
      else if (!inFile.good())
      {
         // Log error
         DataLog(log_level_datfile_error) << "Unexpected I/O error reading " << BIO_PIN_FILE << ".  Value " << inFile.rdstate() << endmsg;
         break;
      }

      if (badEntry)
      {
         // Report error.
         DataLog(log_level_datfile_error) << "bad entry in " << BIO_PIN_FILE
                                          << ".  Item # " << numberOfEntries << ", badge(" << badge << ")(" << badgeLength
                                          << "), pin(" << pin << ")(" << pinLength << ")." << endmsg;
      }
      else
      {
         // Terminate the strings.
         badge[badgeLength] = 0;
         pin[pinLength]     = 0;

         if (_logPins)
         {
            // Log good entry
            DataLog(log_level_datfile_info) << BIO_PIN_FILE
                                            << ".  Item # " << numberOfEntries << ", badge(" << badge << ")(" << badgeLength
                                            << "), pin(" << pin << ")(" << pinLength << ")." << endmsg;
         }

         // Insert the item
         _pinList[badge] = pin;
      }
   }

   inFile.close();

   DataLog(log_level_datfile_info) << numberOfEntries << " PINs added.  PIN List size is " << _pinList.size() << endmsg;
}



//
// update
//
void PinDat::update ()
{
   // Update the dat files
   datalog_ForcePeriodicOutput(_set);
}



//
// writeDataToFile
//
bool PinDat::writeDataToFile ()
{
   DataLog(log_level_datfile_info) << "Writing " << BIO_PIN_FILE << " data..." << endmsg;

   // First, delete the CRC file to invalidate the PIN FILE.
   remove(BIO_PIN_CRC_FILE);

   // Next, create the PIN file.
   attrib(BIO_PIN_FILE, "-R");
   ofstream outFile;
   outFile.open(BIO_PIN_FILE, ios::out | ios::trunc | ios::binary, 0777);

   if (!outFile.good())
   {
      DataLog(log_level_datfile_error) << "File " << BIO_PIN_FILE << " couldn't be opened[" << outFile.rdstate() << "]." << endmsg;
      return false;
   }

   for (PIN_LIST_TYPE::iterator iter = _pinList.begin(); iter != _pinList.end(); ++iter)
   {
      const string badge       = iter->first;
      const string pin         = iter->second;

      const int    badgeLength = badge.size();
      const int    pinLength   = pin.size();

      outFile.write(&badgeLength, sizeof(badgeLength));
      outFile.write(badge.c_str(), badgeLength);
      outFile.write(&pinLength, sizeof(pinLength));
      outFile.write(pin.c_str(), pinLength);

      if (_logPins)
      {
         DataLog(log_level_datfile_info) << "Wrote Badge[" << badge << "](" << badgeLength
                                         << "), PIN[" << pin << "](" << pinLength << ")." << endmsg;
      }
   }

   outFile.close();
   attrib(BIO_PIN_FILE, "+R");

   DataLog(log_level_datfile_info) << "File " << BIO_PIN_FILE << " updated successfully with " << _pinList.size() << " entries." << endmsg;

   // Compute the CRC for the PIN file.
   unsigned long calculatedCrc = 0;

   if (file_crcgen32(BIO_PIN_FILE, &calculatedCrc) == -1)
   {
      DataLog(log_level_datfile_error) << "CRC couldn't be generated for " << BIO_PIN_FILE << ".  Operation failed." << endmsg;

      return false;
   }

   attrib(BIO_PIN_CRC_FILE, "-R");

   // Store the computed CRC in the crc file
   ofstream crcFile;
   crcFile.open(BIO_PIN_CRC_FILE, ios::out | ios::trunc, 0777);

   if (!crcFile.good())
   {
      DataLog(log_level_datfile_error) << "File " << BIO_PIN_CRC_FILE << " couldn't be opened." << endmsg;
      return false;
   }

   crcFile << calculatedCrc;

   crcFile.close();

   attrib(BIO_PIN_CRC_FILE, "+R");

   DataLog(log_level_datfile_info) << "CRC (0x" << hex << calculatedCrc << dec << ") stored in " << BIO_PIN_CRC_FILE << "." << endmsg;

   DataLog(log_level_datfile_info) << BIO_PIN_CRC_FILE << " data updated" << endmsg;

   return true;
}

/* FORMAT HASH dea8e5b15845b41e6c40b843e12ca023 */

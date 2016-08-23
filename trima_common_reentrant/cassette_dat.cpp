#include <fstream>
#include <map>
#include <usrLib.h>
#include <stdlib.h>


#include "cassette_dat.h"

#include "filenames.h"        // Get Trima filename locations.
#include "trima_datalog.h"    // Datalog stuff
#include "an_alarm.h"         // Get proc alarm defn.
#include "CatalogChecker.h"   // Check CRC on catalog numbers.


#define MAX_NUM_LINES 400


// THE MASTER LIST OF CASSETTES
static CASSETTE_MAP_TYPE _cassetteMap;

// THE ADMIN CASSETTE LIST ( IT'S A VECTOR TO PRESERVE ORDER ).
static CASSETTE_VECTOR_TYPE _adminCassettes;

//
// Base class definitions
//
CassetteDat::CassetteDat()
{}


CassetteDat::~CassetteDat()
{}


void CassetteDat::fileError (const char* msg, ALARM_VALUES alrmVal)
{
   anAlarmMsg alarm(alrmVal);
   alarm.setAlarm(msg);
}



//
// verifies that the main / backup file match
//
bool CassetteDat::checkFile (const char* datFilePath, const char* crcFilePath, const char* datFileBkPath, const char* crcFileBkPath)
{
   bool bMainOk  = false;
   bool bBackOk  = false;
   bool bSuccess = false;

   // read in the main file and check CRC
   CFileHelper fhelper;
   bMainOk = fhelper.Read (datFilePath);

   if (bMainOk)
   {
      bMainOk = fhelper.CompareCRC (crcFilePath);
   }

   // read in the backup file and check CRC
   CFileHelper fhelperBk;
   bBackOk = fhelperBk.Read (datFileBkPath);

   if (bBackOk)
   {
      bBackOk = fhelperBk.CompareCRC (crcFileBkPath);
   }

   // There are four cases to deal with
   const int nCase = (bMainOk ? 1 : 0) + (bBackOk ? 2 : 0);

   switch (nCase)
   {
      case 1 : // Main file Ok, backup is bad.
      {
         // we need to update the backup file with the main one.
         DataLog(log_level_datfile_info) << "Attempting to fix backup file " << datFileBkPath << endmsg;
         bSuccess = fixFile(fhelper, datFileBkPath, crcFileBkPath);
         break;
      }

      case 2 : // Backup file is good, main file is bad.
      {
         // we need to update the main file with the backup one.
         DataLog(log_level_datfile_info) << "Attempting to fix main file " << datFilePath << endmsg;
         bSuccess = fixFile(fhelperBk, datFilePath, crcFilePath);
         break;
      }

      case 3 : // Both files are good, make sure they are the same.
      {
         if (fhelper.CRC() != fhelperBk.CRC())
         {
            // assume the main one is the correct one.
            DataLog(log_level_datfile_info) << "Config files ok, but don't match.  Replacing backup with " << datFilePath << endmsg;
            bSuccess = fixFile(fhelper, datFileBkPath, crcFileBkPath);
         }
         else
         {
            // both match and are fine
            DataLog(log_level_datfile_info) << "Backup and crc are ok for both " << datFilePath << " and its backup." << endmsg;
            bSuccess = true;
            break;
         }
         break;
      }

      default : // neither file is good, $%#*$#@!*
      {
         DataLog(log_level_datfile_info) << "Neither " << datFilePath << " or its backup is ok." << endmsg;
         bSuccess = false;
         break;
      }
   }

   return bSuccess;
}



//
// attempts to fix a backup file with a good file including crc
//
bool CassetteDat::fixFile (CFileHelper& goodFile, const char* fname, const char* crcfname)
{
   bool bSuccess = true;
   char crcTxt[15];

   // obtain text copy of the crc
   CFileHelper fCRC;
   goodFile.CRC(crcTxt);

   // put it into the empty crc file
   fCRC.Append (crcTxt);
   fCRC.Append ("\n");

   // write file to backup location and crc to backup file
   bSuccess = goodFile.FastWrite (fname) && fCRC.FastWrite (crcfname, false);

   return bSuccess;
}




//
// Master Cassette.dat methods
//
bool MasterCassetteDat::_fileOK          = false;
char MasterCassetteDat::_fileVersion[80] = {};

MasterCassetteDat::MasterCassetteDat()
{}



MasterCassetteDat::~MasterCassetteDat()
{}



//
// begin
//
CASSETTE_MAP_ITERATOR MasterCassetteDat::begin ()
{
   return _cassetteMap.begin();
}



//
// end
//
CASSETTE_MAP_ITERATOR MasterCassetteDat::end ()
{
   return _cassetteMap.end();
}



//
// find
//
CASSETTE_MAP_ITERATOR MasterCassetteDat::find (const char* cassetteIdString)
{
   return _cassetteMap.find(cassetteIdString);
}



//
// read
//
void MasterCassetteDat::read ()
{
   int          numberOfEntries = 0, lineNumber = 0;
   unsigned int catNum          = 0, adminCode = 0;

   char*        token           = 0;
   char*        barcodeString   = 0;
   char*        catNumString    = 0;
   char         buffer[256];
   char         line[256];

   // Match the CRC calculated from cassette.dat with the CRC value stored in cassette.crc.
   if (!checkFile(PNAME_CASSETTEDAT, PNAME_CASSETTEDAT_CRC, PNAME_CASSETTEDAT_BK, PNAME_CASSETTEDAT_CRC_BK))
   {
      DataLog(log_level_datfile_error) << "Error matching cassette file CRC." << endmsg;
      fileError("Error matching cassette.dat CRC.", CASSETTE_DAT_FILE_BAD_CRC);
      _fileOK = false;
      return;
   }

   ifstream inFile;
   inFile.open(PNAME_CASSETTEDAT, ios::in | ios::binary);

   if (!inFile.good())
   {
      DataLog (log_level_datfile_error) << "File " << PNAME_CASSETTEDAT << " doesn't exist." << endmsg;
      inFile.close();
      fileError("Unable to open file cassette.dat", CASSETTE_DAT_FILE_OPEN_ERROR);
      _fileOK = false;
      return;
   }

   while (!inFile.eof())
   {
      lineNumber++;

      if (lineNumber > MAX_NUM_LINES) // Make sure the read loop terminates.
      {
         inFile.close();
         fileError("cassette.dat read loop wouldn't terminate.", CASSETTE_DAT_FILE_PARSE_ERROR);
         _fileOK = false;
         return;
      }

      if (!inFile.good())
      {
         // Log error
         DataLog (log_level_datfile_error) << "Unexpected I/O error reading " << PNAME_CASSETTEDAT << ".  Value " << inFile.rdstate() << ".  Line number " << lineNumber << endmsg;
         inFile.close();
         fileError("unexpected cassette.dat I/O error", CASSETTE_DAT_FILE_PARSE_ERROR);
         _fileOK = false;
         return;
      }

      char* nextToken = 0;

      inFile.getline(buffer, 256);

      // Ignore comments, file version line, and blank lines.
      if (!inFile.good())
         continue;
      else if ((buffer[0] == '#') || (buffer[0] == 13)  || (buffer[0] == 10)  || (buffer[0] == 0)) // # or CR/LF
         continue;
      else if (strstr(buffer, "file_version") != NULL)
      {
         DataLog(log_level_datfile_info) << "File " << PNAME_CASSETTEDAT << buffer << endmsg;
         continue;
      }

      strcpy(line, buffer);  // Copy line to print out in case of error.

      // Catalog number
      token = strtok_r(buffer, "\t ", &nextToken);

      if (token == 0)
      {
         DataLog(log_level_datfile_error) << "Bad Token: " << token << " line read was[" << buffer << "] at line number " << lineNumber << endmsg;
         fileError("Bad line in cassette.dat file", CASSETTE_DAT_FILE_INVALID_ENTRY);
         _fileOK = false;
         return;
      }

      catNumString = token;
      catNum       = atoi(token);

      // Admin code
      token = strtok_r(NULL, "\t ", &nextToken);

      if (token == 0)
      {
         DataLog(log_level_datfile_error) << "Bad Token: " << token << " in line " << line << " at line number " << lineNumber << endmsg;
         fileError("Bad line in cassette.dat file", CASSETTE_DAT_FILE_INVALID_ENTRY);
         _fileOK = false;
         return;
      }

      adminCode = (unsigned int)strtoul(token, NULL, 10);

      // Barcode string
      token = strtok_r(NULL, "\t\r\n ", &nextToken);

      if (token == 0)
      {
         DataLog(log_level_datfile_error) << "Bad Token: " << token << " in line " << line << " at line number " << lineNumber << endmsg;
         fileError("Bad line in cassette.dat file", CASSETTE_DAT_FILE_INVALID_ENTRY);
         _fileOK = false;
         return;
      }

      barcodeString = token;

      DataLog (log_level_datfile_info) << "REF (" << catNum
                                       << ") + admin code (" << adminCode << ") + barcode ("
                                       << barcodeString << ") --> ";

      CatalogChecker catalogChecker(catNum, adminCode);

      // If the admin code does not match the REF, complain and quit.
      if (!catalogChecker.isValid())
      {
         DataLog (log_level_datfile_info) << "Invalid" << endmsg;
         fileError("REF does not match admin code.", CASSETTE_DAT_FILE_INVALID_ENTRY);
         _fileOK = false;
         return;
      }

      // If a barcode exists, and it does not contain the REF number as a substring,
      // then we should reject it and quit, because that's not cool with us. (See ITs 7754 and 7776)
      if ((strcmp(barcodeString, "0") != 0) &&
          (strstr(barcodeString, catNumString) == NULL))
      {
         DataLog (log_level_datfile_info) << "Invalid" << endmsg;

         DataLog (log_level_datfile_error) << "Listed barcode does not match REF number.  Barcode("
                                           << barcodeString << "), REF(" << catNumString << ")" << endmsg;

         fileError("Error matching barcode with REF number.", CASSETTE_DAT_FILE_INVALID_ENTRY);
         _fileOK = false;

         return;
      }

      DataLog (log_level_datfile_info) << "Valid (Stamp: "
                                       << (int)catalogChecker.checkBit(CatalogChecker::STAMP)
                                       << ", PAS:" << (int)catalogChecker.checkBit(CatalogChecker::MSS_PLT)
                                       << ", RAS:" << (int)catalogChecker.checkBit(CatalogChecker::MSS_RBC)
                                       << ")" << endmsg;

      CatalogNumber::STAMP_COLOR stampColor = (CatalogNumber::STAMP_COLOR)catalogChecker.checkBit(CatalogChecker::STAMP);

      bool isMssPlt = catalogChecker.checkBit(CatalogChecker::MSS_PLT);

      _cassetteMap[catNumString] = new CatalogNumber(catNumString, adminCode, barcodeString);

      numberOfEntries++;
   }

   inFile.close();
   DataLog(log_level_datfile_info) << numberOfEntries << " cassettes added to master cassette table." << endmsg;

   _fileOK = true;
}





//
// clear
//
void MasterCassetteDat::clear ()
{
   // Delete all items in the cassette map structure
   for (CASSETTE_MAP_ITERATOR it = _cassetteMap.begin(); it != _cassetteMap.end(); ++it)
   {
      delete it->second;
   }

   _cassetteMap.clear();

   _fileOK         = false;

   _fileVersion[0] = 0;
}



//
// Admin Cassette.dat methods
//
bool AdminCassetteDat::_fileOK          = false;
char AdminCassetteDat::_fileVersion[80] = {};
bool AdminCassetteDat::_fileHasChanged = false;


AdminCassetteDat::AdminCassetteDat()
{}



AdminCassetteDat::~AdminCassetteDat()
{}


//
// begin
//
CASSETTE_VECTOR_ITERATOR AdminCassetteDat::begin ()
{
   return _adminCassettes.begin();
}



//
// end
//
CASSETTE_VECTOR_ITERATOR AdminCassetteDat::end ()
{
   return _adminCassettes.end();
}



//
// erase
//
void AdminCassetteDat::erase (CASSETTE_VECTOR_ITERATOR it)
{
   AdminCassetteDat::_fileHasChanged = true;
   _adminCassettes.erase(it);
}



//
// clear
//
void AdminCassetteDat::clear ()
{
   AdminCassetteDat::_fileHasChanged = true;

   // Delete all items in the admin cassettes structure
   for (CASSETTE_VECTOR_ITERATOR it = _adminCassettes.begin(); it != _adminCassettes.end(); ++it)
   {
      delete *it;
   }

   _adminCassettes.clear();
}



//
//  addCassette
//
void AdminCassetteDat::addCassette (const char* refNumber, unsigned int tubingSetCode, const char* barcode)
{
   AdminCassetteDat::_fileHasChanged = true;
   _adminCassettes.push_back(new CatalogNumber(refNumber, tubingSetCode, barcode));
}



//
// size
//
unsigned int AdminCassetteDat::size (void)
{
   return _adminCassettes.size();
}



//
// read
//
void AdminCassetteDat::read ()
{
   ifstream     inFile;
   char         catNumString[100]  = "";
   char         barcodeString[100] = "";
   unsigned int catNum             = 0, adminCode = 0;

   // Match up the CRCs
   if (!checkFile(PNAME_SETCONFIGDAT, PNAME_SETCONFIG_CRC, PNAME_SETCONFIGDAT_BK, PNAME_SETCONFIG_CRC_BK))
   {
      DataLog(log_level_datfile_error) << "Error matching cassette file CRC." << endmsg;
      fileError("Error matching setconfig.dat CRC.", SETCONFIG_DAT_FILE_BAD_CRC);
      _fileOK = false;
      return;
   }

   inFile.open(PNAME_SETCONFIGDAT, ios::in | ios::binary);

   if (!inFile.good())
   {
      DataLog (log_level_datfile_error) << "File " << PNAME_SETCONFIGDAT << " doesn't exist." << endmsg;
      inFile.close();
      fileError("Unable to open file setconfig.dat", SETCONFIG_DAT_FILE_OPEN_ERROR);
      _fileOK = false;
      return;
   }

   int index     = 0;
   int iteration = 0;
   while (!inFile.eof() && index < MAX_CATALOG_ITEMS)
   {
      if (!inFile.good())
      {
         // Log error
         DataLog (log_level_datfile_error) << "Unexpected I/O error reading " << PNAME_SETCONFIGDAT << ".  Value " << inFile.rdstate() << endmsg;
         inFile.close();
         fileError("unexpected setconfig.dat I/O error", SETCONFIG_DAT_FILE_PARSE_ERROR);
         _fileOK = false;

         return;
      }

      char buffer[256];

      inFile.getline(buffer, 256);

      // Ignore comments, file version line, and blank lines.
      if (buffer[0] == '#'
          || strcmp(buffer, "") == 0
          ) continue;

      char* nextToken = 0;

      if (strstr(buffer, "file_version") != NULL)
      {
         bzero(_fileVersion, sizeof(_fileVersion));

         strtok_r(buffer, "=", &nextToken); // chop off the first chunk of "buffer".
         sprintf(_fileVersion, strtok_r(NULL, "\t\r\n ", &nextToken) );

         if (!_fileVersion || strcmp(_fileVersion, "") == 0)
         {
            DataLog (log_level_datfile_error) << "File Version was empty." << endmsg;
            bzero(_fileVersion, sizeof(_fileVersion));
         }
         else
         {
            DataLog (log_level_datfile_info) << "File Version: " << _fileVersion << endmsg;
         }

         continue;
      }

      sprintf(catNumString, strtok_r(buffer, "\t ", &nextToken) );
      catNum    = atoi(catNumString);
      adminCode = (unsigned int)strtoul( (strtok_r(NULL, "\t ", &nextToken)), NULL, 10);
      sprintf(barcodeString,  strtok_r(NULL, "\t\r\n ", &nextToken) );

      DataLog (log_level_datfile_info) << "REF (" << catNum << ") + admin code (" << adminCode
                                       << ") + barcode (" << barcodeString << ") --> ";

      CatalogChecker catalogChecker(catNum, adminCode);

      // If the admin code does not match the REF, complain and quit.
      if (!catalogChecker.isValid())
      {
         DataLog (log_level_datfile_info) << "Invalid" << endmsg;

         fileError("REF does not match admin code.", SETCONFIG_DAT_FILE_INVALID_ENTRY);
         _fileOK = false;
         return;
      }

      // If a barcode exists, and it does not contain the REF number as a substring,
      // then we should reject it and quit, because that's not cool with us. (See ITs 7754 and 7776)
      if ((strcmp(barcodeString, "0") != 0) &&
          (strstr(barcodeString, catNumString) == NULL))
      {
         DataLog (log_level_datfile_info) << "Invalid" << endmsg;

         DataLog (log_level_datfile_error) << "Listed barcode does not match REF number.  Barcode("
                                           << barcodeString << "), REF(" << catNumString << ")" << endmsg;

         fileError("Error matching barcode with REF number.", SETCONFIG_DAT_FILE_INVALID_ENTRY);
         _fileOK = false;

         return;
      }

      DataLog (log_level_datfile_info) << "Valid (Stamp: "
                                       << (int)catalogChecker.checkBit(CatalogChecker::STAMP)
                                       << ", PAS:" << (int)catalogChecker.checkBit(CatalogChecker::MSS_PLT)
                                       << ", RAS:" << (int)catalogChecker.checkBit(CatalogChecker::MSS_RBC)
                                       << ", DRBC Bag:" << (int)catalogChecker.checkBit(CatalogChecker::DRBC_BAG)
                                       << ", SRBC Bag:" << (int)catalogChecker.checkBit(CatalogChecker::RBC_BAG)
                                       << ", Pls Bag:" << (int)catalogChecker.checkBit(CatalogChecker::PLASMA_BAG)
                                       << ", Plt Bag:" << (int)catalogChecker.checkBit(CatalogChecker::PLATELET_BAG)
                                       << ")" << endmsg;

      CatalogNumber::STAMP_COLOR stampColor = (CatalogNumber::STAMP_COLOR)catalogChecker.checkBit(CatalogChecker::STAMP);

      bool isMssPlt = catalogChecker.checkBit(CatalogChecker::MSS_PLT);

      _adminCassettes.push_back(new CatalogNumber(catNumString, adminCode, barcodeString) );

      index++;
   }

   inFile.close();
   DataLog(log_level_datfile_info) << index << " cassettes added to admin table." << endmsg;

   _fileOK = true;
}


void AdminCassetteDat::updateIfChanged ()
{
   DataLog(log_level_datfile_info) << " has the file changed? " << _fileHasChanged << endmsg;

   if (_fileHasChanged)
      updateCassetteFile();
}



//
// UpdateCassetteFile
//
bool AdminCassetteDat::updateCassetteFile ()
{
   DataLog (log_level_datfile_info) << "Updating setconfig.dat." << endmsg;

   if (!_fileOK)
   {
      DataLog (log_level_datfile_error) << "Admin list did not initialize OK.  Update aborted." << endmsg;

      _fileHasChanged = false;
      return false;
   }

   // First, delete the CRC file to invalidate the old file.
   remove(PNAME_SETCONFIG_CRC);

   // Next, create the new file.
   attrib(PNAME_SETCONFIGDAT, "-R");
   ofstream outFile;
   outFile.open(PNAME_SETCONFIGDAT, ios::out | ios::trunc | ios::binary, 0777);

   if (!outFile.good())
   {
      DataLog(log_level_datfile_error) << "File " << PNAME_SETCONFIGDAT << " couldn't be opened [" << outFile.rdstate() << "]." << endmsg;
      fileError("unable to open file cassette.dat for writing", SETCONFIG_DAT_FILE_OPEN_ERROR);

      _fileHasChanged = false;
      return false;
   }

   decorateFile(outFile);
   taskDelay(0); // Make sure other tasks get a chance to interupt (avoid priority inversion)

   for (CASSETTE_VECTOR_ITERATOR iter = _adminCassettes.begin(); iter != _adminCassettes.end(); ++iter)
   {
      const char* refNumStr  = (*iter)->RefNum();
      const char* barcodeStr = (*iter)->BarcodeNum();

      outFile << refNumStr << "\t" << (*iter)->AdminCode() << "   \t";
      taskDelay(0); // Make sure other tasks get a chance to interupt (avoid priority inversion)

      int barcodeLength = strlen(barcodeStr);

      if (barcodeLength < 5 || barcodeLength > 80)
      {
         outFile << "0" << endl;
         taskDelay(0); // Make sure other tasks get a chance to interupt (avoid priority inversion)
      }
      else
      {
         outFile << barcodeStr << endl;
         taskDelay(0); // Make sure other tasks get a chance to interupt (avoid priority inversion)
      }
   }

   if (_fileVersion && strcmp(_fileVersion, "") != 0)
      outFile << "file_version=" << _fileVersion << endl;
   else
      DataLog (log_level_datfile_error) << "No file version to report, so not outputting one." << endmsg;

   outFile.close();
   attrib(PNAME_SETCONFIGDAT, "+R");

   DataLog(log_level_datfile_info) << "File " << PNAME_SETCONFIGDAT << " updated successfully with " << _adminCassettes.size() << " entries." << endmsg;

   unsigned long calculatedCrc = INITCRC_DEFAULT;

   if (file_crcgen32(PNAME_SETCONFIGDAT, &calculatedCrc) == -1)
   {
      DataLog(log_level_datfile_error) << "CRC couldn't be generated for " << PNAME_SETCONFIGDAT << ".  Operation failed." << endmsg;
      fileError("unable to open file " PNAME_SETCONFIGDAT " for CRC generation", SETCONFIG_DAT_FILE_BAD_CRC);

      _fileHasChanged = false;
      return false;
   }

   // Store the computed CRC in the crc file

   attrib(PNAME_SETCONFIG_CRC, "-R");

   ofstream crcFile;
   crcFile.open(PNAME_SETCONFIG_CRC, ios::out | ios::trunc, 0777);

   if (!crcFile.good())
   {
      DataLog(log_level_datfile_error) << "File " << PNAME_SETCONFIG_CRC << " couldn't be opened." << endmsg;
      fileError("Unable to open file " PNAME_SETCONFIG_CRC " for writing", SETCONFIG_DAT_FILE_BAD_CRC);

      _fileHasChanged = false;
      return false;
   }

   crcFile << "0x" << hex << calculatedCrc << dec << endl;
   crcFile.close();

   attrib(PNAME_SETCONFIG_CRC, "+R");

   DataLog(log_level_datfile_info) << "CRC (" << hex << calculatedCrc << ") stored in " << PNAME_SETCONFIG_CRC << "." << dec << endmsg;
   _fileHasChanged = false;

   return true;
}



//
// decorateFile
//
void AdminCassetteDat::decorateFile (ostream& o)
{
   o << "#"                                                    << endl;
   o << "#\tThis file lists the cassettes configured by the"   << endl;
   o << "#\tadministrator.  It is not a comprehensive list."   << endl;
   o << "#\tRather, it represents the subset of REF numbers"   << endl;
   o << "#\tthat will come up in the listbox on the REF"       << endl;
   o << "#\tnumber entry screen."                              << endl;
   o << "#"                                                    << endl;
   o << "#\tEach cassette has a 5 or 6 digit REF number and"   << endl;
   o << "#\tan 8-digit admin code that encrypts the"           << endl;
   o << "#\tfunctionality of the cassette. Also, a barcode"    << endl;
   o << "#\tstring is available so that those customers who"   << endl;
   o << "#\thave a barcode reader available can scan a"        << endl;
   o << "#\tbarcode instead of manually selecting a REF"       << endl;
   o << "#\tnumber."                                           << endl;
   o << "#"                                                    << endl;
   o << "#\tCassettes can be added or removed through the"     << endl;
   o << "#\tmachine config screen.  If a REF number exists in" << endl;
   o << "#\tboth this file and cassette.dat, but with"         << endl;
   o << "#\tdifferent functionality or barcode listed, this"   << endl;
   o << "#\tfile overrides the other one."                     << endl;
   o << "#"                                                    << endl;
   o << "#\tA list of REF numbers and the sets they are"       << endl;
   o << "#\tassociated with is available from the MDC"         << endl;
   o << "#\t(part number 377500017)."                          << endl;
   o << "#"                                                    << endl;
   o << "#\tID\tADMIN CODE   \tBARCODE"                        << endl;
}

/* FORMAT HASH 17339cac2395ac7b6e363e50ec432699 */

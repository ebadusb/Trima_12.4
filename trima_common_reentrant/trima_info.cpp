/****************************************************************************
*
* Copyright (c) 2002 by Cobe BCT, Inc.  All rights reserved.
*
* TITLE:      trima_info
*
* AUTHOR:     Regis McGarry
*
* REVISION HISTORY:
*
****************************************************************************/
#include <vxworks.h>
#include <stdlib.h>
#include <stdio.h>
#include <net/inet.h>
#include <netinet/in.h>   // Gets htonl
#include <fstream.h>

#include "trima_info.h"
#include "portnumbers.h"   // Gets port numbers used by Trima.
#include "filenames.h"     // List of Trima file names
#include "trima_datalog.h" // Gets DataLog Level names.
#include "env_names.h"


bool TrimaInfo::_testBuild     = false;
bool TrimaInfo::_clinicalBuild = false;
bool TrimaInfo::_serviceMode   = false;


unsigned short TrimaInfo::_vistaPortNumber           = 0;
unsigned int   TrimaInfo::_predictVersion            = 0;
bool           TrimaInfo::_vistaDirectSendConfigured = false;

string         TrimaInfo::_vistaDirectSendIpAddress;


string TrimaInfo::_logFileNameFull;
string TrimaInfo::_logFileNameShort;
string TrimaInfo::_serialNumber;
string TrimaInfo::_ipAddress;
string TrimaInfo::_broadcastAddress;
string TrimaInfo::_broadcastPort;
string TrimaInfo::_trimaCRC;
string TrimaInfo::_safetyCRC;
string TrimaInfo::_buildNumber;
string TrimaInfo::_sequenceNumber;
string TrimaInfo::_hwRevision;


//
// Class constructor
//
TrimaInfo::TrimaInfo()
{}



//
// Class destructor
//
TrimaInfo::~TrimaInfo()
{}



//
// getTrimaLogName
//
void TrimaInfo::getLogFileName ()
{
   const char* logFileName = getenv(LogFileNameEnumName);

   if (logFileName == 0)
   {
      DataLog(log_level_datfile_error) << "Log File Name invalid" << endmsg;
      _logFileNameFull = "1T00000_00000000_000";

      return;
   }
   else
   {
      _logFileNameFull = logFileName;
   }

   // Remove the sequence number from the file name
   char tempString[strlen(logFileName) + 1];

   strcpy(tempString, logFileName);

   char* sequenceNumber = strrchr(tempString, '_');

   if (sequenceNumber == 0)
   {
      DataLog(log_level_datfile_error) << "Log File Name invalid" << endmsg;
      _logFileNameShort = "1T00000_00000000_000";

      return;
   }
   else
   {
      *sequenceNumber   = 0; // terminate log file name string
      _logFileNameShort = tempString;
   }
}



//
// Retrieves the Trima Serial Number
//
void TrimaInfo::getSerialNumber ()
{
   const char* serialNumber = getenv(MachineEnumName);

   if (serialNumber == 0)
   {
      DataLog(log_level_datfile_error) << "Serial Number invalid" << endmsg;
      _serialNumber = "1T00000";
   }
   else
   {
      _serialNumber = serialNumber;
   }
}



//
// getIPAddress
//
void TrimaInfo::getIPAddress ()
{
   const char* ipAddress = getenv(ExternalIpEnumName);

   if (ipAddress == 0)
   {
      DataLog(log_level_datfile_error) << "Trima External IP Address invalid" << endmsg;
      _ipAddress = "172.21.0.0";
   }
   else
   {
      _ipAddress = ipAddress;
   }
}



//
// getTrimaBroadcastAddress
//
void TrimaInfo::getBroadcastAddress ()
{
   const char* broadcastAddress = getenv(BroadcastIpEnumName);

   if (broadcastAddress == 0)
   {
      DataLog(log_level_datfile_error) << "Trima Broadcast IP Address invalid" << endmsg;
      _broadcastAddress = "172.21.255.255";
   }
   else
   {
      _broadcastAddress = broadcastAddress;
   }
}



//
// GetTrimaBroadcastPort
//
void TrimaInfo::getBroadcastPort ()
{
   char temp[6];
   sprintf(temp, "%d", int(Trima_BroadcastPort));
   _broadcastPort = temp;
}



//
// getTrimaCRC
//
void TrimaInfo::getTrimaCRC ()
{
   char* crc = getenv(TrimaCrcEnumName);

   if (crc == 0)
   {
      DataLog(log_level_datfile_error) << "Trima CRC invalid" << endmsg;
      _trimaCRC = "0x00000000";
   }
   else
   {
      _trimaCRC = crc;
   }
}



//
// getSafetyCRC
//
void TrimaInfo::getSafetyCRC ()
{
   const char* crc = getenv(SafetyCrcEnumName);

   if (crc == 0)
   {
      DataLog(log_level_datfile_error) << "Safety CRC invalid" << endmsg;
      _safetyCRC = "0x00000000";
   }
   else
   {
      _safetyCRC = crc;
   }
}



//
// GetTrimaBuildNumber
//
void TrimaInfo::getBuildNumber ()
{
   const char* buildNumber = getenv(BuildEnumName);

   if (buildNumber == 0)
   {
      DataLog(log_level_datfile_error) << "Trima Build Number invalid" << endmsg;
      _buildNumber = "0.0";
   }
   else
   {
      _buildNumber = buildNumber;
   }
}



//
// GetTrimaSequenceNumber
//
void TrimaInfo::getSequenceNumber ()
{
   char* fullLogFileName = getenv(LogFileNameEnumName);

   if (fullLogFileName == 0)
   {
      DataLog(log_level_datfile_error) << "Sequence Number Invalid" << endmsg;
      _sequenceNumber = "-1";
      return;
   }

   string logFileName = fullLogFileName;

   int    index       = logFileName.find_last_of("_");

   if (index != 0)
   {
      _sequenceNumber = logFileName.substr(++index);

      if (_sequenceNumber.size() != 6)
      {
         DataLog(log_level_datfile_error) << "Sequence Number Invalid.  Size is "
                                          << _sequenceNumber.size() << ".  String is " << _sequenceNumber.c_str() << endmsg;
         _sequenceNumber = "-1";
      }
   }
   else
   {
      DataLog(log_level_datfile_error) << "Sequence Number Invalid" << endmsg;
      _sequenceNumber = "-1";
   }
}




//
// GetTrimaHWRevision
//
void TrimaInfo::getHWRevision ()
{
   const char* hwRevision = getenv(TrimaHardwareRevisionEnumName);

   if (hwRevision == 0)
   {
      DataLog(log_level_datfile_error) << "Trima Hardware Revision Number invalid" << endmsg;
      _hwRevision = "0.0";
   }
   else
   {
      _hwRevision = hwRevision;
   }
}



//
// GetServiceMode
//
void TrimaInfo::getServiceMode ()
{
   _serviceMode = (getenv(ServiceModeEnumName) != 0);
}



#if 0

//
// GetBiometricDeviceList
//
void TrimaInfo::biometricDeviceList (vector<string>& list)
{
   list.clear();

   // TBD.
   // Use iosNextDevGet call to get list of BIO ID's.
   // First call pass in null.  List completed when return value is null.
}
#endif

void TrimaInfo::updateVistaDirectSendInfo (string addr, uint32_t portNum, bool directSendConfigured)
{
   _vistaDirectSendIpAddress  = addr;
   _vistaPortNumber           = portNum;
   _vistaDirectSendConfigured = directSendConfigured;
}

void TrimaInfo::read ()
{
   /////////////////////////////////////////////
   // Open and read the Test Build file
   FILE* fp = fopen(TEST_BUILD, "r");

   if (fp)
   {
      _testBuild = true;
      fclose(fp);

      DataLog (log_level_datfile_info) << "Test Build File present" << endmsg;
   }
   else
      _testBuild = false;


   /////////////////////////////////////////////
   // Open and read the Clinical Build file
   fp = fopen(CLINICAL_BUILD, "r");

   if (fp)
   {
      _clinicalBuild = true;

      DataLog (log_level_datfile_info) << "Clinical Build File present" << endmsg;
   }
   else
      _clinicalBuild = false;

   if (fp)
      fclose(fp);

   /////////////////////////////////////////////
   // Open and read the Predict version file
   fp = fopen(PREDICT_VERSION_FILE, "r");

   bool readOK = false;

   if (fp)
   {
      readOK = (fscanf(fp, "%d", &_predictVersion) == 1);
      fclose(fp);
   }

   if (readOK)
   {
      DataLog (log_level_datfile_info) << "Predict version: " << _predictVersion << endmsg;
   }
   else
   {
      DataLog (log_level_datfile_error) << "Predict version file read failed.  Errno=" << errnoGet() << endmsg;
   }

   // Init the rest of the static vars
   getLogFileName();
   getSerialNumber();
   getIPAddress();
   getBroadcastAddress();
   getBroadcastPort();
   getTrimaCRC();
   getSafetyCRC();
   getBuildNumber();
   getSequenceNumber();
   getHWRevision();
   getServiceMode();
}

/* FORMAT HASH 25dd03d17bad11633aaa00f18ca982a9 */

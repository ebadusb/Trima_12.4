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


#ifndef __TRIMAINFO__
#define __TRIMAINFO__

#include <string>
#include <vector>

#define LOG_FILENAME_SIZE     24

#include "portnumbers.h"
#include "trimver.h"

class TrimaInfo
{
public: TrimaInfo::TrimaInfo();
   virtual TrimaInfo::~TrimaInfo();

   //
   // This method will open and read all the files containing the
   // identification values accessable by this class
   static void read ();

   //
   // These functions return common System Information used in Trima
   //
   static const char* logFileNameFull () { return _logFileNameFull.c_str(); }    // with sequence number
   static const char* logFileNameShort () { return _logFileNameShort.c_str(); }  // without sequence number

   static const char* serialNumber () { return _serialNumber.c_str(); }

   static const char* ipAddress () { return _ipAddress.c_str(); }

   static const char* broadcastAddress () { return _broadcastAddress.c_str(); }

   // static const char * broadcastPort() { return _broadcastPort.c_str(); };
   static unsigned short broadcastPort () {  return (unsigned short)Trima_BroadcastPort; }

   static const char* trimaCrc () { return _trimaCRC.c_str(); }
   static const char* safetyCrc () { return _safetyCRC.c_str(); }

   static const char* releaseNumber () { return DCL_TEXT_PWRUP_VERSION; }

   static const char* fullReleaseNumber () { return TEXT_FULL_VERSION; }

   static const char* buildNumber () { return _buildNumber.c_str(); }

   static const char* sequenceNumber () { return _sequenceNumber.c_str(); }

   static const char* hwRevision () { return _hwRevision.c_str(); }

   static void           updateVistaDirectSendInfo (string addr, uint32_t portNum, bool directSendConfigured);
   static bool           vistaDirectSendConfigured () { return _vistaDirectSendConfigured; }
   static const char*    vistaDirectSendIpAddress () { return _vistaDirectSendIpAddress.c_str(); }
   static unsigned short vistaDirectSendPort () { return _vistaPortNumber; }

   static unsigned int predictVersion () { return _predictVersion; }

   // static void biometricDeviceList(vector<string> &list);

   static inline bool isTestBuild () { return _testBuild; }
   static inline bool isClinicalBuild () { return _clinicalBuild; }

   static inline bool isServiceMode () { return _serviceMode; }


private:
   // initializers
   static void getLogFileName ();
   static void getSerialNumber ();
   static void getIPAddress ();
   static void getBroadcastAddress ();
   static void getBroadcastPort ();
   static void getTrimaCRC ();
   static void getSafetyCRC ();
   static void getBuildNumber ();
   static void getSequenceNumber ();
   static void getHWRevision ();
   static void getServiceMode ();


private:
   static bool           _testBuild;
   static bool           _clinicalBuild;
   static bool           _serviceMode;

   static bool           _vistaDirectSendConfigured;
   static unsigned short _vistaPortNumber;
   static string         _vistaDirectSendIpAddress;

   static unsigned int   _predictVersion;

   static string         _logFileNameFull;
   static string         _logFileNameShort;
   static string         _serialNumber;
   static string         _ipAddress;
   static string         _broadcastAddress;
   static string         _broadcastPort;
   static string         _trimaCRC;
   static string         _safetyCRC;
   static string         _buildNumber;
   static string         _sequenceNumber;
   static string         _hwRevision;

};

#endif

/* FORMAT HASH c29d6ca029c503f9b2e12c0dba7852a1 */

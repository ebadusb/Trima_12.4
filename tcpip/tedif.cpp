#include "tedif.h"
#include "time.h"
#include <stdio.h>
#include "crcgen.h"

#include "string.h"


#ifndef INITCRC_DEFAULT
#define INITCRC_DEFAULT 0xFFFFFFFF
#endif


//
// Default Constructor
//
TedIf::TedIf()
{
   memset (_errorString, 0, sizeof(_errorString));
}







//
// Default Destructor
//
TedIf::~TedIf()
{}



//
// PrepareMsg does the following to the message header
// (1) Sets the som field
// (2) Calculates the length field
// (3) Sets the timestamp field
// (4) Sets the headerCRC field
// (5) Sets the bodyCrc field
//
void TedIf::prepareMsg (void* msg, enum TED_MessageId messageId, unsigned long sizeInBytes)
{
   TED_Header* hdr = (TED_Header*)msg;

   hdr->som       = TED_SOM;
   hdr->msgId     = messageId;
   hdr->length    = sizeInBytes - TED_HEADER_SIZE;
   hdr->time      = time (NULL);
   hdr->headerCRC = 0;        // Initialize field before doing CRC.
   hdr->bodyCRC   = 0;        // Initialize field before doing CRC.

   // Compute the CRC for the message body if there is one.
   if (sizeInBytes > TED_HEADER_SIZE)
   {
      unsigned char* msgptr    = (unsigned char*)hdr + TED_HEADER_SIZE;
      const int      MsgLength = sizeInBytes - TED_HEADER_SIZE;

      hdr->bodyCRC = INITCRC_DEFAULT;
      crcgen32 (&hdr->bodyCRC, msgptr, MsgLength);
      hdr->bodyCRC = hdr->bodyCRC ^ INITCRC_DEFAULT;
   }

   hdr->headerCRC = INITCRC_DEFAULT;
   crcgen32 (&hdr->headerCRC, (unsigned char*)hdr, TED_HEADER_SIZE - sizeof(unsigned long));
   hdr->headerCRC = hdr->headerCRC ^ INITCRC_DEFAULT;
}



//
// This routine checks to see that the message is valid.  The following
// Checks are made:
// 1.  Check that the message header CRC is correct
// 2.  Check that the SOM is correct
// 3.  Check that the message ID is a valid range
// 4.  Check that the message length is valid
// 5.  Check that the message body checksum is correct
//
// Usage:
//    (1) pass in pointer to message buffer and the size received.
//    (2) routine will return 0 if message passed validation.  Otherwise
//        routine will pass back the appropriate response status code.  Also,
//        routine will set a descriptive text string that can be gotten by calling
//        errorString;
//
TED_BUFFER_STATUS TedIf::validHeader (const void* msg, unsigned long length)
{
   unsigned long     crcValue = INITCRC_DEFAULT;
   const TED_Header* hdr      = (TED_Header*)msg;

   // Validate the Header.

   // If the size in bytes is less than the header size, then return BUFFER_UNDERFLOW
   if (length < TED_HEADER_SIZE)
   {
      sprintf (_errorString, "Stream length less than size of header, value(%lu), expected(%d)",
               length, TED_HEADER_SIZE);
      return TED_BUFFER_UNDERRUN;
   }

   // 1.  Check that the SOM is correct
   if (hdr->som != TED_SOM)
   {
      // Log event for later debugging.
      sprintf (_errorString, "Invalid SOM in header, value(%lx), expected(%x)", hdr->som, TED_SOM);
      return TED_BAD_SOM;
   }

   // 2.  Check that the message header CRC is correct
   crcgen32 (&crcValue, (unsigned char*)msg, TED_HEADER_SIZE - sizeof (unsigned long));
   crcValue = crcValue ^ INITCRC_DEFAULT;

   if (crcValue != hdr->headerCRC)
   {
      // Log event for later debugging.
      sprintf (_errorString, "Invalid header CRC, value(%lx), expected(%lx)", hdr->headerCRC, crcValue);
      return TED_HEADER_CRC_INVALID;
   }

   // 3.  Check that the message ID is a valid range
   if ( (hdr->msgId < TED_FIRST_MSG) ||
        (hdr->msgId > TED_LAST_MSG) )
   {
      // Log event for later debugging.
      sprintf (_errorString, "Invalid Message ID in header, value(%lu), expected(%d-%d)",
               hdr->msgId, TED_FIRST_MSG, TED_LAST_MSG);
      return TED_MESSAGE_ID_INVALID;
   }

   // 4.  Check that the message length is valid
   if ( (length == TED_HEADER_SIZE) && (hdr->length != 0) )
   {
      sprintf (_errorString, "Header length less than rx stream length, value(%lu), expected(%lu)",
               length, TED_HEADER_SIZE + hdr->length);
      return TED_BUFFER_UNDERRUN;
   }
   else if ( (length != TED_HEADER_SIZE) && (hdr->length == 0) )
   {
      sprintf (_errorString, "Stream length greater than size of header, value(%lu), expected(%u)",
               length, TED_HEADER_SIZE);
      return TED_BUFFER_OVERRUN;
   }
   else if ( (hdr->length + TED_HEADER_SIZE) < length)
   {
      sprintf (_errorString, "Stream Length greater than message length, value=%lu, expected%lu",
               length, hdr->length + TED_HEADER_SIZE);
      return TED_BUFFER_OVERRUN;
   }
   else if ( (hdr->length + TED_HEADER_SIZE) > length)
   {
      sprintf (_errorString, "Stream Length less than message length, value=%lu, expected%lu",
               length, hdr->length + TED_HEADER_SIZE);
      return TED_BUFFER_UNDERRUN;
   }

   // 5.  Check that the message body checksum is correct
   if (hdr->length > 0)
   {
      unsigned char* msgptr = (unsigned char*)msg + TED_HEADER_SIZE;

      unsigned long  msgCRC = INITCRC_DEFAULT;
      crcgen32 (&msgCRC, msgptr, hdr->length);
      msgCRC = msgCRC ^ INITCRC_DEFAULT;

      if (msgCRC != hdr->bodyCRC)
      {
         // Log event for later debugging.
         sprintf (_errorString, "Invalid Msg Body CRC, value(%lx), expected(%lx)",
                  hdr->bodyCRC, msgCRC);
         return TED_BODY_CRC_INVALID;
      }
   }

   // Message Passed all validation.
   // Clear the error string
   _errorString[0] = 0;

   // Return Success
   return TED_OK;
}



//
// findSOM
//
int TedIf::findSOM (const void* msg, unsigned long length)
{
   const int* buffer       = (int*)msg;
   const int  bufferLength = (sizeof(int) - 1) / sizeof(int);

   if (length < sizeof(int))
   {
      return 0;
   }

   for (int i = 0; i < bufferLength; i++)
   {
      if (buffer[i] == TED_SOM)
      {
         return i;
      }
   }

   // we got nothing!
   return 0;
}



//
// Prints the header to the stream.
//
DataLog_Stream& operator << (DataLog_Stream& os, const TED_Header& hdr)
{

   os << "SOM("   << hex << hdr.som << dec
      << ") ID("  << hdr.msgId
      << ") L("   << hdr.length
      << ") T("   << hex << hdr.time
      << ") BC("  << hdr.bodyCRC
      << ") HC("  << hdr.headerCRC << dec
      << ")";

   return os;
}



//
// Prints the header to the stream.
//
DataLog_Stream& operator << (DataLog_Stream& os, const TED_ControlHwStatusMsg& status)
{

   os << "access pressure(" << status.accessPressure
      << ") emi temp("  << status.emiTemperature
      << ") cent current("  << status.centrifugeCurrent
      << ") cent pressure("  << status.centrifugePressure
      << ") 5V supply("  << status.fiveVoltSupply
      << ") leak detector("  << status.leakDetector
      << ") -12V supply("  << status.minusTwelvePS
      << ") +64V current("  << status.sixtyFourVoltCurrent
      << ") +64V supply("  << status.sixtyFourVoltSupply
      << ") +64V switched("  << status.sixtyFourVoltSwitched
      << ") +12V supply("  << status.twelveVoltSupply
      << ") +24V current("  << status.twentyFourVoltCurrent
      << ") +24V supply("  << status.twentyFourVoltSupply
      << ") +24V switched("  << status.twentyFourVoltSwitched
      << ") cassette pos("  << status.cassettePosition
      << ") cassette error("  << status.cassetteError
      << ") cent RPM("  << status.centrifugeRPM
      << ") cent error("  << status.centrifugeError
      << ") door locks("  << status.doorLocks
      << ") pause SW("  << status.pauseSwitch
      << ") stop SW("  << status.stopSwitch
      << ") pause SW Latch("  << status.pauseSwitchLatch
      << ") stop SW Latch("  << status.stopSwitchLatch
      << ") vibration("  << status.vibration
      << ") valve fault("  << status.valveFault
      << ") motor fault("  << status.motorFault
      << ") solenoid fault("  << status.solenoidFault
      << ") red("  << status.red
      << ") green("  << status.green
      << ") rg error("  << status.rbcCommError
      << ")";

   return os;
}



//
// Print the buffer status to the stream
//
DataLog_Stream& operator << (DataLog_Stream& os, const TED_BUFFER_STATUS& status)
{
   switch (status)
   {
      case TED_BUFFER_UNDERRUN :
         os << "BUFFER UNDERRUN";
         break;

      case TED_BUFFER_OVERRUN :
         os << "BUFFER OVERRUN";
         break;

      case TED_HEADER_CRC_INVALID :
         os << "HEADER CRC INVALID";
         break;

      case TED_BODY_CRC_INVALID :
         os << "BODY CRC INVALID";
         break;

      case TED_BAD_SOM :
         os << "BAD SOM";
         break;

      case TED_MESSAGE_ID_INVALID :
         os << "MESSAGE ID INVALID";
         break;

      case TED_OK :
         os << "OK";
         break;
   }

   os << "(" << (int)status << ")";

   return os;
}



//
// Print the status type to the stream
//
DataLog_Stream& operator << (DataLog_Stream& os, const TED_Status& status)
{
   switch (status)
   {
      case TED_CONNECTION_ALLOWED_OPERATIONAL :
         os << "TED CONNECTION ALLOWED OPERATIONAL";
         break;

      case TED_CONNECTION_ALLOWED_SERVICE :
         os << "TED CONNECTION ALLOWED SERVICE";
         break;

      case TED_CONNECTION_ALLOWED_SINGLE_STEP :
         os << "TED CONNECTION ALLOWED SINGLE STEP";
         break;

      case TED_CONNECTION_DENIED :
         os << "TED CONNECTION DENIED";
         break;

      case TED_REQUEST_ALLOWED :
         os << "TED REQUEST ALLOWED";
         break;

      case TED_REQUEST_ALLOWED_OVERRIDE :
         os << "TED REQUEST ALLOWED OVERRIDE";
         break;

      case TED_REQUEST_DENIED_NOT_IN_SERVICE_MODE :
         os << "TED REQUEST DENIED NOT IN SERVICE MODE";
         break;

      case TED_REQUEST_DENIED_NOT_CLIENT :
         os << "TED REQUEST DENIED NOT CLIENT";
         break;

      case TED_BAD_MESSAGE_RECEIVED :
         os << "TED BAD MESSAGE RECEIVED";
         break;

      case TED_OPERATION_FAILED :
         os << "TED OPERATION FAILED";
         break;
   }

   os << "(" << (int)status << ")";

   return os;
}



//
// Print the board type to the stream
//
DataLog_Stream& operator << (DataLog_Stream& os, const TED_BoardType& type)
{
   switch (type)
   {
      case CONTROL :
         os << "CONTROL";
         break;

      case SAFETY :
         os << "SAFETY";
         break;
   }

   os << "(" << (int)type << ")";

   return os;
}



//
// Prints the header to the stream.
//
DataLog_Stream& operator << (DataLog_Stream& os, const TED_Header* hdr)
{
   os << "SOM("   << hex << hdr->som << dec
      << ") ID("  << hdr->msgId
      << ") L("   << hdr->length
      << ") T("   << hex << hdr->time
      << ") BC("  << hdr->bodyCRC
      << ") HC("  << hdr->headerCRC << dec
      << ")";

   return os;
}



//
// Equal operator for Pump Status
//
CHWPumpStatus& CHWPumpStatus::operator = (const PumpStatus& rhs)
{
   this->errors        = rhs.errors;
   this->RPM           = rhs.RPM;
   this->Revs          = rhs.Revs;
   this->Accum         = rhs.Accum;
   this->Integral      = rhs.Integral;
   this->AcummIntegral = rhs.AcummIntegral;

   return *this;
}


//
// Equal operator for Pump Status
//
DataLog_Stream& operator << (DataLog_Stream& os, const CHWPumpStatus& status)
{
   os << "Errors("          << (int)status.errors
      << ") RPM("           << status.RPM
      << ") Revs("          << status.Revs
      << ") Accum("         << status.Accum
      << ") Integral("      << status.Integral
      << ") AcummIntegral(" << status.AcummIntegral << ")" << endmsg;

   return os;
}

/* FORMAT HASH 9d888077000bded749e9d199b26ffbda */

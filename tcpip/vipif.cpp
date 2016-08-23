#include <stdio.h>


#include "vipif.h"
#include "time.h"
#include "crcgen.h"


#ifndef INITCRC_DEFAULT
#define INITCRC_DEFAULT 0xFFFFFFFF
#endif


//
// Default Constructor
//
VipIf::VipIf()
{
   _errorString = new char [256];
}


//
// Default Destructor
//
VipIf::~VipIf()
{
   delete[] _errorString;
}


//
// PrepareMsg does the following to the message header
// (1) Sets the som field
// (2) Calculates the lengthInBytes field
// (3) Sets the timestamp field
// (4) Sets the headerCrc field
// (5) Sets the bodyCrc field
//
void VipIf::prepareMsg (void* msg, int messageId, int sequenceNumber, int status, int sizeInBytes)
{
   trimaVistaHeader* hdr = (trimaVistaHeader*)msg;

   hdr->som            = VIP_SOM_ID;
   hdr->messageType    = messageId;
   hdr->sequenceNumber = sequenceNumber;
   hdr->status         = status;
   hdr->lengthInBytes  = sizeInBytes - trimaVistaHeaderLength;
   hdr->timestamp      = time (NULL);
   hdr->headerCrc      = 0;   // Initialize field before doing CRC.
   hdr->bodyCrc        = 0;   // Initialize field before doing CRC.

   // Compute the CRC for the message body if there is one.
   if (sizeInBytes > trimaVistaHeaderLength)
   {
      const unsigned char* msgptr    = (unsigned char*)hdr + trimaVistaHeaderLength;
      const int            MsgLength = sizeInBytes - trimaVistaHeaderLength;

      hdr->bodyCrc = INITCRC_DEFAULT;
      crcgen32 (&hdr->bodyCrc, msgptr, MsgLength);
      hdr->bodyCrc = hdr->bodyCrc ^ INITCRC_DEFAULT;
   }

   hdr->headerCrc = INITCRC_DEFAULT;
   crcgen32 (&hdr->headerCrc, (unsigned char*)hdr, trimaVistaHeaderLength - sizeof(unsigned long));
   hdr->headerCrc = hdr->headerCrc ^ INITCRC_DEFAULT;
}



//
// This routine checks to see that the message is valid.  The following
// Checks are made:
// 1.  Check that the SOM is correct
// 2.  Check that the message header CRC is correct
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
int VipIf::validHeader (const trimaVistaHeader& hdr, int sizeInBytes)
{
   unsigned long crcValue = INITCRC_DEFAULT;


   // Validate the Header.

   // If the size in bytes is less than the header size, then return BUFFER_UNDERFLOW
   if (sizeInBytes < trimaVistaHeaderLength)
   {
      sprintf (_errorString, "Stream length less than size of header, value(%d), expected(%d)",
               sizeInBytes, trimaVistaHeaderLength);
      return VIP_BUFFER_UNDERRUN;
   }

   // 1.  Check that the SOM is correct
   if (hdr.som != VIP_SOM_ID)
   {
      // Log event for later debugging.
      sprintf (_errorString, "Invalid SOM in header, value(0x%x), expected(0x%lx)", hdr.som, VIP_SOM_ID);

      return VIP_BAD_START_OF_MSG;
   }

   // 2.  Check that the message header CRC is correct
   crcgen32 (&crcValue, (unsigned char*)&hdr, trimaVistaHeaderLength - sizeof (unsigned long));
   crcValue = crcValue ^ INITCRC_DEFAULT;

   if (crcValue != hdr.headerCrc)
   {
      // Log event for later debugging.
      sprintf (_errorString, "Invalid header CRC, value(0x%lx), expected(0x%lx)", hdr.headerCrc, crcValue);

      return VIP_HEADER_CRC_INVALID;
   }

   // 3.  Check that the message ID is a valid range
   if ( ( (hdr.messageType < VIP_FIRST_CMD_MSG) ||
          (hdr.messageType > VIP_LAST_CMD_MSG) ) &&
        ( (hdr.messageType < VIP_FIRST_STATUS_MSG) ||
          (hdr.messageType > VIP_LAST_STATUS_MSG) ) )
   {
      // Log event for later debugging.
      sprintf (_errorString, "Invalid Message ID in header, value(%d), expected(%d-%d,%d-%d)",
               hdr.messageType, VIP_FIRST_CMD_MSG, VIP_LAST_CMD_MSG, VIP_FIRST_STATUS_MSG, VIP_LAST_STATUS_MSG);
      return VIP_MESSAGE_ID_INVALID;
   }

   // 4.  Check that the message length is valid
   if ( (sizeInBytes == trimaVistaHeaderLength) && (hdr.lengthInBytes != 0) )
   {
      sprintf (_errorString, "Header length less than rx stream length, value(%d), expected(%d)",
               sizeInBytes, trimaVistaHeaderLength + hdr.lengthInBytes);
      return VIP_BUFFER_UNDERRUN;
   }
   else if ( (sizeInBytes != trimaVistaHeaderLength) && (hdr.lengthInBytes == 0) )
   {
      sprintf (_errorString, "Stream length greater than size of header, value(%d), expected(%d)",
               sizeInBytes, trimaVistaHeaderLength);
      return VIP_BUFFER_OVERRUN;
   }
   else if ( (hdr.lengthInBytes + trimaVistaHeaderLength) < sizeInBytes)
   {
      sprintf (_errorString, "Stream Length greater than message length, value=%d, expected%d",
               sizeInBytes,
               hdr.lengthInBytes + trimaVistaHeaderLength);
      return VIP_BUFFER_OVERRUN;
   }
   else if ( (hdr.lengthInBytes + trimaVistaHeaderLength) > sizeInBytes)
   {
      sprintf (_errorString, "Stream Length less than message length, value=%d, expected%d",
               sizeInBytes,
               hdr.lengthInBytes + trimaVistaHeaderLength);
      return VIP_BUFFER_UNDERRUN;
   }

   // 5.  Check that the message body checksum is correct
   if (hdr.lengthInBytes > 0)
   {
      unsigned char* msgptr = (unsigned char*)&hdr + trimaVistaHeaderLength;

      unsigned long  msgCRC = INITCRC_DEFAULT;
      crcgen32 (&msgCRC, msgptr, hdr.lengthInBytes);
      msgCRC = msgCRC ^ INITCRC_DEFAULT;

      if (msgCRC != hdr.bodyCrc)
      {
         // Log event for later debugging.
         sprintf (_errorString, "Invalid Msg Body CRC, value(%lx), expected(%lx)",
                  hdr.bodyCrc, msgCRC);
         return VIP_MSG_CRC_INVALID;
      }
   }

   // Message Passed all validation.
   // Clear the error string
   _errorString[0] = 0;

   // Return Success
   return 0;
}



char* VipIf::errorString (void)
{
   return _errorString;
}



#ifndef WIN32
//
// Prints the header to the stream.
//
DataLog_Stream& operator << (DataLog_Stream& os, const trimaVistaHeader& hdr)
{
   os << "SOM("   << hex << hdr.som
      << ") ID("  << dec << hdr.messageType
      << ") SEQ(" << hdr.sequenceNumber
      << ") ST("  << hdr.status
      << ") L("   << hdr.lengthInBytes
      << ") T("   << hex << hdr.timestamp
      << ") BC("  << hdr.bodyCrc
      << ") HC("  << hdr.headerCrc << dec
      << ")";

   return os;
}

#endif

/* FORMAT HASH c31f7fec1153ea6734a46ea8516ae949 */

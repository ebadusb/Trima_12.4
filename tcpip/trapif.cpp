/*******************************************************************************
 *
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      trapif.cpp
 *             TRIMA REMOTE ACCESS PROTOCOL
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the interface for the TRAP protocol
 *
 *
 * HISTORY:    $Log: trapif.cpp $
 * HISTORY:    Revision 1.5  2002/06/07 14:57:54Z  rm70006
 * HISTORY:    First cut at port of Trap
 * HISTORY:    Revision 1.4  2002/04/30 22:10:40Z  rm70006
 * HISTORY:    Fix compiler errors.
 * HISTORY:    Revision 1.3  2002/04/09 16:49:35Z  rm70006
 * HISTORY:    Initial port to VxWorks comlete.  Code compiles.
 * HISTORY:    Revision 1.2  2002/01/18 22:15:04Z  rm70006
 * HISTORY:    IT 5539.
 * HISTORY:    Found bug in buffer validation logic.  Wasn't checking for minimum buffer length first.  Found case where undersized buffer was being crc tested and failed.  Moved size check to top of list.
 * HISTORY:    Revision 1.1  2002/01/08 23:57:18Z  rm70006
 * HISTORY:    Initial revision
 * HISTORY:    Revision 1.1  2001/12/04 22:47:29  rm70006
 * HISTORY:    Initial revision
 *
 ******************************************************************************/

#include "trapif.h"
#include "time.h"
#include <stdio.h>
#include "crcgen.h"


char CTrapIf::m_ErrorString[256] = {0};


#ifndef INITCRC_DEFAULT
#define INITCRC_DEFAULT 0xFFFFFFFF
#endif


//
// Default Constructor
//
CTrapIf::CTrapIf()
{}


//
// Default Destructor
//
CTrapIf::~CTrapIf()
{}


//
// PrepareMsg does the following to the message header
// (1) Sets the som field
// (2) Calculates the lengthInBytes field
// (3) Sets the timestamp field
// (4) Sets the headerCrc field
// (5) Sets the bodyCrc field
//
void CTrapIf::prepareMsg (trapHeader* hdr, int messageId, int sequenceNumber, int status, int sizeInBytes)
{
   hdr->som            = TRAP_SOM_ID;
   hdr->messageType    = messageId;
   hdr->sequenceNumber = sequenceNumber;
   hdr->status         = status;
   hdr->lengthInBytes  = sizeInBytes - trapHeaderLength;
   hdr->timestamp      = (int)time (NULL);
   hdr->headerCrc      = 0;   // Initialize field before doing CRC.
   hdr->bodyCrc        = 0;   // Initialize field before doing CRC.

   // Compute the CRC for the message body if there is one.
   if (sizeInBytes > trapHeaderLength)
   {
      const unsigned char* msgptr    = (unsigned char*)hdr + trapHeaderLength;
      const int            MsgLength = sizeInBytes - trapHeaderLength;

      hdr->bodyCrc = INITCRC_DEFAULT;
      crcgen32 (&hdr->bodyCrc, msgptr, MsgLength);
      hdr->bodyCrc = hdr->bodyCrc ^ INITCRC_DEFAULT;
   }

   hdr->headerCrc = INITCRC_DEFAULT;
   crcgen32 (&hdr->headerCrc, (unsigned char*)hdr, trapHeaderLength - sizeof(unsigned long));
   hdr->headerCrc = hdr->headerCrc ^ INITCRC_DEFAULT;
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
int CTrapIf::validHeader (void const* msg, int sizeInBytes)
{
#ifdef WIN32
#pragma warning(disable:4996)
#endif

   trapHeader*   hdr      = (trapHeader*)msg;

   unsigned long crcValue = INITCRC_DEFAULT;


   // Validate the Header.

   // If the size in bytes is less than the header size, then return BUFFER_UNDERFLOW
   if (sizeInBytes < trapHeaderLength)
   {
      sprintf (m_ErrorString, "Stream length less than size of header, value(%d), expected(%d)",
               sizeInBytes, trapHeaderLength);
      return TRAP_BUFFER_UNDERRUN;
   }

   // 1.  Check that the message header CRC is correct
   crcgen32 (&crcValue, (unsigned char*)msg, trapHeaderLength - sizeof (unsigned long));
   crcValue = crcValue ^ INITCRC_DEFAULT;

   if (crcValue != hdr->headerCrc)
   {
      // Log event for later debugging.
      sprintf (m_ErrorString, "Invalid header CRC, value(%lx), expected(%lx)",
               hdr->headerCrc, crcValue);
      return TRAP_HEADER_CRC_INVALID;
   }

   // 2.  Check that the SOM is correct
   if (hdr->som != TRAP_SOM_ID)
   {
      // Log event for later debugging.
      sprintf (m_ErrorString, "Invalid SOM in header, value(%x), expected(%lx)",
               hdr->som, TRAP_SOM_ID);
      return TRAP_BAD_START_OF_MSG;
   }

   // 3.  Check that the message ID is a valid range
   if ( (hdr->messageType < TRAP_FIRST_MSG) ||
        (hdr->messageType > TRAP_LAST_MSG) )
   {
      // Log event for later debugging.
      sprintf (m_ErrorString, "Invalid Message ID in header, value(%d), expected(%d-%d)",
               hdr->messageType, TRAP_FIRST_MSG, TRAP_LAST_MSG);
      return TRAP_MESSAGE_ID_INVALID;
   }

   // 4.  Check that the message length is valid
   if (sizeInBytes < trapHeaderLength)
   {
      sprintf (m_ErrorString, "Stream length less than size of header, value(%d), expected(%d)",
               sizeInBytes, trapHeaderLength);
      return TRAP_BUFFER_UNDERRUN;
   }
   else if ( (sizeInBytes == trapHeaderLength) && (hdr->lengthInBytes != 0) )
   {
      sprintf (m_ErrorString, "Header length less than rx stream length, value(%d), expected(%d)",
               sizeInBytes, trapHeaderLength + hdr->lengthInBytes);
      return TRAP_BUFFER_UNDERRUN;
   }
   else if ( (sizeInBytes != trapHeaderLength) && (hdr->lengthInBytes == 0) )
   {
      sprintf (m_ErrorString, "Stream length greater than size of header, value(%d), expected(%d)",
               sizeInBytes, trapHeaderLength);
      return TRAP_BUFFER_OVERRUN;
   }
   else if ( (hdr->lengthInBytes + trapHeaderLength) < sizeInBytes)
   {
      sprintf (m_ErrorString, "Stream Length greater than message length, value=%d, expected%d",
               sizeInBytes,
               hdr->lengthInBytes + trapHeaderLength);
      return TRAP_BUFFER_OVERRUN;
   }
   else if ( (hdr->lengthInBytes + trapHeaderLength) > sizeInBytes)
   {
      sprintf (m_ErrorString, "Stream Length less than message length, value=%d, expected%d",
               sizeInBytes,
               hdr->lengthInBytes + trapHeaderLength);
      return TRAP_BUFFER_UNDERRUN;
   }

   // 5.  Check that the message body checksum is correct
   if (hdr->lengthInBytes > 0)
   {
      const unsigned char* msgptr = (unsigned char*)hdr + trapHeaderLength;

      unsigned long        msgCRC = INITCRC_DEFAULT;
      crcgen32 (&msgCRC, msgptr, hdr->lengthInBytes);
      msgCRC = msgCRC ^ INITCRC_DEFAULT;

      if (msgCRC != hdr->bodyCrc)
      {
         // Log event for later debugging.
         sprintf (m_ErrorString, "Invalid Msg Body CRC, value(%lx), expected(%lx)",
                  hdr->bodyCrc, msgCRC);
         return TRAP_MSG_CRC_INVALID;
      }
   }

   // Message Passed all validation.
   // Clear the error string
   m_ErrorString[0] = 0;

   // Return Success
   return 0;
}



char* CTrapIf::errorString (void)
{
   return m_ErrorString;
}



#ifndef WIN32
DataLog_Stream& operator << (DataLog_Stream& os, const TRAP_DAYS_OF_WEEK day)
{
   bool addSeparator = false;

   if ( (day > TRAP_ALL_DAYS) || (day < TRAP_NO_DAYS) )
   {
      os << "BAD DAY VALUE(" << (int)day << ")";
      return os;
   }

   // Test for the easy ones first
   if (day == TRAP_NO_DAYS)
   {
      os << "NO DAYS";
      return os;
   }
   else if (day == TRAP_ALL_DAYS)
   {
      os << "ALL DAYS";
      return os;
   }

   //
   if (day & TRAP_MONDAY)
   {
      os << "MONDAY";
      addSeparator = true;
   }

   if (day & TRAP_TUESDAY)
   {
      os << (addSeparator ? "|TUESDAY" : "TUESDAY");
      addSeparator = true;
   }

   if (day & TRAP_WEDNESDAY)
   {
      os << (addSeparator ? "|WEDNESDAY" : "WEDNESDAY");
      addSeparator = true;
   }

   if (day & TRAP_THURSDAY)
   {
      os << (addSeparator ? "|THURSDAY" : "THURSDAY");
      addSeparator = true;
   }

   if (day & TRAP_FRIDAY)
   {
      os << (addSeparator ? "|FRIDAY" : "FRIDAY");
      addSeparator = true;
   }

   if (day & TRAP_SATURDAY)
   {
      os << (addSeparator ? "|SATURDAY" : "SATURDAY");
      addSeparator = true;
   }

   if (day & TRAP_SUNDAY)
   {
      os << (addSeparator ? "|SUNDAY" : "SUNDAY");
      addSeparator = true;
   }

   return os;
}
#endif

/* FORMAT HASH 13d92d2b8fda629cfd8328122feb55c4 */

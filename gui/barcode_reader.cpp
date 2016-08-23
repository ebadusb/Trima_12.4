/***************************************************************************
* Copyright(c) 2003 Gambro BCT, Inc. All rights reserved
* Author:      Regis McGarry
* Class name:  N/A
* File name:   barcode_reader.cpp
* Contents:    Mainline code for the barcode port reader process.
* Description:
*   This is the "C" Main routine for the barcode reader process. This
*   process will block on the comm. port for the barcode scanner on the
*   Trima. It will buffer I/O, detect scan packets (using prefix and
*   suffix codes) and send scanned packets out as messages via the Trima
*   router.
*
***************************************************************************/

#include <vxworks.h>
#include <ioLib.h>
#include <sioLib.h>

#include "messagesystem.h"        // Get message system stuff
#include "trima_datalog.h"        // Get Datalog stuff
#include "scan_structure.h"       // Get barcocde msg defn
#include "an_alarm.h"



//
// openbarcodePort
//
int openBarcodePort (const char* deviceName)
{
   int fd;

   // Open the port for read only
   fd = open(deviceName, O_RDONLY, DEFAULT_FILE_PERM);

   // Check for successful open
   if (fd == -1)
   {
      // wasn't successful - errno contains the reason why
      DataLog(log_level_barcode_reader_error) << "Failed to open port " << deviceName << ".  errno = " << errnoMsg << endmsg;

      return fd;
   }

   // Set the baud rate to the parameter passed to this routine
   if (ioctl (fd, FIOBAUDRATE, 9600) != 0)  // -1 == unsuccessful; 0 == successful
   {
      DataLog(log_level_barcode_reader_error) << "Failed to set baud rate. Errno =" << errnoMsg << endmsg;
      close (fd);

      return -1;
   }

   // The end of a canonical read is a CR - this will be programmed into the
   // Welsh Allyn 3800 scanner to add this at the end of all symbologies
   // (see page 4-3 of 1999 ops manual)
   // Canonical mode.

   // Line mode doesn't seem to work with VxWorks.  Open in raw mode and
   // process data the hard way.
   if (ioctl(fd, FIOSETOPTIONS, OPT_RAW) != 0)
   {
      DataLog(log_level_barcode_reader_error) << "Failed to set mode.  Errno = " << errnoMsg << endmsg;
      close (fd);

      return -1;
   }

   //
   // Set the control options -- this will be a local line (don't have to handle hangups & job control)
   //  and enable the receiver
   // Set the port to 7 bit; 1 stop; even parity
   //
   if (ioctl (fd, SIO_HW_OPTS_SET, CLOCAL | CREAD | CS7 | PARENB) != 0) // -1 == unsuccessful; 0 == successful
   {
      DataLog(log_level_barcode_reader_error) << "Failed ioctl setting port options> errno = " << errnoMsg << endmsg;
      close (fd);

      return -1;
   }

   DataLog(log_level_barcode_reader_info) << "SUCCESSFUL port opening." << endmsg;

   return fd;
}



//
// getBarcode
//
bool getBarcode (int fd, char* buffer, int& bufferLength)
{
   // Reset the buffer and the buffer length
   buffer[0]    = '\0';
   bufferLength = 0;


   if (fd == -1)
   {
      DataLog(log_level_barcode_reader_error) << "bad file descriptor." << endmsg;

      return false;
   }

   // Flush the stream before reading
   ioctl(fd, FIORFLUSH, 0);

   bufferLength = 0;

   // Read the port until we get a cr
   for (;; )
   {
      bufferLength += read(fd, &buffer[bufferLength], MAX_SCAN_LENGTH - bufferLength - 1);

      // Terminate buffer for strchr
      buffer[bufferLength] = 0;

      char* barcodeTerminator = strchr(buffer, '\r');

      // Guard against filling the buffer without the terminator.
      if (!barcodeTerminator && (bufferLength >= MAX_SCAN_LENGTH - 1))
      {
         bufferLength = MAX_SCAN_LENGTH - 1;

         // Terminate the string
         buffer[bufferLength] = 0;

         // Return because we have a full buffer.
         return false;
      }

      // If complete barcode (has the cr), then process it
      if (barcodeTerminator != 0)
      {
         // Terminate the string.
         *barcodeTerminator = 0;

         // reduce the buffer length by 1 because the read length includes the CR at the end of the line
         // -- we don't want to propagate it.
         bufferLength--;

         return true;
      }
   }

   return false;
}


static const char* deviceName = "/tyCo/1";
//
// Barcode Reader process
//
extern "C" void barcode_reader (void)
{
   DataLog(log_level_barcode_reader_info) << "barcode_reader: barcode device name is [" << deviceName << "]." << endmsg;

   int  fd;
   int  bufferLength;
   char barcodeType;
   char buffer[MAX_SCAN_LENGTH];

   bzero(buffer, MAX_SCAN_LENGTH);

   MessageSystem ms;

   // Initialize the message system
   ms.initNonBlock("barcode_reader");

   // create a message structure
   BARCODE_STRUCT barcode;

   // create a barcode message object
   Message<BARCODE_STRUCT> barcodeMsg(MessageBase::SEND_LOCAL);

   // check to see that we have the serial port name.
   if (deviceName == 0)
   {
      anAlarmMsg alarm(BARCODE_INITIALIZATION_ALARM);
      alarm.setAlarm("device name is null");
      DataLog(log_level_barcode_reader_error) << "null barcode device name. exiting..." << endmsg;
      return;
   }

   DataLog(log_level_barcode_reader_info) << "Opening device " << deviceName << endmsg;

   // open the port
   fd = openBarcodePort(deviceName);

   if (fd == ERROR)
   {
      DataLog(log_level_barcode_reader_error) << "Failed to open barcode serial port " << deviceName << endmsg;
      anAlarmMsg barcode_alarm(BARCODE_INITIALIZATION_ALARM);
      barcode_alarm.setAlarm();
      return;
   }

   for (;; )
   {
      // read the next barcode scan
      if (getBarcode(fd, buffer, bufferLength))
      {
         barcode.barcode_read_error = false;
      }
      else
      {
         barcode.barcode_read_error = true;
      }

      barcode.symbology_code = buffer[0];
      barcode.barcode_len    = bufferLength > 0 ? --bufferLength : 0;

      memcpy(barcode.barcode_data, &buffer[1], barcode.barcode_len);
      barcode.barcode_data[barcode.barcode_len] = 0;

      // send the message to GUI
      barcodeMsg.send(barcode);

      // log this to the tracelog
      datalog_Print(log_handle_barcode_reader_info, __FILE__, __LINE__, "SYMB_ID_HEX[%X] SYMB_ID_PRINTABLE[%c] READ_ERR[%d] DATA_LEN[%d] DATA[%s]",
                    (int)barcode.symbology_code,
                    barcode.symbology_code,
                    (int)barcode.barcode_read_error,
                    barcode.barcode_len,
                    &barcode.barcode_data);
   }

   close(fd);

   DataLog(log_level_barcode_reader_info) << "Barcode Reader exiting" << endmsg;
}

/* FORMAT HASH 099b66934dfd9cd8ec534691abca7178 */

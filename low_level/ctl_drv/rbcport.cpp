/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      rbcport.cpp, Trima rbc detctor interface
 *
 * ABSTRACT:   Interface to the rbc detector microcontroller.  The rbcInterface class
 *             allows the setting of the read and green driver values and
 *             the updating of the red and green sensed values.
 *             This task communicates with the control driver via shared
 *             memory
 *
 */

#include <vxWorks.h>
#include <fcntl.h>
#include <ioLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysLib.h>

#include "crc_util.h"
#include "messagesystem.h"
#include "optionparser.h"
#include "rbcport.hpp"
#include "rbcshmem.h"
#include "sh_mem.h"
#include "trima_assert.h"
#include "trima_datalog.h"

// SPECIFICATION:    rbcCmdInterface constructor
//
// ERROR HANDLING:   _FATAL_ERROR.

rbcCmdInterface::rbcCmdInterface(const char* portName)
   : _commandPollTimer(500, Callback<rbcCmdInterface>(this, &rbcCmdInterface::commandPoll))
{
   TRIMA_ASSERT(portName);

   //
   // link to shared memory for commands from the control driver
   //
   _writeDrive = (writeDrive*)shmem_open("writeDrive");

   if (!_writeDrive)
      _FATAL_ERROR(__FILE__, __LINE__, "writeDrive shmem_open failed");

   // establish communication with the serial device driver
   _fdPort = open(portName, O_RDWR, 0666);

   if (_fdPort == ERROR)
   {
      // quit.  we'll get a recoverable alarm while trying to recognize the set
      // and while trying to calibrate the RBC detector
      DataLog(log_level_rbc_error) << "open(\"" << portName << "\") failed" << endmsg;
      exit(1);
   }

   //
   // Set serial port to raw mode, 9600 baud, 8 bits, no parity, no flow control
   //
   ioctl(_fdPort, FIOSETOPTIONS, OPT_RAW);
   ioctl(_fdPort, FIOBAUDRATE, 9600);
   ioctl(_fdPort, SIO_HW_OPTS_SET, CLOCAL | CS8);

   //
   // Write initial drive values to device
   //
   const char* cmdString = "0,0\r";
   write(_fdPort, (char*)cmdString, strlen(cmdString));
}


// SPECIFICATION:    rbdDetector destructor
//
// ERROR HANDLING:   none.

rbcCmdInterface::~rbcCmdInterface()
{
   if (_fdPort != ERROR)
   {
      close(_fdPort);
      _fdPort = ERROR;
      shmem_close("writeDrive");
   }
}

// SPECIFICATION:    sets drive values for red and green leds
//
// ERROR HANDLING:   none.

void rbcCmdInterface::commandPoll (void)
{
   // do we have a new order?
   if (_writeDrive->writeCount != 0)
   {
      // first copy to a local structure so we don't lose it in the middle of the write
      memcpy(&_localWriteDrive, _writeDrive, sizeof(writeDrive));

      // check the crc on the local copy
      unsigned long checksum = 0;
      CRCGEN32_STRUCT(&checksum, &_localWriteDrive);

      if (checksum == _localWriteDrive.crc)
      {
         // write the local copy to the port
         _writeDrive->newDriveResult = write(_fdPort, _localWriteDrive.newDriveValues, strlen(_localWriteDrive.newDriveValues) );

         // generate a crc on the drive values and result
         _writeDrive->crc = 0;
         CRCGEN32_STRUCT(&_writeDrive->crc, _writeDrive);

         // following code may miss a message if the driver pre-empts with
         // new rbc drive values before this task completes the following instruction
         // clear the flag to indicate a drive value has been written
         _writeDrive->writeCount--;
      }
      else
      {
         DataLog(log_level_rbc_error) << hex << "CRC check error: generated=0x" << checksum
                                      << " expected=0x" << _localWriteDrive.crc << dec << endmsg;
      }
   }
}

rbcDataInterface::rbcDataInterface(const char* portName)
{
   TRIMA_ASSERT(portName);

   //
   // link to shared memory for data to the control driver
   //
   _readReflectance = (readReflectance*)shmem_open("readReflectance");

   if (!_readReflectance)
      _FATAL_ERROR(__FILE__, __LINE__, "readReflectance shmem_open failed");

   // establish communication with the serial device driver
   _fdPort = open(portName, O_RDONLY, 0666);

   if (_fdPort == ERROR)
   {
      // quit.  we'll get a recoverable alarm while trying to recognize the set
      // and while trying to calibrate the RBC detector
      DataLog(log_level_rbc_error) << "open(\"" << portName << "\") failed" << endmsg;
      exit(1);
   }
}

// SPECIFICATION:    read serial data from RBC detector
//
// ERROR HANDLING:   none.

void rbcDataInterface::readTask ()
{
   enum { ReadBufferSize   = 64 };
   enum { MaxMessageLength = 16 };

   char        buffer[ReadBufferSize];
   int         bufferPos     = 0;
   bool        firstMessage  = true;

   const char* writeResponse = "setLED";

   memset(buffer, 0, sizeof(buffer));
   ioctl(_fdPort, FIORFLUSH, 0);

   while (true)
   {
      bufferPos        += read(_fdPort, &buffer[bufferPos], ReadBufferSize - bufferPos - 1);
      buffer[bufferPos] = '\0';

      char* endOfMsg = strchr(buffer, '\n');

      if (!endOfMsg && bufferPos > MaxMessageLength)
      {
         bufferPos = 0;
         memset(buffer, 0, sizeof(buffer));
      }

      if (endOfMsg)
      {
         //
         // Valid messages are terminated with '\n', and can be in one of two
         // formats:
         //      NNNNN,NNNNN     - reflectance values, N=digit, max value = 65000
         //      setLED          - response from setting new drive levels
         //
         *endOfMsg = '\0';

         if (firstMessage)
         {
            firstMessage = false;
         }
         else if (isdigit(buffer[0]) &&
                  isdigit(buffer[1]) &&
                  isdigit(buffer[2]) &&
                  isdigit(buffer[3]) &&
                  isdigit(buffer[4]) &&
                  buffer[5] == ',' &&
                  isdigit(buffer[6]) &&
                  isdigit(buffer[7]) &&
                  isdigit(buffer[8]) &&
                  isdigit(buffer[9]) &&
                  isdigit(buffer[10]))
         {
            processReflectance(buffer);
         }
         else if (strncmp(buffer, writeResponse, strlen(writeResponse)) == 0)
         {
            processWriteResponse(buffer);
         }
         else
         {
            processBadData(buffer);
         }

         bufferPos -= (endOfMsg - buffer) + 1;
         memmove(buffer, endOfMsg + 1, bufferPos);
      }
   }
}

void rbcDataInterface::processBadData (const char* buffer)
{
   DataLog(log_level_rbc_error) << "invalid RBC detector data: \"" << buffer << "\"" << endmsg;

   _readReflectance->errors         = HW_FAULT;
   _readReflectance->red            = 0;
   _readReflectance->green          = 0;

   _readReflectance->crc            = 0;
   CRCGEN32_STRUCT(&_readReflectance->crc, _readReflectance);
   _readReflectance->portReadCount += 1;
}

void rbcDataInterface::processReflectance (const char* buffer)
{
   int newRed, newGreen;

   if (sscanf(buffer, "%d,%d", &newRed, &newGreen) == 2 &&
       newRed >= 0 && newRed <= 65000 &&
       newGreen >= 0 && newGreen <= 65000)
   {
      _readReflectance->red            = (short)newRed;
      _readReflectance->green          = (short)newGreen;
      _readReflectance->errors         = HW_NO_FAULT;

      _readReflectance->crc            = 0;
      CRCGEN32_STRUCT(&_readReflectance->crc, _readReflectance);
      _readReflectance->portReadCount += 1;
   }
}

void rbcDataInterface::processWriteResponse (const char* buffer)
{}

extern "C" void rbcCmd (const char* options)
{
   // create message system
   MessageSystem msgSystem;
   msgSystem.initBlocking("rbcCmd");

   //
   // Parse out the cmdline options
   OptionParser op;
   op.init(options);

   // create handler for RBC detector commands
   rbcCmdInterface rbcCmd(op.getArgv()[1]);

   // enter main message loop
   msgSystem.dispatchMessages();
}

extern "C" void rbcData (const char* options)
{
   //
   // Parse out the cmdline options
   OptionParser op;
   op.init(options);

   // create handler for RBC detector data
   rbcDataInterface rbcData(op.getArgv()[1]);
   rbcData.readTask();
}

/* FORMAT HASH cbf7066db6c0f0e4b708742c1b70b10e */

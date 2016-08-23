/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      rbcport.hpp, rbc detector port
 *
 * ABSTRACT:   Interface task between the rbc detector microcontroller
 *             and the control driver
 *
 */

#ifndef _RBCPORT_INCLUDE
#define _RBCPORT_INCLUDE

#include "chw.hpp"
#include "rbcshmem.h"
#include "timermessage.h"

// SPECIFICATION:    rbcCmdInterface class, this class allows control to set
//                   RBC detector drive levels
//
// ERROR HANDLING:   none.

class rbcCmdInterface
{
public:
   rbcCmdInterface(const char* port);
   ~rbcCmdInterface();

   void commandPoll (void);

private:
   TimerMessage _commandPollTimer;        // timer to control periodic checks for new write commands
   writeDrive*  _writeDrive;              // pointer to writeDrive data
   writeDrive   _localWriteDrive;         // local copy of the write values from shared memory

   int          _fdPort;                        // comm port file descriptor
};

class rbcDataInterface
{
public:
   rbcDataInterface(const char* port);
   ~rbcDataInterface() { }

   void readTask (void);

protected:
   void processBadData (const char* buffer);
   void processReflectance (const char* buffer);
   void processWriteResponse (const char* buffer);

private:
   readReflectance* _readReflectance;     // pointer to readReflectance data

   int              _fdPort;                    // comm port file descriptor
};

#endif /* ifndef _RBCPORT_INCLUDE */

/* FORMAT HASH 6496b85ca5b4fcb13eb7a8e25e3a58b4 */

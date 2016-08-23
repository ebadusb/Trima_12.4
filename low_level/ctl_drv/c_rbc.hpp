/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      c_rbc.hpp, Focussed System control rbc detector io
 *
 * ABSTRACT:   Interface to the control rbc detector via serial port
 *
 * DOCUMENTS
 *
 * CLASSES:
 * class rbcDetector
 *    rbcDetector() - constructor
 *    ~rbcDetector() - destructor
 *    drive() - set red/green drive values
 *    pid_t proxy() - get proxy PID
 *    read() - read device
 */

#ifndef _C_RBC_INCLUDE
#define _C_RBC_INCLUDE

#include "rbcshmem.h"
#include "hw.h"
#include "ostime.hpp"

// constants

static const int RBC_BUFFER_SIZE = 256;

// SPECIFICATION:    rbcPortMonitor class, this class causes the driver
//                   to log an error if the serial port does not deliver
//                   data for 10 seconds
//
// ERROR HANDLING:   none.

class rbcPortMonitor
{
public:
   rbcPortMonitor(void);
   ~rbcPortMonitor();

   void     monitorCommunications ();
   void     petTheDog ();
   HW_ERROR getCommError ();
   void     setCommError (HW_ERROR error);

private:
   rawTime  _lastTime;
   rawTime  _lastPollTime;
   HW_ERROR _commErrorFlag;
};


// SPECIFICATION:    rbcDetector class, this class allows control to set
//                   drive levels and to get new values from the sensor
//
// ERROR HANDLING:   none.

class rbcDetector
{
public:
   rbcDetector(short& red,                          // red status memory location
               short& green,                        // green status memory location
               unsigned long& newReflectanceCounts, // newReflectanceValues status memory location
               unsigned long& newDriveCounts);      // newDriveValues status memory location

   virtual ~rbcDetector();                          // destructor

   void newDrive (unsigned char red,     // red drive, normally 64
                  unsigned char green);  // green drive, normally 128

   void newDriveResult ();
   void read (HW_ERROR& errors);          // read device

private:
   writeDrive*      _writeDrive;            // pointer to writeDrive SHM data
   readReflectance* _readReflectance;       // pointer to readReflectance SHM data
   short&           _red;                   // red status
   short&           _green;                 // green status
   unsigned long&   _newReflectanceCounts;  // tracks changes in readReflectance data
   unsigned long&   _newDriveCounts;        // tracks successful write of new red/green drive values
   unsigned char    _redDriveInProgress;    // temp holder for new red drive value while write is in progress
   unsigned char    _greenDriveInProgress;  // temp holder for new green drive value while write is in progres
   unsigned char    _redDrive;              // red drive value
   unsigned char    _greenDrive;            // green drive value

   int              _lastPortReadCount;    // tracks changes in readReflectance data
   rawTime          _writeDriveTimer;      // watchdog on new drive writes
   rbcPortMonitor   _watchdog;             // a soft watchdog on the serial port
};

#endif // ifndef _C_RBC_INCLUDE

/* FORMAT HASH 7c94309ec2143280e7d2bccac2a3e79c */

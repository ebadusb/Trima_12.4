/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      cmsginfo.hpp, definition of structures in shared memory
 *             used to communicate between the control messaging task
 *             and the control driver.
 *
 * ABSTRACT:   Define message identification information in shared memory
 *             message areas.
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 * PUBLIC FUNCTIONS:
 *
 *
 */

#ifndef CMSGINFO_HPP
#define CMSGINFO_HPP

#include <time.h>
#include "chw.hpp"
#include "fastdata.h"
#include "ostime.hpp"

// structure for command packets
const int     NUM_CHWORDERS = 10;
const rawTime ZERO          = {0, 0};


typedef struct CHOEntry
{
   CHwOrders     orders;
   unsigned int  count;
   unsigned long crc;
   rawTime       timeOrdersEnqueued;
   rawTime       timeOrdersDequeued;

   CHOEntry()
      : count(0), crc(0), timeOrdersEnqueued(ZERO), timeOrdersDequeued(ZERO) {}
};

typedef struct CHO
{
   CHOEntry msg[NUM_CHWORDERS];
   int      NextIn;
   int      NextOut;

   //
   // This flag is set by ctl_msg in response to a message from
   // proc informing the control driver that power fail recovery is
   // in progress.  It is intended to insure that we re-enable
   // air to donor monitoring if the donor is connected, even
   // if the lower level sensor is uncovered (e.g. from a power
   // failure just at the end of a return cycle).
   //
   HW_ORDERS forceAirToDonorMonitor;
};


// structure for status packets
const int NUM_CHWSTATUS = 20;

typedef struct CHSEntry
{
   CHwStates     status;
   unsigned long crc;
};

typedef struct CHS
{
   CHSEntry msg[NUM_CHWSTATUS];
   int      NextIn;
   int      NextOut;

   //
   // Set by ctl_drv to indicate that a timing error has been detected.
   // It should be cleared by ctl_msg when the associated A2 alarm
   // has been set.
   //
   bool timingError;

   //
   // Set by ctl_drv to indicate that an A/D error has been detected.
   // It should be cleared by ctl_msg when the associated A2 alarm
   // has been set.
   //
   bool atodError;

   //
   // Set by ctl_drv to indicate that the time from the last Control order from
   // Proc exceeds the threshold and that the system should pause until the
   // condition clears.
   bool controlOrdersPastThreshold;

   // Set by ctl_drv to indicate that the MAX Control Order delay has occurred and
   // it is time to generate the A2 alarm
   bool    controlOrdersPastMaxThreshold;

   rawTime statusSentTimestamp;
};



// structure for fastfilter packets
const int NUM_FASTFILT = 10;

typedef struct FFEntry
{
   FASTFILTER    data;
   unsigned long crc;
};

typedef struct FASTFIL
{
   FFEntry msg[NUM_FASTFILT];
   int     count;
   int     NextIn;
   int     NextOut;
};


#endif // CMSGINFO_HPP

/* FORMAT HASH ec6c9937b500ddcf01784fab641e50c6 */

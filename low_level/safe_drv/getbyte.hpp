/*
 * Copyright (c) 1995-1999 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: K:/BCT_Development/vxWorks/Trima/low_level/safe_drv/rcs/getbyte.hpp 1.8 2002/12/26 22:05:33Z pn02526 Exp jl11312 $
 * $Log: getbyte.hpp $
 * Revision 1.8  2002/12/26 22:05:33Z  pn02526
 * Runnable safety driver with CRC checking of the code and FATAL timing errors disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.  Compiling with ENABLE_FATAL_TIMING_ERRORS defined will enable timing errors to shut the system down.
 * Revision 1.7  2002/11/14 17:21:52  pn02526
 * Checkins for first operating safety driver under vxWorks
 * Revision 1.6  2002/10/31 18:28:26Z  pn02526
 * Massive checkin for integration testing.  Changes BOOL to bool, and forces build to be for 486, only.
 * Revision 1.5  2002/08/13 13:47:35  pn02526
 * Remove extraneous 0x0C character.
 * Revision 1.4  1999/10/28 13:51:06  BS04481
 * Code review change.  Change sample space variable to unsigned
 * to avoid potential sign extension problem when shifting right.
 * Revision 1.3  1999/08/03 18:45:37  BS04481
 * Increase idle time between packets to at least 500ms.  Remove
 * idle time between bytes.   Force resync at start and when data
 * errors are detected.  Enable error detection for bad start bit, bad
 * stop bit and bad CRC on the packet.
 * Revision 1.2  1999/07/30 17:01:47  BS04481
 * Fixed problem where if communication stopped in an error state,
 * the watchdogs for "no data" did not set the message.
 *
 * TITLE:      c_getbyte.hpp, part of Focussed System basin temperature monitoring
 *
 * ABSTRACT:   Interface single bit data stream from the basin
 *             temperature microprocessor.
 *             This object is not terribly flexible as it assumes a
 *             data format (1 start, 8 data, no parity, 1 stop) and
 *             polarity (start=high, data1=low, data0=high, stop=low)
 *             However, it is much simpler if we assume this information.
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 * PUBLIC FUNCTIONS:
 *
 * class getbyte
 *    temp() - constructor
 *    ~temp() - destructor
 *    update() - update loop
 *
 */

#ifndef GETBYTE_HPP
#define GETBYTE_HPP

#include "safe_hw_private.h"

enum GETBYTE_STATES
{
   WAIT_FOR_START=0,
   SAMPLE,
   STOP,
   WAIT_FOR_IDLE
};

enum GETBYTE_RESULT
{
   NOT_DETERMINED=0,
   DATA_OK,
   DATA_ERR
};

static const int MAX_SAMPLES = (16 * 32);
static const int IDLE_COUNT  = 50;

// classes

class getbyte
{
public:
   getbyte(SafeHWGetByteSource select, float period);
   virtual ~getbyte();

   char assembleByte ();          // convert data stream to a hex byte
   GETBYTE_RESULT _dataValid;     // indicates if new data is ready
   void update ();                // read values
   void setForceIdle (bool idle); // force idle period resync

private:
   SafeHWGetByteSource _select;          // data source
   int                 _syncCounter;     // waits for idle between bytes
   int                 _notSyncCounter;  // detects not idle between bytes
   int                 _numStart;        // number of start bits
   int                 _numData;         // number of data bits
   int                 _numParity;       // number of parity bits
   int                 _numStop;         // number of stop bits
   float               _period;          // hold time of data in (ms)
   int                 _totalBits;       // total bits in data byte
   int                 _totalSamples;    // number of readings to take
   int                 _sampleCount;     // bit counter
   int                 _sampleWord;      // word counter
   unsigned int        _sampleSpace[16]; // max samples 16*32
   bool                _forceIdle;       // true=>wait for a new idle period to resync
   GETBYTE_STATES      _state;           // control getbyte state machine
   GETBYTE_RESULT validate ();           // check integrity of data stream
};


#endif /* ifndef GETBYTE_HPP */

/* FORMAT HASH e21bbf92bdf7a74c5fa6fe74da732c23 */

/*
 * Copyright (c) 1995-1999 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: K:/BCT_Development/vxWorks/Trima/low_level/safe_drv/rcs/basindata.hpp 1.9 2002/12/26 21:51:19Z pn02526 Exp jl11312 $
 * $Log: basindata.hpp $
 * Revision 1.9  2002/12/26 21:51:19Z  pn02526
 * Runnable safety driver with CRC checking of the code and FATAL timing errors disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.  Compiling with ENABLE_FATAL_TIMING_ERRORS defined will enable timing errors to shut the system down.
 * Revision 1.8  2002/12/17 09:14:38  pn02526
 * Checkins for first operating safety driver under vxWorks
 * Revision 1.7  2002/10/31 18:28:26Z  pn02526
 * Massive checkin for integration testing.  Changes BOOL to bool, and forces build to be for 486, only.
 * Revision 1.6  2002/08/13 13:39:38  pn02526
 * Substitute #include of timermessage.h for obsolete message system files.
 * Add basinTimer ::_myTimer to use the new timer message class definition,
 *   rather than deriving from a timer message class.
 * Remove _fd_* member, since it is not used by the shared memory emulation.
 * Revision 1.5  2000/06/06 13:38:03  ms10234
 * Messages were moved from the common to the trima project
 * Revision 1.4  1999/09/30 04:07:25  BS04481
 * Remove message send and receive functions from the driver
 * service loop.
 * Revision 1.3  1999/09/17 06:40:03  BS04481
 * Removed kernel calls to clock_****, qnx_****, dev_****, write
 * and delay functions in an effort to get better determinism in the
 * driver service loops
 * Revision 1.2  1999/07/29 03:28:15  BS04481
 * Changes to basin temp code as a result of QA review:  Driver will
 * notify upper level code of corrupt data received from microprocessor.
 * Driver will always listen at port without regard to cal.dat file.
 * Revision 1.1  1999/07/23 01:41:59  BS04481
 * Initial revision
 *
 * TITLE:      basinData.hpp, part of Focussed System basin temperature monitoring
 *
 * ABSTRACT:   Takes basin data from the getByte object.  Validates and
 *             sends to higher level code.
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 * PUBLIC FUNCTIONS:
 *
 *
 */

#ifndef BASINDATA_HPP
#define BASINDATA_HPP

#include "getbyte.hpp"
#include "hw.h"
#include "crc8bit.hpp"
#include "basin.hpp"
#include "ostime.hpp"
#include "smsginfo.hpp"


enum BASIN_STATES
{
   BASIN_LENGTH_ACQUIRE = 0,
   BASIN_DATA_ACQUIRE,
   BASIN_CRC_ACQUIRE
};

// classes

class basinTimer
{
public:
   basinTimer();
   ~basinTimer();
   void     notify ();
   void     petTheDog (bool valid);
   HW_ERROR getCommError ();
   void     clearCommError ();
   void     setDataError (HW_ERROR status);
   HW_ERROR getDataError ();
   void     clearDataError ();
private:
   rawTime  _lastTime;
   HW_ERROR _commErrorFlag;
   HW_ERROR _dataErrorFlag;
   bool     _validData;                   // true => data byte not corrupt
};


class basinData
{
public:
   basinData(class getbyte* acquisition);     // constructor
   ~basinData();                              // destructor
   void update ();
private:
   GETBYTE_RESULT checkCRC ();
   void           calculateTemp ();
   void           dataToGo ();
   BAS*              _BPtr;
   getbyte*          _acquisition; // acquires single bit data stream
   BASIN_STATES      _state;
   int               _receivedLength;
   int               _receivedData;
   unsigned char     _receivedCRC;
   int               _dataByteCount;
   bool              _valid;
   basinTemp         _temp;
   class basinTimer* _watchdog;
   class crc8Bit*    _crcCheck;

};


#endif // BASINDATA_HPP

/* FORMAT HASH 491a6f61021ca25bd00615e7b44f8d6d */

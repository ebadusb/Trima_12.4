/*
 * Copyright (c) 1995, 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: H:/BCT_Development/Trima5.R/Trima/low_level/safe_drv/rcs/s_ultrasonics.hpp 1.25 2003/06/25 15:47:00Z jl11312 Exp spriusb $
 * $Log: s_ultrasonics.hpp $
 * Revision 1.25  2003/06/25 15:47:00Z  jl11312
 * - additional logging for IT 6158 (safety air to donor shutdown)
 * Revision 1.24  2003/06/19 19:46:07Z  jl11312
 * - moved hardware interface to separate modules so that it is available earlier in the boot process
 * Revision 1.23  2002/11/15 00:21:52Z  pn02526
 * Checkins for first operating safety driver under vxWorks
 * Revision 1.22  2002/10/31 18:28:26Z  pn02526
 * Massive checkin for integration testing.  Changes BOOL to bool, and forces build to be for 486, only.
 * Revision 1.21  2002/08/13 13:52:01  pn02526
 * Remove extraneous #include of mman.h system file.
 * Revision 1.20  2002/05/13 09:48:58  pn02526
 * Name change: c_ultra -> s_ultrasonics
 * Revision 1.19  2001/06/28 09:20:46  jl11312
 * - added logging for timing errors exceeding 1/2 of allowed margin (IT 5182)
 * Revision 1.18  2001/04/05 20:16:01  jl11312
 * - internal timer handling changes required for versa logic CPU board
 * Revision 1.17  2000/08/07 23:00:23  bs04481
 * Allow visibility to 10ms data used/created by the drivers.
 * See IT 4741
 * Revision 1.16  1999/11/03 15:28:52  BS04481
 * Implement logging of min/max/average total counts from the
 * ultrasonic hardware at the end of each return cycle
 * Revision 1.15  1999/09/30 04:07:46  BS04481
 * Remove message send and receive functions from the driver
 * service loop.
 * Revision 1.14  1996/07/24 19:49:14  SS03309
 * fix MKS
 * Revision 1.13  1996/07/19 18:47:31  SS03309
 * lint changes
 * Revision 1.12  1996/07/18 22:39:23  SS03309
 * lint changes
 * Revision 1.11  1996/06/27 15:54:49  SS03309
 * Revised commets
 *
 * TITLE:      c_ultra.hpp, Focussed System control hardware ultrasonics
 *
 * ABSTRACT:   Reads and determines state of ultrasonics
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 * CLASSES:
 * class usMessage - data log message
 *    usMessage() - constructor
 *    ~usMessage() - destructor
 *    dataPoint() - save new data point, and send if full
 *
 * class ultrasonics - determines ultrasonic sensor state
 *    ultrasonics() - constructor
 *    ~ultrasonics() - destructor
 *    actual() - get current value
 *    update() - update loop
 */

#include <fcntl.h>

#include "smsginfo.hpp"
#include "crcgen.h"
#include "fastpack.hpp"

#ifndef _S_ULTRASONICS_INCLUDE
#define _S_ULTRASONICS_INCLUDE

#define US_SAMPLES     (20)     // sample size

typedef struct
{
   unsigned long runningTotal;
   unsigned long sampleCount;
   float         averageTotal;
   unsigned long totalMin;
   unsigned long totalMax;
}ultraStats;


// SPECIFICATION:    ultrasonics class, reads ultrasonic sensor
//
// ERROR HANDLING:   none.

class ultrasonics
{
public:
   ultrasonics(short fluidRatio);
   virtual ~ultrasonics();

   bool isFluid ();                                // =1 Fluid, =0 other
   void update ();                                 // process new reading
   void getStats (ultraStats* stats);
   bool checkTimingError ();                       // process timing errors
   void logHistory (void);

   void setSpoofingEmpty (bool spoofingEmpty){ _spoofingEmpty = spoofingEmpty; }

private:
   void checkCounts (long total);                  // check for expected number of counts

   unsigned char _lastAir;                         // last air count
   unsigned char _lastFluid;                       // last fluid count
   short         _fluidRatio;                      // fluid ratio
   long          _actual;                          // actual > (US_SAMPLES/2)=fluid, else=air
   short         _errorCount;                      // hardware error counter
   ultraStats    _cycleStats;
   fastpack*     _usair;
   fastpack*     _usfluid;

   //
   // See checkCounts() for usage of these data members
   //
   unsigned long _accTotal;                        // accumulated counts for this timing interval
   unsigned long _accInterval;                     // number of 10 msec periods within this interval
   unsigned long _startupDelay;                    // delay until count check starts
   int           _maxTimingError;                  // maximum timing error encountered so far
   bool          _timingError;                     // true if a timing error has been detected

   //
   //	History to be logged on sensor changes
   //
   enum { HistoryLogSize = 50 };
   unsigned char _historyData[HistoryLogSize];
   int           _historyIdx;
   int           _historyLogHoldoff;
   int           _historyLogRequested;

   bool          _spoofingEmpty;
};

#endif /* ifndef _S_ULTRASONICS_INCLUDE */

/* FORMAT HASH 9c7018953f03e7de9848dc92407228b9 */

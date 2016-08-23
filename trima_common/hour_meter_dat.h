/*
 * Copyright (C) 2012 Gambro BCT, Inc.  All rights reserved.
 *
 * This class encapsulates the meter.dat file.  The file is read during task initialization
 * and is a sentinel object.
 *
 * $Header: //depot/main/embedded/Trima/Main/trima_common/hour_meter_dat.h#1 $
 *
 */


#ifndef _HOUR_METER_DAT_H_
#define _HOUR_METER_DAT_H_

#include "datalog.h"

class HourMeterDat
{
public:
   HourMeterDat();
   virtual ~HourMeterDat();

   static void read ();
   static void update (long totalMachineTime, long centrifugeOnTime, long totalNumberOfRuns);

   // accesors
   static long totalMachineTime () {return _totalMachineTime; }
   static long totalCentrifugeOnTime () {return _totalCentrifugeOnTime; }
   static long totalNumberOfRuns () {return _totalNumberOfRuns; }

protected:
   static void updateFile (const char* filename, long totalMachineTime, long centrifugeOnTime, long totalNumberOfRuns);
   static bool writeDataToFile ();

protected:
   static DataLog_SetHandle _set;

   static long              _totalMachineTime;
   static long              _totalCentrifugeOnTime;
   static long              _totalNumberOfRuns;
};

#endif

/* FORMAT HASH 520e19ee8ce0e0cc59bb2d52f8fe361b */

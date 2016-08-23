/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * ABSTRACT:   Interface to the control hardware temperature and fan sansing
 *
 */

#ifndef _C_TEMP_INCLUDE
#define _C_TEMP_INCLUDE

#include "ostime.hpp"
#include "hw.h"

class tempSense
{
public:
   tempSense(HW_ERROR& tempFailure,
             short& emiTemp,
             HW_ERROR& fan0,
             HW_ERROR& fan1,
             HW_ERROR& fan2);
   virtual ~tempSense();

   void update ();             // read values

protected:
   void emiMonitor (void);

private:
   short&    _emiTemp;
   HW_ERROR& _fan0;
   HW_ERROR& _fan1;
   HW_ERROR& _fan2;
   HW_ERROR& _tempFail;
   HW_ERROR  _overTempInProgress;
   rawTime   _emiMonitorTime;

   //
   // Fans are sampled FanSampleSize times. If the number of
   // samples which reported a fan failure is greater than
   // MaxFanBadSamples, a fan fault is reported.
   //
   enum { FanSampleSize    = 1000 };   // 1000 * 10 msec = 10 second window
   enum { MaxFanBadSamples = 400 };    // max 40% of samples bad

   int _fanSampleCount;
   int _fan0BadCount;
   int _fan1BadCount;
   int _fan2BadCount;
};

#endif // ifndef C_TEMP_INCLUDE

/* FORMAT HASH 11dbcb5b81c37ce07f9b1854bc7b4972 */

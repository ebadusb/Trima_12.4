/*
 * Copyright (c) 1995, 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#ifndef _CENT_INCLUDE
#define _CENT_INCLUDE

#include "ostime.hpp"
#include "fastpack.hpp"

// SPECIFICATION:    centrifugeRpm class, this class measures the centrifuge RPM.
// Because this class is shared between Control and Safety, create a pure virtual
// method for resetting the centrifuge status bits (because it is different on each
// board).  So this class is designed to be abstracted on Control and Safety.
//
// ERROR HANDLING:   none.



const int ROLLING_AVG_SIZE = 5;    // number of samples in the rolling average


class centrifugeRpm
{
public:
   centrifugeRpm(void);
   virtual ~centrifugeRpm();                      // destructor

   float rpm () const;                            // return RPM
   void  errorStatus (bool powerOn,               // power avail
                      unsigned short& errors);    // *error=0 no fault, =1 fault

   long encoderRate () { return _encoderRate; }    // get average encoder rate (counts/sec)

   void update ();                                 // update loop

protected:
   virtual void clearCommStatus () = 0;            // Control and Safety have a different reset function

private:
   unsigned int   _rejectCount;                   // count of rejected anomalous signals
   unsigned int   _windowCount;
   long           _encoderRate;                   // order value
   rawTime        _lastMeasurementTime;           // time value
   long           _lastMeasurement;               // for deltas, hall pulses
   long           _rawEncoderRate;                // unaveraged rate, hall pulses/sec
   long           _lastRawRate[ROLLING_AVG_SIZE]; // averaging array
   int            _rollingIndex;                  // index into the average array
   unsigned short _lastErrorStatus;               // value of error bits when current error (if any) was detected

   fastpacklong   _fastRawEncoders;               // 10ms data
   fastpacklong   _fastAvgEncoders;               // 10ms data
};


#endif /* ifndef _CENT_INCLUDE */

/* FORMAT HASH c9e20632d7a85dd13365522d53120cb7 */

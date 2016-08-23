/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      c_pump.hpp, Focussed System pump controls
 *
 * ABSTRACT:   Interface to the control hardware pumps and return pump
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 * PUBLIC FUNCTIONS:
 *
 * class pump
 *    pump() - constructor
 *    ~pump() - destructor
 *    actuals() - get actual values
 *    order() - set new order value
 *    update( BOOL powerOn ) - update loop
 *    internals() - get internals for data logging and validation
 *
 * class returnPump
 *    returnPump() - constructor
 *    ~returnPump() - destructor
 *    update( BOOL powerOn ) - update loop
 */

#include "cmsginfo.hpp"
#include "ctl_hw_private.h"
#include "fastdata.h"
#include "fastpack.hpp"
#include "ostime.hpp"
#include "shw.hpp"

#ifndef C_PUMP_HPP
#define C_PUMP_HPP
// classes

// SPECIFICATION:    pump control class, this class is used to control
//                   pump speeds using a software control loop
//
// ERROR HANDLING:   none.

class pump
{
public:
   pump(HWPump select,                      // pump selection
        long dt,                            // update interval
        CHwOrders& orderData,               // order data
        CHwStates& statusData);             // status structure
   virtual ~pump();                         // destructor

   virtual void actuals (PumpStatus* p);     // get actual values
   void         internals (long spdOrder);   // get internal values for fastdata message
   void         order (float rpm);           // new order value
   virtual void update (BOOL powerOn);       // update orders

   static bool checkSafetyPumpEnabled (void) { return (_safetyOrderDataLinked && (_safetyOrders->pumpPower == HW_ENABLE) ); }
   static void setSHwOrders (SHwOrders* orders) { _safetyOrders = orders; _safetyOrderDataLinked = true; }

protected:
   long computeEncoderSpeed (long de);       // actual speed
   int  mV (short reading);                  // Returns mV from a FPGA reading
   void setDynamicGains ();
   void computeIntegral (long dt, long de);
   void computeVolumeError (long dt, long de);
   void rampOrder (long slope);
   void limitOrder (long& spdOrder);
   BOOL checkPumpVoltageFault ();            // returns TRUE for an unexpected drop in pump voltage
                                             // (i.e. one not commanded by safety)

   HWPump        _select;                    // pump being controlled by this object
   long          _dt;                        // delta Time, ms
   rawTime       _lastTime;                  // last system clock value

   long          _encoderSpeed;              // actual motor shaft speed, ticks/sec
   long          _reportedSpeed;             // actual ticks/sec, filtered
   long          _revs;                      // actual revs, motor shaft
   unsigned char _errors;                    // errors
   long          _cummulativeError;          // integral error accumulated
   long          _encoderOrder;              // encoder/sec order
   long          _encoderOrderRamp;          // encoder order ramp

   float         _fspeedError;               // integrated encoder speed error
   float         _fdeltaSpeed;               // proportional term
   short         _lastEncoder;               // last encoder value
   float         _fvolumeError;              // volume error, encoder counts

   float         _fAvgSpeedError;            // maintain average integral error for logging purposes
   int           _iAvgSpeedErrorCount;       // number of update cycles included in _fAvgSpeedError

   CHwOrders&    _orderData;                 // order data
   CHwStates&    _statusData;                // status data

   long          _pGain;                     // proportional gain
   long          _iGain;                     // integrator gain
   long          _vGain;                     // volume error gain

   short         _iErrorCount;               // integral error counter
   short         _vErrorCount;               // volume error counter
   short         _sErrorCount;               // speed error counter

   fastpacklong* _fastEncoders;            // 10ms data
   fastpacklong* _fastOrders;              // 10ms data
   fastpacklong* _fastIntError;            // 10ms data
   fastpacklong* _fastVolError;            // 10ms data
   fastpacklong* _fastDeltaSpd;            // 10ms data
   fastpacklong* _fastFault;               // 10ms data
   fastpacklong* _fastSpdOrder;            // 10ms data

protected:
   static bool       _safetyOrderDataLinked; // class has linked to safety order data shared memory area
   static SHwOrders* _safetyOrders;          // Safety order data

private:
   pump();                                   // default ctr disabled
   pump(pump const&);
   pump& operator = (pump const&);
};


// SPECIFICATION:    return pump control class, this class controls the
//                   return pump (which is bi-directional)
//
// ERROR HANDLING:   none.

class returnPump
   : public pump
{
public:
   returnPump(HWPump select,                 // pump selection
              long dt,                       // update time
              CHwOrders& orderData,          // order data
              CHwStates& statusData);        // status structure
   virtual ~returnPump();                    // destructor

   virtual void actuals (PumpStatus* p);     // get actual values
   void         update (BOOL powerOn);       // update

private:
   returnPump();                             // default ctr disabled
   returnPump(returnPump const&);
   returnPump& operator = (returnPump const&);
};

#endif /* ifndef C_PUMP_HPP */

/* FORMAT HASH f8617e2167f01b60f19bf51fd7128de6 */

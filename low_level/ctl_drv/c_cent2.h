/*
 * Copyright (C) 2003 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 * $Log$
 *
 */

#ifndef _C_CENT2_INCLUDE
#define _C_CENT2_INCLUDE

#include "fastpack.hpp"

// SPECIFICATION:    centrifugeControl class, controls centrifuge RPM
//                   uses control  hardware
//
// ERROR HANDLING:   none.

class centrifugeControl
{
public:
   centrifugeControl(void);
   virtual ~centrifugeControl();                        // destructor

   void newOrder (float rpm,                    // new rpm order
                  float upRate,                 // up rate rpm/s (w/ rpm>1000)
                  float downRate);              // down rate rpm/s

   void update (long rawEncoderRate,             // last actual value
                unsigned short openLoop,         // open/closed loop
                bool power,                      // power avail
                unsigned short& errors);         // integral error


private:
   centrifugeControl (const centrifugeControl&);   // Hide copy constructor.  Bad idea to copy this class.
   centrifugeControl& operator = (const centrifugeControl& rhs);

private:
   long encoder_to_rpm (long encoderRate);      // convert to rpm based on HW
   long  _internalOrder;                        // ramping order
   long  _integral;                             // I
   long  _integralGain;                         // Gail for the I factor
   long  _order;                                // final order
   long  _upRate;                               // rpm ramp rate rpm/s
   long  _downRate;                             // rpm ramp rate rpm/s
   short _startI;                               // timer
   long  _aveEncoderRate;                       // filtered actual readings
   short _rampCounter;                          // ramp rate counter
                                                // HW has different encoders
   long  _errorCounter;                         // Used to debounce integral errors
   long  _integratorLimit;                      // bounding limit for integrator (varies with speed)
   long  _integratorDebounceCount;              // cycles (10ms) for which the integrator
                                                // error must persist before alarm (varies with speed)

   fastpacklong* _fastInternalOrder;            // 10ms data
   fastpacklong* _fastDeltaRPM;                 // 10ms data
   fastpacklong* _fastIntegral;                 // 10ms data
   fastpacklong* _fastControl;                  // 10ms data
   fastpacklong* _fastErrors;                   // 10ms data

};

#endif /* ifndef _C_CENT2_INCLUDE */

/* FORMAT HASH a7f448c68dde53b6608dcf7dedd586d5 */

/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      c_cent2.cpp, Focussed System centrifuge control
 *
 * ABSTRACT:   Interface to the control hardware centrifuge
 *
 */

#include <vxWorks.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "c_cent2.h"
#include "chw.hpp"
#include "ctl_hw_private.h"
#include "trima_datalog.h"

// constants
const long  FAST_RAMP = 100;                          // RPM/sec. below the SLOW_RAMP_RPM we use thie ramp rate
const long  SLOW_RAMP = 20;                           // RPM/sec. above the SLOW_RAMP_RPM we use this ramp rate
const long  SLOW_RAMP_RPM                     = 1000; // speed to switch to slow ramp

const float NEW_ORDER_DELTA                   = 105.0f; // new order delta
const long  MIN_CONTROL_SPEED                 = 199;    // minimum control speed, rpm
const long  MIN_TEST_SPEED                    = 50;     // minimum speed for test purporses
const long  SLOW_SPEED_ORDER                  = 300;    // slow order for control loop

const long  I_LIMIT                           = 150 * 500; // integrator limit = (1/Gain) * rpm
const long  LOW_SPEED_I_LIMIT                 = 150 * 100; // integrator limit for < 300 RPM
const long  INTEGRAL_DEBOUNCE_COUNT           = 250;       // error must occur for 25 consecutive seconds
const long  LOW_SPEED_INTEGRAL_DEBOUNCE_COUNT = 20;        // error must occur for 2 consecutive seconds

const long  INTEGRAL_HOLD_TIME                = 0 * 20; // was 20 seconds because the integrator was only used
                                                        // in steady state.  This left time for the control loop
                                                        // to settle down, but now the integrator is used for
                                                        // the entire run so this is currently 0.

const long CONTROL_OFFSET = 250;          // control offset, rpm
const long CONTROL_MAX    = 4095;         // max control value


// SPECIFICATION:    centrifugeControl constructor
//                   uses control  hardware
//
// ERROR HANDLING:   none.

centrifugeControl::centrifugeControl(void)
   : _internalOrder(0),
     _integral(0),
     _order(0),
     _startI(0),
     _aveEncoderRate(0),
     _rampCounter(0),
     _errorCounter(0),
     _upRate(0),
     _downRate(0),
     _integralGain(50),
     _integratorLimit(I_LIMIT),
     _integratorDebounceCount(INTEGRAL_DEBOUNCE_COUNT)
{
   _fastInternalOrder = new fastpacklong(CENT_INTERNAL_ORDER, "centInternalOrder");
   _fastDeltaRPM      = new fastpacklong(CENT_DELTA_RPM, "centDeltaRPM");
   _fastIntegral      = new fastpacklong(CENT_INTEGRAL, "centIntegral");
   _fastControl       = new fastpacklong(CENT_CONTROL, "centControl");
   _fastErrors        = new fastpacklong(CENT_ERRORS, "centErrors");
}


// SPECIFICATION:    centrifugeControl destructor
//
// ERROR HANDLING:   none.

centrifugeControl::~centrifugeControl()
{
   delete _fastInternalOrder;
   delete _fastDeltaRPM;
   delete _fastIntegral;
   delete _fastControl;
   delete _fastErrors;
}



// SPECIFICATION:    convert centrifuge encoders to rpm based on
//                   the hardware type. (Beta2.2 has 18 ticks/rev.
//                   Beta2.4 and Beta2.6 have 24 ticks/rev.)
// ERROR HANDLING:   none

long centrifugeControl::encoder_to_rpm (long encoderRate)
{
   static const long FACTOR = 60 / centCountsPerRev;

   return FACTOR * encoderRate;
}

// SPECIFICATION:    centrifugeControl process new commanded rpm
//
// ERROR HANDLING:   none.

void centrifugeControl::newOrder (float rpm, float upRate, float downRate)
{
   // if order changes, adjust for speed ramping
   // also lower the integrator limits so we find a "caught loop" faster
   if ( (rpm < SLOW_SPEED_ORDER) && ( _internalOrder < SLOW_SPEED_ORDER) )
   {
      _startI                  = 0;
      _integratorLimit         = LOW_SPEED_I_LIMIT;
      _integratorDebounceCount = LOW_SPEED_INTEGRAL_DEBOUNCE_COUNT;
   }
   else if (rpm >= SLOW_SPEED_ORDER)
   {
      _integratorLimit         = I_LIMIT;
      _integratorDebounceCount = INTEGRAL_DEBOUNCE_COUNT;

   }

   if (fabs(_order - rpm) > NEW_ORDER_DELTA)
   {
      _startI = INTEGRAL_HOLD_TIME;   // delay to allow closed loop settling
   }

   _order = (int)rpm;               // set new order

// process rate values

   upRate = fabs(upRate);

   if (upRate > 0)               // valid rate?
   {
      _upRate = (int)upRate;     // save it
                                 //
      if (_upRate == 0)          // if truncated to zero
      {
         _upRate = 1;            // set to smallest value
      }
   }
   else
   {
      _upRate = 0;
   }

// process rate values

   downRate = fabs(downRate);

   if (downRate > 0)               // valid rate?
   {
      _downRate = (int)downRate;   // save it
                                   //
      if (_downRate == 0)          // if truncated to zero
      {
         _downRate = 1;            // set to smallest value
      }
   }
   else
   {
      _downRate = 0;
   }
}

// SPECIFICATION:    centrifugeControl update function (0.1 seconds)
//
// ERROR HANDLING:   none.

void centrifugeControl::update (long encoderRate,
                                unsigned short openLoop,
                                bool power,
                                unsigned short& errors)
{
   long control;
   long ramp;
   long errorSnap;
   long deltaRpm = 0;

   errors &= ~HW_CENT_INTEGRAL_ERROR;

   // set ramp rate to a reasonable value in RPM/s

   if (_order < _internalOrder)
   {
      if (_downRate != 0)
      {
         ramp = _downRate;
      }
      else
      {
         // use appropriate default
         if (_internalOrder > SLOW_RAMP_RPM)
            ramp = SLOW_RAMP;
         else
            ramp = FAST_RAMP;
      }
   }
   else
   {
      if (_upRate != 0)
      {
         ramp = _upRate;
      }
      else
      {
         // use appropriate default
         if (_internalOrder > SLOW_RAMP_RPM)
            ramp = SLOW_RAMP;
         else
            ramp = FAST_RAMP;
      }
   }

// ramp internal order

   if ( (_order <= 0) || !power)  // reset from 0
   {
      _internalOrder = 0;
      _integral      = 0;
      _startI        = INTEGRAL_HOLD_TIME;
   }
   else if ((_rampCounter++) % 10 == 0)        // once a second
   {
      if (_order < _internalOrder)        // ramp down
      {
         _internalOrder -= ramp;

         if (_order >= _internalOrder)    // at final value?
         {
            _internalOrder = _order;
         }
      }
      // ramp down
      else if (_order > _internalOrder)
      {
         _internalOrder += ramp;

         if (_order <= _internalOrder)    // at final value?
         {
            _internalOrder = _order;
         }
      }
   }

// slope=1 + offset equals correction

   if (_internalOrder >= MIN_CONTROL_SPEED)
   {
      control = _internalOrder + MIN_CONTROL_SPEED;
   }
   else if ( (_internalOrder < MIN_CONTROL_SPEED)
             &&(_internalOrder > MIN_TEST_SPEED) )
   {
      control = _internalOrder + MIN_TEST_SPEED;
   }
   else
   {
      control = 0;
   }

// time delay between ramp and PI control

   if ((_internalOrder == _order) && (_startI > 0))
   {
      _startI--;
   }

// closed loop control

   if ((openLoop==0) &&                   // closed loop control
       (_internalOrder>= MIN_TEST_SPEED)) // order above threshold
   {

      // how are we doing compared to where we want to be?
      deltaRpm = ( _internalOrder - (encoder_to_rpm(encoderRate)));

      if (_startI<=0)                     // after time delay
      {
         // accumulate integral error but limit it to prevent wild swings
         _integral += deltaRpm;

         if (_integral > _integratorLimit)
         {
            if (_errorCounter++ >= _integratorDebounceCount)
            {
               errors |= HW_CENT_INTEGRAL_ERROR;

               // only log the first occurrence
               if (_errorCounter == _integratorDebounceCount)
                  DataLog_Default << "centrifuge positive integral limit exceeded: integral=" << _integral << endmsg;
            }

            _integral = _integratorLimit;
         }
         else if (_integral < -_integratorLimit)
         {
            if (_errorCounter++ >= _integratorDebounceCount)
            {
               errors |= HW_CENT_INTEGRAL_ERROR;

               // only log the first occurrence
               if (_errorCounter == _integratorDebounceCount)
                  DataLog_Default << "centrifuge negative integral limit exceeded: integral=" << _integral << endmsg;
            }
            _integral = -_integratorLimit;
         }
         else
         {
            // clear the error if it quiets down
            _errorCounter = 0;
            errors       &= ~HW_CENT_INTEGRAL_ERROR;
         }

         // lets take a second look at the integral now and see if we want to shed some of
         // the error to prevent overshoot due to a large accumulated error

         // if we are supposed to be in steady state at a reasonable target
         // and the integrator has accumulated more than 50% of its maximum value
         if ( (_internalOrder == _order)
              &&(_internalOrder >= SLOW_SPEED_ORDER)
              &&( (labs(_integral)) > (labs(_integratorLimit) / 2) ) )
         {
            // if we are over the target point with a big positive integrator error
            // or if we under the target point with a big negative integrator error
            if ( ( (deltaRpm < 0) && (_integral > 0) )
                 ||( (deltaRpm > 0) && (_integral < 0) ) )
            {
               // then shed 60% of the error before we overshoot
               DataLog_Default << "shedding accumulated centrifuge integral error: integral=" << _integral << endmsg;
               _integral = ( (_integral / 10) * 4);
            }

         }
      }

// during ramping, increase P gain

      if ( labs(_internalOrder - _order) > NEW_ORDER_DELTA)
      {
         deltaRpm *= 4;
      }

// compute control value using P I control

      control += ( deltaRpm / 2 + _integral / _integralGain);

      if (control < 0)
      {
         control = 0;
      }
      else if (control > CONTROL_MAX)
      {
         DataLog_Default << "centrifuge control limit exceeded: control=" << control << endmsg;
         control = CONTROL_MAX;
      }
   }

   chw_centSetCommand(control);

   _fastInternalOrder->dataPoint(_internalOrder);
   _fastDeltaRPM->dataPoint(deltaRpm);
   _fastIntegral->dataPoint(_integral);
   _fastControl->dataPoint(control);
   errorSnap = (long)errors;
   _fastErrors->dataPoint(errorSnap);
}

/* FORMAT HASH 850dfd8c6dd5e629fdbe488f73bb6115 */

/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      Focussed System pump controls
 *
 * ABSTRACT:   Interface to the control hardware pumps and return pump
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 * PUBLIC FUNCTIONS:
 * main() - main entry point
 */

// #includes
#include <vxWorks.h>

#include "c_pump.hpp"
#include "trima_datalog.h"

// local data
// control loop calculation time base
//    The control loop calculations are done on each sample but the sample
//    is 'magnified' to look as if it happened for the CALCULATION_PERIOD_IN_MS
//    instead of for the smaller sample period.  This is done to get the numbers
//    big enough to have some effect on the loop when the pump is going at slow speed.
static const float CALCULATION_PERIOD_IN_MS = 100.0f;
// encoder ticks/sec to RPM conversion
// 1 pump RPM is equivalent to 1 motor rev in ticks (512 * 4) times the
//    gear ratio between the motor and pump (10.0625) divided by
//    60 seconds/minute.  The final units are ticks/sec.  The const
//    ENCODER_TO_RPM is one over this number.
static const float ENCODER_TO_RPM = 60.0f / (512.0f * 4.0f * 10.0625f);
// error limit in ticks/CALCULATION_PERIOD_IN_MS
//    The integrator and volume error are limited to +/- I_LIMIT
static const float I_LIMIT = ( (200.0 / (1000.0 / CALCULATION_PERIOD_IN_MS) ) / ENCODER_TO_RPM);

// common values
static const float REV_CONVERT       = 2. / (10.0625 * 4096. );
static const long  MAX_ENCODER       = 4096;
static const long  MAX_SPEED         = 250;
static const int   MS                = 1000;
static const int   US                = 1000000;
static const long  SAMPLES_IN_REPORT = 8;
static const int   COUNTS_PER_VOLT   = 91;
static const int   FIFTEEN_VOLTS     = 15 * COUNTS_PER_VOLT;

// pump adjustable gain parameters
static const long P_GAIN = 6;
static const long I_GAIN = 6;
static const long V_GAIN = 5;

// ramp value for pump orders for the four small pumps
static const long PUMP_ENCODER_RAMP   = (long)(50000 * (CALCULATION_PERIOD_IN_MS) / 1000);
// ramp value for pump orders for the return pump
static const long R_PUMP_ENCODER_RAMP = (long)(50000 * (CALCULATION_PERIOD_IN_MS) / 1000);

// error count limits
static const short I_ERROR_COUNT_LIMIT     = 40; // approximately 200ms
static const short I_RTN_ERROR_COUNT_LIMIT = 45; // approximately 250ms
static const short S_ERROR_COUNT_LIMIT     = 40;
static const short V_ERROR_COUNT_LIMIT     = 40;

//
// static data for the pump class (see c_pump.hpp for more info)
//
bool       pump::_safetyOrderDataLinked = FALSE;
SHwOrders* pump::_safetyOrders          = NULL;

// SPECIFICATION:    pump control constructor
//
// ERROR HANDLING:   none.

pump::pump(HWPump select, long dt, CHwOrders& orderData, CHwStates& statusData)
   : _select(select), _dt(dt), _encoderSpeed(0), _reportedSpeed(0), _revs(0), _errors(0),
     _encoderOrder(0), _encoderOrderRamp(0), _fspeedError(0.0),
     _fdeltaSpeed(0.0), _cummulativeError(0), _fvolumeError(0.0),
     _fAvgSpeedError(0.0), _iAvgSpeedErrorCount(0),
     _orderData(orderData), _statusData(statusData),
     _pGain(P_GAIN), _iGain(I_GAIN), _vGain(V_GAIN),
     _iErrorCount(0), _vErrorCount(0), _sErrorCount(0)
{
   // create pump fast data log objects
   switch ( _select )
   {
      case hw_acPump :
         _fastEncoders = new fastpacklong(AC_PUMP_ENCODER, "acPumpEncoders");
         _fastOrders   = new fastpacklong(AC_PUMP_ORDER, "acPumpOrders");
         _fastIntError = new fastpacklong(AC_PUMP_INTEGRATION_ERROR, "acPumpIntErr");
         _fastVolError = new fastpacklong(AC_PUMP_VOLUME_ERROR, "acPumpVolErr");
         _fastDeltaSpd = new fastpacklong(AC_PUMP_DELTA_SPEED, "acPumpDeltaSpd");
         _fastFault    = new fastpacklong(AC_PUMP_ERRORS, "acPumpErrors");
         _fastSpdOrder = new fastpacklong(AC_PUMP_SPEED_ORDER, "acPumpSpdOrder");
         break;

      case hw_inletPump :
         _fastEncoders = new fastpacklong(INLET_PUMP_ENCODER, "inletPumpEncoders");
         _fastOrders   = new fastpacklong(INLET_PUMP_ORDER, "inletPumpOrders");
         _fastIntError = new fastpacklong(INLET_PUMP_INTEGRATION_ERROR, "inletPumpIntErr");
         _fastVolError = new fastpacklong(INLET_PUMP_VOLUME_ERROR, "inletPumpVolErr");
         _fastDeltaSpd = new fastpacklong(INLET_PUMP_DELTA_SPEED, "inletPumpDeltaSpd");
         _fastFault    = new fastpacklong(INLET_PUMP_ERRORS, "inletPumpErrors");
         _fastSpdOrder = new fastpacklong(INLET_PUMP_SPEED_ORDER, "inletPumpSpdOrder");
         break;

      case hw_plasmaPump :
         _fastEncoders = new fastpacklong(PLASMA_PUMP_ENCODER, "plasmaPumpEncoders");
         _fastOrders   = new fastpacklong(PLASMA_PUMP_ORDER, "plasmaPumpOrders");
         _fastIntError = new fastpacklong(PLASMA_PUMP_INTEGRATION_ERROR, "plasmaPumpIntErr");
         _fastVolError = new fastpacklong(PLASMA_PUMP_VOLUME_ERROR, "plasmaPumpVolErr");
         _fastDeltaSpd = new fastpacklong(PLASMA_PUMP_DELTA_SPEED, "plasmaPumpDeltaSpd");
         _fastFault    = new fastpacklong(PLASMA_PUMP_ERRORS, "plasmaPumpErrors");
         _fastSpdOrder = new fastpacklong(PLASMA_PUMP_SPEED_ORDER, "plasmaPumpSpdOrder");
         break;

      case hw_plateletPump :
         _fastEncoders = new fastpacklong(PLATELET_PUMP_ENCODER, "plateletPumpEncoders");
         _fastOrders   = new fastpacklong(PLATELET_PUMP_ORDER, "plateletPumpOrders");
         _fastIntError = new fastpacklong(PLATELET_PUMP_INTEGRATION_ERROR, "plateletPumpIntErr");
         _fastVolError = new fastpacklong(PLATELET_PUMP_VOLUME_ERROR, "plateletPumpVolErr");
         _fastDeltaSpd = new fastpacklong(PLATELET_PUMP_DELTA_SPEED, "plateletPumpDeltaSpd");
         _fastFault    = new fastpacklong(PLATELET_PUMP_ERRORS, "plateletPumpErrors");
         _fastSpdOrder = new fastpacklong(PLATELET_PUMP_SPEED_ORDER, "plateletPumpSpdOrder");
         break;

      case hw_returnPump :
         _fastEncoders = new fastpacklong(RETURN_PUMP_ENCODER, "returnPumpEncoders");
         _fastOrders   = new fastpacklong(RETURN_PUMP_ORDER, "returnPumpOrders");
         _fastIntError = new fastpacklong(RETURN_PUMP_INTEGRATION_ERROR, "returnPumpIntErr");
         _fastVolError = new fastpacklong(RETURN_PUMP_VOLUME_ERROR, "returnPumpVolErr");
         _fastDeltaSpd = new fastpacklong(RETURN_PUMP_DELTA_SPEED, "returnPumpDeltaSpd");
         _fastFault    = new fastpacklong(RETURN_PUMP_ERRORS, "returnPumpErrors");
         _fastSpdOrder = new fastpacklong(RETURN_PUMP_SPEED_ORDER, "returnPumpSpdOrder");
         break;

      default :
         DataLog(log_level_critical) << "Bad pump ID " << (int)select << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad pump ID");
   }

   osTime::snapshotRawTime(_lastTime);
   _lastEncoder = chw_pumpGetEncoder(_select);
}

// SPECIFICATION:    pump control destructor
//
// ERROR HANDLING:   none.

pump::~pump()
{
   delete _fastEncoders;
   delete _fastOrders;
   delete _fastIntError;
   delete _fastVolError;
   delete _fastDeltaSpd;
   delete _fastFault;
   delete _fastSpdOrder;
}

// SPECIFICATION:    pump control, get internal values for 10ms data messages
//
// ERROR HANDLING:   none.

void pump::internals (long spdOrder)
{
   long errorSnap;
// put 10ms data in shared memory
   _fastEncoders->dataPoint(_encoderSpeed);
   _fastOrders->dataPoint(_encoderOrder);
   _fastIntError->dataPoint((long)_fspeedError);
   _fastVolError->dataPoint((long)_fvolumeError);
   _fastDeltaSpd->dataPoint((long)_fdeltaSpeed);

   errorSnap = (long)_errors;
   if (chw_pumpGetStatus(_select) & chw_pumpFault)
      errorSnap |= (long)CHW_MOTOR_FAULT;
   else
      errorSnap &= (long)~CHW_MOTOR_FAULT;

   _fastFault->dataPoint(errorSnap);
   _fastSpdOrder->dataPoint(spdOrder);
}


// SPECIFICATION:    pump control get actual values
//
// ERROR HANDLING:   none.

void pump::actuals (PumpStatus* p)
{
   p->errors        = _errors;
   p->RPM           = (float)_reportedSpeed * ENCODER_TO_RPM;
   p->Revs          = REV_CONVERT * _revs;
   p->Accum        += _revs;
   p->Integral      = (long)_fspeedError;
   p->AcummIntegral = _cummulativeError;
   _revs            = 0;
}

// SPECIFICATION:    pump control - process new order value
//
// ERROR HANDLING:   none.

void pump::order (float rpm)
{
   long oldEncoderOrder = _encoderOrder;
   // _encoderOrder in ticks/CALCULATION_PERIOD_IN_MS
   _encoderOrder = (long)((rpm / ENCODER_TO_RPM) / (1000 / CALCULATION_PERIOD_IN_MS));
   if ( (_encoderOrder == 0) || (oldEncoderOrder != _encoderOrder)  )
   {
      _fspeedError         = 0.0;
      _fvolumeError        = 0.0;               // clear volume error
      _errors              = 0;                 // clear errors

      _fAvgSpeedError      = 0.0;               // clear average data for integral error
      _iAvgSpeedErrorCount = 0;
   }
}


// SPECIFICATION:    pump control update, computes speed difference and adjusts
//                   order values using a "PI" controller.
//
// ERROR HANDLING:   none.

void pump::update (BOOL powerOn)
{
   short enc = chw_pumpGetEncoder(_select);
   long  de  = enc - _lastEncoder;        // encoder difference
   long  spdOrder;                        // speed order to hardware
   _lastEncoder = enc;                    // update last value

   if (de < -MAX_ENCODER / 8)               // wrap for overflow, but preserve small negative changes
   {
      de += MAX_ENCODER;
   }
   _revs += de;

// compute encoder speed in ticks/CALCULATION_PERIOS_IN_MS

   long dt = computeEncoderSpeed(de);

// check status of pump voltage
   if (checkPumpVoltageFault())
   {
      if (_encoderOrderRamp != 0 || _encoderOrder != 0)
      {
         _errors |= CHW_MOTOR_FAULT;
      }
   }

// test for zero speed

   if (    ( powerOn == FALSE )
           || ( (_encoderOrderRamp==0) && (_encoderOrder==0) )
           )
   {
      chw_pumpSetCommand(_select, 0);
      osTime::snapshotRawTime(_lastTime);

      _fdeltaSpeed         = 0.0;               // clear P
      _fspeedError         = 0.0;               // clear integrator
      _fvolumeError        = 0.0;               // clear volume error
      _errors              = 0;                 // clear errors

      _fAvgSpeedError      = 0.0;               // clear average data for integral error
      _iAvgSpeedErrorCount = 0;

      _lastEncoder         = chw_pumpGetEncoder(_select);

// get 10ms data for fastdata messages
      internals(0);

      return;
   }

   if ( dt <= 0 )
   {
      // Skip control calculations on short update cycles
      return;
   }

// compute and limit volume errors

   computeVolumeError(dt, de);

// ramp order to final value

   rampOrder(PUMP_ENCODER_RAMP);

// set dynamic gains

   setDynamicGains();

// compute integral based on 24 volt supply

   computeIntegral(dt, de);

// compute speed order using offset + proportional + integral

   if ( _orderData.openLoop == 1)             // open loop
   {
      // encoderOrder is in ticks/CALCULATION_PERIOD_IN_MS
      // spdOrder is in ticks/sec and converted to RPM
      spdOrder             = (long)((_encoderOrder * (1000 / CALCULATION_PERIOD_IN_MS) ) * ENCODER_TO_RPM);
      _fspeedError         = 0.0;

      _fAvgSpeedError      = 0.0;               // clear average data for integral error
      _iAvgSpeedErrorCount = 0;
   }
   else                                   // closed loop
   {
      // This is the implementation of the closed loop control.
      // The _encoderOrder, which has been converted from RPM to
      // ticks/CALCULATION_PERIOD_IN_MS, is adjusted by adding
      // components which compensate for the amount that we are
      // off from where we want to be.  _deltaSpeed
      // compensates for the amount we are currently deviating
      // from the requested speed.  _speedError compensates
      // for the cummulative amount which we are deviating
      // from the requested speed.  _volumeError compensates
      // for the cummulative amount which we are deviating
      // from the from the expected encoder ticks in a sampling
      // period.
      //
      // encoderOrderRamp is in ticks/CALCULATION_PERIOD_IN_MS
      // spdOrder is in ticks/sec and later converted to RPM
      spdOrder =
         (long)((_encoderOrderRamp * 1000 / CALCULATION_PERIOD_IN_MS) +
                _pGain * (long)_fdeltaSpeed +
                _iGain * (long)_fspeedError  +
                _vGain * (long)_fvolumeError );
      spdOrder = (long)(spdOrder * ENCODER_TO_RPM); // convert to rpm
   }

// limit orders

   limitOrder(spdOrder);
   chw_pumpSetCommand(_select, spdOrder);

// get 10ms data for fastdata messages
   internals(spdOrder);
}



// SPECIFICATION:    compute encoder speed, actual pump speed
//
// ERROR HANDLING:   none.

long pump::computeEncoderSpeed (long de)
{
   // compute encoder speed in ticks/sec
   int  dt;
   long currentReportedSpeed;

   dt = osTime::howLongMicroSecAndUpdate(_lastTime);

   if ( dt > 0 )
   {
      // _encoder speed in ticks/CALCULATION_PERIOD_IN_MS
      _encoderSpeed        = (long)(( _encoderSpeed + (de * 1000 * CALCULATION_PERIOD_IN_MS / dt) ) / 2);
      // currentReportedSpeed and _reportedSpeed are in ticks/sec
      currentReportedSpeed = (long)(_encoderSpeed * (1000 / CALCULATION_PERIOD_IN_MS));
      _reportedSpeed       = ( (_reportedSpeed * (SAMPLES_IN_REPORT - 1))
                               + currentReportedSpeed ) / SAMPLES_IN_REPORT;
   }

   return(dt);
}


// SPECIFICATION:    set and limit dynamic gains
//
// ERROR HANDLING:   none.


void pump::setDynamicGains ()
{
   // proportional gain ranges from P_GAIN at 1 RPM to P_GAIN+5 at 250 RPM
   _pGain = (long)(P_GAIN +
                   ( (labs(_encoderOrderRamp) * 1000 / CALCULATION_PERIOD_IN_MS * ENCODER_TO_RPM) * 5 / 250));
   // integrator gain ranges from I_GAIN at 1 RPM to I_GAIN+2 at 250 RPM
   _iGain = (long)(I_GAIN +
                   ( (labs(_encoderOrderRamp) * 1000 / CALCULATION_PERIOD_IN_MS * ENCODER_TO_RPM) * 2 / 250));
   // volume gain is constant at V_GAIN
   _vGain = V_GAIN;

}

// SPECIFICATION:    compute integral based on 24 volt supply
//
// ERROR HANDLING:   none.

void pump::computeIntegral (long dt, long de)
{
   if (_statusData.twentyFourVoltSwitched > FIFTEEN_VOLTS)
   {
      // _deltaSpeed represents the difference between the RPM rate
      // we are currently requesting (_encoderOrderRamp) and the
      // RPM rate that is currently being sensed by the encoder on
      // on the motor shaft.  de is in ticks.  dt is in microseconds.
      // _deltaSpeed, _encoderOrderRamp, _speedError and
      // _cummulativeError are in ticks/CALCULATION_PERIOD_IN_MS.
      // _speedError is the cummulative  error that is tracked as the
      // actual speed swings on both sides of the requested speed.
      // When _deltaSpeed is positive, the pump is too slow.  When
      // _deltaSpeed is negative, the pump is too fast.  This should
      // damp out after a speed change unless the control circuitry
      // or the encoder on the motor shaft are flaky.  Or, of course,
      // if the software control loop is incorrect.
      // Calculate deltaSpeed as a float or you may lose necessary
      // precision at low speeds.
      _fdeltaSpeed       = (long)((float)_encoderOrderRamp
                                  -  ( ((float)de * 1000.0 * CALCULATION_PERIOD_IN_MS) / (float)dt));
      _fspeedError      += _fdeltaSpeed;
      _cummulativeError += (long)_fdeltaSpeed;

      // reset the speed error if the order speed is 0.
      if (_encoderOrder == 0)
         _fspeedError = 0.0;

      // limit the speed error if it exceeds the error limits
      if (_fspeedError > I_LIMIT)
      {
         _iErrorCount++;
         _fspeedError = I_LIMIT;
      }
      else if (_fspeedError < -I_LIMIT)
      {
         _iErrorCount++;
         _fspeedError = -I_LIMIT;
      }
      else
      {
         _iErrorCount = 0;
      }

      _iAvgSpeedErrorCount += 1;
      _fAvgSpeedError      += _fspeedError;

      if (_iErrorCount > I_ERROR_COUNT_LIMIT)
      {
         _errors |= CHW_PUMP_INTEGRAL_ERROR;
      }
      else
      {
         _errors &= ~CHW_PUMP_INTEGRAL_ERROR;
      }
   }
   else
   {
      _fspeedError         = 0.0;
      _encoderSpeed        = 0;
      _encoderOrderRamp    = 0;
      _fdeltaSpeed         = 0.0;
      _iErrorCount         = 0;
      _errors              = 0;

      _fAvgSpeedError      = 0.0;
      _iAvgSpeedErrorCount = 0;
   }
}


// SPECIFICATION:    compute volume error
//
// ERROR HANDLING:   none.

void pump::computeVolumeError (long dt, long de)
{
   if (_statusData.twentyFourVoltSwitched > FIFTEEN_VOLTS)
   {
      // _volumeError is calculated by taking the amount of encoder
      // ticks which we would expect to see in a given time and
      // subtracting the amount of ticks which we did see in that time.
      // _encoderOrderRamp is in ticks/CALCULATION_PERIOD_IN_MS.  dt is in
      // microseconds.  de is the number of ticks that occurred in dt
      // microseconds.  We normalize the encoderOrderRamp to the number of
      // ticks that should have occurred in dt microseconds before
      // subtracting de.
      // If volumeError is negative, the delivery is too fast.  If volumeError
      // is positive, the delivery is too slow.  Just as with the speedError,
      // _volumeError is cummulative and should not grow too big unless
      // the control circuitry or the pump encoder are flaky.
      // Do this calculation as a float or you may lose precision at low speeds
      _fvolumeError += ((float)_encoderOrderRamp * 1000.0 / CALCULATION_PERIOD_IN_MS * (float)dt / 1000000.0 )
                       - (float)de;

      // reset the volume error if the order speed is 0.
      if (_encoderOrder == 0)
         _fvolumeError = 0.0;

      // limit the volume error if it exceeds the error limits
      if (_fvolumeError > I_LIMIT)
      {
         _vErrorCount++;
         _fvolumeError = I_LIMIT;
      }
      else if (_fvolumeError < -I_LIMIT)
      {
         _vErrorCount++;
         _fvolumeError = -I_LIMIT;
      }
      else
      {
         _vErrorCount = 0;
      }

      if (_vErrorCount > V_ERROR_COUNT_LIMIT)
      {
         _errors |= CHW_PUMP_VOLUME_ERROR;
      }
      else
      {
         _errors &= ~CHW_PUMP_VOLUME_ERROR;
      }
   }
   else
   {
      _fvolumeError     = 0;
      _encoderSpeed     = 0;
      _encoderOrderRamp = 0;
      _vErrorCount      = 0;
      _errors           = 0;
   }

}

// SPECIFICATION:    ramp pump order value
//
// ERROR HANDLING:   none.

void pump::rampOrder (long slope)
{
   if (_encoderOrderRamp < _encoderOrder)
   {
      _encoderOrderRamp += slope;
      if (_encoderOrderRamp > _encoderOrder)
      {
         _encoderOrderRamp = _encoderOrder;
      }
   }
   else if (_encoderOrderRamp > _encoderOrder)
   {
      _encoderOrderRamp -= slope;
      if (_encoderOrderRamp < _encoderOrder)
      {
         _encoderOrderRamp = _encoderOrder;
      }
   }
}

// SPECIFICATION:    limit final pump order value
//
// ERROR HANDLING:   none.

void pump::limitOrder (long& spdOrder)
{
   if (_statusData.twentyFourVoltSwitched > FIFTEEN_VOLTS)
   {
      if (spdOrder > MAX_SPEED)              // can't maintain speed
      {
         _sErrorCount++;
         spdOrder = MAX_SPEED;
      }
      else if (spdOrder < 0)                 // prevent underflows
      {
         _sErrorCount++;
         spdOrder = 0;
      }
      else
      {
         _sErrorCount = 0;
      }

      if (_sErrorCount > S_ERROR_COUNT_LIMIT)
      {
         _errors |= CHW_PUMP_FAIL_TO_MAINTAIN_SPEED;
      }
      else
      {
         _errors &= ~CHW_PUMP_FAIL_TO_MAINTAIN_SPEED;
      }
   }
   else
   {
      spdOrder          = 0;
      _encoderSpeed     = 0;
      _encoderOrderRamp = 0;
      _sErrorCount      = 0;
      _errors           = 0;
   }

}

// SPECIFICATION:    monitor pump voltage
//
// ERROR HANDLING:   sets CHW_MOTOR_FAULT bit in the event of an unexpected
//                   drop in pump voltage

BOOL pump::checkPumpVoltageFault ()
{
   static rawTime voltageAlarmTime;
   static BOOL    voltageAlarmDelayStarted = FALSE;
   static BOOL    voltageFaultDetected     = FALSE;
   static int     debugTrace               = -1;
   enum { VoltageAlarmDelay = 2000 };

   if (_statusData.twentyFourVoltSwitched > FIFTEEN_VOLTS)
   {
      //
      // Pump voltage is within the acceptable range
      //
      voltageFaultDetected     = FALSE;
      voltageAlarmDelayStarted = FALSE;

      if (debugTrace != 1)
      {
         DataLog(log_level_control_alarm_detail) <<
            "pump 24V supply reached nominal: current mV=" << (float)_statusData.twentyFourVoltSwitched / (float)COUNTS_PER_VOLT <<
            " nominal mV=" << (float)FIFTEEN_VOLTS / (float)COUNTS_PER_VOLT << endmsg;
         debugTrace = 1;
      }
   }
   else if (_safetyOrderDataLinked && (_safetyOrders->pumpPower == HW_ENABLE) )
   {
      //
      // Pump voltage is outside of acceptable range and the power
      // supply was not disabled by the safety controller
      //
      if (!voltageAlarmDelayStarted)
      {
         osTime::snapshotRawTime(voltageAlarmTime);
         voltageAlarmDelayStarted = TRUE;
      }

      int dt = osTime::howLongMilliSec(voltageAlarmTime);
      if (dt > VoltageAlarmDelay)
      {
         //
         // Power supply has dropped for longer than VoltageAlarmDelay.
         // Report that a voltage fault has been detected.
         //
         voltageFaultDetected = TRUE;
         if (debugTrace != 2)
         {
            DataLog(log_level_control_alarm_detail) <<
               "Pump 24V supply fault: current mV=" << _statusData.twentyFourVoltSwitched << endmsg;
            debugTrace = 2;
         }
      }
      else
      {
         //
         // Still waiting for power supply timeout.
         //
         if (debugTrace != 3)
         {
            DataLog(log_level_control_alarm_detail) <<
               "Loss of pump 24V supply: current mV=" << _statusData.twentyFourVoltSwitched << endmsg;
            debugTrace = 3;
         }
      }
   }
   else
   {
      //
      // Pump voltage is outside acceptable range because it was
      // switched of by the safety controller.
      //
      voltageFaultDetected = FALSE;
      if (debugTrace != 4)
      {
         DataLog(log_level_control_alarm_detail) <<
            "Pump 24V supply disabled by safety: current mV=" << _statusData.twentyFourVoltSwitched << endmsg;
         debugTrace = 4;
      }
   }

   return voltageFaultDetected;
}


// SPECIFICATION:    return pump control, constructor
//                   2.2 or 2.4 hardware
//
// ERROR HANDLING:   none.

returnPump::returnPump(HWPump select,
                       long dt,
                       CHwOrders& orderData,
                       CHwStates& statusData)
   : pump(select, dt, orderData, statusData)
{
   _pGain = P_GAIN;
   _iGain = I_GAIN;
   _vGain = V_GAIN;
}

// SPECIFICATION:    return pump control, destructor
//
// ERROR HANDLING:   none.

returnPump::~returnPump()
{}

void returnPump::actuals (PumpStatus* p)
{
   if ( _iAvgSpeedErrorCount >= 500 )
   {
// Remove unused logging.
// TO DO:  Remove all of the integral error logic.
//
#if 0
      DataLog_Default << "return pump integral error=" <<
         _fAvgSpeedError / (double)_iAvgSpeedErrorCount << " RPM=" << p->RPM << endmsg;
#endif
      _fAvgSpeedError      = 0.0;
      _iAvgSpeedErrorCount = 0;
   }

   pump::actuals(p);
}

// SPECIFICATION:    return pump control, update loop
//
// ERROR HANDLING:   none.

void returnPump::update (BOOL powerOn)
{

   // process non-zero speeds
   short enc = chw_pumpGetEncoder(_select);
   long  de  = enc - _lastEncoder;        // encoder difference
   long  spdOrder;                        // speed order to hardware
   _lastEncoder = enc;                    // update last value

   // check for hardware wraps
   if (de < 0)
      de += MAX_ENCODER;
   if (de > (MAX_ENCODER / 2))
   {
      de -= MAX_ENCODER;            // try the other way
   }
   _revs += de;

   long dt = computeEncoderSpeed(de);    // compute encoder speed in ticks/sec

// check status of pump voltage
   if (checkPumpVoltageFault())
   {
      if (_encoderOrderRamp != 0 || _encoderOrder != 0)
      {
         _errors |= CHW_MOTOR_FAULT;
      }
   }

// test for zero speed

   if (    ( powerOn == FALSE )
           || ( (_encoderOrderRamp==0) && (_encoderOrder==0) )
           )
   {
      chw_pumpSetCommand(_select, 0);
      osTime::snapshotRawTime(_lastTime);

      _fdeltaSpeed         = 0.0;               // clear P
      _fspeedError         = 0.0;               // clear integrator

      _fAvgSpeedError      = 0.0;
      _iAvgSpeedErrorCount = 0;

      _lastEncoder         = chw_pumpGetEncoder(_select);

// get 10ms data for fastdata messages
      internals(0);

      return;
   }

// set dynamic gains

   setDynamicGains();

// compute and limit volume errors

   computeVolumeError(dt, de);

// ramp order to final value

   rampOrder(R_PUMP_ENCODER_RAMP);

// compute error and integral

   computeIntegral(dt, de);
   if (_iErrorCount > I_RTN_ERROR_COUNT_LIMIT)
   {
      _errors |= CHW_PUMP_INTEGRAL_ERROR;
   }
   else
   {
      _errors &= ~CHW_PUMP_INTEGRAL_ERROR;
   }

// compute speed order using offset + proportional + integral

   long sign = (_encoderOrderRamp >= 0) ? 1 : -1;
   if (_orderData.openLoop == 1)             // open loop
   {
      // _encoderOrder is in ticks/CALCULATION_PERIOD_IN_MS
      // _spdOrder is in ticks/CALCULATION_PERIOD_IN_MS and is
      // later converted to RPM
      spdOrder             = labs((long)(_encoderOrder * 1000 / CALCULATION_PERIOD_IN_MS));
      _fspeedError         = 0.0;

      _fAvgSpeedError      = 0.0;
      _iAvgSpeedErrorCount = 0;
   }
   else                                   // closed loop
   {
      // _encoderOrder, _deltaSpeed, _speedError and _volumeError
      // are in ticks/CALCULATION_PERIOD_IN_MS.
      // _spdOrder is in ticks/CALCULATION_PERIOD_IN_MS and is
      // later converted to RPM
      if (sign == 1)
      {
         spdOrder =
            (long)( (_encoderOrderRamp * 1000 / CALCULATION_PERIOD_IN_MS) +
                    _pGain * (long)_fdeltaSpeed +
                    _iGain * (long)_fspeedError +
                    _vGain * (long)_fvolumeError );
      }
      else
      {
         spdOrder =
            (long)( labs((long)(_encoderOrderRamp * 1000 / CALCULATION_PERIOD_IN_MS)) -
                    _pGain * (long)_fdeltaSpeed -
                    _iGain * (long)_fspeedError -
                    _vGain * (long)_fvolumeError );
      }
   }

   spdOrder = (long)(spdOrder * ENCODER_TO_RPM);  // convert to rpm
   limitOrder(spdOrder);                          // limit speed order
   chw_pumpSetCommand(_select, sign * spdOrder);

// get 10ms data for fastdata messages
   internals(spdOrder);
}



int pump::mV (short reading)
{
   return (int)(1000 * ((float)reading / (float)COUNTS_PER_VOLT));
}

/* FORMAT HASH b8a99040ce82e14f38b3e18fd199ab0a */

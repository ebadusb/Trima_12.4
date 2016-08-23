/*
 * Copyright (C) 2003 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>

#include "valve.hpp"
#include "c_valve.h"
#include "trima_assert.h"
#include "trima_datalog.h"

const int CASSETTE_OVERDRIVE_MSEC = 500;   // millisec to overdrive
const int CASSETTE_TIMEOUT        = 8000;  // ms
const int VALVE_TIMEOUT           = 1400;  // ms
const int VALVE_HOLDOFF_MSEC      = 15;    // msec between one valve completion and next valve start

// SPECIFICATION:    process valves
//
// ERROR HANDLING:   none.

void processValves (valveRunLogic& rbc,
                    valveRunLogic& plasma,
                    valveRunLogic& platelet,
                    cassetteRunLogic& cassette,
                    CHwOrders& _orderData,         // order data
                    CHwStates& _statusData)        // status data
{
   static rawTime lastValveRunTime;
   static bool    nextValveDelayActive = false;

   HWValve        activeValve          = chw_valveGetActive();

   if (activeValve != hw_noValve)
   {
      switch (activeValve)
      {
         case hw_rbcValve :
            rbc.update();
            break;

         case hw_plasmaValve :
            plasma.update();
            break;

         case hw_plateletValve :
            platelet.update();
            break;

         case hw_cassette :
            cassette.update();
            break;

         default :
            // shouldn't be here, reset the valve in an attempt to allow the system to continue
            chw_valveSetCommand(hw_allValves, chw_stopValve);
            break;
      }

      activeValve = chw_valveGetActive();

      if (activeValve == hw_noValve)
      {
         // valve stopped on this cycle, start the delay until next valve command
         // is accepted
         osTime::snapshotRawTime(lastValveRunTime);
         nextValveDelayActive = true;
      }
   }

   if (nextValveDelayActive &&
       osTime::howLongMilliSec(lastValveRunTime) >= VALVE_HOLDOFF_MSEC)
   {
      nextValveDelayActive = false;
   }

   if (activeValve == hw_noValve &&
       !nextValveDelayActive)
   {
      if ((_orderData.rbcValve != (CHW_VALVE_ORDERS)_statusData.rbcValve) &&
          (_orderData.rbcValve != (CHW_VALVE_ORDERS)0))
      {
         // new command for RBC valve
         rbc.newOrder();
      }
      else if ((_orderData.plasmaValve != (CHW_VALVE_ORDERS)_statusData.plasmaValve) &&
               (_orderData.plasmaValve != (CHW_VALVE_ORDERS)0))
      {
         // new command for plasma valve
         plasma.newOrder();
      }
      else if ((_orderData.plateletValve != (CHW_VALVE_ORDERS)_statusData.plateletValve) &&
               (_orderData.plateletValve != (CHW_VALVE_ORDERS)0))
      {
         // new command for plasma valve
         platelet.newOrder();
      }
      else if ((_orderData.cassettePosition != (CHW_CASSETTE_ORDERS)_statusData.cassettePosition) &&
               (_orderData.cassettePosition != (CHW_CASSETTE_ORDERS)0))
      {
         // new command for cassette
         cassette.newOrder();
      }
   }
}



// SPECIFICATION:    valve run constructor
//
// ERROR HANDLING:   none.

valveRunLogic::valveRunLogic(HWValve select,
                             CHW_VALVE_ORDERS& order,
                             HW_VALVE_STATES& vstatus)
   : _select(select),
     _order(order),
     _status(vstatus)
{
   switch (select)
   {
      case hw_rbcValve :
         _valveName = "rbc";
         break;
      case hw_plasmaValve :
         _valveName = "plasma";
         break;
      case hw_plateletValve :
         _valveName = "platelet";
         break;
      default :
         _FATAL_ERROR(__FILE__, __LINE__, "unknown valve");
   }

   memset(&_startTime, 0, sizeof(_startTime));
}

// SPECIFICATION:    valve run destructor
//
// ERROR HANDLING:   none.

valveRunLogic::~valveRunLogic()
{}

// SPECIFICATION:    valve run new order
//
// ERROR HANDLING:   none.
void valveRunLogic::newOrder ()
{
   // initialize
   osTime::snapshotRawTime(_startTime);

   DataLog_Default << "Valve " << _valveName << ": order=" << (int)_order << " status=" << (int)_status << endmsg;

   // determine direction
   CtlHWValveCommand command = chw_stopValve;

   if (_lastOrder == _order &&
       _status    == HW_VALVE_UNKNOWN &&
       _lastCmdTimedOut)
   {
      // last attempt to move to this position timed out without reaching a
      // valid position - try again in the other direction
      command = (_lastCmd == chw_cwValve) ? chw_ccwValve : chw_cwValve;
   }
   else
   {
      switch (_order)
      {
         case CHW_VALVE_COLLECT :
            command = chw_ccwValve;
            break;
         case CHW_VALVE_OPEN :
            command = ( _status==HW_VALVE_COLLECT) ? chw_cwValve : chw_ccwValve;
            break;
         case CHW_VALVE_RETURN :
            command = chw_cwValve;
            break;
         default :
            DataLog(log_level_critical) << "Bad valve order: order=" << (int)_order << endmsg;
            _FATAL_ERROR(__FILE__, __LINE__, "bad valve order");
      }
   }

   _startStatus     = _status;
   _lastOrder       = _order;
   _lastCmd         = command;
   _lastCmdTimedOut = false;

   chw_valveSetCommand(_select, command);
}

// SPECIFICATION:    valve run update
//
// ERROR HANDLING:   none.

void valveRunLogic::update ()
{
   const int dt = osTime::howLongMilliSec(_startTime);

   if (_order == (CHW_VALVE_ORDERS)_status)
   {
      // we're where we want to be, so stop
      chw_valveSetCommand(_select, chw_stopValve);

      DataLog_Default << "Valve " << _valveName << " in position: status="
                      << (int)_status << " time=" << dt << " ms" << endmsg;
   }
   else if (_status != _startStatus &&
            _status != HW_VALVE_UNKNOWN)
   {
      // we're at a valid position, but not the one commanded
      chw_valveSetCommand(_select, chw_stopValve);

      DataLog_Default << "Valve " << _valveName << " in wrong position: status="
                      << (int)_status << " time=" << dt << " ms" << endmsg;
   }
   else if (dt > VALVE_TIMEOUT)
   {
      chw_valveSetCommand(_select, chw_stopValve);

      DataLog_Default << "Valve " << _valveName << " position not attained: order="
                      << (int)_order << " status=" << _status << endmsg;

      _order           = (CHW_VALVE_ORDERS)(_status);
      _lastCmdTimedOut = true;
   }
}

// SPECIFICATION:    cassette run constructor
//
// ERROR HANDLING:   none.

cassetteRunLogic::cassetteRunLogic(CHW_CASSETTE_ORDERS& order,
                                   HW_CASSETTE_STATES& pstatus,
                                   HW_ERROR& errors)
   : _order(order),
     _status(pstatus),
     _errors(errors)
{
   _errors = HW_NO_FAULT;
   memset(&_startTime, 0, sizeof(_startTime));
}

// SPECIFICATION:    cassette run destructor
//
// ERROR HANDLING:   none.

cassetteRunLogic::~cassetteRunLogic()
{}

// SPECIFICATION:    cassette run new order
//
// ERROR HANDLING:   none.

void cassetteRunLogic::newOrder ()
{
   // initialize
   _errors              = HW_NO_FAULT;
   _overdriveInProgress = false;
   osTime::snapshotRawTime(_startTime);

   DataLog_Default << "Cassette: " << " order=" << (int)_order << " status=" << (int)_status << endmsg;

   // set motor orders for position desired
   CtlHWValveCommand command = chw_stopValve;
   _currentOrder = _order;

   switch (_order)
   {
      case CHW_CASSETTE_UP :
         command = chw_ccwValve;
         break;

      case CHW_CASSETTE_DOWN :
         command = chw_cwValve;
         break;

      default :
         DataLog(log_level_critical) << "Bad cassette order: order=" << (int)_order << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "bad cassette order");
   }

   chw_valveSetCommand(hw_cassette, command);
}

// SPECIFICATION:    cassette run update
//
// ERROR HANDLING:   none.

void cassetteRunLogic::update ()
{
   const int dt = osTime::howLongMilliSec(_startTime);

   if (_order == (CHW_CASSETTE_ORDERS)_status)
   {
      if (!_overdriveInProgress)
      {
         _overdriveInProgress = true;
         osTime::snapshotRawTime(_overdriveTime);
      }
      else if (osTime::howLongMilliSec(_overdriveTime) >= CASSETTE_OVERDRIVE_MSEC)
      {
         DataLog_Default << "Cassette in position: status=" << (int)_status
                         << " time=" << dt << " ms" << endmsg;

         chw_valveSetCommand(hw_cassette, chw_stopValve);
      }
   }
   else if (chw_valveGetStatus(hw_cassette) & chw_valveFault)
   {
      chw_valveSetCommand(hw_cassette, chw_stopValve);
      _errors = HW_FAULT;

      DataLog(log_level_control_alarm_detail) << "Cassette position not attained due to fault: order="
                                              << (int)_order << " status=" << (int)_status << " time=" << dt << endmsg;

      _order = (CHW_CASSETTE_ORDERS)_status;
   }
   else if (_currentOrder != _order)
   {
      chw_valveSetCommand(hw_cassette, chw_stopValve);
      DataLog(log_level_control_alarm_detail) << "Cassette order changed: order="
                                              << (int)_order << " old order=" << (int)_currentOrder << endmsg;
   }
   else if (dt > CASSETTE_TIMEOUT)
   {
      chw_valveSetCommand(hw_cassette, chw_stopValve);
      _errors = HW_FAULT;
      DataLog(log_level_control_alarm_detail) << "Cassette position not attained due to timeout: order="
                                              << (int)_order << " status=" << (int)_status << " time=" << dt << endmsg;

      _order = (CHW_CASSETTE_ORDERS)_status;
   }
}

/* FORMAT HASH 93fe1c0c41358d648999ae2e47fc0d96 */

/*
 * Copyright (C) 2003 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: K:/BCT_Development/vxWorks/Trima/low_level/ctl_drv/rcs/c_valve.h 1.1 2003/06/19 19:48:29Z jl11312 Exp jl11312 $
 * $Log: c_valve.h $
 * Revision 1.1  2003/06/19 19:48:29Z  jl11312
 * Initial revision
 *
 */

#ifndef _C_VALVE_INCLUDE
#define _C_VALVE_INCLUDE

#include "chw.hpp"
#include "ctl_hw_private.h"
#include "ostime.hpp"

// SPECIFICATION:    valve run class, this class processes the running
//                   of the selected valve
//
// ERROR HANDLING:   none.

class valveRunLogic
{
public:
   valveRunLogic(HWValve select,              // valve to be controlled
                 CHW_VALVE_ORDERS& order,
                 HW_VALVE_STATES& vstatus);

   virtual ~valveRunLogic();

   void newOrder ();                       // start valve movement
   void update ();                         // update loop

private:
   HWValve           _select;
   CHW_VALVE_ORDERS& _order;              // current order
   HW_VALVE_STATES&  _status;             // current status

   const char*       _valveName;          // name of the valve
   rawTime           _startTime;          // timer
   HW_VALVE_STATES   _startStatus;        // status at start of motion

   CtlHWValveCommand _lastCmd;            // last direction commanded
   bool              _lastCmdTimedOut;    // true if last motion attempt failed due to timeout
   CHW_VALVE_ORDERS  _lastOrder;          // last ordered position
};

// SPECIFICATION:    cassette run class, this class processes up and down
//                   cassette requests
//
// ERROR HANDLING:   none.

class cassetteRunLogic
{
public:
   cassetteRunLogic(CHW_CASSETTE_ORDERS& order,
                    HW_CASSETTE_STATES& pstatus,
                    HW_ERROR& errors);

   virtual ~cassetteRunLogic();

   void newOrder ();
   void update ();

private:
   CHW_CASSETTE_ORDERS& _order;                    // current order
   HW_CASSETTE_STATES&  _status;                   // current status

   CHW_CASSETTE_ORDERS  _currentOrder;             // current order in progress (if any)
   HW_ERROR&            _errors;                   // current error

   rawTime              _startTime;
   rawTime              _overdriveTime;
   bool                 _overdriveInProgress;
};

void processValves (valveRunLogic& rbc,
                    valveRunLogic& plasma,
                    valveRunLogic& platelet,
                    cassetteRunLogic& cassette,
                    CHwOrders& _orderData,
                    CHwStates& _statusData);

#endif /* ifndef _C_VALVE_INCLUDE */

/* FORMAT HASH 727cb6bfcfc42ce76de6b281d37ea789 */

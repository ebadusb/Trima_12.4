/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      Focussed System control read/set door states
 *
 * ABSTRACT:   Reads and debounces the door sensors, sets the door lock
 *
 */

#ifndef _C_DOOR_INCLUDE
#define _C_DOOR_INCLUDE

#include "chw.hpp"
#include "shw.hpp"

// SPECIFICATION:    debounce door locks
//
// ERROR HANDLING:   none.

class locks
{
public:
   locks(void);
   virtual ~locks();

   void           update ();              // update with new data
   HW_DOOR_STATES state ();               // get current state

private:
   HW_DOOR_STATES _now;                    // current value
   short          _count;                  // debounce count
};



class doorControlStatus
{
public:
   doorControlStatus(CHwOrders& _orders, CHwStates& _status);
   virtual ~doorControlStatus();
   void update (bool override);            // update, called every 10 ms
   void setSHwOrders (SHwOrders* orders) { _safetyOrders = orders; }

private:
   doorControlStatus();                   // default ctr disabled
   doorControlStatus(doorControlStatus const&);
   doorControlStatus& operator = (doorControlStatus const&);

   locks           _locks;                 // door locks
   CHwOrders&      _orderData;             // order data
   CHwStates&      _statusData;            // status data
   SHwOrders*      _safetyOrders;          // Safety orders
   long            _onTimer;               // on timer
   long            _offTimer;              // off timer
   long            _dutyCycleTimer;        // timer to enforce 7% duty cycle
   bool            _dutyCycleComplete;     // TRUE=>have enforced duty cycle
   bool            _overrideAttempt;       // TRUE=>currently in a lock override operation
   long            _retryCount;            // retry count
   CHW_DOOR_ORDERS _oldDirection;          // old direction
};

#endif /* ifndef _C_DOOR_INCLUDE */

/* FORMAT HASH ccffee4ecc07fef10114f955b3a3e888 */

/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      Focussed System control rbc detctor interface
 *
 * ABSTRACT:   Interface to the rbc detector.  The rbcDetector class
 *             allows the setting of the read and green driver values and
 *             the updating of the red and green sensed values.
 *
 */

// #includes
#include <vxWorks.h>

#include "c_door.hpp"
#include "ctl_hw_private.h"
#include "trima_datalog.h"

static const unsigned DEBOUNCE_COUNT = 10;     // debounce count
static const unsigned ON_TIME_COUNT  = 100;    // on time, 10ms counts

//
// RETRY_LIMIT controls the number of ON_TIME_COUNT duration pulses
// which will be attempted before setting a fault bit.  Note that the code
// is structured to attempt RETRY_LIMIT+1 pulses at first.  On subsequent
// retries (after the operator presses continue on the alarm screen) only
// RETRY_LIMIT pulses will be used.  This means that the 7% duty cycle will
// be transiently exceeded, but will be enforced over the long term.  Field
// experience has shown that this is not an issue for the solenoid hardware.
//
static const unsigned RETRY_LIMIT = 2;       // retry limit

//
// Compute minimum off times based on 7% duty cycle.
//
// For a single try (of ON_TIME_COUNT), we need a total cycle time of ON_TIME_COUNT/0.07
// (or 100*ON_TIME_COUNT)/7).  Of that period, ON_TIME_COUNT is spent on, so the
// minimum off time is 100*ON_TIME_COUNT)/7 - ON_TIME_COUNT.
//
// For RETRY_LIMIT tries, the minimum off time is scaled appropriately from
// the single cycle time.
//
static const unsigned DUTY_CYCLE_COUNT_SINGLE_TRY = 100 * ON_TIME_COUNT / 7 - ON_TIME_COUNT;
static const unsigned DUTY_CYCLE_COUNT            = RETRY_LIMIT * DUTY_CYCLE_COUNT_SINGLE_TRY;


// SPECIFICATION:    door lock constructor
//
// ERROR HANDLING:   none.

locks::locks(void)
   : _now(HW_DOOR_OPEN),
     _count(0)
{}

// SPECIFICATION:    door lock destructor
//
// ERROR HANDLING:   none.

locks::~locks()
{}

// SPECIFICATION:    door lock update data
//
// ERROR HANDLING:   none.

void locks::update ()
{
   // get current door closed and locked states
   const unsigned short doorStatus = hw_doorGetStatus();
   const HW_DOOR_STATES doorLocked = (doorStatus & hw_doorLocked) ? HW_DOOR_LOCKED_STATE : HW_DOOR_OPEN;
   const HW_DOOR_STATES doorClosed = (doorStatus & hw_doorClosed) ? HW_DOOR_CLOSED_STATE : HW_DOOR_OPEN;

   // test door states
   HW_DOOR_STATES data = HW_DOOR_OPEN;

   if ( (doorLocked == HW_DOOR_LOCKED_STATE) &&
        (doorClosed == HW_DOOR_CLOSED_STATE)
        )
   {
      data = HW_DOOR_CLOSED_AND_LOCKED;
   }
   else if (doorLocked == HW_DOOR_LOCKED_STATE)
   {
      data = doorLocked;
   }
   else if (doorClosed == HW_DOOR_CLOSED_STATE)
   {
      data = doorClosed;
   }

   if (_now != data)                         // different?
   {
      _count++;                              // debounce count

      if (_count >= DEBOUNCE_COUNT)          // enough counts?
      {
         _now   = data;                      // change state
         _count = 0;                         // reset count
      }
   }
   else
   {
      _count = 0;                            // clear count
   }
}



// SPECIFICATION:    door lock get state
//
// ERROR HANDLING:   none.

HW_DOOR_STATES locks::state ()
{
   return _now;
}

// SPECIFICATION:    door control and status constructor
//
// ERROR HANDLING:   none.
doorControlStatus::doorControlStatus(CHwOrders& orders,
                                     CHwStates& status)
   : _orderData(orders),
     _statusData(status),
     _safetyOrders(NULL),
     _onTimer(0),
     _offTimer(0),
     _dutyCycleTimer(0),
     _retryCount(RETRY_LIMIT),
     _oldDirection(CHW_DOOR_UNLOCK),
     _locks(),
     _dutyCycleComplete(true),
     _overrideAttempt(false)
{}



// SPECIFICATION:    door control and status destructor
//
// ERROR HANDLING:   none.

doorControlStatus::~doorControlStatus()
{}



// SPECIFICATION:    door control and status update function.
//
// ERROR HANDLING:   none.
void doorControlStatus::update (bool override)
{
   unsigned char       faultData;

   static HW_ERROR     previousStatus = HW_NO_FAULT;
   static unsigned int count          = 0;

   // update status
   _locks.update();
   _statusData.doorLocks = _locks.state();

   // If the door solenoid power is turned off, nothing to do
   if ( (_safetyOrders == NULL) || (_safetyOrders->doorSolenoidPower == HW_DISABLE) )
   {
      return;
   }

   // force door unlocked if requested from the control driver (used to unlock
   // door when stop button is pressed)
   if (override)
   {
      if ( (_dutyCycleTimer == 0) &&
           (_onTimer == 0)        &&
           (_offTimer == 0)
           )
      {
         // Door lock is idle - go ahead and unlock
         if (!_overrideAttempt && (hw_doorGetStatus() & hw_doorLocked))
         {
            DataLog_Default << "solenoid override attempted" << endmsg;

            _retryCount              = 1;
            _overrideAttempt         = true;
            _orderData.doorDirection = _oldDirection = CHW_DOOR_UNLOCK;
         }
         else
         {
            return;
         }
      }
      else if (!_overrideAttempt && !_dutyCycleTimer)
      {
         // Stop command in progress and impose a duty cycle delay before
         // attempting the door override
         chw_doorLockSetCommand(chw_doorLockOff);

         _onTimer           = _offTimer = 0;
         _dutyCycleTimer    =  DUTY_CYCLE_COUNT_SINGLE_TRY;
         _dutyCycleComplete = false;

         DataLog_Default << "aborting current door command due to override" << endmsg;
      }
   }

   // door logic
   if (_dutyCycleTimer)
   {
      _dutyCycleTimer--;

      if (_dutyCycleTimer <= 0)
      {
         DataLog_Default << "solenoid duty cycle timeout complete" << endmsg;
         _dutyCycleComplete = true;
      }
   }
   else if (_onTimer)                          // check on timer
   {
      _onTimer--;

      if ( (_onTimer <= 0) ||
           (_orderData.doorDirection == (_statusData.doorLocks & 0x01) )
           )
      {
         _offTimer = ON_TIME_COUNT - _onTimer;
         _onTimer  = 0;

         chw_doorLockSetCommand(chw_doorLockOff);
      }

      // if the lock is in the requested position, we don't have to enforce the duty cycle
      if (_orderData.doorDirection == (_statusData.doorLocks & 0x01))
         _dutyCycleComplete = true;
   }
   else if (_offTimer)                         // check off timer
   {
      _offTimer--;
   }
   else if ( ( (_orderData.doorLatches == HW_ENABLE) || override) &&
             (_orderData.doorDirection != (_statusData.doorLocks & 0x01) ) &&
             (_retryCount > 0)
             )
   {
      DataLog_Default << "door lock command in progress: latches="
                      << (int)_orderData.doorLatches << " direction="
                      << (int)_orderData.doorDirection << " retry count=" << (int)_retryCount
                      << " override=" << override
                      << endmsg;

      // turn on door power
      _retryCount--;
      _onTimer = ON_TIME_COUNT;                        // door timer
      chw_doorLockSetCommand(_orderData.doorDirection ? chw_doorLock : chw_doorUnlock);

      // only check the solenoid fault bit when power is on
      _statusData.solenoidFault = (chw_doorLockGetStatus() & chw_doorLockFault) ? HW_FAULT : HW_NO_FAULT;

      if (_statusData.solenoidFault == HW_FAULT)
         DataLog_Default << "solenoid fault" << endmsg;

      // for the moment, assume it will fail and we will have to enforce the duty cycle
      _dutyCycleComplete = false;
   }
   else
   {
      chw_doorLockSetCommand(chw_doorLockOff);

      // set error if we never got it where we wanted it
      if ( (_retryCount <= 0) &&
           (_orderData.doorDirection != (_statusData.doorLocks & 0x01))
           )
      {
         _statusData.solenoidFault = HW_FAULT;

         // enforce the duty cycle restriction
         if (!_dutyCycleComplete)
         {
            // if this was a single try, we don't have to wait as long
            if (_overrideAttempt)
               _dutyCycleTimer = DUTY_CYCLE_COUNT_SINGLE_TRY;
            else
               _dutyCycleTimer = DUTY_CYCLE_COUNT;
         }

         if (previousStatus != _statusData.solenoidFault)
         {
            DataLog_Default << "Door lock attempt failed.  Setting solenoid status to HW_FAULT"
                            << ".  _retryCount=" << _retryCount
                            << ".  door direction=" << _orderData.doorDirection
                            << ".  dutyCycleComplete=" << _dutyCycleComplete
                            << ".  overrideAttempt=" << _overrideAttempt
                            << ".  dutyCycleTimer=" << _dutyCycleTimer
                            << endmsg;

            previousStatus = _statusData.solenoidFault;
         }
      }
      else
         _statusData.solenoidFault = HW_NO_FAULT;

      // however the override ended, it ended
      _overrideAttempt = false;
   }

   // reset retry count based on direction ordered
   if (_orderData.doorDirection != _oldDirection)
   {
      _oldDirection = _orderData.doorDirection;
      _retryCount   = RETRY_LIMIT;
   }
}

/* FORMAT HASH 1a6d0d66444932925d88820afe45e02d */

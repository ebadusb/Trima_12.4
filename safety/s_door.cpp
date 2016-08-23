/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.12  2004/12/03 17:35:41Z  rm70006
 * IT 7025.  Fix bad log statement for door status.
 * Revision 1.11  2003/07/15 21:31:44Z  ms10234
 * 6157 - log level changes from debug to info
 * Revision 1.10  2003/07/01 22:34:40Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.9  2003/04/04 00:07:20Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.8  2003/01/23 22:46:36Z  pn02526
 * Remove "datalog_delete" comments.  Improve some logging.
 * Revision 1.7  2002/08/07 18:51:28Z  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * Revision 1.6  2002/07/23 16:52:26  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.5  2002/04/11 17:14:46  pn02526
 * Revise logging to use DataLogLevel class.
 * Revision 1.4  2001/12/14 14:26:29Z  jl11312
 * - modified centrifuge spinning with door open/unlocked alarm to occur at 1 second instead of 2.5 during startup tests
 * Revision 1.3  2000/07/26 22:06:58  bs04481
 * Relocate code to control solenoid power from the power contol
 * timer to the door object because the door object normally
 * controls the solenoid power based on centrifuge speed.  Also,
 * delay 1 second before sending a response for solenoid power
 * disable.
 * Revision 1.2  2000/06/20 20:48:45  bs04481
 * Missing an 'else' in the door fault counter.
 * Revision 1.1  2000/03/15 18:28:46  BS04481
 * Initial revision
 *
 *
 * TITLE:      s_door.cpp, a door
 *
 * ABSTRACT:   Safety's view of the basin door.
 *
 */

#include "trima_datalog.h"
#include "s_door.hpp"
#include "s_exec.hpp"

/*******************************************************************************
*
*  aDoor_Safety CLASS DEFINITION
*
******************************************************************************/
// SPECIFICATION:    Door constructor
//
//
// ERROR HANDLING:   none.

aDoor_Safety::aDoor_Safety(aCentrifuge_Safety* centrifuge
                           , aSafety24vPowerTest* powerTest)
   : _centrifuge(centrifuge), _Safety24vPowerTest(powerTest)
{
   _cT1Status                = NO_POSITION_VERIFIED;
   _cDeviceConstraint        = DOOR_MAY_BE_UNLOCKED_AND_OPENED;
   _DoorFaultTolerantCounter = 0;
   _doorLocks                = HW_DOOR_OPEN;
   _bForceDisable            = FALSE;
   _bForceDisableDone        = FALSE;
}

// SPECIFICATION:    Update system state
//
//
// ERROR HANDLING:   none.

void aDoor_Safety::initializeForState (long newState)
{}

// SPECIFICATION:    Door tests and operation
//                   depending on safety status information
//
//
// ERROR HANDLING:   none.

void aDoor_Safety::Update (HW_DOOR_STATES locks)
{
   _doorLocks = locks;
   // disable door lock power if centrifuge running > 60 RPM
   // or if procedure has requested the solenoid disabled for the
   // purpose of testing it
   if ( ( _centrifuge->WhatIsCurrentRPM() > MAX_CENT_SPD_DOOR_NOT_LOCKED_AND_CLOSED )
        ||( _bForceDisable == TRUE) )
   {
      /// gate debug logging
      if (_cDeviceConstraint == DOOR_MAY_BE_UNLOCKED_AND_OPENED )
      {
         if (_bForceDisable == TRUE)
         {
            DataLog(log_level_safe_exec_info) << "Force DISABLE Door Power" << endmsg;
         }
         else
         {
            DataLog(log_level_safe_exec_info) << "Cent RPM > 60 , DISABLE Door Power" << endmsg;
         }
      }
      paSafetyHardwareCommandsExec->SetDoorPower(HW_DISABLE);
      _cDeviceConstraint = DOOR_MUST_BE_CLOSED_AND_LOCKED;
      _bForceDisableDone = TRUE;
   }

   // enable door lock power if centrifuge RPM < 60 and
   // if the 24v tests good.  (because solenoid uses 24v?)
   if ( (_centrifuge->WhatIsCurrentRPM() < MAX_CENT_SPD_DOOR_NOT_LOCKED_AND_CLOSED )
        &&(_Safety24vPowerTest->GetT1Status() == PWR_CONTROL_NOMINAL)
        &&(_bForceDisable == FALSE) )
   {
      /// gatedebug logging
      if (_cDeviceConstraint == DOOR_MUST_BE_CLOSED_AND_LOCKED )
      {
         DataLog(log_level_safe_exec_info) << "Cent RPM < 60 , ENABLE Door Power" << endmsg;
      }
      paSafetyHardwareCommandsExec->SetDoorPower(HW_ENABLE);
      _cDeviceConstraint = DOOR_MAY_BE_UNLOCKED_AND_OPENED ;
   }
}

// SPECIFICATION:    Door tests above 60 RPM
//                   depending on safety status information
//
//
// ERROR HANDLING:   none.

void aDoor_Safety::UpdateAndTestDoorFault ()
{
   if ( (_doorLocks != HW_DOOR_CLOSED_AND_LOCKED )
        &&(_centrifuge->WhatIsCurrentRPM() > MAX_CENT_SPD_DOOR_NOT_LOCKED_AND_CLOSED) )
   {
      _DoorFaultTolerantCounter += 1;
      if ( _DoorFaultTolerantCounter > DOOR_FAULT_TOLERANCE ||
           ( SafetyExec::instance()->SystemState() == STARTUP_TESTS && _DoorFaultTolerantCounter > 1 ) )
      {
         DataLog(log_level_safety_alarm_detail) << "Door Lock Status: "
                                                << (int)_doorLocks
                                                << "  Door FAILURE count exceeded "
                                                << (int)DOOR_FAULT_TOLERANCE
                                                << " - Alarm"
                                                << endmsg;

         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, DOOR_NOT_LOCKED_CENTIFUGE_SPINNING);
      }
      else
      {

         DataLog(log_level_safe_exec_info) << "Door Lock Status: "
                                           << (int)_doorLocks
                                           << "  Door FAILURE count "
                                           << _DoorFaultTolerantCounter
                                           << " - Counting"
                                           << endmsg;
      }
   }
   else
   {
      _DoorFaultTolerantCounter = 0;
   }
}

// SPECIFICATION:    Is door currently closed and locked
//
//
// ERROR HANDLING:   none.

bool aDoor_Safety::DoorT1OK (void)
{
   bool result = FALSE;

   if (_doorLocks == HW_DOOR_CLOSED_AND_LOCKED)
      result = TRUE;

   DataLog(log_level_safe_exec_info) << "Door Lock T1 : "
                                     << ( (_doorLocks ==   HW_DOOR_OPEN) ? "HW_DOOR_OPEN" :
        (_doorLocks ==   HW_DOOR_LOCKED_STATE) ? "HW_DOOR_LOCKED_STATE" :
        (_doorLocks ==   HW_DOOR_CLOSED_STATE) ? "HW_DOOR_CLOSED_STATE" :
        (_doorLocks ==   HW_DOOR_CLOSED_AND_LOCKED) ? "HW_DOOR_CLOSED_AND_LOCKED" :
        "UNKNOWN" )
                                     << "(" << (int)_doorLocks << ")"
                                     << "; Pass Value : HW_DOOR_CLOSED_AND_LOCKED(" << (int)HW_DOOR_CLOSED_AND_LOCKED << ")" << endmsg;

   return(result);
}

/* FORMAT HASH 9e1e6cd50fac1e2b242237e78d43341e */

/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.17  2007/10/17 22:39:47Z  spriusb
 * Rolling back dRBC auto-split changes
 * Revision 1.16  2007/08/30 20:06:50Z  spriusb
 * Enhancements to support dRBC auto-split cassettes
 * Revision 1.15  2003/12/04 20:16:10Z  ms10234
 * 6604 - add forceInitialize to PFR data to enforce cassette position monitoring after PFR.
 * Revision 1.14  2003/10/23 16:25:54Z  ms10234
 * 6324 - fixed logging statement used for T1 status.
 * Revision 1.13  2003/09/09 23:44:47Z  ms10234
 * 6406 - fix pfr code
 * Revision 1.12  2003/07/01 22:35:11Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.11  2003/04/04 16:22:52Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.10  2003/01/23 22:09:21Z  pn02526
 * Remove "datalog_delete" comments.  Improve some logging.
 * Revision 1.9  2002/12/27 16:47:59Z  pn02526
 * Runnable safety executive with CRC checking of the code disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.
 * Revision 1.8  2002/08/08 13:49:41  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * Revision 1.7  2002/07/23 16:52:21  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.6  2002/04/10 14:48:53  pn02526
 * "Revise logging to use DataLogLevel class."
 * Revision 1.5  2002/03/01 22:37:26Z  ms10234
 * 5613 - Increased the robustness of the AC pump monitor by enabling
 * the monitor whenever the donor is connected, regardless of state.
 * The exceptions are for rinseback and donor disconnect states where
 * rinseback monitoring is in effect.
 * Revision 1.4  2000/06/30 21:29:24  bs04481
 * Re-did valve monitoring to not monitor by draw and return cycles
 * but to alarm anytime there is a valve out of position for 20
 * consecutive seconds while the associated pump is running.
 * Revision 1.3  2000/06/22 16:46:49  bs04481
 * Plumbing changes for single-stage.  1) Change to RBC flow.
 * 2) Add valve position monitoring during Blood Run and Blood
 * Rinseback. 3)  Add compile flag to support dual-stage
 * Revision 1.2  2000/05/26 17:12:41  BS04481
 * More maintanence changes:  This set contains changes for
 * 1. Moving valve object from container to bag
 * 2. Removing SafetyTimer->SafetySoftTimer and letting the
 *      switch and disconnect tests have their own timers.
 * 3.  All message types are changed to NO_BOUNCE to cut down
 *      the on unnecessary stuff in the message que.
 * Revision 1.1  2000/03/15 18:28:38  BS04481
 * Initial revision
 *
 * TITLE:      s_valve.cpp
 *             Safety valve and cassette monitors.
 *
 *
 * ABSTRACT:   This file contains the class methods for the
 *             valve and cassette monitors.  Includes blink test.
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 ******************************************************************************/

#include "s_valve.hpp"
#include "s_glob.hpp"
#include "s_pfr.hpp"

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

/*******************************************************************************
*
*  aCassette_Safety CLASS METHODS DEFINITION
*
******************************************************************************/
aCassette_Safety::aCassette_Safety(aDonor* paDonor)
{
   _cT1Status        = NO_POSITION_VERIFIED;
   _deviceConstraint = CASSETTE_MUST_REMAIN_DOWN;
   _position         = HW_CASSETTE_UNKNOWN;
   _forceInitialize  = TRUE;
   _paDonor          = paDonor;

   saveToPFR();
}

void aCassette_Safety::UpdateT1Status (char cCassettePosition)
{
   _cT1Status |= (char)(1 << (int)cCassettePosition);
}

void aCassette_Safety::SetDeviceConstraint (int deviceConstraint)
{
   _deviceConstraint = deviceConstraint ;
   saveToPFR();
}

HW_CASSETTE_STATES aCassette_Safety::GetCassettePosition (void)
{
   return(_position);
}

void aCassette_Safety::Update (HW_CASSETTE_STATES position)
{
   _position = position;

   // on the first status update...
   if (_forceInitialize == TRUE)
   {
      _forceInitialize = FALSE;

      // if the cassete is not down, allow it to move
      if (_position != HW_CASSETTE_DOWN)
      {
         _deviceConstraint = CASSETTE_MAY_BE_MOVED;

         // if the cassette is not down, then the donor is
         // not connected
         _paDonor->SetDonorConnectedState(DONOR_IS_NOT_CONNECTED);

         // allow pump power to be enabled if the cassette is not down
         paSafetyHardwareCommandsExec->BlockPumpPower(FALSE);
      }
   }

   // check for inappropriate cassette rasing
   if ( (_position != HW_CASSETTE_DOWN) && (_deviceConstraint == (char)CASSETTE_MUST_REMAIN_DOWN ) )
   {
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, INAPPROPRIATE_CASSETTE_MOVEMENT);

      DataLog(log_level_safety_alarm_detail) << "Cassette position not down.  position= " << (int)_position
                                             << ".  Constraint was " << _deviceConstraint << endmsg;
   }

   // expect to see unloaded and loaded positions for T1
   UpdateT1Status((char)_position);

   saveToPFR();

}

void aCassette_Safety::initializeForState (long newState)
{
   switch (newState)
   {
      case AC_PRIME :
         SetDeviceConstraint(CASSETTE_MUST_REMAIN_DOWN);
         break;
      default :
         break;
   }
   saveToPFR();
}

bool aCassette_Safety::CassetteT1OK (void)
{
   bool result = FALSE;

   if (_cT1Status == (char)ALL_CASS_POSITIONS_VERIFIED)
      result = TRUE;

   // debug
   DataLog(log_level_safe_exec_info) << "Cassette T1 : " << (int)_cT1Status << "; Pass Value : ALL_CASS_POSITIONS_VERIFIED(" << (int)ALL_CASS_POSITIONS_VERIFIED << ")" << endmsg;

   return(result);

}

bool aCassette_Safety::isFirstSampleIn (void)
{
   if (_forceInitialize == TRUE)
      return(FALSE);
   else
      return(TRUE);
}

void aCassette_Safety::saveToPFR ()
{
   SafetyPFRDataToSave.PFRCassette_cDeviceConstraint = _deviceConstraint;
   SafetyPFRDataToSave.PFRCassette_position          = _position;
   SafetyPFRDataToSave.PFRCassette_forceInitialize   = _forceInitialize;
}

void aCassette_Safety::restoreFromPFR ()
{
   _deviceConstraint = SafetyPFRDataFromRestore.PFRCassette_cDeviceConstraint;
   _position         = SafetyPFRDataFromRestore.PFRCassette_position;
   _forceInitialize  = SafetyPFRDataFromRestore.PFRCassette_forceInitialize;

   DataLog(log_level_safe_exec_info) << "aCassette_Safety::RestoreFromPFR(), "
                                     << "deviceConstraint: " << _deviceConstraint
                                     << " position: " << (HW_CASSETTE_STATES)_position
                                     << " forceInitialize: " << _forceInitialize
                                     << endmsg;

   DataLog(log_level_safe_exec_info).precision(5);
}

/*******************************************************************************
*
*  valve CLASS DEFINITION
*
******************************************************************************/
// constructor for an unknown valve
valve::valve()
{
   strcpy(_pszValveName, "");
   _iT1Status                    = NO_POSITION_VERIFIED;
   _positionC                    = HW_VALVE_UNKNOWN;
   _positionS                    = HW_VALVE_UNKNOWN;
   _bValvePositionMonitorEnabled = FALSE;
   _iBadValvePositionCount       = 0;
   _alarm = NULL_ALARM;
}

// Modified copy constructor as a Prototype
valve::valve(valve const& v)
   : _iT1Status(v._iT1Status),
     _positionC(v._positionC),
     _positionS(v._positionS),
     _bValvePositionMonitorEnabled(v._bValvePositionMonitorEnabled),
     _iBadValvePositionCount(v._iBadValvePositionCount),
     _alarm(v.GetAlarm())
{
   strcpy(_pszValveName, v._pszValveName);
}

valve::~valve()
{}

void valve::MakeItReal (const char* pszValveName, ALARM_VALUES alarm)
{
   strcpy(_pszValveName, pszValveName);
   _alarm     = alarm;
   _iT1Status = NO_POSITION_VERIFIED; // reset T1 status to force retest
}

void valve::MakeItFake ()
{
   strcpy(_pszValveName, "pseudoValve");
   _iT1Status = ALL_VALVE_POSITIONS_VERIFIED; // pretend we tested it
   _positionC = HW_VALVE_COLLECT;             // allow flow
   _positionS = HW_VALVE_COLLECT;             // allow flow
}

void valve::UpdateT1Status (HW_VALVE_STATES positionS)
{
   int oldstatus = _iT1Status;
   _iT1Status |= 1 << (int)positionS;
   if (oldstatus != _iT1Status)
   {
      DataLog(log_level_safe_exec_info) << "safety valve T1 sees " << _pszValveName << " in position " << (int)positionS << endmsg;
   }
}

void valve::UpdateControlValve (HW_VALVE_STATES positionC)
{
   _positionC = positionC;
}

void valve::UpdateSafetyValve (HW_VALVE_STATES positionS)
{
   _positionS = positionS;
   UpdateT1Status(positionS);
}

void valve::CheckValvePosition ()
{

   if (_bValvePositionMonitorEnabled == TRUE)
   {
      if ( (_positionS == HW_VALVE_OPEN)
           ||(_positionS == HW_VALVE_UNKNOWN)
           ||(_positionC == HW_VALVE_OPEN)
           ||(_positionC == HW_VALVE_UNKNOWN) )
      {
         // send up the alarm
         reportOutOfPosition();
      }
      else
         _iBadValvePositionCount = 0;
   }
}

HW_VALVE_STATES valve::GetControlValve (void)
{
   return ( _positionC );
}

HW_VALVE_STATES valve::GetSafetyValve (void)
{
   return (_positionS);
}

bool valve::ValveT1OK (void)
{
   bool result = FALSE;

   if ( (_iT1Status == ALL_VALVE_POSITIONS_VERIFIED)
        ||(_iT1Status == ALL_VALVE_POSITIONS_VERIFIED_XCEPT_UNKNOWN ) )
      result = TRUE;

   // debug
   DataLog(log_level_safe_exec_info) << _pszValveName
                                     << " T1 : "
                                     << _iT1Status
                                     << "; Pass Values : ALL_VALVE_POSITIONS_VERIFIED("
                                     << (int)ALL_VALVE_POSITIONS_VERIFIED << ")"
                                     << "or ALL_VALVE_POSITIONS_VERIFIED_XCEPT_UNKNOWN("
                                     << (int)ALL_VALVE_POSITIONS_VERIFIED_XCEPT_UNKNOWN << ")"
                                     << endmsg;

   return(result);

}

void valve::InitializeForState (long newState)
{
   switch (newState)
   {
      case BLOOD_PRIME :
      case BLOOD_RUN :
         _iBadValvePositionCount       = 0;
         _bValvePositionMonitorEnabled = TRUE;
         break;
      default :
         _bValvePositionMonitorEnabled = FALSE;
         break;
   }
}

void valve::reportOutOfPosition ()
{
   if (++_iBadValvePositionCount > MAX_SAMPLES_BAD_VALVE)
   {
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, (ALARM_VALUES)_alarm);

      DataLog(log_level_safety_alarm_detail) << _pszValveName
                                             << " Out of position in draw. Control: "
                                             << (int)_positionC
                                             << " Safety: "
                                             << (int)_positionS
                                             << endmsg;
   }
}

/* FORMAT HASH b9992f5ca3aa52679540770694510c4a */

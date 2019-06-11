/*******************************************************************************
 *
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.42  2008/08/26 18:39:03Z  spriusb
 * Added code to send proc state down to safety driver layer through test1 field to better facilitate unit testing.
 * Revision 1.41  2003/08/20 19:04:15Z  jl11312
 * - changes for IT 6373
 * Revision 1.40  2003/07/07 21:04:45Z  jl11312
 * - implemented APS auto-zero
 * Revision 1.39  2003/07/01 22:34:49Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.38  2003/04/04 00:38:22Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.37  2003/01/06 18:14:59Z  pn02526
 * Runnable safety executive with CRC checking of the code disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.
 * Revision 1.36  2002/10/11 10:43:43  pn02526
 * Massive checkin of fixed modules for integration checking
 * Revision 1.35  2002/08/07 17:20:36  pn02526
 * Substitute #includes of new message system files for obsolete message system files and an obsolete dispatcher file.
 * Use the new buffered message base class definition to send the Alarm_struct message, rather than deriving class aSafetyAlarmMsg_sndr from it.
 * Remove unneeded, null method aSafetyAlarmMsg_sndr :: notify().
 * Add aSafetyAlarmRepeater :: _myTimer to use the new timer message base class definition, rather than deriving class aSafetyAlarmRepeater from a timer message base class.
 * Add code to aSafetyAlarmRepeater's destructor to also destroy the aSafetyAlarmMsg_sndr created in its constructor.
 * Make aSafetyAlarmRepeater :: notify() a private method since it doesn't need to be public for the new message system.
 * Use the new buffered message base class definition to send the SHwOrders message, rather than deriving class aSafetyHardwareCommandsExec from it.
 * Revision 1.34  2002/04/15 15:15:13  pn02526
 * Change log(LOG_ALARM_DETAIL) to alarm_detail(LOG_ALARM_DETAIL)
 * Revision 1.33  2002/04/05 16:39:57  pn02526
 * Revise logging to use DataLogLevel class.  Revise FATAL_ERROR calls to use the new one.  Change ASSERT() macro calls to trima_assert().
 * Revision 1.32  2000/08/15 10:29:18  MS10234
 * Changed alarm state enum names to be more meaningfule
 * Revision 1.31  2000/07/06 01:18:54  bs04481
 * Variable name change
 * Revision 1.30  2000/06/08 21:35:44  bs04481
 * Get rid of the separate alarm sender and just use the repeater
 * Revision 1.29  2000/05/26 17:08:01  BS04481
 * Added alarm repeater.  Changed messages to NO_BOUNCE.
 * Revision 1.28  2000/03/15 18:19:09  BS04481
 * Make hardware command object into the message handler and
 * remove unnecessary handler; remove unnecessary defines
 * Revision 1.27  1999/10/08 15:37:30  BS04481
 * Restore air-to-donor monitoring in the safety driver following a
 * PF if the procedure is not recoverable and the cassette is down.
 * Revision 1.26  1999/04/06 18:32:55  BS04481
 * Remove default of set type to white when unknown.  If set type is
 * not known in states from Donor Connect to Rinseback, inclusive,
 * and any pumps run, alarm.  Default to white if unknown in Donor
 * Disconnect
 * Revision 1.25  1998/07/14 21:23:17  bs04481
 * Temporarily move this file back to previous revision
 * Revision 1.24  1998/07/14 18:00:33  bs04481
 * Move read of the cal.dat file to after the dispatcher is initialized.
 * Revision 1.23  1998/01/22 18:04:29  bs04481
 * Fixed problem with erroneously alarming the AC Ratio Too Low
 * when we had previously disabled pump power due to the pumps
 * being paused more than 10 minutes.
 * Revision 1.22  1997/12/17 21:53:07  bs04481
 * Initialization cleanup
 * Revision 1.21  1997/12/12 17:12:51  bs04481
 * Change BlockPumpPower function to also turn off 24v if it is
 * already enabled.
 * Revision 1.20  1997/12/02 15:59:48  bs04481
 * Changes to implement power fail recovery
 * Revision 1.19  1997/05/15 00:43:23  SM02805
 * Implemented three point APS calibration.
 * Revision 1.18  1997/03/13 23:21:05  bs04481
 * Changes to set the new donorMode in the safety command data
 * Revision 1.17  1997/02/11 18:00:40  bs04481
 * Changes due to enumerated types in the hardware structures
 * Revision 1.16  1996/12/13 18:17:12  bs04481
 * Added range testing of calculated APS : limited to +400 : -250 mmHg.
 * Revision 1.15  1996/11/28 00:08:14  SM02805
 * Revision 1.14  1996/09/15 21:49:32  SM02805
 * Significantly reduce print instrumentation in source files.
 * Implement _LOG_ERROR processing for cases where
 * data is important for post-run safety analysis.
 * Revision 1.13  1996/09/15 21:49:32  SM02805
 * Fixes for safety bug that was not keeping power disabled following
 * alarm detection :
 * 1. eliminate inclusion of safact.hpp in all files and remove all conditional
 * safety ( no code remains to allow continuation from safety alarms).
 * 2. Implement _cAlarmState flag in aSafetyHardwareCommandsExec to
 * prevent pump/centrifuge power enabling post alarm generation.
 * _cAlarmState is initialized to SAFETY_ALARM_ASSERTED in
 * constructor, cleared by s_exec.cpp (NO_SAFETY_ALARM) if
 * positive response to query safety alive msg is sent to proc.
 * 3. Gate door power messages on change of door power
 * cmd from last cmd to reduce msg loading to safety driver.
 * Revision 1.12  1996/09/04 19:35:29  SM02805
 * Numerous changes for safety power bug detailed in s_glob.cpp rev 1.15
 * Revision 1.11  1996/08/14 01:59:14  SM02805
 * Modify alarm msg constructor set
 * m_Data.alarm_state = NONACTIVE to allow
 * for "recoverable" safety alrms during testing.  This
 * modification does NOT need to be removed as
 * procedure processing of alarms does not use this
 * field to ascertain how to respond to safety alarms
 * IF the DEBUG constant is NOT defined in alarms.h
 * Revision 1.10  1996/07/31 21:36:56  SM02805
 * Revision 1.9  1996/07/31 21:36:56  SM02805
 * Added pressure calibration fix - read cal.dat for gain and offset for
 * APS.
 * Revision 1.8  1996/07/29 14:44:34  SM02805
 * Lint fixes per 07/23/96, 07/26/96 reviews.
 * Revision 1.7  1996/07/24 13:57:03  SM02805
 * Mods to remove absolute paths for include files, includes source
 * code modifications and makefile ( safety.mak) modifications.
 *
 * safety.mak mods use CPPFLAGS , now getting warnings level 3.
 *
 * Sorce code mods to implement donor disconnect test, only partially tested
 * - works as expected when test is passed first time, ie access line
 * properly clamped on first test try...
 *
 ******************************************************************************/

#ifndef S_GLOB_HPP
#define S_GLOB_HPP

#include <time.h>
#include <stdio.h>
#include <string.h>

/// debug : define for alarm generation console output
#define S_ALARM_CONSOLE_OUTPUT

/// debug : define for Set methods output in safety hardware command exec.
#define SAFETY_COMMAND_EXEC_CONSOLE_OUTPUT

#include "trima_datalog.h"
#include "error.h"

#include "message.h"
#include "timermessage.h"
#include "callback.h"

#include "chw.hpp"
#include "shw.hpp"

#include "s_defs.hpp"
#include "alarms.h"
#include "alarmcontainer.h"

// global defines
enum { NO_SAFETY_ALARM        = 0, SAFETY_ALARM_ASSERTED = 1 };
enum { DONOR_IS_NOT_CONNECTED = 0, DONOR_IS_CONNECTED = 1 };
enum { DRAW_CYCLE             = 1, RETURN_CYCLE = 2 };


// global functions.
float calc_elapsed_time (const struct timespec* pstructlastTime);
float calc_elapsed_time_running (struct timespec* pstructlastTime);
void  initialize_APS_cal (void);
float calc_APS_mmhg_3_pt (short raw_adc);
void  set_APS_mmhg_offset (float zero);


/*******************************************************************************
*
*  aSafetyAlarmMsg_sndr CLASS DEFINITION
*
******************************************************************************/
// SPECIFICATION:    "sender" for safety alarm messages.
//
// NOTES:
//
// ERROR HANDLING:   none.

class aSafetyAlarmMsg_sndr
{
public:
   aSafetyAlarmMsg_sndr();

   virtual ~aSafetyAlarmMsg_sndr() { }

   void postAlarm (ALARM_VALUES alarm_name);

   inline const Alarm_struct* getAlarmStruct () const { return( &_alarm ); }

private:
   aSafetyAlarmMsg_sndr(aSafetyAlarmMsg_sndr const&);              // not implemented
   aSafetyAlarmMsg_sndr& operator = (aSafetyAlarmMsg_sndr const&); // not implemented

private:
   Alarm_struct          _alarm;
   Message<Alarm_struct> _alarmMsg;
};



// SPECIFICATION:    "repeater" for safety alarm messages.
//
// NOTES:            repeats current alarm every second
//
// ERROR HANDLING:   none.

class aSafetyAlarmRepeater
{
public:
   aSafetyAlarmRepeater();
   virtual ~aSafetyAlarmRepeater() { }

   void enableAlarmRepeater (ALARM_VALUES alarm_name, const char* alarmText);

private:
   void notify ();

private:
   aSafetyAlarmRepeater(aSafetyAlarmRepeater const&);              // not implemented
   aSafetyAlarmRepeater& operator = (aSafetyAlarmRepeater const&); // not implemented

private:
   aSafetyAlarmMsg_sndr _paSafetyAlarmMsg_sndr;
   TimerMessage         _myTimer;
   bool                 _alarmActive;
   ALARM_VALUES         _repeatAlarm;
   char                 _alarmText[MAX_CHAR];
};



/*******************************************************************************
*
*  aSafetyHardwareCommandsExec CLASS DEFINITION
*
******************************************************************************/
// SPECIFICATION:    "sender" for safety hardware command messages.
//
// NOTES :
//
// ERROR HANDLING:   none.

class aSafetyHardwareCommandsExec
{
public:
   aSafetyHardwareCommandsExec();
   virtual ~aSafetyHardwareCommandsExec() {}

   void SetPumpPower (HW_ORDERS ucCmd);
   void SetCentrifugePower (HW_ORDERS ucCmd);
   void SetBothPower (HW_ORDERS ucCmd);
   void SetDoorPower (HW_ORDERS ucCmd);
   void SetAlarmLight (HW_ORDERS ucCmd);
   void SetAlarmState (bool alarmState);
   void respond_to_alarm (char cSafeStateAction, ALARM_VALUES alarm_name);
   void SetDonorMode (HW_ORDERS mode);
   void SetDonorDisconnectState (bool inDDC);
   void SetSubstate (long newState);
   void initializeForState (long newState, HW_CASSETTE_STATES cassettePosition);
   void BlockPumpPower (bool state);
   void send (void);

   inline HW_ORDERS GetCentrifugePower (void) const { return _SHwOrdersData.centrifugePower; }
   inline bool      isPowerBlocked (void) const { return _blockPower; }

private:
   // not implemented
   aSafetyHardwareCommandsExec(aSafetyHardwareCommandsExec const&);
   aSafetyHardwareCommandsExec& operator = (aSafetyHardwareCommandsExec const&);

private:
   SHwOrders            _SHwOrdersData;
   Message<SHwOrders>   _theSHwOrdersMsg;
   aSafetyAlarmRepeater _safetyAlarmRepeater;

   bool          _alarmState;
   bool          _blockPower;
   unsigned long _msgCounter;
   AlarmTable    _alarmTable;

};  // END aSafetyHardwareCommandsExec class definition

extern aSafetyHardwareCommandsExec* paSafetyHardwareCommandsExec;

#endif

/* FORMAT HASH 4944cdf658914fe020b235b477489d4f */

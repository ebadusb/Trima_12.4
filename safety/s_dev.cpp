/*******************************************************************************
 *
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * Corrections to pump monitoring : but for platelet pump due to two magnet rotor.
 *
 * stest.mak target changed to be  saf_exec.
 *
 * TITLE:      s_dev.cpp
 *             Safety devices class definitions.
 *
 *
 * ABSTRACT:   This file contains the class definitions for safety physical devices
 *             door, cassette, valves, and switches.  Also contained are the class
 *             definitions for the safety "soft clock" logical device.
 *
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 ******************************************************************************/

#include <vxworks.h>
#include <stdio.h>
#include <stdlib.h>
#include <stat.h>

#ifndef _NONTRIMA
#include <ioLib.h>
#else
#include <sys/types.h>
#include <fcntl.h>
#endif

#include "s_dev.hpp"
#include "trima_datalog.h"
#include "s_glob.hpp"
#include "s_valve.hpp" // try to remove after restructure
#include "states.h"    // proc file
#include "procop.hpp"
#include "filenames.h"


/*+++++++++++++++++++++++++++++++++++++++++++++++++
Soft Timer Definitions : Start
++++++++++++++++++++++++++++++++++++++++++++++++++*/

// valve position unknown fault tolerant time
// during donor connect
//
// 08/18/96 per cross functional approval, make valve position fault tolerant
// time 10 seconds ( cahnge from 2 seconds )
//
#define VALVE_POSITION_UNKNOWN_TOLERANCE           100      // 10000 ms

// pause switch fault tolerant time
#define  PAUSE_TOLERANCE                            20      // 2 seconds

// stop switch fault tolerant time
#define  STOP_TOLERANCE                             20      // 2 seconds

#define MAX_ALLOWABLE_CENT_SPD                    3200.0f   // in RPM
#define CENTRIFUGE_ERROR_DEBOUNCE                   15


#define SAFETY_SOFT_TIMER_PERIOD                   100      // 100 ms period for soft timer messages

/*+++++++++++++++++++++++++++++++++++++++++++++++++
T1 Definitions : Start
++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define NO_CENT_SENSE_VERIFIED               0                           // no centrifuge sense has been verified ( init value )

#define CENT_0_RPM_SENSE_NOMINAL             0                          // 0 RPM sense test passes
#define CENT_SPEED_SENSE_NOMINAL             1                          // non-zero RPM sense test passed
#define CENT_0_RPM_SENSE_VERIFIED            ( 1 << 0 )
#define CENT_SPEED_SENSE_VERIFIED            ( 1 << 1 )

#define ALL_CENT_SENSE_VERIFIED              ( CENT_0_RPM_SENSE_VERIFIED | CENT_SPEED_SENSE_VERIFIED )

// 12/03/96
// range of centrifuge RPM requried to be "seen" by safety at state trans from
// BLOOD_PRIME to BLOOD_RUN for T1 of centrifuge non-zero speed detection to pass
// previous low range limit was 150 RPM cahnged to 60 RPM per marlene due to
// decreased blood prime time for current startup design in Beta 2.3.
//
#define MIN_ALLOWABLE_CENT_SPD_BLOOD_RUN_TRANS     60.0f
#define MAX_ALLOWABLE_CENT_SPD_BLOOD_RUN_TRANS    250.0f

#define MACHINE_GOLD   0x01
#define MACHINE_CRC    0x02
#define SOFT_GOLD      0x04
#define SOFT_CRC       0x08
#define SAFE_GOLD      0x10
#define SAFE_CRC       0x20

#define ALL_CRC_FILES  ( MACHINE_GOLD | MACHINE_CRC | SOFT_GOLD | SOFT_CRC | SAFE_GOLD | SAFE_CRC )


/*+++++++++++++++++++++++++++++++++++++++++++++++++
        T1 Defines : End
++++++++++++++++++++++++++++++++++++++++++++++++++*/



/*******************************************************************************
 *
 *  aSoftTimer_Safety CLASS DEFINITION
 *
 ******************************************************************************/

aSoftTimer_Safety::aSoftTimer_Safety(short sTimerTickLimit, const char* name)
   : _cTimer(TIMER_DISARMED),
     _sTimerTickCount(0),
     _sTimerTickLimit(sTimerTickLimit),
     _myTimer( (unsigned long)SAFETY_SOFT_TIMER_PERIOD,
               Callback<aSoftTimer_Safety>(this, &aSoftTimer_Safety::notify), TimerMessage::DISARMED)
{
   strncpy(_cName, name, MAX_CHAR);
}



void aSoftTimer_Safety::StartTimer (void)
{
   _sTimerTickCount = 0;
   _cTimer          = TIMER_ARMED;
   _myTimer.armTimer(TimerMessage::ARMED);
   DataLog(log_level_safe_exec_debug) << "Timer: status=" << (int)_cTimer << "  Started " << _cName << " timer for " << _sTimerTickLimit << " cycles" << endmsg;
}



void aSoftTimer_Safety::StopTimer (void)
{
   _myTimer.armTimer(TimerMessage::DISARMED);
   _cTimer          = TIMER_DISARMED;
   DataLog(log_level_safe_exec_debug) << "Timer: status=" << (int)_cTimer << "  Stopped " << _cName << " timer at " << _sTimerTickCount << " cycles" << endmsg;
   _sTimerTickCount = 0;
}



void aSoftTimer_Safety::ResetTimer (short sTimerTickLimit)
{
   DataLog(log_level_safe_exec_debug) << "Timer: status=" << (int)_cTimer;

   if (_cTimer != TIMER_DISARMED)
   {
      _myTimer.armTimer(TimerMessage::DISARMED);
      _cTimer = TIMER_DISARMED;
      DataLog(log_level_safe_exec_debug) << " At cycle " << _sTimerTickCount << "reset running ";
   }
   else
      DataLog(log_level_safe_exec_debug) << " Reset ";

   DataLog(log_level_safe_exec_debug) << _cName << " timer to " << sTimerTickLimit << " cycles" << endmsg;

   _sTimerTickLimit = sTimerTickLimit;
   StartTimer();
}



void aSoftTimer_Safety::notify (void)
{
   if (_cTimer == TIMER_ARMED && _sTimerTickLimit > 0)  // 0 limit just lets timer free-run
   {
      if (++_sTimerTickCount > _sTimerTickLimit)
      {
         DataLog(log_level_safe_exec_debug) << "Timer: status=" << (int)_cTimer << "  Expired " << _cName << " timer at " << _sTimerTickCount << " cycles" << endmsg;
         _myTimer.armTimer(TimerMessage::DISARMED);
         _cTimer          = TIMER_LIMIT_EXPIRED;
         _sTimerTickCount = 0;
      }
   }
}



/*******************************************************************************
*
*  aSwitch_Safety CLASS DEFINITION
*
******************************************************************************/
aSwitch_Safety::aSwitch_Safety(const char* pszSwitchName,
                               short sTimerTickLimit,
                               ALARM_VALUES alarm_name,
                               aProcOpMonitor_Safety* paProcOpMonitor)
   : aSoftTimer_Safety(sTimerTickLimit, pszSwitchName),
     _alarm_name(alarm_name),
     _paProcOpMonitor(paProcOpMonitor)
{}



aSwitch_Safety::~aSwitch_Safety()
{
   _paProcOpMonitor = NULL;
}



void aSwitch_Safety::SetSwitchTimerStatus (char cTimerStatus)
{
   if (cTimerStatus == TIMER_ARMED)
      StartTimer();
   else if (cTimerStatus == TIMER_DISARMED)
      StopTimer();
}



/*******************************************************************************
 *
 *  Switch classes derived from aSwitch_Safety
 *
 ******************************************************************************/
aPauseSwitch_Safety::aPauseSwitch_Safety(aProcOpMonitor_Safety* paProcOpMonitor)
   : aSwitch_Safety("PauseSwitch", (short)PAUSE_TOLERANCE, (ALARM_VALUES)PAUSE_SW_FAILED, paProcOpMonitor)
{}



bool aPauseSwitch_Safety::checkOperationalStatus ()
{
   bool             result = false;
   SYSTEM_OP_STATUS operationalStatus;
   operationalStatus = _paProcOpMonitor->GetOperationalStatus();

   if ( (operationalStatus == SYSTEM_IS_PAUSED) ||
        (operationalStatus == SYSTEM_IS_STOPPED) )
   {
      result = true;
   }

   return result;
}



void aPauseSwitch_Safety::notify (void)
{
   const bool current = checkOperationalStatus();
   char       timerStatus;

   aSoftTimer_Safety::notify();

   timerStatus = GetTimerStatus();

   // did the switch cause Procedure to do the operation (Stop or Pause)
   // before the timer ran out?
   if (timerStatus == TIMER_ARMED)
   {
      if (current)
         StopTimer();

   }
   else if (timerStatus == TIMER_LIMIT_EXPIRED)
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, _alarm_name);

}



aStopSwitch_Safety::aStopSwitch_Safety(aProcOpMonitor_Safety* paProcOpMonitor)
   : aSwitch_Safety("StopSwitch", (short)STOP_TOLERANCE, (ALARM_VALUES)STOP_SW_FAILED, paProcOpMonitor)
{}



bool aStopSwitch_Safety::checkOperationalStatus ()
{
   bool result = false;

   if (_paProcOpMonitor->GetOperationalStatus() == SYSTEM_IS_STOPPED)
   {
      result = true;
   }

   return result;
}



void aStopSwitch_Safety::notify (void)
{
   const bool current = checkOperationalStatus();
   char       timerStatus;

   aSoftTimer_Safety::notify();

   timerStatus = GetTimerStatus();

   // did the switch cause Procedure to do the operation (Stop or Pause)
   // before the timer ran out?
   if (timerStatus == TIMER_ARMED)
   {
      if (current)
         StopTimer();
   }
   else if (timerStatus == TIMER_LIMIT_EXPIRED)
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, _alarm_name);
}



/*******************************************************************************
*
*  aCentrifuge_Safety CLASS DEFINITION
*
******************************************************************************/
aCentrifuge_Safety::aCentrifuge_Safety()
   : _cT1Status(NO_CENT_SENSE_VERIFIED),
     _RPM(0.0f),
     _commandRPM(0),
     _error(HW_NO_FAULT),
     _iCentrifugeErrorCount(0)
{}



void aCentrifuge_Safety::UpdateT1Status (T1STATUS cCentrifugeT1)
{
   static T1STATUS logEachOnce = 0;

   _cT1Status |= (T1STATUS)(1 << (int)cCentrifugeT1);

   if (_cT1Status != logEachOnce)
   {
      DataLog(log_level_safe_exec_info) << "Centrifuge T1 state changed.  Test "
                                        << hex << (int)cCentrifugeT1 << dec << " passed." << endmsg;

      logEachOnce = _cT1Status;
   }
}



void aCentrifuge_Safety::Update (float RPM, unsigned char error)
{
   _RPM   = RPM;
   _error = error;

   // check for and record 0 speed sensed for T1
   if ( (_commandRPM == 0.f) && (_RPM == 0.f ) )
      UpdateT1Status((T1STATUS)CENT_0_RPM_SENSE_NOMINAL);

   // check for overspeed
   if ( RPM > MAX_ALLOWABLE_CENT_SPD )
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, CENTRIFUGE_OVERSPEED);

   // check for centrifuge hardware error
   if (_error == (char)HW_CENT_HARDWARE_ERROR)
   {
      if (++_iCentrifugeErrorCount >= CENTRIFUGE_ERROR_DEBOUNCE)
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, CENTRIFUGE_ERROR);
   }
   else
   {
      if (--_iCentrifugeErrorCount < 0)
         _iCentrifugeErrorCount = 0;
   }
}



bool aCentrifuge_Safety::CentrifugeT1OK (void)
{
   bool result = false;

   if (_cT1Status == (T1STATUS)ALL_CENT_SENSE_VERIFIED)
      result = true;

   DataLog(log_level_safe_exec_info) << "Centrifuge T1 : "
                                     << std::hex << (int)_cT1Status << std::dec
                                     << "; Pass Value : ALL_CENT_SENSE_VERIFIED("
                                     << std::hex << (int)ALL_CENT_SENSE_VERIFIED << std::dec << ")" << endmsg;

   return result;
}



void aCentrifuge_Safety::CentrifugeAtBloodPrimeT1 (void)
{
   // check the centrifuge speed during Blood Prime
   // 12/03/96 min allowable 60.0 RPM ( 150 RPM previously )
   //          max allowable 250 RPM  ( same as previous )
   // de-magic these numbers.
   //
   if ( (_RPM) >= MIN_ALLOWABLE_CENT_SPD_BLOOD_RUN_TRANS &&
        (_RPM) <= MAX_ALLOWABLE_CENT_SPD_BLOOD_RUN_TRANS )
   {
      UpdateT1Status( (T1STATUS)CENT_SPEED_SENSE_NOMINAL);
   }

   DataLog(log_level_safe_exec_info) << "Centrifuge T1 RPM : " << _RPM
                                     << ".  Min(" << MIN_ALLOWABLE_CENT_SPD_BLOOD_RUN_TRANS << ") MAX("
                                     << MAX_ALLOWABLE_CENT_SPD_BLOOD_RUN_TRANS << ").  " << endmsg;

}



/*******************************************************************************
*
*  aBlinkTest_Safety CLASS DEFINITION
*
******************************************************************************/
aBlinkTest_Safety::aBlinkTest_Safety()
   : _test((char)VALVE_LED_TEST_NOMINAL) // start as OK
{}



// alarms if driver indicates the blink test is not working
void aBlinkTest_Safety::Update (unsigned char test)
{
   _test = test;

   if (_test != (unsigned char)VALVE_LED_TEST_NOMINAL)
   {
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, VALVE_LED_ERROR);
   }
}



bool aBlinkTest_Safety::BlinkT1OK (void)
{
   bool result = false;

   if (_test == VALVE_LED_TEST_NOMINAL)
      result = true;

   DataLog(log_level_safe_exec_info) << "Blink Test T1: " << (int)_test
                                     << "; Pass Value : VALVE_LED_TEST_NOMINAL(" << (int)VALVE_LED_TEST_NOMINAL << ")"
                                     <<  endmsg;

   return result;
}



/*******************************************************************************
*
*  aCRC_Safety CLASS DEFINITION
*
******************************************************************************/

// SPECIFICATION:     Constructor.
//
// ERROR HANDLING:    None.
// RETURN:            NA
//
aCRC_Safety::aCRC_Safety()
   : _machCRCGold(0),
     _machCRC(0),
     _softCRCGold(0),
     _softCRC(0),
     _safeCRCGold(0),
     _safeCRC(0),
     _allFilesFound(0),
     _cT1Status(0)
{}



// SPECIFICATION:     Opens a file and reads a word.
//
// ERROR HANDLING:    None.
// RETURN:            Var is filled with the word read.
//
bool aCRC_Safety::ReadCRC (const char* filename, long* var)
{
   int  fd;
   char buf[80];
   int  size;
   int  val;
   int  status = false;

   if ( (fd = open(filename, O_RDONLY | O_NONBLOCK | S_IRUSR | S_IRGRP | S_IROTH, 0) ) != ERROR)
   {
      size = read(fd, buf, sizeof(buf) - 1);

      if (size == ERROR)
      {
         DataLog(log_level_safe_exec_error) << "System status " << errnoMsg << " reading " << filename << endmsg;
      }
      else
      {
         buf[size] = 0;
         char* temp = NULL;

         val = strtoul(buf, &temp, 16);

         if (buf != temp) // This it testing for a non-empty file
         {
            *var   = val;
            status = true;

            DataLog(log_level_safe_exec_info) << "String: " << buf << " - Value: 0x" << std::hex
                                              << *var << std::dec << " read from " << filename << endmsg;
         }
         else
            DataLog(log_level_safe_exec_error) << "File reports EMPTY STRING.  String: " << buf << " read from " << filename << endmsg;

      }

      close(fd);
   }
   else
   {
      DataLog(log_level_safe_exec_error) << "Unable to open " << filename << ".  System status " << errnoMsg << endmsg;
   }

   return status;
}



// SPECIFICATION:     Checks that files exist.
//                    Returns after two are found to
//                    avoid overloading the internal
//                    ethernet with File I/O traffic.
//
// ERROR HANDLING:    None.
// RETURN:            NA
//
void aCRC_Safety::FindAllFiles ()
{
   if ( ( (_allFilesFound & MACHINE_GOLD) == 0) &&
        ReadCRC(MACHINE_CRC_FILE, &_machCRCGold)
        )
   {
      _allFilesFound |= MACHINE_GOLD;
   }

   if ( ( (_allFilesFound & MACHINE_CRC) == 0) &&
        ReadCRC(TEMP_MACHINE_CRC_FILE, &_machCRC)
        )
   {
      _allFilesFound |= MACHINE_CRC;
   }

   if ( ( (_allFilesFound & SOFT_GOLD) == 0) &&
        ReadCRC(TRIMA_CRC_FILE, &_softCRCGold)
        )
   {
      _allFilesFound |= SOFT_GOLD;
   }

   if ( ( (_allFilesFound & SOFT_CRC) == 0) &&
        ReadCRC(TEMP_TRIMA_CRC_FILE, &_softCRC)
        )
   {
      _allFilesFound |= SOFT_CRC;
   }

   if ( ( (_allFilesFound & SAFE_GOLD) == 0) &&
        ReadCRC(SAFETY_CRC_FILE, &_safeCRCGold)
        )
   {
      _allFilesFound |= SAFE_GOLD;
   }

   if ( ( (_allFilesFound & SAFE_CRC) == 0) &&
        ReadCRC(TEMP_SAFETY_CRC_FILE, &_safeCRC)
        )
   {
      _allFilesFound |= SAFE_CRC;
   }

   DataLog(log_level_safe_exec_info) << "All files: " << std::hex << _allFilesFound << std::dec << endmsg;
}



// SPECIFICATION:     Checks that all files exists.
//                    If all files are found, compares
//                    the CRCs in the pairs.
//
// ERROR HANDLING:    None.
// RETURN:            Sets _cT1Status to a bit significant
//                    value depending on which CRCs match
//
void aCRC_Safety::UpdateT1Status ()
{
   FindAllFiles();

   if ( (_allFilesFound == ALL_CRC_FILES) && (_cT1Status != ALL_CRC_FILES) )
   {
      if (_machCRCGold == _machCRC)
         _cT1Status |= (MACHINE_GOLD | MACHINE_CRC);

      if (_softCRCGold == _softCRC)
         _cT1Status |= (SOFT_GOLD | SOFT_CRC);

      if (_safeCRCGold == _safeCRC)
         _cT1Status |= (SAFE_GOLD | SAFE_CRC);
   }

   DataLog(log_level_safe_exec_info) << "T1 Status: " << std::hex << (int)_cT1Status << std::dec << "  All CRCs" << endmsg;
}



/*******************************************************************************
*
*  aCRC_Timer CLASS DEFINITION
*
******************************************************************************/

aCRC_Timer::aCRC_Timer()
   : _crc()
{}



void aCRC_Timer::CheckCRCs ()
{
   do
   {
      taskDelay(300);
      _crc.UpdateT1Status();
   } while (_crc.GetT1Status() != ALL_CRC_FILES);
}



bool aCRC_Timer::CRCT1OK (void)
{
   bool     result = false;

   T1STATUS statusCRC;

   statusCRC = _crc.GetT1Status();

   DataLog(log_level_safe_exec_info) << "CRC status: " << std::hex << (int)statusCRC << std::dec << endmsg;

   if (statusCRC == ALL_CRC_FILES)
      result = true;

   return result;
}



bool aCRC_Timer::CRCDisconnectOK (void)
{
   bool     result = false;

   T1STATUS statusCRC;

   statusCRC = _crc.GetT1Status();

   DataLog(log_level_safe_exec_info) << "CRC status: " << std::hex << (int)statusCRC << std::dec << endmsg;

   if ( (statusCRC & (SOFT_GOLD | SOFT_CRC)) == (SOFT_GOLD | SOFT_CRC) )
      result = true;

   return result;
}

/* FORMAT HASH d83fdf911642125ba4bd734989013da3 */

/*******************************************************************************
 *
 * Copyright (c) 1997 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.40.1.2  2009/10/05 16:04:17Z  dslausb
 * Shawn's safety code
 * Revision 1.1  2009/10/02 19:12:43Z  dslausb
 * Initial revision
 * Revision 1.40  2008/04/09 22:11:10Z  jheiusb
 * 8501 -- remove SNTP
 * Revision 1.39  2008/04/02 22:11:12Z  jheiusb
 * 8496 -- sync time on safety with control at start up
 * Revision 1.38  2008/01/23 17:32:43Z  dslausb
 * IT 8435 - VxWorks 5.5 Checkin
 * Revision 1.37  2006/12/28 20:05:42Z  rm70006
 * IT 7163.  Fix dlog statement
 * Revision 1.36  2006/10/31 20:35:01Z  jheiusb
 * 7712 --  add mss disconnect too
 * Revision 1.35  2006/10/23 18:32:10Z  jheiusb
 * 7712 -- MSS PFR is not time limited
 * Revision 1.34  2006/05/03 17:24:07Z  jheiusb
 * 7515 -- PFR for MSS Disconnect
 * Revision 1.33  2006/03/18 00:07:11Z  jheiusb
 * revised for PFR in MSS
 * Revision 1.32  2004/03/11 23:52:13Z  ms10234
 * 6900 - removed extra time drift compensation term in the PFR duration calculation.
 * Revision 1.31  2003/11/05 22:12:08Z  ms10234
 * 5959 - compensate for possible time drift between processors in PF conditions.
 * Revision 1.30  2003/09/09 22:43:38Z  ms10234
 * 6099 - change platelet depletion algorithm
 * Revision 1.29  2003/08/29 14:18:32Z  ms10234
 * 6390 - allow PFRs in ACPrime no matter how long the machine was off.
 * Revision 1.28  2003/07/18 15:36:42Z  jl11312
 * - modified power fail save to use raw disk writes
 * Revision 1.27  2003/07/01 22:34:55Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.26  2003/06/19 18:45:53Z  ms10234
 * 5829 - added back logging for PFR
 * Revision 1.25  2003/05/13 15:18:36Z  rm70006
 * IT 5806.
 *
 * Change logging calls to use errnoMsg manipulator.
 * Revision 1.24  2003/04/03 17:50:20Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.23  2002/12/27 09:22:37  pn02526
 * Runnable safety executive with CRC checking of the code disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.
 * Revision 1.22  2002/10/11 10:59:16  pn02526
 * Massive checkin of fixed modules for integration checking
 * Revision 1.21  2002/08/08 11:21:00  pn02526
 * Substitute #include of trimamessages.h for obsolete message system file msg.h.
 * Remove "trace" external; use DataLog_Default instead.
 * To keep GNU compiler happy for production builds
 * + Declare all untyped functions as void.
 * + Change cast of crcgen32 parameter from (char) to (unsigned char).
 * Change aPFR_Safety::SendPFRStatusMsg() to use new message system types and calls.
 * Revision 1.20  2002/07/23 16:52:27  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.19  2002/04/23 14:54:45  pn02526
 * Remove (comment out) QNX includes.  Revise file I/O for vxWorks.
 * Revision 1.18  2002/04/11 17:16:29  pn02526
 * Revise logging to use DataLogLevel class.
 * Revision 1.17  2001/07/18 21:57:41Z  jl11312
 * - modified to have safety notify proc of safety PFR file status
 * Revision 1.16  2001/06/07 21:47:18  ms10234
 * 3303 - PFR for v5.0
 * Revision 1.15  2000/03/15 18:22:01  BS04481
 * Remove PFR timer in favor of generalized pump timer in the
 * needle monitor; add saved data for needle monitor in order to
 * time pauses across power fails.
 * Revision 1.14  1999/10/04 23:00:59  BS04481
 * Restore component depletion monitor if the PFR data is reliable.
 * This will allow component depletion to be in effect during donor
 * disconnect even though the procedure is not recoverable.
 * Revision 1.13  1999/09/14 19:27:59  TD10216
 * IT4333
 * Revision 1.12  1999/08/01 20:50:02  BS04481
 * Don't allow PFR if time has gone backwards which could happen
 * if Everest syncs clocks as Trima is coming back from a PF.
 * Revision 1.11  1999/06/06 17:30:30  BS04481
 * Make decision on recoverability after a PF that happens during
 * first phase of rinseback dependent on time only.  In an AKO, only
 * disable the pump power timer if 3 ml (measured by encoders) has
 * been returned.
 * Revision 1.10  1999/05/31 16:53:01  BS04481
 * Remove path info from includes
 * Revision 1.9  1998/11/12 18:35:55  bs04481
 * If the procedure is not recoverable but the PFR data is reliable,
 * make the donor EC limit, the set type and the hypvolemia
 * monitor data available so that we can do hypo monitoring during
 * donor disconnect.
 * Revision 1.8  1998/09/28 22:18:06  bs04481
 * Recover from Donor Disconnect if the cassette was down when
 * power went off.  Also, block power until we determine if there is
 * AC in the needle.
 * Revision 1.7  1998/05/12 19:47:15  bs04481
 * Change reservoir monitoring, which includes ac ratio, ac infusion
 * and hypovolemia montiroing, to an object oriented design.
 * Previously, the volumes were accumulated in the s_res code.
 * With this change, the machine is modeled as six containers, each
 * consisting of some combination of bags, valves and pumps.  The
 * safety monitors now query the containers and calculate the
 * safety variables.  See the safety executive design document for
 * version 3.2 software.
 * Revision 1.6  1998/03/13 23:35:52  bs04481
 * Don't start the PFR pump power timer unless the cassette is down.
 * Revision 1.5  1998/01/22 18:01:26  bs04481
 * Fix potential problem with incorrectly deciding the anti-coagulation
 * of the needle if we paused or power failed in Rinseback before
 * we had pumped 3 ml back to the donor.
 * Revision 1.4  1997/12/12 17:12:17  bs04481
 * Make the pump power timer object which is used during PFR
 * usable for the pump power timer which is used after a stop
 * or pause button push.
 * Revision 1.3  1997/12/11 16:46:34  bs04481
 * Add CRC of Safety's power fail data
 * Revision 1.2  1997/12/05 21:09:10  bs04481
 * Change to disable the pump power timer if the timer has already
 * gone off because no one pushed the button but now we have
 * decided to go to donor disconnect.  Previously, the power was
 * off and it was necessary to recycle before the pumps could run
 * the disconnect test.
 * Revision 1.1  1997/12/02 16:00:33  bs04481
 * Initial revision
 *
 * TITLE:      s_pfr.cpp
 *             Safety Power Fail Recovery implementations.
 *
 *
 * ABSTRACT:   This file contains the class methods for the safety
 *             power fail object
 *
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#if !defined(__WIN32__)
#include <ctime>
#include <sysLib.h>
#include <ioLib.h>
#include "../os/power_fail.h"
#include "../os/os_comm.h"
#else
#include "node_data.h"
void trimaSysSaveSafetyPFData (char*, unsigned int) {}
int OSComm_ClearSafetyPFData = 0;
void osCommSend (int){}
int  sysGetNodeData (SysNodeID, SysNodeData*) { }
int  clock_settime (clockid_t, const struct timespec*) { return 0; }
#endif

#include "trima_datalog.h"
#include "node_data.h"
#include "trimamessages.h"
#include "s_pfr.hpp"
#include "s_type.h"
#include "error.h"
#include "states.h"
#include "s_donor.hpp" // for RETURN_CYCLE def
#include "acratio.hpp" // for MAX_ALLOWABLE_AC_RATIO def
#include "crcgen.h"    // for crc generation and checking
#include "s_exec.hpp"


// global definition for Safety PFR data
SPFRecovery SafetyPFRDataToSave;
SPFRecovery SafetyPFRDataFromRestore;

// SPECIFICATION:    Safety PFR Data constructor
//                   Examines and validate the saved data.
//                   Decides if the procedure is recoverable.
//
// ERROR HANDLING:   none.

aPFR_Safety::aPFR_Safety()
{}

void aPFR_Safety::init (aDonor* paDonor, aReservoir_Safety* paReservoir)
{
   // block pump power initially (and immediately)
   // we'll re-enable it if the procedure is recoverable or
   // if the cassette is up when we get the first safety status command
   paSafetyHardwareCommandsExec->BlockPumpPower(TRUE);

   // disallow PF Recovery until we know for sure
   memset(&SafetyPFRDataFromRestore, 0, sizeof(SPFRecovery));
   memset(&SafetyPFRDataToSave, 0, sizeof(SPFRecovery));
   SafetyPFRDataToSave.PFRSafetyOneShot      = FALSE;
   SafetyPFRDataFromRestore.PFRSafetyOneShot = FALSE;
   memset(&_SafetyPFRData, 0, sizeof(SPFRecovery));

   DataLog(log_level_safe_exec_debug) << "aPFR_Safety::aPFR_Safety() PFR structures initialized" << endmsg;

   _dt             = 0;
   _PFRChecksumOK  = FALSE;
   _PFRDurationOK  = FALSE;
   _PFRStateOK     = FALSE;
   _PFRNeedleOK    = FALSE;
   _PFRAlarmOK     = FALSE;
   _PFRTimerNeeded = FALSE;

   DataLog(log_level_safe_exec_debug) << "aPFR_Safety::aPFR_Safety() opening PF data file ("
                                      << sizeof(SPFRecovery) << ") byte(s)"  << endmsg;

   // open file
   int fd;
   if ( (fd = open(SAFETY_PFSAVE_FILE, O_RDONLY | O_NONBLOCK, S_IRUSR | S_IRGRP | S_IROTH) ) == -1)
   {
      // can't open file.  PFR not allowed.
      int error_no = errno;
      DataLog(log_level_safe_exec_error) << "System status " << errnoMsg(error_no) << " opening " << SAFETY_PFSAVE_FILE << endmsg;
      SafetyPFRDataToSave.PFRSafetyOneShot      = FALSE;
      SafetyPFRDataFromRestore.PFRSafetyOneShot = FALSE;

   }
   else
   {
      // we have a PF data file so read it in to the member variable space
      int size = read(fd, (char*)&_SafetyPFRData, sizeof(SPFRecovery));

      DataLog(log_level_safe_exec_debug) << "aPFR_Safety::aPFR_Safety() first read on PF data file." << endmsg;

      if (size <= 0)
      {
         int error_no = errno;
         DataLog(log_level_safe_exec_error) << "System status " << errnoMsg(error_no) << " reading "  << SAFETY_PFSAVE_FILE << endmsg;
      }
      else
      {
         DataLog(log_level_safe_exec_info) << "Read " << SAFETY_PFSAVE_FILE << endmsg;

         // ok, we've got data.  now let's figure out if we want to recover
         // is the CRC OK?
         setPFChecksum();

         // what state were we in?
         setPFState();

         // was there a safety alarm active?
         setPFAlarm();

         // if we are in a recoverable state and there was no active alarm and the data is not corrupt
         if ( (_PFRStateOK == TRUE) && (_PFRAlarmOK == TRUE)  && (_PFRChecksumOK == TRUE) )
         {
            // how long were we down?
            setPFDuration();

            // is there AC in the needle?
            setPFNeedle();

            // and we either have AC in the needle or the duration was short
            if ( (_PFRDurationOK == TRUE) || (_PFRNeedleOK == TRUE) )
            {
               // go for it
               // copy the local PFR data to the global PFR data
               memcpy(&SafetyPFRDataFromRestore, &_SafetyPFRData, sizeof(SPFRecovery));

               // allow pump power to be enabled when requested
               paSafetyHardwareCommandsExec->BlockPumpPower(FALSE);

               // set the PFR One shot to allow recovery
               SafetyPFRDataFromRestore.PFRSafetyOneShot = TRUE;
               SafetyPFRDataToSave.PFRSafetyOneShot      = TRUE;

               DataLog(log_level_safe_exec_info) << "Safety One Shot: "
                                                 << (SafetyPFRDataFromRestore.PFRSafetyOneShot==TRUE ? "TRUE" : "FALSE")
                                                 << "  PFR allowed"
                                                 << endmsg;

               // if we are here because we did not exceed the duration specification, we are
               // not out of the woods yet.  the inlet or return pump needs to move before
               // we are sure that the needle did not coagulate, so set a flag to start a timer
               // that will block the pump power if the pumps don't move soon enough
               if ( (_PFRNeedleOK == FALSE)
                    &&(SafetyPFRDataFromRestore.PFRCassette_position == HW_CASSETTE_DOWN) )
               {
                  _PFRTimerNeeded = TRUE;
               }

            }
            else
            {
               DataLog(log_level_safe_exec_info) << "Safety One Shot: "
                                                 << (SafetyPFRDataFromRestore.PFRSafetyOneShot==TRUE ? "TRUE" : "FALSE")
                                                 << "  PFR DISALLOWED.  DurationOK = " << _PFRDurationOK
                                                 << "  NeedleOK = " << _PFRNeedleOK
                                                 << endmsg;
            }
         }
         else
         {
            // we aren't in a recoverable state but check if the PFR data is
            // reliable and the cassette was down when the data was captured.
            // if so, we can make use of some of the data for hypovolemia monitoring
            // during Donor Disconnect.
            if ( (_PFRChecksumOK == TRUE)
                 &&(_SafetyPFRData.PFRCassette_position == HW_CASSETTE_DOWN) )
            {
               //
               //
               // copy the local PFR data to the global PFR data.  Note that each object
               // decides whether to use this data or not, independently, so there's no problem
               // with copying everything.
               //
               memcpy(&SafetyPFRDataFromRestore, &_SafetyPFRData, sizeof(SPFRecovery));


               //
               //
               //  The following monitors should always be active.  At this point we should only be going
               //  to donor disconnect.
               //
               SafetyExec::instance()->RestoreFromPFR();               // proc state, global set type and halls/encoders information
               paDonor->restoreDonorDataFromPFR();                     // donor data

               DataLog(log_level_safe_exec_error) << "Safety One Shot: "
                                                  << (SafetyPFRDataFromRestore.PFRSafetyOneShot==TRUE ? "TRUE" : "FALSE")
                                                  << "  PFR Not allowed but Disconnect monitoring data is available"
                                                  << endmsg;
            }

            // set the PFR oneshot to disallow recovery, although it should be disallowed above
            SafetyPFRDataFromRestore.PFRSafetyOneShot = FALSE;
         }
      }

      close(fd);
      remove(SAFETY_PFSAVE_FILE);
   }

   // create a file to save to if power fails again
   initializePFRFile();

}

// SPECIFICATION:    Saves the Safety PF Data file
//
// ERROR HANDLING:   none.

void aPFR_Safety::saveFile ()
{
   int         size;
   timespec    now, real;

   SysNodeData control;
   sysGetNodeData(ControlNode, &control);

   // copy global to local copy
   memcpy(&_SafetyPFRData, &SafetyPFRDataToSave, sizeof(SPFRecovery));

   // slap in the timestamp
   clock_gettime(CLOCK_REALTIME, &now);
   _SafetyPFRData.PFTimeOfFailure = now.tv_sec;

   // calculate the checksum and put it in the last word
   if ( (crcgen32(&_SafetyPFRData.PFRDataCRC, (unsigned char*)&_SafetyPFRData, (sizeof(SPFRecovery) - 4))) != 0)
      DataLog(log_level_safe_exec_error) << "CRC generation error in data for " << SAFETY_PFSAVE_FILE << endmsg;

   // write to file
   trimaSysSaveSafetyPFData((char*)&_SafetyPFRData, sizeof(SPFRecovery));
   DataLog(log_level_safe_exec_info) << "Safety power fail save complete" << endmsg;
}

// SPECIFICATION:    Safety PFR Data destructor
//                   Saves the Safety PF Data during
//                   a power failure
//
// ERROR HANDLING:   none.

aPFR_Safety::~aPFR_Safety()
{
// we're going down.  do we have a file?
   if (_pfSaveEnabled)
      saveFile();
   else
      DataLog(log_level_safe_exec_error) << "Power down, no file save" << endmsg;

}


// SPECIFICATION:    Create a Safety PF Data file
//                   Bypasses the filesystem cache
//
// ERROR HANDLING:   none.

void aPFR_Safety::initializePFRFile (void)
{
   // clear disk area for safety power fail data
   osCommSend(OSComm_ClearSafetyPFData);

   // clear the local data structure
   memset(&_SafetyPFRData, 0x0, sizeof(SPFRecovery));

   // put in a bad CRC just to be on the safe side
   _SafetyPFRData.PFRDataCRC = 0x1;

   _pfSaveEnabled            = true;
}

// SPECIFICATION:    Determine if the checksum of the power
//                   fail data is good
//
// ERROR HANDLING:   none.

void aPFR_Safety::setPFChecksum (void)
{
   unsigned long checksum = 0L;

   // calculate the checksum on the saved data.  our saved checksum
   // is in the last word so don't include it in the new checksum
   if ( (crcgen32(&checksum, (unsigned char*)&_SafetyPFRData, (sizeof(SPFRecovery) - 4))) != 0)
      DataLog(log_level_safe_exec_error) << "CRC Regeneration error" << endmsg;

   // if they match, we are good to go
   if (checksum == _SafetyPFRData.PFRDataCRC)
   {
      _PFRChecksumOK = TRUE;
      DataLog(log_level_safe_exec_info) << "CRC: "
                                        << "OK"
                                        << "   CRC Read back 0x"
                                        << std::hex << _SafetyPFRData.PFRDataCRC
                                        << " Calculated 0x"
                                        << checksum << std::dec
                                        << endmsg;
   }
   else
   {
      _PFRChecksumOK = FALSE;
      DataLog(log_level_safe_exec_error) << "CRC: "
                                         << "BAD"
                                         << "   CRC Read back 0x"
                                         << std::hex << _SafetyPFRData.PFRDataCRC
                                         << " Calculated 0x"
                                         << checksum << std::dec
                                         << endmsg;
   }

}


// SPECIFICATION:    Determine duration of power outage
//
// ERROR HANDLING:   none.

void aPFR_Safety::setPFDuration (void)
{
   //
   // We don't need to check the duration if we don't have a donor connected ...
   //
   if (_SafetyPFRData.PFR_SystemState == AC_PRIME)
   {
      // set PFR duration flag to allow recovery
      _PFRDurationOK = TRUE;
      DataLog(log_level_safe_exec_info) << "PFR Duration OK: TRUE -> ACPRIME state" << endmsg;
      return;
   }

   // PFR not time dependant for Metered storage states
   if (   (_SafetyPFRData.PFR_SystemState == METERED_STORAGE)
          || (_SafetyPFRData.PFR_SystemState == METERED_STORAGE_DISCONNECT))
   {
      // set PFR duration flag to allow recovery
      _PFRDurationOK = TRUE;
      DataLog(log_level_safe_exec_info) << "PFR Duration OK: TRUE -> METERED_STORAGE state" << endmsg;
      return;
   }

   timespec now;

   // we only need resolution to the second
   clock_gettime(CLOCK_REALTIME, &now);
   _dt = now.tv_sec - _SafetyPFRData.PFTimeOfFailure;


   DataLog_Default << "Time Now: " << now.tv_sec  << endmsg;
   DataLog_Default << "PFR Save time stamp->" << _SafetyPFRData.PFTimeOfFailure  << endmsg;
   DataLog_Default << "Duration of power outage: " << _dt << "  maximum: " << PFR_MAX_DURATION << endmsg;


   // if the PF duration has been less than the maximum allowed, go for it.
   if ( (_dt > 0) &&  (_dt < PFR_MAX_DURATION) )
   {
      // set PFR duration flag to allow recovery
      _PFRDurationOK = TRUE;
      DataLog(log_level_safe_exec_info) << "PFR Duration OK: "
                                        << "TRUE"
                                        << "  Time Read back "
                                        << _SafetyPFRData.PFTimeOfFailure
                                        << ", Time i think it is now -> "
                                        << now.tv_sec
                                        << endmsg;

   }
   else
   {
      // set PFR duration flag to block recovery if it has been too long
      // or if time has gone backwards
      _PFRDurationOK = FALSE;
      DataLog(log_level_safe_exec_error) << "PFR Duration OK: "
                                         << "FALSE"
                                         << "  Time Read back "
                                         << _SafetyPFRData.PFTimeOfFailure
                                         << ", Time i think it is now -> "
                                         << now.tv_sec
                                         << endmsg;
   }



}

// SPECIFICATION:    Determine if, at power fail, the machine was in
//                   a state from which recovery is possible
//
// ERROR HANDLING:   none.

void aPFR_Safety::setPFState (void)
{

   // we only recover from these states
   // we only recover from donor disconnect if the cassette was
   // down when we went down.
   if (
      (_SafetyPFRData.PFR_SystemState == AC_PRIME)
      ||(_SafetyPFRData.PFR_SystemState == DONOR_CONNECTED)
      ||(_SafetyPFRData.PFR_SystemState == BLOOD_PRIME)
      ||(_SafetyPFRData.PFR_SystemState == BLOOD_RUN)
      ||(_SafetyPFRData.PFR_SystemState == BLOOD_RINSEBACK)
      ||(_SafetyPFRData.PFR_SystemState == METERED_STORAGE)
      ||(_SafetyPFRData.PFR_SystemState == METERED_STORAGE_DISCONNECT)
      ||( (_SafetyPFRData.PFR_SystemState == DONOR_DISCONNECT)
          &&(_SafetyPFRData.PFRCassette_position == HW_CASSETTE_DOWN) ) )
      _PFRStateOK = TRUE;
   else
      _PFRStateOK = FALSE;

   // debug
   DataLog(log_level_safe_exec_info) << "PFR State OK: "
                                     << (_PFRStateOK==TRUE ? "TRUE" : "FALSE")
                                     << "  State Read back "
                                     << _SafetyPFRData.PFR_SystemState
                                     << endmsg;
}

// SPECIFICATION:    Determine if, at power fail, there was AC in the needle
//
// ERROR HANDLING:   none.

void aPFR_Safety::setPFNeedle (void)
{
   _PFRNeedleOK = FALSE;

   // first, is there AC is the set?  average AC ratio should be less than 15
   if (_SafetyPFRData.PFR_fAvgACRatio < MAX_ALLOWABLE_AC_RATIO )
   {
      // if we were in a return cycle during blood run, then make sure that
      // at least 3 ml of fluid was pumped out the needle.
      if ( (_SafetyPFRData.PFR_SystemState == BLOOD_RUN)
           &&(_SafetyPFRData.PFR_cSNCycle == RETURN_CYCLE)
           &&( (  _SafetyPFRData.PFRReturn_returnCycleVolume
                  - _SafetyPFRData.PFRInlet_returnCycleVolume ) >= PFR_AC_IN_NEEDLE_VOL ) )
         _PFRNeedleOK = TRUE;

      // alternatively, if we were in rinseback and there had been at least
      // 3 ml of fluid pumped out of the needle, then there is AC in the needle
      else if ( (_SafetyPFRData.PFR_SystemState == BLOOD_RINSEBACK)
                &&( (  _SafetyPFRData.PFRReturn_returnCycleVolume
                       - _SafetyPFRData.PFRInlet_returnCycleVolume ) >= PFR_AC_IN_NEEDLE_VOL ) )
         _PFRNeedleOK = TRUE;
   }

   DataLog(log_level_safe_exec_info) << "PFR Needle OK: "
                                     << (_PFRNeedleOK==TRUE ? "TRUE" : "FALSE")
                                     << "  Needle Read back  State: "
                                     << _SafetyPFRData.PFR_SystemState
                                     << " ACRatio: "
                                     << _SafetyPFRData.PFR_fAvgACRatio
                                     << " Cycle: "
                                     << (int)_SafetyPFRData.PFR_cSNCycle
                                     << " InletVol: "
                                     << _SafetyPFRData.PFRInlet_returnCycleVolume
                                     << " ReturnVol: "
                                     << _SafetyPFRData.PFRReturn_returnCycleVolume
                                     << endmsg;


}


// SPECIFICATION:    Determine if, at power fail, there was a
//                   Safety alarm active
//
// ERROR HANDLING:   none.

void aPFR_Safety::setPFAlarm (void)
{

   if (_SafetyPFRData.PFRSafetyAlarmInProgress == TRUE)
      _PFRAlarmOK = FALSE;
   else
      _PFRAlarmOK = TRUE;

   DataLog(log_level_safe_exec_info) << "PFR Alarm OK: "
                                     << (_PFRAlarmOK==TRUE ? "TRUE" : "FALSE")
                                     << " Alarm Read back "
                                     << _SafetyPFRData.PFRSafetyAlarmInProgress
                                     << endmsg;

}


// SPECIFICATION:    If there is a need for a timer to make sure that the
//                   pumps move after a PFR, this method kicks one off
//
// ERROR HANDLING:   none.

void aPFR_Safety::ifPFRTimerNeeded (needleMonitor* Needle)
{
   if (_PFRTimerNeeded == TRUE)
   {
      timespec now;
      clock_gettime(CLOCK_REALTIME, &now);
      long     duration = PFR_MAX_DURATION
                          - (now.tv_sec - SafetyPFRDataFromRestore.PFTimeOfFailure);
      Needle->turnOnTimer(duration);
   }
}


// SPECIFICATION:    Let proc know if the safety PFR file exists and
//                   the checksum is correct.
//
// ERROR HANDLING:   none.

void aPFR_Safety::SendPFRStatusMsg (void)
{
   SafetyPFRFileStatusMsg theSafetyPFRFileStatusMsg(MessageBase::SEND_GLOBAL);
   DataLog(log_level_safe_exec_info) << "aPFR_Safety::SendPFRStatusMsg(void) sending SafetyPFRFileStatusMsg(" << ( _PFRChecksumOK ? 1 : 0 ) << ")" << endmsg;
   theSafetyPFRFileStatusMsg.send(_PFRChecksumOK ? 1 : 0);
}

/* FORMAT HASH 798d8c0db1129fe7355dab6192e06199 */

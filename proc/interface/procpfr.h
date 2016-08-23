/*
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      procpfr.h
 *
 * ABSTRACT:   The ProcPFRHandler class contains the methods necessary
 *             to save and restore power fail data.
 *
 * $Header: //bctquad3/HOME/BCT_Development/vxWorks/Trima/proc/interface/rcs/procpfr.h 1.23 2003/08/19 16:49:46Z ms10234 Exp $
 * $Log: procpfr.h $
 * Revision 1.23  2003/08/19 16:49:46Z  ms10234
 * 6363 - added an initialized flag to allow early initialization for alarm cases.
 * Revision 1.22  2003/07/18 15:39:51Z  jl11312
 * - modified power fail to use raw disk writes
 * Revision 1.21  2003/07/08 22:18:02Z  ms10234
 * 5829 - changes for PFR
 * Revision 1.20  2003/06/19 18:43:24Z  ms10234
 * 5829 - changes for PFR
 * Revision 1.19  2003/05/09 18:55:54Z  ms10234
 * 5829 - Use adjust CDS and runtargets CDS instead of messages
 * Revision 1.18  2003/04/30 14:09:41Z  jl11312
 * - start of power fail recovery implementation
 * Revision 1.17  2003/04/04 21:03:58Z  ms10234
 * 5818 - Updated logging for new standard log levels
 * Revision 1.16  2002/10/28 23:30:54Z  ms10234
 * Changed proc files to use interface class - ProcData
 * Revision 1.15  2002/08/06 18:55:51  ms10234
 * Changed all int message names to end with "Msg"
 * Revision 1.14  2002/07/02 16:44:47  sb07663
 * Version that compiles (not working)
 * Revision 1.13  2002/06/21 21:24:08  sb07663
 * Mods to remove dynamic data and implement CDS
 * Revision 1.12  2002/06/05 14:25:43  sb07663
 * Modifications for compatibility with new Vx messaging system
 * Revision 1.11  2002/04/09 17:17:00  sb07663
 * Updates for compatibility with VxWorks port
 * Revision 1.10  2002/02/21 21:01:49  jl11312
 * - save/restore substate across power fails (IT 5601)
 * Revision 1.9  2002/01/11 21:16:12  pn02526
 * Add PFR handling for PQIMonitor's internal data
 * Move procpfr.cpp to interface in the process
 * Revision 1.8  2001/12/21 16:54:18  pn02526
 * Same as Revision 1.8  2001/09/27 11:51:08  jl11312, below.  It was moved from the proc directory to the interface directory.
 * Revision 1.1  2001/12/21 16:20:55  pn02526
 * Initial revision
 * Revision 1.8  2001/09/27 11:51:08  jl11312
 * - restore RBC calibration data after power fail (IT 5331)
 * Revision 1.7  2001/07/18 21:56:37  jl11312
 * - modified to have safety notify proc of safety PFR file status
 * Revision 1.6  2001/07/13 15:39:41  jl11312
 * - changes to save/restore adjust control information (IT 5196)
 * - changes to save/restore run targets (IT 5200)
 * Revision 1.5  2001/06/21 21:36:32  jl11312
 * - corrected handling of alarms during PFR (IT 4922)
 * Revision 1.4  2001/06/13 22:21:40  jl11312
 * - added ability to mark run data as unrecoverable
 * - added handling for restoring alarms on power fail recovery
 * - added handling for 1/3/10 minute pause alarms on power fail recovery (IT 4922)
 * Revision 1.3  2001/06/07 21:58:15  jl11312
 * - additional logging
 * - simplified file I/O functions
 * Revision 1.2  2001/06/05 23:21:17  pn02526
 * Enable PF saving of the Alarm Queue
 * Revision 1.1  2001/06/05 14:01:22  jl11312
 * Initial revision
 */

//
// Make sure file is included only once
//
#ifndef PROCPFR_H
#define PROCPFR_H

#include "adjustctrl.h"
#include "alarmqueue.h"
#include "cassette.h"
#include "crcgen.h"
#include "donor.h"
#include "trimamessages.h"
#include "states.h"

#include <time.h>

class ProcPFRHandler
{
public:
   ProcPFRHandler(void);
   virtual ~ProcPFRHandler();

   //
   // Initialize the handler.  This function will read the PFR file and
   //  ready the data for use when the system needs it.
   //
   void Initialize ();

   //
   // Routines can be used by proc to determine if power-fail data
   // is available, and if the system should allow recovery.  Routines
   // other than IsPFDataAvailable should only be called if power
   // fail data is available, otherwise they will generate a fatal error.
   //
   bool        ACCheck (void);
   ALARM_TYPES AlarmState (void) const;
   bool        IsPFDataAvailable (void) { return _dataAvailable; }
   bool        IsRunRecoverable (void) { return _runRecoverable; }
   bool        IsSafetyPFRFileOK (void) { return _safetyPFRFileOK; }
   double      MinutesSincePF (void) const;
   double      MinutesSincePumpsStopped (void) const;

   //
   // Restore routine handles restoring data to proc data area
   //
   void RestoreVitalData (void);
   void ForgetVitalData (void);
   void RestoreAllData (void);
   void MarkRestoreCheckDone (void);
   int  IsRestoreCheckDone (void) const { return _restoreCheckDone; }

   //
   // Routines for retrieving alarm information from the power fail data
   //
   void         ResetAlarmIndex (void);
   ALARM_VALUES GetNextAlarm (void);

   //
   // The proc task should call EnablePFSave when the procedure
   // has reached a point where power fail recovery is possible.
   // DisablePFSave should be called after the run has completed,
   // or after other events which prevent power fail recovery.
   // MarkUnrecoverable should be called after any event which
   // makes the current run un-continuable (e.g. an unreoverable
   // alarm has been set).
   //
   void EnablePFSave (void);
   void DisablePFSave (void);
   void MarkUnrecoverable (void);

   //
   // The DoPFSave function should be called as part of the cleanup
   // routine in proc.  It will perform the power fail save based
   // on previous calls to EnablePFSave and DisablePFSave.
   //
   void DoPFSave (void);

private:
   //
   // Class utility functions
   //
   void DoPFRestore (void);
   void adjustCurrentTime (long deltaTime);
   void EnablePFUpdate (void);
   void LogPFFileContents (void);
   bool LogPFFileInfo (void);
   bool ReadPFData (void);
   void ReceiveSafetyPFRFileStatus (void);
   void VerifyDataAvailable (int line) const;

private:
   //
   // The power fail save file contains the following data image
   //    struct PowerFailSaveData
   //    data from common data store
   //    AlarmQueueEntry array (size determined by PowerFailSaveData.alarmCount)
   //    file CRC (unsigned long)
   //
   // The information in the PowerFailSaveData struct in some cases duplicates
   // information already available in the CDS.  Note however, that the CDS can not
   // be restored until we have evaluated if power fail recovery is necessary.  The
   // duplicate information is used as part of this determination.
   //
   struct PowerFailSaveData
   {
      struct timespec pfTime;                   // time at power fail
      long            deltaTime;                // delta time drift since power on till power failure
      struct timespec pumpsStopTime;            // time since pumps stopped moving
      float           vNonAcBloodAtNeedle;      // negative if blood at the needle has AC mixed in it
      bool            runRecoverable;           // non-zero if the run was not marked as unrecoverable before power fail
      unsigned int    alarmCount;               // number of alarms active at power fail
   };

   struct PowerFailSaveHeader
   {
      unsigned int writeSize;
      time_t       writeTime;
   };

private:
   PowerFailSaveData _powerFailData;            // data from/to power fail save file
   AlarmQueueEntry*  _powerFailAlarms;          // alarm data from/to power fail save file

   bool              _pfSaveEnabled;            // power fail save enabled
   unsigned char*    _pfDataBuffer;             // power fail data image
   size_t            _pfDataBufferSize;         // size of pfDataBuffer
   unsigned char*    _pfDataBufferPtr;          // current position in data image for read/write

   bool              _dataAvailable;            // true if a valid power fail save file was detected
   int               _lastACCheck;              // last AC check status - used to control logging
   bool              _restoreCheckDone;         // true if proc has notified object that power-fail
                                                //  data restore is either complete or not necessary
   int               _nextAlarmIndex;           // alarm index to be returned by GetNextAlarm()

   bool              _runRecoverable;           // true if run has not been marked as unrecoverable
   bool              _updateOnly;               // true if should only update alarm information on
                                                //  power fail save

   bool                   _safetyPFRFileOK;     // true if safety reports a valid PFR file
   SafetyPFRFileStatusMsg _safetyPFRFileStatus; // setup to receive status message from safety

   bool                   _initialized;         // true after the initialized function has been called
};

#endif // ifndef PROCPFR_H

/* FORMAT HASH 42576538452c645fd6e972cbde151fb6 */

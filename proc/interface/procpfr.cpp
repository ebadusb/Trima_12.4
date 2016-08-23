/*
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      procpfr.h
 *
 * ABSTRACT:   The ProcPFRHandler class contains the methods necessary
 *             to save and restore power fail data.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/proc/interface/rcs/procpfr.cpp 1.27 2008/01/23 17:31:07Z dslausb Exp dslausb $
 * $Log: procpfr.cpp $
 * Revision 1.27  2008/01/23 17:31:07Z  dslausb
 * IT 8435 - VxWorks 5.5 Checkin
 * Revision 1.26  2007/11/21 15:17:37Z  dslausb
 * Add VxWorks.exe
 * Revision 1.25  2003/11/05 22:13:44Z  ms10234
 * 5959 - compensate for time drift between control and safety.
 * Revision 1.24  2003/08/19 16:49:10Z  ms10234
 * 6323, 6363 - Fixed code to allow PFR update before proc continues the run.
 * Revision 1.23  2003/08/06 19:21:45Z  jl11312
 * - fixed writing of alarm data
 * Revision 1.22  2003/07/18 15:39:47Z  jl11312
 * - modified power fail to use raw disk writes
 * Revision 1.21  2003/07/08 22:18:00Z  ms10234
 * 5829 - changes for PFR
 * Revision 1.20  2003/06/19 18:43:21Z  ms10234
 * 5829 - changes for PFR
 * Revision 1.19  2003/04/30 14:09:37Z  jl11312
 * - start of power fail recovery implementation
 * Revision 1.18  2003/04/04 21:03:57Z  ms10234
 * 5818 - Updated logging for new standard log levels
 * Revision 1.17  2002/11/20 17:43:52Z  rm70006
 * Use ANSI compliant date codes.
 * Revision 1.16  2002/08/06 18:55:50Z  ms10234
 * Changed all int message names to end with "Msg"
 * Revision 1.15  2002/07/18 20:17:32  jl11312
 * - added variable initialization to avoid compiler warnings
 * Revision 1.14  2002/07/02 16:44:46  sb07663
 * Version that compiles (not working)
 * Revision 1.13  2002/06/21 21:24:08  sb07663
 * Mods to remove dynamic data and implement CDS
 * Revision 1.12  2002/06/05 14:25:42  sb07663
 * Modifications for compatibility with new Vx messaging system
 * Revision 1.11  2002/05/13 14:41:34  sb07663
 * Updated 'unsigned char *' signature for crcgen32
 * Revision 1.10  2002/04/09 17:17:00  sb07663
 * Updates for compatibility with VxWorks port
 * Revision 1.9  2002/02/21 21:01:47  jl11312
 * - save/restore substate across power fails (IT 5601)
 * Revision 1.8  2002/01/11 21:23:27  pn02526
 * Add PFR handling for PQIMonitor's internal data - IT 5642
 * Move procpfr.cpp to interface in the process
 * Revision 1.1  2002/01/11 14:14:51  pn02526
 * Initial revision
 * Revision 1.7  2001/07/18 15:56:35  jl11312
 * - modified to have safety notify proc of safety PFR file status
 * Revision 1.6  2001/07/13 15:39:37  jl11312
 * - changes to save/restore adjust control information (IT 5196)
 * - changes to save/restore run targets (IT 5200)
 * Revision 1.5  2001/06/21 21:36:30  jl11312
 * - corrected handling of alarms during PFR (IT 4922)
 * Revision 1.4  2001/06/13 22:21:38  jl11312
 * - added ability to mark run data as unrecoverable
 * - added handling for restoring alarms on power fail recovery
 * - added handling for 1/3/10 minute pause alarms on power fail recovery (IT 4922)
 * Revision 1.3  2001/06/07 21:58:02  jl11312
 * - additional logging
 * - simplified file I/O functions
 * Revision 1.2  2001/06/05 23:21:15  pn02526
 * Enable PF saving of the Alarm Queue
 * Revision 1.1  2001/06/05 14:01:29  jl11312
 * Initial revision
 */

#include <vxworks.h>
#include <fcntl.h>
#include <ioLib.h>
#include <sys/stat.h>

#include "an_alarm.h"
#include "filenames.h"
#include "procdata.h"
#include "procpfr.h"
#include "rtc.h"
#include "../../os/power_fail.h"

//
// The ProcPFRHandler constructor checks for a valid power fail save
// file.  If a valid file exists, the data from the file is read to
// a local buffer area, and can be restored using the
// ProcPFRHandle::Restore... functions.
//
ProcPFRHandler::ProcPFRHandler(void)
   : _powerFailAlarms(NULL),
     _pfSaveEnabled(false),
     _pfDataBuffer(0),
     _pfDataBufferSize(0),
     _pfDataBufferPtr(0),
     _dataAvailable(false),
     _lastACCheck(-1),
     _restoreCheckDone(false),
     _nextAlarmIndex(0),
     _runRecoverable(true),
     _updateOnly(false),
     _safetyPFRFileOK(false),
     _safetyPFRFileStatus(Callback<ProcPFRHandler>(this, &ProcPFRHandler::ReceiveSafetyPFRFileStatus), MessageBase::SNDRCV_RECEIVE_ONLY),
     _initialized(false)
{
   _pfDataBufferSize = trimaSysControlPFSaveArea.nSecs * 512;
   _pfDataBuffer     = new unsigned char[_pfDataBufferSize];
}

//
// The ProcPFRHandler destructor disables the power fail save operation if it
// is currently enabled, and cleans up any dynamically allocated memory.
//
ProcPFRHandler::~ProcPFRHandler()
{
   //
   // If we get to this point with pfSaveEnabled == true
   // it means we are exiting with power fail enabled.
   //
   if ( _pfSaveEnabled )
   {
      DataLog(log_level_proc_pfr_info) << "power fail save handler exited without saving data" << endmsg;
   }

   //
   // Cleanup handler data
   //
   _dataAvailable = false;
   delete[] _powerFailAlarms;
   delete[] _pfDataBuffer;
}

void ProcPFRHandler::Initialize ()
{
   if ( !_initialized )
   {
      _initialized = true;

      //
      // Read and validate the control PFR file
      //
      _pfDataBufferPtr                    = _pfDataBuffer;
      trimaSysControlPFSaveArea.pBuf      = (char*)_pfDataBuffer;
      trimaSysControlPFSaveArea.direction = 0;
      ataRawio(0, 0, &trimaSysControlPFSaveArea);

      if ( LogPFFileInfo() )
      {
         DoPFRestore();
         if ( _dataAvailable )
         {
            EnablePFUpdate();
         }
      }
      else
      {
         DataLog(log_level_proc_pfr_info) << "no power fail save data available" << endmsg;
      }
   }
}


//
// Return highest priority alarm active in power fail
// save data, or -1 if no alarms are active. This
// function will generate a fatal error if called with
// no power fail data available.
//
ALARM_TYPES ProcPFRHandler::AlarmState (void) const
{
   VerifyDataAvailable(__LINE__);

   //
   // ALARM_TYPES is defined so that enumeration values are
   // in order of decreasing severity (i.e. the most severe alarm
   // type has a value of 0 and the least severe alarm type has
   // the highest value).
   //
   ALARM_TYPES alarmState = OPERATOR_ATTENTION;
   if ( _powerFailData.alarmCount > 0 )
   {
      alarmState = _powerFailAlarms[0].type();
      for ( int i = 1; i<_powerFailData.alarmCount; i += 1 )
      {
         if ( (int)_powerFailAlarms[i].type() < (int)alarmState )
         {
            alarmState = _powerFailAlarms[i].type();
         }
      }
   }

   return alarmState;
}

//
// Return number of minutes since power failure.  This
// function will generate a fatal error if called with no
// power fail data available.
//
double ProcPFRHandler::MinutesSincePF (void) const
{
   VerifyDataAvailable(__LINE__);

   struct timespec now;
   clock_gettime(CLOCK_REALTIME, &now);

   double dt = 0.0;
   dt  = (now.tv_sec - _powerFailData.pfTime.tv_sec) / 60.0;
   dt += (now.tv_nsec - _powerFailData.pfTime.tv_nsec) / 60.0 / 1.0e9;

   if ( dt < 0.0 )
   {
      DataLog(log_level_critical) << "saved power fail time = {" << _powerFailData.pfTime.tv_sec << "." << _powerFailData.pfTime.tv_nsec
                                  << "} current time = {" << now.tv_sec << "." << now.tv_nsec << "}" << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "invalid power fail time");
   }

   return dt;
}

//
// Return number of minutes since pump was last running. This
// function will generate a fatal error if called with no
// power fail data available.
//
double ProcPFRHandler::MinutesSincePumpsStopped (void) const
{
   VerifyDataAvailable(__LINE__);

   struct timespec now;
   clock_gettime(CLOCK_REALTIME, &now);

   double dt = 0.0;
   dt  = (now.tv_sec - _powerFailData.pumpsStopTime.tv_sec) / 60.0;
   dt += (now.tv_nsec - _powerFailData.pumpsStopTime.tv_nsec) / 60.0 / 1.0e9;

   if ( dt < 0.0 )
   {
      DataLog(log_level_critical) << "saved pump stop time = {" << _powerFailData.pumpsStopTime.tv_sec << "." << _powerFailData.pumpsStopTime.tv_nsec
                                  << "} current time = {" << now.tv_sec << "." << now.tv_nsec << "}" << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "invalid pumps stopped time");
   }

   return dt;
}

//
// Check that power fail recovery is disallowed due to coagulated
// blood in the needle.  Returns 0 if power fail recovery is disallowed
// due to coagulated blood, otherwise returns non-zero.  This
// function will generate a fatal error if called with no
// power fail data available.
//
bool ProcPFRHandler::ACCheck (void)
{
   VerifyDataAvailable(__LINE__);

   bool acCheck = false;
   if ( _powerFailData.vNonAcBloodAtNeedle <= 0 ||
        MinutesSincePumpsStopped() < 9.5 )
   {
      acCheck = true;
   }

   if ( (int)acCheck != _lastACCheck )
   {

      _lastACCheck = (int)acCheck;
      DataLog(log_level_proc_pfr_info) << "power fail AC check: vNonAcBloodAtNeedle=" << _powerFailData.vNonAcBloodAtNeedle
                                       << ", MinutesSincePumpsStopped=" << MinutesSincePumpsStopped()
                                       << ", AC check=" << acCheck
                                       << endmsg;
   }

   return acCheck;
}

//
// Reset the alarm index (i.e. the first alarm will be returned by GetNextAlarm()
//
void ProcPFRHandler::ResetAlarmIndex (void)
{
   VerifyDataAvailable(__LINE__);

   _nextAlarmIndex = 0;
}

//
// Get the next alarm in the list from the power fail data
//
ALARM_VALUES ProcPFRHandler::GetNextAlarm (void)
{
   ALARM_VALUES retVal = NULL_ALARM;

   while ( _nextAlarmIndex < _powerFailData.alarmCount &&
           retVal == NULL_ALARM )
   {
      ALARM_VALUES alarmValue = _powerFailAlarms[_nextAlarmIndex].name();
      if ( alarmValue != ONE_MINUTE_PAUSE &&
           alarmValue != MAINTAIN_DONOR_ACCESS_ALERT &&
           alarmValue != TEN_MINUTE_PAUSE )
      {
         retVal = alarmValue;
      }

      _nextAlarmIndex += 1;
   }

   return retVal;
}

void ProcPFRHandler::RestoreVitalData ()
{
   ProcData pd;
   pd.Adjustments().RestorePfData();
   pd.Donor().RestorePfData();
   pd.Predict().RestorePfData();
   pd.RunTargets().RestorePfData();
}

void ProcPFRHandler::ForgetVitalData ()
{
   ProcData pd;
   pd.Adjustments().ForgetPfData();
   pd.Donor().ForgetPfData();
   pd.Predict().ForgetPfData();
   pd.RunTargets().ForgetPfData();
}

void ProcPFRHandler::RestoreAllData ()
{
   DataStore::RestoreAllPfData();
}

//
// Mark power fail restore as complete.  This value is
// not used internally by the power fail handler, but is
// provided as a utility function for proc to allow it to
// determine if power-fail recovery has been either performed
// or determined to be unnecessary.
//
void ProcPFRHandler::MarkRestoreCheckDone (void)
{
   _restoreCheckDone = true;
}

//
// Enable power fail save.  This function should be called
// by proc when the procedure reaches a state at which
// power fail recovery is possible.
//
void ProcPFRHandler::EnablePFSave (void)
{
   _updateOnly = false;
   if ( !_pfSaveEnabled )
   {
      //
      // Clear power fail data buffer
      //
      memset(_pfDataBuffer, 0, _pfDataBufferSize);
      _pfDataBufferPtr = _pfDataBuffer;
      _pfSaveEnabled   = true;
      _dataAvailable   = false;

      delete[] _powerFailAlarms;
      _powerFailAlarms = NULL;

      //
      // Clear any saved data on disk
      //
      trimaSysControlPFSaveArea.pBuf      = (char*)_pfDataBuffer;
      trimaSysControlPFSaveArea.direction = 1;
      ataRawio(0, 0, &trimaSysControlPFSaveArea);

      DataLog(log_level_proc_pfr_info) << "power fail save enabled" << endmsg;
   }
}

//
// Disable power fail save.  This function should be called
// by proc when the procedure reaches a state at which power
// fail recovery should no longer be attempted.
//
void ProcPFRHandler::DisablePFSave (void)
{
   //
   // Clear power fail data buffer
   //
   memset(_pfDataBuffer, 0, _pfDataBufferSize);
   _pfDataBufferPtr = _pfDataBuffer;
   _pfSaveEnabled   = false;
   _dataAvailable   = false;

   //
   // Clear any saved data on disk
   //
   trimaSysControlPFSaveArea.pBuf      = (char*)_pfDataBuffer;
   trimaSysControlPFSaveArea.direction = 1;
   ataRawio(0, 0, &trimaSysControlPFSaveArea);

   DataLog(log_level_proc_pfr_info) << "power fail save disabled" << endmsg;
}

//
// Mark the current run as unrecoverable.
//
void ProcPFRHandler::MarkUnrecoverable (void)
{
   Initialize();

   if ( _runRecoverable || _powerFailData.runRecoverable )
   {
      DataLog(log_level_proc_pfr_info) << "marking run as unrecoverable" << endmsg;
      _runRecoverable               = false;
      _powerFailData.runRecoverable = false;
   }
}

//
// Enable power fail update (i.e. existing power fail recovery data
// is updated to reflect any additional alarm data).
//
void ProcPFRHandler::EnablePFUpdate (void)
{
   _updateOnly = true;
   if ( !_pfSaveEnabled )
   {
      memset(_pfDataBuffer, 0, _pfDataBufferSize);
      _pfDataBufferPtr = _pfDataBuffer;
      _pfSaveEnabled   = true;

      //
      // Clear any saved data on disk
      //
      trimaSysControlPFSaveArea.pBuf      = (char*)_pfDataBuffer;
      trimaSysControlPFSaveArea.direction = 1;
      ataRawio(0, 0, &trimaSysControlPFSaveArea);
   }
}

//
// Perform the actual power fail save operation.
//
void ProcPFRHandler::DoPFSave (void)
{
   ProcData pd;

   if ( _pfSaveEnabled && !_updateOnly )
   {
      //
      // Save time at power fail
      //
      timespec realTime;

      #if (CPU!=SIMNT) // This symbol is missing from the simulator.  We don't do PFR in sim anyway.
      getCurrentTimeFromRTC(&realTime);
      #endif // #if (CPU!=SIMNT)

      clock_gettime(CLOCK_REALTIME, &_powerFailData.pfTime);

      //
      // If the pumps are currently running, then the pumps stopped
      // time is the current time.  Otherwise, we use the pump stopped
      // time from proc dynamic data.
      //
      if ( pd.Run().StartPauseTime.Get().tv_sec == 0 )
      {
         //
         // pumps are not paused
         //
         pd.MakeRunWritable();
         pd.Run().StartPauseTime.Set(_powerFailData.pfTime);
         pd.MakeRunReadable();


         DataLog (log_level_proc_alarm_monitor_info) << " setting pd.Run().StartPauseTime.Get().tv_sec :" << pd.Run().StartPauseTime.Get().tv_sec << endmsg;

      }

      _powerFailData.deltaTime           = _powerFailData.pfTime.tv_sec - realTime.tv_sec;
      _powerFailData.pumpsStopTime       = pd.Run().StartPauseTime.Get();
      _powerFailData.vNonAcBloodAtNeedle = pd.Volumes().VnonACBloodAtNeedle.Get();
      _powerFailData.runRecoverable      = _runRecoverable;

      //
      // Create a copy of all CDS data ...
      //
      DataStore::SetAllPfData();

      //
      // Lock the alarms in the current state ...
      pd.AlarmQ().lock();
      _powerFailData.alarmCount = pd.AlarmQ().sizeOf();

      //
      // Write buffer must be long enough for PowerFailSaveData, the array of alarm entries,
      // and the unsigned long file CRC
      //
      size_t dataSize = sizeof( PowerFailSaveData ) + DataStore::PfCompleteLength() + _powerFailData.alarmCount * sizeof(AlarmQueueEntry);
      if ( dataSize + sizeof(unsigned long) + sizeof(PowerFailSaveHeader) <= _pfDataBufferSize )
      {
         PowerFailSaveHeader* header = (PowerFailSaveHeader*)_pfDataBuffer;
         header->writeSize = dataSize + sizeof(unsigned long);
         header->writeTime = time(NULL);
         _pfDataBufferPtr  = _pfDataBuffer + sizeof(PowerFailSaveHeader);

         memcpy(_pfDataBufferPtr, &_powerFailData, sizeof(PowerFailSaveData));
         _pfDataBufferPtr += sizeof(PowerFailSaveData);
         memcpy(_pfDataBufferPtr, DataStore::PfMemoryBlock(), DataStore::PfCompleteLength() );
         _pfDataBufferPtr += DataStore::PfCompleteLength();
         pd.AlarmQ().dump( (AlarmQueueEntry*)_pfDataBufferPtr);

         unsigned long crc = 0;
         crcgen32(&crc, &_pfDataBuffer[sizeof(PowerFailSaveHeader)], dataSize);
         memcpy(&_pfDataBuffer[dataSize + sizeof(PowerFailSaveHeader)], &crc, sizeof(unsigned long));

         trimaSysControlPFSaveArea.pBuf      = (char*)_pfDataBuffer;
         trimaSysControlPFSaveArea.direction = 1;
         ataRawio(0, 0, &trimaSysControlPFSaveArea);

         DataLog(log_level_proc_pfr_info) << "power fail save complete.  Saved "
                                          << dataSize << " bytes.  " << _pfDataBufferSize - sizeof(PowerFailSaveHeader)
                                          << " available." << endmsg;
      }
      else
      {
         DataLog(log_level_proc_pfr_error) << "power fail image (" << dataSize <<
            " bytes) exceeds available disk space (" << _pfDataBufferSize - sizeof(PowerFailSaveHeader) << " bytes)" << endmsg;
      }
   }
   else if ( _pfSaveEnabled && _updateOnly )
   {
      //
      // Power fail is occurring before the system has enabled power fail save and there
      // was an existing power fail save file.  Any additional alarm conditions must
      // be saved along with the original power fail recovery data.
      //
      int              additionalAlarmCount = 0;
      AlarmQueueEntry* additionalAlarm      = NULL;

      pd.AlarmQ().lock();
      if ( pd.AlarmQ().sizeOf() > 0 )
      {
         additionalAlarm = new AlarmQueueEntry[pd.AlarmQ().sizeOf()];
         list< AlarmQueueEntry* >::iterator alarmEntry;

         for ( alarmEntry = pd.AlarmQ().alarmQ().begin();
               alarmEntry != pd.AlarmQ().alarmQ().end();
               alarmEntry++ )
         {
            if ( (*alarmEntry)->name() != POWER_FAIL_CONTINUE &&
                 (*alarmEntry)->name() != POWER_FAIL_RINSEBACK &&
                 (*alarmEntry)->name() != POWER_FAIL_ABORT )
            {
               additionalAlarm[additionalAlarmCount] = **alarmEntry;
               additionalAlarmCount                 += 1;
            }
         }
      }

      unsigned int origAlarmCount = _powerFailData.alarmCount;
      _powerFailData.alarmCount += additionalAlarmCount;

      size_t originalDataSize = sizeof(PowerFailSaveData) + DataStore::PfCompleteLength() + origAlarmCount * sizeof(AlarmQueueEntry);
      size_t newDataSize      = originalDataSize + additionalAlarmCount * sizeof(AlarmQueueEntry);

      if ( newDataSize + sizeof(unsigned long) + sizeof(PowerFailSaveHeader) <= _pfDataBufferSize )
      {
         PowerFailSaveHeader* header = (PowerFailSaveHeader*)_pfDataBuffer;
         header->writeSize = newDataSize + sizeof(unsigned long);
         _pfDataBufferPtr  = _pfDataBuffer + sizeof(PowerFailSaveHeader);

         memcpy(_pfDataBufferPtr, &_powerFailData, sizeof(PowerFailSaveData));
         _pfDataBufferPtr += sizeof(PowerFailSaveData);
         memcpy(_pfDataBufferPtr, DataStore::PfMemoryBlock(), DataStore::PfCompleteLength() );
         _pfDataBufferPtr += DataStore::PfCompleteLength();
         memcpy(_pfDataBufferPtr, _powerFailAlarms, origAlarmCount * sizeof(AlarmQueueEntry));

         if ( additionalAlarmCount > 0 )
         {
            _pfDataBufferPtr += origAlarmCount * sizeof(AlarmQueueEntry);
            memcpy(_pfDataBufferPtr, additionalAlarm, additionalAlarmCount * sizeof(AlarmQueueEntry));
         }

         unsigned long crc = 0;
         crcgen32(&crc, &_pfDataBuffer[sizeof(PowerFailSaveHeader)], newDataSize);
         memcpy(&_pfDataBuffer[newDataSize + sizeof(PowerFailSaveHeader)], &crc, sizeof(unsigned long));

         trimaSysControlPFSaveArea.pBuf      = (char*)_pfDataBuffer;
         trimaSysControlPFSaveArea.direction = 1;
         ataRawio(0, 0, &trimaSysControlPFSaveArea);

         DataLog(log_level_proc_pfr_info) << "power fail alarm update complete" << endmsg;
      }
      else
      {
         DataLog(log_level_proc_pfr_error) << "power fail image (" << newDataSize <<
            " bytes) exceeds available disk space (" << _pfDataBufferSize - sizeof(PowerFailSaveHeader) << " bytes)" << endmsg;
      }

      if ( additionalAlarm )
      {
         delete[] additionalAlarm;
      }
   }
   else
   {
      DataLog(log_level_proc_pfr_info) << "shutdown with power fail save disabled" << endmsg;
   }
}

//
// Read the power fail save file into the local data buffers
//
void ProcPFRHandler::DoPFRestore (void)
{
   unsigned long dataCRC = 0;
   const char*   statusString;

   if ( !ReadPFData() )
   {
      statusString = "data read failed";
   }
   else if (    crcgen32(&dataCRC, (unsigned char*)&_powerFailData, sizeof(_powerFailData) )
                || crcgen32(&dataCRC, DataStore::PfMemoryBlock(), DataStore::PfCompleteLength() )
                || (    _powerFailAlarms
                        && crcgen32(&dataCRC, (unsigned char*)_powerFailAlarms, _powerFailData.alarmCount * sizeof(AlarmQueueEntry))
                        )
                )
   {
      statusString = "CRC generation failed";
   }
   else if ( dataCRC != *(unsigned long*)_pfDataBufferPtr )
   {
      statusString = "CRC mismatch";
   }
   else
   {
      statusString   = "valid power fail save data available";
      _dataAvailable = true;

      //
      // Take into account the possible time drift during the previous power-up to power failure
      //  by adding back in the accumulated drift from the PF data ...
      //
      adjustCurrentTime(_powerFailData.deltaTime);
   }

   DataLog(log_level_proc_pfr_info) << statusString << endmsg;
   if ( _dataAvailable )
   {
      LogPFFileContents();
   }
}

void ProcPFRHandler::adjustCurrentTime (long deltaTime)
{
   timespec now, adjustedNow;
   clock_gettime(CLOCK_REALTIME, &now);
   adjustedNow.tv_sec  = now.tv_sec + deltaTime;
   adjustedNow.tv_nsec = now.tv_nsec;
   clock_settime(CLOCK_REALTIME, &adjustedNow);


   DataLog(log_level_proc_pfr_info) << "Control time adjusted from " << now.tv_sec << "." << now.tv_nsec
                                    << " to " << adjustedNow.tv_sec << "." << adjustedNow.tv_nsec << endmsg;

}

//
// Generic read functions for restoring data from the power fail save file
//
template<class T> static inline int readFromData (T& obj, unsigned char* pfDataBuffer, size_t pfDataBufferSize, unsigned char* pfDataBufferPtr)
{
   int readSize = 0;
   if ( pfDataBufferPtr - pfDataBuffer + sizeof(obj) < pfDataBufferSize )
   {
      memcpy(&obj, pfDataBufferPtr, sizeof(obj));
      readSize = sizeof(obj);
   }

   return readSize;
}

template<class T> static inline int readFromData (T* obj, size_t count, unsigned char* pfDataBuffer, size_t pfDataBufferSize, unsigned char* pfDataBufferPtr)
{
   int readSize = 0;
   if ( pfDataBufferPtr - pfDataBuffer + count * sizeof(obj[0]) < pfDataBufferSize )
   {
      memcpy(obj, pfDataBufferPtr, count * sizeof(obj[0]));
      readSize = count * sizeof(obj[0]);
   }

   return readSize;
}

bool ProcPFRHandler::ReadPFData ()
{
   int readSize;

   readSize          = readFromData(_powerFailData, _pfDataBuffer, _pfDataBufferSize, _pfDataBufferPtr);
   _pfDataBufferPtr += readSize;

   if ( readSize > 0 )
   {
      unsigned char* cdsData = DataStore::PfMemoryBlock();
      readSize          = readFromData(cdsData, DataStore::PfCompleteLength(), _pfDataBuffer, _pfDataBufferSize, _pfDataBufferPtr);
      _pfDataBufferPtr += readSize;
   }
   else
      DataLog(log_level_proc_pfr_info) << "power fail data read failed" << endmsg;

   if ( readSize > 0 )
   {
      readSize = (DataStore::RetrieveAllPfData()) ? 1 : 0;
   }
   else
      DataLog(log_level_proc_pfr_info) << "power fail data cds read failed" << endmsg;

   if ( readSize > 0 )
   {
      if ( _powerFailData.alarmCount > 0 )
      {
         _powerFailAlarms  = new AlarmQueueEntry[_powerFailData.alarmCount];
         readSize          = readFromData(_powerFailAlarms, _powerFailData.alarmCount, _pfDataBuffer, _pfDataBufferSize, _pfDataBufferPtr);
         _pfDataBufferPtr += readSize;

         if ( !readSize )
            DataLog(log_level_proc_pfr_info) << "power fail data alarm read failed" << endmsg;
      }
   }

   return (readSize > 0);
}

//
// Log summary of power fail save file contents
//
void ProcPFRHandler::LogPFFileContents (void)
{
   enum { timeStringSize = 30 };
   char       pfTimeString[timeStringSize];
   char       pumpStopTimeString[timeStringSize];
   struct tm* timeData;

   timeData = localtime(&_powerFailData.pfTime.tv_sec);
   strftime(pfTimeString, timeStringSize, "%c", timeData);

   timeData = localtime(&_powerFailData.pumpsStopTime.tv_sec);
   strftime(pumpStopTimeString, timeStringSize, "%c", timeData);

   DataLog(log_level_proc_pfr_info) << "power fail time=" << pfTimeString
                                    << " pumps stopped time=" << pumpStopTimeString
                                    << " recoverable=" << _powerFailData.runRecoverable
                                    << " alarm count=" << _powerFailData.alarmCount
                                    << " alarm state=" << (int)AlarmState()
                                    << endmsg;
}

//
// Log information about the power fail save file at system startup
//
bool ProcPFRHandler::LogPFFileInfo ()
{
   PowerFailSaveHeader header;
   bool                result;

   memcpy(&header, _pfDataBufferPtr, sizeof(header));
   _pfDataBufferPtr += sizeof(header);

   if ( header.writeSize > 0 )
   {
      struct tm localTimeData;
      enum  { timeStringSize = 30 };
      char      timeString[timeStringSize];

      localtime_r(&header.writeTime, &localTimeData);
      strftime(timeString, timeStringSize, "%b %d %Y %X", &localTimeData);
      DataLog(log_level_proc_pfr_info) << "reading power fail save file: " << header.writeSize << " bytes, written " << timeString << endmsg;
      result = true;
   }
   else
   {
      DataLog(log_level_proc_pfr_info) << "power fail save data not available" << endmsg;
      result = false;
   }

   return result;
}

//
// Handle status message for safety PFR file
//
void ProcPFRHandler::ReceiveSafetyPFRFileStatus (void)
{
   _safetyPFRFileOK = (bool)_safetyPFRFileStatus.getData();
   DataLog(log_level_proc_pfr_info) << "safety PFR file status: " << _safetyPFRFileOK << endmsg;
}

//
// Utility function to check if power fail data is available.
// Generates a fatal error if no data is available.
//
void ProcPFRHandler::VerifyDataAvailable (int line) const
{
   if ( !_dataAvailable )
   {
      _FATAL_ERROR(__FILE__, __LINE__, "no power fail save data available");
   }
}

/* FORMAT HASH 3aec3b7882d2f71e58d953389d1c41b1 */

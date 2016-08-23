/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      procdata.h
 *
 * ABSTRACT:   This file contains the declaration for the interface
 *             for the globally used procedure data
 */

#ifndef _PROC_DATA_H_
#define _PROC_DATA_H_

#include <vxWorks.h>
#include <string>
#include <cmath>

#include "modaldata.hpp"
#include "config_cds.h"
#include "halstatus_cds.h"
#include "predict_cds.h"
#include "procadjust_cds.h"
#include "proccycle_cds.h"
#include "procdonor_cds.h"
#include "procpqi_cds.h"
#include "procplsrinseback_cds.h"
#include "procrun_cds.h"
#include "procruntargets_cds.h"
#include "proctime_cds.h"
#include "proctrimaset_cds.h"
#include "procvolumes_cds.h"
#include "xif_cds.h"
#include "halorders.h"
#include "timekeeper.h"
#include "alarmqueue.h"
#include "procpfr.h"
#include "adjustctrl.h"
#include "alarms.h"
#include "trimamessages.h"
#include "substatemap.h"
#include "shw.hpp"
#include "runsummary.h"
#include "guiproc.h"
#include "clearablealarm.h"
#include "timermessage.h"

// TODO: Fix this compromise/workaround, as it's not strictly necessary here.
// It's no longer included by config_cds.h, but since so many proc modules use
// Software_CDS without providing this include, it's not practical to fix now.
#include "software_cds.h"

class ProcData
   : public AbsModalData
{
public:

   enum UpdateModeType
   {
      TEST,         // anything before prime
      ACPRIME,      // after creation, but before blood prime
      PRIME,        // after AC Prime, but before blood run
      BLOODRUN,     // during blood run
      RECOVERY,     // if in a recovery mode
      RINSEBACK,    // during rinseback
      DISCONNECT,   // during disconnect test
      SOLUTION,     // Storage Solution addition
      POSTRUN       // post run
   };

public:

   //
   // Default constructor
   ProcData();

   //
   // Default destructor
   virtual ~ProcData();

   //
   // Function to update procedure data based on state
   void Update (const UpdateModeType mode);

   //
   // Get operators for the proc data ...
   Config_CDS&           Config ()                { return _TheProcData->_ConfigurationData; }
   HalStatus_CDS&        Status ()                { return _TheProcData->_StatusData; }
   Predict_CDS&          Predict ()               { return _TheProcData->_PredictData; }
   ProcAdjust_CDS&       Adjustments ();
   ProcCycle_CDS&        Cycle ();
   ProcDonor_CDS&        Donor ();
   ProcPQI_CDS&          PQI ();
   ProcPlsRinseback_CDS& PlasmaRinseback ()       { return _TheProcData->_PlsRinsebackData; }
   ProcRun_CDS&          Run ();
   ProcRunTargets_CDS&   RunTargets ()            { return _TheProcData->_RunTargetsData; }
   ProcTrimaSet_CDS&     TrimaSet ();
   ProcVolumes_CDS&      Volumes ();
   External_IF_CDS&      xIf ()                   { return _TheProcData->_xifData; }
   CHalOrders&           Orders ()                { return _TheProcData->_Orders; }
   TimeKeeper&           AbsTime ()               { return _TheProcData->_AbsTime; }
   TimeKeeper&           ProcTime ()              { return _TheProcData->_ProcTime; }
   TimeKeeper&           SysRunTime ()            { return _TheProcData->_SysRunTime; }
   TimeKeeper&           ProcRunTime ()           { return _TheProcData->_ProcRunTime; }
   TimeKeeper&           MSSRunTime ()            { return _TheProcData->_MSSRunTime; }
   AlarmQueue&           AlarmQ ()                { return _TheProcData->_AlarmQueue; }
   ProcPFRHandler&       PFRHandler ()            { return _TheProcData->_PFRHandler; }
   AdjustCtrl&           AdjustControl ()         { return _TheProcData->_AdjustControl; }
   const SHwStates&   SafetyStatus ()          { return _TheProcData->_SafetyStatus.getData(); }
   bool&              AlarmActive ()           { return _TheProcData->_AlarmActive; }
   ALARM_VALUES&      DisplayedAlarm ()        { return _TheProcData->_DisplayedAlarm; }
   unsigned int       ButtonPressCount () const { return _TheProcData->_ButtonPressCounter; }
   long               ButtonPress ()            const { return _TheProcData->_ButtonMsg.getData().BUTTON_ID; }
   const char*        ButtonPressId ()         const { return _TheProcData->_ButtonMsg.getData().BUTTON_NAME; }
   const char*        ButtonPressScreenId ()   const { return _TheProcData->_ButtonMsg.getData().SCREEN_NAME; }
   const State_names& SystemState ()  const { return _TheProcData->_SystemState; }
   const char*        SystemStateStr ()        { return SubstateMap::ssStr(_TheProcData->_SystemState); }
   const string&      SubstateName () const { return _TheProcData->_SubstateName; }
   const string&      VistaSubstateName () const { return _TheProcData->_VistaSubstateName; }
   const string&      RecoveryName () const { return _TheProcData->_RecoveryName; }
   TimeKeeper&        CurrentCycleTime ()      { return _TheProcData->_CurrentCycleTime; }
   float              LastReturnCycleTime ()    { return _TheProcData->_LastReturnCycleTime; }
   bool               SkipCycleForRatioCalc ()  { return _TheProcData->_SkipCycleForRatioCalc; }
   float              RBCVolumeDeltaForCycle () { return _TheProcData->_RBCVolumeDeltaForCycle; }
   bool               PredictBlocked ()         { return _TheProcData->_PredictBlocked; }
   bool               NewAKO ()                 { return _TheProcData->_NewAKO; }
   float              GetAbsTimeNowinMinutes () { return _TheProcData->_TimeData.AbsTime(); }



   bool LogMSSPumps (void) const {return _TheProcData->_logMSSPumps; }


   void RequestWbcAlarm (const char* file, const int lineNum, const char* logMessage) { _TheProcData->requestWbcAlarm(file, lineNum, logMessage); }
   void RequestObtrusiveWbcAlarm (const char* file, const int lineNum, const char* logMessage) { _TheProcData->requestObtrusiveWbcAlarm(file, lineNum, logMessage); }

   //
   // These functions change the permissions on our writable datastores to
   //  allow one and only one user to change the datastore ...
   bool MakeAdjustmentsWritable ();
   void MakeAdjustmentsReadable ();
   bool MakeCycleWritable ();
   void MakeCycleReadable ();
   bool MakeDonorWritable ();
   void MakeDonorReadable ();
   bool MakePQIWritable ();
   void MakePQIReadable ();
   bool MakeRunWritable ();
   void MakeRunReadable ();
   bool MakeTrimaSetWritable ();
   void MakeTrimaSetReadable ();
   bool MakeVolumesWritable ();
   void MakeVolumesReadable ();


   //
   // Set operators ...
   void AlarmActive (const bool f);
   void DisplayedAlarm (const ALARM_VALUES a);
   void SystemState (const State_names s, const bool global = true);
   void SubstateName (const string& s);
   void VistaSubstateName (const string& s);
   void RecoveryName (const string& s);
   void LastReturnCycleTime (const float minutes) { _TheProcData->_LastReturnCycleTime = minutes; }
   void SkipCycleForRatioCalc (const bool f)      { _TheProcData->_SkipCycleForRatioCalc = f; }
   void RBCVolumeDeltaForCycle (const float v)    { _TheProcData->_RBCVolumeDeltaForCycle = v; }
   void PredictBlocked (const bool f)             { _TheProcData->_PredictBlocked = f; }
   void SafetyStatusCb (const CallbackBase& cb)   { _TheProcData->_SafetyStatusCb = cb; }
   void NewAKO (const bool ako)                     { _TheProcData->_NewAKO = ako; }
   void PlasmaRinsebackActive (bool state)          { _TheProcData->_PlasmaRinsebackActive = state; }
   void SalineRinsebackActive (bool state)          { _TheProcData->_SalineRinsebackActive = state; }
   void LogMSSPumps (const bool l)                  { _TheProcData->_logMSSPumps = l; }
   void SetPluggedFilter ();
   bool Filter1Plugged ();
   bool Filter2Plugged ();

   // for rinsebcak monitoring
   bool isPlasmaRinsebackActive ()                  { return _TheProcData->_PlasmaRinsebackActive; }
   bool isSalineRinsebackActive ()                  { return _TheProcData->_SalineRinsebackActive; }

   // Cassette volume constants
   float ResVolume ();
   float MixVolume ();
   float ReCircVolume ();
   float NominalFillVolume ();
   float ExtracorporealVolume ();
   float TotalResidualVolume ();

   // Reservoir alarm thresholds based on cassette capacity
   float DrawTooShort ();
   float DrawTooLong ();
   float ReturnTooShort ();
   float ReturnTooLong ();
   float FirstCycleTooShort ();
   float FirstCycleTooLate ();
   float DrawHighLimit ();

   //
   // Perform cycle switching
   void SwitchtoDraw (void);
   void SwitchtoReturn (void);
   void ResetReservoir (void);
   void LogFirstCycleVolumes (void);
   void GiveFirstCycleSound (void);

   //
   // Transfer donor info for possible message sending
   bool FillInDonorStruct (SDonorVitals& donorStruct);
   void FillInDonorUnits (SDonorVitals& donorStruct);

   void DoorLockTestStarted (void)     { _TheProcData->_DoorLockTestStarted = true; }
   void DoorLockTestFinished (void)     { _TheProcData->_DoorLockTestStarted = false; }
   bool DoorLockTestInProgress (void)  { return _TheProcData->_DoorLockTestStarted; }

   bool SafetyControlReservoirIsEmpty (void);
   bool IsProductCollectionSubstate ();


public:
   //
   // AbsModalData inteface implementations
   //
   virtual void definePeriodicSet (DataLog_SetHandle pset);
   virtual void defineEORSet (DataLog_SetHandle pset);

   static const int totalSalineToDonor ();

private:
   //
   // Helper functions for periodic logging
   //
   static const char* displayAlarm (const ALARM_VALUES& alarm);
   static int         displayAlarmAsInt (const ALARM_VALUES& alarm);
   static const char* displayButtonPress ();
   static const char* displayButtonString ();
   static const float calcPASVolume ();
   static const float calcRASDelivered ();

   static const int RBCCalibrationLogging () ;

private:

   class ProcDataStruct
   {
   public:
      ProcDataStruct();

      void getRunSummaryData (RUN_SUMMARY_STRUCT& data);

      void requestWbcAlarm (const char* file, const int lineNum, const char* logMessage);
      void requestObtrusiveWbcAlarm (const char* file, const int lineNum, const char* logMessage);

      //
      // Read only common data stores...
      Config_CDS           _ConfigurationData;
      HalStatus_CDS        _StatusData;
      Predict_CDS          _PredictData;
      ProcAdjust_CDS       _ROAdjustmentsData;
      ProcAdjust_CDS       _RWAdjustmentsData;
      void*                _AdjustmentsDataWriter;
      ProcCycle_CDS        _ROCycleData;
      ProcCycle_CDS        _RWCycleData;
      void*                _CycleDataWriter;
      ProcDonor_CDS        _RODonorData;
      ProcDonor_CDS        _RWDonorData;
      void*                _DonorDataWriter;
      ProcPQI_CDS          _ROPQIData;
      ProcPQI_CDS          _RWPQIData;
      void*                _PQIDataWriter;
      ProcPlsRinseback_CDS _PlsRinsebackData;

      ProcRun_CDS          _RORunData;
      ProcRun_CDS          _RWRunData;
      void*                _RunDataWriter;
      ProcRunTargets_CDS   _RunTargetsData;
      ProcTime_CDS         _TimeData;
      ProcTrimaSet_CDS     _ROTrimaSetData;
      ProcTrimaSet_CDS     _RWTrimaSetData;
      void*                _TrimaSetDataWriter;
      ProcVolumes_CDS      _ROVolumesData;
      ProcVolumes_CDS      _RWVolumesData;
      void*                _VolumesDataWriter;

      External_IF_CDS      _xifData;

      //
      // HAL information
      CHalOrders _Orders;

      // Absolute Time ( Total time since Procedure started ) ...
      TimeKeeper _AbsTime;
      // Procedure Time ( Total time since Procedure started, not
      //  including pauses ) ...
      TimeKeeper _ProcTime;
      // System Run Time ( Time since the donor was connected till
      //  the donor was disconnected )...
      TimeKeeper _SysRunTime;
      // Procedure Run Time ( Time since the donor was connected
      //  till the donor was disconnected, not including pauses ) ...
      TimeKeeper _ProcRunTime;

      // time of Metered storgae procedure, not including pauses and operator interfaces
      TimeKeeper          _MSSRunTime;

      AlarmQueue          _AlarmQueue;
      ProcPFRHandler      _PFRHandler;
      AdjustCtrl          _AdjustControl;
      PredictRespondedMsg _PredictResponseMsg;

      //
      // Status as seen by safety
      Message< SHwStates > _SafetyStatus;
      CallbackBase         _SafetyStatusCb;

      // System wide flag informing that the system is in an
      //  alarm state ...
      bool         _AlarmActive;
      ALARM_VALUES _DisplayedAlarm;

      //
      // Message which saves the current button press
      Message< BUTTON_PRESS_MESSAGE > _ButtonMsg;
      // ButtonPressedIDMsg _ButtonMsg;
      unsigned int                    _ButtonPressCounter;

      //
      // Message for requesting run summary data
      RequestEndRunStatsMsg         _SendStatsMsg;
      RequestGenericWbcAlarm        _RequestGenericWbcAlarmMsg;
      CancelMssMsg                  _cancelMssMsg;
      Message< RUN_SUMMARY_STRUCT > _StatsMsg;

      //
      // Message for requesting MSS Cancel during PFR
      RequestCancelMssPfr _RequestCancelMssPfr;

      //
      // Current substate name
      State_names _SystemState;

      //
      // Current substate name
      string _SubstateName;
      string _VistaSubstateName;

      //
      // Current recovery name (Blank if no recovery active)
      string     _RecoveryName;

      TimeKeeper _CurrentCycleTime;
      float      _LastReturnCycleTime;

      bool       _SkipCycleForRatioCalc;
      float      _RBCVolumeDeltaForCycle;

      //
      // Flag to keep track of Predict process status ...
      bool _PredictBlocked;

      //
      // Flag to keep track of whether the first cycle sound has been given ...
      bool _FirstCycleSoundGiven;

      bool _NewAKO;

      bool _PlasmaRinsebackActive;
      bool _SalineRinsebackActive;

      // log metered storage pump movements more aggressively
      bool           _logMSSPumps;

      ClearableAlarm _SilentPause;      // pause to double check ULS
      TimerMessage   _PauseTimer;
      int            _FCVChecks;
      bool           _FluidStillOnULS;

      bool           _DoorLockTestStarted;

      enum DepotStateEnum
      {
         DEPOT_RINSEBACK,
         DEPOT_SOLUTION_ADDITION,
         DEPOT_ADDITION_DONE,
         DEPOT_POSTRUN,
         NUM_DEPOT_STATES
      };

      // Goal is to calculate summary data only when necessary
      bool               _runSummWritten[NUM_DEPOT_STATES];
      RUN_SUMMARY_STRUCT _runSummData;


   protected:
      //
      // Callback function for the runtargets message used for logging...
      void predictRespondedCb ();

      //
      // Callback function for the safety status message ...
      void safetyStatusCb () { _SafetyStatusCb(); }

      //
      // Callback function for the button press message used for logging...
      void buttonPressCb ();

      //
      // Callback function for the PFR MSS cancelation message...
      void cancelMssRequestedPfrCb ();

      //
      // Callback function for the generic WBC alarm request message...
      void genericWbcAlarmRequested ();

      //
      // Callback function for end run summary request
      void sendRunSummary ();

      void setMssUserCancelFlag ();

      // clear the silent ULS pause alarm
      void clearTimer ();

      void logEORData ();
   };

   //
   // Task wide static data pointer ...
   static ProcDataStruct* _TheProcData;
   static int             _TheOwningTaskId;

private:
   ProcData(const ProcData&);              // not implemented
   ProcData& operator = (const ProcData&); // not implemented

};

#endif

/* FORMAT HASH bec6f79693e6ee357e82315af62f1581 */

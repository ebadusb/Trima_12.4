/*******************************************************************************
 *
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      vip_sts.h
 *             Vista Interface Status process
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the interface between Vista and Trima version 5.
 *
 ******************************************************************************/

#ifndef INCLUDE_VIP_STS
#define INCLUDE_VIP_STS

#include "timermessage.h"    // Messaging System stuff
#include "trimamessages.h"

#include "vipif.h"           // Include vista interface message definitions.

// Trima Message Definitions
#include "alarms.h"          // Include class for alarm msgs
#include "scan_structure.h"  // Include class for Scan Structure
#include "safproc.h"         // Include struct for donor vitals
#include "runsummary.h"      // Include struct for end of run summary
#include "pqinformation.h"   // Include class for Reason List
#include "bio_signatures.h"  // Include bio signature messages
#include "pfrmsg.h"          // Include PFR status message

// CDS includes
#include "proctrimaset_cds.h"  // Gets cassette State
#include "procvolumes_cds.h"   // Gets procedure volumes
#include "procrun_cds.h"       // Gets procedure run info
#include "proctime_cds.h"      // Get various proc times
#include "config_cds.h"        // Gets trima configuration
#include "halstatus_cds.h"     // Gets Pump Volumes
#include "xif_cds.h"           // Gets external interface flags

#include "vip_status_msg.h"           // Gets the Vista status message object class
#include "vip_periodic_status_msg.h"  // Gets the Vista Periodic Status Message filler class


class VipStatusProcess
{
public:

   VipStatusProcess();
   virtual ~VipStatusProcess();

   //
   // Initialize the class.
   //
   void Init ();


   //
   // Cleanup does a gracefull shutdown of the class.
   //
   void Cleanup ();



// Private Member functions.
private:
   //
   // Message System Handler Methods
   //

   //
   // Callback routine that handles the Vista Status Request Msg
   //
   void HandleVistaStatusMsg ();



   //
   // Callback routine that handles the Vista Machine Status Request Msg
   //
   void HandleMachineStatusMsg ();



   //
   // Callback routine that handles the Pin Signature Request message from GUI
   //
   void HandleLogItemMsg ();



   //
   // Callback for the Run Status Event Timer Message
   //
   void HandleRunStatusTimerMsg ();



   //
   // Callback for the Machine Status Event Timer Message
   //
   void HandleMachineStatusTimerMsg ();


   //
   // VIP Interface message handlers
   //

   //
   // Callback routine that handles the Barcode message
   //
   bool HandleBarcodeMsg (const SCAN_STRUCTURE& in, barcodeContentsMsg& out);


   //
   // Callback routine that handles the Donor Rejected Msg
   //
   bool HandleDonorRejectedMsg (const int& in, donorRejectedMsg& out);


   //
   // Callback routine that handles the Procedure Param Adj. Msg from Trima
   //
   bool HandleProcedureParamAdjMsg (const int& in, procedureParameterAdjustmentMsg& out);


   //
   // Callback routine that handles the Donor Vitals Update Msg from Trima
   //
   bool HandleDonorVitalsUpdatedMsg (const SDonorVitals& in, donorVitalsUpdatedMsg& out);


   //
   // Callback routine that handles the End Of Run Summary Msg from Trima
   //
   bool HandleEndRunSummaryMsg (const RUN_SUMMARY_STRUCT& in, endRunSummaryMsg& out);


   //
   // Callback routine that handles the Alarm Msg from Trima
   //
   bool HandleAlarmMsg (const GUI_Alarm_struct& in, alarmMsg& out);


   //
   // Callback routine that handles the GUI Alarm Response Msg from Trima
   //
   bool HandleGuiAlarmResponseMsg (const Alarm_response_struct& in, alarmAckMsg& out);


   //
   // Callback routine that handles the PFR Status message from Trima
   //
   bool HandlePfrStatus (const PFR_Status& in, powerFailRecoveryIndicationMsg& out);


   //
   // Callback routine that handles the Operator button push message from GUI
   //
   bool HandleButtonPushMsg (const BUTTON_PRESS_MESSAGE& in, operatorButtonPushMsg& out);


   //
   // Callback routine that handles the Pin Signature Request message from GUI
   //
   bool HandlePinSignatureRequestMsg (const PinSignatureRequestMsg& in, pinAuthorizationEventMsg& out);



   //
   // Helper functions.
   //

   //
   // Helper routine that fills in reason codes for each reason.
   //
   void PhilInReasonCodes (int* ptr, int& arraySize, ENUMERATED_FLAG flag, const char* msg);


   //
   // Helper routine that starts the status messages to Vista
   //
   void StartStatusMsgs ();


   //
   // Helper routine that stops the status messages to Vista
   //
   void StopStatusMsgs ();


   //
   // Helper routine that copies reason codes for binary output
   //
   int getPQIReasons (DataLog_UINT32* array, ENUMERATED_FLAG flag);


// Data Members
private:
   VipStatusProcess(const VipStatusProcess&);              // noncopyable
   VipStatusProcess& operator = (const VipStatusProcess&); // nonassignable

   // CDS instances
   External_IF_CDS  _xifCds;
   ProcTrimaSet_CDS _procTrimaSetCds;
   ProcVolumes_CDS  _procVolumeCds;
   ProcRun_CDS      _procRunCds;
   ProcTime_CDS     _procTimeCds;
   HalStatus_CDS    _halCds;

   // Internally uses a CDS.
   PQInformation _pqInfo;

   // Messages from Trima.
   Message<startPeriodicStatusMsg> _vistaRunStatusMsg;
   Message<startMachineStatusMsg>  _vistaMachineStatusMsg;
   Message<CFR11_Log_Type>         _cfr11LogItemMsg;

   // Timer Messages
   TimerMessage _runStatusTimer;
   TimerMessage _machineStatusTimer;

   // External Messages to Vista.
   VipMessageObj<trimaPeriodicStatusMsg> _trimaRunPeriodicStatusMsg;
   trimaPeriodicStatusMsg                _trimaRunPeriodicStatus;

   VipMessageObj<trimaMachineStatusMsg>  _trimaMachineStatusMsg;
   trimaMachineStatusMsg                 _trimaMachineStatus;

   // Struct messages
   VipOneToOneStatusMsg<VipStatusProcess, RUN_SUMMARY_STRUCT,     endRunSummaryMsg>*                    _endRunSummary;
   VipOneToOneStatusMsg<VipStatusProcess, SCAN_STRUCTURE,         barcodeContentsMsg>*                  _barcodeContents;
   VipOneToOneStatusMsg<VipStatusProcess, GUI_Alarm_struct,       alarmMsg>*                            _alarm;
   VipOneToOneStatusMsg<VipStatusProcess, Alarm_response_struct,  alarmAckMsg>*                         _alarmAck;
   VipOneToOneStatusMsg<VipStatusProcess, PinSignatureRequestMsg, pinAuthorizationEventMsg>*            _pinSignatureRequest;
   VipOneToOneStatusMsg<VipStatusProcess, PFR_Status,             powerFailRecoveryIndicationMsg>       _pfrIndication;
   VipOneToOneStatusMsg<VipStatusProcess, BUTTON_PRESS_MESSAGE,   operatorButtonPushMsg>*               _operatorButtonPush;

   VipOneToOneWrappedStatusMsg<VipStatusProcess, DVitalsFromProc, SDonorVitals, donorVitalsUpdatedMsg>* _donorVitalsUpdated;

   // Int messages
   VipOneToOneIntStatusMsg<VipStatusProcess, VistaDonorRejectedMsg,  donorRejectedMsg>                 _donorRejected;
   VipOneToOneIntStatusMsg<VipStatusProcess, ProcedureAdjustmentMsg, procedureParameterAdjustmentMsg>* _procedureParamAdj;

   // Helper Classes
   VistaPeriodicStatusMsg _vistaRunPeriodicStatusMsg;

   // Send Socket
   sockinetaddr    _trimaStatusAddr;
   sockinetaddr    _machineStatusAddr;

   ALARM_VALUES    _prevAlarmName;
   ALARM_STATES    _prevAlarmState;
   ALARM_RESPONSES _prevAlarmResponse;
};

#endif

/* FORMAT HASH 173cbdd5fc5eea6f92de65a615ce2e4b */

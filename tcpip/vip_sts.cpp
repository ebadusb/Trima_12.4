/*******************************************************************************
 *
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      vip_sts.cpp
 *             Vista Interface Status process
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This class manages the status interface with Vista
 *
 ******************************************************************************/

#include "vip_sts.h"

#include "vipif.h"           // Vista Interface message formats

#include "messagesystem.h"   // Messaging System stuff.
#include "trima_datalog.h"   // Logging Stuff
#include "trima_tasks.h"     // Task synchronization stuff
#include "trima_assert.h"    // assert stuff
#include "software_cds.h"    // Gets feature settings

#include "units.h"           // Gets units and conversions.
#include "adjust.h"          // Needed for adjustment message types.
#include "tedif.h"           // Needed for Biometric signature list



extern "C" void vip_sts ()
{
   DataLog (log_level_vip_info) << "vip_sts process started" << endmsg;

   MessageSystem ms;

   // Blocking loop that receives messages.
   ms.initBlocking("VIP Status", MessageSystemConstant::DEFAULT_Q_SIZE * 2);

   DataLog (log_level_vip_info) << "vip_sts creating Status Process Object" << endmsg;

   VipStatusProcess* vip_sts = new VipStatusProcess();

   // Initialize object
   vip_sts->Init();

   TASK_DONE;  // Signal to sysinit that the task is initialized.

   ms.dispatchMessages();

   vip_sts->Cleanup();

   delete vip_sts;
}


//
// Standard class constructor
//
VipStatusProcess::VipStatusProcess()
   : // DataStores
     _xifCds(ROLE_RW),
     _procTrimaSetCds(ROLE_RO),
     _procVolumeCds(ROLE_RO),
     _procTimeCds(ROLE_RO),
     _procRunCds(ROLE_RO),
     _halCds(ROLE_RO),

     _vistaRunStatusMsg    (Callback<VipStatusProcess>(this, &VipStatusProcess::HandleVistaStatusMsg),   MessageBase::SNDRCV_RECEIVE_ONLY),
     _vistaMachineStatusMsg(Callback<VipStatusProcess>(this, &VipStatusProcess::HandleMachineStatusMsg), MessageBase::SNDRCV_RECEIVE_ONLY),
     _cfr11LogItemMsg      (Callback<VipStatusProcess>(this, &VipStatusProcess::HandleLogItemMsg),       MessageBase::SNDRCV_RECEIVE_ONLY),

     _runStatusTimer    (0, Callback<VipStatusProcess>(this, &VipStatusProcess::HandleRunStatusTimerMsg),     TimerMessage::DISARMED),
     _machineStatusTimer(0, Callback<VipStatusProcess>(this, &VipStatusProcess::HandleMachineStatusTimerMsg), TimerMessage::DISARMED),

     _trimaRunPeriodicStatusMsg(_trimaRunPeriodicStatus, _trimaStatusAddr),
     _trimaMachineStatusMsg(_trimaMachineStatus,         _machineStatusAddr),

     _endRunSummary(0),
     _barcodeContents(0),
     _donorVitalsUpdated(0),
     _alarm(0),
     _alarmAck(0),

     _procedureParamAdj(0),
     _operatorButtonPush(0),

     _pinSignatureRequest(0),

// These messages are registered for initially because they have trima side-effects besides sending
// messages to vista (like setting CDS values).
     _donorRejected(_trimaStatusAddr, VIP_DONOR_REJECTED_MSG, this, &VipStatusProcess::HandleDonorRejectedMsg),
     _pfrIndication(_trimaStatusAddr, VIP_POWER_FAIL_RECOVER_MSG, this, &VipStatusProcess::HandlePfrStatus),

     _prevAlarmName((ALARM_VALUES)-1),
     _prevAlarmState((ALARM_STATES)-1),
     _prevAlarmResponse((ALARM_RESPONSES)-1)

{
   DataLog(log_level_vip_debug) << "Constructor Complete" << endmsg;
}



//
// Standard class destructor
//
VipStatusProcess::~VipStatusProcess()
{
   delete _endRunSummary;
   delete _barcodeContents;
   delete _alarm;
   delete _alarmAck;
   delete _pinSignatureRequest;
   delete _donorVitalsUpdated;
   delete _procedureParamAdj;
   delete _operatorButtonPush;
}



//
// Cleanup
//
void VipStatusProcess::Cleanup ()
{
   // Stop the Trima Run Periodic Timer
   _runStatusTimer.interval(0);
   _machineStatusTimer.interval(0);

   // Turn off the status messages
   StopStatusMsgs();
}



//
// Init
//
void VipStatusProcess::Init ()
{
   // Reset the sequence number
   VipMessageObj<int>::reset();

   // Init the Periodic Status Message
   memset(&_trimaRunPeriodicStatus, 0, trimaPeriodicStatusMsgLength);

   // Signal vip_cmd that it is OK to accept a connection from Vista
   _xifCds.vistaStatusReady.Set(true);

   //
   // Sanity tests to ensure that external interface and internal interface constants
   // are consistent
   //
   TRIMA_STATIC_ASSERT(VIP_MAX_REASONS == REASON_TO_SHOW_END_OF_LIST - 1);
   TRIMA_STATIC_ASSERT(VIP_SCAN_STRING_LENGTH == MAX_SCAN_LENGTH);
   TRIMA_STATIC_ASSERT(VIP_BIO_MAX_BADGE_LENGTH == BARCODE_ID_SIZE);
   TRIMA_STATIC_ASSERT(VIP_BIO_MAX_PIN_LENGTH == PIN_SIZE);
   TRIMA_STATIC_ASSERT((int)VIP_MAX_ADJUSTMENTS == (int)END_OPERATOR_INITIATED_ADJUSTMENTS);

   DataLog(log_level_vip_info) << "VIP STS initialized" << endmsg;
}



//
// This routine sends on to Vista the Periodic Status Message.
// This routine is triggered from the timer message set on the
// initial request from Vista.
//
void VipStatusProcess::HandleRunStatusTimerMsg ()
{
   static bool donorCommitMsgNotSent = true;

   // Update the message contents
   _vistaRunPeriodicStatusMsg.Update(_trimaRunPeriodicStatus);

   // Send the message
   _trimaRunPeriodicStatusMsg.Send(VIP_PERIODIC_STATUS_MSG);
   DataLog(log_level_vip_debug) << "Trima Run Status sent." << endmsg;

   // This test determines whether donor is "committed" to
   // Vista.  This means that the donor is considered deferred if the run is later aborted.
   if (donorCommitMsgNotSent)
   {
      const ProcTrimaSet_CDS::CassetteStateType cassetteState = _procTrimaSetCds.CassetteState;

      if ( (cassetteState >= ProcTrimaSet_CDS::BLOOD) && (cassetteState <= ProcTrimaSet_CDS::RINSEBACK) )
      {
         donorCommittedMsg                msg;
         VipMessageObj<donorCommittedMsg> msgObj(msg, _trimaStatusAddr);

         DataLog(log_level_vip_info) << "Donor Committed." << endmsg;

         // Send it.
         msgObj.Send(VIP_DONOR_COMMIT_MSG);

         _xifCds.donorCommitted.Set(true);
         donorCommitMsgNotSent = false;


         //
         // Send the Regionalization Features once on Donor Commit
         //
         struct FeatureSet
         {
            DataLog_UINT32 featureId;
            DataLog_UINT32 value;
         };

         FeatureSet logItem;

         // Log the Regionalization information for Vista
         for (int i = 0; i < (int)FEATURE_DEF_END; i++)
         {
            logItem.featureId = i;
            logItem.value     = Software_CDS::GetInstance().getFeature((FEATURE_ID)i);

            // Log binary entry (for Vista)
            datalog_WriteBinaryRecord(log_handle_vista_binary, VISTA_RecordType, VISTA_RegionalizationFeatureSubType, &logItem, sizeof(logItem));
         }

         // Log the Regionalization information for Vista
         for (int i = 0; i < (int)VALUE_FEATURE_DEF_END; i++)
         {
            logItem.featureId = i;
            logItem.value     = Software_CDS::GetInstance().getValueFeature((VALUE_FEATURE_ID)i);

            // Log binary entry (for Vista)
            datalog_WriteBinaryRecord(log_handle_vista_binary, VISTA_RecordType, VISTA_RegionalizationConstantSubType, &logItem, sizeof(logItem));
         }

         // Log the Confirmation Code plus the null string (for Vista)
         string code = Software_CDS::GetInstance().getFullConfirmationCode();
         datalog_WriteBinaryRecord(log_handle_vista_binary, VISTA_RecordType, VISTA_ConfirmationCodeSubType, (void*)code.c_str(), code.size() + 1);
      }
   }
#if 0
   else
   {
      // Vip Cmd Lost Connection to Vista.  Close existing socket connection.
      DataLog(log_level_vip_info) << "Stopping existing messages." << endmsg;

      // Stop the Periodic Timer
      _runStatusTimer.interval(0);
      _machineStatusTimer.interval(0);

      // Reset the sequence number
      VipMessageObj<int>::reset();

      // Turn off the status messages
      StopStatusMsgs();
   }
#endif
}



//
// This routine sends on to Vista the Machine Status Message.
// This routine is triggered from the timer message set on the
// initial request from Vista.
//
void VipStatusProcess::HandleMachineStatusTimerMsg ()
{
   // Update the message contents
   _trimaMachineStatus.acPump.commandedRate           = _halCds.ACPump.CmdFlow();
   _trimaMachineStatus.acPump.actualRate              = _halCds.ACPump.ActFlow();
   _trimaMachineStatus.acPump.accumulatedVolume       = _procVolumeCds.Vac;

   _trimaMachineStatus.inletPump.commandedRate        = _halCds.InletPump.CmdFlow();
   _trimaMachineStatus.inletPump.actualRate           = _halCds.InletPump.ActFlow();
   _trimaMachineStatus.inletPump.accumulatedVolume    = _procVolumeCds.Vin;

   _trimaMachineStatus.plasmaPump.commandedRate       = _halCds.PlasmaPump.CmdFlow();
   _trimaMachineStatus.plasmaPump.actualRate          = _halCds.PlasmaPump.ActFlow();
   _trimaMachineStatus.plasmaPump.accumulatedVolume   = _procVolumeCds.PlasmaVolume;

   _trimaMachineStatus.plateletPump.commandedRate     = _halCds.CollectPump.CmdFlow();
   _trimaMachineStatus.plateletPump.actualRate        = _halCds.CollectPump.ActFlow();
   _trimaMachineStatus.plateletPump.accumulatedVolume = _procVolumeCds.PlateletVolume +
                                                        _procVolumeCds.VReplacement   +
                                                        _procVolumeCds.VSalineBolus;

   _trimaMachineStatus.returnPump.commandedRate     = _halCds.ReturnPump.CmdFlow();
   _trimaMachineStatus.returnPump.actualRate        = _halCds.ReturnPump.ActFlow();
   _trimaMachineStatus.returnPump.accumulatedVolume = _procVolumeCds.Vreturn;


   _trimaMachineStatus.replacementPump.commandedRate     = _halCds.CollectPump.CmdFlow();
   _trimaMachineStatus.replacementPump.actualRate        = _halCds.CollectPump.ActFlow();
   _trimaMachineStatus.replacementPump.accumulatedVolume = _procVolumeCds.VReplacement + _procVolumeCds.VSalineBolus;

   // Send the message
   _trimaMachineStatusMsg.Send(VIP_MACHINE_STATUS_MSG);
}



//
// This routine handles the Vista Status Message from VIP CMD.
//
void VipStatusProcess::HandleVistaStatusMsg ()
{
   const startPeriodicStatusMsg statusMsg = _vistaRunStatusMsg.getData();

   DataLog(log_level_vip_info) << "Rx new Run Status Request Msg for port " << statusMsg.portNumber
                               << ", period " << statusMsg.repeatPeriod << " ms." << endmsg;

   // Set the address for the Vista Status message
   _trimaStatusAddr.setport(statusMsg.portNumber);
   _trimaStatusAddr.setaddr(_xifCds.replyToAddress.Get(), sockinetaddr::network);

   DataLog(log_level_vip_info) << "Run Status messages will be sent to " << _trimaStatusAddr << endmsg;

   _trimaRunPeriodicStatusMsg.setAddr(_trimaStatusAddr);

   // Set the timer msg to trigger the status message to Vista every N seconds (configured by Vista).
   _runStatusTimer.interval(statusMsg.repeatPeriod);

   // Start the status messages.
   StartStatusMsgs();
}



//
// This routine handles the Machine Status Message from VIP CMD.
//
void VipStatusProcess::HandleMachineStatusMsg ()
{
   const startMachineStatusMsg statusMsg = _vistaMachineStatusMsg.getData();

   DataLog(log_level_vip_info) << "Rx new Machine Status Request Msg.  Rpt " << statusMsg.repeatPeriod << ", Port "
                               << statusMsg.portNumber << endmsg;

   // Set the address for the Machine Status message
   _machineStatusAddr.setaddr(_xifCds.replyToAddress.Get(), sockinetaddr::network);
   _machineStatusAddr.setport(statusMsg.portNumber);

   DataLog(log_level_vip_info) << "Machine Status messages will be sent to " << _trimaStatusAddr << endmsg;

   _trimaMachineStatusMsg.setAddr(_machineStatusAddr);

   // Set the timer msg to trigger the status message to Vista every N seconds (configured by Vista).
   _machineStatusTimer.interval(statusMsg.repeatPeriod);
}



//
// This routine handles the CFR11 Log Item message from GUI.
// NOTE:  This message does not go out over the Vista interface.  It is logged in the dlog file
// and read by Vista when they play back the file.
//
void VipStatusProcess::HandleLogItemMsg ()
{
   CFR11_Log_Type logItem = _cfr11LogItemMsg.getData();

   // Log binary entry (for Vista)
   datalog_WriteBinaryRecord(log_handle_vista_binary, VISTA_RecordType, VISTA_AuditTrailSubType, &logItem, sizeof(logItem));
}



//
// This routine handles the Barcode Message from the GUI
//
bool VipStatusProcess::HandleBarcodeMsg (const SCAN_STRUCTURE& in, barcodeContentsMsg& out)
{
   datalog_Print (log_level_vip_info.getHandle(),
                  __FILE__,
                  __LINE__,
                  "Barcode Contents CAT(%d) SEQ(%d) ST(%d) STR(%s) LEN(%d), SYM(%d).",
                  in.scan_category,
                  in.scan_sequence,
                  (int)in.status,
                  in.scan_string,
                  in.scan_strlen,
                  in.scan_symbology_char[0]);

   // Create the Vista outbound message.
   out.scanCategory     = in.scan_category;
   out.scanSequence     = in.scan_sequence;
   out.scanStatus       = in.status;
   out.scanStringLength = in.scan_strlen;
   out.scanSymbol       = in.scan_symbology_char[0];

   strncpy (out.scanString, in.scan_string, VIP_SCAN_STRING_LENGTH);

   //
   // Write a binary format record to the datalog for Vista processing
   //
   struct
   {
      DataLog_UINT32 category;
      DataLog_UINT32 sequence;
      DataLog_UINT32 status;
      DataLog_UINT32 vista_response;
      DataLog_UINT32 scan_len;
      DataLog_UINT32 scan_symbology;
      char           scan_string[MAX_SCAN_LENGTH];
   } barcodeBinaryRecord;

   barcodeBinaryRecord.category       = in.scan_category;
   barcodeBinaryRecord.sequence       = in.scan_sequence;
   barcodeBinaryRecord.status         = in.status;
   barcodeBinaryRecord.vista_response = in.vista_response;
   barcodeBinaryRecord.scan_len       = in.scan_strlen;
   barcodeBinaryRecord.scan_symbology = in.scan_symbology_char[0];
   memcpy(barcodeBinaryRecord.scan_string, in.scan_string, MAX_SCAN_LENGTH);
   datalog_WriteBinaryRecord(log_handle_vista_binary, VISTA_RecordType, VISTA_BarcodeScanSubType, &barcodeBinaryRecord, sizeof(barcodeBinaryRecord));

   return true;
}



//
// Callback routine that handles the Donor Rejected Msg
// Note.  This message has no associated data.
//
bool VipStatusProcess::HandleDonorRejectedMsg (const int& in, donorRejectedMsg& out)
{
   DataLog(log_level_vip_info) << "Received Donor Rejected message" << endmsg;

   _xifCds.donorDownloaded.Set(false);
   _xifCds.donorCommitted.Set(false);

   // Save record of rejection in log file
   datalog_WriteBinaryRecord(log_handle_vista_binary, VISTA_RecordType, VISTA_DonorRejectedSubType, NULL, 0);

   return (_xifCds.vistaConnected.Get());
}



//
// Callback routine that handles the Procedure Param Adj Msg
//
bool VipStatusProcess::HandleProcedureParamAdjMsg (const int& in, procedureParameterAdjustmentMsg& out)
{
   const AdjustmentPayload           payload(in);
   const troubleshooting_adjustments adjustment = payload._enum;

   // Only forward operator initiated actions
   if (adjustment > END_OPERATOR_INITIATED_ADJUSTMENTS)
   {
      return false;
   }
   else
   {
      DataLog(log_level_vip_info) << "Adjustment Msg Sent with value " << (int)adjustment << endmsg;

      out.adjustment = (int)adjustment;

      // Save binary record of adjustment in data log
      datalog_WriteBinaryRecord(log_handle_vista_binary, VISTA_RecordType, VISTA_ParamAdjustSubType, &out.adjustment, sizeof(int));

      return true;
   }
}



//
// Callback routine that handles the Donor Vitals Update Msg
//
bool VipStatusProcess::HandleDonorVitalsUpdatedMsg (const SDonorVitals& in, donorVitalsUpdatedMsg& out)
{
   out.donorBloodType      = in.DonorBloodType;
   out.donorDownloadStatus = in.cDonorDownloadStatus;
   out.donorGender         = in.cDonorSex;
   out.donorInfoStatus     = in.cDonorInfoStatus;
   out.donorVitalsStatus   = in.cDonorVitalsStatus;
   out.hematocrit          = in.fDonorHct;
   out.precount            = in.fDonorPltPrecount;
   out.procDonorTBV        = in.fProcDonorTBV;
   out.safetyDonorTBV      = in.fSafetyDonorTBV;
   out.height              = in.fDonorHeight;
   out.weight              = in.fDonorWeight;

   if (in.cDonorHeightUnits == HEIGHT_UNITS_IN)
   {
      out.height *= CM_PER_IN;
   }

   if (in.cDonorWeightUnits == WEIGHT_UNITS_LB)
   {
      out.weight *= KG_PER_LB;
   }

   DataLog(log_level_vip_info) << "Donor Vitals Updated Msg Sent" << endmsg;

   // Write a binary format record to the data log for Vista processing
   struct
   {
      DataLog_UINT32 bloodType;
      DataLog_UINT32 downloadStatus;
      DataLog_UINT32 gender;
      DataLog_UINT32 infoStatus;
      DataLog_UINT32 vitalsStatus;
      float          hct;
      float          pltPrecount;
      float          procTBV;
      float          safetyTBV;
      float          donorHeight;
      float          donorWeight;
   } donorBinaryRecord;

   donorBinaryRecord.bloodType      = in.DonorBloodType;
   donorBinaryRecord.downloadStatus = in.cDonorDownloadStatus;
   donorBinaryRecord.gender         = in.cDonorSex;
   donorBinaryRecord.infoStatus     = in.cDonorInfoStatus;
   donorBinaryRecord.vitalsStatus   = in.cDonorVitalsStatus;
   donorBinaryRecord.hct            = in.fDonorHct;
   donorBinaryRecord.pltPrecount    = in.fDonorPltPrecount;
   donorBinaryRecord.procTBV        = in.fProcDonorTBV;
   donorBinaryRecord.safetyTBV      = in.fSafetyDonorTBV;
   donorBinaryRecord.donorHeight    = in.fDonorHeight * ((in.cDonorHeightUnits == HEIGHT_UNITS_IN) ? CM_PER_IN : 1.0);
   donorBinaryRecord.donorWeight    = in.fDonorWeight * ((in.cDonorWeightUnits == WEIGHT_UNITS_LB) ? KG_PER_LB : 1.0);

   datalog_WriteBinaryRecord(log_handle_vista_binary, VISTA_RecordType, VISTA_DonorVitalsSubType, &donorBinaryRecord, sizeof(donorBinaryRecord));

   return true;
}



//
// Callback routine that handles the End Of Run Summary Msg
//
bool VipStatusProcess::HandleEndRunSummaryMsg (const RUN_SUMMARY_STRUCT& in, endRunSummaryMsg& out)
{
   if (!in.run_complete)
   {
      DataLog(log_level_vip_info) << "Run Summary data not complete" << endmsg;
      return false;
   }

   DataLog(log_level_vip_info) << "Received End Run Summary message" << endmsg;

   out.totalAcUsed                   = in.rs1_total_AC_used;
   out.ActualAcToDonor               = in.rs1_actual_AC_to_donor;
   out.bloodVolumeProcessed          = in.rs1_total_blood_vol_processed;
   out.bloodVolumeProcessedPlatelet  = _procVolumeCds.VBPPlatelet;
   out.plateletVolumeActual          = in.rs2_platelet_volume;
   out.plateletYieldActual           = in.rs2_yield_of_platelets;
   out.acVolumeInPlatelets           = in.rs2_vol_AC_in_platelets;
   out.PASVolume                     = in.rs2_SS_plat_vol;
   out.offlinePASVolume              = in.platelet_salvage_volume;
   out.plasmaActualVolume            = in.rs2_plasma_volume;
   out.acVolumeInPlasma              = in.rs2_vol_AC_in_plasma;

   out.rbcIndividualProductReporting = in.drbcAutoSplit;
   out.startofAasTime                = in.rs1_start_of_aas_time;


   if (in.drbcAutoSplit)  // Double product
   {
      out.rbc1ActualVolume  = in.rs2_RBC_volume;
      out.rbc2ActualVolume  = in.rs2_RBC_volume_2;
      out.acVolumeInRBC1    = in.rs2_vol_AC_in_RBC;
      out.acVolumeInRBC2    = in.rs2_vol_AC_in_RBC_2;
      out.rbc1RasVolume     = in.rs2_SS_rbc_vol;
      out.rbc2RasVolume     = in.rs2_SS_rbc_vol_2;
      out.offlineRas1Volume = in.rbc_salvage_volume;
      out.offlineRas2Volume = in.rbc_salvage_volume_2;

      // Calculate the RBC 1 Collect Crit
      if (out.rbc1ActualVolume > 0)
         out.rbc1ActualHematocrit = _procVolumeCds.RBCProduct_A_FinalDose.Get() / _procVolumeCds.RBCProduct_A_FinalVolume.Get();
      else
         out.rbc1ActualHematocrit = 0;

      // Calculate the RBC 2 Collect Crit
      if (out.rbc2ActualVolume > 0)
         out.rbc2ActualHematocrit = _procVolumeCds.RBCProduct_B_FinalDose.Get() / _procVolumeCds.RBCProduct_B_FinalVolume.Get();
      else
         out.rbc2ActualHematocrit = 0;

      out.labelRBC1Leukoreduced = in.PQdata.label_rbcs_leukoreduced;
      out.labelRBC2Leukoreduced = in.PQdata.label_rbcs_leukoreduced_prod2;

      if (!out.labelRBC1Leukoreduced)
         PhilInReasonCodes (out.rbc1WbcReason, out.numberOfRbc1WbcReasons, VERIFY_RBC_WBCS,       "RBC WBC");

      if (!out.labelRBC2Leukoreduced)
         PhilInReasonCodes (out.rbc2WbcReason, out.numberOfRbc2WbcReasons, VERIFY_RBC_WBCS_PROD2, "RBC WBC Product 2");

      PhilInReasonCodes (out.rbc1VolumeReason,   out.numberOfRbc1VolumeReasons, VERIFY_RBC_VOL,           "RBC Volume");
      PhilInReasonCodes (out.rbc2VolumeReason,   out.numberOfRbc2VolumeReasons, VERIFY_RBC_VOL_PROD2,     "RBC Volume Product 2");

      PhilInReasonCodes (out.rbc1QualityReason, out.numberOfRbc1QualityReasons, VERIFY_RBC_QUALITY,       "RBC Quality");
      PhilInReasonCodes (out.rbc2QualityReason, out.numberOfRbc2QualityReasons, VERIFY_RBC_QUALITY_PROD2, "RBC Quality Product 2");
   }
   else  // Single product only
   {
      out.rbc1ActualVolume  = in.rs2_RBC_volume;
      out.rbc2ActualVolume  = 0;
      out.acVolumeInRBC1    = in.rs2_vol_AC_in_RBC;
      out.acVolumeInRBC2    = 0;
      out.rbc1RasVolume     = in.rs2_SS_rbc_vol;
      out.rbc2RasVolume     = 0;
      out.offlineRas1Volume = in.rbc_salvage_volume;
      out.offlineRas2Volume = 0;

      // Calculate the RBC Collect Crit for the End Run Summary Msg
      if (out.rbc1ActualVolume > 0)
         out.rbc1ActualHematocrit = _procVolumeCds.RBCDose.Get() / _procVolumeCds.RBCVolume.Get();
      else
         out.rbc1ActualHematocrit = 0;

      out.rbc2ActualHematocrit  = 0;

      out.labelRBC1Leukoreduced = in.PQdata.label_rbcs_leukoreduced;
      out.labelRBC2Leukoreduced = 0;

      if (!out.labelRBC1Leukoreduced)
         PhilInReasonCodes (out.rbc1WbcReason, out.numberOfRbc1WbcReasons, VERIFY_RBC_WBCS, "RBC WBC");
      out.numberOfRbc2WbcReasons = 0;

      PhilInReasonCodes (out.rbc1QualityReason, out.numberOfRbc1QualityReasons, VERIFY_RBC_QUALITY, "RBC Quality");
      out.numberOfRbc2QualityReasons = 0;

      PhilInReasonCodes (out.rbc1VolumeReason,  out.numberOfRbc1VolumeReasons,  VERIFY_RBC_VOL,     "RBC Volume");
      out.numberOfRbc2VolumeReasons = 0;
   }

   out.packedRbcResidualInSet = in.rs1_residual_loss_prbc;
   out.plasmaResidualInSet    = in.rs1_residual_loss_plasma;

   if (Software_CDS::GetInstance().getFeature(ComplementaryPlasmaAvailable))
   {
      out.complementaryPlasmaVolume = in.rs2_comp_pls_volm;
      out.complementaryPlasmaIcon   = in.rs2_comp_pls_icon;
   }
   else
   {
      out.complementaryPlasmaVolume = 0;
      out.complementaryPlasmaIcon   = false;
   }

   out.postHematocrit         = in.rs1_post_hct;

   out.postPlateletCount      = in.rs1_post_platelet_count;
   out.replacementFluidVolume = in.replacement_fluid_volume;
   out.endOfRunTime           = in.rs1_end_of_run_time;


   out.lengthOfRunWithPauseRecover    = (int)_procTimeCds.SysRunTime();
   out.lengthOfRunWithoutPauseRecover = (int)_procTimeCds.ProcRunTime();

   // Copy across the Count flag stuff.
   out.labelPlateletsLeukoreduced = in.PQdata.label_platelets_leukoreduced;
   out.labelPlasmaLeukoreduced    = in.PQdata.label_plasma_leukoreduced;

   PhilInReasonCodes (out.plateletConcentrationReason, out.numberOfPlateletConcentrationReasons, CONC_OUT_OF_RANGE,     "Platelet Concentration");
   PhilInReasonCodes (out.plateletVolumeReason,        out.numberOfPlateletVolumeReasons,        VERIFY_PLATELET_YIELD, "Platelet Yield");
   PhilInReasonCodes (out.plasmaVolumeReason,          out.numberOfPlasmaVolumeReasons,          VERIFY_PLASMA_VOL,     "Plasma Volume");

   if (!out.labelPlateletsLeukoreduced)
   {
      PhilInReasonCodes (out.plateletWbcReason, out.numberOfPlateletWbcReasons, VERIFY_PLATELET_WBCS, "Platelet WBC");
   }

   if (!out.labelPlasmaLeukoreduced)
   {
      PhilInReasonCodes (out.plasmaWbcReason, out.numberOfPlasmaWbcReasons, VERIFY_PLASMA_WBCS, "Plasma WBC");
   }

   // Set the rinseback complete status
   const int rinsebackStatus = (_procRunCds.RinsebackComplete.Get() == RINSEBACK_UNSUCCESSFUL) ? 0 : 1;
   out.rinsebackCompleteFlag = rinsebackStatus;
   out.plasmaRinsebackStatus = in.plasma_rinseback_status;
   out.salineRinsebackStatus = in.saline_rinseback_status;

   out.adjustedInletFlowRate = in.rs1_last_adjust_flow_rate;


   // Unregister for the end run summary message so we don't get more than one.
   _endRunSummary->deregisterMsg();

   // Maximum possible number of EndRun Product Quality Info items
   static const size_t MaxPQIArraySize = (int)NO_FLAG * (REASON_TO_SHOW_END_OF_LIST - REASON_TO_SHOW_FIRST_ITEM + 1);

   // Save binary record of run summary information not otherwise readily available from the data log
   struct BinaryRunSummaryRecord
   {
      DataLog_UINT32 acToDonor;
      float          postDonorHCT;
      DataLog_UINT32 postDonorPltCount;

      DataLog_UINT32 startofAASTime;
      DataLog_UINT32 endOfRunTime;
      DataLog_UINT32 lengthOfRun;
      DataLog_UINT32 replacementFluidVolume;
      DataLog_UINT32 rinsebackComplete;
      DataLog_UINT32 rbcIndividualReporting;

      DataLog_UINT32 PasOfflineVolume;
      DataLog_UINT32 Ras1OfflineVolume;
      DataLog_UINT32 Ras2OfflineVolume;

      float          RBC1ActualHct;
      float          RBC2ActualHct;

      DataLog_UINT32 RBCResidualInSet;
      DataLog_UINT32 PlasmaResidualInSet;

      DataLog_UINT32 labelPlatelet;
      DataLog_UINT32 labelPlasma;
      DataLog_UINT32 labelRBC1;
      DataLog_UINT32 labelRBC2;

      DataLog_UINT32 complementaryPlasmaVolume;
      DataLog_UINT32 complementaryPlasmaIcon;

      DataLog_UINT32 pqiData[MaxPQIArraySize];
   };

   BinaryRunSummaryRecord* binaryRunSummaryRecord = new BinaryRunSummaryRecord();

   binaryRunSummaryRecord->acToDonor              = in.rs1_actual_AC_to_donor;
   binaryRunSummaryRecord->postDonorHCT           = in.rs1_post_hct;
   binaryRunSummaryRecord->postDonorPltCount      = in.rs1_post_platelet_count;
   binaryRunSummaryRecord->startofAASTime         = mktime((tm*)&in.rs1_start_of_aas_time);
   binaryRunSummaryRecord->endOfRunTime           = mktime((tm*)&in.rs1_end_of_run_time);
   binaryRunSummaryRecord->lengthOfRun            = in.rs1_length_of_run;
   binaryRunSummaryRecord->replacementFluidVolume = in.replacement_fluid_volume;
   binaryRunSummaryRecord->rinsebackComplete      = rinsebackStatus;
   binaryRunSummaryRecord->rbcIndividualReporting = out.rbcIndividualProductReporting;
   binaryRunSummaryRecord->PasOfflineVolume       = in.platelet_salvage_volume;
   binaryRunSummaryRecord->Ras1OfflineVolume      = in.rbc_salvage_volume;
   binaryRunSummaryRecord->Ras2OfflineVolume      = in.rbc_salvage_volume_2;
   binaryRunSummaryRecord->RBC1ActualHct          = out.rbc1ActualHematocrit;
   binaryRunSummaryRecord->RBC2ActualHct          = out.rbc2ActualHematocrit;
   binaryRunSummaryRecord->RBCResidualInSet       = in.rs1_residual_loss_prbc;
   binaryRunSummaryRecord->PlasmaResidualInSet    = in.rs1_residual_loss_plasma;
   binaryRunSummaryRecord->labelPlatelet          = in.PQdata.label_platelets_leukoreduced;
   binaryRunSummaryRecord->labelPlasma            = in.PQdata.label_plasma_leukoreduced;
   binaryRunSummaryRecord->labelRBC1              = in.PQdata.label_rbcs_leukoreduced;
   binaryRunSummaryRecord->labelRBC2              = in.PQdata.label_rbcs_leukoreduced_prod2;

   if (Software_CDS::GetInstance().getFeature(ComplementaryPlasmaAvailable))
   {
      binaryRunSummaryRecord->complementaryPlasmaVolume = in.rs2_comp_pls_volm;
      binaryRunSummaryRecord->complementaryPlasmaIcon   = in.rs2_comp_pls_icon;
   }
   else
   {
      binaryRunSummaryRecord->complementaryPlasmaVolume = 0;
      binaryRunSummaryRecord->complementaryPlasmaIcon   = 0;
   }

   int pqiDataIdx = 0;
   pqiDataIdx += getPQIReasons(&binaryRunSummaryRecord->pqiData[pqiDataIdx], VERIFY_RBC_VOL);           // Fifth
   pqiDataIdx += getPQIReasons(&binaryRunSummaryRecord->pqiData[pqiDataIdx], VERIFY_PLASMA_WBCS);       // Forth
   pqiDataIdx += getPQIReasons(&binaryRunSummaryRecord->pqiData[pqiDataIdx], VERIFY_PLATELET_WBCS);     // First
   pqiDataIdx += getPQIReasons(&binaryRunSummaryRecord->pqiData[pqiDataIdx], CONC_OUT_OF_RANGE);        // Eleventh
   pqiDataIdx += getPQIReasons(&binaryRunSummaryRecord->pqiData[pqiDataIdx], VERIFY_PLASMA_VOL);        // Third
   pqiDataIdx += getPQIReasons(&binaryRunSummaryRecord->pqiData[pqiDataIdx], VERIFY_PLATELET_YIELD);    // Second
   pqiDataIdx += getPQIReasons(&binaryRunSummaryRecord->pqiData[pqiDataIdx], VERIFY_RBC_WBCS);          // Sixth
   pqiDataIdx += getPQIReasons(&binaryRunSummaryRecord->pqiData[pqiDataIdx], VERIFY_RBC_QUALITY);       // Seventh
   pqiDataIdx += getPQIReasons(&binaryRunSummaryRecord->pqiData[pqiDataIdx], VERIFY_RBC_VOL_PROD2);     // Eighth
   pqiDataIdx += getPQIReasons(&binaryRunSummaryRecord->pqiData[pqiDataIdx], VERIFY_RBC_WBCS_PROD2);    // Ninth
   pqiDataIdx += getPQIReasons(&binaryRunSummaryRecord->pqiData[pqiDataIdx], VERIFY_RBC_QUALITY_PROD2); // Tenth

   datalog_WriteBinaryRecord(log_handle_vista_binary, VISTA_RecordType, VISTA_RunSummarySubType, binaryRunSummaryRecord, sizeof(BinaryRunSummaryRecord) + (pqiDataIdx - 1) * sizeof(DataLog_UINT32));

   delete binaryRunSummaryRecord;

   return true;
}



//
// Callback routine that handles the Alarm Msg from Trima
//
bool VipStatusProcess::HandleAlarmMsg (const GUI_Alarm_struct& in, alarmMsg& out)
{
   //
   // Write a binary format record to the datalog for Vista processing.
   // NOTE: Conscious decision to log all alarms to dlog file.  Filtering logic should
   // be done by Vista.
   //
   struct
   {
      DataLog_UINT32 name;
      DataLog_UINT32 state;
   } alarmBinaryRecord;

   alarmBinaryRecord.name  = in.alarm_name;
   alarmBinaryRecord.state = in.alarm_state;
   datalog_WriteBinaryRecord(log_handle_vista_binary, VISTA_RecordType, VISTA_AlarmSubType, &alarmBinaryRecord, sizeof(alarmBinaryRecord));

   // If the alarm name and the alarm state are the same as the previous one, filter it out.

   // Filter out duplicate alarms
   if (in.alarm_name == _prevAlarmName)
   {
      if (in.alarm_state == _prevAlarmState)
         return false;    // Don't send duplicate alarms (same alarm, same state)
      else
      {
         // Only send when the state either transitions to or from clear.
         // Active to Latched and visa versa are not changes Vista cares about (IT 6315).
         if ( (_prevAlarmState == CLEARED) ||
              (in.alarm_state == CLEARED) )
         {
            // Save old state.
            _prevAlarmState = in.alarm_state;

            out.alarmId     = in.alarm_name;

            DataLog(log_level_vip_info) << "Alarm " << out.alarmId << " Sent to Vista." << endmsg;

            return true;
         }
         else
            return false;  // Filter out alarms that change state from latched to active and visa versa
      }
   }
   else
   {
      out.alarmId = in.alarm_name;

      // Save previous values
      _prevAlarmName  = in.alarm_name;
      _prevAlarmState = in.alarm_state;

      DataLog(log_level_vip_info) << "Alarm " << out.alarmId << " Sent to Vista." << endmsg;

      return true;
   }
}



//
// Callback routine that handles the GUI Alarm Response Msg from Trima
//
bool VipStatusProcess::HandleGuiAlarmResponseMsg (const Alarm_response_struct& in, alarmAckMsg& out)
{
   out.alarmId       = in.alarm_name;
   out.alarmResponse = in.alarm_response;

   //
   // Write a binary format record to the datalog for Vista processing
   // NOTE: Conscious decision to log all alarms to dlog file.  Filtering logic should
   // be done by Vista.
   //
   struct
   {
      DataLog_UINT32 name;
      DataLog_UINT32 response;
   } alarmResponseBinaryRecord;

   alarmResponseBinaryRecord.name     = in.alarm_name;
   alarmResponseBinaryRecord.response = in.alarm_response;
   datalog_WriteBinaryRecord(log_handle_vista_binary, VISTA_RecordType, VISTA_AlarmResponseSubType, &alarmResponseBinaryRecord, sizeof(alarmResponseBinaryRecord));


   if ( (in.alarm_name == _prevAlarmName) && (in.alarm_response == _prevAlarmResponse) )
   {
      DataLog(log_level_vip_info) << "Alarm " << in.alarm_name << ", " << in.alarm_response << " filtered out. " << endmsg;
      return false;
   }

   if ( (in.alarm_name == _prevAlarmName) && (in.alarm_response == CONTINUE) )
   {
      _prevAlarmName     = (ALARM_VALUES)-1;      // Clear out alarm.
      _prevAlarmState    = (ALARM_STATES)-1;      // Clear out alarm.
      _prevAlarmResponse = (ALARM_RESPONSES)-1;   // Clear out alarm.

      DataLog(log_level_vip_info) << "Alarm " << out.alarmId << " cleared. " << endmsg;
   }

   DataLog(log_level_vip_info) << "GUI Alarm Reponse Msg Sent. ID = " << out.alarmId
                               << ", Response = " << out.alarmResponse
                               << ", Previous = " << _prevAlarmName << endmsg;

   _prevAlarmResponse = in.alarm_response;

   return true;
}



//
// Callback routine that handles the PFR Status message from Trima
//
bool VipStatusProcess::HandlePfrStatus (const PFR_Status& in, powerFailRecoveryIndicationMsg& out)
{
   const bool inBloodRun = (_procTrimaSetCds.CassetteState.Get() > ProcTrimaSet_CDS::ACPRIMED);

   switch (in)
   {
      case NoRestore :
         out.powerFailPerformed = PFR_FAILED_OR_NO_PFR;
         break;

      case RestorePFRData :
         // IT 7599.  If we are restoring from PFR, it is either during blood run or AC PRIME.
         // Donor is not saved if PFR happens during AC Prime.  We must allow Vista to
         // send us a new one.
         out.powerFailPerformed = inBloodRun ? PFR_COMPLETE_ALL_RESTORED : PFR_COMPLETE_NOT_RESTORED;

         if (_xifCds.donorDownloaded.Get() && !inBloodRun)
         {
            _xifCds.donorDownloaded.Set(false);

            DataLog(log_level_vip_info) << "PFR during AC Prime.  Donor download data not restored" << endmsg;
         }
         break;

      default :
         _xifCds.pfrComplete.Set(false);
         DataLog(log_level_vip_error) << "Bad PFR Status " << in << endmsg;
         break;
   }

   _xifCds.pfrComplete.Set(true);

   // Save binary record of the power fail status for Vista
   datalog_WriteBinaryRecord(log_handle_vista_binary, VISTA_RecordType, VISTA_PFRStatusSubType, &out.powerFailPerformed, sizeof(int));

   struct interfaceStateMsg
   {
      DataLog_UINT32 privilegedState;
      DataLog_UINT32 donorDownloaded;
      DataLog_UINT32 pfrStatus;
      DataLog_UINT32 originalSequenceNumber;
      DataLog_UINT32 biometricConfig;
      DataLog_UINT32 predictVersion;
   };

   interfaceStateMsg interfaceState;
   interfaceState.privilegedState        = _xifCds.vistaInControl.Get();
   interfaceState.donorDownloaded        = _xifCds.donorDownloaded.Get();
   interfaceState.pfrStatus              = _xifCds.pfrComplete.Get();
   interfaceState.originalSequenceNumber = _xifCds.firstSequenceNumber.Get();
   interfaceState.biometricConfig        = TED_BIO_SIGNATURE_PIN;
   interfaceState.predictVersion         = TrimaInfo::predictVersion();

   // Save the snapshot of the interface status
   datalog_WriteBinaryRecord(log_handle_vista_binary, VISTA_RecordType, VISTA_InterfaceStateSubType, &interfaceState, sizeof(interfaceState));

   DataLog(log_level_vip_info) << "PFR Status Msg Sent with value " << out.powerFailPerformed << endmsg;

   return (_xifCds.vistaConnected.Get());
}



//
// Callback routine that handles the Operator button push message from GUI
//
bool VipStatusProcess::HandleButtonPushMsg (const BUTTON_PRESS_MESSAGE& in, operatorButtonPushMsg& out)
{
   out.screenId = in.SCREEN_ID;
   out.buttonId = in.BUTTON_ID;

   DataLog(log_level_vip_info) << "Button Push Msg Sent with value " << out.screenId << ", "
                               << out.buttonId << endmsg;

   return true;
}



//
// Callback routine that handles the PinSignatureRequestMsg message from GUI
//
bool VipStatusProcess::HandlePinSignatureRequestMsg (const PinSignatureRequestMsg& in, pinAuthorizationEventMsg& out)
{
   DataLog(log_level_vip_info) << "Received Pin Signature Request (RN " << in.scanSequenceNumber
                               << ", Dev " << in.biometricDeviceId << ", " << in.badge << ")[" << strlen(in.badge) << "]." << endmsg;

   out.biometricDeviceId  = in.biometricDeviceId;
   out.scanSequenceNumber = in.scanSequenceNumber;
   strncpy(out.badge, in.badge, VIP_BIO_MAX_BADGE_LENGTH);
   strncpy(out.pin, in.pin, VIP_BIO_MAX_PIN_LENGTH);

   DataLog(log_level_vip_info) << "Pin Signature Request Msg Sent" << endmsg;


   return true;
}



//
// Helper routine that fills in the reason code list for each reason.
//
void VipStatusProcess::PhilInReasonCodes (int* ptr, int& arraySize, ENUMERATED_FLAG flag, const char* msg)
{
   arraySize = 0;

   if (_pqInfo.NumberOfReasons(flag) == 0)
   {
      return;
   }

   // Assume that each pass of this loop guarantees a hit.
   // Assume an unordered list (no loop optimization).
   for (int j = REASON_TO_SHOW_FIRST_ITEM; j < REASON_TO_SHOW_END_OF_LIST; ++j)
   {
      if (_pqInfo.ShouldShow(flag, (ENUMERATED_REASON_TO_SHOW)j))
      {
         // This case would happen if someone adds reasons and doesn't change the
         // Vista interface AND we get a full list of reasons.
         if (arraySize >= VIP_MAX_REASONS)
         {
            DataLog(log_level_vip_error) << "MAX Reasons exceeded for " << msg << endmsg;
            break;
         }
         else
         {
            ptr[arraySize++] = j;
         }
      }
   }

   if (arraySize > 0)
   {
      DataLog(log_level_vip_info) << "Found " << arraySize << " reasons for " << msg << ".  ";

      for (int i = 0; i < arraySize; i++)
      {
         DataLog(log_level_vip_info) << ptr[i] << ", ";
      }

      DataLog(log_level_vip_info) << " END" << endmsg;
   }
}



//
// Helper routine that starts the status messages to Vista
//
void VipStatusProcess::StartStatusMsgs ()
{
   static bool firstTime = true;

   if (!firstTime)
   {
      DataLog(log_level_vip_info) << "Status Message Handlers already started" << endmsg;
      return;
   }

   firstTime = false;

   DataLog(log_level_vip_info) << "Registering Status Message Handlers" << endmsg;

   // Create message handlers for status messages.
   _endRunSummary       = new VipOneToOneStatusMsg<VipStatusProcess, RUN_SUMMARY_STRUCT,     endRunSummaryMsg>         (_trimaStatusAddr, VIP_END_RUN_SUMMARY_MSG,               this, &VipStatusProcess::HandleEndRunSummaryMsg);
   _barcodeContents     = new VipOneToOneStatusMsg<VipStatusProcess, SCAN_STRUCTURE,         barcodeContentsMsg>       (_trimaStatusAddr, VIP_BARCODE_CONTENTS_MSG,              this, &VipStatusProcess::HandleBarcodeMsg);
   _alarm               = new VipOneToOneStatusMsg<VipStatusProcess, GUI_Alarm_struct,       alarmMsg>                 (_trimaStatusAddr, VIP_ALARM_MSG,                         this, &VipStatusProcess::HandleAlarmMsg);
   _alarmAck            = new VipOneToOneStatusMsg<VipStatusProcess, Alarm_response_struct,  alarmAckMsg>              (_trimaStatusAddr, VIP_ALARM_ACK_MSG,                     this, &VipStatusProcess::HandleGuiAlarmResponseMsg);
   _pinSignatureRequest = new VipOneToOneStatusMsg<VipStatusProcess, PinSignatureRequestMsg, pinAuthorizationEventMsg> (_trimaStatusAddr, VIP_BIOMETRIC_AUTHORIZATION_EVENT_MSG, this, &VipStatusProcess::HandlePinSignatureRequestMsg);
   _operatorButtonPush  = new VipOneToOneStatusMsg<VipStatusProcess, BUTTON_PRESS_MESSAGE,   operatorButtonPushMsg>    (_trimaStatusAddr, VIP_OPERATOR_BUTTON_PUSH_MSG,          this, &VipStatusProcess::HandleButtonPushMsg);

   _donorVitalsUpdated  = new VipOneToOneWrappedStatusMsg<VipStatusProcess, DVitalsFromProc, SDonorVitals, donorVitalsUpdatedMsg>(_trimaStatusAddr, VIP_DONOR_VITALS_UPDATED_MSG, this, &VipStatusProcess::HandleDonorVitalsUpdatedMsg);

   _procedureParamAdj   = new VipOneToOneIntStatusMsg<VipStatusProcess, ProcedureAdjustmentMsg, procedureParameterAdjustmentMsg>(_trimaStatusAddr, VIP_PROCEDURE_PARAMETER_ADJ_MSG, this, &VipStatusProcess::HandleProcedureParamAdjMsg);

   _donorRejected.setAddr(_trimaStatusAddr);
   _pfrIndication.setAddr(_trimaStatusAddr);
}



//
// Helper routine that stops the status messages to Vista
//
void VipStatusProcess::StopStatusMsgs ()
{
   // Delete (stop) message handlers for status messages.
   delete _endRunSummary;
   _endRunSummary = 0;

   delete _barcodeContents;
   _barcodeContents = 0;

   delete _alarm;
   _alarm = 0;

   delete _alarmAck;
   _alarmAck = 0;

   delete _donorVitalsUpdated;
   _donorVitalsUpdated = 0;

   delete _procedureParamAdj;
   _procedureParamAdj = 0;

   delete _operatorButtonPush;
   _operatorButtonPush = 0;

   delete _pinSignatureRequest;
   _pinSignatureRequest = 0;
}



//
// Helper routine that copies reason codes for binary output
//
int VipStatusProcess::getPQIReasons (DataLog_UINT32* array, ENUMERATED_FLAG flag)
{
   PQInformation pqi;
   int           reason;
   int           idx = 1;

   array[0] = 0;
   for (reason = REASON_TO_SHOW_FIRST_ITEM; reason < REASON_TO_SHOW_END_OF_LIST; ++reason)
   {
      if ( pqi.ShouldShow(flag, (ENUMERATED_REASON_TO_SHOW)reason) )
      {
         array[idx++] = reason;
         array[0]    += 1;
      }
   }

   return idx;
}

/* FORMAT HASH a68113ec2ce43cdbb435fcd215a57b04 */

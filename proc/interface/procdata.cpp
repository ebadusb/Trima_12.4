/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      procdata.cpp
 *
 */


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vxWorks.h>
#include <taskLib.h>

#include "procdata.h"
#include "adjustedhct.h"
#include "bloodtype.h"
#include "cobeconfig.h"
#include "cycleacc.h"
#include "datalog.h"
#include "datalog_periodic.h"
#include "error.h"
#include "volumeacc.h"
#include "periodic_log.h"
#include "sounds.hpp"
#include "posthct.hpp"
#include "postcount.hpp"
#include "display_formatter.h"
#include "MssCommon.h"
#include "CatalogChecker.h"
#include "ptfResidual.h"
#include "residualAlgorithm.h"
#include "donor.h"
#include "alarmcontainer.h"
#include "software_cds.h"


ProcData::ProcDataStruct* ProcData::_TheProcData = 0;
int ProcData::_TheOwningTaskId                   = 0;




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define RETURN_CDS(__cds__) if ( _TheProcData->_##__cds__##DataWriter == this ) return _TheProcData->_RW##__cds__##Data; else return _TheProcData->_RO##__cds__##Data
#define MAKEWRITABLE(__cds__) if ( _TheProcData->_##__cds__##DataWriter == 0 ) { _TheProcData->_##__cds__##DataWriter = this; return true; } else return false
#define MAKEREADONLY(__cds__) if ( _TheProcData->_##__cds__##DataWriter == this ) _TheProcData->_##__cds__##DataWriter = 0

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ProcData::ProcDataStruct::ProcDataStruct()
   : _ConfigurationData        (ROLE_RO),
     _StatusData               (ROLE_RO),
     _PredictData              (ROLE_RO),
     _ROAdjustmentsData        (ROLE_RO),
     _RWAdjustmentsData        (ROLE_RW),
     _AdjustmentsDataWriter    (0),
     _ROCycleData              (ROLE_RO),
     _RWCycleData              (ROLE_RW),
     _CycleDataWriter          (0),
     _RODonorData              (ROLE_RO),
     _RWDonorData              (ROLE_RW),
     _DonorDataWriter          (0),
     _ROPQIData                (ROLE_RO),
     _RWPQIData                (ROLE_RW),
     _PQIDataWriter            (0),
     _PlsRinsebackData         (ROLE_RW),
     _RORunData                (ROLE_RO),
     _RWRunData                (ROLE_RW),
     _RunTargetsData           (ROLE_RO),
     _RunDataWriter            (0),
     _TimeData                 (ROLE_RW),
     _ROTrimaSetData           (ROLE_RO),
     _RWTrimaSetData           (ROLE_RW),
     _TrimaSetDataWriter       (0),
     _ROVolumesData            (ROLE_RO),
     _RWVolumesData            (ROLE_RW),
     _VolumesDataWriter        (0),
     _xifData                  (ROLE_RW),
     _Orders                   (),
     _AbsTime                  (),
     _ProcTime                 (),
     _SysRunTime               (),
     _ProcRunTime              (),
     _MSSRunTime               (),
     _AlarmQueue               (),
     _PFRHandler               (),
     _AdjustControl            (),
     _PredictResponseMsg       (Callback<ProcData::ProcDataStruct>(this, &ProcData::ProcDataStruct::predictRespondedCb)),
     _SafetyStatus             (Callback<ProcData::ProcDataStruct>(this, &ProcData::ProcDataStruct::safetyStatusCb)),
     _AlarmActive              (false),
     _DisplayedAlarm           (NULL_ALARM),
     _ButtonPressCounter       (0),
     _SystemState              (FIRST_STATE),
     _SubstateName             (),
     _VistaSubstateName        (),
     _RecoveryName             (RecoveryTypes::RecoveryStr(RecoveryTypes::None) ),
     _CurrentCycleTime         (),
     _LastReturnCycleTime      (0.0f),
     _SkipCycleForRatioCalc    (true),
     _RBCVolumeDeltaForCycle   (0.0f),
     _PredictBlocked           (false),
     _FirstCycleSoundGiven     (false),
     _NewAKO                   (false),
     _PlasmaRinsebackActive    (false),
     _SalineRinsebackActive    (false),
     _logMSSPumps              (false),
     _ButtonMsg                (Callback<ProcData::ProcDataStruct>(this, &ProcData::ProcDataStruct::buttonPressCb)),
     _RequestCancelMssPfr      (Callback<ProcData::ProcDataStruct>(this, &ProcData::ProcDataStruct::cancelMssRequestedPfrCb)),
     _RequestGenericWbcAlarmMsg(Callback<ProcData::ProcDataStruct>(this, &ProcData::ProcDataStruct::genericWbcAlarmRequested)),
     _SendStatsMsg             (Callback<ProcData::ProcDataStruct>(this, &ProcData::ProcDataStruct::sendRunSummary)),
     _cancelMssMsg             (Callback<ProcData::ProcDataStruct>(this, &ProcData::ProcDataStruct::setMssUserCancelFlag)),
     _StatsMsg                 (MessageBase::SEND_LOCAL),
     _SilentPause              (RESERVOIR_CHECK_SILENT_ALARM),
     _PauseTimer(0, Callback<ProcData::ProcDataStruct>(this, &ProcData::ProcDataStruct::clearTimer), TimerMessage::DISARMED),
     _FCVChecks(0),
     _FluidStillOnULS(false),
     _DoorLockTestStarted(false),
     _runSummWritten(),
     _runSummData()
{}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::ProcDataStruct::predictRespondedCb ()
{
   DataLog(log_level_proc_info) << "New Run Targets: Time-> " << _RunTargetsData.ProcTimeTarget.Get()
                                << " Procedure Number-> " << _RunTargetsData.ProcNumber.Get()
                                << " Platelet Yield-> " << _RunTargetsData.PlateletYieldTarget.Get()
                                << " Platelet Volume-> " << _RunTargetsData.PlateletVolumeTarget.Get()
                                << " Platelet Concentration-> " << _RunTargetsData.PlateletConcenTarget.Get()
                                << " Plasma-> " << _RunTargetsData.PlasmaTarget.Get()
                                << " RBC-> " << _RunTargetsData.RbcVolumeTarget.Get()
                                << " RBC Dose-> " << _RunTargetsData.RbcDoseTarget.Get()
                                << " RBC Vsvn-> " << _RunTargetsData.RbcVsvnTarget.Get()
                                << " RBC Crit-> " << _RunTargetsData.RbcCritTarget.Get()
                                << " Infusion Rate-> " << _RunTargetsData.InfusionRateTarget.Get()
                                << " Sel Platelet Yld-> " << _RunTargetsData.SelectedPlateletYield.Get()
                                << " Sel Platelet Vol-> " << _RunTargetsData.SelectedPlateletVolume.Get()
                                << " Sel Platelet Conc-> " << _RunTargetsData.SelectedPlateletConcen.Get()
                                << " Sel Plasma Vol-> " << _RunTargetsData.SelectedPlasmaVolume.Get()
                                << " Sel RBC Vol-> " << _RunTargetsData.SelectedRbcVolume.Get()
                                << " Replacement Fluid-> " << _RunTargetsData.RequiresRepFluid.Get()
                                << endmsg;
   //
   // Set the predict process status to the unblocked state ...
   _PredictBlocked = false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::ProcDataStruct::cancelMssRequestedPfrCb ()
{
   _RWRunData.cancelMSS.Set(true);

   DataLog (log_level_proc_info) << "PFR MSS Cancelation recieved." << endmsg;
}

void ProcData::ProcDataStruct::genericWbcAlarmRequested ()
{
   if (_RWRunData.wbcAlarmPending.Get())
   {
      requestWbcAlarm(__FILE__, __LINE__, "WBC event alarm is needed");
   }
}

void ProcData::ProcDataStruct::requestWbcAlarm (const char* file, const int lineNum, const char* logMessage)
{
   DataLog (log_level_proc_info) << "WBC Alarm Requested by " << file << "(" << lineNum << "):"
                                 << " key_show_pre_aas_flags: " << _ConfigurationData.Procedure.Get().key_show_pre_aas_flags
                                 << " PlateletVolumeTarget: " << _RunTargetsData.PlateletVolumeTarget.Get()
                                 << " ShowedPltWbcContamAlarm: " << _RWRunData.ShowedPltWbcContamAlarm.Get()
                                 << endmsg;

   if (_ConfigurationData.Procedure.Get().key_show_pre_aas_flags &&
       _RunTargetsData.PlateletVolumeTarget.Get() > 0.0 &&
       !_RWRunData.ShowedPltWbcContamAlarm.Get())
   {
      DataLog (log_level_proc_info) << "PLT contamination alert from " << file << "(" << lineNum << "): " << logMessage << endmsg;

      anAlarmMsg genericWbcAlarm(GENERIC_PLT_WBC_CONTAMINATION_ALERT);
      genericWbcAlarm.setAlarm();
   }
}

void ProcData::ProcDataStruct::requestObtrusiveWbcAlarm (const char* file, const int lineNum, const char* logMessage)
{
   DataLog (log_level_proc_info) << "WBC Alarm (Obtrusive) Requested by " << file << "(" << lineNum << "):"
                                 << " ShowedPltWbcContamAlarm: " << _RWRunData.ShowedPltWbcContamAlarm.Get()
                                 << endmsg;

   if (!_RWRunData.ShowedPltWbcContamAlarm.Get())
   {
      DataLog (log_level_proc_info) << "PLT contamination alert from " << file << "(" << lineNum << "): " << logMessage << endmsg;

      anAlarmMsg genericWbcAlarm(GENERIC_PLT_WBC_CONTAMINATION_OBTRUSIVE_ALERT);
      genericWbcAlarm.setAlarm();
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::ProcDataStruct::buttonPressCb ()
{
   _ButtonPressCounter++;
   PeriodicLog::forceOutput();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::ProcDataStruct::sendRunSummary ()
{
   DepotStateEnum depotState;

   // Goal is to only calculate run summary data when necessary. So translate
   // the current system state to one of the depot states that matter.

   switch (_SystemState)
   {
      case BLOOD_RINSEBACK :
         depotState = DEPOT_RINSEBACK;
         break;

      case METERED_STORAGE :
         depotState = DEPOT_SOLUTION_ADDITION;
         break;

      case METERED_STORAGE_DISCONNECT :
         depotState = DEPOT_ADDITION_DONE;
         break;

      case POST_RUN :
         depotState = DEPOT_POSTRUN;
         break;

      default :
         DataLog(log_level_proc_error) << "Unintended state during Run Summary request: " << SubstateMap::ssStr(_SystemState) << endmsg;
         depotState = DEPOT_RINSEBACK; // not sure what the best choice is here.
         break;
   }

   // Update the run summary data on first pass of any of the depot states
   if (!_runSummWritten[depotState])
   {
      DataLog(log_level_proc_info) << "Saving Run Summary data during " << SubstateMap::ssStr(_SystemState) << endmsg;
      getRunSummaryData(_runSummData);
      _runSummWritten[depotState] = true;
   }
   else
   {
      DataLog(log_level_proc_info) << "Re-using existing Run Summary data in state " << SubstateMap::ssStr(_SystemState) << endmsg;
   }

   _StatsMsg.send(_runSummData);
}


void ProcData::ProcDataStruct::setMssUserCancelFlag ()
{
   bool isUserCancellation = _cancelMssMsg.getData();

   DataLog (log_level_proc_info) << "setMssUserCancelFlag(): isUserCancellation=" << isUserCancellation << endmsg;

   if (isUserCancellation)
   {
      ProcData pd;

      DataLog (log_level_proc_info) << "Set Mss User Abort Type ALL(" << MSS_USER_ABORT_TYPE_ALL << ")" << endmsg;

      pd.MakeRunWritable();
      pd.Run().MssUserAbortType.Set(MSS_USER_ABORT_TYPE_ALL);
      pd.MakeRunReadable();
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// gathers all the data of the run for the endrun Summary GUIs
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::ProcDataStruct::getRunSummaryData (RUN_SUMMARY_STRUCT& data)
{
   ////////////////////////////////////////////////////////////////////////
   //
   // Some initial facts ...
   //
   const unsigned int func_bits         = _RORunData.CassetteFunctionBits.Get();

   const int          procNumber        = _RunTargetsData.ProcNumber.Get();

   const bool         Filter1Plugged    = _RORunData.PtfStatus.Get() & PTF_STATUS_P1_PLUGGED;
   const bool         Filter2Plugged    = _RORunData.PtfStatus.Get() & PTF_STATUS_P2_PLUGGED;
   const bool         setIsPtf          = CatalogChecker::checkBit(func_bits, CatalogChecker::MSS_RBC);
   const bool         setIsDrbc         = CatalogChecker::checkBit(func_bits, CatalogChecker::DRBC_BAG);
   const bool         promptDrbcSplit   = _ConfigurationData.isPromptedDrbcSplit(procNumber);

   const float        DonorTBV          = _RODonorData.TBV.Get();

   const float        VacPlateletBag    = _ROVolumesData.VacPlateletBag.Get();
   const float        VacPlasmaBag      = _ROVolumesData.VacPlasmaBag.Get();
   const float        VacRBCBag         = _ROVolumesData.VacRBCBag.Get();
   const float        VacTotal          = _ROVolumesData.VacTotal.Get();
   const float        VacTotal_tmp      = _ROVolumesData.VacTotal_tmp.Get();

   const float        PlateletYield     = _ROVolumesData.PlateletYield.Get();
   const float        PlateletVolume    = _ROVolumesData.PlateletVolume.Get();

   const float        PlasmaVolume      = _ROVolumesData.PlasmaVolume.Get();

   const float        RBCVolume         = _ROVolumesData.RBCVolume.Get();
   const float        RBCDose           = _ROVolumesData.RBCDose.Get();

   const float        VstoragePltBag    = _ROVolumesData.VstoragePltBag.Get();
   const float        VReplacement      = _ROVolumesData.VReplacement.Get();

   const float        VSalineBolus      = _ROVolumesData.VSalineBolus.Get();

   static bool        alreadyLoggedOnce = false;

   data.run_complete  = (_SystemState == POST_RUN);
   data.drbcAutoSplit = setIsPtf && promptDrbcSplit;


   ////////////////////////////////////////////////////////////////////////
   //
   // Get the end of run time ...
   //
   time_t t = _RORunData.ProcEndRunTime.Get();
   if (t == 0)
      time(&t);

   struct tm* endtime = localtime(&t);
   data.rs1_end_of_run_time = *endtime;

   ////////////////////////////////////////////////////////////////////////
   //
   // Get the start of AAS time ...
   //
   t = _RORunData.ProcStartAasTime.Get();
   if (t == 0)
      time(&t);

   endtime                    = localtime(&t);
   data.rs1_start_of_aas_time = *endtime;


   ////////////////////////////////////////////////////////////////////////
   //
   // Report the procedure run time....
   //
   data.rs1_length_of_run = (long)(_ProcRunTime.getMins());


   ////////////////////////////////////////////////////////////////////////
   //
   // Metering in effect...
   //
   data.meteringStarted = _RORunData.MeteringStarted.Get();


   ////////////////////////////////////////////////////////////////////////
   //
   // Report the max draw flow rate.  Per requirements this is the minimum of
   // the last cap value or the maximum draw flow rate seen during the procedure.
   // If we did not hit a monitoring state then MaxQin will be zero and the run
   // summary should report it as zero.
   //
   data.rs1_last_adjust_flow_rate = 0; // default
   if ( _ROAdjustmentsData.MaxQin > 0.0f)
   {
      data.rs1_last_adjust_flow_rate = (_ROAdjustmentsData.Draw.CurrentCap < _ROAdjustmentsData.MaxQin)
                                       ? (int)_ROAdjustmentsData.Draw.CurrentCap : (int)_ROAdjustmentsData.MaxQin;
      DataLog (log_level_proc_info) << "Run summary draw flow rate: " << data.rs1_last_adjust_flow_rate << " (min of "
                                    << _ROAdjustmentsData.Draw.CurrentCap << " , " << _ROAdjustmentsData.MaxQin << ")" << endmsg;
   }

   ////////////////////////////////////////////////////////////////////////
   //
   // Report the AC used...
   //
   static const float AC_BEFORE_MANIFOLD = 13.0f;
   if ((_RORunData.NumReturn.Get() == 0) && (!_RORunData.RinsebackStarted.Get()))
   {
      // IT 8146: There is no return or rinse back, therefore set values to zero.

      // Report the total amount of AC processed.
      data.rs1_total_AC_used      = (long)(VacTotal);
      // Calculate the amount of ACD given back to the donor...
      data.rs1_actual_AC_to_donor = 0;
   }
   else if (_ROTrimaSetData.CassetteState.Get() == ProcTrimaSet_CDS::STORAGE)
   {
      // IT 8360 during RASmetering we need to get the VacTotal from a tmp storage
      // holder because we are playing with the AC pump, this messes up the runsummary screen
      // if you look at it during metering.

      // Report the total amount of AC processed.
      data.rs1_total_AC_used      = (long)(VacTotal_tmp);
      // Calculate the amount of ACD given back to the donor...
      data.rs1_actual_AC_to_donor = (long)(VacTotal_tmp -
                                           VacPlateletBag -
                                           VacPlasmaBag -
                                           VacRBCBag -
                                           AC_BEFORE_MANIFOLD);
   }
   else
   {
      // Report the total amount of AC processed.
      data.rs1_total_AC_used      = (long)(VacTotal);
      // Calculate the amount of ACD given back to the donor...
      data.rs1_actual_AC_to_donor = (long)(VacTotal -
                                           VacPlateletBag -
                                           VacPlasmaBag -
                                           VacRBCBag -
                                           AC_BEFORE_MANIFOLD);
   }

   if (data.rs1_actual_AC_to_donor < 0)
      data.rs1_actual_AC_to_donor = 0;


   ////////////////////////////////////////////////////////////////////////
   //
   // Calculate the Post Platelet count.
   //
   if (DonorTBV > 0.0f)
   {

      // Post count = precount - platelets collected/(TBV*1e6).
      Postcount postcount(_RODonorData.PreCnt.Get(),
                          DonorTBV,
                          PlateletYield,
                          _ConfigurationData.Procedure.Get().key_post_plat);
      data.rs1_post_platelet_count = (long)(postcount.postcount());
   }
   else
   {
      data.rs1_post_platelet_count = 0;
   }


   ////////////////////////////////////////////////////////////////////////
   //
   // Report yield calculated.
   //
   data.rs2_yield_of_platelets = DisplayFormatter::FormatPlateletYield(PlateletYield / 1.0E11f);



   ////////////////////////////////////////////////////////////////////////
   //
   // Report Product Hct change based on real collection.
   //
   float rbcBagHct = 0.0f;
   if (RBCVolume > 0.0f)
   {
      rbcBagHct = (RBCDose / RBCVolume);
   }


   ///////////////////////////////////////////////////////////////////////
   //
   // Calculate the Residuals based on cassette & run
   //
   ResidualAlgorithm myResiduals;

   bool              dumpLogging = false; // just log at the end of run.

   if (_StatusData.CassIsUp() && !alreadyLoggedOnce)
   {
      dumpLogging       = true;
      alreadyLoggedOnce = true;
   }

   myResiduals.initialize(dumpLogging);
   myResiduals.calculate(dumpLogging);
   // ResidualAlgorithm resid;  IT 8473
   //

   //////////////////////////////////////////////////////////////////////////////////////
   //
   // Residual Reporting
   //
   data.rs1_residual_loss_prbc   = (int)(myResiduals.getResidualRBCDose()   + 0.5f);
   data.rs1_residual_loss_plasma = (int)(myResiduals.getResidualPLSVolume() + 0.5f);


   ////////////////////////////////////////////////////////////////////////
   //
   // Report PostHct.
   //
   if (DonorTBV > 0.0f)
   {
      const float residualSetVolume = myResiduals.getCassetteOnlyResidualRBCDose(); // these dont include the filter loses

      PostHct     posthct;

      const float hct = 100.0f * posthct.postHct(_RODonorData.Hct.Get(),
                                                 DonorTBV,
                                                 RBCDose * _ConfigurationData.Predict.Get().key_rsf,  // undo the RSF dose  Note: includes PTF filter losses if any
                                                 residualSetVolume,  // cassette only no PTF filter losses
                                                 _RODonorData.SampleVolume.Get() );


      // IT 8233 add log of a more accurite Post Crit for QA.
      DataLog(log_level_proc_external) << "Actual Post-Crit sent to EndRun Summary(before rounding to Int) = " << hct << " %" << endmsg;


      CDonor myDonor;
      // IT 10264 - only round if the units are hematocrit
      if (myDonor.RbcUnitsHct())
      {
         // IT 2557 - post hematocrit needs to be rounded, not truncated
         data.rs1_post_hct = hct + 0.5;
      }
      else
      {
         data.rs1_post_hct = hct;
      }
   }
   else
   {
      data.rs1_post_hct = 0.0;
   }


   ////////////////////////////////////////////////////////////////////////
   //
   //  Total amount of Blood processed.....
   //
   // Report the Volume of blood processed .
   data.rs1_total_blood_vol_processed = _ROVolumesData.BloodVolumeProcessed();


   ////////////////////////////////////////////////////////////////////////
   //
   // Rinsebacks flavors  if active
   //
   data.plasma_rinseback_active = _PlasmaRinsebackActive;
   data.saline_rinseback_active = _SalineRinsebackActive;


   ////////////////////////////////////////////////////////////////////////
   //
   //  Platelet Reporting.
   //
   // Report the collect bag volume. now includes any plt SS too. IT7804
   data.rs2_platelet_volume = (long)(PlateletVolume + VstoragePltBag + 0.5);   // Round

   // Report the ac in the collect bag volume.
   data.rs2_vol_AC_in_platelets = (long)(VacPlateletBag);

   ////////////////////////////////////////////////////////////////////////
   //
   //  PAS Reporting.
   //
   // Report the metered volume
   data.rs2_SS_plat_vol = (long)(VstoragePltBag + 0.5);


   ////////////////////////////////////////////////////////////////////////
   //
   //  Plasma Reporting.
   //
   if (_PlasmaRinsebackActive)
   {
      // Report the plasma bag volume.
      data.rs2_plasma_volume = (long)(_PlsRinsebackData.VPlasmaTarget.Get() + 0.5);   // Round

      // Report the ac in the plasma bag volume.
      if (PlasmaVolume > 0)
      {
         const float ac_fraction = VacPlasmaBag / PlasmaVolume;
         data.rs2_vol_AC_in_plasma = (long)(data.rs2_plasma_volume * ac_fraction);
      }
      else
      {
         data.rs2_vol_AC_in_plasma = 0;
      }
   }
   else // Normal RB
   {
      // Report the plasma bag volume.
      data.rs2_plasma_volume = (long)(PlasmaVolume + 0.5);   // Round

      // Report the ac in the plasma bag volume.
      data.rs2_vol_AC_in_plasma = (long)(VacPlasmaBag);
   }


   ////////////////////////////////////////////////////////////////////////////////////////////
   //
   // reported RBC volume:
   //
   data.rs2_RBC_volume   = (long)(myResiduals.getReportedRBCVolume_Prod1() + 0.5);
   data.rs2_RBC_volume_2 = (long)(myResiduals.getReportedRBCVolume_Prod2() + 0.5);

   ////////////////////////////////////////////////////////////////////////////////////////////
   //
   // reported SS volume:
   //
   data.rs2_SS_rbc_vol   = (long)(myResiduals.getReportedSSVolume_Prod1());   // dont round these up see IT 8997 and it doesnt matter
   data.rs2_SS_rbc_vol_2 = (long)(myResiduals.getReportedSSVolume_Prod2());


   //////////////////////////////////////////////////////////////////////////////////////
   //
   //  Make sure we don't have any negative or nonsensical values.
   //
   if (data.rs2_RBC_volume < 0)
      data.rs2_RBC_volume = 0;

   if (data.rs2_RBC_volume_2 < 0)
      data.rs2_RBC_volume_2 = 0;

   if (data.rs2_SS_rbc_vol < 0)
      data.rs2_SS_rbc_vol = 0;

   if (data.rs2_SS_rbc_vol_2 < 0)
      data.rs2_SS_rbc_vol_2 = 0;


   //////////////////////////////////////////////////////////////////////////////////////
   //
   //  AC in RBC bag.
   //
   if (data.drbcAutoSplit)
   {
      // Report the AC in the rbc bag volume (product 1).
      data.rs2_vol_AC_in_RBC   = (long)(_ROVolumesData.RBCProduct_A_FinalVac.Get() + 0.5f); // and round it UP!
      // Report the AC in the rbc bag volume (product 2).
      data.rs2_vol_AC_in_RBC_2 = (long)(_ROVolumesData.RBCProduct_B_FinalVac.Get() + 0.5f); // and round it UP!
   }
   else
   {
      // Report the AC in the rbc bag
      data.rs2_vol_AC_in_RBC   = (long)(VacRBCBag + 0.5f); // and round it UP!
      // Second product doesn't exist.
      data.rs2_vol_AC_in_RBC_2 = 0;
   }

   ////////////////////////////////////////////
   //  dont report AC if no product collected... this could happen because AC in filter may be reported as in bag
   if (data.rs2_RBC_volume <= 0)
      data.rs2_vol_AC_in_RBC = 0;

   if (data.rs2_RBC_volume_2 <= 0)
      data.rs2_vol_AC_in_RBC_2 = 0;


   //////////////////////////////////////////////////////////////////////////////////////
   //
   //  Saline to the donor.
   //
   // Report the volume of replacement fluid used.

   float salineRinsebackVolume = _ROVolumesData.VSalineUsedinRinseback.Get() - CobeConfig::data().SrTotalResidualVolume;
   if (salineRinsebackVolume < 0.0f)
      salineRinsebackVolume = 0.0f;

   data.replacement_fluid_volume = (long)(VReplacement + VSalineBolus + salineRinsebackVolume + 0.5); // resolve with IT 8812

   DataLog(log_level_proc_info) << "Saline to Donor in Saline Rinseback -> " << salineRinsebackVolume << endmsg;


   /////////////////////////////////////////////////////////////////////
   //  OFFLINE ADVICE TO OPER
   bool mssCompleted = (_SystemState == METERED_STORAGE_DISCONNECT);
   //////////////////////////////////////////////////////////////////////////////////////
   //
   // Determine any OFFLINE PAS volume
   //
   data.platelet_salvage_volume = 0; // ml


   if ( readyForPLTMSS(true /* ignore A2 alarms*/)
        && (mssCompleted || data.run_complete ))
   {
      const int AdjustedPASVolume = (int)getAdjustedPAS();
      const int volmSSDelivered   = (int)(VstoragePltBag + 0.5f);

      // get the volume of SS not automatically added by Trima
      const int sVolmDiff = (AdjustedPASVolume - volmSSDelivered);

      // dont show small addition within the flagging volume limit
      if (sVolmDiff >= CobeConfig::data().Vplt1)
      {
         data.platelet_salvage_volume = sVolmDiff;
      }
   }


   //////////////////////////////////////////////////////////////////////////////////////
   //
   // Determine any OFFLINE RAS volume
   //
   data.rbc_salvage_volume   = 0; // myResiduals.getOfflineSSVolume_Prod1(); // ml
   data.rbc_salvage_volume_2 = 0; // myResiduals.getOfflineSSVolume_Prod2(); // ml

   if ( readyForRBCMSS(true /* ignore A2 alarms*/)
        && (mssCompleted || data.run_complete ) )
   {
      // IT 8306 - Always determine whether this is prompted split based on configured
      // rbc dose with no scaling or adjustments
      const bool targetedAsSplit = setIsDrbc && promptDrbcSplit;

      if (targetedAsSplit)
      {
         if (_RORunData.drbcSplitDone.Get())
         {
            data.rbc_salvage_volume   = (int)(myResiduals.getOfflineSSVolume_Prod1() + 0.5f);
            data.rbc_salvage_volume_2 = (int)(myResiduals.getOfflineSSVolume_Prod2() + 0.5f);
         }
         else // The split was never actually completed, product 2 wasn't collected.
         {
            // As ugly as this is, RAS volume accumulates in the opposite product if we
            // never actually got to the DRBC split.
            data.rbc_salvage_volume   = (int)(myResiduals.getOfflineSSVolume_Prod1() + 0.5f);
            data.rbc_salvage_volume_2 = 0;
         }
      }
      else
      {
         data.rbc_salvage_volume   = (int)(myResiduals.getOfflineSSVolume_Prod1() + 0.5f);
         data.rbc_salvage_volume_2 = 0;
      }

      // If we flagged for RBC quality, don't show offline addition stats.
      const bool prod1Flagged = Filter1Plugged ||
                                _RORunData.MssApsTooHighRBCProduct1.Get() > CobeConfig::data().PTFAPSAlertsPerProductMetering;
      const bool prod2Flagged = Filter2Plugged ||
                                _RORunData.MssApsTooHighRBCProduct2.Get() > CobeConfig::data().PTFAPSAlertsPerProductMetering;

      // Don't give a negative volume, and don't show salvage volume on a CPS-flagged product.
      if (data.rbc_salvage_volume   < 0 || prod1Flagged)
         data.rbc_salvage_volume = 0;

      if (data.rbc_salvage_volume_2 < 0 || prod2Flagged)
         data.rbc_salvage_volume_2 = 0;
   }

   ///////////////////////////////////////////
   //
   // SET RINSEBACK STATUS IT7973 & 8464
   //
   const RINSEBACK_OUTCOMES RinsebackStatus = _RORunData.RinsebackComplete.Get();
   switch (RinsebackStatus)
   {
      case RINSEBACK_UNSUCCESSFUL :
      case RINSEBACK_SUCCESSFUL_NORMAL :
      {
         data.plasma_rinseback_status = SPECIAL_RINSEBACK_STATUS_INCOMPLETE;
         data.saline_rinseback_status = SPECIAL_RINSEBACK_STATUS_INCOMPLETE;
      }
      break;

      case RINSEBACK_SUCCESSFUL_PLASMA :
      {
         data.plasma_rinseback_status = SPECIAL_RINSEBACK_STATUS_COMPLETE;
         data.saline_rinseback_status = SPECIAL_RINSEBACK_STATUS_INCOMPLETE;
      }
      break;

      case RINSEBACK_SUCCESSFUL_SALINE :
      {
         data.plasma_rinseback_status = SPECIAL_RINSEBACK_STATUS_INCOMPLETE;
         data.saline_rinseback_status = SPECIAL_RINSEBACK_STATUS_COMPLETE;
      }
      break;

      default :
      {
         DataLog(log_level_proc_error) << "Rinseback cases not covered for " << RinsebackStatus << endmsg;
         data.plasma_rinseback_status = SPECIAL_RINSEBACK_STATUS_INCOMPLETE;
         data.saline_rinseback_status = SPECIAL_RINSEBACK_STATUS_INCOMPLETE;
      }
      break;
   }

   /////////////////////////////////////////////////////////////////////////////////////]
   // report complementary plasma volume
   //
   //
   //


   data.rs2_comp_pls_volm = 0;
   data.rs2_comp_pls_icon = false;


   if (   Software_CDS::GetInstance().getFeature(ComplementaryPlasmaAvailable)
          && _ConfigurationData.Predict.Get().key_complementary_plasma
          && _ROTrimaSetData.Cassette != cassette::RBCPLS)
   {


      // IT 11667 & 11669
      if ( readyForPLTMSS(true /* ignore A2 alarms*/) )
      {



         int   comp_pls_volm  = 0; // the value to be determined.
         float ppco           = _RunTargetsData.PltPctCarryover / 100.0f;
         int   PLSvolume      = data.rs2_plasma_volume;
         float Plateletvolume = data.rs2_platelet_volume;
         int   PASvolume      = 0; // calculate or actual depends if PAS is done yet.

         if (_RORunData.PltMeteringDone.Get())
         {
            // done with PAS, use actual
            PASvolume = data.rs2_SS_plat_vol;
            DataLog(log_level_proc_info) << "compPls: PAS actual vol = " << PASvolume << endmsg;


         }
         else
         {
            // not done with PAS,  estimate it
            PASvolume = (int)(floor(Plateletvolume * (( 1.0f - ppco ) / ppco)));
            DataLog(log_level_proc_info) << "compPls: PAS estimated vol = " << PASvolume << endmsg;
         }


         // IT 11676
         if (data.platelet_salvage_volume > 0)
         {

            // use the calculated offline amt plus any actual
            PASvolume = data.platelet_salvage_volume + data.rs2_SS_plat_vol;
            DataLog(log_level_proc_info) << "compPls: PAS offline vol(Offline+Actual) = " << PASvolume
                                         << " ( " << data.platelet_salvage_volume
                                         << " + "  << data.rs2_SS_plat_vol << " )"
                                         << endmsg;
         }




         if (PLSvolume > 1)   // slop from plsma RB so make sure this is GT 1 not zero....
         {

            if (PASvolume >= PLSvolume )
            {
               comp_pls_volm = PLSvolume;
            }
            else // (PASvolume < PLSvolume)
            {
               comp_pls_volm = PASvolume;
            }

            if (comp_pls_volm < 0)
               comp_pls_volm = 0;

         }



         data.rs2_comp_pls_volm = comp_pls_volm;

         DataLog(log_level_proc_info) << "compPls: Complementary Plasma = " << comp_pls_volm
                                      << ", (PAS = " << PASvolume
                                      << ", Plasma =" << PLSvolume
                                      << ")" << endmsg;

         if ( (PLSvolume <= PASvolume) &&
              (PLSvolume > 1.0f)     // Don't use 0 as it is a trivial volume and may happen
              )
         {
            data.rs2_comp_pls_icon = true;
            DataLog (log_level_proc_info) << "compPls: Plasma is completely complementary" << endmsg;
         }
         else
            data.rs2_comp_pls_icon = false;

      }

   }


   //////////////////////////////////////////////////////////////////////////////////////
   //
   // Report Product Quality results
   //
   data.PQdata = _ROPQIData.PQdata.Get();


   //////////////////////////////////////////////////////////////////////////////////////
   //
   // Log the EORS results to the periodic data set only once
   //
   if (data.run_complete && dumpLogging)
   {
      //
      //   Log summary data to run_data file
      //
      //  Note:  These values are grabbed by outcome review.  Coordinate before making
      //  changes.
      //
      DataLog(log_level_proc_external).flags(ios::showpoint | ios::fixed);
      DataLog(log_level_proc_external).precision(3);
      DataLog(log_level_proc_external) << "Postcount: " << data.rs1_post_platelet_count
                                       << " PostHct: " << data.rs1_post_hct
                                       << endmsg;

      DataLog(log_level_proc_external) << "AC summary stats: "
                                       << " total=" << data.rs1_total_AC_used
                                       << " to donor=" << data.rs1_actual_AC_to_donor
                                       << endmsg;

      logEORData();
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ProcData::ProcData()
   : AbsModalData(AbsModalData::NMDATA)
{
   if (_TheOwningTaskId != taskIdSelf()
       && _TheOwningTaskId != 0)
   {
      //
      // Notify the system of the error ...
      DataLog_Critical criticalLog;
      DataLog(criticalLog) << "ProcData object may already in use by taskId->" << taskIdSelf() << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "ProcData initialization failed");
      return;
   }

   if (!_TheProcData)
   {
      //
      // Create the task wide object
      _TheProcData     = new ProcDataStruct();
      _TheOwningTaskId = taskIdSelf();

      //
      // Initialize the TimeKeepers to the datastore elements ...
      _TheProcData->_AbsTime.init(_TheProcData->_TimeData.tmAbsolute);
      _TheProcData->_ProcTime.init(_TheProcData->_TimeData.tmProcTime);
      _TheProcData->_SysRunTime.init(_TheProcData->_TimeData.tmSysRun);
      _TheProcData->_ProcRunTime.init(_TheProcData->_TimeData.tmProcRun);
      _TheProcData->_MSSRunTime.init(_TheProcData->_TimeData.tmMSSRun);
   }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ProcData::~ProcData()
{
   MakeAdjustmentsReadable();
   MakeCycleReadable();
   MakeDonorReadable();
   MakePQIReadable();
   MakeRunReadable();
   MakeTrimaSetReadable();
   MakeVolumesReadable();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::ProcDataStruct::clearTimer ()
{

   ProcData pd;
   _PauseTimer.interval(0);   // disable timer
   _TheProcData->_SilentPause.clearAlarm();
   DataLog(log_level_proc_info) << "Clearing pause and silent ULS alarm and reset timer" << endmsg;

   if (pd.Status().ReservoirIsFull())
   {
      _TheProcData->_FluidStillOnULS = true;
      DataLog(log_level_proc_info) << "Fluid still detected at ULS after pause, call normal FCV alarm" << endmsg;
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::Update (const UpdateModeType mode)
{
   MakeRunWritable();
   Run().CycleSwitch.Set(false);
   MakeRunReadable();

   if (mode == ProcData::PRIME || mode == ProcData::BLOODRUN || mode == ProcData::RECOVERY)
   {
      if (Status().ReservoirIsFull() && !Status().ReservoirError())
      {

         //////////////////////////////////////////////////////////////////////////////////
         if (Run().NumReturn.Get() <= 0)
         {
            float fVol = Volumes().VinTotal.Get() - Volumes().Vreturn.Get();

            DataLog(log_level_proc_info) << "ULS reporting res full. FVOL = " << fVol << endmsg;
            if (fVol < FirstCycleTooShort())
            {
               if ((  _TheProcData->_SilentPause.getState() != ACTIVE )
                   && (  _TheProcData->_FCVChecks < CobeConfig::data().FirstCycleTooSoonRetryDoublecheck )
                   && ( !(_TheProcData->_FluidStillOnULS))
                   )
               {

                  DataLog(log_level_proc_info)
                     << "FCV pause and silent ULS alarm set. FVOL = "
                     << fVol
                     << ".  Count = "
                     << (_TheProcData->_FCVChecks + 1) // add one for the non-C bean counters (1..5)
                     << endmsg;


                  // wait a second
                  // silent alarm to pause pumps
                  _TheProcData->_FCVChecks++;
                  _TheProcData->_SilentPause.setAlarm();
                  _TheProcData->_PauseTimer.interval(CobeConfig::data().FirstCycleTooSoonPauseDoublecheck);    // callback to ProcData::ProcDataStruct::clearTimer()
               }
            }
         }
         ///////////////////////////////////////////////////////////////////////////////////


         if (_TheProcData->_SilentPause.getState() != ACTIVE /*we are not in the silent stop pump alarm*/)
         {

            SwitchtoReturn();
         }
      }
      else if (Status().ReservoirIsEmpty() && !Status().ReservoirError())
      {
         SwitchtoDraw();
      }
   }

   //
   // If we are in the first draw cycle or
   //    we just switched to the first return cycle,
   // then possible give the first cycle sound and
   //    log the volumes
   if (TrimaSet().CassetteState.Get() > ProcTrimaSet_CDS::BLOODPRIME
       && TrimaSet().CassetteState.Get() < ProcTrimaSet_CDS::RINSEBACK
       && (    Run().NumReturn.Get() <= 0
               || (    Run().DrawCycle.Get() == false
                       && Run().CycleSwitch.Get() == true
                       && Run().NumDraw.Get() == 1
                       )
               )
       )
   {
      LogFirstCycleVolumes();
      GiveFirstCycleSound();
   }

   // update cycle volumes
   if (TrimaSet().Cassette.Get() == cassette::PLTPLSRBC)
      PltPlsRBCCycleAcc::Accumulate(mode);
   if (TrimaSet().Cassette.Get() == cassette::RBCPLS)
      RBCPlsCycleAcc::Accumulate(mode);

   // accumulate product and system volumes
   if (TrimaSet().Cassette.Get() == cassette::PLTPLSRBC)
      PltPlsRBCVolumeAcc::Accumulate(mode);
   if (TrimaSet().Cassette.Get() == cassette::RBCPLS)
      RBCPlsVolumeAcc::Accumulate(mode);

   // compute adjusted hct only after blood prime
   if (mode >= ProcData::PRIME)
   {
      MakeRunWritable();
      Run().AdjustedHct.Set(AdjustedHct::compute() );
      MakeRunReadable();
   }

   // Update AC in the Needle manifold
   VolumeAcc::UpdateNeedleAC(mode);

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ProcAdjust_CDS&   ProcData::Adjustments () { RETURN_CDS(Adjustments); }
ProcCycle_CDS&    ProcData::Cycle () { RETURN_CDS(Cycle); }
ProcDonor_CDS&    ProcData::Donor () { RETURN_CDS(Donor); }
ProcPQI_CDS&      ProcData::PQI () { RETURN_CDS(PQI); }
ProcRun_CDS&      ProcData::Run () { RETURN_CDS(Run); }
ProcTrimaSet_CDS& ProcData::TrimaSet () { RETURN_CDS(TrimaSet); }
ProcVolumes_CDS&  ProcData::Volumes () { RETURN_CDS(Volumes); }

bool ProcData::MakeAdjustmentsWritable () { MAKEWRITABLE(Adjustments); }
void ProcData::MakeAdjustmentsReadable () { MAKEREADONLY(Adjustments); }
bool ProcData::MakeCycleWritable () { MAKEWRITABLE(Cycle); }
void ProcData::MakeCycleReadable () { MAKEREADONLY(Cycle); }
bool ProcData::MakeDonorWritable () { MAKEWRITABLE(Donor); }
void ProcData::MakeDonorReadable () { MAKEREADONLY(Donor); }
bool ProcData::MakePQIWritable () { MAKEWRITABLE(PQI); }
void ProcData::MakePQIReadable () { MAKEREADONLY(PQI); }
bool ProcData::MakeRunWritable () { MAKEWRITABLE(Run); }
void ProcData::MakeRunReadable () { MAKEREADONLY(Run); }
bool ProcData::MakeTrimaSetWritable () { MAKEWRITABLE(TrimaSet); }
void ProcData::MakeTrimaSetReadable () { MAKEREADONLY(TrimaSet); }
bool ProcData::MakeVolumesWritable () { MAKEWRITABLE(Volumes); }
void ProcData::MakeVolumesReadable () { MAKEREADONLY(Volumes); }



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::AlarmActive (const bool f)
{
   if (_TheProcData->_AlarmActive != f)
   {
      _TheProcData->_AlarmActive = f;
      PeriodicLog::forceOutput();
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::DisplayedAlarm (const ALARM_VALUES a)
{
   if (_TheProcData->_DisplayedAlarm != a)
   {
      _TheProcData->_DisplayedAlarm = a;
      PeriodicLog::forceOutput();
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::SystemState (const State_names s, const bool global)
{
   if (_TheProcData->_SystemState != s)
   {
      _TheProcData->_SystemState = s;
      PeriodicLog::forceOutput();
   }

   if (s == BLOOD_RINSEBACK
       && Run().DrawCycle.Get())
   {
      //
      // Rinseback is always a return cycle ...
      SwitchtoReturn();
   }

   SystemStateChangeMsg sm;
   if (global)
      sm.init(MessageBase::SEND_GLOBAL);
   else
      sm.init(MessageBase::SEND_LOCAL);
   sm.send(s);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::SubstateName (const string& s)
{
   VistaSubstateName(s);
   if (_TheProcData->_SubstateName != s)
   {
      _TheProcData->_SubstateName = s;
      PeriodicLog::forceOutput();
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::VistaSubstateName (const string& s)
{
   char temp[32];
   strncpy(temp, s.c_str(), 31);
   temp[31] = 0;
   string tempStr = temp;
   if (_TheProcData->_VistaSubstateName != tempStr)
   {
      _TheProcData->_VistaSubstateName = tempStr;
   }
}

void ProcData::RecoveryName (const string& s)
{
   if (_TheProcData->_RecoveryName != s)
   {
      _TheProcData->_RecoveryName = s;
      PeriodicLog::forceOutput();
   }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float ProcData::ResVolume ()
{
   return CobeConfig::data().ReservoirVolume;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float ProcData::MixVolume ()
{
   return CobeConfig::data().ReservoirMixVolume;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float ProcData::ReCircVolume ()
{
   return CobeConfig::data().RecirculateSnDrawVol;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float ProcData::NominalFillVolume ()
{
   if (TrimaSet().Cassette.Get() == cassette::PLTPLSRBC)
      return CobeConfig::data().PltPlsRBCNominalFillVolume;
   else if (TrimaSet().Cassette.Get() == cassette::RBCPLS)
      return CobeConfig::data().RBCPlsNominalFillVolume;
   return 0.0f;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float ProcData::ExtracorporealVolume ()
{
   if (TrimaSet().Cassette.Get() == cassette::PLTPLSRBC)
      return CobeConfig::data().PltPlsRBCExtracorporealVolume;
   else if (TrimaSet().Cassette.Get() == cassette::RBCPLS)
      return CobeConfig::data().RBCPlsExtracorporealVolume;
   return 0.0f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float ProcData::TotalResidualVolume ()
{
   if (TrimaSet().Cassette.Get() == cassette::PLTPLSRBC)
      return CobeConfig::data().PltPlsRBCTotalResidualVolume;
   else if (TrimaSet().Cassette.Get() == cassette::RBCPLS)
      return CobeConfig::data().RBCPlsTotalResidualVolume;
   return 0.0f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float ProcData::DrawTooShort ()
{
   return CobeConfig::data().DrawTooSmallWarning;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float ProcData::DrawTooLong ()
{
   return CobeConfig::data().DrawTooLargeWarning;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float ProcData::ReturnTooShort ()
{
   return CobeConfig::data().ReturnTooSmallWarning;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float ProcData::ReturnTooLong ()
{
   return CobeConfig::data().ReturnTooLargeWarning;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float ProcData::FirstCycleTooShort ()
{
   if (TrimaSet().Cassette.Get() == cassette::PLTPLSRBC)
      return CobeConfig::data().PltPlsRBCFirstCycleTooSoon;
   else if (TrimaSet().Cassette.Get() == cassette::RBCPLS)
      return CobeConfig::data().RBCPlsFirstCycleTooSoon;
   return 0.0f;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float ProcData::FirstCycleTooLate ()
{
   if (TrimaSet().Cassette.Get() == cassette::PLTPLSRBC)
      return CobeConfig::data().PltPlsRBCFirstCycleTooLate;
   else if (TrimaSet().Cassette.Get() == cassette::RBCPLS)
      return CobeConfig::data().RBCPlsFirstCycleTooLate;
   return 0.0f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float ProcData::DrawHighLimit ()
{
   return CobeConfig::data().DrawTooLargeLimit;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::SwitchtoDraw ()
{
   if (!Run().DrawCycle.Get())
   {
      MakeRunWritable();
      Run().CycleSwitch.Set(true);
      Run().DrawCycle.Set(true);
      Run().FirstCycleComplete.Set(true);
      Run().NumDraw.Set(Run().NumDraw.Get() + 1);
      MakeRunReadable();

      //
      // Reset the volumes in the reservoir for the next cycle...
      ResetReservoir();
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::SwitchtoReturn ()
{
   if (Run().DrawCycle.Get())
   {
      MakeRunWritable();
      Run().CycleSwitch.Set(true);
      Run().DrawCycle.Set(false);
      Run().FirstDrawComplete.Set(true);
      Run().NumReturn.Set(Run().NumReturn.Get() + 1);
      MakeRunReadable();

      //
      // Reset the volumes in the reservoir for the next cycle...
      ResetReservoir();
   }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::ResetReservoir ()
{
   //
   // save fill volume for this half cycle
   CycleAcc::ClearFillVolume();

   //
   // clear cycle only pump sums and miscellaneous vars
   CycleAcc::ClearAll();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::LogFirstCycleVolumes ()
{
   if (Run().CycleSwitch.Get())
   {
      float fVol = Volumes().VinTotal.Get() - Volumes().Vreturn.Get();
      DataLog(log_level_proc_blood_run_info) << "1st Cycle Complete: " << fVol
                                             << "ml [" << FirstCycleTooShort() << "," << FirstCycleTooLate() << "]"
                                             << endmsg;
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::GiveFirstCycleSound ()
{
   bool giveSound = false;
   if (Run().CycleSwitch.Get())
   {
      if (!_TheProcData->_FirstCycleSoundGiven) giveSound = true;
   }

   // if not paused
   if (Status().InletPump.dV() > 0)
   {
      // Time before the first return cycle when a sound will notify the
      // operator of its imminent occurrence
      static const float TIME_BEEP_BEFORE_RETURN = 0.25f; // min

      // calculate volume required for reservoir
      // to be 15 seconds from full

      // get the normal set volume
      float fAlertVol = NominalFillVolume();

      // subtract off how much we have pumped in so far
      fAlertVol -= Volumes().VinTotal.Get();
      fAlertVol += Volumes().Vreturn.Get();  // VReturn is negative

      // subtract off 15 seconds worth of flow at
      // the present rate
      fAlertVol -= (TIME_BEEP_BEFORE_RETURN * Status().InletPump.ActFlow());

      if (fAlertVol <= 0)
      {   // give the sound
         giveSound = true;
      }
   }

   if (giveSound && !_TheProcData->_FirstCycleSoundGiven)
   {
      _TheProcData->_FirstCycleSoundGiven = true;

      //
      // Initialize the sound structure ...
      //
      SoundOrders sound;
      sound.soundCode = FIRST_RETURN_SOUND;
      sound.mode      = SOUND_ONCE;

      //
      // Send the sound message
      //
      Message< SoundOrders > msgSoundRequest(MessageBase::SEND_LOCAL);
      msgSoundRequest.send(sound);
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProcData::FillInDonorStruct (SDonorVitals& donorStruct)
{
   FillInDonorUnits(donorStruct);

   donorStruct.cDonorSex            = ( Donor().Female.Get() ? DONOR_SEX_IS_FEMALE : DONOR_SEX_IS_MALE );
   donorStruct.DonorBloodType       = ( BLOODTYPE )Donor().BloodType.Get();
   donorStruct.fDonorHct            = Donor().Hct.Get() * 100.0f;
   donorStruct.fDonorHeight         = Donor().Height.Get();
   donorStruct.fDonorPltPrecount    = Donor().PreCnt.Get();
   donorStruct.fDonorSampleVolume   = Donor().SampleVolume.Get();
   donorStruct.fDonorWeight         = Donor().Weight.Get();
   donorStruct.fProcDonorTBV        = Donor().TBV.Get();
   donorStruct.fProcDonorTBVOgawa   = Donor().TBVOgawa.Get();
   donorStruct.cDonorDownloadStatus = Donor().DownloadStatus.Get();

   CDonor donor(donorStruct);

   if (donor.DonorVitalsEntered())
      donorStruct.cDonorVitalsStatus = DONOR_VITAL_OPERATOR_COMFIRMATION;

   if (donor.DonorInfoEntered())
      donorStruct.cDonorInfoStatus = DONOR_INFO_OPERATOR_COMFIRMATION;

   if (donor.Valid())
      return true;

   return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::FillInDonorUnits (SDonorVitals& donorStruct)
{
   if (Config().LangUnit.Get().key_height == FT_HEIGHT)
      donorStruct.cDonorHeightUnits = HEIGHT_UNITS_IN;
   else
      donorStruct.cDonorHeightUnits = HEIGHT_UNITS_CM;

   if (Config().LangUnit.Get().key_weight == LB_WEIGHT)
      donorStruct.cDonorWeightUnits = WEIGHT_UNITS_LB;
   else
      donorStruct.cDonorWeightUnits = WEIGHT_UNITS_KG;

   if (Config().LangUnit.Get().key_crit_or_glob == RBC_UNIT_CRIT)
      donorStruct.cDonorRbcCount = RBC_COUNT_HEMATOCRIT;
   else
      donorStruct.cDonorRbcCount = RBC_COUNT_HEMOGLOBIN;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ProcData::displayAlarmAsInt (const ALARM_VALUES& alarm)
{
   return (int)_TheProcData->_DisplayedAlarm;
}


const char* ProcData::displayAlarm (const ALARM_VALUES& alarm)
{
   static AlarmTable alarmTable;

   static char       result[128];
   memset(result, 0, sizeof(result));

   if (alarm != NULL_ALARM)
   {

      strcpy(result, "ALARM TYPE: ");

      AlarmStruct& alarmStruct = alarmTable[alarm];

      ALARM_TYPES  alarm_type  = alarmStruct.getType();

      // set the alarm classification to the appropriate value
      switch (alarm_type)
      {
         case OPERATOR_ATTENTION :
            strncat(result,  "W-", 2);
            break;

         case R1 :
            strncat(result,  "R1-", 3);
            break;

         case R2 :
            strncat(result,  "R2-", 3);
            break;

         case A1 :
            strncat(result,  "A1-", 3);
            break;

         case A2 :
            strncat(result,  "A2-", 3);
            break;

         default :
            break;
      }     // End of switch(alarm_type)


      // traverse the alarm name table to access the alarm description string
      int index           = 0;
      int length          = strlen(result);

      int alarmTextLength = strlen(alarmStruct.getDescription());
      strncat(result, alarmStruct.getDescription(), length + alarmTextLength > 127 ? 127 - length : alarmTextLength);

      if (alarmStruct.getName() == NULL_ALARM)
         // Copy a not found message into the alarm in case the given alarm
         // in not contained in the alarm_name_table (ALARM_LIST.)
         strcat(result, "ALARM NOT FOUND IN TABLE");

      // replace any carriage returns in alarm string
      for (index = 0; index <= (int)strlen(result); index++)
      {
         if (result[index] == '\n')
         {
            result[index] = ' ';
         }
      }
      //
      // Ensure no overruns on our char array ...
      result[127] = 0;
   }
   return result;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* ProcData::displayButtonPress ()
{
   static char result[ 22 ];


   long screen_button_id = ((long)_TheProcData->_ButtonMsg.getData().SCREEN_ID << ((sizeof(screen_button_id) / 2) * 8)) + _TheProcData->_ButtonMsg.getData().BUTTON_ID;


   sprintf(result, "0x%08lx %d", screen_button_id, _TheProcData->_ButtonPressCounter);

   return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* ProcData::displayButtonString ()
{
   static char result[64];

   const int   btnPressCount = _TheProcData->_ButtonPressCounter;

   const char* scrnName      = _TheProcData->_ButtonMsg.getData().SCREEN_NAME;
   const int   scrnId        = _TheProcData->_ButtonMsg.getData().SCREEN_ID;

   const char* btnName       = _TheProcData->_ButtonMsg.getData().BUTTON_NAME;
   const int   btnId         = _TheProcData->_ButtonMsg.getData().BUTTON_ID;


   if (scrnName == NULL || btnName == NULL) sprintf(result, "None");
   else
      sprintf(result,
              "C=%d;S=%s(%d);B=%s(%d)",
              btnPressCount,
              scrnName, scrnId,
              btnName, btnId);

   return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


const int ProcData::totalSalineToDonor ()
{
   int   saline                = 0;

   float salineRinsebackVolume =  _TheProcData->_ROVolumesData.VSalineUsedinRinseback.Get()
                                 - CobeConfig::data().SrTotalResidualVolume;
   if (salineRinsebackVolume < 0.0f)
      salineRinsebackVolume = 0.0f;

   saline = (int)(  _TheProcData->_ROVolumesData.VReplacement.Get()
                    +  _TheProcData->_ROVolumesData.VSalineBolus.Get()
                    + salineRinsebackVolume  // resolve with IT 8812
                    + 0.5);

   return saline;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float ProcData::calcPASVolume ()
{
   float ss         = _TheProcData->_RunTargetsData.PltSolutionTarget;
   float multiplier = _TheProcData->_RORunData.PltSSVolumeFactor;

   return(ss * multiplier);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float ProcData::calcRASDelivered ()
{

   float A_SS = _TheProcData->_ROVolumesData.RBCProduct_A_FinalVss.Get() - CobeConfig::data().MssPtfVolume + CobeConfig::data().mssPtfFilterLoss;
   float B_SS = _TheProcData->_ROVolumesData.RBCProduct_B_FinalVss.Get() - CobeConfig::data().MssPtfVolume + CobeConfig::data().mssPtfFilterLoss;

   if (A_SS < 0.0f)
      A_SS = 0.0f;


   if (B_SS < 0.0f)
      B_SS = 0.0f;

   return(A_SS + B_SS);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int ProcData::RBCCalibrationLogging ()
{
   if (_TheProcData->_ROTrimaSetData.RBCDetectorCalibrated.Get())
      return 1;
   else
      return 0;
}


bool ProcData::Filter1Plugged ()
{
   return Run().PtfStatus.Get() & PTF_STATUS_P1_PLUGGED;
}

bool ProcData::Filter2Plugged ()
{
   return Run().PtfStatus.Get() & PTF_STATUS_P2_PLUGGED;
}

void ProcData::SetPluggedFilter ()
{
   unsigned int currentPtfStatus = Run().PtfStatus.Get();
   unsigned int newPtfStatus     = currentPtfStatus;

   bool         targetedAsSplit  = Config().isPromptedDrbcSplit(RunTargets().ProcNumber.Get());
   bool         inMeteredStorage = Volumes().TotalRASVolumePumped.Get() > 0.0f;

   if (targetedAsSplit)
   {
      if (inMeteredStorage)
      {
         if (Run().mssDrbcSplitDone)
         {
            newPtfStatus |= PTF_STATUS_P1_PLUGGED;
            DataLog (log_level_proc_info) << "RBC Filter 1 plugged (oldStatus=" << currentPtfStatus << "; newStatus=" << newPtfStatus << ");" << endmsg;
         }
         else
         {
            newPtfStatus |= PTF_STATUS_P2_PLUGGED;
            DataLog (log_level_proc_info) << "RBC Filter 2 plugged (oldStatus=" << currentPtfStatus << "; newStatus=" << newPtfStatus << ");" << endmsg;
         }
      }
      else
      {
         if (Run().drbcSplitDone)
         {
            newPtfStatus |= PTF_STATUS_P2_PLUGGED;
            DataLog (log_level_proc_info) << "RBC Filter 2 plugged (oldStatus=" << currentPtfStatus << "; newStatus=" << newPtfStatus << ");" << endmsg;
         }
         else
         {
            newPtfStatus |= PTF_STATUS_P1_PLUGGED;
            DataLog (log_level_proc_info) << "RBC Filter 1 plugged (oldStatus=" << currentPtfStatus << "; newStatus=" << newPtfStatus << ");" << endmsg;
         }
      }
   }
   else
   {
      newPtfStatus |= PTF_STATUS_P1_PLUGGED;
      DataLog (log_level_proc_info) << "RBC Filter plugged (oldStatus=" << currentPtfStatus << "; newStatus=" << newPtfStatus << ");" << endmsg;
   }

   if (newPtfStatus != currentPtfStatus)
   {
      MakeRunWritable();
      Run().PtfStatus.Set(newPtfStatus);
      MakeRunReadable();
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
bool ProcData::IsProductCollectionSubstate ()
{
   bool incollect = false;
   if (SystemState() == BLOOD_RUN)
   {
      State_names substate = Run().Substate.Get();
      if ((substate == SS_PREPLATELET_PLASMA) ||
          (substate == SS_PLATELET_PLASMA) ||
          (substate == SS_PCA) ||
          (substate == SS_MIDRUN) ||
          (substate == SS_PIR_PLASMA) ||
          (substate == SS_PIR_NOPLASMA) ||
          (substate == SS_EXTENDED_PCA) ||
          (substate == SS_EXTENDED_PLASMA) ||
          (substate == SS_RBC) ||

          (substate == SS_RP_PLASMA_ONLY) ||        // RBC-Plasma plasma only
          (substate == SS_RP_RBC) ||                // RBC collection
          (substate == SS_RP_RBC_DOUBLE)) incollect = true;
   }
   return incollect;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
bool ProcData::SafetyControlReservoirIsEmpty (void)
{

   SHW_RESERVOIR_STATES s_resStatus = SafetyStatus().reservoir;

   static bool          isEmpty     = true;
   static bool          isNotEmpty  = false;


   if ( ( SHW_RESERVOIR_EMPTY == s_resStatus )
        && Status().ReservoirIsEmpty()
        )
   {
      isEmpty = true;
      if (isNotEmpty)
         DataLog (log_level_proc_info) << "Res Empty control/safety agree."  << endmsg;
      isNotEmpty = false;

   }
   else
   {

      isEmpty = false;
      if (!isNotEmpty)
         DataLog (log_level_proc_info) << "Res Fluid control/safety agree."  << endmsg;
      isNotEmpty = true;
   }

   return isEmpty;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcData::definePeriodicSet (DataLog_SetHandle pset)
{
   // First, setup some quick references to make are lives easier
   short (BaseElement<short>::* short_func) (void) const = &BaseElement<short>::Get;
   int (BaseElement<int>::* int_func) (void) const       = &BaseElement<int>::Get;
   unsigned int (BaseElement<unsigned int>::* uint_func) (void) const = &BaseElement<unsigned int>::Get;
   float (BaseElement<float>::* float_func) (void) const = &BaseElement<float>::Get;
   const char* (string::* string_func)(void)const        = &string::c_str;

   HalStatus_CDS&   hal_status = Status();
   const HalOrders& hal_orders = Orders().cData();

   datalog_AddArgFunc(pset,    &ProcData::displayAlarm,      DisplayedAlarm(), "Alarm",     "Current top alarm", "%s");
   datalog_AddArgFunc(pset,    &ProcData::displayAlarmAsInt, DisplayedAlarm(), "AlarmEnum", "Current top alarm Enum", "%d");
   datalog_AddFunc(pset,       &ProcData::displayButtonPress,             "GUIButtonPress",       "GUI Button Id (SSBB) S-screen B-button",  "%s");
   datalog_AddFunc(pset,       &ProcData::displayButtonString,            "GUIButtonPressString", "GUI Button String (C=count;S=Screen;B=Button)",  "%s");

   datalog_AddMemberFunc(pset, &Run().CassetteRef,          int_func,     "Ref",                  "disposable set REF number",         "%d");
   datalog_AddMemberFunc(pset, &Run().CassetteFunctionBits, uint_func,    "FunctBits",            "disposable set function bits",      "%d");

   datalog_AddMemberFunc(pset, &AbsTime(), &TimeKeeper::getMins,             "AbsTime",           "absolute time (minutes)",      "%.2lf");
   datalog_AddMemberFunc(pset, &ProcTime(), &TimeKeeper::getMins,            "ProcTime",          "procedure time (minutes)",     "%.2lf");
   datalog_AddMemberFunc(pset, &SysRunTime(), &TimeKeeper::getMins,          "SysRunTime",        "system run time (minutes)",    "%.2lf");
   datalog_AddMemberFunc(pset, &RunTargets().ProcTimeTarget,        float_func,             "TargetRunTime",     "procedure target run time (minutes)", "%.2f");
   datalog_AddMemberFunc(pset, &MSSRunTime(), &TimeKeeper::getMins,       "MeteringTime",      "metered storage run time (minutes)", "%.2f");
   datalog_AddMemberFunc(pset, &RunTargets().ProcNumber, int_func,          "ProcedureNumber", "selected procedure number", "%d");

   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertSystemState, this->SystemState(),               "SystemState",       "system wide procedure state",  "%s");
   datalog_AddMemberFunc(pset, &SubstateName(), string_func,                 "ProcSubstate",      "procedure substate name",      "%s");
   datalog_AddMemberFunc(pset, &RecoveryName(), string_func,                 "Recovery",          "procedure recovery name",      "%s");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertAlarmActive, this->AlarmActive(),             "AlarmStateFlag",   "system has an active alarm",   "%s");

   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertReservoirStatus, hal_status._reservoir,      "ReservoirStatus",   "reservoir status",              "%c");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertSafetyReservoirStatus, SafetyStatus().reservoir,      "SafetyReservoirStatus",   "safety reservoir status",   "%c");
   datalog_AddMemberFunc(pset, &Cycle().VresLast, float_func,                "LastResVol",        "last reservoir volume (ml)",    "%.1f");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertDrawCycle, this->Run().DrawCycle,               "DrawCycle",         "draw cycle (1=draw 0=return)",  "%d");

   datalog_AddRef(pset, hal_orders.CmdFlow.Inlet,                               "InletCmd",          "inlet pump command (ml/min)",   "%.1f");
   datalog_AddMemberFunc(pset, &hal_status.InletPump._ActFlow, float_func,         "InletAct",          "inlet pump actual (ml/min)",    "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().Vin, float_func,                "InletVol",          "inlet pump volume (ml)",        "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().VinTotal, float_func,              "InletTotalVol",     "inlet pump total volume (ml)",  "%.1f");

   datalog_AddRef(pset, hal_orders.CmdFlow.Ac,                                  "ACCmd",             "AC pump command (ml/min)",      "%.1f");
   datalog_AddMemberFunc(pset, &hal_status.ACPump._ActFlow, float_func,            "ACAct",             "AC pump actual (ml/min)",       "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().Vac, float_func,                "ACVol",             "AC pump volume (ml)",           "%.1f");

   datalog_AddRef(pset, hal_orders.CmdFlow.Plasma,                                 "PlasmaCmd",         "plasma pump command (ml/min)",  "%.1f");
   datalog_AddMemberFunc(pset, &hal_status.PlasmaPump._ActFlow, float_func,     "PlasmaAct",         "plasma pump actual (ml/min)",   "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().Vp, float_func,                    "PlasmaVol",         "plasma pump volume (ml)",       "%.1f");

   datalog_AddRef(pset, hal_orders.CmdFlow.Collect,                             "CollectCmd",        "collect pump command (ml/min)",  "%.1f");
   datalog_AddMemberFunc(pset, &hal_status.CollectPump._ActFlow, float_func,    "CollectAct",        "collect pump actual (ml/min)",   "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().Vc, float_func,                    "CollectVol",        "collect pump volume (ml)",       "%.1f");

   datalog_AddMemberFunc(pset, &TrimaSet().CmdQch, float_func,               "ChamberCmd",        "chamber flow command (ml/min)", "%.3f");
   datalog_AddMemberFunc(pset, &TrimaSet().ActQch, float_func,               "ChamberAct",        "chamber flow actual (ml/min)",  "%.3f");

   datalog_AddRef(pset, hal_orders.CmdFlow.Return,                                 "ReturnCmd",         "return pump command (ml/min)",  "%.1f");
   datalog_AddMemberFunc(pset, &hal_status.ReturnPump._ActFlow, float_func,     "ReturnAct",         "return pump actual (ml/min)",   "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().Vreturn, float_func,                     "ReturnVol",         "return pump volume (ml)",       "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().ReturnPumpTotalVolume, float_func,       "RunningVolret",     "return pump volume(ml) running total",       "%.1f");

   datalog_AddRef(pset, hal_orders.CmdCent.RPM,                                    "CentCmd",           "centrifuge command (RPM)",      "%.0f");
   datalog_AddMemberFunc(pset, &hal_status._centRPM, float_func,                "CentAct",           "centrifuge actual (RPM)",       "%.0f");

   datalog_AddMemberFunc(pset, &hal_status._redValue, short_func,                  "RedReflectance",    "red reflectance",               "%d");
   datalog_AddMemberFunc(pset, &hal_status._greenValue, short_func,             "GreenReflectance",  "green reflectance",             "%d");

   datalog_AddFunc(pset, &ProcData::RBCCalibrationLogging,      "RBCinCal",    "cal=1",                "%d");



   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertValveStatus, hal_status.CollectValve._status,   "CollectValvePos",   "collect valve position",        "%c");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertValveOrder, hal_orders.CmdValve.collect,        "CollectValveCmd",   "collect valve command",         "%c");

   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertValveStatus, hal_status.PlasmaValve._status,    "PlasmaValvePos",    "plasma valve position",         "%c");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertValveOrder, hal_orders.CmdValve.plasma,         "PlasmaValveCmd",    "plasma valve command",          "%c");

   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertValveStatus, hal_status.RBCValve._status,       "RBCValvePos",       "RBC valve position",            "%c");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertValveOrder, hal_orders.CmdValve.rbc,            "RBCValveCmd",       "RBC valve command",             "%c");

   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertCassetteStatus, hal_status._cassettePos,        "CassettePos",       "Cassette position",             "%c");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertCassetteCommand, hal_orders.cassettePosition,   "CassetteCmd",       "Cassette command",              "%s");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertCassetteID, hal_status._cassetteId,             "CassetteID",        "Cassette type",                 "%d");


   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertCassetteState, this->TrimaSet().CassetteState,     "CassetteState",        "Cassette State",    "%s");

   datalog_AddMemberFunc(pset, &hal_status._cps, float_func,                    "CPS",               "CPS (mm Hg)",                   "%.1f");
   datalog_AddMemberFunc(pset, &hal_status._apsLow, short_func,                 "APSLow",            "APS low limit",                 "%d");
   datalog_AddMemberFunc(pset, &hal_status._aps, float_func,                    "APS",               "APS (mm Hg)",                   "%.1f");
   datalog_AddMemberFunc(pset, &hal_status._apsHigh, short_func,                "APSHigh",           "APS high limit",                "%d");

   datalog_AddMemberFunc(pset, &Adjustments().Draw.CurrentCap, float_func, "QinAdjustment",   "inlet flow rate operator adjustment", "%.2f");
   datalog_AddMemberFunc(pset, &Adjustments().Return.CurrentCap, float_func, "QrpAdjustment",   "return flow rate operator adjustment", "%.2f");
   datalog_AddMemberFunc(pset, &Adjustments().Clumping.CurrentCap, float_func, "RatioAdjustment", "AC ratio operator adjustment", "%.2f");
   datalog_AddMemberFunc(pset, &Adjustments().Tingling.CurrentCap, float_func, "IRAdjustment",     "AC infusion rate operator adjustment", "%.2f");


   datalog_AddMemberFunc(pset, &Run().LastRunInfusion, float_func,       "LastRunInfusion", "the last infusion rate used", "%.1f");
   datalog_AddMemberFunc(pset, &Run().LastRunRatio, float_func,       "LastRunRatio", "the last ratio used", "%.1f");


   datalog_AddMemberFunc(pset, &Run().AdjustedHct, float_func,               "AdjustedDonorHCT",  "adjusted donor HCT (fraction)", "%.5f");
   datalog_AddMemberFunc(pset, &Cycle().ResHct, float_func,                  "ReservoirHCT",      "reservoir HCT (fraction)",      "%.2f");
   datalog_AddMemberFunc(pset, &Cycle().Hrbc, float_func,                    "Hrbc",              "rbc line HCT (fraction)",       "%.2f");
   datalog_AddMemberFunc(pset, &Volumes().VnonACBloodAtNeedle, float_func,   "CoagNeedleBlood",   "coagulated needle blood (ml)",  "%.1f");

   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertFluidDetected, hal_status._acDetect,         "ACDetected",        "1=fluid 0=air E=error",         "%c");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertFluidDetected, hal_status._Leak,             "LeakDetected",      "1=fluid 0=air E=error",         "%c");

   datalog_AddMemberFunc(pset, &Run().IntegratedPlateletYield, float_func,  "IntegratedPltYield", "platelet yield as seen from the RBC detector", "%.1f");
   // datalog_AddMemberFunc(pset, &Predict().CalibratedPlateletYield, float_func,  "CalibratedPltYield","calibrated platelet yield as seen from the RBC detector", "%.1f");
   datalog_AddMemberFunc(pset, &RunTargets().PlateletYieldTarget,   float_func,             "TargetPltYield",    "target platelet yield", "%.1f");
   datalog_AddMemberFunc(pset, &RunTargets().PlateletVolumeTarget,  float_func,             "TargetPltVolume",   "target platelet volume (ml)", "%.1f");
   datalog_AddMemberFunc(pset, &Run().Ydpur, float_func, "Ydpur",   "Platelet Yield decrement (e11) due to WBC purges", "%.2f");

   datalog_AddMemberFunc(pset, &RunTargets().PlasmaTarget,          float_func,             "TargetPlsVolume",   "target plasma volume (ml)", "%.1f");

   datalog_AddMemberFunc(pset, &RunTargets().RbcVolumeTarget,       float_func,             "TargetRBCVolume",   "target RBC volume (ml)", "%.1f");
   datalog_AddMemberFunc(pset, &RunTargets().RbcDoseTarget,         float_func,             "TargetRBCDose",     "target RBC dose", "%.1f");

   datalog_AddMemberFunc(pset, &Volumes().VSalineUsedinRinseback, float_func, "SalineRinsebackVolume", "volume of replacement fluid given during saline Rinseback", "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().VSalineBolus, float_func,     "SalineBolusVolume", "volume of saline bolus given during the run", "%.1f");
   datalog_AddMemberFunc(pset, &Run().LastRunInfusion, float_func,         "InfusionRate",      "instantaneous AC infusion rate", "%.2f");

   datalog_AddMemberFunc(pset, &Volumes().VpltStorageRemaining, float_func,   "PltStorageRemaining",  "Remaining storage volume in PAS bag", "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().VrbcStorageRemaining, float_func,   "RBCStorageRemaining",  "Remaining storage volume in RAS bag", "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().VpltBagCapacity, float_func,        "PltBagCapacity",       "PAS storage bag capacity", "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().VrbcBagCapacity, float_func,        "RBCBagCapacity",       "RAS storage bag capacity", "%.1f");

   datalog_AddFunc(pset, &ProcData::calcRASDelivered,                            "RASDelivered",       "RAS Delivered in the product bag mL", "%.1f");

   datalog_AddMemberFunc(pset, &Volumes().TotalPASVolumePumped, float_func,        "TotalPASPumped",       "Total PAS volume pumped out", "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().TotalRASVolumePumped, float_func,        "TotalRASPumped",       "Total RAS volume pumped out", "%.1f");

   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertDoorCommand, hal_orders.doorCmd,             "DoorCommand",       "1=door commanded locked",       "%d");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertDoorStatus, hal_status._doorStatus,          "DoorStatus",        "O/C=open/close L/U=lock/unlock", "%s");

   datalog_AddMemberFunc(pset, &VistaSubstateName(), string_func,            "Substate",          "procedure substate name sent to vista",      "%s");

   datalog_AddMemberFunc(pset, &hal_status._lowAGC, short_func,                 "LowAGC",               "lower level sensor raw reading",   "%d");
   datalog_AddMemberFunc(pset, &hal_status._highAGC, short_func,                "HighAGC",              "upper level sensor raw reading",   "%d");

   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertTemperature, hal_status._EMITemp,         "EMITemp",           "EMI box temperature (celsius)", "%.1f");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertCentCurrent, hal_status._centI,           "CentI",             "centrifuge current (amps)",     "%.3f");

   datalog_AddMemberFunc(pset, &hal_status._LeakValue, short_func,           "LeakValue",         "raw leak sensor reading",       "%d");

   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertVoltage, hal_status._plus5V,              "PS+5V",             "+5 supply (volts)",             "%.3f");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertVoltage, hal_status._minus12V,            "PS-12V",            "-12 supply (volts)",            "%.3f");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertVoltage, hal_status._twelveV,             "PS+12V",            "+12 supply (volts)",            "%.3f");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertVoltage, hal_status._twentyFourV,         "PS+24V",            "+24 supply (volts)",            "%.3f");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertVoltage, hal_status._twentyFourSwV,       "PS+24V/Switched",   "+24 switched supply (volts)",   "%.3f");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertCurrent, hal_status._twentyFourVI,        "PS+24VI",           "+24 supply current (amps)",     "%.3f");

   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertVoltage, hal_status._sixtyFourV,          "PS+64V",            "+64 supply (volts)",            "%.3f");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertVoltage, hal_status._sixtyFourSwV,        "PS+64V/Switched",   "+64 switched supply (volts)",   "%.3f");
   datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertCurrent, hal_status._sixtyFourVI,         "PS+64VI",           "+64 supply current (amps)",     "%.3f");
}



void ProcData::defineEORSet (DataLog_SetHandle pset)
{
   // First, setup some quick references to make are lives easier
   float (BaseElement<float>::* float_func) (void) const = &BaseElement<float>::Get;

   // Time
   datalog_AddMemberFunc(pset, &ProcRunTime(),                     &TimeKeeper::getMins,    "ProcRunTime",       "procedure run time (minutes)", "%.2lf");

   // Product collect volumes
   datalog_AddMemberFunc(pset, &Volumes().PlateletYield,            float_func,             "PlateletYield",     "platelet yield as predicted by the collection algorithm", "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().PlateletVolume,           float_func,             "PlateletBagVolume", "total volume in the platelet bag (ml)", "%.1f");

   datalog_AddMemberFunc(pset, &Volumes().PlasmaVolume,             float_func,             "PlasmaBagVolume",   "total volume in the plasma bag (ml)", "%.1f");

   datalog_AddMemberFunc(pset, &Volumes().RBCDose,                  float_func,             "TotalRBCs",         "total rbcs in the RBC bag", "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().RBCVolume,                float_func,             "RBCBagVolume",      "total volume in the RBC bag (ml)", "%.1f");

   // DRBC collect volumes
   datalog_AddMemberFunc(pset, &Volumes().RBCProduct_A_FinalVolume, float_func,             "RBC_1_Vol",         "RBC volume past the valve to RBC bag 1 (ml) ", "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().RBCProduct_A_FinalDose,   float_func,             "RBC_1_Dose",        "RBC Dose past the valve to RBC bag 1 (ml)", "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().RBCProduct_A_FinalVac,    float_func,             "RBC_1_AC",          "AC volume past the valve to RBC bag 1 (ml)", "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().RBCProduct_A_FinalVss,    float_func,             "RBC_1_SS",          "SS volume past the valve to RBC bag 1 (ml)", "%.1f");

   datalog_AddMemberFunc(pset, &Volumes().RBCProduct_B_FinalVolume, float_func,             "RBC_2_Vol",         "RBC volume past the valve to RBC bag 2 (ml)", "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().RBCProduct_B_FinalDose,   float_func,             "RBC_2_Dose",        "RBC Dose past the valve to RBC bag 2 (ml)", "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().RBCProduct_B_FinalVac,    float_func,             "RBC_2_AC",          "AC volume past the valve to RBC bag 2 (ml)", "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().RBCProduct_B_FinalVss,    float_func,             "RBC_2_SS",          "SS volume past the valve to RBC bag 2 (ml)", "%.1f");

   datalog_AddMemberFunc(pset, &Volumes().VacPlateletBag,           float_func,             "PlateletBagAC",     "AC volume in the platelet bag (ml)", "%.3f");
   datalog_AddMemberFunc(pset, &Volumes().VacPlasmaBag,             float_func,             "PlasmaBagAC",       "AC volume in the plasma bag (ml)", "%.3f");
   datalog_AddMemberFunc(pset, &Volumes().VacRBCBag,                float_func,             "RBCBagAC",          "AC volume in the RBC bag (ml)", "%.3f");

   datalog_AddMemberFunc(pset, &Volumes().VstorageRBCBag,           float_func,             "RBCStorageBag",     "RAS in prod bag", "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().VstoragePltBag,           float_func,             "PltStorageBag",     "PAS in prod bag", "%.1f");
   datalog_AddMemberFunc(pset, &RunTargets().RBCSolutionTarget,     float_func,             "RASVolume",         "Required RAS Volm mL", "%.1f");
   datalog_AddFunc(pset,       &ProcData::calcPASVolume,                                    "PASVolume",         "Required PAS Volm mL", "%.1f");


   datalog_AddMemberFunc(pset, &Volumes().VBP,                      float_func,             "VbpTotal",          "total volume of blood processed", "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().VBPPlatelet,              float_func,             "VbpPlatelet",       "volume of blood processed during platelet collection", "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().VacTotal,                 float_func,             "ACTotalVol",        "AC pump total volume (ml)",     "%.1f");
   datalog_AddMemberFunc(pset, &Volumes().VReplacement,             float_func,             "ReplacementVolume", "volume of replacement fluid given during the run", "%.1f");
}


//
// Log things not otherwise logged by Proc
//
void ProcData::ProcDataStruct::logEORData ()
{
   static DataLog_SetHandle eorData = NULL;

   // Create the EORS data set
   datalog_CreatePeriodicSet("Proc_EORData2", &eorData);

   DataLog(log_level_proc_info) << "Logging EOR Data" << endmsg;

   // End of Run data in an easy to use column format :)
   datalog_AddRef(eorData,  _runSummData.rs2_RBC_volume,                  "EOR_RBC1_VOLM",               "end of run value RBC volume prod 1 (use last entry)", "%d");
   datalog_AddRef(eorData,  _runSummData.rs2_RBC_volume_2,                "EOR_RBC2_VOLM",               "end of run value RBC volume prod 2 (use last entry)", "%d");
   datalog_AddRef(eorData,  _runSummData.rs2_SS_rbc_vol,                  "EOR_RAS1_VOLM",               "end of run value RAS volume in prod 1 (use last entry)", "%d");
   datalog_AddRef(eorData,  _runSummData.rs2_SS_rbc_vol_2,                "EOR_RAS2_VOLM",               "end of run value RAS volume in prod 2 (use last entry)", "%d");
   datalog_AddRef(eorData,  _runSummData.rs2_SS_plat_vol,                 "EOR_PAS_VOLM",                "end of run value PAS volume in PLT prod (use last entry)", "%d");
   datalog_AddRef(eorData,  _runSummData.rs1_post_platelet_count,         "EOR_POSTCOUNT",               "end of run value PostCount (use last entry)", "%d");
   datalog_AddRef(eorData,  _runSummData.rs1_post_hct,                    "EOR_POSTHCT",                 "end of run value Post HCT (use last entry)", "%.1f");
   datalog_AddRef(eorData,  _runSummData.rs1_actual_AC_to_donor,          "EOR_AC_TO_DONOR",             "end of run value AC to Donor (use last entry)", "%d");
   datalog_AddRef(eorData,  _runSummData.rs1_residual_loss_prbc,          "EOR_RBC_Residual",            "end of run value RBC Residual (use last entry)", "%d");
   datalog_AddRef(eorData,  _runSummData.rs1_residual_loss_plasma,        "EOR_PLS_Residual",            "end of run value PLS Residual (use last entry)", "%d");
   datalog_AddRef(eorData,  _runSummData.rbc_salvage_volume,              "EOR_Offline_RAS1",            "end of run value Offline RAS prod 1 (use last entry)", "%d");
   datalog_AddRef(eorData,  _runSummData.rbc_salvage_volume_2,            "EOR_Offline_RAS2",            "end of run value Offline RAS prod 2 (use last entry)", "%d");
   datalog_AddRef(eorData,  _runSummData.platelet_salvage_volume,         "EOR_Offline_PAS",             "end of run value Offline PAS (use last entry)", "%d");
   datalog_AddRef(eorData,  _runSummData.rs1_last_adjust_flow_rate,       "EOR_Draw_FlowRate",           "end of run value flow rate (use last entry)", "%d");
   datalog_AddRef(eorData,  _runSummData.plasma_rinseback_status,         "EOR_Plasma_Rinseback_Status", "end of run value Plasma Rinseback Status", "%d");
   datalog_AddRef(eorData,  _runSummData.plasma_rinseback_status,         "EOR_Saline_Rinseback_Status", "end of run value Saline Rinseback Status", "%d");
   datalog_AddRef(eorData,  _runSummData.rs1_total_blood_vol_processed,   "EOR_TBV_Processed",           "end of run value TBV Processed", "%d");
   datalog_AddRef(eorData,  _runSummData.rs2_comp_pls_volm,               "EOR_COMP_PLS_VOLM",           "end of run value Complementary Plasma Volume", "%d");
   datalog_AddRef(eorData,  _runSummData.rs2_comp_pls_icon,               "EOR_COMP_PLS_ICON",           "end of run value Complementary Plasma Icon (1=displayed 0=no display)", "%d");
   datalog_AddFunc(eorData, &ProcData::totalSalineToDonor,                "EOR_ReplacementFluid",        "end of run value of the total saline (replacement fluid) to the donor (use last entry)",  "%d");

   // Write the data
   datalog_SetPeriodicOutputInterval(eorData, 0);
   datalog_ForcePeriodicOutput(eorData, true);
}

/* FORMAT HASH f924a2dcd198b87044bae3d81f1aed9c */

#include <vxWorks.h>

#ifndef WIN32
#pragma implementation "procrun_cds.h"
#endif

#include "procrun_cds.h"

ProcRun_CDS::ProcRun_CDS (Role role)
   : MultWriteDataStore ("ProcRun_CDS", role)
{
   Substate.Register                (this, PFR_RECOVER, INVALID_SUBSTATE);
   PredictRequest.Register          (this, NO_PFR, 0);

   EnteredCollect.Register          (this, PFR_RECOVER, false);
   DrawCycle.Register               (this, PFR_RECOVER, true);
   CycleSwitch.Register             (this, PFR_RECOVER, false);
   FirstDrawComplete.Register       (this, PFR_RECOVER, false);
   FirstCycleComplete.Register      (this, PFR_RECOVER, false);
   NumDraw.Register                 (this, PFR_RECOVER, 1);
   NumReturn.Register               (this, PFR_RECOVER, 0);

   struct timespec timeSpec; timeSpec.tv_nsec = 0; timeSpec.tv_sec = 0;
   StartPauseTime.Register          (this, PFR_RECOVER, timeSpec);
   ProcEndRunTime.Register          (this, PFR_RECOVER, 0);
   ProcStartAasTime.Register        (this, PFR_RECOVER, 0);

   NumRetTooLong.Register           (this, PFR_RECOVER, 0);
   NumConsecRetTooLong.Register     (this, PFR_RECOVER, 0);
   NumConsecDrawTooLong.Register    (this, PFR_RECOVER, 0);
   DrawTooShortInPltCol.Register    (this, PFR_RECOVER, false);
   BalanceCyclesDisableWBCPLTFlag.Register    (this, PFR_RECOVER, false);

   DrawTooShortInPlsCol.Register    (this, PFR_RECOVER, false);
   DrawTooLongInRBCCol.Register     (this, PFR_RECOVER, false);
   DrawTooLongInRBCColProd2.Register(this, PFR_RECOVER, false);

   LastRunRatio.Register            (this, PFR_RECOVER, MAX_RATIO);
   LastRunInfusion.Register         (this, PFR_RECOVER, MAX_INFUSION_RATE);
   IntegratedPlateletYield.Register (this, PFR_RECOVER, 0.0f);
   AdjustedHct.Register             (this, PFR_RECOVER, 0.0f);
   Qchcurrent.Register              (this, PFR_RECOVER, 0);
   Qchmin.Register                  (this, PFR_RECOVER, 0);
   Ydpur.Register                   (this, PFR_RECOVER, 0.0f);
   NumberWBCPurges.Register         (this, PFR_RECOVER, 0);
   WBCPurgeType.Register            (this, PFR_RECOVER, WBCPURGE_NONE);
   LastProcPurge.Register           (this, PFR_RECOVER, 0.0f);

   RecoveryCountArray.Register      (this, PFR_RECOVER);

   SpilloverDuringPIR.Register      (this, PFR_RECOVER, 0);
   SalineConnected.Register         (this, PFR_RECOVER, 0);
   RinsebackStarted.Register        (this, PFR_RECOVER, 0);
   RinsebackComplete.Register       (this, PFR_RECOVER, RINSEBACK_UNSUCCESSFUL);

   ShowedPltWbcContamAlarm.Register (this, PFR_RECOVER, false);

   Red.Register                     (this, NO_PFR,      0);
   RedPl.Register                   (this, PFR_RECOVER, 100);
   Green.Register                   (this, NO_PFR,      0);
   RGr.Register                     (this, NO_PFR,      0.0f);
   RGrMd.Register                   (this, NO_PFR,      0.0f);
   dRGrMd.Register                  (this, NO_PFR,      0.0f);
   Cr.Register                      (this, NO_PFR,      0.0f);
   CrAv.Register                    (this, NO_PFR,      0.0f);
   QchChamSat.Register              (this, NO_PFR,      0.0f);
   RGp.Register                     (this, NO_PFR,      0.0f);
   RGpMd.Register                   (this, NO_PFR,      0.0f);
   AvRGpMd.Register                 (this, NO_PFR,      0.0f);
   RGpMdBaseline.Register           (this, NO_PFR,      0.0f);
   RGpMdBaselineA.Register          (this, PFR_RECOVER, 0.0f);
   RGpMdBaselineB.Register          (this, NO_PFR,      0.0f);
   Crit3Status.Register             (this, NO_PFR,      0);
   Crit3aStatus.Register            (this, NO_PFR,      0);
   Crit3bStatus.Register            (this, NO_PFR,      0);
   Crit4Status.Register             (this, NO_PFR,      0);
   Crit4aStatus.Register            (this, NO_PFR,      0);
   Crit4bStatus.Register            (this, NO_PFR,      0);

   PurgeDueToChamberSat.Register    (this, PFR_RECOVER, false);

   procedureStartTime.Register      (this, PFR_RECOVER, 0.0f);

   MeteringStarted.Register         (this, PFR_RECOVER, false);
   MeterPlatelets_OK.Register       (this, PFR_RECOVER, false);
   MeterRbcs_OK.Register            (this, PFR_RECOVER, false);
   PltMeteringDone.Register         (this, PFR_RECOVER, false);
   RBCMeteringDone.Register         (this, PFR_RECOVER, false);
   PltSSVolumeFactor.Register       (this, PFR_RECOVER, 1.0f);

   // MSS PFR
   currentMSSSubState.Register       (this, PFR_RECOVER, INVALID_SUBSTATE); // the substate MSS is in currently and should goto on PFR
   currentRBCMSSSubState.Register    (this, PFR_RECOVER, INVALID_SUBSTATE); // the substate RBC MSS is in currently during the parallel state process
   currentPLTMSSSubState.Register    (this, PFR_RECOVER, INVALID_SUBSTATE); // the substate PLT MSS is in currently during the parallel state process
   mssInPFR.Register                 (this, PFR_RECOVER, false);
   mssVacCycle.Register              (this, PFR_RECOVER, 0);
   mssVacPhase.Register              (this, PFR_RECOVER, 0);
   vacPurgePltVolm.Register          (this, PFR_RECOVER, 0.0f);
   vacPurgePlsVolm.Register          (this, PFR_RECOVER, 0.0f);
   hadPrimedVolm.Register            (this, PFR_RECOVER, 0.0f);
   hadPrimedReturnVolm.Register      (this, PFR_RECOVER, 0.0f);


   // inlet volm needs to be saved for
   // channel check PFR and ongoing Inlet Volmume alarm monitoring
   inletVolm.Register                (this, PFR_RECOVER, 0.0f);
   channelInletVolm.Register         (this, PFR_RECOVER, 0.0f);
   clampCloseRetVolm.Register        (this, PFR_RECOVER, 0.0f);
   CassetteFunctionBits.Register     (this, PFR_RECOVER, 0);
   CassetteRef.Register              (this, PFR_RECOVER, 0);

   cancelMSS.Register                   (this, PFR_RECOVER, false);
   AllowMSSAfterA2SpecialCase.Register  (this, PFR_RECOVER, false);


   cancelJustRas.Register            (this, PFR_RECOVER, false);
   cancelJustPas.Register            (this, PFR_RECOVER, false);

   redoCassettePurge.Register        (this, PFR_RECOVER, false);
   pauseCassettePurge.Register       (this, PFR_RECOVER, false);

   cassettePurgeSetup.Register       (this, PFR_RECOVER, false);

   rinsebackEnteredMss.Register         (this, PFR_RECOVER, false);
   rinsebackVolumeSuccessMss.Register   (this, PFR_RECOVER, false);


   flagMSSPASConcetrationError.Register   (this, PFR_RECOVER, false);
   flagMSSPASVolumeError.Register         (this, PFR_RECOVER, false);



   flagMSSRASVolumeError.Register         (this, PFR_RECOVER, false);
   RASAdditionSpeedReductionFactor.Register     (this, PFR_RECOVER, 0);

   SecondPLTSSBag.Register           (this, PFR_RECOVER, false);
   CurrentRecovery.Register          (this, PFR_RECOVER, RecoveryTypes::None);
   PreviousRecovery.Register         (this, PFR_RECOVER, RecoveryTypes::None);
   VinClearLine.Register             (this, PFR_RECOVER, 0.0f);


   PTFRBCprod1Disabled.Register     (this, PFR_RECOVER, false); // these turn off the individual RBC collections on a PTF double.
   PTFRBCprod2Disabled.Register     (this, PFR_RECOVER, false);

   stopCheckingRbcASBagEmpty.Register     (this, NO_PFR, false);
   stopCheckingPltASBagEmpty.Register     (this, NO_PFR, false);

   drbcSplitDone.Register        (this, PFR_RECOVER, false);
   mssDrbcSplitDone.Register     (this, PFR_RECOVER, false);


   WBCPurgeStartTime.Register            (this, PFR_RECOVER, 0.0f);
   MssApsTooHighRBCProduct1.Register     (this, PFR_RECOVER, 0);
   MssApsTooHighRBCProduct2.Register     (this, PFR_RECOVER, 0);


   IRMax.Register                              (this, PFR_RECOVER, 0.0f);
   QReturnMax.Register                         (this, PFR_RECOVER, 0.0f);

   wbcAlarmPending.Register                    (this, PFR_RECOVER, false);
   LastRbcQp.Register                          (this, PFR_RECOVER, 0.0f);
   DonorDisconnectTime.Register                (this, PFR_RECOVER, 0.0f);
   MeteringDisconnectTime.Register             (this, PFR_RECOVER, 0.0f);
   LastCommandedQp.Register                    (this, NO_PFR, 0.0f);

   LastCentrifugeSpeed.Register                (this, PFR_RECOVER, 0.0f);

   SolutionsConnected.Register                (this, PFR_RECOVER, false);

   MssUserAbortType.Register               (this, PFR_RECOVER, MSS_USER_ABORT_TYPE_NONE);

   PtfStatus.Register                      (this, PFR_RECOVER, PTF_STATUS_OK);
   FinishedFirstRBCDoubleReturn.Register   (this, PFR_RECOVER, false);
   PrePirRamp.Register     (this, PFR_RECOVER, false);
   FinishedFirstRBCReturn.Register         (this, PFR_RECOVER, false);
   SkipPauseValveInMSSPurge.Register       (this, PFR_RECOVER, false);
   SalinePrimed.Register                   (this, PFR_RECOVER, false);

   DoCPTPTFRecovery.Register               (this, NO_PFR, false);

   PlsBagEvacVc.Register                   (this, NO_PFR, 0.0f);

   PlsBagEvacVpls.Register                 (this, NO_PFR, 0.0f);
   PlsBagEvacVin_done.Register             (this, NO_PFR, 0.0f);

   FlagforShortDrawNow.Register            (this, NO_PFR, false);

   ShowCompPlsSymbol.Register              (this, PFR_RECOVER, false);


   NScheduledWBCPurges.Register     (this, PFR_RECOVER, 0);
   ActualPrimeDuration.Register                (this, NO_PFR, 0.0f);
   SubstateStartTime.Register              (this, PFR_RECOVER, 0.0f);
   MidrunEntered.Register                  (this, PFR_RECOVER, 0);
   PreviousSubstate.Register               (this, PFR_RECOVER, INVALID_SUBSTATE);


   inPPR.Register                 (this, NO_PFR, false);

   LatestQch.Register                (this, PFR_RECOVER, 0.0f);

   // AUTOFLOW CHANGES
   stopRamp.Register                 (this, PFR_RECOVER, false);
   deadRampMaxQin.Register           (this, PFR_RECOVER, 0.0f);

   AutoflowDeltaTime.Register                  (this, PFR_RECOVER, 0.0f);
   AutoflowTotalNetTimeChanges.Register        (this, PFR_RECOVER, 0.0f);
   AutoflowTotal10MinAlarms.Register           (this, PFR_RECOVER, 0);
}

ProcRun_CDS::~ProcRun_CDS()
{}

/* FORMAT HASH b5efb348ce926e9a7921a8d8c467d500 */

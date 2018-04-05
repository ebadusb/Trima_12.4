/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      procrun_cds.h
 *
 */

#ifndef __PROCRUN_CDS_H_
#define __PROCRUN_CDS_H_

#include <time.h>
#include "datastore.h"
#include "carray.h"
#include "recoverytypes.h"
#include "run_defs.h"
#include "states.h"

#ifndef WIN32
#pragma interface
#endif


//
// Look at how the Vista interface uses these enums before making changes
//
enum RINSEBACK_OUTCOMES
{
   RINSEBACK_UNSUCCESSFUL,
   RINSEBACK_SUCCESSFUL_NORMAL,
   RINSEBACK_SUCCESSFUL_PLASMA,
   RINSEBACK_SUCCESSFUL_SALINE
};

enum WBCPURGE_TYPE
{
   WBCPURGE_SCHEDULED,
   WBCPURGE_UNSCHEDULED,
   WBCPURGE_NONE
};

enum MSS_USER_ABORT_TYPE
{
   MSS_USER_ABORT_TYPE_NONE,
   MSS_USER_ABORT_TYPE_ALL,
   MSS_USER_ABORT_TYPE_RAS,
   MSS_USER_ABORT_TYPE_PAS
};

enum PTF_STATUS // This is a bitmask.  Use it accordingly.
{
   PTF_STATUS_OK        =0,
   PTF_STATUS_P1_PLUGGED=1,
   PTF_STATUS_P2_PLUGGED=2
};

class ProcRun_CDS
   : public MultWriteDataStore
{
public:

   typedef carray<int, (size_t) RecoveryTypes::RecoveryNum> recovery_count_type;

public:

   BaseElement<State_names>         Substate;
   BaseElement<int>                 PredictRequest;

   BaseElement<bool>                EnteredCollect;
   BaseElement<bool>                DrawCycle;
   BaseElement<bool>                CycleSwitch;
   BaseElement<bool>                FirstDrawComplete;
   BaseElement<bool>                FirstCycleComplete;
   BaseElement<int>                 NumDraw;
   BaseElement<int>                 NumReturn;

   BaseElement<timespec>            StartPauseTime;
   BaseElement<time_t>              ProcEndRunTime;
   BaseElement<time_t>              ProcStartAasTime;

   BaseElement<int>                 NumRetTooLong;
   BaseElement<int>                 NumConsecRetTooLong;
   BaseElement<int>                 NumConsecDrawTooLong;
   BaseElement<bool>                DrawTooShortInPltCol;
   BaseElement<bool>                BalanceCyclesDisableWBCPLTFlag;
   BaseElement<bool>                DrawTooShortInPlsCol;
   BaseElement<bool>                DrawTooLongInRBCCol;
   BaseElement<bool>                DrawTooLongInRBCColProd2;

   BaseElement<float>               LastRunRatio;
   BaseElement<float>               LastRunInfusion;
   BaseElement<float>               IntegratedPlateletYield;
   BaseElement<float>               AdjustedHct;    // fraction, e.g. 0.43
   BaseElement<float>               Qchcurrent;
   BaseElement<float>               Qchmin;
   BaseElement<float>               Ydpur;
   BaseElement<int>                 NumberWBCPurges;
   BaseElement<int>                 NScheduledWBCPurges;
   BaseElement<WBCPURGE_TYPE>       WBCPurgeType;
   BaseElement<float>               LastProcPurge;

   BaseElement<recovery_count_type> RecoveryCountArray;

   BaseElement<bool>                SpilloverDuringPIR;
   BaseElement<bool>                SalineConnected;
   BaseElement<bool>                RinsebackStarted;
   BaseElement<RINSEBACK_OUTCOMES>  RinsebackComplete;

   BaseElement<bool>                ShowedPltWbcContamAlarm;

   BaseElement<int>                 Red;
   BaseElement<int>                 RedPl;
   BaseElement<int>                 Green;
   BaseElement<float>               RGr;
   BaseElement<float>               RGrMd;
   BaseElement<float>               dRGrMd;
   BaseElement<float>               Cr;
   BaseElement<float>               CrAv;
   BaseElement<float>               QchChamSat;
   BaseElement<float>               RGp;
   BaseElement<float>               RGpMd;
   BaseElement<float>               AvRGpMd;
   BaseElement<float>               RGpMdBaseline;
   BaseElement<float>               RGpMdBaselineA;
   BaseElement<float>               RGpMdBaselineB;
   BaseElement<int>                 Crit3Status;
   BaseElement<int>                 Crit3aStatus;
   BaseElement<int>                 Crit3bStatus;
   BaseElement<int>                 Crit4Status;
   BaseElement<int>                 Crit4aStatus;
   BaseElement<int>                 Crit4bStatus;

   BaseElement<bool>                PurgeDueToChamberSat;

   BaseElement<float>               procedureStartTime;        // procedure start time (minutes since midnight)

   BaseElement<bool>                MeterPlatelets_OK;     // these flags will mirror runTarget flags unless an alarm turns them false
   BaseElement<bool>                MeterRbcs_OK;
   BaseElement<bool>                PTFRBCprod1Disabled;     // these turn off the individual RBC collections on a PTF double.
   BaseElement<bool>                PTFRBCprod2Disabled;

   BaseElement<bool>                MeteringStarted;    // This flag indicates that metering (in some form) has been started.
   BaseElement<bool>                PltMeteringDone;    // these flags indicate that metering has been completed
   BaseElement<bool>                RBCMeteringDone;
   BaseElement<float>               PltSSVolumeFactor;    // used to adjust the PLT SS is PLT collection is off from target

   // MSS PFR
   BaseElement<State_names> currentMSSSubState;            // the substate MSS is in currently and should goto on PFR
   BaseElement<State_names> currentRBCMSSSubState;         // the substate RBC MSS is in currently during the parallel state process
   BaseElement<State_names> currentPLTMSSSubState;         // the substate PLT MSS is in currently during the parallel state process
   BaseElement<bool>        mssInPFR;

   // for vacuum purge remember cycle and phase for PFR
   BaseElement<int>                 mssVacCycle;
   BaseElement<int>                 mssVacPhase;
   BaseElement<float>               inletVolm;
   BaseElement<float>               channelInletVolm;
   BaseElement<float>               clampCloseRetVolm;
   BaseElement<float>               vacPurgePltVolm;
   BaseElement<float>               vacPurgePlsVolm;

   BaseElement<float>               hadPrimedReturnVolm; // used to bookmark both RAS&PAS priming
   BaseElement<float>               hadPrimedVolm;       // used to bookmark both RAS&PAS priming

   BaseElement<unsigned int>        CassetteFunctionBits;       // cassette functions
   BaseElement<int>                 CassetteRef;                // cassette REF
   BaseElement<bool>                cancelMSS;                  // disable MSS due to A2 error
   BaseElement<bool>                AllowMSSAfterA2SpecialCase; // A2 in rinseback (air to donor only)

   BaseElement<bool>                cancelJustRas;
   BaseElement<bool>                cancelJustPas;

   BaseElement<bool>                redoCassettePurge;     // RBC detector sees issue.. re-run vac purge for MSS
   BaseElement<bool>                pauseCassettePurge;    // RBC detector sees issue.. re-run vac purge for MSS
   BaseElement<bool>                cassettePurgeSetup;

   BaseElement<bool>                rinsebackEnteredMss;
   BaseElement<bool>                rinsebackVolumeSuccessMss;

   BaseElement<bool>                flagMSSPASVolumeError;
   BaseElement<bool>                flagMSSRASVolumeError;
   BaseElement<bool>                flagMSSPASConcetrationError;     // if due to switching proc our concentration preclude PAS flag
   BaseElement<int>                 RASAdditionSpeedReductionFactor; // reduce speed of addition for each APS alert

   BaseElement<bool>                SecondPLTSSBag;
   BaseElement<RecoveryTypes::Type> CurrentRecovery;
   BaseElement<RecoveryTypes::Type> PreviousRecovery;
   BaseElement<float>               VinClearLine;
   BaseElement<float>               WBCPurgeStartTime;

   BaseElement<bool>                stopCheckingRbcASBagEmpty;    // quit the Bag check if we are close.
   BaseElement<bool>                stopCheckingPltASBagEmpty;    // quit the Bag check if we are close.

   BaseElement<int>                 MssApsTooHighRBCProduct1;
   BaseElement<int>                 MssApsTooHighRBCProduct2;

   BaseElement<bool>                drbcSplitDone;
   BaseElement<bool>                PrePirRamp;                 // true if chamber is ramping prior to Pir
   BaseElement<bool>                mssDrbcSplitDone;

   BaseElement< float >             IRMax;
   BaseElement< float >             QReturnMax;

   BaseElement< bool >              wbcAlarmPending;


   BaseElement< float >               LastRbcQp;
   BaseElement< float >               DonorDisconnectTime; // absolute time of donor disco, in minutes (used in MSS)
   BaseElement< float >               MeteringDisconnectTime;

   BaseElement< float >               LastCommandedQp;

   BaseElement< float >               LastCentrifugeSpeed;
   BaseElement< float >               ActualPrimeDuration;
   BaseElement< bool >                SolutionsConnected;

   BaseElement< MSS_USER_ABORT_TYPE > MssUserAbortType;

   BaseElement< unsigned int >        PtfStatus;
   BaseElement< float >               SubstateStartTime;
   BaseElement< short >               MidrunEntered;
   BaseElement< short >               PreviousSubstate;

   // used to control the first return speed for PTF second collection IT9227
   BaseElement< bool > FinishedFirstRBCDoubleReturn;
   BaseElement< bool > FinishedFirstRBCReturn;

   // dont pause the valve during PAS purge... IT9818
   BaseElement< bool >  SkipPauseValveInMSSPurge;

   BaseElement< bool >  SalinePrimed;

   BaseElement< bool >  DoCPTPTFRecovery;

   BaseElement< float > PlsBagEvacVc;

   BaseElement<float>   PlsBagEvacVin_done; // black stamp only  used with rbcAirProcessing.cpp
   BaseElement<float>   PlsBagEvacVpls;     // black stamp only  used with rbcAirProcessing.cpp

   BaseElement< bool >  FlagforShortDrawNow;

   BaseElement<float>   ShowCompPlsSymbol;

   BaseElement< bool >  inPPR;

   BaseElement<float>   LatestQch;

   // AUTOFLOW CHANGES
   BaseElement< bool >  stopRamp;
   BaseElement< float > deadRampMaxQin;
   BaseElement<float>   AutoflowDeltaTime;
   BaseElement<float>   AutoflowTotalNetTimeChanges;
   BaseElement<int>     AutoflowTotal10MinAlarms;

// Class Methods

public:

   ProcRun_CDS (Role role);
   virtual ~ProcRun_CDS();

private:
   ProcRun_CDS();  // Base Constructor not available
};

#endif

/* FORMAT HASH 2b8454285e06fb2ba90dd45b062d0e10 */

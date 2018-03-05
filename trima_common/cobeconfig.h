#ifndef cobeconfig_h
#define cobeconfig_h

#include "alarms.h"
#include "datfile.h"
//
//  This class encapsulates the Cobe Configuration file.  It is designed
//  as a static class, i.e., there is only one Cobe config file per task
//  and each task has a single instance of the file.
//
//  At the start of task execution, it is the task's responsibility to
//  initialize() the Cobe configuration.  From that point on, any object
//  within the task can access a Cobe configuration value via a reference:
//     float vol = CobeConfig::data().ReservoirVolume + temp;
//
//  alternately, after the initialization, the task could create a singleton
//  data reference for use:
//     const CobeConfig::CobeConfigStruct &cobeConfig = CobeConfig::data();
//  at which point the object would reference the values like:
//     float vol = cobeConfig.ReservoirVolume + temp;
//
//
//  To add a value to the Cobe configuration file:
//    1: Change cobe_config.dat to include the value
//    2: Add the equivalent value to this structure
//    3: Add an initialization statement in initialize() in cobeconfig.cpp
//
class CobeConfig
{

public:
   struct CobeConfigStruct
   {
      // [SYSTEM]
      int   MinimumControlMemory;
      int   MinimumSafetyMemory;

      float MinimumPumpRPM;

      float RBCVolumeKluge;

      int   APSAutoZeroAverage;
      float APSAutoZeroRange;
      float APSAutoZeroMin;
      float APSAutoZeroMax;
      float APSAutoZeroMaxDiff;

      float ReservoirVolume;
      float DrawTooLargeWarning;
      float DrawTooLargeLimit;
      float DrawTooSmallWarning;
      float ReturnTooSmallWarning;
      float ReturnTooLargeWarning;
      float ReturnFirstCycleMargin;
      float ReservoirMixVolume;

      float DrawReturnDeltaForWBCflagging;

      float PltPlsRBCFirstCycleTooSoon;
      float PltPlsRBCFirstCycleTooLate;
      float PltPlsRBCExtracorporealVolume;
      float PltPlsRBCNominalFillVolume;
      float RBCPlsFirstCycleTooSoon;
      float RBCPlsFirstCycleTooLate;
      float RBCPlsExtracorporealVolume;
      float RBCPlsNominalFillVolume;

      float FirstCycleTooSoonPauseDoublecheck;
      float FirstCycleTooSoonRetryDoublecheck;

      float NominalReturnLineVolume;
      float LowLevelTooSoonLimit;
      float LowLevelTooLateLimit;
      float SampleVolume;
      float AKOVolume;
      float QrpAKO;
      float ACPrimeVolume;

      float FirstReturnNoCollectQnCap;

      float PltPlsRBCTotalResidualVolume;
      float RBCPlsTotalResidualVolume;

      float MaxCentStopCurrent;
      float MaxCentErrorTime;
      float CentShutdownTestDuration;

      float PredictTimeout;

      // [LRS]
      float CollectMaximumFlowHigh;
      float CollectMaximumFlowStandard;
      float FirstMaxPIRChamberFlowStandard;
      float FirstPIRFlowLimitStandard;
      float SecondMaxPIRChamberFlowStandard;
      float CollectMinimumFlow;
      float CollectRampRate;
      float SecondPIRFlowLimit;
      float PostPurgeWBCInChamber;
      float PostPurgeWBCInChamberLargeBMI;
      float PlasmaMinimumFlow;
      float BMIForIncreasedPurges;

      // [QIN]
      float QinLimitMin;
      float QrpLimitMin;
      float QinLimitMax;
      float QinFlowLimitSlow;
      float QinFlowLimitMedium;
      float QinFlowLimitFast;
      float QneedleLimitMax;
      float QneedleLimitMin;
      float MaxInstantQreturn;
      float TbvQinMaxCoefficient;
      float TbvQnretMaxCoefficient;
      float InletFlowRampConstant;
      float InletFlowRampMaxStartQin;
      float MaxQinPlasmaCollect;
      float MaxQinPC;
      float MaxQinPCAE;
      float MaxQinPCE;
      float MaxQchForQinDuringPC;
      float QnCoeffThirdOrder;
      float QnCoeffSecondOrder;
      float QnCoeffFirstOrder;
      float QnConst;
      float MaxQinPCA;



      // [VIN]
      float VinMaxTarget;
      float VinMinTarget;
      float VinPlasmaStart;
      float VinEndOfQinRamp;
      float VinCollectStartConstant;
      float MinVinCollectStart;
      float MaxVinCollectStart;
      float VinInterfaceSetup;

      // [PURGE]
      float PurgeLossPltInChamber;
      float PurgeLossMaxFractionYtarg;
      float PurgeScaleVariable;
      float PurgeMaxTotalVolume;
      float VinChamberPack;
      float QplasmaPurgeStd;
      float QplasmaPurgeHigh;
      float VplasmaPurgeCycle1Std;
      float VplasmaPurgeCycle1High;
      float VplasmaPurgeCycle2Std;
      float VplasmaPurgeCycle2High;
      float VinClearChannelStd;
      float VinClearChannelHigh;
      int   MinNumOfPurgeCyclesStd;
      int   MinNumOfPurgeCyclesHigh;
      float MinPurgeDistanceFromMidrun;
      float MinPurgeDistanceFromPir;
      // [CLEAR_LINE]
      float ClearLineVcollect;
      float SpillOverClearLineVcollect;
      float ClearLineQin;
      float ClearLineQplasma;
      float ClearLineQcollect;

      // [AIR_BLOCK]
      float AirBlockCentrifugeRPM;
      float AirBlockStartRPMTarget;
      float AirBlockQPlasma;
      float AirBlockQChamber;
      float AirBlockVPlasmaTarget;

      // [PUMPS_PAUSE]
      float PauseVinProcessedTarget;
      float PauseQPlasma;
      float PauseQCollect;

      // [CENTRIFUGE_SLOW]
      float CentSlowRPMTrigger;
      float CentSlowRPMTriggerScaledDelta;
      float CentSlowSystemStateTimeoutSeconds;
      float CentSlowRPMTargetScaledDelta;
      float CentSlowRPMTarget;
      float CentSlowInterfaceSetupTime;
      float CentSlowNumExpected;

      // [CENTRIFUGE_STOP]
      float CentStopRPMTrigger;
      float CentStopRPMTriggerScaled;
      float CentStopRPMTargetScaledDelta;
      float CentStopRPMTarget;
      float CentStopInterfaceSetupTime;
      float CentStopQplasma;
      float CentStopQcollect;
      float CentStopNumExpected;

      // [RBC_PURGE]
      float RBCPurgeQplasma;
      float RBCPurgeQcollect;
      float RBCPurgeVplasma;
      float RBCPurgeNumExpected;

      // [PLASMA_VALVE_MOTION]
      float PumpsStopWaitTime;

      // [SETTLE_CHANNEL]
      float SettleChannelQin;
      float SettleChannelVin;

      // [RECIRCULATE]
      float MinRecirculatePlateletCollect;
      float MaxRecirculatePlateletCollect;
      float RecirculatePcePreRbc;
      float RecirculateSnDrawVol;
      float PltPlsCrr;
      float RBCPlRecircConstant;

      // [RBC_PLASMA]
      float RbcPlasmaPrimeVolume;
      float RbcPlasmaPrimeTime;
      float MaxRbcPlasmaHrbc;
      float RbcPlasmaQinmaxr;
      float RbcPlasmaQmaxr;
      float RbcHctDelta;
      float MinimumIRAdjustmentRatio;
      float ApplyAdjustmentSafetyIR;
      float ApplyAdjustmentControlIR;
      float SafetyIRMultiplier;


      // [RBC_COLLECTION]
      float RBCQch;
      float MaxQinRbcCollection;

      // [WBC]
      float WBCAwSaturationConstant;
      float WBCBwSaturationConstant;
      float WBCChannelInventory;

      // [MNC]
      float MNCPrecountForPurge;
      float MNCPrecountForPurgeLargeBMI;
      float MNCPrecountForPltLoss;

      // [PIR]
      float PIRYtargLossNormal;
      float PIRYtargLossPreferPIR;

      float PIRVcAtMax;
      float PIRAllowableVcError;
      float PIRRampRateCoefficient;
      float PIRLinearRampQchLimit;
      float PIRLinearRampRate;
      float PIRQcdot;
      float PIRPercentOffload;

      // [PLATELET]
      float PlateletPlasmaPrimeVolume;
      float PlateletPlasmaPrimeTime;
      float MaxPlateletPlasmaHrbc;
      float PlateletSaturationAp;
      float PlateletSaturationBp;
      float ChannelPlateletInventory;
      float ChamberFlowAvgTime;
      float MaximumEpcaVolume;
      float EpcaChamberFlow;
      float PlateletDeltaVinYield;
      float MinimumCollectVolume;
      float PlateletMobilizationFactor;
      float PpcConcentrationThreshold;
      float TppYieldThreshold;

      // [CENTRIFUGE]
      float StandardLowCentrifugeRamp;
      float StandardHighCentrifugeRamp;
      float StandardCentrifugeRampTransition;
      float CentrifugeRampDownRate;
      float StopRecoveryLowCentrifugeRampUpRate;
      float CentrifugeMinCommandedRunSpeed;
      float CentrifugeOpenSpeed;
      float CentrifugePumpsPausedSpeed;
      float CentrifugeSpeedError;
      float CentrifugeSpeedErrorPrime;
      float CentResonantRPM;
      float CentPresLimitLow;
      float CentPresLimitHigh;
      float CentSpeedMax;

      float CsfMax;
      float CsfMin;
      float CsfQin;
      float CentRecoveryMaxVolume;


      // [DISPOSABLE_TEST]
      float EvacuationVolume;
      float EvacuationDeltaPressureLimit;
      float EvacuationAbsPressureLimit;
      float EvacuationStartupDelay;
      float EvacuationValveOpenDwell;
      float EvacuationReturnSpeed;
      float EvacuationPressureRecoveryLimit;
      float EvacuationRecoveryVolumeLimit;
      float DispTestVolumeFail;
      float DispTestVolumeCheck;
      float DispTestDeltaAPS;
      float DispTestAPSThreshold;
      float DispTestVolumeFail2;
      float DispTestReturnSpeed1;
      float DispTestSampleBagVolm;
      float DispTestApsMax;
      float DispTestApsMin;
      float DispTestCrossoverLow;
      float DispTestCrossoverHigh;
      float DispTestCrossoverDelta;
      float DispTestCrackpressureVolm;

      float DispTestTestSpinSpeed;
      float DispTestTestSpinTime;

      float EvacuationDesiredAps;
      float EvacuationGain;
      float EvacuationQreturnEvac;
      float EvacuationStartingQinSpeed;
      float EvacuationMinQinWatchVolume;
      float EvacuationEndQinDeltaOffSlowestSpeed;
      float EvacuationPlsEvacVc;
      float EvacuationMaxPltEvacLimit;
      float EvacuationBlackplsExtra;

      // [STARTUP]
      float PressureMeterMinLimit;
      float PressureMeterMaxLimit;

      float RPMBloodPrime;
      float QinBloodPrime;
      float QrpBloodPrime;
      float VinBloodPrimeInlet;

      float QrpStartup;
      float QinStartup;
      float CPSStartup;

      float QinPrimeChannel;
      float QcolPrimeChannel;
      float QpPrimeChannelPltPlsRBC;
      float QpPrimeChannelRBCPls;
      float RPMPrimeChannel;
      float VinPrimeChannel;

      float QinPrimeChannel2;
      float QcolPrimeChannel2;
      float QpPrimeChannel2;
      float RPMPrimeChannel2;
      float VinPrimeChannel2;

      float QinPrimeChannel2A;
      float QcolPrimeChannel2A;
      float QpPrimeChannel2A;
      float RPMPrimeChannel2A;
      float VinPrimeChannel2A;
      float HctTriggerChannel2A;

      float QinPrimeChannel3;
      float QcolPrimeChannel3;
      float QpPrimeChannel3;
      float RPMPrimeChannel3;
      float VinPrimeChannel3;

      float QinPrimeChannel4;
      float QcolPrimeChannel4;
      float QpPrimeChannel4;
      float RPMPrimeChannel4;
      float VinPrimeChannel4;

      float QinPrimeVent;
      float QcolPrimeVent;
      float QpPrimeVent;
      float RPMPrimeVent;
      float VinPrimeVent;

      float QinRampCentrifuge;
      float QcolRampCentrifuge;
      float QpRampCentrifuge;
      float RPMRampCentrifuge;

      float QinPrimeAirout1;
      float QcolPrimeAirout1;
      float QpPrimeAirout1;
      float RPMPrimeAirout1;

      float QinPrimeAirout2;
      float QcolPrimeAirout2;
      float QpPrimeAirout2;
      float RPMPrimeAirout2;
      float DeltaVinPrimeAirout2;

      float RbcPrimeSpilloverRisePercent;
      int   PrimeSpilloverSampleThreshold;
      int   MaxConsecutiveSpillovers;

      // [COLLECT]
      float RPMCollect;
      float DutyCycleLimit;
      float RBCDutyCycleLimit;
      float QneedleRampCoefficient;


      // [RINSEBACK]
      /////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////
      // Plasma Rinseback values
      /////////////////////////////////////////////////////////
      float RinsebackIrbmin;
      float RinsebackQchrb1;
      float RinsebackQchrb2;
      float RinsebackQcspill;
      float RinsebackQcpurge;
      float RinsebackQcrinse;
      float RinsebackQinrb;
      float RinsebackQinrec;
      float RinsebackQpm;
      float RinsebackQpmev;
      float RinsebackQpmrb1;
      float RinsebackQpmrb2;
      float RinsebackQrrec;
      float RinsebackQcdecel;
      float RinsebackVplsdecel;
      float RinsebackVcpurge;
      float RinsebackVcrb1;
      float RinsebackVcrinse1;
      float RinsebackVcrinse2;
      float RinsebackVinrec;
      float RinsebackVpevhold;
      float RinsebackVplrb;
      float RinsebackVpec;
      float RinsebackVpvalve;
      float RinsebackVr;
      float RinsebackVrpevmin;
      float RinsebackVs;
      float RinsebackVtrplt;
      float RinsebackVtrrbc;
      float RPMRinsebackSpillAndPurge;
      /////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////
      // Normal rinseback values
      /////////////////////////////////////////////////////////
      float RinsebackQinRecirculate;
      float RinsebackQchamberRecirculate;
      float RinsebackVpRecirculate;
      float RinsebackQcolMargin;
      float RinsebackQchamber;
      float RinsebackMinInfusion;
      float RinsebackVretLower;
      float RinsebackEvacuationHoldVolume;
      float RinsebackEvacuationMinVolume;
      float RinsebackValveVolume;
      /////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////
      float RPMRinsebackThreshold;

      // [YIELD]
      float YieldQinConstant;
      float YieldVinConstant;
      float PlateletEfficiencyMultiplier;
      float YieldStartupVolumeStd;                      // Vsu
      float YieldStartupVolumePPC;                      // Vsu
      float EfficiencyMinPostcount;
      float YieldPostPltConstant;
      float EfficiencyMinQinc;
      float YieldEfficiencyConstant;

      // [APS_PAUSE]
      float ApsPausePeriod;
      float ApsMaxPausesInPeriod;
      float ApsPauseTimeBeforeAlarm;
      float ApsMinimumPauseTime;
      float ApsNominalPressure;
      float ApsFlowReductionFactor;
      float ApsAdditionalDelay;
      float ApsPositivePressureCheck;

      // AUTOFLOW
      float AutoflowIncreaseTimer;
      float AutoflowIncreaseIncrement;
      float AutoflowDecreaseIncrement;
      float AutoflowMinUpTime;





      // [HYPOVOLEMIA]
      float HypovolemiaFCVwhite;
      float HypovolemiaFCVblack;

      // [VERIFY_FLAGS]

      // Maximum platelet concentration before a HIGH_PLATELET_CONCENTRATION reason
      // to verify platelets is declared.
      float ConcMax;

      // Minimum LRS chamber flow before a HIGH_PLATELET_CONCENTRATION
      // or LOW_CHAMBER_FLOW reason to verify platelets is declared.
      float Qch1;

      // Platelet collecting time after which to start checking for high
      // donor hematocrit or an excessive change in donor hematocrit.
      float Tss1;

      // Minimum donor hematocrit change that will trigger a HIGH_DELTA_HCT
      // reason to verify platelets.
      float Hcmin;

      // Donor hematocrit above which a HIGH_HCT reason to verify platelets is declared.
      float HCThigh;

      // Milliliters of deviation from the target that the actual platelet volume is allowed
      // before a PLATELET_VOLUME_ERROR reason is declared.
      float Vplt1;

      // Milliliters of deviation from the target that the actual plasma volume is allowed
      // before a PLASMA_VOLUME_ERROR reason is declared.
      float Vplasma1;

      // Milliliters of deviation from the target that the actual RBC volume is allowed
      // before a RBC_VOLUME_ERROR reason is declared.
      float Vrbc1;

      // Milliliters of Inlet that must pass after a "disturbance" before which
      // Steady State Platelet collection is assumed to be reestablished.
      float VinSS;

      // [RETURN_OCCLUSION]
      // see comments in cobe_config.dat and retoccmon.h/.cpp
      float RetOccStabilizationDelay;
      float RetOccAlarmPowerLimit;
      int   RetOccAlarmType;
      float RetOccInletFirstHarmonicScale;
      float RetOccInletSecondHarmonicScale;

      // [CHAMBER_SATURATION]
      float ChamberSatVs3a;
      float ChamberSatTs3b;
      float ChamberSatTs3aAPS;
      float ChamberSatTs3bAPS;
      float ChamberSatTs4APS;
      float ChamberSatFs3a;
      float ChamberSatFs3b;
      float ChamberSatDur_3a;
      float ChamberSatdRGrMd_3a;
      float ChamberSatDur_3b;
      float ChamberSatdRGrMd_3b;
      float ChamberSatRgpMd_4a;
      float ChamberSatRgpMd_4b;

      float CalculationInterval;
      float ActivationTime;
      float MinDeltaChamberFlow;
      float MinChamberFlow;
      float MinRed;
      float MaxRed;
      float MinUpdateInterval;
      float OneMin_Msecs;
      float ThreeMinMsecs;
      float PostPurgeWait;
      float MinPltConcentrationJapan;
      float MinPltConcentration;
      float MinDeltaCentSpeed;
      float MinCentSpeed;
      float VinStartc4a;
      float VinEndc4b;
      float MinBaseline;


      // [PLASMA_LINE_OCCLUSION]
      int PLOebsmin;
      int PLOebsmax;
      int PLOesammin;
      int CVedmin;
      int CVedmax;
      int Rdropthresh;
      int Gdropthresh;


      // [SALINE_PRIME]
      float SalinePrimeSpeed;
      float SalinePrimevolume;
      float SalinePrimeVinPoint;

      // [PTF]
      float PtfRbcPlsSetup1Volume;
      float PtfRbcPlsSetup1Hrbc;

      float PtfRbcPlsSetup2Volume;


      float PtfRbcPlsSetup1BVolume;
      float PtfRbcPlsSetup1BHrbc;

      float PtfRbcPlsSetup2BVolume;
      float PtfRbcPlsSetup2BPlasmaRampVolume;


      float PtfPltPlsRbcSetup1Volume;
      float PtfPltPlsRbcSetup1Hrbc;

      float PtfPltPlsRbcSetup2Volume;
      float PtfPltPlsRbcSetup2Hrbc;

      float PtfPltPlsRbcPrimeVolume;
      float PtfPltPlsRbcPrimeHrbc;
      float PtfPltPlsRbcPrimeQrbc;

      float PtfHctScalingFactor;

      float PtfQrbc;
      float PtfSetupReturnPumpMultiple;
      float PtfSetupReturnMax;


      float PTFCPSLoadHigh;
      float PTFCPSLoadLow;

      float PTFCPSTestChange;
      float PTFVrbcCollectPoint;
      float PTFRBCInitialCPSLimit;
      float PTFCPSRBCCollectDelta;
      float PTFCPSRBCCollectLimit;
      float PTFVrbcCollectCPSNocount;

      float PTFCPSAlertsPerProductCollecting;
      float PTFAPSAlertsPerProductMetering;



      // [MSS]
      float MssApsAlertRecoveryQin;
      float MssMinPltVolume;
      float MssCpsHigh;
      float MssApsBackpressure;
      float MssCassetteVolMax;
      float MssApsHigh;
      float MssPltSubstateMinPrimeVol;
      float MssPltSubstateMaxPrimeVol;
      float MssPltSubstateSalvageVol;
      float MssVacuumCycles;
      float MssPltAddSpeed;
      int   MssPltSSAddRampup;
      float MssCvlWhite;
      float MssCvlBlack;
      float MssCvlSalvage;
      float MssPrimeSpeedPercent;
      float MssPLTPrimeSpeed;
      float MssPLTValveTimer;
      float MssPrimeReturnVol;
      float MssPLTPrimeFinalSpeed;
      float MssPLTPrimeFinalVol;
      float MssChannelCheck1CollectSpeed;
      float MssChannelCheck1CollectVol;
      float MssChannelCheck2CollectSpeed;
      float MssChannelCheck2PlasmaSpeed;
      float MssChannelCheck2PlasmaVol;

      // Mss Stroke volm const
      float MssACSecondOrderConst;
      float MssACLinearConst;
      float MssACOffset;

      float MssPltSecondOrderConst;
      float MssPltLinearConst;
      float MssPltOffset;

      float MssPlsSecondOrderConst;
      float MssPlsLinearConst;
      float MssPlsOffset;

      // vacuum purge
      float MssVac1CollectSpeed;
      float MssVac1PlasmaSpeed;
      float MssVac2CollectSpeed;
      float MssVac2PlasmaSpeed;
      float MssVac3CollectSpeed;
      float MssVac3PlasmaSpeed;
      float MssVac4CollectSpeed;
      float MssVac4PlasmaSpeed;
      float MssVac1CollectVol;
      float MssVac1PlasmaVol;
      float MssVac2PlasmaVol;
      float MssVac3PlasmaVol;
      float MssVac4PlasmaVol;
      float MssVacPurgeCyclePause;

      float MssPostPurgeFillPlasmaSpeed;
      float MssPostPurgeFillCollectSpeed;
      float MssPostPurgeFillPlasmaVolume;

      float MssPltSSPercent; // % of PAS bag to bias before triggering PAS empty alarm
      float MssRbcSSPercent; // % of RAS bag to bias before triggering RAS empty alarm

      // MssRBC monitor
      float MssRGratio;
      float MssTriggerSamples;
      float mssConnectionSamples;
      float MssRbcDetectorZsamples;
      int   MssCheckLimit;

      // determine if rinseback has emptied enough volume for regular MSS
      float MssChannelVolume_w;
      float MssRinsebackSuccess_w;
      float MssChannelVolume_b;
      float MssRinsebackSuccess_b;
      float MssPlasmaRinsebackVolume;
      float MssSalineRinsebackVolume;




      float MssRbcSubstateAddAcSpeed;
      float MssRbcSubstateAddPrime1Speed;
      float MssRbcSubstateAddPrime2Speed;
      float MssRbcSubstateAddPrime3Speed;
      float MssRbcSubstateAddMeterSpeed;
      float MssRbcSubstateAddPrime1Vol;
      float MssRbcSubstateAddPrime2Vol;
      float MssRbcSubstateAddPrime3Vol;
      float MssRbcSubstateMinPrimeVol;
      float MssRbcSubstateMaxPrimeVol;
      float MssRbcSubstateSalvageVol;
      float MssPtfVolume;
      float MssPtfNumber;
      float MssACprimePause;
      float MssPtfApsHigh;
      float MssRBCPrimeSpeed;

      // verify closure test
      float MssVerifyClosureAPSDelta;
      float MssVerifyClosureMaxVol;
      float MssVerifyClosureInletSpeed;
      float MssVerifyClosureACSpeed;
      float MssVerifyClosureACPresureUpSpeed;

      float MssReturnInletMatchSpeed;

      // use to check if the second SS bag is connected.
      float mssConnectionDeltaRGmax;
      float mssConnectionRGmaxVol;

      float mssPtfFilterLoss;


      float MssReminderChimeTime;



      // [CONVERGENCE]
      int   NumIterationsMidrunEpca;
      int   NumIterationsQcmrHoldStd;
      int   NumIterationsQcmrHoldPpc;
      int   NumIterationsDeadband;
      int   ApsSlowDisable;
      float CollectVolErr;
      float QchConvergenceFloor;
      int   QchDropSwitch;
      float QchDropLimitPPC;
      float QchDropLimitStd;
      float QchIterationChange;



      // [PRIME HEMOLYSIS]
      float PrimeHemolysisRGTrigger;
      float PrimeHemolysisRGReturnLevel;
      float PrimeHemolysisGreenReturnLimit;

      // [SALINE PRIME]
      float SrRecirc1Qin;
      float SrRecirc1Qp;
      float SrRecirc1Mod;
      float SrRecirc1Vr;
      float SrRecirc1Vin;

      float SrEvac1Qp;
      float SrEvac1VrLimit;
      float SrEvac1VpLimit;

      float SrDrawVr;

      float SrRecirc2Qin;
      float SrRecirc2Qp;
      float SrRecirc2Qr;
      float SrRecirc2Vin;

      float SrEvac2Qp;
      float SrEvac2VrLimit;
      float SrEvac2VpLimit;

      float SrFlushVr;

      float SrEvac1MinVp;
      float SrEvac1MaxVp;

      float SrEvac2MinVp;
      float SrEvac2MaxVp;
      float SrTotalResidualVolume;
      float SrMaxReturnVolumeBuffer;


      float ResPltPlsRbc_NormalRB_RBC;  // m5
      float ResPltPlsRbc_NormalRB_PLS;  // m6
      float ResPltPlsRbc_NoRB_RBC;      // m7
      float ResPltPlsRbc_NoRB_PLS;      // m8
      float ResPltPlsRbc_PlasmaRB_RBC;  // m9
      float ResPltPlsRbc_PlasmaRB_PLS;  // m10

      float ResPlsRbc_NormalRB_RBC;     // m11
      float ResPlsRbc_NormalRB_PLS;     // m12
      float ResPlsRbc_NoRB_RBC;         // m13
      float ResPlsRbc_NoRB_PLS;         // m14
      float ResPlsRbc_SalineRB_RBC;     // m15
      float ResPlsRbc_SalineRB_PLS;     // m16


      // [DISPOSABLE_CONSTRAINTS]
      float MaxPlateletBagVolume;

      // [PUMP_HW]
      float PlasmaLargeCmdRatio;

      // [AMAP_PLATELETS]
      float YieldStepSize;


      // [PLASMA_LINE_OCCLUSION_PCA]
      float CVPdMin_PLO_PCA;
      float CVPdMax_PLO_PCA;
      float RDropThresh_PLO_PCA;
      float PlasmaTargetMinimum_PLO_PCA;


      // todo
      // float HypovolemiaAMAPPLTMargin;

   };


public:
   //
   //
   //   When you initialize me I will read in the Cobe configuration file
   //   and report back with an alarm value if something goes wrong.  I do
   //   not actually raise an alarm.  If everything initializes properly
   //   I return NULL_ALARM.
   //
   static ALARM_VALUES            initialize (const int logError = 0);
   static const CobeConfigStruct& data ();

   static ALARM_VALUES ReadConfig (CDatFileReader& config);
   static ALARM_VALUES initialize (const char* const path, const int logError = 0); // for offline use
protected:
   //
   //
   //   My static singleton instance of the cobe configuration data
   //
   static CobeConfigStruct _data;
   static bool             _initialized;

//
//
//   These methods are currently protected since we do not expect
//   anyone to actually instantiate a CobeConfig class.
//
protected:
   CobeConfig();
   virtual ~CobeConfig();

};
#endif

/* FORMAT HASH f6c9d495ef99aaf4cca03d7641d5cb58 */

#include <vxworks.h>
#include <cobeconfig.h>
#include <datfile.h>
#include <filenames.h>
#include <messagesystem.h>
#include <an_alarm.h>


//
// -----------------------------------------------------------------------------
//                      Constructor, destructor
//
//
CobeConfig::CobeConfig()
{}

CobeConfig::~CobeConfig()
{}

//
// -----------------------------------------------------------------------------
//
//   Return a constant reference to my data
//
const CobeConfig::CobeConfigStruct& CobeConfig::data ()
{
   if (!_initialized)
   {
      initialize(); // but ignore status
   }
   return _data;
}

//
// -----------------------------------------------------------------------------
// for offline use

ALARM_VALUES CobeConfig::initialize (const char* const path, const int logError)
{
   CDatFileReader config;
   if (config.Initialize(path, logError) != DFR_OK)
   {
      return COBE_CONFIG_BAD_FILE;
   }
   return CobeConfig::ReadConfig(config);
}

//
// -----------------------------------------------------------------------------
//
ALARM_VALUES CobeConfig::initialize (const int logError)
{
#ifndef _NONTRIMA
   //
   //
   // Create the dat file reader to retrieve the Cobe configuration data.
   //
   CDatFileReader config;

   if (config.Initialize(PNAME_COBECONFIGDAT, logError) != DFR_OK)
   {
      return COBE_CONFIG_BAD_FILE;
   }
   return CobeConfig::ReadConfig(config);
#else
   UNUSED_ARGUMENT(logError);
   return NULL_ALARM;
#endif
}
//
// -----------------------------------------------------------------------------
//
ALARM_VALUES CobeConfig::ReadConfig (CDatFileReader& config)
{
   ALARM_VALUES status = NULL_ALARM;  // default
   // [SYSTEM]
   _data.MinimumControlMemory   = config.get_int("SYSTEM", "control_minimum_memory", 1024, 1048576);
   _data.MinimumSafetyMemory    = config.get_int("SYSTEM", "safety_minimum_memory", 1024, 1048576);

   _data.MinimumPumpRPM         = config.get_float("SYSTEM", "minimum_pump_rpm", 0.0f, 5.0f);

   _data.RBCVolumeKluge         = config.get_float("SYSTEM", "rbc_volume_kluge", 0.0f, 2.0f);

   _data.APSAutoZeroAverage     = config.get_int("SYSTEM", "aps_auto_zero_average", 1, 10);
   _data.APSAutoZeroRange       = config.get_float("SYSTEM", "aps_auto_zero_range", 0.0f, 200.0f);
   _data.APSAutoZeroMin         = config.get_float("SYSTEM", "aps_auto_zero_min", -500.0f, 0.0f);
   _data.APSAutoZeroMax         = config.get_float("SYSTEM", "aps_auto_zero_max", 0.0f, 500.0f);
   _data.APSAutoZeroMaxDiff     = config.get_float("SYSTEM", "aps_auto_zero_max_diff", 0.0f, 500.0f);

   _data.ReservoirVolume        = config.get_float("SYSTEM", "reservoir_volume", 35.0f, 70.0f);
   _data.DrawTooLargeWarning    = config.get_float("SYSTEM", "draw_too_large_warning", 35.0f, 80.0f);
   _data.DrawTooLargeLimit      = config.get_float("SYSTEM", "draw_too_large_limit", 80.0f, 160.0f);
   _data.DrawTooSmallWarning    = config.get_float("SYSTEM", "draw_too_small_warning", 25.0f, 65.0f);
   _data.ReturnTooSmallWarning  = config.get_float("SYSTEM", "return_too_small_warning", -65.0f, -25.0f);
   _data.ReturnTooLargeWarning  = config.get_float("SYSTEM", "return_too_large_warning", -85.0f, -35.0f);
   _data.ReturnFirstCycleMargin = config.get_float("SYSTEM", "return_first_cycle_margin", 0.0f, 50.0f);
   _data.ReservoirMixVolume     = config.get_float("SYSTEM", "reservoir_mix_volume", 0.0f, 50.0f);


   _data.DrawReturnDeltaForWBCflagging = config.get_float("SYSTEM", "draw_return_delta_for_wbc_flag", 0.0f, 10.0f);


   _data.PltPlsRBCFirstCycleTooSoon        = config.get_float("SYSTEM", "pltplsrbc_first_cycle_too_soon", 0.0f, 500.0f);
   _data.PltPlsRBCFirstCycleTooLate        = config.get_float("SYSTEM", "pltplsrbc_first_cycle_too_late", 0.0f, 500.0f);
   _data.PltPlsRBCExtracorporealVolume     = config.get_float("SYSTEM", "pltplsrbc_extracorporeal_volume", 150.0f, 300.0f);
   _data.PltPlsRBCNominalFillVolume        = config.get_float("SYSTEM", "pltplsrbc_nominal_fill_volume", 0.0f, 500.0f);
   _data.RBCPlsFirstCycleTooSoon           = config.get_float("SYSTEM", "rbcpls_first_cycle_too_soon", 0.0f, 500.0f);
   _data.RBCPlsFirstCycleTooLate           = config.get_float("SYSTEM", "rbcpls_first_cycle_too_late", 0.0f, 500.0f);
   _data.RBCPlsExtracorporealVolume        = config.get_float("SYSTEM", "rbcpls_extracorporeal_volume", 100.0f, 300.0f);
   _data.RBCPlsNominalFillVolume           = config.get_float("SYSTEM", "rbcpls_nominal_fill_volume", 0.0f, 500.0f);

   _data.NominalReturnLineVolume           = config.get_float("SYSTEM", "nominal_return_line_volume", 0.0f, 500.0f);
   _data.LowLevelTooSoonLimit              = config.get_float("SYSTEM", "low_level_too_soon_limit", -80.0f, 0.0f);
   _data.LowLevelTooLateLimit              = config.get_float("SYSTEM", "low_level_too_late_limit", -80.0f, 0.0f);
   _data.SampleVolume                      = config.get_float("SYSTEM", "sample_volume", 0.0f, 100.0f);
   _data.AKOVolume                         = config.get_float("SYSTEM", "ako_volume", 0.0f, 20.0f);
   _data.QrpAKO                            = config.get_float("SYSTEM", "qrp_ako", 10.0f, 200.0f);
   _data.ACPrimeVolume                     = config.get_float("SYSTEM", "ac_prime_volume", 0.0f, 100.0f);

   _data.FirstCycleTooSoonPauseDoublecheck = config.get_float("SYSTEM", "first_cycle_too_soon_pause_doublecheck", 0.0f, 8000.0f);
   _data.FirstCycleTooSoonRetryDoublecheck = config.get_float("SYSTEM", "first_cycle_too_soon_retry_doublecheck", 0.0f, 8.0f);

   _data.PltPlsRBCTotalResidualVolume      = config.get_float("SYSTEM", "pltplsrbc_total_residual_volume", 0.0f, 500.0f);
   _data.RBCPlsTotalResidualVolume         = config.get_float("SYSTEM", "rbcpls_total_residual_volume", 0.0f, 500.0f);

   _data.MaxCentStopCurrent                = config.get_float("SYSTEM", "maximum_centrifuge_stop_current", 0.0f, 100.0f);
   _data.MaxCentErrorTime                  = config.get_float("SYSTEM", "maximum_centrifuge_error_time", 0.0f, 10.0f);
   _data.CentShutdownTestDuration          = config.get_float("SYSTEM", "centrifuge_shutdown_test_duration", 0.0f, 10.0f);

   _data.PredictTimeout                    = config.get_float("SYSTEM", "predict_timeout", 0.0f, 600.0f);

   _data.FirstReturnNoCollectQnCap         = config.get_float("SYSTEM", "first_return_nocollect_qn_cap", 0.0f, 500.0f);

   // [LRS]
   _data.CollectMinimumFlow              = config.get_float("LRS", "collect_minimum_flow", 0.1f, 25.0f);
   _data.CollectMaximumFlowHigh          = config.get_float("LRS", "collect_maximum_flow_high", 0.1f, 25.0f);
   _data.CollectMaximumFlowStandard      = config.get_float("LRS", "collect_maximum_flow_standard", 0.1f, 25.0f);
   _data.CollectRampRate                 = config.get_float("LRS", "collect_ramp_rate", 0.0f, 25.0f);
   _data.FirstMaxPIRChamberFlowStandard  = config.get_float("LRS", "first_max_pir_chamber_flow_standard", 1.0f, 10.0f);
   _data.FirstPIRFlowLimitStandard       = config.get_float("LRS", "first_pir_flow_limit_standard", 0.0f, 5.0f);
   _data.SecondMaxPIRChamberFlowStandard = config.get_float("LRS", "second_max_pir_chamber_flow_standard", 1.0f, 10.0f);
   _data.SecondPIRFlowLimit              = config.get_float("LRS", "second_pir_flow_limit", 0.0f, 5.0f);
   _data.PostPurgeWBCInChamber           = config.get_float("LRS", "post_purge_wbc_in_chamber", 0.0f, 10.0f);
   _data.PostPurgeWBCInChamberLargeBMI   = config.get_float("LRS", "post_purge_wbc_in_chamber_large_bmi", 0.0f, 10.0f);
   _data.PlasmaMinimumFlow               = config.get_float("LRS", "plasma_minimum_flow", 0.1f, 25.0f);
   _data.BMIForIncreasedPurges           = config.get_float("LRS", "bmi_for_increased_purges", 0.0f, 100.0f);

   // [QIN]
   _data.QinLimitMin              = config.get_float("QIN", "qin_limit_min", 10.0f, 200.0f);
   _data.QrpLimitMin              = config.get_float("QIN", "qrp_limit_min", 10.0f, 200.0f);
   _data.QinLimitMax              = config.get_float("QIN", "qin_limit_max", 10.0f, 200.0f);
   _data.QinFlowLimitSlow         = config.get_float("QIN", "qin_flow_limit_slow", 10.0f, 200.0f);
   _data.QinFlowLimitMedium       = config.get_float("QIN", "qin_flow_limit_medium", 10.0f, 200.0f);
   _data.QinFlowLimitFast         = config.get_float("QIN", "qin_flow_limit_fast", 10.0f, 200.0f);
   _data.QneedleLimitMax          = config.get_float("QIN", "qneedle_limit_max", 10.0f, 500.0f);
   _data.QneedleLimitMin          = config.get_float("QIN", "qneedle_limit_min", 10.0f, 500.0f);
   _data.MaxInstantQreturn        = config.get_float("QIN", "max_instant_qreturn", 10.0f, 500.0f);
   _data.TbvQinMaxCoefficient     = config.get_float("QIN", "tbv_qin_max_coefficient", 0.01f, 0.05f);
   _data.TbvQnretMaxCoefficient   = config.get_float("QIN", "tbv_qnret_max_coefficient", 0.05f, 0.20f);
   _data.InletFlowRampConstant    = config.get_float("QIN", "inlet_flow_ramp_constant", 0.0f, 5.0f);
   _data.InletFlowRampMaxStartQin = config.get_float("QIN", "inlet_flow_ramp_max_start_qin", 0.0f, 142.0f);
   _data.MaxQinPlasmaCollect      = config.get_float("QIN", "max_qin_plasma_collect", 0.0f, 142.0f);
   _data.MaxQinPC                 = config.get_float("QIN", "max_qin_pc", 0.0f, 142.0f);
   _data.MaxQinPCAE               = config.get_float("QIN", "max_qin_pcae", 0.0f, 142.0f);
   _data.MaxQinPCE                = config.get_float("QIN", "max_qin_pce", 0.0f, 142.0f);
   _data.MaxQchForQinDuringPC     = config.get_float("QIN", "max_qch_for_qin_during_pc", 0.0f, 5.0f);
   _data.QnCoeffThirdOrder        = config.get_float("QIN", "qn_coeff_third_order", -5.0f, 5.0f);
   _data.QnCoeffSecondOrder       = config.get_float("QIN", "qn_coeff_second_order", -5.0f, 5.0f);
   _data.QnCoeffFirstOrder        = config.get_float("QIN", "qn_coeff_first_order", -5.0f, 5.0f);
   _data.QnConst                  = config.get_float("QIN", "qn_const", 50.0f, 350.0f);
   _data.MaxQinPCA                = config.get_float("QIN", "max_qin_pca", 0.0f, 142.0f);
   _data.QnCoeffThirdOrder        = config.get_float("QIN", "qn_coeff_third_order", -5.0f, 5.0f);
   _data.QnCoeffSecondOrder       = config.get_float("QIN", "qn_coeff_second_order", -5.0f, 5.0f);
   _data.QnCoeffFirstOrder        = config.get_float("QIN", "qn_coeff_first_order", -5.0f, 5.0f);
   _data.QnConst                  = config.get_float("QIN", "qn_const", 50.0f, 350.0f);



   // [VIN]
   _data.VinMaxTarget            = config.get_float("VIN", "vin_max_target", 0.0f, 25000.0f);
   _data.VinMinTarget            = config.get_float("VIN", "vin_min_target", 0.0f, 5000.0f);
   _data.VinPlasmaStart          = config.get_float("VIN", "vin_plasma_start", 0.0f, 5000.0f);
   _data.VinEndOfQinRamp         = config.get_float("VIN", "vin_end_of_qin_ramp", 100.0f, 5000.0f);
   _data.VinCollectStartConstant = config.get_float("VIN", "vin_collect_start_constant", 0.0f, 5.0f);
   _data.MaxVinCollectStart      = config.get_float("VIN", "max_vin_collect_start", 300.0f, 3000.0f);
   _data.MinVinCollectStart      = config.get_float("VIN", "min_vin_collect_start", 100.0f, 3000.0f);
   _data.VinInterfaceSetup       = config.get_float("VIN", "vin_interface_setup", 50.0f, 300.0f);

   // [PURGE]
   _data.PurgeLossPltInChamber      = config.get_float("PURGE", "purge_loss_plt_in_chamber", 0.0f, 2.0f);
   _data.PurgeLossMaxFractionYtarg  = config.get_float("PURGE", "purge_loss_max_fraction_ytarg", 0.0f, 2.0f);
   _data.PurgeScaleVariable         = config.get_float("PURGE", "purge_scale_variable", 0.0f, 4.0f);
   _data.PurgeMaxTotalVolume        = config.get_float("PURGE", "purge_max_total_volume", 0.0f, 10.0f);
   _data.VinChamberPack             = config.get_float("PURGE", "vin_chamber_pack", 0.0f, 100.0f);
   _data.QplasmaPurgeStd            = config.get_float("PURGE", "qplasma_purge_std", 0.0f, 100.0f);
   _data.QplasmaPurgeHigh           = config.get_float("PURGE", "qplasma_purge_high", 0.0f, 100.0f);
   _data.VplasmaPurgeCycle1Std      = config.get_float("PURGE", "vplasma_purge_cycle1_std", 0.0f, 20.0f);
   _data.VplasmaPurgeCycle1High     = config.get_float("PURGE", "vplasma_purge_cycle1_high", 0.0f, 20.0f);
   _data.VplasmaPurgeCycle2Std      = config.get_float("PURGE", "vplasma_purge_cycle2_std", 0.0f, 20.0f);
   _data.VplasmaPurgeCycle2High     = config.get_float("PURGE", "vplasma_purge_cycle2_high", 0.0f, 20.0f);
   _data.VinClearChannelStd         = config.get_float("PURGE", "vin_clear_channel_std", 0.0f, 100.0f);
   _data.VinClearChannelHigh        = config.get_float("PURGE", "vin_clear_channel_high", 0.0f, 100.0f);
   _data.MinNumOfPurgeCyclesStd     = config.get_int("PURGE", "min_num_of_purge_cycles_std", 0, 20);
   _data.MinNumOfPurgeCyclesHigh    = config.get_int("PURGE", "min_num_of_purge_cycles_high", 0, 20);

   _data.MinPurgeDistanceFromMidrun = config.get_float("PURGE", "min_purge_distance_from_midrun", 0.0f, 1500.0f);
   _data.MinPurgeDistanceFromPir    = config.get_float("PURGE", "min_purge_distance_from_pir", 0.0f, 2500.0f);

   // [CLEAR_LINE]
   _data.SpillOverClearLineVcollect = config.get_float("CLEAR_LINE", "spillover_clear_line_vcollect", 0.0f, 80.0f);
   _data.ClearLineVcollect          = config.get_float("CLEAR_LINE", "clear_line_vcollect", 0.0f, 80.0f);
   _data.ClearLineQin               = config.get_float("CLEAR_LINE", "clear_line_qin",      10.0f, 200.0f);
   _data.ClearLineQplasma           = config.get_float("CLEAR_LINE", "clear_line_qplasma",  0.0f, 200.0f);
   _data.ClearLineQcollect          = config.get_float("CLEAR_LINE", "clear_line_qcollect", 0.0f, 200.0f);

   // [AIR_BLOCK]
   _data.AirBlockCentrifugeRPM  = config.get_float("AIR_BLOCK", "air_block_centrifuge_rpm", 0.0f, 3000.0f);
   _data.AirBlockStartRPMTarget = config.get_float("AIR_BLOCK", "air_block_start_rpm_target", 0.0f, 3000.0f);
   _data.AirBlockQPlasma        = config.get_float("AIR_BLOCK", "air_block_qplasma", 0.0f, 100.0f);
   _data.AirBlockQChamber       = config.get_float("AIR_BLOCK", "air_block_qchamber", 0.0f, 100.0f);
   _data.AirBlockVPlasmaTarget  = config.get_float("AIR_BLOCK", "air_block_vplasma_target", 0.0f, 100.0f);

   // [PUMPS_PAUSE]
   _data.PauseVinProcessedTarget = config.get_float("PUMPS_PAUSE", "pause_vin_processed_target", 0.0f, 100.0f);
   _data.PauseQPlasma            = config.get_float("PUMPS_PAUSE", "pause_qplasma", 0.0f, 100.0f);
   _data.PauseQCollect           = config.get_float("PUMPS_PAUSE", "pause_qcollect", 0.0f, 100.0f);

   // [CENTRIFUGE_SLOW]
   _data.CentSlowRPMTriggerScaledDelta     = config.get_float("CENTRIFUGE_SLOW", "centslow_rpm_trigger_scaled_delta", 0.0f, 500.0f);
   _data.CentSlowSystemStateTimeoutSeconds = config.get_float("CENTRIFUGE_SLOW", "centslow_system_state_timeout_seconds", 0.0f, 30.0f);
   _data.CentSlowRPMTrigger                = config.get_float("CENTRIFUGE_SLOW", "centslow_rpm_trigger", 0.0f, 3000.0f);
   _data.CentSlowRPMTargetScaledDelta      = config.get_float("CENTRIFUGE_SLOW", "centslow_rpm_target_scaled_delta", 0.0f, 500.0f);
   _data.CentSlowRPMTarget                 = config.get_float("CENTRIFUGE_SLOW", "centslow_rpm_target", 0.0f, 3000.0f);
   _data.CentSlowInterfaceSetupTime        = config.get_float("CENTRIFUGE_SLOW", "centslow_interface_setup_time", 0.0f, 60.0f);
   _data.CentSlowNumExpected               = config.get_float("CENTRIFUGE_SLOW", "centslow_num_expected", 0.0f, 10.0f);


   // [CENTRIFUGE_STOP]
   _data.CentStopRPMTriggerScaled     = config.get_float("CENTRIFUGE_STOP", "centstop_rpm_trigger_scaled", 0.0f, 3000.0f);
   _data.CentStopRPMTrigger           = config.get_float("CENTRIFUGE_STOP", "centstop_rpm_trigger", 0.0f, 3000.0f);
   _data.CentStopRPMTargetScaledDelta = config.get_float("CENTRIFUGE_STOP", "centslow_rpm_target_scaled_delta", 0.0f, 500.0f);
   _data.CentStopRPMTarget            = config.get_float("CENTRIFUGE_STOP", "centstop_rpm_target", 0.0f, 3000.0f);
   _data.CentStopInterfaceSetupTime   = config.get_float("CENTRIFUGE_STOP", "centstop_interface_setup_time", 0.0f, 60.0f);
   _data.CentStopQplasma              = config.get_float("CENTRIFUGE_STOP", "centstop_qplasma", 0.0f, 100.0f);
   _data.CentStopQcollect             = config.get_float("CENTRIFUGE_STOP", "centstop_qcollect", 0.0f, 100.0f);
   _data.CentStopNumExpected          = config.get_float("CENTRIFUGE_STOP", "centstop_num_expected", 0.0f, 10.0f);



   // [RBC_PURGE]
   _data.RBCPurgeQplasma     = config.get_float("RBC_PURGE", "rbcpurge_qplasma", 0.0f, 100.0f);
   _data.RBCPurgeQcollect    = config.get_float("RBC_PURGE", "rbcpurge_qcollect", 0.0f, 100.0f);
   _data.RBCPurgeVplasma     = config.get_float("RBC_PURGE", "rbcpurge_vplasma", 0.0f, 10.0f);
   _data.RBCPurgeNumExpected = config.get_float("RBC_PURGE", "rbcpurge_num_expected", 0.0f, 10.0f);

   // [PLASMA_VALVE_MOTION]
   _data.PumpsStopWaitTime = config.get_float("PLASMA_VALVE_MOTION", "pumps_stop_wait_time", 0.0f, 10.0f);        // seconds

   // [SETTLE_CHANNEL]
   _data.SettleChannelQin = config.get_float("SETTLE_CHANNEL", "settle_channel_qin", 0.0f, 200.0f);
   _data.SettleChannelVin = config.get_float("SETTLE_CHANNEL", "settle_channel_vin", 0.0f, 1000.0f);

   // [RECIRCULATE]
   _data.MinRecirculatePlateletCollect = config.get_float("RECIRCULATE", "min_recirculate_platelet_collect", 0.0f, 1.0f);
   _data.MaxRecirculatePlateletCollect = config.get_float("RECIRCULATE", "max_recirculate_platelet_collect", 0.0f, 1.0f);
   _data.RecirculatePcePreRbc          = config.get_float("RECIRCULATE", "recirculate_pce_prerbc", 0.0f, 5.0f);
   _data.RecirculateSnDrawVol          = config.get_float("RECIRCULATE", "recirculate_sn_draw_vol", 0.0f, 20.0f);
   _data.PltPlsCrr                     = config.get_float("RECIRCULATE", "plt_pls_crr", 0.0f, 1.0f);
   _data.RBCPlRecircConstant           = config.get_float("RECIRCULATE", "rbc_pl_recirc_constant", 0.0f, 1.0f);

   // [RBC_PLASMA]
   _data.RbcPlasmaPrimeVolume     = config.get_float("RBC_PLASMA", "rbc_plasma_prime_volume", 0.0f, 500.0f);
   _data.RbcPlasmaPrimeTime       = config.get_float("RBC_PLASMA", "rbc_plasma_prime_time", 0.0f, 20.0f);
   _data.MaxRbcPlasmaHrbc         = config.get_float("RBC_PLASMA", "max_rbc_plasma_hrbc", 0.0f, 1.0f);
   _data.RbcPlasmaQinmaxr         = config.get_float("RBC_PLASMA", "rbc_plasma_qinmaxr", 50.0f, 142.0f);
   _data.RbcPlasmaQmaxr           = config.get_float("RBC_PLASMA", "rbc_plasma_qmaxr", 50.0f, 142.0f);
   _data.RbcHctDelta              = config.get_float("RBC_PLASMA", "rbc_hct_delta", 0.0f, 0.25f);
   _data.MinimumIRAdjustmentRatio = config.get_float("RBC_PLASMA", "minimum_IR_adjustment_ratio", 0.2f, 1.5f);
   _data.ApplyAdjustmentSafetyIR  = config.get_float("RBC_PLASMA", "apply_adjustment_safetyIR", 0.5f, 1.5f);
   _data.ApplyAdjustmentControlIR = config.get_float("RBC_PLASMA", "apply_adjustment_controlIR", 0.5f, 1.5f);
   _data.SafetyIRMultiplier       = config.get_float("RBC_PLASMA", "safety_IR_multiplier", 0.5f, 3.0f);

   // [RBC_COLLECTION]
   _data.RBCQch              = config.get_float("RBC_COLLECTION", "rbc_qch", 0.0f, 5.0f);
   _data.MaxQinRbcCollection = config.get_float("RBC_COLLECTION", "max_qin_rbc_collection", 0.0f, 200.0f);

   // [WBC]
   _data.WBCAwSaturationConstant = config.get_float("WBC", "wbc_aw_saturation_constant", 10.0f, 500.0f);
   _data.WBCBwSaturationConstant = config.get_float("WBC", "wbc_bw_saturation_constant", 0.0f, 1.0f);
   _data.WBCChannelInventory     = config.get_float("WBC", "wbc_channel_inventory", 0.0f, 5.0f);

   // [MNC]
   _data.MNCPrecountForPurge         = config.get_float("MNC", "mnc_precount_for_purge", 0.0f, 50.0f);
   _data.MNCPrecountForPurgeLargeBMI = config.get_float("MNC", "mnc_precount_for_purge_large_bmi", 0.0f, 10.0f);
   _data.MNCPrecountForPltLoss       = config.get_float("MNC", "mnc_precount_for_plt_loss", 0.0f, 5.0f);

   // [PIR]
   _data.PIRYtargLossNormal     = config.get_float("PIR", "pir_ytarg_loss_normal", 0.0f, 10.0f);
   _data.PIRYtargLossPreferPIR  = config.get_float("PIR", "pir_ytarg_loss_prefer_pir", 0.0f, 10.0f);

   _data.PIRVcAtMax             = config.get_float("PIR", "pir_vc_at_max", 0.0f, 40.0f);
   _data.PIRAllowableVcError    = config.get_float("PIR", "pir_allowable_vc_error", 0.0f, 40.0f);
   _data.PIRRampRateCoefficient = config.get_float("PIR", "pir_ramp_rate_coefficient", 0.0f, 1.0f);
   _data.PIRLinearRampQchLimit  = config.get_float("PIR", "pir_linear_ramp_qch_limit", 0.0f, 10.0f);
   _data.PIRLinearRampRate      = config.get_float("PIR", "pir_linear_ramp_rate", 0.0f, 5.0f);
   _data.PIRQcdot               = config.get_float("PIR", "pir_qcdot", 0.0f, 5.0f);
   _data.PIRPercentOffload      = config.get_float("PIR", "pir_percent_offload", 0.0f, 1.0f);

   // [PLATELET]
   _data.PlateletPlasmaPrimeVolume  = config.get_float("PLATELET", "platelet_plasma_prime_volume", 0.0f, 1000.0f);
   _data.PlateletPlasmaPrimeTime    = config.get_float("PLATELET", "platelet_plasma_prime_time", 0.0f, 20.0f);
   _data.MaxPlateletPlasmaHrbc      = config.get_float("PLATELET", "max_platelet_plasma_hrbc", 0.0f, 0.9f);
   _data.PlateletSaturationAp       = config.get_float("PLATELET", "platelet_saturation_ap", 1.0f, 10.0f);
   _data.PlateletSaturationBp       = config.get_float("PLATELET", "platelet_saturation_bp", 0.0f, 1.0f);
   _data.ChannelPlateletInventory   = config.get_float("PLATELET", "channel_platelet_inventory", 0.0f, 5.0f);
   _data.ChamberFlowAvgTime         = config.get_float("PLATELET", "chamber_flow_avg_time", 0.0f, 20.0f);
   _data.MaximumEpcaVolume          = config.get_float("PLATELET", "maximum_epca_volume", 0.0f, 1000.0f);
   _data.EpcaChamberFlow            = config.get_float("PLATELET", "epca_chamber_flow", 0.0f, 10.0f);
   _data.PlateletDeltaVinYield      = config.get_float("PLATELET", "platelet_delta_vin_yield", 0.0f, 1000.0f);
   _data.MinimumCollectVolume       = config.get_float("PLATELET", "minimum_collect_volume", 0.0f, 200.0f);
   _data.PlateletMobilizationFactor = config.get_float("PLATELET", "platelet_mobilization_factor", 0.0f, 1.0f);
   _data.PpcConcentrationThreshold  = config.get_float("PLATELET", "ppc_concentration_threshold", 2000.0f, 9900.0f);
   _data.TppYieldThreshold          = config.get_float("PLATELET", "tpp_yield_threshold", 8.9f, 12.0f);

   // [CENTRIFUGE]
   _data.StandardLowCentrifugeRamp           = config.get_float("CENTRIFUGE", "standard_low_centrifuge_ramp", 1.0f, 300.0f);
   _data.StandardHighCentrifugeRamp          = config.get_float("CENTRIFUGE", "standard_high_centrifuge_ramp", 1.0f, 300.0f);
   _data.StandardCentrifugeRampTransition    = config.get_float("CENTRIFUGE", "standard_centrifuge_ramp_transition", 0.0f, 3000.0f);
   _data.CentrifugeRampDownRate              = config.get_float("CENTRIFUGE", "ramp_down_rate", 0.0f, 10000.0f);
   _data.StopRecoveryLowCentrifugeRampUpRate = config.get_float("CENTRIFUGE", "stop_recovery_low_centrifuge_ramp", 0.0f, 3000.0f);
   _data.CentrifugeMinCommandedRunSpeed      = config.get_float("CENTRIFUGE", "cent_min_cmd_run_spd", 0.0f, 3000.0f);
   _data.CentrifugeOpenSpeed                 = config.get_float("CENTRIFUGE", "cent_open_speed", 0.0f, 3000.0f);
   _data.CentrifugePumpsPausedSpeed          = config.get_float("CENTRIFUGE", "cent_pumps_paused_speed", 0.0f, 3000.0f);
   _data.CentrifugeSpeedError                = config.get_float("CENTRIFUGE", "cent_speed_error", 0.0f, 3000.0f);
   _data.CentResonantRPM                     = config.get_float("CENTRIFUGE", "cent_resonant_rpm", 0.0f, 3000.0f);
   _data.CentPresLimitLow                    = config.get_float("CENTRIFUGE", "cent_pres_limit_low", 0.0f, 5000.0f);
   _data.CentPresLimitHigh                   = config.get_float("CENTRIFUGE", "cent_pres_limit_high", 0.0f, 5000.0f);
   _data.CentSpeedMax                        = config.get_float("CENTRIFUGE", "cent_speed_max", 0.0f, 3300.0f);
   _data.CentrifugeSpeedErrorPrime           = config.get_float("CENTRIFUGE", "cent_speed_error_prime", 0.0f, 3000.0f);
   _data.CsfMax                              = config.get_float("CENTRIFUGE", "csf_max", 0.0f, 1.5f);
   _data.CsfMin                              = config.get_float("CENTRIFUGE", "csf_min", 0.0f, 1.5f);
   _data.CsfQin                              = config.get_float("CENTRIFUGE", "csf_qin", 10.0f, 200.0f);
   _data.CentRecoveryMaxVolume               = config.get_float("CENTRIFUGE", "cent_recovery_max_volume", 0.0f, 300.0f);


   // [DISPOSABLE_TEST]
   _data.EvacuationVolume                     = config.get_float("DISPOSABLE_TEST", "evacuation_volume",  0.0f, 500.0f);
   _data.EvacuationDeltaPressureLimit         = config.get_float("DISPOSABLE_TEST", "evacuation_delta_pressure_limit",  0.0f, 200.0f);
   _data.EvacuationAbsPressureLimit           = config.get_float("DISPOSABLE_TEST", "evacuation_abs_pressure_limit",  0.0f, 200.0f);
   _data.EvacuationStartupDelay               = config.get_float("DISPOSABLE_TEST", "evacuation_startup_delay",  0.0f, 30.0f);
   _data.EvacuationValveOpenDwell             = config.get_float("DISPOSABLE_TEST", "evacuation_valve_open_dwell",  0.0f, 30.0f);
   _data.EvacuationReturnSpeed                = config.get_float("DISPOSABLE_TEST", "evacuation_return_speed",  0.0f, 401.0f);
   _data.EvacuationPressureRecoveryLimit      = config.get_float("DISPOSABLE_TEST", "evacuation_pressure_recovery_limit",  0.0f, 400.0f);
   _data.EvacuationRecoveryVolumeLimit        = config.get_float("DISPOSABLE_TEST", "evacuation_recovery_volume_limit",  -100.0f, 0.0f);

   _data.DispTestVolumeFail                   = config.get_float("DISPOSABLE_TEST", "disptest_volume_fail",  0.0f, 200.0f);
   _data.DispTestVolumeCheck                  = config.get_float("DISPOSABLE_TEST", "disptest_volume_check",  0.0f, 200.0f);
   _data.DispTestDeltaAPS                     = config.get_float("DISPOSABLE_TEST", "disptest_delta_aps",  0.0f, 400.0f);
   _data.DispTestAPSThreshold                 = config.get_float("DISPOSABLE_TEST", "disptest_aps_threshold", 0.0f, 400.0f);
   _data.DispTestVolumeFail2                  = config.get_float("DISPOSABLE_TEST", "disptest_volume_fail2",  0.0f, 200.0f);
   _data.DispTestReturnSpeed1                 = config.get_float("DISPOSABLE_TEST", "disptest_return_speed1",  0.0f, 400.0f);
   _data.DispTestSampleBagVolm                = config.get_float("DISPOSABLE_TEST", "disptest_samplebag_vol",  0.0f, 400.0f);
   _data.DispTestApsMax                       = config.get_float("DISPOSABLE_TEST", "disptest_aps_max",  0.0f, 500.0f);

   _data.DispTestApsMin                       = config.get_float("DISPOSABLE_TEST", "disptest_aps_min",  -400.0f, 0.0f);
   _data.DispTestCrossoverLow                 = config.get_float("DISPOSABLE_TEST", "disptest_crossover_low",  0.0f, 400.0f);
   _data.DispTestCrossoverHigh                = config.get_float("DISPOSABLE_TEST", "disptest_crossover_high",  0.0f, 400.0f);
   _data.DispTestCrossoverDelta               = config.get_float("DISPOSABLE_TEST", "disptest_crossover_delta",  0.0f, 100.0f);
   _data.DispTestCrackpressureVolm            = config.get_float("DISPOSABLE_TEST", "disptest_crackpressure_volm",  0.0f, 400.0f);

   _data.DispTestTestSpinSpeed                = config.get_float("DISPOSABLE_TEST", "disptest_testspin_speed",  0.0f, 3000.0f);
   _data.DispTestTestSpinTime                 = config.get_float("DISPOSABLE_TEST", "disptest_testspin_time",  0.0f, 120.0f);

   _data.EvacuationDesiredAps                 = config.get_float("DISPOSABLE_TEST", "evacuation_desired_aps",   0.0f, 199.0f);
   _data.EvacuationGain                       = config.get_float("DISPOSABLE_TEST", "evacuation_gain",  -5.0f, 5.0f);
   _data.EvacuationQreturnEvac                = config.get_float("DISPOSABLE_TEST", "evacuation_qreturn_evac",  0.0f, 140.0f);
   _data.EvacuationStartingQinSpeed           = config.get_float("DISPOSABLE_TEST", "evacuation_starting_qin_speed",  0.0f, 140.0f);
   _data.EvacuationMinQinWatchVolume          = config.get_float("DISPOSABLE_TEST", "evacuation_min_qin_watch_volume",  0.0f, 120.0f);
   _data.EvacuationEndQinDeltaOffSlowestSpeed = config.get_float("DISPOSABLE_TEST", "evacuation_end_qin_delta_off_slowest_speed", -30.0f, 75.0f);
   _data.EvacuationPlsEvacVc                  = config.get_float("DISPOSABLE_TEST", "evacuation_pls_evac_vc", 0.0f, 601.0f);
   _data.EvacuationMaxPltEvacLimit            = config.get_float("DISPOSABLE_TEST", "evacuation_max_plt_evac_volm", 0.0f, 401.0f);
   _data.EvacuationBlackplsExtra              = config.get_float("DISPOSABLE_TEST", "evacuation_black_pls_extra", 0.0f, 200.0f);

   // [STARTUP]
   _data.PressureMeterMinLimit         = config.get_float("STARTUP", "pressure_meter_min_limit", -500.0f, 0.0f);
   _data.PressureMeterMaxLimit         = config.get_float("STARTUP", "pressure_meter_max_limit", 0.0f, 500.0f);

   _data.RPMBloodPrime                 = config.get_float("STARTUP", "rpm_blood_prime",  0.0f, 3000.0f);
   _data.QinBloodPrime                 = config.get_float("STARTUP", "qin_blood_prime",  0.0f, 200.0f);
   _data.QrpBloodPrime                 = config.get_float("STARTUP", "qrp_blood_prime",  0.0f, 400.0f);
   _data.VinBloodPrimeInlet            = config.get_float("STARTUP", "vin_blood_prime_inlet", 0.0f, 1000.0f);

   _data.QrpStartup                    = config.get_float("STARTUP", "qrp_startup", 0.0f, 400.0f);
   _data.QinStartup                    = config.get_float("STARTUP", "qin_startup", 0.0f, 142.0f);
   _data.CPSStartup                    = config.get_float("STARTUP", "cps_startup", 500.0f, 2500.0f);

   _data.QinPrimeChannel               = config.get_float("STARTUP", "qin_prime_channel",  0.0f, 200.0f);
   _data.QcolPrimeChannel              = config.get_float("STARTUP", "qcol_prime_channel",  0.0f, 100.0f);
   _data.QpPrimeChannelPltPlsRBC       = config.get_float("STARTUP", "qp_prime_channel_pltplsrbc",  0.0f, 100.0f);
   _data.QpPrimeChannelRBCPls          = config.get_float("STARTUP", "qp_prime_channel_rbcpls",  0.0f, 100.0f);
   _data.RPMPrimeChannel               = config.get_float("STARTUP", "rpm_prime_channel",  0.0f, 3000.0f);
   _data.VinPrimeChannel               = config.get_float("STARTUP", "vin_prime_channel",  0.0f, 1000.0f);

   _data.QinPrimeChannel2              = config.get_float("STARTUP", "qin_prime_channel2",  0.0f, 200.0f);
   _data.QcolPrimeChannel2             = config.get_float("STARTUP", "qcol_prime_channel2",  0.0f, 100.0f);
   _data.QpPrimeChannel2               = config.get_float("STARTUP", "qp_prime_channel2",  0.0f, 100.0f);
   _data.RPMPrimeChannel2              = config.get_float("STARTUP", "rpm_prime_channel2",  0.0f, 3000.0f);
   _data.VinPrimeChannel2              = config.get_float("STARTUP", "vin_prime_channel2",  0.0f, 1000.0f);

   _data.QinPrimeChannel2A             = config.get_float("STARTUP", "qin_prime_channel2a",  0.0f, 200.0f);
   _data.QcolPrimeChannel2A            = config.get_float("STARTUP", "qcol_prime_channel2a",  0.0f, 100.0f);
   _data.QpPrimeChannel2A              = config.get_float("STARTUP", "qp_prime_channel2a",  0.0f, 100.0f);
   _data.RPMPrimeChannel2A             = config.get_float("STARTUP", "rpm_prime_channel2a",  0.0f, 3000.0f);
   _data.VinPrimeChannel2A             = config.get_float("STARTUP", "vin_prime_channel2a",  0.0f, 1000.0f);
   _data.HctTriggerChannel2A           = config.get_float("STARTUP", "hct_trigger_channel2a",  0.0f, 1000.0f);

   _data.QinPrimeChannel3              = config.get_float("STARTUP", "qin_prime_channel3",  0.0f, 200.0f);
   _data.QcolPrimeChannel3             = config.get_float("STARTUP", "qcol_prime_channel3",  0.0f, 100.0f);
   _data.QpPrimeChannel3               = config.get_float("STARTUP", "qp_prime_channel3",  0.0f, 100.0f);
   _data.RPMPrimeChannel3              = config.get_float("STARTUP", "rpm_prime_channel3",  0.0f, 3000.0f);
   _data.VinPrimeChannel3              = config.get_float("STARTUP", "vin_prime_channel3",  0.0f, 1000.0f);

   _data.QinPrimeChannel4              = config.get_float("STARTUP", "qin_prime_channel4",  0.0f, 200.0f);
   _data.QcolPrimeChannel4             = config.get_float("STARTUP", "qcol_prime_channel4",  0.0f, 100.0f);
   _data.QpPrimeChannel4               = config.get_float("STARTUP", "qp_prime_channel4",  0.0f, 100.0f);
   _data.RPMPrimeChannel4              = config.get_float("STARTUP", "rpm_prime_channel4",  0.0f, 3000.0f);
   _data.VinPrimeChannel4              = config.get_float("STARTUP", "vin_prime_channel4",  0.0f, 1000.0f);

   _data.QinPrimeVent                  = config.get_float("STARTUP", "qin_prime_vent",  0.0f, 200.0f);
   _data.QcolPrimeVent                 = config.get_float("STARTUP", "qcol_prime_vent",  0.0f, 100.0f);
   _data.QpPrimeVent                   = config.get_float("STARTUP", "qp_prime_vent",  0.0f, 100.0f);
   _data.RPMPrimeVent                  = config.get_float("STARTUP", "rpm_prime_vent",  0.0f, 3000.0f);
   _data.VinPrimeVent                  = config.get_float("STARTUP", "vin_prime_vent",  0.0f, 1000.0f);

   _data.QinRampCentrifuge             = config.get_float("STARTUP", "qin_ramp_centrifuge",  0.0f, 200.0f);
   _data.QcolRampCentrifuge            = config.get_float("STARTUP", "qcol_ramp_centrifuge", 0.0f, 100.0f);
   _data.QpRampCentrifuge              = config.get_float("STARTUP", "qp_ramp_centrifuge", 0.0f, 100.0f);
   _data.RPMRampCentrifuge             = config.get_float("STARTUP", "rpm_ramp_centrifuge",  0.0f, 3000.0f);

   _data.QinPrimeAirout1               = config.get_float("STARTUP", "qin_prime_airout_1",  0.0f, 200.0f);
   _data.QcolPrimeAirout1              = config.get_float("STARTUP", "qcol_prime_airout_1", 0.0f, 100.0f);
   _data.QpPrimeAirout1                = config.get_float("STARTUP", "qp_prime_airout_1", 0.0f, 100.0f);
   _data.RPMPrimeAirout1               = config.get_float("STARTUP", "rpm_prime_airout_1",  0.0f, 3200.0f);

   _data.QinPrimeAirout2               = config.get_float("STARTUP", "qin_prime_airout_2",  0.0f, 200.0f);
   _data.QcolPrimeAirout2              = config.get_float("STARTUP", "qcol_prime_airout_2", 0.0f, 100.0f);
   _data.QpPrimeAirout2                = config.get_float("STARTUP", "qp_prime_airout_2", 0.0f, 100.0f);
   _data.RPMPrimeAirout2               = config.get_float("STARTUP", "rpm_prime_airout_2",  0.0f, 3000.0f);
   _data.DeltaVinPrimeAirout2          = config.get_float("STARTUP", "delta_vin_prime_airout_2", 0.0f, 500.0f);

   _data.RbcPrimeSpilloverRisePercent  = config.get_float("STARTUP", "rbc_prime_spillover_rise_percent", 0.0f, 100.0f);
   _data.PrimeSpilloverSampleThreshold = config.get_int("STARTUP", "prime_spillover_sample_threshold", 0, 30);
   _data.MaxConsecutiveSpillovers      = config.get_int("STARTUP", "max_consecutive_spillovers",  0, 9);

   // [COLLECT]
   _data.RPMCollect             = config.get_float("COLLECT", "rpm_collect",              0.0f, 3000.0f);
   _data.DutyCycleLimit         = config.get_float("COLLECT", "duty_cycle_limit",         0.0f,    1.0f);
   _data.RBCDutyCycleLimit      = config.get_float("COLLECT", "rbc_duty_cycle_limit",     0.0f,    1.0f);
   _data.QneedleRampCoefficient = config.get_float("COLLECT", "qneedle_ramp_coefficient", 0.0f,    2.0f);

   // [RINSEBACK]
   _data.RinsebackIrbmin           = config.get_float("RINSEBACK", "rinseback_i_rbmin", 0.6f, 1.2f);
   _data.RinsebackQchrb1           = config.get_float("RINSEBACK", "rinseback_q_chrb1", 0.0f, 20.0f);
   _data.RinsebackQchrb2           = config.get_float("RINSEBACK", "rinseback_q_chrb2", 0.0f, 20.0f);
   _data.RinsebackQcspill          = config.get_float("RINSEBACK", "rinseback_q_cspill", 0.0f, 140.0f);
   _data.RinsebackQcpurge          = config.get_float("RINSEBACK", "rinseback_q_cpurge", 0.0f, 140.0f);
   _data.RinsebackQcrinse          = config.get_float("RINSEBACK", "rinseback_q_crinse", 0.0f, 140.0f);
   _data.RinsebackQinrb            = config.get_float("RINSEBACK", "rinseback_q_inrb", 40.0f, 140.0f);
   _data.RinsebackQinrec           = config.get_float("RINSEBACK", "rinseback_q_inrec", 40.0f, 140.0f);
   _data.RinsebackQpm              = config.get_float("RINSEBACK", "rinseback_q_pm", 1.0f, 19.0f);
   _data.RinsebackQpmev            = config.get_float("RINSEBACK", "rinseback_q_pmev", 0.0f, 19.0f);
   _data.RinsebackQpmrb1           = config.get_float("RINSEBACK", "rinseback_q_pmrb1", 0.0f, 19.0f);
   _data.RinsebackQpmrb2           = config.get_float("RINSEBACK", "rinseback_q_pmrb2", 0.0f, 19.0f);
   _data.RinsebackQrrec            = config.get_float("RINSEBACK", "rinseback_q_rrec", 40.0f, 140.0f);
   _data.RinsebackQcdecel          = config.get_float("RINSEBACK", "rinseback_q_cdecel", 1.0f, 140.0f);
   _data.RinsebackVplsdecel        = config.get_float("RINSEBACK", "rinseback_v_plsdecel", 0.0f, 100.0f);
   _data.RinsebackVcpurge          = config.get_float("RINSEBACK", "rinseback_v_cpurge", 0.0f, 100.0f);
   _data.RinsebackVcrb1            = config.get_float("RINSEBACK", "rinseback_v_crb1", 0.0f, 100.0f);
   _data.RinsebackVcrinse1         = config.get_float("RINSEBACK", "rinseback_v_crinse1", 0.0f, 100.0f);
   _data.RinsebackVcrinse2         = config.get_float("RINSEBACK", "rinseback_v_crinse2", 0.0f, 100.0f);
   _data.RinsebackVinrec           = config.get_float("RINSEBACK", "rinseback_v_inrec", 0.0f, 250.0f);
   _data.RinsebackVpevhold         = config.get_float("RINSEBACK", "rinseback_v_pevhold", 0.0f, 100.0f);
   _data.RinsebackVplrb            = config.get_float("RINSEBACK", "rinseback_v_plrb", 0.0f, 100.0f);
   _data.RinsebackVpec             = config.get_float("RINSEBACK", "rinseback_v_prec", 10.0f, 80.0f);
   _data.RinsebackVpvalve          = config.get_float("RINSEBACK", "rinseback_v_pvalve", 0.0f, 100.0f);
   _data.RinsebackVr               = config.get_float("RINSEBACK", "rinseback_v_r", 0.0f, 100.0f);
   _data.RinsebackVrpevmin         = config.get_float("RINSEBACK", "rinseback_v_rpevmin", 0.0f, 100.0f);
   _data.RinsebackVs               = config.get_float("RINSEBACK", "rinseback_v_s", 0.0f, 100.0f);
   _data.RinsebackVtrplt           = config.get_float("RINSEBACK", "rinseback_v_trplt", 10.0f, 100.0f);
   _data.RinsebackVtrrbc           = config.get_float("RINSEBACK", "rinseback_v_trrbc", 10.0f, 100.0f);
   _data.RPMRinsebackSpillAndPurge = config.get_float("RINSEBACK", "rpm_rinseback_spillandpurge", 0.0f, 3000.0f);
   _data.RPMRinsebackThreshold     = config.get_float("RINSEBACK", "rpm_rinseback_threshold", 0.0f, 3000.0f);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Old v5 values
   _data.RinsebackQinRecirculate       = config.get_float("RINSEBACK", "rinseback_qin_recirculate", 40.0f, 200.0f);
   _data.RinsebackQchamberRecirculate  = config.get_float("RINSEBACK", "rinseback_qchamber_recirculate", 0.0f, 40.0f);
   _data.RinsebackVpRecirculate        = config.get_float("RINSEBACK", "rinseback_vp_recirculate", 0.0f, 100.0f);
   _data.RinsebackQcolMargin           = config.get_float("RINSEBACK", "rinseback_qcol_margin", 0.0f, 30.0f);
   _data.RinsebackQchamber             = config.get_float("RINSEBACK", "rinseback_qchamber", 0.0f, 40.0f);
   _data.RinsebackMinInfusion          = config.get_float("RINSEBACK", "rinseback_min_infusion", 0.6f, 1.2f);
   _data.RinsebackVretLower            = config.get_float("RINSEBACK", "rinseback_vret_lower", 0.0f, 100.0f);
   _data.RinsebackEvacuationHoldVolume = config.get_float("RINSEBACK", "rinseback_evacuation_hold_volume", 0.0f, 100.0f);
   _data.RinsebackEvacuationMinVolume  = config.get_float("RINSEBACK", "rinseback_evacuation_min_volume", 0.0f, 100.0f);
   _data.RinsebackValveVolume          = config.get_float("RINSEBACK", "rinseback_valve_volume", 0.0f, 100.0f);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   // [YIELD]
   _data.YieldQinConstant             = config.get_float("YIELD", "yield_qin_constant", -100.0f, 1000001.0f);
   _data.YieldVinConstant             = config.get_float("YIELD", "yield_vin_constant", -100.0f, 1000001.0f);
   _data.PlateletEfficiencyMultiplier = config.get_float("YIELD", "platelet_efficiency_multiplier", 0.0f, 10.0f);
   _data.YieldStartupVolumeStd        = config.get_float("YIELD", "yield_startup_volume_std", 0.0f, 5000.0f);
   _data.YieldStartupVolumePPC        = config.get_float("YIELD", "yield_startup_volume_ppc", 0.0f, 5000.0f);

   _data.YieldEfficiencyConstant      = config.get_float("YIELD", "yield_efficiency_constant", 0.0f, 20.0f);

   _data.YieldPostPltConstant         = config.get_float("YIELD", "yield_postplt_constant", -100.0f, 1000001.0f);

   _data.EfficiencyMinQinc            = config.get_float("YIELD", "efficiency_min_qinc", 0.0f, 142.0f);
   _data.YieldEfficiencyConstant      = config.get_float("YIELD", "yield_efficiency_constant", 0.0f, 20.0f);
   _data.EfficiencyMinPostcount       = config.get_float("YIELD", "efficiency_min_postcount", 0.0f, 500.0f);


   // [APS_PAUSE]
   _data.ApsPausePeriod           = config.get_float("APS_PAUSE", "aps_pause_period", 0.0f, 150.0f);
   _data.ApsMaxPausesInPeriod     = config.get_float("APS_PAUSE", "aps_max_pauses_in_period", 0.0f, 500.0f);
   _data.ApsPauseTimeBeforeAlarm  = config.get_float("APS_PAUSE", "aps_pause_time_before_alarm", 0.0f, 60.0f);
   _data.ApsMinimumPauseTime      = config.get_float("APS_PAUSE", "aps_minimum_pause_time", 0.0f, 60.0f);
   _data.ApsNominalPressure       = config.get_float("APS_PAUSE", "aps_nominal_pressure", 0.0f, 500.0f);
   _data.ApsFlowReductionFactor   = config.get_float("APS_PAUSE", "aps_flow_reduction_factor", 0.0f, 1.0f);
   _data.ApsAdditionalDelay       = config.get_float("APS_PAUSE", "aps_additional_delay", 0.0f, 8.0f);
   _data.ApsPositivePressureCheck = config.get_float("APS_PAUSE", "aps_positive_pressure_check", 0.0f, 55.0f);

   // AUTOFLOW
   _data.AutoflowIncreaseTimer     = config.get_float("APS_PAUSE", "autoflow_increase_timer", 0.0f, 25.0f);
   _data.AutoflowIncreaseIncrement = config.get_float("APS_PAUSE", "autoflow_increase_increment", 0.0f, 20.0f);
   _data.AutoflowDecreaseIncrement = config.get_float("APS_PAUSE", "autoflow_decrease_increment", 0.0f, 20.0f);




   // [HYPOVOLEMIA]
   _data.HypovolemiaFCVwhite = config.get_float("HYPOVOLEMIA", "hypovolemia_fcv_white", 50.0f, 500.0f);
   _data.HypovolemiaFCVblack = config.get_float("HYPOVOLEMIA", "hypovolemia_fcv_black", 50.0f, 500.0f);


   // [VERIFY_FLAGS]

   _data.ConcMax  = config.get_float("VERIFY_FLAGS", "maximum_concentration_for_no_verify_flag", 2100.0f, 9900.0f);
   _data.Hcmin    = config.get_float("VERIFY_FLAGS", "minimum_donor_hematocrit_change_for_no_verify_flag", 1.0f, 9.0f);
   _data.HCThigh  = config.get_float("VERIFY_FLAGS", "maximum_donor_hematocrit_for_no_verify_flag", 45.0f, 55.0f);
   _data.Qch1     = config.get_float("VERIFY_FLAGS", "minimum_chamber_flow_rate_for_no_verify_flag", 0.0f, 5.0f);
   _data.Tss1     = config.get_float("VERIFY_FLAGS", "initial_platelet_collecting_time_for_no_verify_flag", 0.0f, 99.0f);
   _data.VinSS    = config.get_float("VERIFY_FLAGS", "steady_state_inlet_volume", 50.0f, 2000.0f);
   _data.Vplt1    = config.get_float("VERIFY_FLAGS", "maximum_plt_volume_deviation_for_no_verify_flag", 1.0f, 50.0f);
   _data.Vplasma1 = config.get_float("VERIFY_FLAGS", "maximum_plasma_volume_deviation_for_no_verify_flag", 1.0f, 50.0f);
   _data.Vrbc1    = config.get_float("VERIFY_FLAGS", "maximum_rbc_volume_deviation_for_no_verify_flag", 1.0f, 50.0f);

   _data.RetOccStabilizationDelay       = config.get_float("RETURN_OCCLUSION", "stabilization_delay", 1.0f, 10.0f);
   _data.RetOccAlarmPowerLimit          = config.get_float("RETURN_OCCLUSION", "alarm_power_limit", 0.0f, 20000.0f);
   _data.RetOccAlarmType                = config.get_int("RETURN_OCCLUSION", "alarm_type", 0, 2);
   _data.RetOccInletFirstHarmonicScale  = config.get_float("RETURN_OCCLUSION", "inlet_first_harmonic_scale", 0.0f, 2.0f);
   _data.RetOccInletSecondHarmonicScale = config.get_float("RETURN_OCCLUSION", "inlet_second_harmonic_scale", 0.0f, 2.0f);

   // [CHAMBER_SATURATION]
   _data.ChamberSatVs3a           = config.get_float("CHAMBER_SATURATION", "chamber_sat_vs3a", 50.0f, 5000.0f);
   _data.ChamberSatTs3b           = config.get_float("CHAMBER_SATURATION", "chamber_sat_ts3b",  1.0f,   20.0f);
   _data.ChamberSatTs3aAPS        = config.get_float("CHAMBER_SATURATION", "chamber_sat_ts3a_aps",  0.0f,   60.0f);
   _data.ChamberSatTs3bAPS        = config.get_float("CHAMBER_SATURATION", "chamber_sat_ts3b_aps",  0.0f,   60.0f);
   _data.ChamberSatTs4APS         = config.get_float("CHAMBER_SATURATION", "chamber_sat_ts4_aps",  0.0f,   60.0f);
   _data.ChamberSatFs3a           = config.get_float("CHAMBER_SATURATION", "chamber_sat_fs3a",  1.00f,    4.99f);
   _data.ChamberSatFs3b           = config.get_float("CHAMBER_SATURATION", "chamber_sat_fs3b",  2.00f,    9.99f);
   _data.ChamberSatDur_3a         = config.get_float("CHAMBER_SATURATION", "chamber_sat_dur_3a",  0.0f,   20.0f);
   _data.ChamberSatdRGrMd_3a      = config.get_float("CHAMBER_SATURATION", "chamber_sat_drgrmd_3a",  0.800f,    1.200f);
   _data.ChamberSatDur_3b         = config.get_float("CHAMBER_SATURATION", "chamber_sat_dur_3b",  0.0f,   20.0f);
   _data.ChamberSatdRGrMd_3b      = config.get_float("CHAMBER_SATURATION", "chamber_sat_drgrmd_3b",  0.800f,    1.200f);
   _data.ChamberSatRgpMd_4a       = config.get_float("CHAMBER_SATURATION", "chamber_sat_rgpmd_4a",  0.010f,    0.120f);
   _data.ChamberSatRgpMd_4b       = config.get_float("CHAMBER_SATURATION", "chamber_sat_rgpmd_4b",  0.010f,    0.120f);

   _data.CalculationInterval      = config.get_float("CHAMBER_SATURATION", "calculation_interval", 0.0f, 20000.0f);
   _data.ActivationTime           = config.get_float("CHAMBER_SATURATION", "activation_time", 0.0f, 20000.0f);
   _data.MinDeltaChamberFlow      = config.get_float("CHAMBER_SATURATION", "min_delta_chamber_flow", 0.0f, 1.9f);
   _data.MinChamberFlow           = config.get_float("CHAMBER_SATURATION", "min_chamber_flow", 0.0f, 9.0f);
   _data.MinRed                   = config.get_float("CHAMBER_SATURATION", "min_red", 0.0f, 350.0f);
   _data.MaxRed                   = config.get_float("CHAMBER_SATURATION", "max_red", 0.0f, 2500.0f);
   _data.MinUpdateInterval        = config.get_float("CHAMBER_SATURATION", "min_update_interval", 0.0f, 5000.0f);
   _data.OneMin_Msecs             = config.get_float("CHAMBER_SATURATION", "one_min_msecs", 10.0f,  1000000.0f);
   _data.ThreeMinMsecs            = config.get_float("CHAMBER_SATURATION", "three_min_msecs", 0.0f, 1000000.0f);
   _data.PostPurgeWait            = config.get_float("CHAMBER_SATURATION", "post_purge_wait", 0.0f, 1000000.0f);
   _data.MinPltConcentrationJapan = config.get_float("CHAMBER_SATURATION", "min_plt_concentration_japan", 0.0f, 12.0f);
   _data.MinPltConcentration      = config.get_float("CHAMBER_SATURATION", "min_plt_concentration", 0.0f, 12.0f);
   _data.MinDeltaCentSpeed        = config.get_float("CHAMBER_SATURATION", "min_delta_cent_speed", 0.0f, 3000.0f);
   _data.MinCentSpeed             = config.get_float("CHAMBER_SATURATION", "min_cent_speed", 10.0f, 3000.0f);
   _data.VinStartc4a              = config.get_float("CHAMBER_SATURATION", "vin_start_c4a", 10.0f, 1200.0f);
   _data.VinEndc4b                = config.get_float("CHAMBER_SATURATION", "vin_end_c4b", 0.0f, 1200.0f);
   _data.MinBaseline              = config.get_float("CHAMBER_SATURATION", "min_baseline", 0.0f, 2.0f);

   // [PLASMA_LINE_OCCLUSION]
   _data.PLOebsmin   = config.get_int("PLASMA_LINE_OCCLUSION", "PLOebsmin",    0, 100);
   _data.PLOebsmax   = config.get_int("PLASMA_LINE_OCCLUSION", "PLOebsmax",    0, 100);
   _data.PLOesammin  = config.get_int("PLASMA_LINE_OCCLUSION", "PLOesammin",   0, 100);
   _data.CVedmin     = config.get_int("PLASMA_LINE_OCCLUSION", "CVedmin",      0, 100);
   _data.CVedmax     = config.get_int("PLASMA_LINE_OCCLUSION", "CVedmax",      0, 100);
   _data.Rdropthresh = config.get_int("PLASMA_LINE_OCCLUSION", "Rdropthresh",  0, 1000);
   _data.Gdropthresh = config.get_int("PLASMA_LINE_OCCLUSION", "Gdropthresh",  0, 1000);

   // [SALINE_PRIME]
   _data.SalinePrimeSpeed    = config.get_float("SALINE_PRIME", "saline_prime_speed", 0.0f,  70.0f);
   _data.SalinePrimevolume   = config.get_float("SALINE_PRIME", "saline_prime_volume", 0.0f,  25.0f);
   _data.SalinePrimeVinPoint = config.get_float("SALINE_PRIME", "saline_prime_vinPoint", 164.0f,  215.0f);

   // [PTF]
   // Predict constants
   _data.PtfRbcPlsSetup1Volume = config.get_float("PTF", "ptf_rbcpls_setup_1_volume", 0.0f,  100.0f);
   _data.PtfRbcPlsSetup1Hrbc   = config.get_float("PTF", "ptf_rbcpls_setup_1_hrbc",   45.0f, 99.9f);

   _data.PtfRbcPlsSetup2Volume = config.get_float("PTF", "ptf_rbcpls_setup_2_volume", 0.0f,  100.0f);


   _data.PtfRbcPlsSetup1BVolume           = config.get_float("PTF", "ptf_rbcpls_setup_1B_volume", 0.0f,  100.0f);
   _data.PtfRbcPlsSetup1BHrbc             = config.get_float("PTF", "ptf_rbcpls_setup_1B_hrbc",   45.0f, 99.9f);

   _data.PtfRbcPlsSetup2BVolume           = config.get_float("PTF", "ptf_rbcpls_setup_2B_volume", 0.0f,  100.0f);
   _data.PtfRbcPlsSetup2BPlasmaRampVolume = config.get_float("PTF", "ptf_rbcpls_setup_2B_plasma_ramp_volume", 0.0f,  150.0f);


   _data.PtfPltPlsRbcSetup1Volume   = config.get_float("PTF", "ptf_pltplsrbc_setup_1_volume",  0.0f,  100.0f);
   _data.PtfPltPlsRbcSetup1Hrbc     = config.get_float("PTF", "ptf_pltplsrbc_setup_1_hrbc",    45.0f, 99.9f);
   _data.PtfPltPlsRbcSetup2Volume   = config.get_float("PTF", "ptf_pltplsrbc_setup_2_volume",  0.0f,  100.0f);
   _data.PtfPltPlsRbcSetup2Hrbc     = config.get_float("PTF", "ptf_pltplsrbc_setup_2_hrbc",    45.0f,  99.9f);

   _data.PtfPltPlsRbcPrimeVolume    = config.get_float("PTF", "ptf_pltplsrbc_prime_volume",  0.0f,  100.0f);
   _data.PtfPltPlsRbcPrimeHrbc      = config.get_float("PTF", "ptf_pltplsrbc_prime_hrbc",    45.0f,  99.9f);
   _data.PtfPltPlsRbcPrimeQrbc      = config.get_float("PTF", "ptf_pltplsrbc_prime_qrbc",      0.0f,  100.0f);

   _data.PtfHctScalingFactor        = config.get_float("PTF", "ptf_hct_scaling_factor",      0.0f,  50.0f);

   _data.PtfQrbc                    = config.get_float("PTF", "ptf_qrbc",                   25.0f,   45.0f);
   _data.PtfSetupReturnPumpMultiple = config.get_float("PTF", "ptf_setup_return_pump_multiple",    0.4f,   1.0f);
   _data.PtfSetupReturnMax          = config.get_float("PTF", "ptf_setup_return_max",    50.0f,   150.0f);

   // Proc constants
   _data.PTFCPSLoadHigh        = config.get_float("PTF", "ptf_cps_load_high",       0.0f,  400.0f);
   _data.PTFCPSLoadLow         = config.get_float("PTF", "ptf_cps_load_low",      -400.0f,  0.0f);

   _data.PTFCPSTestChange      = config.get_float("PTF", "ptf_cps_test_change",       0.0f,  1100.0f);
   _data.PTFVrbcCollectPoint   = config.get_float("PTF", "ptf_vrbc_collect_point",      0.0f,  250.0f);
   _data.PTFRBCInitialCPSLimit = config.get_float("PTF", "ptf_rbc_initial_cps_limit", 0.0f, 1350.0f);
   _data.PTFCPSRBCCollectDelta = config.get_float("PTF", "ptf_rbc_collect_delta",       0.0f, 1000.0f);
   _data.PTFCPSRBCCollectLimit = config.get_float("PTF", "ptf_rbc_collect_limit",     0.0f, 1350.0f);


   _data.PTFVrbcCollectCPSNocount         = config.get_float("PTF", "ptf_vrbc_collect_cps_nocount",     0.0f, 75.0f);
   _data.PTFCPSAlertsPerProductCollecting = config.get_float("PTF", "ptf_cps_alerts_per_product",     0.0f, 5.0f);
   _data.PTFAPSAlertsPerProductMetering   = config.get_float("PTF", "ptf_aps_alerts_per_product",     0.0f, 5.0f);


   // [MSS]
   _data.MssApsAlertRecoveryQin    = config.get_float("MSS", "mss_aps_recovery_qin",      0.0f, 35.0f);
   _data.MssMinPltVolume           = config.get_float("MSS", "mss_min_plt_volume",      0.0f, 99.0f);
   _data.MssCpsHigh                = config.get_float("MSS", "mss_cps_high",                0.0f, 1500.0f);
   _data.MssApsBackpressure        = config.get_float("MSS", "mss_aps_backPressure",                 0.0f, 1000.0f);
   _data.MssCassetteVolMax         = config.get_float("MSS", "mss_cassette_vol_max",        0.0f, 100.0f);
   _data.MssApsHigh                = config.get_float("MSS", "mss_aps_high",                0.0f, 1000.0f);
   _data.MssPltSubstateMinPrimeVol = config.get_float("MSS", "mss_plt_mss_min_prime_vol",   0.0f, 100.0f);
   _data.MssPltSubstateMaxPrimeVol = config.get_float("MSS", "mss_plt_mss_max_prime_vol",   0.0f, 100.0f);

   _data.MssPltSubstateSalvageVol  = config.get_float("MSS", "mss_plt_ss_salvage_prime_vol", 0.0f, 100.0f);
   _data.MssVacuumCycles           = config.get_float("MSS", "mss_vacuum_cycles",           0.0f, 10.0f);
   _data.MssPltAddSpeed            = config.get_float("MSS", "mss_plt_ss_add_speed",        0.0f, 300.0f);

   _data.MssPltSSAddRampup         = config.get_int("MSS", "mss_plt_ss_add_rampup",           0, 1);

   _data.MssCvlWhite               = config.get_float("MSS", "mss_cvl_white",               0.0f, 80.0f);
   _data.MssCvlBlack               = config.get_float("MSS", "mss_cvl_black",               0.0f, 80.0f);
   _data.MssCvlSalvage             = config.get_float("MSS", "mss_cvl_salvage",             0.0f, 100.0f);

   _data.MssPrimeSpeedPercent      = config.get_float("MSS", "mss_prime_speed_percent",     0.0f, 1.0f);
   _data.MssPLTPrimeSpeed          = config.get_float("MSS", "mss_plt_prime_speed",         0.0f, 300.0f);
   _data.MssPLTValveTimer          = config.get_float("MSS", "mss_plt_valve_timer",         0.0f, 5.0f);
   _data.MssVac1CollectSpeed       = config.get_float("MSS", "mss_vac_1_collect_speed",     0.0f, 300.0f);
   _data.MssVac1PlasmaSpeed        = config.get_float("MSS", "mss_vac_1_plasma_speed",      0.0f, 300.0f);
   _data.MssVac2CollectSpeed       = config.get_float("MSS", "mss_vac_2_collect_speed",     0.0f, 300.0f);
   _data.MssVac2PlasmaSpeed        = config.get_float("MSS", "mss_vac_2_plasma_speed",      0.0f, 300.0f);
   _data.MssVac3CollectSpeed       = config.get_float("MSS", "mss_vac_3_collect_speed",     0.0f, 300.0f);
   _data.MssVac3PlasmaSpeed        = config.get_float("MSS", "mss_vac_3_plasma_speed",      0.0f, 300.0f);
   _data.MssVac1CollectVol         = config.get_float("MSS", "mss_vac_1_collect_vol",       0.0f, 100.0f);
   _data.MssVac1PlasmaVol          = config.get_float("MSS", "mss_vac_1_plasma_vol",        0.0f, 100.0f);
   _data.MssVac2PlasmaVol          = config.get_float("MSS", "mss_vac_2_plasma_vol",        0.0f, 100.0f);
   _data.MssVac3PlasmaVol          = config.get_float("MSS", "mss_vac_3_plasma_vol",        0.0f, 100.0f);
   _data.MssPrimeReturnVol         = config.get_float("MSS", "mss_prime_return_vol",        0.0f, 15.0f);




   _data.MssVac4PlasmaVol      = config.get_float("MSS", "mss_vac_4_plasma_vol",        0.0f, 100.0f);
   _data.MssVac4CollectSpeed   = config.get_float("MSS", "mss_vac_4_collect_speed",     0.0f, 300.0f);
   _data.MssVac4PlasmaSpeed    = config.get_float("MSS", "mss_vac_4_plasma_speed",      0.0f, 300.0f);
   _data.MssVacPurgeCyclePause = config.get_float("MSS", "mss_vac_purge_cycle_pause",   0.0f, 10.0f);




   _data.MssACSecondOrderConst  = config.get_float("MSS", "mss_ac_second_order_const", -1.0f, 3.0f);
   _data.MssACLinearConst       = config.get_float("MSS", "mss_ac_linear_const",       0.0f, 3.0f);
   _data.MssACOffset            = config.get_float("MSS", "mss_ac_offset",             0.0f, 3.0f);

   _data.MssPlsSecondOrderConst = config.get_float("MSS", "mss_pls_second_order_const", -1.0f, 3.0f);
   _data.MssPlsLinearConst      = config.get_float("MSS", "mss_pls_linear_const",       0.0f, 3.0f);
   _data.MssPlsOffset           = config.get_float("MSS", "mss_pls_offset",             0.0f, 3.0f);

   _data.MssPltSecondOrderConst = config.get_float("MSS", "mss_plt_second_order_const", -1.0f, 3.0f);
   _data.MssPltLinearConst      = config.get_float("MSS", "mss_plt_linear_const",       0.0f, 3.0f);
   _data.MssPltOffset           = config.get_float("MSS", "mss_plt_offset",             0.0f, 3.0f);
   _data.MssPLTPrimeFinalSpeed  = config.get_float("MSS", "mss_plt_prime_final_speed",  0.0f, 300.0f);
   _data.MssPLTPrimeFinalVol    = config.get_float("MSS", "mss_plt_prime_final_vol",    0.0f, 100.0f);


   _data.MssChannelCheck1CollectSpeed = config.get_float("MSS", "mss_channel_check1_collect_speed",  0.0f, 300.0f);
   _data.MssChannelCheck1CollectVol   = config.get_float("MSS", "mss_channel_check1_collect_vol",  0.0f, 100.0f);
   _data.MssChannelCheck2CollectSpeed = config.get_float("MSS", "mss_channel_check2_collect_speed",  0.0f, 300.0f);
   _data.MssChannelCheck2PlasmaSpeed  = config.get_float("MSS", "mss_channel_check2_plasma_speed",  0.0f, 300.0f);
   _data.MssChannelCheck2PlasmaVol    = config.get_float("MSS", "mss_channel_check2_plasma_vol",  0.0f, 100.0f);
   _data.MssPltSSPercent              = config.get_float("MSS", "mss_plt_ss_empty_percentage",  0.0f, 100.0f);
   _data.MssRbcSSPercent              = config.get_float("MSS", "mss_rbc_ss_empty_percentage",  0.0f, 100.0f);
   _data.MssRGratio                   = config.get_float("MSS", "mss_rbc_detector_offset_from_avg",  0.0f, 50.0f);
   _data.MssTriggerSamples            = config.get_float("MSS", "mss_rbc_detector_trigger_samples",  0.0f, 50.0f);
   _data.MssRbcDetectorZsamples       = config.get_float("MSS", "mss_rbc_detector_z_samples",  0.0f, 50.0f);
   _data.MssCheckLimit                = config.get_int("MSS", "mss_pas_flagging_check_limit",  0, 25);


   _data.MssPostPurgeFillPlasmaSpeed  = config.get_float("MSS", "mss_post_purge_fill_plasma_speed",   0.0f, 250.0f);
   _data.MssPostPurgeFillCollectSpeed = config.get_float("MSS", "mss_post_purge_fill_collect_speed",  0.0f, 250.0f);
   _data.MssPostPurgeFillPlasmaVolume = config.get_float("MSS", "mss_post_purge_fill_plasma_volume",  0.0f, 40.0f);

   _data.MssChannelVolume_w           = config.get_float("MSS", "mss_channel_volume_w",  0.0f, 100.0f);
   _data.MssRinsebackSuccess_w        = config.get_float("MSS", "mss_rinseback_success_w",  0.0f, 100.0f);
   _data.MssChannelVolume_b           = config.get_float("MSS", "mss_channel_volume_b",  0.0f, 100.0f);
   _data.MssRinsebackSuccess_b        = config.get_float("MSS", "mss_rinseback_success_b",  0.0f, 100.0f);
   _data.MssPlasmaRinsebackVolume     = config.get_float("MSS", "mss_plasma_rinseback_volume",  0.0f, 100.0f);
   _data.MssSalineRinsebackVolume     = config.get_float("MSS", "mss_saline_rinseback_volume",  0.0f, 125.0f);


   _data.MssRbcSubstateAddAcSpeed     = config.get_float("MSS", "mss_rbc_ss_add_ac_speed",     0.0f, 300.0f);
   _data.MssRbcSubstateAddPrime1Speed = config.get_float("MSS", "mss_rbc_ss_add_prime1_speed", 0.0f, 300.0f);
   _data.MssRbcSubstateAddPrime2Speed = config.get_float("MSS", "mss_rbc_ss_add_prime2_speed", 0.0f, 300.0f);
   _data.MssRbcSubstateAddPrime3Speed = config.get_float("MSS", "mss_rbc_ss_add_prime3_speed", 0.0f, 300.0f);
   _data.MssRbcSubstateAddMeterSpeed  = config.get_float("MSS", "mss_rbc_ss_add_meter_speed",  0.0f, 300.0f);
   _data.MssRbcSubstateAddPrime1Vol   = config.get_float("MSS", "mss_rbc_ss_add_prime1_vol",   0.0f, 300.0f);
   _data.MssRbcSubstateAddPrime2Vol   = config.get_float("MSS", "mss_rbc_ss_add_prime2_vol",   0.0f, 300.0f);
   _data.MssRbcSubstateAddPrime3Vol   = config.get_float("MSS", "mss_rbc_ss_add_prime3_vol",   0.0f, 300.0f);
   _data.MssRbcSubstateMinPrimeVol    = config.get_float("MSS", "mss_rbc_ss_min_prime_vol",    0.0f, 100.0f);
   _data.MssRbcSubstateMaxPrimeVol    = config.get_float("MSS", "mss_rbc_ss_max_prime_vol",    0.0f, 100.0f);
   _data.MssRbcSubstateSalvageVol     = config.get_float("MSS", "mss_rbc_ss_salvage_prime_vol", 0.0f, 100.0f);



   _data.MssPtfVolume                     = config.get_float("MSS", "mss_ptf_ss_volume",           0.0f, 90.0f);
   _data.MssPtfNumber                     = config.get_float("MSS", "mss_ptf_filter_no",           1.0f, 2.0f);

   _data.MssRBCPrimeSpeed                 = config.get_float("MSS", "mss_rbc_prime_speed",         0.0f, 300.0f);
   _data.MssACprimePause                  = config.get_float("MSS", "mss_acprime_pause",           0.0f, 15.0f);
   _data.MssPtfApsHigh                    = config.get_float("MSS", "mss_ptf_aps_high",            0.0f, 1000.0f);

   _data.MssVerifyClosureACPresureUpSpeed = config.get_float("MSS", "mss_verifyClosure_ac_pressure_up_speed",      0.0f, 100.0f);
   _data.MssVerifyClosureACSpeed          = config.get_float("MSS", "mss_verifyClosure_ac_speed",      0.0f, 100.0f);
   _data.MssVerifyClosureInletSpeed       = config.get_float("MSS", "mss_verifyClosure_inlet_speed",   0.0f, 100.0f);
   _data.MssVerifyClosureAPSDelta         = config.get_float("MSS", "mss_verifyclosure_aps_delta",          -500.0f,    500.0f);
   _data.MssVerifyClosureMaxVol           = config.get_float("MSS", "mss_verifyClosure_max_vol",        0.0f, 100.0f);

   _data.MssReturnInletMatchSpeed         = config.get_float("MSS", "mss_return_inlet_match_speed",    0.0f, 300.0f);


   _data.mssConnectionDeltaRGmax        = config.get_float("MSS", "mss_connection_deltaRGmax",  0.0f, 15.0f);
   _data.mssConnectionRGmaxVol          = config.get_float("MSS", "mss_connection_RGmax_vol",  0.0f, 80.0f);

   _data.mssPtfFilterLoss               = config.get_float("MSS", "mss_ptf_filter_loss",  0.0f, 30.0f);
   _data.mssConnectionSamples           = config.get_float("MSS", "mss_connection_samples",  0.0f, 100.0f);

   _data.MssReminderChimeTime           = config.get_float("MSS", "mss_reminder_chime_time",  0.0f, 30.0f);

   _data.PrimeHemolysisRGTrigger        = config.get_float("PRIME_HEMOLYSIS", "prime_hemolysis_rg_trigger",        0.0f,  100.0f);
   _data.PrimeHemolysisRGReturnLevel    = config.get_float("PRIME_HEMOLYSIS", "prime_hemolysis_rg_return_level",   0.0f,  6.0f);
   _data.PrimeHemolysisGreenReturnLimit = config.get_float("PRIME_HEMOLYSIS", "prime_hemolysis_green_return_limit", 0.0f,  1500.0f);


   _data.SrRecirc1Qin   = config.get_float("SALINE_RINSEBACK", "sr_recirc1_qin",  0.0f,  150.0f);
   _data.SrRecirc1Qp    = config.get_float("SALINE_RINSEBACK", "sr_recirc1_qp",   0.0f,  100.0f);
   _data.SrRecirc1Mod   = config.get_float("SALINE_RINSEBACK", "sr_recirc1_mod",  0.0f,  100.0f);
   _data.SrRecirc1Vr    = config.get_float("SALINE_RINSEBACK", "sr_recirc1_vr",   0.0f,  100.0f);
   _data.SrRecirc1Vin   = config.get_float("SALINE_RINSEBACK", "sr_recirc1_vin",  0.0f,  100.0f);

   _data.SrEvac1Qp      = config.get_float("SALINE_RINSEBACK", "sr_evac1_qp",     0.0f,  100.0f);
   _data.SrEvac1VrLimit = config.get_float("SALINE_RINSEBACK", "sr_evac1_vr_limit",     0.0f,  15.0f);
   _data.SrEvac1VpLimit = config.get_float("SALINE_RINSEBACK", "sr_evac1_vp_limit",     0.0f, 50.0f);


   _data.SrDrawVr     = config.get_float("SALINE_RINSEBACK", "sr_draw_vr",     0.0f, 100.0f);

   _data.SrRecirc2Qin = config.get_float("SALINE_RINSEBACK", "sr_recirc2_qin",     0.0f, 150.0f);
   _data.SrRecirc2Qp  = config.get_float("SALINE_RINSEBACK", "sr_recirc2_qp",     0.0f, 100.0f);
   _data.SrRecirc2Qr  = config.get_float("SALINE_RINSEBACK", "sr_recirc2_qr",     0.0f, 100.0f);
   _data.SrRecirc2Vin = config.get_float("SALINE_RINSEBACK", "sr_recirc2_vin",     0.0f, 100.0f);


   _data.SrEvac2Qp      = config.get_float("SALINE_RINSEBACK", "sr_evac2_qp",     0.0f,  100.0f);
   _data.SrEvac2VrLimit = config.get_float("SALINE_RINSEBACK", "sr_evac2_vr_limit",     0.0f,  15.0f);
   _data.SrEvac2VpLimit = config.get_float("SALINE_RINSEBACK", "sr_evac2_vp_limit",     0.0f, 50.0f);



   _data.SrFlushVr = config.get_float("SALINE_RINSEBACK", "sr_flush_vr",   0.0f,  100.0f);




   _data.SrEvac1MinVp            = config.get_float("SALINE_RINSEBACK", "sr_evac1_min_vp",   0.0f,  200.0f);
   _data.SrEvac1MaxVp            = config.get_float("SALINE_RINSEBACK", "sr_evac1_max_vp",   0.0f,  500.0f);

   _data.SrEvac2MinVp            = config.get_float("SALINE_RINSEBACK", "sr_evac2_min_vp",   0.0f,  200.0f);
   _data.SrEvac2MaxVp            = config.get_float("SALINE_RINSEBACK", "sr_evac2_max_vp",   0.0f, 500.0f);
   _data.SrTotalResidualVolume   = config.get_float("SALINE_RINSEBACK", "sr_total_residual_volume",   0.0f, 125.0f);

   _data.SrMaxReturnVolumeBuffer = config.get_float("SALINE_RINSEBACK", "sr_max_return_volume_buffer",   0.0f, 125.0f);


   _data.ResPltPlsRbc_NormalRB_RBC = config.get_float("RESIDUAL_ALGORITHM", "ResPltPlsRbc_NormalRB_RBC",   0.0f,  200.0f); // m5
   _data.ResPltPlsRbc_NormalRB_PLS = config.get_float("RESIDUAL_ALGORITHM", "ResPltPlsRbc_NormalRB_PLS",   0.0f,  200.0f); // m6
   _data.ResPltPlsRbc_NoRB_RBC     = config.get_float("RESIDUAL_ALGORITHM", "ResPltPlsRbc_NoRB_RBC",   0.0f,  200.0f);     // m7
   _data.ResPltPlsRbc_NoRB_PLS     = config.get_float("RESIDUAL_ALGORITHM", "ResPltPlsRbc_NoRB_PLS",   0.0f,  200.0f);     // m8
   _data.ResPltPlsRbc_PlasmaRB_RBC = config.get_float("RESIDUAL_ALGORITHM", "ResPltPlsRbc_PlasmaRB_RBC",   0.0f,  200.0f); // m9
   _data.ResPltPlsRbc_PlasmaRB_PLS = config.get_float("RESIDUAL_ALGORITHM", "ResPltPlsRbc_PlasmaRB_PLS",   0.0f,  200.0f); // m10
   _data.ResPlsRbc_NormalRB_RBC    = config.get_float("RESIDUAL_ALGORITHM", "ResPlsRbc_NormalRB_RBC",   0.0f,  200.0f);    // m11
   _data.ResPlsRbc_NormalRB_PLS    = config.get_float("RESIDUAL_ALGORITHM", "ResPlsRbc_NormalRB_PLS",   0.0f,  200.0f);    // m12
   _data.ResPlsRbc_NoRB_RBC        = config.get_float("RESIDUAL_ALGORITHM", "ResPlsRbc_NoRB_RBC",   0.0f,  200.0f);        // m13
   _data.ResPlsRbc_NoRB_PLS        = config.get_float("RESIDUAL_ALGORITHM", "ResPlsRbc_NoRB_PLS",   0.0f,  200.0f);        // m14
   _data.ResPlsRbc_SalineRB_RBC    = config.get_float("RESIDUAL_ALGORITHM", "ResPlsRbc_SalineRB_RBC",   0.0f,  200.0f);    // m15
   _data.ResPlsRbc_SalineRB_PLS    = config.get_float("RESIDUAL_ALGORITHM", "ResPlsRbc_SalineRB_PLS",   0.0f,  200.0f);    // m16


   _data.MaxPlateletBagVolume = config.get_float("DISPOSABLE_CONSTRAINTS", "max_Platelet_bag_Volume",   500.0f,  2400.0f);

   _data.PlasmaLargeCmdRatio  = config.get_float("PUMP_HW", "plasma_large_cmd_ratio", 0.0f, 200.0f);

   // [CONVERGENCE]
   _data.NumIterationsMidrunEpca  = config.get_int("CONVERGENCE", "num_iterations_midrun_epca",  0, 25);
   _data.NumIterationsQcmrHoldStd = config.get_int("CONVERGENCE", "num_iterations_qcmr_hold_std",  0, 25);
   _data.NumIterationsQcmrHoldPpc = config.get_int("CONVERGENCE", "num_iterations_qcmr_hold_ppc",  0, 25);
   _data.NumIterationsDeadband    = config.get_int("CONVERGENCE", "num_iterations_deadband",  0, 25);
   _data.ApsSlowDisable           = config.get_int("CONVERGENCE", "aps_slow_disable",  0, 1);
   _data.CollectVolErr            = config.get_float("CONVERGENCE", "collect_vol_err", 0.0f, 1.0f);
   _data.QchConvergenceFloor      = config.get_float("CONVERGENCE", "qch_convergence_floor", 0.5f, 15.0f);
   _data.QchDropSwitch            = config.get_int("CONVERGENCE", "qch_drop_switch", 0, 1);
   _data.QchDropLimitPPC          = config.get_float("CONVERGENCE", "qch_drop_limit_ppc", 0.5f, 15.0f);
   _data.QchDropLimitStd          = config.get_float("CONVERGENCE", "qch_drop_limit_std", 0.5f, 15.0f);
   _data.QchIterationChange       = config.get_float("CONVERGENCE", "qch_iteration_change", 0.0f, 5.0f);


   // [AMAP_PLATELETS]
   _data.YieldStepSize = config.get_float("AMAP_PLATELETS", "yield_search_step_size",  0.01f,    2.5f);

   // [PLASMA_LINE_OCCLUSION_PCA]
   _data.CVPdMin_PLO_PCA             = config.get_float("PLASMA_LINE_OCCLUSION_PCA", "CVpdmin",    0.0f, 100.0f);
   _data.CVPdMax_PLO_PCA             = config.get_float("PLASMA_LINE_OCCLUSION_PCA", "CVpdmax",    0.0f, 100.0f);
   _data.RDropThresh_PLO_PCA         = config.get_float("PLASMA_LINE_OCCLUSION_PCA", "Rdropthresh",   0.0f, 1000.0f);
   _data.PlasmaTargetMinimum_PLO_PCA = config.get_int("PLASMA_LINE_OCCLUSION_PCA", "PlasmaTargetMin", 0, 1000);


   //
   //
   //  Final status check to ensure that all values were read in properly.
   //
   DFR_ERROR configStatus = config.Error();

   if (configStatus == DFR_NOTFOUND)
   {
      status = COBE_CONFIG_VALUE_NOT_FOUND;
   }
   else if (configStatus == DFR_VALUE)
   {
      status = COBE_CONFIG_VALUE_OUT_OF_RANGE;
   }
   else if (configStatus == DFR_PARSE)
   {
      status = COBE_CONFIG_PARSE_ERROR;
   }
   _initialized = true;
   return status;
}

//
//
//  My singleton data instance
//
CobeConfig::CobeConfigStruct CobeConfig::_data = {0};
bool CobeConfig::_initialized = false;

/* FORMAT HASH 8f1c34798ebbcc82f240a4f66b5ddf83 */

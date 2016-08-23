Attribute VB_Name = "Predict60"
' Product Status
Public Enum ProductStatus60
   GOOD_PRODUCT = 0
   BAD_POSTHCT
   BAD_POSTCOUNT
   BAD_HCT_WITH_PLASMA
   YIELD_NOT_ACHIEVED
   PLASMA_NOT_ACHIEVED
   RBC_NOT_ACHIEVED
   HYPOVOLEMIA_EXCEEDED
   BAD_POSTHCT_TYPE2
   BAD_POSTCOUNT_TYPE2
   INTERNAL_ERROR
   INVALID_PRODUCT_COMBINATION
   PRODUCT_ACHIEVED
   TIME_EXCEEDED
   RBC_VBP_EXCEEDED
   BLOOD_TYPE_MISMATCH
   CASSETTE_TYPE_MISMATCH
   VBP_EXCEEDED
   REMAINING_TIME_TOO_SHORT
   DUPLICATE_PRODUCT
   QIN_BELOW_MINIMUM_SPEC
   QRP_BELOW_MINIMUM_SPEC
   INCORRECT_DESIGNATOR
   DONOR_INFO_OUT_OF_RANGE
   UNOPTIMIZED
   VIN_MAX_EXCEEDED
   VIN_MIN_NOT_MET
   CPS_PRESSURE_PRECLUDES_RBCS
   TRALI_CRITERIA_NOT_MET
   RBC_BAG_VOLUME_VIOLATION
   DRBC_VOL_LIMIT_VIOLATION
   PRODUCT_METERING_STATUS_LOCKED
End Enum

' Setup functions
Public Declare Function Optimizer_LoadOptimizerFromLog Lib "ExcelPredict.6.0.xll" Alias "Optimizer_60_LoadOptimizerFromLog" (ByVal datalog As String, ByVal cobeconfig As String) As Long
Public Declare Function Optimizer_LoadOptimizerFromConfig Lib "ExcelPredict.6.0.xll" Alias "Optimizer_60_LoadOptimizerFromConfig" (ByVal userconfig As String, ByVal cobeconfig As String) As Long
Public Declare Function Optimizer_UpdateConditions Lib "ExcelPredict.6.0.xll" Alias "Optimizer_60_UpdateConditions" () As Long
Public Declare Function Optimizer_OptimizeProduct Lib "ExcelPredict.6.0.xll" Alias "Optimizer_60_OptimizeProduct" (ByVal product As Long) As Long

' Condition functions
Public Declare Function Condition_id Lib "ExcelPredict.6.0.xll" Alias "Condition_60_id" (ByVal value As Long) As Long
Public Declare Function Condition_height Lib "ExcelPredict.6.0.xll" Alias "Condition_60_height" (ByVal value As Double) As Double
Public Declare Function Condition_weight Lib "ExcelPredict.6.0.xll" Alias "Condition_60_weight" (ByVal value As Double) As Double
Public Declare Function Condition_tbv Lib "ExcelPredict.6.0.xll" Alias "Condition_60_tbv" (ByVal value As Double) As Double
Public Declare Function Condition_precount Lib "ExcelPredict.6.0.xll" Alias "Condition_60_precount" (ByVal value As Double) As Double
Public Declare Function Condition_hct Lib "ExcelPredict.6.0.xll" Alias "Condition_60_hct" (ByVal value As Double) As Double
Public Declare Function Condition_bmi Lib "ExcelPredict.6.0.xll" Alias "Condition_60_bmi" (ByVal value As Double) As Double
Public Declare Function Condition_female Lib "ExcelPredict.6.0.xll" Alias "Condition_60_female" (ByVal value As Long) As Long
Public Declare Function Condition_blood_type Lib "ExcelPredict.6.0.xll" Alias "Condition_60_blood_type" (ByVal value As Long) As Long
Public Declare Function Condition_vin Lib "ExcelPredict.6.0.xll" Alias "Condition_60_vin" (ByVal value As Double) As Double
Public Declare Function Condition_vac Lib "ExcelPredict.6.0.xll" Alias "Condition_60_vac" (ByVal value As Double) As Double
Public Declare Function Condition_vplasma_bag Lib "ExcelPredict.6.0.xll" Alias "Condition_60_vplasma_bag" (ByVal value As Double) As Double
Public Declare Function Condition_vplasma_bag_ac Lib "ExcelPredict.6.0.xll" Alias "Condition_60_vplasma_bag_ac" (ByVal value As Double) As Double
Public Declare Function Condition_vcollect_bag Lib "ExcelPredict.6.0.xll" Alias "Condition_60_vcollect_bag" (ByVal value As Double) As Double
Public Declare Function Condition_vcollect_bag_ac Lib "ExcelPredict.6.0.xll" Alias "Condition_60_vcollect_bag_ac" (ByVal value As Double) As Double
Public Declare Function Condition_vrbc_bag Lib "ExcelPredict.6.0.xll" Alias "Condition_60_vrbc_bag" (ByVal value As Double) As Double
Public Declare Function Condition_vrbc_bag_ac Lib "ExcelPredict.6.0.xll" Alias "Condition_60_vrbc_bag_ac" (ByVal value As Double) As Double
Public Declare Function Condition_dose_rbc_bag Lib "ExcelPredict.6.0.xll" Alias "Condition_60_dose_rbc_bag" (ByVal value As Double) As Double
Public Declare Function Condition_vrep Lib "ExcelPredict.6.0.xll" Alias "Condition_60_vrep" (ByVal value As Double) As Double
Public Declare Function Condition_vsaline_bolus Lib "ExcelPredict.6.0.xll" Alias "Condition_60_vsaline_bolus" (ByVal value As Double) As Double
Public Declare Function Condition_platelet_yield Lib "ExcelPredict.6.0.xll" Alias "Condition_60_platelet_yield" (ByVal value As Double) As Double
Public Declare Function Condition_time_of_vincv Lib "ExcelPredict.6.0.xll" Alias "Condition_60_time_of_vincv" (ByVal value As Double) As Double
Public Declare Function Condition_vincv Lib "ExcelPredict.6.0.xll" Alias "Condition_60_vincv" (ByVal value As Double) As Double
Public Declare Function Condition_vin_epca_start Lib "ExcelPredict.6.0.xll" Alias "Condition_60_vin_epca_start" (ByVal value As Double) As Double
Public Declare Function Condition_rbc_s1 Lib "ExcelPredict.6.0.xll" Alias "Condition_60_rbc_s1" (ByVal value As Double) As Double
Public Declare Function Condition_rbc_s2 Lib "ExcelPredict.6.0.xll" Alias "Condition_60_rbc_s2" (ByVal value As Double) As Double
Public Declare Function Condition_rbc_prime Lib "ExcelPredict.6.0.xll" Alias "Condition_60_rbc_prime" (ByVal value As Double) As Double
Public Declare Function Condition_rbc_s1_double Lib "ExcelPredict.6.0.xll" Alias "Condition_60_rbc_s1_double" (ByVal value As Double) As Double
Public Declare Function Condition_rbc_s2_double Lib "ExcelPredict.6.0.xll" Alias "Condition_60_rbc_s2_double" (ByVal value As Double) As Double
Public Declare Function Condition_rbc_double Lib "ExcelPredict.6.0.xll" Alias "Condition_60_rbc_double" (ByVal value As Double) As Double
Public Declare Function Condition_substate Lib "ExcelPredict.6.0.xll" Alias "Condition_60_substate" (ByVal value As Long) As Long
Public Declare Function Condition_qch_min Lib "ExcelPredict.6.0.xll" Alias "Condition_60_qch_min" (ByVal value As Double) As Double
Public Declare Function Condition_qch_cur Lib "ExcelPredict.6.0.xll" Alias "Condition_60_qch_cur" (ByVal value As Double) As Double
Public Declare Function Condition_npurges Lib "ExcelPredict.6.0.xll" Alias "Condition_60_npurges" (ByVal value As Long) As Long
Public Declare Function Condition_adjusted_hct Lib "ExcelPredict.6.0.xll" Alias "Condition_60_adjusted_hct" (ByVal value As Double) As Double
Public Declare Function Condition_current_recovery Lib "ExcelPredict.6.0.xll" Alias "Condition_60_current_recovery" (ByVal value As Long) As Long
Public Declare Function Condition_vin_clearline Lib "ExcelPredict.6.0.xll" Alias "Condition_60_vin_clearline" (ByVal value As Double) As Double
Public Declare Function Condition_wbc_purge_start_time Lib "ExcelPredict.6.0.xll" Alias "Condition_60_wbc_purge_start_time" (ByVal value As Long) As Long
Public Declare Function Condition_wbc_purge_type Lib "ExcelPredict.6.0.xll" Alias "Condition_60_wbc_purge_type" (ByVal value As Long) As Long
Public Declare Function Condition_proctime Lib "ExcelPredict.6.0.xll" Alias "Condition_60_proctime" (ByVal value As Double) As Double
Public Declare Function Condition_type Lib "ExcelPredict.6.0.xll" Alias "Condition_60_type" (ByVal value As Long) As Long
Public Declare Function Condition_rbc_detector_calibrated Lib "ExcelPredict.6.0.xll" Alias "Condition_60_rbc_detector_calibrated" (ByVal value As Boolean) As Boolean
Public Declare Function Condition_qin Lib "ExcelPredict.6.0.xll" Alias "Condition_60_qin" (ByVal value As Double) As Double
Public Declare Function Condition_qrp Lib "ExcelPredict.6.0.xll" Alias "Condition_60_qrp" (ByVal value As Double) As Double
Public Declare Function Condition_ir Lib "ExcelPredict.6.0.xll" Alias "Condition_60_ir" (ByVal value As Double) As Double
Public Declare Function Condition_ratio Lib "ExcelPredict.6.0.xll" Alias "Condition_60_ratio" (ByVal value As Double) As Double

' Predict functions
Public Declare Function Predict_RBCState Lib "ExcelPredict.6.0.xll" Alias "Predict_60_RBCState" (ByVal value As Long) As Long
Public Declare Function Predict_RBCDoubleState Lib "ExcelPredict.6.0.xll" Alias "Predict_60_RBCDoubleState" (ByVal value As Long) As Long
Public Declare Function Predict_NeedleFlowCoefficient Lib "ExcelPredict.6.0.xll" Alias "Predict_60_NeedleFlowCoefficient" (ByVal value As Long) As Double
Public Declare Function Predict_VinTarget Lib "ExcelPredict.6.0.xll" Alias "Predict_60_VinTarget" (ByVal value As Long) As Double
Public Declare Function Predict_VinPIRStart Lib "ExcelPredict.6.0.xll" Alias "Predict_60_VinPIRStart" (ByVal value As Long) As Double
Public Declare Function Predict_Ydch Lib "ExcelPredict.6.0.xll" Alias "Predict_60_Ydch" (ByVal value As Long) As Double
Public Declare Function Predict_YdchPIR Lib "ExcelPredict.6.0.xll" Alias "Predict_60_YdchPIR" (ByVal value As Long) As Double
Public Declare Function Predict_getPIRLockin Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getPIRLockin" (ByVal value As Long) As Double
Public Declare Function Predict_totalPurges Lib "ExcelPredict.6.0.xll" Alias "Predict_60_totalPurges" (ByVal value As Long) As Double
Public Declare Function Predict_GetPIRRequired Lib "ExcelPredict.6.0.xll" Alias "Predict_60_GetPIRRequired" (ByVal value As Long) As Long
Public Declare Function Predict_doingPTFDouble Lib "ExcelPredict.6.0.xll" Alias "Predict_60_doingPTFDouble" (ByVal value As Long) As Boolean
Public Declare Function Predict_secondPartofPTFDouble Lib "ExcelPredict.6.0.xll" Alias "Predict_60_secondPartofPTFDouble" (ByVal value As Long) As Boolean
Public Declare Function Predict_getNumOfSubstates Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getNumOfSubstates" (ByVal value As Long) As Long
Public Declare Function Predict_getExtracorporealVolume Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getExtracorporealVolume" (ByVal value As Long) As Double
Public Declare Function Predict_getResidualRBCVolume Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getResidualRBCVolume" (ByVal value As Long) As Double
Public Declare Function Predict_getPrimeDuration Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getPrimeDuration" (ByVal value As Long) As Double
Public Declare Function Predict_getVinPrime Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getVinPrime" (ByVal value As Long) As Double
Public Declare Function Predict_getnumOfTimes Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getnumOfTimes" (ByVal value As Long) As Long
Public Declare Function Predict_getconvergenceCtr Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getconvergenceCtr" (ByVal value As Long) As Long
Public Declare Function Predict_getProcedureNumber Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getProcedureNumber" (ByVal value As Long) As Long
Public Declare Function Predict_getDesignator Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getDesignator" (ByVal value As Long) As Long
Public Declare Function Predict_getProcAMAPorMC Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getProcAMAPorMC" (ByVal value As Long) As Long
Public Declare Function Predict_getProcBloodType Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getProcBloodType" (ByVal value As Long) As Long
Public Declare Function Predict_getProcCollectPlasma Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getProcCollectPlasma" (ByVal value As Long) As Boolean
Public Declare Function Predict_getProcCollectVolFixed Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getProcCollectVolFixed" (ByVal value As Long) As Long
Public Declare Function Predict_getProcFixedCollectVol Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getProcFixedCollectVol" (ByVal value As Long) As Double
Public Declare Function Predict_getProcFixedPlasmaVol Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getProcFixedPlasmaVol" (ByVal value As Long) As Double
Public Declare Function Predict_getProcPlasmaVolFixed Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getProcPlasmaVolFixed" (ByVal value As Long) As Long
Public Declare Function Predict_getProcPlateletProdCon Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getProcPlateletProdCon" (ByVal value As Long) As Double
Public Declare Function Predict_getProcRBCDose Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getProcRBCDose" (ByVal value As Long) As Double
Public Declare Function Predict_getProcRBCHct Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getProcRBCHct" (ByVal value As Long) As Double
Public Declare Function Predict_getProcRBCVolume Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getProcRBCVolume" (ByVal value As Long) As Long
Public Declare Function Predict_getProcPtfIsSplit Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getProcPtfIsSplit" (ByVal value As Long) As Boolean
Public Declare Function Predict_getProcTime Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getProcTime" (ByVal value As Long) As Double
Public Declare Function Predict_getDesiredYield Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getDesiredYield" (ByVal value As Long) As Double
Public Declare Function Predict_getMSSPlt Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getMSSPlt" (ByVal value As Long) As Boolean
Public Declare Function Predict_getMSSPltVolume Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getMSSPltVolume" (ByVal value As Long) As Long
Public Declare Function Predict_getMSSPltPctCarryover Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getMSSPltPctCarryover" (ByVal value As Long) As Long
Public Declare Function Predict_getOriginalPltVolume Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getOriginalPltVolume" (ByVal value As Long) As Double
Public Declare Function Predict_getOriginalPltYeild Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getOriginalPltYeild" (ByVal value As Long) As Double
Public Declare Function Predict_getMSSRbc Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getMSSRbc" (ByVal value As Long) As Boolean
Public Declare Function Predict_getMSSRbcVolume Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getMSSRbcVolume" (ByVal value As Long) As Long
Public Declare Function Predict_getMaxPlasma Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getMaxPlasma" (ByVal value As Long) As Double
Public Declare Function Predict_hypovolemic Lib "ExcelPredict.6.0.xll" Alias "Predict_60_hypovolemic" (ByVal value As Long) As Long
Public Declare Function Predict_hypovolemiaDistance Lib "ExcelPredict.6.0.xll" Alias "Predict_60_hypovolemiaDistance" (ByVal value As Long) As Double
Public Declare Function Predict_getHypoMargin Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getHypoMargin" (ByVal value As Long) As Double
Public Declare Function Predict_getPlasmaTargetCannotChange Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getPlasmaTargetCannotChange" (ByVal value As Long) As Long
Public Declare Function Predict_getMaxACRate Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getMaxACRate" (ByVal value As Long) As Double
Public Declare Function Predict_getPredRBCDose Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getPredRBCDose" (ByVal value As Long) As Double
Public Declare Function Predict_getPredRBCVolume Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getPredRBCVolume" (ByVal value As Long) As Double
Public Declare Function Predict_getPredRBCCrit Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getPredRBCCrit" (ByVal value As Long) As Double
Public Declare Function Predict_getRBCCalculatedCrit Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getRBCCalculatedCrit" (ByVal value As Long) As Double
Public Declare Function Predict_getStatus Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getStatus" (ByVal value As Long) As Long
Public Declare Function Predict_ggetOptimized Lib "ExcelPredict.6.0.xll" Alias "Predict_60_ggetOptimized" (ByVal value As Long) As Boolean
Public Declare Function Predict_getMaxQin Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getMaxQin" (ByVal value As Long) As Double
Public Declare Function Predict_getPredVc Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getPredVc" (ByVal value As Long) As Double
Public Declare Function Predict_getPredVp Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getPredVp" (ByVal value As Long) As Double
Public Declare Function Predict_getPredVac Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getPredVac" (ByVal value As Long) As Double
Public Declare Function Predict_getPredVrep Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getPredVrep" (ByVal value As Long) As Double
Public Declare Function Predict_getPredTp Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getPredTp" (ByVal value As Long) As Double
Public Declare Function Predict_getI Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getI" (ByVal value As Long) As Double
Public Declare Function Predict_getTc Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getTc" (ByVal value As Long) As Double
Public Declare Function Predict_getYtarg Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getYtarg" (ByVal value As Long) As Double
Public Declare Function Predict_getEc Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getEc" (ByVal value As Long) As Double
Public Declare Function Predict_getQinc Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getQinc" (ByVal value As Long) As Double
Public Declare Function Predict_getYdpur Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getYdpur" (ByVal value As Long) As Double
Public Declare Function Predict_getVintarg Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getVintarg" (ByVal value As Long) As Double
Public Declare Function Predict_getVptarg Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getVptarg" (ByVal value As Long) As Double
Public Declare Function Predict_getVrtarg Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getVrtarg" (ByVal value As Long) As Double
Public Declare Function Predict_getQcmr Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getQcmr" (ByVal value As Long) As Double
Public Declare Function Predict_getVpmin Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getVpmin" (ByVal value As Long) As Double
Public Declare Function Predict_getVinpur Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getVinpur" (ByVal value As Long) As Double
Public Declare Function Predict_getYdch Lib "ExcelPredict.6.0.xll" Alias "Predict_60_getYdch" (ByVal value As Long) As Double

' Results functions
Public Declare Function Substate_Qcc Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Qcc" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Qpc Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Qpc" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_T Lib "ExcelPredict.6.0.xll" Alias "Substate_60_T" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Qnret Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Qnret" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Vsvn Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Vsvn" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Tcn Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Tcn" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Td Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Td" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Tr Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Tr" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_D Lib "ExcelPredict.6.0.xll" Alias "Substate_60_D" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Qrinst Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Qrinst" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_EndVin Lib "ExcelPredict.6.0.xll" Alias "Substate_60_EndVin" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Vp Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Vp" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Vacp Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Vacp" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Vc Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Vc" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Vacc Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Vacc" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Vr Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Vr" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Vacr Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Vacr" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_AccumVp Lib "ExcelPredict.6.0.xll" Alias "Substate_60_AccumVp" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_AccumVacp Lib "ExcelPredict.6.0.xll" Alias "Substate_60_AccumVacp" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_AccumVc Lib "ExcelPredict.6.0.xll" Alias "Substate_60_AccumVc" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_AccumVacc Lib "ExcelPredict.6.0.xll" Alias "Substate_60_AccumVacc" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_AccumTime Lib "ExcelPredict.6.0.xll" Alias "Substate_60_AccumTime" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Vac Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Vac" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_AccumVac Lib "ExcelPredict.6.0.xll" Alias "Substate_60_AccumVac" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Vrep Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Vrep" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_AccumVrep Lib "ExcelPredict.6.0.xll" Alias "Substate_60_AccumVrep" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Qrep Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Qrep" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Hin Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Hin" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Facb Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Facb" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Facp Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Facp" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_AvgHt Lib "ExcelPredict.6.0.xll" Alias "Substate_60_AvgHt" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Hend Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Hend" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Qininstd Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Qininstd" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Qininstr Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Qininstr" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Qacinst Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Qacinst" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Qcinstd Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Qcinstd" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Qcinstr Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Qcinstr" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Qpinstd Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Qpinstd" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Qpinstr Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Qpinstr" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_Qch Lib "ExcelPredict.6.0.xll" Alias "Substate_60_Qch" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_HypovolemiaDistance Lib "ExcelPredict.6.0.xll" Alias "Substate_60_HypovolemiaDistance" (ByVal product As Long, ByVal index As Long) As Double
Public Declare Function Substate_HypovolemiaMargin Lib "ExcelPredict.6.0.xll" Alias "Substate_60_HypovolemiaMargin" (ByVal product As Long, ByVal index As Long) As Double




Public Declare Function Optimizer_63_LoadOptimizerFromLog Lib "ExcelPredict.6.3.xll" (ByVal datalog As String, ByVal cobeconfig As String) As Long
Public Declare Function Optimizer_63_LoadOptimizerFromConfig Lib "ExcelPredict.6.3.xll" (ByVal userconfig As String, ByVal cobeconfig As String) As Long
Public Declare Function Optimizer_63_LoadOptimizerFromConfigs Lib "ExcelPredict.6.3.xll" (ByVal userconfig As String, ByVal cobeconfig As String, ByVal softwareconfig As String) As Long
Public Declare Function Optimizer_63_UpdateConditions Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Optimizer_63_UpdateConfigurations Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Optimizer_63_OptimizeProduct Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function SoftwareOptions_63_SetOption Lib "ExcelPredict.6.3.xll" (ByVal opt As Long, ByVal enabled As Long) As Long
Public Declare Function SoftwareOptions_63_GetOption Lib "ExcelPredict.6.3.xll" () As Long

Public Declare Function Predict_63_Get_RBCState Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_RBCDoubleState Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_VinTarget Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_VinPIRStart Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_YdchPIR Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_PIRLockin Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_totalPurges Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_PIRRequired Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_doingPTFDouble Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Boolean
Public Declare Function Predict_63_Get_secondPartofPTFDouble Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Boolean
Public Declare Function Predict_63_Get_NumOfSubstates Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_ExtracorporealVolume Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_ResidualRBCVolume Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_PrimeDuration Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_VinPrime Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_numOfTimes Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_convergenceCtr Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_ProcedureNumber Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_Designator Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_ProcAMAPorMC Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_ProcBloodType Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_ProcCollectPlasma Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Boolean
Public Declare Function Predict_63_Get_ProcCollectVolFixed Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_ProcFixedCollectVol Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_ProcFixedPlasmaVol Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_ProcPlasmaVolFixed Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_ProcPlateletProdCon Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_ProcRBCDose Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_ProcRBCHct Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_ProcRBCVolume Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_ProcPtfIsSplit Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Boolean
Public Declare Function Predict_63_Get_ProcTime Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_DesiredYield Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_MSSPlt Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Boolean
Public Declare Function Predict_63_Get_MSSPltVolume Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_MSSPltPctCarryover Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_OriginalPltVolume Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_OriginalPltYeild Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_MSSRbc Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Boolean
Public Declare Function Predict_63_Get_MSSRbcVolume Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_MaxPlasma Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_hypovolemic Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_hypovolemiaDistance Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_HypoMargin Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_PlasmaTargetCannotChange Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_MaxACRate Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_PredRBCDose Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_PredRBCVolume Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_PredRBCCrit Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_RBCCalculatedCrit Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_Status Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Long
Public Declare Function Predict_63_Get_Optimized Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Boolean
Public Declare Function Predict_63_Get_MaxQin Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_PredVc Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_PredVp Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_PredVac Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_PredVrep Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_PredTp Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_InfusionRate Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_Tc Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_Ytarg Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_Ec Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_Qinc Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_Ydpur Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_Vintarg Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_Vptarg Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_Vrtarg Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_Qcmr Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_Vpmin Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_Vinpur Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Predict_63_Get_Ydch Lib "ExcelPredict.6.3.xll" (ByVal product As Long) As Single
Public Declare Function Config_63_Get_key_lang Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_lang Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_height Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_height Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_weight Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_weight Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_date_format Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_date_format Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_time_format Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_time_format Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_decimal_delimiter Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_decimal_delimiter Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_crit_or_glob Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_crit_or_glob Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_set_time Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_set_time Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_set_date Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_set_date Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_set_audio Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_set_audio Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_pword Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_pword Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_proc_time Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_proc_time Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_return_press Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_return_press Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_draw_press Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_draw_press Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_custom_ratio Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_custom_ratio Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_plasma_ratio Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_plasma_ratio Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_ac_rate Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_ac_rate Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_post_crit Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_post_crit Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_tbv_vol_setting Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_tbv_vol_setting Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_tbv_percent Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_tbv_percent Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_weight_setting Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_weight_setting Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_weight_less_than_vol Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_weight_less_than_vol Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_weight_greater_than_vol Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_weight_greater_than_vol Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_ml_per_kg Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_ml_per_kg Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_drbc_body_vol Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_drbc_body_vol Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_max_plasma_vol_during_plasma_collection Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_max_plasma_vol_during_plasma_collection Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_max_plasma_vol_during_platelet_collection Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_max_plasma_vol_during_platelet_collection Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_post_plat Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_post_plat Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_max_draw_flow Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_max_draw_flow Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_min_replacement_volume Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_min_replacement_volume Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_plt_mss_split_notif Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_plt_mss_split_notif Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_override_pas_bag_vol Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_override_pas_bag_vol Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_drbc_alert Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_drbc_alert Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_drbc_threshold Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_drbc_threshold Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_plasma_rinseback Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_plasma_rinseback Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_saline_rinseback Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_saline_rinseback Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_audit_tracking Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_audit_tracking Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_air_removal Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_air_removal Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_blood_diversion Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_blood_diversion Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_mss_plt_on Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_mss_plt_on Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_mss_rbc_on Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_mss_rbc_on Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_show_pre_aas_flags Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_show_pre_aas_flags Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_plt_def_bag_vol Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_plt_def_bag_vol Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_rbc_def_bag_vol Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_rbc_def_bag_vol Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_rbc_ratio Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_rbc_ratio Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_rbc_replace_fluid Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_rbc_replace_fluid Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_rbc_fluid_percent Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_rbc_fluid_percent Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_pls_amap_min Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_pls_amap_min Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_pls_amap_max Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_pls_amap_max Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_ysf Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_ysf Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_guaranteed_yield Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_guaranteed_yield Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_confidence_level Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_confidence_level Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_coefficient_of_variation Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_coefficient_of_variation Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_inlet_flow_ramp Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_inlet_flow_ramp Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_inlet_management Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_inlet_management Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_return_management Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_return_management Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_rsf Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Config_63_Set_key_rsf Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_male_only_plt Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_male_only_plt Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_male_only_plasma Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_male_only_plasma Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_complementary_plasma Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_complementary_plasma Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_ffp_volume Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Config_63_Set_key_ffp_volume Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_rbc_crit Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Single
Public Declare Function Config_63_Set_key_rbc_crit Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_rbc_dose Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Single
Public Declare Function Config_63_Set_key_rbc_dose Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_rbc_mss Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Long
Public Declare Function Config_63_Set_key_rbc_mss Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_rbc_mss_volume Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Long
Public Declare Function Config_63_Set_key_rbc_mss_volume Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_plt_amap_min_yield Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Single
Public Declare Function Config_63_Set_key_plt_amap_min_yield Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_plt_amap_max_yield Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Single
Public Declare Function Config_63_Set_key_plt_amap_max_yield Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_plt_amap_conc Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Single
Public Declare Function Config_63_Set_key_plt_amap_conc Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_plt_yield Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Single
Public Declare Function Config_63_Set_key_plt_yield Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_plt_volume Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Single
Public Declare Function Config_63_Set_key_plt_volume Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_plt_mss Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Long
Public Declare Function Config_63_Set_key_plt_mss Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_plt_pct_carryover Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Long
Public Declare Function Config_63_Set_key_plt_pct_carryover Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_pls_volume Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Single
Public Declare Function Config_63_Set_key_pls_volume Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Single) As Single
Public Declare Function Config_63_Get_key_platelet Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Long
Public Declare Function Config_63_Set_key_platelet Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_plasma Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Long
Public Declare Function Config_63_Set_key_plasma Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_rbc Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Long
Public Declare Function Config_63_Set_key_rbc Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_blood_type Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Long
Public Declare Function Config_63_Set_key_blood_type Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Long) As Long
Public Declare Function Config_63_Get_key_time Lib "ExcelPredict.6.3.xll" (ByVal index As Long) As Single
Public Declare Function Config_63_Set_key_time Lib "ExcelPredict.6.3.xll" (ByVal index As Long, ByVal value As Single) As Single
Public Declare Function Substate_63_Get_Qcc Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Qpc Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_T Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Qnret Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Vsvn Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Tcn Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Td Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Tr Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_D Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Qrinst Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_EndVin Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Vp Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Vacp Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Vc Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Vacc Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Vr Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Vacr Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_AccumVp Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_AccumVacp Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_AccumVc Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_AccumVacc Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_AccumTime Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Vac Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_AccumVac Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Vrep Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_AccumVrep Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Qrep Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Hin Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Facb Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Facp Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_AvgHt Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Hend Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Qininstd Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Qininstr Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Qacinst Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Qcinstd Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Qcinstr Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Qpinstd Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Qpinstr Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Qch Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_HypovolemiaDistance Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_HypovolemiaMargin Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Yield Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Csf Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Substate_63_Get_Rpm Lib "ExcelPredict.6.3.xll" (ByVal product As Long, ByVal index As Long) As Single
Public Declare Function Condition_63_Get_id Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Condition_63_Set_id Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Condition_63_Get_height Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_height Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_weight Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_weight Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_tbv Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_tbv Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_precount Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_precount Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_hct Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_hct Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_bmi Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_bmi Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_female Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Condition_63_Set_female Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Condition_63_Get_blood_type Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Condition_63_Set_blood_type Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Condition_63_Get_vin Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_vin Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_vac Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_vac Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_vplasma_bag Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_vplasma_bag Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_vplasma_bag_ac Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_vplasma_bag_ac Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_vcollect_bag Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_vcollect_bag Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_vcollect_bag_ac Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_vcollect_bag_ac Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_vrbc_bag Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_vrbc_bag Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_vrbc_bag_ac Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_vrbc_bag_ac Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_dose_rbc_bag Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_dose_rbc_bag Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_vrep Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_vrep Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_vsaline_bolus Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_vsaline_bolus Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_platelet_yield Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_platelet_yield Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_time_of_vincv Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_time_of_vincv Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_vincv Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_vincv Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_vin_epca_start Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_vin_epca_start Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_rbc_s1 Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_rbc_s1 Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_rbc_s2 Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_rbc_s2 Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_rbc_prime Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_rbc_prime Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_rbc_s1_double Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_rbc_s1_double Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_rbc_s2_double Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_rbc_s2_double Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_rbc_double Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_rbc_double Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_substate Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Condition_63_Set_substate Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Condition_63_Get_qch_min Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_qch_min Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_qch_cur Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_qch_cur Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_npurges Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Condition_63_Set_npurges Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Condition_63_Get_adjusted_hct Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_adjusted_hct Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_current_recovery Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Condition_63_Set_current_recovery Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Condition_63_Get_vin_clearline Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_vin_clearline Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_wbc_purge_start_time Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_wbc_purge_start_time Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_wbc_purge_type Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Condition_63_Set_wbc_purge_type Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Condition_63_Get_substate_start_time Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_substate_start_time Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_midrun_entered Lib "ExcelPredict.6.3.xll" () As short
Public Declare Function Condition_63_Set_midrun_entered Lib "ExcelPredict.6.3.xll" (ByVal value As short) As short
Public Declare Function Condition_63_Get_proctime Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_proctime Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_type Lib "ExcelPredict.6.3.xll" () As Long
Public Declare Function Condition_63_Set_type Lib "ExcelPredict.6.3.xll" (ByVal value As Long) As Long
Public Declare Function Condition_63_Get_rbc_detector_calibrated Lib "ExcelPredict.6.3.xll" () As Boolean
Public Declare Function Condition_63_Set_rbc_detector_calibrated Lib "ExcelPredict.6.3.xll" (ByVal value As Boolean) As Boolean
Public Declare Function Condition_63_Get_qin Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_qin Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_qrp Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_qrp Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_ir Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_ir Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single
Public Declare Function Condition_63_Get_ratio Lib "ExcelPredict.6.3.xll" () As Single
Public Declare Function Condition_63_Set_ratio Lib "ExcelPredict.6.3.xll" (ByVal value As Single) As Single

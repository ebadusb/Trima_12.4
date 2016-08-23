VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "C61"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Function LoadOptimizerFromConfigs(datalog As String, cobeconfig As String, softwareconfig As String)
    LoadOptimizerFromConfigs = Optimizer_61_LoadOptimizerFromConfigs(datalog, cobeconfig, softwareconfig)
    LoadOptimizerFromConfigs = 1
End Function
Function UpdateOptimizerConditions()
    UpdateOptimizerConditions = Optimizer_61_UpdateConditions
End Function
Function OptimizeProduct(productindex)
    Optimizer_61_OptimizeProduct productindex
    'Debug.Print Optimizer_OptimizeProduct(productindex)
End Function
Function tbv(value As Double)
    Condition_61_Set_tbv value
    'Debug.Print Condition_Get_tbv
End Function
Function precount(value As Double)
   Condition_61_Set_precount value
End Function
Function hct(value As Double)
   Condition_61_Set_hct value
End Function
Function height(value As Double)
   Condition_61_Set_height value
End Function
Function weight(value As Double)
   Condition_61_Set_weight value
End Function
Function blood_type(value As Double)
   Condition_61_Set_blood_type value
End Function
Function female(value As Double)
   Condition_61_Set_female value
End Function
Public Function getStatus(value As Integer)
    getStatus = Predict_61_Get_Status(value)
End Function
Function getDesiredYield(productindex)
    getDesiredYield = Predict_61_Get_DesiredYield(productindex)
End Function
Function getPredVc(productindex)
    getPredVc = Predict_61_Get_PredVc(productindex)
End Function
Function getProcFixedPlasmaVol(productindex)
    getProcFixedPlasmaVol = Predict_61_Get_ProcFixedPlasmaVol(productindex)
End Function
Function getPredVp(productindex)
    getPredVp = Predict_61_Get_PredVp(productindex)
End Function
Function getProcRBCVolume(productindex)
    getProcRBCVolume = Predict_61_Get_ProcRBCVolume(productindex)
End Function
Function getPredTp(productindex)
    getPredTp = Predict_61_Get_PredTp(productindex)
End Function
Function getTc(productindex)
    getTc = Predict_61_Get_Tc(productindex)
End Function
Function getQinc(productindex)
    getQinc = Predict_61_Get_Qinc(productindex)
End Function
Function getQcmr(productindex)
    getQcmr = Predict_61_Get_Qcmr(productindex)
End Function
Function getMaxQin(productindex)
    getMaxQin = Predict_61_Get_MaxQin(productindex)
End Function
Function S_Qrinst(productindex, ss)
    S_Qrinst = Substate_61_Get_Qrinst(productindex, ss)
End Function
Function S_Qininstr(productindex, ss)
    S_Qininstr = Substate_61_Get_Qininstr(productindex, ss)
End Function
Function S_Qininstd(productindex, ss)
    S_Qininstd = Substate_61_Get_Qininstd(productindex, ss)
End Function
Function VinTarget(productindex)
    VinTarget = Predict_61_Get_VinTarget(productindex)
End Function
Function S_EndVin(productindex, ss)
    S_EndVin = Substate_61_Get_EndVin(productindex, ss)
End Function
Function getNumOfSubstates(productindex)
    getNumOfSubstates = Predict_61_Get_NumOfSubstates(productindex)
End Function
Function getPredVac(productindex)
    getPredVac = Predict_61_Get_PredVac(productindex)
End Function
Function getProcPlateletProdCon(productindex)
    getProcPlateletProdCon = Predict_61_Get_ProcPlateletProdCon(productindex)
End Function
Function getPredRBCDose(productindex)
    getPredRBCDose = Predict_61_Get_PredRBCDose(productindex)
End Function
Function getResidualRBCVolume(productindex)
    getResidualRBCVolume = Predict_61_Get_ResidualRBCVolume(productindex)
End Function
Function S_T(productindex, ss)
    S_T = Substate_61_Get_T(productindex, ss)
End Function
Function totalPurges(productindex)
    totalPurges = Predict_61_Get_totalPurges(productindex)
End Function
Function getYdpur(productindex)
    getYdpur = Predict_61_Get_Ydpur(productindex)
End Function
Function getYdch(productindex)
    getYdch = Predict_61_Get_Ydch(productindex)
End Function
Function YdchPIR(productindex)
    YdchPIR = Predict_61_Get_YdchPIR(productindex)
End Function
Function S_Qacinst(productindex, ss)
    S_Qacinst = Substate_61_Get_Qacinst(productindex, ss)
End Function
Function S_Qch(productindex, ss)
    S_Qch = Substate_61_Get_Qch(productindex, ss)
End Function
Function RBCState(productindex)
    RBCState = Predict_61_Get_RBCState(productindex)
End Function
Function HypoDistance(productindex)
    HypoDistance = Predict_61_Get_hypovolemiaDistance(productindex)
End Function

Function Set_key_plt_yield(productindex, Yt) ' Sets Ytarg for platelet def.
    Set_key_plt_yield = Config_61_Set_key_plt_yield(productindex, Yt)
End Function
Function Set_key_plt_volume(productindex, Vt) ' Sets Vtarg for platelet def.
    Set_key_plt_volume = Config_61_Set_key_plt_volume(productindex, Vt)
End Function
Function Set_key_pls_volume(productindex, Vt) ' Sets Vtarg for pls def.
    Set_key_pls_volume = Config_61_Set_key_pls_volume(productindex, Vt)
End Function
Function Set_key_rbc_dose(productindex, dose) ' Sets Vtarg for rbc def.
    Set_key_rbc_dose = Config_61_Set_key_rbc_dose(productindex, dose)
End Function
Function Set_key_platelet(procindex, pltindex) ' Sets product def. for procedure
    Set_key_platelet = Config_61_Set_key_platelet(procindex, pltindex)
End Function
Function Set_key_plasma(procindex, plsindex) ' Sets product def. for procedure
    Set_key_plasma = Config_61_Set_key_plasma(procindex, plsindex)
End Function
Function Set_key_rbc(procindex, rbcindex) ' Sets product def. for procedure
    Set_key_rbc = Config_61_Set_key_rbc(procindex, rbcindex)
End Function
    
Function UpdateConfigurations()
    'UpdateConfigurations =
    Optimizer_61_UpdateConfigurations
End Function

Function get_Vinpur(productindex)
    get_Vinpur = Predict_61_Get_Vinpur(productindex)
End Function
Function get_bmi()
    get_bmi = Condition_61_Get_bmi
End Function

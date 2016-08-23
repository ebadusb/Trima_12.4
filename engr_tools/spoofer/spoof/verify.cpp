#include "verify.hpp"
#include <string.h>

// search the list of supported objects
// note that this list must be maintained in addition to SpoofedObject

bool verify::isSupportedObject(const char* objectName) {
    char* supportedObjectList[] = {
        "CHwOrders","CHwStates","FastData","RW_FastData","CassettePositionStatusMsg","CentrifugePressureStatusMsg","CentrifugeStopMsg",
        "ConfigChangedMsg","DateTimeSetMsg","DeleteProductMsg","DisconnectCompleteMsg","DisposableIDMsg","ProcStateMsg","SystemStateChangeMsg",
        "RequestSafetyPowerMsg","APSZeroMsg","ButtonPressedIDMsg","DonorDataStatusMsg","DVitalsFromGUI","Predict_SubstateInfo","HalStatus_aps",
        "HalStatus_cps","HalStatus_inletCmdRpm","HalStatus_acCmdRpm","HalStatus_returnCmdRpm","HalStatus_plateletCmdRpm","HalStatus_plasmaCmdRpm",
        "HalStatus_inletCmdFlow","HalStatus_acCmdFlow","HalStatus_returnCmdFlow","HalStatus_plateletCmdFlow","HalStatus_plasmaCmdFlow",
        "HalStatus_doorStatus","HalStatus_RBCValveStatus","HalStatus_PlasmaValveStatus","HalStatus_CollectValveStatus","HalStatus_RBCValveOrder",
        "HalStatus_PlasmaValveOrder","HalStatus_CollectValveOrder","HalStatus_redValue","HalStatus_greenValue","HalStatus_redDriveValue",
        "HalStatus_greenDriveValue","HalStatus_centActRPM","HalStatus_centCmdRPM","HalStatus_PS64Vsw_nValue","HalStatus_PS64V_nValue",
        "HalStatus_PS24Vsw_nValue","HalStatus_PS24V_nValue","HalStatus_PSp12V_nValue","HalStatus_PSm12V_nValue","HalStatus_PS5V_nValue",
        "ProcRun_CrAv","ProcRun_IntegratedPlateletYield","ProcVolumes_Vin","ProcVolumes_VinTotal","ProcVolumes_Vreturn","ProcVolumes_Vac",
        "ProcVolumes_PlateletVolume","ProcVolumes_ACinPlateletBag","ProcVolumes_PlasmaVolume","ProcVolumes_ACinPlasmaBag","ProcVolumes_RBCVolume",
        "ProcVolumes_ACinRBCBag","ProcVolumes_VinCurrentSubstate","ProcVolumes_VstorageRBCBag","ProcVolumes_VstoragePLTBag",
        "ProcVolumes_PreviousPlateletYield","ProcVolumes_PlateletYield","ProcVolumes_VSalineBolus","ProcRunTargets_RbcVolumeTarget",
        "ProcRunTargets_PlateletVolumeTarget","ProcRunTargets_PlasmaTarget","ProcRunTargets_PlateletYieldTarget",
        "ProcRunTargets_SelectedPlateletYield","ProcRunTargets_SelectedPlateletVolume","ProcRunTargets_PlateletConcenTarget","ProcRun_RedPl","ProcCycle_ResHct",
        "ProcTrimaSet_APSZero","ProcDonorCDS_NadlerTBV","ProcDonorCDS_OgawaTBV","ProcDonorCDS_Weight","ProcDonorCDS_Hct",
        "SHwStates","SHwOrders","DVitalsFromProc","DVitalsFromSafety","DVitalsFromGUI","HwStatesFromControl","SPFRecovery",
        "LatentSafetyStatusRequest","LatentSafetyStatus","HalStatus_inletActRpm","HalStatus_acActRpm","HalStatus_returnActRpm",
        "HalStatus_plateletActRpm","HalStatus_plasmaActRpm",0
    };

    int i = 0;
    while (supportedObjectList[i]) {
        if (strcmp(supportedObjectList[i++],objectName) == 0)
            return true;
    };

    return false;
}

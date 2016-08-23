/*****************************************************************************\
**
** ExcelPredict.cpp
**
** Provides the interface to the Trima Predict.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <windows.h>
#include <xlcall.h>

#include "ExcelPredict.h"

/* Macros ********************************************************************/

#define DLLEXPORT extern "C" __declspec(dllexport)

/* Functions *****************************************************************/

/**
 *
 * Optimizer_64_LoadOptimizerFromLog
 *
 * Creates an offline optimizer and populates the configurations
 * using the given datalog and cobe config files.
 *
 * @param dataLogPath The datalog file
 * @param cobeConfigPath The cobe config file.
 *
 * @return True if the optimizer was created.
 *
 */
DLLEXPORT int WINAPI Optimizer_64_LoadOptimizerFromLog(
		LPCSTR dataLogPath,
		LPCSTR cobeConfigPath)
{
	return PredictInstance::GetInstance().LoadOptimizerFromLog((char*)dataLogPath, (char*)cobeConfigPath);
}

/**
 *
 * Optimizer_64_LoadOptimizerFromConfig
 *
 * Creates an offline optimizer and populates the configurations
 * using the given user and cobe config files.
 *
 * @param userConfigPath The user config file
 * @param cobeConfigPath The cobe config file.
 *
 * @return True if the optimizer was created.
 *
 */
DLLEXPORT int WINAPI Optimizer_64_LoadOptimizerFromConfig(
		LPCSTR userConfigPath,
		LPCSTR cobeConfigPath)
{
	return PredictInstance::GetInstance().LoadOptimizerFromConfig(
			(char*)userConfigPath, (char*)cobeConfigPath);
}

/**
 *
 * Optimizer_64_LoadOptimizerFromConfigs
 *
 * Creates an offline optimizer and populates the configurations
 * using the given user and cobe config files.
 *
 * @param userConfigPath The user config file
 * @param cobeConfigPath The cobe config file.
 * @param softwareConfigPath The software config file.
 *
 * @return True if the optimizer was created.
 *
 */
DLLEXPORT int WINAPI Optimizer_64_LoadOptimizerFromConfigs(
		LPCSTR userConfigPath,
		LPCSTR cobeConfigPath,
		LPCSTR softwareConfigPath)
{
	return PredictInstance::GetInstance().LoadOptimizerFromConfig(
			(char*)userConfigPath, (char*)cobeConfigPath, (char*)softwareConfigPath);
}

/**
 *
 * Optimizer_64_UpdateConditions
 *
 * Updates the optimizer with the donor settings.
 *
 * @return True if the optimizer was updated.
 *
 */
DLLEXPORT int WINAPI Optimizer_64_UpdateConditions(void)
{
	return PredictInstance::GetInstance().UpdateConditions();
}

/**
 *
 * Optimizer_64_UpdateConfigurations
 *
 * Updates the optimizer with the donor settings.
 *
 * @return True if the optimizer was updated.
 *
 */
DLLEXPORT int WINAPI Optimizer_64_UpdateConfigurations(void)
{
	return PredictInstance::GetInstance().UpdateConfigurations();
}

/**
 *
 * Optimizer_64_OptimizeProduct
 *
 * Optimizes the product with the given index.
 *
 * @return True if the product was optimized.
 *
 */
DLLEXPORT int WINAPI Optimizer_64_OptimizeProduct(int product)
{
	return PredictInstance::GetInstance().OptimizeProduct(product);
}

/**
 *
 * SoftwareOptions_64_SetOption
 *
 * Set the software option.
 *
 * @return No return.
 *
 */
DLLEXPORT int WINAPI SoftwareOptions_64_SetOption(int option, bool enabled )
{
	PredictInstance::GetInstance().GetSoftwareCDS().SetFeature(option, enabled, AUTH_R_N_D, false);
	return option;
}

/**
 *
 * SoftwareOptions_64_GetOption
 *
 * Get the software option.
 *
 * @return 1 if the option is enabled.
 *
 */
DLLEXPORT int WINAPI SoftwareOptions_64_GetOption(int option)
{
	if( FEATURE_DEF_END <= option )
		return 0;
	FEATURE_ID id = (FEATURE_ID)option;
	return (int)PredictInstance::GetInstance().GetSoftwareCDS().getFeature(id) > 0;
}
DLLEXPORT int WINAPI Predict_64_Get_RBCState(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.RBCState.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Predict_64_Get_RBCDoubleState(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.RBCDoubleState.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_VinTarget(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.VinTarget.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_VinPIRStart(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.VinPIRStart.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_YdchPIR(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.YdchPIR.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_PIRLockin(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.PIRLockin.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Predict_64_Get_PIRRequired(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.PIRRequired.Get(optimizer, product); 
} 

DLLEXPORT bool WINAPI Predict_64_Get_doingPTFDouble(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.doingPTFDouble.Get(optimizer, product); 
} 

DLLEXPORT bool WINAPI Predict_64_Get_secondPartofPTFDouble(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.secondPartofPTFDouble.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Predict_64_Get_NumOfSubstates(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.NumOfSubstates.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_ExtracorporealVolume(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ExtracorporealVolume.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_ResidualRBCVolume(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ResidualRBCVolume.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_PrimeDuration(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.PrimeDuration.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_VinPrime(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.VinPrime.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Predict_64_Get_numOfTimes(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.numOfTimes.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Predict_64_Get_convergenceCtr(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.convergenceCtr.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Predict_64_Get_ProcedureNumber(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ProcedureNumber.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Predict_64_Get_Designator(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Designator.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Predict_64_Get_ProcAMAPorMC(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ProcAMAPorMC.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Predict_64_Get_ProcBloodType(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ProcBloodType.Get(optimizer, product); 
} 

DLLEXPORT bool WINAPI Predict_64_Get_ProcCollectPlasma(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ProcCollectPlasma.Get(optimizer, product); 
} 

DLLEXPORT bool WINAPI Predict_64_Get_ProcCollectVolFixed(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ProcCollectVolFixed.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_ProcFixedCollectVol(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ProcFixedCollectVol.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_ProcFixedPlasmaVol(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ProcFixedPlasmaVol.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Predict_64_Get_ProcPlasmaVolFixed(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ProcPlasmaVolFixed.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_ProcPlateletProdCon(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ProcPlateletProdCon.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_ProcRBCDose(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ProcRBCDose.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_ProcRBCHct(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ProcRBCHct.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_ProcRBCVolume(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ProcRBCVolume.Get(optimizer, product); 
} 

DLLEXPORT bool WINAPI Predict_64_Get_ProcPtfIsSplit(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ProcPtfIsSplit.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_ProcTime(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ProcTime.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_DesiredYield(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.DesiredYield.Get(optimizer, product); 
} 

DLLEXPORT bool WINAPI Predict_64_Get_MSSPlt(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.MSSPlt.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Predict_64_Get_MSSPltVolume(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.MSSPltVolume.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Predict_64_Get_MSSPltPctCarryover(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.MSSPltPctCarryover.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_OriginalPltVolume(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.OriginalPltVolume.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_OriginalPltYeild(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.OriginalPltYeild.Get(optimizer, product); 
} 

DLLEXPORT bool WINAPI Predict_64_Get_MSSRbc(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.MSSRbc.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Predict_64_Get_MSSRbcVolume(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.MSSRbcVolume.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_MaxPlasma(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.MaxPlasma.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Predict_64_Get_hypovolemic(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.hypovolemic.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_hypovolemiaDistance(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.hypovolemiaDistance.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_HypoMargin(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.HypoMargin.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Predict_64_Get_PlasmaTargetCannotChange(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.PlasmaTargetCannotChange.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_MaxACRate(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.MaxACRate.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_PredRBCDose(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.PredRBCDose.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_PredRBCVolume(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.PredRBCVolume.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_PredRBCCrit(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.PredRBCCrit.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_RBCCalculatedCrit(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.RBCCalculatedCrit.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Predict_64_Get_Status(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Status.Get(optimizer, product); 
} 

DLLEXPORT bool WINAPI Predict_64_Get_Optimized(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Optimized.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_MaxQin(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.MaxQin.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_PredVc(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.PredVc.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_PredVp(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.PredVp.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_PredVac(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.PredVac.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_PredVrep(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.PredVrep.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_PredTp(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.PredTp.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_InfusionRate(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.InfusionRate.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_Tc(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Tc.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_Ytarg(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Ytarg.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_Ec(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Ec.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_Qinc(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Qinc.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_Ydpur(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Ydpur.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_Vintarg(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Vintarg.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_Vptarg(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Vptarg.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_Vrtarg(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Vrtarg.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_Qcmr(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Qcmr.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_Vpmin(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Vpmin.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_Vinpur(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Vinpur.Get(optimizer, product); 
} 

DLLEXPORT float WINAPI Predict_64_Get_Ydch(int product) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Ydch.Get(optimizer, product); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_lang(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_lang.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_lang(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_lang.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_height(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_height.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_height(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_height.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_weight(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_weight.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_weight(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_weight.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_date_format(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_date_format.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_date_format(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_date_format.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_time_format(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_time_format.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_time_format(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_time_format.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_decimal_delimiter(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_decimal_delimiter.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_decimal_delimiter(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_decimal_delimiter.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_crit_or_glob(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_crit_or_glob.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_crit_or_glob(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_crit_or_glob.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_set_time(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_set_time.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_set_time(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_set_time.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_set_date(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_set_date.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_set_date(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_set_date.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_set_audio(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_set_audio.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_set_audio(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_set_audio.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_pword(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_pword.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_pword(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_pword.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_proc_time(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_proc_time.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_proc_time(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_proc_time.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_return_press(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_return_press.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_return_press(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_return_press.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_draw_press(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_draw_press.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_draw_press(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_draw_press.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_custom_ratio(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_custom_ratio.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_custom_ratio(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_custom_ratio.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_plasma_ratio(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plasma_ratio.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_plasma_ratio(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plasma_ratio.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_ac_rate(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_ac_rate.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_ac_rate(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_ac_rate.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_post_crit(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_post_crit.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_post_crit(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_post_crit.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_tbv_vol_setting(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_tbv_vol_setting.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_tbv_vol_setting(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_tbv_vol_setting.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_tbv_percent(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_tbv_percent.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_tbv_percent(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_tbv_percent.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_weight_setting(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_weight_setting.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_weight_setting(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_weight_setting.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_weight_less_than_vol(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_weight_less_than_vol.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_weight_less_than_vol(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_weight_less_than_vol.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_weight_greater_than_vol(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_weight_greater_than_vol.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_weight_greater_than_vol(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_weight_greater_than_vol.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_ml_per_kg(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_ml_per_kg.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_ml_per_kg(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_ml_per_kg.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_drbc_body_vol(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_drbc_body_vol.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_drbc_body_vol(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_drbc_body_vol.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_max_plasma_vol_during_plasma_collection(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_max_plasma_vol_during_plasma_collection.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_max_plasma_vol_during_plasma_collection(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_max_plasma_vol_during_plasma_collection.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_max_plasma_vol_during_platelet_collection(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_max_plasma_vol_during_platelet_collection.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_max_plasma_vol_during_platelet_collection(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_max_plasma_vol_during_platelet_collection.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_post_plat(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_post_plat.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_post_plat(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_post_plat.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_max_draw_flow(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_max_draw_flow.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_max_draw_flow(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_max_draw_flow.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_min_replacement_volume(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_min_replacement_volume.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_min_replacement_volume(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_min_replacement_volume.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_plt_mss_split_notif(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_mss_split_notif.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_plt_mss_split_notif(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_mss_split_notif.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_override_pas_bag_vol(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_override_pas_bag_vol.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_override_pas_bag_vol(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_override_pas_bag_vol.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_drbc_alert(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_drbc_alert.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_drbc_alert(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_drbc_alert.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_drbc_threshold(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_drbc_threshold.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_drbc_threshold(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_drbc_threshold.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_plasma_rinseback(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plasma_rinseback.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_plasma_rinseback(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plasma_rinseback.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_saline_rinseback(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_saline_rinseback.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_saline_rinseback(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_saline_rinseback.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_audit_tracking(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_audit_tracking.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_audit_tracking(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_audit_tracking.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_air_removal(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_air_removal.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_air_removal(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_air_removal.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_blood_diversion(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_blood_diversion.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_blood_diversion(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_blood_diversion.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_mss_plt_on(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_mss_plt_on.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_mss_plt_on(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_mss_plt_on.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_mss_rbc_on(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_mss_rbc_on.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_mss_rbc_on(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_mss_rbc_on.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_show_pre_aas_flags(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_show_pre_aas_flags.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_show_pre_aas_flags(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_show_pre_aas_flags.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_plt_def_bag_vol(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_def_bag_vol.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_plt_def_bag_vol(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_def_bag_vol.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_rbc_def_bag_vol(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc_def_bag_vol.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_rbc_def_bag_vol(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc_def_bag_vol.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_rbc_ratio(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc_ratio.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_rbc_ratio(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc_ratio.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_rbc_replace_fluid(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc_replace_fluid.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_rbc_replace_fluid(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc_replace_fluid.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_rbc_fluid_percent(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc_fluid_percent.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_rbc_fluid_percent(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc_fluid_percent.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_pls_amap_min(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_pls_amap_min.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_pls_amap_min(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_pls_amap_min.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_pls_amap_max(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_pls_amap_max.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_pls_amap_max(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_pls_amap_max.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_ysf(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_ysf.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_ysf(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_ysf.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_guaranteed_yield(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_guaranteed_yield.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_guaranteed_yield(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_guaranteed_yield.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_confidence_level(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_confidence_level.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_confidence_level(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_confidence_level.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_coefficient_of_variation(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_coefficient_of_variation.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_coefficient_of_variation(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_coefficient_of_variation.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_inlet_flow_ramp(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_inlet_flow_ramp.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_inlet_flow_ramp(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_inlet_flow_ramp.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_inlet_management(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_inlet_management.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_inlet_management(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_inlet_management.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_return_management(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_return_management.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_return_management(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_return_management.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_rsf(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rsf.Get(optimizer); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_rsf(float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rsf.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_male_only_plt(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_male_only_plt.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_male_only_plt(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_male_only_plt.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_male_only_plasma(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_male_only_plasma.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_male_only_plasma(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_male_only_plasma.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_complementary_plasma(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_complementary_plasma.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_complementary_plasma(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_complementary_plasma.Set(optimizer, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_ffp_volume(void) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_ffp_volume.Get(optimizer); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_ffp_volume(int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_ffp_volume.Set(optimizer, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_rbc_crit(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc_crit.Get(optimizer, index); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_rbc_crit(int index, float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc_crit.Set(optimizer, index, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_rbc_dose(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc_dose.Get(optimizer, index); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_rbc_dose(int index, float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc_dose.Set(optimizer, index, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_rbc_mss(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc_mss.Get(optimizer, index); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_rbc_mss(int index, int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc_mss.Set(optimizer, index, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_rbc_mss_volume(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc_mss_volume.Get(optimizer, index); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_rbc_mss_volume(int index, int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc_mss_volume.Set(optimizer, index, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_plt_amap_min_yield(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_amap_min_yield.Get(optimizer, index); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_plt_amap_min_yield(int index, float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_amap_min_yield.Set(optimizer, index, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_plt_amap_max_yield(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_amap_max_yield.Get(optimizer, index); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_plt_amap_max_yield(int index, float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_amap_max_yield.Set(optimizer, index, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_plt_amap_conc(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_amap_conc.Get(optimizer, index); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_plt_amap_conc(int index, float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_amap_conc.Set(optimizer, index, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_plt_yield(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_yield.Get(optimizer, index); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_plt_yield(int index, float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_yield.Set(optimizer, index, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_plt_volume(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_volume.Get(optimizer, index); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_plt_volume(int index, float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_volume.Set(optimizer, index, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_plt_mss(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_mss.Get(optimizer, index); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_plt_mss(int index, int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_mss.Set(optimizer, index, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_plt_pct_carryover(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_pct_carryover.Get(optimizer, index); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_plt_pct_carryover(int index, int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plt_pct_carryover.Set(optimizer, index, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_pls_volume(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_pls_volume.Get(optimizer, index); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_pls_volume(int index, float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_pls_volume.Set(optimizer, index, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_platelet(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_platelet.Get(optimizer, index); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_platelet(int index, int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_platelet.Set(optimizer, index, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_plasma(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plasma.Get(optimizer, index); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_plasma(int index, int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_plasma.Set(optimizer, index, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_rbc(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc.Get(optimizer, index); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_rbc(int index, int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_rbc.Set(optimizer, index, value); 
} 

DLLEXPORT int WINAPI Config_64_Get_key_blood_type(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_blood_type.Get(optimizer, index); 
} 

DLLEXPORT int WINAPI Config_64_Set_key_blood_type(int index, int value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_blood_type.Set(optimizer, index, value); 
} 

DLLEXPORT float WINAPI Config_64_Get_key_time(int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_time.Get(optimizer, index); 
} 

DLLEXPORT float WINAPI Config_64_Set_key_time(int index, float value) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.key_time.Set(optimizer, index, value); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Qcc(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Qcc.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Qpc(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Qpc.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_T(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.T.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Qnret(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Qnret.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Vsvn(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Vsvn.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Tcn(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Tcn.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Td(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Td.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Tr(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Tr.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_D(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.D.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Qrinst(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Qrinst.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_EndVin(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.EndVin.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Vp(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Vp.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Vacp(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Vacp.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Vc(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Vc.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Vacc(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Vacc.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Vr(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Vr.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Vacr(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Vacr.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_AccumVp(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.AccumVp.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_AccumVacp(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.AccumVacp.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_AccumVc(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.AccumVc.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_AccumVacc(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.AccumVacc.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_AccumTime(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.AccumTime.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Vac(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Vac.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_AccumVac(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.AccumVac.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Vrep(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Vrep.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_AccumVrep(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.AccumVrep.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Qrep(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Qrep.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Hin(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Hin.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Facb(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Facb.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Facp(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Facp.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_AvgHt(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.AvgHt.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Hend(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Hend.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Qininstd(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Qininstd.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Qininstr(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Qininstr.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Qacinst(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Qacinst.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Qcinstd(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Qcinstd.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Qcinstr(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Qcinstr.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Qpinstd(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Qpinstd.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Qpinstr(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Qpinstr.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Qch(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Qch.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_HypovolemiaDistance(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.HypovolemiaDistance.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_HypovolemiaMargin(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.HypovolemiaMargin.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Yield(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Yield.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Csf(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Csf.Get(optimizer, product, index); 
} 

DLLEXPORT float WINAPI Substate_64_Get_Rpm(int product, int index) 
{ 
	PREDICT::OfflineOptimizer *optimizer = PredictInstance::GetInstance().GetOptimizer(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.Rpm.Get(optimizer, product, index); 
} 

DLLEXPORT int WINAPI Condition_64_Get_id(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.id.Get(conditions); 
} 

DLLEXPORT int WINAPI Condition_64_Set_id(int value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.id.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_height(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.height.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_height(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.height.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_weight(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.weight.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_weight(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.weight.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_tbv(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.tbv.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_tbv(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.tbv.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_precount(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.precount.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_precount(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.precount.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_hct(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.hct.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_hct(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.hct.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_bmi(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.bmi.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_bmi(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.bmi.Set(conditions, value); 
} 

DLLEXPORT int WINAPI Condition_64_Get_female(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.female.Get(conditions); 
} 

DLLEXPORT int WINAPI Condition_64_Set_female(int value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.female.Set(conditions, value); 
} 

DLLEXPORT int WINAPI Condition_64_Get_blood_type(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.blood_type.Get(conditions); 
} 

DLLEXPORT int WINAPI Condition_64_Set_blood_type(int value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.blood_type.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_vin(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vin.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_vin(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vin.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_vac(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vac.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_vac(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vac.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_vplasma_bag(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vplasma_bag.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_vplasma_bag(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vplasma_bag.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_vplasma_bag_ac(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vplasma_bag_ac.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_vplasma_bag_ac(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vplasma_bag_ac.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_vcollect_bag(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vcollect_bag.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_vcollect_bag(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vcollect_bag.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_vcollect_bag_ac(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vcollect_bag_ac.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_vcollect_bag_ac(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vcollect_bag_ac.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_vrbc_bag(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vrbc_bag.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_vrbc_bag(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vrbc_bag.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_vrbc_bag_ac(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vrbc_bag_ac.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_vrbc_bag_ac(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vrbc_bag_ac.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_dose_rbc_bag(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.dose_rbc_bag.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_dose_rbc_bag(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.dose_rbc_bag.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_vrep(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vrep.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_vrep(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vrep.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_vsaline_bolus(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vsaline_bolus.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_vsaline_bolus(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vsaline_bolus.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_platelet_yield(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.platelet_yield.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_platelet_yield(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.platelet_yield.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_time_of_vincv(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.time_of_vincv.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_time_of_vincv(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.time_of_vincv.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_vincv(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vincv.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_vincv(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vincv.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_vin_epca_start(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vin_epca_start.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_vin_epca_start(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vin_epca_start.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_rbc_s1(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.rbc_s1.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_rbc_s1(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.rbc_s1.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_rbc_s2(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.rbc_s2.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_rbc_s2(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.rbc_s2.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_rbc_prime(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.rbc_prime.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_rbc_prime(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.rbc_prime.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_rbc_s1_double(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.rbc_s1_double.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_rbc_s1_double(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.rbc_s1_double.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_rbc_s2_double(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.rbc_s2_double.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_rbc_s2_double(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.rbc_s2_double.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_rbc_double(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.rbc_double.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_rbc_double(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.rbc_double.Set(conditions, value); 
} 

DLLEXPORT int WINAPI Condition_64_Get_substate(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.substate.Get(conditions); 
} 

DLLEXPORT int WINAPI Condition_64_Set_substate(int value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.substate.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_qch_min(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.qch_min.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_qch_min(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.qch_min.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_qch_cur(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.qch_cur.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_qch_cur(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.qch_cur.Set(conditions, value); 
} 

DLLEXPORT int WINAPI Condition_64_Get_npurges(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.npurges.Get(conditions); 
} 

DLLEXPORT int WINAPI Condition_64_Set_npurges(int value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.npurges.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_adjusted_hct(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.adjusted_hct.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_adjusted_hct(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.adjusted_hct.Set(conditions, value); 
} 

DLLEXPORT int WINAPI Condition_64_Get_current_recovery(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.current_recovery.Get(conditions); 
} 

DLLEXPORT int WINAPI Condition_64_Set_current_recovery(int value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.current_recovery.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_vin_clearline(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vin_clearline.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_vin_clearline(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.vin_clearline.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_wbc_purge_start_time(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.wbc_purge_start_time.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_wbc_purge_start_time(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.wbc_purge_start_time.Set(conditions, value); 
} 

DLLEXPORT int WINAPI Condition_64_Get_wbc_purge_type(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.wbc_purge_type.Get(conditions); 
} 

DLLEXPORT int WINAPI Condition_64_Set_wbc_purge_type(int value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.wbc_purge_type.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_substate_start_time(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.substate_start_time.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_substate_start_time(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.substate_start_time.Set(conditions, value); 
} 

DLLEXPORT int WINAPI Condition_64_Get_midrun_entered(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.midrun_entered.Get(conditions); 
} 

DLLEXPORT int WINAPI Condition_64_Set_midrun_entered(int value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.midrun_entered.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_proctime(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.proctime.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_proctime(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.proctime.Set(conditions, value); 
} 

DLLEXPORT int WINAPI Condition_64_Get_type(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.type.Get(conditions); 
} 

DLLEXPORT int WINAPI Condition_64_Set_type(int value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.type.Set(conditions, value); 
} 

DLLEXPORT bool WINAPI Condition_64_Get_rbc_detector_calibrated(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.rbc_detector_calibrated.Get(conditions); 
} 

DLLEXPORT bool WINAPI Condition_64_Set_rbc_detector_calibrated(bool value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.rbc_detector_calibrated.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_qin(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.qin.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_qin(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.qin.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_qrp(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.qrp.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_qrp(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.qrp.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_ir(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ir.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_ir(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ir.Set(conditions, value); 
} 

DLLEXPORT float WINAPI Condition_64_Get_ratio(void) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ratio.Get(conditions); 
} 

DLLEXPORT float WINAPI Condition_64_Set_ratio(float value) 
{ 
	PREDICT::Conditions &conditions = PredictInstance::GetInstance().GetConditions(); 
	PREDICT::PredictAccess &accessor = PredictInstance::GetInstance().GetAccessor(); 
	return accessor.ratio.Set(conditions, value); 
} 


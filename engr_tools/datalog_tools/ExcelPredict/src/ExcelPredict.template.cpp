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

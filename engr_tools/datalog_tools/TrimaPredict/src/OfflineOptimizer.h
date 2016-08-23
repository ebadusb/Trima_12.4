/*****************************************************************************\
**
** OfflineOptimizer.h
**
** Predict offline optimizer.
**
\*****************************************************************************/

#ifndef OFFLINEOPTIMIZER_H
#define OFFLINEOPTIMIZER_H

/* Includes ******************************************************************/

#include "software_cds.h"
#include "cfg_ids.h"
#include "predict_msgs.h"
//#include "safproc.h"
#include "procdet.h"
#include "predict.h"
#include "config_cds.h"
#include "alarms.h"
#include "sharedstruct.h"
#include "PredictRecordMrg.h"

namespace PREDICT {

/* Classes *******************************************************************/

class OfflineOptimizer {
public:

	/**
	 *
	 * CreateOptimizerFromLog
	 *
	 * Creates an offline optimizer and populates the configurations
	 * using the given datalog and cobe config files.
	 *
	 * @param dataLogPath The datalog file
	 * @param cobeConfigPath The cobe config file.
	 * @param optimizer The new optimizer.
	 *
	 * @return True if the optimizer was created.
	 *
	 */
	static bool CreateOptimizerFromLog(
			const char* dataLogPath,
			const char* cobeConfigPath, 
			OfflineOptimizer **optimizer);

	/**
	 *
	 * CreateOptimizerFromLog
	 *
	 * Creates an offline optimizer and populates the configurations
	 * using the given datalog and cobe config files. This version 
	 * also extracts the predict initial conditions from the datalog. 
	 *
	 * @param dataLogPath The datalog file
	 * @param cobeConfigPath The cobe config file.
	 * @param manager The predict record manager.
	 * @param optimizer The new optimizer.
	 *
	 * @return True if the optimizer was created.
	 *
	 */
	static bool CreateOptimizerFromLog(
			const char* dataLogPath,
			const char* cobeConfigPath, 
			PredictRecordMrg &manager,
			OfflineOptimizer **optimizer);

	/**
	 *
	 * CreateOptimizerFromConfig
	 *
	 * Creates an offline optimizer and populates the configurations
	 * using the given user and cobe config files.
	 *
	 * @param userConfigPath The user config file
	 * @param cobeConfigPath The cobe config file.
	 * @param optimizer The new optimizer.
	 *
	 * @return True if the optimizer was created.
	 *
	 */
	static bool CreateOptimizerFromConfig(
			const char* userConfigPath,
			const char* cobeConfigPath, 
			OfflineOptimizer **optimizer);

	/**
	 *
	 * CreateOptimizerFromConfig
	 *
	 * Creates an offline optimizer and populates the configurations
	 * using the given user and cobe config files.
	 *
	 * @param userConfigPath The user config file
	 * @param cobeConfigPath The cobe config file.
	 * @param softwareConfigPath The software config file.
	 * @param optimizer The new optimizer.
	 *
	 * @return True if the optimizer was created.
	 *
	 */
	static bool CreateOptimizerFromConfig(
			const char* userConfigPath,
			const char* cobeConfigPath, 
			const char* softwareConfigPath, 
			OfflineOptimizer **optimizer);

	/**
	 *
	 * DestroyOptimizer
	 *
	 * Destroys the given offline optimizer.
	 *
	 * @param optimizer The optimizer to delete.
	 *
	 * @return True if the optimizer was destroyed.
	 *
	 */
	static bool DestroyOptimizer(
			OfflineOptimizer *optimizer);

	void UpdateConditions(Conditions &conditions);

	int GetNumProcedures();
	procedureDetail *GetOptimizeProcedure(int index);
   procedureDetail *GetProcedure(int index);

	Config_CDS &GetConfigs(void) { return mConfig; }
	Predict &GetPredict(void) { return *mPredict; }

private:
	OfflineOptimizer(void);
	~OfflineOptimizer(void);

	const float calculateBMI(float height, float weight);
	const float calculateTBV(float height, float weight, int female);
	const float calculateHeight(float tbv, float weight, int female);

	Config_CDS mConfig;
   Predict* mPredict;
};

} // namespace PREDICT

#endif // OFFLINEOPTIMIZER_H

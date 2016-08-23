/*****************************************************************************\
**
** ExtractConfig.h
**
** Stack trace implementation.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <algorithm>
#include <iostream>

#include "ExtractConfig.h"
#include "LogExecute.h"
#include "config_cds.h"

namespace CONFIG {

using namespace DECODER;

/* Functions *****************************************************************/

/**
 *
 * ExtractConfig
 *
 * Constructor
 *
 * @param config The config CDS.
 *
 */
ExtractConfig::ExtractConfig(Config_CDS &config) : 
		mConfigCDS(config),
		mProcessPeriodicSet(mColumnData.configDataRegister),
		mProcessPeriodicSetItem(mColumnData.periodicSetItemList),
		mProcesseriodicLogOutput(mColumnData.periodicLogOutputList),
		mFilterPeriodicSet(mProcessPeriodicSet, "ConfigData"),
		mFilterPeriodicSetItem(mProcessPeriodicSetItem, mColumnData.configDataRegister),
		mFilterPeriodicLogOutput(mProcesseriodicLogOutput, mColumnData.periodicSetItemList),
		mFilterPeriodicLogOutputBySetID(mFilterPeriodicLogOutput, mColumnData.configDataRegister)
{
}

/**
 *
 * Register
 *
 * Registers the necessary handlers with the reader.
 *
 * @param reader The file reader.
 *
 */
void ExtractConfig::Register(LogReader &reader)
{
	reader.RegisterPeriodicSet(mFilterPeriodicSet);
	reader.RegisterPeriodicSetItem(mFilterPeriodicSetItem);
	reader.RegisterPeriodicLogOutput(mFilterPeriodicLogOutputBySetID);
}

/**
 *
 * Unregister
 *
 * Unregisters the handlers from the reader.
 *
 * @param reader The file reader.
 *
 */
void ExtractConfig::Unregister(LogReader &reader)
{
	reader.UnregisterPeriodicSet(mFilterPeriodicSet);
	reader.UnregisterPeriodicSetItem(mFilterPeriodicSetItem);
	reader.UnregisterPeriodicLogOutput(mFilterPeriodicLogOutputBySetID);
}

/**
 *
 * PreProcess
 *
 * Perform the procedures before the import process.
 *
 */
void ExtractConfig::PreProcess(void)
{
	mColumnData.configDataRegister.mTimeSeconds = 0;
	mColumnData.configDataRegister.mTimeNanosec = 0;
	mColumnData.configDataRegister.mNodeID = 0;
	mColumnData.configDataRegister.mSetID = 0;
	mColumnData.configDataRegister.mSetName = "";
	mColumnData.periodicSetItemList.clear();
	mColumnData.periodicLogOutputList.clear();
}

/**
 *
 * PostProcess
 *
 * Perform the procedures after the import process.
 *
 */
void ExtractConfig::PostProcess(void)
{
	LangUnitCfg langUnitCfg;
	BasicMachineCfg basicMachineCfg;
	ProcedureConfig procedureConfig;   
	ProductTemplates productTemplates;
	PredictionConfig predictionConfig;
	ProductDefinitionArray productDefinitionArray;
	char keyName[32] = { 0 };

	//Language Config
	ExtractConfig::Assign(mColumnData, "ConfigLang", &langUnitCfg.key_lang);
	ExtractConfig::Assign(mColumnData, "ConfigHeight", &langUnitCfg.key_height);
	ExtractConfig::Assign(mColumnData, "ConfigWeight", &langUnitCfg.key_weight);
	ExtractConfig::Assign(mColumnData, "ConfigDecimal", &langUnitCfg.key_decimal_delimiter);
	ExtractConfig::Assign(mColumnData, "ConfigCritOrHemoglobin",	&langUnitCfg.key_crit_or_glob);
	ExtractConfig::Assign(mColumnData, "ConfigDateFormat", &langUnitCfg.key_date_format);
	ExtractConfig::Assign(mColumnData, "ConfigTimeFormat", &langUnitCfg.key_time_format);

	//Machine Config
	ExtractConfig::Assign(mColumnData, "ConfigAudioLevel", &basicMachineCfg.key_set_audio);
	// These columns are not logged because the operator cannot change them.
	basicMachineCfg.key_set_time = 12;
	basicMachineCfg.key_set_date = 10;
	basicMachineCfg.key_pword = 789852;

	//Procedure Config
	ExtractConfig::Assign(mColumnData, "ConfigProcTime", &procedureConfig.key_proc_time);
	ExtractConfig::Assign(mColumnData, "ConfigReturnPressure", &procedureConfig.key_return_press);
	ExtractConfig::Assign(mColumnData, "ConfigDrawPressure", &procedureConfig.key_draw_press);
	ExtractConfig::Assign(mColumnData, "ConfigCustomRatio", &procedureConfig.key_custom_ratio);
	ExtractConfig::Assign(mColumnData, "ConfigPlasmaRatio", &procedureConfig.key_plasma_ratio);
	ExtractConfig::Assign(mColumnData, "ConfigACRate", &procedureConfig.key_ac_rate);
	ExtractConfig::Assign(mColumnData, "ConfigPostCrit", &procedureConfig.key_post_crit);
	ExtractConfig::Assign(mColumnData, "ConfigTBVSetting", &procedureConfig.key_tbv_vol_setting);
	ExtractConfig::Assign(mColumnData, "ConfigTBVPercent", &procedureConfig.key_tbv_percent);
	
	ExtractConfig::Assign(mColumnData, "ConfigWeightSetting", &procedureConfig.key_weight_setting);
	ExtractConfig::Assign(mColumnData, "ConfigWeightLessThanVol", &procedureConfig.key_weight_less_than_vol);
	ExtractConfig::Assign(mColumnData, "ConfigWeightGreaterThanVol", &procedureConfig.key_weight_greater_than_vol);
//	ExtractConfig::Assign(mColumnData, "ConfigWeight1Setting", &procedureConfig.key_weight_1_setting);
//	ExtractConfig::Assign(mColumnData, "ConfigWeight2Setting", &procedureConfig.key_weight_2_setting);
//	ExtractConfig::Assign(mColumnData, "ConfigWeight3Setting", &procedureConfig.key_weight_3_setting);
//	ExtractConfig::Assign(mColumnData, "ConfigWeight1LessThanVol", &procedureConfig.key_weight_1_less_than_vol);
//	ExtractConfig::Assign(mColumnData, "ConfigWeight2LessThanVol", &procedureConfig.key_weight_2_less_than_vol);
//	ExtractConfig::Assign(mColumnData, "ConfigWeight3LessThanVol", &procedureConfig.key_weight_3_less_than_vol);
//	ExtractConfig::Assign(mColumnData, "ConfigWeight3GreaterThanVol", &procedureConfig.key_weight_3_greater_than_vol);
	
	ExtractConfig::Assign(mColumnData, "ConfigMlPerKg", &procedureConfig.key_ml_per_kg);
	ExtractConfig::Assign(mColumnData, "ConfigDrbcBodyVol", &procedureConfig.key_drbc_body_vol);
	ExtractConfig::Assign(mColumnData, "ConfigMaxPlasmaVolPltCollect", &procedureConfig.key_max_plasma_vol_during_platelet_collection);
	ExtractConfig::Assign(mColumnData, "ConfigMaxPlasmaVolPlsCollect", &procedureConfig.key_max_plasma_vol_during_plasma_collection);
	ExtractConfig::Assign(mColumnData, "ConfigPostCount", &procedureConfig.key_post_plat);
	ExtractConfig::Assign(mColumnData, "ConfigMaxDrawFlow", &procedureConfig.key_max_draw_flow);
	ExtractConfig::Assign(mColumnData, "ConfigMinReplaceVolume", &procedureConfig.key_min_replacement_volume);
	ExtractConfig::Assign(mColumnData, "ConfigPLTMssSplitNotif", &procedureConfig.key_plt_mss_split_notif);
	ExtractConfig::Assign(mColumnData, "ConfigMssOverrideBagVol", &procedureConfig.key_override_pas_bag_vol);
	ExtractConfig::Assign(mColumnData, "ConfigBloodDiversion", &procedureConfig.key_blood_diversion);
	ExtractConfig::Assign(mColumnData, "ConfigDRBCAlert", &procedureConfig.key_drbc_alert);
    ExtractConfig::Assign(mColumnData, "ConfigDRBCThreshold", &procedureConfig.key_drbc_threshold);
	ExtractConfig::Assign(mColumnData, "ConfigPlasmaRinseback", &procedureConfig.key_plasma_rinseback);
	ExtractConfig::Assign(mColumnData, "ConfigSalineRinseback", &procedureConfig.key_saline_rinseback);
	ExtractConfig::Assign(mColumnData, "ConfigAuditTracking", &procedureConfig.key_audit_tracking);
	ExtractConfig::Assign(mColumnData, "ConfigAirRemoval", &procedureConfig.key_air_removal);
    ExtractConfig::Assign(mColumnData, "ConfigPLTMeteredSolution", &procedureConfig.key_mss_plt_on);
    ExtractConfig::Assign(mColumnData, "ConfigRBCMeteredSolution", &procedureConfig.key_mss_rbc_on);
    ExtractConfig::Assign(mColumnData, "ConfigShowPreAasFlags", &procedureConfig.key_show_pre_aas_flags);
    ExtractConfig::Assign(mColumnData, "ConfigPLTDefBagVol", &procedureConfig.key_plt_def_bag_vol);
	ExtractConfig::Assign(mColumnData, "ConfigRBCDefBagVol", &procedureConfig.key_rbc_def_bag_vol);

	//Product Templates 
	for (int i = 0; i < NUM_YIELD_VOLUME; ++i) {
	    sprintf(keyName, "ConfigPltYield%d", i+1);
	    ExtractConfig::Assign(mColumnData, keyName, &productTemplates.key_plt_yield[i]);

	    sprintf(keyName, "ConfigPltVolume%d", i+1);
	    ExtractConfig::Assign(mColumnData, keyName, &productTemplates.key_plt_volume[i]);

        sprintf(keyName, "ConfigPLTMSS%d", i+1);
		ExtractConfig::Assign(mColumnData, keyName, &productTemplates.key_plt_mss[i]);

		sprintf(keyName, "ConfigPctCarryoverVal%d", i+1);
		ExtractConfig::Assign(mColumnData, keyName, &productTemplates.key_plt_pct_carryover[i]);
    }

	for (int i = 0; i < NUM_PLASMA_VOLUME; ++i) {
		sprintf(keyName, "ConfigPlasmaVolume%d", i+1);
		ExtractConfig::Assign(mColumnData, keyName, &productTemplates.key_pls_volume[i]);
	}

	for (int i = 0; i < NUM_CRIT_DOSE; ++i) {
		sprintf(keyName, "ConfigRBCCrit%d", i+1);
		ExtractConfig::Assign(mColumnData, keyName, &productTemplates.key_rbc_crit[i]);

		sprintf(keyName, "ConfigRBCDose%d", i+1);
		ExtractConfig::Assign(mColumnData, keyName, &productTemplates.key_rbc_dose[i]);     

        sprintf(keyName, "ConfigRBCMss%d", i+1);
		ExtractConfig::Assign(mColumnData, keyName, &productTemplates.key_rbc_mss[i]);

		sprintf(keyName, "ConfigRBCMssVolume%d", i+1);
		ExtractConfig::Assign(mColumnData, keyName, &productTemplates.key_rbc_mss_volume[i]);
	}

	// Predict Config
	ExtractConfig::Assign(mColumnData, "ConfigRBCRatio", &predictionConfig.key_rbc_ratio);
	ExtractConfig::Assign(mColumnData, "ConfigRBCReplaceFluid", &predictionConfig.key_rbc_replace_fluid);
	ExtractConfig::Assign(mColumnData, "ConfigRBCFluidPercent", &predictionConfig.key_rbc_fluid_percent);
	ExtractConfig::Assign(mColumnData, "ConfigAMAPMin", &predictionConfig.key_pls_amap_min);
	ExtractConfig::Assign(mColumnData, "ConfigAMAPMax", &predictionConfig.key_pls_amap_max);
	ExtractConfig::Assign(mColumnData, "ConfigYSF", &predictionConfig.key_ysf);

	predictionConfig.key_guaranteed_yield = 0;
	predictionConfig.key_confidence_level = 95;
	predictionConfig.key_coefficient_of_variation = 10;

	ExtractConfig::Assign(mColumnData, "ConfigInletManagement", &predictionConfig.key_inlet_management);
	ExtractConfig::Assign(mColumnData, "ConfigInletFlowRamp", &predictionConfig.key_inlet_flow_ramp);
	ExtractConfig::Assign(mColumnData, "ConfigReturnManagement", &predictionConfig.key_return_management);
	ExtractConfig::Assign(mColumnData, "ConfigRSF", &predictionConfig.key_rsf);
	ExtractConfig::Assign(mColumnData, "ConfigMaleOnlyPlt",	&predictionConfig.key_male_only_plt);
	ExtractConfig::Assign(mColumnData, "ConfigMaleOnlyPlasma",	&predictionConfig.key_male_only_plasma);
	ExtractConfig::Assign(mColumnData, "ConfigFFPVolume",	&predictionConfig.key_ffp_volume);
	ExtractConfig::Assign(mColumnData, "ConfigComplementaryPlasma",	&predictionConfig.key_complementary_plasma);

	//Procedure Definition
	for (int i = 0; i < NUM_OFFLINE_PROCEDURES; ++i) {
		std::memset(&productDefinitionArray[i], 0, sizeof(ProductDefinition));

		sprintf(keyName, "ConfigProcPlt%d", i+1);
		ExtractConfig::Assign(mColumnData, keyName, &productDefinitionArray[i].key_platelet);

		sprintf(keyName, "ConfigProcPls%d", i+1);
		ExtractConfig::Assign(mColumnData, keyName, &productDefinitionArray[i].key_plasma);

		sprintf(keyName, "ConfigProcRBC%d", i+1);
		ExtractConfig::Assign(mColumnData, keyName, &productDefinitionArray[i].key_rbc);

		sprintf(keyName, "ConfigProcBloodType%d", i+1);
		ExtractConfig::Assign(mColumnData, keyName, &productDefinitionArray[i].key_blood_type);

		sprintf(keyName, "ConfigProcTime%d", i+1);
		ExtractConfig::Assign(mColumnData, keyName, &productDefinitionArray[i].key_time);
	}

	mConfigCDS.LangUnit = langUnitCfg;
	mConfigCDS.Machine = basicMachineCfg;
	mConfigCDS.Procedure = procedureConfig;
	mConfigCDS.Templates = productTemplates;
	mConfigCDS.Predict = predictionConfig;
	mConfigCDS.Definition = productDefinitionArray;

//	if (!mConfigCDS.SWOptionEnabled("four_way_volume_cutoff", langUnitCfg.key_lang)) {
//		procedureConfig.key_weight_1_setting = procedureConfig.key_weight_3_setting;
//		procedureConfig.key_weight_2_setting = procedureConfig.key_weight_3_setting;
//		procedureConfig.key_weight_1_less_than_vol = procedureConfig.key_weight_3_less_than_vol;
//		procedureConfig.key_weight_2_less_than_vol = procedureConfig.key_weight_3_less_than_vol;
//
//		mConfigCDS.Procedure = procedureConfig;
//	}

	// If successful read, initialize the offline procedure list for predict
	// Initialize the number of procedures to the number of offline ones.
	mConfigCDS.ProcedureList.numberOfProcedures.Set(NUM_OFFLINE_PROCEDURES);
	mConfigCDS.CopyOfflineProcedureList();
}

/**
 *
 * Extract
 *
 * Extracts the config data from the log file.
 *
 * @param logFile The log file to analyze.
 * @param config The config CDS to update.
 *
 * @return The results.
 *
 */
ExtractConfig::Result ExtractConfig::Extract(const String &logFile, Config_CDS &config)
{
	return DECODER::ProcessLogData(logFile.c_str(), ExtractConfig(config));
}

/**
 *
 * Assign
 *
 * Assigns the value with hte given name to the address.
 *
 * @param data The data structure.
 * @param name The data name.
 * @param address The location to copy.
 *
 */
void ExtractConfig::Assign(const ColumnData &data, const String &name, void *address)
{
	PeriodicSetItemList::const_iterator setItemIter = data.periodicSetItemList.begin();
	PeriodicSetItemList::const_iterator setItemEnd = data.periodicSetItemList.end();

	Uint16 keyCode = 0;

	for(; setItemIter != setItemEnd; ++setItemIter) {
		if((*setItemIter).mName == name) {
			keyCode = (*setItemIter).mKeyCode;
			break;
		}
	}

	if(setItemIter == setItemEnd) {
		std::cerr << "Key not found in Periodic Set: " << name << std::endl;
		return;
	}

	PeriodicLogOutputList::const_reverse_iterator logItemIter = data.periodicLogOutputList.rbegin();
	PeriodicLogOutputList::const_reverse_iterator logItemEnd = data.periodicLogOutputList.rend();

	for(; logItemIter != logItemEnd; ++logItemIter) {
		RecordPeriodicLogOutput::Map::const_iterator iter = (*logItemIter).mData.find(keyCode);
		RecordPeriodicLogOutput::Map::const_iterator end = (*logItemIter).mData.end();

		if (iter != end) {
			memcpy(address, &iter->second.front(), iter->second.size());
			break;
		}
	}

	if(logItemIter == logItemEnd) {
		std::cerr << "Key not found in Periodic Log: " << name << std::endl;
		return;
	}
}

} // namespace CONFIG


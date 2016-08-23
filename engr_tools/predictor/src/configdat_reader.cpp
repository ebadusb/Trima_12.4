#include "configdat_reader.h"
#include <map>
#if defined(__MINGW__) || defined(__WIN32__)
/* These structures must be packed to match the format from the data log file */
#pragma pack(push, _dlog_previous_pack, 1)
#endif

#if defined(__MINGW__) || defined(__WIN32__)
/* These structures must be packed to match the format from the data log file */
#pragma pack(pop, _dlog_previous_pack)
#endif

ConfigDatReader::ConfigDatReader() { }

bool ConfigDatReader::initialize(const char* dlogname)
{
	_dlogName = dlogname;
	/* Set up the config filter */
	/* The first periodic log is the config */
	//_dlogFilter.includeRecordTypes.push_back(DLOG_Record::DLOG_PeriodicLogId);
	/* every item in the periodic log is keyed into a PeriodicItem */
	//_dlogFilter.includeRecordTypes.push_back(DLOG_Record::DLOG_PeriodicSetItemId);
	/* Go on till you hit the first periodicLogId*/
	//_dlogFilter.recordCountMap.insert(std::make_pair(DLOG_Record::DLOG_PeriodicLogId, 1));
	/* filter by the periodic set "ConfigData" */
	_dlogFilter.periodicSetNames.push_back("ConfigData");

	//_dlogFilter = filter;
	if(!_initialized) {
		if (DLOG_Reader::initialize() == 0)
			return true;
		else
			return false;
	}
	return true;
}

ConfigDatReader::~ConfigDatReader() { }

void ConfigDatReader::ReadConfig(Config_CDS* config_cds)
{

	//clear prior to every read
	if(!_logItemsFound.empty()) {
		_logItemsFound.clear();
	}

	_periodicSetItems = &_recordMap[DLOG_Record::DLOG_PeriodicSetItemId];
	_config_cds = config_cds;
	//Retrieve _periodicLogs
	vector< DLOG_Record* > periodicConfigLogs = _recordMap[DLOG_Record::DLOG_PeriodicLogId];
	vector< DLOG_Record* >::iterator lit;

	for(lit = periodicConfigLogs.begin(); lit != periodicConfigLogs.end(); lit++) {
		_periodicLog = (DLOG_PeriodicLog*)*lit;
		read();
	}

//four_way_volume_cutoff is no longer a feature
#if 0
	bool four_way_volume_cutoff_config_allowed = _config_cds->SWOptionEnabled("four_way_volume_cutoff", _langCfg.key_lang);
#endif
    bool four_way_volume_cutoff_config_allowed = false;

	if (!four_way_volume_cutoff_config_allowed)
	{
		_procedureCfg.key_weight_1_setting = _procedureCfg.key_weight_3_setting;
		_procedureCfg.key_weight_2_setting = _procedureCfg.key_weight_3_setting;
		_procedureCfg.key_weight_1_less_than_vol = _procedureCfg.key_weight_3_less_than_vol;
		_procedureCfg.key_weight_2_less_than_vol = _procedureCfg.key_weight_3_less_than_vol;

		_config_cds->Procedure = _procedureCfg;
	}


	// If successful read, initialize the offline procedure list for predict
	   // Initialize the number of procedures to the number of offline ones.
	   _config_cds->ProcedureList.numberOfProcedures.Set(NUM_OFFLINE_PROCEDURES);

	   _config_cds->CopyOfflineProcedureList();
}

bool ConfigDatReader::WriteConfig(string filename)
{
	return _config_cds->WriteConfig(filename);
}

/* Retrieves the Periodic Item corresponding to a keyName and assigns 
 * data to the corresponding address 
 * dlogKeyName - Keyname as logged in the dlog file (the column name) eg. ConfigLang
 * configKeyName - Keyname in the config.dat file eg. key_lang
 */
void ConfigDatReader::assign(string dlogKeyName, void* address, string configKeyName)
{
	//Retrieve keyCode from _periodicItems
	DLOG_PeriodicSetItem* p_itemMetaData = NULL;
	vector<DLOG_Record*>::iterator it;
	for(it = _periodicSetItems->begin(); it != _periodicSetItems->end(); it++) {
		p_itemMetaData = (DLOG_PeriodicSetItem*)*it;
		if(p_itemMetaData->keyName == dlogKeyName) {
			break;
		}
	}

	if(it == _periodicSetItems->end() ){
		//Try to read the min value from cfg_vars.cpp as default.
		if(!assignDefaultMin(configKeyName, address))
			cerr << "Key not found in Periodic Set: " << dlogKeyName << endl;
		return;
		//exit(5);
	}
	//Retrieve data from periodic items list in _periodicLog
	vector<PeriodicLogItem>::iterator pit;
	for(pit = _periodicLog->periodicItemList.begin(); pit != _periodicLog->periodicItemList.end(); pit++) {
		if(pit->keyCode == p_itemMetaData->keyCode) {
			_logItemsFound.insert(dlogKeyName);
			break;
			//return *pit;
		}
	}

	if(pit == _periodicLog->periodicItemList.end()) { 
		if ( find(_logItemsFound.begin(), _logItemsFound.end(), dlogKeyName) == _logItemsFound.end() ) {
			cerr << "Key not found in Periodic Log: " << dlogKeyName << endl;
			//exit(5);
		}
		return;
	}
	memcpy(address, pit->data.data(), pit->itemSize);
}
									   
void ConfigDatReader::read()
{   
	//Language Config
	assign("ConfigLang", &_langCfg.key_lang);
	assign("ConfigHeight", &_langCfg.key_height);
	assign("ConfigWeight", &_langCfg.key_weight);
	assign("ConfigDecimal", &_langCfg.key_decimal_delimiter);
	assign("ConfigCritOrHemoglobin",	&_langCfg.key_crit_or_glob, "key_crit_or_glob");
	assign("ConfigDateFormat", &_langCfg.key_date_format);
	assign("ConfigTimeFormat", &_langCfg.key_time_format);

	_config_cds->LangUnit = _langCfg;

	//Machine Config
	assign("ConfigAudioLevel", &_machineCfg.key_set_audio);
	// These columns are not logged because the operator
	// cannot change them.
	_machineCfg.key_set_time = 12;
	_machineCfg.key_set_date = 10;
	_machineCfg.key_pword = 789852;

	_config_cds->Machine = _machineCfg;

	//Procedure Config
	assign("ConfigProcTime", &_procedureCfg.key_proc_time);
	assign("ConfigReturnPressure", &_procedureCfg.key_return_press);
	assign("ConfigDrawPressure", &_procedureCfg.key_draw_press);
	assign("ConfigCustomRatio", &_procedureCfg.key_custom_ratio);
	assign("ConfigPlasmaRinseback", &_procedureCfg.key_plasma_rinseback, "key_plasma_rinseback");
	assign("ConfigSalineRinseback", &_procedureCfg.key_saline_rinseback, "key_saline_rinseback");
	assign("ConfigAuditTracking", &_procedureCfg.key_audit_tracking);
	assign("ConfigAirRemoval", &_procedureCfg.key_air_removal, "key_air_removal");
	
	assign("ConfigDRBCAlert", &_procedureCfg.key_drbc_alert);
    assign("ConfigDRBCThreshold", &_procedureCfg.key_drbc_threshold, "key_drbc_threshold");
	assign("ConfigACRate", &_procedureCfg.key_ac_rate);
	assign("ConfigPostCrit", &_procedureCfg.key_post_crit);
	assign("ConfigPostCount", &_procedureCfg.key_post_plat);
	assign("ConfigMaxDrawFlow", &_procedureCfg.key_max_draw_flow);
	assign("ConfigTBVSetting", &_procedureCfg.key_tbv_vol_setting);
	assign("ConfigTBVPercent", &_procedureCfg.key_tbv_percent);
	assign("ConfigWeight1Setting", &_procedureCfg.key_weight_1_setting);
	assign("ConfigWeight2Setting", &_procedureCfg.key_weight_2_setting);
	assign("ConfigWeight3Setting", &_procedureCfg.key_weight_3_setting);
	assign("ConfigWeight1LessThanVol", &_procedureCfg.key_weight_1_less_than_vol);
	assign("ConfigWeight2LessThanVol", &_procedureCfg.key_weight_2_less_than_vol);
	assign("ConfigWeight3LessThanVol", &_procedureCfg.key_weight_3_less_than_vol);
	assign("ConfigWeight3GreaterThanVol", &_procedureCfg.key_weight_3_greater_than_vol);
	assign("ConfigMlPerKg", &_procedureCfg.key_ml_per_kg);
    assign("ConfigDrbcBodyVol", &_procedureCfg.key_drbc_body_vol, "key_drbc_body_vol");
	assign("ConfigBloodDiversion", &_procedureCfg.key_blood_diversion, "key_blood_diversion");
	assign("ConfigMinReplaceVolume", &_procedureCfg.key_min_replacement_volume);
	assign("ConfigPLTMssSplitNotif", &_procedureCfg.key_plt_mss_split_notif, "key_plt_mss_split_notif");
	assign("ConfigMssOverrideBagVol", &_procedureCfg.key_override_pas_bag_vol, "key_override_pas_bag_vol");
		
	// Metered Storage Config
    assign("ConfigRBCMeteredSolution", &_procedureCfg.key_mss_rbc_on, "key_mss_rbc_on");
    assign("ConfigPLTMeteredSolution", &_procedureCfg.key_mss_plt_on, "key_mss_plt_on");
    assign("ConfigRBCDefBagVol", &_procedureCfg.key_rbc_def_bag_vol, "key_rbc_def_bag_vol");
    assign("ConfigPLTDefBagVol", &_procedureCfg.key_plt_def_bag_vol, "key_plt_def_bag_vol");
    assign("ConfigShowPreAasFlags", &_procedureCfg.key_show_pre_aas_flags, "key_show_pre_aas_flags");



	_config_cds->Procedure = _procedureCfg;
	//Product Templates 
	char dlogKeyName[32];
	char configKeyName[32];
	int	i;

	for ( i=0; i<NUM_YIELD_VOLUME; i++ )
	{
	    sprintf(dlogKeyName, "ConfigPltYield%d", i+1);
	    sprintf(configKeyName, "key_plt_yield_%d", i+1);
	    assign(dlogKeyName, &_prodTemplates.key_plt_yield[i], configKeyName);

	    sprintf(dlogKeyName, "ConfigPltVolume%d", i+1);
	    sprintf(configKeyName, "key_plt_volume_%d", i+1);
	    assign(dlogKeyName, &_prodTemplates.key_plt_volume[i], configKeyName);

        sprintf(dlogKeyName, "ConfigPLTMSS%d", i+1);
	    sprintf(configKeyName, "key_plt_mss_%d", i+1);
		assign(dlogKeyName, &_prodTemplates.key_plt_mss[i], configKeyName);

		sprintf(dlogKeyName, "ConfigPctCarryoverVal%d", i+1);
	    sprintf(configKeyName, "key_plt_pct_carryover_%d", i+1);
		assign(dlogKeyName, &_prodTemplates.key_plt_pct_carryover[i], configKeyName);
    }
	for ( i=0; i<NUM_PLASMA_VOLUME; i++ )
	{
		sprintf(dlogKeyName, "ConfigPlasmaVolume%d", i+1);
		assign(dlogKeyName, &_prodTemplates.key_pls_volume[i]);

	}
	for ( i=0; i<NUM_CRIT_DOSE; i++ )
	{
		sprintf(dlogKeyName, "ConfigRBCCrit%d", i+1);
		assign(dlogKeyName, &_prodTemplates.key_rbc_crit[i]);

		sprintf(dlogKeyName, "ConfigRBCDose%d", i+1);
		assign(dlogKeyName, &_prodTemplates.key_rbc_dose[i]);     

        sprintf(dlogKeyName, "ConfigRBCMss%d", i+1);
        sprintf(configKeyName, "key_rbc_mss_%d", i+1);
		assign(dlogKeyName, &_prodTemplates.key_rbc_mss[i], configKeyName);

		sprintf(dlogKeyName, "ConfigRBCMssVolume%d", i+1);
        sprintf(configKeyName, "key_rbc_mss_volume_%d", i+1);
		assign(dlogKeyName, &_prodTemplates.key_rbc_mss_volume[i], configKeyName);
	}

	_config_cds->Templates = _prodTemplates;
	// Predict Config
	assign("ConfigRBCRatio", &_predictionCfg.key_rbc_ratio);
	assign("ConfigYSF", &_predictionCfg.key_ysf);
	assign("ConfigAMAPMin", &_predictionCfg.key_pls_amap_min);
	assign("ConfigAMAPMax", &_predictionCfg.key_pls_amap_max);
    assign("ConfigMaleOnlyPlasma",	&_predictionCfg.key_male_only_plasma, "key_male_only_plasma");
	assign("ConfigRSF", &_predictionCfg.key_rsf);
	assign("ConfigInletFlowRamp", &_predictionCfg.key_inlet_flow_ramp);
	assign("ConfigInletManagement", &_predictionCfg.key_inlet_management);
	assign("ConfigReturnManagement", &_predictionCfg.key_return_management);
	assign("ConfigRBCReplaceFluid", &_predictionCfg.key_rbc_replace_fluid);
	assign("ConfigRBCFluidPercent", &_predictionCfg.key_rbc_fluid_percent);

	//Temporary
	_predictionCfg.key_guaranteed_yield = 0;
	_predictionCfg.key_male_only_plt = 0;
	_predictionCfg.key_confidence_level = 95;
	_predictionCfg.key_coefficient_of_variation = 10;

	_config_cds->Predict = _predictionCfg;
	//Procedure Definition
	for ( i=0; i<NUM_OFFLINE_PROCEDURES; i++ )
	{
		sprintf(dlogKeyName, "ConfigProcPlt%d", i+1);
        sprintf(configKeyName, "key_platelet_%c", i+(int)'a');
		assign(dlogKeyName, &_prodDefArray[i].key_platelet, configKeyName);

		sprintf(dlogKeyName, "ConfigProcPls%d", i+1);
        sprintf(configKeyName, "key_plasma_%c", i+(int)'a');
		assign(dlogKeyName, &_prodDefArray[i].key_plasma, configKeyName);

		sprintf(dlogKeyName, "ConfigProcRBC%d", i+1);
        sprintf(configKeyName, "key_rbc_%c", i+(int)'a');
		assign(dlogKeyName, &_prodDefArray[i].key_rbc, configKeyName);

		sprintf(dlogKeyName, "ConfigProcBloodType%d", i+1);
        sprintf(configKeyName, "key_blood_type_%c", i+(int)'a');
		assign(dlogKeyName, &_prodDefArray[i].key_blood_type, configKeyName);

		sprintf(dlogKeyName, "ConfigProcTime%d", i+1);
        sprintf(configKeyName, "key_time_%c", i+(int)'a');
		assign(dlogKeyName, &_prodDefArray[i].key_time, configKeyName);
	}
	_config_cds->Definition = _prodDefArray;
}

bool ConfigDatReader::assignDefaultMin(string configKeyName, void* address)
{
	CCfgVarList cfgVarList;
	float fMin;
	int iMin;
	for (int i=0; i<cfgVarList.NumberVars(); i++) {
		const CCfgVarInfo* item =cfgVarList.pIthVar(i); 
	   if (configKeyName == item->Name()) {
		   if (item->DisplayType() == INT_TYPE) {
			   iMin = (int)item->Min();
			   memcpy(address, &iMin, sizeof(int));
		   }
		   else { /*Float0, Float1, etc.. */
			   fMin = item->Min();
			   memcpy(address, &fMin, sizeof(float));
		   }
		   return true;
		}
	}
	return false;
}

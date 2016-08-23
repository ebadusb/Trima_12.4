#ifndef __CONFIGDAT_READER_H__
#define __CONFIGDAT_READER_H__
#include "log_format.h"
#include "configdef.h"
#include "config_cds.h"
#include "cfg_vars.h"
#include <set>

class ConfigDatReader : public DLOG_Reader
{
	friend class ConfigDatUtils;
private:
	LangUnitCfg _langCfg;
	BasicMachineCfg _machineCfg;
	ProcedureConfig _procedureCfg;        
	PredictionConfig _predictionCfg;
	ProductTemplates _prodTemplates;
	ProductDefinitionArray _prodDefArray;
	ProductListArray _prodList;

	DLOG_PeriodicLog* _periodicLog;
	DLOGREC_VECTOR* _periodicSetItems;
	Config_CDS* _config_cds;
	set<string> _logItemsFound;

	void assign(string keyName, void* address, string configKeyName="");
	void read();
	bool assignDefaultMin(string configKeyName, void* address);
public:
	ConfigDatReader();
	~ConfigDatReader();
	bool initialize(const char* dlogname);     
	void ReadConfig(Config_CDS* config_cds); 
	bool WriteConfig(string filename);
};
#endif /* __CONFIGDAT_READER_H__ */

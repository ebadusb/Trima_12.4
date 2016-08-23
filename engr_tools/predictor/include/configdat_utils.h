#ifndef __CONFIGDAT_UTILS_H__
#define __CONFIGDAT_UTILS_H__
#include <string>
#include "config_cds.h"
#include "configdef.h"

class ConfigDatUtils
{
public:
	static ConfigStruct GetConfigStructFromDlog(std::string filename);
	static bool SaveConfigStructToFile(ConfigStruct cs, std::string filename);
	static ConfigStruct GetConfigStructFromFile(std::string filename);
	static bool ConfigDatUtils::CreateConfigFileFromDlog(std::string dlogPath, std::string configPath);
	//static int SaveDefault(ConfigStruct cs);
	//static ConfigStruct LoadDefault();

};

#endif /* __CONFIGDAT_UTILS_H__ */

#include "configdat_utils.h"
#include "configdat_reader.h"
#include "configdef.h"
#include "config_cds.h"
#include <string>
//Use sparingly.
ConfigStruct ConfigDatUtils::GetConfigStructFromDlog(std::string dlogPath)
{
	ConfigDatReader datReader;
	datReader.initialize(dlogPath.c_str());
	Config_CDS* userconfig = new Config_CDS(ROLE_RW);
	datReader.ReadConfig(userconfig);
	ConfigStruct cs = userconfig->getConfigStruct();
	delete userconfig;
	return cs;
}

ConfigStruct ConfigDatUtils::GetConfigStructFromFile(std::string pathname)
{
	Config_CDS* userconfig = new Config_CDS(ROLE_RW);
	userconfig->ReadConfig(pathname.c_str());
	ConfigStruct cs = userconfig->getConfigStruct();
	delete userconfig;
	return cs;
}

bool ConfigDatUtils::SaveConfigStructToFile(ConfigStruct cs, std::string filepath)
{
	Config_CDS* userconfig = new Config_CDS(ROLE_RW);
	userconfig->setConfig(cs);
	bool success = userconfig->WriteConfig(filepath);
	delete userconfig;
	return success;

}

bool ConfigDatUtils::CreateConfigFileFromDlog(std::string dlogPath, std::string configPath)
{
	ConfigDatReader datReader;
	Config_CDS* userconfig = new Config_CDS(ROLE_RW);

	datReader.initialize(dlogPath.c_str());
	datReader.ReadConfig(userconfig);
	return datReader.WriteConfig(configPath);
}



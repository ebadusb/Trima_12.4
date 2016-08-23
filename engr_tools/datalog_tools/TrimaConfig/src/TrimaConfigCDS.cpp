/*****************************************************************************\
**
** TrimaConfigCDS.cpp
**
** Trima config CDS interface.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include "TrimaConfigCDS.h"
#include "Config_CDS.h"

namespace CONFIG {

/* Functions *****************************************************************/

TrimaConfigCDS::TrimaConfigCDS(void) : mConfigCDSImplP(NULL)
{
	mConfigCDSImplP = new Config_CDS(ROLE_RW);
}

TrimaConfigCDS::~TrimaConfigCDS(void)
{
	delete mConfigCDSImplP;
}

bool TrimaConfigCDS::WriteConfig(const char *filename)
{
	return mConfigCDSImplP->WriteConfig(filename);
}

bool TrimaConfigCDS::ReadConfig(const char *filename)
{
	return mConfigCDSImplP->ReadConfig(filename);
}

ConfigStruct TrimaConfigCDS::GetConfig(void)
{
	return mConfigCDSImplP->getConfigStruct();
}

void TrimaConfigCDS::SetConfig(const ConfigStruct &config)
{
	mConfigCDSImplP->setConfig(config);
}

} // namespace CONFIG



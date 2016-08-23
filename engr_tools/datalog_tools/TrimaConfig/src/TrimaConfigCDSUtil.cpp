/*****************************************************************************\
**
** TrimaConfigCDSUtil.h
**
** Trima config CDS utility.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include "TrimaConfigCDSUtil.h"
#include "TrimaConfigCDS.h"
#include "ExtractConfig.h"
#include "config_cds.h"
#include "cfg_vars.h"

namespace CONFIG {

/* Functions *****************************************************************/

bool TrimaConfigCDSUtil::LoadCDSFromDatalog(const char *filename, TrimaConfigCDS &cds)
{
	Config_CDS config(ROLE_RW);

	switch (CONFIG::ExtractConfig::Extract(filename, config)) {
	case CONFIG::ExtractConfig::kResultNone:
	case CONFIG::ExtractConfig::kResultDLogUnexpectedEOF:
	case CONFIG::ExtractConfig::kResultDLogUnknownRecordType:
		cds.SetConfig(config.getConfigStruct());
		return true;
		break;
	case CONFIG::ExtractConfig::kResultDLogFileOpenFailed:
	case CONFIG::ExtractConfig::kResultDLogInvalidHeaderType:
	case CONFIG::ExtractConfig::kResultDLogInvalidVersion:
	default:
		return false;
		break;
	}
}

bool TrimaConfigCDSUtil::IsValueValid(CONFIG_DATA_KEYS key, int value, int *min, int *max)
{
	CCfgVarInfo info(key);

	if (min != NULL) {
		*min = info.Min();
	}

	if (max != NULL) {
		*max = info.Max();
	}

	return (value >= info.Min()) && (value <= info.Max());
}

bool TrimaConfigCDSUtil::IsValueValid(CONFIG_DATA_KEYS key, float value, float *min, float *max)
{
	CCfgVarInfo info(key);

	if (min != NULL) {
		*min = info.Min();
	}

	if (max != NULL) {
		*max = info.Max();
	}

	return (value >= info.Min()) && (value <= info.Max());
}

} // namespace CONFIG



/*****************************************************************************\
**
** TrimaConfigCDSUtil.h
**
** Trima config CDS utility.
**
\*****************************************************************************/

#ifndef TRIMACONFIGCDSUTIL_H
#define TRIMACONFIGCDSUTIL_H

/* Includes ******************************************************************/

#include <cstddef>
#include "cfg_ids.h"

/* Macros ********************************************************************/

#define DllExport   __declspec( dllexport )

/* Classes *******************************************************************/

namespace CONFIG {

class TrimaConfigCDS;

class DllExport TrimaConfigCDSUtil {
public:
	static bool LoadCDSFromDatalog(const char *filename, TrimaConfigCDS &cds);

	static bool IsValueValid(CONFIG_DATA_KEYS key, int value, int *min = NULL, int *max = NULL);
	static bool IsValueValid(CONFIG_DATA_KEYS key, float value, float *min = NULL, float *max = NULL);
};

} // namespace CONFIG

#endif // TRIMACONFIGCDSUTIL_H


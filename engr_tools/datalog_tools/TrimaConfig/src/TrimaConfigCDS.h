/*****************************************************************************\
**
** TrimaConfigCDS.h
**
** Trima config CDS interface.
**
\*****************************************************************************/

#ifndef TRIMACONFIGCDS_H
#define TRIMACONFIGCDS_H

/* Includes ******************************************************************/

#include "configdef.h"

/* Macros ********************************************************************/

#define DllExport   __declspec( dllexport )

/* Classes *******************************************************************/

class Config_CDS;

namespace CONFIG {

class DllExport TrimaConfigCDS {
public:
	TrimaConfigCDS(void);
	~TrimaConfigCDS(void);

	bool WriteConfig(const char *filename);
	bool ReadConfig(const char *filename);

	ConfigStruct GetConfig(void);
	void SetConfig(const ConfigStruct &config);

private:
	Config_CDS *mConfigCDSImplP;
};

} // namespace CONFIG

#endif // TRIMACONFIGCDS_H


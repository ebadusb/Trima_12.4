/*****************************************************************************\
**
** CustomSpoofCollection.h
**
** Provides the implementaion for loading custom spoofs.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include "CustomSpoofCollection.h"

#include <vxWorks.h>
#include <dirent.h>
#include <sys/stat.h>

#include "load_module.h"
#include "filenames.h"

/* Constants *****************************************************************/

static const char *kCreateFunctionPrefix = "Create";

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

/**
 * 
 * CustomSpoofCollection
 * 
 * Constructor. Loads all the custom spoofs.
 * 
 */
CustomSpoofCollection::CustomSpoofCollection(void)
{
	 DIR *dir = opendir(CUSTOMSPOOF_PATH);

	 DataLog(log_level_spoofer_info) << "Begin loading spoofs from directory: " << CUSTOMSPOOF_PATH << endmsg;

	 if (dir) {
		  struct dirent *dirEntry = NULL;

		  // Search directory for custom spoofs.
		  while ((dirEntry = readdir(dir)) != NULL) {
				char *filename = new char[strlen(CUSTOMSPOOF_PATH) + 1 + strlen(dirEntry->d_name) + 1];
				struct stat	fileStat = { 0 };

				strcpy(filename, CUSTOMSPOOF_PATH);
				strcat(filename, "/");
				strcat(filename, dirEntry->d_name);

				if (!stat(filename, &fileStat) && S_ISREG(fileStat.st_mode)) {
					 DataLog(log_level_spoofer_info) << "Loading spoof file: " << filename << endmsg;

					 LoadModuleInfo info = { 0 };
					 STATUS status = loadModuleFromFile(filename, &info);

					 if (status == OK) {
						  char *functionName = new char[strlen(kCreateFunctionPrefix) + strlen(dirEntry->d_name) + 1];
						  CreateCustomSpoof function = NULL;
						  SYM_TYPE symType;

						  strcpy(functionName, kCreateFunctionPrefix);
						  strcat(functionName, dirEntry->d_name);
						  *(strchr(functionName, '.')) = 0;

						  status = symFindByName(sysSymTbl, functionName, (char **)&function, &symType);

						  if (status == OK) {
								DataLog(log_level_spoofer_info) << "Adding spoof create handler: " << functionName << endmsg;

								mCustomSpoofCreateFunctions.push_back(function); 
						  }

						  delete []functionName;
					 }
				}

				delete []filename;
		  }

		  closedir(dir);
	 }

	 DataLog(log_level_spoofer_info) << "End loading spoofs from directory: " << CUSTOMSPOOF_PATH << endmsg;
}

/**
 * 
 * ~CustomSpoofCollection
 * 
 * Destructor.
 * 
 */
CustomSpoofCollection::~CustomSpoofCollection(void)
{
}

/**
 * 
 * GetInstance
 * 
 * Gets the custom spoof collection instance.
 * 
 * @return The custom spoof collection.
 * 
 */
const CustomSpoofCollection::CustomSpoofCollection& CustomSpoofCollection::GetInstance(void) 
{
  static CustomSpoofCollection collection;

  return collection;
}

/**
 * 
 * Begin
 * 
 * Gets an iterator to the beginning of the handler list.
 * 
 * @return An iterator to the beginning of the handler list.
 * 
 */
CustomSpoofCollection::Iterator CustomSpoofCollection::Begin(void) const
{ 
	 return mCustomSpoofCreateFunctions.begin(); 
}

/**
 * 
 * End
 * 
 * Gets an iterator to the end of the handler list.
 * 
 * @return An iterator to the end of the handler list.
 * 
 */
CustomSpoofCollection::Iterator CustomSpoofCollection::End(void) const
{ 
	 return mCustomSpoofCreateFunctions.end(); 
}



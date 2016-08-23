/*
 * $Header*
 *
 * Trima Driver Load function
 *
 * $Log$
 *
 */

#include <vxworks.h>
#include <symLib.h>

#include "trima_driver.h"
#include "trima_options.h"


void loadTrimaHWDriver (const char* name)
{
   LoadModuleInfo info;
   SYM_TYPE       symType;

   int            (* hwInitFunc)(void);
   const char*    fileName = trimaSysGetHardwareSetting(name);

   if (!fileName || loadModuleFromFile(fileName, &info) != OK)
   {
      if (!fileName)
      {
         printf("missing setting for \"%s\"\n", name);
      }
      else
      {
         perror(fileName);
      }

      usrCommonInitSetError("failed to load Trima hardware driver");
      return;
   }

   /* used to print size as well as load address but size is no longer supported */
   printf ("%s T=%lx D=%lx B=%lx\n", fileName, info.textAddr, info.dataAddr, info.bssAddr);

   if (symFindByName(sysSymTbl, "hw_init", (char**)&hwInitFunc, &symType) != OK)
   {
      perror("hw_init");
      usrCommonInitSetError("Failed to locate Trima hardware driver initialization function");
      return;
   }

   (* hwInitFunc)();
}

/* FORMAT HASH afbee4d2d6912ef778daf35fb5d85731 */

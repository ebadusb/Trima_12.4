/*
 * $Header$
 *
 * This class loads the appropriate UGL layer based on the
 * detected video hardware.
 *
 */

#include "load_ugl.h"

#include <vxWorks.h>
#include <stdio.h>
#include <drv/pci/pciConfigLib.h>
#include <drv/pci/pciIntLib.h>
#include "filenames.h"
#include "load_module.h"
#include "discover_hw.h"


bool loadUglModule (void)
{
   bool         retVal   = false;
#if CPU != SIMNT
   const UINT32 vendorID = getGraphicsVendorID();

   /* Is this device recognized by the system? */
   if (vendorID != 0)
   {
      LoadModuleInfo info;
      char           filename[80];

      sprintf(filename, UGL_MODULE_PATH "/UGL_%08X.o", vendorID);
      printf("Loading UGL module file:'%s'\n", filename);

      if (loadModuleFromFile(filename, &info) == OK)
      {
         retVal = true;
      }
   }

   /* If no acceptable PCI device was found, load the default. */
   if (!retVal)
   {
      printf("PCI display (%x) not found.\n", vendorID);
   }
#endif
   return retVal;
}

/* FORMAT HASH 83387aff83d96f54d918fd0701552608 */

/*
 * Copyright (C) 2005 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/os/rcs/trima_options.c 1.1 2007/11/07 22:45:09Z dslausb Exp $
 * $Log: trima_options.c $
 * Revision 1.1  2007/11/07 22:45:09Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:38:45Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:34:19Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:30:47Z  dslausb
 * Initial revision
 * Revision 1.2  2007/07/06 21:44:50Z  dslausb
 * Got rid of references to trimaOpSys stuff, because it's now obsolete
 * Revision 1.1  2007/05/22 18:20:18Z  dslausb
 * Initial revision
 *
 */

#include <vxworks.h>
#include <usrLib.h>


#include "os/option_file.h"
#include "os/trima_options.h"


static OptionFileData* hwData = NULL;

const char* trimaSysGetHardwareSetting (const char* optionName)
{
   const char* result = NULL;
   if ( getOption(optionName, &result, hwData) == OK &&
        strspn(result, " \t") == strlen(result) )
   {
      /* treat empty setting line as if not present */
      result = NULL;
   }

   return result;
}


/*
 * Loads the Trima Option files
 *
 */
int trimaSysLoadOptionFiles (void)
{
   int result = 0;

   if ( readOptionFile(PNAME_HWDAT, "DRIVERS", &hwData, 1) != OK )
   {
      result = -1;
   }

   return result;
}

/* FORMAT HASH 7dc0cc0627498d721c3b1135aa00f27b */

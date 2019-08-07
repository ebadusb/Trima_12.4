/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1999 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  NA

*     File name:   trimver.h

*     Contents:    Single string representing the current release of Trima software

*     Description:

        This string is #included into the English string table (stringen.cpp)
          for Trima GUI. This string is the one that appears on the intial Trima
          "splash screen" displayed after a Trima re-boot. It has been broken
          out into its own module so that other sub-domain programs can #include
          it into their source code.

*************************< FOCUSSED GUI DOMAIN >*****************************/
#ifndef __TRIMVER__H

#define __TRIMVER__H

/**********************************************************************************
 NOTE: This file is now shared with Vista. Take care when adding new dependencies
 **********************************************************************************/

static const char DCL_TEXT_PWRUP_VERSION [] =                    "Version 6.4" ;
static const char TEXT_FULL_VERSION []      =                         "Version 6.4.2";

#endif

/* FORMAT HASH a08c4d8d177da6ebc615dd18df40a743 */

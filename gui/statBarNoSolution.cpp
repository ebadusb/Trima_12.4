/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  StatBarBase

*     File name:   statBarBase.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >***************************/

#include <vxworks.h>
#include "aphwind.hpp"
#include "guiglobs.hpp"
#include "statBarNoSolution.hpp"


StatBarNoSolution::StatBarNoSolution()
   : StatBarBase(STATUS_BAR_TYPE_NO_ADDITION,
                 textProgBarLoad,
                 textProgBarDonorInfo,
                 textProgBarDonorPrep,
                 textProgBarDonate,
                 textEmpty,
                 textProgBarNoAdditionUnload)
{
   statBarGraphics["CCCCC"] = BITMAP_STATUS_NO_SOL_CCCCC;
   statBarGraphics["CGCCC"] = BITMAP_STATUS_NO_SOL_CGCCC;
   statBarGraphics["CYCCC"] = BITMAP_STATUS_NO_SOL_CYCCC;
   statBarGraphics["GCCCC"] = BITMAP_STATUS_NO_SOL_GCCCC;
   statBarGraphics["GGCCC"] = BITMAP_STATUS_NO_SOL_GGCCC;
   statBarGraphics["GGGGG"] = BITMAP_STATUS_NO_SOL_GGGGG;
   statBarGraphics["GGGGY"] = BITMAP_STATUS_NO_SOL_GGGGY;
   statBarGraphics["GGGYC"] = BITMAP_STATUS_NO_SOL_GGGYC;
   statBarGraphics["GGYCC"] = BITMAP_STATUS_NO_SOL_GGYCC;
   statBarGraphics["GYCCC"] = BITMAP_STATUS_NO_SOL_GYCCC;
   statBarGraphics["YCCCC"] = BITMAP_STATUS_NO_SOL_YCCCC;
   statBarGraphics["YGCCC"] = BITMAP_STATUS_NO_SOL_YGCCC;
}

string StatBarNoSolution::getStatusString ()
{
   char statusString[6] = "" ;

   // Build the status pattern based on the contents of the _currentPhase
   sprintf(statusString, "%c%c%c%c%c",
           convert_status_to_strchar(_currentPhase.status_load),
           convert_status_to_strchar(_currentPhase.status_info),
           convert_status_to_strchar(_currentPhase.status_prepare),
           convert_status_to_strchar(_currentPhase.status_donate),
           convert_status_to_strchar(_currentPhase.status_unload));

   return statusString;
}

/* FORMAT HASH f2827a009b8b2d0d3bd63b0e1eb7eeb3 */

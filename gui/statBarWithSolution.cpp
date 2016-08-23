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
#include "statBarWithSolution.hpp"


StatBarWithSolution::StatBarWithSolution()
   : StatBarBase(STATUS_BAR_TYPE_WITH_ADDITION,
                 textProgBarLoad,
                 textProgBarDonorInfo,
                 textProgBarDonorPrep,
                 textProgBarDonate,
                 textProgBarAddition,
                 textProgBarUnload)
{
   statBarGraphics["CCCCCC"] = BITMAP_STATUS_WITH_SOL_CCCCCC;
   statBarGraphics["CGCCCC"] = BITMAP_STATUS_WITH_SOL_CGCCCC;
   statBarGraphics["CYCCCC"] = BITMAP_STATUS_WITH_SOL_CYCCCC;
   statBarGraphics["GCCCCC"] = BITMAP_STATUS_WITH_SOL_GCCCCC;
   statBarGraphics["GGCCCC"] = BITMAP_STATUS_WITH_SOL_GGCCCC;
   statBarGraphics["GGGGCC"] = BITMAP_STATUS_WITH_SOL_GGGGCC;
   statBarGraphics["GGGGGG"] = BITMAP_STATUS_WITH_SOL_GGGGGG;
   statBarGraphics["GGGGGY"] = BITMAP_STATUS_WITH_SOL_GGGGGY;
   statBarGraphics["GGGGYC"] = BITMAP_STATUS_WITH_SOL_GGGGYC;
   statBarGraphics["GGGYCC"] = BITMAP_STATUS_WITH_SOL_GGGYCC;
   statBarGraphics["GGYCCC"] = BITMAP_STATUS_WITH_SOL_GGYCCC;
   statBarGraphics["GYCCCC"] = BITMAP_STATUS_WITH_SOL_GYCCCC;
   statBarGraphics["YCCCCC"] = BITMAP_STATUS_WITH_SOL_YCCCCC;
   statBarGraphics["YGCCCC"] = BITMAP_STATUS_WITH_SOL_YGCCCC;
}

string StatBarWithSolution::getStatusString ()
{
   char statusString[7] = "" ;

   // Build the status pattern based on the contents of the _currentPhase
   sprintf(statusString, "%c%c%c%c%c%c",
           convert_status_to_strchar(_currentPhase.status_load),
           convert_status_to_strchar(_currentPhase.status_info),
           convert_status_to_strchar(_currentPhase.status_prepare),
           convert_status_to_strchar(_currentPhase.status_donate),
           convert_status_to_strchar(_currentPhase.status_solutions),
           convert_status_to_strchar(_currentPhase.status_unload));

   return statusString;
}

/* FORMAT HASH 74afbf3569f7fc13b49d82c1b1faafb0 */

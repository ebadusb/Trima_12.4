/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2009 GambroBCT, Inc. All rights reserved

*     Author:       Dan Slauson (x4664)

*     File name:    reasonMap.h

*     Contents:     ENUMERATED_REASON_TO_SHOW

*     Description:

*         This file enumerates the reasons to show, and sets up
          a data type for reason mapping.  This source is separate
*		  out and written in such a way that it can be used in
          win32 tools, like the reason_table utility in engr_tools.
*		  Because of that, take care when adding dependencies.

*************************< FOCUSSED GUI DOMAIN >****************************
*
*	$Header: E:/BCT_Development/Trima5.R/Trima/trima_common/rcs/reasonMap.h 1.1 2009/03/09 18:28:20Z dslausb Exp $
*	$Log: reasonMap.h $
*
****************************************************************************/


#ifndef __REASON_MAP_H__
#define __REASON_MAP_H__

#include "verifyflags.h"

// This is the list of message enumerations for any given reason message to be shown.  Add new ones here.
// NOTE:
// This list of ENUMS is used in the Vista Interface.  Do not make changes here without also
// changing the Vista interface and ICD.
//
enum ENUMERATED_REASON_TO_SHOW
{
   SHOW_NOTHING,

   // REASON_TO_SHOW_FIRST_ITEM must have the enumeration value of the first valid reason
   // on the list
   REASON_TO_SHOW_FIRST_ITEM,
   SHOW_CENTRIFUGE_STOPPED = REASON_TO_SHOW_FIRST_ITEM,
   SHOW_DETECTED_CONCENTRATION_TOO_LOW,
   SHOW_DONOR_HEMATOCRIT_CHANGE_TOO_LARGE,
   SHOW_DONOR_HEMATOCRIT_TOO_HIGH,
   SHOW_OBSOLETE_1, // obsolete ; keep to preserve numbering from 5.1/5.2.
   SHOW_TARGET_VOL_NOT_ACHIEVED,
   SHOW_PLATELET_CONCENTRATION_TOO_HIGH,
   SHOW_POSSIBLE_AIR_BLOCK,
   SHOW_PREDICTED_CONCENTRATION_BELOW_STORAGE_RANGE,
   SHOW_PREDICTED_CONCENTRATION_ABOVE_STORAGE_RANGE,
   SHOW_RBC_SPILLOVER,
   SHOW_RBC_DETECTOR_CALIBRATION_ERROR,
   SHOW_OBSOLETE_2, // obsolete; keep to preserve numbering from 5.1/5.2.
   SHOW_PLATELET_CONTAMINANT_DETECTION,
   SHOW_METERED_VOLUME_ERROR,
   SHOW_RINSEBACK_NOT_COMPLETED,
   SHOW_CPS_HIGH,
   SHOW_APS_HIGH,
   SHOW_FINAL_CONC_NOT_ACHIEVED,
   SHOW_METERED_SOLUTION_PRIME_ERROR,
   SHOW_CHANNEL_CLAMP_ERROR,
   SHOW_WBC_CONTENT_NOT_VALIDATED,

   // Must be last item in enumeration
   REASON_TO_SHOW_END_OF_LIST
};

typedef struct
{
   ENUMERATED_REASON         reason;
   ENUMERATED_REASON_TO_SHOW show;
} REASON_MAPPING;

#endif // #ifndef __REASON_MAP_H__

/* FORMAT HASH d7fa2ef160611861e8dca6c6714c8bc7 */

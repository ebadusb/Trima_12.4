/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2009 GambroBCT, Inc. All rights reserved

*     Author:       Dan Slauson (x4664)

*     Class name:   reasonToString

*     File name:    reasonToString.cpp

*     Contents:     static helper functions

*     Description:

*        The concept here is pretty simple: the functions define a mapping between
*        an enumeration and its corresponding TEXT_STRING_ID for getting strings
*        related to the Run Summary Product Verification screens. As such, it
*        provides a central, decoupled file for all of the Product Verify screens.
*        That's it.  The source file is separated out and written in such a way
*        that it can be used in win32 tools, like the reason_table and
*        mkProdVerifyStrs.awk utilities in engr_tools. As such, take care when
*        adding dependencies.
*
*		 ALSO, don't reformatting the code willy-nilly or change variable name
*		 as such, as the awk parsing script may depend on a specific format!!!
*
*		 Also, note that there's no corresponding header. I figured that since
         it's just a few functions, you could add a prototype for the functions
*		 just as easily as you could add in #include file.

*
*************************< FOCUSSED GUI DOMAIN >****************************
*
****************************************************************************/

#include <map>

#include "reasonMap.h"

#ifdef WIN32
#include <iostream>
using namespace std;
#define ERRLOG    cerr
#define ENDERRLOG endl
#else
#include "trima_datalog.h"
#define ERRLOG    DataLog(log_level_gui_error)
#define ENDERRLOG endmsg
#endif

#include "string_id.h"
#include "gui.h"

TEXT_STRING_ID getStringIdFromReasonCode (ENUMERATED_REASON_TO_SHOW reasonToShow)
{
   switch (reasonToShow)
   {
      case SHOW_CENTRIFUGE_STOPPED :                return textProdVerifReasonCentrifugeStopped;
      case SHOW_DETECTED_CONCENTRATION_TOO_LOW :    return textProdVerifReasonLowPltConc;
      case SHOW_DONOR_HEMATOCRIT_CHANGE_TOO_LARGE : return textProdVerifReasonHighDeltaHct;
      case SHOW_DONOR_HEMATOCRIT_TOO_HIGH :         return textProdVerifReasonHctHigh;
      case SHOW_TARGET_VOL_NOT_ACHIEVED :           return textProdVerifReasonTargetVolumesNotAchieved;
      case SHOW_PLATELET_CONCENTRATION_TOO_HIGH :   return textProdVerifReasonHighPltConc;
      case SHOW_POSSIBLE_AIR_BLOCK :                return textProdVerifReasonAirBlockRecovery;
      case SHOW_PREDICTED_CONCENTRATION_BELOW_STORAGE_RANGE :   return textProdVerifReasonPltConcBelowStorageRange;
      case SHOW_PREDICTED_CONCENTRATION_ABOVE_STORAGE_RANGE :   return textProdVerifReasonPltConcAboveStorageRange;
      case SHOW_RBC_SPILLOVER :                     return textProdVerifReasonRbcSpillover;
      case SHOW_RBC_DETECTOR_CALIBRATION_ERROR :    return textProdVerifReasonRbcDetectorError;
      case SHOW_PLATELET_CONTAMINANT_DETECTION :    return textProdVerifReasonPltContamDetected;
      case SHOW_METERED_VOLUME_ERROR :              return textProdVerifReasonAasVolError;
      case SHOW_RINSEBACK_NOT_COMPLETED :           return textProdVerifReasonRinsebackNotCompleted;
      case SHOW_CPS_HIGH :                          return textProdVerifReasonCpsHigh;
      case SHOW_APS_HIGH :                          return textProdVerifReasonApsHigh;
      case SHOW_FINAL_CONC_NOT_ACHIEVED :           return textProdVerifReasonFinalConcNotAchieved;
      case SHOW_METERED_SOLUTION_PRIME_ERROR :      return textProdVerifReasonAasPrimeError;
      case SHOW_CHANNEL_CLAMP_ERROR :               return textProdVerifReasonChannelClampError;
      case SHOW_WBC_CONTENT_NOT_VALIDATED :         return textProdVerifReasonWbcNotValidated;

      default :
         ERRLOG << "Unknown reason to show: " << reasonToShow << ENDERRLOG;
         return textEmpty;
   }
}

// Similarly, this helper function maps the string ids to use for the header
// text displayed in the reason box
//
TEXT_STRING_ID getReasonHeaderIdFromVerifyFlag (ENUMERATED_FLAG verifyFlag)
{
   switch (verifyFlag)
   {
      case VERIFY_PLATELET_WBCS :     return textProdVerifReasonHeaderPurity;
      case VERIFY_PLATELET_YIELD :    return textProdVerifReasonHeaderPltYieldVol;
      case VERIFY_PLASMA_VOL :        return textProdVerifReasonHeaderPlasVol;
      case VERIFY_PLASMA_WBCS :       return textProdVerifReasonHeaderPurity;
      case VERIFY_RBC_VOL :           return textProdVerifReasonHeaderRbcProduct;
      case VERIFY_RBC_WBCS :          return textProdVerifReasonHeaderPurity;
      case VERIFY_RBC_QUALITY :       return textProdVerifReasonHeaderRbcQuality;
      case VERIFY_RBC_VOL_PROD2 :     return textProdVerifReasonHeaderRbcProduct;
      case VERIFY_RBC_WBCS_PROD2 :    return textProdVerifReasonHeaderPurity;
      case VERIFY_RBC_QUALITY_PROD2 : return textProdVerifReasonHeaderRbcQuality;
      case CONC_OUT_OF_RANGE :        return textProdVerifReasonHeaderPltConc;
      default :
         ERRLOG << "Unknown verify flag: " << verifyFlag << ENDERRLOG;
         return textEmpty;
   }
}

// Map to associate Product Verification strings with Run Summary Product screens
typedef map<GUI_SCREEN_ID, TEXT_STRING_ID> PVSMap;

// This maps the Product Verification "Category" string (column heading for the
// verification buttons)
//
TEXT_STRING_ID getProdVerifyCategory (GUI_SCREEN_ID screenId)
{
   static PVSMap PVSMap_Category;
   if (PVSMap_Category.empty())
   {
      PVSMap_Category[GUI_SCREEN_RUNSUM_PLASMA]    = textProdVerifLeukLabelPlasma;
      PVSMap_Category[GUI_SCREEN_RUNSUM_PLT]       = textProdVerifLeukLabelPlt;
      PVSMap_Category[GUI_SCREEN_RUNSUM_RBC]       = textProdVerifLeukLabelRbc;
      PVSMap_Category[GUI_SCREEN_RUNSUM_RBC_PROD1] = textProdVerifLeukLabelRbc1;
      PVSMap_Category[GUI_SCREEN_RUNSUM_RBC_PROD2] = textProdVerifLeukLabelRbc2;
   }
   PVSMap::const_iterator it = PVSMap_Category.find(screenId);
   return (it != PVSMap_Category.end() ? it->second : TEXT_NULL );
}

// This maps the Product Verification "Do Label" string
//
TEXT_STRING_ID getProdVerifyDoLabel (GUI_SCREEN_ID screenId)
{
   static PVSMap PVSMap_DoLabel;
   if (PVSMap_DoLabel.empty())
   {
      PVSMap_DoLabel[GUI_SCREEN_RUNSUM_PLASMA]    = textProdVerifLeukMsgPlasLeukoreduced;
      PVSMap_DoLabel[GUI_SCREEN_RUNSUM_PLT]       = textProdVerifLeukMsgPltLeukoreduced;
      PVSMap_DoLabel[GUI_SCREEN_RUNSUM_RBC]       = textProdVerifLeukMsgRbcLeukoreduced;
      PVSMap_DoLabel[GUI_SCREEN_RUNSUM_RBC_PROD1] = textProdVerifLeukMsgRbc1Leukoreduced;
      PVSMap_DoLabel[GUI_SCREEN_RUNSUM_RBC_PROD2] = textProdVerifLeukMsgRbc2Leukoreduced;
   }
   PVSMap::const_iterator it = PVSMap_DoLabel.find(screenId);
   return (it != PVSMap_DoLabel.end() ? it->second : TEXT_NULL );
}

// This maps the Product Verification "Don't Label" string
//
TEXT_STRING_ID getProdVerifyDontLabel (GUI_SCREEN_ID screenId)
{
   static PVSMap PVSMap_DontLabel;
   if (PVSMap_DontLabel.empty())
   {
      PVSMap_DontLabel[GUI_SCREEN_RUNSUM_PLASMA]    = textProdVerifLeukMsgPlasNotLeukoreduced;
      PVSMap_DontLabel[GUI_SCREEN_RUNSUM_PLT]       = textProdVerifLeukMsgPltNotLeukoreduced;
      PVSMap_DontLabel[GUI_SCREEN_RUNSUM_RBC]       = textProdVerifLeukMsgRbcNotLeukoreduced;
      PVSMap_DontLabel[GUI_SCREEN_RUNSUM_RBC_PROD1] = textProdVerifLeukMsgRbc1NotLeukoreduced;
      PVSMap_DontLabel[GUI_SCREEN_RUNSUM_RBC_PROD2] = textProdVerifLeukMsgRbc2NotLeukoreduced;
   }
   PVSMap::const_iterator it = PVSMap_DontLabel.find(screenId);
   return (it != PVSMap_DontLabel.end() ? it->second : TEXT_NULL );
}

// This maps the Product Verification status line string
//
TEXT_STRING_ID getProdVerifyStatusLine (GUI_SCREEN_ID screenId)
{
   static PVSMap PVSMap_StatusLine;
   if (PVSMap_StatusLine.empty())
   {
      PVSMap_StatusLine[GUI_SCREEN_RUNSUM_PLASMA]    = textStatEndOfRunPlasma;
      PVSMap_StatusLine[GUI_SCREEN_RUNSUM_PLT]       = textStatEndOfRunPlt;
      PVSMap_StatusLine[GUI_SCREEN_RUNSUM_RBC]       = textStatEndOfRunRbc;
      PVSMap_StatusLine[GUI_SCREEN_RUNSUM_RBC_PROD1] = textStatRbc1Runsum;
      PVSMap_StatusLine[GUI_SCREEN_RUNSUM_RBC_PROD2] = textStatRbc2Runsum;
   }
   PVSMap::const_iterator it = PVSMap_StatusLine.find(screenId);
   return (it != PVSMap_StatusLine.end() ? it->second : TEXT_NULL );
}

/* FORMAT HASH d44e8f2ef5b7e77377336a3bb17ebf3a */

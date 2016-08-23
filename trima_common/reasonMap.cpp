/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2009 GambroBCT, Inc. All rights reserved

*     Author:       Dan Slauson (x4664)

*     File name:    reasonMap.cpp

*     Contents:     _reason_map data strucure

*     Description:

*         This is just a table that maps ENUMERATED_REASONs to
          their corresponding ENUMERATED_REASON_TO_SHOW values.
*		  The source file is separated out and written in such a
          way that it can be used in win32 tools, like the
*		  reason_table utility in engr_tools.  Because of that,
          take care when adding dependencies.
*
*************************< FOCUSSED GUI DOMAIN >****************************
*
*	$Header: E:/BCT_Development/Trima5.R/Trima/trima_common/rcs/reasonMap.cpp 1.1 2009/03/09 19:08:58Z dslausb Exp dslausb $
*	$Log: reasonMap.cpp $
*	Revision 1.1  2009/03/09 19:08:58Z  dslausb
*	Initial revision
*
****************************************************************************/

#include "reasonMap.h"
#include "verifyflags.h"

// For WIN32, _reason_map[] is a global
// Otherwise, _reason_map[] is a member of the PQInformation class
#ifdef WIN32
 #define REASON_MAP_DECL   REASON_MAPPING
#else
#include "pqinformation.h"
 #define REASON_MAP_DECL   REASON_MAPPING PQInformation::
#endif

REASON_MAP_DECL _reason_map[] = {
//      Reason Flag                          Message to Show                             Table CV-4 text (reversed)
   { SPILL_DETECTION,                     SHOW_RBC_SPILLOVER },                                            // RBC spillover detected  RBC spillover
   { SPILL_RECOVERY,                      SHOW_RBC_SPILLOVER },                                            // RBC spillover recovery  RBC spillover
   { STOPPED_CENTRIFUGE,                  SHOW_CENTRIFUGE_STOPPED },                                       // centrifuge stopped  centrifuge stopped
   { RBC_DETECTOR_ERROR,                  SHOW_RBC_DETECTOR_CALIBRATION_ERROR },                           // RBC-Detector error  RBC-Detector calibration error *
   { HIGH_PLATELET_CONCENTRATION,         SHOW_PLATELET_CONCENTRATION_TOO_HIGH },                          // platelet concentration too high  platelet concentration too high *
   { LOW_CHAMBER_FLOW,                    SHOW_PLATELET_CONCENTRATION_TOO_HIGH },                          // chamber flow too low  platelet concentration too high *
   { HIGH_DELTA_HCT,                      SHOW_DONOR_HEMATOCRIT_CHANGE_TOO_LARGE },                        // hematocrit change  donor hematocrit change too large
   { AIR_BLOCK_RECOVERY,                  SHOW_POSSIBLE_AIR_BLOCK },                                       // air block recovery  possible air block
   { HIGH_HCT,                            SHOW_DONOR_HEMATOCRIT_TOO_HIGH },                                // hematocrit too high  donor hematocrit too high *
   { PLATELET_CONTAMINANT_DETECTION,      SHOW_PLATELET_CONTAMINANT_DETECTION },                           // contamination detected
   { RUN_ENDED_EARLY,                     SHOW_TARGET_VOL_NOT_ACHIEVED },                                  // run ended early  run ended early
   { LOW_DETECTED_PLATELET_CONCENTRATION, SHOW_DETECTED_CONCENTRATION_TOO_LOW },                           // detected concentration too low  detected concentration too low
   { HIGH_PLATELET_YIELD_DEVIATION,       SHOW_TARGET_VOL_NOT_ACHIEVED },                                  // platelet predict yield out of range  late adjustments or input changes
   { LONG_DRAW,                           SHOW_POSSIBLE_AIR_BLOCK },                                       // draw cycle too long  possible air block
   { SHORT_DRAW,                          SHOW_POSSIBLE_AIR_BLOCK },                                       // draw cycle too small  possible air block
   { PLATELET_VOLUME_ERROR,               SHOW_TARGET_VOL_NOT_ACHIEVED },                                  // platelet product volume error  late adjustments or input changes
   { PLASMA_VOLUME_ERROR,                 SHOW_TARGET_VOL_NOT_ACHIEVED },                                  // plasma product volume error  late adjustments or input changes
   { RBC_VOLUME_ERROR,                    SHOW_TARGET_VOL_NOT_ACHIEVED },                                  // RBC product volume error  late adjustments or input changes
   { PLATELET_CONCENTRATION_BELOW_STORAGE_RANGE,       SHOW_PREDICTED_CONCENTRATION_BELOW_STORAGE_RANGE }, // platelet concentration < 1000  predicted concentration < 1000
   { PLATELET_CONCENTRATION_ABOVE_STORAGE_RANGE,       SHOW_PREDICTED_CONCENTRATION_ABOVE_STORAGE_RANGE }, // platelet concentration > 2100  predicted concentration > 2100
   { METERED_SOLUTION_VOLUME_ERROR,       SHOW_METERED_VOLUME_ERROR },                                     // Metered Solution processing compromised
   { METERED_SOLUTION_ENDED_BY_OPERATOR,  SHOW_METERED_VOLUME_ERROR },                                     // Metered Solution processing compromised
   { RINSEBACK_NOT_COMPLETED,             SHOW_RINSEBACK_NOT_COMPLETED},
   { CPS_HIGH,                            SHOW_CPS_HIGH},
   { APS_HIGH,                            SHOW_APS_HIGH},
   { FINAL_CONC_NOT_ACHIEVED,             SHOW_FINAL_CONC_NOT_ACHIEVED},
   { METERED_SOLUTION_PRIME_ERROR,        SHOW_METERED_SOLUTION_PRIME_ERROR},
   { CHANNEL_CLAMP_ERROR,                 SHOW_CHANNEL_CLAMP_ERROR},
   { TRIPLE_PPC_AS_NON_LEUKOREDUCED,      SHOW_WBC_CONTENT_NOT_VALIDATED},          // Non-Luekoreduced Triple PPC condition
   { NULL_REASON,                         SHOW_NOTHING }                            // END OF TABLE marker; insert new ones above here
};

/* FORMAT HASH 64e2cb3aa6b95073f3d3377bace85da3 */

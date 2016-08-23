/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      runsummary.h
 *
 *             Structure used to communicate procedure and
 *              product statistics from the Procedure to the GUI.
 *             (used in the RUNSUMMx screens).
 *
 ******************************************************************************/


#ifndef _RUNSUMMARY_H_
#define _RUNSUMMARY_H_

#include <time.h>
#include "productqualitydata.h"

typedef enum
{
   SPECIAL_RINSEBACK_STATUS_INCOMPLETE,
   SPECIAL_RINSEBACK_STATUS_COMPLETE
} SPECIAL_RINSEBACK_STATUS;

typedef struct
{
   //
   // Ongoing status (stuff Vista doesn't care about)
   //
   bool run_complete;                         // true if all data is final
   bool plasma_rinseback_active;              // true if currently performing plasma rinseback
   bool saline_rinseback_active;              // true if currently performing saline rinseback
   bool meteringStarted;                      // true if metering started (only show start time if this is true).

   //
   // Procedure related information
   //
   int                      rs1_total_AC_used;             // Units: mL
   struct tm                rs1_end_of_run_time;           // Units: VX rep. of point in time
   struct tm                rs1_start_of_aas_time;         // Units: VX rep. of point in time
   int                      rs1_length_of_run;             // Units: minutes
   float                    rs1_post_hct;                  // Units: % Hct.
   int                      rs1_post_platelet_count;       // Units: "n" where: n x 1000/uL
   int                      rs1_actual_AC_to_donor;        // Units: mL
   int                      rs1_total_blood_vol_processed; // Units: mL
   int                      rs1_residual_loss_prbc;        // Units: mL
   int                      rs1_residual_loss_plasma;      // Units: mL
   int                      replacement_fluid_volume;      // Units: mL
   int                      rs1_last_adjust_flow_rate;     // Units: mL/min

   SPECIAL_RINSEBACK_STATUS plasma_rinseback_status;
   SPECIAL_RINSEBACK_STATUS saline_rinseback_status;

   //
   // Product related information
   //
   bool drbcAutoSplit;                      // true if displaying data for two products
   int  rs2_platelet_volume;                // Units: mL
   int  rs2_yield_of_platelets;             // Units: "n" where: n x 10^11
   int  rs2_vol_AC_in_platelets;            // Units: mL
   int  rs2_SS_plat_vol;                    // Units: mL
   int  rs2_plasma_volume;                  // Units: mL
   int  rs2_vol_AC_in_plasma;               // Units: mL
   int  rs2_RBC_volume;                     // Units: mL
   int  rs2_RBC_volume_2;                   // Units: mL
   int  rs2_vol_AC_in_RBC;                  // Units: mL
   int  rs2_vol_AC_in_RBC_2;                // Units: mL
   int  rs2_SS_rbc_vol;                     // Units: mL
   int  rs2_SS_rbc_vol_2;                   // Units: mL
   int  rs2_comp_pls_volm;                  // Units: mL
   bool rs2_comp_pls_icon;                  // true means display icon

   int  platelet_salvage_volume;            // amount of AS to add to plt product offline
                                            // 0 if no offline needed.
   int  rbc_salvage_volume;                 // amount of AS to add to RBC product offline (0 if no offline needed).
   int  rbc_salvage_volume_2;               // amount of AS to add to RBC product offline (0 if no offline needed).

   //
   // Product Quality Data
   //
   ProductQualityData PQdata;

} RUN_SUMMARY_STRUCT;

#endif

/* FORMAT HASH faabb3ba48789724f0bb76954e0e77a2 */

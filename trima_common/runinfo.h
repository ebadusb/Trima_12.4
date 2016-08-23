/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      runinfo.h
 *            Definition of runinfo data for GUI.
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/

#if !defined( INCLUDE_RUNINFO )
#define INCLUDE_RUNINFO

struct CURRENT_PROCEDURE_STATUS_STRUCT
{
   char  alarm_condition;             // Alarm condition = 1, not in alarm = 0
   char  return_pump_cycle;           // Draw cycle = 0, return = 1
   float current_inlet_pressure_max;  // APS high Limit mmHg
   float current_inlet_pressure_min;  // APS Low Limit mmHg
   int   use_startup_pressure_meter;  // zero == use normal pressure meter
   float current_inlet_pressure;      // Current APS in mmHg
   float current_platelets_collected; // Current Yield (*1E11)
   float current_plasma_collected;    // Current Volume (ml)
   float current_rbc_collected;       // Current Volume (ml)
   float current_rbcs_prod1;          // If so, keep separate books
   float current_rbcs_prod2;          // for RBC products 1 and 2
   float estimated_platelets;         // Yield target * 1E11
   float estimated_plasma;            // volume target (ml)
   float estimated_rbcs;              // volume target (ml)
};


#endif

/* FORMAT HASH bda8b2c6e2441634c955dbb7c0d62bd5 */

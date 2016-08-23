/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael J. Cobb

*     Class name:  N/A

*     File name:   guiproc.h

*     Contents:    All relevant structures, enums, #defines, and string #defines
                   for GUI screen <-> procedure interaction.
*     Description:

*************************< FOCUSSED GUI DOMAIN >*****************************/

// Single include define guardian
#ifndef GUIPROC_H
#define GUIPROC_H


// Alarm definitions
#include "alarms.h"

#ifndef WIN32 // For Vista's benefit, eliminate some dependencies.

// State definitions
#include "states.h"

#include "sounds.hpp"
#include "runinfo.h"        // run status info message struct
#include "adjust.h"         // adjustment screen message struct

#endif // #ifndef WIN32

////////////////////////////////////////////////////////////////////////////////
// Values for CurrentProcedureStatus struct to communicate current return pump
// and inlet pressure status
const int RETURN_PUMP_CYCLE_DRAW   = 0;
const int RETURN_PUMP_CYCLE_RETURN = 1;

// metric for sqeeze icon display MINIMUM_PRESSURE_SCALING_FACTOR * current_inlet_pressure_min
const double MINIMUM_PRESSURE_SCALING_FACTOR = 0.75;


////////////////////////////////////////////////////////////////////////////////
// enumerate int32 rinseback message invocations for GUI <-> proc rinseback
// status communication
enum RINSEBACK_STATUS
{
   REQUEST_RINSEBACK_STATUS,
   RINSEBACK_OK,
   RINSEBACK_NOT_OK
};


enum PREDICT_INVOKE_TYPE
{
   // Explicitly set the enum values so that a Predict screen, parameter string
   // that hasn't had one of these set won't be mistaken for the first enum
   // during "int" conversion within Predict.

   DONINFO_DELTA     = 11,
   DONINFO_NO_DELTA  = 22,
   ADJUST_DELTA      = 33,
   ADJUST_NO_DELTA   = 44,
   REPFLUID_NO_DELTA = 55,
   CPS_DISABLES_RBCS = 66,
   AUTO_FLOW_TIMEOUT = 77
};

DataLog_Stream& operator << (DataLog_Stream& os, const PREDICT_INVOKE_TYPE&);

// The two outcomes of a Replacement Fluid prompt screen: either RF is connected or it isn't.
enum RF_CONNECT_STATUS
{
   RF_CONNECTED,
   RF_NOT_CONNECTED
};

// used in CassettePositionStatus message
enum CASSETTE_POSITION_STATUS
{
   CASSETTE_LOWERING,
   CASSETTE_LOWERED,
   CASSETTE_EVACUATING,
   CASSETTE_RAISING,
   CASSETTE_RAISED
};

// The SYSDONOR screen states
enum SYSDONOR_STATE
{
   SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE, // reset state
   SETUP_COMPLETE_DONOR_NOTCOMPLETE,    // disposable setup complete, haven't entered donor data yet.
   SETUP_NOTCOMPLETE_DONOR_COMPLETE,    // donor data entered, disposable setup must be completed
   SETUP_COMPLETE_DONOR_COMPLETE,       // both donor data and disposable setup are complete
                                        // (redo donor data, or connect donor).
   SYSDONOR_INVALID_STATE
} ;

// the set type and therefore the procedure type
// these are sent from PROC via the MSSBeginGuiMsg
enum MSS_RUN_TYPE
{
   NON_MSS  = 0,               // not metered storage set...
   MSS_RBC  = 1,               // metered storage for RBC only
   MSS_PLT  = 2,               // metered storage for PLT only
   MSS_BOTH = 3,               // metered storage for PLT and RBC
};

enum AAS_PFR_SCREEN
{
   AAS_PFR_CLAMPCHANNEL,
   AAS_PFR_CHANNELTEST,
   AAS_PFR_PASCONNECT,
   AAS_PFR_RASCONNECT,
   AAS_PFR_REDIRECT_PAS,
   AAS_PFR_PRIME,
   AAS_PFR_ADDITION,
   AAS_PFR_UNLOAD
};

enum PARTIAL_END_RUN_TYPE
{
   NORMAL_END_RUN,
   END_RAS_ADDITION,
   END_PAS_ADDITION
};

enum  END_MSS_MESSAGE_SENDER
{
   END_MSS_MESSAGE_SENDER_PAS,
   END_MSS_MESSAGE_SENDER_RAS,
};

struct GUI_COBE_CONFIG_STRUCT
{
   float pltWbcThresholdConc;
   float PtfFilterLoss;
   float ptfFilterVol;

   float rasBagEmptyPercent;
   float pasBagEmptyPercent;

   float hctToHgbConvFactor;

   float flowRateMin;
   float flowRateMax;

   float pasPrimeVol;
   float pasVolBiasPercent;
   float pasNoRinsebackPenalty;

   float minPlsRinseVol;
};


struct BUTTON_PRESS_MESSAGE
{
   int         SCREEN_ID;
   const char* SCREEN_NAME;
   int         BUTTON_ID;
   const char* BUTTON_NAME;
};

struct CASSETTE_MESSAGE
{
   unsigned int functBits;
   int          ref;
};

//
// RBC product enumerations ...
//   ( anything outside the range is out of our recommended storage range )
//
const float MinRBCProductDose  = 150.0f;
const float MaxRBCProductDose  = 250.0f;
const float MinDRBCProductDose = 2 * MinRBCProductDose;
const float MaxDRBCProductDose = 2 * MaxRBCProductDose;

#endif // include guard

/* FORMAT HASH bb7b5ff17966f33d60d4412a0be95543 */

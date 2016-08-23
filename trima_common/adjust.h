/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      adjust.h
 *             Definitions for adjustment messages.
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/

#ifndef INCLUDE_ADJUST
#define INCLUDE_ADJUST

#include "trima_datalog.h"
#include "message_payload.h"

// int32 message codes from GUI in response
// to operator action at the adjust screen.
// Mid = ProcedureAdjustment
enum troubleshooting_adjustments
{
   ACCESS_PRESSURE_UP,
   ACCESS_PRESSURE_DOWN,
   RETURN_PRESSURE_UP,
   RETURN_PRESSURE_DOWN,
   TINGLING_UP,
   TINGLING_DOWN,
   CLUMPING_UP,
   CLUMPING_DOWN,
   SPILLOVER,
   AIR_IN_PLASMA_LINE,
   SALINE_BOLUS,

   END_OPERATOR_INITIATED_ADJUSTMENTS = SALINE_BOLUS,   // Please put any new operator initiated adjustments above here

   ADJUSTMENT_SCREEN_CLOSED, // Exited screen.

   // Automatic adjustmeents (not sent on to Vista)

   // AUTOFLOW CHANGES
   AUTOFLOW_QIN_INCREASE,
   AUTOFLOW_QIN_DECREASE,

   ADJUSTMENT_MAX_ENUM_VALUE // this must always be the last and largest value!  Used for payload sizing
};

template<class Stream>
Stream& operator << (Stream& os, troubleshooting_adjustments adjustment)
{

   switch (adjustment)
   {
      case AUTOFLOW_QIN_INCREASE     : return os << "AUTOFLOW_QIN_INCREASE"                    ;
      case AUTOFLOW_QIN_DECREASE     : return os << "AUTOFLOW_QIN_DECREASE"                    ;
      case ACCESS_PRESSURE_UP        : return os << "ACCESS_PRESSURE_UP"                       ;
      case ACCESS_PRESSURE_DOWN      : return os << "ACCESS_PRESSURE_DOWN"                     ;
      case RETURN_PRESSURE_UP        : return os << "RETURN_PRESSURE_UP"                       ;
      case RETURN_PRESSURE_DOWN      : return os << "RETURN_PRESSURE_DOWN"                     ;
      case TINGLING_UP               : return os << "TINGLING_UP"                              ;
      case TINGLING_DOWN             : return os << "TINGLING_DOWN"                            ;
      case CLUMPING_UP               : return os << "CLUMPING_UP"                              ;
      case CLUMPING_DOWN             : return os << "CLUMPING_DOWN"                            ;
      case SPILLOVER                 : return os << "SPILLOVER"                                ;
      case AIR_IN_PLASMA_LINE        : return os << "AIR_IN_PLASMA_LINE"                       ;
      case SALINE_BOLUS              : return os << "SALINE_BOLUS"                             ;
      case ADJUSTMENT_SCREEN_CLOSED  : return os << "ADJUSTMENT_SCREEN_CLOSED"                 ;
      case ADJUSTMENT_MAX_ENUM_VALUE : return os << "ADJUSTMENT VALUE that should not be used" ;
   }

   return os << "UNKNOWN(" << (int)adjustment << ")";
}

typedef Payload < troubleshooting_adjustments, ADJUSTMENT_MAX_ENUM_VALUE > AdjustmentPayload;


// Payloads for ProcedureAdjustmentStatusMsgs.
namespace procedure_adjustment_status
{

enum type
{
   ADJUSTMENT_ACK,    // response to adjustment (i.e. [ACCESS_PRESSURE_UP, CLUMPING_DOWN] )
   ADJUSTMENT_UPDATE, // Sent by AdjustCtrl periodically, or when something changes
   MAX_TYPE_VALUE     // this must always be the largest value in the enum
};

template<class Stream>
Stream& operator << (Stream& os, type payload)
{
   switch (payload)
   {
      case ADJUSTMENT_ACK    : return os << "ADJUSTMENT_ACK"    ;
      case ADJUSTMENT_UPDATE : return os << "ADJUSTMENT_UPDATE" ;
      case MAX_TYPE_VALUE    : return os << "ADJUSTMENT Value that should not be used";
   }

   return os << "UNKNOWN PROCEDURE ADJUSTMENT STATUS PAYLOAD: " << (int)payload ;
}

typedef Payload< type, MAX_TYPE_VALUE > AdjustmentStatusPayload;

} // namespace procedure_adjustment_status


// Info for a single adjustment scale for
// the adjust screen
struct ADJUSTMENT_STRUCTURE
{
   float maximum_scale;            // value top of scale
   float minimum_scale;            // value bottom of scale
   float current_cap;              // Position of Bar showing our new cap
   float current_value;            // actual present value
   float current_maximum;          // max value if not limited
   int   active;                   // true if meter is active
};


// Adjust screen GUI buttons
enum BUTTON_VISIBILITY_STATUS
{
   BUTTON_INVISIBLE,
   BUTTON_GREY,
   BUTTON_VISIBLE
};

// Adjustment Info struct contained in AdjustMsg object
// Message Id: ProcedureToGUIAdjust
//
struct ADJUST_SCREEN_CONFIG
{
   BUTTON_VISIBILITY_STATUS spillover_button_state;
   BUTTON_VISIBILITY_STATUS air_block_button_state;
   BUTTON_VISIBILITY_STATUS saline_bolus_button_state;
   ADJUSTMENT_STRUCTURE     access_pressure_adjust;
   ADJUSTMENT_STRUCTURE     return_pressure_adjust;
   ADJUSTMENT_STRUCTURE     tingling_adjust;
   ADJUSTMENT_STRUCTURE     clumping_adjust;
};



//
// Dlog output functions for the predict message structs and enums.
//
DataLog_Stream& operator << (DataLog_Stream& os,  const BUTTON_VISIBILITY_STATUS& value);

#endif

/* FORMAT HASH e052f9361da3b60afcfdcc7cdaa2cfec */

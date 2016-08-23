/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      haldata.h
 *             Local status data maintained on HW Status updates.
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/


#if !defined( INCLUDE_HALDATA)
#define INCLUDE_HALDATA

#include "haldefs.h"
#include "cassette.h"

///////////////////////////////////////////////////////////////////
// Orders from the outside to HAL


// Pump flows in ml/min
typedef struct
{
   float Inlet;
   float Collect;
   float Ac;
   float Plasma;
   float Return;
} PumpFlows;

typedef struct
{
   HAL_VALVESTATE rbc;
   HAL_VALVESTATE plasma;
   HAL_VALVESTATE collect;
} ValveStates;

typedef struct
{
   float RPM;       // commanded RPM
   float RampUp;    // RPM rate to ramp above 1000RPM per sec.
   float RampDn;    // RPM rate to ramp down above 1000RPM per sec.
} CmdCentrifuge;

typedef struct
{

// Cassette information
   HAL_CASSETTEORDERS     cassettePosition;
   cassette::cassetteType cassetteId;

// centrifuge
   CmdCentrifuge CmdCent;

// misc
   HAL_ALARMLIGHTORDERS alarmLight;
   HAL_LEAKTESTORDERS   leakTest;
   short                soundLevel; // Alarm sound level.

// door command
   HAL_DOORORDERS doorCmd;          // Open or close door solenoid

// pump flow orders in milliliters/minute
   PumpFlows CmdFlow;

// valve commands
   ValveStates CmdValve;

// RBC detector drive values
   unsigned char redDrive;
   unsigned char greenDrive;

// Raw alarm limits for driver
   short APSHighLimit;      // in mmHg
   short APSLowLimit;       // in mmHg

   HAL_PWR_SPLY_OUTPUT_STATUS powerSupplyEnabled;      // Turn off switched power (Artesyn PS only)
} HalOrders;

#endif

/* FORMAT HASH 1ce5c0c0636a0a8fccd3aa957b4b9720 */

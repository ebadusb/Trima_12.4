/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 *  cal.dat access class
 *
 * $Header: //depot/main/embedded/Trima/Main/trima_common_reentrant/caldat.cpp#1 $
 * $Log: caldat.cpp $
 * Revision 1.1  2002/10/11 16:42:22Z  jl11312
 * Initial revision
 *
 */

#include <vxworks.h>
#include "caldat.h"
#include "datfile.h"
#include "filenames.h"

//
//  My singleton data instance
//
CalDat::CalDatStruct CalDat::_data;

//
// -----------------------------------------------------------------------------
//                      Constructor, destructor
//
//
CalDat::CalDat()
{}

CalDat::~CalDat()
{}

//
// -----------------------------------------------------------------------------
//
//   Return a constant reference to my data
//
const CalDat::CalDatStruct& CalDat::data ()
{
   return _data;
}

//
// -----------------------------------------------------------------------------
//
ALARM_VALUES CalDat::initialize (bool logError)
{
   ALARM_VALUES status = NULL_ALARM; // default

   //
   //
   // Create the dat file reader to retrieve the Cobe configuration data.
   //
   CDatFileReader config;

   if (config.Initialize(PNAME_CALDAT, logError) != DFR_OK)
   {
      return CAL_DAT_BAD_FILE;
   }

   _data.APSRawPositive    = config.get_float("APS", "aps_raw_pos", -2000.0f, 5000.0f);
   _data.APSmmHgPositive   = config.get_float("APS", "aps_mmHg_pos", -500.0f, 500.0f);
   _data.APSRawReference   = config.get_float("APS", "aps_raw_ref", -2000.0f, 5000.0f);
   _data.APSmmHgReference  = config.get_float("APS", "aps_mmHg_ref", -500.0f, 500.0f);
   _data.APSRawNegative    = config.get_float("APS", "aps_raw_neg", -2000.0f, 5000.0f);
   _data.APSmmHgNegative   = config.get_float("APS", "aps_mmHg_neg", -500.0f, 500.0f);

   _data.CPSOffset         =  config.get_float("CPS", "cps_offset", -1000.0f, -100.0f);
   _data.CPSGain           =  config.get_float("CPS", "cps_gain", 0.25f, 1.5f);

   _data.RBCInitRedDrive   = config.get_float("RBC_DETECTOR", "init_red_drive", 0.0f, 255.0f);
   _data.RBCInitGreenDrive = config.get_float("RBC_DETECTOR", "init_green_drive", 0.0f, 255.0f);

   CDatFileReader touchscreen;

   if (touchscreen.Initialize(PNAME_TCHSCRNDAT, logError) != DFR_OK)
   {
      return CAL_DAT_BAD_FILE;
   }

   _data.touchScreenCalA = touchscreen.get_float("TOUCHSCREEN", "a", -1000.0f, 1000.0f);
   _data.touchScreenCalB = touchscreen.get_float("TOUCHSCREEN", "b", -1000.0f, 1000.0f);
   _data.touchScreenCalC = touchscreen.get_float("TOUCHSCREEN", "c", -1000.0f, 1000.0f);
   _data.touchScreenCalD = touchscreen.get_float("TOUCHSCREEN", "d", -1000.0f, 1000.0f);
   _data.touchScreenCalE = touchscreen.get_float("TOUCHSCREEN", "e", -1000.0f, 1000.0f);
   _data.touchScreenCalF = touchscreen.get_float("TOUCHSCREEN", "f", -1000.0f, 1000.0f);

   //
   //
   //  Final status check to ensure that all values were read in properly.
   //
   ALARM_VALUES configStatus  = convertAlarm(config.Error() );
   ALARM_VALUES tchscrnStatus = convertAlarm(touchscreen.Error() );

   if (configStatus != NULL_ALARM)
   {
      return configStatus;
   }
   else if (tchscrnStatus != NULL_ALARM)
   {
      return tchscrnStatus;
   }
   else
      return NULL_ALARM;
}

ALARM_VALUES CalDat::convertAlarm (DFR_ERROR configStatus)
{
   ALARM_VALUES status = NULL_ALARM;
   if (configStatus == DFR_NOTFOUND)
   {
      status = CAL_DAT_VALUE_NOT_FOUND;
   }
   else if (configStatus == DFR_VALUE)
   {
      status = CAL_DAT_VALUE_OUT_OF_RANGE;
   }
   else if (configStatus == DFR_PARSE)
   {
      status = CAL_DAT_PARSE_ERROR;
   }

   return status;
}

/* FORMAT HASH caa9864d9103106585bc4dbd11a7d3d0 */

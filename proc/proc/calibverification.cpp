/*
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      calibverification.cpp
 *
 */

#include "an_alarm.h"
#include "caldat.h"
#include "caldat_default.h"
#include "calibverification.h"
#include <math.h>

DEFINE_STATE(CalibVerification) CalibVerification::CalibVerification()
   : StateAbs(),
     _validAPSCal(0),
     _validCPSCal(0),
     _validRBCCal(0)
{}

CalibVerification::~CalibVerification()
{}

int CalibVerification::transitionStatus ()
{
   if ( _validAPSCal && _validCPSCal && _validRBCCal )
   {
      return 1;
   }

   return NO_TRANSITION;
}

int CalibVerification::preProcess ()
{
   return NORMAL;
}

int CalibVerification::postProcess ()
{
   return NORMAL;
}

int CalibVerification::preEnter ()
{
   if ( compareFloatEqual(CalDat::data().APSRawNegative, CAL_DAT_DEFAULT_APS_RAW_NEG) &&
        compareFloatEqual(CalDat::data().APSRawReference, CAL_DAT_DEFAULT_APS_RAW_REF) &&
        compareFloatEqual(CalDat::data().APSRawPositive, CAL_DAT_DEFAULT_APS_RAW_POS) )
   {
      anAlarmMsg noAPSCal(CAL_DAT_NO_APS_CAL);
      noAPSCal.setAlarm();
   }
   else
   {
      _validAPSCal = 1;
   }

   if ( compareFloatEqual(CalDat::data().CPSOffset, CAL_DAT_DEFAULT_CPS_OFFSET) &&
        compareFloatEqual(CalDat::data().CPSGain, CAL_DAT_DEFAULT_CPS_GAIN) )
   {
      anAlarmMsg noCPSCal(CAL_DAT_NO_CPS_CAL);
      noCPSCal.setAlarm();
   }
   else
   {
      _validCPSCal = 1;
   }

   if ( compareFloatEqual(CalDat::data().RBCInitRedDrive, CAL_DAT_DEFAULT_RBC_INIT_RED_DRIVE) &&
        compareFloatEqual(CalDat::data().RBCInitGreenDrive, CAL_DAT_DEFAULT_RBC_INIT_GREEN_DRIVE) )
   {
      anAlarmMsg noRBCCal(CAL_DAT_NO_RBC_CAL);
      noRBCCal.setAlarm();
   }
   else
   {
      _validRBCCal = 1;
   }

   return NORMAL;
}

void CalibVerification::reset ()
{
   _validAPSCal = 0;
   _validCPSCal = 0;
   _validRBCCal = 0;
}

int CalibVerification::compareFloatEqual (double val1, double val2)
{
   //
   // Assuming val2 is a comparison constant, specified to 7 digits of precision,
   // it should match (worst case) to 1 part in 1.0e6.  A factor of 1.5 is used
   // below to insure we don't have problems with rounding.
   //
   return ( fabs(val1 - val2) / val2 < 1.5e-6 );
}

/* FORMAT HASH ba0b38a3a9b89a35783c1939cfa1e3ef */

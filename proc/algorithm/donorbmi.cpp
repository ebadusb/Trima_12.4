/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      donorbmi.cpp
 *
 */

#include <vxworks.h>
#include "donorbmi.h"
#include "procdata.h"

void DonorBMI::calculate ()
{
   ProcData pd;

   float    height = pd.Donor().Height.Get();
   float    weight = pd.Donor().Weight.Get();

   //
   // check for height in inches, convert to meters
   //
   if ( pd.Config().LangUnit.Get().key_height == HEIGHT_UNITS_IN )
   {
      height /= 39.370079;
   }

   //
   // check for height in cm, convert to meters
   //
   if ( pd.Config().LangUnit.Get().key_height == HEIGHT_UNITS_CM )
   {
      height /= 100.0;
   }

   //
   // check for weight in lbs, convert to kg
   //
   if ( pd.Config().LangUnit.Get().key_weight == WEIGHT_UNITS_LB )
   {
      weight /= 2.2046226;
   }

   pd.MakeDonorWritable();
   pd.Donor().BMI.Set(weight / ( height * height ) );
}

/* FORMAT HASH 6adb161012ea686ab04cde5c0630ab8e */

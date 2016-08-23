/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      rtplateletyield.h
 *
 * ABSTRACT:   This class calculates the real-time platelet yield
 *
 */


#ifndef _RT_PLATELET_YIELD_H_  //  Prevents multiple inclusions.
#define _RT_PLATELET_YIELD_H_

#include "donor.h"
#include "haldefs.h"
#include "states.h"
#include "procvolumes_cds.h"

class RTPlateletYield
{
public:

   //
   // Function which calculates the yield from the given
   //  parameters.
   //
   static const float calculateYield (const float Qch);

   //
   // Determine which config variable to use based on donor
   //  BMI...
   //
   static float MNCPrecountForPurge ();

};

#endif

/* FORMAT HASH fa47bba0d65768ee6d1d027c20251d2d */

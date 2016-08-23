/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      posthct.cpp
 *             Focussed system posthct functions.
 *
 * AUTHOR:     R.E. Edwards
 *
 * ABSTRACT:   This file encapsulates the posthct algorithm.
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 * EXAMPLE:
 *
 */

#include <vxworks.h>
#include "posthct.hpp"
#include "error.h"

// ----------------------------------------------------------------------------
// SPECIFICATION:    PostHct::PostHct
//                   Parameters: None
//                   Returns: None
//
//                   Description: Constructor for the PostHct class.
//
//                   Error Handling: None
//
PostHct::PostHct()
{}
// ----------------------------------------------------------------------------
// SPECIFICATION:    PostHct::~PostHct
//                   Parameters: None
//                   Returns: None
//                   Description: Destructor for PostHct class.
//                   Error Handling: None
//
PostHct::~PostHct()
{}


//
// ----------------------------------------------------------------------------
// SPECIFICATION:    PostHct::postHctOK
//                   Parameters:varies
//                   Returns: int
//                   Description: This method determines if the current
//                                post hematocrit, as determined by the
//                                parameters and/or my member data, will
//                                exceed the configuration limit. If the limit
//                                is exceeded the method passes back l0,
//                                otherwise l1.
//                   Error Handling: None
//

const int PostHct::postHctOK (const float Hct,
                              const float TBV,
                              const float PackedRBCsInBag,
                              const float ResidualRBCVolume,
                              const float SampleVolume,
                              const float MinPostHct)
{

   float chct      = postHct(Hct, TBV, PackedRBCsInBag,
                             ResidualRBCVolume, SampleVolume);
   int   controlok =  (chct > MinPostHct);

   //
   //
   //  This condition is to account for corner cases where safety would shut
   //  us down for "too low of an instantaneous hct" (see IT5995)
   //
   static const float ABSOLUTE_MIN_DONOR_POSTHCT = 0.285f;
   float              shct                       = (((TBV * Hct) - (PackedRBCsInBag +
                                                                    (ResidualRBCVolume + 30.0f) + (SampleVolume * Hct))) / TBV);
   int                safetyok                   = (shct > ABSOLUTE_MIN_DONOR_POSTHCT);

   return (controlok && safetyok);
}
// ----------------------------------------------------------------------------
// SPECIFICATION:    PostHct::postHct
//                   Parameters:varies
//                   Returns: float
//                   Description: This method will return the current
//                                donor hematocrit using the parameters given
//                                and/or my member data.
//                   Error Handling: None
//

const float PostHct::postHct (const float Hct,
                              const float TBV,
                              const float PackedRBCsInBag,
                              const float ResidualRBCVolume,
                              const float SampleVolume)
{
   if (TBV == 0.0f)
   {
#ifdef __TRIMA__

      _FATAL_ERROR(__FILE__, __LINE__, TRACE_PROC, 0,
                   "Divide by zero protection.");
#else
      return 0.00;
#endif
   }
   // Equation comes from Donor Post Hct calculation memo dated
   // 4/30/96, Muriel Keller.
   return (((TBV * Hct) - (PackedRBCsInBag +
                           ResidualRBCVolume + (SampleVolume * Hct))) / TBV);

}

/* FORMAT HASH 7caa66f3002528eccf80079e88a3cd38 */

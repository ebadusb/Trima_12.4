/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      posthct.hpp
 *             Focussed system posthct functions.
 *
 * AUTHOR:     R.E. Edwards
 *
 * ABSTRACT:   This file encapsulates the posthct algorithm
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 * EXAMPLE:
 *
 */
#ifndef POSTHCT_HPP
#define POSTHCT_HPP

class PostHct
{

   //
   //
   //   The following methods are available for general use.
   //
public:
   // Constructor
   PostHct();

   // Destructor
   virtual ~PostHct();

   //
   //
   //  For the following methods, if you do not specify the values then I
   //  use the latest values from my constructor or the last call to the
   //  initialize() method.  New parameters replace my current data for
   //  subsequent calls.
   //
   virtual const int postHctOK (const float Hct,
                                const float TBV,
                                const float PackedRBCsInBag,
                                const float ResidualRBCVolume,
                                const float SampleVolume,
                                const float MinPostHct);

   virtual const float postHct (const float Hct,
                                const float TBV,
                                const float PackedRBCsInBag,
                                const float ResidualRBCVolume,
                                const float SampleVolume);


// -----------------------------------------------------------------------------
//                     Member data
//
protected:
};

#endif

/* FORMAT HASH 5489e191940dacd2fc0c47cc70499edb */

/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      postcount.cpp
 *             Focussed system postcount functions.
 *
 * AUTHOR:     R.E. Edwards
 *
 * ABSTRACT:   This file encapsulates the postcount algorithm.
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 * EXAMPLE:
 *
 */

#include <vxworks.h>
#include "postcount.hpp"
#include "error.h"
#include "cobeconfig.h"
#include "procdata.h"
#include "software_cds.h"

// Initialize static
Config_CDS* Postcount::_pConfig_CDS = 0;

// ----------------------------------------------------------------------------
// SPECIFICATION:    Postcount::Postcount
//                   Parameters: None
//                   Returns: None
//
//                   Description: Constructor for the Postcount class.
//
//                   Error Handling: None
//
Postcount::Postcount(const float PltPre,
                     const float TBV,
                     const float PltYield,
                     const float MinPostcount)
   : _PltPre(PltPre),
     _TBV(TBV),
     _PltYield(PltYield),
     _MinPostcount(MinPostcount)
{
   if (!_pConfig_CDS)
   {
      _pConfig_CDS = new Config_CDS(ROLE_RO);
   }
}

Postcount::Postcount()
   : _PltPre(0.0f),
     _TBV(0.0f),
     _PltYield(0.0f),
     _MinPostcount(0.0f)
{
   if (!_pConfig_CDS)
   {
      _pConfig_CDS = new Config_CDS(ROLE_RO);
   }
}
// ----------------------------------------------------------------------------
// SPECIFICATION:    Postcount::~Postcount
//                   Parameters: None
//                   Returns: None
//                   Description: Destructor for Postcount class.
//                   Error Handling: None
//
Postcount::~Postcount()
{}


// ----------------------------------------------------------------------------
// SPECIFICATION:    Postcount::initialize
//                   Parameters: bunches
//                   Returns: None
//                   Description: reinitialization of member data
//
//                   Error Handling: None
//
void Postcount::initialize (const float PltPre,
                            const float TBV,
                            const float PltYield,
                            const float MinPostcount)
{
   _PltPre       = PltPre;
   _TBV          = TBV;
   _PltYield     = PltYield;
   _MinPostcount = MinPostcount;
}


//
// ----------------------------------------------------------------------------
// SPECIFICATION:    Postcount::PostcountOK
//                   Parameters:varies
//                   Returns: int
//                   Description: This method determines if the current
//                                postcount, as determined by the parameters
//                                and/or my member data, will exceed the
//                                configuration postcount limit. If the limit
//                                is exceeded the method passes back l0,
//                                otherwise l1.
//                   Error Handling: None
//
const int Postcount::postcountOK () const
{
   return (postcount() > _MinPostcount);
}


const int Postcount::postcountOK (const float PltYield)
{
   _PltYield = PltYield;
   return postcountOK();
}

const int Postcount::postcountOK (const float PltPre,
                                  const float TBV,
                                  const float PltYield,
                                  const float MinPostcount)
{
   _PltPre       = PltPre;
   _TBV          = TBV;
   _PltYield     = PltYield;
   _MinPostcount = MinPostcount;
   return postcountOK();
}
// ----------------------------------------------------------------------------
// SPECIFICATION:    Postcount::postcount
//                   Parameters:varies
//                   Returns: float
//                   Description: This method will return the postcount given
//                                the parameters given and my member data.
//                   Error Handling: None
//
const float Postcount::postcount () const
{
   if (_TBV == 0.0f)
   {
// ********************************************************
#ifdef __TRIMA__

      _FATAL_ERROR(__FILE__, __LINE__, TRACE_PROC, 0,
                   "Divide by zero protection.");
#else
      return 0.0;
#endif
// ******************************************************
   }
   if (Software_CDS::GetInstance().getFeature(PostcountPlateletMobilizationAlgorithm))
      return (_PltPre - CobeConfig::data().PlateletMobilizationFactor * (_PltYield / (_TBV * 1E6f)));
   return (_PltPre - (_PltYield / (_TBV * 1E6f)));
}

const float Postcount::postcount (const float PltYield)
{
   _PltYield = PltYield;
   return postcount();
}

const float Postcount::postcount (const float PltPre,
                                  const float TBV,
                                  const float PltYield)
{
   _PltPre   = PltPre;
   _TBV      = TBV;
   _PltYield = PltYield;
   return postcount();
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// copy constructor and op=
Postcount::Postcount(const Postcount& orig)
   : _PltPre(orig._PltPre),
     _TBV(orig._TBV),
     _PltYield(orig._PltYield),
     _MinPostcount(orig._MinPostcount)
{}

Postcount& Postcount::operator = (const Postcount& orig)
{
   if (this != &orig)
   {
      _PltPre       = orig._PltPre;
      _TBV          = orig._TBV;
      _PltYield     = orig._PltYield;
      _MinPostcount = orig._MinPostcount;
   }
   return *this;
}

/* FORMAT HASH a67353ed91db085108ce6b691581fe27 */

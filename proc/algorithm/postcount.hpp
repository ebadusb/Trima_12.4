/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      postcount.hpp
 *             Focussed system postcount functions.
 *
 * AUTHOR:     R.E. Edwards
 *
 * ABSTRACT:   This file encapsulates the postcount algorithm
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 * EXAMPLE:
 *
 */
#ifndef POSTCOUNT_HPP
#define POSTCOUNT_HPP
#include "config_cds.h"

class Postcount
{

   //
   //
   //   The following methods are available for general use.
   //
public:
   // Constructors.  If you use the default constructor then you MUST
   // initialize me before using me or use one of the methods which
   // accepts all of the parameters as member data.
   Postcount();
   Postcount(const float PltPre,
             const float TBV,
             const float PltYield,
             const float MinPostcount);

   // Destructor
   virtual ~Postcount();

   //
   //
   //  You can reinitialize me with new values if desired.
   //
   virtual void initialize (const float PltPre,
                            const float TBV,
                            const float PltYield,
                            const float MinPostcount);

   //
   //
   //  For the following methods, if you do not specify the values then I
   //  use the latest values from my constructor or the last call to the
   //  initialize() method.  New parameters replace my current data for
   //  subsequent calls.
   //
   virtual const int postcountOK () const;
   virtual const int postcountOK (const float PltYield);
   virtual const int postcountOK (const float PltPre,
                                  const float TBV,
                                  const float PltYield,
                                  const float MinPostcount);

   virtual const float postcount () const;
   virtual const float postcount (const float PltYield);
   virtual const float postcount (const float PltPre,
                                  const float TBV,
                                  const float PltYield);


   // copy constructor and op= are valid for this class.
   Postcount(const Postcount&);
   Postcount& operator = (const Postcount&);

// -----------------------------------------------------------------------------
//                     Member data
//
protected:
   float              _PltPre;
   float              _TBV;
   float              _PltYield;
   float              _MinPostcount;
   static Config_CDS* _pConfig_CDS;
};

#endif

/* FORMAT HASH 6fd1dfd712081c21f9179061b8da5bcf */

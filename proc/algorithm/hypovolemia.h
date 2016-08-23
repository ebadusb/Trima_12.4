/*
 * Copyright (c) 2000  Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      hypovolemia.hpp
 *             Focussed system hypovolemia functions.
 *
 * AUTHOR:     R.E. Edwards
 *
 * ABSTRACT:   This file encapsulates the hypovolemia algorithm
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 * EXAMPLE:
 *
 */
#ifndef HYPOVOLEMIA_HPP
#define HYPOVOLEMIA_HPP

// **************************

#include <stddef.h>
#include "cassette.h"
#include "states.h"
#include "config_cds.h"
#include "procdonor_cds.h"

// **************************

class Hypovolemia
{
public:
   // Constructors.  You MUST initialize me before using me.
   Hypovolemia();

   // Destructor
   virtual ~Hypovolemia();

   //
   //  You must reinitialize me with new values before use
   //
   virtual void initialize (const cassette::cassetteType currentcassette);

   //
   //
   //   In general this variant should be used.  It returns the smaller of the control remaining
   //   or safety remaining.  If the caller needs either control or safety specifically they
   //   they can use the variants below.
   //
   virtual const float VolumeRemainingToHypovolemia (const int thisIsTheCurrentProcedure,
                                                     const float overdrawVolume,
                                                     const float Vcol, const float Vac_col,
                                                     const float Vp,   const float Vac_p,
                                                     const float Vrbc, const float Vac_rbc,
                                                     const float Vrep, const float Vac) const;

   virtual const float ControlVolumeRemaining (const int thisIsTheCurrentProcedure,
                                               const float overdrawVolume,
                                               const float Vcol, const float Vac_col,
                                               const float Vp,   const float Vac_p,
                                               const float Vrbc, const float Vac_rbc,
                                               const float Vrep, const float Vac) const;

   virtual const float SafetyVolumeRemaining (const int thisIsTheCurrentProcedure,
                                              const float overdrawVolume,
                                              const float Vcol, const float Vac_col,
                                              const float Vp,   const float Vac_p,
                                              const float Vrbc, const float Vac_rbc,
                                              const float Vrep, const float Vac) const;
   //
   //  Internal logging
   //
   virtual void log () const;


   // copy constructor and op= are valid for this class.
   Hypovolemia(const Hypovolemia&);
   Hypovolemia& operator = (const Hypovolemia&);

   virtual const float ControlLimit (const float Vcol, const float Vp) const;
   virtual const float SafetyLimit () const;

// -----------------------------------------------------------------------------
//                     Member data
//
protected:
   int                    _Initialized;
   int                    _RemovalSetting;
   int                    _WeightUnits;
   float                  _MaxPercentRemoved;
   float                  _TBV;       // either Ogawa or Nadler, as configured
   float                  _TBVNadler; // always Nadler
   float                  _DonorWeight;
   float                  _RemoveVolumeWeight;
   float                  _RemoveVolumeGreaterThan;
   float                  _RemoveVolumeLessThan;
   float                  _RemoveVolumePlatelet;
   float                  _RemoveVolumePlasma;

   float                  _MlPerKg;
   cassette::cassetteType _Cassette;
   Config_CDS             _Config_CDS;
   ProcDonor_CDS          _ProcDonor_CDS;
};

#endif

/* FORMAT HASH 8d1d551fa8d8f58a79f58f6e1c0fae5d */

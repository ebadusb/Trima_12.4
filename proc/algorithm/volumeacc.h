/*******************************************************************************
 *
 * Copyright (c) 2002 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      VolumeAcc.h
 *             Accumulates Volumes for products and pumps
 *
 *
 ******************************************************************************/

#ifndef _VOLUMEACC_H_
#define _VOLUMEACC_H_

#include <vxWorks.h>
#include "procdata.h"

class VolumeAcc
{
public:

   static void Clear (void);   // clear all pump and bag volumes
   static void ClearVac (void);
   static void ClearVacTotal (void);
   static void ClearVin (void);
   static void ClearVinTotal (void);
   static void ClearReturn (void);
   static void ClearPlateletBag (void);

   static void UpdateNeedleAC (const ProcData::UpdateModeType mode);

protected:

   static void Accumulate (const ProcData::UpdateModeType md);

   //
   // Discount the inlet pump for recycled blood
   static float DiscountForRecycled ();

};

class RBCPlsVolumeAcc
   : public VolumeAcc
{
public:

   static void Accumulate (const ProcData::UpdateModeType md);

};

class PltPlsRBCVolumeAcc
   : public VolumeAcc
{
public:

   static void Accumulate (const ProcData::UpdateModeType md);
   static void CalculateYield ();

};

#endif

/* FORMAT HASH f1d87256d1150d08cc8409e0df481a2b */

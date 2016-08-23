/*******************************************************************************
 *
 * Copyright (c) 2002 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      cycleacc.h
 *             Accumulates set specific volumes for a given cycle.  The
 *              reservoir object uses this data.
 *
 *
 ******************************************************************************/


#ifndef _CYCLES_H_
#define _CYCLES_H_

#include <vxWorks.h>
#include "procdata.h"

class CycleAcc
{
public:

   // Use to clear volumes on a cycle switch
   static void ClearAll ();

   // Save current reservoir volume to last and clear current
   static void ClearFillVolume ();

protected:

   //
   // Accumulate the volumes per this cycle
   static void Accumulate (const ProcData::UpdateModeType md);

   //
   // Update the estimates for hct and ratio for this cycle
   static void UpdateEstimates ();

   // Calculate the HRBC value for this cycle
   static float CalculateHRBC (float ratio, float inletdv, float acdv);

public:
   // volume in excess of this counted toward hypovolemia
   static const float OVERDRAW_THRESHOLD_VOLUME = 72.0f;
};


class RBCPlsCycleAcc
   : public CycleAcc
{
public:

   static void Accumulate (const ProcData::UpdateModeType md);

};

class PltPlsRBCCycleAcc
   : public CycleAcc
{
public:

   static void  Accumulate (const ProcData::UpdateModeType md);
   static float CalculateHRBC (float ratio, float inletdv, float acdv);

};


#endif

/* FORMAT HASH 479126d4d05cf6b7d14f8766167ba811 */

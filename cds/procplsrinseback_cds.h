/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      procplsrinseback_cds.h
 *
 */

#ifndef __PROCPLSRINSEBACK_CDS
#define __PROCPLSRINSEBACK_CDS

#include "datastore.h"
#include "proctrimaset_cds.h"

#include <time.h>

#ifndef WIN32
#pragma interface
#endif

class ProcPlsRinseback_CDS
   : public SingleWriteDataStore
{
public:
   enum PlsRinsebackSubstateComplete
   {
      None       =0,
      Lower      =1,
      Spillover  =2,
      No1        =4,
      Rinse      =8,
      Recirculate=16,
      Reservoir  =32,
      Rinseback  =64,
      No2        =128,
      Purge      =256,
      Evacuate   =512,
      All        =1023
   };

   BaseElement< PlsRinsebackSubstateComplete > Status;
   BaseElement< float >                        VPlasmaTarget;
   BaseElement< float >                        Facb;
   BaseElement< float >                        Vtotal;
   BaseElement< float >                        Vac;
   BaseElement< float >                        IRMax;
   BaseElement< float >                        QReturnMax;
   BaseElement< float >                        Ratio;
   BaseElement< timespec >                     PlasmaValveOpenAccumulatedTime;
   BaseElement< timespec >                     PlasmaValveOpenLastSeenTime;

   // Determine what the Plasma Target is given plasma rinseback setting
   // and other factors
   float PlasmaRinsebackTarget (float RinsebackPlasmaTarget,
                                float RunTargetPlasmaTarget,
                                float CurrentVolume,
                                ProcTrimaSet_CDS::CassetteStateType cassetteState);

public:

   ProcPlsRinseback_CDS(Role role);
   virtual ~ProcPlsRinseback_CDS();

private:
   ProcPlsRinseback_CDS(); // Base Constructor not available
};

#endif

/* FORMAT HASH cc89eec2c8f6868c17ebcbea8f6b80b9 */

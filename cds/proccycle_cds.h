/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      proccycle_cds.h
 *
 */

#ifndef __PROCCYCLE_CDS_H_
#define __PROCCYCLE_CDS_H_

#include "datastore.h"

#ifndef WIN32
#pragma interface
#endif
class ProcCycle_CDS
   : public SingleWriteDataStore
{
public:

   BaseElement<float> Vin;
   BaseElement<float> Vac;
   BaseElement<float> Vret;
   BaseElement<float> Vpls;
   BaseElement<float> Vplt;
   BaseElement<float> Vrbc;
   BaseElement<float> VrbcRes;
   BaseElement<float> Vres;
   BaseElement<float> VresDraw;
   BaseElement<float> VresReturn;
   BaseElement<float> VresLast;
   BaseElement<float> VAccumOverdraw;
   BaseElement<float> ACRatio;
   BaseElement<float> Hrbc;
   BaseElement<float> Facb;
   BaseElement<float> ResHct;
   BaseElement<float> ResRBCs;

public:

   ProcCycle_CDS (Role role);
   virtual ~ProcCycle_CDS();

private:
   ProcCycle_CDS();  // Base Constructor not available
};

#endif

/* FORMAT HASH 99be80573990bbdea8a6e9f1c803350a */

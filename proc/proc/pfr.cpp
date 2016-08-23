/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pfr.cpp
 *
 */

#include <pfr.h>
#include <procdata.h>
#include <states.h>

DEFINE_STATE(PFR);

PFR::PFR()
   : StateAbs()
{}

PFR::PFR(const PFR& state)
   : StateAbs(state)
{
   copyOver(state);
}

PFR::~PFR()
{
   cleanup();
}

int PFR::preProcess ()
{
   return NORMAL;
}

int PFR::postProcess ()
{
   return NORMAL;
}

int PFR::preEnter ()
{
   ProcData pd;
   //
   // Initialize the PFR handler before performing any PFR specific code ...
   //
   pd.PFRHandler().Initialize();

   return NORMAL;
}

int PFR::postExit ()
{
   return NORMAL;
}

void PFR::cleanup ()
{}

/* FORMAT HASH c11a6d621a38f76f53de970fdfdabccc */

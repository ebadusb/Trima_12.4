/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssPLT.cpp
 *
 */

/*
$Header:  $
$Log:  $
*/

#include "mssPLT.h"
#include "states.h"
#include "procdata.h"

#include "cobeconfig.h"

DEFINE_STATE(MSSPLT);



MSSPLT::MSSPLT()
   : StateAbs()
{}

MSSPLT::MSSPLT(const MSSPLT& state)
   : StateAbs(state)
{}

MSSPLT::~MSSPLT()
{}

int MSSPLT::preProcess ()
{

   ProcData pd;

   return NORMAL;
}

int MSSPLT::postProcess ()
{
   ProcData pd;

   return NORMAL;
}


int MSSPLT::preEnter ()
{
   ProcData pd;

   return NORMAL;
}

int MSSPLT::postExit ()
{

   ProcData pd;



   return NORMAL;
}

void MSSPLT::copyOver (const MSSPLT&)
{}

void MSSPLT::cleanup ()
{}

/* FORMAT HASH aab2485f40e1c7e2a33515f9b608893a */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimePurge.cpp
 *
 */

#include "primepurge.h"

DEFINE_STATE(PrimePurge);

PrimePurge::PrimePurge()
   : StateAbs(),
     _VpAtStart(0.0f)
{}

PrimePurge::PrimePurge(const PrimePurge& state)
   : StateAbs(state)
{
   copyOver(state);
}

PrimePurge::~PrimePurge()
{
   cleanup();
}

int PrimePurge::transitionStatus ()
{
   //
   //
   //   This state is not applicable for an RBC/Plasma set
   //
   if (_TrimaSet->cData().nCassetteType != cassette::PLTPLSRBC) return 1;
   return _TransitionStatus;
}

int PrimePurge::preProcess ()
{

   _Orders->ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   _Orders->ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);
   _Orders->ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);

   //
   //
   //  Calculate max inlet flow as lesser of Cobe config or adjustment value
   //
   float qin          = _Adjustments->QinCap();
   if (qin > CobeConfig::data().QinPrimePurge) qin = CobeConfig::data().QinPrimePurge;
   float adjustFactor = qin / CobeConfig::data().QinPrimePurge;

   if (_TrimaSet->Reservoir().DrawCycle())
   {
      _Orders->Qac(qin / _Config->cData().Procedure.key_custom_ratio);
      _Orders->Qrp(0.0f);
   }
   else
   {
      _Orders->Qac(0.0f);
      float qrp = _Adjustments->QrpCap() + qin;
      if (qrp > CobeConfig::data().QrpStartup) qrp = CobeConfig::data().QrpStartup;
      _Orders->Qrp(qrp);
   }
   _Orders->Qin(qin);
   _Orders->Qplasma(CobeConfig::data().QpPrimePurge * adjustFactor);
   _Orders->Qcollect(CobeConfig::data().QcolPrimePurge * adjustFactor);
   _Orders->RPM(CobeConfig::data().RPMPrimePurge);

   return NORMAL;
}

int PrimePurge::postProcess ()
{
   float deltaVp = _TrimaSet->Volume().VPlasma() - _VpAtStart;

//  Per prime definition 7/25/00 no longer transition back on centrifuge speed
//   if ( _Status->CentrifugeRPM() < CobeConfig::data().RPMPrimeFinalRampTransition ) {
//      _TransitionStatus = 2;
//   }

   if ( deltaVp > CobeConfig::data().DeltaVpPrimePurge )
   {
      _TransitionStatus = 1;
   }
   return NORMAL;
}

int PrimePurge::preEnter ()
{
   _VpAtStart = _TrimaSet->Volume().VPlasma();
   return NORMAL;
}

int PrimePurge::postExit ()
{
   _TransitionStatus = NO_TRANSITION; // in case of re-entry
   return NORMAL;
}

void PrimePurge::copyOver (const PrimePurge&)
{}

void PrimePurge::cleanup ()
{
   //
   // Delete the dynamic memory ...
   //
}

/* FORMAT HASH f00263c414978eec4a98a586f2cebc38 */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      clearline.cpp
 *
 */

#include "clearline.h"
#include "recoveryman.h"
#include "substatemap.h"
#include "cobeconfig.h"
#include "procdata.h"
#include "software_cds.h"

DEFINE_OBJ(ClearLine);


ClearLine::ClearLine()
   : ClearLineAbs()
{}

ClearLine::ClearLine(const ClearLine& rec)
   : ClearLineAbs(rec)
{}

int ClearLine::shouldRecover ()
{
   if (disabled()) return 0;

   if (   ( _ClearLineRequested ) ||
          ( active() && !_LineCleared )
          )
   {
      return 1;
   }

   //
   // Set the flags to 0 ...
   //
   _LineCleared = 0;

   return 0;
}


bool ClearLine::disabled () const
{
   // return getCentAlgType() != REGULAR;
   // This version of the recovery is now used regardless of centrifuge scaling
   return 0;
}

void ClearLine::setPumpSpeeds (ProcData& pd)
{
   //
   // Run the pumps per nocollect flows corrected for clear line chamber flows
   //
   // calculate Qchmax from the latest non-recovery Qch
   const float Qchmax  = pd.Run().LatestQch.Get();

   float       QinDraw = pd.Predict().Qin(/* DrawCycle == */ true, SubstateMap::alg(NOCOLLECT_STATE));

   //
   //
   //  Scale Qp and Qc if Qin drops below Cobe config min.  Note that we want to reduce these
   //  flows but not affect Qch.
   //
   float Qcollect = CobeConfig::data().ClearLineQcollect;
   if (QinDraw < CobeConfig::data().ClearLineQin)
   {
      Qcollect *= QinDraw / CobeConfig::data().ClearLineQin;
   }
   if (pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC) Qcollect = 0.0f;

   pd.Orders().Qplasma(Qcollect - Qchmax);
   pd.Orders().Qcollect(Qcollect);
   if ( pd.Run().DrawCycle.Get() )
   {
      pd.Orders().Qin(QinDraw);
      pd.Orders().Qac(pd.Predict().Qac(/* DrawCycle == */ true, SubstateMap::alg(NOCOLLECT_STATE)));
      pd.Orders().Qrp(0.0f);
   }
   else   // run Qrp and Qin for 100% recirc
   {
      pd.Orders().Qin(pd.Predict().Qin(/* DrawCycle == */ false, SubstateMap::alg(NOCOLLECT_STATE)));
      pd.Orders().Qac(0.0f);
      pd.Orders().Qrp(pd.Predict().Qrp(/* DrawCycle == */ false, SubstateMap::alg(NOCOLLECT_STATE)));
   }
}

/* FORMAT HASH 249736afc11fe463297c3ab302496830 */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      clearlinescaled.cpp
 *
 */

#include "clearlinescaled.h"
#include "recoveryman.h"
#include "substatemap.h"
#include "cobeconfig.h"

DEFINE_OBJ(ClearLineScaled);

ClearLineScaled::ClearLineScaled()
   : ClearLineAbs()
{}

ClearLineScaled::ClearLineScaled(const ClearLineScaled& rec)
   : ClearLineAbs(rec)
{}

int ClearLineScaled::shouldRecover ()
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


bool ClearLineScaled::disabled () const
{
   // return getCentAlgType() != SCALED;
   //  This version of the recovery is no longer used regardless of centrifuge algorithm type
   return 1;
}

void ClearLineScaled::setPumpSpeeds (ProcData& pd)
{
   //
   // Run the pumps per nocollect flows corrected for clear line chamber flows
   //
   const float RPM = pd.Orders().RPM();     // use currently commanded RPM for calculations

   // calculate Qchmax from the latest non-recovery Qch
   const float Qchmax  = pd.Run().LatestQch.Get() * (RPM / CobeConfig::data().RPMCollect) * (RPM / CobeConfig::data().RPMCollect);

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
   DataLog(log_level_proc_debug) << "Clearline: RPM:" << RPM << " Qchmax:" << Qchmax << " Qin:" << QinDraw << " Qc:" << Qcollect << endmsg;

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

/* FORMAT HASH 777b1d4af1586ec9ea0c5d1ad6bafeb4 */

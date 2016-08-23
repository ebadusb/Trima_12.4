/*******************************************************************************
 *
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      AdjustedHct()
 *             From donor data, disposable set-specific constants and run
 *             parameters, computes adjusted Hematocrit as a decimal value.
 *
 ******************************************************************************/

#include <vxworks.h>
#include "adjustedhct.h"
#include "cobeconfig.h"
#include "procdata.h"

float AdjustedHct::compute ()
{
   ProcData pd;

   // initial hct
   const float Hct         = pd.Donor().Hct.Get();
   float       AdjustedHct = Hct;

   if ( pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC )
   {
      // Current TBV with set fluid removed
      float fV = pd.Donor().TBV.Get() - CobeConfig::data().RBCPlsExtracorporealVolume;

      // adjusted hct = (initial Vrbcs - rbcs removed to bag ) / current TBV
      float denom =
         ( fV - pd.Volumes().PlasmaVolume.Get()
           - pd.Volumes().RBCVolume.Get()
           - pd.Volumes().PlateletVolume.Get()
           + pd.Volumes().VReplacement.Get()
           + pd.Volumes().VSalineBolus.Get()
           + pd.Volumes().Vac.Get() );

      if ( denom != 0.0f )
      {
         AdjustedHct = ( fV * Hct - pd.Volumes().RBCDose.Get() ) / denom ;

      }
   }

   if ( AdjustedHct > 1.0 || AdjustedHct < 0.0f )
      AdjustedHct = 0.0f;

   return AdjustedHct;
}

/* FORMAT HASH 031a9999aeaf021012948d274eadca5e */

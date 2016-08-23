/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      rtplateletyield.cpp
 *
 */

#include <vxWorks.h>
#include <math.h>

#include "cobeconfig.h"
#include "donorbmi.h"
#include "procdata.h"
#include "rtplateletyield.h"
#include "trima_datalog.h"


const float RTPlateletYield::calculateYield (const float Qch)
{

   ProcData pd;

   // IT 6933. Avoid updates when predict is busy.
   if (pd.PredictBlocked())
      return pd.Volumes().PreviousPlateletYield.Get();

   const HAL_VALVESTATE cv       = pd.Status().CollectValve.State();                      // State of the collect valve
   float                Ypt      = pd.Volumes().PlateletYield.Get();                      // Estimated platelet yield
   const State_names    state    = pd.Run().Substate.Get();                               // Current state of the procedure
   const float          YtargSel = pd.RunTargets().SelectedPlateletYield.Get() * 1.0E11f; // Currently selected yield

   //
   // If not collecting any platelets either because it is not
   // part of the procedure selected OR we have not hit that time
   // when we are suppose to start collecting, then we do not care
   // about the calculated yield (since there is not any change).
   //
   if (    cv == HAL_VALVE_COLLECT
           && Qch > 0.0f )
   {
      const float Vintarg = pd.Predict().VinTarget.Get(); // Current inlet target
      const float VinPIR  = (pd.Volumes().VinPIRStart.Get() != 0.0f) ?
                            (pd.Volumes().VinPIRStart.Get()) : Vintarg; // Current inlet target for PIR start
      const float Ytarg   = pd.RunTargets().PlateletYieldTarget.Get();  // Currently targeted yield
      const float Ydch    = pd.Predict().Ydch.Get();                    // Chamber capacity for non-PIR
      const float YdchPIR = pd.Predict().YdchPIR.Get();                 // Chamber capacity for PIR
      const float Vin     = pd.Volumes().Vin.Get();                     // Current inlet volume processed
      const float Viny    = CobeConfig::data().PlateletDeltaVinYield;   // Volume between when the valve opens and platelets
                                                                        //  flow up the line
      const float Vincv   = pd.Volumes().Vincv.Get();                   // Vin at which the collect valve opened and platelet
      //  collection begins
      float       Npsat;                                          // Np at LRS chamber saturation ( 10E11 plts )
      float       Np;                                             // number of platelets in LRS chamber
      const float Fch = CobeConfig::data().PurgeLossPltInChamber; // fraction of platelets in LRS chamber for purge platelet loss
      float       Ap  = CobeConfig::data().PlateletSaturationAp;  // LRS chamber saturation constant for platelets
      const float Bp  = CobeConfig::data().PlateletSaturationBp;  // LRS chamber saturation constant for platelets
      const float Csf = pd.Predict().Csf(state);
      Ap = Ap * Csf;
      float       Fv;        // Fraction of platelet collection completed, not counting PIR
      float       FvPIR;     // Fraction of platelet collection completed during PIR
      float       DeltaYpir; // Difference between the chamber in non-PIR and PIR states
      float       Vfraction; // Ratio of non-PIR Vin to total Vin
      float       denom;     // Denominator
      float       Vinsetup = Vincv + Viny;
      float       Temp;

      if ( state < SS_EXTENDED_PCA )
      {
         float Ypur;                                           // Maximum number of platelets lost per any purge
         float Fw;                                             // Fractional WBC saturation of LRS chamber
         float Cw  = CobeConfig::data().MNCPrecountForPltLoss; // Average donor MNC count
         float Cwp = MNCPrecountForPurge();                    // 99th percentile value of Cw
         Fw    = Cw / Cwp;
         // IT 8188. Calculation of Npsat does not match algorithms document.
         denom = Ap;                           ( denom != 0.0f ) ?
         denom = (Qch + Bp * Ap) / denom                                            : denom = 1.0f;
         Npsat = 1.0 / denom;                       // Qch can't be zero, so denom can't be zero
         Np    = Npsat * ( 1 - Fw );
         Ypur  = Fch * Np;

         //
         // Save the total platelets lost due to purges ...
         //  ( number of purges * platelets lost per purge )
         //
         pd.MakeRunWritable();
         pd.Run().Ydpur.Set(Ypur * pd.Run().NumberWBCPurges.Get());
         pd.MakeRunReadable();
      }

      Vfraction = 1.0;
      if ( Vintarg >= Vinsetup + 1.0f && Vintarg > VinPIR )
         Vfraction = ( VinPIR - Vinsetup ) / ( Vintarg - Vinsetup );
      Vfraction = ( Vfraction > 0 ) ? Vfraction : 0.0f;

      DeltaYpir = 0.0f;
      if ( Vfraction < 1.0 )
         DeltaYpir = Ydch - YdchPIR;
      if ( DeltaYpir < 0.05f )
         DeltaYpir = 0.0f;

      denom = ( VinPIR - Vincv - Viny );         ( denom > 0.0f ) ?
      Fv    = ( Vin - Vincv - Viny ) / denom                           : Fv = 1.0f;
      if ( Fv < 0.0f )
         Fv = 0.0f;
      else if ( Fv > 1.0f )
         Fv = 1.0f;

      denom = ( Vintarg - VinPIR );              ( denom > 0.0f ) ?
      FvPIR = ( Vin - VinPIR ) / denom                              : FvPIR = 0.0f; // FvPIR should be 0 if no PIR state
      if ( FvPIR < 0.0f )
         FvPIR = 0.0f;
      else if ( FvPIR > 1.0f )
         FvPIR = 1.0f;

      Ypt  = ( Vfraction * ( Ytarg - DeltaYpir ) ) * Fv;
      Temp = ( ( ( 1 - Vfraction ) * ( Ytarg - DeltaYpir ) ) + DeltaYpir ) * FvPIR;
      Ypt += Temp;

      if ( Ypt < 0.0f )
         Ypt = 0.0f;
      else if ( Ypt > Ytarg )
         Ypt = Ytarg;

      //
      // The collection finished to target, the yield should
      //  be adjusted to be equal ... (change when within 3.0mls of target)
      //
      if ( ( Vin + 3.0f ) >= Vintarg )
      {
         Ypt = Ytarg;
      }

      Ypt *= 1.0E11f;
   }

   // IT 12974 don't set the yield to target if the target is 0 and the collected volume > 0
   if ( state >= SS_EXTENDED_PCA && state <= SS_RBC_PTF_SETUP_1 && Ypt > YtargSel
        && !(YtargSel < 0.01f && pd.Volumes().PlateletVolume.Get() > 0.0 ) )
   {
      Ypt = YtargSel;
   }

   pd.MakeVolumesWritable();
   pd.Volumes().PlateletYield.Set(Ypt);
   pd.Volumes().PreviousPlateletYield.Set(Ypt);
   return Ypt;
}


float RTPlateletYield::MNCPrecountForPurge ()
{
   ProcData pd;
   if ( pd.Donor().BMI.Get() < CobeConfig::data().BMIForIncreasedPurges )
      return CobeConfig::data().MNCPrecountForPurge;
   else
      return CobeConfig::data().MNCPrecountForPurgeLargeBMI;
}

/* FORMAT HASH 50144ec86877fe3c5f7ce4812c07311f */

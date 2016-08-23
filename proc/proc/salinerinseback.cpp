/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plasmarinseback.cpp
 *
 */

#include "salinerinseback.h"
#include "states.h"
#include "cobeconfig.h"
#include "procdata.h"

#include "trimamessages.h"

DEFINE_STATE(SalineRinseback);

SalineRinseback::SalineRinseback()
   : StateAbs(),
     _totalVolumeToDonor(0.0f),
     _PeriodicStateMsg(0, Callback<SalineRinseback>(this, &SalineRinseback::rinsebackState), TimerMessage::DISARMED),
     _QreturnMax(0.0f),
     _Qr(0.0f),
     _IRMax(0.0f),
     _VacRes(0.0f),
     _VRes(0.0f)

{}

SalineRinseback::~SalineRinseback()
{}

void SalineRinseback::rinsebackState ()
{
   ProcData pd;

   //
   // Set the new system state notification.  Make sure we don't set
   // it if we have already gotten to disconnect prompt.
   //
   if ( pd.TrimaSet().CassetteState.Get() == ProcTrimaSet_CDS::RINSEBACK )
   {
      pd.SystemState(BLOOD_RINSEBACK, false);
   }

   DataLog(log_level_proc_rinseback_info) << "SalineRinseback :: rinsebackState()" <<   endmsg;



   _PeriodicStateMsg.interval(0);
}

int SalineRinseback::preProcess ()
{
   ProcData pd;

   //
   // Accumulate volumes ...
   //
   pd.Update(ProcData::RINSEBACK);

   //
   // Deactivate the timers ...
   //
   pd.SysRunTime().inactivate();
   pd.ProcRunTime().inactivate();


   return NORMAL;
}

int SalineRinseback::postProcess ()
{
   ProcData pd;

   // check amount returned vs Donor limitations.
   if (pd.Volumes().VRetDuringSalineRinsebackRinseback.Get() > pd.Volumes().VMaxSalRinseback.Get())
   {
      DataLog(log_level_proc_rinseback_info) << "SalineRinseback: Ending SalRB because returning too much fluid to donor.   Limit:"
                                             << pd.Volumes().VMaxSalRinseback.Get()
                                             << ".  Fluid Returned to Donor: "
                                             << pd.Volumes().VRetDuringSalineRinsebackRinseback.Get()
                                             << endmsg;

      AckRunCompleteMsg endit(MessageBase::SEND_LOCAL);
      endit.send(0);

   }



   return NORMAL;
}

int SalineRinseback::preEnter ()
{


   //
   // Notify the system we are in rinseback state
   //
   ProcData pd;
   pd.SystemState(BLOOD_RINSEBACK);


   pd.MakeRunWritable();
   // rinseback considered started for MSS purposes.. not a slavage case.
   pd.Run().rinsebackEnteredMss.Set(true);
   pd.MakeRunReadable();

   //
   // Plasma rinseback requested ...
   pd.SalineRinsebackActive(true);


   //
   // Set up the state message
   _PeriodicStateMsg.interval(5000);     // 5 seconds

   //
   // Deactivate the times for  ...
   //
   pd.SysRunTime().inactivate();
   pd.ProcRunTime().inactivate();




   // calc the maxQR baded on I and ac stuff


   //
   //  Check to see if we have processed enough volume.  Before this point
   //  we always return assuming a ratio of 5 to avoid safety shutdowns.
   //
   float ratio = (pd.Volumes().VinTotal.Get() > 325.0f) ? pd.Run().LastRunRatio.Get() : 5.0f;
   if (ratio < 5.0f) ratio = 5.0f;



   //
   //
   //   Calculate maximum infusion rate and return flow allowable given procedure constraints
   //
   _IRMax = pd.RunTargets().InfusionRateTarget.Get();

   if (_IRMax < MIN_INFUSION_RATE)
      _IRMax = MIN_INFUSION_RATE;


   float Hin      = pd.Run().AdjustedHct.Get() * (1.0f - 1.0f / ratio);
   float QrpLimit = 520.0f * (1.0f - Hin);

   _QreturnMax = _IRMax * ratio * (pd.Donor().TBV.Get() / 1000.0f);

   if (_QreturnMax > QrpLimit) _QreturnMax = QrpLimit;

   //
   //
   //  Safety has a hardcoded limit of 90ml/min.  Do not exceed this
   //  limit or safety will shut the system down.
   //
   if (_QreturnMax > 90.0f) _QreturnMax = 90.0f;

   DataLog(log_level_proc_rinseback_info) << "SalineRinseback: _QreturnMax = " << _QreturnMax << endmsg;
   double facb;
   if ( ratio <= 1.0 || pd.Run().AdjustedHct.Get() >= 1.0 )  // Can't happen, but better SAFE than sorry
      facb = 1.0;                                            //  ( assume All fluid in the reservoir is AC )
   else
      //
      // Calculate the fraction of AC from the fluid in the reservoir
      //
      facb = 1 / ( 1 + ( ratio - 1.0 ) * ( 1.0 - pd.Run().AdjustedHct.Get() ) );

   //
   // Calculate the volume of AC in the reservoir
   //
   float vRes     = pd.Cycle().Vres.Get();
   float vResLast = pd.Cycle().VresLast.Get();
   if ( vRes < 0.0f && vResLast > 0.0f )  // return cycle
   {
      _VRes = vResLast + vRes + CobeConfig::data().NominalReturnLineVolume;
   }
   else  // draw cycle
   {
      _VRes = fabs(vRes) + CobeConfig::data().NominalReturnLineVolume;
   }
   _VacRes = facb * _VRes * ( 1 - pd.Cycle().ResHct.Get() );

   DataLog(log_level_proc_rinseback_info) << "SalineRinseback: ratio=" << ratio << " IRMax=" << _IRMax
                                          << " QrpLimit=" << QrpLimit << " facb=" << facb
                                          << " VacRes=" << _VacRes << " VRes=" << _VRes << " MaxQr=" << _QreturnMax
                                          << endmsg;

   // set these values for use in evac2 also
   pd.MakeRunWritable();
   pd.Run().QReturnMax.Set(_QreturnMax);
   pd.Run().IRMax.Set(_IRMax);


   pd.MakeVolumesWritable();
   pd.Volumes().Vres.Set(_VRes);
   pd.Volumes().VacRes.Set(_VacRes);


   float resVolume             = (float)fabs(pd.Cycle().Vres);
   float distance_to_ul_sensor = (pd.Run().DrawCycle.Get() ?
                                  (CobeConfig::data().ReservoirVolume - resVolume)
                                  : resVolume);


   float srDrawVolume = CobeConfig::data().SrDrawVr *  2.0f;  // 2.0 is the ratio between the Qc/Qr


   float maxVreturn = pd.ExtracorporealVolume()                    // a full set
                      + CobeConfig::data().NominalReturnLineVolume // allow all of return line
                      + 5.0f                                       // return prime above low level sensor
                      - pd.TotalResidualVolume()                   // without residuals
                      - distance_to_ul_sensor
                      + (srDrawVolume + CobeConfig::data().SrFlushVr) // saline we nominally use
                      + CobeConfig::data().SrMaxReturnVolumeBuffer;   // just some extra margin if we need it



   // do the calculation here to figure how much more fluid the donor is allowed:
   pd.MakeVolumesWritable();
   pd.Volumes().VMaxSalRinseback.Set(maxVreturn);

   DataLog(log_level_proc_rinseback_info) << "SalineRinseback: Rinseback VMaxSalRinseback  Limit:"  << maxVreturn            << endmsg;
   DataLog(log_level_proc_rinseback_info) << "SalineRinseback: distance_to_ul_sensor:"              << distance_to_ul_sensor << endmsg;



   return NORMAL;
}

int SalineRinseback::postExit ()
{

   ProcData pd;
   pd.SalineRinsebackActive(false);

   _PeriodicStateMsg.interval(0);

   return NORMAL;
}

/* FORMAT HASH 5ceb34a862728e306a1ebeb231216a6d */

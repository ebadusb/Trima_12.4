/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plasmarinseback.cpp
 *
 */

#include "plasmarinseback.h"
#include "states.h"
#include "cobeconfig.h"
#include "procdata.h"
#include "procplsrinseback_cds.h"

DEFINE_STATE(PlasmaRinseback);

PlasmaRinseback::PlasmaRinseback()
   : StateAbs(),
     _PeriodicStateMsg(0, Callback<PlasmaRinseback>(this, &PlasmaRinseback::rinsebackState), TimerMessage::DISARMED),
     _LogRinsebackValuesTimer(0, Callback<PlasmaRinseback>(this, &PlasmaRinseback::logRinsebackValues), TimerMessage::DISARMED),
     _PlasmaValveOpenAccumulator()
{}

PlasmaRinseback::~PlasmaRinseback()
{}

void PlasmaRinseback::rinsebackState ()
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

   //
   // Stop the timer...
   _PeriodicStateMsg.interval(0);
}

int PlasmaRinseback::preProcess ()
{
   ProcData pd;

   //
   // Make sure our estimates are still good ...
   //  ( If the volume estimates are wrong, assume all plasma )
   //
   if (    pd.PlasmaRinseback().Vtotal.Get() < 1.0 /*mls*/
           || pd.PlasmaRinseback().Vac.Get() < 1.0 /*mls*/ )
   {
      //
      // Reset the data to assume all plasma ...
      //
      volumeAllPlasma();
   }

   //
   // Check for the plasma valve being open for more than 50 seconds...
   //
   monitorPlasmaValve();

   //
   // Deactivate the timers ...
   //
   pd.SysRunTime().inactivate();
   pd.ProcRunTime().inactivate();

   //
   // Accumulate volumes ...
   //
   pd.Update(ProcData::RINSEBACK);

   return NORMAL;
}

int PlasmaRinseback::postProcess ()
{
   ProcData pd;

   //
   // Always close the valve when we stop the pumps ...
   //
   if ( pd.AlarmActive() )
   {
      pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);
   }

   return NORMAL;
}

int PlasmaRinseback::preEnter ()
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
   pd.PlasmaRinsebackActive(true);

   //
   // Calculate the ending plasma target ...
   //
   float plasmaTarget = pd.RunTargets().PlasmaTarget.Get();
   plasmaTarget -= CobeConfig::data().RinsebackVplrb;
   if ( plasmaTarget >= 0.5f )
   {
      pd.PlasmaRinseback().VPlasmaTarget.Set(plasmaTarget);
   }

   //
   // Set up the state message
   //
   _PeriodicStateMsg.interval(5000);    // 5 seconds

   //
   // Set up the state message
   //
   _LogRinsebackValuesTimer.interval(10000);    // 10 seconds

   calculateMaxVolumeLimit();
   calculateRunningLimits();
   calculateVolumes();

   DataLog(log_level_proc_rinseback_info) << "Rinseback: ratio=" << pd.PlasmaRinseback().Ratio.Get()
                                          << " IRMax=" << pd.PlasmaRinseback().IRMax.Get()
                                          << " QrpLimit=" << pd.PlasmaRinseback().QReturnMax
                                          << " facb=" << pd.PlasmaRinseback().Facb.Get()
                                          << " VacTotal=" << pd.PlasmaRinseback().Vac.Get()
                                          << " VTotal=" << pd.PlasmaRinseback().Vtotal.Get()
                                          << endmsg;

   _PlasmaValveOpenAccumulator.init(pd.PlasmaRinseback().PlasmaValveOpenAccumulatedTime);

   return NORMAL;
}

int PlasmaRinseback::postExit ()
{
   //
   // Turn off the timers
   _PeriodicStateMsg.interval(0);
   _LogRinsebackValuesTimer.interval(0);
   _PlasmaValveOpenAccumulator.inactivate();

   //
   // Normal rinseback default ...
   ProcData pd;
   pd.PlasmaRinsebackActive(false);

   return NORMAL;
}

void PlasmaRinseback::monitorPlasmaValve ()
{
   ProcData      pd;
   unsigned long diff = 0;
   timespec      current;
   clock_gettime(CLOCK_REALTIME, &current);
   if ( pd.PlasmaRinseback().PlasmaValveOpenLastSeenTime.Get().tv_sec != 0 )
   {
      diff = current.tv_sec - pd.PlasmaRinseback().PlasmaValveOpenLastSeenTime.Get().tv_sec;
   }

   if (    _PlasmaValveOpenAccumulator.getSecs() >= 50                                                     /*secs*/
           || ( pd.PlasmaRinseback().PlasmaValveOpenAccumulatedTime.Get().tv_sec + diff ) >= 50 /*secs*/ ) // PFR case ...
   {
      //
      // Set the plasma volume flag ...
      //
      pd.MakePQIWritable();
      ProductQualityData PQLocal = pd.PQI().PQdata;
      PQLocal.verify_plasma_volume_reasons.Put(PLASMA_VOLUME_ERROR);
      pd.PQI().PQdata.Set(PQLocal);
      pd.MakePQIReadable();
      static bool logged = false;
      if ( !logged )
      {
         DataLog(log_level_proc_pqi_info) << "VERIFY PLASMA VOLUME because of PFR with Plasma Valve open during plasma rinseback" << endmsg;
         logged = true;
      }

      //
      // Assume all remaining volume is plasma ...
      //
      volumeAllPlasma();

      return;
   }

   if (    pd.Status().PlasmaValve.IsOpen()
           || pd.Status().PlasmaValve.IsUnknown() )
   {
      _PlasmaValveOpenAccumulator.activate();
      pd.PlasmaRinseback().PlasmaValveOpenLastSeenTime.Set(current);
   }
   else
   {
      _PlasmaValveOpenAccumulator.inactivate();
      timespec zeroTime = {0, 0};
      pd.PlasmaRinseback().PlasmaValveOpenLastSeenTime.Set(zeroTime);
   }

}

void PlasmaRinseback::volumeAllPlasma ()
{
   ProcData pd;

   //
   // Calculate the volume of AC as if the whole set is full of plasma
   //  mixed with AC ...
   //
   float facb = pd.PlasmaRinseback().Facb.Get();
   float VSet = pd.NominalFillVolume();
   float VacSet;
   if ( facb > 0.0f )
   {
      VacSet = facb * VSet;
   }
   else
   {
      VacSet = VSet;
   }

   pd.PlasmaRinseback().Vac.Set(VacSet);
   pd.PlasmaRinseback().Vtotal.Set(VSet);
}

void PlasmaRinseback::calculateMaxVolumeLimit ()
{
   ProcData pd;

   //
   //   Limit the return rinseback volume to avoid possible failures of the
   //   low level sensor.  See IT3764 for details. Note that the reservoir fill
   //   volume may be negative if we ended during a return cycle.
   //
   float resVolume             = (float)fabs(pd.Cycle().Vres.Get() );
   float distance_to_ul_sensor = ( pd.Run().DrawCycle.Get()
                                   ? ( CobeConfig::data().ReservoirVolume - resVolume)
                                   : resVolume );
   float limit =  pd.ExtracorporealVolume()   // a full set
                 + CobeConfig::data().RinsebackVplrb
                 + CobeConfig::data().NominalReturnLineVolume // allow all of return line
                 + 5.0f                                       // return prime above low level sensor
                 - pd.TotalResidualVolume()                   // without residuals
                 - distance_to_ul_sensor;

   pd.MakeVolumesWritable();
   pd.Volumes().VMaxPltRinseback.Set(limit);

   DataLog(log_level_proc_rinseback_info) << "Rinseback VMaxPltRinseback  Limit:"  << limit << endmsg;

}

void PlasmaRinseback::calculateRunningLimits ()
{
   ProcData pd;

   //
   //  Check to see if we have processed enough volume.  Before this point
   //  we always return assuming a ratio of 5 to avoid safety shutdowns.
   //
   float ratio = (pd.Volumes().VinTotal.Get() > 325.0f) ? pd.Run().LastRunRatio.Get() : 5.0f;
   if (ratio < 5.0f) ratio = 5.0f;
   pd.PlasmaRinseback().Ratio.Set(ratio);

   //
   //
   //   Calculate maximum infusion rate and return flow allowable given procedure constraints
   //
   float irMax = pd.RunTargets().InfusionRateTarget.Get();
   if (irMax < MIN_INFUSION_RATE) irMax = MIN_INFUSION_RATE;
   pd.PlasmaRinseback().IRMax.Set(irMax);

   float Hin        = pd.Run().AdjustedHct.Get() * (1.0f - 1.0f / ratio);
   float QrpLimit   = 520.0f * (1.0f - Hin);
   float QreturnMax = irMax * ratio * (pd.Donor().TBV.Get() / 1000.0f);
   if (QreturnMax > QrpLimit) QreturnMax = QrpLimit;

   //
   //
   //  Safety has a hardcoded limit of 100ml/min.  Do not exceed this
   //  limit or safety will shut the system down.
   //
   if (QreturnMax > 100.0f) QreturnMax = 100.0f;
   pd.PlasmaRinseback().QReturnMax.Set(QreturnMax);

}

void PlasmaRinseback::calculateVolumes ()
{
   ProcData pd;

   double   facb;
   float    ratio = pd.PlasmaRinseback().Ratio.Get();
   if ( ratio <= 1.0 || pd.Run().AdjustedHct.Get() >= 1.0 ) // Can't happen, but better SAFE than sorry
   {
      facb = 1.0;                                           //  ( assume All fluid in the reservoir is AC )
   }
   else
   {
      //
      // Calculate the fraction of AC from the fluid in the reservoir
      //
      facb = 1 / ( 1 + ( ratio - 1.0 ) * ( 1.0 - pd.Run().AdjustedHct.Get() ) );
   }
   pd.PlasmaRinseback().Facb.Set(facb);


   //
   // Calculate the volume of AC in the reservoir
   //
   float VRes     = pd.Cycle().Vres.Get();
   float VResLast = pd.Cycle().VresLast.Get();
   if ( VRes < 0.0f && VResLast > 0.0f ) // return cycle
   {
      VRes = VResLast + VRes - pd.Volumes().VRetRinseback.Get() + CobeConfig::data().NominalReturnLineVolume;
   }
   else // draw cycle
   {
      VRes = fabs(VRes) - pd.Volumes().VRetRinseback.Get() + CobeConfig::data().NominalReturnLineVolume;
   }
   float VacRes = facb * VRes * ( 1 - pd.Cycle().ResHct.Get() );

   //
   // Calculate the volume of AC in the rest of the set
   //
   float VSet   = pd.NominalFillVolume();
   VSet -= CobeConfig::data().ReservoirVolume + CobeConfig::data().NominalReturnLineVolume;
   float VacSet = facb * VSet * ( 1 - pd.Run().AdjustedHct.Get() );

   pd.PlasmaRinseback().Vac.Set(VacRes + VacSet);
   pd.PlasmaRinseback().Vtotal.Set(VRes + VSet);

}

void PlasmaRinseback::logRinsebackValues ()
{
   ProcData pd;

   //
   // Log the rinseback values
   //
   DataLog(log_level_proc_rinseback_info) << "PlsRinseback: "
                                          << pd.PlasmaRinseback().VPlasmaTarget.Get() << " "
                                          << pd.PlasmaRinseback().Facb.Get() << " "
                                          << pd.PlasmaRinseback().Vtotal.Get() << " "
                                          << pd.PlasmaRinseback().Vac.Get() << " "
                                          << pd.PlasmaRinseback().IRMax.Get() << " "
                                          << pd.PlasmaRinseback().QReturnMax.Get() << " "
                                          << pd.PlasmaRinseback().Ratio.Get() << " "
                                          << pd.PlasmaRinseback().PlasmaValveOpenAccumulatedTime.Get().tv_sec << "."
                                          << pd.PlasmaRinseback().PlasmaValveOpenAccumulatedTime.Get().tv_nsec << " "
                                          << pd.PlasmaRinseback().PlasmaValveOpenLastSeenTime.Get().tv_sec << "."
                                          << pd.PlasmaRinseback().PlasmaValveOpenLastSeenTime.Get().tv_nsec << " "
                                          << endmsg;
}

/* FORMAT HASH 72a40c6b0a67724189dc23b9c8c3dd8d */

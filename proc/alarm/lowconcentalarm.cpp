/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      lowconcentalarm.cpp
 *
 */

#include "lowconcentalarm.h"
#include "alarms.h"
#include "rbcdat.h"
#include "trima_datalog.h"
#include "procdata.h"

DEFINE_OBJ(LowConcentrationAlarm);

LowConcentrationAlarm::LowConcentrationAlarm()
   : _LowConcentrationAlert(LOW_PLATELET_CONCENTRATION_ALERT),
     _NotAlreadyGiven(1)
{}

LowConcentrationAlarm::~LowConcentrationAlarm()
{}

void LowConcentrationAlarm::Monitor ()
{
   ProcData pd;

   float    triggerpoint = RbcDat::data().LowConcentrationTrigger;

   if (    _NotAlreadyGiven
           && pd.RunTargets().SelectedPlateletConcen.Get()
           <= RbcDat::data().LowConcentrationShutoffConcentration
           && pd.TrimaSet().RBCDetectorCalibrated.Get()
           && triggerpoint > 0.0f
           && pd.Volumes().PlateletYield.Get() >= ( triggerpoint * 1.0E11f )
           )
   {
      float pcntYield = ( pd.Run().IntegratedPlateletYield.Get() /
                          pd.Volumes().PlateletYield.Get() );
      if ( pcntYield < RbcDat::data().LowConcentrationThreshold )
      {
         _LowConcentrationAlert.setAlarm();
         _NotAlreadyGiven = 0;

         DataLog(log_level_proc_alarm_monitor_info) << "RBC cal: " << pd.TrimaSet().RBCDetectorCalibrated.Get()
                                                    << " LCtr: " << triggerpoint
                                                    << " PltYield: " << pd.Volumes().PlateletYield.Get()
                                                    << " iYield: " << pd.Run().IntegratedPlateletYield.Get()
                                                    << " %Yield: " << pcntYield
                                                    << " LCth: " << RbcDat::data().LowConcentrationThreshold << endmsg;

      }
   }
}

/* FORMAT HASH 8ca8a68e16f739011addbfb582ecb81b */

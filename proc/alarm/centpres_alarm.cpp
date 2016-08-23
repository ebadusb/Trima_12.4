/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      centpres_alarm.cpp
 *
 */

#include "centpres_alarm.h"
#include "trima_datalog.h"
#include "procdata.h"
#include "CatalogChecker.h"

DEFINE_OBJ(CentPresAlarm);

CentPresAlarm::CentPresAlarm()
   : _centPresAlertNoClamp(CPS_TOO_HIGH_ALERT_NO_CLAMP),
     _centPresAlertOneClamp(CPS_TOO_HIGH_ALERT_ONE_CLAMP),
     _centPresAlertThreeClamp(CPS_TOO_HIGH_ALERT_THREE_CLAMP),
     _centPresAlertThreeClampPtf(CPS_TOO_HIGH_ALERT_THREE_CLAMP_PTF)
{}

CentPresAlarm::~CentPresAlarm()
{}

void CentPresAlarm::Monitor ()
{
   ProcData pd;
   if ( pd.Status().CPS() > pd.TrimaSet().CentPresLimit.Get() )
   {
      bool setIsRas = CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::MSS_RBC);
      bool setIsPas = CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::MSS_PLT);

      //  once is more than enough!
      if (_centPresAlertThreeClamp.getState()      != ACTIVE &&
          _centPresAlertOneClamp.getState()        != ACTIVE &&
          _centPresAlertNoClamp.getState()         != ACTIVE &&
          _centPresAlertThreeClampPtf.getState()   != ACTIVE)
      {
         DataLog(log_level_proc_alarm_monitor_info)
            << "General CPS alarm :: CPS value too high (non-zero'ed) : " << pd.Status().CPS()
            << " ( limit-> " << pd.TrimaSet().CentPresLimit.Get() << " )"
            << "RBC Volume -> " << pd.Volumes().RBCVolume
            << "(rbc prod1 -> " << pd.Volumes().RBCProduct_A_FinalVolume
            << " rbc prod2 -> " << pd.Volumes().RBCProduct_B_FinalVolume << " )"
            << endmsg;

         // alarm based on set configuration:
         if (setIsPas)
         {
            if (setIsRas) _centPresAlertThreeClampPtf.setAlarm();
            else _centPresAlertThreeClamp.setAlarm();
         }
         else
         {
            if (setIsRas) _centPresAlertOneClamp.setAlarm();
            else _centPresAlertNoClamp.setAlarm();
         }
      }

   }
}

/* FORMAT HASH c5831ef212885c0644c8288ffe9f47cd */

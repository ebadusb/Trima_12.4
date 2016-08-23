/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      EvacuateBags.cpp
 *
 */

#include "evacuatebags.h"
#include "an_alarm.h"
#include "guiproc.h"
#include "procdata.h"
#include "cobeconfig.h"


DEFINE_STATE(EvacuateBags);

EvacuateBags::EvacuateBags()
   : StateAbs(),
     _PressureAlarm(EVACUATION_PRESSURE_ALERT),
     _PressureAlarmJapBmp (EVACUATION_PRESSURE_ALERT_JP),
     _PressureAlarmLast(EVACUATION_PRESSURE_ALERT_FINAL),
     _ReturnVolume(0.0f),
     _APSatStart(0.0f),
     _NumberofAlarms(0),
     _TestPassed(0),
     _ReturnVolumeReducePressure(0.0),
     _doPressureAndAlarm(false),
     _BailOut(0)
{}

EvacuateBags::EvacuateBags(const EvacuateBags& state)
   : StateAbs(state),
     _PressureAlarm(EVACUATION_PRESSURE_ALERT),
     _PressureAlarmJapBmp (EVACUATION_PRESSURE_ALERT_JP),
     _PressureAlarmLast(EVACUATION_PRESSURE_ALERT_FINAL),
     _ReturnVolume(state._ReturnVolume),
     _APSatStart(state._APSatStart),
     _NumberofAlarms(0),
     _TestPassed(0),
     _ReturnVolumeReducePressure(0.0),
     _doPressureAndAlarm(false),
     _BailOut(0)
{}

EvacuateBags::~EvacuateBags()
{}


//////////////////////////////////
int EvacuateBags::transitionStatus ()
{
   if (Software_CDS::GetInstance().getFeature(AiroutMitigation))
      return 1;

   if (_BailOut)
      return 2;
   else if (_TestPassed)
      return 1;
   else
      return NO_TRANSITION;
}
////////////////////////////////////


int EvacuateBags::preProcess ()
{
   ProcData pd;


   if (reducingPressure())
      return NORMAL;

   if ( pd.AlarmActive() )
   {
      _APSatStart = pd.Status().APS();
   }
   else if (   (CobeConfig::data().EvacuationVolume <= 0)
               || (!pd.Config().Procedure.Get().key_air_removal)
               )
   {
      _TestPassed = 1;

   }
   else
   {
      if ( _ReturnVolume > CobeConfig::data().EvacuationVolume )
      {
         _TestPassed = 1;
      }
      else
      {
         pd.Orders().Qrp(CobeConfig::data().EvacuationReturnSpeed);
      }
   }

   return NORMAL;
}
////////////////////////////////////////

int EvacuateBags::postProcess ()
{
   ProcData pd;


   if (reducingPressure())
      return NORMAL;


   _ReturnVolume += pd.Status().ReturnPump.dV();

   float deltaAPS = pd.Status().APS() - _APSatStart;



   if (deltaAPS > CobeConfig::data().EvacuationDeltaPressureLimit)
   {

      DataLog(log_level_proc_info) << "  Pressure too high = " <<  pd.Status().APS()  << endmsg;

      _doPressureAndAlarm = true;
      // _ReturnVolumeReducePressure = 0.0f;
      _APSatStart         = pd.Status().APS();
   }

   return NORMAL;
}

//////////////////////////////////////

bool EvacuateBags::reducingPressure (void)
{

   ProcData pd;
   if (_doPressureAndAlarm)
   {

      _ReturnVolumeReducePressure += pd.Status().ReturnPump.dV();

      if (_ReturnVolumeReducePressure <  CobeConfig::data().EvacuationRecoveryVolumeLimit )
      {
         // we are doing too much return volume to mitigate the pressure... stop this

         DataLog(log_level_proc_info) << "Return pump ran too long trying to reduce APS pressure (aps= " <<  pd.Status().APS()
                                      << "; _ReturnVolumeReducePressure=" << _ReturnVolumeReducePressure << ")"  << endmsg;

         _NumberofAlarms = 5;     // bump this high to cause the A1 end run option
         setAlarm();
      }



      // DataLog( log_level_proc_info ) << "_ReturnVolumeReducePressure=" << _ReturnVolumeReducePressure << endmsg;

      if (pd.Status().APS() > CobeConfig::data().EvacuationPressureRecoveryLimit)
      {
         pd.Orders().Qrp(-1 * CobeConfig::data().EvacuationReturnSpeed);

         // DataLog( log_level_proc_info ) << "correcing pressure  Pressure = " <<  pd.Status().APS()
         //                               << "_ReturnVolumeReducePressure=" << _ReturnVolumeReducePressure << endmsg;
      }
      else
      {

         DataLog(log_level_proc_info) << "stopping pressure OK! Pressure = " <<  pd.Status().APS()
                                      << "_ReturnVolumeReducePressure=" << _ReturnVolumeReducePressure << endmsg;

         pd.Orders().Qrp(0.0f);
         _doPressureAndAlarm         = false;
         _ReturnVolumeReducePressure = 0.0f;
         setAlarm();
      }

   }
   return _doPressureAndAlarm;

}


//////////////////////////////////////

void EvacuateBags::setAlarm (void)
{
   if (Software_CDS::isJapaneseWorkflow())
   {
      if (_PressureAlarm.getState() != ACTIVE)
      {
         _NumberofAlarms++;
         if (_NumberofAlarms < 2)
         {
            _PressureAlarmJapBmp.setAlarm();
         }
         else
         {
            _BailOut = 1;
            _PressureAlarmLast.setAlarm();
         }
      }

   }
   else
   {

      if (_PressureAlarm.getState() != ACTIVE)
      {
         _NumberofAlarms++;
         if (_NumberofAlarms < 2)
         {
            _PressureAlarm.setAlarm();
         }
         else
         {
            _BailOut = 1;
            _PressureAlarmLast.setAlarm();
         }
      }

   }




}
//////////////////////////////////////


int EvacuateBags::preEnter ()
{
   ProcData pd;

   //
   // Make sure pumps are stopped
   //
   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qrp(0.0f);

   _ReturnVolume       = 0.0f;
   _APSatStart         = pd.Status().APS();
   _TestPassed         = 0;
   _doPressureAndAlarm = false;
   //
   //
   //   Tell GUI what we're doing
   //
   if ( CobeConfig::data().EvacuationVolume > 0 )
   {
      CassettePositionStatusMsg posmsg(MessageBase::SEND_LOCAL);
      posmsg.send(CASSETTE_EVACUATING);
   }

   return NORMAL;
}


//////////////////////////////////////////

int EvacuateBags::postExit ()
{
   ProcData pd;

   //
   // Make sure pumps are stopped
   //
   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qrp(0.0f);

   //
   // Reset the test passed flag
   //
   _TestPassed = 0;

   return NORMAL;
}

/* FORMAT HASH 52c188354ab2a0d7caddc89e40b7d82d */

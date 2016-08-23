/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      salinealarm.cpp
 *
 */


#include "salinealarm.h"
#include "guiproc.h"
#include "procdata.h"
#include "MssCommon.h"
#include "cobeconfig.h"

DEFINE_OBJ(SalineAlarm);

SalineAlarm::SalineAlarm()
   : _SalineAlarmMsg(REPLACEMENT_FLUID_ALARM),
     _SalineBolusAlarmMsg(REPLACEMENT_FLUID_BOLUS_ALARM),
     _RepFluidConMsg(Callback<SalineAlarm>(this, &SalineAlarm::repFluidConnected) ),
     _ACConMsg(Callback<SalineAlarm>(this, &SalineAlarm::acConnected) ),
     _SalineBolusMsg(Callback<SalineAlarm>(this, &SalineAlarm::salineBolusRequest) ),
     _ACConnected(0)
{}

SalineAlarm::~SalineAlarm()
{}


void SalineAlarm::Monitor ()
{
   ProcData pd;
   if (    pd.Status().CassIsPltPlsRBC()
           || pd.Status().CassIsUnknown()
           || !pd.Status().CassIsDown() )
   {
      //
      // We don't have the ability to administer replacement fluid
      //  so this alarm isn't applicable ...
      //
      return;
   }

   //
   // Set the alarm if rep fluid is required ...
   //
   if ( _ACConnected &&
        !pd.Run().SalineConnected.Get() &&
        pd.RunTargets().RequiresRepFluid.Get() &&
        pd.TrimaSet().CassetteState.Get() < ProcTrimaSet_CDS::RINSEBACK
        )
   {
      if (_SalineAlarmMsg.getState() == CLEARED)   // reduce alarm thrashing
         _SalineAlarmMsg.setAlarm();

   }
   else if ( !_ACConnected &&
             pd.TrimaSet().CassetteState.Get() >= ProcTrimaSet_CDS::ACPRIMED )
   {
      _ACConnected = 1;
   }

   //
   // If we are in a saline bolus and run out of saline,
   //  give the saline bolus alarm ...
   //

   bool CollectValveIsReturning = false;

   if (pd.Status().CollectValve.IsReturning())
   {
      CollectValveIsReturning = true;
   }
   else
   {
      CollectValveIsReturning = false;
   }



   if ( pd.Status().ReservoirIsEmpty()
        && pd.Run().SalineConnected.Get()
        && (pd.Status().CollectPump.ActRPM() >= CobeConfig::data().MinimumPumpRPM)
        && (pd.Status().CollectPump.CmdFlow() == pd.Status().ReturnPump.CmdFlow())
        && ((pd.Status().CollectPump.CmdFlow() != 0)  && (pd.Status().ReturnPump.CmdFlow()  != 0) )
        && CollectValveIsReturning
        && !pd.AlarmActive()
        && pd.TrimaSet().CassetteState.Get() < ProcTrimaSet_CDS::RINSEBACK
        )
   {
      _SalineBolusAlarmMsg.setAlarm();
   }


}

void SalineAlarm::enable ()
{}

void SalineAlarm::disable ()
{}

void SalineAlarm::repFluidConnected ()
{
   ProcData pd;
   int      status = _RepFluidConMsg.getData();

   pd.MakeRunWritable();
   if ( status == RF_CONNECTED )
   {
      pd.Run().SalineConnected.Set(true);
   }
   else
   {
      pd.Run().SalineConnected.Set(false);

      //
      // Unrequest saline bolus ...
      //
      SalineBolusMsg sbMsg(MessageBase::SEND_LOCAL);
      sbMsg.send(0);
   }
}

void SalineAlarm::acConnected ()
{
   _ACConnected = 1;
}

void SalineAlarm::salineBolusRequest ()
{
   ProcData pd;
   int      status = _SalineBolusMsg.getData();

   if ( status &&
        !pd.Run().SalineConnected.Get() )
   {
      _SalineBolusAlarmMsg.setAlarm();
   }
}

/* FORMAT HASH 99140366da358bd9da3e97e9ddb17785 */

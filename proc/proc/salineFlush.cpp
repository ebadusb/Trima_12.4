/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      salrinseback.cpp
 *
 */

#include "cobeconfig.h"
#include "procdata.h"
#include "salineFlush.h"
#include "volumeacc.h"
#include "trimamessages.h"

DEFINE_STATE(SalFlush);

SalFlush::SalFlush()
   : StateAbs(),
     _transitionNow(false),
     _collectVolume(0.0f),
     _returnVolume(0.0f),
     _salineBagEmpty(REPLACEMENT_FLUID_NEW_BAG_ALARM),
     _salineRinsebackError(SAL_RINSEBACK_RETURN_TOO_HIGH),
     _alarmCntr(0),
     _CollectVolumeForInitPause(0.0f),
     _totalReturnVolume(0.0f)
{}

SalFlush::~SalFlush()
{}

int SalFlush::transitionStatus ()
{
   if (_transitionNow)
      return 1;

   return NO_TRANSITION;
}

int SalFlush::preProcess ()
{
   ProcData pd;
   float    Qr = pd.Run().QReturnMax.Get();
   if (Qr > 50.0f)
      Qr = 50.0f;

   float Qc = Qr  ;


   pd.Orders().Qac(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qplasma(0.0f);

   pd.Orders().Qcollect(Qc);

   if (pd.SafetyControlReservoirIsEmpty()
       || (_CollectVolumeForInitPause < 7.0f)  )
   {
      pd.Orders().Qrp(0.0f);
   }
   else
   {
      pd.Orders().Qrp(Qr);
   }

   return NORMAL;
}

int SalFlush::postProcess ()
{
   ProcData pd;


   _returnVolume              += pd.Status().ReturnPump.dV();
   _totalReturnVolume         += pd.Status().ReturnPump.dV();
   _collectVolume             += pd.Status().CollectPump.dV();
   _CollectVolumeForInitPause += pd.Status().CollectPump.dV();


   if (_totalReturnVolume >= CobeConfig::data().SrFlushVr)
   {
      _transitionNow = true;
   }

   if (_collectVolume >= 25.0f)
   {
      if (_returnVolume < 5.0f)
      {

         _alarmCntr++;
         if (_alarmCntr <= 4)
         {
            DataLog(log_level_proc_rinseback_info) << "SalineRinseback: Saline not seen; set saline bag alert" << endmsg;
            _salineBagEmpty.setAlarm();
         }
      }

      _returnVolume  = 0.0f;
      _collectVolume = 0.0f;


   }



   // update the limiting monitor volume
   float currVolm       = pd.Volumes().VRetDuringSalineRinsebackRinseback.Get();
   float currSalineVolm = pd.Volumes().VSalineUsedinRinseback.Get();

   pd.MakeVolumesWritable();
   pd.Volumes().VRetDuringSalineRinsebackRinseback.Set(currVolm + pd.Status().ReturnPump.dV() - pd.Status().InletPump.dV()  ) ;
   pd.Volumes().VSalineUsedinRinseback.Set(currSalineVolm +  pd.Status().CollectPump.dV() ) ;



   if ( pd.Status().ReservoirIsFull() || _alarmCntr > 4)
   {

      if (_alarmCntr > 4)
         DataLog(log_level_proc_rinseback_info) << "SalineRinseback: Ending SalRB because of too many bag alerts" << endmsg;
      else
      {

         DataLog(log_level_proc_rinseback_info) << "SalineRinseback: Ending SalRB because the Res is at ULS" << endmsg;
         _salineRinsebackError.setAlarm();
      }

      AckRunCompleteMsg endit(MessageBase::SEND_LOCAL);
      endit.send(0);
   }

   return NORMAL;
}

int SalFlush::preEnter ()
{

   ProcData pd;

   _returnVolume              = 0.0f;
   _collectVolume             = 0.0f;
   _CollectVolumeForInitPause = 0.0f;


   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);
   if ( pd.Volumes().RBCDose.Get() < 1.0  )
   {
      pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_COLLECT);
   }
   else
   {
      pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   }

   return NORMAL;
}

int SalFlush::postExit ()
{


   ProcData pd;

   // shut it down!
   pd.Orders().Qac(0.0f);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().RPM(0.0f);
   pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);

   if ( _transitionNow   )
   {
      pd.MakeRunWritable();
      pd.Run().RinsebackComplete.Set(RINSEBACK_SUCCESSFUL_SALINE);
      pd.MakeRunReadable();

      // Tell GUI that you are finished with procedure.
      RunCompleteMsg RunFinished(MessageBase::SEND_GLOBAL);
      RunFinished.send(1);
   }

   //
   // The cassette has now been used and should be considered
   //  in a used state ...
   //
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CassetteState.Set(ProcTrimaSet_CDS::USED);

   DataLog(log_level_proc_rinseback_info) << "SalineRinseback: limit checked at "
                                          << pd.Volumes().VRetDuringSalineRinsebackRinseback.Get()
                                          << "  Max="
                                          << pd.Volumes().VMaxSalRinseback.Get()
                                          << endmsg;


   _transitionNow = false;
   return NORMAL;
}

/* FORMAT HASH afcc28135cfb81d6076482038c2a365e */

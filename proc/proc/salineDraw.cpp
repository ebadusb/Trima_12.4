/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      salrinseback.cpp
 *
 */

#include "cobeconfig.h"
#include "procdata.h"
#include "salineDraw.h"
#include "volumeacc.h"
#include "trimamessages.h"

DEFINE_STATE(SalineDraw);

SalineDraw::SalineDraw()
   : StateAbs(),
     _returnVolume(0.0f),
     _collectVolume(0.0f),
     _transitionNow(false),
     _salineBagEmpty(REPLACEMENT_FLUID_NEW_BAG_ALARM),
     _salineRinsebackError(SAL_RINSEBACK_RETURN_TOO_HIGH),
     _alarmCntr(0),
     _totalReturnVolume(0.0f)
{}

SalineDraw::~SalineDraw()
{}

int SalineDraw::transitionStatus ()
{
   if (_transitionNow)
      return 1;

   return NO_TRANSITION;
}

int SalineDraw::preProcess ()
{
   ProcData pd;


   float Qr = pd.Run().QReturnMax.Get();
   if (Qr > 25.0f)
      Qr = 25.0f;

   float Qc = Qr * 2.0f;



   pd.Orders().Qac(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qcollect(Qc);
   pd.Orders().Qplasma(0.0f);
   if (pd.SafetyControlReservoirIsEmpty())
      pd.Orders().Qrp(0.0f);
   else
      pd.Orders().Qrp(Qr);


   return NORMAL;
}

int SalineDraw::postProcess ()
{
   ProcData pd;

   _returnVolume      += pd.Status().ReturnPump.dV();
   _totalReturnVolume += pd.Status().ReturnPump.dV();
   _collectVolume     += pd.Status().CollectPump.dV();




   if (_totalReturnVolume >= CobeConfig::data().SrDrawVr)
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

      // reset every time to avoid runs forever
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

int SalineDraw::preEnter ()
{
   ProcData pd;

   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);

   return NORMAL;
}

int SalineDraw::postExit ()
{

   ProcData pd;
   DataLog(log_level_proc_rinseback_info) << "SalineRinseback: limit checked at "
                                          << pd.Volumes().VRetDuringSalineRinsebackRinseback.Get()
                                          << "  Max="
                                          << pd.Volumes().VMaxSalRinseback.Get()
                                          << endmsg;

   _transitionNow = false;

   return NORMAL;
}

/* FORMAT HASH 31c5a5a658641edbd45e8583613c2493 */

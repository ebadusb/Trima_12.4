/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      closevalves.cpp
 *
 */

#include "checkSampleBag.h"
#include "procdata.h"

DEFINE_STATE(CheckSampleBag);

CheckSampleBag::CheckSampleBag()
   : StateAbs(),
     _MSSRBCset(false),
     _initialAPS(0.0),
     _transitionNow(false),
     _initialRetvolm(0.0),
     _initialACvolm(0.0),
     _bailOut(false),
     _alrmCnt(0),
     _InletPressureAlarmNonRasSet(FAILURE_TO_PRESSURIZE_INLET_NON_RAS_SET),
     _InletPressureAlarm(FAILURE_TO_PRESSURIZE_INLET),
     _InletPressureAlarmFinal(EVACUATION_PRESSURE_ALERT_FINAL),
     _InletPressureAlarmNonARMFinal (FAILURE_TO_PRESSURIZE_INLET_FINAL)
{}

CheckSampleBag::CheckSampleBag(const CheckSampleBag& state)
   : StateAbs(state),
     _MSSRBCset(false),
     _initialAPS(0.0),
     _transitionNow(false),
     _initialRetvolm(0.0),
     _initialACvolm(0.0),
     _bailOut(false),
     _alrmCnt(0),
     _InletPressureAlarmNonRasSet(FAILURE_TO_PRESSURIZE_INLET_NON_RAS_SET),
     _InletPressureAlarm(FAILURE_TO_PRESSURIZE_INLET),
     _InletPressureAlarmFinal(EVACUATION_PRESSURE_ALERT_FINAL),
     _InletPressureAlarmNonARMFinal (FAILURE_TO_PRESSURIZE_INLET_FINAL)
{}

CheckSampleBag::~CheckSampleBag()
{}

int CheckSampleBag::transitionStatus ()
{
   ProcData pd;

   if (_bailOut)
   {
      return 2;   // to donor disconect....

   }

   if (_transitionNow)
   {
      return 1;
   }

   return NO_TRANSITION;
}


int CheckSampleBag::preProcess ()
{

   ProcData pd;




   pd.Orders().Qrp(CobeConfig::data().MaxInstantQreturn);

   if (pd.Status().APS() < CobeConfig::data().DispTestAPSThreshold)
   {
      if (_InletPressureAlarm.getState() == LATCHED)
         _InletPressureAlarm.unlatchAlarm();

      if (_InletPressureAlarmNonRasSet.getState() == LATCHED)
         _InletPressureAlarmNonRasSet.unlatchAlarm();
   }

   return NORMAL;
}

int CheckSampleBag::postProcess ()
{

   ProcData pd;

   float    apsDelta    =  pd.Status().APS()    - _initialAPS;
   float    volumeDelta = 0.0f;

   volumeDelta = pd.Volumes().Vreturn - _initialRetvolm;

   // MSS set
   if (_MSSRBCset)
   {
      if (volumeDelta <= CobeConfig::data().DispTestSampleBagVolm )
      {
         if (apsDelta > CobeConfig::data().DispTestDeltaAPS && !pd.AlarmActive() )
         {
            DataLog(log_level_proc_disposable_test_info) << "checking sample bag Passed! (RAS)"
                                                         << "    volume processed->" << volumeDelta
                                                         << " (  volume LIMIT cc = " << CobeConfig::data().DispTestSampleBagVolm << ")"
                                                         << "; APS Delta->" << apsDelta << endmsg;


            _transitionNow = true;
         }

      }
      else
      {

         _alrmCnt++;

         DataLog(log_level_proc_disposable_test_info)
            << "checking sample bag FAILED! (RAS)"
            << "   volume processed->" << volumeDelta
            << "; APS Delta->" << apsDelta << endmsg;

         if ( (_alrmCnt > 1) /*&& (Software_CDS::GetInstance().getFeature(AiroutMitigation)) Per IT12233 */ )
         {

            if ( Software_CDS::GetInstance().getFeature(AiroutMitigation) )
               _InletPressureAlarmFinal.setAlarm("Donor clamp open Final");
            else
               _InletPressureAlarmNonARMFinal.setAlarm("Donor clamp open Final");


            DataLog(log_level_proc_disposable_test_info) << "sample bag check failed " <<  _alrmCnt << " times. goto DonorDisconnect" << endmsg;
            _bailOut = true;
         }
         else
         {
            _InletPressureAlarm.latchAlarm();

            _initialAPS     = pd.Status().APS();
            _initialRetvolm = pd.Volumes().Vreturn;
            _initialACvolm  = pd.Volumes().Vac;

         }

      }


   }
   else       // Non-MSS  set

   {
      if (volumeDelta <= CobeConfig::data().DispTestVolumeCheck)
      {
         if (apsDelta > CobeConfig::data().DispTestDeltaAPS  && !pd.AlarmActive())
         {
            DataLog(log_level_proc_disposable_test_info) << "checking sample bag Passed! (NON_RAS)"
                                                         << "    volume processed->" << volumeDelta
                                                         << " (  volume LIMIT cc = " << CobeConfig::data().DispTestSampleBagVolm << ")"
                                                         << "; APS Delta->" << apsDelta << endmsg;


            _transitionNow = true;
         }

      }
      else
      {

         _alrmCnt++;

         DataLog(log_level_proc_disposable_test_info)
            << "checking sample bag FAILED! (NON_RAS)"
            << "  volume processed->" << volumeDelta
            << "; APS Delta->" << apsDelta << endmsg;

         if ( (_alrmCnt > 1)  )
         {

            if ( Software_CDS::GetInstance().getFeature(AiroutMitigation) )
               _InletPressureAlarmFinal.setAlarm("Donor clamp open Final");
            else
               _InletPressureAlarmNonARMFinal.setAlarm("Donor clamp open Final");

            DataLog(log_level_proc_disposable_test_info) << "sample bag check failed " <<  _alrmCnt << " times. goto DonorDisconnect" << endmsg;
            _bailOut = true;
         }
         else
         {
            _InletPressureAlarmNonRasSet.latchAlarm();

            _initialAPS     = pd.Status().APS();
            _initialRetvolm = pd.Volumes().Vreturn;
            _initialACvolm  = pd.Volumes().Vac;

         }


      }

   }


   return NORMAL;
}

int CheckSampleBag::preEnter ()
{
   ProcData pd;

   pd.Orders().LockDoor();

   _initialAPS     = pd.Status().APS();
   _initialRetvolm = pd.Volumes().Vreturn;

   if (CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::MSS_RBC))
   {
      DataLog(log_level_proc_disposable_test_info) << "checking sample bag as RAS set" << "(Starting APS -> " << pd.Status().APS() << "mmHg)" << endmsg;
      _MSSRBCset = true;
   }
   else
   {

      DataLog(log_level_proc_disposable_test_info) << "checking sample bag as non-RAS set" << "(Starting APS -> " << pd.Status().APS() << "mmHg)" << endmsg;
      _MSSRBCset = false;
   }
   return NORMAL;
}

int CheckSampleBag::postExit ()
{

   ProcData pd;

   _alrmCnt = 0; // restart count if we re-enter
                 //
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qac(0.0f);

   _transitionNow  = false;
   _initialAPS     = 0.0;
   _initialRetvolm = 0.0;
   _initialACvolm  = 0.0;

   return NORMAL;
}



void CheckSampleBag::copyOver (const CheckSampleBag&)
{}

void CheckSampleBag::cleanup ()
{}

/* FORMAT HASH 08f8054a2efd4dbbf01d2f1471e26ffe */

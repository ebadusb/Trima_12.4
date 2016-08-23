/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      negativepressrelief.cpp
 *
 */

#include "air2Channel2.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "software_cds.h"     // Get Optional config file settings
                              //
#include "CatalogChecker.h"

DEFINE_STATE(Air2Channel2);

Air2Channel2::Air2Channel2()
   : StateAbs(),
     _transition(false),
     _airEvacMitigation(false),
     _currentReturnVolm(0.0f),
     _currentInletVolm(0.0f),
     _failureCnt(0),
     _whiteStamp(false),
     _closeClampAlarm(FAILURE_TO_PRESSURIZE_INLET)
{}

Air2Channel2::Air2Channel2(const Air2Channel2& state)
   : StateAbs(state),
     _transition(false),
     _airEvacMitigation(false),
     _currentReturnVolm(0.0f),
     _currentInletVolm(0.0f),
     _failureCnt(0),
     _whiteStamp(false),
     _closeClampAlarm(FAILURE_TO_PRESSURIZE_INLET)
{}

Air2Channel2::~Air2Channel2()
{}

int Air2Channel2::transitionStatus ()
{
   ProcData pd;


   const bool setIsDrbc     = CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::DRBC_BAG);
   const bool setIsRbc      = CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::RBC_BAG);
   const bool setHasSomeRBC = ( setIsRbc || setIsDrbc );
   if ( !setHasSomeRBC )
   {
      DataLog(log_level_proc_disposable_test_info) << " No RBC bags, state not needed" << endmsg;
      return 1;
   }

   if (!Software_CDS::GetInstance().getFeature(AiroutMitigation))
   {
      DataLog(log_level_proc_disposable_test_info) << " Air mitigation off" << endmsg;
      return 1;
   }


   if (!pd.Config().Procedure.Get().key_air_removal)
   {
      DataLog(log_level_proc_info) << "No Air2Channel2 key_air_removal is false -- skipping Air2Channel2" << endmsg;
      return 1;
   }

   if (pd.TrimaSet().Cassette == cassette::RBCPLS)
   {
      DataLog(log_level_proc_info) << "No Air2Channel2 on Black stamp set -- skipping Air2Channel2" << endmsg;
      return 1;
   }



   if (_transition)
   {
      return 1;
   }
   return NO_TRANSITION;
}

int Air2Channel2::preProcess ()
{
   ProcData pd;

   if (_airEvacMitigation && _whiteStamp)
   {
      if ( pd.Status().APS() > 50.0f )
      {
         pd.Orders().Qin(110.0f);        // look careful these are switched for disp test two als diff test 1
         pd.Orders().Qrp(0.0f);
      }
      else
      {
         pd.Orders().Qrp(110.0f);
         pd.Orders().Qin(110.0f);
      }

      if (pd.Status().CPS() > 315.0f)
      {
         DataLog(log_level_proc_disposable_test_info)
            << " Air2Channel2 finished pressure of @ "
            << pd.Status().APS()  << " mmHg"
            << endmsg;

         _transition = true;
      }

   }

   return NORMAL;
}

int Air2Channel2::postProcess ()
{
   ProcData pd;

   if (_airEvacMitigation && _whiteStamp)
   {
      _currentReturnVolm += pd.Status().ReturnPump.dV();
      if (_currentReturnVolm > 115.0f)
      {
         _closeClampAlarm.setAlarm("Donor clamp open");
         _currentReturnVolm = 0.0f;
         _currentInletVolm  = 0.0f;

         _failureCnt++;

         DataLog(log_level_proc_disposable_test_info)
            << " failed to deliver Air2Channel2 within Vret limit (count = "
            <<  (_failureCnt) << ")" << endmsg;

      }

      _currentInletVolm += pd.Status().InletPump.dV();
      if (_currentInletVolm > 50.0f)
      {
         DataLog(log_level_proc_disposable_test_info)
            << " Air2Channel2 finished @ "
            << _currentInletVolm  << "mL of Vin"
            << "( completet in " << (_failureCnt + 1) << " attempt(s)"
            << endmsg;
         _transition = true;
      }

   }

   return NORMAL;
}

int Air2Channel2::preEnter ()
{

   if (Software_CDS::GetInstance().getFeature(AiroutMitigation))
      _airEvacMitigation = true;
   else
      _airEvacMitigation = false;

   ProcData pd;
   _whiteStamp =  (pd.TrimaSet().Cassette == cassette::PLTPLSRBC);

   if (_airEvacMitigation && _whiteStamp)
   {
      ProcData pd;
      if (pd.Config().Procedure.Get().key_air_removal)
      {


         pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_COLLECT);
         pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);
         pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);


      }

   }
   return NORMAL;
}

int Air2Channel2::postExit ()
{
   ProcData pd;

   pd.Orders().Qrp(0.0f);
   pd.Orders().Qin(0.0f);

   pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   _currentReturnVolm = 0.0f;
   _currentInletVolm  = 0.0f;


   return NORMAL;
}

void Air2Channel2::copyOver (const Air2Channel2&)
{}

void Air2Channel2::cleanup ()
{}

/* FORMAT HASH 6d93b0a1d9186c9e264061ba2d866ed3 */

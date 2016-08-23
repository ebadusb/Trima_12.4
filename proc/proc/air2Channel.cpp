/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      negativepressrelief.cpp
 *
 */

#include "air2Channel.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "software_cds.h"     // Get Optional config file settings
                              //
#include "CatalogChecker.h"

DEFINE_STATE(Air2Channel);

Air2Channel::Air2Channel()
   : StateAbs(),
     _transition(false),
     _airEvacMitigation(false),
     _currentReturnVolm(0.0f),
     _currentInletVolm(0.0f),
     _failureCnt(0),
     _whiteStamp(false),
     _closeClampAlarmNonRasSet(FAILURE_TO_PRESSURIZE_INLET_NON_RAS_SET)
{}

Air2Channel::Air2Channel(const Air2Channel& state)
   : StateAbs(state),
     _transition(false),
     _airEvacMitigation(false),
     _currentReturnVolm(0.0f),
     _currentInletVolm(0.0f),
     _failureCnt(0),
     _whiteStamp(false),
     _closeClampAlarmNonRasSet(FAILURE_TO_PRESSURIZE_INLET_NON_RAS_SET)
{}

Air2Channel::~Air2Channel()
{}

int Air2Channel::transitionStatus ()
{
   if (!Software_CDS::GetInstance().getFeature(AiroutMitigation))
   {
      DataLog(log_level_proc_disposable_test_info) << " Air mitigation off" << endmsg;
      return 1;
   }


   ProcData pd;


   const bool setIsDrbc     = CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::DRBC_BAG);
   const bool setIsRbc      = CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::RBC_BAG);
   const bool setHasSomeRBC = ( setIsRbc || setIsDrbc );
   if ( !setHasSomeRBC )
   {
      DataLog(log_level_proc_disposable_test_info) << " No RBC bags, state not needed" << endmsg;
      return 1;
   }


   if (!pd.Config().Procedure.Get().key_air_removal)
   {
      DataLog(log_level_proc_info) << "No Air2Channel key_air_removal is false -- skipping Air2Channel" << endmsg;
      return 1;
   }

   if (pd.TrimaSet().Cassette == cassette::RBCPLS)
   {
      DataLog(log_level_proc_info) << "No Air2Channel on Black stamp set -- skipping Air2Channel" << endmsg;
      return 1;
   }



   if (_transition)
   {
      return 1;
   }
   return NO_TRANSITION;
}

int Air2Channel::preProcess ()
{
   ProcData pd;

   if (_airEvacMitigation && _whiteStamp)
   {
      if ( pd.Status().APS() < 50.0f )
      {
         pd.Orders().Qrp(110.0f);
         pd.Orders().Qin(0.0f);
      }
      else
      {
         pd.Orders().Qrp(110.0f);
         pd.Orders().Qin(110.0f);
      }

      if (pd.Status().CPS() > 315.0f)
      {
         DataLog(log_level_proc_disposable_test_info)
            << " Air2Channel finished pressure of @ "
            << pd.Status().APS()  << " mmHg"
            << endmsg;

         _transition = true;
      }

   }

   return NORMAL;
}

int Air2Channel::postProcess ()
{
   ProcData pd;

   if (_airEvacMitigation && _whiteStamp)
   {
      _currentReturnVolm += pd.Status().ReturnPump.dV();
      if (_currentReturnVolm > 115.0f)
      {
         _closeClampAlarmNonRasSet.setAlarm("Donor clamp open");
         _currentReturnVolm = 0.0f;
         _currentInletVolm  = 0.0f;

         _failureCnt++;

         DataLog(log_level_proc_disposable_test_info)
            << " failed to deliver Air2channel within Vret limit (count = "
            <<  (_failureCnt) << ")" << endmsg;

      }

      _currentInletVolm += pd.Status().InletPump.dV();
      if (_currentInletVolm > 50.0f)
      {
         DataLog(log_level_proc_disposable_test_info)
            << " Air2Channel finished @ "
            << _currentInletVolm  << "mL of Vin"
            << "( completet in " << (_failureCnt + 1) << " attempt(s)"
            << endmsg;
         _transition = true;
      }

   }

   return NORMAL;
}

int Air2Channel::preEnter ()
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

int Air2Channel::postExit ()
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

void Air2Channel::copyOver (const Air2Channel&)
{}

void Air2Channel::cleanup ()
{}

/* FORMAT HASH 9b743565ac2e3f3f01dc51cf7daafa1b */

/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pressInletLine3.cpp
 *
 */

/*
$Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/proc/proc/rcs/pressInletLine3.cpp 1.4 2007/08/30 22:47:44Z jheiusb Exp jheiusb $
$Log: pressInletLine3.cpp $
Revision 1.4  2007/08/30 22:47:44Z  jheiusb
8057 -- autoSplit on black stamp
Revision 1.3  2007/06/04 20:09:03Z  jheiusb
8034 -- new disposable test
Revision 1.2  2007/02/23 23:03:36Z  jheiusb
add RBC MSS disposable test


*/

#include "pressInletLine3.h"
#include "states.h"
#include "procdata.h"
#include "MssCommon.h"

#include "cobeconfig.h"

DEFINE_STATE(PressInletLine3);

PressInletLine3::PressInletLine3()
   : StateAbs(),
     _TransitionNow(false),
     _ACVolume(0.0),
     _PressureLineAlarm(FAILURE_TO_CLOSE_CROSSOVER)
{}

PressInletLine3::PressInletLine3(const PressInletLine3& state)
   : StateAbs(state),
     _TransitionNow(false),
     _ACVolume(0.0),
     _PressureLineAlarm(FAILURE_TO_CLOSE_CROSSOVER)
{

   copyOver(state);
}

PressInletLine3::~PressInletLine3()
{
   cleanup();
}

int PressInletLine3::preProcess ()
{

   ProcData pd;

   // Ac pump to max instant. flow rate.
   pd.Orders().Qac(CobeConfig::data().QinLimitMax);

   return NORMAL;
}

int PressInletLine3::postProcess ()
{

   ProcData pd;

   // accum the return pump value
   _ACVolume += pd.Status().ACPump.dV();


   if (_ACVolume >=  CobeConfig::data().DispTestVolumeFail )
   {
      DataLog(log_level_proc_disposable_test_info)
         << "Disposable test failed ::PressInletLine3, AC Volume="
         << _ACVolume
         << " APS="
         << pd.Status().APS()
         << endmsg;


      // raise the alarm
      _PressureLineAlarm.setAlarm();

      _ACVolume = 0.0f;

   }


   if ( pd.Status().APS() >= CobeConfig::data().DispTestApsMax )
   {

      DataLog(log_level_proc_disposable_test_info)
         << "completed ... APS="
         << pd.Status().APS()
         << endmsg;

      _TransitionNow = true;
   }
   return NORMAL;
}

int PressInletLine3::transitionStatus ()
{

   if (_TransitionNow)
   {
      return 1;

   }
   return NO_TRANSITION;

}

int PressInletLine3::preEnter ()
{

   _ACVolume      = 0.0f;
   _TransitionNow = false;

   return NORMAL;
}

int PressInletLine3::postExit ()
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
   // Make sure valves are returning
   //
   //
   if (!Software_CDS::GetInstance().getFeature(AiroutMitigation))
   {
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
      pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
      pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);       //
   }

   _ACVolume      = 0.0f;
   _TransitionNow = false;



   return NORMAL;
}

void PressInletLine3::copyOver (const PressInletLine3&)
{}

void PressInletLine3::cleanup ()
{}

/* FORMAT HASH a32e2ddf38b87315766b8a058d02f90a */

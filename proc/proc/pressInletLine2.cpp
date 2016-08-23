/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pressInletLine2.cpp
 *
 */

/*
$Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/proc/proc/rcs/pressInletLine2.cpp 1.7 2007/10/08 22:32:07Z jheiusb Exp jheiusb $
$Log: pressInletLine2.cpp $
Revision 1.7  2007/10/08 22:32:07Z  jheiusb
8187 -- raise the pressure for sets check valves
Revision 1.6  2007/08/31 20:26:57Z  jheiusb
8187 -- fix disp test for new disposabe checkvalves
Revision 1.5  2007/08/30 22:47:40Z  jheiusb
8057 -- autoSplit on black stamp
Revision 1.4  2007/08/24 21:47:37Z  jheiusb
8092 -- fix disposable tests
Revision 1.3  2007/03/16 22:10:14Z  jheiusb
7920 -- increase volm in disposable test
Revision 1.2  2007/02/23 23:03:29Z  jheiusb
add RBC MSS disposable test


*/

#include "pressInletLine2.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "MssCommon.h"


DEFINE_STATE(PressInletLine2);

PressInletLine2::PressInletLine2()
   : StateAbs(),
     _InletPressureAlarm(FAILURE_TO_PRESSURIZE_INLET),
     _TransitionNow(false),
     _ReturnVolume(0.0)
{}

PressInletLine2::PressInletLine2(const PressInletLine2& state)
   : StateAbs(state),
     _InletPressureAlarm(FAILURE_TO_PRESSURIZE_INLET),
     _TransitionNow(false),
     _ReturnVolume(0.0)
{
   copyOver(state);
}

PressInletLine2::~PressInletLine2()
{
   cleanup();
}

int PressInletLine2::preProcess ()
{

   ProcData pd;

   pd.Orders().Qrp(CobeConfig::data().MaxInstantQreturn);

   return NORMAL;
}

int PressInletLine2::postProcess ()
{
   ProcData pd;

   // accum the return pump value
   _ReturnVolume += pd.Status().ReturnPump.dV();


   if (    (_ReturnVolume >=  CobeConfig::data().DispTestVolumeFail2 )
           && (pd.Status().APS() < 300.0f))
   {
      DataLog(log_level_proc_disposable_test_info)
         << "Disposable test failed ::PressInletLine2, Ret Volume="
         << _ReturnVolume
         << " APS="
         << pd.Status().APS()
         << endmsg;


      // raise the alarm
      _InletPressureAlarm.setAlarm();
      _ReturnVolume = 0.0f;

   }


   if (    (pd.Status().APS() >= 300.0f)
           && (!pd.AlarmActive())
           )
   {
      DataLog(log_level_proc_disposable_test_info)
         << "completed ... APS="
         << pd.Status().APS()
         << endmsg;

      _TransitionNow = true;
   }
   return NORMAL;
}

int PressInletLine2::transitionStatus ()
{

   if (_TransitionNow)
   {
      return 1;

   }
   return NO_TRANSITION;
}

int PressInletLine2::preEnter ()
{

   _ReturnVolume  = 0.0f;
   _TransitionNow = false;

   return NORMAL;
}

int PressInletLine2::postExit ()
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


   if (!Software_CDS::GetInstance().getFeature(AiroutMitigation))
   {
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
      pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
      pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);      //
   }

   _ReturnVolume  = 0.0f;
   _TransitionNow = false;


   return NORMAL;
}

void PressInletLine2::copyOver (const PressInletLine2&)
{}

void PressInletLine2::cleanup ()
{}

/* FORMAT HASH 19a029400f5866848042ee4314e93bd6 */

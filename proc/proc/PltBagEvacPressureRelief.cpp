/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      EvacuateBags.cpp
 *
 */

#include "PltBagEvacPressureRelief.h"
#include "an_alarm.h"
#include "guiproc.h"
#include "procdata.h"
#include "cobeconfig.h"

#include "software_cds.h"     // Get Optional sw.dat settings

#include "periodic_log.h"
#include <vxWorks.h>


DEFINE_STATE(PltBagEvacPressureRelief);

PltBagEvacPressureRelief::PltBagEvacPressureRelief()
   : StateAbs(),
     _transitionNow(false),
     _currentVcoll(0.0f),
     _currentVpls(0.0f)
{}


//////////////////////////////////
PltBagEvacPressureRelief::PltBagEvacPressureRelief(const PltBagEvacPressureRelief& state)
   : StateAbs(),
     _transitionNow(false),
     _currentVcoll(0.0f),
     _currentVpls(0.0f)
{
   copyOver(state);
}


//////////////////////////////////
PltBagEvacPressureRelief::~PltBagEvacPressureRelief()
{
   cleanup();
}


//////////////////////////////////
int PltBagEvacPressureRelief::transitionStatus ()
{

   if (!Software_CDS::GetInstance().getFeature(AiroutMitigation))
      return 1;

   ProcData pd;
   if (!pd.Config().Procedure.Get().key_air_removal)
   {
      DataLog(log_level_proc_info) << "No PLT evac key_air_removal is false -- skipping PltBagEvac" << endmsg;
      return 1;
   }

   if (pd.Status().CPS() < 315.0f)
   {

      DataLog(log_level_proc_info) << "CPS pressure normalized" << endmsg;
      return 1;
   }

   if (_transitionNow)
   {

      DataLog(log_level_proc_info) << "Transitioned by volume excess" << endmsg;
      return 1;
   }

   return NO_TRANSITION;

}
////////////////////////////////////


int PltBagEvacPressureRelief::preProcess ()
{
   ProcData pd;

   if (     pd.Status().CollectValve.IsReturning()
            && pd.Status().PlasmaValve.IsReturning()
            && pd.Status().RBCValve.IsCollecting()
            )
   {
      if (pd.TrimaSet().Cassette == cassette::RBCPLS)
      {
         pd.Orders().Qplasma(90.0f);
      }
      else
      {
         pd.Orders().Qcollect(90.0f);
      }

   }

   return NORMAL;
}
////////////////////////////////////////

int PltBagEvacPressureRelief::postProcess ()
{
   ProcData pd;


   _currentVpls  += pd.Status().PlasmaPump.dV();
   _currentVcoll += pd.Status().CollectPump.dV();

   if (  (_currentVpls  > 200.0f)
         || (_currentVcoll > 200.0f)
         )
   {
      _transitionNow = true;
      pd.Orders().Qplasma(0.0f);
      pd.Orders().Qcollect(0.0f);
      pd.Orders().Qin(0.0f);
      pd.Orders().Qrp(0.0f);
   }



   return NORMAL;
}

//////////////////////////////////////
//////////////////////////////////////


int PltBagEvacPressureRelief::preEnter ()
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



   if (pd.Status().CPS() >= 315.0f)
   {
      DataLog(log_level_proc_info) << "Moving valves to pressure relief positions" << endmsg;
      pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_COLLECT);
      pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   }



   return NORMAL;
}


//////////////////////////////////////////

int PltBagEvacPressureRelief::postExit ()
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
   _transitionNow = false;
   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void PltBagEvacPressureRelief::copyOver (const PltBagEvacPressureRelief&)
{}

void PltBagEvacPressureRelief::cleanup ()
{}

/////////////////////////////////////////////////////////////////////////////

/* FORMAT HASH 82d677121df3c353456fafd0f25a7a33 */

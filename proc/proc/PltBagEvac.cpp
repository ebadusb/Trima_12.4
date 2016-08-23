/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      EvacuateBags.cpp
 *
 */

#include "PltBagEvac.h"
#include "an_alarm.h"
#include "guiproc.h"
#include "procdata.h"
#include "cobeconfig.h"

#include "software_cds.h"     // Get Optional sw.dat settings

#include "periodic_log.h"
#include <vxWorks.h>


DEFINE_STATE(PltBagEvac);

PltBagEvac::PltBagEvac()
   : StateAbs(),
     _lastQin(90.0f),
     _transitionNow(false),
     _closePltBagvlave(false),
     myGAIN(1.0f),
     myDesiredAPS(50.0f),
     myEndQinSpeed(40.0f),
     myQret(90.0f),
     myStartingQin(90.0f),
     _minQin(1000.0f),
     _currentVin(0.0f),
     _highPressModeFinished(false),
     _highPressMode(false),
     _currentVcoll(0.0f),
     _currentVpls(0.0f),
     _totalVreturn(0.0f),
     _watchVin(0.0f),
     _minRunVolm(0.0f),
     _maxRunVolm(0.0f)
{}


//////////////////////////////////
PltBagEvac::PltBagEvac(const PltBagEvac& state)
   : StateAbs(state),
     _lastQin(90.0f),
     _transitionNow(false),
     _closePltBagvlave(false),
     myGAIN(1.0f),
     myDesiredAPS(50.0f),
     myEndQinSpeed(40.0f),
     myQret(90.0f),
     myStartingQin(90.0f),
     _minQin(1000.0f),
     _currentVin(0.0f),
     _highPressModeFinished(false),
     _highPressMode(false),
     _currentVcoll(0.0f),
     _currentVpls(0.0f),
     _totalVreturn(0.0f),
	 _watchVin(0.0f),
     _minRunVolm(0.0f),
     _maxRunVolm(0.0f)
{
   copyOver(state);
}


//////////////////////////////////
PltBagEvac::~PltBagEvac()
{
   cleanup();
}


//////////////////////////////////
int PltBagEvac::transitionStatus ()
{

   if (!Software_CDS::GetInstance().getFeature(AiroutMitigation))
      return 1;

   ProcData pd;
   if (!pd.Config().Procedure.Get().key_air_removal)
   {
      DataLog(log_level_proc_info) << "No PLT evac key_air_removal is false -- skipping PltBagEvac" << endmsg;
      return 1;
   }


   if (_closePltBagvlave && pd.Status().CollectValve.IsReturning())
   {

      DataLog(log_level_proc_info) << "PLT evac completed normal" << endmsg;
      return 1;
   }


   if (_highPressMode && _highPressModeFinished)
   {

      DataLog(log_level_proc_info) << "PLT evac completed alternate. CPS = "  << pd.Status().CPS()  << endmsg;
      return 1;
   }

   return NO_TRANSITION;

}
////////////////////////////////////


int PltBagEvac::preProcess ()
{
   ProcData pd;

   if (_highPressMode)
   {


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


   }
   else
   {


      if (_closePltBagvlave)
      {
         pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
         return NORMAL;
      }


      float CurrentAps = pd.Status().APS();
      float DesiredAPS = myDesiredAPS;    // to be CC value in final
      float myQin      = myStartingQin;

      float GAIN       = myGAIN;   // to be CC value in final

      if (CurrentAps > DesiredAPS)
      {
         myQin =  _lastQin + ( fabs((DesiredAPS -  CurrentAps) / 20.0f) * GAIN );
      }
      else
      {
         if (CurrentAps < DesiredAPS)
         {
            myQin =  _lastQin - ( fabs((DesiredAPS -  CurrentAps) / 20.0f) * GAIN );
         }
         else
         {
            myQin = 90.0f;
         }
      }

      if (myQin > 140.0f)    // tobe CC value in final
         myQin = 140.0f;

      if (myQin < 1.0f)    // tobe CC value in final
         myQin = 1.0f;


      pd.Orders().Qin(myQin);
      pd.Orders().Qrp(myQret);

      PeriodicLog::forceOutput();

      _lastQin = myQin;
   }


   return NORMAL;
}
////////////////////////////////////////

int PltBagEvac::postProcess ()
{
   ProcData pd;
   float    CPS_Pressure = pd.Status().CPS() ;

   if ( CPS_Pressure > 1000.0f)
   {
      _highPressMode = true;
      pd.Orders().Qin(0.0f);
      pd.Orders().Qrp(0.0f);

      pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_COLLECT);
      pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   }


   if (_highPressMode)
   {


      _currentVpls  += pd.Status().PlasmaPump.dV();
      _currentVcoll += pd.Status().CollectPump.dV();

      if (
         (CPS_Pressure < 315.0f)
         || (_currentVpls  > 200.0f)
         || (_currentVcoll > 200.0f)
         )
      {
         _highPressModeFinished = true;
         pd.Orders().Qplasma(0.0f);
         pd.Orders().Qcollect(0.0f);
         pd.Orders().Qin(0.0f);
         pd.Orders().Qrp(0.0f);
      }


   }
   else
   {
      _currentVin   +=  pd.Status().InletPump.dV();
      _totalVreturn +=  pd.Status().ReturnPump.dV();

      if (_currentVin < _watchVin)
      {
         if (pd.Orders().Qin() < _minQin)
            _minQin = pd.Orders().Qin();

      }
      else if (_currentVin >=  _watchVin )
      {
         static int logged = 0;
         if (!logged)
         {
            logged = 1;
            DataLog(log_level_proc_info) << "Plt Bag Evac MinQin " << _minQin << " within the first " <<  _currentVin << endmsg;
         }

         if (  pd.Orders().Qin() <= (_minQin - myEndQinSpeed) )
         {

            DataLog(log_level_proc_info) << "Plt Bag Evac Transitioning at Qin " << pd.Orders().Qin() << endmsg;
            _closePltBagvlave = true;
         }

         if ( _currentVin >  _maxRunVolm)
         {

            DataLog(log_level_proc_info) << "Plt Bag Evac Vin max volume exceeded, Transitioning at Vin "
                                         << _currentVin << endmsg;
            _closePltBagvlave = true;
         }
      }


      if (_totalVreturn >  _maxRunVolm)
      {

         DataLog(log_level_proc_info) << "Plt Bag Evac exceeded max Vret="
                                      <<  _totalVreturn
                                      << ", Transitioning at Vin "
                                      << _currentVin
                                      << endmsg;
         _closePltBagvlave = true;
      }
   }
   PeriodicLog::forceOutput();

   return NORMAL;
}

//////////////////////////////////////
//////////////////////////////////////


int PltBagEvac::preEnter ()
{
   ProcData pd;

   bool     isBlackStamp =  (pd.TrimaSet().Cassette == cassette::RBCPLS);
   if (isBlackStamp)
   {
      pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_COLLECT);
      pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_OPEN);
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);

   }
   else
   {
      pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_COLLECT);
      pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_OPEN);
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_OPEN);

   }

   //
   // Make sure pumps are stopped
   //
   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qrp(0.0f);

   _totalVreturn = 0.0f; // reset

   myDesiredAPS  = CobeConfig::data().EvacuationDesiredAps;
   myGAIN        = CobeConfig::data().EvacuationGain;
   myEndQinSpeed = CobeConfig::data().EvacuationEndQinDeltaOffSlowestSpeed;
   myQret        = CobeConfig::data().EvacuationQreturnEvac;
   myStartingQin = CobeConfig::data().EvacuationStartingQinSpeed;
   _lastQin      = CobeConfig::data().EvacuationStartingQinSpeed;
   _watchVin     = CobeConfig::data().EvacuationMinQinWatchVolume;
   _maxRunVolm   = CobeConfig::data().EvacuationMaxPltEvacLimit;


   _minQin = 200.0f;


   return NORMAL;
}


//////////////////////////////////////////

int PltBagEvac::postExit ()
{
   ProcData pd;

   pd.MakeRunWritable();
   pd.Run().PlsBagEvacVin_done.Set(_currentVin);
   pd.MakeRunReadable();
   DataLog(log_level_proc_info) << "Plt Bag Evac Vin =  " << pd.Run().PlsBagEvacVin_done.Get() << endmsg;

   //
   // Make sure pumps are stopped
   //
   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qrp(0.0f);

   _closePltBagvlave      = false;
   _transitionNow         = false;
   _closePltBagvlave      = false;
   _transitionNow         = false;
   _currentVin            = 0.0f;
   _highPressModeFinished = false;
   _highPressMode         = false;
   _currentVcoll          = 0.0f;
   _currentVpls           = 0.0f;
   _totalVreturn          = 0.0f;
   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void PltBagEvac::copyOver (const PltBagEvac&)
{}

void PltBagEvac::cleanup ()
{}

/////////////////////////////////////////////////////////////////////////////

/* FORMAT HASH 743e6d092c8b7b5f516ccc36b57e8191 */

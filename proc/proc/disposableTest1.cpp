/*
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      DisposableTest1.cpp
 *
 */

/*
$Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/proc/proc/rcs/disposableTest1.cpp 1.1 2007/02/20 22:10:04Z jheiusb Exp jheiusb $
$Log: disposableTest1.cpp $
*/

#include "disposableTest1.h"
#include "states.h"
#include "procdata.h"

#include "CatalogChecker.h"

#include "cobeconfig.h"

DEFINE_STATE(DisposableTest1);

DisposableTest1::DisposableTest1()
   : StateAbs(),
     useAirMitigation(false),
     whiteStamp(false)
{}



DisposableTest1::DisposableTest1(const DisposableTest1& state)
   : StateAbs(state),
     useAirMitigation(false)
{

   copyOver(state);
}

DisposableTest1::~DisposableTest1()
{
   cleanup();
}

int DisposableTest1::preProcess ()
{
   if (useAirMitigation)
   {
      ProcData pd;
      if ((bool)CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::PLASMA_BAG))
      {
         ProcData pd;
         pd.MakeRunWritable();

         if (whiteStamp)
            pd.Run().PlsBagEvacVc.Set(pd.Run().PlsBagEvacVc.Get()   + pd.Status().CollectPump.dV() );
         else
            pd.Run().PlsBagEvacVpls.Set(pd.Run().PlsBagEvacVpls.Get() + pd.Status().PlasmaPump.dV()  );


      }
   }

   return NORMAL;
}

int DisposableTest1::postProcess ()
{

   if (useAirMitigation)
   {
      ProcData pd;
      if ((bool)CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::PLASMA_BAG))
      {

         if (pd.Config().Procedure.Get().key_air_removal)
         {

            if (whiteStamp)
            {

               if (pd.Run().PlsBagEvacVc.Get() < CobeConfig::data().EvacuationPlsEvacVc  + pd.Run().PlsBagEvacVin_done.Get() )
               {
                  pd.Orders().Qcollect(140.0f);
               }
               else
               {
                  pd.Orders().Qcollect(0.0f);
               }

            }
            else
            {

               if (pd.Run().PlsBagEvacVpls.Get() < CobeConfig::data().EvacuationBlackplsExtra + pd.Run().PlsBagEvacVin_done.Get())
                  pd.Orders().Qplasma(140.0f);
               else
                  pd.Orders().Qplasma(0.0f);

            }

         }

      }

   }
   return NORMAL;
}

int DisposableTest1::transitionStatus ()
{
   // no transition here ... msg in state table transitions
   return NO_TRANSITION;
}

int DisposableTest1::preEnter ()
{

   ProcData pd;
   whiteStamp =  (bool)(pd.TrimaSet().Cassette == cassette::PLTPLSRBC);

   if (Software_CDS::GetInstance().getFeature(AiroutMitigation)) useAirMitigation = true;

   if (useAirMitigation)
   {
      ProcData pd;
      if ((bool)CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::PLASMA_BAG))
      {
         if (pd.Config().Procedure.Get().key_air_removal)
         {
            if (whiteStamp)
            {
               pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_COLLECT);
               pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_OPEN);
               pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
            }
            else
            {

               pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_COLLECT);
               pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);
               pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
            }
         }

      }
   }

   return NORMAL;
}

int DisposableTest1::postExit ()
{

   if (useAirMitigation)
   {
      ProcData pd;
      if ((bool)CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::PLASMA_BAG))
      {
         if (whiteStamp)
            pd.Orders().Qcollect(0.0f);
         else
            pd.Orders().Qplasma(0.0f);
      }
   }

   return NORMAL;
}

void DisposableTest1::copyOver (const DisposableTest1&)
{}

void DisposableTest1::cleanup ()
{}

/* FORMAT HASH 17c101f48994326ad85fe80fc567c607 */

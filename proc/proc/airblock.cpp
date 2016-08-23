/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      airblock.cpp
 *
 */

#include "airblock.h"
#include "substatemap.h"
#include "cobeconfig.h"
#include "procdata.h"

DEFINE_OBJ(AirBlock);

AirBlock::AirBlock()
   : RecoveryAbs(),
     _AirBlockRecoveryRequest(Callback<AirBlock>(this, &AirBlock::requestReceived) ),
     _RequestPending(0),
     _AirBlockCleared(1),
     _PlasmaProcessed(0.0f)
{}

AirBlock::AirBlock(const AirBlock& rec)
   : RecoveryAbs(rec),
     _AirBlockRecoveryRequest(Callback<AirBlock>(this, &AirBlock::requestReceived) ),
     _RequestPending(0),
     _AirBlockCleared(rec._AirBlockCleared),
     _PlasmaProcessed(rec._PlasmaProcessed)
{
   copyOver(rec);
}

AirBlock::~AirBlock()
{
   cleanup();
}

int AirBlock::init ()
{

   return 1;
}

int AirBlock::doRecovery ()
{
   ProcData pd;

   //
   // We are doing the recovery, so the request is
   //  no longer pending ...
   //
   _RequestPending = 0;

   //
   // Command the centrifuge to Zero
   //
   pd.Orders().RPM(CobeConfig::data().AirBlockCentrifugeRPM);


   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);
   // DataLog( log_level_proc_recovery_info ) << "Closed Plasma Valve" << endmsg;

   //
   // Update plasma volume ...
   //
   _PlasmaProcessed += pd.Status().PlasmaPump.dV();

   if ( pd.Status().CentrifugeRPM() < CobeConfig::data().AirBlockStartRPMTarget )
   {
      // DataLog( log_level_proc_recovery_info ) << "Centrifuge less than Target" << endmsg;
      pd.Orders().Qplasma(CobeConfig::data().AirBlockQPlasma);   // mls/min

      //
      // Only if a platelet set
      //
      if ( pd.Status().CassIsPltPlsRBC() )
      {
         //
         // Set the chamber flow which will be affected by the
         //  plasma valve state ...
         if ( pd.Status().PlasmaValve.State() == HAL_VALVE_RETURN )
         {
            pd.Orders().Qcollect(CobeConfig::data().AirBlockQPlasma +
                                 CobeConfig::data().AirBlockQChamber);   // mls/min
         }
         else
         {
            pd.Orders().Qcollect(CobeConfig::data().AirBlockQChamber);   // mls/min
         }
      }

      //
      // Run the rest of the pumps at QinAvg of 45mls/min flows
      //
      pd.Orders().Qin(pd.Predict().Qin(pd.Run().DrawCycle.Get(), SubstateMap::alg(NOCOLLECT_STATE)));
      pd.Orders().Qac(pd.Predict().Qac(pd.Run().DrawCycle.Get(), SubstateMap::alg(NOCOLLECT_STATE)));
      pd.Orders().Qrp(pd.Predict().Qrp(pd.Run().DrawCycle.Get(), SubstateMap::alg(NOCOLLECT_STATE)));
      //
      // When we have processed 2.0 mls of plasma we are
      //  finished ...
      //
      if ( _PlasmaProcessed >= CobeConfig::data().AirBlockVPlasmaTarget )
      {
         _AirBlockCleared = 1;
         DataLog(log_level_proc_recovery_info) << "AirBlock cleared" << endmsg;
      }

   }
   else
   {
      pd.Orders().Qin(0.0f);
      pd.Orders().Qac(0.0f);
      pd.Orders().Qrp(0.0f);
      pd.Orders().Qplasma(0.0f);
      pd.Orders().Qcollect(0.0f);
   }

   return 1;
}

int AirBlock::mayEnter () const
{
   return 1;
}

void AirBlock::requestReceived ()
{
   //
   // Set the flag showing a request was received
   _RequestPending  = 1;
   _AirBlockCleared = 0;
}

int AirBlock::shouldRecover ()
{


   static bool logged = false;
   if ( _RequestPending ||
        ( active() && !_AirBlockCleared )
        )
   {

      if (!logged)
      {
         DataLog(log_level_proc_recovery_info) << "AirBlock shouldRecover" << endmsg;
         logged = true;
      }


      return 1;
   }

   //
   // Reset the plasma processed volume
   //
   _PlasmaProcessed = 0.0f;
   _AirBlockCleared = 1;
   logged           = false;

   return 0;
}

void AirBlock::copyOver (const AirBlock&)
{}

void AirBlock::cleanup ()
{}

/* FORMAT HASH 5f1cb179962ffd37079d920fe313ea7d */

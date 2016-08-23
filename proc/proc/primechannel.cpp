/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeChannel.cpp
 *
 */

#include <vxworks.h>
#include "primechannel.h"
#include "limitprimeratio.h"
#include "procdata.h"
#include "cobeconfig.h"


DEFINE_STATE(PrimeChannel);

PrimeChannel::PrimeChannel()
   : StateAbs(),
     _CustomRatio(0.0f)
{}

PrimeChannel::PrimeChannel(const PrimeChannel& state)
   : StateAbs(state),
     _CustomRatio(state._CustomRatio)
{}

PrimeChannel::~PrimeChannel()
{}

int PrimeChannel::transitionStatus ()
{
   ProcData pd;

   float    vol = pd.Volumes().VinTotal.Get();
   if ( vol > CobeConfig::data().VinPrimeChannel )
   {
      DataLog(log_level_proc_debug) << "VinTotal ( " << vol << " ) > "
                                    << CobeConfig::data().VinPrimeChannel  << endmsg;

      return 1;
   }
   DataLog(log_level_proc_debug) << "VinTotal ( " << vol << " ) <= "
                                 << CobeConfig::data().VinPrimeChannel  << endmsg;

   return NO_TRANSITION;
}

int PrimeChannel::preProcess ()
{
   ProcData pd;

   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);



   //
   //
   //  Calculate max inlet flow as lesser of Cobe config or adjustment value
   //
   float qin          = pd.Adjustments().Draw.CurrentCap.Get();
   if (qin > CobeConfig::data().QinPrimeChannel) qin = CobeConfig::data().QinPrimeChannel;
   float adjustFactor = qin / CobeConfig::data().QinPrimeChannel;

   float ratio        = LimitPrimeRatio(pd.Config().Procedure.Get().key_custom_ratio,
                                        pd.Donor().Hct.Get());
   if ( ratio != _CustomRatio )
   {
      //
      // Log the priming ratio ...
      DataLog(log_level_proc_blood_run_info) << "Custom ratio: " << ratio << endmsg;
      _CustomRatio = ratio;
   }


   float qcol = CobeConfig::data().QcolPrimeChannel;
   if (pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC) qcol = 0.0f;

   pd.Orders().Qin(qin);
   pd.Orders().Qac(qin / ratio);
   pd.Orders().Qrp(0.0f);
   if ( pd.TrimaSet().Cassette.Get() == cassette::PLTPLSRBC )
      pd.Orders().Qplasma(CobeConfig::data().QpPrimeChannelPltPlsRBC * adjustFactor);
   else
      pd.Orders().Qplasma(CobeConfig::data().QpPrimeChannelRBCPls * adjustFactor);
   pd.Orders().Qcollect(qcol * adjustFactor);
   pd.Orders().RPM(CobeConfig::data().RPMPrimeChannel);

   //
   // Set the centrifuge pressure limit ...
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CentPresLimit.Set(CobeConfig::data().CentPresLimitHigh);

   return NORMAL;
}

int PrimeChannel::postProcess ()
{
   return NORMAL;
}

int PrimeChannel::preEnter ()
{
   return NORMAL;
}

int PrimeChannel::postExit ()
{
   //
   // Place action here that will occur after the state decides to transition ...
   //
   return NORMAL;
}

void PrimeChannel::copyOver (const PrimeChannel&)
{}

void PrimeChannel::cleanup ()
{
   //
   // Delete the dynamic memory ...
   //
}

/* FORMAT HASH 619ff665b813cb92dd94f5ebd23ada43 */

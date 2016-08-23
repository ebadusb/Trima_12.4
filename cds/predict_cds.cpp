#include <vxWorks.h>
#include <cstring>

#ifndef WIN32
#pragma implementation "predict_cds.h"
#endif

#include "predict_cds.h"

Predict_CDS::Predict_CDS(Role role)
   : MultWriteDataStore ("Predict_CDS", role)
{
   // Base Element Registers
   SubstateInfo.Register      (this, PFR_RECOVER);
   VinTarget.Register         (this, PFR_RECOVER, 0.0f);
   VinPIRStart.Register       (this, PFR_RECOVER, 0.0f);
   MaxPIRQc.Register          (this, PFR_RECOVER, 0.0f);
   Ydch.Register              (this, PFR_RECOVER, 0.0f);
   YdchPIR.Register           (this, PFR_RECOVER, 0.0f);
   RBCVsvn.Register           (this, PFR_RECOVER, 0.0f);
   accumulatedYdpur.Register  (this, PFR_RECOVER, 0.0f);
   accumulatedPurges.Register (this, PFR_RECOVER, 0);
   maxDonorTime.Register      (this, PFR_RECOVER, 150);
   PTFReduceFlows.Register    (this, PFR_RECOVER, false);
   lastCalcQchMax.Register    (this, PFR_RECOVER, 0.0f);
   VinNextPurge.Register      (this, PFR_RECOVER, 0.0f);
   TimeNextPurge.Register     (this, PFR_RECOVER, 0.0f);
   AccumulatedPurgeTime.Register (this, PFR_RECOVER, 0.0f);
   PurgeEndTime.Register      (this, NO_PFR, 0.0f); // if we get a power fail we do not continue a purge
   PIRLockin.Register         (this, PFR_RECOVER, 0.0f);

   NoCollectFirstReturnOnly.Register (this, PFR_RECOVER);

   PirCsf.Register            (this, PFR_RECOVER, 1.0f);
   PirQinLimit.Register         (this, PFR_RECOVER, 0.0f);
   OptimizingSelProc.Register   (this, PFR_RECOVER, false);
   Qcmr.Register                (this, PFR_RECOVER, 0.0f);
   PlateletPlasmaQch.Register   (this, PFR_RECOVER, 0.0f);
   LimitingMidrunQrp.Register   (this, PFR_RECOVER, 0.0f);
   PrePirQch.Register           (this, PFR_RECOVER, 0.0f);

   VinPurge.Register            (this, PFR_RECOVER, 0.0f);     // Inlet volume target for WBC purging of the chamber
   VinDeltaPurge.Register       (this, PFR_RECOVER, 0.0f);     // The delta for the next purge according to VIN


}

Predict_CDS::~Predict_CDS()
{}

/* FORMAT HASH 5b3feae0a7d9919b5be55d75b4b73ce5 */

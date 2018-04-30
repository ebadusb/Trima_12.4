/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: H:/BCT_Development/Trima5.R/Trima/cds/rcs/procpqi_cds.cpp 1.8 2007/08/15 22:43:21Z dslausb Exp jheiusb $
 * $Log: procpqi_cds.cpp $
 * Revision 1.8  2007/08/15 22:43:21Z  dslausb
 * Separate flags for two RBC products
 * Revision 1.7  2007/01/16 16:04:51Z  dslausb
 * IT 7482 - Add RBC leukoreduction message and flag
 * Revision 1.6  2006/03/03 17:26:27Z  jheiusb
 * initial merge
 * Revision 1.5  2003/11/19 15:36:08Z  rm70006
 * IT 6584.  Replace int's with booleans.
 * Revision 1.4  2003/08/11 15:28:12Z  ms10234
 * 5838 - changes for the chamber saturation algorithm
 * Revision 1.3  2003/05/08 12:49:27Z  sb07663
 * Modifications to work offline (checked in under IT5853)
 * Revision 1.2  2003/04/17 19:37:04Z  pn02526
 * Add PFR fields for monitors.
 * Revision 1.1  2002/11/18 11:30:43  jl11312
 * Initial revision
 *
 */

#include <vxWorks.h>

#ifndef WIN32
#pragma implementation "procpqi_cds.h"
#endif

#include "procpqi_cds.h"

ProcPQI_CDS::ProcPQI_CDS (Role role)
   : SingleWriteDataStore ("ProcPQI_CDS", role)
{
   // First, build an initialized flags struct.
   ProductQualityData PQLocal;

   PQLocal.verify_platelet_wbc_reasons.Initialize();
   PQLocal.verify_platelet_yield_volume_reasons.Initialize();
   PQLocal.platelet_concentration_reasons.Initialize();
   PQLocal.label_platelets_leukoreduced = false;

   PQLocal.verify_plasma_wbc_reasons.Initialize();
   PQLocal.verify_plasma_volume_reasons.Initialize();
   PQLocal.label_plasma_leukoreduced = false;

   PQLocal.verify_rbc_volume_reasons.Initialize();
   PQLocal.verify_rbc_volume_reasons_prod2.Initialize();
   PQLocal.verify_rbc_wbc_reasons.Initialize();
   PQLocal.verify_rbc_wbc_reasons_prod2.Initialize();
   PQLocal.verify_rbc_quality_reasons.Initialize();
   PQLocal.verify_rbc_quality_reasons_prod2.Initialize();
   PQLocal.label_rbcs_leukoreduced       = false;
   PQLocal.label_rbcs_leukoreduced_prod2 = false;

   PQdata.Register                  (this, PFR_RECOVER, PQLocal);
   rbcSpilloverDetection.Register   (this, PFR_RECOVER, false);
   rbcSpilloverRecovery.Register    (this, PFR_RECOVER, false);
   valveErrorDetection.Register     (this, PFR_RECOVER, false);  // A Valve Error alarm occurred.
   RBCDetectorError.Register        (this, PFR_RECOVER, false);  // An RBC Detector Error occurred.
   PlateletsWereCollected.Register  (this, PFR_RECOVER, false);
   PlasmaWasCollected.Register      (this, PFR_RECOVER, false);
   RbcWereCollected.Register        (this, PFR_RECOVER, false);
   wasInPIR.Register                (this, PFR_RECOVER, false); // Flag that tracks whether PIR ever occurred.
   spoofChamberSat.Register         (this, NO_PFR, 0);
   ClampClosureFailedFullFailure.Register      (this, PFR_RECOVER, false);
   ClampClosureFailedPartFailure.Register      (this, PFR_RECOVER, false);
   VinEndedPIR.Register             (this, PFR_RECOVER, 0.0f);
   VinTargetEndPIR.Register         (this, PFR_RECOVER, 0.0f);
   previousDonorHct.Register        (this, PFR_RECOVER, 0.0f);
}

ProcPQI_CDS::~ProcPQI_CDS()
{}

/* FORMAT HASH 994585039e2e2e15cce03f8a6fcfca28 */

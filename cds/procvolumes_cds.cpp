/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: H:/BCT_Development/Trima5.R/Trima/cds/rcs/procvolumes_cds.cpp 1.18 2008/11/25 23:58:14Z jheiusb Exp jheiusb $
 * $Log: procvolumes_cds.cpp $
 * Revision 1.18  2008/11/25 23:58:14Z  jheiusb
 * -- add running return pump volume to dlog
 * Revision 1.17  2008/10/16 14:57:54Z  ssunusb
 * IT 6933 - Race condition between predict and rtplateletyield resulting in incorrect yield display
 * Revision 1.16  2008/09/10 18:49:55Z  jheiusb
 * 8845 -- fix cds problems
 * Revision 1.15  2008/09/08 15:50:09Z  jheiusb
 * -- fix global saline rinseback data
 * Revision 1.14  2008/08/19 15:47:38Z  jheiusb
 * 7073 --track saline used in SAL RB
 * Revision 1.13  2008/08/04 18:32:04Z  jheiusb
 * 7973 -- volumes for Saline RB
 * Revision 1.12  2007/11/30 17:04:32Z  jheiusb
 * 8360 -- fix the runsummary in MSS
 * Revision 1.11  2007/08/10 18:50:28Z  jheiusb
 * 8098 add the per product SS for RBC's
 * Revision 1.10  2007/07/31 20:33:20Z  jheiusb
 * 7927 -- CDS variable to hold the RBC product finals for endrun summary screens.
 * Revision 1.9  2007/04/16 22:00:53Z  jheiusb
 * 6174 -- add PTF and double fields
 * Revision 1.8  2007/02/16 23:21:49Z  rm70006
 * IT 7849.  Add new cds variables for PTF
 * Revision 1.7  2006/03/06 21:55:09Z  jheiusb
 * Revision 1.6  2006/03/03 21:24:26Z  jheiusb
 * Revision 1.5  2006/03/03 17:30:50Z  dslausb
 * Revision 1.4  2003/05/09 20:03:09Z  ms10234
 * 3834 - Added plasma rinseback variables.
 * Revision 1.3  2003/04/29 16:10:10Z  ms10234
 * 5799 - renamed the ac at the needle variable.
 * Revision 1.2  2002/12/11 18:35:08Z  sb07663
 * conditional non-win32 compilation of pragma interface directives
 * Revision 1.1  2002/11/18 18:30:40Z  jl11312
 * Initial revision
 *
 */

#include <vxWorks.h>

#ifndef WIN32
#pragma implementation "procvolumes_cds.h"
#endif

#include "procvolumes_cds.h"

ProcVolumes_CDS::ProcVolumes_CDS (Role role)
   : SingleWriteDataStore ("ProcVolumes_CDS", role)
{
   //
   // Product information
   //
   //   PLATELET BAG
   PlateletYield.Register      (this, PFR_RECOVER, 0.0f);
   PreviousPlateletYield.Register      (this, PFR_RECOVER, 0.0f);
   PlateletVolume.Register     (this, PFR_RECOVER, 0.0f);
   VacPlateletBag.Register     (this, PFR_RECOVER, 0.0f);
   VstoragePltBag.Register     (this, PFR_RECOVER, 0.0f);

   //   PLASMA BAG
   PlasmaVolume.Register       (this, PFR_RECOVER, 0.0f);
   VacPlasmaBag.Register       (this, PFR_RECOVER, 0.0f);

   //   RBC BAG
   RBCVolume.Register          (this, PFR_RECOVER, 0.0f);
   RBCDose.Register            (this, PFR_RECOVER, 0.0f);

   RBCDoubleDose.Register      (this, PFR_RECOVER, 0.0f);
   RBCDoubleVolume.Register    (this, PFR_RECOVER, 0.0f);

   RBCSetup1Volume.Register    (this, PFR_RECOVER, 0.0f);
   RBCSetup2Volume.Register    (this, PFR_RECOVER, 0.0f);

   RBCPrimeVolume.Register     (this, PFR_RECOVER, 0.0f);
   VacRBCBag.Register          (this, PFR_RECOVER, 0.0f);
   VstorageRBCBag.Register     (this, PFR_RECOVER, 0.0f);


   RBCSetup1DoubleVolume.Register    (this, PFR_RECOVER, 0.0f);
   RBCSetup2DoubleVolume.Register    (this, PFR_RECOVER, 0.0f);


   RBCProduct_A_FinalDose.Register   (this, PFR_RECOVER, 0.0f);
   RBCProduct_A_FinalVolume.Register (this, PFR_RECOVER, 0.0f);
   RBCProduct_A_FinalVac.Register    (this, PFR_RECOVER, 0.0f);
   RBCProduct_A_FinalVss.Register    (this, PFR_RECOVER, 0.0f);

   RBCProduct_B_FinalDose.Register   (this, PFR_RECOVER, 0.0f);
   RBCProduct_B_FinalVolume.Register (this, PFR_RECOVER, 0.0f);
   RBCProduct_B_FinalVac.Register    (this, PFR_RECOVER, 0.0f);
   RBCProduct_B_FinalVss.Register    (this, PFR_RECOVER, 0.0f);
   //
   // Pumped volume information
   //
   Vin.Register                  (this, PFR_RECOVER, 0.0f);
   VinTotal.Register             (this, PFR_RECOVER, 0.0f);
   Vac.Register                  (this, PFR_RECOVER, 0.0f);
   VacTotal.Register             (this, PFR_RECOVER, 0.0f);
   VacTotal_tmp.Register        (this, PFR_RECOVER, 0.0f);



   VnonACBloodAtNeedle.Register  (this, PFR_RECOVER, 0.0f);
   Vp.Register                   (this, PFR_RECOVER, 0.0f);
   Vc.Register                   (this, PFR_RECOVER, 0.0f);
   Vreturn.Register              (this, PFR_RECOVER, 0.0f);
   VReplacement.Register         (this, PFR_RECOVER, 0.0f);
   VSalineBolus.Register         (this, PFR_RECOVER, 0.0f);
   VBP.Register                  (this, PFR_RECOVER, 0.0f);
   VBPPlatelet.Register          (this, PFR_RECOVER, 0.0f);
   VpltStorageRemaining.Register (this, PFR_RECOVER, 0.0f);
   VrbcStorageRemaining.Register (this, PFR_RECOVER, 0.0f);

   //
   // Volume information based on state
   Vincv.Register              (this, PFR_RECOVER, 0.0f);
   TimeOfVincv.Register        (this, PFR_RECOVER, 0.0f);
   VinCurrentSubstate.Register (this, PFR_RECOVER, 0.0f);
   VinEPCAStart.Register       (this, PFR_RECOVER, 0.0f);
   VinPIRStart.Register        (this, PFR_RECOVER, 0.0f);
   VMaxPltRinseback.Register   (this, PFR_RECOVER, 0.0f);
   VRetRinseback.Register      (this, PFR_RECOVER, 0.0f);

   VRinsePurge.Register        (this, PFR_RECOVER, 0.0f);
   VRinseRecirc.Register       (this, PFR_RECOVER, 0.0f);
   VRinseRes.Register          (this, PFR_RECOVER, 0.0f);
   VRinseSpill.Register        (this, PFR_RECOVER, 0.0f);
   Vcrb1.Register              (this, PFR_RECOVER, 0.0f);
   Vcrinse.Register            (this, PFR_RECOVER, 0.0f);
   VRecycled.Register          (this, NO_PFR, 0.0f);

   // Capacities
   VpltBagCapacity.Register (this, PFR_RECOVER, 0.0f);
   VrbcBagCapacity.Register (this, PFR_RECOVER, 0.0f);



   Vres.Register                                (this, PFR_RECOVER, 0.0f);
   VacRes.Register                              (this, PFR_RECOVER, 0.0f);
   VMaxSalRinseback.Register                    (this, PFR_RECOVER, 0.0f);
   VRetDuringSalineRinsebackRinseback.Register  (this, PFR_RECOVER, 0.0f);
   VSalineUsedinRinseback.Register              (this, PFR_RECOVER, 0.0f);


   // this is for tracking return volume for dlog analysis. this CDS value
   // is not used in proc calulation and should not be
   // this value is not PFR persistant:
   ReturnPumpTotalVolume.Register               (this, NO_PFR, 0.0f);

   TotalPASVolumePumped.Register                (this, PFR_RECOVER, 0.0f);
   TotalRASVolumePumped.Register                (this, PFR_RECOVER, 0.0f);
   Vreturn_for_residuals_calc.Register          (this, PFR_RECOVER, 0.0f);
}

ProcVolumes_CDS::~ProcVolumes_CDS()
{}

/* FORMAT HASH bbc92de3fce9b3e99d8651705179b89a */

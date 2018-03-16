/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      procvolumes_cds.h
 *
 */

#ifndef __PROCVOLUMES_CDS
#define __PROCVOLUMES_CDS

#include <time.h>
#include "datastore.h"

#ifndef WIN32
#pragma interface
#endif

class ProcVolumes_CDS
   : public SingleWriteDataStore
{

public:

   BaseElement<float> PlateletYield;                            // in platelets, eg 3.0e11
   BaseElement<float> PreviousPlateletYield;                    // in platelets, eg 3.0e11. Used in rtplateletyield.cpp
   BaseElement<float> PlateletVolume;                           // including AC
   BaseElement<float> VacPlateletBag;
   BaseElement<float> PlasmaVolume;                             // including AC
   BaseElement<float> VacPlasmaBag;
   BaseElement<float> RBCVolume;                                // including AC and plasma
   BaseElement<float> RBCDose;                                  // packed RBCs only
   BaseElement<float> RBCSetup1Volume;                          // Vin only
   BaseElement<float> RBCSetup2Volume;                          // Vin only
   BaseElement<float> RBCPrimeVolume;

   BaseElement<float> RBCSetup1DoubleVolume;                    // Vin only
   BaseElement<float> RBCSetup2DoubleVolume;                    // Vin only

   BaseElement<float> RBCDoubleVolume;
   BaseElement<float> RBCDoubleDose;

   // for reporting the individual double values:
   BaseElement<float> RBCProduct_A_FinalDose;
   BaseElement<float> RBCProduct_A_FinalVolume;
   BaseElement<float> RBCProduct_A_FinalVac;
   BaseElement<float> RBCProduct_A_FinalVss;                    // RAS volm

   BaseElement<float> RBCProduct_B_FinalDose;
   BaseElement<float> RBCProduct_B_FinalVolume;
   BaseElement<float> RBCProduct_B_FinalVac;
   BaseElement<float> RBCProduct_B_FinalVss;                    // RAS volm

   BaseElement<float> VacRBCBag;

   BaseElement<float> VstorageRBCBag;                           // Metered Solution vol for RBC product
   BaseElement<float> VstoragePltBag;                           // Metered Solution vol for Plt product

   BaseElement<float> Vin;                                      // Inlet vol during draw cycle - recycle volume from return pump, excluding  recoveries during draw, etc.
   BaseElement<float> VinTotal;                                 // Total Inlet pump volume
   BaseElement<float> Vac;
   BaseElement<float> VacTotal;

   BaseElement<float> VacTotal_tmp;                  // used to keep the runs actual ACTotal value while RAS runs the AC pump
                                                     // only valid for use during metering

   BaseElement<float> VnonACBloodAtNeedle;
   BaseElement<float> Vp;
   BaseElement<float> Vc;
   BaseElement<float> Vreturn;
   BaseElement<float> VReplacement;                        // Replacement fluid pumped to donor
   BaseElement<float> VSalineBolus;
   BaseElement<float> VBP;
   BaseElement<float> VBPPlatelet;

   BaseElement<float> Vincv;                        // Vin at collect valve open
   BaseElement<float> TimeOfVincv;                  // procedure time at collect valve open
   BaseElement<float> VinCurrentSubstate;
   BaseElement<float> VinEPCAStart;                   // Vin at start of EPCA
   BaseElement<float> VinPIRStart;                    // Vin at start of PIR
   BaseElement<float> VMaxPltRinseback;
   BaseElement<float> VRetRinseback;                       // tracks return to donor volume for any RB
   BaseElement<float> VRinsePurge;
   BaseElement<float> VRinseRecirc;
   BaseElement<float> VRinseRes;
   BaseElement<float> VRinseSpill;
   BaseElement<float> Vcrb1;
   BaseElement<float> Vcrinse;
   BaseElement<float> VRecycled;

   BaseElement<float> VpltStorageRemaining;                    // Remaining storage solution bag volume
   BaseElement<float> VrbcStorageRemaining;                    // Remaining storage solution bag volume

   BaseElement<float> VpltBagCapacity;                         // Capacity of storage solution bag
   BaseElement<float> VrbcBagCapacity;                         // Capacity of storage solution bag


   BaseElement< float > Vres;
   BaseElement< float > VacRes;
   BaseElement< float > VMaxSalRinseback;
   BaseElement< float > VRetDuringSalineRinsebackRinseback; // tracks return to donor volume
   BaseElement< float > VSalineUsedinRinseback;             // tracks saline in the inseback

   BaseElement< float > ReturnPumpTotalVolume;     // just for the dlog calcs, not used by proc.

   BaseElement< float > TotalPASVolumePumped;
   BaseElement< float > TotalRASVolumePumped;

   BaseElement< float > Vreturn_for_residuals_calc; // Vreturn set to zero in donor disconnect so save this for residual blood calc.

// Class Methods
public:

   ProcVolumes_CDS (Role role);
   virtual ~ProcVolumes_CDS();

   // Formatter function to have common conversion for logging, Vista, and GUI.
   int BloodVolumeProcessed () { return (int)(VBP.Get()); }

private:
   ProcVolumes_CDS();  // Base Constructor not available
};

#endif

/* FORMAT HASH de1615e84123d6bd857e185f6446501b */

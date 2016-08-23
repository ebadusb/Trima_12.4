/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      procruntargets_cds.h
 *
 */

#ifndef __PROCRUNTARGETS_CDS_H_
#define __PROCRUNTARGETS_CDS_H_

#include "datastore.h"
#include "configdef.h"

#ifndef WIN32
#pragma interface
#endif

class ProcRunTargets_CDS
   : public SingleWriteDataStore
{
public:

   //
   // Procedure targets are associated with the specified ProcNumber
   //
   BaseElement<int>          ProcNumber;
   BaseElement<float>        ProcTimeTarget;                      // Current Procedure Time

   BaseElement<float>        PlateletYieldTarget;                 // Predicted platelet target
   BaseElement<float>        PlateletVolumeTarget;                // Predicted platelet target
   BaseElement<float>        PlateletConcenTarget;                // Predicted platelet concentration
   BaseElement<AMAPPLTTYPES> PlateletAmapSizeTarget;              // Predicted platelet amap size



   BaseElement<float> PlasmaTarget;                               // Plasma volume target
   BaseElement<float> RbcVolumeTarget;                            // RBC product volume target
   BaseElement<float> RbcDoseTarget;                              // RBC dose target
   BaseElement<float> RbcVsvnTarget;                              // Theoretical Vsvn for RBC collection state
   BaseElement<float> RbcCritTarget;                              // RBC post hematocrit
   BaseElement<float> InfusionRateTarget;                         // Infusion rate

   BaseElement<float> SelectedPlateletYield;                      // Selected platelet target yield associated with procNumber
   BaseElement<float> SelectedPlateletVolume;                     // Selected platelet target volume associated with procNumber
   BaseElement<float> SelectedPlateletConcen;                     // Selected platelet concentration associated with procNumber
   BaseElement<float> SelectedPlasmaVolume;                       // Selected plasma target volume associated with procNumber
   BaseElement<float> SelectedRbcVolume;                          // Selected RBC target volume associated with procNumber

   BaseElement<bool>  RequiresRepFluid;                           // selected procedure requires the use
                                                                  //  of replacement fluid

   BaseElement<float> RBCSolutionTarget;                          // Target Solution addition in mls
   BaseElement<float> PltSolutionTarget;
   BaseElement<float> PltPctCarryover;

   BaseElement<float> OriginalYeild;                        // needed to recalulate the SS if you switch to a non-plt procedure
   BaseElement<float> OriginalVolume;                       //   in the middle of PLT collect.
   //
   // Helper functions
   //

   // Set variants (different plumbing and filter options)
   BaseElement<bool> MeterPlatelets;                    // Meter platelets after collection
   BaseElement<bool> MeterRbcs;                         // Meter platelets after collection;

// Class Methods
public:

   ProcRunTargets_CDS (Role role);
   virtual ~ProcRunTargets_CDS();

private:
   ProcRunTargets_CDS();  // Base Constructor not available
};

#endif

/* FORMAT HASH d000e7d5581a3e4dfead9b4ec8e8b462 */

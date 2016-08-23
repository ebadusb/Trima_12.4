/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      proctrimaset_cds.h
 *
 */

#ifndef __PROCTRIMASET_CDS
#define __PROCTRIMASET_CDS

#include "datastore.h"
#include "cassette.h"

#ifndef WIN32
#pragma interface
#endif

class ProcTrimaSet_CDS
   : public SingleWriteDataStore
{
public:

   // Current State of the cassette
   enum CassetteStateType
   {
      DRY,             // before AC prime
      TESTED,          // at start of pressure test
      AC,              // AC connected
      ACPRIMED,        // After AC primed
      BLOOD,           // At donor connected
      BLOODPRIME,      // At Blood prime
      PRIMED,          // After Blood prime
      RINSEBACK,       // After Blood run
      USED,            // After rinseback
      STORAGE,         // During Metered Storage ( needs to go after used for PFR)
      STORAGE_COMPLETE // After Metered Storage Completes, but before disposables removed (needed for PFR)
   };

public:

   BaseElement<cassette::cassetteType> Cassette;

   BaseElement<CassetteStateType>      CassetteState;

   BaseElement<float>                  CentPresLimit;

   BaseElement<unsigned char>          RBCRedDriveValue;

   BaseElement<unsigned char>          RBCGreenDriveValue;

   BaseElement<bool>                   RBCDetectorCalibrated;

   BaseElement<float>                  APSZero;

   BaseElement<float>                  CmdQch;
   BaseElement<float>                  ActQch;

   // Used on PTF sets
   BaseElement<bool>  PTFCPSTestPassed;
   BaseElement<float> CPSZero;

public:

   ProcTrimaSet_CDS(Role role);
   virtual ~ProcTrimaSet_CDS();

private:
   ProcTrimaSet_CDS(); // Base Constructor not available
};

#endif

/* FORMAT HASH b6700d0236830c3783f2b313c084aa21 */

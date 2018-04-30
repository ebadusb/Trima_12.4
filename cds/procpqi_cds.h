#ifndef __PROCPQI_CDS
#define __PROCPQI_CDS

#ifndef WIN32
#pragma interface
#endif

#include "datastore.h"
#include "productqualitydata.h"

//
//   Procedure common data store:  This class was created to hold
//   the product quality monitor data.
//
class ProcPQI_CDS
   : public SingleWriteDataStore
{

public:

   BaseElement<ProductQualityData> PQdata;

   BaseElement<bool>               rbcSpilloverDetection;

   BaseElement<bool>               rbcSpilloverRecovery;

   BaseElement<bool>               valveErrorDetection; // A Valve Error alarm occurred.

   BaseElement<bool>               RBCDetectorError; // An RBC Detector Error occurred.

   BaseElement<bool>               PlateletsWereCollected;

   BaseElement<bool>               PlasmaWasCollected;

   BaseElement<bool>               RbcWereCollected;

   BaseElement<bool>               wasInPIR; // Flag that tracks whether PIR ever occurred.

   // MSS clamp closure test failed and will flag with wbc.
   BaseElement<bool> ClampClosureFailedFullFailure;

   // MSS clamp closure test failed but needs a prime failure or MSS_RBC average to be above offset to get WBC flag.
   BaseElement<bool>  ClampClosureFailedPartFailure;

   BaseElement<float> VinEndedPIR;

   BaseElement<float> VinTargetEndPIR;

   BaseElement<float> previousDonorHct;

   // For testing only; when true it triggers chamber saturation event
   BaseElement<int> spoofChamberSat;

// Class Methods
public:
   ProcPQI_CDS (Role role);
   virtual ~ProcPQI_CDS();

// Data Members
protected:



// Class Methods
public:

private:
   ProcPQI_CDS();  // Base Constructor not available
};

#endif

/* FORMAT HASH c855f23b826e23d95c619e2b8f4469e8 */

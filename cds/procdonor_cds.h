#ifndef __PROCDONOR_CDS
#define __PROCDONOR_CDS

#include "datastore.h"

#ifndef WIN32
#pragma implementation
#endif

//
//   Procedure common data store:  This class was created to hold
//   the donor information specific to the procedure task
//
class ProcDonor_CDS
   : public SingleWriteDataStore
{

public:

   BaseElement<int>   Female;
   BaseElement<float> Height;           // in configuration units
   BaseElement<float> Weight;           // in configuration units
   BaseElement<float> BMI;
   BaseElement<float> TBV;              // ml
   BaseElement<float> TBVOgawa;         // ml
   BaseElement<float> PreCnt;
   BaseElement<float> Hct;              // as fraction, e.g. 0.43
   BaseElement<int>   BloodType;
   BaseElement<float> SampleVolume;
   BaseElement<char>  DownloadStatus;

// Class Methods
public:
   ProcDonor_CDS (Role role);
   virtual ~ProcDonor_CDS();

   int DonorTBV () { return (int)(TBV.Get() + 0.5); }

// Data Members
protected:


private:
   ProcDonor_CDS();  // Base Constructor not available
};

#endif

/* FORMAT HASH ad3ec80826b9d83e9a5bd5c7c7c4d401 */

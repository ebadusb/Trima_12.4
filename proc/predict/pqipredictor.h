// Static Model
#ifndef PQIPREDICTOR_H
#define PQIPREDICTOR_H


// Include files
#include "procdet.h"
#include "productqualitydata.h"

#include "procpqi_cds.h"
#include "halstatus_cds.h"
#include "procvolumes_cds.h"

// This derived class encapsulates all functionality needed to test a set of
// predicted run results for product verification conditions (reasons) conditions.
// Add new ones to this class.
class PQIPredictor
{
public:
   PQIPredictor();
   virtual ~PQIPredictor();

   // Returns true if there are NO reasons (stemming from the given Procedure
   // Detail prediction) that might preclude labelling the resulting Platelet
   // collection as containing less than 10E6 WBC contaminants.  Returns false
   // if reasons were found that probably will contaminate with WBC in excess of
   // 10E6.
   bool LabelPlatelets (const procedureDetail& ProcedureDetail) const;

protected:

   // CDS's that we use (all read only).
   ProcRun_CDS      _procRunCDS;
   ProcTrimaSet_CDS _procTrimaSetCDS;
   ProcDonor_CDS    _procDonorCDS;
   ProcPQI_CDS      _procPQICDS;
   ProcVolumes_CDS  _procVolumesCDS;
   HalStatus_CDS    _procHalStatusCDS;

};



#endif

/* FORMAT HASH 6cba6832ceec34bda3f326e906e947a6 */

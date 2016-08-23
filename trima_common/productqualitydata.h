// Static Model

//

#ifndef __VerifyFlags__ProductQualityData__
#define __VerifyFlags__ProductQualityData__


// Include files
#include "reasonlist.h"

// This is the storage structure for the Product Quality Information classes:
// - PQI Predictor (run selection time)
// - PQI Monitor (run time)
// - PQI Access (presentation time).
//
// It contains the verify flags and reasons for all possible products.
//
struct ProductQualityData
{
   //////////////////////////////////////////////////
   // Platelet Product
   ReasonList verify_platelet_wbc_reasons;
   ReasonList verify_platelet_yield_volume_reasons;
   ReasonList platelet_concentration_reasons;

   bool       label_platelets_leukoreduced;

   //////////////////////////////////////////////////
   // Plasma Product
   ReasonList verify_plasma_wbc_reasons;
   ReasonList verify_plasma_volume_reasons;

   bool       label_plasma_leukoreduced;

   //////////////////////////////////////////////////
   // RBC Product(s) -> (_prod2 is only used for auto DRBC procedures)
   ReasonList verify_rbc_volume_reasons;
   ReasonList verify_rbc_volume_reasons_prod2;
   ReasonList verify_rbc_wbc_reasons;
   ReasonList verify_rbc_wbc_reasons_prod2;
   ReasonList verify_rbc_quality_reasons;  // Quality reasons are for possible hemolysis on PTF sets.
   ReasonList verify_rbc_quality_reasons_prod2;

   bool       label_rbcs_leukoreduced;
   bool       label_rbcs_leukoreduced_prod2;
};

// END CLASS DEFINITION ProductQualityData

#endif // __VerifyFlags__ProductQualityData__

/* FORMAT HASH 7c23b80546dac47142f36cf45d4441a8 */

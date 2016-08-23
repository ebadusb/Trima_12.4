#include "safproc.h"


DataLog_Stream & operator << (DataLog_Stream& os, const SDonorVitals& vitals)
{
   return os << "DonorVitals("
             << "hUnits="    << ((vitals.cDonorHeightUnits==HEIGHT_UNITS_CM) ? "CM" : ((vitals.cDonorHeightUnits==HEIGHT_UNITS_IN) ? "IN" : "?"))
             << ";WUnits="   << ((vitals.cDonorWeightUnits==WEIGHT_UNITS_KG) ? "KG" : ((vitals.cDonorWeightUnits==WEIGHT_UNITS_LB) ? "LB" : "?"))
             << ";RBCUnits=" << ((vitals.cDonorRbcCount==RBC_COUNT_HEMATOCRIT) ? "HCT" : ((vitals.cDonorRbcCount==RBC_COUNT_HEMOGLOBIN) ? "HGB" : "?"))

          // Never log these (HIPPA)
          // << ";DOB="           << cDonorDateofBirth
          // << ";name="          << cDonorName

             << ";weight="   << (int)vitals.fDonorWeight
             << ";height="   << (int)vitals.fDonorHeight
             << ";crit="             << (int)vitals.fDonorHct
             << ";precount=" << (int)vitals.fDonorPltPrecount
             << ";pTBV="             << (int)vitals.fProcDonorTBV
             << ";sTBV="             << (int)vitals.fSafetyDonorTBV
             << ")";
}

/* FORMAT HASH b7bad389075497fc2a0aa90e577a4775 */

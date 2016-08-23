/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      donor.h
 *             Donor object to contain donor and validity tests.
 *
 ******************************************************************************/


#if !defined( INCLUDE_DONOR)
#define INCLUDE_DONOR

#include "safproc.h"
#include "message.h"

// Added defensive programming.  VxWorks mbuf.h defines clashing macros
#undef m_data


class CDonor
{

public:
   CDonor();
   CDonor(const SDonorVitals& vt);
   CDonor(const CDonor& dn);
   const CDonor& operator = (const CDonor& dn);
   const CDonor& operator = (const SDonorVitals& vt);
   ~CDonor();

   SDonorVitals&       Data (void);
   const SDonorVitals& cData (void) const;

   // Reset to clear donor data and initialize flags
   // Sets all values to 0
   // Info and download status are set to DEFAULT.
   void ResetDonor (void);

   // validity checking
   bool Valid (void) const;

   bool ValidSex (void) const;
   bool ValidWeight (void) const;
   bool ValidHeight (void) const;
   bool ValidHct (void) const;
   bool ValidPreCnt (void) const;
   bool ValidUnits (void) const;
   bool ValidBloodType (void) const;
   bool ValidSampleVolume (void) const;
   bool ValidRBCUnits (void) const;

   bool DonorVitalsEntered (void) const;
   bool DonorInfoEntered (void) const;

   // comparison.  Compare sex, ht, wt, tbv, pre, hct and
   // return true if equal
   bool SameAs (const CDonor& dn) const;
   bool SameAs (const SDonorVitals& dn) const;

   // Units
   bool HeightUnitsMetric (void) const;
   bool HeightUnitsStandard (void) const;
   bool WeightUnitsMetric (void) const;
   bool WeightUnitsStandard (void) const;
   bool RbcUnitsHct (void) const;
   bool RbcUnitsHgb (void) const;

   // vitals
   bool Male (void) const;     // return true if male
   bool Female (void) const;   // return true if female
   int  Gender (void) const { return m_data.cDonorSex; }

   float Height (void) const;
   float Weight (void) const;
   float PreCnt (void) const;
   float Hct (void) const;      // in %.
   float Hgb (void) const;      // in g/dL

   float       SampleVolume (void) const;
   const char* Name (void) const;
   const char* DOB (void) const;
   BLOODTYPE   BloodType (void) const;

   // Conversion routines.  Values are rounded up
   int HeightInCm () const;
   int HeightInInches () const;

   int WeightInKg () const;
   int WeightInLb () const;


   // returns proc TBV
   float TBV (void) const;
   float TBVOgawa (void) const;

   // calculate proc TBV from wt, ht and sex and store in our copy
   float CalTBV (void);

   // calculate TBV from inputs and return TBV
   // weight must be in lbs, height must be in inches
   float cCalTBVNadler (int Male, float weight, float height) const;

   //  weight must be in kg, height in meters
   float cCalTBVOgawa (int Male, float weight, float height) const;


   // These flags: DonorInfoStatus, DonorVitalsStatus, DonorDownloadStatus
   //  are set in GUI to inform about operator actions on donor stuff.

   // Donor information status (info = Hct, BT, Precount)
   // Donor info can be in one of three states:
   //      Default - its initialized state
   //      Complete -  Some or all values entered/valid but not confirmed
   //      Confirmed - Operator confirmed entries
   char DonorInfoStatus (void) const;
   char DonorInfoStatus (char stat);
   bool DonorInfoDefault (void) const;
   bool DonorInfoComplete (void) const;
   bool DonorInfoConfirmed (void) const;

   // Donor vitals status (ht,wt,sex)
   // Donor vitals can be in one of three states
   //      Incomplete , Complete, Confirmed
   char DonorVitalsStatus (void) const;
   bool DonorVitalsIncomplete (void) const;
   bool DonorVitalsComplete (void) const;
   bool DonorVitalsConfirmed (void) const;

   // Donor download status (complete donor packet from vista)
   // Donor download status can be in one of four states
   //      Default , Download complete , Confirmed and Rejected
   char DonorDownloadStatus (void) const;
   bool DonorDownloadDefault (void) const;
   bool DonorDownloadComplete (void) const;
   bool DonorDownloadConfirmed (void) const;
   bool DonorDownloadRejected (void) const;


protected:
   void Transfer (const SDonorVitals& dv);
   void Init (void);


private:
   SDonorVitals m_data;
};

#endif // !defined( INCLUDE_DONOR)

/* FORMAT HASH 2c41e772ef1db866ffa0db0cd71604a7 */

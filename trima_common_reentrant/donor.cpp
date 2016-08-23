/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      donor.cpp
 *             Donor object to contain donor and validity tests.
 *
 *
 ******************************************************************************/
#include <vxworks.h>

#include "string.h"
#include "math.h"
#include "donor.h"
#include "units.h"


CDonor::CDonor()
{
   Init();
}

CDonor::CDonor(const SDonorVitals& vt)
{
   Init();
   Transfer(vt);
}

CDonor::CDonor(const CDonor& dn)
{
   Init();
   Transfer(dn.cData() );
}

const CDonor& CDonor::operator = (const CDonor& dn)
{
   if ( this != &dn )
   {
      Transfer(dn.cData() );
   }
   return *this;
}

const CDonor& CDonor::operator = (const SDonorVitals& vt)
{
   if ( &m_data != &vt )
   {
      Transfer(vt);
   }
   return *this;
}

void CDonor::ResetDonor ()
{
   memset(&m_data, 0, sizeof( SDonorVitals ) );

   // Initialize values
   m_data.cDonorHeightUnits    = HEIGHT_UNITS_UNKNOWN;
   m_data.cDonorWeightUnits    = WEIGHT_UNITS_UNKNOWN;
   m_data.cDonorRbcCount       = RBC_COUNT_UNKNOWN;

   m_data.cDonorInfoStatus     = DONOR_INFO_DEFAULT;
   m_data.cDonorDownloadStatus = DONOR_DOWNLOAD_DEFAULT;

   // Default Donor Sample Volume to Invalid.
   m_data.fDonorSampleVolume = DONOR_SAMPLE_VOLUME_INVALID;
}

void CDonor::Init ()
{
   ResetDonor();
}

void CDonor::Transfer (const SDonorVitals& dv)
{
   memcpy(&m_data, &dv, sizeof( SDonorVitals ) );
}

CDonor::~CDonor()
{}

SDonorVitals& CDonor::Data ()
{
   return m_data;
}

const SDonorVitals& CDonor::cData () const
{
   return m_data;
}

bool CDonor::SameAs (const CDonor& dn) const
{
   return SameAs(dn.cData() );
}

bool CDonor::SameAs (const SDonorVitals& dv) const
{
   return (
      ( m_data.cDonorSex == dv.cDonorSex ) &&
      ( m_data.DonorBloodType == dv.DonorBloodType ) &&
      ( m_data.fDonorHct == dv.fDonorHct ) &&
      ( m_data.fDonorHeight == dv.fDonorHeight ) &&
      ( m_data.fDonorWeight == dv.fDonorWeight ) &&
      ( m_data.fDonorPltPrecount == dv.fDonorPltPrecount ) &&
      ( m_data.fDonorSampleVolume == dv.fDonorSampleVolume )
      );
}

bool CDonor::Male () const
{
   return ( m_data.cDonorSex == DONOR_SEX_IS_MALE );
}

bool CDonor::Female () const
{
   return ( m_data.cDonorSex == DONOR_SEX_IS_FEMALE );
}

bool CDonor::HeightUnitsMetric () const
{
   return ( m_data.cDonorHeightUnits == HEIGHT_UNITS_CM );
}

bool CDonor::HeightUnitsStandard () const
{
   return ( m_data.cDonorHeightUnits == HEIGHT_UNITS_IN );
}

bool CDonor::WeightUnitsMetric () const
{
   return ( m_data.cDonorWeightUnits == WEIGHT_UNITS_KG );
}

bool CDonor::WeightUnitsStandard () const
{
   return ( m_data.cDonorWeightUnits == WEIGHT_UNITS_LB );
}

bool CDonor::RbcUnitsHct () const
{
   return ( m_data.cDonorRbcCount == RBC_COUNT_HEMATOCRIT );
}

bool CDonor::RbcUnitsHgb () const
{
   return ( m_data.cDonorRbcCount == RBC_COUNT_HEMOGLOBIN );
}

float CDonor::Height () const
{
   return m_data.fDonorHeight;
}

float CDonor::Weight () const
{
   return m_data.fDonorWeight;
}

float CDonor::PreCnt () const
{
   return m_data.fDonorPltPrecount;
}

float CDonor::Hct () const
{
   return m_data.fDonorHct;
}

float CDonor::Hgb () const
{
   return (m_data.fDonorHct / HCT_TO_HGB);
}

float CDonor::SampleVolume (void) const
{
   return m_data.fDonorSampleVolume;
}

BLOODTYPE CDonor::BloodType () const
{
   return m_data.DonorBloodType;
}

const char* CDonor::DOB () const
{
   return m_data.cDonorDateofBirth;
}

const char* CDonor::Name () const
{
   return m_data.cDonorName;
}

float CDonor::TBV () const
{
   return m_data.fProcDonorTBV;
}

float CDonor::TBVOgawa () const
{
   return m_data.fProcDonorTBVOgawa;
}

float CDonor::CalTBV ()
{
   float w = Weight();

   if ( WeightUnitsMetric() )
      w *= LB_PER_KG;

   float h = Height();

   if ( HeightUnitsMetric() )
      h *= IN_PER_CM;

   m_data.fProcDonorTBV = cCalTBVNadler (Male(), w, h);

   //
   //
   //   Additionally calculate Ogawa TBV for use by other functions in Japan
   //
   w *= KG_PER_LB;  // weight now in kg
   h *= CM_PER_IN;  // height now in cm
   h /= 100.0f;     // height now in meters
   m_data.fProcDonorTBVOgawa = cCalTBVOgawa (Male(), w, h);

   return m_data.fProcDonorTBV;  // always return Nadler
}

float CDonor::cCalTBVNadler (int male, float wt, float ht) const
{
   float ret;

   if ( !male )
   {
      ret = 183.0f + 0.005835f * pow(ht, 3) + 15.0f * wt;
   }
   else
   {
      ret = 604.0f + 0.006012f * pow(ht, 3) + 14.6f * wt;
   }

   return( ret );
}

float CDonor::cCalTBVOgawa (int male, float wt, float ht) const
{
   //
   //  Formula is calculated in kilograms.
   //
   float ret;

   if (!male)
   {
      ret = (250.0f * ht * ht  * ht  + 63.0f * wt - 662.0f);
   }
   else
   {
      ret = (168.0f * ht * ht * ht + 50.0f * wt + 444.0f);
   }

   return (ret);

}


bool CDonor::Valid () const
{
   return ValidSex() && ValidWeight() && ValidHeight() &&
          ValidHct() && ValidPreCnt() && ValidUnits() &&
          ValidBloodType();
}

bool CDonor::ValidSex () const
{
   return ( Male() || Female() );
}

bool CDonor::ValidUnits () const
{
   return  (
      ( HeightUnitsMetric() || HeightUnitsStandard() ) &&
      ( WeightUnitsMetric() || WeightUnitsStandard() ) &&
      ( RbcUnitsHct()       || RbcUnitsHgb()         )
      );
}

bool CDonor::ValidHeight () const
{
   bool bRet = false;

   if ( HeightUnitsStandard() )
   {
      bRet = true;

      if ( m_data.fDonorHeight > MAX_ALLOWABLE_HEIGHT_IN )
         return false;

      if ( m_data.fDonorHeight < MIN_ALLOWABLE_HEIGHT_IN )
         return false;
   }

   if ( HeightUnitsMetric() )
   {
      bRet = true;

      if ( m_data.fDonorHeight > MAX_ALLOWABLE_HEIGHT_CM )
         return false;

      if ( m_data.fDonorHeight < MIN_ALLOWABLE_HEIGHT_CM )
         return false;
   }

   return bRet;

}

bool CDonor::ValidWeight () const
{
   bool bRet = false;

   if ( WeightUnitsStandard() )
   {
      bRet = true;

      if ( m_data.fDonorWeight > MAX_ALLOWABLE_WEIGHT_LB )
         return false;

      if ( m_data.fDonorWeight < MIN_ALLOWABLE_WEIGHT_LB )
         return false;
   }

   if ( WeightUnitsMetric() )
   {
      bRet = true;

      if ( m_data.fDonorWeight > MAX_ALLOWABLE_WEIGHT_KG )
         return false;

      if ( m_data.fDonorWeight < MIN_ALLOWABLE_WEIGHT_KG )
         return false;
   }

   return bRet;
}

bool CDonor::ValidHct () const
{
   if ( m_data.fDonorHct > MAX_ALLOWABLE_HCT )
      return false;

   if ( m_data.fDonorHct < MIN_ALLOWABLE_HCT )
      return false;

   return true;
}

bool CDonor::ValidPreCnt () const
{
   if ( m_data.fDonorPltPrecount > MAX_ALLOWABLE_PRECOUNT )
      return false;

   if ( m_data.fDonorPltPrecount < MIN_ALLOWABLE_PRECOUNT )
      return false;

   return true;
}

bool CDonor::ValidBloodType () const
{
   bool ret = false;

   switch ( m_data.DonorBloodType )
   {
      case BT_UNKNOWN :
      case BT_O_NEG :
      case BT_O_POS :
      case BT_AB_NEG :
      case BT_AB_POS :
      case BT_A_NEG :
      case BT_A_POS :
      case BT_B_NEG :
      case BT_B_POS :
         ret = true;
         break;

      default :
         break;
   }

   return ret;
}



bool CDonor::ValidSampleVolume (void) const
{
   return ( (m_data.fDonorSampleVolume >= 0) && (m_data.fDonorSampleVolume <= 100) );
}


bool CDonor::ValidRBCUnits (void) const
{
   switch (m_data.cDonorRbcCount)
   {
      case RBC_COUNT_UNKNOWN :
      case RBC_COUNT_HEMATOCRIT :
      case RBC_COUNT_HEMOGLOBIN :
         return true;

      default :
         return false;
   }

   return false;
}



bool CDonor::DonorVitalsEntered () const
{
   return (ValidSex() && ValidUnits() && ValidHeight() && ValidWeight());
}

bool CDonor::DonorInfoEntered () const
{
   return (ValidHct() && ValidPreCnt() && ValidBloodType() );
}

char CDonor::DonorInfoStatus () const
{
   return m_data.cDonorInfoStatus;
}

char CDonor::DonorInfoStatus (char stat)
{
   return m_data.cDonorInfoStatus = stat;
}

bool CDonor::DonorInfoDefault () const
{
   return ( DonorInfoStatus() == DONOR_INFO_DEFAULT );
}

bool CDonor::DonorInfoComplete () const
{
   return ( DonorInfoStatus() == DONOR_INFO_NOMINAL_COMPLETE );
}

bool CDonor::DonorInfoConfirmed () const
{
   return ( DonorInfoStatus() == DONOR_INFO_OPERATOR_COMFIRMATION );
}

char CDonor::DonorVitalsStatus () const
{
   return m_data.cDonorVitalsStatus;
}

bool CDonor::DonorVitalsIncomplete () const
{
   return ( DonorVitalsStatus() == DONOR_VITAL_INCOMPLETE );
}

bool CDonor::DonorVitalsComplete () const
{
   return ( DonorVitalsStatus() == DONOR_VITAL_NOMINAL_COMPLETE );
}

bool CDonor::DonorVitalsConfirmed () const
{
   return ( DonorVitalsStatus() == DONOR_VITAL_OPERATOR_COMFIRMATION );
}

char CDonor::DonorDownloadStatus () const
{
   return m_data.cDonorDownloadStatus;
}

bool CDonor::DonorDownloadComplete () const
{
   return ( DonorDownloadStatus() == DONOR_DOWNLOAD_ALL_AP2AGENT_VALUES );
}

bool CDonor::DonorDownloadDefault () const
{
   return ( DonorDownloadStatus() == DONOR_DOWNLOAD_DEFAULT );
}

bool CDonor::DonorDownloadConfirmed () const
{
   return ( DonorDownloadStatus() == DONOR_DOWNLOAD_OPERATOR_COMMIT );
}

bool CDonor::DonorDownloadRejected () const
{
   return ( DonorDownloadStatus() == DONOR_DOWNLOAD_OPERATOR_REJECT );
}

int CDonor::HeightInCm () const
{
   if (HeightUnitsMetric())
      return (int)(m_data.fDonorHeight + 0.5);
   else
      return (int)( (m_data.fDonorHeight * CM_PER_IN) + 0.5);
}

int CDonor::HeightInInches () const
{
   if (HeightUnitsStandard())
      return (int)(m_data.fDonorHeight + 0.5);
   else
      return (int)( (m_data.fDonorHeight * IN_PER_CM) + 0.5);
}

int CDonor::WeightInKg () const
{
   if (WeightUnitsMetric())
      return (int)(m_data.fDonorWeight + 0.5);
   else
      return (int)( (m_data.fDonorWeight * KG_PER_LB) + 0.5);
}

int CDonor::WeightInLb () const
{
   if (WeightUnitsStandard())
      return (int)(m_data.fDonorWeight + 0.5);
   else
      return (int)( (m_data.fDonorWeight * LB_PER_KG) + 0.5);
}

/* FORMAT HASH 99cf43ebaad3dc8c7a032d356c3dfb99 */

// CBloodHelper.cpp: implementation of the CBloodHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ap2 predictionater.h"
#include "math.h"
#include "cfg_ids.h"
#include "BloodHelper.h"
#include "units.h"
#include "safproc.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBloodHelper::CBloodHelper()
{

}

CBloodHelper::~CBloodHelper()
{

}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//             Blood type conversion
//
void CBloodHelper::load_blood_type_strings()
{
   //
   //
   //   Load in my blood type strings
   //
   if (m_string_O_POS.IsEmpty()) {
      m_string_O_POS.LoadString(IDS_BLOOD_TYPE_O_POS);
      m_string_O_NEG.LoadString(IDS_BLOOD_TYPE_O_NEG);
      m_string_A_POS.LoadString(IDS_BLOOD_TYPE_A_POS);
      m_string_A_NEG.LoadString(IDS_BLOOD_TYPE_A_NEG);
      m_string_B_POS.LoadString(IDS_BLOOD_TYPE_B_POS);
      m_string_B_NEG.LoadString(IDS_BLOOD_TYPE_B_NEG);
      m_string_AB_POS.LoadString(IDS_BLOOD_TYPE_AB_POS);
      m_string_AB_NEG.LoadString(IDS_BLOOD_TYPE_AB_NEG);
   }
}

void CBloodHelper::convert_blood_type(const int blood_type,
                                           CString &result)
{
   CBloodHelper::load_blood_type_strings();
   //
   //
   // verify blood type.
   //

   result.Empty();
   if (blood_type == BT_ALL) {
      result = "All";
   } else {
      if (blood_type & BT_O_NEG) result += m_string_O_NEG;
      if (blood_type & BT_O_POS) result += m_string_O_POS;
      if (blood_type & BT_AB_NEG) result += m_string_AB_NEG;
      if (blood_type & BT_AB_POS) result += m_string_AB_POS;
      if (blood_type & BT_A_NEG) result += m_string_A_NEG;
      if (blood_type & BT_A_POS) result += m_string_A_POS;
      if (blood_type & BT_B_NEG) result += m_string_B_NEG;
      if (blood_type & BT_B_POS) result += m_string_B_POS;
   }
   if (result.IsEmpty()) result = "None entered";
}

const BLOODTYPE CBloodHelper::convert_blood_type(const CString &blood_type)
{	
   CBloodHelper::load_blood_type_strings();

   BLOODTYPE result = BT_ALL;

   if (blood_type == m_string_O_NEG) {
      result = BT_O_NEG;
   } else if (blood_type == m_string_O_POS) {
      result = BT_O_POS;
   } else if (blood_type == m_string_AB_NEG) {
      result = BT_AB_NEG;
   } else if (blood_type == m_string_AB_POS) {
      result = BT_AB_POS;
   } else if (blood_type == m_string_A_NEG) {
      result = BT_A_NEG;
   } else if (blood_type == m_string_A_POS) {
      result = BT_A_POS;
   } else if (blood_type == m_string_B_NEG) {
      result = BT_B_NEG;
   } else if (blood_type == m_string_B_POS) {
      result = BT_B_POS;
   }
   return result;
}

const float CBloodHelper::CalculateTBV(const float height,
                                            const int   height_setting,
                                            const float weight,
                                            const int   weight_setting,
                                            const int   gender)
{
   //
   float DonorTBV = 0.0f;
   float DonorHeight = height;
   float DonorWeight = weight;
   
   // If metric units, must convert to English units
   if (height_setting != FT_HEIGHT)
   {
      DonorHeight = DonorHeight / CM_PER_IN;  // convert centimeters to inches
   }
   if (weight_setting != LB_WEIGHT)
   {
      DonorWeight = DonorWeight * LB_PER_KG;    // convert kg to lbs
   }
   
   if (gender == DONOR_SEX_IS_FEMALE )
   {
      DonorTBV = 183.0f + 0.005835f*(float)pow(DonorHeight,3) + 15.0f*DonorWeight;
   }
   else if (gender == DONOR_SEX_IS_MALE)
   {
      DonorTBV = 604.0f + 0.006012f*(float)pow(DonorHeight,3) + 14.6f*DonorWeight;
   }
   
   return DonorTBV;
}

CString CBloodHelper::m_string_O_POS;// use default constructor, initialize later
CString CBloodHelper::m_string_O_NEG;
CString CBloodHelper::m_string_A_POS;
CString CBloodHelper::m_string_A_NEG;
CString CBloodHelper::m_string_B_POS;
CString CBloodHelper::m_string_B_NEG;
CString CBloodHelper::m_string_AB_POS;
CString CBloodHelper::m_string_AB_NEG;

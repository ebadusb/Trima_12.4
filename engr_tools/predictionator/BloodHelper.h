// BloodHelper.h: interface for the CBloodHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLOODHELPER_H__82F8B794_559A_4F2B_8A97_3A6F673CD30E__INCLUDED_)
#define AFX_BLOODHELPER_H__82F8B794_559A_4F2B_8A97_3A6F673CD30E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bloodtype.h"

class CBloodHelper  
{
public:
	CBloodHelper();
	virtual ~CBloodHelper();
 public:
      //
      //
      //    Blood type conversion functionality
      //
      static const BLOODTYPE convert_blood_type(const CString &blood_type);
      static void convert_blood_type(const int blood_type,CString &result);

	  //
      //
      //  I can calculate a donor TBV given height, weight,
      //  gender, and height/weight units settings.
      //
      static const float CalculateTBV(const float height,
                                      const int   height_setting,
                                      const float weight,
                                      const int weight_setting,
                                      const int gender);


 private:
      static void load_blood_type_strings();
      static CString m_string_O_POS;
      static CString m_string_O_NEG;
      static CString m_string_A_POS;
      static CString m_string_A_NEG;
      static CString m_string_B_POS;
      static CString m_string_B_NEG;
      static CString m_string_AB_POS;
      static CString m_string_AB_NEG;

};

#endif // !defined(AFX_BLOODHELPER_H__82F8B794_559A_4F2B_8A97_3A6F673CD30E__INCLUDED_)

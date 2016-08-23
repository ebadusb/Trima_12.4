/*
 *	Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: J:/BCT_Development/vxWorks/Trima/gui/rcs/text_utils.h 1.1 2003/08/21 19:32:36Z rm70006 Exp rm10919 $
 *
*/


#ifndef __TEXT_UTILS__
#define __TEXT_UTILS__


#include "string_info.h"
#include "bloodtype.h"
#include "guistring.h"
#include "adjust.h"       // Get Adjustments Enum
#include "safproc.h"      // Get SDonorVitals struct


class GuiTextUtils
{
public:
   GuiTextUtils();

   ~GuiTextUtils();

   // Common String conversion routines
   TEXT_STRING_ID GetBloodType (BLOODTYPE bt);

   // Builds the Height string in current language and donor entered height units
   void      BuildHeightString (const SDonorVitals& vitals, guistring& height_string);
   void      BuildWeightString (const SDonorVitals& vitals, guistring& weight_string);
   void      BuildDonorGenderString (const SDonorVitals& vitals, guistring& gender_string);
   guistring BuildAdjustmentString (const troubleshooting_adjustments& adjustment);
};


#endif

/* FORMAT HASH eb736c35536b2420aee1bc1cc55ae0f4 */

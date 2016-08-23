/*
 *	Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 *
 *
*/


#include "text_utils.h"
#include "display_text.h"
#include "donor.h"          // Gets the donor class
#include "error.h"          // Get FATAL_ERROR function.
#include "guiglobs.hpp"


//
// Constructor
//
GuiTextUtils::GuiTextUtils()
{}



//
//
//
GuiTextUtils::~GuiTextUtils()
{}



//
// Common String conversion routines
//
TEXT_STRING_ID GuiTextUtils::GetBloodType (BLOODTYPE bt)
{
   switch (bt)
   {
      case BT_O_NEG :
         return textListboxItemCfgBloodTypeOn;
         break;

      case BT_O_POS :
         return textMiscBloodTypeOPos;
         break;

      case BT_AB_NEG :
         return textMiscBloodTypeAbNeg;
         break;

      case BT_AB_POS :
         return textMiscBloodTypeAbPos;
         break;

      case BT_A_NEG :
         return textMiscBloodTypeANeg;
         break;

      case BT_A_POS :
         return textMiscBloodTypeAPos;
         break;

      case BT_B_NEG :
         return textMiscBloodTypeBNeg;
         break;

      case BT_B_POS :
         return textMiscBloodTypeBPos;
         break;

      case BT_UNKNOWN :
         return textBlank;
         break;

      default :
         return TEXT_NULL;
   }
}



//
// BuildHeightString
//
void GuiTextUtils::BuildHeightString (const SDonorVitals& vitals, guistring& height_string)
{
   CDonor       donor(vitals);
   Display_Text displayText(textEmpty);

   guistring    guiString;

   //
   //  Formula is calculated in meters not centimeters.
   //
   switch (guiglobs::cdsData.config.LangUnit.Get().key_height)
   {
      case FT_HEIGHT :
      {
         //
         // put the feet in the buffer
         //
         const int ht_feet = donor.HeightInInches() / 12;
         sprintf (height_string, "%d", ht_feet);

         displayText.set_string_id(textDataUnitsCfrFeet);
         displayText.get_text_direct (guiString);
         height_string += guiString;

         // Put the Inches in the buffer
         const int ht_inches = donor.HeightInInches() % 12;
         sprintf (guiString, " %d", ht_inches);
         height_string += guiString;

         displayText.set_string_id(textDataUnitsCfrInches);
         displayText.get_text_direct (guiString);
         height_string += guiString;
      }
      break;

      case CENTI_HEIGHT :
      {
         //
         // put the cm in the buffer
         //
         const int ht_cm = donor.HeightInCm();

         sprintf (height_string, "%d", ht_cm);

         displayText.set_string_id(textDataUnitsCfrCm);
         displayText.get_text_direct (guiString);
         height_string += guiString;
      }
      break;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Unable to determine the height units type from donor data.");
         break;
   }
}



//
// BuildWeightString
//
void GuiTextUtils::BuildWeightString (const SDonorVitals& vitals, guistring& weight_string)
{
   CDonor       donor(vitals);
   Display_Text displayText(textEmpty);

   guistring    guiString;

   switch (guiglobs::cdsData.config.LangUnit.Get().key_weight)
   {
      case LB_WEIGHT :
      {
         const int wt_pounds = donor.WeightInLb();

         sprintf (weight_string, "%d", wt_pounds);

         displayText.set_string_id(textDataUnitsCfrLbs);
         displayText.get_text_direct (guiString);

         // Add Label
         weight_string += guiString;
      }
      break;

      case KG_WEIGHT :
      {
         const int wt_kilos = donor.WeightInKg();

         sprintf (weight_string, "%d", wt_kilos);
         displayText.set_string_id(textDataUnitsCfrKg);
         displayText.get_text_direct(guiString);

         // add Label
         weight_string += guiString;
      }
      break;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Unable to determine the weight units type from donor data.");
         break;
   }
}



//
// BuildDonorGenderString
//
void GuiTextUtils::BuildDonorGenderString (const SDonorVitals& vitals, guistring& gender_string)
{
   Display_Text displayText;

   switch (vitals.cDonorSex)
   {
      case DONOR_SEX_IS_MALE :
         displayText.set_string_id(textDataCfrGenderMale);
         break;

      case DONOR_SEX_IS_FEMALE :
         displayText.set_string_id(textDataCfrGenderFemale);
         break;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Unable to determine the gender from AP2 donor data.");
   }

   displayText.get_text_direct(gender_string);
}


//
// BuildAdjustmentString
//
guistring GuiTextUtils::BuildAdjustmentString (const troubleshooting_adjustments& adjustment) // , guistring &adjustment_string)
{
   Display_Text displayText;
   guistring    adjustment_string;

   switch (adjustment)
   {
      case ACCESS_PRESSURE_UP :
         displayText.set_string_id(textMiscCfrActionDrawUp);
         break;

      case ACCESS_PRESSURE_DOWN :
         displayText.set_string_id(textMiscCfrActionDrawDown);
         break;

      case RETURN_PRESSURE_UP :
         displayText.set_string_id(textMiscCfrActionReturnUp);
         break;

      case RETURN_PRESSURE_DOWN :
         displayText.set_string_id(textMiscCfrActionReturnDown);
         break;

      case TINGLING_UP :
         displayText.set_string_id(textMiscCfrActionAcReactionUp);
         break;

      case TINGLING_DOWN :
         displayText.set_string_id(textMiscCfrActionAcReactionDown);
         break;

      case CLUMPING_UP :
         displayText.set_string_id(textMiscCfrActionClumpingUp);
         break;

      case CLUMPING_DOWN :
         displayText.set_string_id(textMiscCfrActionClumpingDown);
         break;

      case SPILLOVER :
      case AIR_IN_PLASMA_LINE :
      case SALINE_BOLUS :
         displayText.set_string_id(textBlank);
         break;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Unable to determine adjustment type from msg.");
   }

   displayText.get_text_direct(adjustment_string);
   return(adjustment_string);
}

/* FORMAT HASH 1eca4cc793dd97e1569f1defa1e7aebb */

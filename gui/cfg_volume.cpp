/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_CONFIG_VOLUME

*     File name:   cfg_volume.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$

*/


#include <vxworks.h>

/** include files **/

// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "an_alarm.h"

// BASE CLASS INCLUDE
#include "cfg_volume.hpp"

static void cbCfgVolToggle         (void* data);
static void cbCfgTbvToggle         (void* data);
static void cbCfgWeightToggle      (void* data);
static void cbCfgDrbcTbvToggle     (void* data);
static void cbCfgPlateletMaxToggle (void* data);
static void cbCfgPlasmaMaxToggle   (void* data);

static void cbCfgWeightSetting    (void* data);
static void cbCfgVolLimitLTSetting (void* data);
static void cbCfgVolLimitGTSetting (void* data);

static void cbCfgTbvSetting     (void* data);
static void cbCfgMlPerKgSetting (void* data);
static void cbCfgDrbcLimSetting (void* data);
static void cbCfgVolPlasmaMax   (void* data);
static void cbCfgVolPlateletMax (void* data);

/** defines for button placement **/

#define ROW1 162
#define ROW2 262
#define ROW3 362

#define COL1 37
#define COL2 185
#define COL3 333
#define COL4 481

#define MIN_WEIGHT_LB 90.0f
#define DASHES "---"

#define CONFIG_BTN_IDS CFG_Look::BUTTON_UP_NAME, CFG_Look::BUTTON_DOWN_NAME
#define CONFIG_STYLE CFG_Look::FONT_COLOR, CFG_Look::FONT_ALIGN, CFG_Look::FONT_SIZE

//
////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_CONFIG_VOLUME::Screen_CONFIG_VOLUME()
   : Screen_Config(TEAL, GUI_SCREEN_CONFIG_VOLUME_LIMITS, textCfgScreenLabelVolLim),
     japan_screen(false),

//
// Row 1
//
     _btnCfgTbvToggle(GUI_BUTTON_CONFIG_DONOR_TBV,
                      COL1, ROW1,
                      textButtonCfgDonLimTbvToggle,
                      CONFIG_BTN_IDS),

     _btnCfgTbvPercent(COL2, ROW1,
                       guiglobs::cdsData.rwConfig.procedure.key_tbv_percent,
                       KEY_TBV_PERCENT,
                       CONFIG_BTN_IDS,
                       GUI_BUTTON_CONFIG_DONOR_TBV_PERCENT,
                       textBlank,
                       CONFIG_STYLE),

//
// Row 2
//
     _btnCfgPlateletMaxToggle(GUI_BUTTON_CONFIG_PLATELET_VOL_MAX_TOGGLE,
                              COL1, ROW2,
                              textButtonCfgDonLimPlateletVolMaxToggle,
                              CONFIG_BTN_IDS),

     _btnCfgPlateletMaxVol(COL2, ROW2,
                           guiglobs::cdsData.rwConfig.procedure.key_max_plasma_vol_during_platelet_collection,
                           KEY_MAX_PLASMA_DURING_PLT_VOL,
                           CONFIG_BTN_IDS,
                           GUI_BUTTON_CONFIG_DONOR_PLATELET_VOL_MAX,
                           textButtonLabelCfgDonLimPlateletMaxVolume,
                           CONFIG_STYLE),

//
// Row 3
//
     _btnCfgPlasmaMaxToggle(GUI_BUTTON_CONFIG_PLASMA_VOL_MAX_TOGGLE,
                            COL1, ROW3,
                            textButtonCfgDonLimPlasmaVolMaxToggle,
                            CONFIG_BTN_IDS),

     _btnCfgPlasmaMaxVol(COL2, ROW3,
                         guiglobs::cdsData.rwConfig.procedure.key_max_plasma_vol_during_plasma_collection,
                         KEY_MAX_PLASMA_DURING_PLS_VOL,
                         CONFIG_BTN_IDS,
                         GUI_BUTTON_CONFIG_DONOR_PLASMA_VOL_MAX,
                         textButtonLabelCfgDonLimPlasmaMaxVolume,
                         CONFIG_STYLE),

     _btnCfgVolToggle(GUI_BUTTON_CONFIG_DONOR_VOLUME,
                      COL1, ROW1,
                      textButtonCfgDonLimVolToggle,
                      CONFIG_BTN_IDS),

     _btnCfgWeight(COL2, ROW1,
                   guiglobs::cdsData.rwConfig.lang.key_weight,
                   KEY_WEIGHT_SETTING,
                   CONFIG_BTN_IDS,
                   GUI_BUTTON_CONFIG_DONOR_WEIGHT,
                   textEmpty,
                   CONFIG_STYLE),

     _btnCfgVolLimitLT(COL3, ROW1,
                       guiglobs::cdsData.rwConfig.procedure.key_weight_less_than_vol,
                       KEY_WEIGHT_LESS_THAN_VOL,
                       CONFIG_BTN_IDS,
                       GUI_BUTTON_CONFIG_DONOR_WEIGHT_LESS_THAN_VOL,
                       textButtonLabelCfgDonLimVolSmallBucketSetting,
                       CONFIG_STYLE),

     _btnCfgVolLimitGT(COL4, ROW1,
                       guiglobs::cdsData.rwConfig.procedure.key_weight_greater_than_vol,
                       KEY_WEIGHT_GREATER_THAN_VOL,
                       CONFIG_BTN_IDS,
                       GUI_BUTTON_CONFIG_DONOR_WEIGHT_GREATER_THAN_VOL,
                       textButtonLabelCfgDonLimVolLargeBucketSetting,
                       CONFIG_STYLE),

     _btnCfgDrbcBodyVolToggle(GUI_BUTTON_CONFIG_DRBC_VOL_LIM,
                              COL3, ROW2,
                              textButtonCfgDonLimDrbcVolToggle,
                              CONFIG_BTN_IDS),

     _btnCfgDrbcBodyVol(COL4, ROW2,
                        guiglobs::cdsData.rwConfig.procedure.key_drbc_body_vol,
                        KEY_DRBC_BODY_VOL,
                        CONFIG_BTN_IDS,
                        GUI_BUTTON_CONFIG_DONOR_BODY_VOL,
                        textButtonLabelCfgDonLimDrbcVolSetting,
                        CONFIG_STYLE),

     _btnCfgMlPerKgToggle(GUI_BUTTON_CONFIG_DONOR_BODY_WEIGHT,
                          COL1, ROW3,
                          textButtonCfgDonLimWeightToggle,
                          CONFIG_BTN_IDS),

     _btnCfgMlPerKg(COL2, ROW3,
                    guiglobs::cdsData.rwConfig.procedure.key_ml_per_kg,
                    KEY_ML_PER_KG,
                    CONFIG_BTN_IDS,
                    GUI_BUTTON_CONFIG_DONOR_ML_PER_KG,
                    textButtonLabelCfgDonLimMlPerKgSetting,
                    CONFIG_STYLE),

     _bmpUnderlineTbv            (COL1 + 40, ROW1 + 54, BITMAP_CFGDONOR_HALF_UL),
     _bmpUnderlineTbvBox         (COL1 + 40, ROW1 + 48, BITMAP_CFGDONOR_UL_BOX),
     _bmpUnderlinePlateletMax    (COL1 + 40, ROW2 + 54, BITMAP_CFGDONOR_HALF_UL),
     _bmpUnderlinePlateletMaxBox (COL1 + 40, ROW2 + 48, BITMAP_CFGDONOR_UL_BOX),
     _bmpUnderlinePlasmaMax      (COL1 + 40, ROW3 + 54, BITMAP_CFGDONOR_HALF_UL),
     _bmpUnderlinePlasmaMaxBox   (COL1 + 40, ROW3 + 48, BITMAP_CFGDONOR_UL_BOX),

     _bmpUnderlineVol       (COL1 + 40, ROW1 + 54, BITMAP_CFGDONOR_FULL_UL),
     _bmpUnderlineDrbcVol   (COL3 + 40, ROW2 + 54, BITMAP_CFGDONOR_HALF_UL),
     _bmpUnderlineMlPerKg   (COL1 + 40, ROW3 + 54, BITMAP_CFGDONOR_HALF_UL),

     _bmpUnderlineVolBox       (COL1 + 40, ROW1 + 48, BITMAP_CFGDONOR_UL_BOX),
     _bmpUnderlineMlPerKgBox   (COL1 + 40, ROW3 + 48, BITMAP_CFGDONOR_UL_BOX),
     _bmpUnderlineDrbcVolBox   (COL3 + 40, ROW2 + 48, BITMAP_CFGDONOR_UL_BOX),

     _txtNoneSelected(textMiscCfgNoVolLimSelected),
     _txtModeHeading(textCfgColLabelCfgDonLimMode),
     _txtTBVPercent(textButtonLabelCfgDonLimTbvSetting),
     _txtTBVPercentJp(textButtonLabelCfgDonLimTbvSettingJp)
{}  // END of Screen_CONFIG_VOLUME CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_CONFIG_VOLUME::~Screen_CONFIG_VOLUME()
{
   Screen_CONFIG_VOLUME::deallocate_resources ();
}  // END of Screen_CONFIG_VOLUME DESTRUCTOR

float Screen_CONFIG_VOLUME::weightConversionFactor ()
{
   if (guiglobs::cdsData.rwConfig.lang.key_weight == KG_WEIGHT)
      return LB_PER_KG;
   else
      return 1.0f;
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_CONFIG_VOLUME::allocate_resources (const char* allocation_parameter)
{
   japan_screen = Software_CDS::GetInstance().getFeature(JapanFeatures);

   // allocate parent's resources first
   Screen_Config::allocate_resources (allocation_parameter);

   selectScreenLayout(guiglobs::cdsData.rwConfig.lang.key_weight == KG_WEIGHT);

   //
   //
   //  TBV button is valid for both regions.
   //
   _btnCfgTbvToggle.set_callback (cbCfgTbvToggle, (void*)this);
   _btnCfgTbvPercent.set_callback(cbCfgTbvSetting, (void*)this);

   // Set up the toggle buttons
   if (japan_screen)
   {
      _btnCfgPlateletMaxToggle.set_callback (cbCfgPlateletMaxToggle, (void*)this);
      _btnCfgPlasmaMaxToggle.set_callback (cbCfgPlasmaMaxToggle, (void*)this);
      _btnCfgPlateletMaxVol.set_callback(cbCfgVolPlateletMax, (void*)this);
      _btnCfgPlasmaMaxVol.set_callback(cbCfgVolPlasmaMax, (void*)this);
   }
   else
   {
      _btnCfgVolToggle.set_callback (cbCfgVolToggle, (void*)this);
      _btnCfgMlPerKgToggle.set_callback (cbCfgWeightToggle, (void*)this);
      _btnCfgDrbcBodyVolToggle.set_callback (cbCfgDrbcTbvToggle, (void*)this);
      _btnCfgDrbcBodyVol.set_callback(cbCfgDrbcLimSetting, (void*)this);
      _btnCfgMlPerKg.set_callback(cbCfgMlPerKgSetting, (void*)this);
      _btnCfgWeight.set_callback(cbCfgWeightSetting, (void*)this);
      _btnCfgVolLimitLT.set_callback(cbCfgVolLimitLTSetting, (void*)this);
      _btnCfgVolLimitGT.set_callback(cbCfgVolLimitGTSetting, (void*)this);
   }

   // Update the data buttons with the correct information
   updateButton(_btnCfgTbvPercent,     VOLUME_REMOVAL_TBV,           guiglobs::cdsData.rwConfig.procedure.key_tbv_percent);
   if (japan_screen)
   {
      updateButton(_btnCfgPlateletMaxVol, VOLUME_REMOVAL_PLATELET_VOL,  guiglobs::cdsData.rwConfig.procedure.key_max_plasma_vol_during_platelet_collection);
      updateButton(_btnCfgPlasmaMaxVol,   VOLUME_REMOVAL_PLASMA_VOL,    guiglobs::cdsData.rwConfig.procedure.key_max_plasma_vol_during_plasma_collection);
   }
   else
   {
      updateButton(_btnCfgWeight,         VOLUME_REMOVAL_WEIGHT,        convertWeight(guiglobs::cdsData.rwConfig.procedure.key_weight_setting));
      updateButton(_btnCfgVolLimitLT,     VOLUME_REMOVAL_WEIGHT,        guiglobs::cdsData.rwConfig.procedure.key_weight_less_than_vol);
      updateButton(_btnCfgVolLimitGT,     VOLUME_REMOVAL_WEIGHT,        guiglobs::cdsData.rwConfig.procedure.key_weight_greater_than_vol);
      updateButton(_btnCfgDrbcBodyVol,    VOLUME_REMOVAL_DRBC_BODY_VOL, guiglobs::cdsData.rwConfig.procedure.key_drbc_body_vol, 1);
      updateButton(_btnCfgMlPerKg,        VOLUME_REMOVAL_ML_PER_KG,     guiglobs::cdsData.rwConfig.procedure.key_ml_per_kg, 1);
   }

   // Update the toggle buttons
   _btnCfgTbvToggle.setActive(isEnabled(VOLUME_REMOVAL_TBV));
   if (japan_screen)
   {
      _btnCfgPlateletMaxToggle.setActive(isEnabled(VOLUME_REMOVAL_PLATELET_VOL));
      _btnCfgPlasmaMaxToggle.setActive(isEnabled(VOLUME_REMOVAL_PLASMA_VOL));
   }
   else
   {

      _btnCfgVolToggle.setActive(isEnabled(VOLUME_REMOVAL_WEIGHT));
      _btnCfgMlPerKgToggle.setActive(isEnabled(VOLUME_REMOVAL_ML_PER_KG));
      _btnCfgDrbcBodyVolToggle.setActive(isEnabled(VOLUME_REMOVAL_DRBC_BODY_VOL));
   }

}

float Screen_CONFIG_VOLUME::convertWeight (float& setting, WEIGHT_CONVERSION conversion = FROM_LBS)
{
   float returnVal = setting;

   switch (conversion)
   {
      case FROM_LBS :
         returnVal = setting / weightConversionFactor();

         if ((guiglobs::cdsData.rwConfig.lang.key_weight == LB_WEIGHT) && (setting < MIN_WEIGHT_LB))
         {
            setting   = MIN_WEIGHT_LB;
            returnVal = MIN_WEIGHT_LB;

            DataLog (log_level_gui_info) << "Weight entry violates constraints.  Setting to " << MIN_WEIGHT_LB << endmsg;
         }

         break;

      case TO_LBS :
         returnVal = setting * weightConversionFactor();
         break;

      default :
         DataLog (log_level_gui_error) << "Unknown conversion type" << endmsg;
         break;
   }

   return returnVal;
}

void Screen_CONFIG_VOLUME::selectScreenLayout (const bool useKgUnits)
{
   const TEXT_STRING_ID weightButtonText = (useKgUnits ? textButtonLabelCfgDonLimVolWeightSettingKg : textButtonLabelCfgDonLimVolWeightSettingLb);

   //
   //
   // Note TBV is valid for both regions but is in a different screen location.
   //
   if (!japan_screen)
   {
      _bmpUnderlineTbv.set_position(COL1 + 40, ROW2 + 54);
      _bmpUnderlineTbvBox.set_position(COL1 + 40, ROW2 + 48);
      _btnCfgTbvToggle.set_position(COL1, ROW2);
      _btnCfgTbvPercent.set_position(COL2, ROW2);
   }
   else
   {
      _bmpUnderlineTbv.set_position(COL1 + 40, ROW1 + 54);
      _bmpUnderlineTbvBox.set_position(COL1 + 40, ROW1 + 48);
      _btnCfgTbvToggle.set_position(COL1, ROW1);
      _btnCfgTbvPercent.set_position(COL2, ROW1);
   }

   _txtModeHeading.allocate_resources(*this);
   _bmpUnderlineTbv.allocate_resources(*this);
   _bmpUnderlineTbvBox.allocate_resources(*this);

   // Create the underlines
   if (japan_screen)
   {
      _txtTBVPercentJp.allocate_resources(*this);
      _txtTBVPercentJp.set_color(isEnabled(VOLUME_REMOVAL_TBV) ? YELLOW : GREY_LIGHT);
      _bmpUnderlinePlateletMax.allocate_resources(*this);
      _bmpUnderlinePlateletMaxBox.allocate_resources(*this);
      _bmpUnderlinePlasmaMax.allocate_resources(*this);
      _bmpUnderlinePlasmaMaxBox.allocate_resources(*this);

   }
   else
   {
      _txtTBVPercent.allocate_resources(*this);
      _txtTBVPercentJp.set_color(isEnabled(VOLUME_REMOVAL_TBV) ? YELLOW : GREY_LIGHT);
      _bmpUnderlineVol.allocate_resources(*this);
      _bmpUnderlineVolBox.allocate_resources(*this);
      _bmpUnderlineDrbcVol.allocate_resources(*this);
      _bmpUnderlineDrbcVolBox.allocate_resources(*this);
      _bmpUnderlineMlPerKg.allocate_resources(*this);
      _bmpUnderlineMlPerKgBox.allocate_resources(*this);
   }

// Allocate toggle buttons
   _btnCfgTbvToggle.allocate_resources(*this);
   _btnCfgTbvPercent.allocate_resources(*this);

   if (japan_screen)
   {
      _btnCfgPlateletMaxToggle.allocate_resources(*this);
      _btnCfgPlateletMaxVol.allocate_resources(*this);

      _btnCfgPlasmaMaxToggle.allocate_resources(*this);
      _btnCfgPlasmaMaxVol.allocate_resources(*this);
   }
   else
   {
      _btnCfgVolToggle.allocate_resources(*this);
      _btnCfgWeight.allocate_resources(*this);
      _btnCfgWeight.change_label_text(weightButtonText); // make kg vs lb label correct
      _btnCfgVolLimitLT.allocate_resources(*this);
      _btnCfgVolLimitGT.allocate_resources(*this);

      _btnCfgMlPerKgToggle.allocate_resources(*this);
      _btnCfgMlPerKg.allocate_resources(*this);

      _btnCfgDrbcBodyVolToggle.allocate_resources(*this);
      _btnCfgDrbcBodyVol.allocate_resources(*this);

   }

}


//
////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_CONFIG_VOLUME::deallocate_resources ()
{
   // Toggle buttons
   _btnCfgVolToggle.deallocate_resources();
   _btnCfgMlPerKgToggle.deallocate_resources();
   _btnCfgDrbcBodyVolToggle.deallocate_resources();
   _btnCfgTbvToggle.deallocate_resources();
   _btnCfgPlateletMaxToggle.deallocate_resources();
   _btnCfgPlasmaMaxToggle.deallocate_resources();
   _btnCfgWeight.deallocate_resources();
   _btnCfgVolLimitGT.deallocate_resources();
   _btnCfgVolLimitLT.deallocate_resources();
   _btnCfgMlPerKg.deallocate_resources();
   _btnCfgDrbcBodyVol.deallocate_resources();
   _btnCfgTbvPercent.deallocate_resources();
   _btnCfgPlateletMaxVol.deallocate_resources();
   _btnCfgPlasmaMaxVol.deallocate_resources();
   _bmpUnderlineVol.deallocate_resources();
   _bmpUnderlineMlPerKg.deallocate_resources();
   _bmpUnderlineDrbcVol.deallocate_resources();
   _bmpUnderlineTbv.deallocate_resources();
   _bmpUnderlinePlateletMax.deallocate_resources();
   _bmpUnderlinePlasmaMax.deallocate_resources();
   _bmpUnderlineTbvBox.deallocate_resources();
   _bmpUnderlinePlateletMaxBox.deallocate_resources();
   _bmpUnderlinePlasmaMaxBox.deallocate_resources();
   _bmpUnderlineDrbcVolBox.deallocate_resources();
   _bmpUnderlineVolBox.deallocate_resources();
   _bmpUnderlineMlPerKgBox.deallocate_resources();
   _txtModeHeading.deallocate_resources();
   _txtNoneSelected.deallocate_resources();
   _txtTBVPercent.deallocate_resources();
   _txtTBVPercentJp.deallocate_resources();

   // deallocate parent's resources
   Screen_Config::deallocate_resources ();
}  // END of Screen_CONFIG_VOLUME DEALLOCATE_RESOURCES


//
////////////////////////////////////////////////////////////////////////////////
// process continue button - Set operator attention if no volume method selected.
void Screen_CONFIG_VOLUME::process_continue_button ()
{
   if (checkForValidConstraints())
      Screen_Config::process_continue_button();
   else
      _txtNoneSelected.allocate_resources(*this);
}

//
////////////////////////////////////////////////////////////////////////////////
//  Message handler for KEYPADS
//  Update button text and CDS with keypad information.
//
void Screen_CONFIG_VOLUME::update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer)
{
   turn_off_min_max_keypad_text();

   if (return_keypad_buffer.keypad_return_value == -1)
      return;

   float the_data = (float)atof(return_keypad_buffer.keypad_return_string);

   switch (return_keypad_buffer.config_data_type)
   {
      case KEY_TBV_PERCENT :
         guiglobs::cdsData.rwConfig.procedure.key_tbv_percent = the_data;
         updateButton(_btnCfgTbvPercent, VOLUME_REMOVAL_TBV, guiglobs::cdsData.rwConfig.procedure.key_tbv_percent);
         break;

      case KEY_MAX_PLASMA_DURING_PLT_VOL :
         guiglobs::cdsData.rwConfig.procedure.key_max_plasma_vol_during_platelet_collection = the_data;
         updateButton(_btnCfgPlateletMaxVol, VOLUME_REMOVAL_PLATELET_VOL, guiglobs::cdsData.rwConfig.procedure.key_max_plasma_vol_during_platelet_collection);
         break;

      case KEY_MAX_PLASMA_DURING_PLS_VOL :
         guiglobs::cdsData.rwConfig.procedure.key_max_plasma_vol_during_plasma_collection = the_data;
         updateButton(_btnCfgPlasmaMaxVol, VOLUME_REMOVAL_PLASMA_VOL, guiglobs::cdsData.rwConfig.procedure.key_max_plasma_vol_during_plasma_collection);
         break;

      case KEY_ML_PER_KG :
         guiglobs::cdsData.rwConfig.procedure.key_ml_per_kg = the_data;
         updateButton(_btnCfgMlPerKg, VOLUME_REMOVAL_ML_PER_KG, guiglobs::cdsData.rwConfig.procedure.key_ml_per_kg, 1);
         break;

      case KEY_DRBC_BODY_VOL :
         guiglobs::cdsData.rwConfig.procedure.key_drbc_body_vol = the_data;
         updateButton(_btnCfgDrbcBodyVol, VOLUME_REMOVAL_DRBC_BODY_VOL, guiglobs::cdsData.rwConfig.procedure.key_drbc_body_vol, 1);
         break;

      case KEY_WEIGHT_SETTING :
         guiglobs::cdsData.rwConfig.procedure.key_weight_setting = convertWeight(the_data, TO_LBS);
         updateButton(_btnCfgWeight, VOLUME_REMOVAL_WEIGHT, the_data);
         break;

      case KEY_WEIGHT_LESS_THAN_VOL :
         guiglobs::cdsData.rwConfig.procedure.key_weight_less_than_vol = the_data;
         updateButton(_btnCfgVolLimitLT, VOLUME_REMOVAL_WEIGHT, guiglobs::cdsData.rwConfig.procedure.key_weight_less_than_vol);
         break;

      case KEY_WEIGHT_GREATER_THAN_VOL :
         guiglobs::cdsData.rwConfig.procedure.key_weight_greater_than_vol = the_data;
         updateButton(_btnCfgVolLimitGT, VOLUME_REMOVAL_WEIGHT, guiglobs::cdsData.rwConfig.procedure.key_weight_greater_than_vol);
         break;

      default :
         DataLog(log_level_gui_error) << "could not find CFG_VOLUME keypad index "
                                      << return_keypad_buffer.config_data_type << " upon return from keypad" << endmsg;

         {
            anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
            alarm.setAlarm();
         }
         break;
   }
} //  END Screen_CONFIG_VOLUME update_screen_with_keypad_data


//
///////////////////////////////////////////////////////////////////////////////
//
bool Screen_CONFIG_VOLUME::toggleSetting (const VOLUME_REMOVAL_TYPE toggleType)
{
   int  setting = guiglobs::cdsData.rwConfig.procedure.key_tbv_vol_setting;
   bool on      = !isEnabled(toggleType);

   if (on)
      setting |= toggleType;
   else
      setting &= (~toggleType);

   guiglobs::cdsData.rwConfig.procedure.key_tbv_vol_setting = setting;

   if (checkForValidConstraints())
      _txtNoneSelected.deallocate_resources();

   return isEnabled(toggleType);
}

bool Screen_CONFIG_VOLUME::checkForValidConstraints ()
{
   // This checks the three main constraints to make sure that at least one of them
   // is turned on.  Note that the DRBC body volume is not included here, as that
   // in itself is not sufficient since it only applies to DRBC procedures.
   //
   // Note the below logic assumes that you cannot turn on a volume limit that is
   // not visible (regionalized off).
   //
   const bool valid = isEnabled(VOLUME_REMOVAL_TBV)    ||
                      isEnabled(VOLUME_REMOVAL_WEIGHT) ||
                      isEnabled(VOLUME_REMOVAL_ML_PER_KG) ||
                      isEnabled(VOLUME_REMOVAL_PLATELET_VOL) ||
                      isEnabled(VOLUME_REMOVAL_PLASMA_VOL);

   return valid;
}


bool Screen_CONFIG_VOLUME::isEnabled (const VOLUME_REMOVAL_TYPE enableMask)
{
   return isEnabled(enableMask, guiglobs::cdsData.rwConfig.procedure.key_tbv_vol_setting);
}



bool Screen_CONFIG_VOLUME::isEnabled (const VOLUME_REMOVAL_TYPE enableMask, const int setting)
{
   return enableMask & setting;
}



void Screen_CONFIG_VOLUME::updateButton (Config_button_with_text& btn, const VOLUME_REMOVAL_TYPE enableMask, const int newValue)
{
   guistring tmpStr;
   sprintf_l10n(tmpStr, "%d", newValue);
   updateButton(btn, enableMask, tmpStr);
}



void Screen_CONFIG_VOLUME::updateButton (Config_button_with_text& btn, const VOLUME_REMOVAL_TYPE enableMask, const float newValue, const int numDigits = 0)
{
   char fmtStr[16];
   sprintf(fmtStr, "%%.%df", numDigits);

   guistring tmpStr;
   sprintf_l10n(tmpStr, fmtStr, newValue);
   updateButton(btn, enableMask, tmpStr);
}



void Screen_CONFIG_VOLUME::updateButton (Config_button_with_text& btn, const VOLUME_REMOVAL_TYPE enableMask, const guistring& newValue)
{
   const bool enabled = isEnabled(enableMask);
   btn.set_disabled(!enabled);
   btn.set_up_bitmap((enabled) ? CFG_Look::BUTTON_UP_NAME : BITMAP_CONFIG_BUTTON_GRAY);
   btn.set_button_text_color((enabled) ? YELLOW : GREY_LIGHT);
   btn.set_text_color((enabled) ? YELLOW : GREY_LIGHT);
   btn.set_text_direct(enabled ? newValue : guistring(DASHES));
}



//
// Toggle buttons
//
void Screen_CONFIG_VOLUME::processTbvToggle ()
{
   const bool active = toggleSetting(VOLUME_REMOVAL_TBV);
   _btnCfgTbvToggle.setActive(active);

   updateButton(_btnCfgTbvPercent, VOLUME_REMOVAL_TBV, guiglobs::cdsData.rwConfig.procedure.key_tbv_percent);
   _txtTBVPercent.set_color((active) ? YELLOW : GREY_LIGHT);  // only one will be allocated
   _txtTBVPercentJp.set_color((active) ? YELLOW : GREY_LIGHT);
}



void Screen_CONFIG_VOLUME::processPlateletMaxToggle ()
{
   const bool active = toggleSetting(VOLUME_REMOVAL_PLATELET_VOL);
   _btnCfgPlateletMaxToggle.setActive(active);

   updateButton(_btnCfgPlateletMaxVol, VOLUME_REMOVAL_PLATELET_VOL, guiglobs::cdsData.rwConfig.procedure.key_max_plasma_vol_during_platelet_collection);
}



void Screen_CONFIG_VOLUME::processPlasmaMaxToggle ()
{
   const bool active = toggleSetting(VOLUME_REMOVAL_PLASMA_VOL);
   _btnCfgPlasmaMaxToggle.setActive(active);

   updateButton(_btnCfgPlasmaMaxVol, VOLUME_REMOVAL_PLASMA_VOL, guiglobs::cdsData.rwConfig.procedure.key_max_plasma_vol_during_plasma_collection);
}


void Screen_CONFIG_VOLUME::processVolumeToggle ()
{
   const bool active = toggleSetting(VOLUME_REMOVAL_WEIGHT);
   _btnCfgVolToggle.setActive(active);

   updateButton(_btnCfgWeight, VOLUME_REMOVAL_WEIGHT, convertWeight(guiglobs::cdsData.rwConfig.procedure.key_weight_setting));
   updateButton(_btnCfgVolLimitLT, VOLUME_REMOVAL_WEIGHT, guiglobs::cdsData.rwConfig.procedure.key_weight_less_than_vol);
   updateButton(_btnCfgVolLimitGT, VOLUME_REMOVAL_WEIGHT, guiglobs::cdsData.rwConfig.procedure.key_weight_greater_than_vol);
}



void Screen_CONFIG_VOLUME::processMlPerKgToggle ()
{
   const bool active = toggleSetting(VOLUME_REMOVAL_ML_PER_KG);
   _btnCfgMlPerKgToggle.setActive(active);

   updateButton(_btnCfgMlPerKg, VOLUME_REMOVAL_ML_PER_KG, guiglobs::cdsData.rwConfig.procedure.key_ml_per_kg, 1);
}



void Screen_CONFIG_VOLUME::processDrbcVolToggle ()
{
   const bool active = toggleSetting(VOLUME_REMOVAL_DRBC_BODY_VOL);
   _btnCfgDrbcBodyVolToggle.setActive(active);

   updateButton(_btnCfgDrbcBodyVol, VOLUME_REMOVAL_DRBC_BODY_VOL, guiglobs::cdsData.rwConfig.procedure.key_drbc_body_vol, 1);
}

//
// Data buttons
//
void Screen_CONFIG_VOLUME::processTbvSetting ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_TBV_PERCENT).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_TBV_PERCENT).Max();

   display_min_max_keypad_limits(min, max, "%.0f");

   _btnCfgTbvPercent.create_keypad_string(keypad_string, KEY_TBV_PERCENT, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y, "##", max, min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}




void Screen_CONFIG_VOLUME::processPlateletMaxSetting ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_MAX_PLASMA_DURING_PLT_VOL).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_MAX_PLASMA_DURING_PLT_VOL).Max();

   display_min_max_keypad_limits(min, max, "%.0f");

   _btnCfgPlateletMaxVol.create_keypad_string(keypad_string, KEY_MAX_PLASMA_DURING_PLT_VOL, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y, "####", max, min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}



void Screen_CONFIG_VOLUME::processPlasmaMaxSetting ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_MAX_PLASMA_DURING_PLS_VOL).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_MAX_PLASMA_DURING_PLS_VOL).Max();

   display_min_max_keypad_limits(min, max, "%.0f");

   _btnCfgPlasmaMaxVol.create_keypad_string(keypad_string, KEY_MAX_PLASMA_DURING_PLS_VOL, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y, "####", max, min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}



void Screen_CONFIG_VOLUME::processWeightSetting ()
{
   float max = guiglobs::config_var_list.Var(KEY_WEIGHT_SETTING).Max() / weightConversionFactor();
   float min = guiglobs::config_var_list.Var(KEY_WEIGHT_SETTING).Min() / weightConversionFactor();

   //
   //  Special feature (see IT4796): set min to 90 if we are dealing in lbs
   //
   if (guiglobs::cdsData.rwConfig.lang.key_weight == LB_WEIGHT)
      min = MIN_WEIGHT_LB;

   display_min_max_keypad_limits(min, max, "%.0f");

   _btnCfgWeight.create_keypad_string(keypad_string, KEY_WEIGHT_SETTING, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y, "###", max, min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}



void Screen_CONFIG_VOLUME::processVolLimitLTSetting ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_WEIGHT_LESS_THAN_VOL).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_WEIGHT_LESS_THAN_VOL).Max();

   display_min_max_keypad_limits(min, max, "%.0f");
   _btnCfgVolLimitLT.create_keypad_string(keypad_string, KEY_WEIGHT_LESS_THAN_VOL, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y, "####", max, min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}



void Screen_CONFIG_VOLUME::processVolLimitGTSetting ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_WEIGHT_GREATER_THAN_VOL).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_WEIGHT_GREATER_THAN_VOL).Max();

   display_min_max_keypad_limits(min, max, "%.0f");
   _btnCfgVolLimitGT.create_keypad_string(keypad_string, KEY_WEIGHT_GREATER_THAN_VOL, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y, "####", max, min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}



void Screen_CONFIG_VOLUME::processMlPerKgSetting ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_ML_PER_KG).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_ML_PER_KG).Max();

   display_min_max_keypad_limits(min, max, "%.1f");

   _btnCfgMlPerKg.create_keypad_string(keypad_string, KEY_ML_PER_KG, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y, "##.#", max, min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}



void Screen_CONFIG_VOLUME::processDrbcVolSetting ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_DRBC_BODY_VOL).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_DRBC_BODY_VOL).Max();

   display_min_max_keypad_limits(min, max, "%.1f");

   _btnCfgDrbcBodyVol.create_keypad_string(keypad_string, KEY_DRBC_BODY_VOL, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y, "#.#", max, min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}


//
// Toggle callbacks
//
void cbCfgTbvToggle (void* data)
{
   ((Screen_CONFIG_VOLUME*)data)->processTbvToggle();
}
void cbCfgPlateletMaxToggle (void* data)
{
   ((Screen_CONFIG_VOLUME*)data)->processPlateletMaxToggle();
}
void cbCfgPlasmaMaxToggle (void* data)
{
   ((Screen_CONFIG_VOLUME*)data)->processPlasmaMaxToggle();
}


void cbCfgVolToggle (void* data)
{
   ((Screen_CONFIG_VOLUME*)data)->processVolumeToggle();
}
void cbCfgWeightToggle (void* data)
{
   ((Screen_CONFIG_VOLUME*)data)->processMlPerKgToggle();
}
void cbCfgDrbcTbvToggle (void* data)
{
   ((Screen_CONFIG_VOLUME*)data)->processDrbcVolToggle();
}


void cbCfgWeightSetting (void* data)
{
   ((Screen_CONFIG_VOLUME*)data)->processWeightSetting();
}
void cbCfgVolLimitLTSetting (void* data)
{
   ((Screen_CONFIG_VOLUME*)data)->processVolLimitLTSetting();
}
void cbCfgVolLimitGTSetting (void* data)
{
   ((Screen_CONFIG_VOLUME*)data)->processVolLimitGTSetting();
}
void cbCfgMlPerKgSetting (void* data)
{
   ((Screen_CONFIG_VOLUME*)data)->processMlPerKgSetting();
}
void cbCfgDrbcLimSetting (void* data)
{
   ((Screen_CONFIG_VOLUME*)data)->processDrbcVolSetting();
}

void cbCfgTbvSetting (void* data)
{
   ((Screen_CONFIG_VOLUME*)data)->processTbvSetting();
}
void cbCfgVolPlateletMax (void* data)
{
   ((Screen_CONFIG_VOLUME*)data)->processPlateletMaxSetting();
}
void cbCfgVolPlasmaMax (void* data)
{
   ((Screen_CONFIG_VOLUME*)data)->processPlasmaMaxSetting();
}

#if CPU==SIMNT
void Screen_CONFIG_VOLUME::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == UNIVERSAL)
   {
      DataLog (log_level_sim_gui_side_info) << "Traversing cfg_volume screen: "
                                            << traverseInfo.parameters[0] << "; " << traverseInfo.parameters[1] << endmsg;

      // TODO: THIS FUNCTION GOT CHANGED SOMEWHERE ALONG THE LINE.  THAT WILL PROBABLY AFFECT L10N SCRIPTS.
      // selectScreenLayout(traverseInfo.parameters[0], traverseInfo.parameters[1]);
      // int parameter = (int)traverseInfo.parameters[0];
      // if ( parameter == 1 )
      // {
      //   japan_screen = true;
      //   selectScreenLayout(true);
      // }
      // else if ( parameter == 2 )
      // {
      //   japan_screen = false;
      //   selectScreenLayout(false);
      // }
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif /* if CPU==SIMNT */

/* FORMAT HASH 7a3637c0b552c2c6d3eb17f61d759475 */

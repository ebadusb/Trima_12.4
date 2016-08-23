/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_CONFIG_VOLUME

*     File name:   cfg_dono.hpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/cfg_volume.hpp 1.17 2008/08/15 20:34:36Z mereusb Exp $
*/

// Single include define guardian
#ifndef SYSCONFIGVOLUME_HPP
#define SYSCONFIGVOLUME_HPP

/** include files **/

// CLASS INCLUDE
#include "screen_config.hpp"
#include "cfg_textabove_button.h"
#include "toggleButton.hpp"

//
///////////////////////////////////////////////////////////////////
//
class Screen_CONFIG_VOLUME
   : public Screen_Config
{
public:
   // CONSTRUCTOR
   Screen_CONFIG_VOLUME();

   // DESTRUCTOR
   virtual ~Screen_CONFIG_VOLUME();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive resources.
   virtual void deallocate_resources ();

   // CONFIG BUTTONS

   virtual void processTbvToggle ();
   virtual void processTbvSetting ();

   virtual void processPlateletMaxToggle ();
   virtual void processPlateletMaxSetting ();

   virtual void processPlasmaMaxToggle ();
   virtual void processPlasmaMaxSetting ();

   virtual void processVolumeToggle ();
   virtual void processWeightSetting ();
   virtual void processVolLimitLTSetting ();
   virtual void processVolLimitGTSetting ();

   virtual void processMlPerKgToggle ();
   virtual void processMlPerKgSetting ();

   virtual void processDrbcVolToggle ();
   virtual void processDrbcVolSetting ();

protected:
   typedef enum { FROM_LBS, TO_LBS } WEIGHT_CONVERSION;

   // The display text objects
   ToggleButton
      _btnCfgVolToggle,
      _btnCfgMlPerKgToggle,
      _btnCfgDrbcBodyVolToggle,
      _btnCfgTbvToggle,
      _btnCfgPlateletMaxToggle,
      _btnCfgPlasmaMaxToggle;

   Config_button_with_text
      _btnCfgWeight,
      _btnCfgVolLimitLT,
      _btnCfgVolLimitGT,
      _btnCfgMlPerKg,
      _btnCfgDrbcBodyVol,
      _btnCfgTbvPercent, // text separate to allow different positioning at regionalization
      _btnCfgPlateletMaxVol,
      _btnCfgPlasmaMaxVol;

   Bitmap
      _bmpUnderlineVol,
      _bmpUnderlineMlPerKg,
      _bmpUnderlineVolBox,
      _bmpUnderlineMlPerKgBox,
      _bmpUnderlineDrbcVol,
      _bmpUnderlineDrbcVolBox,
      _bmpUnderlineTbv,
      _bmpUnderlineTbvBox,
      _bmpUnderlinePlateletMax,
      _bmpUnderlinePlateletMaxBox,
      _bmpUnderlinePlasmaMax,
      _bmpUnderlinePlasmaMaxBox;

   Display_Text
      _txtTBVPercent,
      _txtTBVPercentJp,
      _txtModeHeading,
      _txtNoneSelected;

   bool japan_screen;

   static float weightConversionFactor ();
   static float convertWeight (float& setting, WEIGHT_CONVERSION conversion = FROM_LBS);

   static bool isEnabled (const VOLUME_REMOVAL_TYPE enableMask);
   static bool isEnabled (const VOLUME_REMOVAL_TYPE enableMask, const int setting);

   static void updateButton (Config_button_with_text& btn, const VOLUME_REMOVAL_TYPE enableMask, const int newValue);
   static void updateButton (Config_button_with_text& btn, const VOLUME_REMOVAL_TYPE enableMask, const float newValue, const int numDigits = 0);
   static void updateButton (Config_button_with_text& btn, const VOLUME_REMOVAL_TYPE enableMask, const guistring& newValue);

   virtual void selectScreenLayout (const bool useKgUnits);

protected:
   // MESSAGE NOTIFIES
   virtual void update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer);

   virtual void process_continue_button ();

   // VOLUME METHOD TRACKING
   virtual bool toggleSetting (const VOLUME_REMOVAL_TYPE toggleType);
   virtual bool checkForValidConstraints ();

#if CPU==SIMNT
protected:
   virtual void handleTraverseMessage ();
#endif /* if CPU==SIMNT */

};

#endif

/* FORMAT HASH 5089b521a28fa432187d14c35c19a113 */

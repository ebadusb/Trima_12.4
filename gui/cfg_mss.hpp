/*******************************************************************************
 *
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      cfg_mss.hpp
 *             Config Metered Storage Solutions
 *
 * ABSTRACT:   This class defines the config screen for metered storage.
 *******************************************************************************/


// Single include define guardian
#ifndef SYSCONFIG_MSS_HPP
#define SYSCONFIG_MSS_HPP


// CLASS INCLUDE
#include "screen_config.hpp"
#include "cfg_butt.hpp"
#include "trimamessages.h"

//
///////////////////////////////////////////////////////////////////
//

class Screen_CONFIG_MSS
   : public Screen_Config
{
   Config_button_with_text

   // RBC Buttons
            _btnPasToggle,
            _btnPasDefaultBagVol,
            _btnPasSplitToggle,
            _btnPasBagVolOverride,

   // Platlet Buttons
            _btnRasToggle,
            _btnRasDefaultBagVol,

   // MSS General Buttons
            _btnPreAasFlags;

   Bitmap

   // Icons
      _bmpPasIcon,
      _bmpRasIcon,
      _bmpCompIcon,
   // Separator Lines
      _bmpVertLine,
      _bmpHalfLine,
      _bmpHorizLine;

   Display_Text
      _txtRbcProcLbl,
      _txtCompProcLbl,
      _txtPltProcLbl;

   ConfigButton    config_btn_compl_pls;
   Display_Text    comp_pls_btn_label;

   FeatureChanged* _featureChangedMsg;

protected:

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   void checkForTimeOnlyConfig ();

   static void featureChangedMsgHandler ();

public:
   // CONSTRUCTOR
   Screen_CONFIG_MSS();

   // DESTRUCTOR
   virtual ~Screen_CONFIG_MSS();
   virtual void processPltMssToggleBtn ();
   virtual void processRbcMssToggleBtn ();
   void         processRbcSsVolBtn ();
   void         processPltSsVolBtn ();
   virtual void update_screen_with_keypad_data  (const KEYPAD_RETURN_STRUCT& return_keypad_buffer);
   virtual void processPltSplit ();
   virtual void processPasOverrideBagvolToggleBtn ();
   virtual void processShowFlagsToggleBtn ();

   virtual void updateAllBtns ();
   virtual void updatePasToggleBtn ();
   virtual void updateRasToggleBtn ();
   virtual void updatePasSplitBtn ();
   virtual void updatePltSsBagVolBtn ();
   virtual void updateRbcSsBagVolBtn ();
   virtual void updatePasBagVolOverrideBtn ();
   virtual void updateCompPLSBtn ();
   virtual void updateShowFlagsBtn ();
   virtual void process_compl_pls ();


   void chooseToggleButtonAppearance (bool isOn, ConfigButton& btn,
                                      TEXT_STRING_ID onTxt = textListboxItemCfgOn,
                                      TEXT_STRING_ID offTxt = textListboxItemCfgOff,
                                      BITMAP_ID onBmp = CFG_Look::BUTTON_DOWN_NAME,
                                      BITMAP_ID offBmp = CFG_Look::BUTTON_UP_NAME);

};
// End of single include define guardian
#endif

/* FORMAT HASH 87970cac637ca8eb892e4133d18ba134 */

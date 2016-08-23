/*******************************************************************************
 *
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      cfg_mss.cpp
 *             Config Metered Storage Solutions
 *
 * ABSTRACT:   This class defines the config screen for metered storage.
 *
 *******************************************************************************/

#include <vxworks.h>

// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "an_alarm.h"

// BASE CLASS INCLUDE
#include "cfg_mss.hpp"
#include "statline.hpp"

// DEFINITIONS FOR PLACEMENT
#define __CFG_MSS_BTN_TOP__ 145
#define __CFG_MSS_BTN_DELTA__ 75

#define __CFG_MSS_ROW_Y__(__CFG_MSS_ROW_NUM__) __CFG_MSS_BTN_TOP__ + (((__CFG_MSS_ROW_NUM__)-1) * __CFG_MSS_BTN_DELTA__)

#define __CFG_MSS_COL_1__ 200
#define __CFG_MSS_COL_2__ 515

#define PR_KEYPAD_X 290
#define PR_KEYPAD_Y 120

#define R_KEYPAD_X 260
#define R_KEYPAD_Y 120

// button callbacks
static void cbPasToggleBtn (void* data);
static void cbPltSsDefBagVolBtn (void* data);
static void cbPasSplitBtn (void* data);
static void cbPasOverrideBagVolBtn (void* data);
static void cbToggleRbcMss (void* data);
static void cbRbcSsDefBagVolBtn (void* data);
static void cbToggleShowFlags (void* data);
static void cb_cfg_compl_pls (void* data);


///////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_CONFIG_MSS::Screen_CONFIG_MSS() :

   Screen_Config(TEAL, GUI_SCREEN_CONFIG_METERED_SOLUTIONS, textCfgScreenLabelAutoProc),

// BUTTONS
   _btnPasToggle           (__CFG_MSS_COL_1__,
                            __CFG_MSS_ROW_Y__(1),
                            guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on,
                            KEY_MSS_PLT_ON,
                            CFG_Look::BUTTON_UP_NAME,
                            CFG_Look::BUTTON_DOWN_NAME,
                            GUI_BUTTON_CONFIG_PPC_ON_OFF,
                            textButtonLabelCfgLeftAutoProcPasToggle,
                            CFG_Look::FONT_COLOR,
                            CFG_Look::FONT_ALIGN,
                            CFG_Look::FONT_SIZE),

   _btnPasDefaultBagVol    (__CFG_MSS_COL_1__,
                            __CFG_MSS_ROW_Y__(2),
                            guiglobs::cdsData.rwConfig.procedure.key_plt_def_bag_vol,
                            KEY_PLT_DEF_BAG_VOL,
                            CFG_Look::BUTTON_UP_NAME,
                            CFG_Look::BUTTON_DOWN_NAME,
                            GUI_BUTTON_CONFIG_DEFAULT_PLT_SS_BAG_VOLM,
                            textButtonLabelCfgLeftAutoProcPasBagVol,
                            YELLOW,
                            CFG_Look::FONT_ALIGN,
                            CFG_Look::FONT_SIZE),

   _btnPasBagVolOverride (__CFG_MSS_COL_1__,
                          __CFG_MSS_ROW_Y__(3),
                          guiglobs::cdsData.rwConfig.procedure.key_override_pas_bag_vol,
                          KEY_OVERRIDE_PAS_BAG_VOL,
                          CFG_Look::BUTTON_UP_NAME,
                          CFG_Look::BUTTON_DOWN_NAME,
                          GUI_BUTTON_CONFIG_OVERRIDE_PAS_BAG_VOL,
                          textButtonLabelCfgLeftAutoProcPasOverrideToggle,
                          YELLOW,
                          CFG_Look::FONT_ALIGN,
                          CFG_Look::FONT_SIZE),

   _btnPasSplitToggle      (__CFG_MSS_COL_1__,
                            __CFG_MSS_ROW_Y__(4),
                            guiglobs::cdsData.rwConfig.procedure.key_plt_mss_split_notif,
                            KEY_PLT_MSS_SPLIT_NOTIF,
                            CFG_Look::BUTTON_UP_NAME,
                            CFG_Look::BUTTON_DOWN_NAME,
                            GUI_BUTTON_CONFIG_PLT_MSS_SPLIT_NOTIF,
                            textButtonLabelCfgLeftAutoProcPasSplitNotifToggle,
                            YELLOW,
                            CFG_Look::FONT_ALIGN,
                            CFG_Look::FONT_SIZE),


   _btnRasToggle       (__CFG_MSS_COL_2__,
                        __CFG_MSS_ROW_Y__(1),
                        guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on,
                        KEY_MSS_RBC_ON,
                        CFG_Look::BUTTON_UP_NAME,
                        CFG_Look::BUTTON_DOWN_NAME,
                        GUI_BUTTON_CONFIG_RBC_SOL_TOGGLE,
                        textButtonLabelCfgLeftAutoProcRasToggle,
                        YELLOW,
                        CFG_Look::FONT_ALIGN,
                        CFG_Look::FONT_SIZE),

   _btnRasDefaultBagVol(__CFG_MSS_COL_2__,
                        __CFG_MSS_ROW_Y__(2),
                        guiglobs::cdsData.rwConfig.procedure.key_rbc_def_bag_vol,
                        KEY_RBC_DEF_BAG_VOL,
                        CFG_Look::BUTTON_UP_NAME,
                        CFG_Look::BUTTON_DOWN_NAME,
                        GUI_BUTTON_MSS_BAG_VOL_RBC,
                        textButtonLabelCfgLeftAutoProcRasBagVol,
                        YELLOW,
                        CFG_Look::FONT_ALIGN,
                        CFG_Look::FONT_SIZE),

   _btnPreAasFlags(__CFG_MSS_COL_2__,
                   __CFG_MSS_ROW_Y__(3),
                   guiglobs::cdsData.rwConfig.procedure.key_show_pre_aas_flags,
                   KEY_SHOW_PRE_AAS_FLAGS,
                   CFG_Look::BUTTON_UP_NAME,
                   CFG_Look::BUTTON_DOWN_NAME,
                   GUI_BUTTON_CONFIG_SHOW_PRE_AAS_FLAGS,
                   textButtonLabelCfgLeftAutoProcExtraFlagMsgs,
                   YELLOW,
                   CFG_Look::FONT_ALIGN,
                   CFG_Look::FONT_SIZE),



   config_btn_compl_pls (__CFG_MSS_COL_2__,
                         __CFG_MSS_ROW_Y__(4),
                         guiglobs::cdsData.rwConfig.predict.key_complementary_plasma,
                         KEY_COMPLEMENTARY_PLASMA,
                         CFG_Look::BUTTON_UP_NAME,
                         CFG_Look::BUTTON_DOWN_NAME,
                         GUI_BUTTON_CONFIG_COMPLEMENTARY_PLASMA),


   comp_pls_btn_label(TEXT_CFG_COMP_PLS_LABEL),



// TEXT
   _txtRbcProcLbl(textMiscCfgAutoProcRasLabel),
   _txtPltProcLbl(textMiscCfgAutoProcPasLabel),
   _txtCompProcLbl(TEXT_CFG_COMP_PLS_LABEL_MAIN),

// BITMAPS
   _bmpPasIcon (25, 107, BITMAP_CONFIG_PAS_ICON),
   _bmpRasIcon (350, 107, BITMAP_CONFIG_RAS_ICON),
   _bmpCompIcon(350, 290, BITMAP_CONFIG_COMP_PLS_ICON),

   _bmpHalfLine (328, 280, BITMAP_CONFIG_AUTOPROC_HORIZ_HALF_LINE),
   _bmpVertLine (324, 103, BITMAP_CONFIG_AUTOPROC_VERT_LINE),
   _bmpHorizLine(326, 280, BITMAP_CONFIG_AUTOPROC_HORIZ_LINE),
   _featureChangedMsg(NULL)
{
   if (_featureChangedMsg == NULL )
   {
      _featureChangedMsg = new FeatureChanged(Callback<Screen_CONFIG_MSS>(&Screen_CONFIG_MSS::featureChangedMsgHandler));
   }

   if (!Software_CDS::GetInstance().getFeature(AllowExtraWbcMessages))
   {
      guiglobs::cdsData.rwConfig.procedure.key_show_pre_aas_flags = false;
   }
}

//
////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_CONFIG_MSS::~Screen_CONFIG_MSS()
{
   Screen_CONFIG_MSS::deallocate_resources ();

   delete _featureChangedMsg;
}

//
////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_CONFIG_MSS::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Screen_Config::allocate_resources (allocation_parameter);

   _btnPasToggle.allocate_resources (*this);
   _btnPasDefaultBagVol.allocate_resources (*this);
   _btnPasSplitToggle.allocate_resources(*this);
   _btnPasBagVolOverride.allocate_resources(*this);
   _btnRasToggle.allocate_resources(*this);
   _btnRasDefaultBagVol.allocate_resources(*this);

   _bmpPasIcon.allocate_resources(*this);
   _bmpRasIcon.allocate_resources(*this);


   if ((bool)Software_CDS::GetInstance().getFeature(ComplementaryPlasmaAvailable))
      _bmpCompIcon.allocate_resources(*this);

   _bmpVertLine.allocate_resources(*this);
   _bmpHorizLine.allocate_resources(*this);

   if ((bool)Software_CDS::GetInstance().getFeature(ComplementaryPlasmaAvailable))
      _bmpHalfLine.allocate_resources(*this);

   _btnPasToggle.set_callback (cbPasToggleBtn, (void*)this);
   _btnPasDefaultBagVol.set_callback (cbPltSsDefBagVolBtn, (void*)this);
   _btnPasSplitToggle.set_callback(cbPasSplitBtn, (void*)this);
   _btnPasBagVolOverride.set_callback(cbPasOverrideBagVolBtn, (void*)this);
   _btnRasToggle.set_callback(cbToggleRbcMss, (void*)this);
   _btnRasDefaultBagVol.set_callback(cbRbcSsDefBagVolBtn, (void*)this);

   // This if clause will be important in 6.1.
   if (Software_CDS::GetInstance().getFeature(AllowExtraWbcMessages))
   {
      _bmpHorizLine.allocate_resources(*this);
      _btnPreAasFlags.allocate_resources(*this);
      _btnPreAasFlags.set_callback(cbToggleShowFlags, (void*)this);
   }
   else
   {
      _bmpHorizLine.deallocate_resources();
      _btnPreAasFlags.deallocate_resources();
      _btnPreAasFlags.disable_callbacks();
   }

   _txtRbcProcLbl.allocate_resources(*this);
   _txtPltProcLbl.allocate_resources(*this);

   if ((bool)Software_CDS::GetInstance().getFeature(ComplementaryPlasmaAvailable))
      _txtCompProcLbl.allocate_resources(*this);



//    if (  (bool)Software_CDS::GetInstance().complementary_plasma_enabled )
//    {
//        comp_pls_btn_label.allocate_resources (*this);
//        config_btn_compl_pls.allocate_resources (*this);
//
//
//        if (!((bool)guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on))
//        {
//            config_btn_compl_pls.set_disabled(true);
//            config_btn_compl_pls.set_button_text_color(GREY_LIGHT);
//            config_btn_compl_pls.set_up_bitmap(BITMAP_CONFIG_BUTTON_GRAY);
//        } else {
//
//            config_btn_compl_pls.set_disabled(false);
//            config_btn_compl_pls.set_button_text_color(YELLOW);
//
//            config_btn_compl_pls.set_up_bitmap((guiglobs::cdsData.rwConfig.predict.key_complementary_plasma)?
//                                                       CFG_Look::BUTTON_DOWN_NAME : CFG_Look::BUTTON_UP_NAME);
//
//            config_btn_compl_pls.set_callback (cb_cfg_compl_pls, (void *)this);
//        }
//
//
//        chooseToggleButtonAppearance(guiglobs::cdsData.rwConfig.predict.key_complementary_plasma, config_btn_compl_pls);
//    }
//

   updateCompPLSBtn();

   updateAllBtns();

   checkForTimeOnlyConfig();
} // END of Screen_CONFIG_RBC ALLOCATE_RESOURCES


//
////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_CONFIG_MSS::deallocate_resources ()
{
   // deactivate the status line before leaving
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

   _btnPasToggle.deallocate_resources();
   _btnPasDefaultBagVol.deallocate_resources();
   _btnPasSplitToggle.deallocate_resources();
   _btnPasBagVolOverride.deallocate_resources();
   _btnRasToggle.deallocate_resources();
   _btnRasDefaultBagVol.deallocate_resources();
   _btnPreAasFlags.deallocate_resources();

   _bmpCompIcon.deallocate_resources();

   _bmpPasIcon.deallocate_resources();
   _bmpRasIcon.deallocate_resources();
   _bmpVertLine.deallocate_resources();
   _bmpHorizLine.deallocate_resources();
   _bmpHalfLine.deallocate_resources();

   _txtRbcProcLbl.deallocate_resources();
   _txtPltProcLbl.deallocate_resources();
   _txtCompProcLbl.deallocate_resources();

   config_btn_compl_pls.deallocate_resources();
   comp_pls_btn_label.deallocate_resources();


   // deallocate parent's resources
   Screen_Config::deallocate_resources ();
}  // END of Screen_CONFIG_RBC DEALLOCATE_RESOURCES

void cbToggleRbcMss (void* data)
{
   ((Screen_CONFIG_MSS*)data)->processRbcMssToggleBtn();
}

void Screen_CONFIG_MSS::processRbcMssToggleBtn ()
{
   // Toggle the data
   guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on =
      (guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on == 0);

   // Update the button
   updateRasToggleBtn();
}
//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


void cbPltSsDefBagVolBtn (void* data)
{
   ((Screen_CONFIG_MSS*)data)->processPltSsVolBtn();
}

//
void cbRbcSsDefBagVolBtn (void* data)
{
   ((Screen_CONFIG_MSS*)data)->processRbcSsVolBtn();
}


void Screen_CONFIG_MSS::processRbcSsVolBtn ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_RBC_DEF_BAG_VOL).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_RBC_DEF_BAG_VOL).Max();

   display_min_max_keypad_limits(min, max, "%.0f");

   Screen_Config::build_keypad_string (keypad_string,
                                       KEY_RBC_DEF_BAG_VOL,
                                       PR_KEYPAD_X,
                                       PR_KEYPAD_Y,
                                       "###",
                                       max,
                                       min);

   guiglobs::apheresis_status_line->unparent_status_line (guiglobs::apheresis_status_line->get_owning_screen ());
   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}

void Screen_CONFIG_MSS::processPltSsVolBtn ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_PLT_DEF_BAG_VOL).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_PLT_DEF_BAG_VOL).Max();

   display_min_max_keypad_limits(min, max, "%.0f");

   Screen_Config::build_keypad_string (keypad_string,
                                       KEY_PLT_DEF_BAG_VOL,
                                       PR_KEYPAD_X,
                                       PR_KEYPAD_Y,
                                       "####",
                                       max,
                                       min);

   guiglobs::apheresis_status_line->unparent_status_line (guiglobs::apheresis_status_line->get_owning_screen ());
   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}

////////////////////////////////////////////////////////////////////////////////
// callbacks
void cbPasToggleBtn (void* data)
{
   ((Screen_CONFIG_MSS*)data)->processPltMssToggleBtn();
}
/////////////////////////////////////////////////////////////////
void Screen_CONFIG_MSS::processPltMssToggleBtn ()
{
   // Toggle the data
   guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on =
      (guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on == 0);

   if (guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on == 0)
   {
      guiglobs::cdsData.rwConfig.predict.key_complementary_plasma = 0;
   }

   // Update the button
   updatePasToggleBtn();



   chooseToggleButtonAppearance(guiglobs::cdsData.rwConfig.predict.key_complementary_plasma, config_btn_compl_pls);

}

/////////////////////////////////////////////////////////////////
void cbPasSplitBtn (void* data)
{
   ((Screen_CONFIG_MSS*)data)->processPltSplit();
}
/////////////////////////////////////////////////////////////////
void Screen_CONFIG_MSS::processPltSplit ()
{
   // Toggle the data
   guiglobs::cdsData.rwConfig.procedure.key_plt_mss_split_notif =
      (guiglobs::cdsData.rwConfig.procedure.key_plt_mss_split_notif == 0);

   // Update the button
   updatePasSplitBtn();
}

///////////////////////////////////////////////////////
void cbToggleShowFlags (void* data)
{
   ((Screen_CONFIG_MSS*)data)->processShowFlagsToggleBtn();
}

void Screen_CONFIG_MSS::processShowFlagsToggleBtn ()
{
   // Toggle the config value
   guiglobs::cdsData.rwConfig.procedure.key_show_pre_aas_flags =
      (guiglobs::cdsData.rwConfig.procedure.key_show_pre_aas_flags == 0);

   // Update the button
   updateShowFlagsBtn();
}

/////////////////////////////////////////////////////////////////
void cbPasOverrideBagVolBtn (void* data)
{
   ((Screen_CONFIG_MSS*)data)->processPasOverrideBagvolToggleBtn();
}
/////////////////////////////////////////////////////////////////
void Screen_CONFIG_MSS::processPasOverrideBagvolToggleBtn ()
{
   // Toggle the data
   guiglobs::cdsData.rwConfig.procedure.key_override_pas_bag_vol =
      (guiglobs::cdsData.rwConfig.procedure.key_override_pas_bag_vol == 0);

   // Update the button
   updatePasBagVolOverrideBtn();
}

/////////////////////////////////////////////////////////////////
// Message handler for KEYPADS /////////////////
void Screen_CONFIG_MSS::update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer)
{
   turn_off_min_max_keypad_text();

   if ((return_keypad_buffer.keypad_return_value) != -1)
   {
      int the_data = atoi(return_keypad_buffer.keypad_return_string);

      switch (return_keypad_buffer.config_data_type)
      {
         case KEY_PLT_DEF_BAG_VOL :
            guiglobs::cdsData.rwConfig.procedure.key_plt_def_bag_vol = the_data;
            updatePltSsBagVolBtn();
            break;

         case KEY_RBC_DEF_BAG_VOL :
            guiglobs::cdsData.rwConfig.procedure.key_rbc_def_bag_vol = the_data;
            updateRbcSsBagVolBtn();
            break;

         default :
            DataLog (log_level_gui_error) << "Unable to identify keypad data." << endmsg;
            break;
      }
   }
   else return;
} // end of method

void Screen_CONFIG_MSS::updateAllBtns ()
{   // Update the AAS toggle buttons (the updates will trickle down)
   updatePasToggleBtn();
   updateRasToggleBtn();
   updateShowFlagsBtn();
}

void Screen_CONFIG_MSS::updatePasToggleBtn ()
{
   bool pasOn = guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on;

   _btnPasToggle.set_string_id((pasOn) ? textListboxItemCfgOn : textListboxItemCfgOff);
   _btnPasToggle.set_up_bitmap((pasOn) ? CFG_Look::BUTTON_DOWN_NAME : CFG_Look::BUTTON_UP_NAME);

   updatePasSplitBtn();
   updatePltSsBagVolBtn();
   updatePasBagVolOverrideBtn();

   updateCompPLSBtn();


}

void Screen_CONFIG_MSS::updateRasToggleBtn ()
{
   bool rasOn = guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on;

   _btnRasToggle.set_string_id((rasOn) ? textListboxItemCfgOn : textListboxItemCfgOff);
   _btnRasToggle.set_up_bitmap((rasOn) ? CFG_Look::BUTTON_DOWN_NAME : CFG_Look::BUTTON_UP_NAME);

   updateRbcSsBagVolBtn();
}

void Screen_CONFIG_MSS::updateShowFlagsBtn ()
{
   bool showFlags = guiglobs::cdsData.rwConfig.procedure.key_show_pre_aas_flags;

   _btnPreAasFlags.set_string_id((showFlags) ? textListboxItemCfgOn : textListboxItemCfgOff);
   _btnPreAasFlags.set_up_bitmap((showFlags) ? CFG_Look::BUTTON_DOWN_NAME : CFG_Look::BUTTON_UP_NAME);
}

void Screen_CONFIG_MSS::updatePasSplitBtn ()
{
   bool pasOn   = guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on;
   bool splitOn = guiglobs::cdsData.rwConfig.procedure.key_plt_mss_split_notif;

   _btnPasSplitToggle.set_disabled(!pasOn);
   _btnPasSplitToggle.set_string_id((splitOn) ? textListboxItemCfgOn : textListboxItemCfgOff);
   _btnPasSplitToggle.set_button_text_color((pasOn) ? YELLOW : GREY_LIGHT);

   if (pasOn) _btnPasSplitToggle.set_up_bitmap((splitOn) ? CFG_Look::BUTTON_DOWN_NAME : CFG_Look::BUTTON_UP_NAME);
   else _btnPasSplitToggle.set_up_bitmap(BITMAP_CONFIG_BUTTON_GRAY);
}

void Screen_CONFIG_MSS::updatePltSsBagVolBtn ()
{
   bool      pasOn = guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on;
   guistring btnTxt;
   btnTxt.initialize();
   sprintf(btnTxt, "%d", guiglobs::cdsData.rwConfig.procedure.key_plt_def_bag_vol);

   _btnPasDefaultBagVol.set_text_direct (btnTxt.length(), btnTxt);

   _btnPasDefaultBagVol.set_disabled(!pasOn);
   _btnPasDefaultBagVol.set_button_text_color((pasOn) ? YELLOW : GREY_LIGHT);
   _btnPasDefaultBagVol.set_up_bitmap((pasOn) ? CFG_Look::BUTTON_UP_NAME : BITMAP_CONFIG_BUTTON_GRAY);
}

void Screen_CONFIG_MSS::updateRbcSsBagVolBtn ()
{
   bool      rasOn = guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on;
   guistring btnTxt;
   btnTxt.initialize();
   sprintf(btnTxt, "%d", guiglobs::cdsData.rwConfig.procedure.key_rbc_def_bag_vol);

   _btnRasDefaultBagVol.set_text_direct (btnTxt.length(), btnTxt);

   _btnRasDefaultBagVol.set_disabled(!rasOn);
   _btnRasDefaultBagVol.set_button_text_color((rasOn) ? YELLOW : GREY_LIGHT);
   _btnRasDefaultBagVol.set_up_bitmap((rasOn) ? CFG_Look::BUTTON_UP_NAME : BITMAP_CONFIG_BUTTON_GRAY);
}

void Screen_CONFIG_MSS::updatePasBagVolOverrideBtn ()
{
   bool pasOn      = guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on;
   bool overrideOn = guiglobs::cdsData.rwConfig.procedure.key_override_pas_bag_vol;

   _btnPasBagVolOverride.set_disabled(!pasOn);
   _btnPasBagVolOverride.set_string_id((overrideOn) ? textListboxItemCfgOn : textListboxItemCfgOff);
   _btnPasBagVolOverride.set_button_text_color((pasOn) ? YELLOW : GREY_LIGHT);

   if (pasOn) _btnPasBagVolOverride.set_up_bitmap((overrideOn) ? CFG_Look::BUTTON_DOWN_NAME : CFG_Look::BUTTON_UP_NAME);
   else _btnPasBagVolOverride.set_up_bitmap(BITMAP_CONFIG_BUTTON_GRAY);
}

void Screen_CONFIG_MSS::checkForTimeOnlyConfig ()
{
   if (guiglobs::timeOnlyConfigMode)
   {
      DataLog (log_level_gui_info) << "Buttons should be disabled." << endmsg;

      bool pasOn      = guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on;
      bool rasOn      = guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on;
      bool overrideOn = pasOn && guiglobs::cdsData.rwConfig.procedure.key_override_pas_bag_vol;
      bool splitOn    = pasOn && guiglobs::cdsData.rwConfig.procedure.key_plt_mss_split_notif;
      bool showFlags  = guiglobs::cdsData.rwConfig.procedure.key_show_pre_aas_flags;

      disableConfigBtn(_btnPasDefaultBagVol);
      disableConfigBtn(_btnRasDefaultBagVol);

      disableConfigBtn(_btnPasToggle, CHOOSE_DISABLED_BITMAP(pasOn));
      disableConfigBtn(_btnRasToggle, CHOOSE_DISABLED_BITMAP(rasOn));
      disableConfigBtn(_btnPasBagVolOverride, CHOOSE_DISABLED_BITMAP(overrideOn));
      disableConfigBtn(_btnPasSplitToggle, CHOOSE_DISABLED_BITMAP(splitOn));
      disableConfigBtn(_btnPreAasFlags, CHOOSE_DISABLED_BITMAP(showFlags));

      disableConfigTxt(_txtRbcProcLbl);
      disableConfigTxt(_txtPltProcLbl);
   }
}

////////////////////////////////////////////////////////////////////////////////
//
void Screen_CONFIG_MSS::chooseToggleButtonAppearance (bool isOn, ConfigButton& btn,
                                                      TEXT_STRING_ID onTxt = textListboxItemCfgOn,
                                                      TEXT_STRING_ID offTxt = textListboxItemCfgOff,
                                                      BITMAP_ID onBmp = CFG_Look::BUTTON_DOWN_NAME,
                                                      BITMAP_ID offBmp = CFG_Look::BUTTON_UP_NAME)
{

   if (!((bool)guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on))
   {
      btn.set_up_bitmap(offBmp);
      btn.set_string_id(offTxt);

      config_btn_compl_pls.set_button_text_color(GREY_LIGHT);
      config_btn_compl_pls.set_up_bitmap(BITMAP_CONFIG_BUTTON_GRAY);

      DataLog (log_level_gui_info) << "comp pls disabled." << endmsg;

   }
   else
   {

      config_btn_compl_pls.set_button_text_color(YELLOW);

      config_btn_compl_pls.set_up_bitmap((guiglobs::cdsData.rwConfig.predict.key_complementary_plasma) ?
                                         CFG_Look::BUTTON_DOWN_NAME : CFG_Look::BUTTON_UP_NAME);



      btn.set_up_bitmap((isOn) ? onBmp : offBmp);
      btn.set_string_id((isOn) ? onTxt : offTxt);
   }


}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void cb_cfg_compl_pls (void* data)
{
   ((Screen_CONFIG_MSS*)data)->process_compl_pls();
}
void Screen_CONFIG_MSS::process_compl_pls ()
{


   guiglobs::cdsData.rwConfig.predict.key_complementary_plasma = (bool)(!guiglobs::cdsData.rwConfig.predict.key_complementary_plasma);

   chooseToggleButtonAppearance(guiglobs::cdsData.rwConfig.predict.key_complementary_plasma, config_btn_compl_pls);


}

void Screen_CONFIG_MSS::updateCompPLSBtn ()
{


   if (  (bool)Software_CDS::GetInstance().getFeature(ComplementaryPlasmaAvailable))
   {
      comp_pls_btn_label.allocate_resources (*this);
      config_btn_compl_pls.allocate_resources (*this);


      if (!((bool)guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on))
      {
         config_btn_compl_pls.set_disabled(true);
         config_btn_compl_pls.set_button_text_color(GREY_LIGHT);
         config_btn_compl_pls.set_up_bitmap(BITMAP_CONFIG_BUTTON_GRAY);
      }
      else
      {

         config_btn_compl_pls.set_disabled(false);
         config_btn_compl_pls.set_button_text_color(YELLOW);

         config_btn_compl_pls.set_up_bitmap((guiglobs::cdsData.rwConfig.predict.key_complementary_plasma) ?
                                            CFG_Look::BUTTON_DOWN_NAME : CFG_Look::BUTTON_UP_NAME);

         config_btn_compl_pls.set_callback (cb_cfg_compl_pls, (void*)this);
      }


      chooseToggleButtonAppearance(guiglobs::cdsData.rwConfig.predict.key_complementary_plasma, config_btn_compl_pls);
   }

}

void Screen_CONFIG_MSS::featureChangedMsgHandler ()
{
   if (!Software_CDS::GetInstance().getFeature(AllowExtraWbcMessages))
   {
      guiglobs::cdsData.rwConfig.procedure.key_show_pre_aas_flags = false;
   }
}

/* FORMAT HASH 2834e3e691e10652e42cc46fd8988fcb */

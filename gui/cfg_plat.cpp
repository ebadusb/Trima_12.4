/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  cfg_plat.cpp

*     File name:   Screen_CONFIG_PLATELET

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************


*/
#include <vxworks.h>
#include <math.h>
#include <string>


// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "an_alarm.h"
#include "software_cds.h"
#include "cobeconfig.h"

#include "mathdefs.hpp"

// BASE CLASS INCLUDE
#include "cfg_plat.hpp"
#include "statline.hpp"

/** defines **/
#define PROD_NUM_COLUMN       19
#define YIELD_COLUMN         120
#define FINAL_CONC_COLUMN    210
#define CO_COLUMN            300
#define VOLUME_COLUMN        390
#define COLLECT_CONC_COLUMN  449
#define SS_COLUMN            534

#define CFG_PLT_Y_COORD(__ROWNUM__) 168 + ((__ROWNUM__) * 65)
#define CFGPLTLINEROW(__ROWNUM__) 219 + ((__ROWNUM__) * 65)

#define PLT_BUTTON_PROD(row) ((row==0) ? (GUI_BUTTON_CONFIG_PLT_PROD_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PLT_PROD_2) : (GUI_BUTTON_CONFIG_PLT_PROD_3)))
#define PLT_BUTTON_YIELD(row) ((row==0) ? (GUI_BUTTON_CONFIG_PLT_YIELD_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PLT_YIELD_2) : (GUI_BUTTON_CONFIG_PLT_YIELD_3)))
#define PLT_BUTTON_FINAL_CONC(row) ((row==0) ? (GUI_BUTTON_CONFIG_PLT_FINAL_CONC_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PLT_FINAL_CONC_2) : (GUI_BUTTON_CONFIG_PLT_FINAL_CONC_3)))
#define PLT_BUTTON_PAS_TOGGLE(row) ((row==0) ? (GUI_BUTTON_CONFIG_PLT_PAS_TOGGLE_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PLT_PAS_TOGGLE_2) : (GUI_BUTTON_CONFIG_PLT_PAS_TOGGLE_3)))
#define PLT_BUTTON_PLASMA_PERCENT(row) ((row==0) ? (GUI_BUTTON_CONFIG_PLT_PLASMA_PERCENT_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PLT_PLASMA_PERCENT_2) : (GUI_BUTTON_CONFIG_PLT_PLASMA_PERCENT_3)))
#define PLT_BUTTON_VOL(row) ((row==0) ? (GUI_BUTTON_CONFIG_PLT_VOL_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PLT_VOL_2) : (GUI_BUTTON_CONFIG_PLT_VOL_3)))

#define ROW0 BITMAP_BUTTON_CFGPLT_ROW1_Y
#define ROW1 BITMAP_BUTTON_CFGPLT_ROW2_Y
#define ROW2 BITMAP_BUTTON_CFGPLT_ROW3_Y

#define LABEL_AREA 125

#define PROCEDURE_INDICTOR_X 10
#define PROCEDURE_INDICTOR_Y 35
#define PR_KEYPAD_X          5
#define PR_KEYPAD_Y          150

#define VOL_MIN 50.0f

#define R_KEYPAD_X 260
#define R_KEYPAD_Y 120

#define CFG_PLT_MAG_GLASS_X 547
#define CFG_PLT_MAG_GLASS_TOP_Y 178
#define CFG_PLT_MAG_GLASS_MIDDLE_Y 241
#define CFG_PLT_MAG_GLASS_BOTTOM_Y 307

#define roundToTenth(__OPERAND__) (round((__OPERAND__) * 10.0f) / 10)
#define floorToTenth(__OPERAND__) (floor((__OPERAND__) * 10.0f) / 10)
#define ceilToTenth(__OPERAND__)  (ceil((__OPERAND__) * 10.0f) / 10)

static void cbButton         (void* data);
static void cbToggleButton   (void* data);
static void cbPltYsfButton   (void* data);
static void cbMaleOnlyPltBtn (void* data);
static void cbUpButton       (void* data);
static void cbDownButton     (void* data);

bool Screen_CONFIG_PLATELET::_btnLock = false;
// static void cb_pcarry_toggle(void * data);
// static void cb_ptf_toggle(void *data);

///////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR


Screen_CONFIG_PLATELET::Screen_CONFIG_PLATELET()
   : Screen_Config(TEAL, GUI_SCREEN_CONFIG_PLATELET, textCfgScreenLabelPlt),
     _txtYieldLbl(textCfgColLabelCfgPltYield),
     _txtVolLbl(textCfgColLabelCfgPltVol),
     _txtFinalConcLbl(textCfgColLabelCfgPltFinalConc),
     _txtCollectConcLbl(textCfgColLabelCfgPltCollectConc),
     _txtPasVolLbl(textCfgColLabelCfgPltPasVol),
     _txtPlasmaPercentLbl(textCfgColLabelCfgPltPercentPlasma),
     _toThe3rdCollectConc(textCfgColLabelCfgPltCollectConcExponent),
     _toThe3rdFinalConc(textCfgColLabelCfgPltFinalConcExponent),
     _toThe11thYield(textCfgColLabelCfgPltYieldExponent),
     _txtYsfLbl(textButtonLabelCfgLeftPltYsf),
     _txtDonorsAllowedLbl(textButtonLabelCfgLeftPltTrali),

     _topProduct(0),
     _callbackBtnType(INACTIVE),
     _productIndicatorPtr(NULL),
     _callbackProduct(0),
     _btnPtrUp(NULL),
     _btnPtrUpGray(NULL),
     _btnPtrDown(NULL),
     _btnPtrDownGray(NULL),

     _btnYsf(
        220,
        360,
        guiglobs::cdsData.rwConfig.predict.key_ysf,
        KEY_YSF,
        CFG_Look::BUTTON_UP_NAME,
        CFG_Look::BUTTON_DOWN_NAME,
        GUI_BUTTON_CONFIG_YSF),

     _btnDonorsAllowed (
        460,
        360,
        guiglobs::cdsData.rwConfig.predict.key_male_only_plt,
        KEY_MALE_ONLY_PLT,
        CFG_Look::BUTTON_UP_NAME,
        CFG_Look::BUTTON_DOWN_NAME,
        GUI_BUTTON_CONFIG_MALE_ONLY_PLT)

{
   _btnLock = false;

   // Theses member fns are virtual, so make sure we call the right one...
   Screen_CONFIG_PLATELET::initRowInfo();
   Screen_CONFIG_PLATELET::initProcedureInfo();
}  // END of Screen_CONFIG_PLATELET CONSTRUCTOR



//
////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_CONFIG_PLATELET::~Screen_CONFIG_PLATELET()
{
   Screen_CONFIG_PLATELET::deallocate_resources ();
}



//
////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_CONFIG_PLATELET::allocate_resources (const char* allocation_parameter)
{
   _btnLock                = false;

   _topProduct             = 0;
   OSRegion widget_positions;
   widget_positions.width  = LABEL_AREA;
   widget_positions.height = 70;

   // allocate parentScreenPtr's resources first
   Screen_Config::allocate_resources (allocation_parameter);

   // then allocate constituent objects with this window as parentScreenPtr widget
   // layer allocation from Z-back to Z-front.

   _btnPtrUpGray = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_UP_BUTTON,
                                      BITMAP_BUTTON_CFGPLT_UP_ARROW_X,
                                      BITMAP_BUTTON_CFGPLT_UP_ARROW_Y,
                                      textEmpty,
                                      BITMAP_SCROLL_UP_DISABLED,
                                      BITMAP_SCROLL_UP_DISABLED);

   _btnPtrDownGray = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON,
                                        BITMAP_BUTTON_CFGPLT_DN_ARROW_X,
                                        BITMAP_BUTTON_CFGPLT_DN_ARROW_Y,
                                        textEmpty,
                                        BITMAP_SCROLL_DOWN_DISABLED,
                                        BITMAP_SCROLL_DOWN_DISABLED);

   _btnPtrDown = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON,
                                    BITMAP_BUTTON_CFGPLT_DN_ARROW_X,
                                    BITMAP_BUTTON_CFGPLT_DN_ARROW_Y,
                                    textEmpty,
                                    BITMAP_SCROLL_DOWN_ENABLED,
                                    BITMAP_SCROLL_PRESSED);

   _btnYsf.allocate_resources(*this);
   _btnYsf.set_callback(cbPltYsfButton, (void*)this);

   _btnDonorsAllowed.allocate_resources(*this);
   _btnDonorsAllowed.set_callback(cbMaleOnlyPltBtn, (void*)this);

   _btnPtrDownGray->allocate_resources (*this);
   _btnPtrDownGray->disable_callbacks();

   _btnPtrUpGray->allocate_resources (*this);
   _btnPtrUpGray->disable_callbacks();

   _btnPtrDown->allocate_resources (*this) ;
   _btnPtrDown->set_callback(cbDownButton, (void*)this);

   _txtYieldLbl.allocate_resources(*this) ;
   _txtVolLbl.allocate_resources(*this);
   _txtFinalConcLbl.allocate_resources(*this);
   _txtCollectConcLbl.allocate_resources(*this);
   _txtPasVolLbl.allocate_resources(*this);
   _txtPlasmaPercentLbl.allocate_resources(*this);
   _toThe3rdCollectConc.allocate_resources(*this);
   _toThe3rdFinalConc.allocate_resources(*this);
   _toThe11thYield.allocate_resources(*this);
   _txtYsfLbl.allocate_resources(*this);
   _txtDonorsAllowedLbl.allocate_resources(*this);


   // Allocate stuff in rows
   for (int rowNum = 0; rowNum < MAX_ROWS; rowNum++)
   {
      // Static Elements *******************************************
      _graphicalRows[rowNum].rowLine = new Bitmap(
         BITMAP_CFGPLT_LABEL_BOX1_X,
         CFGPLTLINEROW(rowNum),
         BITMAP_CFGPLT_PLT_LABEL_LINE);

      _graphicalRows[rowNum].rowLine->allocate_resources(*this);

      _graphicalRows[rowNum].txtPasVol = new Display_Text(textMiscCfgPasVolInit);
      _graphicalRows[rowNum].txtPasVol->allocate_resources(*this);
      widget_positions.x               =  SS_COLUMN;
      widget_positions.y               =  (short)(_graphicalRows[rowNum].btnYield.yPos - 12);
      _graphicalRows[rowNum].txtPasVol->set_requested_region(widget_positions);

      _graphicalRows[rowNum].txtCollectConc = new Display_Text(textEmptyFormatCfgPltCollectConc);
      _graphicalRows[rowNum].txtCollectConc->allocate_resources(*this);
      widget_positions.x                    =  COLLECT_CONC_COLUMN;
      widget_positions.y                    =  (short)(_graphicalRows[rowNum].btnYield.yPos - 12);
      _graphicalRows[rowNum].txtCollectConc->set_requested_region(widget_positions);

      // Dynamic Elements *****************************
      _graphicalRows[rowNum].btnYield.btnBmpPtr =
         new Button_Bitmap (PLT_BUTTON_YIELD(rowNum),
                            (short)_graphicalRows[rowNum].btnYield.xPos,
                            (short)_graphicalRows[rowNum].btnYield.yPos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            BITMAP_CONFIG_SUB_NARROW_UP,
                            BITMAP_CONFIG_SUB_NARROW_DOWN);

      _graphicalRows[rowNum].btnProductVol.btnBmpPtr =
         new Button_Bitmap (PLT_BUTTON_VOL(rowNum),
                            (short)_graphicalRows[rowNum].btnProductVol.xPos,
                            (short)_graphicalRows[rowNum].btnProductVol.yPos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            BITMAP_CONFIG_SUB_NARROW_UP,
                            BITMAP_CONFIG_SUB_NARROW_DOWN);

      _graphicalRows[rowNum].btnFinalConc.btnBmpPtr =
         new Button_Bitmap (PLT_BUTTON_FINAL_CONC(rowNum),
                            (short)_graphicalRows[rowNum].btnFinalConc.xPos,
                            (short)_graphicalRows[rowNum].btnFinalConc.yPos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            BITMAP_CONFIG_SUB_NARROW_UP,
                            BITMAP_CONFIG_SUB_NARROW_DOWN);

      _graphicalRows[rowNum].btnRowBoxAndPasToggle.btnBmpPtr =
         new Button_Bitmap (PLT_BUTTON_PROD(rowNum),
                            BITMAP_CFGPLT_LABEL_BOX1_X,
                            CFG_PLT_Y_COORD(rowNum),
                            textCfgRowLabelCfgPlt1,
                            BITMAP_MSS_BUTTON_ICON_UP,
                            BITMAP_MSS_BUTTON_ICON_DOWN,
                            false, false);

      _graphicalRows[rowNum].btnPasPlasmaPercent.btnBmpPtr =
         new Button_Bitmap (PLT_BUTTON_PLASMA_PERCENT(rowNum),
                            (short)_graphicalRows[rowNum].btnPasPlasmaPercent.xPos,
                            (short)_graphicalRows[rowNum].btnPasPlasmaPercent.yPos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            BITMAP_CONFIG_SUB_NARROW_UP,
                            BITMAP_CONFIG_SUB_NARROW_DOWN);

      _graphicalRows[rowNum].btnYield.btnBmpPtr->allocate_resources(*this);
      _graphicalRows[rowNum].btnYield.btnPtr = _graphicalRows[rowNum].btnYield.btnBmpPtr;
      _graphicalRows[rowNum].btnYield.btnBmpPtr->set_callback(cbButton, (void*)&_graphicalRows[rowNum].btnYield);

      _graphicalRows[rowNum].btnProductVol.btnBmpPtr->allocate_resources(*this);
      _graphicalRows[rowNum].btnProductVol.btnPtr = _graphicalRows[rowNum].btnProductVol.btnBmpPtr;
      _graphicalRows[rowNum].btnProductVol.btnBmpPtr->set_callback(cbButton, (void*)&_graphicalRows[rowNum].btnProductVol);

      _graphicalRows[rowNum].btnFinalConc.btnBmpPtr->allocate_resources(*this);
      _graphicalRows[rowNum].btnFinalConc.btnPtr = _graphicalRows[rowNum].btnFinalConc.btnBmpPtr;
      _graphicalRows[rowNum].btnFinalConc.btnBmpPtr->set_callback(cbButton, (void*)&_graphicalRows[rowNum].btnFinalConc);

      _graphicalRows[rowNum].btnRowBoxAndPasToggle.btnBmpPtr->allocate_resources(*this);
      _graphicalRows[rowNum].btnRowBoxAndPasToggle.btnPtr = _graphicalRows[rowNum].btnRowBoxAndPasToggle.btnBmpPtr;
      _graphicalRows[rowNum].btnRowBoxAndPasToggle.btnBmpPtr->set_callback(cbToggleButton, (void*)&_graphicalRows[rowNum].btnRowBoxAndPasToggle);

      _graphicalRows[rowNum].btnPasPlasmaPercent.btnBmpPtr->allocate_resources(*this);
      _graphicalRows[rowNum].btnPasPlasmaPercent.btnPtr = _graphicalRows[rowNum].btnPasPlasmaPercent.btnBmpPtr;
      _graphicalRows[rowNum].btnPasPlasmaPercent.btnBmpPtr->set_callback(cbButton, (void*)&_graphicalRows[rowNum].btnPasPlasmaPercent);
   }

   _graphicalRows[0].bmpMagGlass = new Bitmap(CFG_PLT_MAG_GLASS_X,
                                              CFG_PLT_MAG_GLASS_TOP_Y,
                                              BITMAP_CFGPP_MAG_GLASS_TOP);
   _graphicalRows[1].bmpMagGlass = new Bitmap(CFG_PLT_MAG_GLASS_X,
                                              CFG_PLT_MAG_GLASS_MIDDLE_Y,
                                              BITMAP_CFGPP_MAG_GLASS_MIDDLE);
   _graphicalRows[2].bmpMagGlass = new Bitmap(CFG_PLT_MAG_GLASS_X,
                                              CFG_PLT_MAG_GLASS_BOTTOM_Y,
                                              BITMAP_CFGPP_MAG_GLASS_BOTTOM);

   labelAllButtons();

   checkForTimeOnlyConfig();
} // END of Screen_CONFIG_PLATELET ALLOCATE_RESOURCES

//
////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_CONFIG_PLATELET::deallocate_resources ()
{
   _btnLock = false;

   _txtYieldLbl.deallocate_resources();
   _txtVolLbl.deallocate_resources();
   _txtFinalConcLbl.deallocate_resources();
   _txtCollectConcLbl.deallocate_resources();
   _txtPasVolLbl.deallocate_resources();
   _txtPlasmaPercentLbl.deallocate_resources();
   _btnYsf.deallocate_resources();
   _btnDonorsAllowed.deallocate_resources();
   _toThe3rdCollectConc.deallocate_resources();
   _toThe3rdFinalConc.deallocate_resources();
   _toThe11thYield.deallocate_resources();
   _txtYsfLbl.deallocate_resources();
   _txtDonorsAllowedLbl.deallocate_resources();

   delete _productIndicatorPtr;
   _productIndicatorPtr = NULL;

   delete _btnPtrUp;
   _btnPtrUp = NULL;

   delete _btnPtrDown;
   _btnPtrDown = NULL;

   delete _btnPtrUpGray;
   _btnPtrUpGray = NULL;

   delete _btnPtrDownGray;
   _btnPtrDownGray = NULL;

   for (int rowNum = 0; rowNum < MAX_ROWS; rowNum++)
   {
      delete _graphicalRows[rowNum].rowLine;
      _graphicalRows[rowNum].rowLine = NULL;

      delete _graphicalRows[rowNum].btnYield.btnBmpPtr;
      _graphicalRows[rowNum].btnYield.btnBmpPtr = NULL;

      delete _graphicalRows[rowNum].btnProductVol.btnBmpPtr;
      _graphicalRows[rowNum].btnProductVol.btnBmpPtr = NULL;

      delete _graphicalRows[rowNum].txtPasVol;
      _graphicalRows[rowNum].txtPasVol = NULL;

      delete _graphicalRows[rowNum].txtCollectConc;
      _graphicalRows[rowNum].txtCollectConc = NULL;

      delete _graphicalRows[rowNum].btnFinalConc.btnBmpPtr;
      _graphicalRows[rowNum].btnFinalConc.btnBmpPtr = NULL;

      delete _graphicalRows[rowNum].btnRowBoxAndPasToggle.btnBmpPtr;
      _graphicalRows[rowNum].btnRowBoxAndPasToggle.btnBmpPtr = NULL;

      delete _graphicalRows[rowNum].btnPasPlasmaPercent.btnBmpPtr;
      _graphicalRows[rowNum].btnPasPlasmaPercent.btnBmpPtr = NULL;

      delete _graphicalRows[rowNum].bmpMagGlass;
      _graphicalRows[rowNum].bmpMagGlass = NULL;
   }

   // deactivate the status line before leaving
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

   // deallocate parentScreenPtr's resources
   Screen_Config::deallocate_resources ();
}  // END of Screen_CONFIG_PLATELET DEALLOCATE_RESOURCES



//////////////////////////////////////////////////////////////////////////
//
//		CALLBACK FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
void cbButton (void* data)
{
   Screen_CONFIG_PLATELET::BTN_INFO* btn_info = (Screen_CONFIG_PLATELET::BTN_INFO*)data;

   if (!Screen_CONFIG_PLATELET::_btnLock)
   {
      Screen_CONFIG_PLATELET::_btnLock = true;

      float     min, max;
      guistring keypadString;
      string    minMaxFormat = "%.0f";
      keypadString.initialize();

      guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
      guiglobs::apheresis_status_line->unparent_status_line (guiglobs::apheresis_status_line->get_owning_screen ());

      (btn_info->parentScreenPtr)->setRowIndicator(btn_info->rowNum);
      (btn_info->parentScreenPtr)->awaitingCallback(btn_info->btnType, btn_info->rowNum);

      switch (btn_info->btnType)
      {
         case Screen_CONFIG_PLATELET::YIELD :  // assumes min and max are the same for all yields
         {
            min          = (btn_info->parentScreenPtr)->getMinYield(btn_info->rowNum);
            max          = (btn_info->parentScreenPtr)->getMaxYield(btn_info->rowNum);
            minMaxFormat = "%.1f";

            Screen_Config::build_keypad_string (keypadString,
                                                KEY_PLT_YIELD_1,   // unused
                                                PR_KEYPAD_X,
                                                PR_KEYPAD_Y,
                                                "##.#",
                                                max,
                                                min);
            break;
         }

         case Screen_CONFIG_PLATELET::VOLUME :
         {
            min = guiglobs::config_var_list.Var(KEY_PLT_VOLUME_1).Min();

            max = (float)floor(guiglobs::config_var_list.Var(KEY_PLT_VOLUME_1).Max()
                               * (btn_info->parentScreenPtr)->getPercentCoMultiplier(btn_info->rowNum));

            Screen_Config::build_keypad_string(keypadString,
                                               KEY_PLT_VOLUME_1,   // unused
                                               PR_KEYPAD_X,
                                               PR_KEYPAD_Y,
                                               "####",
                                               max,
                                               min);
            break;
         }

         case Screen_CONFIG_PLATELET::FINAL_CONC :
         {
            min = (btn_info->parentScreenPtr)->getMinConc(btn_info->rowNum);
            max = (btn_info->parentScreenPtr)->getMaxConc(btn_info->rowNum);

            Screen_Config::build_keypad_string(keypadString,
                                               KEY_PLT_VOLUME_1,   // unused
                                               PR_KEYPAD_X,
                                               PR_KEYPAD_Y,
                                               "######",
                                               max,
                                               min);
            break;
         }

         case Screen_CONFIG_PLATELET::PLASMA_PERCENT :
         {
            min = (btn_info->parentScreenPtr)->getMinPasPlasmaPercent(btn_info->rowNum);
            max = guiglobs::config_var_list.Var(KEY_PLT_PCT_CARRYOVER_1).Max();

            Screen_Config::build_keypad_string (keypadString,
                                                KEY_PLT_PCT_CARRYOVER_1,
                                                PR_KEYPAD_X,
                                                PR_KEYPAD_Y,
                                                "##",
                                                max,
                                                min);

            break;
         }

         default :
            return;   // no actions required for other buttons

      }

      (btn_info->parentScreenPtr)->goto_child_screen(GUI_SCREEN_KEYPAD, keypadString);

      // Do this after allocating the keypad.  (We want keypad to allocate quickly so that bad
      // monkeys don't have the chance to allocate multiple keypads.
      (btn_info->parentScreenPtr)->display_min_max_keypad_limits(min, max, minMaxFormat.c_str());
   }
   else
   {
      DataLog (log_level_gui_error) << "Button lock error (Button " << btn_info->btnType << ")" << endmsg;
   }
} // end of cbButton

////////////////////////////////////////////////////////////////////////////////
void cbToggleButton (void* data)
{
   Screen_CONFIG_PLATELET::BTN_INFO* btn_info = (Screen_CONFIG_PLATELET::BTN_INFO*)data;
   (btn_info->parentScreenPtr)->awaitingCallback(btn_info->btnType, btn_info->rowNum);
   (btn_info->parentScreenPtr)->processMssToggleButton(btn_info->rowNum);
}

void Screen_CONFIG_PLATELET::processMssToggleButton (int rowNum)
{
   if (guiglobs::cdsData.rwConfig.templates.key_plt_mss[rowNum + _topProduct] == 0)
   {
      // Turning MSS on.
      guiglobs::cdsData.rwConfig.templates.key_plt_mss[rowNum + _topProduct] = 1;

      float minPlasmaPercent = getMinPasPlasmaPercent(rowNum);

      // The following two if blocks guard against the corner cases where toggling PAS
      // results in a collect volume of less than 50.  In cases where this would happen, we adjust
      // the percent plasma, and if necessary the collect concentration, to keep collect volume at 50.

      if (guiglobs::cdsData.rwConfig.templates.key_plt_pct_carryover[rowNum + _topProduct] < minPlasmaPercent)
      {
         DataLog(log_level_gui_info) << "Percent plasma adjusted from "
                                     << guiglobs::cdsData.rwConfig.templates.key_plt_pct_carryover[rowNum + _topProduct]
                                     << " to " << minPlasmaPercent
                                     << " to keep collect volume above min." << endmsg;

         guiglobs::cdsData.rwConfig.templates.key_plt_pct_carryover[rowNum + _topProduct] = (int)minPlasmaPercent;
      }

      if (guiglobs::cdsData.rwConfig.templates.key_plt_pct_carryover[rowNum + _topProduct] == minPlasmaPercent &&
          guiglobs::cdsData.rwConfig.templates.key_plt_pct_carryover[rowNum + _topProduct] >= 99.0)
      {
         float yield         = guiglobs::cdsData.rwConfig.templates.key_plt_yield[rowNum + _topProduct] * 1.0E5f;
         float volume        = guiglobs::config_var_list.Var(KEY_PLT_VOLUME_1).Min();
         float percentPlasma = guiglobs::cdsData.rwConfig.templates.key_plt_pct_carryover[rowNum + _topProduct] / 100.0f;
         // float pasVol = volume * (( 1 - percentPlasma ) / percentPlasma);
         float pasVol        = floor(volume * (( 1 - percentPlasma ) / percentPlasma));

         DataLog (log_level_gui_info) << "CompPls: MssToggle config PAS= " << pasVol
                                      << ", volm = " << volume
                                      << ", ppco = " << percentPlasma
                                      << endmsg;



         DataLog(log_level_gui_info) << "Bumped percent plasma up to max(99.0). Adjusting final concentration from "
                                     << _products[rowNum + _topProduct].finalConc << " to " << floor(yield / (volume + pasVol)) << " " << endmsg;

         _products[rowNum + _topProduct].finalConc = (float)floor(yield / (volume + pasVol));
      }
   }
   else
      guiglobs::cdsData.rwConfig.templates.key_plt_mss[rowNum + _topProduct] = 0;

   float collect_conc = labelButtonRow(rowNum, rowNum + _topProduct);
   displayAlert(collect_conc, _products[rowNum + _topProduct].finalConc);
}

/////////////////////////////////////////////////////////////////////////////
// callback for the up button
void cbUpButton (void* data)
{
   ((Screen_CONFIG_PLATELET*)data)->processUpButton();
}

///////////////////////////////////////////////////////////////////////////////
void Screen_CONFIG_PLATELET::processUpButton ()
{
   if (_topProduct > 0)
   {
      _topProduct--;
      labelAllButtons();

      if (_topProduct == 0)
      {
         delete _btnPtrUp;
         _btnPtrUp = NULL;
      }
   }

   if (_btnPtrDown == NULL)
   {
      _btnPtrDown = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON,
                                       BITMAP_BUTTON_CFGPLT_DN_ARROW_X,
                                       BITMAP_BUTTON_CFGPLT_DN_ARROW_Y,
                                       textEmpty,
                                       BITMAP_SCROLL_DOWN_ENABLED,
                                       BITMAP_SCROLL_PRESSED);

      _btnPtrDown->allocate_resources(*this);
      _btnPtrDown->set_callback(cbDownButton, (void*)this);
   }

   checkForTimeOnlyConfig();
} // end of processUpButton

//
///////////////////////////////////////////////////////////////////////////////
// callback for the down button
void cbDownButton (void* data)
{
   ((Screen_CONFIG_PLATELET*)data)->processDownButton();
}

///////////////////////////////////////////////////////////////////////////////
void Screen_CONFIG_PLATELET::processDownButton ()
{
   if (_topProduct < (NUM_YIELD_VOLUME - MAX_ROWS))
   {
      ++_topProduct;
      labelAllButtons();

      if (_topProduct >= (NUM_YIELD_VOLUME - MAX_ROWS))
      {
         delete _btnPtrDown;
         _btnPtrDown = NULL;
      }
   }

   if (_btnPtrUp == NULL)
   {
      _btnPtrUp = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_UP_BUTTON,
                                     BITMAP_BUTTON_CFGPLT_UP_ARROW_X,
                                     BITMAP_BUTTON_CFGPLT_UP_ARROW_Y,
                                     textEmpty,
                                     BITMAP_SCROLL_UP_ENABLED,
                                     BITMAP_SCROLL_PRESSED);

      _btnPtrUp->allocate_resources(*this);
      _btnPtrUp->set_callback(cbUpButton, (void*)this);
   }

   checkForTimeOnlyConfig();
} // end of processDownButton

///////////////////////////////////////////////////////////////////////////////
// Message handler for KEYPADS
void Screen_CONFIG_PLATELET::update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer)
{
   bool updateRow = true;

   delete _productIndicatorPtr;
   _productIndicatorPtr = NULL;

   _btnLock             = false;

   turn_off_min_max_keypad_text();

   if ((return_keypad_buffer.keypad_return_value) != -1)
   {
      float the_data = atof(return_keypad_buffer.keypad_return_string);

      switch (_callbackBtnType)
      {
         case YIELD :  // calculate final concentration
            guiglobs::cdsData.rwConfig.templates.key_plt_yield[_callbackProduct] = the_data;
            break;

         case VOLUME :
            guiglobs::cdsData.rwConfig.templates.key_plt_volume[_callbackProduct] = the_data;
            break;

         case FINAL_CONC :
            _products[_callbackProduct].finalConc = the_data;
            break;

         case YSF :
            guiglobs::cdsData.rwConfig.predict.key_ysf = the_data;
            _btnYsf.set_text_direct (10, return_keypad_buffer.keypad_return_string);
            updateRow = false;
            break;

         case PLASMA_PERCENT :
            guiglobs::cdsData.rwConfig.templates.key_plt_pct_carryover[_callbackProduct] = (int)the_data;
            break;

         default :

            DataLog(log_level_gui_error) << "INACTIVE BUTTON_TYPE " << _callbackBtnType << " FOUND!" << endmsg;

            anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
            alarm.setAlarm();
            break;
      }

   }
   else  //  No value returned from keypad.
   {
      //
      //  Check to see if button pushed is a platelet product.
      //  If yes, check concentration value for warning.
      //
      updateRow =  ((_callbackBtnType == VOLUME)||(_callbackBtnType == FINAL_CONC));
   }

   if (updateRow)
   {
      float collect_conc = labelButtonRow(_callbackProduct - _topProduct, _callbackProduct);
      displayAlert(collect_conc, _products[_callbackProduct].finalConc);
   }

} // end of update_screen_with_keypad_data

//
///////////////////////////////////////////////////////////////////////////////
// Message handler for LISTBOXS
void Screen_CONFIG_PLATELET::update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer)
{
   _btnLock = false;
   _btnDonorsAllowed.display_the_text_string(rtn_listbox_buffer.key_value);
   guiglobs::cdsData.rwConfig.predict.key_male_only_plt = int(rtn_listbox_buffer.key_value);
} // end of update_screen_with_date method


void cbPltYsfButton (void* data)
{
   ((Screen_CONFIG_PLATELET*)data)->processYsfButton();
}

void Screen_CONFIG_PLATELET::processYsfButton ()
{
   if (!_btnLock)
   {
      _btnLock = true;

      // Calculate the max and min
      static const float min = guiglobs::config_var_list.Var(KEY_YSF).Min();
      static const float max = guiglobs::config_var_list.Var(KEY_YSF).Max();

      // Build a keypad string
      Screen_Config::build_keypad_string (keypad_string,
                                          KEY_RSF,
                                          PR_KEYPAD_X,
                                          PR_KEYPAD_Y,
                                          "#.##",
                                          max,
                                          min);

      // Put up the keypad
      goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);

      // Additional screen processing stuff
      display_min_max_keypad_limits(min, max, "%.2f");

      awaitingCallback (YSF, 0);
      guiglobs::apheresis_status_line->unparent_status_line (guiglobs::apheresis_status_line->get_owning_screen ());
   }
   else
   {
      DataLog (log_level_gui_error) << "Button lock error (YSF Button)" << endmsg;
   }
}


void cbMaleOnlyPltBtn (void* data)
{
   ((Screen_CONFIG_PLATELET*)data)->processMaleOnlyPltBtn();
}

void Screen_CONFIG_PLATELET::processMaleOnlyPltBtn ()
{
   if (!_btnLock)
   {
      _btnLock = true;
      _btnDonorsAllowed.create_param_string
         (keypad_string, KEY_MALE_ONLY_PLT, 260, 145);

      goto_child_screen(GUI_SCREEN_LISTBOX, keypad_string);
   }
   else
   {
      DataLog (log_level_gui_error) << "Button lock error (Trali Option)" << endmsg;
   }

}

//////////////////////////////////////////////////////////////////////////
//
//		UTILITY FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////

void Screen_CONFIG_PLATELET::initRowInfo ()
{
   for (int i = 0; i < MAX_ROWS; i++)
   {
      _graphicalRows[i].rowLine         = NULL;

      _graphicalRows[i].btnYield.xPos   = YIELD_COLUMN;
      _graphicalRows[i].btnYield.yPos   = CFG_PLT_Y_COORD(i);
      _graphicalRows[i].btnYield.btnPtr = NULL;
      _graphicalRows[i].btnYield.btnBmpPtr                    = NULL;
      _graphicalRows[i].btnYield.parentScreenPtr              = this;
      _graphicalRows[i].btnYield.btnType                      = YIELD;
      _graphicalRows[i].btnYield.rowNum                       = i;

      _graphicalRows[i].btnProductVol.xPos                    = VOLUME_COLUMN;
      _graphicalRows[i].btnProductVol.yPos                    = CFG_PLT_Y_COORD(i);
      _graphicalRows[i].btnProductVol.btnPtr                  = NULL;
      _graphicalRows[i].btnProductVol.btnBmpPtr               = NULL;
      _graphicalRows[i].btnProductVol.parentScreenPtr         = this;
      _graphicalRows[i].btnProductVol.btnType                 = VOLUME;
      _graphicalRows[i].btnProductVol.rowNum                  = i;

      _graphicalRows[i].btnFinalConc.xPos                     = FINAL_CONC_COLUMN;
      _graphicalRows[i].btnFinalConc.yPos                     = CFG_PLT_Y_COORD(i);
      _graphicalRows[i].btnFinalConc.btnPtr                   = NULL;
      _graphicalRows[i].btnFinalConc.btnBmpPtr                = NULL;
      _graphicalRows[i].btnFinalConc.parentScreenPtr          = this;
      _graphicalRows[i].btnFinalConc.btnType                  = FINAL_CONC;
      _graphicalRows[i].btnFinalConc.rowNum                   = i;

      _graphicalRows[i].btnRowBoxAndPasToggle.xPos            = PROD_NUM_COLUMN;
      _graphicalRows[i].btnRowBoxAndPasToggle.yPos            = CFG_PLT_Y_COORD(i);
      _graphicalRows[i].btnRowBoxAndPasToggle.btnPtr          = NULL;
      _graphicalRows[i].btnRowBoxAndPasToggle.btnBmpPtr       = NULL;
      _graphicalRows[i].btnRowBoxAndPasToggle.parentScreenPtr = this;
      _graphicalRows[i].btnRowBoxAndPasToggle.btnType         = INACTIVE;
      _graphicalRows[i].btnRowBoxAndPasToggle.rowNum          = i;

      _graphicalRows[i].txtPasVol                             = 0;
      _graphicalRows[i].txtCollectConc                        = 0;

      _graphicalRows[i].btnPasPlasmaPercent.xPos              = CO_COLUMN;
      _graphicalRows[i].btnPasPlasmaPercent.yPos              = CFG_PLT_Y_COORD(i);
      _graphicalRows[i].btnPasPlasmaPercent.btnPtr            = NULL;
      _graphicalRows[i].btnPasPlasmaPercent.btnBmpPtr         = NULL;
      _graphicalRows[i].btnPasPlasmaPercent.parentScreenPtr   = this;
      _graphicalRows[i].btnPasPlasmaPercent.btnType           = PLASMA_PERCENT;
      _graphicalRows[i].btnPasPlasmaPercent.rowNum            = i;
   }
}

void Screen_CONFIG_PLATELET::initProcedureInfo ()
{
   _products[0].productIndicatorString = textMiscCfgPlt1Indicator;
   _products[0].productLblString       = textCfgRowLabelCfgPlt1;
   _products[0].keyYield               = KEY_PLT_YIELD_1;
   _products[0].keyProductVol          = KEY_PLT_VOLUME_1;
   _products[0].keyPasToggle           = KEY_PLT_MSS_1;
   _products[0].keyPasPlasmaPercent    = KEY_PLT_PCT_CARRYOVER_1;

   _products[1].productIndicatorString = textMiscCfgPlt2Indicator;
   _products[1].productLblString       = textCfgRowLabelCfgPlt2;
   _products[1].keyYield               = KEY_PLT_YIELD_2;
   _products[1].keyProductVol          = KEY_PLT_VOLUME_2;
   _products[1].keyPasToggle           = KEY_PLT_MSS_2;
   _products[1].keyPasPlasmaPercent    = KEY_PLT_PCT_CARRYOVER_2;

   _products[2].productIndicatorString = textMiscCfgPlt3Indicator;
   _products[2].productLblString       = textCfgRowLabelCfgPlt3;
   _products[2].keyYield               = KEY_PLT_YIELD_3;
   _products[2].keyProductVol          = KEY_PLT_VOLUME_3;
   _products[2].keyPasToggle           = KEY_PLT_MSS_3;
   _products[2].keyPasPlasmaPercent    = KEY_PLT_PCT_CARRYOVER_3;

   _products[3].productIndicatorString = textMiscCfgPlt4Indicator;
   _products[3].productLblString       = textCfgRowLabelCfgPlt4;
   _products[3].keyYield               = KEY_PLT_YIELD_4;
   _products[3].keyProductVol          = KEY_PLT_VOLUME_4;
   _products[3].keyPasToggle           = KEY_PLT_MSS_4;
   _products[3].keyPasPlasmaPercent    = KEY_PLT_PCT_CARRYOVER_4;

   _products[4].productIndicatorString = textMiscCfgPlt5Indicator;
   _products[4].productLblString       = textCfgRowLabelCfgPlt5;
   _products[4].keyYield               = KEY_PLT_YIELD_5;
   _products[4].keyProductVol          = KEY_PLT_VOLUME_5;
   _products[4].keyPasToggle           = KEY_PLT_MSS_5;
   _products[4].keyPasPlasmaPercent    = KEY_PLT_PCT_CARRYOVER_5;

   _products[5].productIndicatorString = textMiscCfgPlt6Indicator;
   _products[5].productLblString       = textCfgRowLabelCfgPlt6;
   _products[5].keyYield               = KEY_PLT_YIELD_6;
   _products[5].keyProductVol          = KEY_PLT_VOLUME_6;
   _products[5].keyPasToggle           = KEY_PLT_MSS_6;
   _products[5].keyPasPlasmaPercent    = KEY_PLT_PCT_CARRYOVER_6;

   _products[6].productIndicatorString = textMiscCfgPlt7Indicator;
   _products[6].productLblString       = textCfgRowLabelCfgPlt7;
   _products[6].keyYield               = KEY_PLT_YIELD_7;
   _products[6].keyProductVol          = KEY_PLT_VOLUME_7;
   _products[6].keyPasToggle           = KEY_PLT_MSS_7;
   _products[6].keyPasPlasmaPercent    = KEY_PLT_PCT_CARRYOVER_7;

   _products[7].productIndicatorString = textMiscCfgPlt8Indicator;
   _products[7].productLblString       = textCfgRowLabelCfgPlt8;
   _products[7].keyYield               = KEY_PLT_YIELD_8;
   _products[7].keyProductVol          = KEY_PLT_VOLUME_8;
   _products[7].keyPasToggle           = KEY_PLT_MSS_8;
   _products[7].keyPasPlasmaPercent    = KEY_PLT_PCT_CARRYOVER_8;

   _products[8].productIndicatorString = textMiscCfgPlt9Indicator;
   _products[8].productLblString       = textCfgRowLabelCfgPlt9;
   _products[8].keyYield               = KEY_PLT_YIELD_9;
   _products[8].keyProductVol          = KEY_PLT_VOLUME_9;
   _products[8].keyPasToggle           = KEY_PLT_MSS_9;
   _products[8].keyPasPlasmaPercent    = KEY_PLT_PCT_CARRYOVER_9;

   _products[9].productIndicatorString = textMiscCfgPlt10Indicator;
   _products[9].productLblString       = textCfgRowLabelCfgPlt10;
   _products[9].keyYield               = KEY_PLT_YIELD_10;
   _products[9].keyProductVol          = KEY_PLT_VOLUME_10;
   _products[9].keyPasToggle           = KEY_PLT_MSS_10;
   _products[9].keyPasPlasmaPercent    = KEY_PLT_PCT_CARRYOVER_10;
}

//
///////////////////////////////////////////////////////////////////////////////
// labelAllButtons
void Screen_CONFIG_PLATELET::labelAllButtons ()
{
   int product_num = _topProduct;
   _callbackBtnType = INACTIVE;

   for (int rowNum = 0; rowNum < MAX_ROWS; rowNum++, product_num++)
   {
      labelButtonRow(rowNum, product_num);
   }
} // end of labelAllButtons

///////////////////////////////////////////////////////////////////////////////
// labelButtonRow
float Screen_CONFIG_PLATELET::labelButtonRow (int rowNum, int product_num)
{
   guistring tempString;
   int       tempStringLength = 0;

   bool
      masterPasOn  = guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on,
      productPasOn = guiglobs::cdsData.rwConfig.templates.key_plt_mss[product_num],
      pasOn        = masterPasOn && productPasOn;

   //
   //  These variables are saved in config
   //
   float yield         = guiglobs::cdsData.rwConfig.templates.key_plt_yield[product_num] * 1.0E5f;
   float plasmaPercent = (pasOn) ? ((float)guiglobs::cdsData.rwConfig.templates.key_plt_pct_carryover[product_num]) / 100.0f : 1.0f;

   float vol           = guiglobs::cdsData.rwConfig.templates.key_plt_volume[product_num];

   if (_callbackBtnType == YIELD || _callbackBtnType == FINAL_CONC || _callbackBtnType == PLASMA_PERCENT)
   {
      vol = guiglobs::cdsData.rwConfig.templates.key_plt_volume[product_num]
               = (yield * plasmaPercent) / _products[product_num].finalConc;                // Compute and update Collect Volume
   }
   else
   {
      _products[product_num].finalConc = (yield * plasmaPercent) / vol;     // Compute and update Final Concentration
   }

   //
   //   Collect concentration and PAS volume are always derived from the other values
   //
   float collectConc = (pasOn) ? yield / vol : _products[product_num].finalConc;
   float pasVol      = round(vol * (  (1 - plasmaPercent) / plasmaPercent   )     );

   // Set yield button
   tempStringLength = sprintf_l10n(tempString, "%.1f", guiglobs::cdsData.rwConfig.templates.key_plt_yield[product_num]);
   _graphicalRows[rowNum].btnYield.btnBmpPtr->set_text_direct (tempStringLength, tempString);

   // Set volume button
   tempStringLength = sprintf(tempString, "%.0f", floor(vol + .5));
   _graphicalRows[rowNum].btnProductVol.btnBmpPtr->set_text_direct (tempStringLength, tempString);

   // Set pas toggle
   _graphicalRows[rowNum].btnRowBoxAndPasToggle.btnBmpPtr->set_string_id(
      _products[rowNum + _topProduct].productLblString);

   _graphicalRows[rowNum].btnRowBoxAndPasToggle.btnBmpPtr->set_up_bitmap(((pasOn) ? BITMAP_MSS_BUTTON_ICON_DOWN : ((masterPasOn) ? BITMAP_MSS_BUTTON_ICON_UP : BITMAP_MSS_BUTTON_ICON_GRAY)));
   _graphicalRows[rowNum].btnRowBoxAndPasToggle.btnBmpPtr->set_button_text_color((masterPasOn) ? YELLOW : GREY_LIGHT);
   _graphicalRows[rowNum].btnRowBoxAndPasToggle.btnBmpPtr->set_disabled(!masterPasOn);


   // Set percent carryover
   if (pasOn)
      tempStringLength = sprintf(tempString, "%d", guiglobs::cdsData.rwConfig.templates.key_plt_pct_carryover[product_num]);
   else
      tempStringLength = sprintf(tempString, "--");

   _graphicalRows[rowNum].btnPasPlasmaPercent.btnBmpPtr->set_text_direct(tempStringLength, tempString);
   _graphicalRows[rowNum].btnPasPlasmaPercent.btnBmpPtr->set_disabled(!pasOn);
   _graphicalRows[rowNum].btnPasPlasmaPercent.btnBmpPtr->set_up_bitmap((pasOn) ? BITMAP_CONFIG_SUB_NARROW_UP : BITMAP_CONFIG_SUB_NARROW_GRAY);
   _graphicalRows[rowNum].btnPasPlasmaPercent.btnBmpPtr->set_button_text_color((pasOn) ? YELLOW : GREY_LIGHT);

   // Set pas volume
   if (pasOn)
      tempStringLength = sprintf(tempString, "%.0f", ceil(pasVol));
   else
      tempStringLength = sprintf(tempString, "--");

   _graphicalRows[rowNum].txtPasVol->set_text_direct (tempStringLength, tempString);

   // Set final conc
   tempStringLength = sprintf(tempString, "%.0f", _products[product_num].finalConc);
   _graphicalRows[rowNum].btnFinalConc.btnBmpPtr->set_text_direct (tempStringLength, tempString);

   // Set collect conc
   if (pasOn)
      tempStringLength = sprintf(tempString, "%.0f", collectConc);
   else
      tempStringLength = sprintf(tempString, "--");

   _graphicalRows[rowNum].txtCollectConc->set_text_direct(tempStringLength, tempString);

   // Allocate the magnifying glass if necessary
   // (use config setting for yield)
   yield = guiglobs::cdsData.rwConfig.templates.key_plt_yield[product_num];

   if (  // Platelet Concentration Too High
      (collectConc > CobeConfig::data().ConcMax)
      || // Triple PPC as Non-LR condition [IT-13096]
      (Software_CDS::GetInstance().getFeature(TriplePpcFlag) &&
       (yield > CobeConfig::data().TppYieldThreshold) &&
       (collectConc > CobeConfig::data().PpcConcentrationThreshold) ) )
   {
      _graphicalRows[rowNum].bmpMagGlass->allocate_resources(*this);
   }
   else
   {
      _graphicalRows[rowNum].bmpMagGlass->deallocate_resources();
   }

   return collectConc;
}

// end of labelAllButtons
//


/////////////////////////////////////////////////////////////////////////////////
void Screen_CONFIG_PLATELET::awaitingCallback (const BUTTON_TYPE type, int rowNum)
{
   _callbackBtnType = type;
   _callbackProduct = rowNum + _topProduct;
}

////////////////////////////////////////////////////////////////////////////////
//
float Screen_CONFIG_PLATELET::getYield (int rowNum)
{
   const int index = rowNum + _topProduct;
   return guiglobs::cdsData.rwConfig.templates.key_plt_yield[index];
}

////////////////////////////////////////////////////////////////////////////////
//
void Screen_CONFIG_PLATELET::setRowIndicator (int rowNum)
{
   if (rowNum >= 0)
   {  // filter out ysf
      if (_productIndicatorPtr == NULL)
      {
         _productIndicatorPtr = new Display_Text (_products[rowNum + _topProduct].productIndicatorString);
         _productIndicatorPtr->allocate_resources(*this);
      }
      else
      {
         _productIndicatorPtr->set_string_id(_products[rowNum + _topProduct].productIndicatorString);
      }
   }
} // end of setRowIndicator

////////////////////////////////////////////////////////////////////////////////
float Screen_CONFIG_PLATELET::getMaxConc (int btn_row)
{
   int   productNum = btn_row + _topProduct;
   float yield      = guiglobs::cdsData.rwConfig.templates.key_plt_yield[productNum] * 1.0E5f;
   float minPltVol  = guiglobs::config_var_list.Var(KEY_PLT_VOLUME_1).Min();
   float defaultMax = yield / minPltVol;
   float calcMax    = defaultMax + 1;

   if (guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on && guiglobs::cdsData.rwConfig.templates.key_plt_mss[productNum])
   {
      float percentCO = ((float)guiglobs::cdsData.rwConfig.templates.key_plt_pct_carryover[productNum]) / 100.0f;
      calcMax = (percentCO * yield) / minPltVol;
   }
   if (calcMax < defaultMax) return round(calcMax);
   return round(defaultMax);
}

//
///////////////////////////////////////////////////////////////////////////////
// display the alert message
void Screen_CONFIG_PLATELET::displayAlert (float collect_conc, float final_conc)
{
   TEXT_STRING_ID alertMsg = TEXT_NULL;

   //  Use collect concentration for determination if this is a PPC
   if (Software_CDS::outsideStorageRange(collect_conc, final_conc))
      alertMsg = textStatCfgPltConcRange;

   if (alertMsg == TEXT_NULL)
   {
      guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
      guiglobs::apheresis_status_line->unparent_status_line (guiglobs::apheresis_status_line->get_owning_screen ());
   }
   else
   {
      guiglobs::apheresis_status_line->reparent_status_line(*this);
      guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, alertMsg);
   }
} // end of displayAlert

float Screen_CONFIG_PLATELET::getPercentCoMultiplier (int rowNum)
{
   if (guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on
       && guiglobs::cdsData.rwConfig.templates.key_plt_mss[rowNum + _topProduct])

      return ((float)guiglobs::cdsData.rwConfig.templates.key_plt_pct_carryover[rowNum + _topProduct]) / 100.0f;
   else
      return 1.0f;
}

////////////////////////////////////////////////////////////////////////////////
float Screen_CONFIG_PLATELET::getMinConc (int btn_row)
{
   int   productNum = btn_row + _topProduct;
   bool  pasOn      = guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on && guiglobs::cdsData.rwConfig.templates.key_plt_mss[productNum];
   float yield      = guiglobs::cdsData.rwConfig.templates.key_plt_yield[productNum] * 1.0E5f;
   float maxPltVol  = guiglobs::config_var_list.Var(KEY_PLT_VOLUME_1).Max();
   float defaultMin = yield / maxPltVol;
   float calcMin    = defaultMin - 1;

   if (pasOn)
   {
      float percentCO = ((float)guiglobs::cdsData.rwConfig.templates.key_plt_pct_carryover[productNum]) / 100.0f;
      calcMin = (percentCO * yield) / maxPltVol;
   }
   if (calcMin > defaultMin) return ceil(calcMin);
   return ceil(defaultMin);
}

float Screen_CONFIG_PLATELET::getMinPasPlasmaPercent (int btn_row)
{
   int   productNum = btn_row + _topProduct;
   float yield      = guiglobs::cdsData.rwConfig.templates.key_plt_yield[productNum] * 1.0E5f;
   float minPltVol  = guiglobs::config_var_list.Var(KEY_PLT_VOLUME_1).Min();
   float finalConc  = _products[productNum].finalConc;
   float defaultMin = ceil(guiglobs::config_var_list.Var(KEY_PLT_PCT_CARRYOVER_1).Min());

   float calcMin    = ceil(100.0f * minPltVol * finalConc / yield);

   DataLog(log_level_gui_info) << "PCO: (Hard Min = " << defaultMin << ", Soft Min = "
                               << calcMin << ")" << endmsg;

   return min(99.0, max (defaultMin, calcMin));
}

float Screen_CONFIG_PLATELET::getMaxYield (int btn_row)
{
   int   productNum = btn_row + _topProduct;
   float maxPltVol  = guiglobs::config_var_list.Var(KEY_PLT_VOLUME_1).Max();
   float defaultMax = guiglobs::config_var_list.Var(KEY_PLT_YIELD_1).Max();
   float finalConc  = _products[productNum].finalConc;

   float calcMax    = floorToTenth((maxPltVol * finalConc) / 1.0E5f);

   DataLog(log_level_gui_info) << "Yield: (Hard Max = " << defaultMax << ", Soft Max = "
                               << calcMax << ")" << endmsg;

   if (calcMax < defaultMax) return calcMax;
   return defaultMax;
}

float Screen_CONFIG_PLATELET::getMinYield (int btn_row)
{
   int   productNum    = btn_row + _topProduct;
   bool  pasOn         = guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on && guiglobs::cdsData.rwConfig.templates.key_plt_mss[productNum];
   float minPltVol     = guiglobs::config_var_list.Var(KEY_PLT_VOLUME_1).Min();
   float defaultMin    = guiglobs::config_var_list.Var(KEY_PLT_YIELD_1).Min();
   float finalConc     = _products[productNum].finalConc;
   float percentPlasma = (pasOn) ? ((float)guiglobs::cdsData.rwConfig.templates.key_plt_pct_carryover[productNum]) / 100.0f : 1;

   float calcMin       = ceilToTenth((minPltVol * finalConc) / (percentPlasma * 1.0E5f));

   DataLog(log_level_gui_info) << "Yield: (Hard Min = " << defaultMin << ", Soft Min = "
                               << calcMin << ")" << endmsg;

   if (calcMin > defaultMin) return calcMin;
   return defaultMin;
}


void Screen_CONFIG_PLATELET::checkForTimeOnlyConfig ()
{
   if (guiglobs::timeOnlyConfigMode)
   {
      DataLog (log_level_gui_info) << "Buttons should be disabled." << endmsg;

      disableConfigBtn(_btnYsf);
      disableConfigBtn(_btnDonorsAllowed);

      for (int rowNum = 0; rowNum < MAX_ROWS; rowNum++)
      {
         bool pasOn =
            guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on &&
            guiglobs::cdsData.rwConfig.templates.key_plt_mss[rowNum + _topProduct];

         if (_graphicalRows[rowNum].btnRowBoxAndPasToggle.btnBmpPtr)
            disableConfigBtn(*_graphicalRows[rowNum].btnRowBoxAndPasToggle.btnBmpPtr, ((pasOn) ? BITMAP_MSS_BUTTON_ICON_GRAY_DOWN : BITMAP_MSS_BUTTON_ICON_GRAY));

         if (_graphicalRows[rowNum].btnYield.btnBmpPtr)
            disableConfigBtn(*_graphicalRows[rowNum].btnYield.btnBmpPtr, BITMAP_CONFIG_SUB_NARROW_GRAY);

         if (_graphicalRows[rowNum].btnPasPlasmaPercent.btnBmpPtr)
            disableConfigBtn(*_graphicalRows[rowNum].btnPasPlasmaPercent.btnBmpPtr, BITMAP_CONFIG_SUB_NARROW_GRAY);

         if (_graphicalRows[rowNum].btnProductVol.btnBmpPtr)
            disableConfigBtn(*_graphicalRows[rowNum].btnProductVol.btnBmpPtr, BITMAP_CONFIG_SUB_NARROW_GRAY);

         if (_graphicalRows[rowNum].btnFinalConc.btnBmpPtr)
            disableConfigBtn(*_graphicalRows[rowNum].btnFinalConc.btnBmpPtr, BITMAP_CONFIG_SUB_NARROW_GRAY);

         if (_graphicalRows[rowNum].txtCollectConc)
            disableConfigTxt(*_graphicalRows[rowNum].txtCollectConc);

         if (_graphicalRows[rowNum].txtPasVol)
            disableConfigTxt(*_graphicalRows[rowNum].txtPasVol);
      }

      disableConfigTxt(_txtYieldLbl);
      disableConfigTxt(_txtVolLbl);
      disableConfigTxt(_txtFinalConcLbl);
      disableConfigTxt(_txtCollectConcLbl);
      disableConfigTxt(_txtPasVolLbl);
      disableConfigTxt(_txtPlasmaPercentLbl);
      disableConfigTxt(_toThe3rdCollectConc);
      disableConfigTxt(_toThe3rdFinalConc);
      disableConfigTxt(_toThe11thYield);
      disableConfigTxt(_txtYsfLbl);
      disableConfigTxt(_txtDonorsAllowedLbl);
   }
}

//
///////////////////////////////////////////////////////////////////////////////
// end of cfg_platelet.cpp file
//

/* FORMAT HASH 306dc81a318ca8cff94ce9f030664c9b */

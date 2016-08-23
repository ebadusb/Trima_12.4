#include <vxworks.h>
#include <math.h>
#include <string>

/** include files **/

// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "an_alarm.h"
#include "software_cds.h"
#include "cobeconfig.h"


// BASE CLASS INCLUDE
#include "cfg_plt_amap.hpp"
#include "statline.hpp"

/** defines **/
#define PROD_NUM_COLUMN       19
#define MINYIELD_COLUMN      120
#define MAXYIELD_COLUMN      210
#define FINAL_CONC_COLUMN    300
#define CO_COLUMN            390
#define COLLECT_CONC_COLUMN  449
#define SS_COLUMN            534

#define CFG_PLT_Y_COORD(__ROWNUM__) 128 + (__ROWNUM__ * 85)
#define CFG_PLT_PAS_Y_COORD(__ROWNUM__) 170 + (__ROWNUM__ * 85)
#define CFGPLTLINEROW(__ROWNUM__) 219 + (__ROWNUM__ * 65)

#define PLT_BUTTON_PROD(row) ((row==0) ? (GUI_BUTTON_CONFIG_PLT_PROD_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PLT_PROD_2) : (GUI_BUTTON_CONFIG_PLT_PROD_3)))
#define PLT_BUTTON_YIELD(row) ((row==0) ? (GUI_BUTTON_CONFIG_PLT_YIELD_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PLT_YIELD_2) : (GUI_BUTTON_CONFIG_PLT_YIELD_3)))
#define PLT_BUTTON_MAXYIELD(row) ((row==0) ? (GUI_BUTTON_CONFIG_PLT_YIELD_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PLT_YIELD_2) : (GUI_BUTTON_CONFIG_PLT_YIELD_3)))
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
#define CFG_PLT_MAG_GLASS_MIDDLE_Y 263
#define CFG_PLT_MAG_GLASS_BOTTOM_Y 348
#define CFG_PLT_MAG_GLASS_GAP_Y 42

#if (CPU == SIMNT && !defined(__DEFINED_THE_ROUND_FUNCTION__))
#define __DEFINED_THE_ROUND_FUNCTION__
// Apparently the "round()" function works on Trima, but not in the
// simulator.  Let's define it as a macro for the simulator only.
#define round(__OPERAND__) (( ( __OPERAND__ )-(float)floor(( __OPERAND__ )) > 0.5 ) ? (float)floor(( __OPERAND__ )) + 1 : (float)floor(( __OPERAND__ )))
#endif

#define roundToTenth(__OPERAND__) (round(((float)(__OPERAND__)) * 10) / 10)
#define floorToTenth(__OPERAND__) (floor(((float)(__OPERAND__)) * 10) / 10)
#define ceilToTenth(__OPERAND__)  (ceil(((float)(__OPERAND__)) * 10) / 10)

static void cbButton (void* data);
static void cbPltYsfButton (void* data);
static void cbMaleOnlyPltBtn (void* data);

bool Screen_PLT_AMAP::_btnLock = false;

///////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR


Screen_PLT_AMAP::Screen_PLT_AMAP()
   : Screen_Config(TEAL, GUI_SCREEN_CFG_PLT_AMAP, textCfgScreenLabelPlt, BITMAP_DEFAULT_BACKGROUND2),

     _txtMinYieldLbl          (TEXT_CFG_PLT_AMAP_MIN_YIELD_COL_LBL),
     _txtMaxYieldLbl          (TEXT_CFG_PLT_AMAP_MAX_YIELD_COL_LBL),
     _txtFinalConcLbl         (TEXT_CFG_PLT_AMAP_FINAL_CONC_COL_LBL),
     _txtPlasmaPercentLbl     (TEXT_CFG_PLT_AMAP_PPCO_COL_LBL),
     _txtCollectConcLbl       (TEXT_CFG_PLT_AMAP_CONC_COL_LBL),
     _txtPASRangeLbl          (TEXT_CFG_PLT_AMAP_SS_RANGE_COL_LBL),

     _toThe3rdCollectConc(TEXT_CFG_PLT_AMAP_FINALCONC_EXP_COL_LBL),
     _toThe3rdFinalConc(TEXT_CFG_PLT_AMAP_CONC_EXP_COL_LBL),

     _toThe11thMinYield(TEXT_CFG_PLT_AMAP_MIN_YIELD_EXP_COL_LBL),
     _toThe11thMaxYield(TEXT_CFG_PLT_AMAP_MAX_YIELD_EXP_COL_LBL),

     _txtYsfLbl(textButtonLabelCfgLeftPltAmapYsf),
     _txtDonorsAllowedLbl(textButtonLabelCfgLeftPltAmapTrali),

     _topProduct(0),
     _callbackBtnType(INACTIVE),
     _productIndicatorPtr(NULL),
     _callbackProduct(0),

     _btnYsf(
        220,
        380,
        guiglobs::cdsData.rwConfig.predict.key_ysf,
        KEY_YSF,
        BITMAP_CONFIG_AMAP_SUB_NARROW_UP,
        BITMAP_CONFIG_AMAP_SUB_NARROW_DOWN,
        GUI_BUTTON_CONFIG_YSF),

     _btnDonorsAllowed (
        460,
        380,
        guiglobs::cdsData.rwConfig.predict.key_male_only_plt,
        KEY_MALE_ONLY_PLT,
        BITMAP_CONFIG_AMAP_SUB_NARROW_UP,
        BITMAP_CONFIG_AMAP_SUB_NARROW_DOWN,
        GUI_BUTTON_CONFIG_MALE_ONLY_PLT)

{
   _btnLock = false;
   _pasOn   = false;

   memset(&_graphicalRows, sizeof(_graphicalRows), 0);
   memset(&_graphicalPasRows, sizeof(_graphicalPasRows), 0);
   memset(&_products, sizeof(_products), 0);

   initRowInfo();
   initProcedureInfo();
}



//
////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_PLT_AMAP::~Screen_PLT_AMAP()
{
   Screen_PLT_AMAP::deallocate_resources ();
}  // END of Screen_PLT_AMAP DESTRUCTOR



//
////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_PLT_AMAP::allocate_resources (const char* allocation_parameter)
{
   _btnLock                = false;

   _topProduct             = 0;
   OSRegion widget_positions;
   widget_positions.width  = LABEL_AREA;
   widget_positions.height = 70;

   _pasOn                  = guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on;
   DataLog (log_level_gui_info) << "_pasOn: " <<  _pasOn << endmsg;

   // allocate parentScreenPtr's resources first
   Screen_Config::allocate_resources (allocation_parameter);

   // then allocate constituent objects with this window as parentScreenPtr widget
   // layer allocation from Z-back to Z-front.

   _btnYsf.allocate_resources(*this);
   _btnYsf.set_callback(cbPltYsfButton, (void*)this);

   _btnDonorsAllowed.allocate_resources(*this);
   _btnDonorsAllowed.set_callback(cbMaleOnlyPltBtn, (void*)this);


   _txtMinYieldLbl.allocate_resources(*this) ;
   _txtMaxYieldLbl.allocate_resources(*this);
   _txtFinalConcLbl.allocate_resources(*this);
   _txtCollectConcLbl.allocate_resources(*this);
   _txtPASRangeLbl.allocate_resources(*this);
   _txtPlasmaPercentLbl.allocate_resources(*this);
   _toThe3rdCollectConc.allocate_resources(*this);
   _toThe3rdFinalConc.allocate_resources(*this);
   _toThe11thMinYield.allocate_resources(*this);
   _toThe11thMaxYield.allocate_resources(*this);
   _txtYsfLbl.allocate_resources(*this);
   _txtDonorsAllowedLbl.allocate_resources(*this);


   // Allocate stuff in rows
   for (int rowNum = 0; rowNum < MAX_AMAP_ROWS; rowNum++)
   {
      ////////////  PRODUCT Button //////////////////////////////////
      _graphicalRows[rowNum].btnProdLabel.btnBmpPtr =
         new Button_Bitmap (PLT_BUTTON_PROD(rowNum),
                            BITMAP_CFGPLT_LABEL_BOX1_X,
                            CFG_PLT_Y_COORD(rowNum),
                            _products[rowNum + _topProduct].productLblString,
                            BITMAP_CONFIG_AMAP_SUB_DOWN,
                            BITMAP_CONFIG_AMAP_SUB_DOWN,
                            false, false);


      ////////////  Min Yield Button //////////////////////////////////
      _graphicalRows[rowNum].btnMinYield.btnBmpPtr =
         new Button_Bitmap (PLT_BUTTON_YIELD(rowNum),
                            (short)_graphicalRows[rowNum].btnMinYield.xPos,
                            (short)_graphicalRows[rowNum].btnMinYield.yPos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            BITMAP_CONFIG_AMAP_SUB_NARROW_UP,
                            BITMAP_CONFIG_AMAP_SUB_NARROW_DOWN);


      ////////////  Max Yield Button //////////////////////////////////
      _graphicalRows[rowNum].btnMaxYield.btnBmpPtr =
         new Button_Bitmap (PLT_BUTTON_MAXYIELD(rowNum),
                            (short)_graphicalRows[rowNum].btnMaxYield.xPos,
                            (short)_graphicalRows[rowNum].btnMaxYield.yPos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            BITMAP_CONFIG_AMAP_SUB_NARROW_UP,
                            BITMAP_CONFIG_AMAP_SUB_NARROW_DOWN);


      ////////////  Final Conc Button //////////////////////////////////
      _graphicalRows[rowNum].btnFinalConc.btnBmpPtr =
         new Button_Bitmap (PLT_BUTTON_FINAL_CONC(rowNum),
                            (short)_graphicalRows[rowNum].btnFinalConc.xPos,
                            (short)_graphicalRows[rowNum].btnFinalConc.yPos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            BITMAP_CONFIG_AMAP_SUB_NARROW_UP,
                            BITMAP_CONFIG_AMAP_SUB_NARROW_DOWN);


      ////////////  Plasma percentage text //////////////////////////////////
      _graphicalRows[rowNum].txtPasSep = new Display_Text(textEmptyFormatCfgPltPasPercent);
      _graphicalRows[rowNum].txtPasSep->allocate_resources(*this);
      widget_positions.x               =  CO_COLUMN - 30;
      widget_positions.y               =  (short)(_graphicalRows[rowNum].btnMinYield.yPos - 15);
      _graphicalRows[rowNum].txtPasSep->set_requested_region(widget_positions);


      ////////////  Collect text //////////////////////////////////
      _graphicalRows[rowNum].txtCollectConc = new Display_Text(textEmptyFormatCfgPltCollectConc);
      _graphicalRows[rowNum].txtCollectConc->allocate_resources(*this);
      widget_positions.x                    =  COLLECT_CONC_COLUMN;
      widget_positions.y                    =  (short)(_graphicalRows[rowNum].btnMinYield.yPos - 15);
      _graphicalRows[rowNum].txtCollectConc->set_requested_region(widget_positions);


      ////////////  Pas text //////////////////////////////////
      _graphicalRows[rowNum].txtMinPasVol = new Display_Text(TEXT_CFG_PAS_MIN_VOL);
      _graphicalRows[rowNum].txtMinPasVol->allocate_resources(*this);
      widget_positions.x                  =  SS_COLUMN;
      widget_positions.y                  =  (short)(_graphicalRows[rowNum].btnMinYield.yPos - 26);
      _graphicalRows[rowNum].txtMinPasVol->set_requested_region(widget_positions);

      _graphicalRows[rowNum].txtMaxPasVol = new Display_Text(TEXT_CFG_PAS_MAX_VOL);
      _graphicalRows[rowNum].txtMaxPasVol->allocate_resources(*this);
      widget_positions.x                  =  SS_COLUMN;
      widget_positions.y                  =  (short)(_graphicalRows[rowNum].btnMinYield.yPos - 6);
      _graphicalRows[rowNum].txtMaxPasVol->set_requested_region(widget_positions);

      /////////// Allocate resources /////////////////////////////
      _graphicalRows[rowNum].btnProdLabel.btnBmpPtr->allocate_resources(*this);
      _graphicalRows[rowNum].btnProdLabel.btnPtr = _graphicalRows[rowNum].btnProdLabel.btnBmpPtr;
      _graphicalRows[rowNum].btnProdLabel.btnBmpPtr->set_disabled(true);


      _graphicalRows[rowNum].btnMinYield.btnBmpPtr->allocate_resources(*this);
      _graphicalRows[rowNum].btnMinYield.btnPtr = _graphicalRows[rowNum].btnMinYield.btnBmpPtr;
      _graphicalRows[rowNum].btnMinYield.btnBmpPtr->set_callback(cbButton, (void*)&_graphicalRows[rowNum].btnMinYield);


      _graphicalRows[rowNum].btnMaxYield.btnBmpPtr->allocate_resources(*this);
      _graphicalRows[rowNum].btnMaxYield.btnPtr = _graphicalRows[rowNum].btnMaxYield.btnBmpPtr;
      _graphicalRows[rowNum].btnMaxYield.btnBmpPtr->set_callback(cbButton, (void*)&_graphicalRows[rowNum].btnMaxYield);


      _graphicalRows[rowNum].btnFinalConc.btnBmpPtr->allocate_resources(*this);
      _graphicalRows[rowNum].btnFinalConc.btnPtr = _graphicalRows[rowNum].btnFinalConc.btnBmpPtr;
      _graphicalRows[rowNum].btnFinalConc.btnBmpPtr->set_callback(cbButton, (void*)&_graphicalRows[rowNum].btnFinalConc);

   }
   if (_pasOn)
   {
      for (int rowNum = 0; rowNum < MAX_AMAP_ROWS; rowNum++)
      {
         ////////////  PRODUCT Button //////////////////////////////////
         _graphicalPasRows[rowNum].btnProdLabel.btnBmpPtr =
            new Button_Bitmap (PLT_BUTTON_PROD(rowNum),
                               BITMAP_CFGPLT_LABEL_BOX1_X,
                               CFG_PLT_PAS_Y_COORD(rowNum),
                               _products[rowNum + _topProduct].productLblString,
                               BITMAP_MSS_AMAP_BUTTON_ICON_DOWN,
                               BITMAP_MSS_AMAP_BUTTON_ICON_DOWN,
                               false, false);


         _graphicalPasRows[rowNum].btnFinalConc.btnBmpPtr =
            new Button_Bitmap (PLT_BUTTON_FINAL_CONC(rowNum),
                               (short)_graphicalPasRows[rowNum].btnFinalConc.xPos,
                               (short)_graphicalPasRows[rowNum].btnFinalConc.yPos,
                               textEmptyFormatCfgNumericTextFormatNull,
                               BITMAP_CONFIG_AMAP_SUB_NARROW_UP,
                               BITMAP_CONFIG_AMAP_SUB_NARROW_DOWN);


         ////////////  PPCO  Button //////////////////////////////////
         _graphicalPasRows[rowNum].btnPasPlasmaPercent.btnBmpPtr =
            new Button_Bitmap (PLT_BUTTON_PLASMA_PERCENT(rowNum),
                               (short)_graphicalPasRows[rowNum].btnPasPlasmaPercent.xPos,
                               (short)_graphicalPasRows[rowNum].btnPasPlasmaPercent.yPos,
                               textEmptyFormatCfgNumericTextFormatNull,
                               BITMAP_CONFIG_AMAP_SUB_NARROW_UP,
                               BITMAP_CONFIG_AMAP_SUB_NARROW_DOWN);


         _graphicalPasRows[rowNum].btnProdLabel.btnBmpPtr->allocate_resources(*this);
         _graphicalPasRows[rowNum].btnProdLabel.btnPtr = _graphicalPasRows[rowNum].btnProdLabel.btnBmpPtr;
         _graphicalPasRows[rowNum].btnProdLabel.btnBmpPtr->set_disabled(true);

         _graphicalPasRows[rowNum].btnFinalConc.btnBmpPtr->allocate_resources(*this);
         _graphicalPasRows[rowNum].btnFinalConc.btnPtr = _graphicalPasRows[rowNum].btnFinalConc.btnBmpPtr;
         _graphicalPasRows[rowNum].btnFinalConc.btnBmpPtr->set_callback(cbButton, (void*)&_graphicalPasRows[rowNum].btnFinalConc);

         _graphicalPasRows[rowNum].btnPasPlasmaPercent.btnBmpPtr->allocate_resources(*this);
         _graphicalPasRows[rowNum].btnPasPlasmaPercent.btnPtr = _graphicalPasRows[rowNum].btnFinalConc.btnBmpPtr;
         _graphicalPasRows[rowNum].btnPasPlasmaPercent.btnBmpPtr->set_callback(cbButton, (void*)&_graphicalPasRows[rowNum].btnPasPlasmaPercent);

         ////////////  Collect text //////////////////////////////////
         _graphicalPasRows[rowNum].txtCollectConc = new Display_Text(textEmptyFormatCfgPltCollectConc);
         _graphicalPasRows[rowNum].txtCollectConc->allocate_resources(*this);
         widget_positions.x                       =  COLLECT_CONC_COLUMN;
         widget_positions.y                       =  (short)(_graphicalRows[rowNum].btnMinYield.yPos + CFG_PLT_MAG_GLASS_GAP_Y - 15);
         _graphicalPasRows[rowNum].txtCollectConc->set_requested_region(widget_positions);

         ///////////  Min and Max yield ///////////////////////////////////////
         _graphicalPasRows[rowNum].txtMinYield = new Display_Text();
         _graphicalPasRows[rowNum].txtMinYield->allocate_resources(*this);
         widget_positions.x                    =  MINYIELD_COLUMN - 32;
         widget_positions.y                    =  (short)(_graphicalRows[rowNum].btnMinYield.yPos + 24);
         _graphicalPasRows[rowNum].txtMinYield->set_requested_region(widget_positions);
         _graphicalPasRows[rowNum].txtMinYield->set_color(YELLOW);

         _graphicalPasRows[rowNum].txtMaxYield = new Display_Text();
         _graphicalPasRows[rowNum].txtMaxYield->allocate_resources(*this);
         widget_positions.x                    =  MAXYIELD_COLUMN - 32;
         widget_positions.y                    =  (short)(_graphicalRows[rowNum].btnMinYield.yPos + 24);
         _graphicalPasRows[rowNum].txtMaxYield->set_requested_region(widget_positions);
         _graphicalPasRows[rowNum].txtMaxYield->set_color(YELLOW);

         ////////////  Pas text //////////////////////////////////
         _graphicalPasRows[rowNum].txtMinPasVol = new Display_Text(TEXT_CFG_PAS_MIN_VOL);
         _graphicalPasRows[rowNum].txtMinPasVol->allocate_resources(*this);
         widget_positions.x                     =  SS_COLUMN;
         widget_positions.y                     =  (short)(_graphicalRows[rowNum].btnMinYield.yPos + 14);
         _graphicalPasRows[rowNum].txtMinPasVol->set_requested_region(widget_positions);

         _graphicalPasRows[rowNum].txtMaxPasVol = new Display_Text(TEXT_CFG_PAS_MAX_VOL);
         _graphicalPasRows[rowNum].txtMaxPasVol->allocate_resources(*this);
         widget_positions.x                     =  SS_COLUMN;
         widget_positions.y                     =  (short)(_graphicalRows[rowNum].btnMinYield.yPos + 34);
         _graphicalPasRows[rowNum].txtMaxPasVol->set_requested_region(widget_positions);
      }


      _graphicalPasRows[0].bmpMagGlass = new Bitmap(CFG_PLT_MAG_GLASS_X,
                                                    CFG_PLT_MAG_GLASS_TOP_Y,
                                                    BITMAP_CFGPP_MAG_GLASS_TOP);
      _graphicalPasRows[1].bmpMagGlass = new Bitmap(CFG_PLT_MAG_GLASS_X,
                                                    CFG_PLT_MAG_GLASS_MIDDLE_Y,
                                                    BITMAP_CFGPP_MAG_GLASS_MIDDLE);
      _graphicalPasRows[2].bmpMagGlass = new Bitmap(CFG_PLT_MAG_GLASS_X,
                                                    CFG_PLT_MAG_GLASS_BOTTOM_Y,
                                                    BITMAP_CFGPP_MAG_GLASS_BOTTOM);
   }  // if (_pasOn)
   labelAllButtons();

//   checkForTimeOnlyConfig();
} // END of Screen_PLT_AMAP ALLOCATE_RESOURCES

//
////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_PLT_AMAP::deallocate_resources ()
{
   _btnLock = false;

   _txtMinYieldLbl.deallocate_resources();
   _txtMaxYieldLbl.deallocate_resources();
   _txtFinalConcLbl.deallocate_resources();
   _txtCollectConcLbl.deallocate_resources();
   _txtPASRangeLbl.deallocate_resources();
   _txtPlasmaPercentLbl.deallocate_resources();
   _btnYsf.deallocate_resources();
   _btnDonorsAllowed.deallocate_resources();
   _toThe3rdCollectConc.deallocate_resources();
   _toThe3rdFinalConc.deallocate_resources();
   _toThe11thMinYield.deallocate_resources();
   _toThe11thMaxYield.deallocate_resources();
   _txtYsfLbl.deallocate_resources();
   _txtDonorsAllowedLbl.deallocate_resources();

   delete _productIndicatorPtr;
   _productIndicatorPtr = NULL;

   for (int rowNum = 0; rowNum < MAX_AMAP_ROWS; rowNum++)
   {
      _graphicalRows[rowNum].btnProdLabel.btnBmpPtr->deallocate_resources();
      delete _graphicalRows[rowNum].btnProdLabel.btnBmpPtr;
      _graphicalRows[rowNum].btnProdLabel.btnBmpPtr = NULL;

      _graphicalRows[rowNum].btnMinYield.btnBmpPtr->deallocate_resources();
      delete _graphicalRows[rowNum].btnMinYield.btnBmpPtr;
      _graphicalRows[rowNum].btnMinYield.btnBmpPtr = NULL;

      _graphicalRows[rowNum].btnMaxYield.btnBmpPtr->deallocate_resources();
      delete _graphicalRows[rowNum].btnMaxYield.btnBmpPtr;
      _graphicalRows[rowNum].btnMaxYield.btnBmpPtr = NULL;

      _graphicalRows[rowNum].txtMinPasVol->deallocate_resources();
      delete _graphicalRows[rowNum].txtMinPasVol;
      _graphicalRows[rowNum].txtMinPasVol = NULL;

      _graphicalRows[rowNum].txtPasSep->deallocate_resources();
      delete _graphicalRows[rowNum].txtPasSep;
      _graphicalRows[rowNum].txtPasSep = NULL;

      _graphicalRows[rowNum].txtMaxPasVol->deallocate_resources();
      delete _graphicalRows[rowNum].txtMaxPasVol;
      _graphicalRows[rowNum].txtMaxPasVol = NULL;

      _graphicalRows[rowNum].txtCollectConc->deallocate_resources();
      delete _graphicalRows[rowNum].txtCollectConc;
      _graphicalRows[rowNum].txtCollectConc = NULL;

      _graphicalRows[rowNum].btnFinalConc.btnBmpPtr->deallocate_resources();
      delete _graphicalRows[rowNum].btnFinalConc.btnBmpPtr;
      _graphicalRows[rowNum].btnFinalConc.btnBmpPtr = NULL;

      if (_pasOn)
      {
         _graphicalPasRows[rowNum].btnProdLabel.btnBmpPtr->deallocate_resources();
         delete _graphicalPasRows[rowNum].btnProdLabel.btnBmpPtr;
         _graphicalPasRows[rowNum].btnProdLabel.btnBmpPtr = NULL;

         _graphicalPasRows[rowNum].txtMinYield->deallocate_resources();
         delete _graphicalPasRows[rowNum].txtMinYield;
         _graphicalPasRows[rowNum].txtMinYield = NULL;

         _graphicalPasRows[rowNum].txtMaxYield->deallocate_resources();
         delete _graphicalPasRows[rowNum].txtMaxYield;
         _graphicalPasRows[rowNum].txtMaxYield = NULL;

         _graphicalPasRows[rowNum].txtMinPasVol->deallocate_resources();
         delete _graphicalPasRows[rowNum].txtMinPasVol;
         _graphicalPasRows[rowNum].txtMinPasVol = NULL;

         _graphicalPasRows[rowNum].txtMaxPasVol->deallocate_resources();
         delete _graphicalPasRows[rowNum].txtMaxPasVol;
         _graphicalPasRows[rowNum].txtMaxPasVol = NULL;

         _graphicalPasRows[rowNum].txtCollectConc->deallocate_resources();
         delete _graphicalPasRows[rowNum].txtCollectConc;
         _graphicalPasRows[rowNum].txtCollectConc = NULL;

         _graphicalPasRows[rowNum].btnFinalConc.btnBmpPtr->deallocate_resources();
         delete _graphicalPasRows[rowNum].btnFinalConc.btnBmpPtr;
         _graphicalPasRows[rowNum].btnFinalConc.btnBmpPtr = NULL;

         _graphicalPasRows[rowNum].btnPasPlasmaPercent.btnBmpPtr->deallocate_resources();
         delete _graphicalPasRows[rowNum].btnPasPlasmaPercent.btnBmpPtr;
         _graphicalPasRows[rowNum].btnPasPlasmaPercent.btnBmpPtr = NULL;

         _graphicalPasRows[rowNum].bmpMagGlass->deallocate_resources();
         delete _graphicalPasRows[rowNum].bmpMagGlass;
         _graphicalPasRows[rowNum].bmpMagGlass = NULL;
      }
   }

   // deactivate the status line before leaving
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

   // deallocate parentScreenPtr's resources
   Screen_Config::deallocate_resources ();
}  // END of Screen_PLT_AMAP DEALLOCATE_RESOURCES



//////////////////////////////////////////////////////////////////////////
//
//		CALLBACK FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
void cbButton (void* data)
{
   Screen_PLT_AMAP::BTN_INFO* btn_info = (Screen_PLT_AMAP::BTN_INFO*)data;

   if (!Screen_PLT_AMAP::_btnLock && btn_info->rowNum >= 0)
   {
      Screen_PLT_AMAP::_btnLock = true;

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
         case Screen_PLT_AMAP::MINYIELD :  // assumes min and max are the same for all yields
         {


            if (btn_info->rowNum == 0)
               min = 1.0f;
            else
               min = Screen_PLT_AMAP::getMaxAmapYield(btn_info->rowNum - 1) + 0.1f;  // guiglobs::cdsData.rwConfig.templates.key_plt_amap_max_yield[btn_info->rowNum-1] + 0.1f;



            max = Screen_PLT_AMAP::getMaxAmapYield(btn_info->rowNum) - 0.1f;   // guiglobs::cdsData.rwConfig.templates.key_plt_amap_max_yield[btn_info->rowNum] - 0.1f;


            minMaxFormat = "%.1f";
            Screen_Config::build_keypad_string (keypadString,
                                                KEY_PLT_YIELD_1,   // unused
                                                PR_KEYPAD_X,
                                                PR_KEYPAD_Y,
                                                "##.#",
                                                max + 0.002f,
                                                min - 0.002f);

            break;
         }
         case Screen_PLT_AMAP::MAXYIELD :   // assumes min and max are the same for all yields
         {


            min = Screen_PLT_AMAP::getMinAmapYield(btn_info->rowNum) + 0.1f;     // guiglobs::cdsData.rwConfig.templates.key_plt_amap_min_yield[btn_info->rowNum] + 0.1f;

            if (btn_info->rowNum == 2)
               max = 15.0f;
            else
               max = Screen_PLT_AMAP::getMinAmapYield(btn_info->rowNum + 1) - 0.1f;     // guiglobs::cdsData.rwConfig.templates.key_plt_amap_min_yield[btn_info->rowNum+1] - 0.1f;


            minMaxFormat = "%.1f";
            Screen_Config::build_keypad_string (keypadString,
                                                KEY_PLT_YIELD_1,    // unused
                                                PR_KEYPAD_X,
                                                PR_KEYPAD_Y,
                                                "##.#",
                                                max + 0.002f,
                                                min - 0.002f);
            break;
         }


         case Screen_PLT_AMAP::FINAL_CONC :
         {
            min = 100.0f;
            max = 10000.0f;

            Screen_Config::build_keypad_string(keypadString,
                                               KEY_PLT_VOLUME_1,   // unused
                                               PR_KEYPAD_X,
                                               PR_KEYPAD_Y,
                                               "######",
                                               max,
                                               min);
            break;
         }

         case Screen_PLT_AMAP::PLASMA_PERCENT :
         {
            min = 1.0;
            max = 99.0;

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
      bool wideDisplay = true;
      (btn_info->parentScreenPtr)->display_min_max_keypad_limits(min, max, minMaxFormat.c_str(), wideDisplay);
   }
   else
   {
      DataLog (log_level_gui_error) << "Button lock error (Button " << btn_info->btnType << ")" << endmsg;
   }
} // end of cbButton

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Message handler for KEYPADS
void Screen_PLT_AMAP::update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer)
{

   float collect_conc, final_conc;

   delete _productIndicatorPtr;
   _productIndicatorPtr = NULL;

   _btnLock             = false;

   turn_off_min_max_keypad_text();

   if ((return_keypad_buffer.keypad_return_value) != -1)
   {
      char  tempString[10];
      int   stringLen;
      float the_data = atof(return_keypad_buffer.keypad_return_string);

      DataLog(log_level_gui_info) << "the_data: " << the_data << " _callbackBtnType: " << _callbackBtnType
                                  << " _callbackProduct: " << _callbackProduct << endmsg;

      switch (_callbackBtnType)
      {
         case MINYIELD :
            guiglobs::cdsData.rwConfig.templates.key_plt_amap_min_yield[_callbackProduct] = the_data;
            labelButtonRow(_callbackProduct - _topProduct, _callbackProduct, collect_conc, final_conc);

            if (_pasOn)
            {
               labelButtonRow(_callbackProduct - _topProduct + MAX_AMAP_ROWS, _callbackProduct + MAX_AMAP_ROWS, collect_conc, final_conc);
            }
            break;

         case MAXYIELD :
            guiglobs::cdsData.rwConfig.templates.key_plt_amap_max_yield[_callbackProduct] = the_data;
            labelButtonRow(_callbackProduct - _topProduct, _callbackProduct, collect_conc, final_conc);

            if (_pasOn)
            {
               labelButtonRow(_callbackProduct - _topProduct + MAX_AMAP_ROWS, _callbackProduct + MAX_AMAP_ROWS, collect_conc, final_conc);
            }
            break;

         case FINAL_CONC :
            guiglobs::cdsData.rwConfig.templates.key_plt_amap_conc[_callbackProduct] = the_data;
            _products[_callbackProduct].finalConc = the_data;
            labelButtonRow(_callbackProduct - _topProduct, _callbackProduct, collect_conc, final_conc);
            displayAlert(collect_conc, final_conc);
            break;

         case YSF :
            guiglobs::cdsData.rwConfig.predict.key_ysf = the_data;
            _btnYsf.set_text_direct (10, return_keypad_buffer.keypad_return_string);
            break;

         case PLASMA_PERCENT :
            guiglobs::cdsData.rwConfig.templates.key_plt_amap_pas_pct_carryover[_callbackProduct % MAX_AMAP_ROWS] = (int)the_data;
            labelButtonRow(_callbackProduct - _topProduct, _callbackProduct, collect_conc, final_conc);
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
      if (_callbackBtnType == FINAL_CONC)
      {}
   }

} // end of update_screen_with_keypad_data


//
///////////////////////////////////////////////////////////////////////////////
// Message handler for LISTBOXS
void Screen_PLT_AMAP::update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer)
{
   _btnLock = false;
   _btnDonorsAllowed.display_the_text_string (rtn_listbox_buffer.key_value);
   guiglobs::cdsData.rwConfig.predict.key_male_only_plt = int(rtn_listbox_buffer.key_value);
} // end of update_screen_with_date method

//////////////////////////////////////////////////////////////////////////
//
//		UTILITY FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////

void Screen_PLT_AMAP::initRowInfo ()
{
   for (int i = 0; i < MAX_AMAP_ROWS; i++)
   {
      _graphicalRows[i].btnMinYield.xPos             = MINYIELD_COLUMN;
      _graphicalRows[i].btnMinYield.yPos             = CFG_PLT_Y_COORD(i);
      _graphicalRows[i].btnMinYield.btnPtr           = NULL;
      _graphicalRows[i].btnMinYield.btnBmpPtr        = NULL;
      _graphicalRows[i].btnMinYield.parentScreenPtr  = this;
      _graphicalRows[i].btnMinYield.btnType          = MINYIELD;
      _graphicalRows[i].btnMinYield.rowNum           = i;

      _graphicalRows[i].btnMaxYield.xPos             = MAXYIELD_COLUMN;
      _graphicalRows[i].btnMaxYield.yPos             = CFG_PLT_Y_COORD(i);
      _graphicalRows[i].btnMaxYield.btnPtr           = NULL;
      _graphicalRows[i].btnMaxYield.btnBmpPtr        = NULL;
      _graphicalRows[i].btnMaxYield.parentScreenPtr  = this;
      _graphicalRows[i].btnMaxYield.btnType          = MAXYIELD;
      _graphicalRows[i].btnMaxYield.rowNum           = i;

      _graphicalRows[i].btnFinalConc.xPos            = FINAL_CONC_COLUMN;
      _graphicalRows[i].btnFinalConc.yPos            = CFG_PLT_Y_COORD(i);
      _graphicalRows[i].btnFinalConc.btnPtr          = NULL;
      _graphicalRows[i].btnFinalConc.btnBmpPtr       = NULL;
      _graphicalRows[i].btnFinalConc.parentScreenPtr = this;
      _graphicalRows[i].btnFinalConc.btnType         = FINAL_CONC;
      _graphicalRows[i].btnFinalConc.rowNum          = i;

      _graphicalRows[i].txtMinPasVol                 = 0;
      _graphicalRows[i].txtMaxPasVol                 = 0;
      _graphicalRows[i].txtPasSep                    = 0;

      _graphicalRows[i].txtCollectConc               = 0;


      _graphicalPasRows[i].btnFinalConc.xPos                   = FINAL_CONC_COLUMN;
      _graphicalPasRows[i].btnFinalConc.yPos                   = CFG_PLT_PAS_Y_COORD(i);
      _graphicalPasRows[i].btnFinalConc.btnPtr                 = NULL;
      _graphicalPasRows[i].btnFinalConc.btnBmpPtr              = NULL;
      _graphicalPasRows[i].btnFinalConc.parentScreenPtr        = this;
      _graphicalPasRows[i].btnFinalConc.btnType                = FINAL_CONC;
      _graphicalPasRows[i].btnFinalConc.rowNum                 = i + MAX_AMAP_ROWS;

      _graphicalPasRows[i].btnPasPlasmaPercent.xPos            = CO_COLUMN;
      _graphicalPasRows[i].btnPasPlasmaPercent.yPos            = CFG_PLT_PAS_Y_COORD(i);
      _graphicalPasRows[i].btnPasPlasmaPercent.btnPtr          = NULL;
      _graphicalPasRows[i].btnPasPlasmaPercent.btnBmpPtr       = NULL;
      _graphicalPasRows[i].btnPasPlasmaPercent.parentScreenPtr = this;
      _graphicalPasRows[i].btnPasPlasmaPercent.btnType         = PLASMA_PERCENT;
      _graphicalPasRows[i].btnPasPlasmaPercent.rowNum          = i + MAX_AMAP_ROWS;

      _graphicalPasRows[i].txtMinPasVol   = 0;
      _graphicalPasRows[i].txtMinYield    = 0;
      _graphicalPasRows[i].txtMaxYield    = 0;
      _graphicalPasRows[i].txtMaxPasVol   = 0;

      _graphicalPasRows[i].txtCollectConc = 0;
   }
}

void Screen_PLT_AMAP::initProcedureInfo ()
{
   _products[0].productIndicatorString = TEXT_CFG_PLT_AMAP_SINGLE_ROW_BTN;
   _products[0].productLblString       = TEXT_CFG_PLT_AMAP_SINGLE_ROW_BTN;
   _products[0].keyMinYield            = KEY_PLT_AMAP_MIN_YIELD_1;
   _products[0].keyMaxYield            = KEY_PLT_AMAP_MAX_YIELD_1;
   _products[0].keyAmapConc            = KEY_PLT_AMAP_CONC_1;

   _products[1].productIndicatorString = TEXT_CFG_PLT_AMAP_DOUBLE_ROW_BTN;
   _products[1].productLblString       = TEXT_CFG_PLT_AMAP_DOUBLE_ROW_BTN;
   _products[1].keyMinYield            = KEY_PLT_AMAP_MIN_YIELD_2;
   _products[1].keyMaxYield            = KEY_PLT_AMAP_MAX_YIELD_2;
   _products[1].keyAmapConc            = KEY_PLT_AMAP_CONC_2;

   _products[2].productIndicatorString = TEXT_CFG_PLT_AMAP_TRIPLE_ROW_BTN;
   _products[2].productLblString       = TEXT_CFG_PLT_AMAP_TRIPLE_ROW_BTN;
   _products[2].keyMinYield            = KEY_PLT_AMAP_MIN_YIELD_3;
   _products[2].keyMaxYield            = KEY_PLT_AMAP_MAX_YIELD_3;
   _products[2].keyAmapConc            = KEY_PLT_AMAP_CONC_3;

   _products[3].productIndicatorString = TEXT_CFG_PLT_AMAP_SINGLE_ROW_BTN;
   _products[3].productLblString       = TEXT_CFG_PLT_AMAP_SINGLE_ROW_BTN;
   _products[3].keyMinYield            = KEY_PLT_AMAP_MIN_YIELD_1;
   _products[3].keyMaxYield            = KEY_PLT_AMAP_MAX_YIELD_1;
   _products[3].keyAmapConc            = KEY_PLT_AMAP_CONC_4;
   _products[3].keyPasPlasmaPercent    = KEY_PLT_AMAP_PAS_CARRYOVER_1;

   _products[4].productIndicatorString = TEXT_CFG_PLT_AMAP_DOUBLE_ROW_BTN;
   _products[4].productLblString       = TEXT_CFG_PLT_AMAP_DOUBLE_ROW_BTN;
   _products[4].keyMinYield            = KEY_PLT_AMAP_MIN_YIELD_2;
   _products[4].keyMaxYield            = KEY_PLT_AMAP_MAX_YIELD_2;
   _products[4].keyAmapConc            = KEY_PLT_AMAP_CONC_5;
   _products[4].keyPasPlasmaPercent    = KEY_PLT_AMAP_PAS_CARRYOVER_2;

   _products[5].productIndicatorString = TEXT_CFG_PLT_AMAP_TRIPLE_ROW_BTN;
   _products[5].productLblString       = TEXT_CFG_PLT_AMAP_TRIPLE_ROW_BTN;
   _products[5].keyMinYield            = KEY_PLT_AMAP_MIN_YIELD_3;
   _products[5].keyMaxYield            = KEY_PLT_AMAP_MAX_YIELD_3;
   _products[5].keyAmapConc            = KEY_PLT_AMAP_CONC_6;
   _products[5].keyPasPlasmaPercent    = KEY_PLT_AMAP_PAS_CARRYOVER_3;
}

//
///////////////////////////////////////////////////////////////////////////////
// labelAllButtons
void Screen_PLT_AMAP::labelAllButtons ()
{
   int product_num = _topProduct;
   int nProducts   = MAX_AMAP_ROWS;

   if (_pasOn)
   {
      nProducts += MAX_AMAP_ROWS;
   }

   float dummy1, dummy2;
   for (int rowNum = 0; rowNum < nProducts; rowNum++, product_num++)
   {
      labelButtonRow(rowNum, product_num, dummy1, dummy2);
   }
} // end of labelAllButtons

///////////////////////////////////////////////////////////////////////////////
// labelButtonRow
void Screen_PLT_AMAP::labelButtonRow (const int rowNum, const int product_num, float& collectConc, float& finalConc)
{

   guistring tempString;
   int       tempStringLength = 0;

   float     minyield         = guiglobs::cdsData.rwConfig.templates.key_plt_amap_min_yield[product_num % MAX_AMAP_ROWS] * 1.0E5f;
   float     maxyield         = guiglobs::cdsData.rwConfig.templates.key_plt_amap_max_yield[product_num % MAX_AMAP_ROWS] * 1.0E5f;

   finalConc =  guiglobs::cdsData.rwConfig.templates.key_plt_amap_conc[product_num];

   float minTotalvolume =   minyield / finalConc;
   float maxTotalvolume =   maxyield / finalConc;

   float minPasVol      = 0.0;
   float maxPasVol      = 0.0;

   float minCollConc    = 0.0;
   float maxCollConc    = 0.0;

   float minCollVolm    = 0.0;
   float maxCollVolm    = 0.0;

   float percentPlasma  = 0.0;

   if (_pasOn)
   {
      percentPlasma = ( (float)guiglobs::cdsData.rwConfig.templates.key_plt_amap_pas_pct_carryover[product_num % MAX_AMAP_ROWS]) / 100.0f;

      minPasVol     = minTotalvolume *  ( 1.0 - percentPlasma ) ;
      maxPasVol     = maxTotalvolume *  ( 1.0 - percentPlasma ) ;

      minCollConc   =  minyield / (minTotalvolume - minPasVol);
      maxCollConc   =  maxyield / (maxTotalvolume - maxPasVol);

      minCollVolm   = (minTotalvolume - minPasVol);
      maxCollVolm   = (maxTotalvolume - maxPasVol);

   }

   DataLog (log_level_gui_info) << "AMAPP-Row : " << rowNum << " maxtotalVolume=" <<  maxTotalvolume << endmsg;
   DataLog (log_level_gui_info) << "AMAPP-Row : pasOn : minyield : maxyield : finalConc : plasma% : mincollectConc : maxcollectConc : minvol : maxvol : minpasVol : maxpasVol" << endmsg;
   DataLog (log_level_gui_info)
      << "              " << rowNum << " : "
      <<  _pasOn << " : "

      <<  guiglobs::cdsData.rwConfig.templates.key_plt_amap_min_yield[product_num % MAX_AMAP_ROWS] << " : "
      <<  guiglobs::cdsData.rwConfig.templates.key_plt_amap_max_yield[product_num % MAX_AMAP_ROWS] << " : "

      <<  finalConc << " : "

      <<  percentPlasma << " : "

      <<  minCollConc << " : "
      <<  maxCollConc << " : "

      <<  minCollVolm << " : "
      <<  maxCollVolm << " : "


      <<  minPasVol << " : "
      <<  maxPasVol << " : "
      << endmsg;


   // Set min yield button
   tempStringLength = sprintf_l10n(tempString, "%.1f", guiglobs::cdsData.rwConfig.templates.key_plt_amap_min_yield[product_num % MAX_AMAP_ROWS]);

   if (rowNum < MAX_AMAP_ROWS)
   {
      _graphicalRows[rowNum].btnMinYield.btnBmpPtr->set_text_direct (tempStringLength, tempString);
   }
   else
   {
      _graphicalPasRows[rowNum % MAX_AMAP_ROWS].txtMinYield->set_text_direct (tempStringLength, tempString);
   }

   // Set max yield button
   tempStringLength = sprintf_l10n(tempString, "%.1f", guiglobs::cdsData.rwConfig.templates.key_plt_amap_max_yield[product_num % MAX_AMAP_ROWS]);

   if (rowNum < MAX_AMAP_ROWS)
   {
      _graphicalRows[rowNum].btnMaxYield.btnBmpPtr->set_text_direct (tempStringLength, tempString);
   }
   else
   {
      _graphicalPasRows[rowNum % MAX_AMAP_ROWS].txtMaxYield->set_text_direct (tempStringLength, tempString);
   }

   // Set final conc
   tempStringLength = sprintf(tempString, "%.0f", finalConc);

   if (rowNum < MAX_AMAP_ROWS)
   {
      _graphicalRows[rowNum].btnFinalConc.btnBmpPtr->set_text_direct (tempStringLength, tempString);
   }
   else
   {
      _graphicalPasRows[rowNum % MAX_AMAP_ROWS].btnFinalConc.btnBmpPtr->set_text_direct (tempStringLength, tempString);
   }

   // Set percent carryover
   if (rowNum < MAX_AMAP_ROWS)
   {
      tempStringLength = sprintf(tempString, "--");
      _graphicalRows[rowNum].txtPasSep->set_text_direct(tempStringLength, tempString);
   }
   else
   {
      tempStringLength = sprintf(tempString, "%d", guiglobs::cdsData.rwConfig.templates.key_plt_amap_pas_pct_carryover[product_num % MAX_AMAP_ROWS]);
      _graphicalPasRows[rowNum % MAX_AMAP_ROWS].btnPasPlasmaPercent.btnBmpPtr->set_text_direct(tempStringLength, tempString);
   }
   // -------------  unchangable values: -----------------------------------------
   // Set min collect conc
   if (rowNum < MAX_AMAP_ROWS)
   {
      tempStringLength = sprintf(tempString, "--");
      _graphicalRows[rowNum].txtCollectConc->set_text_direct(tempStringLength, tempString);
   }
   else
   {
      tempStringLength = sprintf(tempString, "%.0f", minCollConc);
      _graphicalPasRows[rowNum % MAX_AMAP_ROWS].txtCollectConc->set_text_direct(tempStringLength, tempString);
   }
   // Set max collect conc
   if (rowNum < MAX_AMAP_ROWS)
   {
      tempStringLength = sprintf(tempString, "--");
      _graphicalRows[rowNum].txtCollectConc->set_text_direct(tempStringLength, tempString);
   }
   else
   {
      tempStringLength = sprintf(tempString, "%.0f", maxCollConc);
      _graphicalPasRows[rowNum % MAX_AMAP_ROWS].txtCollectConc->set_text_direct(tempStringLength, tempString);
   }
   // Set min pas volume
   if (rowNum < MAX_AMAP_ROWS)
   {
      tempStringLength = sprintf(tempString, "--");
      _graphicalRows[rowNum].txtMinPasVol->set_text_direct (tempStringLength, tempString);
   }
   else
   {
      tempStringLength = sprintf(tempString, "%.0f", ceil(minPasVol));
      _graphicalPasRows[rowNum % MAX_AMAP_ROWS].txtMinPasVol->set_text_direct (tempStringLength, tempString);
   }
   // Set max pas volume
   if (rowNum < MAX_AMAP_ROWS)
   {
      tempStringLength = sprintf(tempString, "--");
      _graphicalRows[rowNum].txtMaxPasVol->set_text_direct (tempStringLength, tempString);
   }
   else
   {
      tempStringLength = sprintf(tempString, "%.0f", ceil(maxPasVol));
      _graphicalPasRows[rowNum % MAX_AMAP_ROWS].txtMaxPasVol->set_text_direct (tempStringLength, tempString);
   }

   // Allocate the magnifying glass if necessary
   if (rowNum >= MAX_AMAP_ROWS)
   {
      DataLog (log_level_gui_debug) << "minCollConc: " << minCollConc << endmsg;

      if (minCollConc > CobeConfig::data().ConcMax)
         _graphicalPasRows[rowNum % MAX_AMAP_ROWS].bmpMagGlass->allocate_resources(*this);
      else
         _graphicalPasRows[rowNum % MAX_AMAP_ROWS].bmpMagGlass->deallocate_resources();
   }
}

// end of labelAllButtons
//


/////////////////////////////////////////////////////////////////////////////////
void Screen_PLT_AMAP::awaitingCallback (const BUTTON_TYPE type, const int rowNum)
{
   _callbackBtnType = type;
   _callbackProduct = rowNum + _topProduct;
}


////////////////////////////////////////////////////////////////////////////////
//
void Screen_PLT_AMAP::setRowIndicator (const int rowNum)
{
   if (rowNum >= 0)
   {  // filter out ysf
      if (_productIndicatorPtr == NULL)
      {
         _productIndicatorPtr = new Display_Text (_products[rowNum + _topProduct].productIndicatorString);
         OSRegion widget_region;

         widget_region.width  = 60;
         widget_region.height = 40;
         widget_region.x      = 10;
         widget_region.y      = 25;

         _productIndicatorPtr->set_requested_region(widget_region);
         _productIndicatorPtr->allocate_resources(*this);
      }
      else
      {
         _productIndicatorPtr->set_string_id(_products[rowNum + _topProduct].productIndicatorString);
      }
   }
} // end of setRowIndicator

//
///////////////////////////////////////////////////////////////////////////////
// display the alert message
void Screen_PLT_AMAP::displayAlert (float collect_conc, float final_conc)
{
   TEXT_STRING_ID alertMsg = TEXT_NULL;

   // use collect concentration for determination of PPC
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

float Screen_PLT_AMAP::getPercentCoMultiplier (const int rowNum)
{
   if (guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on
       && guiglobs::cdsData.rwConfig.templates.key_plt_mss[rowNum + _topProduct])

      return ((float)guiglobs::cdsData.rwConfig.templates.key_plt_amap_pas_pct_carryover[(rowNum + _topProduct) % MAX_AMAP_ROWS]) / 100.0f;
   else
      return 1.0f;
}



void Screen_PLT_AMAP::checkForTimeOnlyConfig ()
{
   if (guiglobs::timeOnlyConfigMode)
   {
      DataLog (log_level_gui_info) << "Buttons should be disabled." << endmsg;

      disableConfigBtn(_btnYsf);
      disableConfigBtn(_btnDonorsAllowed);

      for (int rowNum = 0; rowNum < MAX_AMAP_ROWS; rowNum++)
      {
         bool pasOn =
            guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on &&
            guiglobs::cdsData.rwConfig.templates.key_plt_mss[rowNum + _topProduct];

         //  if (_graphicalRows[rowNum].btnRowBoxAndPasToggle.btnBmpPtr)
         //     disableConfigBtn(*_graphicalRows[rowNum].btnRowBoxAndPasToggle.btnBmpPtr, ((pasOn) ? BITMAP_MSS_BUTTON_ICON_GRAY_DOWN : BITMAP_MSS_BUTTON_ICON_GRAY));

         if (_graphicalRows[rowNum].btnMinYield.btnBmpPtr)
            disableConfigBtn(*_graphicalRows[rowNum].btnMinYield.btnBmpPtr, BITMAP_CONFIG_SUB_NARROW_GRAY);

         // if (_graphicalRows[rowNum].btnPasPlasmaPercent.btnBmpPtr)
         //    disableConfigBtn(*_graphicalRows[rowNum].btnPasPlasmaPercent.btnBmpPtr, BITMAP_CONFIG_SUB_NARROW_GRAY);

         if (_graphicalRows[rowNum].btnFinalConc.btnBmpPtr)
            disableConfigBtn(*_graphicalRows[rowNum].btnFinalConc.btnBmpPtr, BITMAP_CONFIG_SUB_NARROW_GRAY);

         if (_graphicalRows[rowNum].txtCollectConc)
            disableConfigTxt(*_graphicalRows[rowNum].txtCollectConc);

         if (_graphicalRows[rowNum].txtMinPasVol)
            disableConfigTxt(*_graphicalRows[rowNum].txtMinPasVol);


         if (_graphicalRows[rowNum].txtPasSep)
            disableConfigTxt(*_graphicalRows[rowNum].txtPasSep);


         if (_graphicalRows[rowNum].txtMaxPasVol)
            disableConfigTxt(*_graphicalRows[rowNum].txtMaxPasVol);

      }

      disableConfigTxt(_txtMinYieldLbl);
      disableConfigTxt(_txtMaxYieldLbl);
      disableConfigTxt(_txtFinalConcLbl);
      disableConfigTxt(_txtCollectConcLbl);
      disableConfigTxt(_txtPASRangeLbl);
      disableConfigTxt(_txtPlasmaPercentLbl);
      disableConfigTxt(_toThe3rdCollectConc);
      disableConfigTxt(_toThe3rdFinalConc);
      disableConfigTxt(_toThe11thMinYield);
      disableConfigTxt(_toThe11thMaxYield);
      disableConfigTxt(_txtYsfLbl);
      disableConfigTxt(_txtDonorsAllowedLbl);
   }
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void cbPltYsfButton (void* data)
{
   ((Screen_PLT_AMAP*)data)->processYsfButton();
}

void Screen_PLT_AMAP::processYsfButton ()
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
   ((Screen_PLT_AMAP*)data)->processMaleOnlyPltBtn();
}

void Screen_PLT_AMAP::processMaleOnlyPltBtn ()
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

float Screen_PLT_AMAP::getMinAmapYield (const int row)
{

   return guiglobs::cdsData.rwConfig.templates.key_plt_amap_min_yield[row];

}


float Screen_PLT_AMAP::getMaxAmapYield (const int row)
{


   return guiglobs::cdsData.rwConfig.templates.key_plt_amap_max_yield[row] ;


}



//
///////////////////////////////////////////////////////////////////////////////
// end of cfg_platelet.cpp file
//

/* FORMAT HASH 4f3c3f18e1d4fe933b134b4768ee021b */

/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Screen_PREDICT

*     File name:   predict.cpp

*     Description: Class representaion of the PREDICT screen

*************************< FOCUSSED GUI DOMAIN >****************************
*/

#include <vxworks.h>

// header file include
#include "predict_screen.hpp"

#include "statline.hpp"


#include "guiglobs.hpp"
#include "software_cds.h"
#include "cobeconfig.h"
#include "predict_manager.hpp"
#include "run_defs.h"
#include "screen_config.hpp"
#include "statBarBase.hpp"




static const int MAX_OPTIMALS_TO_SHOW            = 4;

static const int PREDICT_PREV_ROW_Y              = 191;
static const int PREDICT_CURR_ROW_Y              = 263;
static const int PREDICT_NEXT_ROW_Y              = 325;

static const int PREDICT_ACTV_PLT_DELTA_X        = 30;
static const int PREDICT_ACTV_PLT_ICON_START_X   = 145;

static const int PREDICT_INACTV_PLT_DELTA_X      = 25;
static const int PREDICT_INACTV_PLT_ICON_START_X = 142;

static const int PREDICT_ACTV_RBC_DELTA_X        = 30;
static const int PREDICT_ACTV_RBC_ICON_START_X   = 425;

static const int PREDICT_INACTV_RBC_DELTA_X      = 25;
static const int PREDICT_INACTV_RBC_ICON_START_X = 425;

static const int CURR_AMAP_RBC_ICON_OFFSET       = 115;
static const int OTHER_AMAP_RBC_ICON_OFFSET      = 105;


int PREDICT_ACTV_PLT_ICON (int plt_icon_index) { return PREDICT_ACTV_PLT_ICON_START_X + (plt_icon_index - 1) * PREDICT_ACTV_PLT_DELTA_X; }
int PREDICT_ACTV_RBC_ICON (int rbc_icon_index) { return PREDICT_ACTV_RBC_ICON_START_X + (rbc_icon_index - 1) * PREDICT_ACTV_RBC_DELTA_X; }

int PREDICT_INACTV_PLT_ICON (int plt_icon_index) { return PREDICT_INACTV_PLT_ICON_START_X + (plt_icon_index - 1) * PREDICT_INACTV_PLT_DELTA_X; }
int PREDICT_INACTV_RBC_ICON (int rbc_icon_index) { return PREDICT_INACTV_RBC_ICON_START_X + (rbc_icon_index - 1) * PREDICT_INACTV_RBC_DELTA_X; }

int PREDICT_PLT_ICON (bool is_active, int plt_icon_index) { return (is_active) ? PREDICT_ACTV_PLT_ICON(plt_icon_index) : PREDICT_INACTV_PLT_ICON(plt_icon_index); }
int PREDICT_RBC_ICON (bool is_active, int rbc_icon_index) { return (is_active) ? PREDICT_ACTV_RBC_ICON(rbc_icon_index) : PREDICT_INACTV_RBC_ICON(rbc_icon_index); }

int PREDICT_ICON (bool is_rbc, bool is_active, int icon_index) { return (is_rbc) ? PREDICT_RBC_ICON(is_active, icon_index) : PREDICT_PLT_ICON(is_active, icon_index); }

//////////////////////////////////////////////////////////////////////
// Helper functions
//
const char* getAmapSizeString (const ProductOptionStruct& item)
{
   switch (item.plateletsAmapSize)
   {
      case AMAP_PLT_SINGLE     :
      case AMAP_PLT_PAS_SINGLE : return "Single" ;

      case AMAP_PLT_DOUBLE     :
      case AMAP_PLT_PAS_DOUBLE : return "Double" ;

      case AMAP_PLT_TRIPLE     :
      case AMAP_PLT_PAS_TRIPLE : return "Triple" ;
      case AMAP_PLT_NONE : return " ";
   }

   return "      ";
}


enum PREDICT_PLT_OR_RBC { ICON_PLT, ICON_RBC    };
enum PREDICT_ACTIVE_OR_INACTIVE { ICON_INACTIVE, ICON_ACTIVE };

int getXcoord (PREDICT_PLT_OR_RBC product, PREDICT_ACTIVE_OR_INACTIVE activity, int index)
{
   if (index < 0 || index > 3)
   {
      DataLog (log_level_gui_error) << "ERROR: bad icon index." << endmsg;
      _FATAL_ERROR (__FILE__, __LINE__, "Bad predict icon index");
      return 0;
   }

   return PREDICT_ICON(product, activity, index);
}



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_PREDICT::Screen_PREDICT()
   : Base_Apheresis_Window (TEAL, GUI_SCREEN_PREDICT),

     _amapState (SHOW_ONE),

     _buttons(),
     _keypad (),
     _units  (),

     _plt_prev_text (textEmptyFormatPredPrevPltInit, textEmptyFormatPredPrevPltX10Init, textEmptyFormatPredPrevPltExpInit),
     _plt_curr_text (textEmptyFormatPredCurrPltInit, textEmptyFormatPredCurrPltX10Init, textEmptyFormatPredCurrPltExpInit),
     _plt_next_text (textEmptyFormatPredNextPltInit, textEmptyFormatPredNextPltX10Init, textEmptyFormatPredNextPltExpInit),

     _pltVol_prev_text(TEXT_PREDICT_AMAPP_EMPTY_PREVPLTVOLM, TEXT_PREDICT_AMAPP_EMPTY_PREVPLTVOLM_UNITS),
     _pltVol_curr_text(textEmptyFormatPredCurrPltVolInit, textEmptyFormatPredCurrPltVolLabelInit),
     _pltVol_next_text(TEXT_PREDICT_AMAPP_EMPTY_NEXTPLTVOLM, TEXT_PREDICT_AMAPP_EMPTY_NEXTPLTVOLM_UNITS),

     _plas_prev_text (textEmptyFormatPredPrevPlasInit, textEmptyFormatPredPrevPlasLabelInit),
     _plas_curr_text (textEmptyFormatPredCurrPlasInit, textEmptyFormatPredCurrPlasLabelInit),
     _plas_next_text (textEmptyFormatPredNextPlasInit, textEmptyFormatPredNextPlasLabelInit),

     _rbc_prev_text (textEmptyFormatPredPrevRbcInit, textEmptyFormatPredPrevRbcLabelInit),
     _rbc_curr_text (textEmptyFormatPredCurrRbcInit, textEmptyFormatPredCurrRbcLabelInit),
     _rbc_next_text (textEmptyFormatPredNextRbcInit, textEmptyFormatPredNextRbcLabelInit),

     _time_prev_text (textEmptyFormatPredPrevTimeInit, textEmptyFormatPredPrevTimeLabelInit),
     _time_curr_text (textEmptyFormatPredCurrTimeInit, textEmptyFormatPredCurrTimeLabelInit),
     _time_next_text (textEmptyFormatPredNextTimeInit, textEmptyFormatPredNextTimeLabelInit),

     _flowrate_prev_text (textEmptyFormatPredPrevFlowRateInit, textEmptyFormatPredPrevFlowRateLabelInit),
     _flowrate_curr_text (textEmptyFormatPredCurrFlowRateInit, textEmptyFormatPredCurrFlowRateLabelInit),
     _flowrate_next_text (textEmptyFormatPredNextFlowRateInit, textEmptyFormatPredNextFlowRateLabelInit),

     _rbc_prev_4AMAP_text(TEXT_PREDICT_AMAPP_EMPTY_PREV_RBC, TEXT_PREDICT_AMAPP_EMPTY_PREV_RBC_LABEL),
     _rbc_curr_4AMAP_text(TEXT_PREDICT_AMAPP_EMPTY_CURR_RBC, TEXT_PREDICT_AMAPP_EMPTY_CURR_RBC_LABEL),
     _rbc_next_4AMAP_text(TEXT_PREDICT_AMAPP_EMPTY_NEXT_RBC, TEXT_PREDICT_AMAPP_EMPTY_NEXT_RBC_LABEL),

     _compPlsStatement(TEXT_PREDICT_COMP_PLS_NOTICE, TEXT_PREDICT_COMP_PLS_NOTICE_UNIT),

     _predictPause(PREDICTION_PAUSE),

     _bmpSelectionBox (22, BITMAP_PREDICT_NUMERIC_OPTIONS_Y,
                       BITMAP_PREDICT_NUMERIC_OPTIONS),

     _bmpDisabledPrev (BUTN_SELECT_PREVIOUS_X,
                       BUTN_SELECT_PREVIOUS_Y,
                       BITMAP_BUTTON_SELECT_PREVIOUS_DIS),

     _bmpDisabledNext (BUTN_SELECT_NEXT_X,
                       BUTN_SELECT_NEXT_Y,
                       BITMAP_BUTTON_SELECT_NEXT_DIS),

     _bmpPasPrev (PREDICT_ICON(false, false, 2),
                  PREDICT_PREV_ROW_Y,
                  BITMAP_PAS_ICON_INACTIVE),

     _bmpPasCurr (PREDICT_ICON(false, true, 2),
                  PREDICT_CURR_ROW_Y,
                  BITMAP_PAS_ICON_ACTIVE),

     _bmpPasNext (PREDICT_ICON(false, false, 2),
                  PREDICT_NEXT_ROW_Y,
                  BITMAP_PAS_ICON_INACTIVE),

     _bmpFluidPrev  (PREDICT_ICON(true, false, 1),
                     PREDICT_PREV_ROW_Y,
                     BITMAP_REPLACEMENT_FLUID_ICON_INACTIVE),

     _bmpFluidCurr  (PREDICT_ICON(true, true, 1),
                     PREDICT_CURR_ROW_Y,
                     BITMAP_REPLACEMENT_FLUID_ICON_ACTIVE),

     _bmpFluidNext  (PREDICT_ICON(true, false, 1),
                     PREDICT_NEXT_ROW_Y,
                     BITMAP_REPLACEMENT_FLUID_ICON_INACTIVE),

     _bmpFluidPrev_AMAP(PREDICT_ICON(true, false, 1) + OTHER_AMAP_RBC_ICON_OFFSET,
                        PREDICT_PREV_ROW_Y,
                        BITMAP_REPLACEMENT_FLUID_ICON_INACTIVE),

     _bmpFluidCurr_AMAP(PREDICT_ICON(true, true, 1) + CURR_AMAP_RBC_ICON_OFFSET,
                        PREDICT_CURR_ROW_Y,
                        BITMAP_REPLACEMENT_FLUID_ICON_ACTIVE),

     _bmpFluidNext_AMAP(PREDICT_ICON(true, false, 1) + OTHER_AMAP_RBC_ICON_OFFSET,
                        PREDICT_NEXT_ROW_Y,
                        BITMAP_REPLACEMENT_FLUID_ICON_INACTIVE),


     _bmpWbcCountPrev  (PREDICT_ICON(false, false, 3),
                        PREDICT_PREV_ROW_Y,
                        BITMAP_MAGNIFYING_GLASS_SMALL),

     _bmpWbcCountCurr  (PREDICT_ICON(false, true, 3),
                        PREDICT_CURR_ROW_Y,
                        BITMAP_MAGNIFYING_GLASS_LARGE),

     _bmpWbcCountNext  (PREDICT_ICON(false, false, 3),
                        PREDICT_NEXT_ROW_Y,
                        BITMAP_MAGNIFYING_GLASS_SMALL),

     _bmpHyperPrev (PREDICT_ICON(false, false, 1),
                    PREDICT_PREV_ROW_Y + 1,
                    BITMAP_HYPER_ICON_INACTIVE),

     _bmpHyperCurr (PREDICT_ICON(false, true, 1),
                    PREDICT_CURR_ROW_Y + 2,
                    BITMAP_HYPER_ICON_ACTIVE),

     _bmpHyperNext (PREDICT_ICON(false, false, 1),
                    PREDICT_NEXT_ROW_Y + 1,
                    BITMAP_HYPER_ICON_INACTIVE),

     _bmpRasPrev(PREDICT_ICON(true, false, 2),
                 PREDICT_PREV_ROW_Y,
                 BITMAP_RAS_ICON_INACTIVE),

     _bmpRasCurr(PREDICT_ICON(true, true, 2),
                 PREDICT_CURR_ROW_Y,
                 BITMAP_RAS_ICON_ACTIVE),

     _bmpRasNext(PREDICT_ICON(true, false, 2),
                 PREDICT_NEXT_ROW_Y,
                 BITMAP_RAS_ICON_INACTIVE),

     _bmpCompPls (320,
                  PREDICT_CURR_ROW_Y,
                  BITMAP_COMPL_PLS_ICON),

     _bmpCompPlsNext (320,
                      PREDICT_NEXT_ROW_Y,
                      BITMAP_COMPL_PLS_SMALL_ICON),

     _bmpCompPlsPrev (320,
                      PREDICT_PREV_ROW_Y,
                      BITMAP_COMPL_PLS_SMALL_ICON),

// Initialize various object pointers to NULL

     _txtLblPlt  (textMiscPredPltLabel),
     _txtLblRbc  (textMiscPredRbcLabel),
     _txtLblPlas (textMiscPredPlasLabel),
     _txtLblTime (textMiscPredTimeLabel),

     _txtLblAMAPPPlt  (TEXT_PREDICT_AMAPP_PRED_PLT_LABEL),
     _txtLblAMAPPPlas (TEXT_PREDICT_AMAPP_PRED_PLS_LABEL),
     _txtLblAMAPPRbc  (TEXT_PREDICT_AMAPP_PRED_RBC_LABEL),

     _txtDataAMAPPSizePrev (TEXT_PREDICT_NAME_AMAP_PREV_PLT_SIZE),
     _txtDataAMAPPSizeCurr (TEXT_PREDICT_NAME_AMAP_CURR_PLT_SIZE),
     _txtDataAMAPPSizeNext (TEXT_PREDICT_NAME_AMAP_NEXT_PLT_SIZE),

     _txtDataCompPlsVolm(TEST_PREDICT_COMP_PLS_VOLM),


/////////////  AC in Plasma ////////////////////////
     _txtDataACPlsVolmNext(textACinPlasmaVolumeValueNext),
     _txtACPlsUnitsNext(textACinPlasmaVolumeUnitNext),
     _txtDataACPlsVolmCurr(textACinPlasmaVolumeValueCurr),
     _txtACPlsUnitsCurr(textACinPlasmaVolumeUnitCurr),
     _txtDataACPlsVolmPrev(textACinPlasmaVolumeValuePrev),
     _txtACPlsUnitsPrev(textACinPlasmaVolumeUnitPrev),
     ////////////////////////////////////////////////

///////////////////
     _txtNoProduct   (textMiscPredNoProductAvailable),
     _txtBetterAmapPltAvailable (TEXT_PREDICT_AMAP_PLT_BETTER_AVAIL),

     _invocationType         (DONINFO_DELTA),
     _initial_repredict      (false),
     _goback                 (false),
     _allowAdjustFlowRate    (Software_CDS::GetInstance().getFeature(AllowAdjustFlowRatesOnProcedureSelect)),
     _amapPlatelets          (guiglobs::amapPlatelets()),
     _currentlyConfirming    (false),
     _nextPls_AllComp        (false),
     _currPls_AllComp        (false),
     _prevPls_AllComp        (false),
     _currentProcNumber      (-1),
     _selectedProcNumber     (-1),
     _donor_time             (0),
     _no_change              (false),
     _floatLastFlowAdj       (0.0),
     _floatCompPlsPASVolCurr (0.0f),
     _muteTimeout(MUTE_PERIOD, Callback<Screen_PREDICT>(this, &Screen_PREDICT::startBeepingSound), TimerMessage::DISARMED),


     _procData(guiglobs::proc_status.get_data())

{
   // reset all system states that define proper screen allocation
   Screen_PREDICT::reset_internal_screen_state ();

   _keypad.init_strings();

}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_PREDICT::~Screen_PREDICT()
{
   // deallocate_resources is virtual, so be sure to call the right one!
   Screen_PREDICT::deallocate_resources ();
}

void Screen_PREDICT::clearDisplayedProcedures (void)
{
   _flowrate_curr_text.clear();
   _flowrate_next_text.clear();
   _flowrate_prev_text.clear();

   _plas_curr_text.clear();
   _plas_next_text.clear();
   _plas_prev_text.clear();

   _pltVol_curr_text.clear();
   _pltVol_next_text.clear();
   _pltVol_prev_text.clear();

   _plt_curr_text.clear();
   _plt_next_text.clear();
   _plt_prev_text.clear();

   _rbc_curr_4AMAP_text.clear();
   _rbc_next_4AMAP_text.clear();
   _rbc_prev_4AMAP_text.clear();

   _rbc_curr_text.clear();
   _rbc_next_text.clear();
   _rbc_prev_text.clear();

   _time_next_text.clear();
   _time_prev_text.clear();

   _txtDataAMAPPSizeCurr.set_text_direct(BLANK_STRING);
   _txtDataAMAPPSizeNext.set_text_direct(BLANK_STRING);
   _txtDataAMAPPSizePrev.set_text_direct(BLANK_STRING);

   if (!_amapPlatelets)
   {
      _time_curr_text.clear();
   }

   /////////////  AC in Plasma ////////////////////////
   _txtDataACPlsVolmNext.set_text_direct(BLANK_STRING);
   _txtACPlsUnitsNext.set_text_direct(BLANK_STRING);
   _txtDataACPlsVolmCurr.set_text_direct(BLANK_STRING);
   _txtACPlsUnitsCurr.set_text_direct(BLANK_STRING);
   _txtDataACPlsVolmPrev.set_text_direct(BLANK_STRING);
   _txtACPlsUnitsPrev.set_text_direct(BLANK_STRING);
   ////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES

void Screen_PREDICT::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   _msgTimer.init(TIMER_SCREEN_ALARM_PERIOD, Callback<Screen_PREDICT>(this, &Screen_PREDICT::screenTimerHandler), TimerMessage::DISARMED);

   _currentlyConfirming = false;
   _goback              = false;

   // now deallocate "standard" buttons that are not needed based on system
   // state.

   // decode allocation parameter to determine invocation
   _invocationType = (PREDICT_INVOKE_TYPE)(atoi(allocation_parameter));

   // check if cancel button is needed
   if ( _invocationType == REPFLUID_NO_DELTA ||
        (_procData.system_state != BLOOD_PRIME && _procData.system_state != BLOOD_RUN) )
   {
      // cancel not needed in replace fluid screen or if not in run
      btn_cancel.deallocate_resources ();
   }

   // lowleft button not used on predict screen
   btn_lowleft.deallocate_resources ();

   // goback and continue not allocated until we get predict response
   btn_goback.deallocate_resources ();
   btn_continue.deallocate_resources ();

   // allocate all non-standard widgets

   // the placard bitmap objects
   _bmpSelectionBox.allocate_resources (*this);

   // the disabled button bitmaps
   _bmpDisabledPrev.allocate_resources (*this);
   _bmpDisabledNext.allocate_resources (*this);

   // the various selection buttons
   _buttons.allocate_prev(*this);
   _buttons.allocate_next(*this);


   if (_invocationType == AUTO_FLOW_TIMEOUT)
   {
      btn_goback.deallocate_resources ();
      _buttons.allocate_AF_Buttons(*this);

      if (guiglobs::apheresis_status_line)
         guiglobs::apheresis_status_line->deactivate_status_line_type(DEFAULT_STATUS_LINE);
//         guiglobs::apheresis_status_line->set_current_status_line_text_by_type (ALERT_STATUS_LINE, JPHtextStatPredictScreenAFTimeout);
      else DataLog (log_level_gui_error) << "AF Unallocated apheresis status line." << endmsg;


      //startBeepingSound();

   }
   else
   {
      _buttons.deallocate_AF_Buttons();
   }



   //////////////////AMAP PLT/////////////////////////////////////////////////
   // refresh cached feature
   _amapPlatelets = guiglobs::amapPlatelets();
   if (_amapPlatelets)
   {
      DataLog(log_level_gui_info) << "Alloc AMAPP resources.... " << endmsg;
      _buttons.allocate_MoreOrLess(*this);

      // time button not available until donor connection
      if (  (guiglobs::proc_status.get_data().system_state == BLOOD_RUN) ||
            (guiglobs::proc_status.get_data().system_state == BLOOD_PRIME)
            )
      {
         _buttons.allocate_time(*this);
      }

   }
   else
   {
      _buttons.allocate_time(*this);
      _buttons.deallocate_MoreOrLess();

   }
   //////////////////////////////////////////////////////////////////

   // Get the latest value of this feature
   _allowAdjustFlowRate = Software_CDS::GetInstance().getFeature(AllowAdjustFlowRatesOnProcedureSelect);

   if (_allowAdjustFlowRate && (guiglobs::screen_status.get_current_system_state() < DONOR_CONNECTED))
   {

      _time_prev_text.set_string_id(textEmptyFormatPredPrevTimeInitAlt, textEmptyFormatPredPrevTimeLabelInitAlt);
      _time_curr_text.set_string_id(textEmptyFormatPredCurrTimeInitAlt, textEmptyFormatPredCurrTimeLabelInitAlt);
      _time_next_text.set_string_id(textEmptyFormatPredNextTimeInitAlt, textEmptyFormatPredNextTimeLabelInitAlt);

      _txtLblTime.deallocate_resources();

      _buttons.allocate_flowrate(*this);

      _flowrate_prev_text.allocate_resources (*this);
      _flowrate_curr_text.allocate_resources (*this);
      _flowrate_next_text.allocate_resources (*this);

   }
   else
   {
      _time_prev_text.set_string_id(textEmptyFormatPredPrevTimeInit, textEmptyFormatPredPrevTimeLabelInit);
      _time_curr_text.set_string_id(textEmptyFormatPredCurrTimeInit, textEmptyFormatPredCurrTimeLabelInit);
      _time_next_text.set_string_id(textEmptyFormatPredNextTimeInit, textEmptyFormatPredNextTimeLabelInit);

      _buttons.deallocate_flowrate();

      _flowrate_prev_text.deallocate_resources();
      _flowrate_curr_text.deallocate_resources();
      _flowrate_next_text.deallocate_resources();

      if (!_amapPlatelets)
      {
         _txtLblTime.allocate_resources(*this);
      }
   }


   // Load, the text units factotums.
   _units.allocate_resources();

   // Initially clear procedure information
   clearDisplayedProcedures();

   // Top row of estimated product amounts -- this allocation just establishes coordinates and styling.

   _plt_prev_text.allocate_resources(*this);
   _plas_prev_text.allocate_resources(*this);
   _time_prev_text.allocate_resources(*this);

   _bmpFluidPrev.allocate_resources(*this);
   _bmpFluidPrev.set_visible(false); // until needed
   _bmpFluidCurr.allocate_resources(*this);
   _bmpFluidCurr.set_visible(false); // until needed
   _bmpFluidNext.allocate_resources(*this);
   _bmpFluidNext.set_visible(false); // until needed


   _bmpFluidPrev_AMAP.allocate_resources(*this);
   _bmpFluidPrev_AMAP.set_visible(false); // until needed
   _bmpFluidCurr_AMAP.allocate_resources(*this);
   _bmpFluidCurr_AMAP.set_visible(false); // until needed
   _bmpFluidNext_AMAP.allocate_resources(*this);
   _bmpFluidNext_AMAP.set_visible(false); // until needed

   _bmpWbcCountPrev.allocate_resources(*this);
   _bmpWbcCountPrev.set_visible(false); // until needed
   _bmpWbcCountCurr.allocate_resources(*this);
   _bmpWbcCountCurr.set_visible(false); // until needed
   _bmpWbcCountNext.allocate_resources(*this);
   _bmpWbcCountNext.set_visible(false); // until needed

   _bmpPasPrev.allocate_resources(*this);
   _bmpPasPrev.set_visible(false);  // until needed
   _bmpPasCurr.allocate_resources(*this);
   _bmpPasCurr.set_visible(false);  // until needed
   _bmpPasNext.allocate_resources(*this);
   _bmpPasNext.set_visible(false);  // until needed

   _bmpHyperPrev.allocate_resources(*this);
   _bmpHyperPrev.set_visible(false);
   _bmpHyperCurr.allocate_resources(*this);
   _bmpHyperCurr.set_visible(false);
   _bmpHyperNext.allocate_resources(*this);
   _bmpHyperNext.set_visible(false);

   _bmpRasPrev.allocate_resources(*this);
   _bmpRasPrev.set_visible(false);
   _bmpRasCurr.allocate_resources(*this);
   _bmpRasCurr.set_visible(false);
   _bmpRasNext.allocate_resources(*this);
   _bmpRasNext.set_visible(false);

   // end of top row

   // Middle row of estimated product amounts


   // Heading labels for the middle ("current") select row (all capitals)
   if (_amapPlatelets)
   {
      _txtLblAMAPPPlt.allocate_resources (*this);
      _txtLblAMAPPPlas.allocate_resources (*this);
      _txtLblAMAPPRbc.allocate_resources (*this);
   }
   else
   {

      _txtLblPlt.allocate_resources (*this);
      _txtLblPlas.allocate_resources (*this);
      _txtLblRbc.allocate_resources (*this);
   }

   // this allocation just establishes coordinates and styling. Text is set_directed as needed.
   _plt_curr_text.allocate_resources(*this);

   if (_amapPlatelets ||
       Software_CDS::GetInstance().getFeature(SelectScreenDisplayPlateletVolume))
   {
      _pltVol_curr_text.allocate_resources (*this);
   }


   if ( _amapPlatelets )
   {
      _pltVol_next_text.allocate_resources (*this);
      _pltVol_prev_text.allocate_resources (*this);

      _txtDataAMAPPSizePrev.allocate_resources (*this);
      _txtDataAMAPPSizeCurr.allocate_resources (*this);
      _txtDataAMAPPSizeNext.allocate_resources (*this);

      _rbc_prev_4AMAP_text.allocate_resources(*this);
      _rbc_curr_4AMAP_text.allocate_resources(*this);
      _rbc_next_4AMAP_text.allocate_resources(*this);

   }
   else
   {

      _rbc_prev_text.allocate_resources(*this);
      _rbc_curr_text.allocate_resources(*this);
      _rbc_next_text.allocate_resources(*this);

   }



   /////////////  AC in Plasma ////////////////////////
   if (Software_CDS::GetInstance().getFeature(ShowPlasmaVolume))
   {
      _txtDataACPlsVolmNext.allocate_resources (*this);
      _txtACPlsUnitsNext.allocate_resources (*this);
      _txtDataACPlsVolmCurr.allocate_resources (*this);
      _txtACPlsUnitsCurr.allocate_resources (*this);
      _txtDataACPlsVolmPrev.allocate_resources (*this);
      _txtACPlsUnitsPrev.allocate_resources (*this);
   }
   //////////////////////////////////////////////////

   _plas_curr_text.allocate_resources(*this);
   _time_curr_text.allocate_resources(*this);

   _txtDataCompPlsVolm.allocate_resources (*this);

   _bmpCompPls.allocate_resources(*this);
   _bmpCompPls.set_visible(false);

   _bmpCompPlsNext.allocate_resources(*this);
   _bmpCompPlsNext.set_visible(false);

   _bmpCompPlsPrev.allocate_resources(*this);
   _bmpCompPlsPrev.set_visible(false);


   // end of middle row

   // Bottom row of estimated product amounts  -- this allocation just establishes coordinates and styling.
   //                                              Text is set_directed as needed.

   _plt_next_text.allocate_resources(*this);
   _plas_next_text.allocate_resources(*this);
   _time_next_text.allocate_resources(*this);

   //////////////////////////////////////////////////////////////////////////
   // values for set flags initialized upon request helps to initialize donor
   // info screen during construction and struct reevaluation
   _donor_time       = 0;
   _floatLastFlowAdj = 0.0;

   // deactivate the screen until we receive predict message return
   freezeScreen ("allocate");

   enable_callbacks();


   // Set the Status Bar color and phase relative to state Link the status
   // bar/line to this window; note that the apheresis_status_bar pointer
   // *cannot* be null after this, so we don't need to check below.)
   link_status_bar_line();

   if ((_procData.system_state == BLOOD_PRIME) || (_procData.system_state == BLOOD_RUN))
      guiglobs::apheresis_status_bar->set_phase_active (STATUS_BAR_DONATE);
   else
      guiglobs::apheresis_status_bar->set_phase_active (STATUS_BAR_INFO);

   //////////////////AMAP PLT/////////////////////////////////////////////////
   if (_amapPlatelets)
   {
      DataLog(log_level_gui_info) << "Setting Predict screen to AMAPP SHOW_ONE " << endmsg;
      _amapState = SHOW_ONE;
   }
   /////////////////////////////////////////////////////////////////////////


   //////////////////////////////////////////////////////////////////////
   //
   // We pause the pumps when we enter the predict screen, so that the predictions aren't being made
   // while the conditions are changing (this can lead to a 'bait-and-switch' problem where
   // operators select procedures which are already invalid).  We invoke that logic as late as
   // possible in the allocation process.

   setPumpPause();


   // Finally, tell procedure_status that we're here, and start participating in events of interest
   guiglobs::proc_status.set_last_predict_screen_type(_invocationType);


   DataLog(log_level_gui_info) << "Screen_PREDICT coming up, invoked as " << _invocationType << endmsg;


   // Put up the current info, so the user has something to look at:
   updateScreenData();
   // but *don't* thaw the screen.  Wait until we've got updated data first.


   // Guarantee, period, end of story, no fiddling around, no joking, and we
   // really mean it, that our data is up to date.  When the answer comes back,
   // the appropriate callbacks will update and thaw the screen.
   //
   // ... except in the unusual case that there's a predict transaction in
   // flight (IT 13604), which shouldn't be possible, but can apparently happen
   // when the screen commutator gets feisty.  In this case, we note that we
   // need to repredict everything, and then wait until PredictManager isn't
   // busy.  This is probably inefficient (since the PredictManager is probably
   // handling a REPREDICT_ALL anyway) but safe.
   if (PredictManager::waiting())
   {
      DataLog(log_level_gui_info) << "Screen_PREDICT PredictManager is busy during allocation;"
                                  << " setting the initial repredict flag, freezing the screen"
                                  << " and waiting for the current transaction to complete"
                                  << endmsg;
      // Set the initial repredict flag -- this will generate the necessary
      // behavior in the predictResponseNotify callback
      _initial_repredict = true;
      freezeScreen("allocate_resources + waiting"); // don't let the user do anything until we've caught up
   }
   else
   {
      if (_amapPlatelets && _amapState == SHOW_ONE )
         PredictManager::sendPredictRequest(REPREDICT_ALL_OPTIMAL, HERE);
      else
         PredictManager::sendPredictRequest(REPREDICT_ALL, HERE);
   }

} // END of Screen_PREDICT ALLOCATE_RESOURCES



void Screen_PREDICT::refresh_resources ()
{

   DataLog(log_level_gui_info) << "Screen_PREDICT refresh_resources ... "
                               << (PredictManager::waiting() ? "waiting for Predict response" : "")
                               << (_no_change                ? "" : ", changes to show: " )
                               << (PredictManager::procedures_are_available() ? ", procedures are available" : "")
                               << endmsg;

   Base_Apheresis_Window::refresh_resources();

   enable_callbacks();          // if we've been stepped on (by, say, the Pause
                                // Alarm screen's own private Screen_TROUBLE
                                // object), reassert our callbacks.
   if (_no_change)
   {
      _no_change = false;
      return;
   }

#if CPU==SIMNT
   updateScreenData();
   thawScreen(__FUNCTION__);
#else
   if (_currentlyConfirming)
      _currentlyConfirming = false;
   else
   {
      // freeze screen until we get back updated procedure status
      clearDisplayedProcedures();
      freezeScreen ("refresh_resources ");

      if (  PredictManager::waiting() )
      {
         DataLog(log_level_gui_debug) << "refresh_resources, still waiting for new procedures from Predict"
                                      << endmsg;
      }
      else
      {
         DataLog(log_level_gui_debug) << "refresh_resources : not awaiting for Predict response so carrying on"
                                      << endmsg;

         // ... and we update the screen data (and therefore thaw) because we're not waiting for
         // a message from Predict to do so.
         if (PredictManager::procedures_are_available())
         {
            DataLog(log_level_gui_debug) << "refresh_resources : procedures are waiting, updating screen"
                                         << endmsg;
         }
         updateScreenData();
         thawScreen(__FUNCTION__);
      }

      guiglobs::proc_status.set_last_predict_screen_type(_invocationType);
   }
#endif // #if CPU==SIMNT

   ///////////  AMAP plt //////////////////////////////
   if (_amapPlatelets)
   {
      DataLog(log_level_gui_info) << "Setting amapp status to Optimal on refresh_resources " << endmsg;
      _buttons.set_MoreOrLess_plus();
      _amapState = SHOW_ONE;

   }
}


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_PREDICT::deallocate_resources ()

{
   disable_callbacks();

   destroyConfirmCallbacks();

   _msgPtrKeypadData.deregisterMsg();

   _msgTimer.armTimer(TimerMessage::DISARMED);

   // If we need to restart the pumps, do so now.  (We wait until after we detach from
   // procedure_status just to make sure that we're really done.)
   clearPumpPause();


   _plt_prev_text.deallocate_resources();
   _pltVol_prev_text.deallocate_resources();
   _plas_prev_text.deallocate_resources();
   _rbc_prev_text.deallocate_resources();
   _time_prev_text.deallocate_resources();
   _flowrate_prev_text.deallocate_resources();

   _plt_curr_text.deallocate_resources();
   _pltVol_curr_text.deallocate_resources();
   _plas_curr_text.deallocate_resources();
   _rbc_curr_text.deallocate_resources();
   _time_curr_text.deallocate_resources();
   _flowrate_curr_text.deallocate_resources();

   _plt_next_text.deallocate_resources();
   _pltVol_next_text.deallocate_resources();
   _plas_next_text.deallocate_resources();
   _rbc_next_text.deallocate_resources();
   _time_next_text.deallocate_resources();
   _flowrate_next_text.deallocate_resources();

   _rbc_prev_4AMAP_text.deallocate_resources();
   _rbc_curr_4AMAP_text.deallocate_resources();
   _rbc_next_4AMAP_text.deallocate_resources();

   _txtDataCompPlsVolm.deallocate_resources();

   _txtDataAMAPPSizePrev.deallocate_resources();
   _txtDataAMAPPSizeCurr.deallocate_resources();
   _txtDataAMAPPSizeNext.deallocate_resources();


   /////////////  AC in Plasma ////////////////////////
   if (Software_CDS::GetInstance().getFeature(ShowPlasmaVolume))
   {
      _txtDataACPlsVolmNext.deallocate_resources();
      _txtACPlsUnitsNext.deallocate_resources();
      _txtDataACPlsVolmCurr.deallocate_resources();
      _txtACPlsUnitsCurr.deallocate_resources();
      _txtDataACPlsVolmPrev.deallocate_resources();
      _txtACPlsUnitsPrev.deallocate_resources();
   }
   //////////////////////////////////////////////////


   _txtLblPlt.deallocate_resources();
   _txtLblPlas.deallocate_resources();
   _txtLblRbc.deallocate_resources();
   _txtLblTime.deallocate_resources();
   _txtLblAMAPPPlt.deallocate_resources ();
   _txtLblAMAPPPlas.deallocate_resources ();
   _txtLblAMAPPRbc.deallocate_resources ();

   _bmpCompPls.deallocate_resources();

   _bmpCompPlsNext.deallocate_resources();
   _bmpCompPlsPrev.deallocate_resources();

   _compPlsStatement.deallocate_resources();

   // the placard bitmap objects
   _bmpSelectionBox.deallocate_resources ();

   _bmpFluidPrev.deallocate_resources ();
   _bmpFluidCurr.deallocate_resources ();
   _bmpFluidNext.deallocate_resources ();

   _bmpFluidPrev_AMAP.deallocate_resources ();
   _bmpFluidCurr_AMAP.deallocate_resources ();
   _bmpFluidNext_AMAP.deallocate_resources ();

   _bmpWbcCountPrev.deallocate_resources ();
   _bmpWbcCountCurr.deallocate_resources ();
   _bmpWbcCountNext.deallocate_resources ();

   _bmpPasPrev.deallocate_resources();
   _bmpPasCurr.deallocate_resources();
   _bmpPasNext.deallocate_resources();

   // hyper concentrate indicators
   _bmpHyperPrev.deallocate_resources();
   _bmpHyperCurr.deallocate_resources();
   _bmpHyperNext.deallocate_resources();

   _bmpRasPrev.deallocate_resources();
   _bmpRasCurr.deallocate_resources();
   _bmpRasNext.deallocate_resources();

   // the predict bitmap buttons
   _buttons.deallocate_all();

   // the disabled bitmap buttons
   _bmpDisabledPrev.deallocate_resources ();
   _bmpDisabledNext.deallocate_resources ();

   // the no product message (just in case it's allocated)
   _txtNoProduct.deallocate_resources ();

   // AMAP plt /////////////////////
   _txtBetterAmapPltAvailable.deallocate_resources ();


   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();

}  // END of Screen_PREDICT DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// RESET_INTERNAL_SCREEN_STATE

// This method will reset the internal screen state
void Screen_PREDICT::reset_internal_screen_state ()
{
   // reset all local varibles from text to reals
   _procedure_prev.clear();
   _procedure_curr.clear();
   _procedure_next.clear();
}


////////////////////////////////////////////////////////////////////////////////
// FREEZE_SCREEN

// This method will disable all sceen components
void Screen_PREDICT::freezeScreen (const char* from_)
{
   DataLog(log_level_gui_debug) << "Freeze "  << from_ << endmsg;

   // Disable all the standard buttons on this screen
   disable_standard_buttons();
   _buttons.freeze(_amapPlatelets);
}


////////////////////////////////////////////////////////////////////////////////
// THAW_SCREEN

// This method will enable all sceen components
void Screen_PREDICT::thawScreen (const char* from_)
{

   DataLog(log_level_gui_debug) << "Thaw "  << from_ << endmsg;

   // Enable all the standard buttons on this screen
   enable_standard_buttons();

   _buttons.thaw(_amapPlatelets, _amapState);


   btn_continue.enable_callbacks();

   if (_allowAdjustFlowRate && (guiglobs::screen_status.get_current_system_state() < DONOR_CONNECTED))
      _buttons.enable_flowrate();

}


////////////////////////////////////////////////////////////////////////////////
// SCREEN_EXIT

void Screen_PREDICT::screenExit (GUI_SCREEN_ID gui_screen_transition,
                                 const char* allocation_param_transition)
{

   disable_callbacks();

   // Send an indication to "goto screen" which is the transition screen or if
   // invoked as a stateless screen remove myself and display any other
   // active stateless screen.
   if (gui_screen_transition == GUI_SCREEN_ABORTCNF)
   {
      if (get_screen_invocation_type () == GUI_SCREEN_INVOKE_STATIC)
      {
         // user desires back to goto the ABORTCNF screen as child
         goto_child_screen(gui_screen_transition, allocation_param_transition);
      }
      else if (get_screen_invocation_type () == GUI_SCREEN_INVOKE_STATELESS)
      {
         // user desires back to goto the ABORTCNF screen as stateless
         goto_stateless_screen(gui_screen_transition, allocation_param_transition);
      }
   }
   else if ((gui_screen_transition == GUI_SCREEN_RUNPROC) &&
            (get_screen_invocation_type () == GUI_SCREEN_INVOKE_STATELESS))
   {
      // user desires to remove stateless screen
      RemoveMostRecentStatelessScreenMsg req(MessageBase::SEND_LOCAL);
      req.send(0);
      guiglobs::adj_button_press = false;
   }
   else if ((gui_screen_transition == GUI_SCREEN_TROUBLE) &&
            (get_screen_invocation_type () == GUI_SCREEN_INVOKE_STATELESS))
   {
      // user desires back to goto the RUNPROC screen no repredict necessary
      // or goto the ADJUST screen for repredict but as stateless.
      swap_stateless_screen(gui_screen_transition, allocation_param_transition);
   }
   else if (gui_screen_transition == GUI_SCREEN_KEYPAD)
   {
      _msgPtrKeypadData.init(Callback<Screen_PREDICT>(this, &Screen_PREDICT::keypadStructMgr));

      if (get_screen_invocation_type () == GUI_SCREEN_INVOKE_STATELESS)
      {
         // user desires back to goto the KEYPAD child screen
         // figure out a way to make this a child of a stateless screen using
         // CHILD_OF_STATELESS
         goto_child_screen(gui_screen_transition, allocation_param_transition);
      }
      else
      {
         goto_child_screen(gui_screen_transition, allocation_param_transition);
      }
   }
   else
   {
      // user desires back to goto the DONINFO screen on goback
      goto_screen(gui_screen_transition, allocation_param_transition);
      guiglobs::adj_button_press = false;
   }

}  // End of screenExit


////////////////////////////////////////////////////////////////////////////////
// TIMEOUT_RESET_MANAGER
void Screen_PREDICT::timeoutResetMgr ()
{
   // If the Predict invocation is from Replace Fluid prompt screen, no timer is used,
   // therefore just return as if timer maintenance had been completed.

   if (_invocationType == REPFLUID_NO_DELTA)
   {
      // just return
      return;
   }

   if (_procData.system_state == BLOOD_PRIME ||
       _procData.system_state == BLOOD_RUN)
      _msgTimer.armTimer(TimerMessage::ARMED);
}  // End of TIMEOUT_RESET_MANAGER



void Screen_PREDICT::enable_callbacks ()
{

   guiglobs::proc_status.attach(this, 1250);
   guiglobs::proc_status.set_system_state_callback(Callback<Screen_PREDICT>(this, &Screen_PREDICT::systemStateNotify));
   guiglobs::proc_status.enable_access_pressure_low_warning(true);

   // ... and attach to PredictManager:
   PredictManager::attach(this);
   // We listen to completed requests, but also directly to GuiProceduresUpdated
   // messages.  PredictManager is smart enough not to call us twice for the
   // same message event.
   PredictManager::set_procedure_info_callback(Callback<Screen_PREDICT>(this,
                                                                        &Screen_PREDICT::predictResponseNotify));
   PredictManager::set_request_complete_callback(Callback<Screen_PREDICT>(this,
                                                                          &Screen_PREDICT::predictResponseNotify));

}



void Screen_PREDICT::disable_callbacks ()
{
   DataLog(log_level_gui_info) << "Screen_PREDICT::disable_callbacks()" << endmsg;
   guiglobs::proc_status.detach(this);
   PredictManager::detach(this);
   // Unlikely, but possible that the screen goes down while waiting to complete
   // the initial repredict operation.  Reset that, so when we come up again we
   // start from scratch.
   _initial_repredict = false;
}



void Screen_PREDICT::setPumpPause ()
{
   // We pause the pumps (generally) to prevent the procedures the operator is
   // looking at from changing out from other them.  We don't need to do this
   // when nothing relevent can change, e.g. before donor connect.  We also
   // don't pause them if they were already paused (which can onlyh happen if a
   // previous visit to this screen found that there were no valid procedures,
   // in which case the user had to go to some other screen to fix the
   // situation).

   // We don't need to pause the pumps if nothing relevent to predict is changing:
   if ( (guiglobs::proc_status.get_data().system_state != BLOOD_RUN) &&
        (guiglobs::proc_status.get_data().system_state != BLOOD_PRIME) )
   {
      DataLog(log_level_gui_debug) << "PredictPause: not in blood run or blood prime, "
                                   << "not pausing the pumps." << endmsg;
      return;
   }

   // If the pumps weren't already paused, pause them now:
   if (_predictPause.getState() == CLEARED)
   {
      DataLog(log_level_gui_info) << "PredictPause: set the pump pause alarm, pause the pumps."
                                  << endmsg;
      _predictPause.setAlarm();
   }
   else
   {
      // If they were already paused it's because we didn't clear it
      // the last time we left the screen; see clearPumpPause() below.
      DataLog(log_level_gui_info) << "PredictPause: pump pause alarm was previously set."
                                  << endmsg;
   }
}


void Screen_PREDICT::clearPumpPause ()
{

   if (PredictManager::no_product() && _goback)
   {
      // IT 13681, 13748: if no available products, and the user hits the
      // 'continue' button, they are requesting either an end-run or a
      // rinseback; and if they select a rinseback, we need to restart the
      // pumps.  On the other hand, if they hit the 'goback' button and there
      // are no products, don't restart the pumps; the user came to this screen
      // after making some adjustments that rendered all procedures invalid.
      // They need to use the goback button to reconsider their adjustments,
      // then come back here and select a procedure.  We keep the pumps paused
      // while this goes on.

      DataLog(log_level_gui_info) << "PredictPause: no valid procedures, and 'going back', "
                                  << "so pumps will stay paused after exit." << endmsg;

      return;
   }

   if (_predictPause.getState()  == CLEARED)
   {
      // the pumps may not have been paused when this screen was entered, E.G. before donor connect.
      DataLog(log_level_gui_debug) << "PredictPause: pumps were not paused, exiting the screen."
                                   << endmsg;
   }
   else
   {
      DataLog(log_level_gui_info) << "PredictPause: pumps were paused, restarting before exiting predict screen"
                                  << endmsg;

      _predictPause.clearAlarm();
   }
}


////////////////////////////////////////////////////////////////////////////////
//  PREDICT_RESPONSE_NOTIFY
//
// This is the callback that gets triggered when PredictManager has something
// for us to chew on.  That is, either a predict transaction has completed, or
// something has changed gratuitously and we need to update.
void Screen_PREDICT::predictResponseNotify ()
{
   // If PredictManager was busy when we came up, we need to fire a
   // REPREDICT_ALL to ensure that our data is up to date.  IT 13604
   if (_initial_repredict)
   {
      DataLog(log_level_gui_info) << "Screen_PREDICT::predictResponseNotify, PredictManager is now free "
                                  << "so we can fire our initial repredict request."
                                  << endmsg;

      _initial_repredict = false;
      if (_amapPlatelets && _amapState == SHOW_ONE )
         PredictManager::sendPredictRequest(REPREDICT_ALL_OPTIMAL, HERE);
      else
         PredictManager::sendPredictRequest(REPREDICT_ALL, HERE);

      // ... and we take no further action until we come back here (which is
      // when Predict is done).
      return;
   }

   updateScreenData();
   if (PredictManager::waiting())
   {
      // then we wait until that's done...
      DataLog(log_level_gui_debug) << "Screen_PREDICT update, but waiting on PredictManager before thawing."
                                   << endmsg;
      return;
   }
   thawScreen(__FUNCTION__);
}


////////////////////////////////////////////////////////////////////////////////
// UPDATE SCREEN DATA
void Screen_PREDICT::updateScreenData ()
{
   DataLog(log_level_gui_debug) << "Screen_PREDICT UPDATE" << endmsg;

   // Clear PredictManager flags: new procedures are being displayed.
   // (Note: IT 13240, we don't clear the prediction_screen_requested flag here,
   // because although the screen is up, that flag _really_ means that the
   // current procedure has changed, and we want operator validation.  That
   // happens when we select a procedure.
   PredictManager::clear_procedures_are_available();


   const ProcedureInformationStruct predictData =  guiglobs::cdsData.procedures.ProcedureInfo.Get();

   const bool isAmapp = _amapPlatelets;

   // Log the data
   //   BETTER PUT THIS BACK LATER --JPH
   //  DataLog(log_level_gui_info) << predictData << endmsg;


   // Reset the "no product available" message
   _txtNoProduct.deallocate_resources();


   ///////////////////////////////////////////////////////////////////////////////////
   //

   _currentProcNumber = predictData[CURR].procedure_index;
   DataLog (log_level_gui_info) << "Current Proc Number = " << _currentProcNumber << endmsg;

   // Adjust these values if this is a PTF procedure.
   float       adjustedEstRbcsPrev = predictData[PREV].estimated_rbcs;
   float       adjustedEstRbcsCurr = predictData[CURR].estimated_rbcs;
   float       adjustedEstRbcsNext = predictData[NEXT].estimated_rbcs;

   const float oneFilterLoss       = CobeConfig::data().mssPtfFilterLoss;
   const float twoFilterLoss       = 2.0f * CobeConfig::data().mssPtfFilterLoss;

   // Previous procedure: If this procedure is RAS and actually collects RBCs...
   if (predictData[PREV].metered_rbc && predictData[PREV].estimated_rbc_dose > 0)
   {
      // If the dose exceeds the threshold, and we've either loaded a DRBC cassette or haven't loaded a set at all,
      // assume procedure is a DRBC and subtract two filter loss.
      if (predictData[PREV].ptf_split)
      {
         DataLog (log_level_gui_info) << "Previous procedure is DRBC RAS.  Subtracting two filter loss ("
                                      << twoFilterLoss << ") from rbcsPredicted(" << adjustedEstRbcsPrev
                                      << ") to get " << adjustedEstRbcsPrev - twoFilterLoss << endmsg;

         adjustedEstRbcsPrev = adjustedEstRbcsPrev - twoFilterLoss;
      }
      // Otherwise, just assume single filter loss
      else
      {
         DataLog (log_level_gui_info) << "Previous procedure is SRBC RAS.  Subtracting one filter loss ("
                                      << oneFilterLoss << ") from rbcsPredicted(" << adjustedEstRbcsPrev
                                      << ") to get " << adjustedEstRbcsPrev - oneFilterLoss << endmsg;

         adjustedEstRbcsPrev = adjustedEstRbcsPrev - oneFilterLoss;
      }

      // Make sure we didn't go negative.
      if (adjustedEstRbcsPrev < 0) adjustedEstRbcsPrev = 0.0;
   }

   // Current procedure: If this procedure is RAS and actually collects RBCs...
   if (predictData[CURR].metered_rbc && predictData[CURR].estimated_rbc_dose > 0)
   {
      // If the dose exceeds the threshold, and we've either loaded a DRBC cassette or haven't loaded a set at all,
      // assume procedure is a DRBC and subtract two filter loss.
      if (predictData[CURR].ptf_split)
      {
         DataLog (log_level_gui_info) << "Current procedure is DRBC RAS.  Subtracting two filter loss ("
                                      << (twoFilterLoss) << ") from rbcsPredicted(" << adjustedEstRbcsCurr
                                      << ") to get " << adjustedEstRbcsCurr - twoFilterLoss << endmsg;

         adjustedEstRbcsCurr = adjustedEstRbcsCurr - twoFilterLoss;
      }
      // Otherwise, just assume single filter loss
      else
      {
         DataLog (log_level_gui_info) << "Current procedure is SRBC RAS.  Subtracting one filter loss ("
                                      << oneFilterLoss << ") from rbcsPredicted(" << adjustedEstRbcsCurr
                                      << ") to get " << adjustedEstRbcsCurr - oneFilterLoss << endmsg;

         adjustedEstRbcsCurr = adjustedEstRbcsCurr - oneFilterLoss;
      }

      // Make sure we didn't go negative.
      if (adjustedEstRbcsCurr < 0)
         adjustedEstRbcsCurr = 0.0;
   }

   // Next procedure: If this procedure is RAS and actually collects RBCs...
   if (predictData[NEXT].metered_rbc && predictData[NEXT].estimated_rbc_dose > 0)
   {
      // If the dose exceeds the threshold, and we've either loaded a DRBC cassette or haven't loaded a set at all,
      // assume procedure is a DRBC and subtract two filter loss.
      if (predictData[NEXT].ptf_split)
      {
         DataLog (log_level_gui_info) << "Next procedure is DRBC RAS.  Subtracting two filter loss ("
                                      << twoFilterLoss << ") from rbcsPredicted(" << adjustedEstRbcsNext
                                      << ") to get " << adjustedEstRbcsNext - twoFilterLoss << endmsg;

         adjustedEstRbcsNext = adjustedEstRbcsNext - twoFilterLoss;
      }
      // Otherwise, just assume single filter loss
      else
      {
         DataLog (log_level_gui_info) << "Next procedure is SRBC RAS.  Subtracting one filter loss ("
                                      << oneFilterLoss << ") from rbcsPredicted(" << adjustedEstRbcsNext
                                      << ") to get " << adjustedEstRbcsNext - oneFilterLoss << endmsg;

         adjustedEstRbcsNext = adjustedEstRbcsNext - oneFilterLoss;
      }

      // Make sure we didn't go negative.
      if (adjustedEstRbcsNext < 0)
         adjustedEstRbcsNext = 0.0;
   }
   ///////////////////////////////////////////////////////////////////////////////////

   // obtain the previous procedure status values and read into standing variables

   _procedure_prev     = predictData[PREV]; // doesn't set rbc, do that manually:
   _procedure_prev.rbc = adjustedEstRbcsPrev;

   _prevPls_AllComp    = ( predictData[PREV].comp_pls_entire &&
                           (  predictData[PREV].comp_pls_volume > 0.0f));

   // obtain the current procedure status values and read into standing variables

   _procedure_curr         = predictData[CURR]; // set rbc manually:
   _procedure_curr.rbc     = adjustedEstRbcsCurr;

   _floatCompPlsPASVolCurr = predictData[CURR].comp_pls_PAS_volume;
   _currPls_AllComp        =  (predictData[CURR].comp_pls_entire &&
                               ( _procedure_curr.comp_plasma > 0.0f));


   // obtain the next procedure status values and read into standing variables
   _procedure_next     = predictData[NEXT];
   _procedure_next.rbc =  adjustedEstRbcsNext;

   _nextPls_AllComp    =  (predictData[NEXT].comp_pls_entire &&
                           ( _procedure_next.comp_plasma > 0.0f));


   ////////////////////////////// Populate the "previous" row ///////////////////////////

   static char logstring[200];
   int         iconCount = 0;
   bool        showIcon  = false;
   OSRegion    region;
   ////////////////////////////  PREVIOUS ////////////////////////////////////////////////
   ////////////////////////////  PREVIOUS ////////////////////////////////////////////////
   ////////////////////////////  PREVIOUS ////////////////////////////////////////////////
   //

   if (isAmapp && (_amapState == SHOW_ONE))
   {
      _txtDataCompPlsVolm.set_text_direct(BLANK_STRING);

      _pltVol_prev_text.clear();

      _txtDataAMAPPSizePrev.set_text_direct(BLANK_STRING);

      _plt_prev_text.clear();

      _plas_prev_text.clear();

      _rbc_prev_4AMAP_text.clear();

      showIcon = false;
      _bmpHyperPrev.set_visible(showIcon);
      _bmpPasPrev.set_visible(showIcon);
      _bmpWbcCountPrev.set_visible(showIcon);
      _bmpFluidPrev.set_visible(showIcon);
      _bmpFluidPrev_AMAP.set_visible(showIcon);
      _bmpRasPrev.set_visible(showIcon);

   }
   else
   {
      sprintf(logstring, "prev: %.1f %s %.1f %s %.1f %s %s %.1f repl=%d count=%d pas=%d ras=%d",
              predictData[PREV].estimated_platelets,
              (const char*)_procedure_prev.platelets,
              predictData[PREV].estimated_plasma,
              (const char*)_procedure_prev.plasma,
              predictData[PREV].estimated_rbcs,
              (const char*)_procedure_prev.rbc,
              (const char*)_procedure_prev.time,
              predictData[PREV].estimated_qin_max,
              predictData[PREV].replacement_fluid_required,
              predictData[PREV].count_icon_needed,
              predictData[PREV].metered_platelet,
              predictData[PREV].metered_rbc);
      DataLog(log_level_gui_info) << logstring << endmsg;

      if (predictData[PREV].estimated_platelets > 0 || (predictData[PREV].validProcedure && (_procData.collected_platelets > 0)))
      {
         _plt_prev_text.set(_procedure_prev.platelets);

         if (_amapPlatelets )
         {
            _pltVol_prev_text.set (_procedure_prev.plate_vol);

            const char* size = getAmapSizeString(predictData[PREV]);
            _txtDataAMAPPSizePrev.set_text_direct(size);
         }
      }
      else
      {
         if (_amapPlatelets )
         {
            _pltVol_prev_text.clear();
            _txtDataAMAPPSizePrev.set_text_direct(BLANK_STRING);

         }
         _plt_prev_text.clear();
      }

      if (predictData[PREV].estimated_plasma > 0.0 )
      {
         /////////////  AC in Plasma ////////////////////////
         // _txtDataACPlsVolmPrev.set_text_direct( _procedure_prev.ac_plasma);
         // _txtACPlsUnitsPrev.set_text_direct ( _gstrPlasUnits);
         //////////////////////////////////////////////////

         _plas_prev_text.set(_procedure_prev.plasma);

      }
      else
      {
         /////////////  AC in Plasma ////////////////////////
         _txtDataACPlsVolmPrev.set_text_direct(BLANK_STRING);
         //////////////////////////////////////////////////

         _plas_prev_text.clear();

      }

      if (predictData[PREV].estimated_rbcs > 0.0  )
      {
         if (_amapPlatelets )
         {
            _rbc_prev_4AMAP_text.set(_procedure_prev.rbc);
         }
         else
         {
            _rbc_prev_text.set(_procedure_prev.rbc);
         }
      }
      else
      {
         if (_amapPlatelets )
         {
            _rbc_prev_4AMAP_text.clear();
         }
         else
         {
            _rbc_prev_text.clear();
         }
      }

      if (predictData[PREV].estimated_platelet_volume > 0)
      {
         showIcon = Software_CDS::isPPC((predictData[PREV].estimated_platelets / predictData[PREV].estimated_platelet_volume) * 1.0e5) ;

         _bmpHyperPrev.set_visible(showIcon);

         if (showIcon) iconCount++;

         showIcon = predictData[PREV].metered_platelet;

         _bmpPasPrev.set_visible(showIcon);

         if (showIcon  )
         {
            iconCount++;
            region   = _bmpPasPrev.get_dimensions();
            region.x = getXcoord(ICON_PLT, ICON_INACTIVE, iconCount);
            _bmpPasPrev.set_position(region.x, region.y);
         }

         showIcon = predictData[PREV].count_icon_needed;

         _bmpWbcCountPrev.set_visible(showIcon);

         if (showIcon )
         {
            iconCount++;
            region   = _bmpWbcCountPrev.get_dimensions();
            region.x = getXcoord(ICON_PLT, ICON_INACTIVE, iconCount);
            _bmpWbcCountPrev.set_position(region.x, region.y);
         }
      }
      else
      {
         _bmpHyperPrev.set_visible(false);
         _bmpPasPrev.set_visible(false);
         _bmpWbcCountPrev.set_visible(false);
      }

      iconCount = 0;
      showIcon  = predictData[PREV].replacement_fluid_required;

      if (isAmapp)
         _bmpFluidPrev_AMAP.set_visible(showIcon);
      else
         _bmpFluidPrev.set_visible(showIcon);

      if (showIcon) iconCount++;

      if (predictData[PREV].estimated_rbc_dose)
         showIcon = predictData[PREV].metered_rbc;
      else
         showIcon = false;

      _bmpRasPrev.set_visible(showIcon);

      if (showIcon )
      {
         iconCount++;
         region   = _bmpRasPrev.get_dimensions();
         region.x = getXcoord(ICON_RBC, ICON_INACTIVE, iconCount);
         if (isAmapp) region.x = region.x + OTHER_AMAP_RBC_ICON_OFFSET;
         _bmpRasPrev.set_position(region.x, region.y);
      }


      // If at least one of the previous products exists, display the time and enable the "previous" button.
      if ((predictData[PREV].estimated_platelets > 0.0) ||
          (predictData[PREV].estimated_plasma > 0.0) ||
          (predictData[PREV].estimated_rbcs > 0.0))
      {
         // since a product exists, set the time
         if (!_amapPlatelets)
         {
            _time_prev_text.set(_procedure_prev.time);
         }

         _flowrate_prev_text.set(_procedure_prev.flowrate);

         _buttons.allocate_prev(*this);
      }
      else
      {
         // since a product doesn't exist, blank the time
         _time_prev_text.clear();
         _flowrate_prev_text.clear();
         _buttons.deallocate_prev();
      }
   }


   ////////////////////////////// Populate the "current" row ///////////////////////////

   ////////////////////////////  CURRENT ////////////////////////////////////////////////
   ////////////////////////////  CURRENT ////////////////////////////////////////////////
   ////////////////////////////  CURRENT ////////////////////////////////////////////////
   sprintf(logstring, "curr: %.1f %s %.1f %s %.1f %s %.1f %s %s %.1f repl=%d count=%d pas=%d ras=%d ac=%.1f t=%.1f vbp=%.1f postcount=%d",
           predictData[CURR].estimated_platelets, (const char*)_procedure_curr.platelets,
           predictData[CURR].estimated_platelet_volume, (const char*)_procedure_curr.plate_vol,
           predictData[CURR].estimated_plasma, (const char*)_procedure_curr.plasma,
           predictData[CURR].estimated_rbcs, (const char*)_procedure_curr.rbc, (const char*)_procedure_curr.time,
           predictData[CURR].estimated_qin_max,
           predictData[CURR].replacement_fluid_required,
           predictData[CURR].count_icon_needed,
           predictData[CURR].metered_platelet,
           predictData[CURR].metered_rbc,
           predictData[CURR].estimated_vac,
           predictData[CURR].procedure_time,
           predictData[CURR].estimated_tbv_processed,
           predictData[CURR].estimated_post_platelet_count);
   DataLog(log_level_gui_info) << logstring << endmsg;

   if (guiglobs::complementaryPlasmaEnabled())
   {
      DataLog(log_level_gui_info) <<  "compPls: current complementary pls vol = " << (float)_procedure_curr.comp_plasma << endmsg;
      DataLog(log_level_gui_info) <<  "compPls: current entire product complementary = " << _currPls_AllComp  << endmsg;
   }

   if (predictData[CURR].estimated_platelets > 0 || (_procData.collected_platelets > 0))
   {
      if (_amapPlatelets)
      {
         const char* size = getAmapSizeString(predictData[CURR]);
         _txtDataAMAPPSizeCurr.set_text_direct(size);
      }

      _plt_curr_text.set(_procedure_curr.platelets);

      _pltVol_curr_text.set(_procedure_curr.plate_vol);   // , _gstrPltVolUnits);
   }
   else
   {
      if (_amapPlatelets)
      {
         _txtDataAMAPPSizeCurr.set_text_direct(BLANK_STRING);
      }

      _plt_curr_text.clear();
      _pltVol_curr_text.clear();
   }

   if (predictData[CURR].estimated_plasma > 0.0)
   {
      /////////////  AC in Plasma ////////////////////////
      _txtDataACPlsVolmCurr.set_text_direct(_procedure_curr.ac_plasma);
      _txtACPlsUnitsCurr.set_text_direct (predict_units::_gstrPlasUnits);
      //////////////////////////////////////////////////
      _plas_curr_text.set(_procedure_curr.plasma); // , _gstrPlasUnits );
   }
   else
   {
      /////////////  AC in Plasma ////////////////////////
      _txtDataACPlsVolmCurr.set_text_direct(BLANK_STRING);
      _txtACPlsUnitsCurr.set_text_direct(BLANK_STRING);

      //////////////////////////////////////////////////

      _plas_curr_text.clear();
   }


   if (predictData[CURR].estimated_rbcs > 0.0)
   {
      if (_amapPlatelets )
      {
         _rbc_curr_4AMAP_text.set(_procedure_curr.rbc);
      }
      else
      {
         _rbc_curr_text.set(_procedure_curr.rbc);
      }
   }
   else
   {
      if (_amapPlatelets )
      {
         _rbc_curr_4AMAP_text.clear();
      }
      else
      {
         _rbc_curr_text.clear();
      }
   }

   iconCount = 0;
   if (predictData[CURR].estimated_platelet_volume > 0)
   {
      showIcon = Software_CDS::isPPC((predictData[CURR].estimated_platelets / predictData[CURR].estimated_platelet_volume) * 1.0e5);
      _bmpHyperCurr.set_visible(showIcon);
      if (showIcon) iconCount++;

      showIcon = predictData[CURR].metered_platelet;
      _bmpPasCurr.set_visible(showIcon);
      if (showIcon)
      {
         iconCount++;
         region   = _bmpPasCurr.get_dimensions();
         region.x = getXcoord(ICON_PLT, ICON_ACTIVE, iconCount);
         _bmpPasCurr.set_position(region.x, region.y);
      }

      showIcon = predictData[CURR].count_icon_needed;
      _bmpWbcCountCurr.set_visible(showIcon);
      if (showIcon)
      {
         iconCount++;
         region   = _bmpWbcCountCurr.get_dimensions();
         region.x = getXcoord(ICON_PLT, ICON_ACTIVE, iconCount);
         _bmpWbcCountCurr.set_position(region.x, region.y);
      }
   }
   else
   {
      _bmpHyperCurr.set_visible(false);
      _bmpPasCurr.set_visible(false);
      _bmpWbcCountCurr.set_visible(false);
   }
   iconCount = 0;
   showIcon  = predictData[CURR].replacement_fluid_required;

   if (isAmapp)
   {
      _bmpFluidCurr_AMAP.set_visible(showIcon);
   }
   else
   {
      _bmpFluidCurr.set_visible(showIcon);
   }
   if (showIcon) iconCount++;

   if (predictData[CURR].estimated_rbc_dose > 0)
      showIcon = predictData[CURR].metered_rbc;
   else
      showIcon = false;

   _bmpRasCurr.set_visible(showIcon);
   if (showIcon)
   {
      iconCount++;
      region   = _bmpRasCurr.get_dimensions();
      region.x = getXcoord(ICON_RBC, ICON_ACTIVE, iconCount);
      if (isAmapp) region.x = region.x + CURR_AMAP_RBC_ICON_OFFSET;
      _bmpRasCurr.set_position(region.x, region.y);
   }

   // If at least one of the current products exists, display the time and enable the "current" button.
   if ((predictData[CURR].estimated_platelets > 0.0) ||
       (predictData[CURR].estimated_plasma > 0.0) ||
       (predictData[CURR].estimated_rbcs > 0.0))
   {
      // since a product exists, set the time
      //
      if (!_amapPlatelets)
      {
         _time_curr_text.set(_procedure_curr.time);

      }
      _flowrate_curr_text.set(_procedure_curr.flowrate);

      // Indicate that at least one product is available in the "current" row.
      PredictManager::no_product() = false;

   } // end of: "if ((predictData[CURR].
   else
   {
      // since a product doesn't exist, blank the time
      _time_curr_text.clear();
      _flowrate_curr_text.clear();

      PredictManager::no_product() = true;

   }  // end of: ELSE for "if ((predictData[CURR].

   ////////////////////////////// Populate the "next" row ///////////////////////////

   ////////////////////////////  NEXT ////////////////////////////////////////////////
   ////////////////////////////  NEXT ////////////////////////////////////////////////
   ////////////////////////////  NEXT ////////////////////////////////////////////////
   sprintf(logstring, "next: %.1f %s %.1f %s %.1f %s %s %.1f repl=%d count=%d pas=%d ras=%d",
           predictData[NEXT].estimated_platelets, (const char*)_procedure_next.platelets,
           predictData[NEXT].estimated_plasma, (const char*)_procedure_next.plasma,
           predictData[NEXT].estimated_rbcs, (const char*)_procedure_next.rbc,
           (const char*)_procedure_next.time,
           predictData[NEXT].estimated_qin_max,
           predictData[NEXT].replacement_fluid_required,
           predictData[NEXT].count_icon_needed,
           predictData[NEXT].metered_platelet,
           predictData[NEXT].metered_rbc);

   DataLog(log_level_gui_info) << logstring << endmsg;

   if (isAmapp && (_amapState == SHOW_ONE))
   {
      _pltVol_next_text.clear();

      _txtDataAMAPPSizeNext.set_text_direct(BLANK_STRING);

      _plt_next_text.clear();

      _plas_next_text.clear();

      _rbc_next_4AMAP_text.clear();

      showIcon = false;
      _bmpHyperNext.set_visible(showIcon);
      _bmpPasNext.set_visible(showIcon);
      _bmpWbcCountNext.set_visible(showIcon);
      _bmpFluidNext.set_visible(showIcon);
      _bmpRasNext.set_visible(showIcon);
      _bmpFluidNext_AMAP.set_visible(showIcon);

   }
   else
   {
      if (predictData[NEXT].estimated_platelets > 0 || (predictData[NEXT].validProcedure && (_procData.collected_platelets > 0)))
      {
         _plt_next_text.set(_procedure_next.platelets);

         if (_amapPlatelets)
         {
            _pltVol_next_text.set (_procedure_next.plate_vol);

            const char* size = getAmapSizeString(predictData[NEXT]);
            _txtDataAMAPPSizeNext.set_text_direct(size);
         }
      }
      else
      {
         if (_amapPlatelets )
         {
            _pltVol_next_text.clear();

            _txtDataAMAPPSizeNext.set_text_direct(BLANK_STRING);
         }

         _plt_next_text.clear();
      }

      if (predictData[NEXT].estimated_plasma > 0.0   )
      {
         /////////////  AC in Plasma ////////////////////////
         // _txtDataACPlsVolmNext.set_text_direct( _procedure_next.ac_plasma);
         // _txtACPlsUnitsNext.set_text_direct ( _gstrPlasUnits);
         //////////////////////////////////////////////////

         _plas_next_text.set(_procedure_next.plasma);

      }
      else
      {
         /////////////  AC in Plasma ////////////////////////
         _txtDataACPlsVolmNext.set_text_direct(BLANK_STRING);
         //////////////////////////////////////////////////

         _plas_next_text.clear();
      }

      if (predictData[NEXT].estimated_rbcs > 0.0 )
      {
         if (_amapPlatelets )
         {
            _rbc_next_4AMAP_text.set(_procedure_next.rbc);
         }
         else
         {
            _rbc_next_text.set(_procedure_next.rbc);
         }
      }
      else
      {
         if (_amapPlatelets )
         {
            _rbc_next_4AMAP_text.clear();
         }
         else
         {
            _rbc_next_text.clear();
         }
      }

      iconCount = 0;
      if (predictData[NEXT].estimated_platelet_volume > 0)
      {
         showIcon = Software_CDS::isPPC((predictData[NEXT].estimated_platelets / predictData[NEXT].estimated_platelet_volume) * 1.0e5);
         _bmpHyperNext.set_visible(showIcon);
         if (showIcon) iconCount++;

         showIcon = predictData[NEXT].metered_platelet;
         _bmpPasNext.set_visible(showIcon);
         if (showIcon)
         {
            iconCount++;
            region   = _bmpPasNext.get_dimensions();
            region.x = getXcoord(ICON_PLT, ICON_INACTIVE, iconCount);
            _bmpPasNext.set_position(region.x, region.y);
         }

         showIcon = predictData[NEXT].count_icon_needed;
         _bmpWbcCountNext.set_visible(showIcon);
         if (showIcon)
         {
            iconCount++;
            region   = _bmpWbcCountNext.get_dimensions();
            region.x = getXcoord(ICON_PLT, ICON_INACTIVE, iconCount);
            _bmpWbcCountNext.set_position(region.x, region.y);
         }
      }
      else
      {
         _bmpHyperNext.set_visible(false);
         _bmpPasNext.set_visible(false);
         _bmpWbcCountNext.set_visible(false);
      }
      iconCount = 0;
      showIcon  = predictData[NEXT].replacement_fluid_required;


      if (isAmapp)
         _bmpFluidNext_AMAP.set_visible(showIcon);
      else
         _bmpFluidNext.set_visible(showIcon);


      if (showIcon) iconCount++;

      if (predictData[NEXT].estimated_rbc_dose > 0)
         showIcon = predictData[NEXT].metered_rbc;
      else
         showIcon = false;

      _bmpRasNext.set_visible(showIcon);
      if (showIcon)
      {
         iconCount++;
         region   = _bmpRasNext.get_dimensions();
         region.x = getXcoord(ICON_RBC, ICON_INACTIVE, iconCount);
         if (isAmapp) region.x = region.x + OTHER_AMAP_RBC_ICON_OFFSET;
         _bmpRasNext.set_position(region.x, region.y);
      }

      // If at least one of the next products exists, display the time and enable the "next" button.

      if ((predictData[NEXT].estimated_platelets > 0.0) ||
          (predictData[NEXT].estimated_plasma > 0.0) ||
          (predictData[NEXT].estimated_rbcs > 0.0))
      {
         if ( !_amapPlatelets )
         {
            // since a product exists, set the time
            _time_next_text.set(_procedure_next.time);
         }

         _flowrate_next_text.set(_procedure_next.flowrate);

         // provide the next selection button
         _buttons.allocate_next(*this);

      }   // end of: "if ((predictData[NEXT].
      else
      {
         _bmpFluidNext.set_visible(false);
         _bmpFluidNext_AMAP.set_visible(false);
         _bmpHyperNext.set_visible(false);
         _bmpRasNext.set_visible(false);
         _bmpPasNext.set_visible(false);
         _bmpWbcCountNext.set_visible(false);

         // since a product doesn't exist, blank the time
         _time_next_text.clear();
         _flowrate_next_text.clear();
         // deallocate the next button
         _buttons.deallocate_next();
      }    // end of: ELSE for "if ((predictData[NEXT].
   }


   ////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////



   ////////////////////////////// End of ROW Populations ///////////////////////////


   // Shall we put up the goback button?
   // Not if:
   //   A) we are in prime or blood run and the CPS alarm has disabled RBC
   //      collection, since we get here without anywhere to 'goback' to; IT 8160
   //
   //   B) We are NOT in a run and we were invoked as REPFLUID_NO_DELTA. We're
   //      here because we chose to cancel connection of the replacement fluid,
   //      so we need to pick another procedure. Going back makes no sense.
   //      IT 8020

   bool in_run          = ( _procData.system_state == BLOOD_PRIME || _procData.system_state == BLOOD_RUN );

   // in run, disable go back button if invocation type is either CPS_DISABLES_RBCS or AUTO_FLOW_TIMEOUT
   bool disable_go_back = in_run
                          ? (( _invocationType == CPS_DISABLES_RBCS ) || ( _invocationType == AUTO_FLOW_TIMEOUT ))
                          : ( _invocationType == REPFLUID_NO_DELTA ); // not in run,disable if REPFLUID


   if (disable_go_back)
   {
      btn_goback.deallocate_resources();
   }
   else
   {
      btn_goback.allocate_resources(*this);
   }


   if (isAmapp && (_amapState == SHOW_ONE))
   {
      if (predictData[PREV].estimated_platelets > 0.0f)
      {
         _txtBetterAmapPltAvailable.allocate_resources(*this);
      }
      else
      {
         _txtBetterAmapPltAvailable.deallocate_resources();
      }

      _buttons.deallocate_prev();
      _buttons.deallocate_next();
      _buttons.disable_prev();
      _buttons.disable_next();

   }

   ////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////
   //  COMP PLS PROCESSING
   if (guiglobs::complementaryPlasmaEnabled())
   {
      // advice text for comp plasma CURRENT line only:
      if ( _procedure_curr.comp_plasma /*_floatCompPlsVolCurr*/ > 0.0f)
      {
         _txtDataCompPlsVolm.set_text_direct(_procedure_curr.comp_plasma);
         _compPlsStatement.allocate_resources(*this);

      }
      else
      {

         _txtDataCompPlsVolm.set_text_direct(BLANK_STRING);

         _compPlsStatement.deallocate_resources();
      }

      ///////////////////////////////////////////////////////////////////////////////
      /////////////////////     ICON COMP PLS        ////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////

      /// current ICON  ////////////////////
      if ( _currPls_AllComp && (_procedure_curr.comp_plasma > 0.0f)  )
      {
         _bmpCompPls.set_visible(true);
         // guiglobs::showCompPlsSymbol( true );
      }
      else
      {
         // guiglobs::showCompPlsSymbol( false );
         _bmpCompPls.set_visible(false);
      }

      /// Next ICON  ////////////////////////////
      if ( _nextPls_AllComp && ( _procedure_next.comp_plasma > 0.0f)  )
      {

         if ( !isAmapp )
         {
            _bmpCompPlsNext.set_visible(true);
         }
         else
         {

            if ( _amapState == SHOW_ONE )
               _bmpCompPlsNext.set_visible(false);
            else
               _bmpCompPlsNext.set_visible(true);
         }
      }
      else
      {
         _bmpCompPlsNext.set_visible(false);
      }

      /// Prev ICON  ////////////////////////////////////
      if ( _prevPls_AllComp && ( _procedure_prev.comp_plasma > 0.0f)  )
      {
         if ( !isAmapp )
         {
            _bmpCompPlsPrev.set_visible(true);
         }
         else
         {
            if ( _amapState == SHOW_ONE )
               _bmpCompPlsPrev.set_visible(false);
            else
               _bmpCompPlsPrev.set_visible(true);

         }
      }
      else
      {
         _bmpCompPlsPrev.set_visible(false);
      }
      ///////////////////////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////


   }
   ////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////

   if (isAmapp && (_amapState != SHOW_ONE))
   {
      _txtBetterAmapPltAvailable.deallocate_resources();
   }


   if (PredictManager::no_product() == true)
   {
      // if pre-run remove the continue button, force them to reenter data
      if ((_procData.system_state == STARTUP_TESTS) || (_procData.system_state == AC_PRIME))
      {
         btn_continue.deallocate_resources ();
      }

      // .. and make sure we come back here soon.
      PredictManager::set_prediction_screen_requested();

      // set the no product text
      _txtNoProduct.allocate_resources(*this);
      _time_curr_text.clear();

      // remove scroll buttons
      _buttons.deallocate_prev();
      _buttons.deallocate_next();

      // remove all text and graphics for...
      // "previous" row

      _plt_prev_text.clear();
      _plas_prev_text.clear();
      _rbc_prev_text.clear();
      _rbc_prev_4AMAP_text.clear();

      _bmpFluidPrev.set_visible(false) ;
      _bmpWbcCountPrev.set_visible(false);

      _bmpPasPrev.set_visible(false);
      _bmpHyperPrev.set_visible(false);
      _bmpRasPrev.set_visible(false);

      // "current" row

      _plt_curr_text.clear();
      _pltVol_curr_text.clear();
      _plas_curr_text.clear();
      _rbc_curr_text.clear();
      _rbc_curr_4AMAP_text.clear();

      _bmpFluidCurr.set_visible(false) ;
      _bmpWbcCountCurr.set_visible(false);
      _bmpPasCurr.set_visible(false);
      _bmpHyperCurr.set_visible(false);
      _bmpRasCurr.set_visible(false);

      // "next" row
      _plt_next_text.clear();
      _plas_next_text.clear();
      _rbc_next_text.clear();
      _rbc_next_4AMAP_text.clear();

      _bmpFluidNext.set_visible(false) ;
      _bmpFluidNext_AMAP.set_visible(false) ;
      _bmpWbcCountNext.set_visible(false);
      _bmpPasNext.set_visible(false);
      _bmpHyperNext.set_visible(false);
      _bmpRasNext.set_visible(false);


      // remove all times
      _time_prev_text.clear();
      _time_curr_text.clear();
      _time_next_text.clear();


      // remove all flow rates
      _flowrate_prev_text.clear();
      _flowrate_curr_text.clear();
      _flowrate_next_text.clear();


      if ( _amapPlatelets )
         _buttons.deallocate_MoreOrLess();
   }
   else
   {
      if ( _amapPlatelets )
      {
         _buttons.allocate_MoreOrLess(*this);
      }

      if (!btn_continue.allocated())
      {
         btn_continue.allocate_resources (*this);
         btn_continue.set_callback (aph_callback_continue, (void*)this);
         btn_continue.set_string_id (textButtonLongConfirmProduct);
      }

      _txtNoProduct.deallocate_resources();
   }


   // Create the screen timeout timer that will return the user to
   // the appropriate screen
   // The timer must be reset here to allow the optimizer to complete its calculations.
   if ((_invocationType == ADJUST_DELTA) || (_invocationType == DONINFO_DELTA) ||
       (_invocationType == ADJUST_NO_DELTA) || (_invocationType == DONINFO_NO_DELTA))
   {
      timeoutResetMgr ();
   }

   // if in run set the continue button
   if ((_procData.system_state == BLOOD_PRIME) || (_procData.system_state == BLOOD_RUN))
   {
      if (!btn_continue.allocated())
      {
         btn_continue.allocate_resources (*this);
         btn_continue.set_callback (aph_callback_continue, (void*)this);
         btn_continue.set_string_id (textButtonLongConfirmProduct);
      }
   }
}  // End of updateScreenData


////////////////////////////////////////////////////////////////////////////////
// GET_KEYPAD_STRUCT_MANAGER

void Screen_PREDICT::keypadStructMgr ()
{
   const KEYPAD_RETURN_STRUCT keypad_buffer = _msgPtrKeypadData.getData();
   _msgPtrKeypadData.deregisterMsg();

   if ((keypad_buffer.keypad_return_value) == -1)
   {
      _no_change = true;
      return;
   }
   // provide response to proc based on keyad type and return value
   if (keypad_buffer.keypad_type == DONOR_TIME)
   {
      // set the structures time member with the return value from keypad
      // and set.
      if (((keypad_buffer.keypad_return_value) >= MINIMUM_PROCEDURE_TIME) &&
          ((keypad_buffer.keypad_return_value) <= ABSOLUTE_MAXIMUM_PROCEDURE_TIME) &&
          ((keypad_buffer.keypad_return_value) != _donor_time))
      {
         // deactivate the screen until we recieve predict message return
         freezeScreen ("keypadStructMgr");

         PredictManager::sendDonorTimeUpdate(keypad_buffer.keypad_return_value);

         // set refresh gating value
         _donor_time = keypad_buffer.keypad_return_value;

      }
   }
   else if (_allowAdjustFlowRate)
   {
      if ((keypad_buffer.keypad_return_value) != _floatLastFlowAdj)
      {
         freezeScreen ("keypadStructMgr");

         DataLog (log_level_gui_info) << "Keypad entry for flow adjustment: " << keypad_buffer.keypad_return_value << endmsg;

         PredictManager::sendFlowAdjustment(keypad_buffer.keypad_return_value);

         _floatLastFlowAdj = keypad_buffer.keypad_return_value;
      }
   }
   else DataLog (log_level_gui_error) << "Ignoring keypad data (could be coming from a layered screen)" << endmsg;
}  // End of GET_KEYPAD_STRUCT_MANAGER


////////////////////////////////////////////////////////////////////////////////
// SCREEN_TIMER_HANDLER

void Screen_PREDICT::screenTimerHandler ()
{
   DataLog(log_level_gui_info) << "Screen_PREDICT timeout." << endmsg;

   // deactivate the screen until we recieve predict message return
   freezeScreen ("screenTimerHandler");

   // send the alarm
   // Create alarm message
   anAlarmMsg procedure_changed(PROCEDURE_MODIFIED);

   // Send alarm message indicating procedure modified alarm
   procedure_changed.setAlarm();

   if (_amapPlatelets && _amapState == SHOW_ONE )
      PredictManager::sendPredictRequest(REPREDICT_ALL_OPTIMAL, HERE);
   else
      PredictManager::sendPredictRequest(REPREDICT_ALL, HERE);

}  // END of Screen_PREDICT SCREEN_TIMER_HANDLER


////////////////////////////////////////////////////////////////////////////////
// PROCESS_GOBACK_BUTTON
void Screen_PREDICT::process_goback_button ()
{
   // reset the screen timeout
   timeoutResetMgr ();

   // deactivate the screen until we recieve predict message return
   freezeScreen ("process_goback_button");

   _goback = true;

   // if in prime or run return to invoking screen
   if ((_procData.system_state == BLOOD_PRIME) || (_procData.system_state == BLOOD_RUN))
   {
      // set invocation allocation parameter
      guistring predictCallType;
      sprintf(predictCallType, "%d", (int)_invocationType);

      switch (_invocationType)
      {
         case ADJUST_DELTA    :
         case ADJUST_NO_DELTA : screenExit (GUI_SCREEN_TROUBLE, predictCallType); break;
         default              : screenExit (GUI_SCREEN_DONINFO, predictCallType); break;
      }

      return;
   }

   GUI_SCREEN_ID invoking_screen =  guiglobs::screen_status.get_previous_screen();

   if (invoking_screen == GUI_SCREEN_DONINFO)
   {
      screenExit (GUI_SCREEN_DONINFO, "GUI_SCREEN_DONINFO");
      return;
   }

   if (invoking_screen == GUI_SCREEN_CONFIRM_DON_INFO)
   {
      // // Unfreeze predict
      // DataLog(log_level_gui_info) << "GUI Sending DONOR_INFO_PRECONNECT " << endmsg;
      // Message<DoPrediction_t> predictMsg(MessageBase::SEND_LOCAL);
      // predictMsg.send(DONOR_INFO_PRECONNECT);

      // go back to the donor download screen
      screenExit (GUI_SCREEN_CONFIRM_DON_INFO, "GUI_SCREEN_CONFIRM_DON_INFO");
   }

}  // END of Screen_PREDICT PROCESS_GOBACK_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON
void Screen_PREDICT::process_cancel_button ()
{
   // reset the screen timeout
   timeoutResetMgr ();

   if (guiglobs::proc_status.rinsebackIsAnOption())
   {
      screenExit (GUI_SCREEN_ABORTCNF, ABORT_TYPE_RINSEBACK);
   }
   else
   {
      // Request to move to the ABORTCNF screen
      screenExit (GUI_SCREEN_ABORTCNF, ABORT_TYPE_USER);
   }
}


////////////////////////////////////////////////////////////////////////////////
// PROCESS_PREVIOUS_OPTIONS
void Screen_PREDICT::processPrevBtn ()
{
   // reset the screen timeout
   timeoutResetMgr ();

   // deactivate the screen until we recieve predict message return
   freezeScreen ("processPrevBtn");

   PredictManager::sendPredictRequest(SELECT_PREVIOUS, HERE);

}



////////////////////////////////////////////////////////////////////////////////
void Screen_PREDICT::processAFAdjustBtn ()
{

   guiglobs::button_audio_feedback.create_feedback_sound (ALERT_SOUND, SOUND_CLEAR);

   if (guiglobs::apheresis_status_line)
      guiglobs::apheresis_status_line->deactivate_status_line_type(ALARM_STATUS_LINE);
      //guiglobs::apheresis_status_line->reset_status_line ();
   else DataLog (log_level_gui_error) << "AF Unallocated apheresis status line." << endmsg;

   if(_invocationType == AUTO_FLOW_TIMEOUT)
   {
      guiglobs::adj_button_press = true;
      PredictManager::clear_prediction_screen_requested();
   }

   // user desires to remove stateless screen
   RemoveMostRecentStatelessScreenMsg req(MessageBase::SEND_LOCAL);
   req.send(0);

   // user desires to change procedure, goto the TROUBLE screen
   goto_stateless_screen(GUI_SCREEN_TROUBLE, "GUI_SCREEN_TROUBLE");
}




////////////////////////////////////////////////////////////////////////////////
// PROCESS_NEXT_OPTIONS

void Screen_PREDICT::processNextBtn ()
{
   // reset the screen timeout
   timeoutResetMgr ();

   // deactivate the screen until we recieve predict message return
   freezeScreen ("processNextBtn");

   PredictManager::sendPredictRequest(SELECT_NEXT, HERE);

}


void Screen_PREDICT::processMoreOrLess ()
{
   // reset the screen timeout
   timeoutResetMgr ();

   freezeScreen ("processMoreOrLess");

   // We can assume that this is the AMAPP version because otherwise there would be no more or less button
   switch (_amapState)
   {
      case SHOW_ONE :

         DataLog(log_level_gui_info) << "SHOW_ONE->SHOW_ALL " << endmsg;
         _amapState = SHOW_ALL;
         _buttons.set_MoreOrLess_minus();
         PredictManager::sendPredictRequest(REPREDICT_ALL, HERE);

         break;


      case SHOW_ALL :

         DataLog(log_level_gui_info) << "SHOW_ALL->SHOW_ONE" <<  endmsg;
         _amapState = SHOW_ONE;
         _buttons.set_MoreOrLess_plus();
         PredictManager::sendPredictRequest(REPREDICT_ALL_OPTIMAL, HERE);
         break;
   }
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_DONOR_TIME_BUTTON
//

void Screen_PREDICT::processTimeBtn ()
{
   // reset the screen timeout
   timeoutResetMgr ();

   // Display the time keypad
   // If this is the stateless version of this screen, we need to take
   // special measures to ensure that the keypad appears on top.
   if (get_screen_invocation_type () == GUI_SCREEN_INVOKE_STATELESS)
      screenExit(GUI_SCREEN_KEYPAD, _keypad.time_keypad_stateless()); // _gstrTimeKeypadAllocStateless);
   else
      screenExit(GUI_SCREEN_KEYPAD, _keypad.time_keypad_static()); // _gstrTimeKeypadAllocStatic);
}


void Screen_PREDICT::processAdjustFlowRateBtn ()
{
   // reset the screen timeout
   timeoutResetMgr ();

   // goto the keypad with the appropriate units
   screenExit(GUI_SCREEN_KEYPAD, _keypad.flowrate()); // _gstrCalcStringFlowRate);
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Screen_PREDICT::process_continue_button ()
{
   if (_invocationType == AUTO_FLOW_TIMEOUT)
   {
      guiglobs::button_audio_feedback.create_feedback_sound (ALERT_SOUND, SOUND_CLEAR);

      if (guiglobs::apheresis_status_line)
         guiglobs::apheresis_status_line->deactivate_status_line_type(ALARM_STATUS_LINE);
         //guiglobs::apheresis_status_line->reset_status_line ();
      else DataLog (log_level_gui_error) << "AF Unallocated apheresis status line." << endmsg;

   }


   if (guiglobs::complementaryPlasmaEnabled())
   {

      float CurrentPlsVolume = guiglobs::cdsData.volumes.PlasmaVolume.Get();
      bool  plsRinsebackON   = guiglobs::cdsData.config.Procedure.Get().key_plasma_rinseback && guiglobs::cassette_mgr->currentSetIsWhiteStamp();

      DataLog (log_level_gui_info) << "compPls: continue button : current pls vol= " << CurrentPlsVolume
                                   << ",  Selected CompPls= " << (float)_procedure_curr.comp_plasma   // _floatCompPlsVolCurr
                                   << ",  Selected CompPls PAS vol= " <<  _floatCompPlsPASVolCurr
                                   << ",  Selected PLT volm= " <<  (float)_procedure_curr.plate_vol   // _floatPltVol
                                   << ",  Selected is Entire CompPls= " <<  _currPls_AllComp
                                   << ", is pls rinseback = " << plsRinsebackON
                                   << endmsg;


      float RinsebackOffset = 0.0f;
      if (plsRinsebackON)
      {
         RinsebackOffset = CobeConfig::data().RinsebackVplrb;
      }

#if CPU==SIMNT
      // unable to write to proc volumes, simulate as if plasma has been collected
      CurrentPlsVolume            = 250.0f;
      _selectedProcNumber         = 2; _currentProcNumber = 1;
      _procedure_curr.comp_plasma = 10;
#endif
      // IT 11666:
      if (_procedure_curr.comp_plasma > 0.0f)
      {
         // IT11619 compare the collected plasma to the selected PAS (not selected comp Pls val)
         if ( (CurrentPlsVolume - RinsebackOffset)  > _floatCompPlsPASVolCurr)
         {

            if (_currentProcNumber != _selectedProcNumber)
            {

               DataLog (log_level_gui_info) << "compPls: continue button :"            << endmsg;
               DataLog (log_level_gui_info) << "  Current (pending selection) Pls Target  = " << (float)_procedure_curr.plasma          << endmsg;
               DataLog (log_level_gui_info) << "  Current (pending selection) PAS Target  = " <<  _floatCompPlsPASVolCurr                      << endmsg;
               DataLog (log_level_gui_info) << "  Selected (running) Pls Target   = " <<  guiglobs::cdsData.targets.PlasmaTarget.Get()           << endmsg;
               DataLog (log_level_gui_info) << "  Selected (running) PAS Target   = " <<  guiglobs::cdsData.targets.PltSolutionTarget.Get()                    << endmsg;
               DataLog (log_level_gui_info) << "  Collected Pls volume = " <<  guiglobs::cdsData.volumes.PlasmaVolume.Get()  << endmsg;

               if (guiglobs::cdsData.targets.PlasmaTarget.Get() <= guiglobs::cdsData.targets.PltSolutionTarget.Get())
               {
                  _currentlyConfirming = true;
                  timeoutResetMgr();

                  btn_continue.deallocate_resources();

                  createConfirmCallbacks();
                  _currPls_AllComp = false;     // if the warning came up it is not comp plsm, turn off diamond.

                  DataLog (log_level_gui_info) << "compPls: Putting up a Comp Pls confirm screen." << endmsg;
                  goto_child_screen(GUI_SCREEN_PREDICT_COMP_PLS_CONFIRM_BOX, "PREDICT COMPLEMENTARY PLASMA CONFIRMATION BOX");

               }
               else
               {
                  DataLog (log_level_gui_info) << "compPls: NO Comp Pls confirm screen. IT 13351" << endmsg;
                  select_current_procedure();
               }
            }
            else
            {
               DataLog (log_level_gui_info) << "No change of Procedure." << endmsg;
               select_current_procedure();
            }
         }
         else
         {

            select_current_procedure();
         }
      }
      else
      {

         select_current_procedure();
      }
   }
   else
   {

      bool showscreen = Software_CDS::GetInstance().getFeature(ProcedureConfirmPopup);
      if (showscreen && (guiglobs::screen_status.get_current_system_state() < DONOR_CONNECTED))
      {
         _currentlyConfirming = true;
         timeoutResetMgr();

         btn_continue.deallocate_resources();

         createConfirmCallbacks();

         DataLog (log_level_gui_info) << "Putting up a predict confirmation message." << endmsg;
         goto_child_screen(GUI_SCREEN_PREDICT_CONFIRM_BOX, "PREDICT CONFIRMATION BOX");
      }
      else
      {
         select_current_procedure();
      }
   }
}


///////////////////////////////////////////////////////////////////////////////////////////////////

void Screen_PREDICT::confirmCB ()
{
   destroyConfirmCallbacks();
   DataLog (log_level_gui_info) << "Predict Confirm Callback." << endmsg;
   select_current_procedure();

}


void Screen_PREDICT::cancelCB ()
{
   destroyConfirmCallbacks();
   DataLog (log_level_gui_info) << "Predict Cancel Callback." << endmsg;
   btn_continue.allocate_resources(*this);
}


void Screen_PREDICT::createConfirmCallbacks ()
{
   _procedureConfirmed.init(Callback<Screen_PREDICT>(this, &Screen_PREDICT::confirmCB));
   _procedureCanceled.init(Callback<Screen_PREDICT>(this, &Screen_PREDICT::cancelCB));
}


void Screen_PREDICT::destroyConfirmCallbacks ()
{
   _procedureConfirmed.deregisterMsg();
   _procedureCanceled.deregisterMsg();
}


void Screen_PREDICT::select_current_procedure ()
{
   guiglobs::procedure_selected = true;

   // IT 13240: we clear the predict screen requested flag here, because only at
   // this point have we done what the system needed the predict screen to do:
   // we have selected a procedure.
   PredictManager::clear_prediction_screen_requested();

   // reset the screen timeout
   timeoutResetMgr ();

   _selectedProcNumber = _currentProcNumber ;
   DataLog (log_level_gui_info) << "Selected Proc Number = " << _selectedProcNumber << endmsg;

   // Replace Fluid invocation may cross many "states" yet has only one outcome. Check for replace fluid here.
   if (_invocationType == REPFLUID_NO_DELTA)
   {
      PredictManager::sendPredictRequest(SELECT_CURRENT, HERE);

      // Send Proc an alarm "continue" message since "replace fluid prompt" is
      // invoked through the alarm mechanism and this "predict" invocation is
      // an extention of that alarm.
      //
      // Declare message
      Message<Alarm_response_struct> alarm_resp_msg (MessageBase::SEND_GLOBAL) ;
      // Declare the message structure
      Alarm_response_struct          alarm_resp_struct ;
      // Set the message info and send
      alarm_resp_struct.alarm_name     = REPLACEMENT_FLUID_ALARM;
      alarm_resp_struct.alarm_response = CONTINUE;
      alarm_resp_msg.send(alarm_resp_struct);

      // clear the alarm initialization variable so we can reallocate the same
      // alarm in succession after it has been dismissed
      guiglobs::gui_alarm_manager.clear_alarm_screen ();

      RemoveAllStatelessScreensMsg removeall(MessageBase::SEND_LOCAL);
      removeall.send(0);

      // Just return, don't fall through the old logic below.
      return;
   }

   if (_currPls_AllComp)
   {
      guiglobs::showCompPlsSymbol(true);
   }
   else
   {
      guiglobs::showCompPlsSymbol(false);
   }


   switch (_procData.system_state)
   {
      case STARTUP_TESTS :
      case AC_PRIME :
      {
         PredictManager::sendPredictRequest(SELECT_CURRENT, HERE);
         // Request to move to the SYSDONOR screen
         screenExit (GUI_SCREEN_SYSDONOR, "GUI_SCREEN_SYSDONOR");
      }
      break;

      case BLOOD_PRIME :
      {
         if (PredictManager::no_product() == true)
         {
            // Deallocate the timer receive message object after posting the
            // abort confirmation message once
            _msgTimer.armTimer(TimerMessage::DISARMED);

            // Request to move to the ABORTCNF screen
            screenExit (GUI_SCREEN_ABORTCNF, ABORT_TYPE_USER);
         }
         else
         {
            // register button press and product selection and submit
            PredictManager::sendPredictRequest(SELECT_CURRENT, HERE);
            // user desires back to goto the RUNPROC screen
            screenExit(GUI_SCREEN_RUNPROC, "GUI_SCREEN_RUNPROC");
         }
      }
      break;

      case BLOOD_RUN :
      {
         if (PredictManager::no_product() == true)
         {
            // Deallocate the timer receive message object after posting the
            // rinseback confirmation message once
            _msgTimer.armTimer(TimerMessage::DISARMED);

            // Request to move to the ABORTCNF screen
            screenExit (GUI_SCREEN_ABORTCNF, ABORT_TYPE_RINSEBACK);
         }
         else
         {
            // register button press and product selection and submit
            PredictManager::sendPredictRequest(SELECT_CURRENT, HERE);
            // user desires back to goto the RUNPROC screen
            screenExit(GUI_SCREEN_RUNPROC, "GUI_SCREEN_RUNPROC");
         }
      }
      break;

      case BLOOD_RINSEBACK :  // should not get here normally, if it happens, just switch back to run screen
      {
         DataLog (log_level_gui_info) << "Confirm procedure during rinseback, a corner case." << endmsg;
         screenExit (GUI_SCREEN_RUNPROC, "GUI_SCREEN_RUNPROC");
      }
      break;

      case DONOR_DISCONNECT : // we *really really* shouldn't get here, but if it happens,
                              // just go to the donor disconnect screen
      {
         DataLog (log_level_gui_info) << "Confirm procedure during donor disconnect, really  a corner case."
                                      << endmsg;
         screenExit (GUI_SCREEN_DISCONN, "GUI_SCREEN_DISCONN");
      }
      break;

      default :   // Error conditions, shouldn't happen
         DataLog (log_level_gui_error) << "Confirm procedure after donor disconnect, in system state "
                                       << _procData.system_state << ".  An error." << endmsg;
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm("procedure selected from PREDICT screen from state after donor disconnect");


   }  // End of switch (_procData.system_state)

}  // END of Screen_PREDICT PROCESS_CONTINUE_BUTTON


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_PREVIOUS_OPTION

void cbPrevBtn (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "Bad data from callback" << endmsg;
      return;
   }

   ((Screen_PREDICT*)data) -> processPrevBtn ();
}




////////////////////////////////////////////////////////////////////////////////
// CALLBACK_NEXT_OPTION

void cbNextBtn (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "Bad data from callback" << endmsg;
      return;
   }

   ((Screen_PREDICT*)data) -> processNextBtn ();
}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_MORE_OR_LESS

void callback_more_or_less (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "Bad data from callback" << endmsg;
      return;
   }

   ((Screen_PREDICT*)data) -> processMoreOrLess();
}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_BLOOD_TYPE
void cbTimeBtn (void* data)

{
   if (!data)
   {
      DataLog (log_level_gui_error) << "Bad data from callback" << endmsg;
      return;
   }

   ((Screen_PREDICT*)data) -> processTimeBtn ();
}


void cbAdjustFlowRate (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "Bad data from callback" << endmsg;
      return;
   }

   ((Screen_PREDICT*)data) -> processAdjustFlowRateBtn ();
}


void Screen_PREDICT::systemStateNotify ()
{
   if (_procData.system_state == BLOOD_RINSEBACK)
   {
      // Request to move to the RUNPROC screen
      screenExit (GUI_SCREEN_RUNPROC, "GUI_SCREEN_RUNPROC");
   }
}




void Screen_PREDICT::startBeepingSound ()
{
   // call sound method to stop alarm "beep"
   guiglobs::button_audio_feedback.create_feedback_sound (ALERT_SOUND, SOUND_CONTINUOUS);

   // Change the bitmap to the one with sound waves
   // _btnMute_AF.set_up_bitmap (BITMAP_BUTTON_ACTIVATE_SOUND);
   //
   _buttons.showActiveMuteBtn() ;
}


// PROCESS_MUTE_BUTTON
void Screen_PREDICT::muteBeepingSound ()
{
   // Destroy and recreate the timer that will drive the restart the alarm
   // sounds.
   _muteTimeout.armTimer(TimerMessage::ARMED);

   // Change the bitmap to one with no sound waves
   // _btnMute_AF.set_up_bitmap (BITMAP_BUTTON_DISABLE_SOUND);

   _buttons.showDeactiveMuteBtn();
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////                               /////////////////////////////////////////////
////////////////////////////////    SIM_NT only Code           /////////////////////////////////////////////
////////////////////////////////                               /////////////////////////////////////////////
////////////////////////////////                               /////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if CPU==SIMNT
void Screen_PREDICT::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo    = _screenTraverseMessage->getData();
   bool          allowAdjustFlow = false;
   _amapPlatelets = false;

   if (traverseInfo.type == SET_SHOW_FLOW_ADJUST)
   {
      allowAdjustFlow = (bool)traverseInfo.parameters[0];
      DataLog(log_level_sim_gui_side_info) << "Setting Allow Adjust Flow." << endmsg;
      // bool allowAdjustFlow = guiglobs::cdsData.config.SWOptionEnabled("allow_adjust_flow_rates_on_procedure_select", guiglobs::cdsData.config.LangUnit.Get().key_lang);
      guiglobs::cdsData.config.SWOptionSet("allow_adjust_flow_rates_on_procedure_select", allowAdjustFlow);
      // _allowAdjustFlowRate = !allowAdjustFlow;
      _allowAdjustFlowRate = allowAdjustFlow;

      if (_allowAdjustFlowRate && (guiglobs::screen_status.get_current_system_state() < DONOR_CONNECTED))
      {
         _time_prev_text.set_string_id(textEmptyFormatPredPrevTimeInitAlt, textEmptyFormatPredPrevTimeLabelInitAlt);
         _time_curr_text.set_string_id(textEmptyFormatPredCurrTimeInitAlt, textEmptyFormatPredCurrTimeLabelInitAlt);
         _time_next_text.set_string_id(textEmptyFormatPredNextTimeInitAlt, textEmptyFormatPredNextTimeLabelInitAlt);


         _txtLblTime.deallocate_resources();

         _buttons.allocate_flowrate(*this);

         _flowrate_prev_text.allocate_resources (*this);
         _flowrate_curr_text.allocate_resources (*this);
         _flowrate_next_text.allocate_resources (*this);
      }
      else
      {
         _time_prev_text.set_string_id(textEmptyFormatPredPrevTimeInit, textEmptyFormatPredPrevTimeLabelInit);
         _time_curr_text.set_string_id(textEmptyFormatPredCurrTimeInit, textEmptyFormatPredCurrTimeLabelInit);
         _time_next_text.set_string_id(textEmptyFormatPredNextTimeInit, textEmptyFormatPredNextTimeLabelInit);

         _buttons.deallocate_flowrate();

         _flowrate_prev_text.deallocate_resources();
         _flowrate_curr_text.deallocate_resources();
         _flowrate_next_text.deallocate_resources();

         _txtLblTime.allocate_resources(*this);
      }
   }
   else if (traverseInfo.type == SET_SHOW_PLT_VOLUME)
   {
      DataLog(log_level_sim_gui_side_info) << "Setting Screen Display Platelet Volume." << endmsg;
      bool displayPlateletVolume = (bool)traverseInfo.parameters[0];
      // bool displayPlateletVolume = guiglobs::cdsData.config.SWOptionEnabled("select_screen_display_platelet_volume", guiglobs::cdsData.config.LangUnit.Get().key_lang);
      guiglobs::cdsData.config.SWOptionSet("select_screen_display_platelet_volume", displayPlateletVolume);

      if (displayPlateletVolume)
      {
         _pltVol_curr_text.allocate_resources(*this);
      }
      else
      {
         _pltVol_curr_text.deallocate_resources();
      }
   }
   else if ( traverseInfo.type == SET_SHOW_PLASMA)
   {
      DataLog(log_level_sim_gui_side_info) << "Setting Screen Display Platelet Volume." << endmsg;
      bool displayPlateletVolume = (bool)traverseInfo.parameters[0];
      // bool displayPlateletVolume = guiglobs::cdsData.config.SWOptionEnabled("select_screen_display_platelet_volume", guiglobs::cdsData.config.LangUnit.Get().key_lang);
      guiglobs::cdsData.config.SWOptionSet("select_screen_display_platelet_volume", displayPlateletVolume);

      if (displayPlateletVolume)
      {
         _compPlsStatement.allocate_resources(*this);
         _txtDataACPlsVolmNext.allocate_resources (*this);
         _txtACPlsUnitsNext.allocate_resources (*this);
         _txtDataACPlsVolmCurr.allocate_resources (*this);
         _txtACPlsUnitsCurr.allocate_resources (*this);
         _txtDataACPlsVolmPrev.allocate_resources (*this);
         _txtACPlsUnitsPrev.allocate_resources (*this);
      }
      else
      {
         _compPlsStatement.deallocate_resources();
         _txtDataACPlsVolmNext.deallocate_resources ();
         _txtACPlsUnitsNext.deallocate_resources ();
         _txtDataACPlsVolmCurr.deallocate_resources ();
         _txtACPlsUnitsCurr.deallocate_resources ();
         _txtDataACPlsVolmPrev.deallocate_resources ();
         _txtACPlsUnitsPrev.deallocate_resources ();
      }
   }
   else if (traverseInfo.type == SET_SHOW_AMAP_PLT)
   {
      bool amapPlatelets = (bool)traverseInfo.parameters[0];
      if ( amapPlatelets)
      {
         _buttons.allocate_MoreOrLess(*this);
         _txtLblAMAPPPlt.allocate_resources (*this);
         _txtLblAMAPPPlas.allocate_resources (*this);
         _txtLblAMAPPRbc.allocate_resources (*this);

         _txtDataAMAPPSizePrev.allocate_resources (*this);
         _txtDataAMAPPSizeCurr.allocate_resources (*this);
         _txtDataAMAPPSizeNext.allocate_resources (*this);

         _rbc_prev_4AMAP_text.allocate_resources(*this);
         _rbc_curr_4AMAP_text.allocate_resources(*this);
         _rbc_next_4AMAP_text.allocate_resources(*this);

         _txtBetterAmapPltAvailable.allocate_resources(*this);

         _txtLblPlt.deallocate_resources();
         _txtLblRbc.deallocate_resources();
         _txtLblPlas.deallocate_resources();
         _txtLblTime.deallocate_resources();
      }
      else
      {
         _buttons.deallocate_MoreOrLess();
         _txtLblAMAPPPlt.deallocate_resources ();
         _txtLblAMAPPPlas.deallocate_resources ();
         _txtLblAMAPPRbc.deallocate_resources ();

         _txtDataAMAPPSizePrev.deallocate_resources ();
         _txtDataAMAPPSizeCurr.deallocate_resources ();
         _txtDataAMAPPSizeNext.deallocate_resources ();

         _rbc_prev_4AMAP_text.deallocate_resources();
         _rbc_curr_4AMAP_text.deallocate_resources();
         _rbc_next_4AMAP_text.deallocate_resources();

         _txtBetterAmapPltAvailable.deallocate_resources();

         _txtLblPlt.allocate_resources(*this);
         _txtLblRbc.allocate_resources(*this);
         _txtLblPlas.allocate_resources(*this);
         _txtLblTime.allocate_resources(*this);
      }


      _buttons.deallocate_flowrate();

      _flowrate_prev_text.deallocate_resources();
      _flowrate_curr_text.deallocate_resources();
      _flowrate_next_text.deallocate_resources();
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }

   updateScreenData();
   thawScreen(__FUNCTION__);
}
#endif /* if CPU==SIMNT */

/* FORMAT HASH 7d8b8d1a921ecec5b427cf797fc2c4af */

/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_RUNSUM_PLT

*     File name:   runsum_plt.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/runsum_plt.cpp 1.37 2009/03/23 15:21:23Z dslausb Exp dslausb $

*/

#include <vxworks.h>

// BASE CLASS INCLUDE
#include "runsum_plt.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "frmtdec.hpp"
#include "runsumGlobs.hpp"

// EXTERNAL REFERENCES
#include "guiglobs.hpp"

#define BITMAP_TEXTBOX_X 10
#define BITMAP_TEXTBOX_Y 350

static Screen_RUNSUM_PLT* screenPtrRunsumPlt = NULL;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_RUNSUM_PLT::Screen_RUNSUM_PLT()
   : runsum_product(GUI_SCREEN_RUNSUM_PLT)
{
   screenPtrRunsumPlt = this;
   // add ALL the logical buttons once... at screen creation
   _logicalButtons.createLogicalButton(LB_COUNT_PLATELET_PRODUCT);
   _logicalButtons.createLogicalButton(LB_VERIFY_PLATELET_YIELD_AND_VOLUME);
   _logicalButtons.createLogicalButton(LB_PLATELET_CONC_OUT_OF_RANGE);
}   // END of Screen_RUNSUM_PLT CONSTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_RUNSUM_PLT::~Screen_RUNSUM_PLT()
{}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_RUNSUM_PLT::allocate_resources (const char* allocation_parameter)
{
   runsum_product::allocate_resources(allocation_parameter);

   if (guiglobs::screen_status.get_current_system_state() == METERED_STORAGE)
   {
      // continue button should say "continue" instead of "next page" if we're starting metered storage.
      btn_continue.set_string_id (textButtonLongContinue);
   }
}  // END of Screen_RUNSUM_PLT ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Screen_RUNSUM_PLT::process_continue_button ()
{
   const RUN_SUMMARY_STRUCT& run_summary = _msgRunSummaryStats.getData();
   runsum_product::process_continue_button();
   RunsumGlobs::gotoNextRunSummaryScreen(run_summary);
}   // END of Screen_RUNSUM_PLT PROCESS_CONTINUE_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_PRODUCT_WARNING_DATA
void Screen_RUNSUM_PLT::processProductWarningData ()
{
   // This is invoked when the run summary data message is received.
   //   All the physical button will be allocated based on the verification flags
   //   based in this message.

   // Only process this once.
   if (_runsumMsgAlreadyProcessed) return;
   else _runsumMsgAlreadyProcessed = true;

   const RUN_SUMMARY_STRUCT& run_summary = _msgRunSummaryStats.getData();

   bool atLeastOneFlag                   =
      (PQA.NumberOfReasons(VERIFY_PLATELET_WBCS) > 0) ||
      (PQA.NumberOfReasons(VERIFY_PLATELET_YIELD) > 0) ||
      (PQA.NumberOfReasons(CONC_OUT_OF_RANGE) > 0);

   run_summary.PQdata.platelet_concentration_reasons.Log("Platelet Concentration");
   run_summary.PQdata.verify_platelet_wbc_reasons.Log("Platelet WBC");
   run_summary.PQdata.verify_platelet_yield_volume_reasons.Log("Platelet Yield");

   if (atLeastOneFlag)
   {
      _extendedDataCartouche.allocate_resources (*this) ;
      _txtProductVerifLbl.allocate_resources (*this) ;

      // New the physical button collection
      if (_ptrPhysicalButtons != NULL)
      {
         _FATAL_ERROR (__FILE__, __LINE__,
                       "Dual physical button collection allocation attempted");
         delete _ptrPhysicalButtons;
      }

      _ptrPhysicalButtons = new  PhysicalButtonCollection
                               (this,
                               // common X coord for all buttons
                               BITMAP_BUTTON_PRODUCT_WARNING1_X,
                               // initial Y coord for first button
                               BTN_PRD_WRN_Y,
                               // vertical increment between buttons
                               BITMAP_BUTTON_PRODUCT_WARNING_BTN_INC,
                               // the button ID for the first button (others will be calculated)
                               GUI_BUTTON_PRODVERIF_1) ;

      /////////////////////////////////////////////////////////////////////
      // if we have to count product, create its button
      LogicalButtonElement* logical_button = NULL;
      if ( (PQA.NumberOfReasons(VERIFY_PLATELET_WBCS) > 0) &&
           (NULL != (logical_button = _logicalButtons.getLogicalButton(LB_COUNT_PLATELET_PRODUCT))) )
      {
         // allocate the corresponding physical button using the logical button info.
         _ptrPhysicalButtons -> allocNextPhysBtn(logical_button,
                                                 physical_button_handler,
                                                 BITMAP_MAGNIFYING_GLASS_RUNSUMM_UP,
                                                 BITMAP_MAGNIFYING_GLASS_RUNSUMM_DOWN);

         //
         //   Add in all the needed text strings.  By definition first string is the header.
         //
         logical_button->clearExtendedElements();
         logical_button->addExtendedElement(getReasonHeaderIdFromVerifyFlag(VERIFY_PLATELET_WBCS));

         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_WBCS, SHOW_RBC_SPILLOVER);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_WBCS, SHOW_CENTRIFUGE_STOPPED);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_WBCS, SHOW_RBC_DETECTOR_CALIBRATION_ERROR);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_WBCS, SHOW_PLATELET_CONCENTRATION_TOO_HIGH);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_WBCS, SHOW_DONOR_HEMATOCRIT_CHANGE_TOO_LARGE);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_WBCS, SHOW_POSSIBLE_AIR_BLOCK);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_WBCS, SHOW_DONOR_HEMATOCRIT_TOO_HIGH);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_WBCS, SHOW_DETECTED_CONCENTRATION_TOO_LOW);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_WBCS, SHOW_PLATELET_CONTAMINANT_DETECTION);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_WBCS, SHOW_RINSEBACK_NOT_COMPLETED);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_WBCS, SHOW_CHANNEL_CLAMP_ERROR);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_WBCS, SHOW_WBC_CONTENT_NOT_VALIDATED);
      }  // end of "if (PQA.NumberOfReasons(VERIFY_PLATELET_WBCS) > 0)

      /////////////////////////////////////////////////////////////////////
      // if we have to verify platelet yield, create its button
      if ( (PQA.NumberOfReasons(VERIFY_PLATELET_YIELD) > 0) &&
           (NULL != (logical_button = _logicalButtons.getLogicalButton(LB_VERIFY_PLATELET_YIELD_AND_VOLUME))) )
      {
         // allocate the corresponding physical button using the logical button info.
         _ptrPhysicalButtons -> allocNextPhysBtn(logical_button,
                                                 physical_button_handler,
                                                 BITMAP_BUTTON_PRODUCT_WARNING_UP,
                                                 BITMAP_BUTTON_PRODUCT_WARNING_DN);
         //
         //
         //   Add in all the needed text strings.  By definition first string is the header.
         //
         logical_button->clearExtendedElements();
         logical_button->addExtendedElement(getReasonHeaderIdFromVerifyFlag(VERIFY_PLATELET_YIELD));

         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_YIELD, SHOW_RBC_SPILLOVER);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_YIELD, SHOW_CENTRIFUGE_STOPPED);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_YIELD, SHOW_PLATELET_CONCENTRATION_TOO_HIGH);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_YIELD, SHOW_POSSIBLE_AIR_BLOCK);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_YIELD, SHOW_DETECTED_CONCENTRATION_TOO_LOW);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_YIELD, SHOW_TARGET_VOL_NOT_ACHIEVED);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_YIELD, SHOW_METERED_VOLUME_ERROR);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_YIELD, SHOW_FINAL_CONC_NOT_ACHIEVED);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_YIELD, SHOW_METERED_SOLUTION_PRIME_ERROR);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLATELET_YIELD, SHOW_RINSEBACK_NOT_COMPLETED);
      }  // end of "if (PQA.NumberOfReasons(VERIFY_PLATELET_YIELD) > 0)

      /////////////////////////////////////////////////////////////////////
      // if we have to deal with concentration out of range, create its button

      if ( (PQA.NumberOfReasons(CONC_OUT_OF_RANGE) > 0) &&
           (NULL != (logical_button = _logicalButtons.getLogicalButton(LB_PLATELET_CONC_OUT_OF_RANGE))) )
      {
         // allocate the corresponding physical button using the logical button info.
         _ptrPhysicalButtons -> allocNextPhysBtn(logical_button,
                                                 physical_button_handler,
                                                 BITMAP_BUTTON_PRODUCT_WARNING_UP,
                                                 BITMAP_BUTTON_PRODUCT_WARNING_DN);
         //
         //   Add in all the needed text strings.  By definition first string is the header.
         //
         logical_button->clearExtendedElements();
         logical_button->addExtendedElement(getReasonHeaderIdFromVerifyFlag(CONC_OUT_OF_RANGE));

         addReasonToButtonIfNeeded(logical_button, CONC_OUT_OF_RANGE, SHOW_PREDICTED_CONCENTRATION_BELOW_STORAGE_RANGE);
         addReasonToButtonIfNeeded(logical_button, CONC_OUT_OF_RANGE, SHOW_PREDICTED_CONCENTRATION_ABOVE_STORAGE_RANGE);
         addReasonToButtonIfNeeded(logical_button, CONC_OUT_OF_RANGE, SHOW_FINAL_CONC_NOT_ACHIEVED);
      }  // end of "if (PQA.NumberOfReasons(CONC_OUT_OF_RANGE) > 0)


      // temp fix till screen refresh is fixed. IT - 6038
      _extendedDataCartouche.move_to_back();
      _extendedDataCartouche.move_to_front();

      // set the first (non-button specific) extended text.
      LogicalButtonElement::EXTENDED_TEXT_LIST firsttext;
      firsttext.push_back(textMiscRunsumProdMoreInfo);
      setExtendedText (firsttext) ;
   }
   else
   {
      // This used to be a fatal error, but it isn't anymore.
      DataLog(log_level_gui_info) << "No flags for platelet product." << endmsg;
   }


   LABEL_TOGGLE lblToggle          = DONT_SHOW_LABEL_BOX;

   bool         isPostRun          = (guiglobs::screen_status.get_current_system_state() == POST_RUN);

   bool         prodIsLeukoreduced = PQA.LabelPlatelets();

   if (prodIsLeukoreduced)
   {
      if (isPostRun)
      {
         DataLog(log_level_gui_info) << "Label PLT product as leukoreduced." << endmsg;
         lblToggle = LABEL_PRODUCT;
      }
      else
      {
         DataLog(log_level_gui_info) << "PLT Leukoreduction OK so far, but PAS addition is still ahead, so don't show it." << endmsg;
         lblToggle = DONT_SHOW_LABEL_BOX;
      }
   }
   else
   {
      DataLog(log_level_gui_info) << "DO NOT Label PLT product as leukoreduced." << endmsg;
      lblToggle = DONT_LABEL_PRODUCT;
   }

   setLabel(lblToggle);

   btn_continue.allocate_resources(*this);

   RunsumGlobs::completeRunsumStatsProcessing();
}   // END of Screen_RUNSUM_PLT PROCESS_PRODUCT_WARNING_DATA

////////////////////////////////////////////////////////////////////////////////
// PHYSICAL_BUTTON_HANDLER
void Screen_RUNSUM_PLT::physical_button_handler (void* data)
{
   // Actually call the generic physical button routine within the Screen_RUNSUM_PLT class.
   screenPtrRunsumPlt->processButtonPress((Button_Bitmap*)data);

}   // END of PHYSICAL_BUTTON_HANDLER

/* FORMAT HASH ea4b56781f038ab6cd4870b86ef314f7 */

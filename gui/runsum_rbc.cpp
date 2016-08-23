/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_RUNSUM_RBC

*     File name:   runsum_rbc.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/runsum_rbc.cpp 1.36 2009/03/23 15:21:35Z dslausb Exp dslausb $

*/

#include <vxworks.h>

// BASE CLASS INCLUDE
#include "runsum_rbc.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "frmtdec.hpp"
#include "runsumGlobs.hpp"

// EXTERNAL REFERENCES
#include "guiglobs.hpp"

static Screen_RUNSUM_RBC* Screen_RUNSUM_RBC_ptr       = NULL; // for use by callback
static Screen_RUNSUM_RBC* Screen_RUNSUM_RBC_ptr_prod1 = NULL; // for use by callback
static Screen_RUNSUM_RBC* Screen_RUNSUM_RBC_ptr_prod2 = NULL; // for use by callback

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
//
Screen_RUNSUM_RBC::Screen_RUNSUM_RBC(GUI_SCREEN_ID screenId)
   : runsum_product(screenId)
{
   switch (screenId)
   {
      case GUI_SCREEN_RUNSUM_RBC : Screen_RUNSUM_RBC_ptr             = this; break;
      case GUI_SCREEN_RUNSUM_RBC_PROD1 : Screen_RUNSUM_RBC_ptr_prod1 = this; break;
      case GUI_SCREEN_RUNSUM_RBC_PROD2 : Screen_RUNSUM_RBC_ptr_prod2 = this; break;
      default :
         DataLog (log_level_gui_error) << "ERROR: Unknown screen ID for Runsumm RBC." << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad Screen ID");
         break;
   }

   // add ALL the logical buttons once... at screen creation
   _logicalButtons.createLogicalButton(LB_VERIFY_RBC_PRODUCT);
   _logicalButtons.createLogicalButton(LB_VERIFY_RBC_WBCS);
   _logicalButtons.createLogicalButton(LB_VERIFY_RBC_QUALITY);
}   // END of Screen_RUNSUM_RBC CONSTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_RUNSUM_RBC::~Screen_RUNSUM_RBC()
{}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_RUNSUM_RBC::allocate_resources (const char* allocation_parameter)
{
   runsum_product::allocate_resources(allocation_parameter);
   if (guiglobs::screen_status.get_current_system_state() == METERED_STORAGE)
   {
      // continue button should say "continue" instead of "next page" if we're starting metered storage.
      btn_continue.set_string_id (textButtonLongContinue);
   }
}  // END of Screen_RUNSUM_RBC ALLOCATE_RESOURCES

////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Screen_RUNSUM_RBC::process_continue_button ()
{
   runsum_product::process_continue_button();

   const RUN_SUMMARY_STRUCT& run_summary = _msgRunSummaryStats.getData();
   RunsumGlobs::gotoNextRunSummaryScreen(run_summary);
}   // END of Screen_RUNSUM_RBC PROCESS_CONTINUE_BUTTON

////////////////////////////////////////////////////////////////////////////////
// PROCESS_PRODUCT_WARNING_DATA
void Screen_RUNSUM_RBC::processProductWarningData ()
{
   bool                  atLeastOneFlag = false;
   LogicalButtonElement* logical_button = NULL;
   // This is invoked when the run summary data message is received.
   //   All the physical button will be allocated based on the verification flags
   //   based in this message.

   // Only process this once.
   if (_runsumMsgAlreadyProcessed) return;
   else _runsumMsgAlreadyProcessed = true;

   const RUN_SUMMARY_STRUCT& run_summary = _msgRunSummaryStats.getData();

   DataLog (log_level_gui_info) << "WindowId=" << window_ID << endmsg;

   DataLog (log_level_gui_info) << "PQA.NumberOfReasons(VERIFY_RBC_VOL) = " << PQA.NumberOfReasons(VERIFY_RBC_VOL) << endmsg;
   DataLog (log_level_gui_info) << "PQA.NumberOfReasons(VERIFY_RBC_WBCS) = " << PQA.NumberOfReasons(VERIFY_RBC_WBCS) << endmsg;
   DataLog (log_level_gui_info) << "PQA.NumberOfReasons(VERIFY_RBC_QUALITY) = " << PQA.NumberOfReasons(VERIFY_RBC_QUALITY) << endmsg;

   DataLog (log_level_gui_info) << "PQA.NumberOfReasons(VERIFY_RBC_VOL_PROD2) = " << PQA.NumberOfReasons(VERIFY_RBC_VOL_PROD2) << endmsg;
   DataLog (log_level_gui_info) << "PQA.NumberOfReasons(VERIFY_RBC_WBCS_PROD2) = " << PQA.NumberOfReasons(VERIFY_RBC_WBCS_PROD2) << endmsg;
   DataLog (log_level_gui_info) << "PQA.NumberOfReasons(VERIFY_RBC_QUALITY_PROD2) = " << PQA.NumberOfReasons(VERIFY_RBC_QUALITY_PROD2) << endmsg;

   DataLog (log_level_gui_info) << "run_summary.rs2_RBC_volume = " << run_summary.rs2_RBC_volume << endmsg;
   DataLog (log_level_gui_info) << "run_summary.rs2_RBC_volume_2 = " << run_summary.rs2_RBC_volume_2 << endmsg;

   switch (window_ID)
   {
      case GUI_SCREEN_RUNSUM_RBC :
         atLeastOneFlag =
            (PQA.NumberOfReasons(VERIFY_RBC_VOL) > 0) ||
            (PQA.NumberOfReasons(VERIFY_RBC_WBCS) > 0) ||
            (PQA.NumberOfReasons(VERIFY_RBC_QUALITY) > 0);

         run_summary.PQdata.verify_rbc_volume_reasons.Log("RBC Volume");
         run_summary.PQdata.verify_rbc_wbc_reasons.Log("RBC WBC");
         run_summary.PQdata.verify_rbc_quality_reasons.Log("RBC Quality");
         break;

      case GUI_SCREEN_RUNSUM_RBC_PROD1 :
         atLeastOneFlag =
            (PQA.NumberOfReasons(VERIFY_RBC_VOL) > 0) ||
            (PQA.NumberOfReasons(VERIFY_RBC_WBCS) > 0) ||
            (PQA.NumberOfReasons(VERIFY_RBC_QUALITY) > 0);

         run_summary.PQdata.verify_rbc_volume_reasons.Log("RBC Volume (prod1)");
         run_summary.PQdata.verify_rbc_wbc_reasons.Log("RBC WBC (prod1)");
         run_summary.PQdata.verify_rbc_quality_reasons.Log("RBC Quality (prod1)");
         break;

      case GUI_SCREEN_RUNSUM_RBC_PROD2 :
         atLeastOneFlag =
            (PQA.NumberOfReasons(VERIFY_RBC_VOL_PROD2) > 0) ||
            (PQA.NumberOfReasons(VERIFY_RBC_WBCS_PROD2) > 0) ||
            (PQA.NumberOfReasons(VERIFY_RBC_QUALITY_PROD2) > 0);

         run_summary.PQdata.verify_rbc_volume_reasons_prod2.Log("RBC Volume (prod2)");
         run_summary.PQdata.verify_rbc_wbc_reasons_prod2.Log("RBC WBC (prod2)");
         run_summary.PQdata.verify_rbc_quality_reasons_prod2.Log("RBC Quality (prod2)");
         break;

      default :
         DataLog (log_level_gui_error) << "ERROR: RunsumRbc screen constructed with bad screen ID." << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad screen ID for RBC run summary screen");
         break;
   }

   btn_continue.allocate_resources(*this);

   if (atLeastOneFlag)
   {
      _extendedDataCartouche.allocate_resources (*this) ;
      _txtProductVerifLbl.allocate_resources (*this) ;

      // set the first (non-button specific) extended text.
      LogicalButtonElement::EXTENDED_TEXT_LIST firsttext;
      firsttext.push_back(textMiscRunsumProdMoreInfo);
      setExtendedText (firsttext) ;

      // temp fix till screen refresh is fixed. IT - 6038
      _extendedDataCartouche.move_to_back();
      _extendedDataCartouche.move_to_front();

      // New the physical button collection
      if (_ptrPhysicalButtons != NULL)
      {
         _FATAL_ERROR (__FILE__, __LINE__,
                       "Dual physical button collection allocation attempted");
      }
      else
      {
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
      }

      if (window_ID == GUI_SCREEN_RUNSUM_RBC_PROD2)
      {
         DataLog (log_level_gui_info) << "Populating button info for RBC product" << endmsg;
         if ( (PQA.NumberOfReasons(VERIFY_RBC_VOL_PROD2) > 0) &&
              (NULL != (logical_button = _logicalButtons.getLogicalButton(LB_VERIFY_RBC_PRODUCT))) )
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
            logical_button->addExtendedElement(getReasonHeaderIdFromVerifyFlag(VERIFY_RBC_VOL_PROD2));

            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_VOL_PROD2, SHOW_CENTRIFUGE_STOPPED);
            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_VOL_PROD2, SHOW_POSSIBLE_AIR_BLOCK);
            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_VOL_PROD2, SHOW_TARGET_VOL_NOT_ACHIEVED);
            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_VOL_PROD2, SHOW_METERED_VOLUME_ERROR);
            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_VOL_PROD2, SHOW_APS_HIGH);
            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_VOL_PROD2, SHOW_CPS_HIGH);
            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_VOL_PROD2, SHOW_METERED_SOLUTION_PRIME_ERROR);
         }

         if ( (PQA.NumberOfReasons(VERIFY_RBC_WBCS_PROD2) > 0) &&
              (NULL != (logical_button = _logicalButtons.getLogicalButton(LB_VERIFY_RBC_WBCS))) )
         {
            // allocate the corresponding physical button using the logical button info.
            _ptrPhysicalButtons -> allocNextPhysBtn(logical_button,
                                                    physical_button_handler,
                                                    BITMAP_BUTTON_PRODUCT_WARNING_UP,
                                                    BITMAP_BUTTON_PRODUCT_WARNING_DN);

            logical_button->clearExtendedElements();

            logical_button->addExtendedElement(getReasonHeaderIdFromVerifyFlag(VERIFY_RBC_WBCS_PROD2));

            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_WBCS_PROD2, SHOW_CPS_HIGH);
            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_WBCS_PROD2, SHOW_APS_HIGH);
         }

         if ( (PQA.NumberOfReasons(VERIFY_RBC_QUALITY_PROD2) > 0) &&
              (NULL != (logical_button = _logicalButtons.getLogicalButton(LB_VERIFY_RBC_QUALITY))) )
         {
            // allocate the corresponding physical button using the logical button info.
            _ptrPhysicalButtons -> allocNextPhysBtn(logical_button,
                                                    physical_button_handler,
                                                    BITMAP_BUTTON_PRODUCT_WARNING_UP,
                                                    BITMAP_BUTTON_PRODUCT_WARNING_DN);

            logical_button->clearExtendedElements();
            logical_button->addExtendedElement(getReasonHeaderIdFromVerifyFlag(VERIFY_RBC_QUALITY_PROD2));

            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_QUALITY_PROD2, SHOW_CPS_HIGH);
            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_QUALITY_PROD2, SHOW_APS_HIGH);
         }
      }
      else
      {
         DataLog (log_level_gui_info) << "Populating button info for second RBC product" << endmsg;
         if ( (PQA.NumberOfReasons(VERIFY_RBC_VOL) > 0) &&
              (NULL != (logical_button = _logicalButtons.getLogicalButton(LB_VERIFY_RBC_PRODUCT))) )
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
            logical_button->addExtendedElement(getReasonHeaderIdFromVerifyFlag(VERIFY_RBC_VOL));

            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_VOL, SHOW_CENTRIFUGE_STOPPED);
            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_VOL, SHOW_POSSIBLE_AIR_BLOCK);
            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_VOL, SHOW_TARGET_VOL_NOT_ACHIEVED);
            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_VOL, SHOW_METERED_VOLUME_ERROR);
            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_VOL, SHOW_APS_HIGH);
            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_VOL, SHOW_CPS_HIGH);
            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_VOL, SHOW_METERED_SOLUTION_PRIME_ERROR);
         }

         if ( (PQA.NumberOfReasons(VERIFY_RBC_WBCS) > 0) &&
              (NULL != (logical_button = _logicalButtons.getLogicalButton(LB_VERIFY_RBC_WBCS))) )
         {
            // allocate the corresponding physical button using the logical button info.
            _ptrPhysicalButtons -> allocNextPhysBtn(logical_button,
                                                    physical_button_handler,
                                                    BITMAP_BUTTON_PRODUCT_WARNING_UP,
                                                    BITMAP_BUTTON_PRODUCT_WARNING_DN);

            logical_button->clearExtendedElements();
            logical_button->addExtendedElement(getReasonHeaderIdFromVerifyFlag(VERIFY_RBC_WBCS));

            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_WBCS, SHOW_CPS_HIGH);
            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_WBCS, SHOW_APS_HIGH);
         }

         if ( (PQA.NumberOfReasons(VERIFY_RBC_QUALITY) > 0) &&
              (NULL != (logical_button = _logicalButtons.getLogicalButton(LB_VERIFY_RBC_QUALITY))) )
         {
            // allocate the corresponding physical button using the logical button info.
            _ptrPhysicalButtons -> allocNextPhysBtn(logical_button,
                                                    physical_button_handler,
                                                    BITMAP_BUTTON_PRODUCT_WARNING_UP,
                                                    BITMAP_BUTTON_PRODUCT_WARNING_DN);

            logical_button->clearExtendedElements();
            logical_button->addExtendedElement(getReasonHeaderIdFromVerifyFlag(VERIFY_RBC_QUALITY));

            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_QUALITY, SHOW_CPS_HIGH);
            addReasonToButtonIfNeeded(logical_button, VERIFY_RBC_QUALITY, SHOW_APS_HIGH);
         }
      }
   }
   else
   {
      // This used to be a fatal error, but it isn't anymore.
      DataLog(log_level_gui_info) << "No flags for RBC product." << endmsg;
   }

   bool         setIsPtf  = guiglobs::cdsData.targets.MeterRbcs.Get();
   LABEL_TOGGLE lblToggle = DONT_SHOW_LABEL_BOX;

   if (setIsPtf)
   {
      bool isPostRun          = (guiglobs::screen_status.get_current_system_state() == POST_RUN);
      bool isFirstScreen      = (window_ID == GUI_SCREEN_RUNSUM_RBC) || (window_ID == GUI_SCREEN_RUNSUM_RBC_PROD1);
      bool isSecondScreen     = (window_ID == GUI_SCREEN_RUNSUM_RBC_PROD2);

      bool prodIsLeukoreduced = ((isFirstScreen && PQA.LabelRbcs()) ||
                                 (isSecondScreen && PQA.LabelRbcsProd2()));

      if (prodIsLeukoreduced)
      {
         if (isPostRun)
         {
            DataLog(log_level_gui_info) << "Label RBC product as leukoreduced." << endmsg;
            lblToggle = LABEL_PRODUCT;
         }
         else
         {
            DataLog(log_level_gui_info) << "RBC Leukoreduction OK so far, but RAS addition is still ahead, so don't show it." << endmsg;
            lblToggle = DONT_SHOW_LABEL_BOX;
         }
      }
      else
      {
         DataLog(log_level_gui_info) << "DO NOT Label RBC product as leukoreduced." << endmsg;
         lblToggle = DONT_LABEL_PRODUCT;
      }
   }
   else
   {
      DataLog(log_level_gui_info) << "Disposable set is not PTF. Show no leukoreduction message." << endmsg;
      lblToggle = DONT_SHOW_LABEL_BOX;
   }

   setLabel(lblToggle);

   RunsumGlobs::completeRunsumStatsProcessing();
}   // END of Screen_RUNSUM_RBC PROCESS_PRODUCT_WARNING_DATA

////////////////////////////////////////////////////////////////////////////////
// PHYSICAL_BUTTON_HANDLER
void Screen_RUNSUM_RBC::physical_button_handler (void* data)
{
   DataLog (log_level_gui_info) << "Entering physical button handler." << endmsg;
   // Actually call the generic physical button routine within the Screen_RUNSUM_RBC class.

   GUI_SCREEN_ID      scrnId  = guiglobs::screen_status.get_current_screen();
   Screen_RUNSUM_RBC* scrnPtr = NULL;

   switch (scrnId)
   {
      case GUI_SCREEN_RUNSUM_RBC : scrnPtr       = Screen_RUNSUM_RBC_ptr; break;
      case GUI_SCREEN_RUNSUM_RBC_PROD1 : scrnPtr = Screen_RUNSUM_RBC_ptr_prod1; break;
      case GUI_SCREEN_RUNSUM_RBC_PROD2 : scrnPtr = Screen_RUNSUM_RBC_ptr_prod2; break;
      default :
         DataLog (log_level_gui_error) << "ERROR: Unknown screen pointer for Runsumm RBC." << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad Screen Pointer");
         break;
   }

   DataLog (log_level_gui_info) << "Processing the button press." << endmsg;
   if (scrnPtr) scrnPtr->processButtonPress((Button_Bitmap*)data);

   DataLog (log_level_gui_info) << "Leaving physical button handler." << endmsg;

}   // END of PHYSICAL_BUTTON_HANDLER

/* FORMAT HASH 6212c79d6287388f880593582ba20d0f */

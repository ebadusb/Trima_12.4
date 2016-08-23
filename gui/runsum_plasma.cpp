/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 GAMBRO BCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   Screen_RUNSUM_PLASMA

*     File name:    runsum_plasma.cpp

*     Contents:     See prototype header descriptions below...

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/runsum_plasma.cpp 1.24 2009/03/23 15:21:19Z dslausb Exp dslausb $

*/

#include <vxworks.h>

// BASE CLASS INCLUDE
#include "runsum_plasma.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "frmtdec.hpp"
#include "runsumGlobs.hpp"

// EXTERNAL REFERENCES
#include "guiglobs.hpp"

#define BITMAP_TEXTBOX_X 10
#define BITMAP_TEXTBOX_Y 350

static Screen_RUNSUM_PLASMA* Screen_RUNSUM_PLASMA_ptr = NULL; // for use by callback

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_RUNSUM_PLASMA::Screen_RUNSUM_PLASMA()
   : runsum_product(GUI_SCREEN_RUNSUM_PLASMA)
{
   Screen_RUNSUM_PLASMA_ptr = this;

   // add ALL the logical buttons once... at screen creation
   _logicalButtons.createLogicalButton(LB_COUNT_PLASMA_PRODUCT);
   _logicalButtons.createLogicalButton(LB_VERIFY_PLASMA_VOLUME);
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_RUNSUM_PLASMA::~Screen_RUNSUM_PLASMA()
{}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_RUNSUM_PLASMA::allocate_resources (const char* allocation_parameter)
{
   runsum_product::allocate_resources(allocation_parameter);
}  // END of Screen_RUNSUM_PLASMA ALLOCATE_RESOURCES

////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Screen_RUNSUM_PLASMA::process_continue_button ()
{
   runsum_product::process_continue_button();

   const RUN_SUMMARY_STRUCT& run_summary = _msgRunSummaryStats.getData();

   RunsumGlobs::gotoNextRunSummaryScreen(run_summary);
}   // END of Screen_RUNSUM_PLASMA PROCESS_CONTINUE_BUTTON

////////////////////////////////////////////////////////////////////////////////
// PROCESS_PRODUCT_WARNING_DATA
void Screen_RUNSUM_PLASMA::processProductWarningData ()
{
   // This is invoked when the run summary data message is received.
   //   All the physical button will be allocated based on the verification flags
   //   based in this message.

   // Only process this once.
   if (_runsumMsgAlreadyProcessed) return;
   else _runsumMsgAlreadyProcessed = true;

   const RUN_SUMMARY_STRUCT& run_summary = _msgRunSummaryStats.getData();

   bool atLeastOneFlag                   =
      (PQA.NumberOfReasons(VERIFY_PLASMA_WBCS) > 0) ||
      (PQA.NumberOfReasons(VERIFY_PLASMA_VOL) > 0);

   run_summary.PQdata.verify_plasma_volume_reasons.Log("Plasma Volume");
   run_summary.PQdata.verify_plasma_wbc_reasons.Log("Plasma WBC");


   if (atLeastOneFlag)
   {
      _extendedDataCartouche.allocate_resources (*this) ;
      _txtProductVerifLbl.allocate_resources (*this) ;

      LogicalButtonElement* logical_button = NULL;

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

      if ( (PQA.NumberOfReasons(VERIFY_PLASMA_WBCS) > 0) &&
           (NULL != (logical_button = _logicalButtons.getLogicalButton(LB_COUNT_PLASMA_PRODUCT))) )
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
         logical_button->addExtendedElement(getReasonHeaderIdFromVerifyFlag(VERIFY_PLASMA_WBCS));

         addReasonToButtonIfNeeded(logical_button, VERIFY_PLASMA_WBCS, SHOW_RBC_SPILLOVER);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLASMA_WBCS, SHOW_CENTRIFUGE_STOPPED);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLASMA_WBCS, SHOW_RBC_DETECTOR_CALIBRATION_ERROR);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLASMA_WBCS, SHOW_POSSIBLE_AIR_BLOCK);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLASMA_WBCS, SHOW_DETECTED_CONCENTRATION_TOO_LOW);
      }  // end of "if (PQA.NumberOfReasons(VERIFY_PLASMA_WBCS) > 0)


      /////////////////////////////////////////////////////////////////////
      // if we have to check plasma volume, create its button

      if ( (PQA.NumberOfReasons(VERIFY_PLASMA_VOL) > 0) &&
           (NULL != (logical_button = _logicalButtons.getLogicalButton(LB_VERIFY_PLASMA_VOLUME))) )
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
         logical_button->addExtendedElement(getReasonHeaderIdFromVerifyFlag(VERIFY_PLASMA_VOL));

         addReasonToButtonIfNeeded(logical_button, VERIFY_PLASMA_VOL, SHOW_CENTRIFUGE_STOPPED);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLASMA_VOL, SHOW_POSSIBLE_AIR_BLOCK);
         addReasonToButtonIfNeeded(logical_button, VERIFY_PLASMA_VOL, SHOW_TARGET_VOL_NOT_ACHIEVED);
      }  // end of "if (PQA.NumberOfReasons(VERIFY_PLASMA_VOL) > 0)

      // If we haven't created at least ONE button, then Proc has changed the info
      //  since our caller checked, and this should be honored...


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
      DataLog(log_level_gui_info) << "No flags for plasma product." << endmsg;
   }

   bool labelPlasma = PQA.LabelPlasma();

   if (labelPlasma)
      DataLog(log_level_gui_info) << "Label plasma product as leukoreduced." << endmsg;
   else
      DataLog(log_level_gui_info) << "DO NOT Label plasma product as leukoreduced." << endmsg;

   // temp patch to make text on top of cartouche label product bitmap
   setLabel((LABEL_TOGGLE)labelPlasma);

   _bitmapLblTextBox.move_to_back();
   _bitmapLblTextBox.move_to_front();

   btn_continue.allocate_resources(*this);

   RunsumGlobs::completeRunsumStatsProcessing();
}   // END of Screen_RUNSUM_PLASMA PROCESS_PRODUCT_WARNING_DATA

////////////////////////////////////////////////////////////////////////////////
// PHYSICAL_BUTTON_HANDLER
void Screen_RUNSUM_PLASMA::physical_button_handler (void* data)
{
   // Actually call the generic physical button routine within the Screen_RUNSUM_PLASMA class.
   Screen_RUNSUM_PLASMA_ptr->processButtonPress((Button_Bitmap*)data);

}   // END of PHYSICAL_BUTTON_HANDLER

void Screen_RUNSUM_PLASMA::initialize_status_line ()
{
   runsum_product::initialize_status_line();
   guiglobs::apheresis_status_line->set_current_text (textStatEndOfRunPlasma);
}

/* FORMAT HASH eba87c45483a6fc8906726892736f36e */

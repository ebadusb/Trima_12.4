/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   runsum_product

*     File name:    runsum_product.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************
*/

#include <vxworks.h>

// BASE CLASS INCLUDE
#include "runsum_product.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"

// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "runsumGlobs.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
runsum_product::runsum_product(GUI_SCREEN_ID screen_id)
   : Base_Apheresis_Window(MAGENTA, screen_id),
     _msgRunSummaryStats(),
     _runsumMsgAlreadyProcessed(false),
     _extendedDataCartouche(BITMAP_CARTOUCHE_PRODUCT_WARNING_X,
                            BTN_PRD_WRN_Y,
                            BITMAP_CARTOUCHE_PRODUCT_WARNING),
     _sectionLine(3, 179, BITMAP_RUNSUM_HORIZ_LINE),
     _bitmapLblTextBox(9, 113, BITMAP_CARTOUCHE_LABEL_PRODUCT),
     _txtProductVerifLbl(getProdVerifyCategory(screen_id) ),
     _textLblProdBody(getProdVerifyDoLabel(screen_id) ),
     _textDontLblProdBody(getProdVerifyDontLabel(screen_id) ),
     _extendedheader(NULL),
     _extendedText(),
     _logicalButtons(),
     _ptrPhysicalButtons(NULL),
     PQA(),
     _statLineStrId(getProdVerifyStatusLine(screen_id) )
{}

runsum_product::~runsum_product()
{
   // free resources, including the physical button collection
   runsum_product::deallocate_resources();
}

void runsum_product::requestRunsumStats ()
{
   _msgRunSummaryStats.init(Callback<runsum_product>(this, &runsum_product::processProductWarningData), MessageBase::SNDRCV_RECEIVE_ONLY);

   RunsumGlobs::requestRunsumStats();
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void runsum_product::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();
   btn_cancel.deallocate_resources ();
   btn_goback.deallocate_resources ();
   btn_continue.deallocate_resources(); // until stats received

   // Change the text on the continue button to "next page"
   btn_continue.set_string_id (textButtonLongNextPage);

   // Now link the status bar/line to this window
   link_status_bar_line();

   // Reset the product verification receipt latch (only one per allocation)
   _runsumMsgAlreadyProcessed = false ;

   requestRunsumStats();
}  // END of Screen_RUNSUM_PLT ALLOCATE_RESOURCES

////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void runsum_product::deallocate_resources ()
{
   // DEALLOCATE BITMAPS
   _extendedDataCartouche.deallocate_resources();
   _sectionLine.deallocate_resources();
   _bitmapLblTextBox.deallocate_resources();

   // DEALLOCATE TEXT
   _txtProductVerifLbl.deallocate_resources();
   _textLblProdBody.deallocate_resources();
   _textDontLblProdBody.deallocate_resources();

   for (int item = 0; item<EXTENDED_TEXT_MAXITEMS; item++)
      _extendedText[item].deallocate_resources();

   if (_extendedheader)
   {
      _extendedheader->deallocate_resources();
      delete _extendedheader;
      _extendedheader = 0;
   }

   _msgRunSummaryStats.deregisterMsg();

   //
   //
   //  Remove any existing extended text (deallocation is safe for nondisplayed resources)
   //

   // deallocate the physical button collection
   delete _ptrPhysicalButtons;
   _ptrPhysicalButtons = NULL;

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // Deallocate the run summary receive message object
   _runsumMsgAlreadyProcessed = 0;

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();


}   // END of Screen_RUNSUM_PLT DEALLOCATE_RESOURCES

////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void runsum_product::process_continue_button ()
{
   // Disable all the standard buttons on this screen
   disable_standard_buttons();
}

////////////////////////////////////////////////////////////////////////////////
// SET_EXTENDED_TEXT
void runsum_product::setExtendedText (LogicalButtonElement::EXTENDED_TEXT_LIST& textlist)
{
   //
   //  Start by removing any existing text (deallocation is safe for nondisplayed resources)
   //
   int item;
   for (item = 0; item<EXTENDED_TEXT_MAXITEMS; item++)
      _extendedText[item].deallocate_resources();
   if (_extendedheader)
   {
      _extendedheader->deallocate_resources();
      delete _extendedheader;
      _extendedheader = 0;
   }

   //
   //  Go through and reallocate appropriate text items
   //
   item = 0;
   OSRegion widget_positions(346, 0, 277, 24);

   for (LogicalButtonElement::EXTENDED_TEXT_LIST::iterator iter = textlist.begin(); iter != textlist.end(); iter++)
   {
      if (item == 0)
      {
         _extendedheader = new Display_Text(*iter);
         _extendedheader->allocate_resources(*this);
      }
      else
      {
         _extendedText[item].set_string_id(*iter);

         //
         //   Position this text at the next spot
         //
         widget_positions.y = (short)(260 + (item - 1) * 26);
         _extendedText[item].allocate_resources(*this) ;
         _extendedText[item].set_requested_region(widget_positions);
      }

      if (++item >= EXTENDED_TEXT_MAXITEMS) break; // just in case
   }

}  // END of Screen_RUNSUM_PLT SET_EXTENDED_TEXT

////////////////////////////////////////////////////////////////////////////////
// PROCESS_BUTTON_ARRAY_PRESS
void runsum_product::processButtonPress (Button_Bitmap* ptrPressedButtonWidget)
{
   // This is invoked when any product verification button is pushed
   //  They are differentiated by their associated widget pointer which is passed to
   //  this call back (via the C stub, PHYSICAL_BUTTON_HANDLER).

   // Set the associated button to the "down" state
   _ptrPhysicalButtons -> setRadioBtnDown (ptrPressedButtonWidget) ;

   // Get the extended text ID and set the extended text
   setExtendedText (_ptrPhysicalButtons -> getExtendedTxt (ptrPressedButtonWidget)) ;

}   // END of Screen_RUNSUM_PLT PROCESS_BUTTON_ARRAY_PRESS

////////////////////////////////////////////////////////////////////////////////
// SETLABEL
void runsum_product::setLabel (LABEL_TOGGLE shouldLabel)
{
   switch (shouldLabel)
   {
      case DONT_LABEL_PRODUCT :
         _sectionLine.allocate_resources(*this);
         _bitmapLblTextBox.allocate_resources(*this);
         _textLblProdBody.deallocate_resources();
         _textDontLblProdBody.allocate_resources(*this);
         _bitmapLblTextBox.move_to_back();
         _bitmapLblTextBox.move_to_front();
         break;

      case LABEL_PRODUCT :
         _sectionLine.allocate_resources(*this);
         _bitmapLblTextBox.allocate_resources(*this);
         _textLblProdBody.allocate_resources(*this);
         _textDontLblProdBody.deallocate_resources();
         _bitmapLblTextBox.move_to_back();
         _bitmapLblTextBox.move_to_front();
         break;

      case DONT_SHOW_LABEL_BOX :
         _sectionLine.deallocate_resources();
         _bitmapLblTextBox.deallocate_resources();
         _textLblProdBody.deallocate_resources();
         _textDontLblProdBody.deallocate_resources();
         break;
   }
}

void runsum_product::addReasonToButtonIfNeeded (LogicalButtonElement* btnElement,
                                                ENUMERATED_FLAG flagType,
                                                ENUMERATED_REASON_TO_SHOW reasonToShow)
{
   if (PQA.ShouldShow(flagType, reasonToShow))
      btnElement->addExtendedElement(getStringIdFromReasonCode(reasonToShow));
}

void runsum_product::initialize_status_line ()
{
   guiglobs::apheresis_status_line->set_color (MAGENTA);
   guiglobs::apheresis_status_line->set_current_text (_statLineStrId);
}


// This helper provides a central place to define the mapping of the verification
// button labels for all Run Summary Product screens
//
TEXT_STRING_ID runsum_product::
LogicalButtonCollection::getTextForLogicalButton (LOGICAL_BUTTON button_id)
{
   // !! DO NOT CHANGE THE FORMAT OF THIS BLOCK !!
   //
   // The mkProdVerifyStrs.awk script in engr_tools parses this file
   //
   switch (button_id)
   {
      case LB_COUNT_PLATELET_PRODUCT :           return textButtonFlagPltWbcs;
      case LB_COUNT_PLASMA_PRODUCT :             return textButtonFlagPlasWbcs;
      case LB_VERIFY_PLATELET_YIELD_AND_VOLUME : return textButtonFlagPltYieldVol;
      case LB_VERIFY_PLASMA_VOLUME :             return textButtonFlagPlasVol;
      case LB_VERIFY_RBC_PRODUCT :               return textButtonFlagRbcHctVol;
      case LB_VERIFY_RBC_WBCS :                  return textButtonFlagRbcWbcs;
      case LB_VERIFY_RBC_QUALITY :               return textButtonFlagRbcQuality;
      case LB_PLATELET_CONC_OUT_OF_RANGE :       return textButtonFlagPltConc;
      default :
         DataLog(log_level_gui_error) << "Unknown button id: " << button_id << endmsg;
         break;
   }
   return textEmpty;
}


void runsum_product::
LogicalButtonCollection::createLogicalButton (LOGICAL_BUTTON button_id)
{
   // lookup the button text id
   const TEXT_STRING_ID button_text = getTextForLogicalButton(button_id);

   // create a logical button element and add it to the collection
   runsum_product::LogicalButtonElement* elem =
      new runsum_product::LogicalButtonElement(button_id, button_text);

   add_table_entry(elem);
}

// This is not currently used on Trima, but it's defined for the sake of
// WIN32 tools to specify the mapping between Verify flags and Logical Button
#ifdef WIN32
struct VerifyFlagToLogicButton
{
   ENUMERATED_FLAG verifyFlag;
   LOGICAL_BUTTON  buttonId;
};

static const VerifyFlagToLogicButton TheVerifyFlagToLogicButtonMap[] =
{
   { VERIFY_PLATELET_WBCS,     LB_COUNT_PLATELET_PRODUCT },
   { VERIFY_PLATELET_YIELD,    LB_VERIFY_PLATELET_YIELD_AND_VOLUME },
   { CONC_OUT_OF_RANGE,        LB_PLATELET_CONC_OUT_OF_RANGE },
   { VERIFY_PLASMA_VOL,        LB_VERIFY_PLASMA_VOLUME },
   { VERIFY_PLASMA_WBCS,       LB_COUNT_PLASMA_PRODUCT },
   { VERIFY_RBC_VOL,           LB_VERIFY_RBC_PRODUCT }, // a.k.a. PROD1
   { VERIFY_RBC_WBCS,          LB_VERIFY_RBC_WBCS },    // a.k.a. PROD1
   { VERIFY_RBC_QUALITY,       LB_VERIFY_RBC_QUALITY }, // a.k.a. PROD1
   { VERIFY_RBC_VOL_PROD2,     LB_VERIFY_RBC_PRODUCT }, // same button as PROD1
   { VERIFY_RBC_WBCS_PROD2,    LB_VERIFY_RBC_WBCS },    // same button as PROD1
   { VERIFY_RBC_QUALITY_PROD2, LB_VERIFY_RBC_QUALITY } // same button as PROD1
};
#endif /* WIN32 */


#if (CPU==SIMNT)
void runsum_product::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == CHANGE_LABELS)
   {
      DataLog (log_level_sim_gui_side_info) << "Toggling label to " << (LABEL_TOGGLE)traverseInfo.parameters[0] << endmsg;
      setLabel((LABEL_TOGGLE)traverseInfo.parameters[0]);
   }
   else if (traverseInfo.type == CHANGE_REASONS)
   {
      DataLog (log_level_sim_gui_side_info) << "Switching reasons on runsum product screen." << endmsg;
      switchReasons();
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}

void runsum_product::switchReasons ()
{
   _ptrPhysicalButtons->setRadioBtnsUp();
   _ptrPhysicalButtons->set_cursor_to_top();

   for (PhysicalButtonElement* physBtn = _ptrPhysicalButtons->get_next_entry();
        physBtn != NULL;
        physBtn = _ptrPhysicalButtons->get_next_entry())
   {
      Button_Bitmap* btnPtr = physBtn->getPhysBtnPtr();
      LogicalButtonElement::EXTENDED_TEXT_LIST*          textlist = &_ptrPhysicalButtons->getExtendedTxt(btnPtr);

      LogicalButtonElement::EXTENDED_TEXT_LIST::iterator beginIter, endIter;
      beginIter = endIter = textlist->begin();
      advance(endIter, EXTENDED_TEXT_MAXITEMS);
      beginIter++;

      if (endIter != textlist->end())
      {
         textlist->erase(beginIter, endIter);
      }
   }
}
#endif // #if (CPU==SIMNT)

/* FORMAT HASH b0b0b19dde17c76c59e7222a489ebfcb */

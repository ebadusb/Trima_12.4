/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_PTFMOUNT

*     File name:   ptfMount.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/ptfMount.cpp 1.19 2009/07/30 20:12:17Z dslausb Exp dslausb $

*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "ptfMount.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"

#if CPU==SIMNT
#include "guiUnitTestMessageDefs.h"
#endif // #if CPU==SIMNT

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_PTFMOUNT::Screen_PTFMOUNT() :

   Base_Apheresis_Window(TEAL, GUI_SCREEN_PTF_MOUNT),

   _bmpMountTwoPtf(15, 155, BITMAP_PREPARE_TWO_PTF_PLACARD),
   _bmpMountOnePtf(100, 155, BITMAP_PREPARE_ONE_PTF_PLACARD),

   _txtMountOnePtf1(textMiscLoadOnePtfIntoBracket),
   _txtMountOnePtf2(textMiscClosePtfLineSrbc),
   // _txtMountOnePtf3(textMiscClosePtfLineSrbc),
   _txtMountTwoPtf1(textMiscLoadPtf),
   _txtMountTwoPtf2(textMiscFlossPtfLines),
   _txtMountTwoPtf3(textMiscClosePtfLines),
   _txtDontConnectAc(textMiscPtfMountDontConnectAc)
{}   // END of Screen_PTFMOUNT CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_PTFMOUNT::~Screen_PTFMOUNT()
{
   Screen_PTFMOUNT::deallocate_resources ();
}    // END of Screen_PTFMOUNT DESTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES

void Screen_PTFMOUNT::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();

   // Now link the status bar/line to this window
   link_status_bar_line();

   // Set the status bar to reflect load in progress
   guiglobs::apheresis_status_bar->set_phase_active (STATUS_BAR_LOAD);

   chooseTextAndGraphic(guiglobs::cassette_mgr->currentSetIsDrbc(),
                        Software_CDS::GetInstance().getDontConnectAcMsgAtSetLoad());

   DataLog (log_level_gui_info) << "Screen_PTFMOUNT::allocate_resources()" << endmsg;
}   // END of Screen_PTFMOUNT ALLOCATE_RESOURCES


void Screen_PTFMOUNT::chooseTextAndGraphic (bool setIsDrbc, bool showDontConnectAc)
{
   DataLog (log_level_gui_info) << "chooseTextAndGraphic(drbc=" << setIsDrbc << ")" << endmsg;

   if (setIsDrbc)
   {
      _bmpMountOnePtf.deallocate_resources();
      _txtMountOnePtf1.deallocate_resources();
      _txtMountOnePtf2.deallocate_resources();

      _bmpMountTwoPtf.allocate_resources(*this);
      _txtMountTwoPtf1.allocate_resources(*this);
      _txtMountTwoPtf2.allocate_resources(*this);
      _txtMountTwoPtf3.allocate_resources(*this);
   }
   else
   {
      _bmpMountTwoPtf.deallocate_resources();
      _txtMountTwoPtf1.deallocate_resources();
      _txtMountTwoPtf2.deallocate_resources();
      _txtMountTwoPtf3.deallocate_resources();

      _bmpMountOnePtf.allocate_resources(*this);
      _txtMountOnePtf1.allocate_resources(*this);
      _txtMountOnePtf2.allocate_resources(*this);
   }

   if (showDontConnectAc) _txtDontConnectAc.allocate_resources(*this);
   else _txtDontConnectAc.deallocate_resources();
}



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_PTFMOUNT::deallocate_resources ()
{
   // deallocate all constituent objects first
   _bmpMountOnePtf.deallocate_resources();
   _bmpMountTwoPtf.deallocate_resources();

   _txtMountOnePtf1.deallocate_resources();
   _txtMountOnePtf2.deallocate_resources();
   // _txtMountOnePtf3.deallocate_resources();
   _txtMountTwoPtf1.deallocate_resources();
   _txtMountTwoPtf2.deallocate_resources();
   _txtMountTwoPtf3.deallocate_resources();
   _txtDontConnectAc.deallocate_resources();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}    // END of Screen_PTFMOUNT DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON

void Screen_PTFMOUNT::process_cancel_button ()
{

   // Call aphwind's default abort confirm messaging
   Base_Apheresis_Window::process_cancel_button();

}    // END of Screen_PTFMOUNT PROCESS_CANCEL_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON

void Screen_PTFMOUNT::process_continue_button ()
{
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
   goto_screen(GUI_SCREEN_SYSDISP, "GUI_SCREEN_SYSDISP");
}      // END of Screen_PTFMOUNT PROCESS_CONTINUE_BUTTON

////////////////////////////////////////////////////////////////////////////////
// PROCESS_GOBACK_BUTTON

void Screen_PTFMOUNT::process_goback_button ()
{
   // Remove any residual status line messages
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

   // return to the SYSDONOR screen
   goto_screen (GUI_SCREEN_SYSCASSETTE, "GUI_SCREEN_SYSCASSETTE");
}    // END of Screen_PTFMOUNT PROCESS_GOBACK_BUTTON

void Screen_PTFMOUNT::initialize_status_line ()
{
   // Set the status to reflect the disposable load color
   guiglobs::apheresis_status_line->set_color (TEAL);

   guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, textStatLoadSet);
}


#if CPU==SIMNT // Simulator ONLY (this is for unit testing.)
void Screen_PTFMOUNT::handleTraverseMessage ()
{
   // TODO: THIS IS BROKEN, AND WILL AFFECT L10N SCREENSHOT SCRIPTS.  WE SHOULD FIX THAT AT SOME POINT.
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   // TrimaDataItemText* currentlySelectedRef = (TrimaDataItemText*)OSText::_variableDictionary.getDataItem("currentlySelectedRef");

   if (traverseInfo.type == UNIVERSAL)
   {
      PTF_LOAD_DISPLAY_TYPES ptfLoadDisplayType = (PTF_LOAD_DISPLAY_TYPES)traverseInfo.parameters[0];
      bool                   setIsPas           = false;
      bool                   setIsRas           = true;
      bool                   setIsDrbc          = true;
      bool                   showDontConnectAc  = false;

      DataLog (log_level_sim_gui_side_info) << "PTF Mount Screen got display type " << ptfLoadDisplayType << endmsg;

      switch (ptfLoadDisplayType)
      {
         case PTF_LOAD_WHITE_RAS :
            setIsDrbc = false;
            // if( currentlySelectedRef ) currentlySelectedRef->setValue("80359");
            break;
         case PTF_LOAD_WHITE_BOTH :
            setIsDrbc = false;
            setIsPas  = true;
            // if( currentlySelectedRef ) currentlySelectedRef->setValue("80579");
            break;
         case PTF_LOAD_BLACK_RAS :
            // if( currentlySelectedRef ) currentlySelectedRef->setValue("80529");
            break;
         case PTF_LOAD_WHITE_RAS_JP :
            showDontConnectAc = true;
            // setIsDrbc=false; if( currentlySelectedRef ) currentlySelectedRef->setValue("80359");
            break;
         case PTF_LOAD_WHITE_BOTH_JP :
            showDontConnectAc = true;
            setIsDrbc         = false;
            setIsPas          = true;
            // if( currentlySelectedRef ) currentlySelectedRef->setValue("80579");
            break;
         case PTF_LOAD_BLACK_RAS_JP :
            showDontConnectAc = true;
            // if( currentlySelectedRef ) currentlySelectedRef->setValue("80529");
            break;

         default :
            DataLog (log_level_sim_gui_side_error) << "Malformed message received.  Unknown PTF Load Display Type: " << ptfLoadDisplayType << endmsg;
            return;
      }

      chooseTextAndGraphic(setIsDrbc, showDontConnectAc);

      guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
      guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, textStatLoadSet);
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif

/* FORMAT HASH ca3b7040baa403196757a33047442883 */

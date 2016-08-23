/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_RBCSPLITINIT

*     File name:   rbcSplitInit.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************
*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "rbcSplitInit.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"

#if CPU==SIMNT
#include "guiUnitTestMessageDefs.h"
#endif // #if CPU==SIMNT

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_RBCSPLITINIT::Screen_RBCSPLITINIT() :

   Base_Apheresis_Window(TEAL, GUI_SCREEN_RBC_SPLIT_INIT),

   _bmpRbcSplitInit(225, 145, BITMAP_PREPARE_NON_PTF_PLACARD),
   _txtRbcSplitInit(textMiscPrepNonPtfDrbcBags),
   _a2donorMitigation(guiglobs::airToDonorMitigationEnabled())
{}   // END of Screen_RBCSPLITINIT CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_RBCSPLITINIT::~Screen_RBCSPLITINIT()
{
   Screen_RBCSPLITINIT::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES

void Screen_RBCSPLITINIT::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();

   // Now link the status bar/line to this window
   link_status_bar_line();

   // Set the status bar to reflect load in progress
   guiglobs::apheresis_status_bar->set_phase_active (STATUS_BAR_LOAD);

   // refresh cached feature
   _a2donorMitigation = guiglobs::airToDonorMitigationEnabled();

   // When ARM + AirRemoval are on, this screen is presented during the disp tests;
   // so disable the Go Back button since there's no place to go back to...
   if ( _a2donorMitigation && guiglobs::cdsData.config.Procedure.Get().key_air_removal )
   {
      btn_goback.deallocate_resources();
   }

   _bmpRbcSplitInit.allocate_resources(*this);
   _txtRbcSplitInit.allocate_resources(*this);
}   // END of Screen_RBCSPLITINIT ALLOCATE_RESOURCES

////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_RBCSPLITINIT::deallocate_resources ()
{
   // deallocate all constituent objects first
   _bmpRbcSplitInit.deallocate_resources();
   _txtRbcSplitInit.deallocate_resources();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}    // END of Screen_RBCSPLITINIT DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON

void Screen_RBCSPLITINIT::process_cancel_button ()
{

   // Call aphwind's default abort confirm messaging
   Base_Apheresis_Window::process_cancel_button();

}    // END of Screen_RBCSPLITINIT PROCESS_CANCEL_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON

void Screen_RBCSPLITINIT::process_continue_button ()
{
   // Disable all the standard buttons on this screen
   disable_standard_buttons();

   // Tell the procedure that the disposable is clamped, then wait for
   // Procedure to change states to disposable test which will cause
   // the disptest screen to replace this.

   if (!_a2donorMitigation)
   {
      DataLog (log_level_gui_info) << "Sending ClampedForTestingMsg message" << endmsg;
      ClampedForTestingMsg clamped(MessageBase::SEND_LOCAL);
      clamped.send(0);
   }
   else // ARM is ON
   {
      // If air removal is off, clamping is done
      if ( !guiglobs::cdsData.config.Procedure.Get().key_air_removal )
      {
         DataLog (log_level_gui_info) << "AirEvac off: Sending ClampedForTestingMsg message" << endmsg;
         ClampedForTestingMsg clamped(MessageBase::SEND_GLOBAL);
         clamped.send(0);
      }
      else
      {
         DataLog (log_level_gui_info) << "ARM on: Sending FinishedClampFromARMMsg message" << endmsg;
         FinishedClampFromARMMsg FinishShowClamp(MessageBase::SEND_LOCAL);
         FinishShowClamp.send(0);

         goto_screen(GUI_SCREEN_SYSTEST, "GUI_SCREEN_SYSTEST");
      }
   }

}      // END of Screen_RBCSPLITINIT PROCESS_CONTINUE_BUTTON

////////////////////////////////////////////////////////////////////////////////
// PROCESS_GOBACK_BUTTON

void Screen_RBCSPLITINIT::process_goback_button ()
{
   if (_a2donorMitigation) // here, air removal is off; tests haven't begun yet
   {
      // Disable all the standard buttons on this screen
      disable_standard_buttons();

      // Ask Procedure to raise the cassette, before going to previous screen
      GUIRequestRaiseCassetteMsg raiserequest(MessageBase::SEND_LOCAL);
      raiserequest.send(1);

      // Set the status line to indicate that the disposable is raising
      guiglobs::apheresis_status_line->set_current_status_line_text_by_type
         (ATTENTION_STATUS_LINE, textStatDispRaising);
   }
   else
   {
      goto_screen(GUI_SCREEN_SYSCLAMP, "GUI_SCREEN_SYSCLAMP");
   }
}    // END of Screen_RBCSPLITINIT PROCESS_GOBACK_BUTTON

void Screen_RBCSPLITINIT::initialize_status_line ()
{
   // Set the status to reflect the disposable load color
   guiglobs::apheresis_status_line->set_color (TEAL);
}

/* FORMAT HASH dc9b5563156d46358b1456eb9a76311b */

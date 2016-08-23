/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_SYSCLAMP

*     File name:   sysclamp.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/sysclamp.cpp 1.34 2009/06/18 21:28:06Z dslausb Exp dslausb $

***************************************************************************/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "sysclamp.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "CatalogChecker.h"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "software_cds.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_SYSCLAMP::Screen_SYSCLAMP()
   : Base_Apheresis_Window(TEAL, GUI_SCREEN_SYSCLAMP),
     _bmpDispClamp (50, 150, BITMAP_PLACARD_DISP_CLAMP),
     _txtCloseClamps(textMiscInstructionDispCloseClamps),
     _a2donorMitigation(true) // turn on and prove it's off.
{}


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_SYSCLAMP::~Screen_SYSCLAMP()
{
   Screen_SYSCLAMP::deallocate_resources();
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()

void Screen_SYSCLAMP::allocate_resources (const char* allocation_parameter)
{
   _a2donorMitigation = guiglobs::airToDonorMitigationEnabled();

   const bool airRemoval = guiglobs::cdsData.config.Procedure.Get().key_air_removal;
   const bool japanWF    = Software_CDS::isJapaneseWorkflow();

   DataLog (log_level_gui_info) << "ARM=" << _a2donorMitigation
                                << " AirEvac=" << airRemoval
                                << " JapanWF=" << japanWF
                                << endmsg;

#if CPU==SIMNT
   // remove this compiler switch when this variable becomes
   // configurable
   _a2donorMitigation = false;
#endif

   // When ARM is OFF for non-Japanese case, we display clamping instructions here.
   //
   if ( !_a2donorMitigation && !japanWF )
   {
      // allocate parent's resources first
      Base_Apheresis_Window::allocate_resources (allocation_parameter);

      // now deallocate "standard" buttons that are not needed
      btn_lowleft.deallocate_resources ();

      // Now link the status bar/line to this window
      link_status_bar_line();

      // Set the status bar to reflect load in progress
      guiglobs::apheresis_status_bar->set_phase_active (STATUS_BAR_LOAD);

      // Clamp the donor and sample bag lines
      _txtCloseClamps.allocate_resources(*this);
      _bmpDispClamp.allocate_resources(*this);
   }
   else // ARM is ON or JapanWF
   {
      // If air removal is on, defer any clamping until disposable test is done.
      // Otherwise, check for other disposable-specific clamping instructions.
      //
      if ( !airRemoval && guiglobs::shouldShowClampPlasmaAirBag() )
      {
         DataLog (log_level_gui_info) << "AirEvac off: shouldShowClampPlasmaAirBag" << endmsg;
         goto_screen(GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG, "GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG");
      }
      else if ( !airRemoval && guiglobs::shouldShowClampPas() )
      {
         DataLog (log_level_gui_info) << "AirEvac off: shouldShowClampPas" << endmsg;
         goto_screen(GUI_SCREEN_PAS_INIT_SPLIT, "GUI_SCREEN_PAS_INIT_SPLIT");
      }
      else if ( !airRemoval && guiglobs::shouldShowRbcSplitInit() )
      {
         DataLog (log_level_gui_info) << "AirEvac off: shouldShowRbcSplitInit" << endmsg;
         goto_screen(GUI_SCREEN_RBC_SPLIT_INIT, "GUI_SCREEN_RBC_SPLIT_INIT");
      }
      else
      {
         // Disposable test can proceed
         DataLog (log_level_gui_info) << "sendClampsReadyMsg" << endmsg;
         sendClampsReadyMsg();
      }
   }

}    // END of Screen_SYSCLAMP ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_SYSCLAMP::deallocate_resources ()
{
   // Note that nothing gets allocated when ARM is on
   if ( _bmpDispClamp.allocated() )
   {
      // deallocate all constituent objects first
      _bmpDispClamp.deallocate_resources();
      _txtCloseClamps.deallocate_resources();

      // Now unparent the status bar/line from this window
      unlink_status_bar_line();

      // deallocate parent's resources
      Base_Apheresis_Window::deallocate_resources();
   }
}


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON

void Screen_SYSCLAMP::process_cancel_button ()
{
   // Call aphwind's default abort confirm messaging
   Base_Apheresis_Window::process_cancel_button();
}


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON

void Screen_SYSCLAMP::process_continue_button ()
{
   // When ARM is OFF, handle any final disposable-specific instructions
   //
   if ( !_a2donorMitigation && guiglobs::shouldShowClampPlasmaAirBag() )
   {
      DataLog (log_level_gui_info) << "ARM off: shouldShowClampPlasmaAirBag" << endmsg;
      goto_screen(GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG, "GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG");
   }
   else if ( !_a2donorMitigation && guiglobs::shouldShowClampPas() )
   {
      DataLog (log_level_gui_info) << "ARM off: shouldShowClampPas" << endmsg;
      goto_screen(GUI_SCREEN_PAS_INIT_SPLIT, "GUI_SCREEN_PAS_INIT_SPLIT");
   }
   else if ( !_a2donorMitigation && guiglobs::shouldShowRbcSplitInit() )
   {
      DataLog (log_level_gui_info) << "ARM off: shouldShowRbcSplitInit" << endmsg;
      goto_screen(GUI_SCREEN_RBC_SPLIT_INIT, "GUI_SCREEN_RBC_SPLIT_INIT");
   }
   else
   {
      // Disposable test can proceed
      DataLog (log_level_gui_info) << "sendClampsReadyMsg" << endmsg;
      sendClampsReadyMsg();
   }
}

////////////////////////////////////////////////////////////////////////////////
// PROCESS_GOBACK_BUTTON

void Screen_SYSCLAMP::process_goback_button ()
{
   // Disable all the standard buttons on this screen
   disable_standard_buttons();

   // Ask Procedure to raise the cassette, before going to previous screen
   GUIRequestRaiseCassetteMsg raiserequest(MessageBase::SEND_LOCAL);
   raiserequest.send(0);

   // Set the status line to indicate that the disposable is raising
   guiglobs::apheresis_status_line->set_current_status_line_text_by_type
      (ATTENTION_STATUS_LINE, textStatDispRaising);
}     // END of Screen_SYSCLAMP PROCESS_GOBACK_BUTTON


////////////////////////////////////////////////////////////////////////////////
// This function disables standard buttons and informs proc that all necessary
// clamping has been done.  It will likely trigger a state change that will
// transition away from this screen.
//
void Screen_SYSCLAMP::sendClampsReadyMsg ()
{
   // Disable all the standard buttons on this screen
   //
   if (!_a2donorMitigation)
      disable_standard_buttons();

   // Tell the procedure that the disposable is clamped, then wait for
   //  Procedure to change states to disposable test which will cause
   //  the disptest screen to replace this.
   ClampedForTestingMsg clamped(MessageBase::SEND_GLOBAL);
   clamped.send(0);
}

void Screen_SYSCLAMP::initialize_status_line ()
{
   // Set the status line to reflect the disposable load color
   guiglobs::apheresis_status_line->set_color (TEAL);

   // Remove any residual messages
   guiglobs::apheresis_status_line->set_current_text (textEmpty);
}

#if (CPU==SIMNT)

void Screen_SYSCLAMP::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == UNIVERSAL)
   {
      SYSCLAMP_DISPLAY_TYPE sysClampDisplayType = (SYSCLAMP_DISPLAY_TYPE)traverseInfo.parameters[0];

      bool                  jp                  = false;
      bool                  air                 = false;

      DataLog (log_level_sim_gui_side_info) << "SysClamp screen got display type " << sysClampDisplayType << endmsg;

      switch (sysClampDisplayType)
      {
         case SYSCLAMP_KEY_AIR_REMOVAL : air    = true; break;
         case SYSCLAMP_NO_AIR_REMOVAL : break;
         case SYSCLAMP_KEY_AIR_REMOVAL_JP : air = true; jp = true; break;
         case SYSCLAMP_NO_AIR_REMOVAL_JP : jp   = true; break;

         default :
            DataLog (log_level_sim_gui_side_error) << "Malformed message received.  Unknown Sys Clamp Display Type: " << sysClampDisplayType << endmsg;
            return;
      }

      _txtCloseClamps.allocate_resources(*this);
      _bmpDispClamp.allocate_resources(*this);
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif // #if (CPU==SIMNT)

/* FORMAT HASH e24cd48107cbd793db1118c500500795 */

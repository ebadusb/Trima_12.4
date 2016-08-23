/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_SYSDISP

*     File name:   sysdisp.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "sysdisp.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "CatalogChecker.h"

#include "predict_manager.hpp"

#if CPU==SIMNT
#include "guiUnitTestMessageDefs.h"
#endif // #if CPU==SIMNT



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_SYSDISP::Screen_SYSDISP() :

   Base_Apheresis_Window(TEAL, GUI_SCREEN_SYSDISP),

   _screenState        (LOAD_SET),
   _showClampPasLine   (false),
   _japanWF            (false),
   _a2donorMitigation  (true),

   _bmpDispSetup (PLAC_DISPSYS_X,
                  PLAC_DISPSYS_Y,
                  BITMAP_PLACARD_DISP_SETUP),

   _bmpClampPasLine(50, 170, BITMAP_CLAMP_PLT_AAS_LINE),
   _bmpDispClamp   (50, 150, BITMAP_PLACARD_DISP_CLAMP),
   _bmpDispClampJp (50, 150, BITMAP_PLACARD_DISP_CLAMP_JP),

   _txtHang   (textMiscInstructionHang),
   _txtInsert (textMiscInstructionInsert),
   _txtLoad   (textMiscInstructionLoad),
   _txtClose  (textMiscInstructionClose),
   _txtClampPasLine   (textMiscClampPasLine),
   _txtDontConnectAc  (textMiscSysDispDontConnectAc),
   _txtCloseClamp     (textMiscInstructionDispCloseClamps),

   _msgDispLowered(),
   _msgDispLowerFailure(),
   _dispLowerTimeout(DISP_LOWERED_MSG_PERIOD, Callback<Screen_SYSDISP>(this, &Screen_SYSDISP::remove_lowered_msg), TimerMessage::DISARMED),
   _msgProcedureStatusUpdate(),
   _dispLowered(false)
{}


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_SYSDISP::~Screen_SYSDISP()
{
   // deallocate_resources is virtual, so be sure to call the right one!
   Screen_SYSDISP::deallocate_resources();
}


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES

void Screen_SYSDISP::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);


   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();

   _msgProcedureStatusUpdate.init(Callback<Screen_SYSDISP>(this, &Screen_SYSDISP::cassette_status_message), MessageBase::SNDRCV_RECEIVE_ONLY);

   // This stuff may have already gotten done, but do it again here anyway.
   // Set the status bar to reflect load in progress
   guiglobs::apheresis_status_bar->set_phase_active (STATUS_BAR_LOAD);

   /////////////////////////////////

   // Now link the status bar/line to this window
   link_status_bar_line();

   _showClampPasLine = guiglobs::cassette_mgr->currentSetIsPas();

   // refresh cached features
   _a2donorMitigation = Software_CDS::GetInstance().getFeature(AiroutMitigation);
   _japanWF           = Software_CDS::isJapaneseWorkflow();

   // then allocate constituent objects with this window as parent widget
   //  layer allocation from Z-back to Z-front.
   chooseTextAndGraphic(LOAD_SET, _japanWF);

   // Reset the disposable lowered message flag
   _dispLowered = false;

}   // END of Screen_SYSDISP ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_SYSDISP::deallocate_resources ()
{
   // deallocate all constituent objects first
   _txtHang.deallocate_resources();
   _txtInsert.deallocate_resources();
   _txtLoad.deallocate_resources();
   _txtClose.deallocate_resources();
   _txtCloseClamp.deallocate_resources();
   _txtClampPasLine.deallocate_resources();
   _txtDontConnectAc.deallocate_resources();

   _bmpDispSetup.deallocate_resources();
   _bmpDispClamp.deallocate_resources();
   _bmpDispClampJp.deallocate_resources();
   _bmpClampPasLine.deallocate_resources();

   // Now unparent the status bar/line from this window
   if ( (guiglobs::screen_status.get_current_screen() == GUI_SCREEN_SYSDISP) ||
        (guiglobs::screen_status.get_current_screen() == GUI_SCREEN_SYSCLAMP) )
      guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();

   // Unsubscribe from screen event notifications
   _msgDispLowered.deregisterMsg();
   _msgDispLowerFailure.deregisterMsg();
   _msgProcedureStatusUpdate.deregisterMsg();
   _dispLowerTimeout.armTimer(TimerMessage::DISARMED);
}

////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON

void Screen_SYSDISP::process_cancel_button ()
{

   // Call aphwind's default abort confirm messaging
   Base_Apheresis_Window::process_cancel_button();

}    // END of Screen_SYSDISP PROCESS_CANCEL_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON

void Screen_SYSDISP::process_continue_button ()
{
   // Note: "No break" statements are by design -> fallthrough is intended.
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

   switch (_screenState)
   {
      case LOAD_SET :
         DataLog (log_level_gui_info) << "Sysdisp: continue pressed for load set screen" << endmsg;

         if (_a2donorMitigation || _japanWF)
         {
            chooseTextAndGraphic(CLAMP_DLINE, _japanWF);
            return;
         }
      // No break.  Fallthrough is intended.

      case CLAMP_DLINE :
         // Check before logging, because this could be fallthrough.
         if (_screenState==CLAMP_DLINE)
            DataLog (log_level_gui_info) << "Sysdisp: continue pressed for clamp donor line screen" << endmsg;

         if (_showClampPasLine)
         {
            chooseTextAndGraphic(CLAMP_PAS, _japanWF);
            return;
         }
      // No break.  Fallthrough is intended.

      case CLAMP_PAS :
         // Check before logging, because this could be fallthrough.
         if (_screenState==CLAMP_PAS)
            DataLog (log_level_gui_info) << "Sysdisp: continue pressed for clamp PAS line screen" << endmsg;

         chooseTextAndGraphic(LOAD_SET, _japanWF);

         guiglobs::cassette_mgr->sendFunctBits();

         // Disable all the standard buttons on this screen
         disable_standard_buttons();
         btn_cancel.deallocate_resources();
         btn_continue.deallocate_resources();
         btn_goback.deallocate_resources();

         // Register a handler for receiving "Disposable Lowered" msg
         _msgDispLowered.init(Callback<Screen_SYSDISP>(this, &Screen_SYSDISP::disposable_lowered_received),
                              MessageBase::SNDRCV_RECEIVE_ONLY);

         // Register a handler for receiving "Failed Disposable Lowered" msg
         _msgDispLowerFailure.init(Callback<Screen_SYSDISP>(this, &Screen_SYSDISP::disposable_lower_fail_received),
                                   MessageBase::SNDRCV_RECEIVE_ONLY);

         // Tell the Procedure that the disposable is loaded
         ConfirmDisposableLoadedMsg confirm(MessageBase::SEND_GLOBAL);
         confirm.send(0);
         break;
   }
}      // END of Screen_SYSDISP PROCESS_CONTINUE_BUTTON

////////////////////////////////////////////////////////////////////////////////
// PROCESS_GOBACK_BUTTON

void Screen_SYSDISP::process_goback_button ()
{
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

   switch (_screenState)
   {
      case CLAMP_PAS :
         if (_a2donorMitigation || _japanWF)
         {
            chooseTextAndGraphic(CLAMP_DLINE, _japanWF);
            return;
         }
      // No break.  Fallthrough is intended.

      case CLAMP_DLINE :
         chooseTextAndGraphic(LOAD_SET, _japanWF);
         break;

      case LOAD_SET :
         guiglobs::apheresis_status_line->set_current_text (textEmpty);
         guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

         if (guiglobs::cassette_mgr->currentSetIsRas())
            goto_screen (GUI_SCREEN_PTF_MOUNT, "GUI_SCREEN_PTF_MOUNT");
         else
            goto_screen (GUI_SCREEN_SYSCASSETTE, "GUI_SCREEN_SYSCASSETTE");
         break;
   }
}    // END of Screen_SYSDISP PROCESS_GOBACK_BUTTON



////////////////////////////////////////////////////////////////////////////////
// SCREEN_NOTIFY_HANDLER

void Screen_SYSDISP::disposable_lowered_received ()
{
   // Qualify the processing based on the mutually exclusive cassette status
   if (_dispLowered == false)
   {
      // Set the flag so that failed processing (below) can't happen
      _dispLowered = true;

      _dispLowerTimeout.armTimer(TimerMessage::ARMED);
   }

}

void Screen_SYSDISP::disposable_lower_fail_received ()
{
   // Qualify the processing based on the mutually exclusive cassette status
   if (_dispLowered == false)
   {
      // Set the flag so that successful processing (above) can't happen
      _dispLowered = true;

      // clear the currently selected cassette
      guiglobs::cassette_mgr->clearCurrent();

      // Go back to the SYSDONOR screen
      goto_screen (GUI_SCREEN_SYSDONOR, "GUI_SCREEN_SYSDONOR");
   }

}



////////////////////////////////////////////////////////////////////////////////
// SCREEN_TIMER_HANDLER

void Screen_SYSDISP::remove_lowered_msg ()
{
   _dispLowerTimeout.armTimer(TimerMessage::DISARMED);

   // Display the SYSCLAMP screen once the disposable has completed
   // displaying the "disposable lowering message".
   goto_screen (GUI_SCREEN_SYSCLAMP, "GUI_SCREEN_SYSCLAMP");
}

////////////////////////////////////////////////////////////////////////////////
// CASSETTE_STATUS_MESSAGE

void Screen_SYSDISP::cassette_status_message ()
{
   displayCassetteStatus((CASSETTE_POSITION_STATUS)_msgProcedureStatusUpdate.getData(),
                         guiglobs::cdsData.config.Procedure.Get().key_air_removal);
}

void Screen_SYSDISP::displayCassetteStatus (CASSETTE_POSITION_STATUS status, bool airRemovalOn)
{
   switch (status)
   {
      case CASSETTE_LOWERING :
         guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, textStatDispLowering);
         break;

      case CASSETTE_LOWERED :
         guiglobs::apheresis_status_line->set_current_text(textStatDispLowered);
         guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
         break;

      case CASSETTE_RAISING :
         guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, textStatDispRaising);
         break;

      case CASSETTE_EVACUATING :
         if (airRemovalOn)
         {
            guiglobs::apheresis_status_line->set_current_text(textStatEvac);
            guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
         }
         else
         {
            DataLog (log_level_gui_info) << "Cassette status message " << CASSETTE_EVACUATING
                                         << " ignored because air removal is turned off." << endmsg;
         }
         break;

      case CASSETTE_RAISED :
      default :
         guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
         guiglobs::apheresis_status_line->set_current_text(textEmpty);
         break;
   }
}

void Screen_SYSDISP::chooseTextAndGraphic (SCREEN_STATE newScreenState, bool jpDispSet)
{
   _screenState = newScreenState;

   switch (newScreenState)
   {
      case LOAD_SET :

         DataLog (log_level_gui_info) << "Displaying load set screen" << endmsg;

         // Deallocate unwanted text
         _txtClampPasLine.deallocate_resources();
         _txtCloseClamp.deallocate_resources();

         // Deallocate unwanted graphics
         _bmpClampPasLine.deallocate_resources();

         // Allocate desired graphics
         _bmpDispSetup.allocate_resources(*this);

         // Allocate desired text
         _txtHang.allocate_resources(*this);
         _txtInsert.allocate_resources(*this);
         _txtLoad.allocate_resources(*this);
         _txtClose.allocate_resources(*this);

         break;

      case CLAMP_DLINE :

         DataLog (log_level_gui_info) << "Displaying Clamp donorline screen" << endmsg;

         _txtClampPasLine.deallocate_resources();
         _txtHang.deallocate_resources();
         _txtInsert.deallocate_resources();
         _txtLoad.deallocate_resources();
         _txtClose.deallocate_resources();

         _bmpDispSetup.deallocate_resources();
         _bmpClampPasLine.deallocate_resources();

         if (jpDispSet)
         {
            _bmpDispClamp.deallocate_resources();

            _bmpDispClampJp.allocate_resources(*this);
            _txtCloseClamp.set_string_id(textMiscInstructionDispCloseClampsJp);
         }
         else
         {
            _bmpDispClampJp.deallocate_resources();

            _bmpDispClamp.allocate_resources(*this);
            _txtCloseClamp.set_string_id(textMiscInstructionDispCloseClamps);
         }

         _txtCloseClamp.allocate_resources(*this);
         break;

      case CLAMP_PAS :

         DataLog (log_level_gui_info) << "Displaying clamp PAS screen" << endmsg;

         // Deallocate unwanted text
         _txtCloseClamp.deallocate_resources();
         _txtHang.deallocate_resources();
         _txtInsert.deallocate_resources();
         _txtLoad.deallocate_resources();
         _txtClose.deallocate_resources();

         // Deallocate unwanted graphics
         _bmpDispClamp.deallocate_resources();
         _bmpDispClampJp.deallocate_resources();
         _bmpDispSetup.deallocate_resources();

         // Allocate desired text and graphics
         _bmpClampPasLine.allocate_resources(*this);
         _txtClampPasLine.allocate_resources(*this);
         break;
   }

   if (jpDispSet)
      _txtDontConnectAc.allocate_resources(*this);
   else
      _txtDontConnectAc.deallocate_resources();
}

void Screen_SYSDISP::initialize_status_line ()
{
   // Set the status to reflect the disposable load color
   guiglobs::apheresis_status_line->set_color (TEAL);

   // Remove any residual messages
   guiglobs::apheresis_status_line->set_current_text (textEmpty);

   guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, textStatLoadSet);
}


#if CPU==SIMNT
void Screen_SYSDISP::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == UNIVERSAL)
   {
      SYSDISP_DISPLAY_TYPES    sysdispDisplayType = (SYSDISP_DISPLAY_TYPES)traverseInfo.parameters[0];

      CASSETTE_POSITION_STATUS status             = (CASSETTE_POSITION_STATUS)-1;

      bool                     showAStatLine      = true;
      SCREEN_STATE             screenState        = LOAD_SET;
      bool                     japan              = false;

      switch (sysdispDisplayType)
      {
         case SYSDISP_CASSETTE_LOWERING :
            status = CASSETTE_LOWERING;
            break;
         case SYSDISP_CASSETTE_LOWERED :
            status = CASSETTE_LOWERED;
            // needs extra removal of 'attention status'
            guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
            break;
         case SYSDISP_CASSETTE_EVACUATING :
            status = CASSETTE_EVACUATING;
            break;
         case SYSDISP_CASSETTE_RAISING :
            status = CASSETTE_RAISING;
            break;
         case SYSDISP_CASSETTE_RAISED :
            status = CASSETTE_RAISED;
            break;
         case SYSDISP_CASSETTE_DONT_CONNECT_AC :
            japan = true;
            break;
   #if 0 // TODO: CLAMP_SAMPLE_BAG is obsolete
         case SYSDISP_CLAMP_SAMPLE_BAG :
            screenState   = CLAMP_SAMPLE_BAG;
            showAStatLine = false;
            break;
         case SYSDISP_CLAMP_SAMPLE_BAG_JP :
            screenState   = CLAMP_SAMPLE_BAG;
            showAStatLine = false;
            japan         = true;
            break;
  #endif // TODO: CLAMP_SAMPLE_BAG is obsolete
         case SYSDISP_CLAMP_PAS :
            screenState   = CLAMP_PAS;
            showAStatLine = false;
            break;
         case SYSDISP_CLAMP_PAS_JP :
            screenState   = CLAMP_PAS;
            showAStatLine = false;
            japan         = true;
            break;

         default :
            DataLog (log_level_sim_gui_side_error) << "Malformed message received.  Unknown Sysdisp Display Type: " << sysdispDisplayType << endmsg;
            return;
      }

      DataLog (log_level_sim_gui_side_info) << "sysdisp chooseTextAndGraphic(state="
                                            << screenState << ";jpDispSet=" << japan << ";showDontConAc=" << japan << ");" << endmsg;

      chooseTextAndGraphic(screenState, japan);

      if (showAStatLine &&
          status >= 0)
      {
         DataLog (log_level_sim_gui_side_info)
            << "sysdisp displayCassetteStatus(status=" << status << ");" << endmsg;

         displayCassetteStatus(status);
      }
      else guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif

/* FORMAT HASH 30d1daa4c91fa0b9fb1fb5160f213c0e */

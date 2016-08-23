/*
* Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
*
* TITLE:      aasClampChannel.cpp
*
* $Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/aasClampChannel.cpp 1.20 2009/04/22 20:26:04Z dslausb Exp dslausb $
*
*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "aasClampChannel.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"

// EXTERNAL REFERENCES
#include "guiglobs.hpp"

#define CHANNELCLAMP_PAS_REMINDER_DELAY 600000 // 600.0 seconds - 10 minutes

////////////////////////////////////////////////////////////////////////////////

Screen_AASCLAMPCHANNEL::Screen_AASCLAMPCHANNEL() :

   Base_Apheresis_Window(MAGENTA, GUI_SCREEN_AAS_CLAMP_CHANNEL),

   _txtClampChannelUnclampCrossover(textMiscUnclampCrossover),
   _txtClampChannelClampChannelLines(textMiscClampChannelLines),
   _txtClampChannelClampChannelLine(textMiscClampChannelLine),
   _txtClampChannelClampChannelLinesOnly(textMiscClampChannelLinesOnly),

   _bmpClampChannelWhitePas(143, 157, BITMAP_AAS_CLAMP_CHANNEL_WHITE_PAS),
   _bmpClampChannelWhiteRas(10, 145, BITMAP_AAS_CLAMP_CHANNEL_WHITE_RAS),
   _bmpClampChannelWhiteBoth(10, 145, BITMAP_AAS_CLAMP_CHANNEL_WHITE_BOTH),
   _bmpClampChannelWhiteBlack(10, 145, BITMAP_AAS_CLAMP_CHANNEL_BLACK),
   _timerReminder(CHANNELCLAMP_PAS_REMINDER_DELAY, Callback<Screen_AASCLAMPCHANNEL>(this, &Screen_AASCLAMPCHANNEL::reminder_timer_handler), TimerMessage::DISARMED)


{}


////////////////////////////////////////////////////////////////////////////////
Screen_AASCLAMPCHANNEL::~Screen_AASCLAMPCHANNEL()
{
   Screen_AASCLAMPCHANNEL::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////
void Screen_AASCLAMPCHANNEL::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();
   btn_goback.deallocate_resources();

   chooseTextAndGraphic();

   // Now link the status bar/line to this window
   link_status_bar_line();

   btn_cancel.set_string_id(textButtonEndAddition);

   // Start the reminder timer
   _timerReminder.armTimer(TimerMessage::ARMED);


}

void Screen_AASCLAMPCHANNEL::deallocate_text_and_graphics ()
{
   _txtClampChannelUnclampCrossover.deallocate_resources();
   _txtClampChannelClampChannelLines.deallocate_resources();
   _txtClampChannelClampChannelLine.deallocate_resources();
   _txtClampChannelClampChannelLinesOnly.deallocate_resources();

   _bmpClampChannelWhitePas.deallocate_resources();
   _bmpClampChannelWhiteRas.deallocate_resources();
   _bmpClampChannelWhiteBoth.deallocate_resources();
   _bmpClampChannelWhiteBlack.deallocate_resources();

   // Shut off the reminder timer
   _timerReminder.armTimer(TimerMessage::DISARMED);

   // Stop the audible alert
   guiglobs::button_audio_feedback.create_feedback_sound (ATTENTION1_SOUND, SOUND_CLEAR);


}


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_AASCLAMPCHANNEL::deallocate_resources ()
{
   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   deallocate_text_and_graphics();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////

void Screen_AASCLAMPCHANNEL::process_cancel_button ()
{
   goto_child_screen (GUI_SCREEN_ABORTCNF, ABORT_TYPE_ADDITIVE_SOLUTIONS);
}


////////////////////////////////////////////////////////////////////////////////

void Screen_AASCLAMPCHANNEL::process_continue_button ()
{
   if (guiglobs::aasRunInfo.isPasProcedure())
   {
      goto_screen(GUI_SCREEN_AAS_CHANNEL_TEST, "GUI_SCREEN_AAS_CHANNEL_TEST");
   }
   else
   {
      DataLog (log_level_gui_info) << "Channel test unnecessary.  Go to connect RAS." << endmsg;

      // Tell proc that we clamped the channel and we're ready to go.
      MSSSetupMsg _setupMeteredStorage(MessageBase::SEND_LOCAL);
      _setupMeteredStorage.send(0);

      goto_screen(GUI_SCREEN_AAS_CONNECT_RAS, "GUI_SCREEN_AAS_CONNECT_RAS");
   }
}   // END of Screen_MSSCLAMP PROCESS_CONTINUE_BUTTON

////////////////////////////////////////////////////////////////////////////////

void Screen_AASCLAMPCHANNEL::chooseTextAndGraphic ()
{
   chooseTextAndGraphic(
      guiglobs::cassette_mgr->currentSetIsPas(),
      guiglobs::cassette_mgr->currentSetIsRas(),
      guiglobs::cassette_mgr->currentSetIsWhiteStamp());
}

void Screen_AASCLAMPCHANNEL::chooseTextAndGraphic (bool setIsPasEquipped, bool setIsRasEquipped, bool setIsWhiteStamp)
{
   deallocate_text_and_graphics();

   if (setIsWhiteStamp)
   {
      if (setIsPasEquipped)
      {
         if (setIsRasEquipped)
         {
            DataLog(log_level_gui_info) << "Set is white-stamp RAS/PAS. Show white stamp set with three clamps." << endmsg;
            _txtClampChannelUnclampCrossover.allocate_resources(*this);
            _txtClampChannelClampChannelLines.allocate_resources(*this);
            _bmpClampChannelWhiteBoth.allocate_resources(*this);
         }
         else
         {
            DataLog(log_level_gui_info) << "Set is white-stamp PAS-only. Show white stamp set with three clamps." << endmsg;
            _txtClampChannelClampChannelLinesOnly.allocate_resources(*this);
            _bmpClampChannelWhitePas.allocate_resources(*this);
         }
      }
      else if (setIsRasEquipped)
      {
         DataLog(log_level_gui_info) << "Set is white-stamp RAS-only. Show white stamp set with one clamp." << endmsg;
         _txtClampChannelUnclampCrossover.allocate_resources(*this);
         _txtClampChannelClampChannelLine.allocate_resources(*this);
         _bmpClampChannelWhiteRas.allocate_resources(*this);
      }
   }
   else
   {
      DataLog(log_level_gui_info) << "Set is black-stamp RAS-only. Show black stamp set with one clamp." << endmsg;
      _txtClampChannelUnclampCrossover.allocate_resources(*this);
      _txtClampChannelClampChannelLine.allocate_resources(*this);
      _bmpClampChannelWhiteBlack.allocate_resources(*this);
   }
}

void Screen_AASCLAMPCHANNEL::initialize_status_line ()
{
   guiglobs::apheresis_status_line->set_current_text (textStatAasClampChannel);
}


////////////////////////////////////////////////////////////////////////////////
// REMINDER_TIMER_HANDLER

void Screen_AASCLAMPCHANNEL::reminder_timer_handler ()

{
   // Sound the reminder
   guiglobs::button_audio_feedback.create_feedback_sound (ATTENTION1_SOUND, SOUND_ONCE);

   // Start the timer again
   _timerReminder.armTimer(TimerMessage::ARMED);

}   // END of REMINDER_TIMER_HANDLER




#if CPU==SIMNT
void Screen_AASCLAMPCHANNEL::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == UNIVERSAL)
   {
      bool setIsWhiteStamp                  = true;
      bool setIsPasEquipped                 = true;
      bool setIsRasEquipped                 = true;
      CHANNEL_CLAMP_DISPLAY_TYPES clampType = (CHANNEL_CLAMP_DISPLAY_TYPES)traverseInfo.parameters[0];

      switch (clampType)
      {
         case CHANNEL_CLAMP_WHITE_RAS :
            DataLog (log_level_sim_gui_side_info) << "Recieved SIMNT screen change message for white-stamp RAS set." << endmsg;
            setIsPasEquipped = false;
            break;

         case CHANNEL_CLAMP_WHITE_PAS :
            DataLog (log_level_sim_gui_side_info) << "Recieved SIMNT screen change message for white-stamp PAS set." << endmsg;
            setIsRasEquipped = false;
            break;

         case CHANNEL_CLAMP_WHITE_BOTH :
            DataLog (log_level_sim_gui_side_info) << "Recieved SIMNT screen change message for white-stamp PAS and RAS set." << endmsg;
            break;

         case CHANNEL_CLAMP_BLACK :
            setIsWhiteStamp  = false;
            setIsPasEquipped = false;
            DataLog (log_level_sim_gui_side_info) << "Recieved SIMNT screen change message for black-stamp RAS set." << endmsg;
            break;

         default :
            DataLog (log_level_sim_gui_side_error) << "Malformed message received.  Unknown Channel Clamp Display Type: " << clampType << endmsg;
            return;
      }

      chooseTextAndGraphic(setIsPasEquipped, setIsRasEquipped, setIsWhiteStamp);
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif

/* FORMAT HASH f01c469d7b4df450d5011844fdc99fd8 */

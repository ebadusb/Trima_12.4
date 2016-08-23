/*
* Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
*
* TITLE:      pasSplit.cpp
*
* $Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/pasSplit.cpp 1.5 2008/07/09 23:51:15Z dslausb Exp $
*
*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "pasSplit.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"

#define SPLIT_PAS_REMINDER_DELAY 600000 // 600.0 seconds - 10 minutes

////////////////////////////////////////////////////////////////////////////////

Screen_PAS_SPLIT::Screen_PAS_SPLIT()
   : // init members base classes
     Base_Apheresis_Window(MAGENTA, GUI_SCREEN_AAS_PAS_SPLIT),
     _txtSplitInstructions(textMiscPltPasSplitInstruct),
     _pltSplit(375, 145, BITMAP_MSS_PLT_SPLIT),
     _timerReminder(SPLIT_PAS_REMINDER_DELAY, Callback<Screen_PAS_SPLIT>(this, &Screen_PAS_SPLIT::reminder_timer_handler), TimerMessage::DISARMED)

{}


////////////////////////////////////////////////////////////////////////////////
Screen_PAS_SPLIT::~Screen_PAS_SPLIT()
{
   Screen_PAS_SPLIT::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////
void Screen_PAS_SPLIT::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();
   btn_goback.deallocate_resources();
   btn_cancel.set_string_id(textButtonEndAddition);
   btn_continue.allocate_resources(*this);       // put continue button back

   // Now link the status bar/line to this window
   link_status_bar_line();

   // allocate the text holders  (NULL right now)
   _txtSplitInstructions.allocate_resources(*this);
   _pltSplit.allocate_resources(*this);

   // Start the reminder timer
   _timerReminder.armTimer(TimerMessage::ARMED);


}


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_PAS_SPLIT::deallocate_resources ()
{
   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   _txtSplitInstructions.deallocate_resources();
   _pltSplit.deallocate_resources();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();

   // Shut off the reminder timer
   _timerReminder.armTimer(TimerMessage::DISARMED);

   // Stop the audible alert
   guiglobs::button_audio_feedback.create_feedback_sound (ATTENTION1_SOUND, SOUND_CLEAR);



}


////////////////////////////////////////////////////////////////////////////////

void Screen_PAS_SPLIT::process_cancel_button ()
{
   goto_child_screen (GUI_SCREEN_ABORTCNF, ABORT_TYPE_ADDITIVE_SOLUTIONS);
}


////////////////////////////////////////////////////////////////////////////////

void Screen_PAS_SPLIT::process_continue_button ()
{
   btn_continue.deallocate_resources();
   MSSReadyPltAdditionMsg _readyForPLT(MessageBase::SEND_GLOBAL);
   _readyForPLT.send(0);
   DataLog(log_level_gui_info) << "MSSReadyPltAdditionMsg sent" << endmsg;

   goto_screen(GUI_SCREEN_AAS_ADDITION, "GUI_SCREEN_AAS_ADDITION");
}   // END of Screen_MSSCLAMP PROCESS_CONTINUE_BUTTON

void Screen_PAS_SPLIT::initialize_status_line ()
{
   guiglobs::apheresis_status_line->set_current_text (textStatPasPause);
}

////////////////////////////////////////////////////////////////////////////////
// REMINDER_TIMER_HANDLER

void Screen_PAS_SPLIT::reminder_timer_handler ()

{
   // Sound the reminder
   guiglobs::button_audio_feedback.create_feedback_sound (ATTENTION1_SOUND, SOUND_ONCE);

   // Start the timer again
   _timerReminder.armTimer(TimerMessage::ARMED);

}   // END of REMINDER_TIMER_HANDLER

/* FORMAT HASH 46765ce33ff8635df26b8354abe9c547 */

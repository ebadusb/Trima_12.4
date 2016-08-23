/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_CLAMP_CROSSOVER

*     File name:   clampCrossover.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/clampCrossover.cpp 1.6 2008/07/03 20:17:04Z dslausb Exp dslausb $

*/

#include <vxworks.h>

// BASE CLASS INCLUDE

#include "clampCrossover.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"

#define CLAMP_CROSSOVER_DELAY 600000 //  10.0 min per req C-MSS-1.101 (RAS)

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_CLAMP_CROSSOVER::Screen_CLAMP_CROSSOVER()
   : Base_Apheresis_Window (TEAL, GUI_SCREEN_CLAMP_CROSSOVER),
     _bmpClampCrossover (400, 145, BITMAP_CLAMP_CROSSOVER),
     _txtClampCrossover    (textMiscClampCrossover),
     _timerReminder(CLAMP_CROSSOVER_DELAY, Callback<Screen_CLAMP_CROSSOVER>(this, &Screen_CLAMP_CROSSOVER::reminder_timer_handler), TimerMessage::DISARMED)

{}     // END of Screen_CLAMP_CROSSOVER CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_CLAMP_CROSSOVER::~Screen_CLAMP_CROSSOVER()
{
   Screen_CLAMP_CROSSOVER::deallocate_resources ();
}     // END of Screen_CLAMP_CROSSOVER DESTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()

void Screen_CLAMP_CROSSOVER::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();
   btn_goback.deallocate_resources();

   // Now link the status bar/line to this window
   link_status_bar_line();

   // Now allocate text and graphics.
   _txtClampCrossover.allocate_resources(*this);
   _bmpClampCrossover.allocate_resources(*this);

   // Start the reminder timer
   _timerReminder.armTimer(TimerMessage::ARMED);

}    // END of Screen_CLAMP_CROSSOVER ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_CLAMP_CROSSOVER::deallocate_resources ()
{

   // Shut off the reminder timer
   _timerReminder.armTimer(TimerMessage::DISARMED);

   // Stop the audible alert
   guiglobs::button_audio_feedback.create_feedback_sound (ATTENTION1_SOUND, SOUND_CLEAR);

   // deallocate all constituent objects first
   _txtClampCrossover.deallocate_resources() ;
   _bmpClampCrossover.deallocate_resources();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}     // END of Screen_CLAMP_CROSSOVER DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON

void Screen_CLAMP_CROSSOVER::process_cancel_button ()
{
   // Call aphwind's default abort confirm messaging
   Base_Apheresis_Window::process_cancel_button();
}     // END of Screen_CLAMP_CROSSOVER PROCESS_CANCEL_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON

void Screen_CLAMP_CROSSOVER::process_continue_button ()
{
   // Disable all the standard buttons on this screen
   disable_standard_buttons();

   // Tell the procedure that the disposable is clamped, then wait for
   //  Procedure to change states to disposable test which will cause
   //  the disptest screen to replace this.
   DataLog (log_level_gui_info) << "Sending ClampedForTestingMsg message" << endmsg;

   CrossoverClampedMsg clamped(MessageBase::SEND_GLOBAL);
   clamped.send(0);

   goto_screen(GUI_SCREEN_SYSTEST, "GUI_SCREEN_SYSTEST");
}     // END of Screen_CLAMP_CROSSOVER PROCESS_CONTINUE_BUTTON


////////////////////////////////////////////////////////////////////////////////
// REMINDER_TIMER_HANDLER

void Screen_CLAMP_CROSSOVER::reminder_timer_handler ()

{
   // Sound the reminder
   guiglobs::button_audio_feedback.create_feedback_sound (ATTENTION1_SOUND, SOUND_ONCE);

   // Stop the timer since the sound is running continuously
   _timerReminder.armTimer(TimerMessage::ARMED);

}   // END of REMINDER_TIMER_HANDLER


void Screen_CLAMP_CROSSOVER::initialize_status_line ()
{
   guiglobs::apheresis_status_line->set_current_text (textStatClampCrossover) ;
}

/* FORMAT HASH f1ff7a8d1cf7c6ad6cda70ecaf145514 */

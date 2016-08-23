/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2003 GAMBRO BCT, Inc. All rights reserved

*     Class name:  Discrete_Button

*     File name:   discrete_button.cpp

*     Contents:    See prototype header descriptions in header file.

*************************< FOCUSSED GUI DOMAIN >****************************
*
*/

#include <vxworks.h>

#include "discrete_button.h"
#include "guiglobs.hpp"


// CONSTRUCTORS
Discrete_Button::Discrete_Button (GUI_BUTTON_ID btn_id,
                                  short corner_x,
                                  short corner_y,
                                  TEXT_STRING_ID btn_text_id,
                                  BITMAP_ID btn_up_bitmap_name = BITMAP_NULL,
                                  BITMAP_ID btn_dn_bitmap_name = BITMAP_NULL) :

   Button_Bitmap   (btn_id,
                    corner_x,
                    corner_y,
                    textEmptyFormatCfgNumericTextFormatNull,
                    btn_up_bitmap_name,
                    btn_dn_bitmap_name),

   _active(false)
{}


// DESTRUCTOR
Discrete_Button::~Discrete_Button()
{
   deallocate_widgets();
}


void Discrete_Button::allocate_resources (OSWindow& parent)
{
   Button_Bitmap::allocate_resources(parent);
   _active = false;
}


void Discrete_Button::deallocate_resources ()
{
   Button_Bitmap::deallocate_resources();
   _active = false;
}


void Discrete_Button::setActive ()
{
   btn_text.set_color(GREY_LIGHT);
   btn_up_bitmap.set_visible(false);
   _active = true;
}


void Discrete_Button::setInactive ()
{
   btn_text.set_color(YELLOW);
   btn_up_bitmap.set_visible(true);
   _active = false;
}


const int Discrete_Button::isActive () const
{
   return(_active == true);
}


const int Discrete_Button::isInactive () const
{
   return(_active == false);
}



void Discrete_Button::deallocate_widgets ()
{
   deallocate_resources();
}


///PROCESS_BUTTON_ARM
void Discrete_Button::process_button_arm ()
{
   if (!_callbackEnabled) return;

   // Create an audible feedback tone on button press
   guiglobs::button_audio_feedback.create_feedback_sound (BUTTON_CLICK_SOUND, SOUND_IMMEDIATE);

   // If the the button is active set inactive.
   if (_active)
   {
      // Set button text color yellow and set the button as raised.
      btn_text.set_color(YELLOW);
      btn_up_bitmap.set_visible(true);
      _active = false;

   }
   else
   {
      // Realize the button text as light grey and set button pushed.
      btn_text.set_color(GREY_LIGHT);
      btn_up_bitmap.set_visible(false);
      _active = true;
   }

} // END of Discrete_Button PROCESS_BUTTON_ARM


///PROCESS_BUTTON_DISARM
void Discrete_Button::process_button_disarm ()
{
   log_button_push();

   // Process the callback routine, if any.
   if (callback_routine_ptr && _callbackEnabled)
      (* callback_routine_ptr)(callback_data);

}  // END of Discrete_Button PROCESS_BUTTON_DISARM

/* FORMAT HASH 98b5f1172a09d568cd45793c7aad6621 */

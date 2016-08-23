/*
 * Copyright (c) 2008 by Gambro BCT, Inc.  All rights reserved.
 *
 *
 * Description: This class overloads the normal button class (Button_Bitmap)
 *   in order to make it behave like a toggle button (with down for "on" and
 *   up for "off".)  This replaces the old DiscreteButton class, which started
 *   causing page fault reboots when we ported to VxWorks 5.5.  Rather than track
 *   down exactly why, this seemed like the shorter path to me.
 *
 * Author: Dan Slauson x4664
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/toggleButton.cpp 1.3 2008/05/06 20:38:12Z dslausb Exp dslausb $
 * $Log: toggleButton.cpp $
 * Revision 1.3  2008/05/06 20:38:12Z  dslausb
 * IT 8614 - Fixed sound on toggle button
 * Revision 1.2  2008/04/04 20:34:41Z  dslausb
 * IT 5725 - Configurable DRBC minimum TBV
 * Revision 1.1  2008/04/03 22:16:28Z  dslausb
 * Initial revision
 *
 */

#include <vxworks.h>
#include "toggleButton.hpp"
#include "guiglobs.hpp"

////////////////////////////////////////////////////////////////
// Constructor. Note that DiscreteButton didn't used to take
// bitmaps in the constructor, but I figured it couldn't hurt
// to allow that additional customization.
////////////////////////////////////////////////////////////////
ToggleButton::ToggleButton(GUI_BUTTON_ID btn_id,
                           short x = 0,
                           short y = 0,
                           TEXT_STRING_ID text_id = TEXT_NULL,
                           BITMAP_ID btn_up_bitmap_name = BITMAP_NULL,
                           BITMAP_ID btn_dn_bitmap_name = BITMAP_NULL,
                           bool extraLogging = false)
   : Button_Bitmap(btn_id, x, y, text_id, btn_up_bitmap_name, btn_dn_bitmap_name, extraLogging),
     _isActive(false)
{}

////////////////////////////////////////////////////////////////
// These two functions provide an interface just like the
// old DiscreteButton class.  Active means down/on, inactive
// means up/off.
////////////////////////////////////////////////////////////////
void ToggleButton::setActive (bool isActive = true)
{
   _isActive = isActive;
   btn_up_bitmap.set_visible(!_isActive);
}

void ToggleButton::setInactive () { setActive(false); }


////////////////////////////////////////////////////////////////
// Override arm and disarm from the Button_Bitmap class to get
// the toggle functionality.
////////////////////////////////////////////////////////////////
void ToggleButton::process_button_arm ()
{
   guiglobs::button_audio_feedback.create_feedback_sound (BUTTON_CLICK_SOUND, SOUND_IMMEDIATE);
   setActive(!_isActive);
}

void ToggleButton::process_button_disarm ()
{
   // Call the base class to fire the callback and send the broadcast
   // message
   Button_Bitmap::process_button_disarm();

   // The base class is going to want to show the "up" version.
   // We may want to show the "down" version.
   btn_up_bitmap.set_visible(!_isActive);
}

/* FORMAT HASH 32b9294478ac9318b6b8180a7c8d5def */

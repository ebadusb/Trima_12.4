/*
 * Copyright (c) 2008 by Gambro BCT, Inc.  All rights reserved.
 *
 * Description: This class overloads the normal button class (Button_Bitmap)
 *   in order to make it behave like a toggle button (with down for "on" and
 *   up for "off".)  This replaces the old DiscreteButton class, which started
 *   causing page fault reboots when we ported to VxWorks 5.5.  Rather than track
 *   down exactly why, this seemed like the shorter path to me.
 *
 * Author: Dan Slauson x4664
 *
 *
 *
 * Revision 1.1  2008/04/03 22:16:33Z  dslausb
 * Initial revision
 *
 */

#ifndef __TOGGLE_BUTTON_HPP__
#define __TOGGLE_BUTTON_HPP__

#include "button_bitmap.h"

//
//  The button bitmap class is derived from a window so that it can
//  use event handling to process inputs.
//
class ToggleButton
   : public Button_Bitmap
{
private:

   bool _isActive;

public:

   ToggleButton(GUI_BUTTON_ID btn_id,
                short x = 0,
                short y = 0,
                TEXT_STRING_ID text_id = TEXT_NULL,
                BITMAP_ID btn_up_bitmap_name = BITMAP_NULL,
                BITMAP_ID btn_dn_bitmap_name = BITMAP_NULL,
                bool extraLogging = false
                ) ;

   virtual void setActive (bool isActive = true);
   virtual void setInactive ();

   virtual const int isActive ()    const { return _isActive; }
   virtual const int isInactive ()  const { return !_isActive; }

protected:

   // Override these from button bitmap.
   virtual void process_button_arm ();
   virtual void process_button_disarm ();
} ; // END of ToggleButton Class Declaration


#endif /* ifndef __TOGGLE_BUTTON_HPP__ */

/* FORMAT HASH b9515b91b80abcb09bf95b2942d61a5b */

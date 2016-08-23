/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2003 GAMBRO BCT, Inc. All rights reserved

*     Class name:  Discrete_Button

*     File name:   discrete_button.h

*     Contents:    See prototype header descriptions below...

*     Description:  This class implements a "discrete" button like the one
*                   used for Trima configuration.  The button will toggle
*                   between "active" and "inactive" each time the user
*                   presses the button.  Active is when the button is "pressed".
*                   Inactive is when the button is "raised".
*************************< FOCUSSED GUI DOMAIN >****************************
*/

// Single include define guardian
#ifndef DISCRETE_BUTTON_H
#define DISCRETE_BUTTON_H

#include "button_bitmap.h"

// Class Declares
class Discrete_Button
   : public Button_Bitmap
{
//    typedef void (* CALLBACK_ROUTINE_PTR) (void *);

public:
   // constructor
   Discrete_Button(GUI_BUTTON_ID btn_id = GUI_BUTTON_NULL_BUTTON,      // id of button
                   short corner_x = 0,                                 // x for text, button combo
                   short corner_y = 0,                                 // y for text, button combo
                   TEXT_STRING_ID btn_text_id = TEXT_NULL,             // text to be displayed
                   BITMAP_ID btn_up_bitmap_name = BITMAP_NULL,         // inActive "raised" button bitmap
                   BITMAP_ID btn_dn_bitmap_name = BITMAP_NULL);        // ACTIVE "pressed" button bitmap

   // destructor
   virtual ~Discrete_Button ();

   // allocate resources
   virtual void allocate_resources (OSWindow& parent);

   virtual void deallocate_resources ();

   // Will set a button active - pressed or inactive - raised.
   virtual void setActive ();
   virtual void setInactive ();

   virtual const int isActive () const;
   virtual const int isInactive () const;


protected:
   // PROCESS_BUTTON_ARM
   //  This method will perform general button processing on the depression
   //  or lifting of the button object.
   virtual void process_button_arm ();

   // PROCESS_BUTTON_DISARM
   //  It nullifies the parent class Button_Bitmap routine.
   virtual void process_button_disarm ();

   // non-virtual version of deallocate_resources
   void deallocate_widgets ();

protected:
   bool _active;

} ; // END of Discrete_Button Class Declaration

#endif   // End of single include define guardian

/* FORMAT HASH e4c3c8b1db3e6c60dc591daf88c8aa92 */

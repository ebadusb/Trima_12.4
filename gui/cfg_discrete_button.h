/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Config_discrete_button

*     File name:   cfg_discrete_button.h

*     Contents:    See prototype header descriptions below...

*     Description:  This class implements a "discrete" button for
*                   Trima configuration use.  The button will toggle
*                   between "active" and "inactive" each time the user
*                   presses the button.


*************************< FOCUSSED GUI DOMAIN >****************************

*/

// Single include define guardian
#ifndef CFG_DISCRETE_BUTTON_H
#define CFG_DISCRETE_BUTTON_H

#include "button_bitmap.h"

//
///////////////////////////////////////////////////////////////////
//
class Config_discrete_button
{
   typedef void (* CALLBACK_ROUTINE_PTR) (void*);


public:
   // constructor

   Config_discrete_button(short corner_x,                                // x for text, button combo
                          short corner_y,                                // y for text, button combo
                          GUI_BUTTON_ID btn_id = GUI_BUTTON_NULL_BUTTON, // id of button
                          TEXT_STRING_ID cfg_text_id = TEXT_NULL         // text to be displayed beside button
                          );

   // destructor
   virtual ~Config_discrete_button ();

   // allocate resources
   virtual void allocate_resources (OSWindow* parent);

   virtual void deallocate_resources ();

   virtual void      setActive ();
   virtual void      setInactive ();
   virtual const int isActive () const;
   virtual const int isInactive () const;
   virtual void      do_callback ();
   virtual void      set_callback (CALLBACK_ROUTINE_PTR cb, void* callback_data_ptr);

protected:
   int                  _active;
   OSWindow*            _parent;
   Button_Bitmap*       _UpButton;
   Button_Bitmap*       _DownButton;
   CALLBACK_ROUTINE_PTR _callback_routine;
   void*                _callback_data;
   GUI_BUTTON_ID        _btn_id;
   int                  _corner_x;
   int                  _corner_y;
   TEXT_STRING_ID       _cfg_text_id;


} ; // END of Display_Text Class Declaration


#endif   // End of single include define guardian

/* FORMAT HASH 6cc2c0544713030b81f8394e89c2d1b8 */

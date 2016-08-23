/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Config_discrete_button

*     File name:   cfg_discrete_button.cpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************

*/

#include <vxworks.h>

#include "cfg_discrete_button.h"
#include "cfg_look.h"

static void cb_cfg_discrete_button (void* data)
{
   ((Config_discrete_button*)data)->do_callback ();
}



Config_discrete_button::Config_discrete_button (short corner_x,
                                                short corner_y,
                                                GUI_BUTTON_ID btn_id,
                                                TEXT_STRING_ID cfg_text_id)
   : _active(-1),
     _parent(0),
     _UpButton(NULL),
     _DownButton(NULL),
     _callback_routine(0),
     _callback_data(0),
     _btn_id(btn_id),
     _corner_x(corner_x),
     _corner_y(corner_y),
     _cfg_text_id(cfg_text_id)
{}



Config_discrete_button::~Config_discrete_button()
{
   deallocate_resources();
}



//
// ----------------------------------------------------------------------------
void Config_discrete_button::allocate_resources (OSWindow* parent)
{
   _parent = parent;
   setActive();
}

//
// ----------------------------------------------------------------------------
void Config_discrete_button::deallocate_resources ()
{
   delete _UpButton;
   _UpButton = NULL;

   delete _DownButton;
   _DownButton = NULL;

   _active     = -1;
   _parent     = 0;
}



//
// ----------------------------------------------------------------------------
void Config_discrete_button::setActive ()
{
   if ((_parent) && (_DownButton == NULL))
   {
      delete _UpButton;
      _UpButton   = NULL;

      _DownButton = new Button_Bitmap (_btn_id,
                                       _corner_x,
                                       _corner_y,
                                       _cfg_text_id,
                                       CFG_Look::BUTTON_DOWN_NAME,
                                       CFG_Look::BUTTON_DOWN_NAME);

      _DownButton->allocate_resources(*_parent);
      _DownButton->set_button_text_color(CFG_Look::FONT_COLOR);
      _DownButton->set_callback(cb_cfg_discrete_button, (void*)this);
   }

   _active = 1;
}



//
// ----------------------------------------------------------------------------
void Config_discrete_button::setInactive ()
{
   if ((_parent) && (_UpButton == NULL))
   {
      delete _DownButton;
      _DownButton = NULL;

      _UpButton   = new Button_Bitmap (_btn_id,
                                       _corner_x,
                                       _corner_y,
                                       _cfg_text_id,
                                       CFG_Look::BUTTON_UP_NAME,
                                       CFG_Look::BUTTON_UP_NAME);

      _UpButton->allocate_resources(*_parent);
      _UpButton->set_button_text_color(CFG_Look::FONT_INACTIVE_COLOR);
      _UpButton->set_callback(cb_cfg_discrete_button, (void*)this);
   }

   _active = 0;
}



//
// ----------------------------------------------------------------------------
const int Config_discrete_button::isActive () const
{
   return (_active == 1);
}



const int Config_discrete_button::isInactive () const
{
   return (_active == 0);
}



//
// ----------------------------------------------------------------------------
void Config_discrete_button::set_callback (CALLBACK_ROUTINE_PTR cb, void* callback_data_ptr)
{
   _callback_routine = cb;
   _callback_data    = callback_data_ptr;
}



//
// ----------------------------------------------------------------------------
void Config_discrete_button::do_callback ()
{
   if (_active)
   {
      setInactive();
   }
   else
   {
      setActive();
   }


   if (_callback_routine)
   {
      _callback_routine(_callback_data);
   }

}

/* FORMAT HASH 9f4a9aef7c72e737fa4089ad4b51ac64 */

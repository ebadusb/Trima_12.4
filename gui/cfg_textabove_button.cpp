/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Config_textabove_button

*     File name:   cfg_textabove_button.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

*/
#include <vxworks.h>

static const int BUTTON_HEIGHT = 50;
static const int BUTTON_WIDTH  = 100;

#include "cfg_textabove_button.h"
#include "cfg_look.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Config_textabove_button::Config_textabove_button (short corner_x,
                                                  short corner_y,
                                                  const float& data,
                                                  CONFIG_DATA_KEYS cfg_param_name,
                                                  BITMAP_ID btn_up_bitmap_name,
                                                  BITMAP_ID btn_dn_bitmap_name,
                                                  GUI_BUTTON_ID btn_id_name,
                                                  TEXT_STRING_ID cfg_text_id,
                                                  TEXT_COLOR text_color,
                                                  TEXT_ALIGN text_align,
                                                  short text_size) :

   Config_button_with_text(corner_x,
                           corner_y,
                           data,
                           cfg_param_name,
                           btn_up_bitmap_name,
                           btn_dn_bitmap_name,
                           btn_id_name,
                           cfg_text_id,
                           text_color,
                           text_align,
                           text_size)
{}


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Config_textabove_button::~Config_textabove_button ()
{
   Config_textabove_button::deallocate_resources();
}


///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Config_textabove_button::deallocate_resources ()
{
   Config_button_with_text::deallocate_resources();
}


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Config_textabove_button::allocate_resources (OSWindow& parent)
{
   Config_button_with_text::allocate_resources(parent);

   //
   //  Move text to above button
   //
   OSRegion widget_positions;
   _txtLbl.get_requested_region(widget_positions);

   widget_positions.x = _xCoord - (widget_positions.width - BUTTON_WIDTH) / 2;
   widget_positions.y = (short)(_yCoord - BUTTON_HEIGHT + 10);

   if (widget_positions.y < 0)
      widget_positions.y = 0;

   _txtLbl.set_requested_region(widget_positions);
}


// SetActive
void Config_textabove_button::setActive ()
{
   set_up_bitmap(CFG_Look::BUTTON_UP_NAME);
   set_down_bitmap(CFG_Look::BUTTON_DOWN_NAME);
   set_text_color(CFG_Look::FONT_COLOR);

   redisplay();
}


// SetInactive
void Config_textabove_button::setInactive ()
{
   set_up_bitmap(BITMAP_CONFIG_BUTTON_GRAY);
   set_down_bitmap(BITMAP_NULL);
   set_text_color(CFG_Look::FONT_INACTIVE_COLOR);

   redisplay();
}

/* FORMAT HASH 6a15582d57f6a150aa3c19e70301896c */

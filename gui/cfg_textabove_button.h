/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Config_button_with_text

*     File name:   cfg_butt.hpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************

*/

// Single include define guardian
#ifndef CFG_TEXTABOVE_BUTTON_H
#define CFG_TEXTABOVE_BUTTON_H


#include "cfg_butt.hpp"

//
///////////////////////////////////////////////////////////////////
//
class Config_textabove_button
   : public Config_button_with_text
{
public:
   Config_textabove_button (short corner_x,                                // x for text, button combo
                            short corner_y,                                // y for text, button combo
                            const float& data,
                            CONFIG_DATA_KEYS cfg_param_name,
                            BITMAP_ID btn_up_bitmap_name = BITMAP_NULL,    // bitmap name of up button
                            BITMAP_ID btn_dn_bitmap_name = BITMAP_NULL,    // bitmap name of down button
                            GUI_BUTTON_ID btn_id = GUI_BUTTON_NULL_BUTTON, // id of button
                            TEXT_STRING_ID cfg_text_id = TEXT_NULL,        // text to be displayed beside button
                            TEXT_COLOR text_color = YELLOW,
                            TEXT_ALIGN text_align = LEFT,
                            short text_size = 14
                            );

   virtual ~Config_textabove_button ();

   virtual void allocate_resources (OSWindow& parent);

   virtual void deallocate_resources ();

   // New
   virtual void setActive ();
   virtual void setInactive ();

};

#endif

/* FORMAT HASH 5619afd9cc8ef0f39bf5cb15e0bde123 */

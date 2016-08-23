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
#ifndef CFGBUTT_HPP
#define CFGBUTT_HPP

// System #INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// CLASS INCLUDE

#include "button_bitmap.h"
#include "cfg_ids.h"
#include "list_get.hpp"



class ConfigButton
   : public Button_Bitmap
{
protected:
   union cfg_data_union
   {
      int   _idata;
      float _fdata;
   };

   short                    _xCoord, _yCoord;
   CONFIG_DATA_KEYS         _cfgDataKey;
   const cfg_data_union&    _cfgDataUnion;
   CONFIG_DATA_TYPE         _cfgDataType;
   Get_config_listbox_names _listboxStringRetriever;

public:

   ConfigButton      (short xCoord,                                         // x for text, button combo
                      short yCoord,                                         // y for text, button combo
                      const float& data,
                      CONFIG_DATA_KEYS cfgDataKey,
                      BITMAP_ID bmpUpId = BITMAP_NULL,                // bitmap name of up button
                      BITMAP_ID bmpDownId = BITMAP_NULL,              // bitmap name of down button
                      GUI_BUTTON_ID buttonId = GUI_BUTTON_NULL_BUTTON // id of button
                      );

   ConfigButton      (short xCoord,                                         // x for text, button combo
                      short yCoord,                                         // y for text, button combo
                      const int& data,
                      CONFIG_DATA_KEYS cfgDataKey,
                      BITMAP_ID bmpUpId = BITMAP_NULL,                // bitmap name of up button
                      BITMAP_ID bmpDownId = BITMAP_NULL,              // bitmap name of down button
                      GUI_BUTTON_ID buttonId = GUI_BUTTON_NULL_BUTTON // id of button
                      );

   virtual ~ConfigButton ();

   // allocate resources
   virtual void allocate_resources (OSWindow& parent);

   virtual void deallocate_resources ();

   void create_keypad_string (guistring& return_string, CONFIG_DATA_KEYS key_name, short X, short Y, const char* format, float max, float min);

   void create_param_string  (guistring& return_string, CONFIG_DATA_KEYS key_name, short X, short Y);

   void display_the_text_string (CONFIG_DATA_LISTBOX_STRINGS string_id);

   virtual void redisplay ();
};

//
///////////////////////////////////////////////////////////////////
//
class Config_button_with_text
   : public ConfigButton
{

protected:
   TEXT_COLOR   _lblAttrColor;
   TEXT_ALIGN   _lblAttrAlign;
   short        _lblAttrSize;

   Display_Text _txtLbl;

public:
   // constructor

   Config_button_with_text      (short xCoord,                                         // x for text, button combo
                                 short yCoord,                                         // y for text, button combo
                                 const float& data,
                                 CONFIG_DATA_KEYS cfgDataKey,
                                 BITMAP_ID bmpUpId = BITMAP_NULL,                 // bitmap name of up button
                                 BITMAP_ID bmpDownId = BITMAP_NULL,               // bitmap name of down button
                                 GUI_BUTTON_ID buttonId = GUI_BUTTON_NULL_BUTTON, // id of button
                                 TEXT_STRING_ID lblAttrStringId = TEXT_NULL,      // text to be displayed beside button
                                 TEXT_COLOR lblAttrColor = YELLOW,
                                 TEXT_ALIGN lblAttrAlign = LEFT,
                                 short lblAttrSize = 14
                                 );

   Config_button_with_text      (short xCoord,                                         // x for text, button combo
                                 short yCoord,                                         // y for text, button combo
                                 const int& data,
                                 CONFIG_DATA_KEYS cfgDataKey,
                                 BITMAP_ID bmpUpId = BITMAP_NULL,                 // bitmap name of up button
                                 BITMAP_ID bmpDownId = BITMAP_NULL,               // bitmap name of down button
                                 GUI_BUTTON_ID buttonId = GUI_BUTTON_NULL_BUTTON, // id of button
                                 TEXT_STRING_ID lblAttrStringId = TEXT_NULL,      // text to be displayed beside button
                                 TEXT_COLOR lblAttrColor = YELLOW,
                                 TEXT_ALIGN lblAttrAlign = LEFT,
                                 short lblAttrSize = 14
                                 );

   // destructor

   virtual ~Config_button_with_text ();

   // allocate resources
   virtual void allocate_resources (OSWindow& parent);

   virtual void deallocate_resources ();

   // Change the text that is beside/above the button.
   void change_label_text (guistring label_text);
   void change_label_text (TEXT_STRING_ID txtId);

   void set_region (const OSRegion& newRegion);

   void set_text_color (TEXT_COLOR lblAttrColor);

   virtual void printInfo (DataLog_Stream& os, const char* lbl = "");
} ; // END of Display_Text Class Declaration


#endif   // End of single include define guardian


//
///////////////////////////////////////////////////////////////////
// end of cfg_butt.hpp file

/* FORMAT HASH c3a2da84c673780f0614c8cd39eac883 */

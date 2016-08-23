/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Config_button_with_text

*     File name:   cfg_butt.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

*/
#include <vxworks.h>


#define   TEXT_AREA_WIDTH  400
#define   STRING_TOKEN    "~"


// Focussed #INCLUDES

#include "guiglobs.hpp"
#include "cfg_butt.hpp"
#include "keypad.h"
#include "an_alarm.h"
#include "screen_config.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
ConfigButton::ConfigButton(short xCoord,                  // x for text, button combo
                           short yCoord,                  // y for text, button combo
                           const float& data,
                           CONFIG_DATA_KEYS cfgDataKey,
                           BITMAP_ID bmpUpId = BITMAP_NULL,                // bitmap name of up button
                           BITMAP_ID bmpDownId = BITMAP_NULL,              // bitmap name of down button
                           GUI_BUTTON_ID buttonId = GUI_BUTTON_NULL_BUTTON // id of button
                           ) :

   Button_Bitmap(buttonId, xCoord, yCoord, textEmptyFormatCfgNumericTextFormatNull, bmpUpId, bmpDownId),

   _cfgDataType       (guiglobs::config_var_list.Var(cfgDataKey).DisplayType()),
   _cfgDataUnion       ((cfg_data_union&)data),
   _cfgDataKey      (cfgDataKey),
   _xCoord(xCoord),
   _yCoord(yCoord)
{}


ConfigButton::ConfigButton(short xCoord,                  // x for text, button combo
                           short yCoord,                  // y for text, button combo
                           const int& data,
                           CONFIG_DATA_KEYS cfgDataKey,
                           BITMAP_ID bmpUpId = BITMAP_NULL,                // bitmap name of up button
                           BITMAP_ID bmpDownId = BITMAP_NULL,              // bitmap name of down button
                           GUI_BUTTON_ID buttonId = GUI_BUTTON_NULL_BUTTON // id of button
                           ) :

   Button_Bitmap(buttonId, xCoord, yCoord, textEmptyFormatCfgNumericTextFormatNull, bmpUpId, bmpDownId),

   _cfgDataType       (guiglobs::config_var_list.Var(cfgDataKey).DisplayType()),
   _cfgDataUnion       ((cfg_data_union&)data),
   _cfgDataKey      (cfgDataKey),
   _xCoord(xCoord),
   _yCoord(yCoord)
{}


Config_button_with_text::Config_button_with_text (short xCoord,
                                                  short yCoord,
                                                  const float& data,
                                                  CONFIG_DATA_KEYS cfgDataKey,
                                                  BITMAP_ID bmpUpId,
                                                  BITMAP_ID bmpDownId,
                                                  GUI_BUTTON_ID buttonId,
                                                  TEXT_STRING_ID lblAttrStringId,
                                                  TEXT_COLOR lblAttrColor,
                                                  TEXT_ALIGN lblAttrAlign,
                                                  short lblAttrSize) :

   ConfigButton    (xCoord, yCoord, data, cfgDataKey, bmpUpId, bmpDownId, buttonId),

   _txtLbl       (lblAttrStringId),

   _lblAttrColor (lblAttrColor),
   _lblAttrAlign   (lblAttrAlign),
   _lblAttrSize    (lblAttrSize)
{}

Config_button_with_text::Config_button_with_text (short xCoord,
                                                  short yCoord,
                                                  const int& data,
                                                  CONFIG_DATA_KEYS cfgDataKey,
                                                  BITMAP_ID bmpUpId,
                                                  BITMAP_ID bmpDownId,
                                                  GUI_BUTTON_ID buttonId,
                                                  TEXT_STRING_ID lblAttrStringId,
                                                  TEXT_COLOR lblAttrColor,
                                                  TEXT_ALIGN lblAttrAlign,
                                                  short lblAttrSize) :

   ConfigButton(xCoord, yCoord, data, cfgDataKey, bmpUpId, bmpDownId, buttonId),

   _txtLbl       (lblAttrStringId),
   _lblAttrColor (lblAttrColor),
   _lblAttrAlign   (lblAttrAlign),
   _lblAttrSize    (lblAttrSize)
{}


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
//
ConfigButton::~ConfigButton()
{
   // deallocate_resources is virtual, so be sure to call the right one!
   ConfigButton::deallocate_resources();
}

Config_button_with_text::~Config_button_with_text ()
{
   Config_button_with_text::deallocate_resources();
}


///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
//
void ConfigButton::deallocate_resources ()
{
   Button_Bitmap::deallocate_resources ();
}

void Config_button_with_text::deallocate_resources ()
{
   ConfigButton::deallocate_resources();
   _txtLbl.deallocate_resources ();
}   // END of DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
//

void ConfigButton::allocate_resources (OSWindow& parent)
{
   Button_Bitmap::allocate_resources(parent);
   redisplay();
}

void Config_button_with_text::allocate_resources (OSWindow& parent)
{
   ConfigButton::allocate_resources(parent);

   // This class no longer sets text layout, and relies instead on
   // the styling record, because localization prefers it this way.
   _txtLbl.allocate_resources(parent);
}


void ConfigButton::redisplay ()
{
   guistring button_string;

   switch (_cfgDataType)
   {
      case STRING_TYPE :   // if it is a string
      {
         CONFIG_DATA_LISTBOX_STRINGS string_id = (CONFIG_DATA_LISTBOX_STRINGS)(_cfgDataUnion._idata);
         // set_button_text_color(_lblAttrColor);
         display_the_text_string(string_id);
         break;
      }

      case FLOAT_0DEC_TYPE :   // if it is a float
      {
         int text_len = sprintf(button_string, "%.0f", _cfgDataUnion._fdata);
         Button_Bitmap::set_text_direct (text_len, button_string);
         // set_button_text_color(_lblAttrColor);
         break;
      }

      case FLOAT_1DEC_TYPE :   // if it is a float
      {
         int text_len = sprintf_l10n(button_string, "%.1f", _cfgDataUnion._fdata);
         Button_Bitmap::set_text_direct (text_len, button_string);
         // set_button_text_color(_lblAttrColor);
         break;
      }

      case FLOAT_2DEC_TYPE :   // if it is a float
      {
         int text_len = sprintf_l10n(button_string, "%.2f", _cfgDataUnion._fdata);
         Button_Bitmap::set_text_direct (text_len, button_string);
         // set_button_text_color(_lblAttrColor);
         break;
      }

      case INT_TYPE :
      {
         int text_len = sprintf(button_string, "%d", _cfgDataUnion._idata);
         Button_Bitmap::set_text_direct (text_len, button_string);
         // set_button_text_color(_lblAttrColor);
         break;
      }

      case TIME_TYPE :
      {
         time_t time_of_run;
         char   time_current_display[80];

         memset(time_current_display, 0, sizeof(time_current_display));

         // reset time of run pointer as to access current
         time_of_run = time(NULL);

         // set the middle time element to current time
         strftime(time_current_display, 80, "%H:%M", localtime(&time_of_run));

         // replace default first zero char if present
         if (time_current_display[0] == '0')
         {
            time_current_display[0] = ' ';
         }

         // set the current time text direct
         Button_Bitmap::set_text_direct(strlen(time_current_display), time_current_display);
         // set_button_text_color(_lblAttrColor);

         break;
      }

      case DATE_TYPE :
      {
         time_t time_of_run;
         char   time_current_display[80];
         char   format_string[80] = {0};

         memset(time_current_display, 0, sizeof(time_current_display));

         // reset time of run pointer as to access current
         time_of_run = time(NULL);

         // set the middle time element to current time
         switch (guiglobs::cdsData.rwConfig.lang.key_date_format)
         {
            case MMDDYY_DATE :
            {
               strcpy(format_string, "%m/%d/%y");
               break;
            }

            case DDMMYY_DATE :
            {
               strcpy(format_string, "%d/%m/%y");
               break;
            }

            case YYMMDD_DATE :
            {
               strcpy(format_string, "%y/%m/%d");
               break;
            }

            default :
            {
               anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
               alarm.setAlarm();

               DataLog(log_level_gui_error) << "could not find the correct DATE FORMAT to display the current date" << endmsg;
            }
         }

         strftime(time_current_display, 80, format_string, localtime(&time_of_run));

         // set the current time text direct
         // set_button_text_color(_lblAttrColor);
         Button_Bitmap::set_text_direct(strlen(time_current_display), time_current_display);

         break;

      }

      case NULL_TYPE :
         break;    // no display

      case INACTIVE_TYPE :
         set_string_id(textButtonCfgNonDisplayedNumber);
         // set_button_text_color(_lblAttrColor);
         break;

      default :
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();

         DataLog(log_level_gui_error) << "could not find the correct config data type" << endmsg;
         break;

   }   // end of switch

}  // end of allocate resources


//
///////////////////////////////////////////////////////////////////
// set_text_string method
void ConfigButton::display_the_text_string (CONFIG_DATA_LISTBOX_STRINGS string_id)
{
   TEXT_STRING_ID text_id = _listboxStringRetriever.return_string_for_ID(_cfgDataKey, string_id);
   Button_Bitmap::set_string_id(text_id);
}


//
////////////////////////////////////////////////////////////////////////////////
//
void ConfigButton::create_param_string (guistring& build_str, CONFIG_DATA_KEYS key_name, short X, short Y)
{
   build_str.initialize();  // empty the string
   char temp_str[80] = "";

   sprintf(build_str, "%d", key_name);
   // printf("cfg_butt: first build string= %s \n",build_str);
   strcat(build_str, STRING_TOKEN);
   sprintf(temp_str, "%d", X);
   strcat(build_str, temp_str);
   strcat(build_str, STRING_TOKEN);
   sprintf(temp_str, "%d", Y);
   strcat(build_str, temp_str);

} // end of create_param_string method


//
///////////////////////////////////////////////////////////////////////////////
// method of creating a KEYPAD string
void ConfigButton::create_keypad_string (guistring& keypad_string, CONFIG_DATA_KEYS key_name, short x_pos, short y_pos, const char* format, float max, float min)
{
#define KEYPAD_INT_STRING  "~enter~0~1~2~3~4~5~6~7~8~9~C~"
#define KEYPAD_EXP         "~0~"

   keypad_string.initialize();  // empty out string
   char temp_str[80] = "";

   sprintf(temp_str, "%d", STANDARD);
   strcat(keypad_string, temp_str);
   strcat(keypad_string, STRING_TOKEN);

   // printf("butt: the string is: %s \n", keypad_string);

   sprintf(temp_str, "%f", max);
   strcat(keypad_string, temp_str);
   strcat(keypad_string, STRING_TOKEN);

   sprintf(temp_str, "%f", min);
   strcat(keypad_string, temp_str);
   strcat(keypad_string, STRING_TOKEN);

   sprintf(temp_str, "%d", x_pos);
   strcat(keypad_string, temp_str);
   strcat(keypad_string, STRING_TOKEN);

   sprintf(temp_str, "%d", y_pos);
   strcat(keypad_string, temp_str);
   strcat(keypad_string, STRING_TOKEN);

   sprintf(temp_str, "%d", textEmpty);
   strcat(keypad_string, temp_str);
   strcat(keypad_string, STRING_TOKEN);
   // printf("temp before= %s \n",temp_str);
   strcpy(temp_str, "");
   // printf("temp after= %s \n",temp_str);

   strcpy(temp_str, format);
   strcat(keypad_string, temp_str);
   strcat(keypad_string, STRING_TOKEN);

   sprintf(temp_str, " ");
   strcat(keypad_string, temp_str);
   strcat(keypad_string, STRING_TOKEN);

   sprintf(temp_str, "%d", key_name);
   strcat(keypad_string, temp_str);
   strcat(keypad_string, STRING_TOKEN);

   sprintf(temp_str, "%d", 1);
   strcat(keypad_string, temp_str);
}


///////////////////////////////////////////////////////////////////////////////
///CHANGE_LABEL_TEXT
void Config_button_with_text::change_label_text (guistring label_text)
{
   _txtLbl.set_text_direct(strlen(label_text), label_text);
}

void Config_button_with_text::change_label_text (TEXT_STRING_ID txtId)
{
   _txtLbl.set_string_id(txtId);
}

///////////////////////////////////////////////////////////////////////////////
///SET_TEXT_COLOR
void Config_button_with_text::set_text_color (TEXT_COLOR lblAttrColor)
{
   _lblAttrColor = lblAttrColor;
   _txtLbl.set_color(lblAttrColor);
}

///////////////////////////////////////////////////////////////////////////////
///CHANGE_LABEL_TEXT
void Config_button_with_text::set_region (const OSRegion& newRegion)
{
   Button_Bitmap::set_region(newRegion);
}

void Config_button_with_text::printInfo (DataLog_Stream& os, const char* lbl = "")
{
   os << "[Btn=" << lbl << "BtnId=" << btn_logical_id << ";btnRegion=" << _region << ";txtRegion="
      << _txtLbl.getRegion() <<  "; allocated=" << allocated() << "]" << endmsg;
}


//
///////////////////////////////////////////////////////////////////
// end of cfg_butt.cpp file

/* FORMAT HASH ceb6a312616d859222a5587ddb259640 */

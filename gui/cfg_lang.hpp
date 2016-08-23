/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG_LANGUAGE

*     File name:   cfg_lang.hpp

*     Contents:    See prototype header descriptions below...

*     Description:

         This screen is based on the Base_Apheresis_Window class and is displayed
         when the operator presses
*************************< FOCUSSED GUI DOMAIN >****************************

*/


#ifndef CFGLANG_HPP
#define CFGLANG_HPP

/** include files **/

// CLASS INCLUDE
#include "screen_config.hpp"
#include "cfg_butt.hpp"
#include "guistring.h"
#include "dynamicListboxItem.hpp"

/** defines **/
#define  MAX_STRING_LENGTH 10;

///////////////////////////////////////////////////////////////////////////////
// class definition
class Screen_CONFIG_LANGUAGE
   : public Screen_Config
{
private:
   guistring allocation_param;    // used to pass key name to listbox childscreen
   guistring listbox_string;

   Display_Text
      _txtLangBtnLbl,
      _txtBtnLblHeight,
      _txtBtnLblWeight,
      _txtBtnLblDecimalDelim,
      _txtBtnLblCritGlob;

   Button_Bitmap _btnLang;

   ConfigButton
      _cfgBtnHeight,
      _cfgBtnWeight,
      _cfgBtnDecimalDelim,
      _cfgBtnCritGlob;

   LanguageNameContainer lang_name;

protected:

public:

   // CONSTRUCTOR
   Screen_CONFIG_LANGUAGE();

   // DESTRUCTOR
   virtual ~Screen_CONFIG_LANGUAGE();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   virtual void process_lang_button ();
   virtual void process_weight_button ();
   virtual void process_height_button ();
   virtual void process_decimal_delimiter_button ();
   virtual void process_crit_or_glob_button ();

   // methods for notifies
   virtual void update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer);

   Message<DynamicListboxItem*> _msgLangListboxReturn;

   virtual void langListboxCb ();

   void checkForTimeOnlyConfig ();

}; // END of Screen_CONFIG_language Class Declaration

#endif   // End of single include define guardian

/* FORMAT HASH fcc33cbfa06c594f737121011ad8a03e */

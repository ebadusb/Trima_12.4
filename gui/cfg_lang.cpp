/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG_LANGUAGE

*     File name:   cfg_lang.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$

*/

#include <vxworks.h>

// BASE CLASS INCLUDE
#include "cfg_lang.hpp"
#include "guiglobs.hpp"
#include "an_alarm.h"
#include "dynamicListbox.hpp"
#include "statBarBase.hpp"
#include "language_loader.h"


void cb_cfg_lang (void* data);
void cb_cfg_height (void* data);
void cb_cfg_weight (void* data);
void cb_cfg_decimal_delimiter (void* data);
void cb_cfg_crit_or_glob (void* data);

// EXTERNAL REFERENCES

#define CFG_LANG_BTN_COL_1_X 150
#define CFG_LANG_BTN_COL_2_X 450
#define CFG_LANG_BTN_ROW_1_Y 115
#define CFG_LANG_BTN_ROW_2_Y 215
#define CFG_LANG_BTN_ROW_3_Y 315


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR


Screen_CONFIG_LANGUAGE::Screen_CONFIG_LANGUAGE()
   : Screen_Config(TEAL, GUI_SCREEN_CONFIG_LANGUAGE, textCfgScreenLabelLang),

     _txtLangBtnLbl(textButtonLabelCfgLeftLangLanguage),
     _txtBtnLblHeight(textButtonLabelCfgLeftLangHeight),
     _txtBtnLblWeight(textButtonLabelCfgLeftLangWeight),
     _txtBtnLblDecimalDelim(textButtonLabelCfgLeftLangDecDelim),
     _txtBtnLblCritGlob(textButtonLabelCfgLeftLangCritOrGlob),

     _btnLang(GUI_BUTTON_CFG_LANG,
              CFG_LANG_BTN_COL_1_X,
              CFG_LANG_BTN_ROW_1_Y,
              textEmptyFormatButtonCfgLang,
              CFG_Look::BUTTON_UP_NAME,
              CFG_Look::BUTTON_DOWN_NAME),

     _cfgBtnHeight(CFG_LANG_BTN_COL_1_X,
                   CFG_LANG_BTN_ROW_2_Y,
                   guiglobs::cdsData.rwConfig.lang.key_height,
                   KEY_HEIGHT,
                   CFG_Look::BUTTON_UP_NAME,
                   CFG_Look::BUTTON_DOWN_NAME,
                   GUI_BUTTON_CONFIG_HEIGHT),

     _cfgBtnWeight(CFG_LANG_BTN_COL_1_X,
                   CFG_LANG_BTN_ROW_3_Y,
                   guiglobs::cdsData.rwConfig.lang.key_weight,
                   KEY_WEIGHT,
                   CFG_Look::BUTTON_UP_NAME,
                   CFG_Look::BUTTON_DOWN_NAME,
                   GUI_BUTTON_CONFIG_WEIGHT),

     _cfgBtnDecimalDelim(CFG_LANG_BTN_COL_2_X,
                         CFG_LANG_BTN_ROW_1_Y,
                         guiglobs::cdsData.rwConfig.lang.key_decimal_delimiter,
                         KEY_DECIMAL_DELIMITER,
                         CFG_Look::BUTTON_UP_NAME,
                         CFG_Look::BUTTON_DOWN_NAME,
                         GUI_BUTTON_CONFIG_DECIMAL_DELIMITER),

     _cfgBtnCritGlob(CFG_LANG_BTN_COL_2_X,
                     CFG_LANG_BTN_ROW_2_Y,
                     guiglobs::cdsData.rwConfig.lang.key_crit_or_glob,
                     KEY_CRIT_OR_GLOB,
                     CFG_Look::BUTTON_UP_NAME,
                     CFG_Look::BUTTON_DOWN_NAME,
                     GUI_BUTTON_CONFIG_CRIT_OR_GLOB)
{}




////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_CONFIG_LANGUAGE::~Screen_CONFIG_LANGUAGE()
{
   Screen_CONFIG_LANGUAGE::deallocate_resources ();
}



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_CONFIG_LANGUAGE::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Screen_Config::allocate_resources (allocation_parameter);

   // then allocate constituent objects with this window as parent widget
   // layer allocation from Z-back to Z-front.

   // Allocate buttons
   _txtLangBtnLbl.allocate_resources(*this);
   _txtBtnLblHeight.allocate_resources(*this);
   _txtBtnLblWeight.allocate_resources(*this);
   _txtBtnLblDecimalDelim.allocate_resources(*this);
   _txtBtnLblCritGlob.allocate_resources(*this);

   const LanguageHeader* currentLang = LanguageList::currentLanguage();

   if (currentLang == NULL)
   {
      DataLog(log_level_gui_error) << "Tried to access language index " << currentLang << " failed." << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "can't access current language");
   }

   // The object needs to stick around to make the text calls work
   lang_name = LanguageNameContainer (currentLang->langName.c_str(), currentLang->langStyling, currentLang->fontIndex, currentLang->languageIndex);
   _btnLang.set_text_direct(lang_name.getTextItemPtr());

   DataLog (log_level_gui_info) << "Setting language button text to " << currentLang->langName << endmsg;

   _btnLang.allocate_resources (*this) ;
   _btnLang.set_callback (cb_cfg_lang, (void*)this);

   _cfgBtnHeight.allocate_resources (*this) ;
   _cfgBtnHeight.set_callback (cb_cfg_height, (void*)this);

   _cfgBtnWeight.allocate_resources (*this) ;
   _cfgBtnWeight.set_callback (cb_cfg_weight, (void*)this);

   _cfgBtnDecimalDelim.allocate_resources (*this) ;
   _cfgBtnDecimalDelim.set_callback (cb_cfg_decimal_delimiter, (void*)this);

   _cfgBtnCritGlob.allocate_resources (*this) ;
   _cfgBtnCritGlob.set_callback (cb_cfg_crit_or_glob, (void*)this);

   checkForTimeOnlyConfig();
}



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_CONFIG_LANGUAGE::deallocate_resources ()
{
   _msgLangListboxReturn.deregisterMsg();

   // deallocate all constituent objects first
   _txtLangBtnLbl.deallocate_resources();
   _txtBtnLblHeight.deallocate_resources();
   _txtBtnLblWeight.deallocate_resources();
   _txtBtnLblDecimalDelim.deallocate_resources();
   _txtBtnLblCritGlob.deallocate_resources();

   _btnLang.deallocate_resources ();
   _cfgBtnHeight.deallocate_resources ();
   _cfgBtnWeight.deallocate_resources ();
   _cfgBtnDecimalDelim.deallocate_resources ();
   _cfgBtnCritGlob.deallocate_resources ();

   // deallocate parent's resources
   Screen_Config::deallocate_resources ();

}  // END of Screen_CONFIG_LANGUAGE DEALLOCATE_RESOURCES



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// callbacks for the LANGUAGE buttons

void cb_cfg_lang (void* data)
{
   ((Screen_CONFIG_LANGUAGE*)data)->process_lang_button();
}




//
////////////////////////////////////////////////////////////////////////////////
// Handle the LANGUAGE button
void Screen_CONFIG_LANGUAGE::process_lang_button ()
{
   _msgLangListboxReturn.init(Callback<Screen_CONFIG_LANGUAGE>(this, &Screen_CONFIG_LANGUAGE::langListboxCb), MessageBase::SNDRCV_RECEIVE_ONLY);

   DynamicListbox::createAllocationString(listbox_string, CFG_Look::LISTBOX_X);
   goto_child_screen(GUI_SCREEN_LANG_LISTBOX, listbox_string);
}



////////////////////////////////////////////////////////////////////////////////
// callbacks for the HEIGHT buttons
void cb_cfg_height (void* data)
{
   ((Screen_CONFIG_LANGUAGE*)data)->process_height_button();
}



//
////////////////////////////////////////////////////////////////////////////////
// Handle the HEIGHT button
void Screen_CONFIG_LANGUAGE::process_height_button ()
{
   _cfgBtnHeight.create_param_string(listbox_string, KEY_HEIGHT, CFG_Look::LISTBOX_X, CFG_Look::FIRST_Y + CFG_Look::Y_DIFF);
   goto_child_screen(GUI_SCREEN_LISTBOX, listbox_string);
}



//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the WEIGHT buttons
void cb_cfg_weight (void* data)
{
   ((Screen_CONFIG_LANGUAGE*)data)->process_weight_button();
}



//
////////////////////////////////////////////////////////////////////////////////
// Handle the WEIGHT button
void Screen_CONFIG_LANGUAGE::process_weight_button ()
{
   _cfgBtnWeight.create_param_string(listbox_string, KEY_WEIGHT, CFG_Look::LISTBOX_X, CFG_Look::FIRST_Y + (2 * CFG_Look::Y_DIFF));
   goto_child_screen(GUI_SCREEN_LISTBOX, listbox_string);
}


//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the DECIMAL_DELIMITER buttons
void cb_cfg_decimal_delimiter (void* data)
{
   ((Screen_CONFIG_LANGUAGE*)data)->process_decimal_delimiter_button();
}



//
////////////////////////////////////////////////////////////////////////////////
// Handle the decimal_delimiter button
void Screen_CONFIG_LANGUAGE::process_decimal_delimiter_button ()
{
   _cfgBtnDecimalDelim.create_param_string(listbox_string, KEY_DECIMAL_DELIMITER, CFG_Look::LISTBOX_X_2, CFG_Look::FIRST_Y + (2 * CFG_Look::Y_DIFF));
   goto_child_screen(GUI_SCREEN_LISTBOX, listbox_string);
}

//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the CRIT_OR_GLOB buttons
void cb_cfg_crit_or_glob (void* data)
{
   ((Screen_CONFIG_LANGUAGE*)data)->process_crit_or_glob_button();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the decimal_delimiter button
void Screen_CONFIG_LANGUAGE::process_crit_or_glob_button ()
{
   _cfgBtnCritGlob.create_param_string(listbox_string, KEY_CRIT_OR_GLOB, CFG_Look::LISTBOX_X_2, CFG_Look::FIRST_Y + (2 * CFG_Look::Y_DIFF));
   goto_child_screen(GUI_SCREEN_LISTBOX, listbox_string);
}

//
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Message handler for LISBOXS
void Screen_CONFIG_LANGUAGE::update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer)
{
   switch (rtn_listbox_buffer.key_name)
   {
      case KEY_HEIGHT :
      {
         _cfgBtnHeight.display_the_text_string (CONFIG_DATA_LISTBOX_STRINGS(rtn_listbox_buffer.key_value));
         guiglobs::cdsData.rwConfig.lang.key_height = rtn_listbox_buffer.key_value;
         break;
      }

      case KEY_WEIGHT :
      {
         int newSetting = rtn_listbox_buffer.key_value;
         _cfgBtnWeight.display_the_text_string(CONFIG_DATA_LISTBOX_STRINGS(newSetting));
         guiglobs::cdsData.rwConfig.lang.key_weight = rtn_listbox_buffer.key_value;
         break;
      }

      case KEY_DECIMAL_DELIMITER :
      {
         _cfgBtnDecimalDelim.display_the_text_string(CONFIG_DATA_LISTBOX_STRINGS(rtn_listbox_buffer.key_value));
         guiglobs::cdsData.rwConfig.lang.key_decimal_delimiter = rtn_listbox_buffer.key_value;
         break;
      }

      case KEY_CRIT_OR_GLOB :
      {
         _cfgBtnCritGlob.display_the_text_string(CONFIG_DATA_LISTBOX_STRINGS(rtn_listbox_buffer.key_value));
         guiglobs::cdsData.rwConfig.lang.key_crit_or_glob = rtn_listbox_buffer.key_value;
         break;
      }

      default :
      {
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();

         DataLog(log_level_gui_error) << "Could not find return key from a configuration list box" << endmsg;
      }

   }  // end of switch

}  // end of update_screen_with_date method



// This callback is called with the new language choice
void Screen_CONFIG_LANGUAGE::langListboxCb ()
{
   // Get the return data and cast it as a LanguageNameContainer
   LanguageNameContainer* langChosen = (LanguageNameContainer*)_msgLangListboxReturn.getData();

   // Use the language from the config file because the current language may have been overwritten
   // during language load (corner case)
   const unsigned int currentLanguageIndex = guiglobs::cdsData.rwConfig.getLanguageIndex();

   // Make sure the returned pointer is OK.
   if (!langChosen)
      _FATAL_ERROR(__FILE__, __LINE__, "Unknown language chosen");

   // Don't bother with all the rigamarole if nothing actually changed.
   if (currentLanguageIndex != langChosen->LanguageIndex)
   {
      const LanguageHeader* languageChosen = LanguageList::getLanguageHeader(langChosen->LanguageIndex);
      const LanguageHeader* currentLang    = LanguageList::getLanguageHeader(currentLanguageIndex);

      guiglobs::cdsData.rwConfig.lang.key_lang = langChosen->LanguageIndex;
      LanguageList::setCurrentLanguage(languageChosen);

      // Load up the newly selected language
      if (!LanguageList::loadLanguage(languageChosen, currentLang))
      {
         DataLog (log_level_gui_error) << "Unable to load current language. Try English." << endmsg;

         guiglobs::cdsData.rwConfig.lang.key_lang = 0;

         const LanguageHeader* englishLanguage = *(LanguageList::begin());
         LanguageList::setCurrentLanguage(englishLanguage);

         if (!LanguageList::loadLanguage(englishLanguage, languageChosen))
         {
            DataLog(log_level_gui_error) << "ERROR: failed to load a valid language file." << endmsg;
            _FATAL_ERROR(__FILE__, __LINE__, "No valid string file located.");
         }
      }

      const int fontIndex = langChosen->getReturnIndex();
      const int fontSize  = langChosen->getTextItemPtr()->getStylingRecord().fontSize;

      DataLog (log_level_gui_info) << "Setting language button text to \"" << langChosen->langName
                                   << "\"; font to index=" << fontIndex << "; size=" << fontSize << endmsg;

      // Update button text and font
      _btnLang.setFont(fontIndex, fontSize);
      _btnLang.set_text_direct(&langChosen->langName);

      // Immediately leave this screen, as we did in 5.1.
      process_continue_button ();

      // Refresh the status bar.
      guiglobs::apheresis_status_bar->deallocate_resources();
      guiglobs::apheresis_status_bar->allocate_resources();
   }
}



void Screen_CONFIG_LANGUAGE::checkForTimeOnlyConfig ()
{
   if (guiglobs::timeOnlyConfigMode)
   {
      DataLog (log_level_gui_info) << "Buttons should be disabled." << endmsg;

      disableConfigBtn(_btnLang);
      disableConfigBtn(_cfgBtnHeight);
      disableConfigBtn(_cfgBtnWeight);
      disableConfigBtn(_cfgBtnDecimalDelim);
      disableConfigBtn(_cfgBtnCritGlob);

      disableConfigTxt(_txtLangBtnLbl);
      disableConfigTxt(_txtBtnLblHeight);
      disableConfigTxt(_txtBtnLblWeight);
      disableConfigTxt(_txtBtnLblDecimalDelim);
      disableConfigTxt(_txtBtnLblCritGlob);
   }
}

//
///////////////////////////////////////////////////////////////////////////////
// end of cfg_lang.cpp file

/* FORMAT HASH 1dfae367f5f2664424585972719a3a96 */

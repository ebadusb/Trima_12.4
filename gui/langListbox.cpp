/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 GAMBRO BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  LanguageListbox

*     File name:   langListbox.cpp

*     Contents:    Source file for dynamic listbox class

*     Description: This is the listbox that's used for language selection. It's
    kind of a special case, in that different fonts are needed per box.

*************************< FOCUSSED GUI DOMAIN >****************************
*/

#include <vxworks.h>

/** include files **/

#include "langListbox.hpp"
#include "dynamicListboxItem.hpp"
#include "language_loader.h"
#include "string_data.h"



// CONSTRUCTOR
LanguageListbox::LanguageListbox()
   : DynamicListbox(GUI_SCREEN_LANG_LISTBOX, &_languageNameStrings)
{}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
LanguageListbox::~LanguageListbox()
{
   LanguageListbox::deallocate_resources();
}


void LanguageListbox::allocate_resources (const char* allocation_parameter)
{
   for (LANGUAGE_VECTOR_ITERATOR iter = LanguageList::begin();
        iter != LanguageList::end();
        iter++)
   {
      const LanguageHeader* lang = *(iter);

      if (lang == NULL)
      {
         _FATAL_ERROR(__FILE__, __LINE__, "Bad Language List link");
      }

      LanguageNameContainer* textItem = new LanguageNameContainer(lang->langName.c_str(),
                                                                  lang->langStyling,
                                                                  lang->fontIndex,
                                                                  lang->languageIndex);
      _languageNameStrings.push_back(textItem);  // use copy constructor to create the object

      DataLog(log_level_gui_info) << "Added language " << lang->langName.c_str()
                                  << "[" << textItem->LanguageIndex << "]"
                                  << " font index " << textItem->getReturnIndex()
                                  << " font size " << textItem->langName.getStylingRecord().fontSize
                                  << " to language list." << endmsg;
   }

   // Call the base constructor
   DynamicListbox::allocate_resources(allocation_parameter);
}



void LanguageListbox::deallocate_resources ()
{
   for (DynamicListboxIterator iter = _languageNameStrings.begin();
        iter != _languageNameStrings.end();
        iter++)
   {
      DynamicListboxItem* item = *(iter);

      if (item == NULL)
      {
         _FATAL_ERROR(__FILE__, __LINE__, "Bad Language List link");
      }

      // Delete the contents of the list (all created objects)
      delete item;
   }

   _languageNameStrings.clear();

   // Clean up the base clase stuff
   DynamicListbox::deallocate_resources();
}

void LanguageListbox::updateListboxStrings ()
{
   for (int i = 0; i < _numberOfButtons; i++)
   {
      LanguageNameContainer* lang = ((LanguageNameContainer*)((*_listboxItems)[i + _topItem]));

      if (lang == NULL)
      {
         // Reached the end of the list
         break;
      }

      const int fontIndex = lang->getReturnIndex();
      const int fontSize  = lang->getTextItemPtr()->getStylingRecord().fontSize;

      DataLog (log_level_gui_info) << "Setting language button text to \"" << lang->langName
                                   << " font index to " << fontIndex
                                   << " font size to " << fontSize << endmsg;

      // These are safe because the vector is never inserted into (reordered).
      buttons[i]->setFont(fontIndex, fontSize);
      buttons[i]->set_text_direct(lang->getTextItemPtr());
   }
}

/* FORMAT HASH f8f6b61c5a63b73620e4c279793f2b1c */

/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 GAMBRO BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  DynamicListboxItem

*     File name:   dynamicListboxItem.hpp

*     Contents:    Header file for list item base class

*     Description: This is a very simple pure virtual base class that you
        should inherit from if you want an item to be enumerated out into
        a listbox.  It just guarantees that child classes will be able to
        come up with a text item to show on the button, as well as an
        integer return value.

*************************< FOCUSSED GUI DOMAIN >****************************

*/

#ifndef _DYNAMIC_LISTBOX_ITEM_HPP_
#define _DYNAMIC_LISTBOX_ITEM_HPP_

#include <vector.h>

#include "trima_text_item.h"
#include "styling_record.h"

class DynamicListboxItem
{
public:
   virtual TrimaTextItem* getTextItemPtr () = 0;
   virtual int            getReturnIndex () = 0;

   virtual ~DynamicListboxItem() {}

};

typedef vector<DynamicListboxItem*> DynamicListboxVector;
typedef DynamicListboxVector::iterator DynamicListboxIterator;


////////////////////////////////////////////////////////////////////////////////
//
// This is a simple container class that holds a list of TrimaTextItems containing the
// list of languages available on the Trima (drop in language list).
//
class LanguageNameContainer
   : public DynamicListboxItem
{
public:
   LanguageNameContainer()
      : fontIndex(-1),
        LanguageIndex(-1)
   { }

   LanguageNameContainer(const char* name, const StylingRecord& rec, const int index, const int languageIndex)
      : fontIndex(index),
        LanguageIndex(languageIndex)
   {
      langName.setId(name);
      langName.setText(name);
      langName.setStylingRecord(rec);
   }

   LanguageNameContainer (const LanguageNameContainer& rhs)
   {
      fontIndex     = rhs.fontIndex;
      LanguageIndex = rhs.LanguageIndex;

      langName      = rhs.langName;
   }

   virtual ~LanguageNameContainer() {}

   virtual TrimaTextItem* getTextItemPtr () { return &langName; }
   virtual int            getReturnIndex () { return fontIndex; }

   int           LanguageIndex;      // Language table index
   int           fontIndex;          // Font for the Language Name string
   TrimaTextItem langName;           // Language Name string
};



#endif // #ifndef __DYNAMIC_LISTBOX_ITEM_HPP__

/* FORMAT HASH 08f86098bfb29ab84c9c418d4ef2b3fc */

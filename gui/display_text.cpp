/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Display_Text

*     File name:   disptxt.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/display_text.cpp 1.51 2009/07/23 19:27:35Z dslausb Exp dslausb $

*/

#include <vxWorks.h>
#include "display_text.h"
#include "guiglobs.hpp"
#include "guipalette.h"
#include "../os/trima_options.h"
#include "stringError.h"
#include "string_data.h"
#if CPU == SIMNT
#include "stringError.h"
#endif

extern StringTable stringTable[];

// Color translation information
OSColor Display_Text::color_table[LAST_TEXT_COLOR] = {0};
bool    Display_Text::color_table_initialized = false;


/////////////////////////////////////////////////////////////////////////////////
// pure virtual destructors must have definition.
BlinkableObject::~BlinkableObject() {}

/////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORs
//

Display_Text::Display_Text(TEXT_STRING_ID string_id = textEmpty)
   : OSText(*GuiMain::getDisplay(),  getTextItemPtr(string_id), TRANSPARENT),
     _previousLanguage(guiglobs::cdsData.rwConfig.getLanguageIndex()),
     _allocated(false)
{
   if (!color_table_initialized)
   {
      initialize_color_table();
   }

   set_string_id(string_id);
}  // END of Display_Text CONSTRUCTOR

void Display_Text::set_string_id (TEXT_STRING_ID string_id)
{
   _currentId = string_id;

   TrimaTextItem* txtItemPtr = getTextItemPtr(string_id);
   StylingRecord  style      = txtItemPtr->getStylingRecord();

   setText(txtItemPtr);
   setStylingRecord(&style);    // forces a recompute of region
}

// Load ONLY the location information from a string ID.
// (disregard string, attributes, color, font ID, font size, etc)
void Display_Text::set_location_via_id (TEXT_STRING_ID string_id)
{
   // Don't change the string ID, since we're disregarding most of this info.
   // _currentId = string_id;

   TrimaTextItem* txtItemPtr = getTextItemPtr(string_id);
   StylingRecord  style      = txtItemPtr->getStylingRecord();
   set_requested_region(style.region);  // forces a recompute of region
}

// Load ONLY the string information from a string ID.
// (disregard location, attributes, color, font ID, font size, etc)
void Display_Text::set_text_via_id (TEXT_STRING_ID string_id)
{
   // Don't change the string ID, since we're disregarding most of this info.
   // _currentId = string_id;

   TrimaTextItem* txtItemPtr = getTextItemPtr(string_id);
   _forceCompute = true;  // forces a recompute of region
   setText(txtItemPtr);
}



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Display_Text::~Display_Text()
{
   deallocate_resources();
} // END of Display_Text DESTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES

void Display_Text::allocate_resources (OSWindow& parent)
{
   _allocated = true;

   if (_owner != &parent)
   {
      //
      // Display_Text objects can have only one owner (i.e. be displayed on
      // only on window).
      //
      if (_owner)
      {
         //
         // Delete from previous owner
         //
         _owner->delete_object(this);
      }

      //
      // Text is never clipped by other window objects, so order isn't
      // important here, add_object_to_front was chosen arbitrarily.
      //

      parent.add_object_to_front(this);
   }
} // End of Display_Text ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE RESOURCES

void Display_Text::deallocate_resources ()
{
   _allocated = false;

   if (_owner)
   {
      _owner->delete_object(this);
   }
}  // END of Display_Text DEALLOCATE RESOURCES


////////////////////////////////////////////////////////////////////////////////
// GET_TEXT_DIRECT

void Display_Text::get_text_direct (guistring& buffer_ptr)
{
   // Get the uncode string and then convert it to UTF8 and then copy that to 'buffer_ptr'.
   TrimaUnicodeString textDirectString = getTextObj();
   string             utf8DirectString = textDirectString.getUTF8();
   buffer_ptr = utf8DirectString.c_str();
}  // END of Display_Text GET_TEXT_DIRECT


void Display_Text::get_text_direct (char* result)
{
   // Create a guistring buffer and then call 'get_text_direct(guistring&)' and strcpy that into 'result'
   guistring buffer;
   get_text_direct(buffer);
   strcpy(result, buffer);
}  // END of Display_Text GET_TEXT_DIRECT


int Display_Text::get_text_length () const { return getLength(); }

////////////////////////////////////////////////////////////////////////////////
// SET_TEXT_DIRECT  (assumed single byte inputs)

void Display_Text::set_text_direct (int direct_len, const char* direct_text)
{
   setText(direct_text);
}  // END of Display_Text SET_TEXT_DIRECT


////////////////////////////////////////////////////////////////////////////////
// SET_FONT_COLOR

void Display_Text::set_color (TEXT_COLOR set_text_color)
{
   setColor(color_table[set_text_color]);
}  // END of Display_Text SET_FONT_COLOR


////////////////////////////////////////////////////////////////////////////////
// SET_ALIGNMENT

void Display_Text::set_alignment (TEXT_ALIGN set_text_alignment)
{
   // get the current attributes and then apply alignment
   unsigned int attributes = getAttributes();

   switch (set_text_alignment)
   {
      case LEFT :
         attributes = attributes | HJUSTIFY_LEFT;
         break;

      case RIGHT :
         attributes = attributes | HJUSTIFY_RIGHT;
         break;

      case CENTER :
         attributes = attributes | HJUSTIFY_CENTER;
         break;
   }

   setAttributes(attributes);
}   // END of Display_Text SET_ALIGNMENT


////////////////////////////////////////////////////////////////////////////////
// SET_FONT_SIZE

void Display_Text::set_font_size (int set_font_size)
{
   setFontSize(set_font_size);
}  // END of Display_Text SET_FONT_SIZE


////////////////////////////////////////////////////////////////////////////////
// SET_TEXT_LANGUAGE

void Display_Text::set_text_language (CONFIG_DATA_LISTBOX_STRINGS config_language)
{
   // TODO: WE NEED TO WORK ON THIS HERE STUFFS.
   // setLanguage(config_language);
}  // END of Display_Text SET_TEXT_LANGUAGE

void Display_Text::set_configuration_language (CONFIG_DATA_LISTBOX_STRINGS config_language)
{
   DataLog (log_level_gui_info) << "set_configuration_language(" << config_language << ");" << endmsg;
   // TODO: Make use of this config variable, and figure out
   // what distinguishes this function from set_language.
}


void Display_Text::initialize_color_table ()
{
   memset(color_table, 0, sizeof(color_table));
   color_table[GREY_DARK]  = guipalette::lookupColor(74, 74, 74);
   color_table[GREY_LIGHT] = guipalette::lookupColor(239, 239, 239);
   color_table[YELLOW]     = guipalette::lookupColor(255, 247, 57);
   color_table[WHITE]      = guipalette::lookupColor(255, 255, 255);
   color_table[BLACK]      = guipalette::lookupColor(0, 0, 0);
   color_table[RED]        = guipalette::lookupColor(239, 8, 0);
   color_table[MAROON]     = guipalette::lookupColor(165, 8, 16);
   color_table[NAVY_BLUE]  = guipalette::lookupColor(49, 8, 123);

   color_table_initialized = true;
}

TrimaTextItem* Display_Text::getTextItemPtr (TEXT_STRING_ID string_id)
{
   // This function operates on the assumption that the files string_id.h, string_id.cpp,
   // string_data.h, and string_data.cpp are generated from string.info using the sed scripts
   // in engr_tools, and therefore we can count on them to be in the exact right order to
   // allow us to index the language tables with the enumeration value.
   // If this assumption gets violated (i.e. those source files are edited directly instead
   // of generated), you'll end up with a wiggy text table and all sorts of unruly shenanigans.
   // --D.S. (x4664)

   // TEXT_NULL and TEXT_DEF_END are nonsense.  If anybody tries to use them, or anything
   // above or below them, for that matter, return textEmpty instead.
   if (string_id <= TEXT_NULL || string_id >= TEXT_DEF_END )
      return getTextItemPtr(textEmpty);
   else return &(stringTable[((int)string_id) - 1].item);
}

unsigned int Display_Text::getVerticalJustification ()
{
   if (_stylingRecord.attributes & VJUSTIFY_CENTER) return VJUSTIFY_CENTER;
   else if (_stylingRecord.attributes & VJUSTIFY_BOTTOM)
      return VJUSTIFY_BOTTOM;
   else return VJUSTIFY_TOP;
}

unsigned int Display_Text::getHorizontalJustification ()
{
   if (_stylingRecord.attributes & HJUSTIFY_CENTER) return HJUSTIFY_CENTER;
   else if (_stylingRecord.attributes & HJUSTIFY_RIGHT)
      return HJUSTIFY_RIGHT;
   else return HJUSTIFY_LEFT;
}

void Display_Text::blink (bool visible) { set_visible(visible); }
void Display_Text::reset () { set_visible(true); }
void Display_Text::blinkInit () { /* Do Nothing */ return; }

DataLog_Stream& operator << (DataLog_Stream& os, const Display_Text& text)
{
#if 0   // DEBUG ONLY
   TrimaUnicodeString textDirectString = text.getTextObj();
   string             utf8DirectString = textDirectString.getUTF8();

   return os << "[str=\"" << utf8DirectString.c_str() << "\"; visible=" << text.is_visible()
             << "; allocated=" << text.allocated() << "; color=(r"
             << (int)guiglobs::palette[text.getColor()].red << ";g"
             << (int)guiglobs::palette[text.getColor()].green << ";b"
             << (int)guiglobs::palette[text.getColor()].blue << "); region="
             << text.getRegion() << "]";
#else
   return os;
#endif
}

/* FORMAT HASH bb71acb5226606e77427e443e007cb39 */

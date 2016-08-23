/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Display_Text

*     File name:    disptxt.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class is the base class for all displayable text.  Methods exist to
         set or alter the following properties of the text: logical text string,
         font, font size, font color, flow dimensions on screen, alignment,
         language ID, direct loading of text string (not recommended).

         The setting of language is done by a class method / data member.  Once it
         is set, all futher logical text string ID's will draw from the currenting
         set language.

         Note: On 2/13/08, we migrated to the string.info runtime string system.
         At this point, we switched to a much more common-gui way of doing this.
         This class, now, is mostly just a wrapper for the OSText class, which is
         a Trimafied version of the CGUIText class in common gui.  This keeps us
         from having to go change out all the Display_Text objects scattered througout
         the whole GUI. -Dan Slauson

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$

*/

// Single include define guardian
#ifndef DISPTXT_HPP
#define DISPTXT_HPP

#include <vxworks.h> // Needed for SIMNT compiler directives
#include <vector>

#include "guistring.h"
#include "ostext.h"
#include "cfg_ids.h"
#include "string_info.h"


class BlinkableObject
{
public:
   virtual ~BlinkableObject()        = 0;
   virtual void blinkInit ()         = 0;
   virtual void blink (bool visible) = 0;
   virtual void reset ()             = 0;
};


class Display_Text
   : public OSText, public BlinkableObject
{
public:
   // CONSTRUCTOR
   // Accept the text string identifier at construction for setup at allocation
   Display_Text(TEXT_STRING_ID string_id = textEmpty);
   void set_string_id (TEXT_STRING_ID string_id);

   // This allows us to set the text styling from a string ID, without loading new text.
   void set_location_via_id (TEXT_STRING_ID string_id);

   // This allows us to set the text from a string ID, without loading new styling and attributes.
   void set_text_via_id (TEXT_STRING_ID string_id);

   // DESTRUCTOR
   virtual ~Display_Text();

   // allocate/deallocate
   void allocate_resources (OSWindow& parent);
   void deallocate_resources ();

   // direct text functions
   void get_text_direct (guistring& buffer_ptr);
   void get_text_direct (char* buffer);
   int  get_text_length () const;

   // note: the first of these doesn't acutally use the length parameter, and is only retained for
   // API compatibility; it should be deprecated in favor of the second.
   void set_text_direct (int direct_len, const char* direct_text);
   void set_text_direct (const char* direct_text) { setText(direct_text); }

   void set_text_language (CONFIG_DATA_LISTBOX_STRINGS config_language);

   // Attributes are generally retrieved from the styling record
   // specified in the constructor, or through the set_text_id
   // function.  Calling on of the functions defined below to set
   // an attribute is treated as an override to the default, causing
   // the corresponding sytling record value to be ignored, even if
   // set_text_id is called to select a different styling record.

   void set_color (TEXT_COLOR set_text_color);
   void set_alignment (TEXT_ALIGN set_text_alignment);
   void set_font_size (int set_font_size);

   virtual void blinkInit ();
   virtual void blink (bool visible);
   virtual void reset ();

private:
   // LOAD_LANGUAGE_TABLE_INFORMATION
   // Search a specific language table for an entry and copy all the
   // string attributes to this object.
   static void initialize_color_table ();

   static TrimaTextItem* getTextItemPtr (TEXT_STRING_ID string_id);

   // Store the font index that was used on the last allocation of this
   // object.  This is used to refresh upon a font change.  Initialized to -1,
   // and then set each subsequent allocation.
   int  _previousLanguage;

   bool _allocated;

public:
   // get/set current language - applies to all Display_Text objects
   // Note that setting the current language will force all existing Display_Text
   // objects to be re-initialized when they are next drawn.
   static void set_configuration_language (CONFIG_DATA_LISTBOX_STRINGS config_language);

   unsigned int getVerticalJustification ();
   unsigned int getHorizontalJustification ();

   friend DataLog_Stream& operator << (DataLog_Stream& os, const Display_Text& text);

   bool allocated () const { return _allocated; }

private:
   static OSColor color_table[LAST_TEXT_COLOR];
   static bool    color_table_initialized;
}; // END of Display_Text Class Declaration

DataLog_Stream& operator << (DataLog_Stream& os, const Display_Text& text);

// End of single include define guardian
#endif /* ifndef DISPTXT_HPP */

/* FORMAT HASH a9bd1cd3661fb88bcde922cde8e073ed */

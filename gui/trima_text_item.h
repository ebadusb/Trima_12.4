/*
 *  Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 *
 *
 */

#ifndef __TRIMA_TEXT_ITEM_H__
#define __TRIMA_TEXT_ITEM_H__

#include <string>

#include "language_hdr.h"
#include "styling_record.h"
#include "unicode_string/trima_unicode_string.h"

#ifndef __ALARM_TABLE_BUILD__ // Don't include this if this is an alarm table generator tool build
#include "trima_datalog.h"
#endif


//
// This class will access the strings from the database
// or the file that holds all of the string information
// (both string id and the actual string text).  The
// string id is the key to accessing this information.
//
class TrimaTextItem
{
public:
   TrimaTextItem(void);
   TrimaTextItem(const char* id, StylingRecord* stylingRecord = NULL);
   TrimaTextItem(const TrimaTextItem& textItem);

   virtual ~TrimaTextItem();

   TrimaTextItem& operator = (const TrimaTextItem& textItem);

   inline bool operator == (const TrimaTextItem& textItem) { return (_id == textItem._id); }

#ifndef __ALARM_TABLE_BUILD__ // Don't include this if this is an alarm table generator tool build
   friend DataLog_Stream& operator << (DataLog_Stream& os, TrimaTextItem& txtItem);
#endif // #ifndef __ALARM_TABLE_BUILD__

   // Set the string's database ID
   void        setId (const char* id);
   const char* getId (void) const;

   // Returns a unicode string object of the string
   const TrimaUnicodeString& getTextObj ();

   // Get a UTF8 (ASCII) representation of the string
   void getAscii (string& myString);

   // Helper function that looks up a TrimaTextItem for the given ID.
   // Returns NULL if the item is not found.
   static TrimaTextItem* getTextItem (const char* id);

   // Set the text for this TrimaTextItem using a UTF8 string. The UTF8
   // string is converted to wide character unicode.
   void setText (const char* utf8String);

   // Set the text for this TrimaTextItem using a wide character string.
   void setText (const TrimaStringChar* texString);

   // Get the language ID
   inline LanguageId getLanguageId (void) const { return _languageId; }

   // Set the language ID or default language ID
   inline void setLanguageId (LanguageId languageId) { _languageId = languageId; }

   // Returns true if the text item ID is set
   bool isInitialized (void) const;

   // Set the GUI styling record.
   inline void setStylingRecord (StylingRecord stylingRecord) { _stylingRecord = stylingRecord; }

   // Get the GUI styling record.
   inline StylingRecord getStylingRecord (void) const { return _stylingRecord; }

   // Returns the length of the string in characters
   int getLength (void) const;

   // Returns the length of the string in characters and
   // performs variable substitution.
   int getStringCharVariableLength (void);

   // Perform variable substitution in the text.
   void handleVariableSubstitution (void);

   inline bool isLoaded () { return _wasLoaded; }
   inline void loaded () { _wasLoaded = true; }
   inline void unload () { _wasLoaded = false; }

   inline bool isEnglishOnly () { return _englishOnly; }
   inline void englishOnly (bool value) { _englishOnly = value; }


private:
   string             _id;               // Non-translated internal string ID
   TrimaUnicodeString _string;           // String with variables substituted
   TrimaUnicodeString _template;         // String template with variable placeholders
   LanguageId         _languageId;       // Language ID
   StylingRecord      _stylingRecord;    // GUI styling information
   bool               _hasVariables;     // Indicates that the template cantains variables
   bool               _wasLoaded;        // Indicates that the string was loaded from the .info file
   bool               _englishOnly;      // Indicates that the string does not require localization
};

#endif /* #ifndef __TRIMA_TEXT_ITEM_H__ */

/* FORMAT HASH 52efd784c4b015ae7d87d63341af81ef */

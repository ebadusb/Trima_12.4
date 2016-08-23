/*
 * Copyright (c) 2012 by Terumo BCT, Inc.  All rights reserved.
 *
 * Author:  Regis McGarry
 *
 * Description:
 *  This class encapsulates the static part of the OSText class which is basically
 * the font list
 *
 */

#ifndef __OSTEXTBASE_H__
#define __OSTEXTBASE_H__

#include <list>

#include "unicode_string/trima_unicode_string.h"
#include "language_hdr.h"

struct FontRange
{
   int    startIndex;
   int    endIndex;
   int    fontIndex;
   string fontName;

   FontRange(void)
      : startIndex(0), endIndex(0), fontIndex(0) { }

   FontRange(int start, int end, int font, const char* name)
      : startIndex(start), endIndex(end), fontIndex(font), fontName(name) { }

   // needed for tree function
   bool operator == (const FontRange& fontRange) const;
   bool operator < (const FontRange& fontRange) const;
   bool operator > (const FontRange& fontRange) const;
};


typedef list<FontRange*> FONT_RANGE_LIST;
typedef FONT_RANGE_LIST::iterator FONT_RANGE_LIST_ITER;

class OSTextBase
{
public:
   OSTextBase() {}
   virtual ~OSTextBase();

   // Set tab spacing - this is global for all strings and must be set before formating
   // the first string for display
   //
   static void setTabSpacing (unsigned short spaceCount) { _tabSpaceCount = spaceCount; }

   //   Add a font range to list.
   //
   static bool addFontRange (FontRange* fontRange);

   //   Clear font range list.
   //
   static void clearFontRange (void);

   static void selectTokenSplitMethod (TokenSplitMethod method);
   static void initializeForbiddenChars (bool startCharPresent, bool endCharPresent, const char* startCharList, const char* endCharList);

   static FONT_RANGE_LIST_ITER begin () { return _fontRange.begin(); }
   static FONT_RANGE_LIST_ITER end () { return _fontRange.end(); }

   static int checkInFontRange (int currentChar);

protected:
   static TokenSplitMethod _tokenSplitMethod;

   static unsigned short   _tabSpaceCount;

   static FONT_RANGE_LIST  _fontRange; //  list of unicode ranges with associated font name and index of font

   // Forbidden Char lists are present in string.info files
   // If present, they are read only once and then shared across all OStext objects

   static TrimaUnicodeString _forbiddenStartCharList;
   static bool               _forbiddenStartCharsAvailable;

   static TrimaUnicodeString _forbiddenEndCharList;
   static bool               _forbiddenEndCharsAvailable;
};


#endif

/* FORMAT HASH d95a83288e3033020979e767765dfac8 */

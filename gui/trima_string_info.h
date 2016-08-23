/*
 *	Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 *
 *
 */

#ifndef __TRIMA_STRING_INFO__
#define __TRIMA_STRING_INFO__

#include <stdio.h>
#include <taskLib.h>


#include "trima_text_item.h"
#include "language_loader.h"


//
// This class extracts the language strings from the language file
//
class TrimaStringInfo
   : public LanguageFile
{
public:
   TrimaStringInfo(const char* filename, bool delayRead = true);
   TrimaStringInfo();
   virtual ~TrimaStringInfo(void);

   bool getNextEntry (TrimaTextItem& result, int fontIndex);

   int line () { return _line; }

   bool endOfFile (void) { return !_readingFileTable; }  // If false after a get, no more records, or searched for key and not found.

private:
   void UTF8ToUnicode (char*& data, TrimaStringChar* wString, int& writeIdx);
   bool getQuotedString (char*& data, TrimaStringChar*& str);

   bool getStylingRecord (char* p, TrimaTextItem& result, const int fontIndex = -1);

   void pauseReading () { if (_taskDelay) taskDelay(_taskDelayTime); }

private:
   bool         _taskDelay;
   unsigned int _taskDelayTime;
};

#endif // ifndef __TRIMA_STRING_DATA_H__

/* FORMAT HASH cd595f3fed2a806dd7dc6bbf386c11c6 */

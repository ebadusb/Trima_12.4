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

#include "ostext_base.h"
#include "trima_datalog_levels.h"



unsigned short     OSTextBase::_tabSpaceCount                = 5;

TokenSplitMethod   OSTextBase::_tokenSplitMethod             = WordBased;

bool               OSTextBase::_forbiddenStartCharsAvailable = false;
bool               OSTextBase::_forbiddenEndCharsAvailable   = false;

TrimaUnicodeString OSTextBase::_forbiddenStartCharList;
TrimaUnicodeString OSTextBase::_forbiddenEndCharList;

FONT_RANGE_LIST    OSTextBase::_fontRange;



OSTextBase::~OSTextBase()
{}



bool OSTextBase::addFontRange (FontRange* fontRange)
{
   // future add logic to insert in list in sorted order.
   _fontRange.push_front(fontRange);

   return true;
}



void OSTextBase::clearFontRange ()
{
   if (_fontRange.size() > 0)
   {
      FONT_RANGE_LIST_ITER fontRangeIter = _fontRange.begin();
      FontRange*           fontRange     = NULL;

      while (fontRangeIter != _fontRange.end())
      {
         fontRange = ( *fontRangeIter );

         // go to next item in list
         fontRangeIter++;

         // delete pointer to fontRange struct
         delete fontRange;
      } // while not end of _fontRange list do
   }

   _fontRange.clear();
}



void OSTextBase::selectTokenSplitMethod (TokenSplitMethod method)
{
   _tokenSplitMethod = method;

   if (method == WordBased)
   {
      DataLog(log_level_gui_info) << "WordBased token split algorithm selected" << endmsg;
   }
   else if (method == CharBased)
   {
      DataLog(log_level_gui_info) << "CharBased token split algorithm selected" << endmsg;
   }
   else
   {
      DataLog(log_level_gui_error) << "WARNING:  Undefined split algorithm detected.  WordBased token split algorithm selected" << endmsg;
   }
}



void OSTextBase::initializeForbiddenChars (bool startCharPresent, bool endCharPresent, const char* startCharList, const char* endCharList)
{
   if (startCharPresent)
   {
      _forbiddenStartCharList       = startCharList;
      _forbiddenStartCharsAvailable = true;
      DataLog(log_level_gui_info) << "Found " << strlen(startCharList) << " forbiddenStartCharList characters in .info file" << endmsg;
   }
   else
   {
      DataLog(log_level_gui_info) << "Couldn't find forbiddenStartCharList in .info file" << endmsg;
   }

   if (endCharPresent)
   {
      _forbiddenEndCharList       = endCharList;
      _forbiddenEndCharsAvailable = true;
      DataLog(log_level_gui_info) << "Found " << strlen(endCharList) << " forbiddenEndCharList characters in .info file" << endmsg;
   }
   else
   {
      DataLog(log_level_gui_info) << "Couldn't find forbiddenEndCharList in .info file" << endmsg;
   }
}



int OSTextBase::checkInFontRange (int currentChar)
{
   if (_fontRange.size() > 0)
   {
      for (FONT_RANGE_LIST_ITER fontRangeIter = _fontRange.begin();
           fontRangeIter != _fontRange.end(); fontRangeIter++)
      {
         const FontRange* fontRange = *fontRangeIter;
         if (currentChar >= fontRange->startIndex &&
             currentChar <= fontRange->endIndex)
         {
            return fontRange->fontIndex;
         }
      } // while not end of _fontRange list do
   }

   return -1;
}

/* FORMAT HASH 59ed3a55afea5974b12416b91c7dc702 */

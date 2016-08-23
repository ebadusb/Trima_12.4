/*
 * Copyright (c) 2004 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>
#include "trima_string_data.h"
#include "trima_string_info.h"
#include "trima_datalog_levels.h"
#include "ostext.h"

bool     TrimaStringData::_firstLoad = true;

TEXT_MAP TrimaStringData::_textMap;


bool TrimaStringData::loadLanguage (const LanguageHeader* lang)
{
   TrimaTextItem   textItem;
   TrimaStringInfo stringInfo(lang->fileName.c_str(), false);
   bool            result = true;

   DataLog(log_level_gui_info) << "Clearing out old font ranges" << endmsg;
   OSText::clearFontRange();

   if (!_firstLoad)
   {
      // Clear the text map for a new language
      unloadLanguage();
   }

   _firstLoad = false;

   // Loop reading string info records, converting them to CGUTextItems,
   // and putting them in their proper places in the text map.
   while (stringInfo.getNextEntry(textItem, lang->fontIndex))
   {
      // Do the lookup thing for the textItem.
      //
      TrimaTextItem* pTextItem = findString(textItem.getId());

      // If found the corresponding text item in map,
      // correct the formatting sequences in the text.
      if (pTextItem)
      {
         // Update the text item information in corresponding _textMap[id].
         //
         *pTextItem = textItem;
         pTextItem->setLanguageId(lang->languageIndex);
         pTextItem->loaded();         // mark the string as loaded
      }
      else
      {
         DataLog(log_level_gui_error) << "line " << stringInfo.line() << ": Can't find string Id " << textItem.getId() << " in map!!!!! - " << lang->fileName << endmsg;
         result = false;
      }

   }

   OSTextBase::initializeForbiddenChars(lang->forbiddenStartCharInitialized,
                                        lang->forbiddenEndCharInitialized,
                                        lang->forbiddenStartChar.c_str(),
                                        lang->forbiddenEndChar.c_str());

   OSTextBase::selectTokenSplitMethod(lang->tokenSplitMethod);


   // Check for premature end.
   if (!stringInfo.endOfFile())
   {
      DataLog(log_level_gui_error) << "line " << stringInfo.line() << ": bad entry in string info file - " << lang->fileName << endmsg;
      result = false;
   }

   // Close file.
   stringInfo.close();

   return result;
}


TrimaTextItem* TrimaStringData::findString (const char* id)
{
   TrimaTextItem* result = NULL;
   TEXT_MAP_ITER  iter   = _textMap.find(id);

   if (iter != _textMap.end())
   {
      result = iter->second;

      if (strcmp(result->getId(), id) != 0)
      {
         // This should be a fatal error right?
         DataLog(log_level_gui_error) << "Didn't find text item " << id << endmsg;
         result = NULL;
      }
   }

   return result;
}



bool TrimaStringData::isAnythingMissing (LanguageId lang)
{
   bool somethingWasMissed = false;

   // If this is English, no test is necessary (by definition)
   if (lang == 0)
   {
      return somethingWasMissed;
   }

   // Iterate over the text map.
   for (TEXT_MAP_ITER iter = _textMap.begin(); iter != _textMap.end(); ++iter)
   {
      TrimaTextItem* result = iter->second;

      // 3 chances in 4 of being very very bad
      if (!result->isLoaded())
      {
         // The only good combination is if it is English only and NOT in English
         if (result->isEnglishOnly() && (lang != 0))
         {
#if 0       // Debug only
            DataLog(log_level_gui_info) << "Text item " << result->getId()
                                        << " is missing but that is OK.  EnglishOnly=" << result->isEnglishOnly()
                                        << " Loaded=" << result->isLoaded()
                                        << " languageIndex=" << result->getLanguageId()
                                        << " Loaded Language=" << lang << endmsg;
#endif
         }
         else
         {
            DataLog(log_level_gui_error) << "Text item " << result->getId()
                                         << " is missing.  EnglishOnly=" << result->isEnglishOnly()
                                         << " Loaded=" << result->isLoaded()
                                         << " languageIndex=" << result->getLanguageId()
                                         << " Loaded Language=" << lang << endmsg;

            somethingWasMissed = true;
         }
      }
   }

   return somethingWasMissed;
}




void TrimaStringData::unloadLanguage ()
{
   // Iterate over the text map.
   for (TEXT_MAP_ITER iter = _textMap.begin(); iter != _textMap.end(); ++iter)
   {
      iter->second->unload();
   }
}

/* FORMAT HASH 6e571c07ac10ffa91e139119ecd81a5b */

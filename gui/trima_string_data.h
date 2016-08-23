/*
 *	Copyright (c) 2004 by Gambro BCT, Inc.  All rights reserved.
 *
 *
 *
 */

#ifndef __TRIMA_STRING_DATA_H__
#define __TRIMA_STRING_DATA_H__

#include <string>
#include <map>

#include "language_hdr.h"
class TrimaTextItem;

typedef map<string, TrimaTextItem*> TEXT_MAP;
typedef TEXT_MAP::iterator TEXT_MAP_ITER;


class TrimaStringData
{
public:

   static TrimaTextItem* findString (const char* id);

   // This function reads the language file and copies text, styling record, etc. from .info file
   // to the _textMap
   static bool loadLanguage (const LanguageHeader* lang);

   static int languageSize () { return _textMap.size(); }

   static bool isAnythingMissing (LanguageId lang);

private:
   static void unloadLanguage ();  // resets the loaded flag for missing string check

protected:
   // Trima string map for currently loaded language
   static TEXT_MAP _textMap;
   static bool     _firstLoad;

};
#endif // __TRIMA_STRING_DATA_H__

/* FORMAT HASH b639c3f8c8833d2f1ed74d887ce3d3c7 */

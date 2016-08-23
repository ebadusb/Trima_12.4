/*
 * Copyright (c) 2012 by Terumo BCT, Inc.  All rights reserved.
 *
 * Author:  Regis McGarry
 *
 * Description:
 *  These classes and containers encapsulate reading all language files (.info) on Trima.  This class reads
 * each file header and stores it in a STL vector.  The reading happens in the sysinit task starting context
 * because of the file I/O.  This is a static (sentinel) object that is resident in Trima memory.
 *
 */


#ifndef _LANGUAGE_HEADER_H_
#define _LANGUAGE_HEADER_H_

#include <vector>
#include <string>

#include "styling_record.h"


enum TokenSplitMethod
{
   Undefined,
   WordBased,
   CharBased
};


typedef unsigned int LanguageId;

//
// This class encapsulates a language header.  It stores the language name and styling record to make
// the language list in the lang config screen work.   The TrimaTextItem class contains the actual language.
//
class LanguageHeader
{
public:
   string           fileName;
   string           fontName;
   string           langName;
   string           forbiddenStartChar;
   string           forbiddenEndChar;

   bool             forbiddenStartCharInitialized;
   bool             forbiddenEndCharInitialized;

   int              fontIndex;
   LanguageId       languageIndex;
   TokenSplitMethod tokenSplitMethod;
   unsigned long    languageCRC;

   StylingRecord    langStyling;

   LanguageHeader()
      : languageCRC(0),
        languageIndex(0),
        fontIndex(0),
        fileName(""),
        langName(""),
        fontName(""),
        forbiddenStartChar(""),
        forbiddenEndChar(""),
        tokenSplitMethod(Undefined),
        forbiddenStartCharInitialized(false),
        forbiddenEndCharInitialized(false)
   { }
};

typedef vector<LanguageHeader*> LANGUAGE_VECTOR;
typedef LANGUAGE_VECTOR::iterator LANGUAGE_VECTOR_ITERATOR;

#endif

/* FORMAT HASH e53381fde07f6d1e97eedc890b7e519b */

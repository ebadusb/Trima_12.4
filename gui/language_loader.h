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


#ifndef _LANGUAGE_LOADER_H_
#define _LANGUAGE_LOADER_H_

#include <string>
#include <stdio.h>

#include "language_hdr.h"


class LanguageFile
{
public:
   LanguageFile();
   LanguageFile(const char* filename);
   virtual ~LanguageFile();

   bool getLanguageHeader (LanguageHeader& header);

   // Open the specified file, or if NULL, the file set by constructor.
   bool open (const char* filename = NULL);
   void close ();

protected:
   bool getStylingRecord (const char* line, StylingRecord& r, int fontIndex);

protected:
   string _filename;
   FILE*  _stringInfoFile;

   enum { LineBufferSize = 4096 };

   char         _lineBuffer[LineBufferSize];
   unsigned int _line;

   bool         _readingFileTable;
   bool         _readingFontRanges;
};

//
// Static list of all languages
//
class LanguageList
{
public:
   LanguageList(){}
   virtual ~LanguageList();

   static LANGUAGE_VECTOR_ITERATOR begin ();
   static LANGUAGE_VECTOR_ITERATOR end ();
   static void                     erase (const LanguageHeader* language);

   static unsigned int size (void);

   static void addLanguage (LanguageHeader* language);

   static bool loadLanguage (const LanguageHeader* newHeader, const LanguageHeader* oldHeader = NULL);

   static const LanguageHeader* currentLanguage ();
   static void                  setCurrentLanguage (const LanguageHeader* language);

   static const LanguageHeader* getLanguageHeader (unsigned int index);

   static void read ();
};

#endif

/* FORMAT HASH f91f8e682639b1500d0e402098b2c562 */

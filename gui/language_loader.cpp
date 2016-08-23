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

#include <vxWorks.h>
#include <dirent.h>
#include <sys/stat.h>

#include "language_hdr.h"
#include "language_loader.h"
#include "trima_datalog.h"
#include "filenames.h"
#include "fontdefs.h"
#include "an_alarm.h"
#include "styling_record.h"
#include "guipalette.h"
#include "string_data.h"
#include "ostext_base.h"


#define WORD_BASED "WordBased"
#define CHAR_BASED "CharBased"

// Forward declare utility function
unsigned long getStringInfoCrc (const char* path, const char* logPath = NULL);



// Local static object
static StringData _stringData;


LanguageFile::LanguageFile()
   : _filename(),
     _stringInfoFile(NULL),
     _lineBuffer(),
     _line(0),
     _readingFileTable(false),
     _readingFontRanges(false)
{}



LanguageFile::LanguageFile(const char* filename)
   : _filename(filename),
     _stringInfoFile(NULL),
     _lineBuffer(),
     _line(0),
     _readingFileTable(false),
     _readingFontRanges(false)
{}



LanguageFile::~LanguageFile()
{
   if (_stringInfoFile != NULL)
      fclose(_stringInfoFile);
}


// open the given string.info file
bool LanguageFile::open (const char* filename)
{
   close();

   if (filename != NULL)
   {
      _filename = filename;
   }

   _stringInfoFile = fopen(_filename.c_str(), "r");

   if (_stringInfoFile == NULL)
   {
      DataLog(log_level_config_error) << _filename << " not opened." << endmsg;
      return false;
   }

   return true;
}

// close the string.info file and reset internal state
void LanguageFile::close ()
{
   if (_stringInfoFile != NULL)
   {
      fclose(_stringInfoFile);
      _stringInfoFile = NULL;
   }

   _readingFileTable  = false;
   _readingFontRanges = false;
   _line              = 0;
}



const char SEPARATORS[]          = " \t\n\r\"";
const char SEPARATORS_NO_SPACE[] = "\t\n\r\"";



//
// Get the styling record from the rest of the line in the .info file
//
bool LanguageFile::getStylingRecord (const char* line, StylingRecord& r, int fontIndex)
{
   bool         status = true;

   int          red, green, blue;
   unsigned int attributes;
   int          x, y, width, height, fontSize;

   if (sscanf(line, "%d %d %d %x %d %d %d %d %d", &red, &green, &blue, &attributes, &x, &y, &width, &height, &fontSize) != 9)
   {
      status = false;
      DataLog(log_level_config_error) << "File:" << _filename << ":line # " << _line << ": missing parameters - " << line << endmsg;
   }

   // Create Styling Record from line information.
   if (status)
   {
      r.color      = guipalette::lookupColor(red, green, blue);
      r.attributes = attributes;
      r.region     = OSRegion(x, y, width, height);
      r.fontSize   = fontSize;
      r.fontIndex  = -1;
   }

   return status;
}



//
// Get the language header from the .info file
//
bool LanguageFile::getLanguageHeader (LanguageHeader& header)
{
   bool foundReadyForRelease    = false;
   bool foundFontName           = false;
   bool foundLanguageIndex      = false;
   bool foundLanguageId         = false;
   bool foundLanguageCRC        = false;
   bool foundTokenSplitMethod   = false;
   bool foundForbiddenStartChar = false;
   bool foundForbiddenEndChar   = false;

   // Make sure the file is open and ready to read
   if ( !open() )
   {
      return false;
   }

   header.fileName = _filename.c_str();

   while (fgets(_lineBuffer, LineBufferSize, _stringInfoFile) != NULL)
   {
      _line++;

      char* restOfLine = NULL;
      char* firstToken = strtok_r(_lineBuffer, SEPARATORS, &restOfLine);

      // skip comments
      if (!firstToken || firstToken[0] == '#'|| firstToken[0] == '[')
         continue;

      // Start paying attention when we see the start of the string table
      if (!_readingFileTable && strcmp(firstToken, "STRING_FILE_TABLE_START") == 0)
      {
         _readingFileTable = true;
         continue;
      }

      // skip everything until we get in the string table
      if (!_readingFileTable)
         continue;

      if (strcmp(firstToken, "readyForRelease") == 0)
      {
         char* value = strtok_r(NULL, SEPARATORS, &restOfLine);

         if (value == NULL)
         {
            // Bad value in header
            DataLog(log_level_config_error) << "In " << _filename
                                            << " token readyForRelease could not be read at line " << _line << endmsg;

            LanguageFile::close();

            return false;
         }

         // If language is not ready, abandon reading the rest of the header
         if (!atoi(value))
         {
            DataLog(log_level_config_error) << "In " << _filename
                                            << " language is not ready at line " << _line << endmsg;

            LanguageFile::close();

            return false;
         }

         foundReadyForRelease = true;
         continue;
      }
      else if (strcmp(firstToken, "fontName") == 0)
      {
         // Get first word after the quote
         char* value = strtok_r(NULL, SEPARATORS, &restOfLine);

         if (value == NULL)
         {
            // Bad value in header
            DataLog(log_level_config_error) << "In " << _filename
                                            << " token fontName could not be read at line " << _line << endmsg;

            LanguageFile::close();

            return false;
         }

         // Add first word to the string
         header.fontName = value;

         // Get rest of line to the ending quote
         value            = strtok_r(NULL, SEPARATORS_NO_SPACE, &restOfLine);
         header.fontName += " ";
         header.fontName += value;

         header.fontIndex = getFontIndex(header.fontName.c_str());

         if (header.fontIndex < 0 || header.fontIndex >= MaxMyFonts)
         {
            DataLog(log_level_config_error) << "ERROR: font string \"" << header.fontName
                                            << "\" returned nonsensical index " << header.fontIndex << ". Font does not exist." << endmsg;

            LanguageFile::close();

            return false;
         }

         foundFontName = true;
         continue;
      }
      else if (strcmp(firstToken, "languageIndex") == 0)
      {
         char* value = strtok_r(NULL, SEPARATORS, &restOfLine);

         if (value == NULL)
         {
            // Bad value in header
            DataLog(log_level_config_error) << "In " << _filename
                                            << " token languageIndex could not be read at line " << _line << endmsg;

            LanguageFile::close();

            return false;
         }

         header.languageIndex = atoi(value);
         foundLanguageIndex   = true;
         continue;
      }
      else if (strcmp(firstToken, "languageId") == 0)
      {
         char* value = strtok_r(NULL, SEPARATORS, &restOfLine);

         // languageId is different from the rest of the header tokens.
         // It actuall contains a styling record so call getStylingRecord to extract the
         // styling record info.
         const int     fontIndex = (foundFontName ? header.fontIndex : -1);

         StylingRecord stylingRec;

         if (!getStylingRecord(restOfLine, stylingRec, fontIndex))
         {
            // Bad value in header
            DataLog(log_level_config_error) << "In " << _filename
                                            << " token languageId could not be read at line " << _line << endmsg;

            LanguageFile::close();

            return false;
         }

         foundLanguageId = true;

         // Save off the info from the language header for building the list of languages.
         header.langName    = value;
         header.langStyling = stylingRec;

         continue;
      }
      else if (strcmp(firstToken, "languageCrc") == 0)
      {
#if CPU!=SIMNT // In simulator, don't bother with lang CRCs so as not to impede development.
         char* value = strtok_r(NULL, SEPARATORS, &restOfLine);

         if (value == NULL)
         {
            // Bad value in header
            DataLog(log_level_config_error) << "In " << _filename
                                            << " token languageCrc could not be read at line " << _line << endmsg;

            LanguageFile::close();

            return false;
         }

         const unsigned long languageCRC   = strtoul(value, NULL, 16);
         const unsigned long calculatedCRC = getStringInfoCrc(_filename.c_str());

         if (languageCRC != calculatedCRC)
         {
            // CRC doesn't match calculated value
            DataLog(log_level_config_error) << "In " << _filename
                                            << " languageCrc (0x" << hex << languageCRC
                                            << ") does not match calculated value of (0x"
                                            << calculatedCRC << ")." << dec << endmsg;

            LanguageFile::close();

            return false;
         }
#endif

         foundLanguageCRC = true;
         continue;
      }
      else if (strcmp(firstToken, "languageWrappingAlgorithm") == 0)
      {
         char* value = strtok_r(NULL, SEPARATORS, &restOfLine);

         if (value == NULL)
         {
            // Bad value in header
            DataLog(log_level_config_error) << "In " << _filename
                                            << " token languageWrappingAlgorithm could not be read at line " << _line << endmsg;

            LanguageFile::close();

            return false;
         }

         if (strcmp(value, WORD_BASED) == 0)
         {
            header.tokenSplitMethod = WordBased;
         }
         else if (strcmp(value, CHAR_BASED) == 0)
         {
            header.tokenSplitMethod = CharBased;
         }
         else
         {
            // Bad value in header
            DataLog(log_level_config_error) << "In " << _filename
                                            << " token languageWrappingAlgorithm contained unexpected value "
                                            << value << "at line " << _line << endmsg;

            LanguageFile::close();

            return false;
         }

         foundTokenSplitMethod = true;
         continue;
      }
      else if (strcmp(firstToken, "forbiddenStartCharList") == 0)
      {
         char* value = strtok_r(NULL, SEPARATORS, &restOfLine);

         if (value == NULL)
         {
            // Bad value in header
            DataLog(log_level_config_error) << "In " << _filename
                                            << " token forbiddenStartCharList could not be read at line " << _line << endmsg;

            LanguageFile::close();

            return false;
         }

         header.forbiddenStartChar            = value;
         header.forbiddenStartCharInitialized = true;
         foundForbiddenStartChar              = true;

         continue;
      }
      else if (strcmp(firstToken, "forbiddenEndCharList") == 0)
      {
         char* value = strtok_r(NULL, SEPARATORS, &restOfLine);

         if (value == NULL)
         {
            // Bad value in header
            DataLog(log_level_config_error) << "In " << _filename
                                            << " token forbiddenStartCharList could not be read at line " << _line << endmsg;

            LanguageFile::close();

            return false;
         }

         header.forbiddenEndChar            = value;
         header.forbiddenEndCharInitialized = true;
         foundForbiddenEndChar              = true;

         continue;
      }
      else if (foundReadyForRelease    &&   // No sense continuing to read the file if we are done.
               foundFontName           &&
               foundLanguageIndex      &&
               foundLanguageId         &&
               foundLanguageCRC        &&
               foundTokenSplitMethod   &&
               foundForbiddenStartChar &&
               foundForbiddenEndChar)
      {
         DataLog(log_level_config_info) << "File " << _filename << " successfully processed." << endmsg;

         LanguageFile::close();
         return true;
      }
      else
         continue;
   }

   // Make sure all of the items in the language header have been found
   DataLog(log_level_config_error) << "Language File " << _filename << " has an incomplete header"
                                   << foundReadyForRelease << " " << foundFontName << " " << foundLanguageIndex << " "
                                   << foundLanguageId << " " << foundLanguageCRC << " " << foundTokenSplitMethod << " "
                                   << foundForbiddenStartChar << " " << foundForbiddenEndChar << endmsg;

   LanguageFile::close();

   return false;
}



static const LanguageHeader* _currentLanguage = NULL;
static LANGUAGE_VECTOR       _languageList;


LanguageList::~LanguageList()
{
   _languageList.clear();
}



LANGUAGE_VECTOR_ITERATOR LanguageList::begin ()
{
   return _languageList.begin();
}



LANGUAGE_VECTOR_ITERATOR LanguageList::end ()
{
   return _languageList.end();
}



void LanguageList::erase (const LanguageHeader* language)
{
   for (LANGUAGE_VECTOR_ITERATOR iter = LanguageList::begin();
        iter != LanguageList::end();
        iter++)
   {
      const LanguageHeader* lang = *(iter);

      if (lang->languageIndex == language->languageIndex)
      {
         DataLog(log_level_gui_info) << "Language index " << language->languageIndex
                                     << " was removed from language list" << endmsg;

         _languageList.erase(iter);
         delete lang;

         break;
      }
   }
}



unsigned int LanguageList::size (void)
{
   return _languageList.size();
}



void LanguageList::addLanguage (LanguageHeader* language)
{
   _languageList.push_back(language);
}



const LanguageHeader* LanguageList::currentLanguage ()
{
   return _currentLanguage;
}



void LanguageList::setCurrentLanguage (const LanguageHeader* language)
{
   _currentLanguage = language;
}



const LanguageHeader* LanguageList::getLanguageHeader (unsigned int index)
{
   for (LANGUAGE_VECTOR_ITERATOR iter = LanguageList::begin();
        iter != LanguageList::end();
        iter++)
   {
      const LanguageHeader* lang = *(iter);

      if (lang->languageIndex == index)
      {
         return lang;
      }
   }

   DataLog(log_level_gui_error) << "language index " << index << " not found" << endmsg;

   return NULL;
}


//
// Go through the language directory and find and analyze the string.info files.
// Put the metadata into a language header, unless the language doesn't check out, integrity-wise.
// Note that it is considered mandatory to have a valid English file.
// Other language files are not mandatory, but they cannot be displayed if they don't
// pass the integrity tests.
//
void LanguageList::read ()
{
   DataLog(log_level_config_info) << "Processing Font files..." << endmsg;
   initializeFontTable();

   DataLog (log_level_config_info) << "Processing language files in " << STRING_DIRECTORY << endmsg;

   //
   // Process all language files
   //
   DIR* dir = opendir(STRING_DIRECTORY);

   if (dir)
   {
      struct dirent* dirEntry = NULL;

      // Now, see what else is in there, and add those languages.
      while ((dirEntry = readdir(dir)) != NULL)
      {
         string fileName(STRING_DIRECTORY);
         fileName += '/';
         fileName += dirEntry->d_name;

         struct stat  fileStat;
         const size_t nameLen = strlen(dirEntry->d_name);

         // Only process this if it's a regular old file.
         if ( (stat((char*)fileName.c_str(), &fileStat) == OK) &&
              S_ISREG(fileStat.st_mode) &&
              (nameLen >= 10) // file will be named stringxx-xx.info.  Ignore anything shorter.
              )
         {
            DataLog(log_level_config_info) << "processing language file " << dirEntry->d_name << endmsg;

            LanguageHeader* langHeader = new LanguageHeader();   // List of languages persist throughout the run
            LanguageFile    langFile(fileName.c_str());

            if (!langFile.getLanguageHeader(*langHeader))
            {
               DataLog(log_level_config_error) << "Error processing " << fileName << endmsg;

               anAlarmMsg badStringFile(STRING_INFO_FILE_ERROR);
               badStringFile.setAlarm();

               delete langHeader;
               langHeader = NULL;

               continue;
            }

            LanguageList::addLanguage(langHeader);
         }

      } // while dirEntry

      closedir(dir);
   }
   else
   {
      DataLog (log_level_config_error) << "ERROR: Unable to open directory " << STRING_DIRECTORY << endmsg;
   }

   //
   // Log the language Header list
   //
   DataLog (log_level_config_info) << "Language headers:" << endmsg;

   for (LANGUAGE_VECTOR_ITERATOR iter = LanguageList::begin();
        iter != LanguageList::end();
        iter++)
   {
      const LanguageHeader* lang = *(iter);
      DataLog (log_level_config_info) << "---> File " << lang->fileName
                                      << ": (fontIndex=" << lang->fontIndex << "; languageIndex=" << lang->languageIndex
                                      << "; fontName=\"" << lang->fontName << "\")"
                                      << "; font size " << lang->langStyling.fontSize << " " << endmsg;
   }
}



bool LanguageList::loadLanguage (const LanguageHeader* newHeader, const LanguageHeader* oldHeader = NULL)
{
   if (!newHeader)
   {
      DataLog (log_level_gui_error) << "NULL language header passed." << endmsg;
      return false;
   }

   bool loadNewFont = true;

   if (oldHeader)
   {
      if (oldHeader->languageIndex != newHeader->languageIndex)
      {
         DataLog (log_level_gui_info) << "Changing language from " << oldHeader->languageIndex << " to "
                                      << newHeader->languageIndex << endmsg;

         if (oldHeader->fontIndex == newHeader->fontIndex)
         {
            DataLog (log_level_gui_info) << "Font did not change (index " << newHeader->fontIndex << ")" << endmsg;
            loadNewFont = false;
         }
      }
      else
      {
         DataLog (log_level_gui_info) << "Language did not change (index " << newHeader->languageIndex << ")" << endmsg;
         return true;
      }
   }
   else
   {
      DataLog (log_level_gui_info) << "No previous language header.  Loading language " << newHeader->fileName << endmsg;
   }

   if (loadNewFont)
   {
      DataLog (log_level_gui_info) << "Loading new font " << newHeader->fontName << "(" << newHeader->fontIndex << ")" << endmsg;

      loadFontFileToMemory(newHeader->fontName.c_str());
   }

   DataLog (log_level_gui_info) << "Loading database file for " << newHeader->fileName
                                << ".  LanguageIndex=" << newHeader->languageIndex << endmsg;

   if (!_stringData.loadLanguage(newHeader))
   {
      DataLog(log_level_gui_error) << "ERROR: failed to read current string file: " << newHeader->fileName << endmsg;
      return false;
   }

   // Check here for missing strings.  This includes any string defined in string.info but doesn't
   // exist in another language file.
   if (_stringData.isAnythingMissing(newHeader->languageIndex))
   {
      DataLog(log_level_gui_error) << "ERROR: text items misisng in current string file: " << newHeader->fileName << endmsg;

      // Erase the language header so it doesn't show up in the GUI language list
      LanguageList::erase(newHeader);

      return false;
   }
   else
      DataLog (log_level_gui_info) << "Current string file database loaded: " << newHeader->fileName << endmsg;

// for some reason, unloading the font does not work.  Objects must still be
// pointing to the old font.  Leave commented out for now.
#if 0
   // Only look to unload fonts if we have a language to unload
   if (oldHeader)
   {
      // Now that the new language was loaded, clear out the unused fonts
      // Iterate through the list of fonts and for every one that is loaded, compare it
      // to the currently loaded font id and the list of exceptions.  If the loaded font
      // is not one of those, then unload it.
      // NOTE:  Don't unload the default language.
      for (int i = 1; i < MaxMyFonts; i++)
      {
         const MY_FONT* font = getFontStruct(i);

         // If the font is not loaded or is the same as the currently
         // loaded language, leave it alone!
         if ( (font == NULL)         ||
              (font->memptr == NULL) ||
              (font->index == newHeader->fontIndex)
              )
         {
            continue;
         }

         DataLog(log_level_gui_info) << "font " << font->name << " (" << font->index
                                     << ")" << " is loaded.  Checking against currently loaded language" << endmsg;

         // Iterate through the list of Font Ranges and if the font table entry doesn't
         // exist, then unload it.
         for (FONT_RANGE_LIST_ITER iter = OSTextBase::begin();
              iter != OSTextBase::end();
              ++iter)
         {
            const FontRange* range = *iter;

            // If the font is the same as the currently loaded language, leave it alone!
            if (range->fontIndex == font->index)
            {
               continue;
            }
         }

         // Loaded font is not in either list, remove it
         DataLog(log_level_gui_info) << "Unloading font " << font->name << "; id "
                                     << font->index << endmsg;

         unloadFontFileFromMemory(font->index);
      }
   }
#endif

   return true;
}

/* FORMAT HASH a4626ceab35e13266924b470c9d587d8 */

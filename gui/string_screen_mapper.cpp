/*******************************************************************************
 *
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      string_screen_mapper.hpp
 *             String-to-Screen Mapping
 *
 * AUTHOR:     Gustavo Gomez
 *
 * ABSTRACT:   This file defines the string-screen mapper class, whose job
 *  it is to create a relationship between strings and screenshots.  When a
 *  screenshot is generated in the simulator, two things happen:
 *  1) A text file is created which lists the strings that are displayed in
 *    that screenshot.  There should be one of these files for each screenshot
 *  2) An HTML-formatted table is updated.  This table lists all the strings
 *    in the Trima GUI, and the rightmost column has links to the screenshots
 *    in which that string occurs.
 *******************************************************************************/

#include <vxworks.h>
#include <private/memPartLibP.h>
#if CPU == SIMNT        /* this whole file is only meant for the simulator */
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "filenames.h"
#include "string_info.h"
#include "trima_datalog.h"
#include "string_screen_mapper.hpp"
#include "display_text.h"
#include "guistring.h"
#include "guiglobs.hpp"

#define ignoredStringsFile     CONFIG_PATH "/ignored_strings.dat"

typedef std::set<TEXT_STRING_ID> id_set_type;
typedef std::set<std::string> str_set_type;
typedef std::map<std::string, id_set_type> scr_str_map_type;

typedef std::map<TEXT_STRING_ID, str_set_type> str_scr_map_type;
typedef std::map<TEXT_STRING_ID, std::string> str_text_map_type;
typedef std::map<std::string, id_set_type> text_str_set_map_type;
typedef std::map<TEXT_STRING_ID, id_set_type> str_substring_set_map_type;
typedef std::map<std::string, int> text_ref_count_map_type;
typedef std::map<TEXT_STRING_ID, std::map<TEXT_STRING_ID, std::string> > id_concat_string_map_type;



const char*  convertStringIdToString (TEXT_STRING_ID stringId);         // automatically defined elsewhere
static char* strdup (const char* str);                                  // we need a copy of the screen name string, since it comes from the stack
static void  write_html ();                                             // write html file with mappings from strings <--> screens

static str_scr_map_type           StringToScreenMap; // a mapping from a string to the set of screens it appears on.
static scr_str_map_type           ScreenToStringMap; // a mapping from a screen to the set of strings it contains.
static str_text_map_type          StringIdToTextMap;
static str_set_type               ScreenSet;         // the set of all the screenshot filenames.
static std::string                CurrentScreenName; // The name of the current screen.
static std::string                Mapping = "";      // the string which will keep the mapping.
static id_set_type                CurrentScreenStrings;
static std::vector<std::string>   ignoredStringsVector;

static text_str_set_map_type      TextToStringIdSetMap;
static text_ref_count_map_type    TextReferenceCountMap;

static str_substring_set_map_type StringIdToSubstringSetMap;

static id_concat_string_map_type  concat_string_Map;

// Variables for holding statistics!
static unsigned int numberOfStrings        = (unsigned int)TEXT_DEF_END;
static unsigned int numberOfScreenshots    = 0;
static unsigned int numberOfCoveredStrings = 0;
static id_set_type  CoveredStrings;

template<class Integral>
class IntegralIterator
{
public:
   IntegralIterator(const Integral& init)
      : _currentValue(init) {}
   bool operator                    == (const IntegralIterator& rhs) { return _currentValue == *rhs; }
   bool operator                    != (const IntegralIterator& rhs) { return _currentValue != *rhs; }
   const Integral& operator         * () const { return _currentValue; }
   const IntegralIterator& operator ++ ()
   {
      _currentValue = (Integral)(_currentValue + 1);
      return *this;
   }
   IntegralIterator operator ++ (int)
   {
      IntegralIterator result = *this;
      _currentValue = (Integral)(_currentValue + 1);
      return result;
   }

   typedef input_iterator_tag iterator_category;
   typedef Integral value_type;
   typedef ptrdiff_t difference_type;
   typedef Integral* pointer;
   typedef Integral& reference;
private:
   Integral _currentValue;
};

char* string_screen_mapper::ignoreSubstrings[] =
{
   "UNDEFINED",
   "languageId",
   "languageIndex",
   "languageWrappingAlgorithm",
   "fontName",
   "languageCrc",
   "readyForRelease",
   "forbiddenStartCharList",
   "forbiddenEndCharList",
   "textEmpty",
   "Units",
   "textBlank",
   "Service",
   NULL
};


std::string findAndReplace (const std::string& source, const std::string& text, const std::string& replace)
{
   std::string result = source;

   for (unsigned int i = result.find(text); i != string::npos; i = result.find(text))
   {
      result.replace(i, text.length(), replace);
   }

   return result;
}

void string_screen_mapper::printScreenSet ()
{
   for (str_set_type::iterator it = ScreenSet.begin(); it != ScreenSet.end(); it++)
   {
      std::string fileName(*it);
      DataLog(log_level_sim_mapper_info) << "Screen \"" << fileName << "\"" << endmsg;
   }
}

void string_screen_mapper::printTextToStringIdSetMap ()
{
   for (text_str_set_map_type::iterator it = TextToStringIdSetMap.begin(); it != TextToStringIdSetMap.end(); it++)
   {
      std::string stringText((*it).first);
      DataLog(log_level_sim_mapper_info) << "TextToStringIdSetMap: \"" << stringText << "\"(" << TextReferenceCountMap[(*it).first] << "): ";    // endmsg;
      for (id_set_type::iterator it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
      {
         std::string substringIdName(convertStringIdToString(*it2));
         DataLog(log_level_sim_mapper_info) << substringIdName << ", ";
      }
      DataLog(log_level_sim_mapper_info) << endmsg;
   }
}

void string_screen_mapper::printStringIdToTextMap ()
{
   for (str_text_map_type::iterator it = StringIdToTextMap.begin(); it != StringIdToTextMap.end(); it++)
   {
      std::string stringText((*it).second);
      std::string stringIdName(convertStringIdToString((*it).first));

      DataLog(log_level_sim_mapper_info) << "StringIdToTextMap: " << stringIdName << ": \"" << stringText << "\"" << endmsg;
   }
}

bool string_screen_mapper::checkForCompoundStrings (std::string text)
{
   if (TextToStringIdSetMap.count(text) != 0)
   {
      id_set_type stringIdSet = TextToStringIdSetMap[text];

      for (id_set_type::iterator stringSetIterator = stringIdSet.begin(); stringSetIterator != stringIdSet.end(); stringSetIterator++)
      {
         TEXT_STRING_ID stringId   = *stringSetIterator;
         std::string    stringIdName(convertStringIdToString(stringId));
         std::string    stringText = StringIdToTextMap[stringId];

         _hasNewStrings = true;

         if (StringIdToTextMap.count(stringId) == 0)
         {

            StringIdToTextMap[stringId] = stringText;
            numberOfCoveredStrings++;
            _hasNewStrings              = true;
         }

         if (CoveredStrings.insert(stringId).second == true)
            _hasNewStrings = true;

         CurrentScreenStrings.insert(stringId);
      }
   }
}

void string_screen_mapper::add_string (TEXT_STRING_ID stringId, const char* str)
{
   std::string stringText(str);
   std::string stringIdName(convertStringIdToString(stringId));

   if (!CurrentScreenName.empty() && strlen(str) > 0)
   {
      if ((stringId != TEXT_NULL)  &&  (stringId != textEmpty))
      {
         if (StringIdToTextMap.count(stringId) == 0)
         {

            StringIdToTextMap[stringId] = stringText;
            numberOfCoveredStrings++;
            _hasNewStrings              = true;
         }

         if (CoveredStrings.insert(stringId).second == true)
            _hasNewStrings = true;
         CurrentScreenStrings.insert(stringId);
      }

      checkForCompoundStrings(stringText);
   }
   CheckForConcat(stringId, str);
}

void string_screen_mapper::associate_string_text (const guistring& str, TEXT_STRING_ID stringId)
{
   if (!_isTakingScreenshot)
   {
      std::string stringText(str);
      std::string stringIdName(convertStringIdToString(stringId));

      TextToStringIdSetMap[stringText].insert(stringId);
      StringIdToTextMap[stringId] = stringText;
   }
}

void string_screen_mapper::add_string_text (const guistring& str)
{
   if (!_isTakingScreenshot)
   {
      std::string stringText(str);

      TextReferenceCountMap[stringText]++;
   }
}

void string_screen_mapper::remove_string_text (const guistring& str)
{
   if (!_isTakingScreenshot && str.length() > 0)
   {
      std::string stringText(str);

      TextReferenceCountMap[stringText]--;

      if (TextReferenceCountMap[stringText] <= 0)
      {
         TextReferenceCountMap.erase(stringText);
         TextToStringIdSetMap.erase(stringText);
      }
   }
}

void string_screen_mapper::concatenate_strings (const guistring& lhs, const guistring& rhs)
{
   std::string lhsStringText(lhs);
   std::string rhsStringText(rhs);

   if (TextToStringIdSetMap.count(lhsStringText) != 0)
   {
      id_set_type stringIdSet = TextToStringIdSetMap[lhsStringText];

      for (id_set_type::iterator stringSetIterator = stringIdSet.begin(); stringSetIterator != stringIdSet.end(); stringSetIterator++)
      {
         TextToStringIdSetMap[lhsStringText + rhsStringText].insert(*stringSetIterator);
      }
   }

   if (TextToStringIdSetMap.count(rhsStringText) != 0)
   {
      id_set_type stringIdSet = TextToStringIdSetMap[rhsStringText];

      for (id_set_type::iterator stringSetIterator = stringIdSet.begin(); stringSetIterator != stringIdSet.end(); stringSetIterator++)
      {
         TextToStringIdSetMap[lhsStringText + rhsStringText].insert(*stringSetIterator);
      }
   }
}

void string_screen_mapper::AddConcatMap (TEXT_STRING_ID container, TEXT_STRING_ID contained)
{
   std::map<TEXT_STRING_ID, std::string>* map  = NULL;
   id_concat_string_map_type::iterator    iter = concat_string_Map.find(container);
   if ( iter == concat_string_Map.end() )
   {
      std::map<TEXT_STRING_ID, std::string> amap;
      concat_string_Map[container] = amap;
      map = &concat_string_Map[container];
   }
   else
      map = (&iter->second);
   if ( map != NULL )
   {
      Display_Text displayText(contained);
      guistring    astring;
      displayText.get_text_direct(astring);
      (*map)[contained] = std::string(astring);
   }
}

void string_screen_mapper::CheckForConcat (TEXT_STRING_ID stringId, const char* str)
{
   id_concat_string_map_type::iterator iter = concat_string_Map.find(stringId);
   if ( iter != concat_string_Map.end() )
   {
      std::string input(str);
      std::map<TEXT_STRING_ID, std::string>::iterator itemIter = iter->second.begin();
      for ( ; itemIter != iter->second.end() ; itemIter++ ) {
         if ( input.find(itemIter->second) == string::npos )
            continue;

         if (StringIdToTextMap.count(itemIter->first) == 0)
         {

            StringIdToTextMap[itemIter->first] = itemIter->second;
            numberOfCoveredStrings++;
            _hasNewStrings                     = true;
         }


         if (CoveredStrings.insert(itemIter->first).second == true)
            _hasNewStrings = true;
         CurrentScreenStrings.insert(itemIter->first);
      }
   }
}

bool string_screen_mapper::hasNewStrings ()
{
   return _hasNewStrings;
}

void string_screen_mapper::reset ()
{
   _hasNewStrings      = false;
   _isTakingScreenshot = false;
}

bool string_screen_mapper::isTakingScreenshot ()
{
   return _isTakingScreenshot;
}

void string_screen_mapper::start_screenshot (const char* scr)
{
   _isTakingScreenshot = true;
   numberOfScreenshots++;
   std::string screenName(scr);

   CurrentScreenStrings.clear();

   CurrentScreenName = screenName;
}

void string_screen_mapper::stop_screenshot ()
{
   ScreenToStringMap[CurrentScreenName] = CurrentScreenStrings;
   for (id_set_type::iterator it = CurrentScreenStrings.begin(); it != CurrentScreenStrings.end(); it++)
   {
      StringToScreenMap[*it].insert(CurrentScreenName);

      std::string stringIdName(convertStringIdToString(*it));
      std::string stringText = StringIdToTextMap[*it];

      DataLog(log_level_sim_mapper_debug) << "adding string " << stringIdName.c_str() << " (\""
                                          << stringText.c_str() << "\") to screen \"" << CurrentScreenName.c_str() << "\"" << endmsg;

      Mapping.append(CurrentScreenName).append(" ").append(stringIdName).append(" (\"").append(stringText).append("\")\r\n");

   }
   ScreenSet.insert(CurrentScreenName);

   std::string mappingFileName = std::string(SNAPSHOTS_PATH) + "/" + CurrentScreenName + ".txt";
   CurrentScreenName.erase();

   FILE* mappingFile = fopen(mappingFileName.c_str(), "w");
   fputs(Mapping.c_str(), mappingFile);
   fclose(mappingFile);

   Mapping.erase();

   write_html();

   _hasNewStrings      = false;
   _isTakingScreenshot = false;
}

bool string_screen_mapper::shouldIgnore (std::string compareString)
{
   const char* compareString_c = compareString.c_str();

   // see if compareString array contains an ignored substring
   for (int index = 0; ignoreSubstrings[index]; ++index)
      if (strstr(compareString_c, ignoreSubstrings[index]))
         return true;

   // now check against full strings
   for (int index = 0; index < ignoredStringsVector.size(); ++index)
      if (compareString.compare(ignoredStringsVector[index])==0)
         return true;
   return false;
}

/**
 * Looks in ignored_strings.dat and copies contents to the ignoredStringsVector.
**/
bool string_screen_mapper::populateIgnoredStrings ()
{
   bool          retVal = false;
   std::string   ignoredString;

   std::ifstream ignoredFile;
   ignoredFile.open(ignoredStringsFile);

   if (ignoredFile.is_open())
   {
      DataLog(log_level_sim_mapper_info) << "ignored_strings.dat found in : " << ignoredStringsFile << endmsg;

      while (ignoredFile >> ignoredString)
         ignoredStringsVector.push_back(ignoredString);

      ignoredFile.close();
      retVal = true;
   }
   else
      DataLog(log_level_sim_mapper_info) << "WARNING: ignored_strings.dat unable to open: " << ignoredStringsFile << endmsg;

   return retVal;
}

void string_screen_mapper::write_html ()
{
   DataLog(log_level_sim_mapper_info) << "write_html() starting" << endmsg;

   string buffer = "";
   buffer.append("<html>\n");
   buffer.append("<head>\n");
   buffer.append("<title>Snapshots Trima 5.R</title>\n");
   buffer.append("<script src=\"sorttable.js\"></script>");
   buffer.append("</head>\n");

   buffer.append("<body>\n");

   buffer.append("<a href=\"#STRINGS_TO_SCREENS\">Strings &rarr; Screens/Text</a><br>\n");
   buffer.append("<a href=\"#SCREENS_TO_STRINGS\">Screens &rarr; Strings</a><br>\n");

   // lets start with the strings --> screens
   buffer.append("<a name=\"STRINGS_TO_SCREENS\"><h1>Strings &rarr; Screens/Text</h1></a>\n");
   buffer.append("<table class=\"sortable\" border=\"1\" align=\"center\" cellspacing=\"0\">\n");

   buffer.append("<tr>\n");
   buffer.append("<th><b>String</b></th><th><b>Screen</b></th><th><b>Text</b></th>\n");


   for (TEXT_STRING_ID stringId = TEXT_NULL; stringId < TEXT_DEF_END; stringId = (TEXT_STRING_ID)(stringId + 1))
   {
      std::string stringIdAsText(convertStringIdToString(stringId));
      std::string stringText;
      buffer.append("<tr>\n");
      buffer.append("<td>\n");
      buffer.append("<a name=\"").append(stringIdAsText).append("_STRING_ID\">");
      buffer.append(stringIdAsText);
      buffer.append("</a>\n");
      buffer.append("</td>\n");
      buffer.append("<td>\n");
      for (str_set_type::iterator it = StringToScreenMap[stringId].begin(); it != StringToScreenMap[stringId].end(); it++)
      {
         std::string fileName(*it);

         buffer.append("<a href=\"#").append(fileName).append("_SNAPSHOT\">");
         buffer.append(fileName);
         buffer.append("</a> ");
      }
      buffer.append("\n</td>\n");

      if (StringIdToTextMap.count(stringId) == 0)
      {
         Display_Text currentString(stringId);
         guistring    currentStringText;
         currentString.get_text_direct(currentStringText);
         stringText = currentStringText;
      }
      else
      {
         stringText = StringIdToTextMap[stringId];
      }

      stringText = findAndReplace(stringText, "\n", "<br>");

      buffer.append("<td>\n");
      buffer.append(stringText).append("\n");
      buffer.append("</td>\n");

      buffer.append("</tr>\n");
   }
   buffer.append("</table>\n");
   buffer.append("<br /><br /><br />\n");

   // now lets do screens -> strings
   buffer.append("<a name=\"SCREENS_TO_STRINGS\"><h1>Screens &rarr; Strings</h1></a>\n");
   buffer.append("<table class=\"sortable\" border=\"1\" align=\"center\" cellspacing=\"0\">\n");

   buffer.append("<tr>\n");
   buffer.append("<th><b>Screen</b></th><th><b>String</b></th>\n");

   for (str_set_type::iterator it = ScreenSet.begin(); it != ScreenSet.end(); it++)
   {
      std::string fileName(*it);

      buffer.append("<tr>\n");

      buffer.append("<td>\n");
      buffer.append("<a name=\"").append(fileName).append("_SNAPSHOT\">\n");
      buffer.append("<a href=\"").append(fileName).append(".bmp\">\n");
      buffer.append(fileName);
      buffer.append("</a>\n");
      buffer.append("</a>\n");
      buffer.append("</td>\n");

      buffer.append("<td>\n");
      for (id_set_type::iterator it2 = ScreenToStringMap[fileName].begin(); it2 != ScreenToStringMap[fileName].end(); it2++)
      {
         std::string stringIdAsText(convertStringIdToString(*it2));

         buffer.append("<a href=\"#").append(stringIdAsText).append("_STRING_ID\">");
         buffer.append(stringIdAsText);
         buffer.append("</a> ");
      }
      buffer.append("</td>\n");
      buffer.append("\n</tr>\n");
   }
   buffer.append("</table>\n");
   buffer.append("<br /><br /><br />\n");

   // Now lets print some statistics about the whole shebang.
   buffer.append("<h1>Statistics</h1>\n");

   stringstream stringCoverageStat;

   stringCoverageStat << "<b>Screen Coverage:</b> " << CoveredStrings.size() << "/" << TEXT_DEF_END << " (";
   stringCoverageStat << (float(CoveredStrings.size()) / float(TEXT_DEF_END));
   stringCoverageStat << ")<br>\n";

   buffer.append(stringCoverageStat.str());

   stringstream screenshotCountStat;

   screenshotCountStat << "<b>Screenshot Count:</b> " << numberOfScreenshots << "<br>\n";

   buffer.append(screenshotCountStat.str());


   int         numMissingStrings = 0;
   id_set_type missingStrings;
   std::string missingStr;

   std::set_difference(IntegralIterator<TEXT_STRING_ID>(TEXT_NULL), IntegralIterator<TEXT_STRING_ID>(TEXT_DEF_END), CoveredStrings.begin(), CoveredStrings.end(), std::inserter(missingStrings, missingStrings.begin()));

   for (id_set_type::iterator it = missingStrings.begin(); it != missingStrings.end(); ++it)
   {
      std::string stringIdName(convertStringIdToString(*it));

      if (!shouldIgnore(stringIdName))
      {
         missingStr += "<a href=\"#";
         missingStr += stringIdName;
         missingStr += "_STRING_ID\">";
         missingStr += stringIdName;
         missingStr += "</a><br>";
         ++numMissingStrings;
      }

   }

   std::stringstream sstr;
   sstr << numMissingStrings;

   buffer.append("<h1>Missing Strings: ").append(sstr.str()).append("</h1>\n");
   buffer.append(missingStr);

   buffer.append("</body>\n</html>\n");

   FILE* file = fopen(SNAPSHOTS_PATH "/index.html", "w");
   fputs(buffer.c_str(), file);
   fclose(file);
   DataLog(log_level_sim_mapper_info) << "write_html() finishing" << endmsg;

   // check for simulator success, i.e. no outstanding missing strings
   if (numMissingStrings == 0)
   {
      FILE* successFile = fopen(SNAPSHOTS_PATH "/ScreenshotsSuccess.txt", "w");
      fclose(successFile);
      DataLog(log_level_sim_mapper_info) << "SUCCESS: Simulator has reached all strings. ScreenshotSuccess.txt written" << endmsg;
   }

}

static char* strdup (const char* str)
{
   char* new_str = new char[strlen(str) + 1];
   return strcpy(new_str, str);

}

bool string_screen_mapper::_hasNewStrings      = false;
bool string_screen_mapper::_isTakingScreenshot = false;

#endif  /* CPU==SIMNT */

/* FORMAT HASH 06d16fecd448c6a5a71bee0692372474 */

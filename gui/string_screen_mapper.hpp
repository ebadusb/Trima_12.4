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
 *
 * HISTORY:    $Log: string_screen_mapper.hpp $
 * HISTORY:    Revision 1.4  2008/08/15 20:38:05Z  mereusb
 * HISTORY:    IT 8107: Change for screenshot scripts.
 * HISTORY:    Revision 1.3  2008/07/03 08:14:30  mereusb
 * HISTORY:    IT 8107 - Improved screenshot coverage.
 * HISTORY:    Revision 1.2  2008/01/31 10:02:37  dslausb
 * HISTORY:    Updates for GUI screenshot scripts
 * HISTORY:    Initial revision
*******************************************************************************/

#ifndef STRING_SCREEN_MAPPER_HPP
#define STRING_SCREEN_MAPPER_HPP

#include "string_id.h"
#include "guistring.h"

class string_screen_mapper
{
private:
   static bool  _hasNewStrings;
   static bool  _isTakingScreenshot;

   static char* ignoreSubstrings[];
   static bool shouldIgnore (std::string compareString);
public:

   static bool checkForCompoundStrings (std::string text);
   // adds a string (TEXT_STRING_ID) to the current screen.
   static void add_string (TEXT_STRING_ID stringId, const char* str);

   static void printTextToStringIdSetMap ();
   static void printStringIdToTextMap ();
   static void printScreenSet ();

   static void associate_string_text (const guistring& str, TEXT_STRING_ID stringId);
   static void add_string_text (const guistring& str);
   static void remove_string_text (const guistring& str);
   static void concatenate_strings (const guistring& lhs, const guistring& rhs);

   static void reset ();

   static bool isTakingScreenshot ();

   // sets the name of the screen where the strings are going to be displayed.
   static void start_screenshot (const char* scr);

   // creates the screen <--> strings mapping file.
   static void stop_screenshot ();

   // add strings from ignored_strings.dat to ignored strings vector
   static bool populateIgnoredStrings ();

   // write the mappings to an html file
   static void write_html ();

   // Returns whether or not the current screen has new strings.
   static bool hasNewStrings ();

   static void AddConcatMap (TEXT_STRING_ID container, TEXT_STRING_ID contained);
   static void CheckForConcat (TEXT_STRING_ID stringId, const char* str);
};

#endif /* ifndef STRING_SCREEN_MAPPER_HPP */

/* FORMAT HASH 05694185eec803e33414269ed92ecb7b */

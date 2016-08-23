/*
 *	Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * Derived from trima_string_data.cpp revision 1.7  2006/07/25 15:42:37  cf10242
 * $Header$
 *
 */

#include <vxWorks.h>
#include "trima_string_info.h"
#include "ostext.h"
#include "datalog_levels.h"
#include "guipalette.h"
#include "string.h"
#include "fontdefs.h"
#include "trima_ugl_mem.h"


// constructor that accepts a file name, opens it, and prepares for gets
TrimaStringInfo::TrimaStringInfo(const char* filename, bool delayRead)
   : LanguageFile(filename),
     _taskDelay(delayRead),
     _taskDelayTime(0)
{
   open();

   if (_taskDelay)
   {
      _taskDelayTime = 1;
   }
}

// constructor that just sets up the class members, but does not require a file name to open
TrimaStringInfo::TrimaStringInfo()
   : LanguageFile(),
     _taskDelay(false),
     _taskDelayTime(0)
{}

TrimaStringInfo::~TrimaStringInfo(void)
{}

// convert UTF8 encoding to Unicode
void TrimaStringInfo::UTF8ToUnicode (char*& data, TrimaStringChar* wString, int& writeIdx)
{
   if ( (*data & 0xf0) == 0xe0 &&
        (*(data + 1) & 0xc0) == 0x80 &&
        (*(data + 2) & 0xc0) == 0x80 )
   {
      TrimaStringChar ch1 = *data++ & 0x0f;
      TrimaStringChar ch2 = *data++ & 0x3f;
      TrimaStringChar ch3 = *data++ & 0x3f;

      wString[writeIdx++] = ( ch1 << 12 ) | ( ch2 << 6 ) | ch3;
   }
   else if ( (*data & 0xe0) == 0xc0 &&
             (*(data + 1) & 0xc0) == 0x80 )
   {
      TrimaStringChar ch1 = *data++ & 0x1f;
      TrimaStringChar ch2 = *data++ & 0x3f;

      wString[writeIdx++] = ( ch1 << 6 ) | ch2;
   }
   else
   {
      if ( *data == '\\' )
      {
         data++;

         switch ( *data )
         {
            case '\0' :
               break;

            case 'b' :
               wString[writeIdx++] = '\b';
               data++;
               break;

            case 'n' :
               wString[writeIdx++] = '\n';
               data++;
               break;

            case 'r' :
               wString[writeIdx++] = '\r';
               data++;
               break;

            case 't' :
               wString[writeIdx++] = '\t';
               data++;
               break;

            case '"' :
               wString[writeIdx++] = '"';
               data++;
               break;

            case 'x' :
               char unicode[5];
               int  l;

               data++;
               unicode[0] = (*data != '\0') ? *data++ : '\0';
               unicode[1] = (*data != '\0') ? *data++ : '\0';
               unicode[2] = (*data != '\0') ? *data++ : '\0';
               unicode[3] = (*data != '\0') ? *data++ : '\0';
               unicode[4] = '\0';

               sscanf(unicode, "%x", &l);
               wString[writeIdx++] = (TrimaStringChar)l;
               break;

            default :
               wString[writeIdx++] = (TrimaStringChar)(*data); data++;
               break;
         }
      }
      else
      {
         wString[writeIdx++] = (TrimaStringChar)(*data); data++;
      }
   }
}

//
// Return the next string element in the language file
//
bool TrimaStringInfo::getNextEntry (TrimaTextItem& result, int fontIndex)
{
   if (_stringInfoFile == NULL)
   {
      DataLog(log_level_gui_error) << _filename << " not opened." << endmsg;
      return false;
   }

   while (fgets(_lineBuffer, LineBufferSize, _stringInfoFile) != NULL)
   {
      pauseReading();

      char*       p          = NULL;
      const char* firstToken = strtok_r(_lineBuffer, " \t\n\r\"", &p);

      _line += 1;

      // skip comments
      if (!firstToken || firstToken[0] == '#'|| firstToken[0] == '[')
      {
         continue;
      }

      if (strcmp(firstToken, "STRING_FILE_TABLE_START") == 0)
      {
         if (_readingFileTable)
         {
            DataLog(log_level_gui_error) << _filename << "(" << _line << "): unexpected STRING_FILE_TABLE_START without ending previous table" << endmsg;
            break;
         }

         if ( _readingFontRanges )
         {
            DataLog(log_level_gui_error) << _filename << "(" << _line << "): unexpected STRING_FILE_TABLE_START inside font range table" << endmsg;
            break;
         }

         _readingFileTable = true;
         continue;
      }
      else if (strcmp(firstToken, "STRING_FILE_TABLE_END") == 0)
      {
         if (!_readingFileTable)
         {
            DataLog(log_level_gui_error) << _filename << "(" << _line << "): unexpected STRING_FILE_TABLE_END without preceding STRING_FILE_TABLE_START" << endmsg;
            break;
         }

         if ( _readingFontRanges )
         {
            DataLog(log_level_gui_error) << _filename << "(" << _line << "): unexpected STRING_FILE_TABLE_END inside font range table" << endmsg;
            break;
         }

         _readingFileTable = false;
         continue;
      }
      else if ( strcmp(firstToken, "FONT_RANGE_TABLE_START") == 0 )
      {
         if (_readingFileTable)
         {
            DataLog(log_level_gui_error) << _filename << "(" << _line << "): unexpected FONT_RANGE_TABLE_START inside file table" << endmsg;
            break;
         }

         if ( _readingFontRanges )
         {
            DataLog(log_level_gui_error) << _filename << "(" << _line << "): unexpected FONT_RANGE_TABLE_START without ending previous font range table" << endmsg;
            break;
         }

         _readingFontRanges = true;
         continue;
      }
      else if ( strcmp(firstToken, "FONT_RANGE_TABLE_END") == 0 )
      {
         if ( _readingFileTable )
         {
            DataLog(log_level_gui_error) << _filename << "(" << _line << "): unexpected FONT_RANGE_TABLE_END inside file table" << endmsg;
            break;
         }

         if ( !_readingFontRanges )
         {
            DataLog(log_level_gui_error) << _filename << "(" << _line << "): unexpected FONT_RANGE_TABLE_END without preceding FONT_RANGE_TABLE_START" << endmsg;
            break;
         }

         _readingFontRanges = false;
         continue;
      }
      else if (_readingFontRanges)
      {
         //
         // This block of code reads the font overrides for the file.  Each entry defines a range
         // of character codes that override the base font defined in the file.
         //
         unsigned int startRange, endRange;

         //
         // Get start and end character code ranges
         //
         if (sscanf(firstToken, "%x", &startRange) != 1)
         {
            DataLog(log_level_gui_error) << "malformed line in file " << _filename
                                         << ".  Line number " << _line << ".  Line " << firstToken << p << endmsg;
            continue;
         }

         char* nextToken = strtok_r(NULL, " \t\n\r\"", &p);

         if (sscanf(nextToken, "%x", &endRange) != 1)
         {
            DataLog(log_level_gui_error) << "malformed line in file " << _filename
                                         << ".  Line number " << _line << ".  Line " << nextToken << " - " << p << endmsg;
            continue;
         }

         nextToken = strtok_r(NULL, " \t\n\r\"", &p);

         if (nextToken == NULL)
         {
            DataLog(log_level_gui_error) << "malformed line in file " << _filename
                                         << ".  Line number " << _line << ".  Line " << nextToken << " - " << p << endmsg;
            continue;
         }

         //
         // Extract the font name from the file.
         // This would have been easier of the original line wasn't already tokenized from above.
         // Basically, grab each word of the font name as a token, and glue it back together again
         // adding spaces as necessary for each individual word
         //
         char fontName[120];
         int  fontNameLength = 0;

         while (nextToken != NULL)
         {
            const int length = strlen(nextToken);

            if (fontNameLength + length < sizeof(fontName))
            {
               strncpy(&fontName[fontNameLength], nextToken, length);
               fontNameLength              += length + 1;
               fontName[fontNameLength - 1] = ' ';  // Add a space that get eaten by strtok
               nextToken                    = strtok_r(NULL, " \t\n\r\"", &p);
            }
            else
            {
               DataLog(log_level_gui_error) << "Font name too long in file " << _filename
                                            << ".  Length " << fontNameLength + length + 1
                                            << ".  Line number " << _line << ".  "
                                            << nextToken << " - " << p << endmsg;
               break;
            }
         }

         if (fontNameLength < 1) continue;   // Empty/Bad font name
         fontName[--fontNameLength] = 0;     // Terminate string

         const int  index     = getFontIndex(fontName);
         FontRange* fontRange = new FontRange(startRange, endRange, index, fontName);

         DataLog (log_level_gui_info) << "Font Range [" << startRange << "-" << endRange << "]"
                                      << "=\"" << fontName << "\"" << "  Index " << index << endmsg;

         if (!OSTextBase::addFontRange(fontRange))
         {
            DataLog(log_level_gui_error) << "Failed to add font range for font name " << fontName
                                         << ".  range " << startRange << "-" << endRange << ".  Index " << index << endmsg;
         }

         continue;
      }
      else if (_readingFileTable)
      {
         //
         // This block of code reads an actual string and the styling record
         //
         result.setId(firstToken);
         return getStylingRecord(p, result, fontIndex);
      }
   }

   return false;
}

bool TrimaStringInfo::getQuotedString (char*& data, TrimaStringChar*& wString)
{
   const int newStrNumBytes = strlen(data) * sizeof(TrimaStringChar);
   wString = (TrimaStringChar*)GUI_POOL_MALLOC(newStrNumBytes);
   if (NULL == wString) return false;
   memset (wString, 0, newStrNumBytes);  // Make sure this string is NULL terminated.

   bool started  = false;
   bool done     = false;

   int  writeIdx = 0;

   while ( !done )
   {
      if ( *data == '\0' ||
           ( started && *data == '"' ))
      {
         done = true;
      }
      else if ( !started )
      {
         if (*data == '"')
            started = true;

         data++;
      }
      else // ( started )
      {
         // Added 8/8/08: We're stripping out tags of the
         // type #![] at this level.  There are a ton of them
         // added for string subsegmentation, and there's really
         // no reason to have to deal with them every time we
         // manipulate a string, when we could just chop them
         // out up front.
         //
         // Please note that if we choose to use the indent tag
         // feature that optia uses, we'll have to rip this out
         // or modify it, because this chunk of code will strip
         // that tag out before it's parsed.
         // D.S. (x4664)
         // bool clippedTag = false;

         if (data[0] == '#' &&
             data[1] == '!' &&
             data[2] == '[')
         {
            // Keep moving forward until we find the end of the tag,
            // or the end of the buffer, or the end of the quoted section.
            while (*data != ']' && *data != '\0' && *data != '"')
               data++;

            // Bump it one more, as long as this isn't the end of string.
            if (*data == ']')
               data++;
            else
               DataLog (log_level_gui_error) << "Unterminated markup tag at line " << _line << endmsg;
         }
         // This is the normal case.  Deal with a standard character.
         else
            UTF8ToUnicode(data, wString, writeIdx);
      }
   }

   bool result = false;

   if ( *data == '"' )
   {
      result = true;
      data++;
   }
   else
   {
      DataLog(log_level_gui_error) << "line " << _line << ": unterminated string - " << _filename << endmsg;
   }

   return result;
}

// Parse the line and populate the given TrimaTextItem.
bool TrimaStringInfo::getStylingRecord (char* p, TrimaTextItem& result, const int fontIndex = -1)
{
   TrimaStringChar* wString = NULL;
   bool             status  = getQuotedString(p, wString);

   int              red, green, blue;
   unsigned int     attributes;
   int              x, y, width, height, fontSize, localize;

   if ( status &&
        sscanf(p, "%d %d %d %x %d %d %d %d %d %d", &red, &green, &blue, &attributes, &x, &y, &width, &height, &fontSize, &localize) != 10 )
   {
      status = false;
      DataLog(log_level_gui_error) << "line " << _line << ": missing parameters - " << _filename << endmsg;
   }

   // Create Styling Record from line information.
   //
   if (status)
   {
      StylingRecord stylingRecord;
      stylingRecord.color      = guipalette::lookupColor(red, green, blue);
      stylingRecord.attributes = attributes;
      stylingRecord.region     = OSRegion(x, y, width, height);
      stylingRecord.fontSize   = fontSize;
      stylingRecord.fontIndex  = fontIndex;

      // Populate the TrimaTextItem class.
      //
      result.setText(wString);                // setText copies the string into its own string buffer.
      result.setStylingRecord(stylingRecord); // setStylingRecord copies the styling record into its own styling record.
   }

   if (wString)
      GUI_POOL_FREE(wString);

   return status;
}

/* FORMAT HASH 490421fc5dc6ccdedaa7dcdb2a1d93ff */

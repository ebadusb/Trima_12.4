/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 */

#include <vxWorks.h>

#include "ostext.h"
#include "osgraphics.h"
#include "DataLog_levels.h"
#include "guipalette.h"
#include "fontdefs.h"
#include "oswindow.hpp"

#if CPU==SIMNT
# include <string.h>
#include "stringError.h" // Needed for redefinition of
#include "string_screen_mapper.hpp"
bool        captureTextID     = false;
string      captureTextIDStr;
const char* captureScreenName = NULL;

#endif /* if CPU==SIMNT */

static TrimaStringChar          newline_char  = '\n';
static TrimaStringChar          space_char    = ' ';
static TrimaStringChar          tab_char      = '\t';
static TrimaStringChar          null_char     = '\0';
static TrimaStringChar          decimal_point = '.';
static TrimaStringChar          decimal_comma = ',';
static TrimaUnicodeString       paragraph_format_start("#![PG");
static TrimaUnicodeString       paragraph_format_end("]");
static const TrimaUnicodeString nonBreakingSpace(" ");  // unicode hex or utf8 format = 0xC2A0
static const TrimaUnicodeString regularLatinSpace(" "); // unicode hex = 0x0020

OSText::OSText(OSDisplay& display)
   : OSWindowObject(display),
     _currentId(TEXT_NULL),
     _captureBackgroundColor(false),
     _backgroundColor(0),
     _configLanguage(0)
{
   initializeData(NULL, NULL);
}

OSText::OSText(OSDisplay& display, TrimaTextItem* textItem, StylingRecord* stylingRecord = NULL)
   : OSWindowObject(display),
     _currentId(TEXT_NULL),
     _captureBackgroundColor(false),
     _backgroundColor(0),
     _configLanguage(0)

{
   initializeData(textItem, stylingRecord);
}

OSText::OSText(OSDisplay& display, TrimaTextItem* textItem, OSColor backgroundColor, StylingRecord* stylingRecord = NULL)
   : OSWindowObject(display),
     _currentId(TEXT_NULL),
     _captureBackgroundColor(false),
     _backgroundColor(0),
     _configLanguage(0)
{
   initializeData(textItem, stylingRecord);
   setBackgroundColor(backgroundColor);
}

OSText::~OSText()
{}

void OSText::initializeData (TrimaTextItem* textItem, StylingRecord* stylingRecord)
{
   _textItem           =  textItem;
   _forceCompute       = false;
   _backgroundColorSet = false;

   if (_textItem)
   {
      if (_textItem->isInitialized())
      {
         if (!_textItem->isLoaded())
         {
            DataLog(log_level_gui_error) << "Text item " << _textItem->getId()
                                         << " is missing.  EnglishOnly=" << _textItem->isEnglishOnly()
                                         << " Loaded=" << _textItem->isLoaded()
                                         << " languageIndex=" << _textItem->getLanguageId() << endmsg;

            // _FATAL_ERROR(__FILE__, __LINE__, "Missing text string, who is responsible?");
         }

         //  If styling record from constructor is null, set the the ostext._stylingRecord to
         //  the trima_text_item._stylingRecord by default.
         //
         if (!stylingRecord)
         {
            _stylingRecord = _textItem->getStylingRecord();
         }
         else
         {
            _stylingRecord = *stylingRecord;
         }

         // Set the requested region.
         _requestedRegion = _stylingRecord.region;
         _textString      = _textItem->getTextObj();

         if ( _textString.getLength() == 0 )
            setRegion(OSRegion(0, 0, 0, 0));

         _forceCompute = true;
         computeTextRegion();
      }
      else
      {
         DataLog(log_level_critical) << "Can't find text record for text item " << _textItem->getId() << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Missing text string, who is responsible?");
      }
   }

   _languageSetByApp = false;
}

void OSText::setAttributes (unsigned int newAttributes)
{
   if ( _stylingRecord.attributes != newAttributes )
   {
      _stylingRecord.attributes = newAttributes;
      _forceCompute             = true;
      computeTextRegion();
   }
}

void OSText::setBackgroundColor (OSColor color)
{
   if ( !_backgroundColorSet ||
        _backgroundColor != color )
   {
      _backgroundColorSet = true;
      _backgroundColor    = color;
      if ( _owner ) _owner->invalidate_obj_region(this);
   }
}

void OSText::setColor (OSColor color)
{
   if ( _stylingRecord.color != color )
   {
      _stylingRecord.color = color;
      if (_owner) _owner->invalidate_obj_region(this);
   }
}

void OSText::setColor (int red, int green, int blue)
{
   OSColor newColor = guipalette::lookupColor(red, green, blue);
   setColor(newColor);
}


// This function call determines if the font is the default or in the exclusion list
int OSText::getFontIndex (int currentChar)
{
   int fontIndex = checkInFontRange(currentChar);

   if (fontIndex == -1)
      return _stylingRecord.fontIndex;
   else
      return fontIndex;
}

// This function call determines if the font is the default or in the exclusion list
OSFontId OSText::getFontId (int currentChar)
{
   return _display.get_fontWithFontId(getFontIndex(currentChar), _stylingRecord.fontSize);
}

OSFontId OSText::getFontId ()
{
   return _display.get_fontWithFontId(_stylingRecord.fontIndex, _stylingRecord.fontSize);
}

void OSText::setFontIndex (int newIndex)
{
   _stylingRecord.fontIndex = newIndex;
   _forceCompute            = true;
   computeTextRegion();
}

void OSText::setFontSize (int newSize)
{
   _stylingRecord.fontSize = newSize;
   _forceCompute           = true;
   computeTextRegion();
}

void OSText::setFont (const int newFontIndex, const int newFontSize)
{
   if (_stylingRecord.fontSize == newFontSize &&
       _stylingRecord.fontIndex == newFontIndex)
      return;

   _stylingRecord.fontSize  = newFontSize;
   _stylingRecord.fontIndex = newFontIndex;

   _forceCompute            = true;

   computeTextRegion();
}

void OSText::setLanguage (LanguageId configLanguage)
{
   //
   // This animal will look different as the
   // language module is developed.
   //
   _configLanguage = configLanguage;
   if ( _textItem )
      _textItem->setLanguageId(configLanguage);

   _languageSetByApp = true;
}

void OSText::setStylingRecord (StylingRecord* stylingRecord)
{
   _stylingRecord   = *stylingRecord;
   _requestedRegion = stylingRecord->region;
   _forceCompute    = true;
   computeTextRegion();
}

void OSText::setText (TrimaTextItem* textItem)
{
   if (textItem)
   {
      _textItem =  textItem;

      if ( _textItem && _textItem->isInitialized() )
      {
         _textString = _textItem->getTextObj();

         if ( _textString.getLength() == 0 )
            setRegion(OSRegion(0, 0, 0, 0));

         computeTextRegion();
      }
   }
}

void OSText::setText ()
{
   if (_textItem)
      _textString = _textItem->getTextObj();
   else
      _textString.empty();

   if ( _textString.getLength() == 0 )
      setRegion(OSRegion(0, 0, 0, 0));

   computeTextRegion();
}

void OSText::setText (const TrimaStringChar* text)
{
   if (text)
   {
      _textString = text;
      _textItem   = NULL;
   }
   else
   {
      setRegion(OSRegion(0, 0, 0, 0));
   }

   computeTextRegion();
}

void OSText::setText (const TrimaUnicodeString& text)
{
   _textString = text;
   _textItem   = NULL;

   if (text.getLength() == 0)
   {
      setRegion(OSRegion(0, 0, 0, 0));
   }
   computeTextRegion();
}

void OSText::setText (const char* text)
{
   if (text)
   {
      _textString = text;
      _textItem   = NULL;
   }
   else
   {
      setRegion(OSRegion(0, 0, 0, 0));
   }

   computeTextRegion();
}

const TrimaStringChar* OSText::getText ()
{
   return _textString.getString();
}

const TrimaUnicodeString& OSText::getTextObj ()
{
   handleVariableSubstitution();

    #if CPU==SIMNT
   string_screen_mapper::associate_string_text(_textString.getUTF8().c_str(), _currentId);
    #endif /* if CPU==SIMNT */

   return _textString;
}

int OSText::getLength () const
{
   int retVal = 0;

   if ( _textItem )
   {
      retVal = _textItem->getLength();
   }
   else
   {
      retVal = _textString.getLength();
   }

   return retVal;
}

void OSText::getSize (OSRegion& region, int startIndex, int length, OSFontId fontId)
{
   const unsigned int stringLength = _textString.getLength();

   if ( (fontId == UGL_NULL_ID) ||
        (stringLength <= 0)     ||
        (startIndex >= stringLength)
        )
   {
      region = OSRegion(0, 0, 0, 0);
   }
   else
   {
      UGL_SIZE uglStartIndex = startIndex;
      UGL_SIZE uglLength     = length;

      if ( uglStartIndex < 0 )
      {
         uglStartIndex = 0;
      }

      if ( (uglLength < 0) ||
           (uglStartIndex + uglLength > stringLength)
           )
      {
         uglLength = stringLength - startIndex;
      }

      if ( _stylingRecord.attributes & BOLD )
      {
         UGL_ORD option = 0;
         uglFontInfo(fontId, UGL_FONT_WEIGHT_SET, &option);
      }

      const TrimaStringChar* textStr = _textString.getString();

      UGL_SIZE               width   = 0;
      UGL_SIZE               height  = 0;

      UGL_STATUS_MONITOR(uglTextSizeGetW(fontId, &width, &height, uglLength, &textStr[uglStartIndex]));

      region = OSRegion(0, 0, width, height);
   }
}

void OSText::getPixelSize (OSRegion& pixelRegion, OSFontId fontId)
{
   getSize(pixelRegion, 0, _textString.getLength(), fontId);
}


void OSText::getTokenSize (OSRegion& region, int startIndex, int length)
{
   OSRegion resultRegion  = OSRegion(0, 0, 0, 0);
   OSFontId currentFontId = getFontId(_textString[ startIndex ]);

   if ( _fontRange.size() > 0 )
   {
      int segmentLength = 0;

      if ( startIndex >= 0 && length > 0 )
      {
         for ( int i = startIndex; i <= ( startIndex + length ); i++ )
         {
            int      currentChar = (int)_textString[i];
            OSFontId newFontId   = getFontId(currentChar);

            if ( newFontId == UGL_NULL_ID )
               newFontId = _display.get_fontWithFontId(_stylingRecord.fontIndex, _stylingRecord.fontSize);

            if ( newFontId != currentFontId || ( i == ( startIndex + length )))
            {
               //	update resultRegion to new values
               getSize(resultRegion, ( i - segmentLength ), segmentLength, currentFontId);

               region.width += resultRegion.width;
               segmentLength = 0;       //	This will be automaticaly incremented when it drops out of the if-statement

               currentFontId = newFontId;
            }
            segmentLength++;

         }   // for text length
      }      // if greater than zero

      region.height = resultRegion.height;
   }
   else
   {
      getSize(region, startIndex, length, currentFontId);
   }
}


OSText::GetTokenResult OSText::getCharBasedToken (int start_index, bool start_of_line, int& length)
{
   bool           token_ended  = false;
   int            currentIndex = start_index;
   length = 0;
   GetTokenResult result       = EndOfString;

   // Check for format command.  These are only allowed at the start of a line.
   //
   if ( start_of_line &&
        currentIndex < _textString.getLength() - paragraph_format_start.getLength() )
   {
      if ( _textString[currentIndex] == paragraph_format_start[0] &&
           _textString.mid(currentIndex, paragraph_format_start.getLength()) == paragraph_format_start )
      {
         int endOfFormatToken = _textString.find(paragraph_format_end, currentIndex + paragraph_format_start.getLength());
         if ( endOfFormatToken >= 0 )
         {
            endOfFormatToken += paragraph_format_end.getLength();
            length            = endOfFormatToken - currentIndex;
            token_ended       = true;
            result            = FormatToken;
         }
      }
   }

   // keep moving characters from the text string to the token string until
   // a trailing blank or EOS is found.
   //
   int  tokenWidth          = 0;
   bool englishToken        = false;
   bool forbiddenStartToken = false;
   bool forbiddenEndToken   = false;

   while ( currentIndex < _textString.getLength() &&
           !token_ended )
   {
      if ( _textString[currentIndex] == newline_char )
      {
         token_ended = true;
         if ( currentIndex == start_index )
         {
            length = 1;
         }
      }
      else if ( checkIfForbiddenStart(currentIndex + 1) ) // Asian Forbidden Start char
      {
         if ( englishToken && !checkIfEnglish(currentIndex) && !forbiddenStartToken )
         {
            token_ended = true;
         }
         else
         {
            length             += 1;
            currentIndex       += 1;
            forbiddenStartToken = true;
         }
      }
      else if ( checkIfForbiddenEnd(currentIndex) )  // Asian Forbidden End Char
      {
         if ( englishToken && !forbiddenStartToken && !forbiddenEndToken )
         {
            token_ended = true;
         }
         else
         {
            length           += 1;
            currentIndex     += 1;
            forbiddenEndToken = true;
         }
      }
      else if ( checkIfEnglish(currentIndex) )  // English letters or numbers
      {
         englishToken = true;
         length++;
         currentIndex++;
      }
      else if ( _textString[currentIndex] == tab_char )
      {
         token_ended = true;
         if ( currentIndex == start_index )
         {
            length = 1;
         }
      }
      else if ( _textString[currentIndex] == space_char)
      {
         if ( englishToken )
         {
            token_ended = true;
         }
         else
         {
            length       += 1;
            currentIndex += 1;
         }
      }
      else
      {
         // Floating point numbers
         if ( ( _textString[currentIndex] == decimal_point || _textString[currentIndex] == decimal_comma )
              && checkIfArabicNumeral(currentIndex - 1) && checkIfArabicNumeral(currentIndex + 1) )
         {
            length       += 1;
            currentIndex += 1;
         }
         else    // All other characters
         {
            if ( forbiddenStartToken || forbiddenEndToken || !englishToken )
            {
               length       += 1;
               currentIndex += 1;

               if ( forbiddenStartToken ) forbiddenStartToken = false;
               if ( forbiddenEndToken ) forbiddenEndToken = false;
            }

            if ( englishToken )
            {
               englishToken = false;
            }

            token_ended = true;
         }
      }
   }

   if ( result != FormatToken )
   {
      result = (length <= 0) ? EndOfString : NormalToken;
   }

   return result;
}

bool OSText::checkIfArabicNumeral (int index)
{
   bool               result = false;
   TrimaUnicodeString zero('0');
   TrimaUnicodeString nine('9');

   if ( index < _textString.getLength() && index >= 0 )
   {
      TrimaStringChar charPos = _textString[index];

      if ( charPos >= zero[0] && charPos <= nine[0] )
      {
         result = true;
      }
   }

   return result;

}

bool OSText::checkIfEnglish (int index)
{
   bool               result = false;
   TrimaUnicodeString smallA('a');
   TrimaUnicodeString bigA('A');
   TrimaUnicodeString smallZ('z');
   TrimaUnicodeString bigZ('Z');

   TrimaUnicodeString zero('0');
   TrimaUnicodeString nine('9');

   if ( index < _textString.getLength() && index >= 0 )
   {
      TrimaStringChar charPos = _textString[index];

      if ( ( charPos >= smallA[0] && charPos <= smallZ[0] ) ||
           ( charPos >= bigA[0] && charPos <= bigZ[0] ) ||
           ( charPos >= zero[0] && charPos <= nine[0] ) )
      {
         result = true;
      }
   }

   return result;
}

bool OSText::checkIfForbiddenStart (int index)
{
   bool result = false;

   if ( _forbiddenStartCharsAvailable )
   {
      if ( index < _textString.getLength() && index >= 0 )
      {
         if ( _forbiddenStartCharList.find(_textString[index], 0) != -1 )
         {
            result = true;
         }
      }
   }

   return result;
}

bool OSText::checkIfForbiddenEnd (int index)
{
   bool result = false;

   if ( _forbiddenEndCharsAvailable )
   {
      if ( index < _textString.getLength() && index >= 0 )
      {
         if ( _forbiddenEndCharList.find(_textString[index], 0) != -1 )
         {
            result = true;
         }
      }
   }

   return result;
}

OSText::GetTokenResult OSText::getToken (int start_index, bool start_of_line, int& length)
{
   // this flag is false before encountering non-blank token characters
   // and true after encountering non-blank token characters. This allows
   // leading blanks to be included in the token, and the first trailing
   // blank to delimit the token.
   //
   bool look_for_trailing_blank = false;
   bool token_ended             = false;

   int  currentIndex            = start_index;
   length = 0;

   GetTokenResult result = EndOfString;

   // Check for format command.  These are only allowed at the start of a line.
   //
   if ( start_of_line &&
        currentIndex < _textString.getLength() - paragraph_format_start.getLength() )
   {
      if ( _textString[currentIndex] == paragraph_format_start[0] &&
           _textString.mid(currentIndex, paragraph_format_start.getLength()) == paragraph_format_start )
      {
         int endOfFormatToken = _textString.find(paragraph_format_end, currentIndex + paragraph_format_start.getLength());
         if ( endOfFormatToken >= 0 )
         {
            endOfFormatToken += paragraph_format_end.getLength();
            length            = endOfFormatToken - currentIndex;
            token_ended       = true;
            result            = FormatToken;
         }
      }
   }

   // keep moving characters from the text string to the token string until
   // a trailing blank or EOS is found.
   //
   while (currentIndex < _textString.getLength() &&
          !token_ended )
   {
      if ( _textString[currentIndex] == newline_char ||
           _textString[currentIndex] == tab_char )
      {
         // If this character is an explicit newline or tab and the token buffer is empty,
         // return the character.  Otherwise return the token parsed so far, and this character
         // will be returned on the next call to getToken().
         //
         token_ended = true;
         if ( currentIndex == start_index )
         {
            // nothing has been added to the token buffer, just return the single character
            length = 1;
         }
      }
      else if ( _textString[currentIndex] == space_char &&
                look_for_trailing_blank )
      {
         // if this is the delimiting trailing blank, leave the loop and return the token
         token_ended = true;
      }
      else
      {
         if ( _textString[currentIndex] != space_char )
         {
            look_for_trailing_blank = true;
         }

         // add this character to the token
         length++;
         currentIndex++;
      }
   }

   if ( result != FormatToken )
   {
      result = (length <= 0) ? EndOfString : NormalToken;
   }

   return result;
}

void OSText::convertTextToMultiline (OSRegion& region)
{
   bool     start_of_line      = true;
   int      currentIndex       = 0;
   int      current_line       = 0;
   int      space_pixel_size   = 0;
   int      single_line_height = 0;
   int      x_min              = 0;
   int      x_max              = 0;
   int      y_max              = 0;

   OSFontId currentFontId      = _display.get_fontWithFontId(_stylingRecord.fontIndex, _stylingRecord.fontSize);

   _lineData.clear();
   _formatData.firstLineIndent  = 0;
   _formatData.secondLineIndent = 0;

   if ( currentFontId != UGL_NULL_ID )
   {
      UGL_STATUS_MONITOR(uglTextSizeGet(currentFontId, &space_pixel_size, &single_line_height, 1, " "));
   }
   else
   {
      UGL_STATUS_MONITOR(uglTextSizeGet(getFontId(0x0020), &space_pixel_size, &single_line_height, 1, " "));
   }
   int line_start_x = 0;
   int line_start_y = 0;

   while ( currentIndex < _textString.getLength() )
   {
      int      current_segment_pixel_width = 0;
      int      line_start_index            = currentIndex;
      int      line_byte_count             = 0;
      bool     line_done                   = false;
      bool     line_ended_by_tab           = false;

      OSRegion line_size;

      while ( !line_done )
      {
         int tokenCharCount = 0;

         if (start_of_line)
            line_start_x = _formatData.firstLineIndent * _tabSpaceCount * space_pixel_size;

         // get the next blank delimited token
         GetTokenResult tokenResult = EndOfString;

         switch ( _tokenSplitMethod )
         {
            case WordBased :
               tokenResult = getToken(currentIndex, start_of_line, tokenCharCount);
               break;

            case CharBased :
               tokenResult = getCharBasedToken(currentIndex, start_of_line, tokenCharCount);
               break;

            default :
               break;
         }

         TrimaUnicodeString ustr = _textString.mid(currentIndex, tokenCharCount);

         if ( tokenResult == EndOfString )
         {
            // reached end of string, so this is the last line
            line_done    = true;
            currentIndex = _textString.getLength();
         }
         else if ( tokenResult == FormatToken )
         {
            int parameter_index    = currentIndex + paragraph_format_start.getLength();
            int first_line_indent  = -1;
            int second_line_indent = -1;
            while ( parameter_index < currentIndex + tokenCharCount &&
                    second_line_indent < 0 )
            {
               if ( _textString[parameter_index] >= (TrimaStringChar)'0' &&
                    _textString[parameter_index] <= (TrimaStringChar)'9' )
               {
                  if ( first_line_indent < 0 )
                  {
                     first_line_indent = _textString[parameter_index] - (TrimaStringChar)'0';
                  }
                  else
                  {
                     second_line_indent = _textString[parameter_index] - (TrimaStringChar)'0';
                  }
               }

               parameter_index += 1;
            }

            if ( first_line_indent >= 0 &&
                 second_line_indent >= 0 )
            {
               _formatData.firstLineIndent  = first_line_indent;
               _formatData.secondLineIndent = second_line_indent;
            }

            currentIndex    += tokenCharCount;
            line_start_index = currentIndex;
         }
         else if ( _textString[currentIndex] == newline_char )
         {
            // newline forced a line break
            start_of_line = true;
            currentIndex += tokenCharCount;

            if ( line_byte_count )
            {
               line_done = true;
            }
            else
            {
               line_start_index = currentIndex;
               line_start_y    += 3 * single_line_height / 4;
            }
         }
         else if ( _textString[currentIndex] == tab_char )
         {
            start_of_line = false;
            currentIndex += tokenCharCount;

            if ( line_byte_count )
            {
               line_done         = true;
               line_ended_by_tab = true;
            }
            else
            {
               line_start_x    += _tabSpaceCount * space_pixel_size;
               line_start_index = currentIndex;
            }
         }
         else
         {
            start_of_line = false;

            OSRegion tokenSize;

            getTokenSize(tokenSize, currentIndex, tokenCharCount);

            if ( convertLinePosition(current_segment_pixel_width + tokenSize.width, tokenSize.height, line_start_x, line_size) )
            {
               // new token fits on line
               line_byte_count             += tokenCharCount;
               currentIndex                += tokenCharCount;
               current_segment_pixel_width += tokenSize.width;
            }
            else
            {
               line_done = true;
               if ( line_byte_count <= 0 )
               {
                  // must put token on line
                  line_byte_count             += tokenCharCount;
                  currentIndex                += tokenCharCount;
                  current_segment_pixel_width += tokenSize.width;
               }
               else
               {
                  // update line region information without tokenSize.width
                  convertLinePosition(current_segment_pixel_width, tokenSize.height, line_start_x, line_size);
               }
            }
         }
      }

      // Create line_data struct
      //
      if ( line_byte_count > 0 )
      {
         LineData currentLineData;

         currentLineData.x          = line_size.x;
         currentLineData.y          = line_start_y;

         currentLineData.index      = line_start_index;
         currentLineData.textLength = line_byte_count;
         currentLineData.fontId     = currentFontId;

         // eliminate first char if it is a blank (space character)
         if ( _textString[line_start_index] == space_char )
         {
            currentLineData.index++;
            currentLineData.textLength--;
         }

         //	Check for conbination of fonts for line data
         //
         size_t listSize = _fontRange.size();

         if ( listSize > 0 )
         {
            LineData fontRangeLineData;
            OSRegion fontRangeRegion(0, 0, 0, 0);

            fontRangeLineData.x          = currentLineData.x;
            fontRangeLineData.y          = currentLineData.y;
            fontRangeLineData.index      = currentLineData.index;
            fontRangeLineData.textLength = 0;     // currentLineData.textLength;
            fontRangeLineData.fontId     = currentFontId;

            for ( int i = currentLineData.index; i <= (currentLineData.index + currentLineData.textLength); i++ )
            {
               int      currentChar = (int)_textString[i];
               OSFontId newFontId   = getFontId(currentChar);

               if ( newFontId == UGL_NULL_ID )
               {
                  newFontId = _display.get_fontWithFontId(_stylingRecord.fontIndex, _stylingRecord.fontSize);
               }

               if (( newFontId != currentFontId ) && ( i != (currentLineData.index + currentLineData.textLength )))
               {
                  if ( i != currentLineData.index )
                  {
                     fontRangeLineData.textLength = i - fontRangeLineData.index;

                     _lineData.push_back(fontRangeLineData);

                     //	update  fontRangeLineData to new values
                     getSize(fontRangeRegion, fontRangeLineData.index, fontRangeLineData.textLength, currentFontId);
                     fontRangeLineData.x         += fontRangeRegion.width;
                     fontRangeLineData.index      = i;
                     fontRangeLineData.textLength = 0;          //	This will be automaticaly incremented when it drops out of the if-statement
                  }
                  currentFontId            = newFontId;
                  fontRangeLineData.fontId = newFontId;
               }

               if ( i != (currentLineData.index + currentLineData.textLength ))
                  fontRangeLineData.textLength++;   //	increment text length

            }     //	for current line data

            _lineData.push_back(fontRangeLineData);
         }
         else
         {
            // associate fontId with line data
            currentLineData.fontId = _display.get_fontWithFontId(_stylingRecord.fontIndex, _stylingRecord.fontSize);

            // do what we always did
            _lineData.push_back(currentLineData);
         }

         x_max = (line_size.x + line_size.width - 1 > x_max) ? line_size.x + line_size.width - 1 : x_max;

         if ( current_line <= 0 )
         {
            x_min = line_size.x;
         }
         else
         {
            x_min = (line_size.x < x_min) ? line_size.x : x_min;
         }

         y_max = _lineData.back().y + line_size.height + 1;
         current_line++;
      }

      if ( !line_ended_by_tab )
      {
         line_start_x  = _formatData.secondLineIndent * _tabSpaceCount * space_pixel_size;
         line_start_y += single_line_height;
      }
      else
      {
         int tab_column = ( line_size.x + line_size.width - 1 - _requestedRegion.x ) / ( _tabSpaceCount * space_pixel_size ) + 1;
         line_start_x = tab_column * _tabSpaceCount * space_pixel_size;
      }
   }

   // Setup final region required by the text
   //
   region.x      = x_min;
   region.y      = 0;
   region.width  = x_max - x_min + 1;
   region.height = y_max + 1;

   // Adjust line horizontal positions to left side of region
   //
   list<LineData>::iterator line;
   for ( line = _lineData.begin(); line != _lineData.end(); ++line )
   {
      (*line).x -= region.x;
   }
}


bool OSText::convertLinePosition (int width, int height, int indent_pixels, OSRegion& region)
{
   int available_pixel_width = 0;

   // Given alignment specification and text size information, determine the region
   // in which the text should be drawn
   if (_stylingRecord.attributes & HJUSTIFY_CENTER)
   {
      // CENTER alignment
      region.x              = _requestedRegion.x + (_requestedRegion.width - width) / 2;
      available_pixel_width = _requestedRegion.width;
   }
   else if (_stylingRecord.attributes & HJUSTIFY_RIGHT)
   {
      // RIGHT alignment: text will end at _requestedRegion.x.  Text with RIGHT
      // alignment is not wrapped, so available_pixel_width is always set to the
      // text size.
      region.x              = _requestedRegion.x + _requestedRegion.width - 1 - width;
      available_pixel_width = width;
   }
   else
   {
      // LEFT alignment: text will start at _requestedRegion.x plus the current indent
      region.x              = _requestedRegion.x + indent_pixels;
      available_pixel_width = ( _requestedRegion.width == 0 ) ? _display.width() - region.x : _requestedRegion.width - indent_pixels;
   }

   region.y      = 0;
   region.width  = width;
   region.height = height;

   return( width <= available_pixel_width );
} // END convert_line_position


void OSText::computeTextRegion ()
{
   if (_textString.getLength() > 0)
      handleVariableSubstitution();

   if ( _forceCompute ||
        _textString != _lastTextString )
   {
      _forceCompute   = false;
      _lastTextString = _textString;

      OSRegion text_region;
      convertTextToMultiline(text_region);

      // find vertical region to place text
      if (_stylingRecord.attributes & VJUSTIFY_CENTER)
      {
         // center vertically within specified region
         if (text_region.height <= _requestedRegion.height)
         {
            text_region.vert_shift(_requestedRegion.y + (_requestedRegion.height - text_region.height) / 2);
         }
         else
         {
            text_region.vert_shift(_requestedRegion.y);
         }
      }
      else if (_stylingRecord.attributes & VJUSTIFY_BOTTOM)
      {
         // justify at bottom of requested region
         if (text_region.height <= _requestedRegion.height)
         {
            text_region.vert_shift(_requestedRegion.y + _requestedRegion.height - text_region.height);
         }
         else
         {
            text_region.vert_shift(_requestedRegion.y);
         }
      }
      else
      {
         // justify at top of requested region
         text_region.vert_shift(_requestedRegion.y);
      }

      setRegion(text_region);
   }
}

void OSText::setRegion (const OSRegion& newRegion)
{
   if (_owner)
      _owner->set_object_region(this, newRegion);
   else
      _region = newRegion;
}



void OSText::draw (UGL_GC_ID gc)
{
   if (!_textString.getLength() ||
       _lineData.empty() ||
       _region.width == 0 ||
       _region.height == 0)
   {
      return;
   }

    #if CPU==SIMNT // If simulator, do some string mapping stuff.
   string_screen_mapper::add_string(_currentId, _textString.getUTF8().c_str());
    #endif // #if CPU==SIMNT // If simulator, do some string mapping stuff.

   //
   // Need true coordinates (not viewport relative) on screen so that
   // we can get correct background color for text.
   //
   UGL_POS vp_left, vp_top, vp_right, vp_bottom;
   UGL_STATUS_MONITOR(uglViewPortGet(gc, &vp_left, &vp_top, &vp_right, &vp_bottom));

   //
   // Average the color of the four corner pixels for the text region.
   //
   int      pixelCount = 0;
   UGL_ARGB red        = 0;
   int      green      = 0;
   int      blue       = 0;

   //
   // Get the pixel from each corner of text region and average the color pixel.
   //
   for (int x = _region.x; x <= _region.x + _region.width; x += _region.width)
   {
      for (int y = _region.y; y <= _region.y + _region.height; y += _region.height)
      {
         if (x + vp_left >= 0 && x + vp_left < _display.width() &&
             y + vp_top >= 0 && y + vp_top < _display.height())
         {
            UGL_COLOR pixelColor;
            UGL_ARGB  pixelARGB;
            UGL_STATUS_MONITOR(uglPixelGet(gc, x, y, &pixelColor));
            UGL_STATUS_MONITOR(uglClutGet(_display.display(), pixelColor, &pixelARGB, 1));

            red        += UGL_ARGB_RED(pixelARGB);
            green      += UGL_ARGB_GREEN(pixelARGB);
            blue       += UGL_ARGB_BLUE(pixelARGB);
            pixelCount += 1;
         }
      }
   }

   if (pixelCount > 1)
   {
      red   /= pixelCount;
      green /= pixelCount;
      blue  /= pixelCount;
   }

   UGL_ARGB  backgroundARGB = UGL_MAKE_RGB(red, green, blue);
   UGL_COLOR backgroundColor;

   UGL_STATUS_MONITOR(uglColorAlloc(_display.display(), &backgroundARGB, NULL, &backgroundColor, 1));
   // make copy of text string to draw.
   // TrimaUnicodeString copyTextString( _textString );
   // copyTextString.replace( nonBreakingSpace, regularLatinSpace );

   //  let's write this out
   UGL_STATUS_MONITOR(uglBackgroundColorSet(gc, backgroundColor));

   UGL_STATUS_MONITOR(uglForegroundColorSet(gc, _stylingRecord.color));

   list<LineData>::const_iterator lineIter = _lineData.begin();
   while ( lineIter != _lineData.end() )
   {

      LineData lineData;
      lineData.x          = (*lineIter).x;
      lineData.y          = (*lineIter).y;
      lineData.textLength = (*lineIter).textLength;
      lineData.fontId     = (*lineIter).fontId;

      UGL_FONT_ID testFont = NULL;
      UGL_STATUS_MONITOR(uglFontGet(gc, &testFont));

      if (testFont != (*lineIter).fontId)
         UGL_STATUS_MONITOR(uglFontSet(gc, (*lineIter).fontId));

      UGL_STATUS_MONITOR(uglTextDrawW(gc, (*lineIter).x + _region.x, (*lineIter).y + _region.y,
                                      (*lineIter).textLength, &_textString[(*lineIter).index]));
      ++lineIter;
   }
} // END draw

void OSText::handleVariableSubstitution ()
{
   if ( _textItem)
   {
      _textString = _textItem->getTextObj();
   }
}

// Functions for FontRange
bool FontRange::operator == (const FontRange& fontRange) const
{
   bool result = true;

   do
   {
      if ( startIndex != fontRange.startIndex )
      {
         result = false;
         break;
      }

      if ( endIndex != fontRange.endIndex )
      {
         result = false;
         break;
      }

   } while ( false );   /* only do-while loop once */

   return result;
}

bool FontRange::operator < (const FontRange& fontRange) const
{
   bool result = false;

   do
   {
      if ( startIndex < fontRange.startIndex )
      {
         result = true;
         break;
      }

      if ( startIndex == fontRange.startIndex &&
           endIndex < fontRange.endIndex )
      {
         result = true;
         break;
      }

   } while ( false );     /* only do-while loop once */

   return result;
}

bool FontRange::operator > (const FontRange& fontRange) const
{
   bool result = false;

   do
   {
      if ( startIndex > fontRange.startIndex )
      {
         result = true;
         break;
      }

      if ( startIndex == fontRange.startIndex &&
           endIndex > fontRange.endIndex )
      {
         result = true;
         break;
      }

   } while ( false );     /* only do-while loop once */

   return result;
}


////////////////////////////////////////////////////////////////////////////////
// GET_REQUESTED_REGION

void OSText::get_requested_region (OSRegion& requested_region) const
{
   requested_region = _requestedRegion;
}  // END of Display_Text GET_REQUESTED_REGION


////////////////////////////////////////////////////////////////////////////////
// SET_REQUESTED_REGION

void OSText::set_requested_region (const OSRegion& set_text_requested_region)
{
   _requestedRegion.height = set_text_requested_region.height;
   _requestedRegion.width  = set_text_requested_region.width;
   _requestedRegion.x      = set_text_requested_region.x;
   _requestedRegion.y      = set_text_requested_region.y;

   _forceCompute           = true;
   computeTextRegion();

   // if (_owner) _owner->invalidate_obj_region(this);
} // END of Display_Text SET_REQUESTED_REGION


void OSText::pre_draw ()
{
   computeTextRegion();
}  // END of Display_Text PRE_DRAW

/* FORMAT HASH b58b05f7d74c94b27f8fe1fc423a2cc9 */

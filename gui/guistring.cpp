/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995-2002 COBE BCT, Inc. All rights reserved

*     Author:      Michael J Cobb

*     Class name:  guistring

*     File name:   guistring.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************


*/

#include <vxworks.h>

// Focussed #INCLUDES

#include "guistring.h"
#include "trima_datalog_levels.h"
#include "datalog.h"
#include "error.h"

#include "guiglobs.hpp"

#if CPU==SIMNT
#include "string_screen_mapper.hpp"
#endif /* if CPU==SIMNT */

// ------------------------------------------------------------------------------------
// CONSTRUCTORS
//
//   When I am constructed I automatically initialize my buffer
//

guistring::guistring()
   : _length(0)
{
   guistring::initialize();
}



guistring::guistring(const char* stringText)
{
   if (stringText == NULL)
   {
      guistring::initialize();
      return;
   }

   _length = strlen(stringText);
   if (_length > buffersize)
   {
      DataLog(log_level_gui_error) << __PRETTY_FUNCTION__ << "(" << __LINE__ << "): Buffer overflow encountered. Desired size=" << _length << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "Buffer overflow.");
      guistring::initialize();
      return;
   }

   memcpy(_buffer, stringText, _length);
   _buffer[_length] = 0;

    #if CPU==SIMNT
   string_screen_mapper::add_string_text(*this);
    #endif /* if CPU==SIMNT */
}


// The Big Three (Copy Constructor, Copy Assignment Operator, Destructor)
// Copy Constructor
guistring::guistring(const guistring& orig)
   : _length(orig._length)
{
   memcpy(_buffer, orig._buffer, buffersize + 1);

    #if CPU==SIMNT
   string_screen_mapper::add_string_text(*this);
   #endif /* if CPU==SIMNT */
}

// Destructor
guistring::~guistring()
{
   #if CPU==SIMNT
   string_screen_mapper::remove_string_text(*this);
   #endif /* if CPU==SIMNT */

   _length = 0;
}


// ----------------------------------------------------------------------------------------
const size_t guistring::length () const
{
   return _length;
}

void guistring::initialize ()
{
   // note initialization of last byte
   memset(_buffer, '\0', buffersize + 1);
   _length = 0;
}

// -----------------------------------------------------------------------------------------
//   This function will reverse the characters in a guistring
//
void guistring::reverse ()
{
   guistring revbfr(*this);

   int       len = strlen(revbfr);
   for (int i = 0; i<len; i++)
   {
      _buffer[i] = revbfr[len - i - 1]; // don't move /0
   }
}

// -----------------------------------------------------------------------------------------
//    This function will remove any blanks from the guistring.  Note not 'whitespace'.
//
void guistring::strip_blanks ()
{
   //  start by copying and wiping out my current buffer
   guistring stripped(*this);
   initialize();

   int newlength = 0;
   for (int i = 0; i<stripped._length; i++)
   {
      if (stripped[i] != ' ')
      {
         _buffer[newlength++] = stripped[i];
      }
   }
   _length = newlength;
}

// -----------------------------------------------------------------------------------------
//    This function will remove any blanks from the guistring.  Note not 'whitespace'.
//
void guistring::strip_line_breaks ()
{
   //  start by copying and wiping out my current buffer
   guistring stripped(*this);
   initialize();

   int newlength = 0;
   for (int i = 0; i<stripped._length; i++)
   {
      // If this is anything other than a line break, add it to the new buffer
      if (stripped[i] != '\n')
      {
         _buffer[newlength++] = stripped[i];
      }
      // If this is a line break, and the previous character was not a space, replace the linebreak with a space
      else if ((newlength > 0) && (_buffer[newlength - 1] != ' '))
         _buffer[newlength++] = ' ';

      // Otherwise, just don't add that character.
   }
   _length = newlength;
}


void guistring::strip_unprintable_chars ()
{
   for (int i = 0; i < _length; i++)
   {
      if (!isprint(_buffer[i]))
      {
         _buffer[i] = ' ';      // Replace non-printable characters with spaces.
      }
   }
}

void guistring::swapChar (const char fromChar, const char toChar)
{
   // Don't worry about this is we're not in an alternate decimal delimiter mode.
   if (guiglobs::cdsData.config.LangUnit.Get().key_decimal_delimiter == DECIMAL_DELIMIT)
      for (int i = 0; i < _length; i++)
         if (_buffer[i] == fromChar)
            _buffer[i] = toChar;
}

// This just makes the swap function below a little
// more readable.
#define PERIOD '.'
#define COMMA ','

void guistring::swapOutDelimiter (DELIMITER_CONVERSION conversion = PERIOD_TO_COMMA)
{
   // Don't worry about this if we're not in an alternate decimal delimiter mode.
   if (guiglobs::cdsData.config.LangUnit.Get().key_decimal_delimiter == DECIMAL_DELIMIT)
      switch (conversion)
      {
         case COMMA_TO_PERIOD : swapChar(COMMA, PERIOD); break;
         case PERIOD_TO_COMMA : swapChar(PERIOD, COMMA); break;
      }
}

#undef PERIOD
#undef COMMA


// -----------------------------------------------------------------------------------------
guistring::operator const char* () const { return _buffer; }

const char& guistring::operator [] (unsigned int index) const
{
   if (index < _length)
      return _buffer[index];
   else
   {
      DataLog(log_level_gui_error) << "Index " << index
                                   << " out of range on buffer [" << _buffer << "]" << endmsg;

      _FATAL_ERROR(__FILE__, __LINE__, "Index out of range.");
      return _buffer[0];   // In case _FATAL_ERROR doesn't bomb out.
   }
}

char& guistring::operator [] (unsigned int index)
{
   if (index < buffersize) return _buffer[index];
   else
   {
      DataLog(log_level_gui_error) << "Index " << index
                                   << " out of range on buffer [" << _buffer << "]" << endmsg;

      _FATAL_ERROR(__FILE__, __LINE__, "Index out of range.");
      return _buffer[0];   // In case _FATAL_ERROR doesn't bomb out.
   }
}



// -----------------------------------------------------------------------------------------
const guistring& guistring::operator += (const char ch)
{
   if (_length < buffersize)
   {
      if (ch != '\0')
      {
         _buffer[_length]     = ch;
         _buffer[_length + 1] = '\0';
         _length++;
      }
   }
   else
   {

      DataLog(log_level_gui_error) << "guistring += char ignored, buffer at " << &_buffer << " full: " << _buffer << endmsg;

      _FATAL_ERROR(__FILE__, __LINE__, "buffer overflow in guistring +=");
   }
   return *this;
}

// -----------------------------------------------------------------------------------------
const guistring& guistring::operator += (const char* str)
{
   if (!str) return *this;
   for (int i = 0; i < strlen(str); i++)
      *this += str[i];
   return *this;
}

// -----------------------------------------------------------------------------------------
char* guistring::make_buffer () const
{
   char* buff = (char*)GUI_POOL_MALLOC((_length + 1) * sizeof(char));
   if (buff) memcpy(buff, _buffer, _length + 1);
   else _FATAL_ERROR(__FILE__, __LINE__, "make_buffer unable to allocate memory");
   return buff;
}

const guistring& guistring::operator = (const char* txt)
{
   initialize();
   if (!txt) return *this;  // fix for string = null char ptr

   _length = strlen(txt);

   if (_length > buffersize)
   {

      DataLog(log_level_gui_error) << "guistring op= const char * " << _length
                                   << " truncated to " << buffersize << " characters [txt="
                                   << txt << "]" << endmsg;

      _FATAL_ERROR(__FILE__, __LINE__, "buffer overflow in guistring op=");
      _length = buffersize; // in case fatal is removed
   }

   memcpy(_buffer, txt, _length);
   _buffer[_length] = 0;
   return *this;
}

const guistring& guistring::operator = (const wchar_t* txt)
{
   initialize();
   if (!txt) return *this;  // fix for string = null char ptr

   int chnum = 0;
   while (txt[chnum])
   {
      *this += (char)txt[chnum];
      chnum++;
   }
   return *this;
}

void guistring::truncate (int newSize)
{
   if (newSize > _length)
   {
      DataLog(log_level_gui_error) << "Truncation size (" << newSize << ") exceeds actual string size (" << _length << "). Ignore it." << endmsg;
      return;
   }

   if (newSize < 0)
   {
      DataLog(log_level_gui_error) << __PRETTY_FUNCTION__ << "(" << __LINE__ << "): Received invalid size (" << newSize << ")" << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "truncate() received invalid size.");
      return;
   }

   _length          = newSize;
   _buffer[_length] = '\0';
}

char* guistring::getBufPtr ()
{
   return _buffer;
}

bool guistring::contains (const char* txt)
{
   return (strstr(_buffer, txt) != NULL);
}


// -----------------------------------------------------------------------------------------
//          Global functions for interacting with a guistring
//
void strcpy (guistring& to, const char* __s2) // parasoft-suppress CODSTA-93 "We're intentionally overloading built-in string functionality here."
{
   to = __s2;
}

void strncpy (guistring& to, const char* __s2, size_t __n) // parasoft-suppress CODSTA-93 "We're intentionally overloading built-in string functionality here."
{
   to.initialize();
   if (!__s2) return;
   for (int i = 0; i<__n && __s2[i]; i++)
   {
      to += __s2[i];
   }
}

int vsprintf (guistring& to, const char* fmt, const va_list args) // parasoft-suppress CODSTA-93 "We're intentionally overloading built-in string functionality here."
{
   //
   //
   //  The idea here is to create a new char buffer (on the stack).  Therefore
   //  if there is an overflow hopefully it doesn't corrupt anything important.
   //
   char printbuf[guistring::buffersize] = "";
   int  returnCode = vsprintf(printbuf, fmt, args);

   //
   //
   //  Now check the string length.  If we overflowed then we should have only
   //  overrun extra stack.  (It's still bad.)  Note the redundant checks just
   //  in case we did corrupt something.
   //
   if ((returnCode < 0) || (returnCode >= guistring::buffersize) ||
       (strlen(printbuf) >= guistring::buffersize))
   {
      // Make sure we're not returning a positive number.
      if (returnCode > 0) returnCode = -1;

      // Truncate the string so we can print it to the datalog.
      printbuf[guistring::buffersize - 1] = '\0';
      DataLog (log_level_gui_error) << "Overflow in guistring: \"" << printbuf << "\"" << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "vsprintf overflow in guistring");
   }

   //
   //
   //  Now we can place the resultant string into our real buffer.  Overruns
   //  there are already handled by the class.
   //
   to = printbuf;
   return returnCode;
}

int sprintf (guistring& to, const char* fmt, ...) // parasoft-suppress CODSTA-93 "We're intentionally overloading built-in string functionality here."
{
   va_list args;
   va_start(args, fmt);
   int     retcode = vsprintf(to, fmt, args);
   va_end(args);

   return retcode;
}

// This is the "Localization" sprintf.  It does the same
// thing as regular sprintf, with the added step of then
// replacing periods with commas afterwards, if you are
// configured with the comma decimal delimiter.  Needless
// to say, it probably doesn't make sense to use this
// function if you're not printing numbers to the string.
// Also, text with periods as sentence punctuation shouldn't
// be used with this function.
int sprintf_l10n (guistring& to, const char* fmt, ...)
{
   va_list args;
   va_start(args, fmt);
   int     retcode = vsprintf(to, fmt, args);
   va_end(args);

   // Make sure we have the correct delimiter
   to.swapOutDelimiter();

   return retcode;
}


size_t strlen (const guistring& str) // parasoft-suppress CODSTA-93 "We're intentionally overloading built-in string functionality here."
{
   return str.length();
}

void strcat (guistring& str, const char* __s2)
{
   if (!__s2) return;
   str += __s2;
}

void strncat (guistring& str, const char* __s2, size_t __n)
{
   if (!__s2) return;
   for (int i = 0; i<__n && __s2[i]; i++)
   {
      str += __s2[i];
   }
}

/* FORMAT HASH e5d0a970b0df07c07b8acbf62a23d99b */

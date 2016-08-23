#ifndef GUISTRING_H
#define GUISTRING_H
//
//
//  This class can be used in place of a char[xx] declaration to give additional
//  safety for initialization, copying, and destruction.
//
//  Note that since this string class was designed to deal with the GUI legacy
//  code there will always be a null terminator at the end of the string.  Strings
//  are always created to be 100 bytes long (a legacy standard also).  Note that
//  using dynamic memory allocation (eg new char[xx]) doesn't work with the GUI
//  code since many character buffers are passed in structs, and therefore this
//  class must work with a shallow copy.
//
//  The size of 100 was increased to 150 to support processing of barcode strings which have
//  a max of 120.
//
#include <vxworks.h> // Needed for SIMNT compiler directives
#include <stddef.h>
#include <fioLib.h>

class guistring
{
public:

   typedef enum
   {
      COMMA_TO_PERIOD,
      PERIOD_TO_COMMA
   } DELIMITER_CONVERSION;

    #if CPU==SIMNT
   // Let's give some extra room in the simulator, since we don't care about
   // memory so much and we're using it for some additional unit testing stuff.
   static const int buffersize = 4096;
    #else
   static const int buffersize = 256;
    #endif // #if CPU==SIMNT

   guistring();
   guistring(const char*);

   virtual ~guistring();

   virtual void         initialize ();
   virtual const size_t length () const;

   virtual void strip_line_breaks ();
   virtual void strip_blanks ();
   virtual void strip_unprintable_chars ();
   virtual void reverse ();

   char* getBufPtr ();

   //
   //  Operations on my data
   //
   operator const char* () const;
   const char& operator      [] (unsigned int index) const;
   char& operator            [] (unsigned int index);
   const guistring& operator += (const char);
   const guistring& operator += (const char*);

   bool contains (const char*);

   // copy constructor and op= are valid for this class.
   guistring(const guistring&);

   const guistring& operator = (const char*);    // since guistrings autoconvert via *() op
   const guistring& operator = (const wchar_t*); // moves lsb only

   //
   //  You can ask me to make a new buffer copy from my data.  The caller
   //  is responsible for calling delete[] on the returned data.
   //
   char* make_buffer () const;

   // Truncate the string to the specified length.
   void truncate (int newSize);

   // Swap out all 'fromChar' characters to 'toChar' characters.
   void swapChar (const char fromChar, const char toChar);

   void swapOutDelimiter (DELIMITER_CONVERSION conversion = PERIOD_TO_COMMA);

   friend int sprintf_l10n (guistring& destination, const char* format, ...);

protected:
   int  _length;
   char _buffer[buffersize + 1]; // + nonflexible \0 terminator

};

//  global functions for interacting with a guistring
int    vsprintf (guistring& to, const char* fmt, const va_list args);
void   strcpy (guistring& to, const char* __s2);
int    sprintf_l10n (guistring& destination, const char* format, ...);
void   strncpy (guistring& to, const char* __s2, size_t __n);
int    sprintf (guistring&, const char*, ...);
int    sprintf_l10n (guistring&, const char*, ...);
size_t strlen (const guistring&);
void   strcat (guistring&, const char* __s2);
void   strncat (guistring&, const char* __s2, size_t __n);

#endif

/* FORMAT HASH b70644c76c6f0f4ae82049b8f992108a */

/*
 *  Copyright(c) 2007 by Gambro BCT, Inc. All rights reserved.
 *
 * $Header: //depot/main/embedded/Trima/Main/gui/unicode_string/trima_unicode_string.h#1 $
 *
 *	Wide character string class
 *
 * $Log: unicode_string.h $
 * Revision 1.1  2007/05/18 16:19:19Z  wms10235
 * Initial revision
 *
 */

#ifndef _UNICODE_STRING_INCLUDE
#define _UNICODE_STRING_INCLUDE

#include <string>
#include "trima_unicode_string_types.h"

class TrimaUnicodeString
{
public:
   TrimaUnicodeString(void);
   TrimaUnicodeString(const TrimaStringChar c);
   TrimaUnicodeString(const TrimaStringChar c, unsigned int n);
   TrimaUnicodeString(const TrimaStringChar* ucsStr);
   TrimaUnicodeString(const TrimaUnicodeString& other);
   TrimaUnicodeString(const char* utf8Str);
   virtual ~TrimaUnicodeString();

   TrimaStringChar& operator      [] (int i);
   const TrimaStringChar operator [] (int i) const;

   TrimaUnicodeString& operator = (const TrimaStringChar ucsChar);
   TrimaUnicodeString& operator = (const TrimaStringChar* ucsStr);

   TrimaUnicodeString& operator = (const char* utf8Str);
   TrimaUnicodeString& operator += (const char* utf8Str);
   TrimaUnicodeString& operator = (const TrimaUnicodeString& ucsStr);
   TrimaUnicodeString& operator += (const TrimaUnicodeString& ucsStr);
   TrimaUnicodeString& operator += (const TrimaStringChar* ucsStr);
   TrimaUnicodeString operator  + (const TrimaUnicodeString& ucsStr);
   TrimaUnicodeString operator  + (const TrimaUnicodeString& ucsStr) const;
   TrimaUnicodeString operator  + (const TrimaStringChar* ucsStr);
   TrimaUnicodeString operator  + (const TrimaStringChar* ucsStr) const;

   bool operator == (const TrimaStringChar* other) const;
   bool operator == (const TrimaUnicodeString& other) const;
   bool operator != (const TrimaUnicodeString& other) const;
   bool operator < (const TrimaUnicodeString& other) const;
   bool operator > (const TrimaUnicodeString& other) const;
   bool operator <= (const TrimaUnicodeString& other) const;
   bool operator >= (const TrimaUnicodeString& other) const;

   unsigned int getLength () const { return _length; }
   unsigned int getBytes () const { return _length * sizeof(TrimaStringChar); }

   unsigned int getAllocatedLength () const { return _allocatedSize; }
   unsigned int getAllocatedBytes () const { return _allocatedSize * sizeof(TrimaStringChar); }

   // Find a substring in this string. If the substring is
   // found, the zero based index of where the substring started
   // is returned. Otherwise, -1 is returned. The "index" parameter
   // is the zero based index from where the search begins.
   int find (const TrimaStringChar* ucsStr, int index = 0) const;
   int find (const TrimaUnicodeString& ucsStr, int index = 0) const;
   int find (const TrimaStringChar a, int index = 0) const;

   // Find a substring starting from index and search toward the beginning
   // of the string. An index value of -1 indicates the starting point
   // is the end of the string.
   int reverseFind (const TrimaStringChar* ucsStr, int index = -1) const;
   int reverseFind (const TrimaUnicodeString& ucsStr, int index = -1) const;
   int reverseFind (const TrimaStringChar a, int index = -1) const;

   // Insert a string where index is the 0 based index into the string.
   // If index is greater than the string length, then uscStr is
   // concatinated to the end of the current string. If index is zero,
   // ucsStr is inserted in front of the current string. Returns the
   // length of the string after the insert.
   int insert (const TrimaStringChar* ucsStr, int index = 0);
   int insert (const TrimaUnicodeString& ucsStr, int index = 0);

   // Locate and remove a substring from this string starting
   // at index. If the substring is found, the zero based index
   // of where the substring started is returned. Otherwise, -1
   // is returned.
   int removeString (const TrimaStringChar* ucsStr, int index = 0);
   int removeString (const TrimaUnicodeString& ucsStr, int index = 0);

   // Delete a given number of characters from within the string starting
   // at index. If -1 is used for the count, characters are deleted
   // to the end of the string. Returns number of characters removed or -1
   // if the index is not within the string.
   int deleteChar (int index, int count = -1);

   // Return an object that contains a copy of the first
   // (ie leftmost) count of characters.
   TrimaUnicodeString left (int count) const;

   // Return an object that contains a copy of the last
   // (ie rightmost) count of characters.
   TrimaUnicodeString right (int count) const;

   // Copy up to "count" number of characters starting from the zero based
   // index into a new string that is returned. If -1 is used for the count,
   // characters are copied from index to the end of the string.
   TrimaUnicodeString mid (int index, int count = -1) const;

   // copy characters from index to end of string into a new string
   TrimaUnicodeString rightPos (int index) const;

   // Trim any character in the trim string from the this string.
   void trimLeadingChars (const TrimaStringChar* trim);
   void trimTrailingChars (const TrimaStringChar* trim);
   void trimLeadingTrailingChars (const TrimaStringChar* trim);

   // Returns the internal string.
   const TrimaStringChar* getString (void) const;

   // Returns a UTF8 encoded string
   string getUTF8 (void) const;

   bool isEmpty (void) const { return ( _length < 1 ); }

   // Empty the string
   void empty (void);

   // Set a character or get a character. Returns false
   // if the zero based index is out of bounds.
   bool getAt (int index, TrimaStringChar& ucsChar) const;
   bool setAt (int index, TrimaStringChar ucsChar);

   // Replace a character with another, return the number of replacements
   // or zero if no changes.
   int replace (TrimaStringChar chOld, TrimaStringChar chNew);

   // Replace a substring with another, return the number of replacements
   int replace (const TrimaStringChar* oldStr, const TrimaStringChar* newStr);
   int replace (const TrimaUnicodeString& oldStr, const TrimaUnicodeString& newStr);

protected:

   bool         allocate (const unsigned int nsize);
   void         deallocate (void);
   unsigned int indexCheck (int index) const;

   mutable TrimaStringChar* _data;          // Pointer to the unicode string
   mutable unsigned int     _length;        // Number of characters in the string not including NULL terminator
   mutable unsigned int     _allocatedSize; // Allocated size of the _data buffer in characters
};

#endif /* ifndef _UNICODE_STRING_INCLUDE */

/* FORMAT HASH a3323434d0678a8fa8fab227032dfa8e */

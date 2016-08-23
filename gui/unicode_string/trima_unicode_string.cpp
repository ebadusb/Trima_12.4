/*
 *  Copyright(c) 2007 by Gambro BCT, Inc. All rights reserved.
 *
 * $Header: //depot/main/embedded/Trima/Main/gui/unicode_string/trima_unicode_string.cpp#1 $
 *
 * $Log: unicode_string.cpp $
 * Revision 1.3  2008/01/09 21:32:20Z  wms10235
 * IT86 - Unicode mid() method not working correctly.
 *
 */

#include <vxWorks.h>
#include <stdio.h>
#include "trima_ucs_string.h"
#include "trima_unicode_conversion.h"
#include "trima_unicode_string.h"
#include "../trima_ugl_mem.h"

// Minimum number of characters to allocate or to grow string by.
#define minAllocationSize 32

TrimaUnicodeString::TrimaUnicodeString(void)
   : _data(NULL),
     _length(0),
     _allocatedSize(0)
{}

TrimaUnicodeString::~TrimaUnicodeString()
{
   deallocate();
}

TrimaUnicodeString::TrimaUnicodeString(const TrimaStringChar a)
{
   _data   = NULL;
   _length = _allocatedSize = 0;

   if ( allocate(1) )
   {
      _data[0] = a;
      _data[1] = 0;
   }

   if ( a == 0 )
      _length = 0;
}

TrimaUnicodeString::TrimaUnicodeString(const TrimaStringChar c, unsigned int n)
{
   _data   = NULL;
   _length = _allocatedSize = 0;

   if ( (c != 0 && n > 0) && allocate(n) )
   {
      for (unsigned int i = 0; i<n; i++ ) _data[i] = c;
      _data[n] = 0;
   }
}

TrimaUnicodeString::TrimaUnicodeString(const TrimaStringChar* ucsStr)
{
   _data   = NULL;
   _length = _allocatedSize = 0;

   if ( ucsStr && allocate(ucslen(ucsStr) ) )
   {
      memcpy(_data, ucsStr, (_length + 1) * sizeof(TrimaStringChar) );
   }
}

TrimaUnicodeString::TrimaUnicodeString(const TrimaUnicodeString& other)
{
   _data   = NULL;
   _length = _allocatedSize = 0;

   if ( (other._data != NULL && other._length > 0) && allocate(other._length) )
   {
      memcpy(_data, other._data, (other._length + 1) * sizeof(TrimaStringChar) );
   }
}

TrimaUnicodeString::TrimaUnicodeString(const char* utf8Str)
{
   _data   = NULL;
   _length = _allocatedSize = 0;

   if ( utf8Str )
   {
      UnicodeConversion conv;

      if ( conv.utf8ToUcs(utf8Str) == UnicodeConversion::Success )
      {
         *this = conv.getUcsString();
      }
   }
}

bool TrimaUnicodeString::allocate (const unsigned int nsize)
{
   bool retVal = true;

   if ( nsize >= _allocatedSize )
   {
      const unsigned int blockSize = minAllocationSize * ( (nsize + 1) / minAllocationSize + 1 );

      if ( _data )
      {
         // no need to preserve the current data so just free and allocate anew
         GUI_POOL_FREE(_data);
      }

      _data = (TrimaStringChar*)GUI_POOL_MALLOC(blockSize * sizeof(TrimaStringChar) );

      if ( _data )
      {
         _allocatedSize = blockSize;
      }
      else   // allocation failure
      {
         printf("%s - %s(): allocation failure! blockSize=%d _allocatedSize=%d\n",
                taskName(0), __PRETTY_FUNCTION__, blockSize, _allocatedSize);
      }
   }

   if ( _data )
   {
      memset(_data, 0, _allocatedSize * sizeof(TrimaStringChar) );
      _length = nsize;
   }
   else  // allocation failed
   {
      _length = _allocatedSize = 0;
      retVal  = false;
   }

   return retVal;
}

void TrimaUnicodeString::deallocate (void)
{
   GUI_POOL_FREE(_data);
   _data   = NULL;
   _length = _allocatedSize = 0;
}

unsigned int TrimaUnicodeString::indexCheck (int index) const
{
   unsigned int retVal = 0;

   if ( index > 0 )
   {
      retVal = (unsigned int)index;

      if ( retVal > _length )
         retVal = _length;
   }

   return retVal;
}

int TrimaUnicodeString::insert (const TrimaStringChar* ins, int index)
{
   if ( ins )
   {
      size_t nsize = ucslen(ins);

      if ( _data == NULL )
      {
         if ( allocate(nsize) )
         {
            memcpy(_data, ins, nsize * sizeof(TrimaStringChar) );
         }
      }
      else if ( nsize > 0 )
      {
         unsigned int pos     = indexCheck(index);
         unsigned int newSize = _length + nsize + 1;
         unsigned int i;

         if ( _allocatedSize >= newSize )
         {
            for (i = 0; i<_length - pos + 1; i++)
            {
               _data[_length + nsize - i] = _data[_length - i];
            }

            for (i = 0; i<nsize; i++)
            {
               _data[pos + i] = ins[i];
            }
         }
         else
         {
            unsigned int     blockSize = minAllocationSize * ( newSize / minAllocationSize + 1 );

            TrimaStringChar* holder    = (TrimaStringChar*)GUI_POOL_CALLOC(blockSize, sizeof(TrimaStringChar) );

            if ( !holder )
            {
               // allocation failure
               printf("%s - %s(): allocation failure! blockSize=%d _allocatedSize=%d\n",
                      taskName(0), __PRETTY_FUNCTION__, blockSize, _allocatedSize);
               return (int)_length;
            }

            _allocatedSize = blockSize;

            if ( pos > 0 )
            {
               // put first piece
               ucsncpy(holder, _data, pos);
               holder[pos] = 0;
               // append other one
               ucsncat(holder, ins, nsize);
            }
            else
            {
               ucscpy(holder, ins);
            }

            // append remainder
            ucscat(holder, &_data[pos]);

            GUI_POOL_FREE(_data);
            _data = holder;
         }

         _length += nsize;
      }
   }

   return (int)_length;
}

int TrimaUnicodeString::insert (const TrimaUnicodeString& ucsStr, int index)
{
   return insert(ucsStr._data, index);
}

TrimaUnicodeString& TrimaUnicodeString::operator = (const TrimaStringChar a)
{
   if ( allocate(1) )
   {
      _data[0] = a;
      _data[1] = 0;
   }

   if ( a == 0 )
      _length = 0;

   return *this;
}

TrimaUnicodeString& TrimaUnicodeString::operator = (const TrimaStringChar* ucsStr)
{
   if ( _data != ucsStr )
   {
      if ( !ucsStr )
      {
         deallocate();
      }
      else if ( allocate(ucslen(ucsStr) ) )
      {
         memcpy(_data, ucsStr, (_length + 1) * sizeof(TrimaStringChar) );
      }
   }

   return *this;
}

TrimaUnicodeString& TrimaUnicodeString::operator = (const char* utf8Str)
{
   UnicodeConversion conv;
   if ( conv.utf8ToUcs(utf8Str) == UnicodeConversion::Success )
   {
      *this = conv.getUcsString();
   }

   return *this;
}

TrimaUnicodeString& TrimaUnicodeString::operator = (const TrimaUnicodeString& second)
{
   if ( this != &second )
   {
      *this = second._data;
   }

   return *this;
}


TrimaUnicodeString& TrimaUnicodeString::operator += (const TrimaUnicodeString& second)
{
   insert(second._data, _length);
   return *this;
}
TrimaUnicodeString& TrimaUnicodeString::operator += (const TrimaStringChar* str)
{
   insert(str, _length);
   return *this;
}

TrimaUnicodeString& TrimaUnicodeString::operator += (const char* utf8Str)
{
   UnicodeConversion conv;
   if ( conv.utf8ToUcs(utf8Str) == UnicodeConversion::Success )
   {
      *this += conv.getUcsString();
   }

   return *this;
}

TrimaUnicodeString TrimaUnicodeString::operator + (const TrimaUnicodeString& second)
{
   TrimaUnicodeString temp = *this;
   if ( second._length > 0 )
      temp += second;
   return temp;
}

TrimaUnicodeString TrimaUnicodeString::operator + (const TrimaUnicodeString& second) const
{
   TrimaUnicodeString temp = *this;
   if ( second._length > 0 )
      temp += second;
   return temp;
}

TrimaUnicodeString TrimaUnicodeString::operator + (const TrimaStringChar* str)
{
   TrimaUnicodeString temp = *this;

   if ( str && ucslen(str) > 0 )
      temp += str;

   return temp;
}

TrimaUnicodeString TrimaUnicodeString::operator + (const TrimaStringChar* str) const
{
   TrimaUnicodeString temp = *this;

   if ( str && ucslen(str) > 0 )
      temp += str;

   return temp;
}

bool TrimaUnicodeString::operator == (const TrimaStringChar* other) const
{
   bool retVal = false;

   if ( _data && other )
   {
      retVal = ucscmp(_data, other) == 0;
   }
   else if ( other == NULL && _length == 0 )
   {
      retVal = true;
   }

   return retVal;
}

bool TrimaUnicodeString::operator == (const TrimaUnicodeString& other) const
{
   return operator==(other._data);
}

bool TrimaUnicodeString::operator != (const TrimaUnicodeString& other) const
{
   return !operator==(other._data);
}

bool TrimaUnicodeString::operator < (const TrimaUnicodeString& other) const
{
   bool retVal = false;

   if ( other._data && _data )
   {
      retVal = ucscmp(_data, other._data) < 0;
   }

   return retVal;
}

bool TrimaUnicodeString::operator > (const TrimaUnicodeString& other) const
{
   bool retVal = false;

   if ( other._data && _data )
   {
      retVal = ucscmp(_data, other._data) > 0;
   }

   return retVal;
}

bool TrimaUnicodeString::operator <= (const TrimaUnicodeString& other) const
{
   bool retVal = false;

   if ( other._data && _data )
   {
      retVal = ucscmp(_data, other._data) <= 0;
   }
   else if ( other._length == 0 && _length == 0 )
   {
      retVal = true;
   }

   return retVal;
}

bool TrimaUnicodeString::operator >= (const TrimaUnicodeString& other) const
{
   bool retVal = false;

   if ( other._data && _data )
   {
      retVal = ucscmp(_data, other._data) >= 0;
   }
   else if ( other._length == 0 && _length == 0 )
   {
      retVal = true;
   }

   return retVal;
}

TrimaStringChar& TrimaUnicodeString::operator [] (int i)
{
   if ( _data == NULL )
   {
      if ( allocate(1) )
      {
         _length = 0;
      }
      else
      {
         static TrimaStringChar jnk = 0;
         jnk = 0;
         return jnk;
      }
   }

   unsigned int index = indexCheck(i);

   if ( index == _length && _length > 0 )
      index = _length - 1;

   if ( _length == 0 )
      _data[1] = 0;

   return _data[index];
}

const TrimaStringChar TrimaUnicodeString::operator [] (int i) const
{
   TrimaStringChar retVal = 0;

   if ( _data )
   {
      unsigned int index = i;
      // Allow the NULL terminator to be read.
      if ( index > _length )
         index = _length;
      retVal = _data[index];
   }

   return retVal;
}

int TrimaUnicodeString::find (const TrimaStringChar* ucsStr, int index) const
{
   int retVal = -1;

   if ( _data && ucsStr )
   {
      if ( index >= 0 && index < _length )
      {
         TrimaStringChar* ptr = ucsstr(&_data[index], ucsStr);

         if ( ptr )
         {
            retVal = (int)(ptr - _data);
         }
      }
   }

   return retVal;
}

int TrimaUnicodeString::find (const TrimaUnicodeString& ucsStr, int index) const
{
   return find(ucsStr._data, index);
}

int TrimaUnicodeString::find (const TrimaStringChar a, int index) const
{
   int retVal = -1;

   if ( _data )
   {
      if ( index >= 0 && index < _length )
      {
         for ( int i = index; i<_length; i++ )
         {
            if ( a == _data[i] )
            {
               retVal = i;
               break;
            }
         }
      }
   }

   return retVal;
}

int TrimaUnicodeString::reverseFind (const TrimaStringChar a, int index) const
{
   int retVal = -1;

   if ( _data )
   {
      int nIndex = index;

      if ( nIndex == -1 )
      {
         nIndex = (int)_length - 1;
      }

      if ( nIndex >= 0 && nIndex < _length )
      {
         for ( int i = nIndex; i>=0; i-- )
         {
            if ( a == _data[i] )
            {
               retVal = i;
               break;
            }
         }
      }
   }

   return retVal;
}

int TrimaUnicodeString::reverseFind (const TrimaStringChar* ucsStr, int index) const
{
   int retVal = -1;

   if ( _data && ucsStr )
   {
      int len        = (int)_length;
      int nIndex     = index;
      int findStrLen = ucslen(ucsStr);

      if ( nIndex == -1 )
      {
         nIndex = len - findStrLen;
      }

      if ( nIndex > len - findStrLen )
      {
         nIndex = len - findStrLen;
      }

      if ( nIndex >= 0 && nIndex < len )
      {
         for (int i = nIndex; i>=0; i--)
         {
            if ( ucscmp(&_data[i], ucsStr) == 0 )
            {
               retVal = i;
               break;
            }
         }
      }
   }

   return retVal;
}

int TrimaUnicodeString::reverseFind (const TrimaUnicodeString& ucsStr, int index) const
{
   int retVal = -1;

   if ( _data && ucsStr._data && ucsStr._length > 0 )
   {
      int len        = (int)_length;
      int nIndex     = index;
      int findStrLen = (int)ucsStr._length;

      if ( nIndex == -1 )
      {
         nIndex = len - findStrLen;
      }

      if ( nIndex > len - findStrLen )
      {
         nIndex = len - findStrLen;
      }

      if ( nIndex >= 0 && nIndex < len )
      {
         for (int i = nIndex; i>=0; i--)
         {
            if ( ucsStr == &_data[i] )
            {
               retVal = i;
               break;
            }
         }
      }
   }

   return retVal;
}

int TrimaUnicodeString::deleteChar (int index, int count)
{
   int retVal = -1;

   if ( _data )
   {
      int len    = (int)_length;
      int nCount = count;

      if ( nCount == -1 )
      {
         nCount = len - index;
      }

      if ( index + nCount > len )
         nCount = len - index;

      if ( index >= 0 && index < len && nCount > 0 )
      {
         for (int i = index; i<len; i++)
         {
            if ( i + nCount >= len )
            {
               _data[i] = 0;
            }
            else
            {
               _data[i]          = _data[i + nCount];
               _data[i + nCount] = 0;
            }
         }

         retVal   = nCount;
         _length -= (unsigned int)nCount;
      }
   }

   return retVal;
}

int TrimaUnicodeString::removeString (const TrimaStringChar* ucsStr, int index)
{
   int retVal = find(ucsStr, index);

   if ( retVal >= 0 )
   {
      size_t len = ucslen(ucsStr);

      deleteChar(retVal, len);
   }

   return retVal;
}

int TrimaUnicodeString::removeString (const TrimaUnicodeString& ucsStr, int index)
{
   int retVal = find(ucsStr, index);

   if ( retVal >= 0 )
   {
      deleteChar(retVal, ucsStr._length);
   }

   return retVal;
}

TrimaUnicodeString TrimaUnicodeString::left (int count) const
{
   TrimaUnicodeString retVal;

   if ( _length > 0 && count > 0 && _data )
   {
      if ( (unsigned int)count >= _length )
      {
         retVal = *this;
      }
      else if ( retVal.allocate(count) )
      {
         ucsncpy(retVal._data, _data, count);
      }
   }

   return retVal;
}

TrimaUnicodeString TrimaUnicodeString::right (int count) const
{
   TrimaUnicodeString retVal;

   if ( _length > 0 && count > 0 && _data )
   {
      if ( (unsigned int)count >= _length )
      {
         retVal = *this;
      }
      else if ( retVal.allocate(count) )
      {
         ucscpy(retVal._data, &_data[_length - count]);
      }
   }

   return retVal;
}

TrimaUnicodeString TrimaUnicodeString::mid (int index, int count) const
{
   TrimaUnicodeString retVal;

   if ( _length > 0 && _data )
   {
      if ( index >= 0 && index < _length )
      {
         int nCount = count;

         if ( nCount < 0 || index + nCount > _length )
         {
            nCount = (int)_length - index;
         }

         if ( retVal.allocate(nCount) )
         {
            ucsncpy(retVal._data, &_data[index], nCount);
         }
      }
   }

   return retVal;
}

TrimaUnicodeString TrimaUnicodeString::rightPos (int index) const
{
   TrimaUnicodeString retVal;

   if ( _length > 0 && _data )
   {
      if ( index >= 0 && index < _length )
      {
         if ( retVal.allocate(_length - index) )
         {
            ucscpy(retVal._data, &_data[index]);
         }
      }
   }

   return retVal;
}

void TrimaUnicodeString::trimLeadingChars (const TrimaStringChar* trim)
{
   if ( _data && trim && _length > 0 )
   {
      int count = 0;

      for (unsigned int i = 0; i<_length; i++)
      {
         if ( ucschr(trim, _data[i]) != NULL )
         {
            count++;
         }
         else
         {
            break;
         }
      }

      if ( count > 0 )
         deleteChar(0, count);
   }
}

void TrimaUnicodeString::trimTrailingChars (const TrimaStringChar* trim)
{
   if ( _data && trim && _length > 0 )
   {
      int count = 0;
      int index = _length - 1;

      for (int i = index; i>=0; i--)
      {
         if ( ucschr(trim, _data[i]) != NULL )
         {
            index = i;
            count++;
         }
         else
         {
            break;
         }
      }

      if ( count > 0 )
         deleteChar(index, count);
   }
}

void TrimaUnicodeString::trimLeadingTrailingChars (const TrimaStringChar* trim)
{
   trimLeadingChars(trim);
   trimTrailingChars(trim);
}

const TrimaStringChar* TrimaUnicodeString::getString (void) const
{
   if ( _data == NULL )
   {
      _data = (TrimaStringChar*)GUI_POOL_CALLOC(minAllocationSize, sizeof(TrimaStringChar) );
      if ( _data )
      {
         _allocatedSize = minAllocationSize;
         _length        = 0;
      }
   }

   return _data;
}

string TrimaUnicodeString::getUTF8 (void) const
{
   string retVal;

   if ( _data && _length > 0 )
   {
      UnicodeConversion conv;

      if ( conv.ucsToUtf8(*this) == UnicodeConversion::Success )
      {
         retVal = conv.getUtf8String();
      }
   }

   return retVal;
}

void TrimaUnicodeString::empty (void)
{
   deallocate();
}

bool TrimaUnicodeString::getAt (int index, TrimaStringChar& ucsChar) const
{
   bool retVal = false;

   if ( index >= 0 && index < _length )
   {
      ucsChar = _data[index];
      retVal  = true;
   }

   return retVal;
}

bool TrimaUnicodeString::setAt (int index, TrimaStringChar ucsChar)
{
   bool retVal = false;

   if ( index >= 0 && index < _length )
   {
      _data[index] = ucsChar;
      retVal       = true;
   }

   return retVal;
}

int TrimaUnicodeString::replace (TrimaStringChar chOld, TrimaStringChar chNew)
{
   int nCount = 0;

   if ( _data && _length > 0 )
   {
      for (unsigned int i = 0; i<_length; i++)
      {
         if ( _data[i] == chOld )
         {
            _data[i] = chNew;
            nCount++;
         }
      }

      _length = ucslen(_data);
   }

   return nCount;
}

int TrimaUnicodeString::replace (const TrimaStringChar* oldStr, const TrimaStringChar* newStr)
{
   int nCount = 0;
   int posF   = 0, pos = 0;

   if ( _data && oldStr && newStr && _data != newStr )
   {
      posF = find(oldStr, pos);

      if ( posF >= 0 )
      {
         int oldLen = (int)ucslen(oldStr);
         int newLen = (int)ucslen(newStr);

         while ( posF >= 0 )
         {
            deleteChar(posF, oldLen);
            insert(newStr, posF);

            pos = posF + newLen;

            nCount++;

            posF = find(oldStr, pos);
         }
      }
   }

   return nCount;
}

int TrimaUnicodeString::replace (const TrimaUnicodeString& oldStr, const TrimaUnicodeString& newStr)
{
   int nCount = 0;
   int posF   = 0, pos = 0;

   if ( _data && this != &newStr )
   {
      posF = find(oldStr, pos);

      if ( posF >= 0 )
      {
         int oldLen = (int)oldStr.getLength();
         int newLen = (int)newStr.getLength();

         while ( posF >= 0 )
         {
            deleteChar(posF, oldLen);
            insert(newStr, posF);

            pos = posF + newLen;

            nCount++;

            posF = find(oldStr, pos);
         }
      }
   }

   return nCount;
}

/* FORMAT HASH bd3a1595496a87cbb88325a1d7f3095e */

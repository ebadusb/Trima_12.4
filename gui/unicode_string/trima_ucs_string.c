/*
 *  Copyright(c) 2007 by Gambro BCT, Inc. All rights reserved.
 *
 * $Header: //depot/main/embedded/Trima/Main/gui/unicode_string/trima_ucs_string.c#1 $
 *
 *	Common wide character string functions. These functions have
 * the same functionality as the ANSI functions except take TrimaStringChar
 * type strings for UGL compatibility.
 *
 * $Log: ucs_string.c $
 * Revision 1.1  2007/05/18 16:19:17Z  wms10235
 * Initial revision
 *
 */
#include <vxworks.h>
#include "trima_ucs_string.h"

TrimaStringChar* ucscat (TrimaStringChar* destination, const TrimaStringChar* append)
{
   TrimaStringChar* save = destination;

   while ( *destination++ != 0 )         /* find end of string */
      ;

   destination--;

   while ( (*destination++ = *append++) != 0 )
      ;

   return(save);
}

TrimaStringChar* ucschr (const TrimaStringChar* s, int c)
{
   const TrimaStringChar* r = s;

   while ( *r != (TrimaStringChar)c )    /* search loop */
   {
      if ( *r++ == 0 )        /* end of string */
         return(NULL);
   }

   return((TrimaStringChar*)r);
}

int ucscmp (const TrimaStringChar* s1, const TrimaStringChar* s2)
{
   while ( *s1++ == *s2++ )
      if ( s1 [-1] == 0 )
         return(0);

   return((s1 [-1]) - (s2 [-1]));
}

TrimaStringChar* ucscpy (TrimaStringChar* s1, const TrimaStringChar* s2)
{
   TrimaStringChar* save = s1;

   while ( (*s1++ = *s2++) != 0 )
      ;

   return(save);
}

size_t ucscspn (const TrimaStringChar* s1, const TrimaStringChar* s2)
{
   const TrimaStringChar* save = NULL;
   const TrimaStringChar* p    = NULL;
   TrimaStringChar        c1;
   TrimaStringChar        c2;

   for ( save = s1 + 1; (c1 = *s1++) != 0; ) /* search for EOS */
      for ( p = s2; (c2 = *p++) != 0; )      /* search for first occurance */
      {
         if ( c1 == c2 )
            return(s1 - save);                /* return index of substring */
      }

   return(s1 - save);
}


size_t ucslen (const TrimaStringChar* s)
{
   const TrimaStringChar* save = s + 1;

   while ( *s++ != 0 )
      ;

   return(s - save);
}

TrimaStringChar* ucsncat (TrimaStringChar* dst, const TrimaStringChar* src, size_t n)
{
   if ( n != 0 )
   {
      TrimaStringChar* d = dst;

      while ( *d++ != 0 )         /* find end of string */
         ;

      d--;                      /* rewind back of EOS */

      while ( ((*d++ = *src++) != 0) && (--n > 0) )
         ;

      if ( n == 0 )
         *d = 0;              /* NULL terminate string */
   }

   return(dst);
}

int ucsncmp (const TrimaStringChar* s1, const TrimaStringChar* s2, size_t n)
{
   if ( n == 0 )
      return(0);

   while ( *s1++ == *s2++ )
   {
      if ( (s1 [-1] == 0) || (--n == 0) )
         return(0);
   }

   return((s1 [-1]) - (s2 [-1]));
}

TrimaStringChar* ucsncpy (TrimaStringChar* s1, const TrimaStringChar* s2, size_t n)
{
   register TrimaStringChar* d = s1;

   if ( n != 0 )
   {
      while ( (*d++ = *s2++) != 0 )       /* copy <s2>, checking size <n> */
      {
         if ( --n == 0 )
            return(s1);
      }

      while ( --n > 0 )
         *d++ = 0;                /* NULL terminate string */
   }

   return(s1);
}

TrimaStringChar* ucsstr (const TrimaStringChar* s, const TrimaStringChar* find)
{
   const TrimaStringChar* t1 = NULL;
   const TrimaStringChar* t2 = NULL;
   TrimaStringChar        c;
   TrimaStringChar        c2;

   if ( (c = *find++) == 0 )     /* <find> an empty string */
      return((TrimaStringChar*)(s));

   for (;; )
   {
      while ( ((c2 = *s++) != 0) && (c2 != c) )
         ;

      if ( c2 == 0 )
         return(NULL);

      t1 = s;
      t2 = find;

      while ( ((c2 = *t2++) != 0) && (*t1++ == c2) )
         ;

      if ( c2 == 0 )
         return((TrimaStringChar*)(s - 1));
   }
}

/* FORMAT HASH 45c0461110b42c7935d37269c389a3ae */

/*
 *  Copyright(c) 2007 by Gambro BCT, Inc. All rights reserved.
 *
 * $Header: //depot/main/embedded/Trima/Main/gui/unicode_string/trima_ucs_string.h#1 $
 *
 *	Common wide character string functions. These functions have
 * the same functionality as the ANSI functions except take TrimaStringChar
 * type strings for UGL compatibility.
 *
 * $Log: ucs_string.h $
 * Revision 1.1  2007/05/18 16:19:18Z  wms10235
 * Initial revision
 *
 */

#ifndef _UCS_STRING_INCLUDE_
#define _UCS_STRING_INCLUDE_

#include "trima_unicode_string_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Wide character strcat - equivalent to ANSI wcscat */
TrimaStringChar* ucscat (TrimaStringChar* destination, const TrimaStringChar* append);

/* Wide character strchr - equivalent to ANSI wcschr */
TrimaStringChar* ucschr (const TrimaStringChar* s, int c);

/* Wide character strcmp - equivalent to ANSI wcscmp */
int ucscmp (const TrimaStringChar* s1, const TrimaStringChar* s2);

/* Wide character strcpy - equivalent to ANSI wcscpy */
TrimaStringChar* ucscpy (TrimaStringChar* s1, const TrimaStringChar* s2);

/* Wide character strcspn - equivalent to ANSI wcsspn */
size_t ucscspn (const TrimaStringChar* s1, const TrimaStringChar* s2);

/* Wide character strlen - equivalent to ANSI wcslen */
size_t ucslen (const TrimaStringChar* s);

/* Wide character strncat - equivalent to ANSI wcscat */
TrimaStringChar* ucsncat (TrimaStringChar* dst, const TrimaStringChar* src, size_t n);

/* Wide character strncmp - equivalent to ANSI wcsncmp */
int ucsncmp (const TrimaStringChar* s1, const TrimaStringChar* s2, size_t n);

/* Wide character strncpy - equivalent to ANSI wcsncpy */
TrimaStringChar* ucsncpy (TrimaStringChar* s1, const TrimaStringChar* s2, size_t n);

/* Wide character strstr - equivalent to ANSI wcsstr */
TrimaStringChar* ucsstr (const TrimaStringChar* s, const TrimaStringChar* find);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _UCS_STRING_INCLUDE_ */

/* FORMAT HASH c5121a04b3c00d5a4257a553b9d76764 */

/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Format_date

*     File name:    frmtdate.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class supports formatting time/date stamp strings.

*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.1  1998/09/04 16:38:22  tw09453
Initial revision

*/


// Single include define guardian
#ifndef FRMTDATE_HPP
#define FRMTDATE_HPP

#include "guistring.h"

class Format_date
{

public:

   // CONSTRUCTOR
   Format_date();

   // DESTRUCTOR
   ~Format_date();

   // FORMAT_DATE
   // This routine accepts a string representation of a date number in the
   // form of YYYYMMDD. The machine parameters will determine which of three
   // formats it will present the date: mm/dd/yy, dd/mm/yy, yy/mm/dd.
   //
   // The date will be formatted and placed in the same buffer passed to this
   // routine, therefore the string buffer MUST be large enough to contain
   // these three formats plus a zero byte -- but for future use should be
   // able to contain a FULL year format as well. The buffer should be at
   // least 11 characters in length.
   //
   void format_date (char* str_date) ;

   // Appends the current date (year, month, day) and time (hour, minute, second)
   // as YYYY<x>mm<x>dd<y>HH<z>MM<z>SS, where <x>, <y>, and <z> are delimiters.
   //
   void appendDateAndTime (guistring& str, char x, char y, char z);

};  // END of Format_date Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH fc234321219c2968f043b0684da19a70 */

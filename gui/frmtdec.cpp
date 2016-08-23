/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Format_decimal

*     File name:   frmtdec.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/frmtdec.cpp 1.7 2002/11/11 06:18:01Z sb07663 Exp dslausb $
$Log: frmtdec.cpp $
Revision 1.7  2002/11/11 06:18:01Z  sb07663
changed char[] constructs to guistring equivalents
Revision 1.6  2002/08/16 16:33:03  sb07663
added <vxworks.h> include
Revision 1.5  2002/07/02 20:47:32  sb07663
config CDS implementation
Revision 1.4  2002/06/24 13:47:13  sb07663
Get decimal delimiter from config cds
Revision 1.3  2002/05/07 13:56:29  sb07663
First cut at VxWorks compatibility
Revision 1.2  1997/03/22 02:08:40  tw09453
Do something in the switch default for the compiler's sake.
Revision 1.1  1997/03/22 01:29:47  tw09453
Initial revision

*/
#include <vxworks.h>


// Focussed #INCLUDES

#include "frmtdec.hpp"

// External References

#include "guiglobs.hpp"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Format_decimal::Format_decimal ()
{}  // END of Format_decimal CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Format_decimal::~Format_decimal ()
{}  // END of Format_decimal DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// FORMAT_DEC

void Format_decimal::format_dec (guistring& str_decimal)
{
   if ((int)guiglobs::cdsData.config.LangUnit.Get().key_decimal_delimiter == (int)COMMA_DELIMIT)
   {
      return; // no change to string is necessary since thousands are delimited with
              //  commas.
   }

   guistring temp;

   // Loop through the number representation changing all commas to periods
   //  and visa versa.


   for (int index = 0; index < str_decimal.length(); index++)
   {
      char current = str_decimal[index];
      if (current == '.')
      {
         temp += ',';
      }
      else if (current == ',')
      {
         temp += '.';
      }
      else
      {
         temp += current;
      }

   } // End of "while (*current_char != '\0')

   str_decimal = temp;

}  // END of Format_decimal FORMAT_DEC

/* FORMAT HASH 3c7708b531fd44e2b7267e0cf0c33c7f */

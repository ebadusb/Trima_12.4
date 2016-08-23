/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael J Cobb

*     Class name:  format_string

*     File name:   frmtstr.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/frmtstr.cpp 1.9 2008/08/20 21:26:23Z dslausb Exp dslausb $
$Log: frmtstr.cpp $
Revision 1.9  2008/08/20 21:26:23Z  dslausb
Fix decimal delimiter formatting error that Mark introduced (that damned dirty ape).
Revision 1.8  2008/08/16 19:10:46Z  dslausb
Mark's Screenshot Code
Revision 1.8  2008/08/15 20:35:35Z  mereusb
IT 8107: Change for screenshot scripts.
Revision 1.7  2002/11/22 09:43:52  sb07663
Updates to fix strtok anomalies
Revision 1.6  2002/11/11 06:19:00Z  sb07663
changed char[] constructs to guistring equivalents
Revision 1.5  2002/08/16 16:33:04  sb07663
added <vxworks.h> include
Revision 1.4  2002/05/07 13:56:30  sb07663
First cut at VxWorks compatibility
Revision 1.3  1997/06/03 21:13:59  MC03015
Modification of while pointer condition to NULL
Revision 1.2  1997/05/22 00:44:24  MC03015
Modification of string management, placement in frmtstr
Revision 1.1  1997/05/20 14:11:52  MC03015
Initial revision

*/

#include <vxworks.h>

// Focussed #INCLUDES

#include "frmtstr.hpp"

// External References

#include "guiglobs.hpp"


////////////////////////////////////////////////////////////////////////////////
// CONVERT_NUMBER_TO_STRING

void format_string::convert_number_to_string (guistring& convert_to_string,
                                              const char* format_specifier,
                                              float starting_value,
                                              int max_length_string)

{

   // local storage for error charactr string
   guistring      string_length_error_buffer;

   Format_decimal convert_point_to_comma;

   // reinitialize the string length error buffer
   strcpy(string_length_error_buffer, ERROR_STRING);

   // convert existing numeric system values to text
   sprintf_l10n(convert_to_string, format_specifier, starting_value);

   // check string length to see if it is within range, then display as
   // appropriate
   if (strlen(convert_to_string) > max_length_string)
   {
      DataLog (log_level_gui_error) << "ERROR: strlen(\"" << convert_to_string << "\")=="
                                    << strlen(convert_to_string) << "; max=" << max_length_string
                                    << "; truncating to \"";

      // concat most prevelant string values to ERROR_STRING and prepare for output
      strncat(string_length_error_buffer, convert_to_string, max_length_string - 1);
      strncpy(convert_to_string, string_length_error_buffer, max_length_string);

      DataLog (log_level_gui_error) << convert_to_string << "\"" << endmsg;
   }
}

/* FORMAT HASH 49715958a12c88722593be9b9e57c4c2 */

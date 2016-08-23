/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996, 1997 COBE BCT, Inc. All rights reserved

*     Author:       Michael J Cobb

*     Class name:   format_string

*     File name:    frmttsr.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class will accept a string and perform predefined
         operations on the string and subsequently return a pointer to the
         string.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/frmtstr.hpp 1.10 2008/08/16 19:10:49Z dslausb Exp dslausb $
$Log: frmtstr.hpp $
Revision 1.10  2008/08/16 19:10:49Z  dslausb
Mark's Screenshot Code
Revision 1.10  2008/08/15 20:35:41Z  mereusb
IT 8107: Change for screenshot scripts.
Revision 1.9  2008/06/03 11:16:01  dslausb
IT 8556 - First draft of procedure selection screen flow adjustment
Revision 1.8  2008/04/24 16:56:08Z  dslausb
IT 8590 - Configureable HGB units
Revision 1.7  2002/11/22 16:43:52Z  sb07663
Updates to fix strtok anomalies
Revision 1.6  2002/11/11 06:19:26Z  sb07663
changed char[] constructs to guistring equivalents
Revision 1.5  2002/05/07 13:56:31  sb07663
First cut at VxWorks compatibility
Revision 1.4  1999/05/21 18:09:41  TW09453
Permit formatting of DRBC volumes greater than 3 digits (since RBC's
can be up to 1000 ml)
Revision 1.3  1998/02/25 21:20:53  MC03015
removal of carriage return format string specifier, replaced by text id's for large buttons
Revision 1.2  1997/05/22 00:44:25  MC03015
Modification of string management, placement in frmtstr
Revision 1.1  1997/05/20 14:11:53  MC03015
Initial revision

*/


// Single include define guardian
#ifndef FRMTSTR_HPP
#define FRMTSTR_HPP

// Include for format decimal
#include "frmtdec.hpp"

// Include general gui manifests
#include "gui.h"


////////////////////////////////////////////////////////////////////////////////
// string construction and manipulation #defines////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Character buffers used in string manipulation in the GUI screens

#define  NUMBER_CONV_BUFFER    20
#define  STRING_BUFFER_1C     100
#define  STRING_BUFFER_2C     200


////////////////////////////////////////////////////////////////////////////////
// Token for creating tokenized strings to be passed in the allocation parameter
// to the keypad parsing

#define  STRING_TOKEN           "~"


////////////////////////////////////////////////////////////////////////////////
// Formatting characters and strings used to build allocation parameter strings
// for the keypad screen.  NOTE: If the value for the formatting character
// is changed, the values defined in the keypad types below must be changed

#define  NUMBER_FORMAT_CHAR   '#'  // char for keypad format compare


////////////////////////////////////////////////////////////////////////////////
// Blank strings and chars for string building and checking, primarily used in
// the construction of dynamic text values.  Specifically, the text on large
// button faces and the return values received from procedure.

#define  BLANK_CHAR      ' ' // blank char for strcmps
#define  BLANK_STRING    " " // blank string for strcmps


////////////////////////////////////////////////////////////////////////////////
// Error string character for creating error strings for display to prevent text
// field overrun on a return value from procedure

#define  ERROR_STRING           "*"


////////////////////////////////////////////////////////////////////////////////
// Maximum string length of numeric donor parameters for length error checking
// prevent text field overrun on a return value from procedure

#define  MAX_LENGTH_HEIGHT_US_FT_ENTRY        1
#define  MAX_LENGTH_HEIGHT_US_IN_ENTRY        2
#define  MAX_LENGTH_HEIGHT_ENTRY              3
#define  MAX_LENGTH_WEIGHT_ENTRY              3
#define  MAX_LENGTH_HEMATOCRIT_ENTRY          2 /* ## */
#define  MAX_LENGTH_HEMOGLOBIN_ENTRY          5 /* ##.# */
#define  MAX_LENGTH_PLATELET_PRECOUNT_ENTRY   3
#define  MAX_LENGTH_TIME_ENTRY                3
#define  MAX_LENGTH_FLOW_RATE_ENTRY           3 /* ### */

#define  MAX_LENGTH_TBV_ENTRY                 6
#define  MAX_LENGTH_PLATELET_ENTRY            5
#define  MAX_LENGTH_PLASMA_ENTRY              5
#define  MAX_LENGTH_RBCS_ENTRY                5

#define  MAX_LENGTH_PLATELET_PRECOUNT_DISPLAY 9


class format_string
{

public:

   // CONVERT_NUMBER_TO_STRING
   void convert_number_to_string (guistring& output, const char* format, float value, int maxlength);

};  // END of format_string Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH c2c1207ad57d4813e9d9900a1d3fa2dc */

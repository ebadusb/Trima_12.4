/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996, 1997 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Format_decimal

*     File name:    frmtdec.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class will accept a decimal number in string form and
         convert it (if necessary) to the appropriate European or American
         delimited format - as determined by the configuration parameters.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: L:/BCT_Development/vxWorks/Trima/gui/rcs/frmtdec.hpp 1.2 2002/05/07 13:56:29 sb07663 Exp $
$Log: frmtdec.hpp $
Revision 1.2  2002/05/07 13:56:29  sb07663
First cut at VxWorks compatibility
Revision 1.1  1997/03/22 01:29:48  tw09453
Initial revision

*/


// Single include define guardian
#ifndef FRMTDEC_HPP
#define FRMTDEC_HPP

// Include general gui manifests
#include "gui.h"
#include "guistring.h"

class Format_decimal
{

public:


   // CONSTRUCTOR
   Format_decimal();


   // DESTRUCTOR
   virtual ~Format_decimal();

   // FORMAT_DEC
   // This routine accepts a string representation of a decimal number with
   // optional comma's separating each thousandths place and an optional
   // decimal point separating the integer portion from the decimal portion
   // of the number. It will then check the configuration object to see if the
   // Focussed system is in the European (comma = decimal point) mode or
   // the American (decimal point = decimal point) mode. If it is in the
   // American mode it will return immediately with no change to the string-
   // represented decimal number. If it is in the European mode, it will
   // change all comma's to periods and the decimal period to a comma.
   void format_dec (guistring& str_decimal) ;



};  // END of Format_decimal Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH c23b3f6327ade0f552aa49e9b385376c */

/*******************************************************************************
 *
 * Copyright (c) 2013 by Terumo BCT, Inc.  All rights reserved.
 *
 * TITLE:      adjust.cpp
 *             Definitions for adjustment messages.
 *
 * AUTHOR:     Regis McGarry
 *
 *
 ******************************************************************************/

#include "adjust.h"



//
// Decode Predict results enum for dlog file
//
DataLog_Stream & operator << (DataLog_Stream& os,  const BUTTON_VISIBILITY_STATUS& value)
{
   switch (value)
   {
      case BUTTON_INVISIBLE :
         os << "BUTTON INVISIBLE";
         break;

      case BUTTON_GREY :
         os << "BUTTON GREY";
         break;

      case BUTTON_VISIBLE :
         os << "BUTTON VISIBLE";
         break;
   }

   os << "(" << int(value) << ")";

   return os;
}

/* FORMAT HASH c82ef838dc9e36532fee124e4dac2ae2 */

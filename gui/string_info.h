/*
 *	Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/string_info.h 1.53 2008/02/07 19:50:14Z dslausb Exp $
 *
 **********************************************************************************
 NOTE: This file is now shared with Vista. Take care when adding new dependencies
 **********************************************************************************/

#ifndef _STRING_INFO_INCLUDE
#define _STRING_INFO_INCLUDE

#include "string_id.h"

////////////////////////////////////////////////////////////////////////////////
// These are the standard colors used by the Display_Text class

#ifdef WIN32
#ifdef TRANSPARENT
#undef TRANSPARENT
#endif

#else // Not Win32: datalog does not exist in win32 tools.
#include "datalog.h"
#endif

// #ifndef __ALARM_SPATIAL_DEFINITIONS__
// #define __ALARM_SPATIAL_DEFINITIONS__
#define  TEXT_ALARM_BITMAP_DISPLAY_X   54
#define  TEXT_ALARM_BITMAP_DISPLAY_Y  146
#define  TEXT_ALARM_BITMAP_DISPLAY_H   14
#define  TEXT_ALARM_BITMAP_DISPLAY_W  182
#define  TEXT_ALARM_ONLY_DISPLAY_X     54
#define  TEXT_ALARM_ONLY_DISPLAY_Y    146
#define  TEXT_ALARM_ONLY_DISPLAY_H     14
#define  TEXT_ALARM_ONLY_DISPLAY_W    407
// #endif

enum TEXT_COLOR
{
   TRANSPARENT,     // default background text color
   GREY_DARK,
   GREY_LIGHT,
   YELLOW,
   WHITE,
   BLACK,
   RED,
   MAROON,
   NAVY_BLUE,
   LAST_TEXT_COLOR  // must be last entry
};

////////////////////////////////////////////////////////////////////////////////
// These are the standard alignments used by the Display_Text class

enum TEXT_ALIGN
{
   LEFT,
   RIGHT,
   CENTER
};


///////////////////////////////////////////////////////////////////////////////
// Cross Language manifests (no internationalization dependencies)
//
//


// A1,A2 Alarms: parameters for display of alarm number in lower right of
//               alarm screen -- independent of language.
#define  TEXT_ALARM_NUMBER_DISPLAY_COLOR    MAROON
#define  TEXT_ALARM_NUMBER_DISPLAY_FONTSZ   16
#define  TEXT_ALARM_NUMBER_DISPLAY_X        120
#define  TEXT_ALARM_NUMBER_DISPLAY_Y        115
#define  TEXT_ALARM_NUMBER_DISPLAY_H        25
#define  TEXT_ALARM_NUMBER_DISPLAY_W        100


#ifndef WIN32 // DataLog_Stream does not exist in win32 tools.

static DataLog_Stream& operator << (DataLog_Stream& os, TEXT_COLOR color)
{
   switch (color)
   {
      case TRANSPARENT :       os << "TRANSPARENT";        break;
      case GREY_DARK :         os << "GREY_DARK";          break;
      case GREY_LIGHT :        os << "GREY_LIGHT";         break;
      case YELLOW :            os << "YELLOW";             break;
      case WHITE :             os << "WHITE";              break;
      case BLACK :             os << "BLACK";              break;
      case RED :               os << "RED";                break;
      case MAROON :            os << "MAROON";             break;
      case NAVY_BLUE :         os << "NAVY_BLUE";          break;
      case LAST_TEXT_COLOR :   os << "LAST_TEXT_COLOR";    break;
      default :                os << "UNKNOWN";            break;
   }

   return os << "(" << (int)color << ")";
}

////////////////////////////////////////////////////////////////////////////////
// These are the standard alignments used by the Display_Text class
static DataLog_Stream& operator << (DataLog_Stream& os, TEXT_ALIGN align)
{
   switch (align)
   {
      case LEFT :  os << "LEFT";   break;
      case RIGHT : os << "RIGHT";  break;
      case CENTER : os << "CENTER"; break;
      default :    os << "UNKNOWN"; break;
   }

   return os << "(" << (int)align << ")";
}

#endif // #ifndef WIN32

#endif /* ifndef _STRING_INFO_INCLUDE */

/* FORMAT HASH 076c364ca0dadd36497a8cad2977c183 */

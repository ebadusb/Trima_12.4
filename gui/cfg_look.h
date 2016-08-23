/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  none

*     File name:   cfg_look.h

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************

$Log: cfg_look.h $
Revision 1.12  2002/11/20 17:48:19  rm70006
make const's static to work with T2.2 compiler.
Revision 1.11  2002/07/18 22:09:23Z  rm70006
Changed to be a class.  replaced defines with types.
Revision 1.10  2000/10/04 16:43:27Z  sb07663
Config.dat modifications for new volume removal limits
Revision 1.9  2000/08/14 18:33:51  sb07663
IT4564: consolidation of config.dat parameter tables
Revision 1.8  2000/02/28 21:54:22  SB07663
IT4612: initial checkin for single stage user configuration
Revision 1.7  1997/03/12 18:35:18  Barry_Fulkerson
- added gray'ed out scroll buttons to listbox.
Revision 1.6  1997/03/06 03:41:37  Barry_Fulkerson
- move all button x/y cord. info to cfg_look.h
Revision 1.5  1997/03/03 22:30:24  Barry_Fulkerson
- this time I am really adding the previous changes
Revision 1.4  1997/03/03 22:04:21  Barry_Fulkerson
- changed the reference to the new config scroll buttons to include changes
  from Christelle @ Hauser
Revision 1.3  1997/02/20 17:58:24  Barry_Fulkerson
- added reference to up/down scroll buttons
Revision 1.2  1996/10/07 19:52:40  Barry_Fulkerson
added File Title Block comment section and MKS keywords

*/

#ifndef __CFG_LOOK
#define __CFG_LOOK

#include "bitmap_info.h"
#include "string_info.h"


class CFG_Look
{
public:
   static const BITMAP_ID  BUTTON_UP_NAME;
   static const BITMAP_ID  BUTTON_DOWN_NAME;
   static const BITMAP_ID  BUTTON_UP_SCROLL_NAME;
   static const BITMAP_ID  BUTTON_DOWN_SCROLL_NAME;
   static const BITMAP_ID  BUTTON_SCROLL_DOWN;
   static const BITMAP_ID  BUTTON_SCROLL_GRAY_UP;
   static const BITMAP_ID  BUTTON_SCROLL_GRAY_DN;

   static const TEXT_COLOR FONT_COLOR;
   static const TEXT_COLOR FONT_INACTIVE_COLOR;

   static const TEXT_ALIGN FONT_ALIGN;

   static const int        FONT_SIZE;

   static const int        FIRST_X;
   static const int        SECOND_X;
   static const int        FIRST_Y;
   static const int        Y_DIFF;
   static const int        LISTBOX_X;
   static const int        LISTBOX_X_2;
   static const int        KEYPAD_X;
   static const int        KEYPAD_X_2;

   // Lookup function for color-coded buttons based on cassette reference number
   // Returns the up/down bitmap pair for an element in a list box of cassette refs.
   static void lookupListButtonBitMapByCassetteRefNum (const char* refNum,
                                                       BITMAP_ID& bmid_up,
                                                       BITMAP_ID& bmid_down);

   // Lookup function for color-coded button based on cassette reference number
   // Returns the up/down bitmap pair for the big System Reference button.
   static void lookupSysRefButtonBitMapByCassetteRefNum (const char* refNum,
                                                         BITMAP_ID& bmid_up,
                                                         BITMAP_ID& bmid_down);
};

#endif

//
///////////////////////////////////////////////////////////////////
// end of cfg_look.h file

/* FORMAT HASH 5b9bafbec9e89f27eb6d78ba059303ef */

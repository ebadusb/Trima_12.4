/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  none

*     File name:   cfg_look.cpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************

$Log: cfg_look.cpp $
Revision 1.1  2002/11/20 17:47:51Z  rm70006
Initial revision

*/


#include "cfg_look.h"
#include "software_cds.h"

const BITMAP_ID  CFG_Look::BUTTON_UP_NAME          = BITMAP_CONFIG_SUB_UP;
const BITMAP_ID  CFG_Look::BUTTON_DOWN_NAME        = BITMAP_CONFIG_SUB_DOWN;
const BITMAP_ID  CFG_Look::BUTTON_UP_SCROLL_NAME   = BITMAP_CONFIG_UP_SCROLL;
const BITMAP_ID  CFG_Look::BUTTON_DOWN_SCROLL_NAME = BITMAP_CONFIG_DOWN_SCROLL;
const BITMAP_ID  CFG_Look::BUTTON_SCROLL_DOWN      = BITMAP_CONFIG_SCROLL_DOWN;
const BITMAP_ID  CFG_Look::BUTTON_SCROLL_GRAY_UP   = BITMAP_CONFIG_SCROLL_GRAY_UP;
const BITMAP_ID  CFG_Look::BUTTON_SCROLL_GRAY_DN   = BITMAP_CONFIG_SCROLL_GRAY_DN;

const TEXT_COLOR CFG_Look::FONT_COLOR              = YELLOW;
const TEXT_COLOR CFG_Look::FONT_INACTIVE_COLOR     = WHITE;
const TEXT_ALIGN CFG_Look::FONT_ALIGN              = CENTER;

const int        CFG_Look::FONT_SIZE               = 14;

const int        CFG_Look::FIRST_X                 = 150;
const int        CFG_Look::SECOND_X                = 450;
const int        CFG_Look::FIRST_Y                 = 115;
const int        CFG_Look::Y_DIFF                  = 80;
const int        CFG_Look::LISTBOX_X               = 270;
const int        CFG_Look::LISTBOX_X_2             = 300;
const int        CFG_Look::KEYPAD_X                = 270;
const int        CFG_Look::KEYPAD_X_2              = 300;


// Current needs require only 2 color-coded Cassette Ref buttons, and are for
// Japan only. Consider a more sophisticated map<> if more entries are required.
//
void CFG_Look::lookupListButtonBitMapByCassetteRefNum (const char* refNum,
                                                       BITMAP_ID& bmid_up,
                                                       BITMAP_ID& bmid_down)
{
   // Set the default return values
   bmid_up   = BUTTON_UP_NAME;
   bmid_down = BUTTON_DOWN_NAME;

   if (Software_CDS::isJapaneseWorkflow())
   {
      if (strcmp(refNum, "80337") == 0)
      {
         bmid_up   = BITMAP_CONFIG_SUB_UP_BLUE;
         bmid_down = BITMAP_CONFIG_SUB_DOWN_BLUE;
      }
      else if (strcmp(refNum, "80537") == 0)
      {
         bmid_up   = BITMAP_CONFIG_SUB_UP_ORANGE;
         bmid_down = BITMAP_CONFIG_SUB_DOWN_ORANGE;
      }
   }
}


// This returns the bitmap ids to use on the Sys Ref button once a cassette
// has been selected (has a check mark super-imposed)
//
void CFG_Look::lookupSysRefButtonBitMapByCassetteRefNum (const char* refNum,
                                                         BITMAP_ID& bmid_up,
                                                         BITMAP_ID& bmid_down)
{
   // Set the default return values
   bmid_up   = BITMAP_CASSETTE_BTN_CHECKMARK;
   bmid_down = BITMAP_BUTTON_SETUP_DONOR_DOWN;

   if (Software_CDS::isJapaneseWorkflow())
   {
      if (strcmp(refNum, "80337") == 0)
      {
         bmid_up   = BITMAP_CASSETTE_BTN_CHECKMARK_BLUE;
         bmid_down = BITMAP_CASSETTE_BTN_DOWN_BLUE;
      }
      else if (strcmp(refNum, "80537") == 0)
      {
         bmid_up   = BITMAP_CASSETTE_BTN_CHECKMARK_ORANGE;
         bmid_down = BITMAP_CASSETTE_BTN_DOWN_ORANGE;
      }
   }
}

/* FORMAT HASH 6545feb404e70d4e6c700ce8688eae30 */

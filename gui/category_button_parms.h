/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT Inc. All rights reserved

*     Author:       Terry Wahl

*     Class names:  N/A

*     File name:    category_button_parms.h

*     Contents:     See prototype header descriptions below...

*     Description:

         This file contains manifests and enums used by the category_button
         and the category_button_collection
         ]
*************************< FOCUSSED GUI DOMAIN >***************************/

// Single include define guardian
#ifndef CATEGORY_BUTTON_PARMS
#define CATEGORY_BUTTON_PARMS


#include "bitmap.h"


// Essentially allowing unlimited scans to be entered in a category.
//
#define  UNLIMITED_SCANS   10000


// Category rank enum
//
enum CATEGORY_RANK
{
   RANK_1      =1,
   RANK_2      =2,
   RANK_3      =3,

   FIRST_RANK  =RANK_1,
   LAST_RANK   =RANK_2, // NOTE: one less than the total defined -- currently only two are used

   UNKNOWN_RANK=0
};


#define CARTANCHOR_X ((int)14)
#define CARTANCHOR_Y ((int)174)
#define XMARGIN 12
#define YMARGIN 12
#define INTERCOL 17
#define INTERROW 12
#define BUTTON_HT 83
#define BUTTON_WD 104

#define BTN_Y_COORD(ROWNUM) (CARTANCHOR_Y + YMARGIN + ((ROWNUM)-1) * (BUTTON_HT + INTERROW))
#define BTN_X_COORD(COLNUM) (CARTANCHOR_X + XMARGIN + ((COLNUM)-1) * (BUTTON_WD + INTERCOL))

// End of single include define guardian
#endif

/* FORMAT HASH a1ebc176605232501ffd7a8342953662 */

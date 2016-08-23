/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  no classes, just structure definitions

*     File name:   cfg_kstr.hpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: M:/BCT_Development/Trima/gui/rcs/cfg_kstr.hpp 1.18 2000/02/28 21:54:18 SB07663 Exp $
*/


#ifndef CFG_KSTR_H
#define CFG_KSTR_H

/** include files **/

#include "cfg_ids.h"


//
///////////////////////////////////////////////////////////////////
// structure definitions

typedef struct
{
   CONFIG_DATA_KEYS            key_name;
   CONFIG_DATA_LISTBOX_STRINGS list_name;
   TEXT_STRING_ID              txtid;
} THE_CONFIG_LIST_STRUC;

//
//
//  Global structure type definition
//
typedef struct
{
   CONFIG_DATA_KEYS            key_name;
   CONFIG_DATA_LISTBOX_STRINGS key_value;
}  GUI_LISTBOX_STRUCT;


// End of single include define guardian
#endif
//
///////////////////////////////////////////////////////////////////
// end of cfg_kstr.hpp file

/* FORMAT HASH 827bbc1f88af6bfca86fc46f29fca94c */

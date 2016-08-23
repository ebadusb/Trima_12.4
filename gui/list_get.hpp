/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Get_config_listbox_names

*     File name:   list_get.hpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/list_get.hpp 1.8 2003/01/06 18:31:15Z jl11312 Exp dslausb $
$Log: list_get.hpp $
Revision 1.8  2003/01/06 18:31:15Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.7  2002/11/11 06:35:52  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.6  2002/05/07 13:58:11  sb07663
First cut at VxWorks compatibility
Revision 1.5  2000/08/14 18:35:23  sb07663
IT4564: consolidation of config.dat parameter tables
Revision 1.4  1996/10/14 23:28:44  Barry_Fulkerson
- changed listbox from Photon style to an actual list of buttons
- added x,y location capability to listbox
Revision 1.3  1996/10/09 23:35:11  Barry_Fulkerson
- changed listbox to return CONFIG_KEY types instead of chars
Revision 1.2  1996/10/07 19:52:57  Barry_Fulkerson
added File Title Block comment section and MKS keywords


*/

#ifndef LIST_GET_HPP
#define LIST_GET_HPP

/** include files **/

#include "cfg_ids.h"
#include "display_text.h"
#include "cfg_kstr.hpp"

extern THE_CONFIG_LIST_STRUC config_list_table[];
//
///////////////////////////////////////////////////////////////////
//
class Get_config_listbox_names

{
private:

   char* get_text_from_textwidget (TEXT_STRING_ID txtid);

protected:
   Display_Text text_widget;

public:

   Get_config_listbox_names();

   virtual ~Get_config_listbox_names ();

   TEXT_STRING_ID return_string_for_ID     (CONFIG_DATA_KEYS index_key, CONFIG_DATA_LISTBOX_STRINGS list_name);

   CONFIG_DATA_LISTBOX_STRINGS actual_listbox_string_id (CONFIG_DATA_KEYS index_key, int listbox_position);

};

// End of single include define guardian
#endif

//
///////////////////////////////////////////////////////////////////
// end of list_get.hpp file

/* FORMAT HASH 597d27c459e40b4cfe92a0cd686b7ad8 */

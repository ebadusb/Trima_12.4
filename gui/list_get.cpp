/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Get_config_listbox_names

*     File name:   list_get.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: L:/BCT_Development/vxWorks/Trima/gui/rcs/list_get.cpp 1.5 2002/08/15 22:16:42 sb07663 Exp $
$Log: list_get.cpp $
Revision 1.5  2002/08/15 22:16:42  sb07663
Compiling version for Vx
Revision 1.4  1996/10/14 23:28:44  Barry_Fulkerson
- changed listbox from Photon style to an actual list of buttons
- added x,y location capability to listbox
Revision 1.3  1996/10/09 23:34:44  Barry_Fulkerson
-changed listbox to return CONFIG_KEY types instead of chars
Revision 1.2  1996/10/07 19:52:56  Barry_Fulkerson
added File Title Block comment section and MKS keywords

*/
#include <vxworks.h>

/** include files **/

#include "list_get.hpp"
#include "error.h"

//
///////////////////////////////////////////////////////////////////
//
Get_config_listbox_names::Get_config_listbox_names()
{}
//
///////////////////////////////////////////////////////////////////
//

Get_config_listbox_names::~Get_config_listbox_names     ()
{}
//
///////////////////////////////////////////////////////////////////
//

//
///////////////////////////////////////////////////////////////////
//

TEXT_STRING_ID Get_config_listbox_names::return_string_for_ID (CONFIG_DATA_KEYS index_key,
                                                               CONFIG_DATA_LISTBOX_STRINGS list_name_index
                                                               )
{
   int index = 0;

   while ((config_list_table[index].key_name) != LAST_CONFIG_INDEX_KEY)
   {
      if (((config_list_table[index].key_name) == index_key) && ((config_list_table[index].list_name) == list_name_index))
      {
         TEXT_STRING_ID textid = (config_list_table[index].txtid);
         return(textid);
      }  // end of if statement

      index++; // keep looping until key_name is found

   } // end of while loop

   return(TEXT_NULL);
} // end of method
//
///////////////////////////////////////////////////////////////////
//
CONFIG_DATA_LISTBOX_STRINGS Get_config_listbox_names::actual_listbox_string_id (CONFIG_DATA_KEYS index_key,
                                                                                int listbox_position
                                                                                )
{

   int index = 0;
   // printf("list_get: the index / and pos is: %d, %d \n", index_key, listbox_position);
   while ((config_list_table[index].key_name) != LAST_CONFIG_INDEX_KEY)
   {
      // printf("list_get: while loop key: %d, %d \n", config_list_table[index].key_name, index_key);
      // printf("list_get: while loop list: %d, %d \n", config_list_table[index].list_name, listbox_position);

      if (((config_list_table[index].key_name) == index_key) && ((config_list_table[index].list_name)==listbox_position))
      {
         // printf("list_get: got key name=%d AND index_key=%d \n",config_list_table[index].key_name, index);
         CONFIG_DATA_LISTBOX_STRINGS listid = (config_list_table[index].list_name);
         // printf("list_get: the list id is: %d, stringid= %d\n",listid, config_list_table[index].txtid);
         return(listid);
      }  // end of if statement

      index++; // keep looping until key_name is found

   } // end of while loop

   _FATAL_ERROR(__FILE__, __LINE__, "index key was not found!");
   return(LAST_STRING_INDEX);

}



//
///////////////////////////////////////////////////////////////////
// end of list_get.cpp file

/* FORMAT HASH 6f69153a94e3a3f61b8feade15d9bb4d */

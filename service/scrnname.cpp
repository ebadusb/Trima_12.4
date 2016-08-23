/************************> FOCUSSED GUI DOMAIN <****************************
* Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved
* Author:      Terry Wahl
* Class name:  Screen_Pointer_Table
* File name:   scrnstat.cpp
* Contents:    Implementations of class methods
* Description: See descriptions in corresponding .hpp file.
*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/service/rcs/scrnname.cpp 1.5 2003/06/10 21:15:40Z rm70006 Exp dslausb $
$Log: scrnname.cpp $
Revision 1.5  2003/06/10 21:15:40Z  rm70006
First cut at porting service mode.
Revision 1.4  2002/06/05 15:12:35Z  sb07663
Modifications for compatibility with new Vx messaging system
Revision 1.3  2002/05/07 14:05:26  sb07663
First cut at VxWorks compatibility
Revision 1.2  2000/06/21 16:22:31  sb07663
revised include directives to remove relative paths
Revision 1.1  2000/03/15 17:57:28  SB07663
Initial revision
Revision 1.1  2000/03/15 17:43:13  SB07663
Initial revision
Revision 1.1  1997/05/14 21:39:14  Barry_Fulkerson
Initial revision
Revision 1.1  1997/05/14 21:36:45  Barry_Fulkerson
Initial revision
Revision 1.1  1997/05/13 18:59:36  Barry_Fulkerson
Initial revision
Revision 1.10  1997/05/12 17:51:06  tw09453
Add "PREDICT" and "TROUBLE" as dynamic screens for
alert processing.
Revision 1.9  1997/03/11 16:13:27  Barry_Fulkerson
- removed all references to the old cfg_repo & cfg_serv screens. They are
  do not exist.
Revision 1.8  1997/03/06 22:10:52  tw09453
Allocate all the static screens in the heap instead of on the stack --
for debugging purposes and to unburden the stack space.
Revision 1.7  1997/02/26 02:33:17  Barry_Fulkerson
- changed reference from the cfg_adju file to the cfg_priority file.  Renamed
 cfg_adju.cpp/hpp to cfg_priority to better represent what it is actually doing
 also in preperation for changing the PROCEDURE PRIORITY screen to include
 more than 5 procedures.
Revision 1.6  1997/01/10 22:56:03  tw09453
remove the bloodsmp screen from the world
Revision 1.5  1996/10/07 11:24:01  MC03015
Addition of PREDICT Screen
Revision 1.4  1996/09/28 22:27:52  Barry_Fulkerson
GUI Configuration additions for 2.3
Revision 1.3  1996/07/08 16:44:10  tw09453
Update file header
*/



// BASE CLASS INCLUDES
#include "scrnname.hpp"



// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "service_con.hpp"
#include "touchscreenCal.hpp"




////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_Pointer_Table::Screen_Pointer_Table()
{
   // Dynamic heap allocation (one time per GUI execution).

   screen_ptr_table[0].aph_screen_ptr = new Screen_SERVICE;
   screen_ptr_table[1].aph_screen_ptr = new TouchscreenCal;

   // The following are potential screens (don't forget to increase
   //  the array allocation and MAX_SCREENS manifest)

}   // END of Screen_Pointer_Table CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_Pointer_Table::~Screen_Pointer_Table()
{
   // Loop through all the fixed allocation screens and make sure their resources
   // are released.

   for (int i = 0; i < MAX_SCREENS; i++)
   {
      // Deallocate the Photon resources and free the screen memory from the heap.
      if (screen_ptr_table[i].aph_screen_ptr != NULL)
      {
         screen_ptr_table[i].aph_screen_ptr->deallocate_resources();
         delete screen_ptr_table[i].aph_screen_ptr;
      }
   }
}   // END of Screen_Pointer_Table DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// GET_STATIC_SCREEN_POINTER
Base_Window* Screen_Pointer_Table::get_static_screen_ptr (GUI_SCREEN_ID screen_id)
{
   // walk through the screen pointer table asking each screen for its screen ID
   // and comparing it to the desired screen ID. If a match occurs, return the
   // screen pointer.
   for (SCREEN_TABLE_ITER screen_iter = 0; screen_iter < MAX_SCREENS; screen_iter++)
   {
      if (((screen_ptr_table[screen_iter].aph_screen_ptr)->get_screen_id()) == screen_id)
      {
         return(screen_ptr_table[screen_iter].aph_screen_ptr);
      }
   }

   // If the screen can't be found, software fault
   _FATAL_ERROR (__FILE__, __LINE__, "Attempted to convert invalid screen ID to screen pointer");

   // This never-to-be-reached return is to appease the officious compiler about no return value
   return NULL;
}   // END of Screen_Pointer_Table GET_STATIC_SCREEN_POINTER



////////////////////////////////////////////////////////////////////////////////
// GET_FIRST_STATIC_SCREEN_TABLE_POINTER
Base_Window* Screen_Pointer_Table::get_first_static_screen_table_pointer (SCREEN_TABLE_ITER& screen_iter)
{
   // point to the first entry in the table
   screen_iter = 0;

   // return that entry to the caller
   return screen_ptr_table[screen_iter].aph_screen_ptr;
}   // END of Screen_Pointer_Table GET_FIRST_STATIC_SCREEN_TABLE_POINTER



////////////////////////////////////////////////////////////////////////////////
// GET_NEXT_STATIC_SCREEN_TABLE_POINTER
Base_Window* Screen_Pointer_Table::get_next_static_screen_table_pointer (SCREEN_TABLE_ITER& screen_iter)
{
   // make sure we're not past the end of the table or using a negative iterator index
   if (((screen_iter + 1) < MAX_SCREENS) && (screen_iter >= 0))
   {
      // bump the interator to the next valid entry
      screen_iter++;
      // still in valid-table-land, return the corresponding table pointer
      return screen_ptr_table[screen_iter].aph_screen_ptr;
   }
   else
   {
      // past the end of the table or the iterator has been set to a negative
      // number.
      return NULL;
   }
}   // END of Screen_Pointer_Table GET_NEXT_STATIC_SCREEN_TABLE_POINTER



////////////////////////////////////////////////////////////////////////////////
// GET_DYNAMIC_SCREEN_POINTER
Base_Window* Screen_Pointer_Table::get_dynamic_screen_pointer (GUI_SCREEN_ID screen_id)
{
   switch (screen_id)
   {
      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Attempted to C++ allocate an invalid dynamic screen");
         break;
   }  // End of switch (screen_id)...

   // This never-to-be-reached return is to appease the officious compiler about no return value
   return NULL;
}   // END of Screen_Pointer_Table GET_DYNAMIC_SCREEN_POINTER

/* FORMAT HASH f97504c184de36d828cfec0d010914c5 */

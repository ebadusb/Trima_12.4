/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Default_Screen_Table

*     File name:   defscrn.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: K:/BCT_Development/vxWorks/Trima/gui/rcs/defscrn.cpp 1.5 2002/08/16 16:31:29Z sb07663 Exp jl11312 $
$Log: defscrn.cpp $
Revision 1.5  2002/08/16 16:31:29Z  sb07663
added <vxworks.h> include
Revision 1.4  2002/07/05 21:14:30  sb07663
independent screen / state tables for service and normal modes
Revision 1.3  2002/05/07 13:55:49  sb07663
First cut at VxWorks compatibility
Revision 1.2  1996/07/01 20:20:49  tw09453
Update the file header

*/

#include <vxworks.h>


// BASE CLASS INCLUDES


// CLASS INCLUDES
#include "defscrn.hpp"

// EXTERNAL REFERENCES
#include "guiglobs.hpp"

extern DEFAULT_SCREEN_TABLE default_screen_tab[];

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Default_Screen_Table::Default_Screen_Table()
{}    // END of Default_Screen_Table CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Default_Screen_Table::~Default_Screen_Table()
{}    // END of Default_Screen_Table DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// GET_SCREEN_POINTER

GUI_SCREEN_ID Default_Screen_Table::get_system_state_default_screen (State_names sys_state)
{
   int table_index = 0;


   while (default_screen_tab[table_index].system_state  != TABLE_END)
   {
      if (default_screen_tab[table_index].system_state  == sys_state)
      {
         return (default_screen_tab[table_index].screen_name);
      }

      // Bump the index
      table_index++ ;
   }

   // If the screen can't be found - software fault
   _FATAL_ERROR (__FILE__, __LINE__, "Attempted to convert invalid system state to default screen");

   // This never-to-be-reached return is to appease the officious compiler about no return value
   return (GUI_SCREEN_NULL_SCREEN);

}     // END of Default_Screen_Table GET_SCREEN_POINTER


GUI_SCREEN_ID Default_Screen_Table::get_system_state_default_screen (State_names current_system_state, GUI_SCREEN_ID current_screen)
{
   int table_index = 0;

   while (default_screen_tab[table_index].system_state  != TABLE_END)
   {
      if (default_screen_tab[table_index].system_state  == current_system_state &&
          default_screen_tab[table_index].screen_name == current_screen )
      {
         return current_screen;
      }

      // Bump the index
      table_index++ ;
   }

   return get_system_state_default_screen(current_system_state);
}

/* FORMAT HASH b891f8b52ba4facd5cffdcce05e98262 */

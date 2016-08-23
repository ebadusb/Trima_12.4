/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  State_Manager

*     File name:   statmgr.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< TRIMA GUI DOMAIN >****************************

$Header: I:/BCT_Development/vxWorks/Trima/gui/rcs/statmgr.cpp 1.4 2002/08/16 16:37:37Z sb07663 Exp rm70006 $
$Log: statmgr.cpp $
Revision 1.4  2002/08/16 16:37:37Z  sb07663
added <vxworks.h> include
Revision 1.3  2002/08/15 22:17:24  sb07663
compiling version for Vx
Revision 1.1  1998/08/08 00:04:55  tw09453
Initial revision
*/


#include <vxworks.h>


// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "statmgr.hpp"



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
State_Manager::State_Manager()
{}   // END of State_Manager CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
State_Manager::~State_Manager()
{}   // END of State_Manager DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// ADD_STATE_ENTRY
void State_Manager::add_state_entry (GUI_State* gui_state)
{

   state_collection.add_table_entry (gui_state) ;

}   // END of State_Manager ADD_STATE_ENTRY



////////////////////////////////////////////////////////////////////////////////
// ADD_ACTION_ENTRY
void State_Manager::add_action_entry (GUI_Action* gui_action)
{

   action_collection.add_table_entry (gui_action) ;

}   // END of State_Manager ADD_ACTION_ENTRY




////////////////////////////////////////////////////////////////////////////////
// CHECK_FOR_STATE_CHANGE
int State_Manager::check_for_state_change (void* current_state)
{
   int action_ID     = -1;

   int action_return = 0;


   // Search for this state... if it exists, it will return an action ID, else a -1.
   action_ID = state_collection.search_for_state_match (current_state) ;

   // If a state was found, call the action ID's execution sequence
   if (action_ID != -1)
   {
      action_return = action_collection.execute_an_action_id (action_ID);

      if (action_return == 0) // if zero, no action entry was found or executed
      {
         _FATAL_ERROR (__FILE__, __LINE__, "No action entry for a valid state entry");
         return(0); // this is to assuage the compiler...
      }
      else
      {
         return(1);
      }
   }
   else
   {
      return(0);
   }

}   // END of State_Manager CHECK_FOR_STATE_CHANGE

/* FORMAT HASH 6aab56dec8a3a87ebc1470f63f02c20b */

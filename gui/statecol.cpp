/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  State_Collection

*     File name:   statecol.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< TRIMA GUI DOMAIN >****************************

$Header: M:/BCT_Development/vxWorks/Trima/gui/rcs/statecol.cpp 1.1 1998/08/08 00:04:53 tw09453 Exp $
$Log: statecol.cpp $
Revision 1.1  1998/08/08 00:04:53  tw09453
Initial revision
*/

#include <vxworks.h>



// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "statecol.hpp"
#include "guistate.hpp"



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

State_Collection::State_Collection()
{}    // END of State_Collection CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

State_Collection::~State_Collection()
{}    // END of State_Collection DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// SEARCH_FOR_STATE_MATCH

int State_Collection::search_for_state_match (void* current_state)
{
   GUI_State* state_table_entry = NULL;

   // Set the collection cursor to the top of the colletion
   set_cursor_to_top () ;

   // Get the first entry
   state_table_entry = get_next_entry () ;


   // Loop through all the states and compare the current state with the table
   //  entries looking for equality.

   while (state_table_entry != NULL)
   {
      // This expects the table element class to implement a
      //  compare in order to allow for "don't care" type
      //  compares of specific class data members.
      if (state_table_entry -> compare_states(current_state) == 0)   // zero means "equal"
      {
         // return the action ID to the caller so that the action sequence can
         //  be executed.
         // *DEBUG*/ printf("hit on state # %d \n", state_table_entry -> get_state_id ());
         return ( state_table_entry -> get_action_id() );
      }

      // Get the next entry
      state_table_entry = get_next_entry () ;


   }   // End of while(state_table...

   return (-1) ;   // An action ID of -1 says that no state match was found,
                   //  ergo no action should be taken.

}     // END of State_Collection SEARCH_FOR_STATE_MATCH

/* FORMAT HASH 5d6e96f34efa0a571aaa3f7c39cee514 */

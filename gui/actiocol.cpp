/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Action_Collection

*     File name:   actiocol.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< TRIMA GUI DOMAIN >****************************

$Header: M:/BCT_Development/vxWorks/Trima/gui/rcs/actiocol.cpp 1.1 1998/08/08 00:04:39 tw09453 Exp $
$Log: actiocol.cpp $
Revision 1.1  1998/08/08 00:04:39  tw09453
Initial revision
*/


#include <vxworks.h>


// EXTERNAL REFERENCES

#include "gui.h"
#include "actiocol.hpp"



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Action_Collection::Action_Collection()
{}    // END of Action_Collection CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Action_Collection::~Action_Collection()
{}    // END of Action_Collection DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// EXECUTE_AN_ACTION_ID

int Action_Collection::execute_an_action_id (int action_id)
{
   static const int YES = 0 ;
   static const int NO  = -1 ;



   GUI_Action* action_table_entry = NULL;


   int return_code = 0 ;           // init it to unsuccessful in anticipation of a
                                   // "no hit" lookup.

   int micro_action_seq_id = 0 ;   // init the sequence id of the micro-action to initial_micro_sequence-1.
                                   //  this will be incremented before each micro-
                                   //  action is searched for and executed.

   int look_for_next_micro_action = YES ;    // This flag governs looping again for the
                                             //  next micro-action.


   // Loop through the entire collection for each micro-action in case they
   //  are not contiguous or sequential.
   while (look_for_next_micro_action == YES)
   {
      // Reset the look-for-the-next-micro-action flag to "don't look for
      //  any more micro-actions".
      look_for_next_micro_action = NO ;

      // Bump the micro-action seq. id
      micro_action_seq_id++ ;

      // Set the collection cursor to the top of the collection
      set_cursor_to_top () ;

      // Get the first entry of the collection
      action_table_entry = get_next_entry () ;


      // Loop through all the micro-actions and compare the current
      //  action ID / micro-action sequence with the micro-action table entries
      //  looking for equality.

      while (action_table_entry != NULL)
      {
         // 0 means we found a micro-action with this action / micro-action ID pair
         if (action_table_entry -> compare_action_id (action_id, micro_action_seq_id) == 0)
         {
            action_table_entry -> execute_micro_action () ; // actually do the micro-action.
            return_code                = 1;                 // at least one micro-action was executed, ergo successful call.
            look_for_next_micro_action = YES ;              // this will cause us to look for the next possible micro-action.
            break;                                          // from looping through the action table for this particular micro-action... it's executed.
         }


         // Get the next micro-action entry from the collection
         action_table_entry = get_next_entry () ;


      }   // End of "while(action_table...


   }   // End of: "while (look_for_next_micro_action == YES)


   // return the return code: if we found at least 1 micro-action the return
   //  code should be equal to 1.

   return(return_code) ;


}     // END of Action_Collection EXECUTE_AN_ACTION_ID

/* FORMAT HASH b9d6aa69ba2816961744b56f17574fe8 */

/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   State_Manager

*     File name:    statmgr.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class coordinates the populating of the state and action tables;
         the lookup of current state in the state table and the execution
         of the actions associated with the transition to a new state.
*************************< TRIMA GUI DOMAIN >****************************

$Header: I:/BCT_Development/vxWorks/Trima/gui/rcs/statmgr.hpp 1.3 2002/08/15 22:17:24Z sb07663 Exp rm70006 $
$Log: statmgr.hpp $
Revision 1.3  2002/08/15 22:17:24Z  sb07663
compiling version for Vx
Revision 1.1  1998/08/08 00:04:57  tw09453
Initial revision

*/



// Single include define guardian
#ifndef STATMGR_HPP
#define STATMGR_HPP

#include "statecol.hpp"  // include the state collection header
#include "actiocol.hpp"  // include the action collection header
#include "guistate.hpp"  // include the gui state header


class State_Manager
{
private:
   State_Collection  state_collection;  // the state collection object
   Action_Collection action_collection; // the action collection object

protected:
   // CHECK_FOR_STATE_CHANGE
   // This method receives a current state object and passes it
   // to the state_collection object. If the state_collection object
   // finds a match it will return the action ID to execute and this
   // object will then tell the action_collection object to execute
   // this action. If a state match is found and an action is executed,
   // this routine will return a 1 to its caller.
   // If a state is not found, this routine will return a zero.
   // If a state is found, but the corresponding action is not found,
   // this routine will perform a FATAL_ERROR.
   int check_for_state_change (void* current_state);

public:
   // CONSTRUCTOR
   State_Manager();

   // DESTRUCTOR
   virtual ~State_Manager();

   // ADD_STATE_ENTRY
   // This method adds a GUI_State entry to the state_collection
   void add_state_entry (GUI_State* gui_state);

   // ADD_ACTION_ENTRY
   // This method adds a GUI_Action entry to the action_collection
   void add_action_entry (GUI_Action* gui_action);
};  // END of State_Manager Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 2e8848b721afc024eb7487efd0295c24 */

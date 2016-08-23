/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Action_Collection

*     File name:    actiocol.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This is derived off of the table collection template and contains
         all the possible actions resulting from a state change. It will
         receive an "action ID" and execute all the actions associated with
         that ID.
*************************< TRIMA GUI DOMAIN >****************************

$Header$
$Log$

*/



// Single include define guardian
#ifndef ACTIOCOL_HPP
#define ACTIOCOL_HPP

#include "tablecol.hpp" // include the base class declaration
#include "guiact.hpp"   // include the action class


class Action_Collection
   : public Table_Collection<GUI_Action>
{
private:


protected:



public:


   // CONSTRUCTOR
   Action_Collection();



   // DESTRUCTOR
   virtual ~Action_Collection();



   // EXECUTE_AN_ACTION_ID
   // This method takes an Action ID and executes all micro-actions
   //  associated with that Action ID (in sequential order). If it cannot
   //  find at least one micro-action entry associated with the
   //  Action ID, it returns a 0 (unsuccessful) otherwise it returns
   //  a 1 (successful)
   int execute_an_action_id (int action_id) ;



};  // END of Action_Collection Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 6a64a376a182399b47baf574ce190a9f */

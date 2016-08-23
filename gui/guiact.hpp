/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   GUI_Action

*     File name:    guiact.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This represents a single action to be executed on the Sysdonor
         screen. The identifying data members are a major "action ID" and a
         subordinate or minor "micro action ID". There may be 1 to many
         "micro action ID's" under each major "action ID". Within the class
         are contained members which are used as parameters to the executing
         function whose vector is also contained within this class.

         This is an abstract class that must be overridden by a specific
         action class directed to the screen of interest.
*************************< TRIMA GUI DOMAIN >****************************

$Header: M:/BCT_Development/Trima/gui/rcs/guiact.hpp 1.1 1998/08/08 00:04:42 tw09453 Exp $
$Log: guiact.hpp $
Revision 1.1  1998/08/08 00:04:42  tw09453
Initial revision

*/



// Single include define guardian
#ifndef GUIACT_HPP
#define GUIACT_HPP


class GUI_Action
{
private:


   // CONSTRUCTOR - invalid, except by friend classes, functions
   GUI_Action();

   // Main action category
   int action_id ;

   // Sub action category
   int micro_action_seq_id ;



protected:


   // SET_ACTION_ID
   // This method sets the action_id
   //
   virtual void set_action_id (int p_action_id);    // logically, a pure virtual method


   // SET_MICRO_ACTION_SEQ_ID
   // This method sets the micro action id
   //
   virtual void set_micro_action_seq_id (int p_micro_action_seq_id);    // logically, a pure virtual method


public:

   // CONSTRUCTOR - invalid, except by friend classes, functions
   GUI_Action(int p_action_id, int p_micro_action_seq_id);


   // DESTRUCTOR
   virtual ~GUI_Action();


   // EXECUTE_MICRO_ACTION
   // This method determines what micro-action should take place
   //  and calls the appropriate private member function to
   //  execute that function.
   virtual void execute_micro_action ();    // logically, a pure virtual method


   // COMPARE_ACTION_ID
   // This method compares the action ID and micro action ID to the
   // corresponding parameters and returns the result of that comparison.
   // 0 means equality comparison; -1 means an inequality comparison
   //
   virtual int compare_action_id (int action_id, int micro_action_seq_id);    // logically, a pure virtual method


   // GET_ACTION_ID
   // This method returns the action_id
   //
   virtual int get_action_id ();    // logically, a pure virtual method


   // GET_MICRO_ACTION_SEQ_ID
   // This method returns the micro action id
   //
   virtual int get_micro_action_seq_id ();    // logically, a pure virtual method



};  // END of GUI_Action Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 39601a8c3c44559f208f3d818bdf8685 */

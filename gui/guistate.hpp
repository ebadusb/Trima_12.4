/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   GUI_State

*     File name:    guistate.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

      This represents a single generic "state" possible in the GUI
        state machines. It is an abstract class and must be derived
        from.


*************************< TRIMA GUI DOMAIN >****************************

$Header$
$Log$

*/



// Single include define guardian
#ifndef GUISTATE_HPP
#define GUISTATE_HPP


class GUI_State
{
private:

   // CONSTRUCTOR
   GUI_State();  // disallow a non-parameterized constructor (except by NON-existent friends)

   // ID of the state cooresponding to the design doc.
   int state_id ;

   // the action ID that references an action sequence
   int action_id ;


protected:


   // SET_ACTION_ID
   // This method sets the Action ID for a given state object.
   virtual void set_action_id (int p_action_id) ;


   // SET_STATE_ID
   // This method sets the State ID for a given state object.
   virtual void set_state_id (int p_state_id) ;



public:

   // CONSTRUCTOR
   GUI_State(int state_id,  int action_id) ;


   // DESTRUCTOR
   virtual ~GUI_State();


   // COMPARE_STATES
   // This method takes two GUI_state instances and compares them for
   //  equality. If the two objects are equal, then this returns a zero (0).
   //  otherwise a -1 is returned meaning that the states are not equivalent.
   virtual int compare_states (void* current_state);    // logically, a pure virtual method


   // GET_ACTION_ID
   // This method returns the Action ID for a given state object.
   virtual int get_action_id () ;   // logically, a pure virtual method


   // GET_STATE_ID
   // This method returns the State ID for a given state object.
   virtual int get_state_id () ;   // logically, a pure virtual method




};  // END of GUI_State Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 2a565ab03a173c32c5f7e63ddfbcd36b */

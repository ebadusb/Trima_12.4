/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  GUI_State

*     File name:   guistate.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< TRIMA GUI DOMAIN >****************************

$Header: M:/BCT_Development/vxWorks/Trima/gui/rcs/guistate.cpp 1.2 2002/05/07 13:57:13 sb07663 Exp $
$Log: guistate.cpp $
Revision 1.2  2002/05/07 13:57:13  sb07663
First cut at VxWorks compatibility
Revision 1.1  1998/08/08 00:04:44  tw09453
Initial revision
*/



#include <vxworks.h>

// EXTERNAL REFERENCES

#include "gui.h"
#include "guistate.hpp"
#include "error.h"
#include <stdio.h>



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR(S)

GUI_State::GUI_State ()
{}    // END of GUI_State CONSTRUCTOR


GUI_State::GUI_State (int p_state_id,  int p_action_id)
   : state_id(p_state_id),
     action_id(p_action_id)
{}    // END of GUI_State CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

GUI_State::~GUI_State()
{}    // END of GUI_State DESTRUCTOR




////////////////////////////////////////////////////////////////////////////////
// COMPARE_STATES
int GUI_State::compare_states (void*)
{
   // This method can't be abstract because it's based on a class template - compiler gets snitty
   _FATAL_ERROR (__FILE__, __LINE__, "Compare_States method not overidden in specific screen state class");
   // *DEBUG*/printf("WARNING: did not override compare_states\n");
   return (-10000);   // This assuages the compiler's need for consistency

}     // END of GUI_State COMPARE_STATES




////////////////////////////////////////////////////////////////////////////////
// GET_ACTION_ID
int GUI_State::get_action_id ()
{

   return (action_id);

}     // END of GUI_State GET_ACTION_ID



////////////////////////////////////////////////////////////////////////////////
// GET_STATE_ID
int GUI_State::get_state_id ()
{

   return (state_id);

}     // END of GUI_State GET_ACTION_ID




////////////////////////////////////////////////////////////////////////////////
// SET_ACTION_ID
void GUI_State::set_action_id (int p_action_id)
{

   action_id = p_action_id ;

}     // END of GUI_State SET_ACTION_ID



////////////////////////////////////////////////////////////////////////////////
// SET_STATE_ID
void GUI_State::set_state_id (int p_state_id)
{

   state_id = p_state_id ;

}     // END of GUI_State SET_STATE_ID

/* FORMAT HASH a021f5bc7b6e7140434df254cb4f468f */

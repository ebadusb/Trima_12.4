/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  GUI_Action

*     File name:   guiact.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< TRIMA GUI DOMAIN >****************************

$Header: M:/BCT_Development/vxWorks/Trima/gui/rcs/guiact.cpp 1.2 2002/08/15 21:13:59 sb07663 Exp $
$Log: guiact.cpp $
Revision 1.2  2002/08/15 21:13:59  sb07663
Vx updates
Revision 1.1  1998/08/08 00:04:41  tw09453
Initial revision
*/

#include <vxworks.h>



// EXTERNAL REFERENCES

#include "gui.h"
#include "guiact.hpp"
#include "error.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR(S)

GUI_Action::GUI_Action(int p_action_id, int p_micro_action_seq_id)
   : action_id(p_action_id),
     micro_action_seq_id(p_micro_action_seq_id)
{}    // END of GUI_Action CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR(S)

GUI_Action::GUI_Action()
{}     // END of Sysdonor_State CONSTRUCTOR (invalid, private constructor)




////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

GUI_Action::~GUI_Action()
{}     // END of Sysdonor_State DESTRUCTOR




////////////////////////////////////////////////////////////////////////////////
// EXECUTE_MICRO_ACTION
void GUI_Action::execute_micro_action ()
{
   // This method can't be abstract because it's based on a class template - compiler gets snitty
   _FATAL_ERROR (__FILE__, __LINE__, "Execute_micro_action method not overidden in specific screen state class");

}      // END of Sysdonor_State EXECUTE_MICRO_ACTION




////////////////////////////////////////////////////////////////////////////////
// COMPARE_ACTION_ID
int GUI_Action::compare_action_id (int, int)
{
   // This method can't be abstract because it's based on a class template - compiler gets snitty
   _FATAL_ERROR (__FILE__, __LINE__, "Compare_action_id method not overidden in specific screen state class");
   return (0);   // This assuages the compiler's need for consistency

}      // END of GUI_Action COMPARE_ACTION_ID



////////////////////////////////////////////////////////////////////////////////
// GET_ACTION_ID
int GUI_Action::get_action_id ()
{
   return (action_id) ;

}     // END of GUI_Action GET_ACTION_ID



////////////////////////////////////////////////////////////////////////////////
// GET_MICRO_ACTION_SEQ_ID
int GUI_Action::get_micro_action_seq_id ()
{
   return (micro_action_seq_id) ;

}     // END of GUI_Action GET_MICRO_ACTION_SEQ_ID



////////////////////////////////////////////////////////////////////////////////
// SET_ACTION_ID
void GUI_Action::set_action_id (int p_action_id)
{
   action_id = p_action_id ;

}     // END of GUI_Action SET_ACTION_ID



////////////////////////////////////////////////////////////////////////////////
// SET_MICRO_ACTION_SEQ_ID
void GUI_Action::set_micro_action_seq_id (int p_micro_action_seq_id)
{
   micro_action_seq_id = p_micro_action_seq_id ;

}     // END of GUI_Action SET_MICRO_ACTION_SEQ_ID

/* FORMAT HASH 94264e80cffcf3c5065b33fe036a0043 */

/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Sysdonor_State

*     File name:   sdstate.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< TRIMA GUI DOMAIN >****************************

$Header: I:/BCT_Development/vxWorks/Trima/gui/rcs/sdstate.cpp 1.2 2002/08/16 16:37:35Z sb07663 Exp rm70006 $
$Log: sdstate.cpp $
Revision 1.2  2002/08/16 16:37:35Z  sb07663
added <vxworks.h> include
Revision 1.1  1998/08/08 00:04:50  tw09453
Initial revision
*/


#include <vxworks.h>


// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "sdstate.hpp"



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR(S)

Sysdonor_State::Sysdonor_State(int p_state_id,
                               int p_action_id,
                               SYSDONOR_STATE p_screen_state,
                               SD_T_F_D p_enable_begin_run_state,
                               SD_T_F_D p_display_load_sys_state,
                               SD_T_F_D p_ap2_in_control_state,
                               SD_T_F_D p_ap2_data_available_state
                               )
   : GUI_State(p_state_id, p_action_id),
     screen_state(p_screen_state),
     enable_begin_run_state(p_enable_begin_run_state),
     display_load_sys_state(p_display_load_sys_state),
     ap2_in_control_state(p_ap2_in_control_state),
     ap2_data_available_state(p_ap2_data_available_state)
{}   // END of Sysdonor_State CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR(S)
Sysdonor_State::Sysdonor_State()
   : GUI_State(0, 0)
{}    // END of Sysdonor_State CONSTRUCTOR (invalid, private constructor)



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Sysdonor_State::~Sysdonor_State()
{}   // END of Sysdonor_State DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// COMPARE_STATES
int Sysdonor_State::compare_states (void* p_current_state)
{
   Sysdonor_State* current_state = (Sysdonor_State*)p_current_state;

   /* //DEBUG//

   printf("(DEBUG audit from sdstate.cpp) Compare table entry(#%d) to (~) screen state\nenable=%d~%d\nloadsys=%d~%d\nscreenstate=%d~%d\nap2incontrol=%d~%d\nap2dataavail=%d~%d\n",
          get_state_id (),
          enable_begin_run_state, current_state->enable_begin_run_state,
          display_load_sys_state, current_state->display_load_sys_state,
          screen_state, current_state->screen_state,
          ap2_in_control_state, current_state->ap2_in_control_state,
          ap2_data_available_state, current_state->ap2_data_available_state);

   //DEBUG//
   */
   if ( (screen_state == current_state->screen_state) && // there must always be a valid screen state (no dont cares)
        ( (enable_begin_run_state == current_state->enable_begin_run_state) ||
          (enable_begin_run_state == SD_DONT_CARE)                          ||
          (current_state->enable_begin_run_state == SD_DONT_CARE)
        ) &&
        ( (display_load_sys_state == current_state->display_load_sys_state) ||
          (display_load_sys_state == SD_DONT_CARE)                          ||
          (current_state->display_load_sys_state == SD_DONT_CARE)
        ) &&
        ( (ap2_in_control_state == current_state->ap2_in_control_state) ||
          (ap2_in_control_state ==  SD_DONT_CARE)                       ||
          (current_state->ap2_in_control_state == SD_DONT_CARE)
        ) &&
        ( (ap2_data_available_state == current_state->ap2_data_available_state) ||
          (ap2_data_available_state == SD_DONT_CARE) ||
          (current_state->ap2_data_available_state == SD_DONT_CARE)
        )
        ) // end of if...
   {
      return 0; // zero means that the two states are logically equal
   }
   else
   {
      return -1;  // not logically equal
   }
}   // END of Sysdonor_State COMPARE_STATES



////////////////////////////////////////////////////////////////////////////////
///////////////////////////// the SETS Follow //////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// SET_ACTION_ID
void Sysdonor_State::set_action_id (int p_action_id)
{
   GUI_State::set_action_id (p_action_id);
}   // END of Sysdonor_State SET_ACTION_ID



////////////////////////////////////////////////////////////////////////////////
// SET_STATE_ID
void Sysdonor_State::set_state_id (int p_state_id)
{
   GUI_State::set_state_id (p_state_id);
}   // END of Sysdonor_State SET_STATE_ID



////////////////////////////////////////////////////////////////////////////////
// SET_SCREEN_STATE
void Sysdonor_State::set_screen_state (SYSDONOR_STATE p_screen_state)
{
   screen_state = p_screen_state;
}   // END of Sysdonor_State SET_SCREEN_STATE



////////////////////////////////////////////////////////////////////////////////
// SET_ENABLE_BEGIN_RUN_STATE
void Sysdonor_State::set_enable_begin_run_state (SD_T_F_D p_ebr_state)
{
   enable_begin_run_state = p_ebr_state;
}   // END of Sysdonor_State SET_ENABLE_BEGIN_RUN_STATE



////////////////////////////////////////////////////////////////////////////////
// SET_DISPLAY_LOAD_SYS_STATE
void Sysdonor_State::set_display_load_sys_state (SD_T_F_D p_dls_state)
{
   display_load_sys_state = p_dls_state;
}   // END of Sysdonor_State SET_DISPLAY_LOAD_SYS_STATE



////////////////////////////////////////////////////////////////////////////////
// SET_AP2_IN_CONTROL_STATE
void Sysdonor_State::set_ap2_in_control_state (SD_T_F_D p_aic_state)
{
   ap2_in_control_state = p_aic_state;
}   // END of Sysdonor_State SET_AP2_IN_CONTROL_STATE



////////////////////////////////////////////////////////////////////////////////
// SET_AP2_DATA_AVAILABLE_STATE
void Sysdonor_State::set_ap2_data_available_state (SD_T_F_D p_ada_state)
{
   ap2_data_available_state = p_ada_state;
}   // END of Sysdonor_State SET_AP2_DATA_AVAILABLE_STATE




////////////////////////////////////////////////////////////////////////////////
///////////////////////////// GETS Follow //////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
// GET_ACTION_ID
int Sysdonor_State::get_action_id ()
{
   return(GUI_State::get_action_id());
}   // END of Sysdonor_State GET_ACTION_ID



////////////////////////////////////////////////////////////////////////////////
// GET_STATE_ID
int Sysdonor_State::get_state_id ()
{
   return GUI_State::get_state_id();
}   // END of Sysdonor_State GET_STATE_ID



////////////////////////////////////////////////////////////////////////////////
// GET_SCREEN_STATE
SYSDONOR_STATE Sysdonor_State::get_screen_state ()
{
   return screen_state;
}   // END of Sysdonor_State GET_SCREEN_STATE



////////////////////////////////////////////////////////////////////////////////
// GET_ENABLE_BEGIN_RUN_STATE
SD_T_F_D Sysdonor_State::get_enable_begin_run_state ()
{
   return enable_begin_run_state;
}   // END of Sysdonor_State GET_ENABLE_BEGIN_RUN_STATE



////////////////////////////////////////////////////////////////////////////////
// GET_DISPLAY_LOAD_SYS_STATE
SD_T_F_D Sysdonor_State::get_display_load_sys_state ()
{
   return display_load_sys_state;
}   // END of Sysdonor_State GET_DISPLAY_LOAD_SYS_STATE



////////////////////////////////////////////////////////////////////////////////
// GET_AP2_IN_CONTROL_STATE
SD_T_F_D Sysdonor_State::get_ap2_in_control_state ()
{
   return ap2_in_control_state;
}   // END of Sysdonor_State GET_AP2_IN_CONTROL_STATE



////////////////////////////////////////////////////////////////////////////////
// GET_AP2_DATA_AVAILABLE_STATE
SD_T_F_D Sysdonor_State::get_ap2_data_available_state ()
{
   return ap2_data_available_state;
}   // END of Sysdonor_State GET_AP2_DATA_AVAILABLE_STATE

/* FORMAT HASH f0c22dc4f7aafa4559f41c288e958608 */

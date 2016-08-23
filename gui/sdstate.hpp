/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Sysdonor_State

*     File name:    sdstate.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This represents a single "state" possible in the Sysdonor screen.
         It can contain don't care values in some of the state's attributes
         which will always compare as TRUE in the evaluation of the state
         entries evaluation to the current state of the screen.
*************************< TRIMA GUI DOMAIN >****************************

$Header: I:/BCT_Development/vxWorks/Trima/gui/rcs/sdstate.hpp 1.1 1998/08/08 00:04:52Z tw09453 Exp rm70006 $
$Log: sdstate.hpp $
Revision 1.1  1998/08/08 00:04:52Z  tw09453
Initial revision

*/



// Single include define guardian
#ifndef SDSTATE_HPP
#define SDSTATE_HPP

#include "guistate.hpp"  // include the state class
#include "sysdninc.h"

class Sysdonor_State
   : public GUI_State
{
private:
   SYSDONOR_STATE screen_state ;             // the old screen state based on screen-from and trans-table
   SD_T_F_D       enable_begin_run_state ;   // whether AC has completed priming
   SD_T_F_D       display_load_sys_state ;   // whether proc has finished T1 testing
   SD_T_F_D       ap2_in_control_state ;     // whether AP2 is in control of Trima
   SD_T_F_D       ap2_data_available_state ; // whether data is available from AP2

   // CONSTRUCTOR
   Sysdonor_State(); // disallow a non-parameterized constructor (except by NON-existent friends)

public:
   // CONSTRUCTOR
   Sysdonor_State(int p_state_id,
                  int p_action_id,
                  SYSDONOR_STATE p_screen_state,
                  SD_T_F_D p_enable_begin_run_state,
                  SD_T_F_D p_display_load_sys_state,
                  SD_T_F_D p_ap2_in_control_state,
                  SD_T_F_D p_ap2_data_available_state);

   // DESTRUCTOR
   virtual ~Sysdonor_State();

   // COMPARE_STATES
   // This method takes two Sysdonor_State instances and compares them for
   //  equality. If any member in either instance contains a "don't care" value,
   //  that member is considered to be equal and contributes to the "equality"
   //  of the two objects. If the two objects are equal, then this returns a
   //  zero (0), otherwise a -1 is returned meaning that the
   //  states are not equivalent.
   virtual int compare_states (void* current_state);


   // SET_ACTION_ID
   // This method sets the action ID value in the instance
   void set_action_id (int action_id);


   // SET_STATE_ID
   // This method sets the state ID value in the instance
   void set_state_id (int state_id);


   // SET_SCREEN_STATE
   // This method sets the screen state in the instance
   void set_screen_state (SYSDONOR_STATE screen_state);


   // SET_ENABLE_BEGIN_RUN_STATE
   // This method sets the enable begin run state in the instance
   void set_enable_begin_run_state (SD_T_F_D ebr_state);


   // SET_DISPLAY_LOAD_SYS_STATE
   // This method sets the display load sys state in the instance
   void set_display_load_sys_state (SD_T_F_D dls_state);


   // SET_AP2_IN_CONTROL_STATE
   // This method sets the ap2 in control state in the instance
   void set_ap2_in_control_state (SD_T_F_D aic_state);


   // SET_AP2_DATA_AVAILABLE_STATE
   // This method sets the ap2 data available state state in the instance
   void set_ap2_data_available_state (SD_T_F_D ada_state);


   // GET_STATE_ID
   // This method gets the state ID value in the instance
   int get_state_id ();


   // GET_ACTION_ID
   // This method gets the action ID value in the instance
   int get_action_id ();


   // GET_SCREEN_STATE
   // This method gets the screen state in the instance
   SYSDONOR_STATE get_screen_state ();


   // GET_ENABLE_BEGIN_RUN_STATE
   // This method gets the enable begin run state in the instance
   SD_T_F_D get_enable_begin_run_state ();


   // GET_DISPLAY_LOAD_SYS_STATE
   // This method gets the display load sys state in the instance
   SD_T_F_D get_display_load_sys_state ();


   // GET_AP2_IN_CONTROL_STATE
   // This method gets the ap2 in control state in the instance
   SD_T_F_D get_ap2_in_control_state ();


   // GET_AP2_DATA_AVAILABLE_STATE
   // This method gets the ap2 data available state state in the instance
   SD_T_F_D get_ap2_data_available_state ();


};  // END of Sysdonor_State Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 2583714e3feec0e5d442627cce6bc3c3 */

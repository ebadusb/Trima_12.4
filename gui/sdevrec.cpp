/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Sysdonor_Event_Recorder

*     File name:   sdevrec.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< TRIMA GUI DOMAIN >****************************

$Header: I:/BCT_Development/vxWorks/Trima/gui/rcs/sdevrec.cpp 1.3 2002/08/16 16:36:55Z sb07663 Exp rm70006 $
$Log: sdevrec.cpp $
Revision 1.3  2002/08/16 16:36:55Z  sb07663
compiling vx version
Revision 1.2  2002/08/15 22:17:22  sb07663
compiling version for Vx
Revision 1.1  1998/08/08 00:04:48  tw09453
Initial revision
*/



#include <vxworks.h>

// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "sdevrec.hpp"



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Sysdonor_Event_Recorder::Sysdonor_Event_Recorder()
   : current_state (-1,
                    -1,
                    SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE,
                    SD_FALSE,
                    SD_FALSE,
                    SD_FALSE,
                    SD_FALSE)
{}   // END of Sysdonor_Event_Recorder CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Sysdonor_Event_Recorder::~Sysdonor_Event_Recorder()
{}   // END of Sysdonor_Event_Recorder DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// UNCONDITIONAL_STATE_CHECK
void Sysdonor_Event_Recorder::unconditional_state_check ()
{
   check_for_state_change ((void*)&current_state);

}   // END of UNCONDITIONAL_STATE_CHECK DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// POST_EVENT - two different signatures follow...
void Sysdonor_Event_Recorder::post_event (SD_EVENT_TYPE ev_type, SD_T_F_D true_false_type)
{
   // Process the posting of the true / false type state components
   switch (ev_type)
   {
      case SD_DISPLAY_LOAD_SYSTEM :
         if (current_state.get_display_load_sys_state() != true_false_type)
         {
            current_state.set_display_load_sys_state(true_false_type);
            check_for_state_change ((void*)&current_state);
         }
         break;

      case SD_ENABLE_BEGIN_RUN :
         if (current_state.get_enable_begin_run_state() != true_false_type)
         {
            current_state.set_enable_begin_run_state(true_false_type);
            check_for_state_change ((void*)&current_state);
         }
         break;

      case SD_AP2_IN_CONTROL :
         if (current_state.get_ap2_in_control_state() != true_false_type)
         {
            current_state.set_ap2_in_control_state(true_false_type);
            check_for_state_change ((void*)&current_state);
         }
         break;

      case SD_AP2_DONOR_DATA_AVAILABILITY :
         if (current_state.get_ap2_data_available_state() != true_false_type)
         {
            current_state.set_ap2_data_available_state(true_false_type);
            check_for_state_change ((void*)&current_state);
         }
         break;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Unknown posting element for SYSDONOR state management");
   }  // End of: "switch (ev_type)


}   // END of Sysdonor_Event_Recorder POST_EVENT - signature 1



void Sysdonor_Event_Recorder::post_event (SD_EVENT_TYPE, SYSDONOR_STATE display_state)
{
   if (current_state.get_screen_state() != display_state)
   {
      current_state.set_screen_state(display_state);
      check_for_state_change ((void*)&current_state);
   }
}   // END of Sysdonor_Event_Recorder POST_EVENT - signature 2



////////////////////////////////////////////////////////////////////////////////
// POST_EVENT_NOCHECK - two different signatures follow...
void Sysdonor_Event_Recorder::post_event_nocheck (SD_EVENT_TYPE ev_type, SD_T_F_D true_false_type)
{
   // Process the posting of the true / false type state components
   //  without checking for state changes - this permits mass state component changes
   //  without updating the screen for each one

   switch (ev_type)
   {
      case SD_DISPLAY_LOAD_SYSTEM :
         current_state.set_display_load_sys_state(true_false_type);
         break;

      case SD_ENABLE_BEGIN_RUN :
         current_state.set_enable_begin_run_state(true_false_type);
         break;

      case SD_AP2_IN_CONTROL :
         current_state.set_ap2_in_control_state(true_false_type);
         break;

      case SD_AP2_DONOR_DATA_AVAILABILITY :
         current_state.set_ap2_data_available_state(true_false_type);
         break;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Unknown posting element for SYSDONOR state management (NOCHECK)");
   }  // End of: "switch (ev_type)

}   // END of Sysdonor_Event_Recorder POST_EVENT_NOCHECK - signature 1



void Sysdonor_Event_Recorder::post_event_nocheck (SD_EVENT_TYPE, SYSDONOR_STATE display_state)
{
   current_state.set_screen_state(display_state);
}   // END of Sysdonor_Event_Recorder POST_EVENT_NOCHECK - signature 2

/* FORMAT HASH 67398015cabafae9867ec8e0d99da908 */

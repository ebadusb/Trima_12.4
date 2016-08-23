/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Current_Screen_Status

*     File name:   scrnstat.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$

*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "scrnstat.hpp"


// EXTERNAL REFERENCES
#define NO_EXTERN_SCREEN_STATUS // don't include extern for screen status
#include "guiglobs.hpp"



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Current_Screen_Status::Current_Screen_Status()
   : current_displayed_screen(GUI_SCREEN_NULL_SCREEN),
     current_displayed_screen_ptr(NULL),
     previous_displayed_screen(GUI_SCREEN_NULL_SCREEN),
     current_substate((State_names) - 1)
{
   // Initialize the current screen status to NULL
   current_screen = GUI_SCREEN_NULL_SCREEN;

   // Initialize the previous screen status to NULL
   previous_screen = GUI_SCREEN_NULL_SCREEN;

   // Initialize the current system state to -1 since
   // there is no NULL state defined.
   current_system_state = (State_names) - 1 ;

}    // END of Current_Screen_Status CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Current_Screen_Status::~Current_Screen_Status()
{}   // END of Current_Screen_Status DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// GET_CURRENT_SCREEN

GUI_SCREEN_ID Current_Screen_Status::get_current_screen ()
{
   return (current_screen);

}    // END of Current_Screen_Status GET_CURRENT_SCREEN




////////////////////////////////////////////////////////////////////////////////
// GET_CURRENT_DISPLAYED_SCREEN

GUI_SCREEN_ID Current_Screen_Status::get_current_displayed_screen ()
{
   return (current_displayed_screen);

}    // END of Current_Screen_Status GET_CURRENT_DISPLAYED_SCREEN

////////////////////////////////////////////////////////////////////////////////
// GET_CURRENT_DISPLAYED_SCREEN_PTR

Base_Window* Current_Screen_Status::get_current_displayed_screen_ptr ()
{
   return (current_displayed_screen_ptr);

}    // END of Current_Screen_Status GET_CURRENT_DISPLAYED_SCREEN


////////////////////////////////////////////////////////////////////////////////
// GET_CURRENT_SYSTEM_STATE

State_names Current_Screen_Status::get_current_system_state ()
{
   return (current_system_state);

}    // END of Current_Screen_Status GET_CURRENT_SYSTEM_STATE

State_names Current_Screen_Status::get_current_substate ()
{
   return (current_substate);

}    // END of Current_Screen_Status GET_CURRENT_SYSTEM_STATE



////////////////////////////////////////////////////////////////////////////////
// GET_PREVIOUS_SCREEN

GUI_SCREEN_ID Current_Screen_Status::get_previous_screen ()
{
   return (previous_screen);

}    // END of Current_Screen_Status GET_PREVIOUS_SCREEN


////////////////////////////////////////////////////////////////////////////////
// GET_PREVIOUS_SCREEN

GUI_SCREEN_ID Current_Screen_Status::get_previous_displayed_screen ()
{
   return (previous_displayed_screen);

}    // END of Current_Screen_Status GET_PREVIOUS_SCREEN

////////////////////////////////////////////////////////////////////////////////
// CHECKPOINT_SCREEN_STATE

void Current_Screen_Status::checkpoint_screen_state ()
{
   // DEBUG/TEMP CODE TO BE DEVELOPED
   printf ("SCRNSTAT checkpoint code is not implemented\n");

}    // END of Current_Screen_Status CHECKPOINT_SCREEN_STATE



////////////////////////////////////////////////////////////////////////////////
// RESTORE_CHECKPOINT

void Current_Screen_Status::restore_checkpoint ()
{
   // DEBUG/TEMP CODE TO BE DEVELOPED
   printf ("SCRNSTAT restore code is not implemented\n");

}    // END of Current_Screen_Status RESTORE_CHECKPOINT



////////////////////////////////////////////////////////////////////////////////
// SET_CURRENT_SCREEN

void Current_Screen_Status::set_current_screen (GUI_SCREEN_ID status_current_screen)
{
   current_screen = status_current_screen ;
}    // END of Current_Screen_Status SET_CURRENT_SCREEN



////////////////////////////////////////////////////////////////////////////////
// SET_CURRENT_DISPLAYED_SCREEN

void Current_Screen_Status::set_current_displayed_screen (Base_Window* status_currently_displayed_screen_ptr)
{
   current_displayed_screen     = status_currently_displayed_screen_ptr ? status_currently_displayed_screen_ptr->get_screen_id() : GUI_SCREEN_NULL_SCREEN ;
   current_displayed_screen_ptr = status_currently_displayed_screen_ptr;
}    // END of Current_Screen_Status SET_CURRENT_DISPLAYED_SCREEN




////////////////////////////////////////////////////////////////////////////////
// SET_CURRENT_SYSTEM_STATE

void Current_Screen_Status::set_current_system_state (State_names status_current_system_state)
{
   current_system_state = status_current_system_state ;
}    // END of Current_Screen_Status SET_CURRENT_SYSTEM_STATE

void Current_Screen_Status::set_current_substate (State_names status_current_substate)
{
   current_substate = status_current_substate ;
}    // END of Current_Screen_Status SET_CURRENT_SYSTEM_STATE



////////////////////////////////////////////////////////////////////////////////
// SET_PREVIOUS_SCREEN

void Current_Screen_Status::set_previous_screen (GUI_SCREEN_ID status_previous_screen)
{
   previous_screen = status_previous_screen ;

}    // END of Current_Screen_Status SET_PREVIOUS_SCREEN


////////////////////////////////////////////////////////////////////////////////
// SET_PREVIOUS_DISPLAYED_SCREEN

void Current_Screen_Status::set_previous_displayed_screen (GUI_SCREEN_ID status_previous_displayed_screen)
{
   previous_displayed_screen = status_previous_displayed_screen ;

}    // END of Current_Screen_Status SET_PREVIOUS_SCREEN

////////////////////////////////////////////////////////////////////////////////
// SWAP_CURRENT_PREVIOUS_SCREEN

void Current_Screen_Status::swap_current_previous_screen ()
{
   GUI_SCREEN_ID temp_screen;

   temp_screen     = previous_screen ;

   previous_screen = current_screen ;

   current_screen  = temp_screen ;


}    // END of Current_Screen_Status SWAP_CURRENT_PREVIOUS_SCREEN

/* FORMAT HASH fcfd4318f3447b4fd20aa848b384be05 */

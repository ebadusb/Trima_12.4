/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_Commutator

*     File name:   scrncomm.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************/

#include <vxworks.h>
#include <stdio.h>

#include "scrncomm.hpp"
#include "guiglobs.hpp"
#include "statline.hpp"
#include "frmtdate.hpp"


extern SCREEN_STATE_TABLE screen_state_transition_table[];

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_Commutator::Screen_Commutator()
   : _showingChildOverStateless(false),
     child_screen_stack(),
     stateless_screen_stack(),
     screen_transition_table(),
     screen_pointer_table(),
     // Initialize all the message objects
     Msg_Display_Screen                     (Callback<Screen_Commutator>(this, &Screen_Commutator::display_screen), MessageBase::SNDRCV_RECEIVE_ONLY),
     Msg_Notify_System_State_Change         (Callback<Screen_Commutator>(this, &Screen_Commutator::notify_system_state_change), MessageBase::SNDRCV_RECEIVE_ONLY),
     Msg_Notify_Substate_Change             (Callback<Screen_Commutator>(this, &Screen_Commutator::notify_substate_change), MessageBase::SNDRCV_RECEIVE_ONLY),
     Msg_Initialize_States                  (Callback<Screen_Commutator>(this, &Screen_Commutator::initialize_states), MessageBase::SNDRCV_RECEIVE_ONLY),
     Msg_Remove_All_Child_Screens           (Callback<Screen_Commutator>(this, &Screen_Commutator::remove_all_child_screens), MessageBase::SNDRCV_RECEIVE_ONLY),
     Msg_Remove_Most_Recent_Child_Screen    (Callback<Screen_Commutator>(this, &Screen_Commutator::remove_most_recent_child_screen), MessageBase::SNDRCV_RECEIVE_ONLY),
     Msg_Switch_To_Previous_Screen          (Callback<Screen_Commutator>(this, &Screen_Commutator::switch_to_previous_screen), MessageBase::SNDRCV_RECEIVE_ONLY),
     Msg_Remove_All_Stateless_Screens       (Callback<Screen_Commutator>(this, &Screen_Commutator::remove_all_stateless_screens), MessageBase::SNDRCV_RECEIVE_ONLY),
     Msg_Remove_Most_Recent_Stateless_Screen(Callback<Screen_Commutator>(this, &Screen_Commutator::remove_most_recent_stateless_screen), MessageBase::SNDRCV_RECEIVE_ONLY),
     Msg_Vista_Connection_Update            (Callback<Screen_Commutator>(this, &Screen_Commutator::process_vista_connection_message), MessageBase::SNDRCV_RECEIVE_ONLY),
     Msg_SnapScreenshot                     (Callback<Screen_Commutator>(this, &Screen_Commutator::snap_screenshot), MessageBase::SNDRCV_RECEIVE_ONLY)
#if CPU==SIMNT
     , _currentWindow(NULL)
#endif // #if CPU==SIMNT
{}



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_Commutator::~Screen_Commutator()
{
   // Deallocate the stateless screen stack
   stateless_screen_stack.clear_child_screen_stack();

   // Deallocate the child screen stack
   child_screen_stack.clear_child_screen_stack();

   // Get the name of the active procedure screen
   GUI_SCREEN_ID active_screen = guiglobs::screen_status.get_current_screen();

   // If there is an active procedure screen, unrealize and deallocate it
   if (active_screen != GUI_SCREEN_NULL_SCREEN)
   {
      Base_Window* current_screen = screen_pointer_table.get_static_screen_ptr(active_screen);

      if (current_screen)
         current_screen->deallocate_resources();
   }

}   // END of Screen_Commutator DESTRUCTOR




////////////////////////////////////////////////////////////////////////////////
// START_COMMUTATOR

void Screen_Commutator::start_commutator ()
{

   // The powerup screen will always be the first screen to be
   //  displayed. Mark NULL screen as the current screen so that
   //  no deallocate will be attempted.
   guiglobs::screen_status.set_current_screen (GUI_SCREEN_NULL_SCREEN);
   guiglobs::screen_status.set_current_displayed_screen (NULL);
   guiglobs::screen_status.set_previous_screen (GUI_SCREEN_NULL_SCREEN);
   guiglobs::screen_status.set_previous_displayed_screen (GUI_SCREEN_NULL_SCREEN);

   // We will always start the procedure on the powerup selftest screen
   guiglobs::screen_status.set_current_system_state (SELF_TEST);

   // Actually display the powerup screen now
   switch_to_new_apheresis_screen (screen_state_transition_table[0].current_screen, "");

}   // END of Screen_Commutator START_COMMUTATOR



////////////////////////////////////////////////////////////////////////////////
// SWITCH_TO_NEW_APHERESIS_SCREEN

void Screen_Commutator::switch_to_new_apheresis_screen (GUI_SCREEN_ID new_screen, const char* allocation_parameter)
{
   Base_Window* current_screen_ptr = NULL;

   DataLog(log_level_gui_info) << "switch_to_new_apheresis_screen " << new_screen << endmsg;

   // If the screen to be switched-to is the current screen, don't bother.
   if (new_screen == guiglobs::screen_status.get_current_screen ())
   {
      return;
   }

   _showingChildOverStateless = false;
   // First remove all of the child screens, if they exist
   child_screen_stack.clear_child_screen_stack ();


   // If the current screen status shows an active (non-NULL) screen, remember it.
   if (guiglobs::screen_status.get_current_screen () != GUI_SCREEN_NULL_SCREEN)
   {

      // Deallocate the current active screen
      GUI_SCREEN_ID current_screen_id = guiglobs::screen_status.get_current_screen ();

      current_screen_ptr = screen_pointer_table.get_static_screen_ptr
                              (current_screen_id);
   }


   // Make the current screen the "previous screen"
   guiglobs::screen_status.set_previous_screen (guiglobs::screen_status.get_current_screen ());

   // Make the new_screen the current_screen
   guiglobs::screen_status.set_current_screen (new_screen);

   // Find the new screen's pointer
   Base_Window* new_screen_ptr = screen_pointer_table.get_static_screen_ptr(new_screen);

   // Make sure nobody's still listening to the status line
   if (guiglobs::apheresis_status_line) guiglobs::apheresis_status_line->clearTextChangeCallback();

   // Allocate the new screen's resources
   new_screen_ptr -> allocate_resources(allocation_parameter);

   // Set the new screen's invocation ID
   new_screen_ptr -> set_screen_invocation_type (GUI_SCREEN_INVOKE_STATIC);

   // Resolve which screen from the child stack, stateless stack or procedure screen
   //  should be displayed.
   resolve_current_screen_display(true);

   // If the previous screen status shows that it was active, deallocate it.
   if (current_screen_ptr != NULL)
   {
      // Deallocate it, but don't unrealize it. The next screen will fully paint over it.
      current_screen_ptr -> deallocate_resources ();

      // Set the screen's invocation ID to not actively displayed
      current_screen_ptr -> set_screen_invocation_type (GUI_SCREEN_INVOKE_NOTACTIVE);

   }
   update_dont_connect_donor_icon();
   update_vista_connected_icon();

}   // END of Screen_Commutator SWITCH_TO_NEW_APHERESIS_SCREEN




////////////////////////////////////////////////////////////////////////////////
// SWITCH_TO_SCREEN

void Screen_Commutator::switch_to_screen (GUI_SCREEN_ID new_screen, const char* allocation_parameter)
{
   State_names new_state;
   State_names current_state;
   int         reset_screens_flag;


   // Note Scrncomm call in log.
   DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: switch to screen " << new_screen << endmsg;

   // Get the next system state required to display the new screen. If its the same as the
   // current system state, just display the new screen, otherwise, request a system state
   // change from Procedure.
   new_state = screen_transition_table.get_next_system_state_associative (new_screen, reset_screens_flag);

   // If the new_state = -1, then we've encountered an invalid screen change request
   if (new_state == -1)
   {
       #if CPU==SIMNT // If we're in the simulator and we call for an unknown transition, just do it with no state change.
      new_state = guiglobs::screen_status.get_current_system_state();
       #else

      DataLog(log_level_gui_info) << "Invalid screen transition request ignored: "
                                  << "Current Screen = " << guiglobs::screen_status.get_current_screen()
                                  << ", New Screen = " << new_screen
                                  << ", Current State = " << guiglobs::screen_status.get_current_system_state()
                                  << endmsg;
      return;


      #endif // #if CPU==SIMNT
   }

   // If this transition specifies that all of the procedure screen's internal states have to be reset,
   //  do it now.
   if (reset_screens_flag == TRUE)
   {
      initialize_states ();
   }

   // Get the current state
   current_state = guiglobs::screen_status.get_current_system_state();

   if (current_state == new_state)
   {
      // no state change required, just display new screen
      switch_to_new_apheresis_screen (new_screen, allocation_parameter);
   }
   else
   {
      // Request that procedure change to the transition-table-determined new state.
      SystemStateChangeRequestMsg statemsg(MessageBase::SEND_LOCAL);
      statemsg.send(new_state);
      #if CPU==SIMNT
      // Allow me to jump around in the simulator.
      switch_to_new_apheresis_screen (new_screen, allocation_parameter);
      #endif
   }
   update_dont_connect_donor_icon();
   update_vista_connected_icon();

}   // END of Screen_Commutator SWITCH_TO_SCREEN



////////////////////////////////////////////////////////////////////////////////
// INITIALIZE_STATES

void Screen_Commutator::initialize_states ()
{
   Screen_Pointer_Table::SCREEN_TABLE_ITER screen_name_table_cursor;

   // Note Scrncomm call in log.
   DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: initialize states" << endmsg;

   // Get the first screen pointer
   Base_Window* screen_pointer = screen_pointer_table.get_first_static_screen_table_pointer
                                    (screen_name_table_cursor);

   // Loop through all the screen pointers, resetting the internal states of the apheresis screens.
   while (screen_pointer != NULL)
   {
      // Call the screen's internal reset method.
      screen_pointer -> reset_internal_screen_state ();

      // Get the next procedure screen pointer
      screen_pointer = screen_pointer_table.get_next_static_screen_table_pointer
                          (screen_name_table_cursor);
   } // End of "while (screen_pointer != NULL)

}   // END of Screen_Commutator INITIALIZE_STATES




////////////////////////////////////////////////////////////////////////////////
// NOTIFY_SYSTEM_STATE_CHANGE

void Screen_Commutator::notify_system_state_change ()
{
   State_names new_state = (State_names)Msg_Notify_System_State_Change.getData();
   // This is called when a system state change message arrives in the GUI domain

   GUI_SCREEN_ID new_screen;
   int           default_used;
   int           reset_screens_flag;

   // Note Scrncomm call in log.
   DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: notify system state change " << new_state
                               << " (" << state_name_table[new_state].state_text << ')'
                               << endmsg;

   guiglobs::display_dont_connect_donor_icon = (
      (new_state == STARTUP_TESTS) ||
      (new_state == DISPOSABLE_TEST) ||
      (new_state == AC_CONNECTED) ||
      (new_state == AC_PRIME) ||
      (new_state == METERED_STORAGE) ||
      (new_state == METERED_STORAGE_DISCONNECT)
      );

   // See if switching system states requires the switching to a new screen
   new_screen = screen_transition_table.get_next_screen_associative (new_state, default_used, reset_screens_flag);

   // If the default screen for the new state was used, or if this screen/state transition
   // required mass screen reset then initialize all the internal screen states.
   if ((default_used == TRUE) || (reset_screens_flag == TRUE))
   {
      initialize_states ();
   }

   // Set the status to reflect the new system state
   guiglobs::screen_status.set_current_system_state(new_state);

   // If the screen associated with this state is different than the currently
   //  displayed screen (most probably), switch to it.
   if (new_screen != guiglobs::screen_status.get_current_screen())
   {
      // New procedure screens currently have no meaningful allocation parameters
      switch_to_new_apheresis_screen (new_screen, "");
   }

}   // END of Screen_Commutator NOTIFY_SYSTEM_STATE_CHANGE


void Screen_Commutator::update_dont_connect_donor_icon ()
{
   if (guiglobs::display_dont_connect_donor_icon)
   {
      guiglobs::dont_connect_icon.deallocate_resources();
      Base_Window* curscrn = screen_pointer_table.get_static_screen_ptr(guiglobs::screen_status.get_current_screen());
      DataLog(log_level_gui_info) << "setting dont connect icon" << endmsg;
      guiglobs::dont_connect_icon.allocate_resources(*curscrn);
      guiglobs::dont_connect_icon.set_visible(1);
   }
   else
   {
      guiglobs::dont_connect_icon.set_visible(0);
      DataLog(log_level_gui_debug) << "removing dont connect icon" << endmsg;
   }
}


void Screen_Commutator::update_vista_connected_icon ()
{
   GUI_SCREEN_ID curscrnID = guiglobs::screen_status.get_current_screen();

   DataLog(log_level_gui_debug) << "update_vista_connected_icon current screen ID: " << curscrnID << " setting vista connect icon" << endmsg;

   if ((curscrnID != GUI_SCREEN_POWERUP) && (curscrnID != GUI_SCREEN_POWERDOWN))
   {
      if (guiglobs::display_vista_connected_icon == 0)
      {
         guiglobs::vista_connected_icon.set_visible(0);
      }
      else
      {
         Base_Window* curscrn = screen_pointer_table.get_static_screen_ptr(curscrnID);

         guiglobs::vista_connected_icon.deallocate_resources();
         guiglobs::vista_connected_icon.allocate_resources(*curscrn);

         guiglobs::vista_connected_icon.set_visible(1);
      }
   }
}


void Screen_Commutator::process_vista_connection_message ()
{

   guiglobs::display_vista_connected_icon = (int)Msg_Vista_Connection_Update.getData();
   DataLog(log_level_gui_info) << "Receive VistaConnectionStatusMsg message, display_vista_connected_icon: "
                               << guiglobs::display_vista_connected_icon << endmsg;

   update_vista_connected_icon();

}


////////////////////////////////////////////////////////////////////////////////
//

void Screen_Commutator::notify_substate_change ()
{
   // This is called when a system substate change message arrives in the GUI
   // domain.  The substate is simply captured for use by other methods.
   guiglobs::screen_status.set_current_substate((State_names)Msg_Notify_Substate_Change.getData());
}



////////////////////////////////////////////////////////////////////////////////
// SWITCH_TO_PREVIOUS_SCREEN

void Screen_Commutator::switch_to_previous_screen ()
{
   // Get the ID of the previous screen and pass it to the screen commutator -
   //  WARNING: this had better be a valid screen/state transition or KABOOM!

   // Note Scrncomm call in log.
   DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: switch to previous screen "
                               << guiglobs::screen_status.get_previous_screen()
                               << endmsg;

   switch_to_screen (guiglobs::screen_status.get_previous_screen(), "");

}   // END of Screen_Commutator SWITCH_TO_PREVIOUS_SCREEN



////////////////////////////////////////////////////////////////////////////////
//////////////////////// STACK-BASED SCREEN ROUTINES ///////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// DISPLAY_CHILD_SCREEN

void Screen_Commutator::display_child_screen (GUI_SCREEN_ID child_screen, const char* allocation_parameter)
{
   // Note Scrncomm call in log.
   DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: display child screen "
                               << child_screen << endmsg;

   // Call the generic display stack screen routine referencing the child screen stack
   display_stack_screen (child_screen,
                         allocation_parameter,
                         &child_screen_stack,
                         GUI_SCREEN_INVOKE_CHILD);

   // Resolve which screen from the child stack, stateless stack or procedure screen
   //  should be displayed.
   resolve_current_screen_display();

}   // END of Screen_Commutator DISPLAY_CHILD_SCREEN



////////////////////////////////////////////////////////////////////////////////
// SWAP_CHILD_SCREEN

void Screen_Commutator::swap_child_screen (GUI_SCREEN_ID child_screen, const char* allocation_parameter)
{
   // Note Scrncomm call in log.
   DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: swap child screen "
                               << child_screen << endmsg;

   // Call the generic swap stack screen routine referencing the child screen stack
   swap_stack_screen (child_screen,
                      allocation_parameter,
                      &child_screen_stack,
                      GUI_SCREEN_INVOKE_CHILD);

   // Resolve which screen from the child stack, stateless stack or procedure screen
   //  should be displayed.
   resolve_current_screen_display();

}   // END of Screen_Commutator SWAP_CHILD_SCREEN



////////////////////////////////////////////////////////////////////////////////
// SWAP_CHILD_STACK_WITH_A_SCREEN

void Screen_Commutator::swap_child_stack_with_a_screen (GUI_SCREEN_ID child_screen, const char* allocation_parameter)
{

   // Note Scrncomm call in log.
   DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: swap child stack with screen "
                               << child_screen << endmsg;

   _showingChildOverStateless = false;
   // Call the generic stack cleaning routine to clear the child stack
   remove_all_stack_screens (&child_screen_stack);

   // Call the generic display stack screen routine referencing the child screen stack
   display_stack_screen (child_screen,
                         allocation_parameter,
                         &child_screen_stack,
                         GUI_SCREEN_INVOKE_CHILD);


   // Resolve which screen from the child stack, stateless stack or procedure screen
   //  should be displayed.
   resolve_current_screen_display();

}   // END of Screen_Commutator SWAP_CHILD_STACK_WITH_A_SCREEN



////////////////////////////////////////////////////////////////////////////////
// REMOVE_ALL_CHILD_SCREENS

void Screen_Commutator::remove_all_child_screens ()
{
   // Note Scrncomm call in log.
   DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: remove all child screens" << endmsg;

   _showingChildOverStateless = false;
   // Call the generic stack cleaning routine to clear the child stack
   remove_all_stack_screens (&child_screen_stack);

   // Resolve which screen from the child stack, stateless stack or procedure screen
   //  should be displayed.
   resolve_current_screen_display();

}   // END of Screen_Commutator REMOVE_ALL_CHILD_SCREENS



////////////////////////////////////////////////////////////////////////////////
// REMOVE_MOST_RECENT_CHILD_SCREEN

void Screen_Commutator::remove_most_recent_child_screen ()
{
   // Note Scrncomm call in log.
   DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: remove most recent child screen" << endmsg;

   _showingChildOverStateless = false;
   // Call the generic top-of-stack removal routine.
   remove_most_recent_stack_screen (&child_screen_stack);

   // Resolve which screen from the child stack, stateless stack or procedure screen
   //  should be displayed.
   resolve_current_screen_display();

}   // END of Screen_Commutator REMOVE_MOST_RECENT_CHILD_SCREEN



////////////////////////////////////////////////////////////////////////////////
// DISPLAY_STATELESS_SCREEN

void Screen_Commutator::display_stateless_screen (GUI_SCREEN_ID stateless_screen,
                                                  const char* allocation_parameter)
{
   // Note Scrncomm call in log.
   DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: display stateless screen "
                               << stateless_screen << endmsg;

   // Call the generic display stack screen routine referencing the stateless stack
   display_stack_screen (stateless_screen,
                         allocation_parameter,
                         &stateless_screen_stack,
                         GUI_SCREEN_INVOKE_STATELESS);

   // Resolve which screen from the child stack, stateless stack or procedure screen
   //  should be displayed.
   resolve_current_screen_display(true);

}   // END of Screen_Commutator DISPLAY_STATELESS_SCREEN



////////////////////////////////////////////////////////////////////////////////
// SWAP_STATELESS_SCREEN

void Screen_Commutator::swap_stateless_screen (GUI_SCREEN_ID stateless_screen,
                                               const char* allocation_parameter)
{
   // Note Scrncomm call in log.
   DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: swap stateless screen "
                               << stateless_screen << endmsg;

   // Call the generic swap stack screen routine referencing the stateless stack
   swap_stack_screen (stateless_screen,
                      allocation_parameter,
                      &stateless_screen_stack,
                      GUI_SCREEN_INVOKE_STATELESS);

   // Resolve which screen from the child stack, stateless stack or procedure screen
   //  should be displayed.
   resolve_current_screen_display(true);

}   // END of Screen_Commutator SWAP_STATELESS_SCREEN



////////////////////////////////////////////////////////////////////////////////
// SWAP_STATELESS_STACK_WITH_A_SCREEN

void Screen_Commutator::swap_stateless_stack_with_a_screen (GUI_SCREEN_ID stateless_screen,
                                                            const char* allocation_parameter)
{
   bool alarmScreen = ( stateless_screen == GUI_SCREEN_ALARM );
   if ( alarmScreen )
   {
      if ( guiglobs::stack_screens_to_process == 0 )
      {
         DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: swap stateless stack with screen "
                                     << stateless_screen << " request -- invalid protocol." << endmsg;
      }
      else
      {
         guiglobs::stack_screens_to_process--;
      }
   }

   if ( alarmScreen && (guiglobs::stack_screens_to_process > 0) )
   {
      // Note Scrncomm call in log.
      DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: swap stateless stack with screen "
                                  << stateless_screen << " ignored because more requests pending." << endmsg;
   }
   else
   {
      // Note Scrncomm call in log.
      DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: swap stateless stack with screen "
                                  << stateless_screen << endmsg;

      // Call the generic stack cleaning routine to clear the stateless stack
      remove_all_stack_screens (&stateless_screen_stack);

      // Call the generic display stack screen routine referencing the stateless stack
      display_stack_screen (stateless_screen,
                            allocation_parameter,
                            &stateless_screen_stack,
                            GUI_SCREEN_INVOKE_STATELESS);

      // Resolve which screen from the child stack, stateless stack or procedure screen
      //  should be displayed.
      resolve_current_screen_display(true);

   }

}   // END of Screen_Commutator SWAP_STATELESS_STACK_WITH_A_SCREEN



////////////////////////////////////////////////////////////////////////////////
// REMOVE_ALL_STATELESS_SCREENS

void Screen_Commutator::remove_all_stateless_screens ()
{
   // Note Scrncomm call in log.
   DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: remove all stateless screens" << endmsg;

   // Call the generic clean stack routine to empty the stateless stack
   remove_all_stack_screens (&stateless_screen_stack);

   // Resolve which screen from the child stack, stateless stack or procedure screen
   //  should be displayed.
   resolve_current_screen_display(true);

}   // END of Screen_Commutator REMOVE_ALL_STATELESS_SCREENS



////////////////////////////////////////////////////////////////////////////////
// REMOVE_MOST_RECENT_STATELESS_SCREEN

void Screen_Commutator::remove_most_recent_stateless_screen ()
{
   // Note Scrncomm call in log.
   DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: remove most recent stateless screen" << endmsg;

   // Call the generic top-of-stack removal routine.
   remove_most_recent_stack_screen (&stateless_screen_stack);

   // Resolve which screen from the child stack, stateless stack or procedure screen
   //  should be displayed.
   resolve_current_screen_display(true);

}   // END of Screen_Commutator REMOVE_MOST_RECENT_STATELESS_SCREEN



////////////////////////////////////////////////////////////////////////////////
// DISPLAY_STACK_SCREEN

void Screen_Commutator::display_stack_screen (GUI_SCREEN_ID stack_screen,
                                              const char*          allocation_parameter,
                                              Child_Screen_Stack*  screen_stack,
                                              GUI_SCREEN_INVOCATION invocation_type)
{
   if (guiglobs::screen_status.get_current_screen () == GUI_SCREEN_NULL_SCREEN)
   {
      // Don't display stack screens unless a static procedure screen is underneath it
      return;
   }

   // See if any other stack screens are active, if so make the most recent
   // stack screen inactive.
   CHILD_STACK_INFO stack_parm;

   // Get the youngest active stack screen
   screen_stack->peek_child_screen(&stack_parm);

   if (stack_parm.child_screen_ptr != NULL)
   {
      // Deactivate it
      stack_parm.child_screen_ptr->set_disabled(true);
   }

   // Get an allocation of the stack screen
   stack_parm.child_screen_ptr = screen_pointer_table.get_dynamic_screen_pointer (stack_screen);

   // Copy the allocation parameter into the stack parameter structure & cauterize the string
   strncpy (stack_parm.alloc_param, allocation_parameter, MAXSIZE_BWINDOW_PARAM - 1);

   // Push this stack screen information onto the screen stack.
   screen_stack->push_child_screen (&stack_parm);

   if (stack_parm.child_screen_ptr)
   {
      // Allocate the stack screen
      stack_parm.child_screen_ptr->allocate_resources (stack_parm.alloc_param);

      // Set the screen's invocation ID to actively displayed
      stack_parm.child_screen_ptr->set_screen_invocation_type (invocation_type);
   }
}   // END of Screen_Commutator DISPLAY_STACK_SCREEN



////////////////////////////////////////////////////////////////////////////////
// SWAP_STACK_SCREEN

void Screen_Commutator::swap_stack_screen (GUI_SCREEN_ID screen_id,
                                           const char* allocation_parameter,
                                           Child_Screen_Stack* screen_stack,
                                           GUI_SCREEN_INVOCATION invocation_type)
{
   // Remove the Top-Stack-Screen, if one exists
   remove_most_recent_stack_screen (screen_stack);

   // Add the new stack screen onto the stack
   display_stack_screen (screen_id, allocation_parameter, screen_stack, invocation_type);

}   // END of Screen_Commutator SWAP_STACK_SCREEN


////////////////////////////////////////////////////////////////////////////////
// REMOVE_ALL_STACK_SCREENS

void Screen_Commutator::remove_all_stack_screens (Child_Screen_Stack* screen_stack)
{

   // Call the clear stack to Photon-deallocate and C++ delete all
   // stateless screens and empty the stack.
   screen_stack -> clear_child_screen_stack ();

}   // END of Screen_Commutator REMOVE_ALL_STACK_SCREENS



////////////////////////////////////////////////////////////////////////////////
// REMOVE_MOST_RECENT_STACK_SCREEN

void Screen_Commutator::remove_most_recent_stack_screen (Child_Screen_Stack* screen_stack)
{
   // See if there is anything in the stack, if not go away
   if (screen_stack->get_stack_count() > 0)
   {
      CHILD_STACK_INFO popee_stack_parm;

      // Pop off the youngest active stack screen
      screen_stack->pop_child_screen(&popee_stack_parm);

      if (popee_stack_parm.child_screen_ptr != NULL)
      {
         // Deallocate the original TOS stack screen
         popee_stack_parm.child_screen_ptr->deallocate_resources();

         // Return the screen stack object to the great HEAP.
         delete popee_stack_parm.child_screen_ptr;
         popee_stack_parm.child_screen_ptr = NULL;
      }

   }  // End of "if (screen_stack...


}   // END of Screen_Commutator REMOVE_MOST_RECENT_STACK_SCREEN



////////////////////////////////////////////////////////////////////////////////
// RESOLVE_CURRENT_SCREEN_DISPLAY

void Screen_Commutator::resolve_current_screen_display (bool sendSimMsg)
{
   // This is called after all screen management routines are finished
   //  to determine what screen should be realized on the physical display
   //  (static_screen, child_screen, stateless_screen).
   // Note V5 (Photon) disabled all screens first, then re-enabled top screen.
   // This didn't work with Zinc (although the screen was "enabled", it no
   // longer received inputs).  The assumption is that the only time a screen
   // needs to be disabled is the case where a child screen is on top of a
   // static screen, in which case the static screen needs to be disabled.

   Base_Window*     displayed_screen_ptr = NULL;                       // pointer to the screen to display
   CHILD_STACK_INFO child_stack_peekee;                                // child stack element to contain peeked child.
   Base_Window*     disabled_screen_ptr  = NULL;


   // The order of display precedence is:
   //     First:   Top of stateless_child screen stack
   //     Second:  Top of child screen stack
   //     Third:   Static screen

   // See if there are any stateless screens to be displayed
   if (stateless_screen_stack.get_stack_count() > 0)
   {
      // Yes, a stateless screen exists, get info and manifest it.
      const GUI_SCREEN_ID displayedScreenId        = guiglobs::screen_status.get_current_displayed_screen();
      Base_Window*        displayedScreenWindowPtr = 0;

      // Yes, a stateless screen exists, get info and manifest it.
      stateless_screen_stack.peek_child_screen(&child_stack_peekee);

      displayed_screen_ptr = child_stack_peekee.child_screen_ptr;

      if (displayedScreenId != GUI_SCREEN_NULL_SCREEN)
         displayedScreenWindowPtr = child_stack_peekee.child_screen_ptr;

      if (child_screen_stack.get_stack_count() > 0)
      {
         // a child screen exists, get info and manifest it.
         child_screen_stack.peek_child_screen(&child_stack_peekee);

         if (child_stack_peekee.alloc_param.contains(CHILD_OF_STATELESS))
         {
            DataLog (log_level_gui_info) << "OK to display child over stateless." << endmsg;

            displayed_screen_ptr       = child_stack_peekee.child_screen_ptr;

            _showingChildOverStateless = true;

            if (displayedScreenWindowPtr && (displayedScreenWindowPtr != displayed_screen_ptr))
            {
               disabled_screen_ptr = displayedScreenWindowPtr;
            }
         }
         else
         {
            DataLog (log_level_gui_info) << "NOT OK to display child over stateless." << endmsg;

            // Don't bother trying to show the child screen because it's not meant for that.
            // show the stateless screen instead.
            stateless_screen_stack.peek_child_screen(&child_stack_peekee);

            displayed_screen_ptr = child_stack_peekee.child_screen_ptr;
         }
      }
      else
      {
         // No child screen exists, so show the stateless screen.
         stateless_screen_stack.peek_child_screen(&child_stack_peekee);

         displayed_screen_ptr = child_stack_peekee.child_screen_ptr;
      }
   }
   else // look at child screens and static screens
   {
      const GUI_SCREEN_ID static_screen_id = guiglobs::screen_status.get_current_screen();
      Base_Window*        static_screen    = 0;

      if (static_screen_id != GUI_SCREEN_NULL_SCREEN)
         static_screen = screen_pointer_table.get_static_screen_ptr(static_screen_id);

      if (child_screen_stack.get_stack_count() > 0)
      {
         // a child screen exists, get info and manifest it.
         child_screen_stack.peek_child_screen(&child_stack_peekee);
         displayed_screen_ptr = child_stack_peekee.child_screen_ptr;

         if (static_screen && (static_screen != displayed_screen_ptr))
         {
            disabled_screen_ptr = static_screen;
         }
      }
      else // look at the static screen
      {
         displayed_screen_ptr = static_screen;
      }
   }

   // Take the pointer to the screen that should be displayed and
   //  realize it
   if (displayed_screen_ptr != NULL)
   {
      // Note Screen that will display on top in log.
      DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: Screen "
                                  << displayed_screen_ptr->get_screen_id () << " resolved for display" << endmsg;

      // Ensure this screen is in front and active
      displayed_screen_ptr->attach(NULL);
      displayed_screen_ptr->set_disabled(false);
      displayed_screen_ptr->refresh_resources();

      //
      //   Disable the screen behind if needed.
      //
      if (disabled_screen_ptr)
      {
         disabled_screen_ptr->set_disabled(true);

         DataLog(log_level_gui_info) << "SCREEN_COMMUTATOR LOG: Screen "
                                     << disabled_screen_ptr->get_screen_id() << " disabled." << endmsg;
      }

      // Make the current screen the "previous screen"
      guiglobs::screen_status.set_previous_displayed_screen (guiglobs::screen_status.get_current_displayed_screen ());

      // Make the new_screen the current_screen
      guiglobs::screen_status.set_current_displayed_screen (displayed_screen_ptr);

      #if (CPU==SIMNT)
      _currentWindow = displayed_screen_ptr;
      // In the simulator, screen commutator sends out this message for stateless and
      // static screen transitions to facilitate synchronization with the screenshot
      // task.
      if (sendSimMsg)
      {
         SimntGuiScreenChange screenChange(MessageBase::SEND_LOCAL);
         DataLog (log_level_gui_info) << "Sending screen change message." << endmsg;
         screenChange.send();
      }
      #endif
   } // End of " if (displayed_screen_ptr != NULL)

}   // END of Screen_Commutator RESOLVE_CURRENT_SCREEN_DISPLAY

void Screen_Commutator::display_screen ()
{
   SCREEN_INVOKE_STRUCT data = Msg_Display_Screen.getData();

   // IT 8629 - Fix to clear child screens over stateless screens to avoid crazy shenanigans.
   if (_showingChildOverStateless)
   {
      DataLog (log_level_gui_info) << "Showing a child screen over a stateless screen.  Clear child stack." << endmsg;
      remove_all_stack_screens (&child_screen_stack);
   }

   // Call the generic stack cleaning routine to clear the child stack
   switch (data.screen_invocation)
   {
      case DISPLAY_AS_CHILD :
         display_child_screen(data.screen_id, data.alloc_param);
         break;
      case INVOKE_SCREEN :
         switch_to_screen(data.screen_id, data.alloc_param);
         break;
      case DISPLAY_AS_STATELESS :
         display_stateless_screen(data.screen_id, data.alloc_param);
         break;
      case SWAP_CHILD_SCREEN :
         swap_child_screen(data.screen_id, data.alloc_param);
         break;
      case SWAP_STATELESS_SCREEN :
         swap_stateless_screen(data.screen_id, data.alloc_param);
         break;
      case SWAP_CHILD_WITH_SCREEN :
         swap_child_stack_with_a_screen(data.screen_id, data.alloc_param);
         break;
      case SWAP_STATELESS_WITH_SCREEN :
         swap_stateless_stack_with_a_screen(data.screen_id, data.alloc_param);
         break;
   }
}

Base_Window* Screen_Commutator::getScreenInstancePointer (GUI_SCREEN_ID screenId = GUI_SCREEN_NULL_SCREEN)
{
   if (screenId==GUI_SCREEN_NULL_SCREEN) screenId = guiglobs::screen_status.current_screen;
   return screen_pointer_table.get_static_screen_ptr(screenId);
}


void Screen_Commutator::snap_screenshot ()
{
   // Generate a time-stamped filename: XX_YYYY_mm_dd_HH_MM_SS
   // where X is the optional prefix letter; default is 'X'

   guistring  filename("XX_");

   const char prefix = (char)Msg_SnapScreenshot.getData();

   // replace the default prefix letter if it's a valid char
   if ( isalpha(prefix) )
   {
      sprintf(filename, "%c%c_", prefix, prefix);
   }

   // append the time stamp
   Format_date formatter;
   formatter.appendDateAndTime(filename, '_', '_', '_');

   Base_Window::screenshot(filename);

   DataLog(log_level_gui_info) << "Saved screenshot to: " << filename << endmsg;
}


#if CPU==SIMNT // This is only used for simulator.
// Parameter is just for double checking that we're getting what we think we're
// getting.
//
Base_Window* Screen_Commutator::getCurrentChildScreen ()
{
   CHILD_STACK_INFO stack_parm;

   if (guiglobs::screen_status.get_current_screen () == GUI_SCREEN_NULL_SCREEN)
   {
      DataLog (log_level_gui_error) << "There's no parent screen, so we can't deal with a child screen." << endmsg;
      // Don't display stack screens unless a static procedure screen is underneath it
      return NULL;
   }

   // See if any other stack screens are active, if so make the most recent
   //  stack screen inactive.
   if (child_screen_stack.get_stack_count() > 0)
   {
      // Get the youngest active stack screen and deactivate it.
      child_screen_stack.peek_child_screen(&stack_parm);
      return stack_parm.child_screen_ptr;
   }
   else
   {
      DataLog(log_level_gui_error) << "ERROR: No child screens in stack. Returning NULL." << endmsg;
      return NULL;
   }
}

Base_Window* Screen_Commutator::getCurrentChildScreen (GUI_SCREEN_ID screenId)
{
   Base_Window* stackPeekScreen = getCurrentChildScreen();

   if (stackPeekScreen)
   {
      if (stackPeekScreen->get_screen_id() == screenId) return stackPeekScreen;
      else
      {
         DataLog(log_level_gui_error) << "ERROR: Requested screen " << screenId
                                      << " not displayed. Current=" << stackPeekScreen->get_screen_id() << endmsg;
         return NULL;
      }
   }
   else
   {
      DataLog(log_level_gui_error) << "ERROR: Top of stack was NULL." << endmsg;
      return NULL;
   }
}

void Screen_Commutator::setScreenshotName (const char* info)
{
   if (info && _currentWindow) _currentWindow->setScreenshotName(info);
   else DataLog (log_level_gui_error) << "Unable to set string due to NULL pointer." << endmsg;
}

void Screen_Commutator::removeScreenshotName ()
{
   if (_currentWindow) _currentWindow->removeScreenshotName();
   else DataLog (log_level_gui_error) << "Unable to remove string due to NULL pointer." << endmsg;
}

void Screen_Commutator::setScreenshotInfo (const char* name)
{
   if (name && _currentWindow) _currentWindow->setScreenshotInfo(name);
   else DataLog (log_level_gui_error) << "Unable to set string due to NULL pointer." << endmsg;
}

void Screen_Commutator::removeScreenshotInfo ()
{
   if (_currentWindow) _currentWindow->removeScreenshotInfo();
   else DataLog (log_level_gui_error) << "Unable to remove string due to NULL pointer." << endmsg;
}
#endif // #if CPU==SIMNT // This is only used for simulator.

/* FORMAT HASH 14978183e7a15c38c5c96870fd2cd041 */

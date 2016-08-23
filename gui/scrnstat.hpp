/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Current_Screen_Status

*     File name:    scrnstat.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class manages the GUI domain's memory regarding the currently active
         screen, the currently displayed screen, the previous screen (not child),
         the current system state (as last transmitted to GUI by Procedure).  This
         class also controls the checkpointing of this information to and from
         harddisk.  All write ("set") methods (methods that change class data) are
         private and can only be accessed by the friend class, Screen_Commutator.


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: K:/BCT_Development/vxWorks/Trima/gui/rcs/scrnstat.hpp 1.6 1998/09/30 21:51:40 SB07663 Exp jl11312 $
$Log: scrnstat.hpp $
Revision 1.6  1998/09/30 21:51:40  SB07663
ITs 3496, 3373, 3376: Allow 'end run' button in the donor
disconnect system state if the operator has not yet confirmed that
the donor is disconnected.
Revision 1.5  1998/07/02 17:36:25  TD10216
IT3374
Revision 1.4  1996/07/08 16:44:23  tw09453
Update file header

*/



// Single include define guardian
#ifndef SCRNSTAT_HPP
#define SCRNSTAT_HPP



// System #INCLUDES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


// CLASS INCLUDE

#include "gui.h"
#include "states.h"
#include "bwindow.hpp"


class Current_Screen_Status
{

   // The Screen_Commutator is the only class that can "set" the
   // status values managed by this class or manage status
   // checkpointing - it does this by using the private "set"
   // methods below.

   friend class Screen_Commutator ;


private:


   // The currently displayed Procedure screen
   GUI_SCREEN_ID current_screen ;


   // The currently displayed screen (either procedure or child)
   GUI_SCREEN_ID current_displayed_screen ;

   Base_Window*  current_displayed_screen_ptr ;


   // The previously displayed screen
   GUI_SCREEN_ID previous_screen ;


   // The currently displayed screen (either procedure or child)
   GUI_SCREEN_ID previous_displayed_screen;


   // The current system state and substate
   State_names current_system_state ;
   State_names current_substate ;



private:

   // CHECKPOINT_SCREEN_STATE
   // This method saves the various state data to non-volatile storage
   void checkpoint_screen_state ();


   // RESTORE_CHECKPOINT
   // This method restores the various state data from non-volatile storage
   // to this classes private data representations of that data.
   void restore_checkpoint ();


   // SET_CURRENT_SCREEN
   // This method sets the value of the current_screen status variable above
   void set_current_screen (GUI_SCREEN_ID status_current_screen);


   // SET_CURRENT_DISPLAYED_SCREEN
   // This method sets the ID of the currently displayed screen, either
   // procedure type or child screen type.
   void set_current_displayed_screen (Base_Window* status_currently_displayed_screen_ptr);

   // SET_CURRENT_SYSTEM_STATE
   // This method sets the value of the current_system_state variable above
   void set_current_system_state (State_names status_current_system_state);
   void set_current_substate (State_names status_current_substate);


   // SET_PREVIOUS_SCREEN
   // This method sets the value of the previous_screen variable above
   void set_previous_screen (GUI_SCREEN_ID status_previous_screen);
   void set_previous_displayed_screen (GUI_SCREEN_ID status_previous_displayed_screen);


   // SWAP_CURRENT_PREVIOUS_SCREEN
   // This method swaps the values of the previous_screen and current_screen
   // variables above.
   void swap_current_previous_screen ();



public:


   // CONSTRUCTOR
   Current_Screen_Status();


   // DESTRUCTOR
   virtual ~Current_Screen_Status();


   // GET_CURRENT_SCREEN
   // This method returns the ID of the currently displayed procedure screen.
   GUI_SCREEN_ID get_current_screen ();


   // GET_CURRENT_DISPLAYED_SCREEN
   // This method returns the ID of the currently displayed screen, either
   // procedure type or child screen type.
   GUI_SCREEN_ID get_current_displayed_screen ();

   // GET_CURRENT_DISPLAYED_SCREEN_PTR
   // This method returns the pointer to the currently displayed screen, either
   // procedure type or child screen type.
   Base_Window* Current_Screen_Status::get_current_displayed_screen_ptr ();


   // GET_CURRENT_SYSTEM_STATE
   // This method returns the value of the current_system_state variable above.
   State_names get_current_system_state ();
   State_names get_current_substate ();


   // GET_PREVIOUS_SCREEN
   // This method returns the value of the previous_screen variable above.
   GUI_SCREEN_ID get_previous_screen ();

   // GET_PREVIOUS_DISPLAYED_SCREEN
   // This method returns the value of the previous_screen variable above.
   GUI_SCREEN_ID get_previous_displayed_screen ();
};  // END of Current_Screen_Status Class Declaration


// End of single include define guardian
#endif /* ifndef SCRNSTAT_HPP */

/* FORMAT HASH 750615e4f7af530de86dd0a7368d5432 */

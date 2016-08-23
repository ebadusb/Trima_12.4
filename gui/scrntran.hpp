/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_Transition_Table

*     File name:    scrntran.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class can be passed the ID of a new Procedure state and depending on
         the current Procedure state and current screen will return the next screen
         to display.  It can also be passed the ID of a screen and depending on
         the current screen and current Procedure state will return the next
         Procedure state that must be in effect before the new screen can be
         displayed.


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: I:/BCT_Development/vxWorks/Trima/gui/rcs/scrntran.hpp 1.7 2003/10/16 17:02:45Z jl11312 Exp rm70006 $
$Log: scrntran.hpp $
Revision 1.7  2003/10/16 17:02:45Z  jl11312
- changes for displaying run summary during rinse back (IT 5862)
Revision 1.6  2002/05/07 14:00:06Z  sb07663
First cut at VxWorks compatibility
Revision 1.5  1998/07/02 17:36:28  TD10216
IT3374
Revision 1.4  1997/03/11 16:13:31  Barry_Fulkerson
- removed all references to the old cfg_repo & cfg_serv screens. They are
  do not exist.
Revision 1.3  1996/07/08 17:21:13  tw09453
Update file header

*/



// Single include define guardian
#ifndef SCRNTRAN_HPP
#define SCRNTRAN_HPP



// System #INCLUDES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


// CLASS INCLUDES

#include "gui.h"
#include "defscrn.hpp" // default screen table declaration


// PROCEDURE'S INCLUDES

#include "states.h"


// the GUI screen/state transition table
struct SCREEN_STATE_TABLE
{
   State_names   current_state;
   GUI_SCREEN_ID current_screen ;
   State_names   next_state;
   GUI_SCREEN_ID next_screen ;
   int           reset_all_screens_flag ;   // will be either TRUE (reset em) or FALSE (leave em alone)
   int           disable_on_run_complete;   // transition is not allowed if run complete message has been received from proc
};
#define TABLE_END ((State_names) - 1000)



class Screen_Transition_Table
{
private:
   // Default screen for any given system state table
   Default_Screen_Table default_screen_table ;

public:

   // CONSTRUCTOR
   Screen_Transition_Table();


   // DESTRUCTOR
   virtual ~Screen_Transition_Table();


   // GET_NEXT_SCREEN_ASSOCIATIVE
   // This method returns the ID of the next screen to be displayed given
   // the new system state. If the screen cannot be found for the current state,
   // current screen and next screen, this routine will return the default screen for that
   // state and set the default_used flag to TRUE. The reset_all flag will be set, if for this
   // state transition all the screen's internal states should be reset.
   GUI_SCREEN_ID get_next_screen_associative (State_names new_system_state, int& default_used, int& reset_all);


   // GET_NEXT_SYSTEM_STATE_ASSOCIATIVE
   // This method returns the ID of the next system state that must be in
   // effect given the ID of the new screen. If the next state cannot be found
   // for the current state, current screen and new screen, this routine will
   // return a -1 (there is no NULL_STATE defined). The reset_all flag will be set,
   // if for this state transition all the screen's internal states should be reset.
   State_names get_next_system_state_associative (GUI_SCREEN_ID new_screen, int& reset_all);


}; // END of Screen_Transition_Table Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 215b20e4885c99ac90fdc5aea46ad401 */

/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_Transition_Table

*     File name:   scrntran.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: I:/BCT_Development/vxWorks/Trima/gui/rcs/scrntran.cpp 1.32 2003/10/16 17:02:41Z jl11312 Exp rm70006 $
$Log: scrntran.cpp $
Revision 1.32  2003/10/16 17:02:41Z  jl11312
- changes for displaying run summary during rinse back (IT 5862)
Revision 1.31  2002/08/16 16:36:44Z  sb07663
added <vxworks.h> include
Revision 1.30  2002/05/07 14:00:05  sb07663
First cut at VxWorks compatibility
Revision 1.29  2000/10/04 16:43:44  sb07663
Config.dat modifications for new volume removal limits
Revision 1.28  2000/02/28 22:20:48  SB07663
IT4612: new configuration guaranteed yield screen
Revision 1.27  1999/06/15 00:00:37  TW09453
Add the legal screen transitions for the Product warning screen and
the Label product screen. Also add a transition from runproc to predict
for those occasions when the adjust screen times out just after
initiating a transition to predict.
Revision 1.26  1999/05/07 16:18:48  SB07663
Added new "replacement fluid" configuration screen
Revision 1.25  1998/06/08 17:01:13  MC03015
removal of donordl to onvital screen transition
Revision 1.24  1998/05/20 21:48:49  MC03015
addition of donordl screen transitions
Revision 1.23  1998/02/10 01:38:27  MC03015
addition of appropriate screen transition entries to facilitate the transition from the sysdonor screen to runpoc in blood run.  the transition to blood pime is not allowed.   the transition to blood rinseback is not necessary.
Revision 1.22  1998/02/06 22:54:05  MC03015
introduce the screen transition from donor connect to runproc in a transition from the donor connected state to blood prime state
Revision 1.21  1997/05/02 22:50:10  tw09453
Restore to  Version 1.18 before the State to State mania changes
of 1.19, and 1.20
Revision 1.18  1997/04/15 00:06:32  tw09453
Change the transition from SYSTEST in the Disposable test state to
transition to the SYSDISP screen when the system state goes back
to the startup tests state. This permits a recovery from a back
disposable test.
Revision 1.17  1997/03/17 21:04:03  MC03015
Addition of PREDICT to DONINFO and TROUBLE in BLOOD_PRIME and BLOOD_RUN
Revision 1.16  1997/03/11 16:13:30  Barry_Fulkerson
- removed all references to the old cfg_repo & cfg_serv screens. They are
do not exist.
Revision 1.15  1997/02/26 02:33:18  Barry_Fulkerson
- changed reference from the cfg_adju file to the cfg_priority file.  Renamed
cfg_adju.cpp/hpp to cfg_priority to better represent what it is actually doing
also in preperation for changing the PROCEDURE PRIORITY screen to include
more than 5 procedures.
Revision 1.14  1997/01/10 22:56:15  tw09453
remove the bloodsmp screen from the world
Revision 1.13  1996/12/06 23:06:45  MC03015
Removed transition from DONCONN to SYSDONOR
Revision 1.12  1996/10/17 09:02:30  MC03015
Added DONINFO to PREDICT transition in BLOOD_PRIME and BLOOD_RUN
Revision 1.11  1996/10/09 23:24:04  Barry_Fulkerson
-added screen transitions for Configuration during the AC PRIME state
Revision 1.10  1996/10/07 11:24:05  MC03015
Addition of PREDICT Screen
Revision 1.9  1996/09/28 22:28:01  Barry_Fulkerson
GUI Configuration additions for 2.3
Revision 1.8  1996/07/08 17:21:07  tw09453
Update file header

*/

#include <vxworks.h>


// BASE CLASS INCLUDES


// CLASS INCLUDES
            #include "scrntran.hpp"

// EXTERNAL REFERENCES
            #include "guiglobs.hpp"


extern SCREEN_STATE_TABLE screen_state_transition_table[];


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_Transition_Table::Screen_Transition_Table()
{}   // END of Screen_Transition_Table CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_Transition_Table::~Screen_Transition_Table()
{}   // END of Screen_Transition_Table DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// GET_NEXT_SCREEN_ASSOCIATIVE

GUI_SCREEN_ID Screen_Transition_Table::get_next_screen_associative (State_names new_system_state, int& default_used, int& reset_all)
{

   int           table_index = 0 ;

   State_names   status_current_state ;
   GUI_SCREEN_ID status_current_screen ;

   status_current_screen = guiglobs::screen_status.get_current_screen ();
   status_current_state  = guiglobs::screen_status.get_current_system_state ();


   // If the state we are asked to go to is the CURRENT state, then
   //  return the name of the default screen.
   if (status_current_state == new_system_state)
   {
      // Pass back the flag that says that all the screens SHOULD be reset with this transition
      //  since it was a same state transition.
      reset_all = TRUE ;

      // We DID use a default screen for this state change, so indicate as much.
      default_used = TRUE ;

      return(default_screen_table.get_system_state_default_screen (new_system_state, status_current_screen)) ;
   }


   // Search the transition table to determine what is the next screen.
   while (screen_state_transition_table[table_index].current_state != TABLE_END)
   {

      // If the table entry contains the current state, current screen, and the newly notified
      // state then return that entry's associated new screen ID.
      if (screen_state_transition_table[table_index].current_state == status_current_state
          &&
          screen_state_transition_table[table_index].current_screen == status_current_screen
          &&
          screen_state_transition_table[table_index].next_state == new_system_state
          )
      {
         // Pass back the flag that says whether all the screens should be reset with this transition
         reset_all = screen_state_transition_table[table_index].reset_all_screens_flag ;

         // We DID NOT use a default screen for this state change, so indicate as much.
         default_used = FALSE ;

         return(screen_state_transition_table[table_index].next_screen);
      }

      // Bump the table index
      table_index++ ;
   }

   // Couldn't find the proper screen, so return the default screen
   //  for this new system state

   // Pass back the flag that says that all the screens should NOT be reset with this transition
   //  since it was a default transition.
   reset_all = FALSE ;

   // We DID use a default screen for this state change, so indicate as much.
   default_used = TRUE ;

   return(default_screen_table.get_system_state_default_screen (new_system_state)) ;

}   // END of Screen_Transition_Table GET_NEXT_SCREEN_ASSOCIATIVE



////////////////////////////////////////////////////////////////////////////////
// GET_NEXT_SYSTEM_STATE_ASSOCIATIVE

State_names Screen_Transition_Table::get_next_system_state_associative (GUI_SCREEN_ID new_screen, int& reset_all)
{

   int           table_index = 0 ;

   State_names   status_current_state ;
   GUI_SCREEN_ID status_current_screen ;

   status_current_screen = guiglobs::screen_status.get_current_screen ();
   status_current_state  = guiglobs::screen_status.get_current_system_state ();


   while (screen_state_transition_table[table_index].current_state != TABLE_END)
   {

      // If the table entry contains the current state, current screen, and the newly requested
      // screen then return that entry's associated new system state.
      if (screen_state_transition_table[table_index].current_state == status_current_state &&
          screen_state_transition_table[table_index].current_screen == status_current_screen &&
          screen_state_transition_table[table_index].next_screen == new_screen &&
          ( !screen_state_transition_table[table_index].disable_on_run_complete ||
            !guiglobs::proc_status.get_data().run_complete )
          )
      {
         // Pass back the flag that says whether all screens should be reset with this transition
         reset_all = screen_state_transition_table[table_index].reset_all_screens_flag ;

         return(screen_state_transition_table[table_index].next_state);
      }

      // Bump the table index
      table_index++ ;
   }

   // Pass back the flag that says that all the screens should NOT be reset with this transition
   //  since no screen transition was found.
   reset_all = FALSE ;

   // Couldn't find the proper screen, so return NULL system state
   return((State_names) - 1);

}   // END of Screen_Transition_Table GET_NEXT_SYSTEM_STATE_ASSOCIATIVE

/* FORMAT HASH a8c2d6e3f07cd4e53f21bb244623a35a */

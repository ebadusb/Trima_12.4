/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_Commutator

*     File name:    scrncomm.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

        This class is responsible for switching among the fixed screens and the
        dynamically allocated child / stateless screens.  It is driven by a
        repertoire of router messages that effect the screen switching.


*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.27  2008/08/28 19:41:14Z  dslausb
Some modifications for screenshot scripting
Revision 1.26  2007/08/21 18:56:44Z  dslausb
Changes to DRBC split config functionality
Revision 1.25  2006/03/03 17:30:03Z  dslausb
First attempt at bringing over MSS stuff
Revision 1.24  2003/10/30 17:08:31Z  rm70006
IT 6501.  Remove unneeded 5.0 code to refresh the current screen.  The 5.1 way uses the native code in display_text and is done through the graphics package in vxWorks.
Revision 1.23  2003/06/13 21:10:35Z  sb07663
IT5789 - display of "don't connect donor" icon
Revision 1.22  2003/01/06 18:31:21Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.21  2002/11/11 07:21:36  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.20  2002/08/06 18:58:28  ms10234
Changed the int messages to end with "Msg"
Revision 1.19  2002/06/05 14:35:19  sb07663
Modifications for compatibility with new Vx messaging system
Revision 1.18  2002/05/07 14:00:03  sb07663
First cut at VxWorks compatibility
Revision 1.17  2001/12/07 20:48:44  rm70006
IT 5355.
Change the behavior of refresh screen to call the new refresh
handler to update the language (possibly).
Revision 1.16  2001/10/01 17:13:55  rm70006
IT 4562.
Added refresh command to Screen coordinator that refreshes the
current screen.  Needed for Vista language change.
Revision 1.15  1999/05/28 15:51:29  TD10216
IT3950
Revision 1.14  1999/05/28 02:31:56  TD10216
IT3950
Revision 1.13  1998/09/30 21:51:38  SB07663
ITs 3496, 3373, 3376: Allow 'end run' button in the donor
disconnect system state if the operator has not yet confirmed that
the donor is disconnected.
Revision 1.12  1998/07/02 17:36:22  TD10216
IT3374
Revision 1.11  1997/05/12 17:50:42  tw09453
Add functionality to swap "stateless" and "child" stacks with an
individual screen.
Revision 1.10  1996/06/28 23:38:16  tw09453
Add logic to set the invocation-type flags in newly invoked
screens
Revision 1.9  1996/06/28 16:45:44  tw09453
Add the Stateless Screen functions and the stack swap functions
for both the Child and Stateless screens.

*/

// Single include define guardian
#ifndef SCRNCOMM_HPP
#define SCRNCOMM_HPP


// System #INCLUDES
#include <vxworks.h> // Needed for SIMNT compiler directives in header

// CLASS INCLUDES
#include "gui.h"
#include "chldstak.hpp"
#include "scrntran.hpp"
#include "scrnname.hpp"
#include "scrnstat.hpp"
#include "states.h"

// MESSAGE INCLUDES
#include "trimamessages.h"
#include "iguimsgs.h"

class Screen_Commutator
{

private:

   bool _showingChildOverStateless;


   // This is the sole declaration of the child screen stack object.
   Child_Screen_Stack child_screen_stack;


   // This is the sole declaration of the stateless screen stack object --
   //  It uses all the mechanisms of the child screen stack, therefore,
   //  we build another stack here.
   Child_Screen_Stack stateless_screen_stack;


   // This is the sole declaration of the screen transition table.
   Screen_Transition_Table screen_transition_table;


   // This is the sole declaration of the screen pointer table
   Screen_Pointer_Table screen_pointer_table;


   // SWITCH_TO_NEW_APHERESIS_SCREEN
   // This method does the actual swapping of active procedure (non-child) screens.
   // It deallocates any active screen, allocates and realizes the new screen andj
   // updates the status accordingly.
   void switch_to_new_apheresis_screen (GUI_SCREEN_ID new_screen, const char* allocation_parameter);


   // REMOVE_ALL_STACK_SCREENS
   // This method will unrealize all stack screens and clear the screen stack.
   void remove_all_stack_screens (Child_Screen_Stack* screen_stack);


   // REMOVE_MOST_RECENT_STACK_SCREEN
   // This method will unrealize and remove the most current stack screen from the
   // screen stack.
   void remove_most_recent_stack_screen (Child_Screen_Stack* screen_stack);


   // DISPLAY_STACK_SCREEN
   // This method will place an instance of a stack screen on the screen stack
   // then allocate the stack screen.
   void display_stack_screen (GUI_SCREEN_ID screen_id,
                              const char* allocation_parameter,
                              Child_Screen_Stack* screen_stack,
                              GUI_SCREEN_INVOCATION invocation_type);

   // SWAP_STACK_SCREEN
   // This method will replace an instance of a stack screen on the screen stack
   // with another then allocate the stack screen. If one doesn't currently exist
   // this routine will just add the requested screen to the top of the stack.
   void swap_stack_screen (GUI_SCREEN_ID screen_id,
                           const char* allocation_parameter,
                           Child_Screen_Stack* screen_stack,
                           GUI_SCREEN_INVOCATION invocation_type);

   // RESOLVE_CURRENT_SCREEN_DISPLAY
   // This routine will look at the child screen stack, the stateless screen stack, and the
   // current procedure screen and decide which should be made active displayed on top. Once
   // it decides, it activates that screen, moves it to the front, and realizes it.
   void resolve_current_screen_display (bool newAphScr = false);

   // Handler for the ScreenshotRequestMessage
   void snap_screenshot ();

public:

   // CONSTRUCTOR
   Screen_Commutator();


   // DESTRUCTOR
   virtual ~Screen_Commutator();


   // START_COMMUTATOR
   // This method will allocate the initial router (receive) messages that make the
   // commutator active to requests.
   void start_commutator ();


   // DISPLAY_CHILD_SCREEN
   // This method will place an instance of a child screen on the child screen stack
   // then allocate the child screen.
   void display_child_screen (GUI_SCREEN_ID new_child, const char* allocation_parameter);


   // INITIALIZE_STATES
   // This method will call the reset state method for all the fixed procedure screens.
   // It will also clear the child screen stack. This leaves the GUI environment as if
   // just starting.
   void initialize_states ();


   // NOTIFY_SYSTEM_STATE_CHANGE
   // This method is called when the router message is received that notifies the
   // commutator that a Procedure (system) state has changed.
   void notify_system_state_change ();


   // NOTIFY_SUBSTATE_CHANGE
   // This method is called when the router message is received that notifies the
   // commutator that a Procedure substate has changed.
   void notify_substate_change ();


   // REMOVE_ALL_CHILD_SCREENS
   // This method will unrealize all child screens and clear the child screen stack.
   void remove_all_child_screens ();


   // REMOVE_MOST_RECENT_CHILD_SCREEN
   // This method will unrealize and remove the most current child screen from the child
   // screen stack.
   void remove_most_recent_child_screen ();


   // SWITCH_TO_PREVIOUS_SCREEN
   // This method will display the previous procedure screen displayed, deallocate
   // and remove the current procedure screen, and note that the current screen is now
   // the previous screen.
   void switch_to_previous_screen ();


   // SWITCH_TO_SCREEN
   // This method will check the GUI state transition table to see if a state change is
   // necessary before displaying this screen. If a state transition is required, it will
   // send a message to procedure and leave. If no state transition is required AND the screen
   // to be displayed is different than the currently displayed screen, this routine will call
   // SWITCH_TO_NEW_APHERESIS_SCREEN to actually change screens,
   void switch_to_screen (GUI_SCREEN_ID new_screen, const char* allocation_parameter);



   // REMOVE_ALL_STATELESS_SCREENS
   // This method will unrealize all stateless screens and clear the stateless screen stack.
   void remove_all_stateless_screens ();


   // REMOVE_MOST_RECENT_STATELESS_SCREEN
   // This method will unrealize and remove the most current stateless screen from the stateless
   // screen stack.
   void remove_most_recent_stateless_screen ();


   // DISPLAY_STATELESS_SCREEN
   // This method will place an instance of a stateless screen on the stateless screen stack
   // then allocate the stateless screen.
   void display_stateless_screen (GUI_SCREEN_ID new_stateless, const char* allocation_parameter);


   // SWAP_STATELESS_SCREEN
   // This method will replace and display the current top-of-stack stateless screen with
   //  the screen passed as a parameter. If no stateless screen currently exists, this routine
   //  will simply display the screen passed as a parameter.
   void swap_stateless_screen (GUI_SCREEN_ID new_stateless, const char* allocation_parameter);


   // SWAP_CHILD_SCREEN
   // This method will replace and display the current top-of-stack child screen with
   //  the screen passed as a parameter. If no child screen currently exists, this routine
   //  will simply display the screen passed as a parameter.
   void swap_child_screen (GUI_SCREEN_ID new_child, const char* allocation_parameter);


   // SWAP_STATELESS_STACK_WITH_A_SCREEN
   // This method will clean the stateless stack of all screens, and then
   // display the new stateless screen passed as a parameter. If no stateless screen(s)
   // currently exist(s) on the stack, this routine will just display the
   // screen passed as a parameter.
   void swap_stateless_stack_with_a_screen (GUI_SCREEN_ID new_stateless, const char* allocation_parameter);


   // SWAP_CHILD_STACK_WITH_A_SCREEN
   // This method will clean the child stack of all screens, and then
   // display the new child screen passed as a parameter. If no child screen(s)
   // currently exist(s) on the stack, this routine will just display the
   // screen passed as a parameter.
   void swap_child_stack_with_a_screen (GUI_SCREEN_ID new_child, const char* allocation_parameter);

   void display_screen ();

   //
   //
   //   This method can be used to display / remove the "don't connect" icon
   //
   void update_dont_connect_donor_icon ();

   void process_vista_connection_message ();
   void update_vista_connected_icon ();

   Base_Window* getScreenInstancePointer (GUI_SCREEN_ID screenId = GUI_SCREEN_NULL_SCREEN);

   // Alas, this is sometimes necessary...
   int stateless_screen_stack_count () const { return stateless_screen_stack.get_stack_count(); }

   ////// MESSAGING DECLARATIONS AND SUPPORTING POINTERS //////

protected:

   Message<SCREEN_INVOKE_STRUCT>      Msg_Display_Screen;
   SystemStateChangeMsg               Msg_Notify_System_State_Change;
   ProcSubstateChangeMsg              Msg_Notify_Substate_Change;
   InitializeInternalStatesMsg        Msg_Initialize_States;
   RemoveAllChildScreensMsg           Msg_Remove_All_Child_Screens;
   RemoveMostRecentChildScreenMsg     Msg_Remove_Most_Recent_Child_Screen;
   SwitchToPreviousScreenMsg          Msg_Switch_To_Previous_Screen;
   RemoveAllStatelessScreensMsg       Msg_Remove_All_Stateless_Screens;
   RemoveMostRecentStatelessScreenMsg Msg_Remove_Most_Recent_Stateless_Screen;
   VistaConnectionStatusMsg           Msg_Vista_Connection_Update;
   SnapScreenshotMsg                  Msg_SnapScreenshot;

#if CPU==SIMNT
public:
   Base_Window* getCurrentChildScreen ();
   Base_Window* getCurrentChildScreen (GUI_SCREEN_ID screenId);
   void         setScreenshotName (const char* info);
   void         removeScreenshotName ();
   void         setScreenshotInfo (const char* name);
   void         removeScreenshotInfo ();
private:
   Base_Window* _currentWindow;
#endif // #if CPU==SIMNT

};  // END of Screen_Commutator Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 37e25335e2a9399feb02f3a0a9ef6e5b */

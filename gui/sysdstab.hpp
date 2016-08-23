/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   instances of the Sysdonor_State and Sysdonor_Action classes
                     which are included in the Screen_SYSDONOR class

*     File name:    sysdstab.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         These instances are included in the SYSDONOR class and represent the
         screen states and the corresponding actions associated with the screen
         states.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/sysdstab.hpp 1.12 2008/07/03 20:19:29Z dslausb Exp $
$Log: sysdstab.hpp $
Revision 1.12  2008/07/03 20:19:29Z  dslausb
Further standardization of string IDs for localization
Revision 1.11  2003/11/18 14:51:44Z  rm70006
IT 6223.  Beautify.
Revision 1.10  2003/01/31 15:58:25Z  rm70006
Fix problem with buttons be inactivated at incorrect times.  This whole process should be revisited and redesigned.
Revision 1.9  2003/01/06 18:32:33Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.8  2001/09/17 21:56:59  rm70006
IT 5311.
Modified state actions to put donor name in status bar when
downloaded.
Revision 1.7  2001/09/07 16:14:05  rm70006
IT 4766.
Changed some of the button transition states to remove old stuff.
Revision 1.6  2001/08/06 21:09:54  rm70006
IT 4766.
Removed reference to TEXT_DONOR_NAME.
Revision 1.5  2001/08/06 17:20:24  rm70006
IT 4766.
Interim change to test out putting donor name on status bar.
Revision 1.4  2001/08/02 17:35:34  rm70006
IT 4627, 4766.
Changed transitions to match new Vista flow
Revision 1.3  1998/10/16 16:14:13  tw09453
Resolve lost status line messages when in EOT (AP2) mode.
Revision 1.2  1998/09/16 00:04:58  tw09453
Adjust some of the action micro-ops for sysdonor display.
Revision 1.1  1998/08/08 00:05:00  tw09453
Initial revision

*/

// Single include define guardian
#ifndef SYSDSTAB_HPP
#define SYSDSTAB_HPP

// Number of seconds to wait after receiving donor info and going to DONORDL screen
#define TO_DONORDL_DELAY 2

// state_ID's are bumped (or set) at the end of a state / actions sequence
int state_ID = 1;

// action_ID's are bumped (or set) at the end of a state / actions sequence
int action_ID = 1;

// micro_inc's are bumped at the end of each micro-action
int micro_inc = 1;




////////////////System NOT Loaded -- Donor Data NOT Entered Phase////////////////

///// State  1 //////////////////////////////////////////////////////////////////
///// Action 1 //////////////////////////////////////////////////////////////////


state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                            // state id
                                   action_ID,                           // action id
                                   SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE, // Screen state
                                   SD_DONT_CARE,                        // Enable begin run
                                   SD_FALSE,                            // Display load sys
                                   SD_FALSE,                            // AP2 in control
                                   SD_FALSE));                          // AP2 data available


/// Action - Status Line
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,            // action id
                                     micro_inc,            // micro-action seq. id
                                     this,                 // SYSDONOR instance
                                     SD_STATUSLINE,        // target object
                                     SDOP_STATUSLINE_RESET // object method
                                                           // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Status Bar

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                      // action id
                                     micro_inc,                      // micro-action seq. id
                                     this,                           // SYSDONOR instance
                                     SD_STATUSBAR,                   // target object
                                     SDOP_STATUSBAR_RESET_ALL_PHASES // object method
                                                                     // ,                     // parameter
                                     ));  micro_inc++;

/// Action - End Run Button

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_ENDRUN_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_MAN_COL_UNCHK_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_DONORINFO_BTN,                     // target object
                                     SDOP_BUTTON_DONINFO_MAN_COL_UNCHK_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;



/// Action - System Load Button - SDOP_BUTTON_SYSLOAD_INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                    // action id
                                     micro_inc,                    // micro-action seq. id
                                     this,                         // SYSDONOR instance
                                     SD_LOADSYS_BTN,               // target object
                                     SDOP_BUTTON_SYSLOAD_INVISIBLE // object method
                                                                   // ,                     // parameter
                                     ));  micro_inc++;




/// Action - Continue Button - make it invisible

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONTINUE_BTN,          // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it visible

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                  // action id
                                     micro_inc,                  // micro-action seq. id
                                     this,                       // SYSDONOR instance
                                     SD_CONFIG_BTN,              // target object
                                     SDOP_BUTTON_CONFIG_ACTIVATE // object method
                                                                 // ,                     // parameter
                                     ));  micro_inc++;



///// End of State 1, Action 1 //////////////////////////////////////////////////




// Donor Not Loaded, Set Setup not complete, but T1 Tests completed.
///// State  2 //////////////////////////////////////////////////////////////////
///// Action 2 //////////////////////////////////////////////////////////////////

/// Beginning of State / Action sequence maintenance


state_ID  = 2;
action_ID = 2;
micro_inc = 1;



state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                            // state id
                                   action_ID,                           // action id
                                   SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE, // Screen state
                                   SD_DONT_CARE,                        // Enable begin run
                                   SD_TRUE,                             // Display load sys
                                   SD_FALSE,                            // AP2 in control
                                   SD_FALSE));                          // AP2 data available


/// Action - Status Line
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,            // action id
                                     micro_inc,            // micro-action seq. id
                                     this,                 // SYSDONOR instance
                                     SD_STATUSLINE,        // target object
                                     SDOP_STATUSLINE_RESET // object method
                                                           // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Status Bar

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                      // action id
                                     micro_inc,                      // micro-action seq. id
                                     this,                           // SYSDONOR instance
                                     SD_STATUSBAR,                   // target object
                                     SDOP_STATUSBAR_RESET_ALL_PHASES // object method
                                                                     // ,                     // parameter
                                     ));  micro_inc++;

/// Action - End Run Button

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_ENDRUN_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_MAN_COL_UNCHK_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,             // action id
                                     micro_inc,             // micro-action seq. id
                                     this,                  // SYSDONOR instance
                                     SD_DONORINFO_BTN,      // target object
                                     SDOP_BUTTON_SET_ACTIVE // object method
                                                            // ,                     // parameter
                                     ));  micro_inc++;


/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_MAN_COL_UNCHK_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_DONORINFO_BTN,                     // target object
                                     SDOP_BUTTON_DONINFO_MAN_COL_UNCHK_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;



/// Action - System Load Button - SDOP_BUTTON_SYSLOAD_ALL_COL_UNCHK_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_LOADSYS_BTN,                       // target object
                                     SDOP_BUTTON_SYSLOAD_ALL_COL_UNCHK_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;

/// Action - System Load Button - SDOP_BUTTON_SET_ACTIVE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,             // action id
                                     micro_inc,             // micro-action seq. id
                                     this,                  // SYSDONOR instance
                                     SD_LOADSYS_BTN,        // target object
                                     SDOP_BUTTON_SET_ACTIVE // object method
                                                            // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Continue Button - make it invisible

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONTINUE_BTN,          // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it visible

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                  // action id
                                     micro_inc,                  // micro-action seq. id
                                     this,                       // SYSDONOR instance
                                     SD_CONFIG_BTN,              // target object
                                     SDOP_BUTTON_CONFIG_ACTIVATE // object method
                                                                 // ,                     // parameter
                                     ));  micro_inc++;

/// Action - Configuration Button - make it active
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,             // action id
                                     micro_inc,             // micro-action seq. id
                                     this,                  // SYSDONOR instance
                                     SD_CONFIG_BTN,         // target object
                                     SDOP_BUTTON_SET_ACTIVE // object method
                                                            // ,                     // parameter
                                     ));  micro_inc++;


///// End of State 2, Action 2 //////////////////////////////////////////////////





// Donor Not Loaded, Set Setup not complete, Vista In Control, No Donor Data, T1 Tests not Complete
///// State  3 //////////////////////////////////////////////////////////////////
///// Action 3 //////////////////////////////////////////////////////////////////

/// Beginning of State / Action sequence maintenance


state_ID  = 3;
action_ID = 3;
micro_inc = 1;



state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                            // state id
                                   action_ID,                           // action id
                                   SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE, // Screen state
                                   SD_DONT_CARE,                        // Enable begin run
                                   SD_FALSE,                            // Display load sys
                                   SD_TRUE,                             // AP2 in control
                                   SD_FALSE));                          // AP2 data available


/// Action - Status Line
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                           // action id
                                     micro_inc,                           // micro-action seq. id
                                     this,                                // SYSDONOR instance
                                     SD_STATUSLINE,                       // target object
                                     SDOP_STATUSLINE_DEACTIVATE_ATTENTION // object method
                                                                          // ,                     // parameter
                                     ));  micro_inc++;




/// Action - Status Line    - Notify that we're waiting for the donor download
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                 // action id
                                     micro_inc,                 // micro-action seq. id
                                     this,                      // SYSDONOR instance
                                     SD_STATUSLINE,             // target object
                                     SDOP_STATUSLINE_SET_TEXT,  // object method
                                     textStatAwaitingAp2Xmit    // parameter
                                     ));  micro_inc++;



/// Action - Status Bar

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                      // action id
                                     micro_inc,                      // micro-action seq. id
                                     this,                           // SYSDONOR instance
                                     SD_STATUSBAR,                   // target object
                                     SDOP_STATUSBAR_RESET_ALL_PHASES // object method
                                                                     // ,                     // parameter
                                     ));  micro_inc++;

/// Action - End Run Button

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_ENDRUN_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_AP2_GRY_UNCHK_INACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                              // action id
                                     micro_inc,                              // micro-action seq. id
                                     this,                                   // SYSDONOR instance
                                     SD_DONORINFO_BTN,                       // target object
                                     SDOP_BUTTON_DONINFO_AP2_GRY_UNCHK_INACT // object method
                                                                             // ,                     // parameter
                                     ));  micro_inc++;



/// Action - System Load Button - SDOP_BUTTON_SYSLOAD_INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                    // action id
                                     micro_inc,                    // micro-action seq. id
                                     this,                         // SYSDONOR instance
                                     SD_LOADSYS_BTN,               // target object
                                     SDOP_BUTTON_SYSLOAD_INVISIBLE // object method
                                                                   // ,                     // parameter
                                     ));  micro_inc++;




/// Action - Continue Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONTINUE_BTN,          // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONFIG_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;




///// End of State 3, Action 3 //////////////////////////////////////////////////





// Donor Not Loaded, Set Setup not complete, Vista In Control, No Donor Data, T1 Tests are Complete
///// State  4 //////////////////////////////////////////////////////////////////
///// Action 4 //////////////////////////////////////////////////////////////////

/// Beginning of State / Action sequence maintenance


state_ID  = 4;
action_ID = 4;
micro_inc = 1;



state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                            // state id
                                   action_ID,                           // action id
                                   SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE, // Screen state
                                   SD_DONT_CARE,                        // Enable begin run
                                   SD_TRUE,                             // Display load sys
                                   SD_TRUE,                             // AP2 in control
                                   SD_FALSE));                          // AP2 data available


/// Action - Status Line - be sure to reset the attention status line
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                           // action id
                                     micro_inc,                           // micro-action seq. id
                                     this,                                // SYSDONOR instance
                                     SD_STATUSLINE,                       // target object
                                     SDOP_STATUSLINE_DEACTIVATE_ATTENTION // object method
                                                                          // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Status Line    - Notify that we're waiting for the donor download
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                 // action id
                                     micro_inc,                 // micro-action seq. id
                                     this,                      // SYSDONOR instance
                                     SD_STATUSLINE,             // target object
                                     SDOP_STATUSLINE_SET_TEXT,  // object method
                                     textStatAwaitingAp2Xmit    // parameter
                                     ));  micro_inc++;


/// Action - Status Bar

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                      // action id
                                     micro_inc,                      // micro-action seq. id
                                     this,                           // SYSDONOR instance
                                     SD_STATUSBAR,                   // target object
                                     SDOP_STATUSBAR_RESET_ALL_PHASES // object method
                                                                     // ,                     // parameter
                                     ));  micro_inc++;

/// Action - End Run Button

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_ENDRUN_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_AP2_GRY_UNCHK_INACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                              // action id
                                     micro_inc,                              // micro-action seq. id
                                     this,                                   // SYSDONOR instance
                                     SD_DONORINFO_BTN,                       // target object
                                     SDOP_BUTTON_DONINFO_AP2_GRY_UNCHK_INACT // object method
                                                                             // ,                     // parameter
                                     ));  micro_inc++;

/// Action - Configuration Button - make it active
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,               // action id
                                     micro_inc,               // micro-action seq. id
                                     this,                    // SYSDONOR instance
                                     SD_DONORINFO_BTN,        // target object
                                     SDOP_BUTTON_SET_INACTIVE // object method
                                                              // ,                     // parameter
                                     ));  micro_inc++;


/// Action - System Load Button - SDOP_BUTTON_SYSLOAD_ALL_COL_UNCHK_ACT
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_LOADSYS_BTN,                       // target object
                                     SDOP_BUTTON_SYSLOAD_ALL_COL_UNCHK_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;

/// Action - Configuration Button - make it active
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,             // action id
                                     micro_inc,             // micro-action seq. id
                                     this,                  // SYSDONOR instance
                                     SD_LOADSYS_BTN,        // target object
                                     SDOP_BUTTON_SET_ACTIVE // object method
                                                            // ,                     // parameter
                                     ));  micro_inc++;




/// Action - Continue Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONTINUE_BTN,          // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONFIG_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;




///// End of State 4, Action 4 //////////////////////////////////////////////////






///// State  5 //////////////////////////////////////////////////////////////////
///// Action 5 //////////////////////////////////////////////////////////////////

/// This state is when AP2 is in control, data is available, and the donor has not been
/// confirmed.

/// Beginning of State / Action sequence maintenance


state_ID  = 5;
action_ID = 5;
micro_inc = 1;



state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                            // state id
                                   action_ID,                           // action id
                                   SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE, // Screen state
                                   SD_DONT_CARE,                        // Enable begin run
                                   SD_FALSE,                            // Display load sys
                                   SD_TRUE,                             // AP2 in control
                                   SD_TRUE));                           // AP2 data available



/// Action - Status Line
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                           // action id
                                     micro_inc,                           // micro-action seq. id
                                     this,                                // SYSDONOR instance
                                     SD_STATUSLINE,                       // target object
                                     SDOP_STATUSLINE_DEACTIVATE_ATTENTION // object method
                                                                          // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Status Line    - Display the Donor's name in the status bar.
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                        // action id
                                     micro_inc,                        // micro-action seq. id
                                     this,                             // SYSDONOR instance
                                     SD_STATUSLINE,                    // target object
                                     SDOP_STATUSLINE_LOAD_DONOR_NAME,  // object method
                                     textEmptyFormatDonDlDonorNameInit // parameter
                                     ));  micro_inc++;



/// Action - Status Bar

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                      // action id
                                     micro_inc,                      // micro-action seq. id
                                     this,                           // SYSDONOR instance
                                     SD_STATUSBAR,                   // target object
                                     SDOP_STATUSBAR_RESET_ALL_PHASES // object method
                                                                     // ,                     // parameter
                                     ));  micro_inc++;

/// Action - End Run Button

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_ENDRUN_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_AP2_COL_UNCHK_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_DONORINFO_BTN,                     // target object
                                     SDOP_BUTTON_DONINFO_AP2_COL_UNCHK_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;



/// Action - System Load Button - INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_LOADSYS_BTN,           // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;




/// Action - Continue Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONTINUE_BTN,          // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONFIG_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;




///// End of State 5, Action 5 //////////////////////////////////////////////////






///// State  6 //////////////////////////////////////////////////////////////////
///// Action 6 //////////////////////////////////////////////////////////////////

/// Beginning of State / Action sequence maintenance


state_ID  = 6;
action_ID = 6;
micro_inc = 1;



state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                            // state id
                                   action_ID,                           // action id
                                   SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE, // Screen state
                                   SD_DONT_CARE,                        // Enable begin run
                                   SD_TRUE,                             // Display load sys
                                   SD_TRUE,                             // AP2 in control
                                   SD_TRUE));                           // AP2 data available




/// Action - Status Line
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                        // action id
                                     micro_inc,                        // micro-action seq. id
                                     this,                             // SYSDONOR instance
                                     SD_STATUSLINE,                    // target object
                                     SDOP_STATUSLINE_RESET,            // object method
                                     textEmptyFormatDonDlDonorNameInit // parameter
                                     ));  micro_inc++;



/// Action - Status Line    - Display the Donor's name in the status bar.
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                        // action id
                                     micro_inc,                        // micro-action seq. id
                                     this,                             // SYSDONOR instance
                                     SD_STATUSLINE,                    // target object
                                     SDOP_STATUSLINE_LOAD_DONOR_NAME,  // object method
                                     textEmptyFormatDonDlDonorNameInit // parameter
                                     ));  micro_inc++;



/// Action - Status Bar

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                      // action id
                                     micro_inc,                      // micro-action seq. id
                                     this,                           // SYSDONOR instance
                                     SD_STATUSBAR,                   // target object
                                     SDOP_STATUSBAR_RESET_ALL_PHASES // object method
                                                                     // ,                     // parameter
                                     ));  micro_inc++;

/// Action - End Run Button

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_ENDRUN_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_AP2_COL_UNCHK_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_DONORINFO_BTN,                     // target object
                                     SDOP_BUTTON_DONINFO_AP2_COL_UNCHK_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;



/// Action - System Load Button - SDOP_BUTTON_SYSLOAD_ALL_COL_UNCHK_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_LOADSYS_BTN,                       // target object
                                     SDOP_BUTTON_SYSLOAD_ALL_COL_UNCHK_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;




/// Action - Continue Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONTINUE_BTN,          // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONFIG_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;




///// End of State 6, Action 6 //////////////////////////////////////////////////






////////////////System IS Loaded -- Donor Data NOT Entered Phase/////////////////


///// State  7 //////////////////////////////////////////////////////////////////
///// Action 7 //////////////////////////////////////////////////////////////////

/// Beginning of State / Action sequence maintenance


state_ID  = 7;
action_ID = 7;
micro_inc = 1;



state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                         // state id
                                   action_ID,                        // action id
                                   SETUP_COMPLETE_DONOR_NOTCOMPLETE, // Screen state
                                   SD_DONT_CARE,                     // Enable begin run
                                   SD_DONT_CARE,                     // Display load sys
                                   SD_FALSE,                         // AP2 in control
                                   SD_FALSE));                       // AP2 data available


/// Action - Status Line
/* Placeholder
state_management.add_action_entry
   ((GUI_Action *) new Sysdonor_Action( action_ID,              // action id
                                        micro_inc,              // micro-action seq. id
                                        this,                   // SYSDONOR instance
                                        SD_STATUSLINE,          // target object
                                        SDOP_STATUSLINE_???,    // object method
                                        ???                     // parameter
                                       ));  micro_inc++;

*/

/// Action - Status Bar - set system load as complete

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                         // action id
                                     micro_inc,                         // micro-action seq. id
                                     this,                              // SYSDONOR instance
                                     SD_STATUSBAR,                      // target object
                                     SDOP_STATUSBAR_SET_PHASE_COMPLETE, // object method
                                     STATUS_BAR_LOAD
                                     ));  micro_inc++;



/// Action - Status Bar - set donor info as active

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                       // action id
                                     micro_inc,                       // micro-action seq. id
                                     this,                            // SYSDONOR instance
                                     SD_STATUSBAR,                    // target object
                                     SDOP_STATUSBAR_SET_PHASE_ACTIVE, // object method
                                     STATUS_BAR_INFO
                                     ));  micro_inc++;

/// Action - End Run Button

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_ENDRUN_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_MAN_COL_UNCHK_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_DONORINFO_BTN,                     // target object
                                     SDOP_BUTTON_DONINFO_MAN_COL_UNCHK_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;



/// Action - System Load Button - SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                              // action id
                                     micro_inc,                              // micro-action seq. id
                                     this,                                   // SYSDONOR instance
                                     SD_LOADSYS_BTN,                         // target object
                                     SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT // object method
                                                                             // ,                     // parameter
                                     ));  micro_inc++;




/// Action - Continue Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONTINUE_BTN,          // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONFIG_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



///// End of State 7, Action 7 //////////////////////////////////////////////////






///// State  8 //////////////////////////////////////////////////////////////////
///// Action 8 //////////////////////////////////////////////////////////////////

/// Beginning of State / Action sequence maintenance


state_ID  = 8;
action_ID = 8;
micro_inc = 1;



state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                         // state id
                                   action_ID,                        // action id
                                   SETUP_COMPLETE_DONOR_NOTCOMPLETE, // Screen state
                                   SD_DONT_CARE,                     // Enable begin run
                                   SD_DONT_CARE,                     // Display load sys
                                   SD_TRUE,                          // AP2 in control
                                   SD_FALSE));                       // AP2 data available



/// Action - Status Line    - Notify that we're waiting for the donor download
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                 // action id
                                     micro_inc,                 // micro-action seq. id
                                     this,                      // SYSDONOR instance
                                     SD_STATUSLINE,             // target object
                                     SDOP_STATUSLINE_SET_TEXT,  // object method
                                     textStatAwaitingAp2Xmit    // parameter
                                     ));  micro_inc++;



/// Action - Status Bar - set system load as complete

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                         // action id
                                     micro_inc,                         // micro-action seq. id
                                     this,                              // SYSDONOR instance
                                     SD_STATUSBAR,                      // target object
                                     SDOP_STATUSBAR_SET_PHASE_COMPLETE, // object method
                                     STATUS_BAR_LOAD
                                     ));  micro_inc++;



/// Action - Status Bar - set donor info as active

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                       // action id
                                     micro_inc,                       // micro-action seq. id
                                     this,                            // SYSDONOR instance
                                     SD_STATUSBAR,                    // target object
                                     SDOP_STATUSBAR_SET_PHASE_ACTIVE, // object method
                                     STATUS_BAR_INFO
                                     ));  micro_inc++;

/// Action - End Run Button

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_ENDRUN_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_AP2_GRY_UNCHK_INACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                              // action id
                                     micro_inc,                              // micro-action seq. id
                                     this,                                   // SYSDONOR instance
                                     SD_DONORINFO_BTN,                       // target object
                                     SDOP_BUTTON_DONINFO_AP2_GRY_UNCHK_INACT // object method
                                                                             // ,                     // parameter
                                     ));  micro_inc++;



/// Action - System Load Button - SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                              // action id
                                     micro_inc,                              // micro-action seq. id
                                     this,                                   // SYSDONOR instance
                                     SD_LOADSYS_BTN,                         // target object
                                     SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT // object method
                                                                             // ,                     // parameter
                                     ));  micro_inc++;




/// Action - Continue Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONTINUE_BTN,          // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONFIG_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



///// End of State 8, Action 8 //////////////////////////////////////////////////






///// State  9 //////////////////////////////////////////////////////////////////
///// Action 9 //////////////////////////////////////////////////////////////////

/// Beginning of State / Action sequence maintenance


state_ID  = 9;
action_ID = 9;
micro_inc = 1;



state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                         // state id
                                   action_ID,                        // action id
                                   SETUP_COMPLETE_DONOR_NOTCOMPLETE, // Screen state
                                   SD_DONT_CARE,                     // Enable begin run
                                   SD_DONT_CARE,                     // Display load sys
                                   SD_TRUE,                          // AP2 in control
                                   SD_TRUE));                        // AP2 data available



/// Action - Status Line    - Display the Donor Name in the status bar
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                        // action id
                                     micro_inc,                        // micro-action seq. id
                                     this,                             // SYSDONOR instance
                                     SD_STATUSLINE,                    // target object
                                     SDOP_STATUSLINE_LOAD_DONOR_NAME,  // object method
                                     textEmptyFormatDonDlDonorNameInit // parameter
                                     ));  micro_inc++;



/// Action - Status Bar - set system load as complete

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                         // action id
                                     micro_inc,                         // micro-action seq. id
                                     this,                              // SYSDONOR instance
                                     SD_STATUSBAR,                      // target object
                                     SDOP_STATUSBAR_SET_PHASE_COMPLETE, // object method
                                     STATUS_BAR_LOAD
                                     ));  micro_inc++;



/// Action - Status Bar - set donor info as active

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                       // action id
                                     micro_inc,                       // micro-action seq. id
                                     this,                            // SYSDONOR instance
                                     SD_STATUSBAR,                    // target object
                                     SDOP_STATUSBAR_SET_PHASE_ACTIVE, // object method
                                     STATUS_BAR_INFO
                                     ));  micro_inc++;

/// Action - End Run Button

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_ENDRUN_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_AP2_COL_UNCHK_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_DONORINFO_BTN,                     // target object
                                     SDOP_BUTTON_DONINFO_AP2_COL_UNCHK_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;



/// Action - System Load Button - SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                              // action id
                                     micro_inc,                              // micro-action seq. id
                                     this,                                   // SYSDONOR instance
                                     SD_LOADSYS_BTN,                         // target object
                                     SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT // object method
                                                                             // ,                     // parameter
                                     ));  micro_inc++;




/// Action - Continue Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONTINUE_BTN,          // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONFIG_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



///// End of State 9, Action 9 //////////////////////////////////////////////////



///// Note: State  10 is intentionally missing (documentation consistency)





////////////////System NOT Loaded -- Donor Data IS Entered  - Phase/////////////////



///// State  11 //////////////////////////////////////////////////////////////////
///// Action 11 //////////////////////////////////////////////////////////////////

/// Beginning of State / Action sequence maintenance


state_ID  = 11;
action_ID = 11;
micro_inc = 1;



state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                         // state id
                                   action_ID,                        // action id
                                   SETUP_NOTCOMPLETE_DONOR_COMPLETE, // Screen state
                                   SD_DONT_CARE,                     // Enable begin run
                                   SD_FALSE,                         // Display load sys
                                   SD_TRUE,                          // AP2 in control
                                   SD_DONT_CARE));                   // AP2 data available



/// Action - Status Line    - Display the Donor Name in the status bar
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                        // action id
                                     micro_inc,                        // micro-action seq. id
                                     this,                             // SYSDONOR instance
                                     SD_STATUSLINE,                    // target object
                                     SDOP_STATUSLINE_LOAD_DONOR_NAME,  // object method
                                     textEmptyFormatDonDlDonorNameInit // parameter
                                     ));  micro_inc++;



/// Action - Status Bar - set system info as complete

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                         // action id
                                     micro_inc,                         // micro-action seq. id
                                     this,                              // SYSDONOR instance
                                     SD_STATUSBAR,                      // target object
                                     SDOP_STATUSBAR_SET_PHASE_COMPLETE, // object method
                                     STATUS_BAR_INFO
                                     ));  micro_inc++;



/// Action - Status Bar - set load donor as active

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                       // action id
                                     micro_inc,                       // micro-action seq. id
                                     this,                            // SYSDONOR instance
                                     SD_STATUSBAR,                    // target object
                                     SDOP_STATUSBAR_SET_PHASE_ACTIVE, // object method
                                     STATUS_BAR_LOAD
                                     ));  micro_inc++;

/// Action - End Run Button

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_ENDRUN_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_AP2_COL_CHKED_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_DONORINFO_BTN,                     // target object
                                     SDOP_BUTTON_DONINFO_AP2_COL_CHKED_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;




/// Action - System Load Button - INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_LOADSYS_BTN,           // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;




/// Action - Continue Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONTINUE_BTN,          // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONFIG_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;




///// End of State 11, Action 11 //////////////////////////////////////////////////





///// Note: State  12 is intentionally missing (documentation consistency)





///// State  13 //////////////////////////////////////////////////////////////////
///// Action 13 //////////////////////////////////////////////////////////////////

/// Beginning of State / Action sequence maintenance


state_ID  = 13;
action_ID = 13;
micro_inc = 1;



state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                         // state id
                                   action_ID,                        // action id
                                   SETUP_NOTCOMPLETE_DONOR_COMPLETE, // Screen state
                                   SD_DONT_CARE,                     // Enable begin run
                                   SD_FALSE,                         // Display load sys
                                   SD_FALSE,                         // AP2 in control
                                   SD_DONT_CARE));                   // AP2 data available



/// Action - Status Line
/* placeholder
state_management.add_action_entry
   ((GUI_Action *) new Sysdonor_Action( action_ID,              // action id
                                        micro_inc,              // micro-action seq. id
                                        this,                   // SYSDONOR instance
                                        SD_STATUSLINE,          // target object
                                        SDOP_STATUSLINE_???,    // object method
                                        ???                     // parameter
                                       ));  micro_inc++;
*/



/// Action - Status Bar - set system info as complete

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                         // action id
                                     micro_inc,                         // micro-action seq. id
                                     this,                              // SYSDONOR instance
                                     SD_STATUSBAR,                      // target object
                                     SDOP_STATUSBAR_SET_PHASE_COMPLETE, // object method
                                     STATUS_BAR_INFO
                                     ));  micro_inc++;



/// Action - Status Bar - set load donor as active

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                       // action id
                                     micro_inc,                       // micro-action seq. id
                                     this,                            // SYSDONOR instance
                                     SD_STATUSBAR,                    // target object
                                     SDOP_STATUSBAR_SET_PHASE_ACTIVE, // object method
                                     STATUS_BAR_LOAD
                                     ));  micro_inc++;

/// Action - End Run Button

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_ENDRUN_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_MAN_COL_CHKED_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_DONORINFO_BTN,                     // target object
                                     SDOP_BUTTON_DONINFO_MAN_COL_CHKED_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;




/// Action - System Load Button - INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_LOADSYS_BTN,           // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;




/// Action - Continue Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONTINUE_BTN,          // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONFIG_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;




///// End of State 13, Action 13 //////////////////////////////////////////////////





///// Note: State  14 is intentionally missing (documentation consistency)



// This state is where the Donor info has been confirmed and the procedure has been
// selected.
///// State  15 //////////////////////////////////////////////////////////////////
///// Action 15 //////////////////////////////////////////////////////////////////

/// Beginning of State / Action sequence maintenance


state_ID  = 15;
action_ID = 15;
micro_inc = 1;



state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                         // state id
                                   action_ID,                        // action id
                                   SETUP_NOTCOMPLETE_DONOR_COMPLETE, // Screen state
                                   SD_DONT_CARE,                     // Enable begin run
                                   SD_TRUE,                          // Display load sys
                                   SD_TRUE,                          // AP2 in control
                                   SD_DONT_CARE));                   // AP2 data available



/// Action - Status Line    - Display the Donor Name in the status bar
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                        // action id
                                     micro_inc,                        // micro-action seq. id
                                     this,                             // SYSDONOR instance
                                     SD_STATUSLINE,                    // target object
                                     SDOP_STATUSLINE_LOAD_DONOR_NAME,  // object method
                                     textEmptyFormatDonDlDonorNameInit // parameter
                                     ));  micro_inc++;



/// Action - Status Bar - set system info as complete

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                         // action id
                                     micro_inc,                         // micro-action seq. id
                                     this,                              // SYSDONOR instance
                                     SD_STATUSBAR,                      // target object
                                     SDOP_STATUSBAR_SET_PHASE_COMPLETE, // object method
                                     STATUS_BAR_INFO
                                     ));  micro_inc++;



/// Action - Status Bar - set load donor as active

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                       // action id
                                     micro_inc,                       // micro-action seq. id
                                     this,                            // SYSDONOR instance
                                     SD_STATUSBAR,                    // target object
                                     SDOP_STATUSBAR_SET_PHASE_ACTIVE, // object method
                                     STATUS_BAR_LOAD
                                     ));  micro_inc++;

/// Action - End Run Button

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_ENDRUN_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_AP2_COL_CHKED_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_DONORINFO_BTN,                     // target object
                                     SDOP_BUTTON_DONINFO_AP2_COL_CHKED_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;




/// Action - System Load Button - SDOP_BUTTON_SYSLOAD_ALL_COL_UNCHK_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_LOADSYS_BTN,                       // target object
                                     SDOP_BUTTON_SYSLOAD_ALL_COL_UNCHK_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;




/// Action - Continue Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONTINUE_BTN,          // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONFIG_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;




///// End of State 15, Action 15 //////////////////////////////////////////////////





///// Note: State  16 is intentionally missing (documentation consistency)




///// State  17 //////////////////////////////////////////////////////////////////
///// Action 17 //////////////////////////////////////////////////////////////////

/// Beginning of State / Action sequence maintenance


state_ID  = 17;
action_ID = 17;
micro_inc = 1;



state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                         // state id
                                   action_ID,                        // action id
                                   SETUP_NOTCOMPLETE_DONOR_COMPLETE, // Screen state
                                   SD_DONT_CARE,                     // Enable begin run
                                   SD_TRUE,                          // Display load sys
                                   SD_FALSE,                         // AP2 in control
                                   SD_DONT_CARE));                   // AP2 data available



/// Action - Status Line
/* placeholder
state_management.add_action_entry
   ((GUI_Action *) new Sysdonor_Action( action_ID,              // action id
                                        micro_inc,              // micro-action seq. id
                                        this,                   // SYSDONOR instance
                                        SD_STATUSLINE,          // target object
                                        SDOP_STATUSLINE_???,    // object method
                                        ???                     // parameter
                                       ));  micro_inc++;
*/



/// Action - Status Bar - set system info as complete

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                         // action id
                                     micro_inc,                         // micro-action seq. id
                                     this,                              // SYSDONOR instance
                                     SD_STATUSBAR,                      // target object
                                     SDOP_STATUSBAR_SET_PHASE_COMPLETE, // object method
                                     STATUS_BAR_INFO
                                     ));  micro_inc++;



/// Action - Status Bar - set load donor as active

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                       // action id
                                     micro_inc,                       // micro-action seq. id
                                     this,                            // SYSDONOR instance
                                     SD_STATUSBAR,                    // target object
                                     SDOP_STATUSBAR_SET_PHASE_ACTIVE, // object method
                                     STATUS_BAR_LOAD
                                     ));  micro_inc++;

/// Action - End Run Button

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_ENDRUN_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_MAN_COL_CHKED_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_DONORINFO_BTN,                     // target object
                                     SDOP_BUTTON_DONINFO_MAN_COL_CHKED_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;




/// Action - System Load Button - SDOP_BUTTON_SYSLOAD_ALL_COL_UNCHK_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_LOADSYS_BTN,                       // target object
                                     SDOP_BUTTON_SYSLOAD_ALL_COL_UNCHK_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;




/// Action - Continue Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONTINUE_BTN,          // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONFIG_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;




///// End of State 17, Action 17 //////////////////////////////////////////////////





///// Note: State  18 is intentionally missing (documentation consistency)





////////////////System IS Loaded -- Donor Data IS Entered  - Phase/////////////////



///// State  19 //////////////////////////////////////////////////////////////////
///// Action 19 //////////////////////////////////////////////////////////////////

/// Beginning of State / Action sequence maintenance


state_ID  = 19;
action_ID = 19;
micro_inc = 1;



state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                      // state id
                                   action_ID,                     // action id
                                   SETUP_COMPLETE_DONOR_COMPLETE, // Screen state
                                   SD_TRUE,                       // Enable begin run
                                   SD_DONT_CARE,                  // Display load sys
                                   SD_FALSE,                      // AP2 in control
                                   SD_DONT_CARE));                // AP2 data available



/// Action - Status Line
/* placeholder
state_management.add_action_entry
   ((GUI_Action *) new Sysdonor_Action( action_ID,              // action id
                                        micro_inc,              // micro-action seq. id
                                        this,                   // SYSDONOR instance
                                        SD_STATUSLINE,          // target object
                                        SDOP_STATUSLINE_???,    // object method
                                        ???                     // parameter
                                       ));  micro_inc++;
*/



/// Action - Status Bar - set system info as complete

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                         // action id
                                     micro_inc,                         // micro-action seq. id
                                     this,                              // SYSDONOR instance
                                     SD_STATUSBAR,                      // target object
                                     SDOP_STATUSBAR_SET_PHASE_COMPLETE, // object method
                                     STATUS_BAR_INFO
                                     ));  micro_inc++;



/// Action - Status Bar - set load donor as complete

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                         // action id
                                     micro_inc,                         // micro-action seq. id
                                     this,                              // SYSDONOR instance
                                     SD_STATUSBAR,                      // target object
                                     SDOP_STATUSBAR_SET_PHASE_COMPLETE, // object method
                                     STATUS_BAR_LOAD
                                     ));  micro_inc++;


/// Action - Status Bar - set prepare as active

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                       // action id
                                     micro_inc,                       // micro-action seq. id
                                     this,                            // SYSDONOR instance
                                     SD_STATUSBAR,                    // target object
                                     SDOP_STATUSBAR_SET_PHASE_ACTIVE, // object method
                                     STATUS_BAR_PREPARE
                                     ));  micro_inc++;



/// Action - End Run Button - visible

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                  // action id
                                     micro_inc,                  // micro-action seq. id
                                     this,                       // SYSDONOR instance
                                     SD_ENDRUN_BTN,              // target object
                                     SDOP_BUTTON_ENDRUN_ACTIVATE // object method
                                                                 // ,                     // parameter
                                     ));  micro_inc++;




/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_MAN_COL_CHKED_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_DONORINFO_BTN,                     // target object
                                     SDOP_BUTTON_DONINFO_MAN_COL_CHKED_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;




/// Action - System Load Button - SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                              // action id
                                     micro_inc,                              // micro-action seq. id
                                     this,                                   // SYSDONOR instance
                                     SD_LOADSYS_BTN,                         // target object
                                     SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT // object method
                                                                             // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Continue Button - set text to start procedure

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                    // action id
                                     micro_inc,                    // micro-action seq. id
                                     this,                         // SYSDONOR instance
                                     SD_CONTINUE_BTN,              // target object
                                     SDOP_BUTTON_BEGINRUN_ACTIVATE // object method
                                                                   // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONFIG_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



///// End of State 19, Action 19 //////////////////////////////////////////////////





///// State  20 //////////////////////////////////////////////////////////////////
///// Action 20 //////////////////////////////////////////////////////////////////

/// Beginning of State / Action sequence maintenance


state_ID  = 20;
action_ID = 20;
micro_inc = 1;



state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                      // state id
                                   action_ID,                     // action id
                                   SETUP_COMPLETE_DONOR_COMPLETE, // Screen state
                                   SD_TRUE,                       // Enable begin run
                                   SD_DONT_CARE,                  // Display load sys
                                   SD_TRUE,                       // AP2 in control
                                   SD_DONT_CARE));                // AP2 data available



/// Action - Status Line    - Display the Donor Name in the status bar
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                        // action id
                                     micro_inc,                        // micro-action seq. id
                                     this,                             // SYSDONOR instance
                                     SD_STATUSLINE,                    // target object
                                     SDOP_STATUSLINE_LOAD_DONOR_NAME,  // object method
                                     textEmptyFormatDonDlDonorNameInit // parameter
                                     ));  micro_inc++;



/// Action - Status Bar - set system info as complete

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                         // action id
                                     micro_inc,                         // micro-action seq. id
                                     this,                              // SYSDONOR instance
                                     SD_STATUSBAR,                      // target object
                                     SDOP_STATUSBAR_SET_PHASE_COMPLETE, // object method
                                     STATUS_BAR_INFO
                                     ));  micro_inc++;



/// Action - Status Bar - set load donor as complete

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                         // action id
                                     micro_inc,                         // micro-action seq. id
                                     this,                              // SYSDONOR instance
                                     SD_STATUSBAR,                      // target object
                                     SDOP_STATUSBAR_SET_PHASE_COMPLETE, // object method
                                     STATUS_BAR_LOAD
                                     ));  micro_inc++;


/// Action - Status Bar - set prepare as active

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                       // action id
                                     micro_inc,                       // micro-action seq. id
                                     this,                            // SYSDONOR instance
                                     SD_STATUSBAR,                    // target object
                                     SDOP_STATUSBAR_SET_PHASE_ACTIVE, // object method
                                     STATUS_BAR_PREPARE
                                     ));  micro_inc++;



/// Action - End Run Button - visible

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                  // action id
                                     micro_inc,                  // micro-action seq. id
                                     this,                       // SYSDONOR instance
                                     SD_ENDRUN_BTN,              // target object
                                     SDOP_BUTTON_ENDRUN_ACTIVATE // object method
                                                                 // ,                     // parameter
                                     ));  micro_inc++;





/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_AP2_COL_CHKED_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_DONORINFO_BTN,                     // target object
                                     SDOP_BUTTON_DONINFO_AP2_COL_CHKED_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;




/// Action - System Load Button - SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                              // action id
                                     micro_inc,                              // micro-action seq. id
                                     this,                                   // SYSDONOR instance
                                     SD_LOADSYS_BTN,                         // target object
                                     SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT // object method
                                                                             // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Continue Button - set text to start procedure

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                    // action id
                                     micro_inc,                    // micro-action seq. id
                                     this,                         // SYSDONOR instance
                                     SD_CONTINUE_BTN,              // target object
                                     SDOP_BUTTON_BEGINRUN_ACTIVATE // object method
                                                                   // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONFIG_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



///// End of State 20, Action 20 //////////////////////////////////////////////////







///// State  21 //////////////////////////////////////////////////////////////////
///// Action 21 //////////////////////////////////////////////////////////////////

/// Beginning of State / Action sequence maintenance


state_ID  = 21;
action_ID = 21;
micro_inc = 1;



state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                      // state id
                                   action_ID,                     // action id
                                   SETUP_COMPLETE_DONOR_COMPLETE, // Screen state
                                   SD_FALSE,                      // Enable begin run
                                   SD_DONT_CARE,                  // Display load sys
                                   SD_FALSE,                      // AP2 in control
                                   SD_DONT_CARE));                // AP2 data available



/// Action - Status Line
/* placeholder
state_management.add_action_entry
   ((GUI_Action *) new Sysdonor_Action( action_ID,              // action id
                                        micro_inc,              // micro-action seq. id
                                        this,                   // SYSDONOR instance
                                        SD_STATUSLINE,          // target object
                                        SDOP_STATUSLINE_???,    // object method
                                        ???                     // parameter
                                       ));  micro_inc++;
*/



/// Action - Status Bar - set system info as complete

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                         // action id
                                     micro_inc,                         // micro-action seq. id
                                     this,                              // SYSDONOR instance
                                     SD_STATUSBAR,                      // target object
                                     SDOP_STATUSBAR_SET_PHASE_COMPLETE, // object method
                                     STATUS_BAR_INFO
                                     ));  micro_inc++;



/// Action - Status Bar - set load donor as complete

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                         // action id
                                     micro_inc,                         // micro-action seq. id
                                     this,                              // SYSDONOR instance
                                     SD_STATUSBAR,                      // target object
                                     SDOP_STATUSBAR_SET_PHASE_COMPLETE, // object method
                                     STATUS_BAR_LOAD
                                     ));  micro_inc++;


/// Action - Status Bar - set prepare as active

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                       // action id
                                     micro_inc,                       // micro-action seq. id
                                     this,                            // SYSDONOR instance
                                     SD_STATUSBAR,                    // target object
                                     SDOP_STATUSBAR_SET_PHASE_ACTIVE, // object method
                                     STATUS_BAR_PREPARE
                                     ));  micro_inc++;



/// Action - End Run Button - visible

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                  // action id
                                     micro_inc,                  // micro-action seq. id
                                     this,                       // SYSDONOR instance
                                     SD_ENDRUN_BTN,              // target object
                                     SDOP_BUTTON_ENDRUN_ACTIVATE // object method
                                                                 // ,                     // parameter
                                     ));  micro_inc++;





/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_MAN_COL_CHKED_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_DONORINFO_BTN,                     // target object
                                     SDOP_BUTTON_DONINFO_MAN_COL_CHKED_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;




/// Action - System Load Button - SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                              // action id
                                     micro_inc,                              // micro-action seq. id
                                     this,                                   // SYSDONOR instance
                                     SD_LOADSYS_BTN,                         // target object
                                     SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT // object method
                                                                             // ,                     // parameter
                                     ));  micro_inc++;




/// Action - Continue Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONTINUE_BTN,          // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONFIG_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



///// End of State 21, Action 21 //////////////////////////////////////////////////







///// State  22 //////////////////////////////////////////////////////////////////
///// Action 22 //////////////////////////////////////////////////////////////////

/// Beginning of State / Action sequence maintenance


state_ID  = 22;
action_ID = 22;
micro_inc = 1;



state_management.add_state_entry
   ((GUI_State*)new Sysdonor_State(state_ID,                      // state id
                                   action_ID,                     // action id
                                   SETUP_COMPLETE_DONOR_COMPLETE, // Screen state
                                   SD_FALSE,                      // Enable begin run
                                   SD_DONT_CARE,                  // Display load sys
                                   SD_TRUE,                       // AP2 in control
                                   SD_DONT_CARE));                // AP2 data available



/// Action - Status Line    - Display the Donor Name in the status bar
state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                        // action id
                                     micro_inc,                        // micro-action seq. id
                                     this,                             // SYSDONOR instance
                                     SD_STATUSLINE,                    // target object
                                     SDOP_STATUSLINE_LOAD_DONOR_NAME,  // object method
                                     textEmptyFormatDonDlDonorNameInit // parameter
                                     ));  micro_inc++;



/// Action - Status Bar - set system info as complete

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                         // action id
                                     micro_inc,                         // micro-action seq. id
                                     this,                              // SYSDONOR instance
                                     SD_STATUSBAR,                      // target object
                                     SDOP_STATUSBAR_SET_PHASE_COMPLETE, // object method
                                     STATUS_BAR_INFO
                                     ));  micro_inc++;



/// Action - Status Bar - set load donor as complete

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                         // action id
                                     micro_inc,                         // micro-action seq. id
                                     this,                              // SYSDONOR instance
                                     SD_STATUSBAR,                      // target object
                                     SDOP_STATUSBAR_SET_PHASE_COMPLETE, // object method
                                     STATUS_BAR_LOAD
                                     ));  micro_inc++;


/// Action - Status Bar - set prepare as active

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                       // action id
                                     micro_inc,                       // micro-action seq. id
                                     this,                            // SYSDONOR instance
                                     SD_STATUSBAR,                    // target object
                                     SDOP_STATUSBAR_SET_PHASE_ACTIVE, // object method
                                     STATUS_BAR_PREPARE
                                     ));  micro_inc++;



/// Action - End Run Button - visible

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                  // action id
                                     micro_inc,                  // micro-action seq. id
                                     this,                       // SYSDONOR instance
                                     SD_ENDRUN_BTN,              // target object
                                     SDOP_BUTTON_ENDRUN_ACTIVATE // object method
                                                                 // ,                     // parameter
                                     ));  micro_inc++;





/// Action - Donor Info Button - SDOP_BUTTON_DONINFO_AP2_COL_CHKED_ACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                            // action id
                                     micro_inc,                            // micro-action seq. id
                                     this,                                 // SYSDONOR instance
                                     SD_DONORINFO_BTN,                     // target object
                                     SDOP_BUTTON_DONINFO_AP2_COL_CHKED_ACT // object method
                                                                           // ,                     // parameter
                                     ));  micro_inc++;




/// Action - System Load Button - SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                              // action id
                                     micro_inc,                              // micro-action seq. id
                                     this,                                   // SYSDONOR instance
                                     SD_LOADSYS_BTN,                         // target object
                                     SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT // object method
                                                                             // ,                     // parameter
                                     ));  micro_inc++;




/// Action - Continue Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONTINUE_BTN,          // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



/// Action - Configuration Button - make it INVISIBLE

state_management.add_action_entry
   ((GUI_Action*)new Sysdonor_Action(action_ID,                // action id
                                     micro_inc,                // micro-action seq. id
                                     this,                     // SYSDONOR instance
                                     SD_CONFIG_BTN,            // target object
                                     SDOP_BUTTON_SET_INVISIBLE // object method
                                                               // ,                     // parameter
                                     ));  micro_inc++;



///// End of State 22, Action 22 //////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////
// End of single include define guardian
/////////////////////////////////////////////////////////////////////////////////
#endif

/* FORMAT HASH 468e9050378695238e086dc350fcd870 */

/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996, 1997, 1998 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb, Terry Wahl

*     Class name:  N/A

*     File name:   xcommmsg.cpp

*     Contents:    Test code for the GUI class structures

*     Description: This is a Router Msg test of the SCREEN COMMUTATOR
                   module for Focussed System

*     Documents:
          Requirements:    I:\....
          Select OMT:      I:\....
          Test:            I:\....

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: H:/BCT_Development/Trima5.R/Trima/gui/guitest/rcs/xcommmsg.cpp 1.19 2003/06/03 14:16:38Z rm70006 Exp spriusb $
$Log: xcommmsg.cpp $
Revision 1.19  2003/06/03 14:16:38Z  rm70006
IT 6096.

Change code to use new alarm interface.
Revision 1.18  2002/02/01 18:45:14Z  rm10919
Enhanced service mode, predict, sysdonor, donordl screen messages.
Revision 1.17  2002/01/19 00:00:18Z  rm10919
Changed to show stick meter for priming message on Run Proc screen.
Revision 1.16  2002/01/15 21:11:31Z  pn02526
Changed to allow the input of the donor name and birth date in the 6,8 menu choice,
 exactly as as it is in the 6,9 menu choice.
Revision 1.15  2001/12/19 14:27:11  pn02526
Change LOW_PREDICTED_PLATELET_YIELD to HIGH_PLATELET_YIELD_DEVIATION.
Add LONG_DRAW reason to PLATELET_WBCS and PLASMA_VOLUME flags.
Revision 1.14  2001/12/19 12:44:04  rm10919
Added added more GUI CONFIG messages.
Revision 1.13  2001/10/01 23:30:33Z  rm10919
Added Vista sysdonor capability,
 cassette substates and two alarms to alarm suite
Revision 1.12  2001/06/12 22:39:34Z  sb07663
revised for latest alarms.h
Revision 1.11  2001/05/31 14:58:07  sb07663
Removed unneeded alarms; added in some "count" icons
Revision 1.10  2001/05/16 22:48:54  pn02526
Revise count/verify flags code for new Product Quality data
Revision 1.9  2001/05/15 08:44:47  sb07663
Initial rev for V5 screen snapshot capability
Revision 1.8  2001/01/25 00:37:02  tw09453
Added functionality to allow the service messages to be simulated
for GUI screen testing.
Revision 1.7  2000/11/18 02:43:48  tw09453
Added graphic for the Air Evacuation alarm
Revision 1.6  2000/11/01 00:38:46  tw09453
Adding the "stick" pressure meter to the donor connect and run proc screens
Revision 1.5  2000/08/15 16:28:33  MS10234
Changed alarm state names to be more meaningful
Revision 1.4  2000/06/28 19:29:22  bd10648
Made compatable to new adjustments structure
Revision 1.3  2000/06/06 19:35:38  ms10234
Messages were moved from the common project to the trima project
Revision 1.2  2000/04/06 23:58:06  MS10234
Commented out references to proc blood run states
Revision 1.1  2000/02/09 20:03:06  JR08014
Initial revision
Revision 1.52  1999/08/11 00:11:18  TW09453
Update GUI test driver for 3.3a
Revision 1.51  1999/06/15 00:11:24  TW09453
Add testing logic for 3.3a
Revision 1.50  1999/05/21 18:20:27  TW09453
Add test messaging for new predict screen and adjust screen
functioning
Revision 1.49  1999/04/28 23:17:53  TW09453
Make structure, define, enum changes throughout the program to
keep in sync with Proc changes made in Adjust and Predict
messaging.
Revision 1.48  1998/10/26 21:30:08  BF02858
- added additional ProcConfigUpdate int32 message need to exit configuration
  because CONFIG now waits for two of these messages from AOT
Revision 1.47  1998/10/07 18:04:16  tw09453
Correct the cassette ID enum problem for the cassette msg.
Revision 1.46  1998/10/02 22:53:10  tw09453
Add the Proc Config Update complete int32 message in order to test
config's msg callback.
Revision 1.45  1998/10/01 00:33:10  tw09453
Adjust the alarm range checking to be congruent with the alarm table.
Revision 1.44  1998/09/16 00:27:07  tw09453
Change data for donordl donor vitals
Revision 1.43  1998/09/04 16:03:06  tw09453
Re-add the AP2AgentInterface messaging for signalling the change
of AP2 data. This was lost in 1.42
Revision 1.42  1998/08/15 01:30:42  MC03015
addition of status flags and proc tbv to donordat suite
Revision 1.40  1998/08/08 00:16:16  MC03015
addition of donordl screen to screen transitions
Revision 1.39  1998/08/05 22:39:54  MC03015
addition of drbc substates
Revision 1.36  1998/07/07 22:57:49  MC03015
updated to remove external tbv information
Revision 1.35  1998/04/24 22:43:36  MC03015
addition of drbc substate processing
Revision 1.34  1998/04/20 19:35:28  MC03015
addition of alarm autosequencing for snapshots
Revision 1.33  1998/04/09 01:03:34  MC03015
update adjust screen values to appear more realistic
Revision 1.32  1998/04/01 04:00:21  MC03015
extend snapshot time for reliability, fix keypad autorun
Revision 1.31  1998/03/27 22:45:38  MC03015
addition of alarm table scroll feature
Revision 1.29  1998/03/03 05:11:40  MC03015
addition of substate messaging for adjut screen testing
Revision 1.28  1998/03/02 20:30:18  MC03015
addition of no blood type in donor info blood type keypad.  addition of queue length of 6000 to new dispatcher.
Revision 1.27  1998/02/18 01:37:03  MC03015
removal of multiple dispatcher calls and debug of output file comment call
Revision 1.26  1998/02/06 04:12:31  MC03015
addition of 30 sec syncing delay for snapshots.  addition of memeet of donvitals struct prior to units implementation.  addition of clear all entries for the doninfo screen.
Revision 1.25  1998/01/28 23:40:21  MC03015
addition of procedure tim start logic and alarm number printout logic
Revision 1.24  1998/01/28 20:05:15  MC03015
removal of INLET_PRESSURE_STATUS_OK and INLET_PRESSURE_STATUS_NOT_OK metric for access pressure too high and low warnings
Revision 1.23  1998/01/19 17:36:09  MC03015
addition of mouse autodetect (for future implementation).  we want all scripts to be able to use the autorun feature prior to the mouse capture implementation, (i.e. we want all scripts to work wether the autocapture is implemented or not so we put in a placeholder now)
Revision 1.22  1998/01/06 22:16:46  MC03015
modification to mouse event handler - debug
Revision 1.21  1997/12/09 19:35:15  MC03015
combined units message for initializaion on donor vitals screen
Revision 1.20  1997/12/05 19:38:38  MC03015
addition of runsumm2 random number population of interface
Revision 1.19  1997/12/04 21:57:15  MC03015
modification to dynamic units switching
Revision 1.18  1997/12/04 21:51:41  MC03015
modification to height and weight units messaging
Revision 1.17  1997/12/04 19:54:49  MC03015
dynamic pressure modeling changes
Revision 1.16  1997/12/02 22:02:23  MC03015
addition of alarm range checking and system call feature.  addition of rinseback ok and not ok messaging
*/

#include "xcommmsg.hpp"

#include "alarmcontainer.h"
#define _ALARM_HANDLER_
#include "alarms.h"
#include "vip_shm.h"    // add vista sysdonor screen


// REFERENCE THE ROUTER MESSAGE DISPATCHER (VIA ITS GLOBAL POINTER)

dispatcher* dispatch ;



/////////////////////////////////////MAIN//////////////////////////////////////

main(int argc, char* argv[])
{

   CVipShm::create();
   GUI_message_driver procedure_user_emulator;

   // prepare xcommmsg by managing all input parameters
   procedure_user_emulator.parse_xcommmsg_input_arguments (argc, argv) ;

   // start xcommmsg
   procedure_user_emulator.start_xcommmsg (argc, argv);

   // return success upon completion
   return(0);

}; // End of Main

// CONSTRUCTOR
GUI_message_driver::GUI_message_driver()
{

   true_one             = 1;      // define primary loop control value

   xcommmsg_input_file  = NULL;      // initialize input file states to false
   xcommmsg_output_file = NULL;      // initialize output file states to false
   snapshot_data_dir    = NULL;      // initialize snapshot directory states to false

}

// DESTRUCTOR
GUI_message_driver::~GUI_message_driver()
{}

// gui xcommmsg driver main loop
void GUI_message_driver::start_xcommmsg (int argc, char* argv[])
{
   // start the dispatcher
   dispatch = new dispatcher(argc, argv, 6000);

   // add an explicit EOL and 30sec delay before the first pass through the main
   // loop when the output is open but an input file is not,  i.e. we don't want
   // to echo the delay when it is already contained in the input file.
   if ((xcommmsg_output_file != NULL) && (xcommmsg_input_file == NULL))
   {
      // provide c-return and delays after text entries between main loop cycles
      // this code is temporary until the messaging is in place to support screen
      // refresh complete logic which will gate the next cycle
      fputs(MESSAGE_PROCESSING_ENTRY, xcommmsg_output_file);
      fputs(BUTTON_CAR_RETURN, xcommmsg_output_file);
      fputs(SNAPSHOT_PROCESSING_DELAY, xcommmsg_output_file);
      fputs(BUTTON_CAR_RETURN, xcommmsg_output_file);
   }

   // Start the keyboard loop asking to send messages
   while (true_one == 1)
   {

      printf("\n +********************************************************+\n\n"
             "+ press 1  for system message indication\n"
             "+ press 2  for state change messages\n"
             "+ press 3  for static screen goto messages\n"
             "+ press 4  for child screen messages\n"
             "+ press 5  for stateless screen messages\n"
             "+ press 6  for specific screen message suites\n"
             "+ press 7  for system string messages\n"
             "+ press 8  to  set system delay in milliseconds\n"
             "+ press 9  to  set mouse event\n"
             "+ press 10 to  EXIT all applications\n"
             "+ enter selection -> "
             );

      // Get the choice
      switch (get_switch_value())
      {

         case 1 :
         {
            printf("++++ press  1 for  gui alive message\n"
                   "++++ press  2 for  enable load system message\n"
                   "++++ press  3 for  disposable completed lowering message\n"
                   "++++ press  4 for  disposable raised message\n"
                   "++++ press  5 for  cassette type message\n"
                   "++++ press  6 for  unable to load disposable message\n"
                   "++++ press  7 for  disposable test completed message\n"
                   "++++ press  8 for  AC prime completed message\n"
                   "++++ press  9 for  switch to previous screen message\n"
                   "++++ press 10 for  RINSEBACK OK message\n"
                   "++++ press 11 for  RINSEBACK NOT OK message\n"
                   "++++ press 12 for  run complete message\n"
                   "++++ press 13 for  donor disconnected message\n"
                   "++++ press 14 for  reset internal system states\n"
                   "++++ press 15 for  Proc configuration update message\n"
                   "++++ press 16 for  Proc to GUI Predict status message\n"
                   "++++ press 17 for  Safety Service Mode message\n"
                   "++++ press 18 for  Cassette Status messages\n"
                   "++++ press 19  to  EXIT\n"                                                "++++ enter selection -> "
                   );

            // Get the choice
            switch (get_switch_value())
            {

               case 1 :
                  send_int32_message (focusInt32Msg::QueryGUIAlive, 0);
                  break;

               case 2 :
                  send_int32_message (focusInt32Msg::EnableLoadSystem, 0);
                  break;

               case 3 :
                  send_int32_message (focusInt32Msg::DisposableLowered, 0);
                  break;

               case 4 :
                  send_int32_message (focusInt32Msg::DisposableRaised, 0);
                  break;

               case 5 :
                  printf("+++++++ enter the number for the cassette type\n"
                         "+++++++ 1  for  noloaded\n"
                         "+++++++ 2  for  unknown\n"
                         "+++++++ 3  for  platelet/plasma/rbc\n"
                         "+++++++ 4  for  \n"
                         "+++++++ 5  for  rbc/plasma\n"
                         "+++++++ enter selection -> "
                         );

                  get_input(&cassette_id);

                  if ((cassette_id >= 1) && (cassette_id <= 5))
                  {
                     send_int32_message (focusInt32Msg::ProcToGuiCassetteInfo, (long)(cassette_id - 1)) ;
                     confirm_msg();
                  }
                  else
                  {
                     printf("\n +**** UNKNOWN CASSETTE SELECTION - start over ****+\n");
                  }

                  break;

               case 6 :
                  send_int32_message (focusInt32Msg::UnsuccessfulCassetteLowering, 0);
                  break;

               case 7 :
                  send_int32_message (focusInt32Msg::TestCompleted, 0);
                  break;

               case 8 :
                  send_int32_message (focusInt32Msg::ACPrimeCompleted, 0);
                  break;

               case 9 :
                  send_int32_message (focusInt32Msg::SwitchToPreviousScreen, 0);
                  break;

               case 10 :
                  send_int32_message (focusInt32Msg::RinsebackStatus, RINSEBACK_OK);
                  break;

               case 11 :
                  send_int32_message (focusInt32Msg::RinsebackStatus, RINSEBACK_NOT_OK);
                  break;

               case 12 :
                  send_int32_message (focusInt32Msg::RunComplete, 0);
                  break;

               case 13 :
                  send_int32_message (focusInt32Msg::ConfirmDonorDisconnected, 0);
                  break;

               case 14 :
                  send_int32_message (focusInt32Msg::InitializeInternalStates, 0);
                  break;

               case 15 :
                  send_int32_message (focusInt32Msg::ProcConfigUpdateMsg, 0);
                  send_int32_message (focusInt32Msg::ProcConfigUpdateMsg, 1);
                  break;

               case 16 :
                  printf("+++++++ enter the number for the Proc to GUI Predict message\n"
                         "+++++++ 1  for  PROCEDURE_VALID\n"
                         "+++++++ 2  for  REQUEST_SELECTION_SCREEN\n"
                         "+++++++ 3  for exit\n"
                         "+++++++ enter selection -> "
                         );
                  {           // local block

                     int proc_predict_message ;

                     get_input(&proc_predict_message);

                     switch (proc_predict_message)
                     {
                        case 1 :
                           send_int32_message (focusInt32Msg::ProcToGuiPredictStatus, (int)PROCEDURE_VALID) ;
                           break;

                        case 2 :
                           send_int32_message (focusInt32Msg::ProcToGuiPredictStatus, (int)REQUEST_SELECTION_SCREEN) ;
                           break;

                        case 3 :
                           break;

                        default :
                           printf("\n +**** UNKNOWN PROC PREDICT Selection - start over ****+\n");
                     }

                  }           // end local block

                  break;

               case 17 :
                  send_int32_message (focusInt32Msg::SafetyServiceMode, 0);
                  break;

               case 18 :
                  printf("+++++++ enter the number for the cassette status\n"
                         "+++++++ 1  for  Cassette Lowering\n"
                         "+++++++ 2  for  Cassette Lowered\n"
                         "+++++++ 3  for  Evacuating\n"
                         "+++++++ 4  for  Cassette Raising\n"
                         "+++++++ 5  for  Cassette Raised\n"
                         "+++++++ enter selection -> "
                         );
                  {           // local block

                     int cassette_status;

                     get_input(&cassette_status);

                     switch (cassette_status)
                     {
                        case 1 :
                           send_int32_message (focusInt32Msg::CassettePositionStatus, CASSETTE_LOWERING);
                           break;

                        case 2 :
                           send_int32_message (focusInt32Msg::CassettePositionStatus, CASSETTE_LOWERED);
                           break;

                        case 3 :
                           send_int32_message (focusInt32Msg::CassettePositionStatus, CASSETTE_EVACUATING);
                           break;

                        case 4 :
                           send_int32_message (focusInt32Msg::CassettePositionStatus, CASSETTE_RAISING);
                           break;

                        case 5 :
                           send_int32_message (focusInt32Msg::CassettePositionStatus, CASSETTE_RAISED);
                           break;

                        default :
                           printf("\n +**** UNKNOWN Cassette Status Selection - start over ****+\n");
                     }

                  }           // end local block

                  break;


               case 19 :
                  break;


               default :
                  printf("\n +**** UNKNOWN MESSAGE SELECTION - start over ****+\n");

            }         // End of switch statement
         }
         break;

         case 2 :
         {
            printf("++++ press 1  for  FIRST STATE\n"
                   "++++ press 2  for  SELF TEST\n"
                   "++++ press 3  for  STARTUP TESTS\n"
                   "++++ press 4  for  DISPOSABLE TEST\n"
                   "++++ press 5  for  AC CONNECTED\n"
                   "++++ press 6  for  AC PRIME\n"
                   "++++ press 7  for  DONOR CONNECTED\n"
                   "++++ press 8  for  BLOOD PRIME\n"
                   "++++ press 9  for  BLOOD RUN\n"
                   "++++ press 10 for  BLOOD RINSEBACK\n"
                   "++++ press 11 for  DONOR DISCONNECT\n"
                   "++++ press 12 for  POST RUN\n"
                   "++++ press 13 for  \n"
                   "++++ press 14 for  DEFAULT STATE\n"
                   "++++ press 15 for  \n"
                   "++++ press 16 for  BARCODE\n"
                   "++++ press 17  to  EXIT\n"
                   "++++ enter selection -> "
                   );

            // Get the choice
            switch (get_switch_value())
            {

               case 1 :
                  send_int32_message (focusInt32Msg::SystemStateChange, (int)FIRST_STATE);
                  break;

               case 2 :
                  send_int32_message (focusInt32Msg::SystemStateChange, (int)SELF_TEST);
                  break;

               case 3 :
               {
                  printf("+++++++ enter the number for Vista or non-Vista connection\n"
                         "+++++++ 1 for Vista SysDonor Screen\n"
                         "+++++++ 2 for regular SysDonor Screen\n"
                         "+++++++ 3  to  EXIT\n"
                         "+++++++ enter selection -> "
                         );

                  int get_vista_value;
                  get_input (&get_vista_value);
                  switch (get_vista_value)
                  {
                     case 1 :
                        // Set to show Vista Sysdonor Screen
                        CVipShm::privileged(1);
                        CVipShm::broadcastCompleted(1);
                        CVipShm::vistaConnected(1);
                        send_int32_message (focusInt32Msg::SystemStateChange, (int)STARTUP_TESTS);
                        break;

                     case 2 :
                        // Set to show regular Sysdonor Screen
                        CVipShm::privileged(0);
                        CVipShm::broadcastCompleted(0);
                        CVipShm::vistaConnected(0);
                        send_int32_message (focusInt32Msg::SystemStateChange, (int)STARTUP_TESTS);
                        break;

                     case 3 :
                        break;

                     default :
                        printf("\n +**** UNKNOWN SELECTION - start over ****+\n");
                  }
               }
               break;

               case 4 :
                  send_int32_message (focusInt32Msg::SystemStateChange, (int)DISPOSABLE_TEST);
                  break;

               case 5 :
                  send_int32_message (focusInt32Msg::SystemStateChange, (int)AC_CONNECTED);
                  break;

               case 6 :
                  send_int32_message (focusInt32Msg::SystemStateChange, (int)AC_PRIME);
                  break;

               case 7 :
                  send_int32_message (focusInt32Msg::SystemStateChange, (int)DONOR_CONNECTED);
                  break;

               case 8 :
                  send_int32_message (focusInt32Msg::SystemStateChange, (int)BLOOD_PRIME);
                  break;

               case 9 :
                  send_int32_message (focusInt32Msg::SystemStateChange, (int)BLOOD_RUN);
                  break;

               case 10 :
                  send_int32_message (focusInt32Msg::SystemStateChange, (int)BLOOD_RINSEBACK);
                  break;

               case 11 :
                  send_int32_message (focusInt32Msg::SystemStateChange, (int)DONOR_DISCONNECT);
                  break;

               case 12 :
                  send_int32_message (focusInt32Msg::SystemStateChange, (int)POST_RUN);
                  break;

               case 13 :
                  break;

               case 14 :
                  send_int32_message (focusInt32Msg::SystemStateChange, (int)DEFAULT_STATE);
                  break;

               case 15 :
                  break;

               case 16 :
                  send_int32_message (focusInt32Msg::BarcodeScanned, 0);
                  confirm_msg();
                  break;


               case 17 :
                  break;


               default :
                  printf("\n +**** UNKNOWN STATE SELECTION - start over ****+\n");

            }         // End of switch statement


         }
         break;

         case 3 :
         {
            printf("++++ press 1  for  SYSDONOR\n"
                   "++++ press 2  for  SYSDISP\n"
                   "++++ press 3  for  SYSCLAMP\n"
                   "++++ press 4  for  SYSTEST\n"
                   "++++ press 5  for  DISCONN\n"
                   "++++ press 6  for  DONCONN\n"
                   "++++ press 7  for  DONINFO\n"
                   "++++ press 8  for  DONVITAL\n"
                   "++++ press 9  for  POWERUP\n"
                   "++++ press 10 for  RUNPROC\n"
                   "++++ press 11 for  RUNSUMM1\n"
                   "++++ press 12 for  RUNSUMM2\n"
                   "++++ press 13 for  SYSACATT\n"
                   "++++ press 14 for  ADJUST\n"
                   "++++ press 15 for  SERV1\n"
                   "++++ press 16 for  PREDICT\n"
                   "++++ press 17 for  CONFIG\n"
                   "++++ press 18 for  CONFIG MACHINE\n"
                   "++++ press 19 for  CONFIG LANGUAGE\n"
                   "++++ press 20 for  CONFIG PLASMA\n"
                   "++++ press 21 for  CONFIG PLATELET\n"
                   "++++ press 22 for  CONFIG PROCEDURE\n"
                   "++++ press 23 for  CONFIG RBC\n"
                   "++++ press 24 for  CONFIG DONOR\n"
                   "++++ press 25 for  CONFIG REPORT\n"
                   "++++ press 26 for  CONFIG PRIORITY\n"
                   "++++ press 27 for  \n"
                   "++++ press 28 for  DONORDL\n"
                   "++++ press 29 for  LABELPRD\n"
                   "++++ press 30 for  VERIFPRD\n"
                   "++++ press 31 for  BARCODE\n"
                   "++++ press 32  to  EXIT\n"
                   "++++ enter selection -> "
                   );

            // Get the choice
            switch (get_switch_value())
            {
               case 1 :
                  goto_screen(GUI_SCREEN_SYSDONOR, "No Parameter") ;
                  break;

               case 2 :
                  goto_screen(GUI_SCREEN_SYSDISP, "No Parameter") ;
                  break;

               case 3 :
                  goto_screen(GUI_SCREEN_SYSCLAMP, "No Parameter") ;
                  break;

               case 4 :
                  goto_screen(GUI_SCREEN_SYSTEST, "No Parameter") ;
                  break;

               case 5 :
                  goto_screen(GUI_SCREEN_DISCONN, "No Parameter") ;
                  break;

               case 6 :
                  goto_screen(GUI_SCREEN_DONCONN, "No Parameter") ;
                  break;

               case 7 :
                  goto_screen(GUI_SCREEN_DONINFO, "No Parameter") ;
                  break;

               case 8 :
                  goto_screen(GUI_SCREEN_DONVITAL, "No Parameter") ;
                  break;

               case 9 :
                  goto_screen(GUI_SCREEN_POWERUP, "No Parameter") ;
                  break;

               case 10 :
                  goto_screen(GUI_SCREEN_RUNPROC, "No Parameter") ;
                  break;

               case 11 :
                  goto_screen(GUI_SCREEN_RUNSUMM1, "No Parameter") ;
                  break;

               case 12 :
                  goto_screen(GUI_SCREEN_RUNSUMM2, "No Parameter") ;
                  break;

               case 13 :
                  goto_screen(GUI_SCREEN_SYSACATT, "No Parameter") ;
                  break;

               case 14 :
                  goto_screen(GUI_SCREEN_TROUBLE, "No Parameter") ;
                  break;

               case 15 :
                  goto_screen(GUI_SCREEN_SERV1, "No Parameter") ;
                  break;

               case 16 :
                  goto_screen(GUI_SCREEN_PREDICT, "No Parameter") ;
                  break;

               case 17 :
                  goto_screen(GUI_SCREEN_CONFIG, "No Parameter") ;
                  break;

               case 18 :
                  goto_screen(GUI_SCREEN_CONFIG_MACHINE, "No Parameter") ;
                  break;

               case 19 :
                  goto_screen(GUI_SCREEN_CONFIG_LANGUAGE, "No Parameter") ;
                  break;

               case 20 :
                  goto_screen(GUI_SCREEN_CONFIG_PLASMA, "No Parameter") ;
                  break;

               case 21 :
                  goto_screen(GUI_SCREEN_CONFIG_PLATELET, "No Parameter") ;
                  break;

               case 22 :
                  goto_screen(GUI_SCREEN_CONFIG_PROCEDURE, "No Parameter") ;
                  break;

               case 23 :
                  goto_screen(GUI_SCREEN_CONFIG_RBC, "No Parameter") ;
                  break;

               case 24 :
                  goto_screen(GUI_SCREEN_CONFIG_DONOR, "No Parameter") ;
                  break;

               case 25 :
                  goto_screen(GUI_SCREEN_CONFIG_REPORT, "No Parameter") ;
                  break;

               case 26 :
                  goto_screen(GUI_SCREEN_CONFIG_PRIORITY, "No Parameter") ;
                  break;

               case 27 :
                  break;

               case 28 :
                  goto_screen(GUI_SCREEN_DONORDL, "No Parameter") ;
                  break;

               case 29 :
                  goto_screen(GUI_SCREEN_LABELPRD, "No Parameter") ;
                  break;

               case 30 :
                  goto_screen(GUI_SCREEN_VERIFPRD, "No Parameter") ;
                  break;

               case 31 :
                  goto_screen(GUI_SCREEN_BARCODE, "No Parameter");
                  break;

               case 32 :
                  break;

               default :
                  printf("\n +**** UNKNOWN STATIC SCREEN SELECTION - start over ****+\n");

            }         // End of switch statement
         }
         break;

         case 4 :
         {
            printf("++++ press 1  to  remove most recent child screen\n"
                   "++++ press 2  to  swap child screen  \n"
                   "++++ press 3  to  swap child stack with screen\n"
                   "++++ press 4  to  invoke the KEYPAD screen\n"
                   "++++ press 5  to  invoke the ABORTCNF screen\n"
                   "++++ press 6  to  invoke the LISTBOX screen\n"
                   "++++ press 7  to  EXIT\n"
                   "++++ enter selection -> "
                   );

            // Get the choice
            switch (get_switch_value())
            {

               case 1 :
                  send_int32_message (focusInt32Msg::RemoveMostRecentChildScreen, 0);
                  break;

               case 2 :
                  printf("+++++++ press  1  to  swap child screen ABORTCNF\n"
                         "+++++++ press  2  to  swap child screen ADJUST\n"
                         "+++++++ press  3  to  swap child screen PREDICT\n"
                         "+++++++ enter selection -> ");

                  switch (get_switch_value())
                  {

                     case 1 :
                        printf("++++++++++ press 1  to invoke the alarm confirmation\n"
                               "++++++++++ press 2  to invoke the alarm rinseback confirm\n"
                               "++++++++++ press 3  to invoke the rinseback confirmation\n"
                               "++++++++++ press 4  to invoke the end run confirmation\n"
                               "++++++++++ press 5  to invoke the spillover confirmation\n"
                               "++++++++++ press 6  to invoke the air in plasma confirmation\n"
                               "++++++++++ enter selection -> ");

                        switch (get_switch_value())
                        {

                           case 1 :
                              strcpy(screen_choice.alloc_param, "ALARM") ;
                              break;

                           case 2 :
                              strcpy(screen_choice.alloc_param, "RINSE") ;
                              break;

                           case 3 :
                              strcpy(screen_choice.alloc_param, "RINSEBACK") ;
                              break;

                           case 4 :
                              strcpy(screen_choice.alloc_param, "USER") ;
                              break;

                           case 5 :
                              strcpy(screen_choice.alloc_param, "SPILLOVER") ;
                              break;

                           case 6 :
                              strcpy(screen_choice.alloc_param, "AIR_IN_PLASMA") ;
                              break;

                           default :
                              printf("\n +**** UNKNOWN SPECIFIC SCREEN SELECTION - start over ****+\n");

                        }             // End of switch statement

                        screen_choice.screen_id   = GUI_SCREEN_ABORTCNF ;

                        Msg_Ptr_Swap_Child_Screen = new Msg_Swap_Child_Screen;
                        Msg_Ptr_Swap_Child_Screen -> set (&screen_choice);
                        confirm_msg();
                        break;

                     case 2 :
                        screen_choice.screen_id = GUI_SCREEN_TROUBLE ;
                        strcpy(screen_choice.alloc_param, "Parameter Here") ;

                        Msg_Ptr_Swap_Child_Screen = new Msg_Swap_Child_Screen;
                        Msg_Ptr_Swap_Child_Screen -> set (&screen_choice);
                        confirm_msg();
                        break;

                     case 3 :
                        printf("++++++++++ press 1  to  invoke from doninfo w no delta\n"
                               "++++++++++ press 2  to  invoke from doninfo w delta\n"
                               "++++++++++ press 3  to  invoke from adjust w no delta\n"
                               "++++++++++ press 4  to  invoke from adjut w delta\n"
                               "++++++++++ enter selection -> ");

                        switch (get_switch_value())
                        {

                           case 1 :
                              sprintf(predict_call_type, "%d", (int)DONINFO_NO_DELTA);
                              break;

                           case 2 :
                              sprintf(predict_call_type, "%d", (int)DONINFO_DELTA);
                              break;

                           case 3 :
                              sprintf(predict_call_type, "%d", (int)ADJUST_NO_DELTA);
                              break;

                           case 4 :
                              sprintf(predict_call_type, "%d", (int)ADJUST_DELTA);
                              break;

                           default :
                              printf("\n +**** UNKNOWN SPECIFIC SCREEN SELECTION - start over ****+\n");

                        }             // End of switch statement

                        strcpy(screen_choice.alloc_param, predict_call_type) ;
                        screen_choice.screen_id   = GUI_SCREEN_PREDICT ;

                        Msg_Ptr_Swap_Child_Screen = new Msg_Swap_Child_Screen;
                        Msg_Ptr_Swap_Child_Screen -> set (&screen_choice);
                        confirm_msg();
                        break;

                     default :
                        printf("\n +**** UNKNOWN SPECIFIC SCREEN SELECTION - start over ****+\n");

                  }           // End of switch statement

                  break;

               case 3 :
                  printf("+++++++ press  1  to  swap child w screen ABORTCNF\n"
                         "+++++++ press  2  to  swap child w screen ADJUST\n"
                         "+++++++ press  3  to  swap child w screen PREDICT\n"
                         "+++++++ enter selection -> ");

                  switch (get_switch_value())
                  {

                     case 1 :
                        printf("++++++++++ press 1  to invoke the alarm confirmation\n"
                               "++++++++++ press 2  to invoke the alarm rinseback confirm\n"
                               "++++++++++ press 3  to invoke the rinseback confirmation\n"
                               "++++++++++ press 4  to invoke the end run confirmation\n"
                               "++++++++++ press 5  to invoke the spillover confirmation\n"
                               "++++++++++ press 6  to invoke the air in plasma confirmation\n"
                               "++++++++++ enter selection -> ");

                        switch (get_switch_value())
                        {

                           case 1 :
                              strcpy(screen_choice.alloc_param, "ALARM") ;
                              break;

                           case 2 :
                              strcpy(screen_choice.alloc_param, "RINSE") ;
                              break;

                           case 3 :
                              strcpy(screen_choice.alloc_param, "RINSEBACK") ;
                              break;

                           case 4 :
                              strcpy(screen_choice.alloc_param, "USER") ;
                              break;

                           case 5 :
                              strcpy(screen_choice.alloc_param, "SPILLOVER") ;
                              break;

                           case 6 :
                              strcpy(screen_choice.alloc_param, "AIR_IN_PLASMA") ;
                              break;

                           default :
                              printf("\n +**** UNKNOWN SPECIFIC SCREEN SELECTION - start over ****+\n");

                        }             // End of switch statement

                        screen_choice.screen_id         = GUI_SCREEN_ABORTCNF ;

                        Msg_Ptr_Swap_Child_Stack_Screen = new Msg_Swap_Child_Stack_Screen;
                        Msg_Ptr_Swap_Child_Stack_Screen -> set (&screen_choice);
                        confirm_msg();
                        break;

                     case 2 :
                        screen_choice.screen_id = GUI_SCREEN_TROUBLE ;
                        strcpy(screen_choice.alloc_param, "Parameter Here") ;

                        Msg_Ptr_Swap_Child_Stack_Screen = new Msg_Swap_Child_Stack_Screen;
                        Msg_Ptr_Swap_Child_Stack_Screen -> set (&screen_choice);
                        confirm_msg();
                        break;

                     case 3 :
                        printf("++++++++++ press 1  to  invoke from doninfo w no delta\n"
                               "++++++++++ press 2  to  invoke from doninfo w delta\n"
                               "++++++++++ press 3  to  invoke from adjust w no delta\n"
                               "++++++++++ press 4  to  invoke from adjut w delta\n"
                               "++++++++++ enter selection -> ");

                        switch (get_switch_value())
                        {

                           case 1 :
                              sprintf(predict_call_type, "%d", (int)DONINFO_NO_DELTA);
                              break;

                           case 2 :
                              sprintf(predict_call_type, "%d", (int)DONINFO_DELTA);
                              break;

                           case 3 :
                              sprintf(predict_call_type, "%d", (int)ADJUST_NO_DELTA);
                              break;

                           case 4 :
                              sprintf(predict_call_type, "%d", (int)ADJUST_DELTA);
                              break;

                           default :
                              printf("\n +**** UNKNOWN SPECIFIC SCREEN SELECTION - start over ****+\n");

                        }             // End of switch statement

                        strcpy(screen_choice.alloc_param, predict_call_type) ;
                        screen_choice.screen_id         = GUI_SCREEN_PREDICT ;

                        Msg_Ptr_Swap_Child_Stack_Screen = new Msg_Swap_Child_Stack_Screen;
                        Msg_Ptr_Swap_Child_Stack_Screen -> set (&screen_choice);
                        confirm_msg();
                        break;

                     default :
                        printf("\n +**** UNKNOWN SPECIFIC SCREEN SELECTION - start over ****+\n");

                  }           // End of switch statement

                  break;

               case 4 :
                  printf("+++++++ press 1  to  invoke the height ft/in keypad\n"
                         "+++++++ press 2  to  invoke the height cm keypad\n"
                         "+++++++ press 3  to  invoke the weight lbs keypad\n"
                         "+++++++ press 4  to  invoke the weight kgs keypad\n"
                         "+++++++ press 5  to  invoke the blood type keypad\n"
                         "+++++++ press 6  to  invoke the hematocrit keypad\n"
                         "+++++++ press 7  to  invoke the platelet precount keypad\n"
                         "+++++++ press 8  to  invoke the donor time keypad\n"
                         "+++++++ press 9  to  invoke a custom keypad\n"
                         "+++++++ enter selection -> ");

                  switch (get_switch_value())
                  {

                     case 1 :
                        itoa((int)HEIGHT_US, keypad_allocation_string, 10);
                        break;

                     case 2 :
                        itoa((int)HEIGHT_CM, keypad_allocation_string, 10);
                        break;

                     case 3 :
                        itoa((int)WEIGHT_LB, keypad_allocation_string, 10);
                        break;

                     case 4 :
                        itoa((int)WEIGHT_KG, keypad_allocation_string, 10);
                        break;

                     case 5 :
                        itoa((int)BLOOD_TYPE, keypad_allocation_string, 10);
                        break;

                     case 6 :
                        itoa((int)HEMATOCRIT, keypad_allocation_string, 10);
                        break;

                     case 7 :
                        itoa((int)PLATELET_PRECOUNT, keypad_allocation_string, 10);
                        break;

                     case 8 :
                        itoa((int)DONOR_TIME, keypad_allocation_string, 10);
                        break;

                     case 9 :
                        printf("++++++++++ instrucions on building a keypad string\n"
                               "++++++++++ a) start the string with a '~0~', this is the\n"
                               "++++++++++    request for the standard keypad bounded by\n"
                               "++++++++++    the keypad's string token '~'.\n"
                               "++++++++++ b) enter values in order defined below,\n"
                               "++++++++++    without leading spaces, (or the corresponding\n"
                               "++++++++++    value will be ignoredby the parser), followed\n"
                               "++++++++++    by another '~' token.\n"
                               "++++++++++ c) if you want to ignore a value insret a blank\n"
                               "++++++++++    in the value field. all values after the last\n"
                               "++++++++++    tokenized position in the input string is set\n"
                               "++++++++++    to the defaults.\n"
                               "++++++++++ \n"
                               "++++++++++ e.g. ~0~100.0~0.0~ ~feet~\n"
                               "++++++++++ \n"
                               "++++++++++ KEYPAD_IDS       keypad_name\n"
                               "++++++++++ float            keypad_max_value\n"
                               "++++++++++ float            keypad_min_value\n"
                               "++++++++++ short            keypad_location_x\n"
                               "++++++++++ short            keypad_location_y\n"
                               "++++++++++ char             keypad_format_1[STRING_BUFFER]\n"
                               "++++++++++ char             keypad_format_2[STRING_BUFFER]\n"
                               "++++++++++ float            keypad_initial_value_1\n"
                               "++++++++++ float            keypad_initial_value_2\n"
                               "++++++++++ int              keypad_exponent\n"
                               "++++++++++ int              keypad_zero_ok\n"
                               "++++++++++ ENTRY_DIRECTION  keypad_entry_type\n"
                               "++++++++++ CONFIG_DATA_KEYS keypad_data_type\n"
                               "++++++++++ TEXT_STRING_ID   keypad_units_text_id_1\n"
                               "++++++++++ TEXT_STRING_ID   keypad_units_text_id_2\n"
                               "++++++++++ TEXT_STRING_ID   keypad_button_enter_text_id\n"
                               "++++++++++ TEXT_STRING_ID   keypad_button_0_text_id\n"
                               "++++++++++     .....                 ......  \n"
                               "++++++++++ TEXT_STRING_ID   keypad_button_9_text_id\n"
                               "++++++++++ TEXT_STRING_ID   keypad_button_clear_text_id\n"
                               "++++++++++ enter string -> ");
                        get_input(keypad_allocation_string);

                        strip_whitespace(keypad_allocation_string) ;

                        break;

                     default :
                        printf("\n +**** UNKNOWN SPECIFIC SCREEN SELECTION - start over ****+\n");

                  }           // End of switch statement
                  screen_choice.screen_id = GUI_SCREEN_KEYPAD ;
                  strcpy(screen_choice.alloc_param, keypad_allocation_string) ;

                  Msg_Ptr_Swap_Child_Stack_Screen = new Msg_Swap_Child_Stack_Screen;
                  Msg_Ptr_Swap_Child_Stack_Screen -> set (&screen_choice);
                  confirm_msg();
                  break;

               case 5 :
                  printf("+++++++ press 1  to  invoke the alarm confirmation\n"
                         "+++++++ press 2  to  invoke the alarm rinseback confirm\n"
                         "+++++++ press 3  to  invoke the rinseback confirmation\n"
                         "+++++++ press 4  to  invoke the end run confirmation\n"
                         "+++++++ press 5  to  invoke the spillover confirmation\n"
                         "+++++++ press 6  to  invoke the air in plasma confirmation\n"
                         "+++++++ enter selection -> ");

                  switch (get_switch_value())
                  {

                     case 1 :
                        strcpy(screen_choice.alloc_param, "ALARM") ;
                        break;

                     case 2 :
                        strcpy(screen_choice.alloc_param, "RINSE") ;
                        break;

                     case 3 :
                        strcpy(screen_choice.alloc_param, "RINSEBACK") ;
                        break;

                     case 4 :
                        strcpy(screen_choice.alloc_param, "USER") ;
                        break;

                     case 5 :
                        strcpy(screen_choice.alloc_param, "SPILLOVER") ;
                        break;

                     case 6 :
                        strcpy(screen_choice.alloc_param, "AIR_IN_PLASMA") ;
                        break;

                     default :
                        printf("\n +**** UNKNOWN SPECIFIC SCREEN SELECTION - start over ****+\n");

                  }           // End of switch statement

                  screen_choice.screen_id   = GUI_SCREEN_ABORTCNF ;

                  Msg_Ptr_Swap_Child_Screen = new Msg_Swap_Child_Screen;
                  Msg_Ptr_Swap_Child_Screen -> set (&screen_choice);
                  confirm_msg();
                  break;

               case 6 :
                  printf("\n +**** UNKNOWN LISTBOX SCREEN SELECTION - start over ****+\n");
                  break;

               case 7 :
                  break;

               default :
                  printf("\n +**** UNKNOWN CHILD SCREEN SELECTION - start over ****+\n");

            }         // End of switch statement
         }
         break;

         case 5 :
         {
            printf("++++ press  1  to  remove all stateless screens\n"
                   "++++ press  2  to  remove most recent stateless screen\n"
                   "++++ press  3  to  swap stateless screen\n"
                   "++++ press  4  to  swap stateless screen with the stack\n"
                   "++++ press  5  to  invoke the ALARM screen\n"
                   "++++ press  6  to  invoke the ABORTCNF screen\n"
                   "++++ press  7  to  invoke the ADJUST screen\n"
                   "++++ press  8  to  invoke the PREDICT screen\n"
                   "++++ press  9  to  EXIT\n"
                   "++++ enter selection -> "
                   );

            // Get the choice
            switch (get_switch_value())
            {

               case 1 :
                  send_int32_message (focusInt32Msg::RemoveAllStatelessScreens, 0);
                  break;

               case 2 :
                  send_int32_message (focusInt32Msg::RemoveMostRecentStatelessScreen, 0);
                  break;

               case 3 :
                  printf("+++++++ press  1  to  swap stateless screen ABORTCNF\n"
                         "+++++++ press  2  to  swap stateless screen ADJUST\n"
                         "+++++++ press  3  to  swap stateless screen PREDICT\n"
                         "+++++++ enter selection -> ");

                  switch (get_switch_value())
                  {

                     case 1 :
                        printf("++++++++++ press 1  to invoke the alarm confirmation\n"
                               "++++++++++ press 2  to invoke the alarm rinseback confirm\n"
                               "++++++++++ press 3  to invoke the rinseback confirmation\n"
                               "++++++++++ press 4  to invoke the end run confirmation\n"
                               "++++++++++ press 5  to invoke the spillover confirmation\n"
                               "++++++++++ press 6  to invoke the air in plasma confirmation\n"
                               "++++++++++ enter selection -> ");

                        switch (get_switch_value())
                        {

                           case 1 :
                              strcpy(screen_choice.alloc_param, "ALARM") ;
                              break;

                           case 2 :
                              strcpy(screen_choice.alloc_param, "RINSE") ;
                              break;

                           case 3 :
                              strcpy(screen_choice.alloc_param, "RINSEBACK") ;
                              break;

                           case 4 :
                              strcpy(screen_choice.alloc_param, "USER") ;
                              break;

                           case 5 :
                              strcpy(screen_choice.alloc_param, "SPILLOVER") ;
                              break;

                           case 6 :
                              strcpy(screen_choice.alloc_param, "AIR_IN_PLASMA") ;
                              break;

                           default :
                              printf("\n +**** UNKNOWN SPECIFIC SCREEN SELECTION - start over ****+\n");

                        }             // End of switch statement

                        screen_choice.screen_id       = GUI_SCREEN_ABORTCNF ;

                        Msg_Ptr_Swap_Stateless_Screen = new Msg_Swap_Stateless_Screen;
                        Msg_Ptr_Swap_Stateless_Screen -> set (&screen_choice);

                        confirm_msg();
                        break;

                     case 2 :
                        screen_choice.screen_id = GUI_SCREEN_TROUBLE ;
                        strcpy(screen_choice.alloc_param, "Parameter Here") ;

                        Msg_Ptr_Swap_Stateless_Screen = new Msg_Swap_Stateless_Screen;
                        Msg_Ptr_Swap_Stateless_Screen -> set (&screen_choice);

                        confirm_msg();
                        break;

                     case 3 :
                        printf("++++++++++ press 1  to  invoke from doninfo w no delta\n"
                               "++++++++++ press 2  to  invoke from doninfo w delta\n"
                               "++++++++++ press 3  to  invoke from adjust w no delta\n"
                               "++++++++++ press 4  to  invoke from adjut w delta\n"
                               "++++++++++ enter selection -> ");

                        switch (get_switch_value())
                        {

                           case 1 :
                              sprintf(predict_call_type, "%d", (int)DONINFO_NO_DELTA);
                              break;

                           case 2 :
                              sprintf(predict_call_type, "%d", (int)DONINFO_DELTA);
                              break;

                           case 3 :
                              sprintf(predict_call_type, "%d", (int)ADJUST_NO_DELTA);
                              break;

                           case 4 :
                              sprintf(predict_call_type, "%d", (int)ADJUST_DELTA);
                              break;

                           default :
                              printf("\n +**** UNKNOWN SPECIFIC SCREEN SELECTION - start over ****+\n");

                        }             // End of switch statement

                        strcpy(screen_choice.alloc_param, predict_call_type) ;
                        screen_choice.screen_id       = GUI_SCREEN_PREDICT ;

                        Msg_Ptr_Swap_Stateless_Screen = new Msg_Swap_Stateless_Screen;
                        Msg_Ptr_Swap_Stateless_Screen -> set (&screen_choice);

                        confirm_msg();
                        break;

                     default :
                        printf("\n +**** UNKNOWN SPECIFIC SCREEN SELECTION - start over ****+\n");

                  }           // End of switch statement

                  break;

               case 4 :
                  printf("+++++++ press  1  to  swap stateless stack w screen ALARM\n"
                         "+++++++ press  2  to  swap stateless stack w screen ABORTCNF\n"
                         "+++++++ press  3  to  swap stateless stack w screen ADJUST\n"
                         "+++++++ press  4  to  swap stateless stack w screen PREDICT\n"
                         "+++++++ enter selection -> ");

                  switch (get_switch_value())
                  {

                     case 1 :
                        select_alarm () ;
                        break;

                     case 2 :
                        printf("++++++++++ press 1  to invoke the alarm confirmation\n"
                               "++++++++++ press 2  to invoke the alarm rinseback confirm\n"
                               "++++++++++ press 3  to invoke the rinseback confirmation\n"
                               "++++++++++ press 4  to invoke the end run confirmation\n"
                               "++++++++++ press 5  to invoke the spillover confirmation\n"
                               "++++++++++ press 6  to invoke the air in plasma confirmation\n"
                               "++++++++++ enter selection -> ");

                        switch (get_switch_value())
                        {

                           case 1 :
                              strcpy(screen_choice.alloc_param, "ALARM") ;
                              break;

                           case 2 :
                              strcpy(screen_choice.alloc_param, "RINSE") ;
                              break;

                           case 3 :
                              strcpy(screen_choice.alloc_param, "RINSEBACK") ;
                              break;

                           case 4 :
                              strcpy(screen_choice.alloc_param, "USER") ;
                              break;

                           case 5 :
                              strcpy(screen_choice.alloc_param, "SPILLOVER") ;
                              break;

                           case 6 :
                              strcpy(screen_choice.alloc_param, "AIR_IN_PLASMA") ;
                              break;

                           default :
                              printf("\n +**** UNKNOWN SPECIFIC SCREEN SELECTION - start over ****+\n");

                        }             // End of switch statement

                        screen_choice.screen_id             = GUI_SCREEN_ABORTCNF ;

                        Msg_Ptr_Swap_Stateless_Stack_Screen = new Msg_Swap_Stateless_Stack_Screen;
                        Msg_Ptr_Swap_Stateless_Stack_Screen -> set (&screen_choice);

                        confirm_msg();
                        break;

                     case 3 :
                        screen_choice.screen_id = GUI_SCREEN_TROUBLE ;
                        strcpy(screen_choice.alloc_param, "Parameter Here") ;

                        Msg_Ptr_Swap_Stateless_Stack_Screen = new Msg_Swap_Stateless_Stack_Screen;
                        Msg_Ptr_Swap_Stateless_Stack_Screen -> set (&screen_choice);
                        confirm_msg();
                        break;

                     case 4 :
                        printf("+++++++ press 1  to  invoke from doninfo w no delta\n"
                               "+++++++ press 2  to  invoke from doninfo w delta\n"
                               "+++++++ press 3  to  invoke from adjust w no delta\n"
                               "+++++++ press 4  to  invoke from adjut w delta\n"
                               "+++++++ enter selection -> ");

                        switch (get_switch_value())
                        {

                           case 1 :
                              sprintf(predict_call_type, "%d", (int)DONINFO_NO_DELTA);
                              break;

                           case 2 :
                              sprintf(predict_call_type, "%d", (int)DONINFO_DELTA);
                              break;

                           case 3 :
                              sprintf(predict_call_type, "%d", (int)ADJUST_NO_DELTA);
                              break;

                           case 4 :
                              sprintf(predict_call_type, "%d", (int)ADJUST_DELTA);
                              break;

                           default :
                              printf("\n +**** UNKNOWN SPECIFIC SCREEN SELECTION - start over ****+\n");

                        }             // End of switch statement

                        strcpy(screen_choice.alloc_param, predict_call_type) ;
                        screen_choice.screen_id             = GUI_SCREEN_PREDICT ;

                        Msg_Ptr_Swap_Stateless_Stack_Screen = new Msg_Swap_Stateless_Stack_Screen;
                        Msg_Ptr_Swap_Stateless_Stack_Screen -> set (&screen_choice);

                        confirm_msg();
                        break;

                     default :
                        printf("\n +**** UNKNOWN SPECIFIC SCREEN SELECTION - start over ****+\n");

                  }           // End of switch statement

                  break;

               case 5 :
                  select_alarm () ;
                  break;

               case 6 :
                  printf("+++++++ press 1  to  invoke the alarm confirmation\n"
                         "+++++++ press 2  to  invoke the alarm rinseback confirm\n"
                         "+++++++ press 3  to  invoke the rinseback confirmation\n"
                         "+++++++ press 4  to  invoke the end run confirmation\n"
                         "+++++++ press 5  to  invoke the spillover confirmation\n"
                         "+++++++ press 6  to  invoke the air in plasma confirmation\n"
                         "+++++++ enter selection -> ");

                  switch (get_switch_value())
                  {

                     case 1 :
                        strcpy(screen_choice.alloc_param, "ALARM") ;
                        break;

                     case 2 :
                        strcpy(screen_choice.alloc_param, "RINSE") ;
                        break;

                     case 3 :
                        strcpy(screen_choice.alloc_param, "RINSEBACK") ;
                        break;

                     case 4 :
                        strcpy(screen_choice.alloc_param, "USER") ;
                        break;

                     case 5 :
                        strcpy(screen_choice.alloc_param, "SPILLOVER") ;
                        break;

                     case 6 :
                        strcpy(screen_choice.alloc_param, "AIR_IN_PLASMA") ;
                        break;

                     default :
                        printf("\n +**** UNKNOWN SPECIFIC SCREEN SELECTION - start over ****+\n");

                  }           // End of switch statement

                  screen_choice.screen_id          = GUI_SCREEN_ABORTCNF ;

                  Msg_Ptr_Display_Stateless_Screen = new Msg_Display_Stateless_Screen;
                  Msg_Ptr_Display_Stateless_Screen -> set (&screen_choice);

                  confirm_msg();
                  break;

               case 7 :
                  screen_choice.screen_id = GUI_SCREEN_TROUBLE ;
                  strcpy(screen_choice.alloc_param, "Parameter Here") ;

                  Msg_Ptr_Display_Stateless_Screen = new Msg_Display_Stateless_Screen;
                  Msg_Ptr_Display_Stateless_Screen -> set (&screen_choice);

                  confirm_msg();
                  break;

               case 8 :
                  printf("+++++++ press 1  to  invoke from doninfo w no delta\n"
                         "+++++++ press 2  to  invoke from doninfo w delta\n"
                         "+++++++ press 3  to  invoke from adjust w no delta\n"
                         "+++++++ press 4  to  invoke from adjut w delta\n"
                         "+++++++ enter selection -> ");

                  switch (get_switch_value())
                  {

                     case 1 :
                        sprintf(predict_call_type, "%d", (int)DONINFO_NO_DELTA);
                        break;

                     case 2 :
                        sprintf(predict_call_type, "%d", (int)DONINFO_DELTA);
                        break;

                     case 3 :
                        sprintf(predict_call_type, "%d", (int)ADJUST_NO_DELTA);
                        break;

                     case 4 :
                        sprintf(predict_call_type, "%d", (int)ADJUST_DELTA);
                        break;

                     default :
                        printf("\n +**** UNKNOWN SPECIFIC SCREEN SELECTION - start over ****+\n");

                  }           // End of switch statement

                  strcpy(screen_choice.alloc_param, predict_call_type) ;
                  screen_choice.screen_id          = GUI_SCREEN_PREDICT ;

                  Msg_Ptr_Display_Stateless_Screen = new Msg_Display_Stateless_Screen;
                  Msg_Ptr_Display_Stateless_Screen -> set (&screen_choice);

                  confirm_msg();
                  break;

               case 9 :
                  break;

               default :
                  printf("\n +**** UNKNOWN STATELESS SCREEN SELECTION - start over ****+\n");

            }         // End of switch statement
         }
         break;

         case 6 :    // 6
         {
            printf("+++ press  1 for DONVITAL screen messages\n"
                   "+++ press  2 for DONINFO  screen messages\n"
                   "+++ press  3 for PREDICT  screen messages\n"
                   "+++ press  4 for RUNPROC  screen messages\n"
                   "+++ press  5 for ADJUST   screen messages\n"
                   "+++ press  6 for RUNSUMM1, LABELPRD, VERIFPRD screen message\n"
                   "+++ press  7 for RUNSUMM2 screen messages\n"
                   "+++ press  8 for DONORDL  screen messages\n"
                   "+++ press  9 for SYSDONOR Vista screen messages\n"
                   "+++ press 10 for SERVICE  screen meter messages\n"
                   "+++ press 11 to  EXIT\n"
                   "+++ enter selection -> "
                   );

            // Get the choice
            switch (get_switch_value())
            {

               case 1 :         // 6-1
                  printf("+++++++ enter the number for the donvital message type\n"
                         "+++++++ 1  for  preset donvital screen message\n"
                         "+++++++   sex      height      weight       tbv       units\n"
                         "+++++++    M        6'3''      235lbs      6579ml     ft/lb\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 2  for  preset donvital screen message\n"
                         "+++++++   sex      height      weight       tbv       units\n"
                         "+++++++    F        5'5''      128lbs      3711ml     ft/lb\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 3  for  preset donvital screen message\n"
                         "+++++++   sex      height      weight       tbv       units\n"
                         "+++++++    M        183cm       87kgs      5653ml     cm/kg\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 4  for  preset donvital screen message\n"
                         "+++++++   sex      height      weight       tbv       units\n"
                         "+++++++    F        160cm       54kgs      3428ml     cm/kg\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 5  for a custom donvital screen message\n"
                         "+++++++ 6  to  EXIT\n"
                         "+++++++ enter selection -> "
                         );

                  switch (get_switch_value())
                  {

                     case 1 :            // 6-1-1

                        donor_vitals.cDonorSex         = DONOR_SEX_IS_MALE;                //
                        donor_vitals.cDonorHeightUnits = HEIGHT_UNITS_IN;                  //
                        donor_vitals.cDonorWeightUnits = WEIGHT_UNITS_LB;                  //
                        donor_vitals.fDonorHeight      = 75.0;                             //
                        donor_vitals.fDonorWeight      = 235.0;                            //
                        donor_vitals.fSafetyDonorTBV   = 6579.0;                           //

                        donor_vitals_ptr               = new focusBufferMsg<SDonorVitals>(BufferMessages::SafetyToGuiDonorVitals) ;
                        // Actually send the message
                        donor_vitals_ptr -> set(&donor_vitals);


                        confirm_msg();
                        break;

                     case 2 :            // 6-1-2

                        donor_vitals.cDonorSex         = DONOR_SEX_IS_FEMALE;  //
                        donor_vitals.cDonorHeightUnits = HEIGHT_UNITS_IN;      //
                        donor_vitals.cDonorWeightUnits = WEIGHT_UNITS_LB;      //
                        donor_vitals.fDonorHeight      = 65.0;                 //
                        donor_vitals.fDonorWeight      = 128.0;                //
                        donor_vitals.fSafetyDonorTBV   = 3711.0;               //

                        donor_vitals_ptr               = new focusBufferMsg<SDonorVitals>(BufferMessages::SafetyToGuiDonorVitals) ;
                        // Actually send the message
                        donor_vitals_ptr -> set(&donor_vitals);


                        confirm_msg();
                        break;

                     case 3 :            // 6-1-2

                        donor_vitals.cDonorSex         = DONOR_SEX_IS_MALE;   //
                        donor_vitals.cDonorHeightUnits = HEIGHT_UNITS_CM;     //
                        donor_vitals.cDonorWeightUnits = WEIGHT_UNITS_KG;     //
                        donor_vitals.fDonorHeight      = 183.0;               //
                        donor_vitals.fDonorWeight      = 87.0;                //
                        donor_vitals.fSafetyDonorTBV   = 5653.0;              //

                        donor_vitals_ptr               = new focusBufferMsg<SDonorVitals>(BufferMessages::SafetyToGuiDonorVitals) ;
                        // Actually send the message
                        donor_vitals_ptr -> set(&donor_vitals);


                        confirm_msg();
                        break;

                     case 4 :            // 6-1-4

                        donor_vitals.cDonorSex         = DONOR_SEX_IS_FEMALE; //
                        donor_vitals.cDonorHeightUnits = HEIGHT_UNITS_CM;     //
                        donor_vitals.cDonorWeightUnits = WEIGHT_UNITS_KG;     //
                        donor_vitals.fDonorHeight      = 160.0;               //
                        donor_vitals.fDonorWeight      = 54.0;                //
                        donor_vitals.fSafetyDonorTBV   = 3428.0;              //

                        donor_vitals_ptr               = new focusBufferMsg<SDonorVitals>(BufferMessages::SafetyToGuiDonorVitals) ;
                        // Actually send the message
                        donor_vitals_ptr -> set(&donor_vitals);


                        confirm_msg();
                        break;

                     case 5 :            // 6-1-5
                        donvital_true_one = 1;

                        while (donvital_true_one == 1)
                        {

                           printf("++++++++++ enter the number for specific donvital message\n"
                                  "++++++++++ 1  for donor female sex message\n"
                                  "++++++++++ 2  for donor male sex message\n"
                                  "++++++++++ 3  for donor height message\n"
                                  "++++++++++ 4  for donor weight message\n"
                                  "++++++++++ 5  for donor tbv message\n"
                                  "++++++++++ 6  for english/metric units message\n"
                                  "++++++++++ 7  to  clear all entries\n"
                                  "++++++++++ 8  to  EXIT\n"
                                  "++++++++++ enter selection -> "
                                  );

                           switch (get_switch_value())
                           {
                              case 1 :              // 6-1-5-1
                                 donor_vitals.cDonorSex = DONOR_SEX_IS_FEMALE;
                                 break;

                              case 2 :               // 6-1-5-2
                                 donor_vitals.cDonorSex = DONOR_SEX_IS_MALE;
                                 break;

                              case 3 :               // 6-1-5-3
                                 printf("+++++++++++++ enter height in inches/cms -> ");

                                 if ((donor_vitals.cDonorHeightUnits == HEIGHT_UNITS_IN) || (donor_vitals.cDonorHeightUnits == HEIGHT_UNITS_CM))
                                 {
                                    get_input(&donvitalsheight);
                                    donor_vitals.fDonorHeight = donvitalsheight;
                                 }
                                 else
                                 {
                                    printf("\n+++++++++++++ enter height units first !\n");
                                 }

                                 break;

                              case 4 :               // 6-1-5-4
                                 printf("+++++++++++++ enter weight in lbs/kgs -> ");

                                 if ((donor_vitals.cDonorWeightUnits == WEIGHT_UNITS_LB) || (donor_vitals.cDonorWeightUnits == WEIGHT_UNITS_KG))
                                 {
                                    get_input(&donvitalsweight);
                                    donor_vitals.fDonorWeight = donvitalsweight;
                                 }
                                 else
                                 {
                                    printf("\n+++++++++++++ enter weight units first !\n");
                                 }

                                 break;

                              case 5 :               // 6-1-5-5
                                 printf("+++++++++++++ enter tbv -> ");
                                 get_input(&donvitalstbv);
                                 donor_vitals.fSafetyDonorTBV = donvitalstbv;
                                 break;

                              case 6 :               // 6-1-5-6
                                 printf("+++++++++++++ enter the number for the type of units\n"
                                        "+++++++++++++ 1  for english (ft/lb)\n"
                                        "+++++++++++++ 2  for metric (cm/kg)\n"
                                        "+++++++++++++ enter selection -> ");
                                 switch (get_switch_value())
                                 {
                                    case 1 :                 // 6-1-5-6-1
                                       memset(&donor_vitals, 0, sizeof(donor_vitals));
                                       donor_vitals.cDonorHeightUnits = HEIGHT_UNITS_IN;
                                       donor_vitals.cDonorWeightUnits = WEIGHT_UNITS_LB;
                                       break;

                                    case 2 :                 // 6-1-5-6-2
                                       memset(&donor_vitals, 0, sizeof(donor_vitals));
                                       donor_vitals.cDonorHeightUnits = HEIGHT_UNITS_CM;
                                       donor_vitals.cDonorWeightUnits = WEIGHT_UNITS_KG;
                                       break;

                                    default :
                                       printf("\n +**** UNKNOWN HEIGHT UNITS MESSAGE - start over ****+\n");
                                 }

                                 break;

                              case 7 :               // 6-1-5-7
                                 memset(&donor_vitals, 0, sizeof(donor_vitals));
                                 break;

                              case 8 :               // 6-1-5-8
                                 donvital_true_one = 0;
                                 break;

                              default :
                                 printf("\n +**** UNKNOWN SPECIFIC SCREEN MESSAGE - start over ****+\n");
                                 donvital_true_one = 0;

                           }


                           donor_vitals_ptr = new focusBufferMsg<SDonorVitals>(BufferMessages::SafetyToGuiDonorVitals) ;
                           // Actually send the message
                           donor_vitals_ptr -> set(&donor_vitals);


                        }             // End of while statement
                        break;

                     case 6 :            // 6-1-6
                        break;

                     default :
                        printf("\n +**** UNKNOWN DONVITAL SCREEN MESSAGE - start over ****+\n");

                  }
                  break;

               case 2 :          // 6-2
                  printf("+++++++ enter the number for the doninfo message type\n"
                         "+++++++ 1  for  preset doninfo screen message\n"
                         "+++++++   btype        hct         platelets         tbv\n"
                         "+++++++     O+          48         244x10^11        6579ml\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 2  for  preset doninfo screen message\n"
                         "+++++++   btype        hct         platelets         tbv\n"
                         "+++++++    AB-          43         272x10^11        3711ml\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 3  for  preset doninfo screen message\n"
                         "+++++++   btype        hct         platelets         tbv\n"
                         "+++++++     A+          50         361x10^11        5653ml\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 4  for  preset doninfo screen message\n"
                         "+++++++   btype        hct         platelets         tbv\n"
                         "+++++++    none         41         229x10^11        3428ml\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 5  for a custom doninfo screen message\n"
                         "+++++++ 6  to  EXIT\n"
                         "+++++++ enter selection -> "
                         );

                  switch (get_switch_value())
                  {

                     case 1 :            // 6-2-1

                        donor_vitals.fSafetyDonorTBV   = 6579.0;               //
                        donor_vitals.fDonorHct         = 48.0;                 //
                        donor_vitals.fDonorPltPrecount = 244.0;                //
                        donor_vitals.DonorBloodType    = BT_O_POS;

                        donor_vitals_ptr               = new focusBufferMsg<SDonorVitals>(BufferMessages::SafetyToGuiDonorVitals) ;
                        // Actually send the message
                        donor_vitals_ptr -> set(&donor_vitals);


                        confirm_msg();
                        break;

                     case 2 :            // 6-2-2

                        donor_vitals.fSafetyDonorTBV   = 3711.0;               //
                        donor_vitals.fDonorHct         = 43.0;                 //
                        donor_vitals.fDonorPltPrecount = 272.0;                //
                        donor_vitals.DonorBloodType    = BT_AB_NEG;

                        donor_vitals_ptr               = new focusBufferMsg<SDonorVitals>(BufferMessages::SafetyToGuiDonorVitals) ;
                        // Actually send the message
                        donor_vitals_ptr -> set(&donor_vitals);


                        confirm_msg();
                        break;

                     case 3 :            // 6-2-3

                        donor_vitals.fSafetyDonorTBV   = 5653.0;               //
                        donor_vitals.fDonorHct         = 50.0;                 //
                        donor_vitals.fDonorPltPrecount = 361.0;                //
                        donor_vitals.DonorBloodType    = BT_A_POS;

                        donor_vitals_ptr               = new focusBufferMsg<SDonorVitals>(BufferMessages::SafetyToGuiDonorVitals) ;
                        // Actually send the message
                        donor_vitals_ptr -> set(&donor_vitals);


                        confirm_msg();
                        break;

                     case 4 :            // 6-2-4

                        donor_vitals.fSafetyDonorTBV   = 3428.0;               //
                        donor_vitals.fDonorHct         = 41.0;                 //
                        donor_vitals.fDonorPltPrecount = 229.0;                //
                        donor_vitals.DonorBloodType    = BT_UNKNOWN ;

                        donor_vitals_ptr               = new focusBufferMsg<SDonorVitals>(BufferMessages::SafetyToGuiDonorVitals) ;
                        // Actually send the message
                        donor_vitals_ptr -> set(&donor_vitals);


                        confirm_msg();
                        break;

                     case 5 :            // 6-2-5
                        doninfo_true_one = 1;

                        while (doninfo_true_one == 1)
                        {

                           printf("++++++++++ enter the number for specific doninfo message\n"
                                  "++++++++++ 1  for donor blood type message\n"
                                  "++++++++++ 2  for donor hematocrit message\n"
                                  "++++++++++ 3  for donor platelet precount message\n"
                                  "++++++++++ 4  for donor tbv message\n"
                                  "++++++++++ 5  to  clear all entries\n"
                                  "++++++++++ 6  to  EXIT\n"
                                  "++++++++++ enter selection -> "
                                  );

                           switch (get_switch_value())
                           {
                              case 1 :               // 6-2-5-1
                                 printf("+++++++++++++ enter the number for specific blood type\n"
                                        "+++++++++++++ 1  for donor blood type A+\n"
                                        "+++++++++++++ 2  for donor blood type A-\n"
                                        "+++++++++++++ 3  for donor blood type AB+\n"
                                        "+++++++++++++ 4  for donor blood type AB-\n"
                                        "+++++++++++++ 5  for donor blood type B+\n"
                                        "+++++++++++++ 6  for donor blood type B-\n"
                                        "+++++++++++++ 7  for donor blood type O+\n"
                                        "+++++++++++++ 8  for donor blood type O-\n"
                                        "+++++++++++++ 9  for no donor blood type\n"
                                        "+++++++++++++ 10 to  EXIT\n"
                                        "+++++++++++++ enter selection -> ");

                                 switch (get_switch_value())
                                 {
                                    case 1 :                 // 6-2-5-1-1
                                       donor_vitals.DonorBloodType = BT_A_POS;
                                       break;

                                    case 2 :                 // 6-2-5-1-2
                                       donor_vitals.DonorBloodType = BT_A_NEG;
                                       break;

                                    case 3 :                 // 6-2-5-1-3
                                       donor_vitals.DonorBloodType = BT_AB_POS;
                                       break;

                                    case 4 :                 // 6-2-5-1-4
                                       donor_vitals.DonorBloodType = BT_AB_NEG;
                                       break;

                                    case 5 :                 // 6-2-5-1-5
                                       donor_vitals.DonorBloodType = BT_B_POS;
                                       break;

                                    case 6 :                 // 6-2-5-1-6
                                       donor_vitals.DonorBloodType = BT_B_NEG;
                                       break;

                                    case 7 :                 // 6-2-5-1-7
                                       donor_vitals.DonorBloodType = BT_O_POS;
                                       break;

                                    case 8 :                 // 6-2-5-1-8
                                       donor_vitals.DonorBloodType = BT_O_NEG;
                                       break;

                                    case 9 :                 // 6-2-5-1-9
                                       donor_vitals.DonorBloodType = BT_UNKNOWN;
                                       break;

                                    case 10 :                 // 6-2-5-1-10
                                       break;

                                    default :
                                       printf("\n +**** UNKNOWN SPECIFIC SCREEN MESSAGE - start over ****+\n");

                                 }
                                 break;

                              case 2 :               // 6-2-5-2
                                 printf("+++++++++++++ enter hct -> ");
                                 get_input(&doninfohct);
                                 donor_vitals.fDonorHct = doninfohct;
                                 break;

                              case 3 :               // 6-2-5-3
                                 printf("+++++++++++++ enter platelet precount -> ");
                                 get_input(&doninfoplateletprecount);
                                 donor_vitals.fDonorPltPrecount = doninfoplateletprecount;
                                 break;

                              case 4 :               // 6-2-5-4
                                 printf("+++++++++++ enter tbv -> ");
                                 get_input(&doninfotbv);
                                 donor_vitals.fSafetyDonorTBV = doninfotbv;
                                 break;

                              case 5 :               // 6-2-5-5
                                 memset(&donor_vitals, 0, sizeof(donor_vitals));
                                 break;

                              case 6 :               // 6-2-5-6
                                 doninfo_true_one = 0;
                                 break;

                              default :
                                 printf("\n +**** UNKNOWN SPECIFIC SCREEN MESSAGE - start over ****+\n");
                                 doninfo_true_one = 0;

                           }


                           donor_vitals_ptr = new focusBufferMsg<SDonorVitals>(BufferMessages::SafetyToGuiDonorVitals) ;
                           // Actually send the message
                           donor_vitals_ptr -> set(&donor_vitals);


                        }             // End of while statement

                        break;

                     case 6 :            // 6-2-6
                        break;

                     default :
                        printf("\n +**** UNKNOWN DONINFO SCREEN MESSAGE - start over ****+\n");

                  }
                  break;

               case 3 :          // 6-3
                  printf("+++++++ enter the number for the predict message type\n"
                         "+++++++ 1  for  preset predict screen message, (SAME PROC)\n"
                         "+++++++    platelet       plasma         rbc          time\n"
                         "+++++++   0.0x10^11         0ml           0ml          0min\n"
                         "+++++++   3.5x10^11       200ml           0ml         55min\n"
                         "+++++++   6.0x10^11         0ml           0ml         78min\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 2  for  preset predict screen message, (NEW PROC)\n"
                         "+++++++    platelet       plasma         rbc          time\n"
                         "+++++++   3.5x10^11       200ml           0ml         55min\n"
                         "+++++++   6.0x10^11         0ml           0ml         78min\n"
                         "+++++++   6.0x10^11         0ml         150ml         88min\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 3  for  preset predict screen message, (NEW PROC)\n"
                         "+++++++    platelet       plasma         rbc          time\n"
                         "+++++++   6.0x10^11         0ml           0ml         78min\n"
                         "+++++++   6.0x10^11         0ml         150ml         88min\n"
                         "+++++++   3.5x10^11       200ml         150ml         74min\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 4  for  preset predict screen message, (NEW PROC)\n"
                         "+++++++    platelet       plasma         rbc          time\n"
                         "+++++++   6.0x10^11         0ml         150ml         88min\n"
                         "+++++++   3.5x10^11       200ml         150ml         74min\n"
                         "+++++++   0.0x10^11         0ml           0ml          0min\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 5  for  preset predict screen message, (NEW PROC)\n"
                         "+++++++    platelet       plasma         rbc          time\n"
                         "+++++++   0.0x10^11         0ml           0ml          0min\n"
                         "+++++++   0.0x10^11         0ml           0ml          0min\n"
                         "+++++++   0.0x10^11         0ml           0ml          0min\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 6  OBSOLETE: for a NEW PROCEDURE predict screen message\n"
                         "+++++++ 7  OBSOLETE: for a SAME PROCEDURE predict screen message\n"
                         "+++++++ 8  for a custom predict screen message\n"
                         "+++++++ 9  to  EXIT\n"
                         "+++++++ enter selection -> "
                         );

                  switch (get_switch_value())
                  {

                     case 1 :            // 6-3-1

                        proc_predict.previous_option.estimated_platelets = 0.0f;
                        proc_predict.previous_option.estimated_plasma    = 0.0f;
                        proc_predict.previous_option.estimated_rbcs      = 0.0f;
                        proc_predict.previous_option.procedure_time      = 0.0f;
                        proc_predict.current_option.estimated_platelets  = 3.0;
                        proc_predict.current_option.estimated_plasma     = 200.0f;
                        proc_predict.current_option.estimated_rbcs       = 0.0f;
                        proc_predict.current_option.procedure_time       = 55.0f;
                        proc_predict.next_option.estimated_platelets     = 6.0f;
                        proc_predict.next_option.estimated_plasma        = 0.0f;
                        proc_predict.next_option.estimated_rbcs          = 0.0f;
                        proc_predict.next_option.procedure_time          = 78.0f;
                        // proc_predict.procedure_change = SAME_PROCEDURE;

                        proc_predict_ptr = new focusBufferMsg<ProcedureInformationStruct>(BufferMessages::ProcedureInformation);
                        // Actually send the message
                        proc_predict_ptr -> set(&proc_predict);


                        confirm_msg();
                        break;

                     case 2 :            // 6-3-2

                        proc_predict.previous_option.estimated_platelets    = 3.0f;
                        proc_predict.previous_option.estimated_plasma       = 200.0f;
                        proc_predict.previous_option.estimated_rbcs         = 0.0f;
                        proc_predict.previous_option.procedure_time         = 55.0f;
                        proc_predict.current_option.estimated_platelets     = 6.0;
                        proc_predict.current_option.estimated_plasma        = 0.0f;
                        proc_predict.current_option.estimated_rbcs          = 0.0f;
                        proc_predict.current_option.procedure_time          = 78.0f;
                        proc_predict.next_option.estimated_platelets        = 6.0f;
                        proc_predict.next_option.estimated_plasma           = 0.0f;
                        proc_predict.next_option.estimated_rbcs             = 150.0f;
                        proc_predict.next_option.replacement_fluid_required = 1;
                        proc_predict.next_option.count_icon_needed          = 1;
                        proc_predict.next_option.procedure_time             = 88.0f;
                        // proc_predict.procedure_change = NEW_PROCEDURE;

                        proc_predict_ptr = new focusBufferMsg<ProcedureInformationStruct>(BufferMessages::ProcedureInformation);
                        // Actually send the message
                        proc_predict_ptr -> set(&proc_predict);


                        confirm_msg();
                        break;

                     case 3 :            // 6-3-3

                        proc_predict.previous_option.estimated_platelets       = 6.0f;
                        proc_predict.previous_option.estimated_plasma          = 0.0f;
                        proc_predict.previous_option.estimated_rbcs            = 0.0f;
                        proc_predict.previous_option.procedure_time            = 78.0f;
                        proc_predict.current_option.estimated_platelets        = 6.0;
                        proc_predict.current_option.estimated_plasma           = 0.0f;
                        proc_predict.current_option.estimated_rbcs             = 150.0f;
                        proc_predict.current_option.replacement_fluid_required = 1;
                        proc_predict.current_option.count_icon_needed          = 1;
                        proc_predict.current_option.procedure_time             = 88.0f;
                        proc_predict.next_option.estimated_platelets           = 3.5f;
                        proc_predict.next_option.estimated_plasma              = 200.0f;
                        proc_predict.next_option.estimated_rbcs                = 150.0f;
                        proc_predict.next_option.replacement_fluid_required    = 1;
                        proc_predict.next_option.count_icon_needed             = 1;
                        proc_predict.next_option.procedure_time                = 74.0f;
                        // proc_predict.procedure_change = NEW_PROCEDURE;

                        proc_predict_ptr = new focusBufferMsg<ProcedureInformationStruct>(BufferMessages::ProcedureInformation);
                        // Actually send the message
                        proc_predict_ptr -> set(&proc_predict);


                        confirm_msg();
                        break;

                     case 4 :            // 6-3-4

                        proc_predict.previous_option.estimated_platelets        = 6.0f;
                        proc_predict.previous_option.estimated_plasma           = 0.0f;
                        proc_predict.previous_option.estimated_rbcs             = 150.0f;
                        proc_predict.previous_option.replacement_fluid_required = 1;
                        proc_predict.previous_option.count_icon_needed          = 1;
                        proc_predict.previous_option.procedure_time             = 88.0f;
                        proc_predict.current_option.estimated_platelets         = 3.5;
                        proc_predict.current_option.estimated_plasma            = 200.0f;
                        proc_predict.current_option.estimated_rbcs              = 150.0f;
                        proc_predict.current_option.replacement_fluid_required  = 1;
                        proc_predict.current_option.count_icon_needed           = 1;
                        proc_predict.current_option.procedure_time              = 74.0f;
                        proc_predict.next_option.estimated_platelets            = 0.0f;
                        proc_predict.next_option.estimated_plasma               = 0.0f;
                        proc_predict.next_option.estimated_rbcs                 = 0.0f;
                        proc_predict.next_option.procedure_time                 = 0.0f;
                        // proc_predict.procedure_change = NEW_PROCEDURE;

                        proc_predict_ptr = new focusBufferMsg<ProcedureInformationStruct>(BufferMessages::ProcedureInformation);
                        // Actually send the message
                        proc_predict_ptr -> set(&proc_predict);


                        confirm_msg();
                        break;

                     case 5 :            // 6-3-5

                        proc_predict.previous_option.estimated_platelets = 0.0f;
                        proc_predict.previous_option.estimated_plasma    = 0.0f;
                        proc_predict.previous_option.estimated_rbcs      = 0.0f;
                        proc_predict.previous_option.procedure_time      = 0.0f;
                        proc_predict.current_option.estimated_platelets  = 0.0;
                        proc_predict.current_option.estimated_plasma     = 0.0f;
                        proc_predict.current_option.estimated_rbcs       = 0.0f;
                        proc_predict.current_option.procedure_time       = 0.0f;
                        proc_predict.next_option.estimated_platelets     = 0.0f;
                        proc_predict.next_option.estimated_plasma        = 0.0f;
                        proc_predict.next_option.estimated_rbcs          = 0.0f;
                        proc_predict.next_option.procedure_time          = 0.0f;
                        // proc_predict.procedure_change = NEW_PROCEDURE;

                        proc_predict_ptr = new focusBufferMsg<ProcedureInformationStruct>(BufferMessages::ProcedureInformation);
                        // Actually send the message
                        proc_predict_ptr -> set(&proc_predict);


                        confirm_msg();
                        break;

                     case 6 :            // 6-3-6

                        memset(&proc_predict, 0, sizeof(proc_predict));
                        // proc_predict.procedure_change = NEW_PROCEDURE;

                        proc_predict_ptr = new focusBufferMsg<ProcedureInformationStruct>(BufferMessages::ProcedureInformation);
                        // Actually send the message
                        proc_predict_ptr -> set(&proc_predict);


                        confirm_msg();
                        break;

                     case 7 :            // 6-3-7

                        memset(&proc_predict, 0, sizeof(proc_predict));
                        // proc_predict.procedure_change = SAME_PROCEDURE;

                        proc_predict_ptr = new focusBufferMsg<ProcedureInformationStruct>(BufferMessages::ProcedureInformation);
                        // Actually send the message
                        proc_predict_ptr -> set(&proc_predict);


                        confirm_msg();
                        break;

                     case 8 :            // 6-3-8
                        predict_true_one = 1;

                        while (predict_true_one == 1)
                        {

                           printf("+++++++ enter the number for specific predict message\n"
                                  "+++++++  1 for previous option platelets message\n"
                                  "+++++++  2 for previous option plasma message\n"
                                  "+++++++  3 for previous option rbcs message\n"
                                  "+++++++  4 for previous option replacement fluid message\n"
                                  "+++++++  5 for previous option procedure time message\n"
                                  "+++++++  6 for previous option count magnify glass message\n"
                                  "+++++++  7 for current option platelets message\n"
                                  "+++++++  8 for current option plasma message\n"
                                  "+++++++  9 for current option rbcs message\n"
                                  "+++++++ 10 for current option replacement fluid message\n"
                                  "+++++++ 11 for current option procedure time message\n"
                                  "+++++++ 12 for current option count magnify glass message\n"
                                  "+++++++ 13 for next option platelets message\n"
                                  "+++++++ 14 for next option plasma message\n"
                                  "+++++++ 15 for next option rbcs message\n"
                                  "+++++++ 16 for next option replacement fluid message\n"
                                  "+++++++ 17 for next option procedure time message\n"
                                  "+++++++ 18 for next option count magnify glass message\n"
                                  "+++++++ 19  to  clear all entries\n"
                                  "+++++++ 20  to  EXIT\n"
                                  "+++++++ enter selection -> "
                                  );

                           switch (get_switch_value())
                           {
                              case 1 :               // 6-3-8-1
                                 printf("+++++++++++++ enter platelets -> ");
                                 get_input(&predictprevplatelets);
                                 proc_predict.previous_option.estimated_platelets = predictprevplatelets;
                                 break;

                              case 2 :               // 6-3-8-2
                                 printf("+++++++++++++ enter plasma -> ");
                                 get_input(&predictprevplasma);
                                 proc_predict.previous_option.estimated_plasma = predictprevplasma;
                                 break;

                              case 3 :               // 6-3-8-3
                                 printf("+++++++++++++ enter rbcs -> ");
                                 get_input(&predictprevrbcs);
                                 proc_predict.previous_option.estimated_rbcs = predictprevrbcs;
                                 break;

                              case 4 :               // 6-3-8-4
                                 printf("+++++++++++++ enter replacement fluid flag (1 or 0) -> ");
                                 get_input(&predictprevrepfluid);
                                 proc_predict.previous_option.replacement_fluid_required = predictprevrepfluid;
                                 break;

                              case 5 :
                                 printf("+++++++++++++ enter time -> ");
                                 get_input(&predictprevtime);
                                 proc_predict.previous_option.procedure_time = predictprevtime;
                                 break;

                              case 6 :
                                 printf("+++++++++++++ enter count magnify glass flag (1 or 0) -> ");
                                 get_input(&predictprevmagnify);
                                 proc_predict.previous_option.count_icon_needed = predictprevmagnify;
                                 break;

                              case 7 :
                                 printf("+++++++++++++ enter platelets -> ");
                                 get_input(&predictestplatelets);
                                 proc_predict.current_option.estimated_platelets = predictestplatelets;
                                 break;

                              case 8 :
                                 printf("+++++++++++++ enter plasma -> ");
                                 get_input(&predictestplasma);
                                 proc_predict.current_option.estimated_plasma = predictestplasma;
                                 break;

                              case 9 :
                                 printf("+++++++++++++ enter rbcs -> ");
                                 get_input(&predictestrbcs);
                                 proc_predict.current_option.estimated_rbcs = predictestrbcs;
                                 break;

                              case 10 :
                                 printf("+++++++++++++ enter replacement fluid flag (1 or 0) -> ");
                                 get_input(&predictestrepfluid);
                                 proc_predict.current_option.replacement_fluid_required = predictestrepfluid;
                                 break;

                              case 11 :
                                 printf("+++++++++++++ enter time -> ");
                                 get_input(&predictesttime);
                                 proc_predict.current_option.procedure_time = predictesttime;
                                 break;

                              case 12 :
                                 printf("+++++++++++++ enter count magnify glass flag (1 or 0) -> ");
                                 get_input(&predictestmagnify);
                                 proc_predict.current_option.count_icon_needed = predictestmagnify;
                                 break;
                              case 13 :
                                 printf("+++++++++++++ enter platelets -> ");
                                 get_input(&predictnextplatelets);
                                 proc_predict.next_option.estimated_platelets = predictnextplatelets;
                                 break;

                              case 14 :
                                 printf("+++++++++++++ enter plamsa -> ");
                                 get_input(&predictnextplasma);
                                 proc_predict.next_option.estimated_plasma = predictnextplasma;
                                 break;

                              case 15 :
                                 printf("+++++++++++++ enter rbcs -> ");
                                 get_input(&predictnextrbcs);
                                 proc_predict.next_option.estimated_rbcs = predictnextrbcs;
                                 break;

                              case 16 :
                                 printf("+++++++++++++ enter replacement fluid flag (1 or 0) -> ");
                                 get_input(&predictnextrepfluid);
                                 proc_predict.next_option.replacement_fluid_required = predictnextrepfluid;
                                 break;

                              case 17 :
                                 printf("+++++++++++++ enter time -> ");
                                 get_input(&predictnexttime);
                                 proc_predict.next_option.procedure_time = predictnexttime;
                                 break;

                              case 18 :
                                 printf("+++++++++++++ enter count magnify glass flag (1 or 0) -> ");
                                 get_input(&predictnextmagnify);
                                 proc_predict.next_option.count_icon_needed = predictnextmagnify;
                                 break;
                              case 19 :
                                 memset(&proc_predict, 0, sizeof(proc_predict));
                                 break;

                              case 20 :
                                 predict_true_one = 0;
                                 break;

                              default :
                                 printf("\n +**** UNKNOWN SPECIFIC SCREEN MESSAGE - start over ****+\n");
                                 predict_true_one = 0;

                           }


                           proc_predict_ptr = new focusBufferMsg<ProcedureInformationStruct>(BufferMessages::ProcedureInformation) ;

                           // Actually send the message
                           proc_predict_ptr -> set(&proc_predict);


                        }             // End of while statement

                        break;

                     case 9 :
                        break;

                     default :
                        printf("\n +**** UNKNOWN PREDICT SCREEN MESSAGE - start over ****+\n");

                  }
                  break;

               case 4 :
                  printf("+++++++ enter the number for the runproc message type\n"
                         "+++++++  1 for  preset runproc screen message\n"
                         "+++++++  draw/ret est pltlet  est plasma  est rbc  rem time\n"
                         "+++++++   METER    3.5x10^11    200ml     150ml      70min\n"
                         "+++++++  pressure  0.0x10^11      0ml       0ml       4min\n"
                         "+++++++   -45.0   cur pltlet  cur plasma  cur rbc  cur time\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++  2 for  preset runproc screen message\n"
                         "+++++++  draw/ret est pltlet  est plasma  est rbc  rem time\n"
                         "+++++++    DRAW    3.5x10^11    200ml     150ml      65min\n"
                         "+++++++  pressure  0.3x10^11     20ml       0ml       9min\n"
                         "+++++++  -105.0   cur pltlet  cur plasma  cur rbc  cur time\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++  3 for  preset runproc screen message\n"
                         "+++++++  draw/ret est pltlet  est plasma  est rbc  rem time\n"
                         "+++++++   RETURN   3.5x10^11    200ml     150ml      23min\n"
                         "+++++++  pressure  1.4x10^11     72ml       0ml      51min\n"
                         "+++++++   185.0   cur pltlet  cur plasma  cur rbc  cur time\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++  4 for  preset runproc screen message\n"
                         "+++++++  draw/ret est pltlet  est plasma  est rbc  rem time\n"
                         "+++++++    DRAW    3.5x10^11    200ml     150ml      57min\n"
                         "+++++++  pressure  3.4x10^11    190ml       0ml      17min\n"
                         "+++++++   -83.0   cur pltlet  cur plasma  cur rbc  cur time\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++  5 for  preset runproc screen message\n"
                         "+++++++  draw/ret est pltlet  est plasma  est rbc  rem time\n"
                         "+++++++   RETURN   3.5x10^11    200ml     150ml      68min\n"
                         "+++++++  pressure  3.5x10^11    200ml      82ml       6min\n"
                         "+++++++   204.0   cur pltlet  cur plasma  cur rbc  cur time\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++  6 for a RETURN PUMP DRAW runproc screen message\n"
                         "+++++++  7 for a RETURN PUMP RETURN runproc screen message\n"
                         "+++++++  8 for runproc Adjusting Platelet Concentraton message\n"
                         "+++++++  9 for a custom runproc screen message\n"
                         "+++++++ 10  to   EXIT\n"
                         "+++++++ enter selection -> "
                         );

                  send_int32_message(focusInt32Msg::ProcSubstateChange, (int)NULL_STATE);

                  switch (get_switch_value())
                  {

                     case 1 :

                        proc_status.return_pump_cycle           = RETURN_PUMP_CYCLE_DRAW; // No units, #define
                        proc_status.procedure_start_time        = 675.3;                  // Units: min
                        proc_status.current_procedure_time      = 4.3;                    // Units: min
                        proc_status.time_remaining_procedure    = 69.7;                   // Units: min
                        proc_status.current_inlet_pressure      = -45.25;                 // Units: mmHg
                        proc_status.current_inlet_pressure_max  = 310.0;                  // Units: mmHg
                        proc_status.current_inlet_pressure_min  = -250.0;                 // Units: mmHg
                        proc_status.current_platelets_collected = 0.0;                    // Units: "n" where: n x 10^11
                        proc_status.current_plasma_collected    = 0.0;                    // Units: mL
                        proc_status.current_rbc_collected       = 0.0;                    // Units: mL
                        proc_status.estimated_platelets         = 3.5f;
                        proc_status.estimated_plasma            = 200.0f;
                        proc_status.estimated_rbcs              = 150.0f;
                        proc_status.use_startup_pressure_meter  = 1 ;                  // stick meter on for blood prime message

                        proc_status_ptr = new focusBufferMsg<CURRENT_PROCEDURE_STATUS_STRUCT>(BufferMessages::CurrentProcedureStatus) ;
                        // Actually send the message
                        proc_status_ptr -> set(&proc_status);


                        confirm_msg();
                        break;

                     case 2 :

                        proc_status.return_pump_cycle           = RETURN_PUMP_CYCLE_DRAW; // No units, #define
                        proc_status.procedure_start_time        = 675.3;                  // Units: min
                        proc_status.current_procedure_time      = 9.3;                    // Units: min
                        proc_status.time_remaining_procedure    = 64.7;                   // Units: min
                        proc_status.current_inlet_pressure      = -105.25;                // Units: mmHg
                        proc_status.current_inlet_pressure_max  = 310.0;                  // Units: mmHg
                        proc_status.current_inlet_pressure_min  = -250.0;                 // Units: mmHg
                        proc_status.current_platelets_collected = .312;                   // Units: "n" where: n x 10^11
                        proc_status.current_plasma_collected    = 20.0;                   // Units: mL
                        proc_status.current_rbc_collected       = 0.0;                    // Units: mL
                        proc_status.estimated_platelets         = 3.5f;
                        proc_status.estimated_plasma            = 200.0f;
                        proc_status.estimated_rbcs              = 150.0f;
                        proc_status.use_startup_pressure_meter  = 0 ;                  // turn off "stick" meter

                        proc_status_ptr = new focusBufferMsg<CURRENT_PROCEDURE_STATUS_STRUCT>(BufferMessages::CurrentProcedureStatus) ;
                        // Actually send the message
                        proc_status_ptr -> set(&proc_status);


                        confirm_msg();
                        break;

                     case 3 :

                        proc_status.return_pump_cycle           = RETURN_PUMP_CYCLE_RETURN; // No units, #define
                        proc_status.procedure_start_time        = 675.3;                    // Units: min
                        proc_status.current_procedure_time      = 23.3;                     // Units: min
                        proc_status.time_remaining_procedure    = 50.7;                     // Units: min
                        proc_status.current_inlet_pressure      = 185.25;                   // Units: mmHg
                        proc_status.current_inlet_pressure_max  = 310.0;                    // Units: mmHg
                        proc_status.current_inlet_pressure_min  = -250.0;                   // Units: mmHg
                        proc_status.current_platelets_collected = 1.412;                    // Units: "n" where: n x 10^11
                        proc_status.current_plasma_collected    = 72.0;                     // Units: mL
                        proc_status.current_rbc_collected       = 0.0;                      // Units: mL
                        proc_status.estimated_platelets         = 3.5f;
                        proc_status.estimated_plasma            = 200.0f;
                        proc_status.estimated_rbcs              = 150.0f;
                        proc_status.use_startup_pressure_meter  = 0 ;                  // turn off "stick" meter

                        proc_status_ptr = new focusBufferMsg<CURRENT_PROCEDURE_STATUS_STRUCT>(BufferMessages::CurrentProcedureStatus) ;
                        // Actually send the message
                        proc_status_ptr -> set(&proc_status);


                        confirm_msg();
                        break;

                     case 4 :

                        proc_status.return_pump_cycle           = RETURN_PUMP_CYCLE_DRAW; // No units, #define
                        proc_status.procedure_start_time        = 675.3;                  // Units: min
                        proc_status.current_procedure_time      = 57.3;                   // Units: min
                        proc_status.time_remaining_procedure    = 16.7;                   // Units: min
                        proc_status.current_inlet_pressure      = -82.95;                 // Units: mmHg
                        proc_status.current_inlet_pressure_max  = 310.0;                  // Units: mmHg
                        proc_status.current_inlet_pressure_min  = -250.0;                 // Units: mmHg
                        proc_status.current_platelets_collected = 3.36;                   // Units: "n" where: n x 10^11
                        proc_status.current_plasma_collected    = 190.0;                  // Units: mL
                        proc_status.current_rbc_collected       = 0.0;                    // Units: mL
                        proc_status.estimated_platelets         = 3.5f;
                        proc_status.estimated_plasma            = 200.0f;
                        proc_status.estimated_rbcs              = 150.0f;
                        proc_status.use_startup_pressure_meter  = 0 ;                  // turn off "stick" meter

                        proc_status_ptr = new focusBufferMsg<CURRENT_PROCEDURE_STATUS_STRUCT>(BufferMessages::CurrentProcedureStatus) ;
                        // Actually send the message
                        proc_status_ptr -> set(&proc_status);


                        confirm_msg();
                        break;

                     case 5 :

                        proc_status.return_pump_cycle           = RETURN_PUMP_CYCLE_RETURN; // No units, #define
                        proc_status.procedure_start_time        = 675.3;                    // Units: min
                        proc_status.current_procedure_time      = 68.3;                     // Units: min
                        proc_status.time_remaining_procedure    = 5.7;                      // Units: min
                        proc_status.current_inlet_pressure      = 174.25;                   // Units: mmHg
                        proc_status.current_inlet_pressure_max  = 310.0;                    // Units: mmHg
                        proc_status.current_inlet_pressure_min  = -250.0;                   // Units: mmHg
                        proc_status.current_platelets_collected = 3.51;                     // Units: "n" where: n x 10^11
                        proc_status.current_plasma_collected    = 200.0;                    // Units: mL
                        proc_status.current_rbc_collected       = 82.0;                     // Units: mL
                        proc_status.estimated_platelets         = 3.5f;
                        proc_status.estimated_plasma            = 200.0f;
                        proc_status.estimated_rbcs              = 150.0f;
                        proc_status.use_startup_pressure_meter  = 0 ;                  // turn off "stick" meter

                        proc_status_ptr = new focusBufferMsg<CURRENT_PROCEDURE_STATUS_STRUCT>(BufferMessages::CurrentProcedureStatus) ;
                        // Actually send the message
                        proc_status_ptr -> set(&proc_status);


                        confirm_msg();
                        break;

                     case 6 :

                        proc_status.return_pump_cycle      = RETURN_PUMP_CYCLE_DRAW;                         // No units, #define
                        proc_status.current_inlet_pressure = proc_status.current_inlet_pressure_min * 0.40;  // Units: mmHg

                        proc_status_ptr                    = new focusBufferMsg<CURRENT_PROCEDURE_STATUS_STRUCT>(BufferMessages::CurrentProcedureStatus) ;
                        // Actually send the message
                        proc_status_ptr -> set(&proc_status);

                        break;

                     case 7 :

                        proc_status.return_pump_cycle      = RETURN_PUMP_CYCLE_RETURN;                       // No units, #define
                        proc_status.current_inlet_pressure = proc_status.current_inlet_pressure_max * 0.50;  // Units: mmHg

                        proc_status_ptr                    = new focusBufferMsg<CURRENT_PROCEDURE_STATUS_STRUCT>(BufferMessages::CurrentProcedureStatus) ;
                        // Actually send the message
                        proc_status_ptr -> set(&proc_status);

                        break;


                     case 8 :
                     {   send_int32_message(focusInt32Msg::ProcSubstateChange, (int)SS_EXTENDED_PCA);

                         proc_status_ptr = new focusBufferMsg<CURRENT_PROCEDURE_STATUS_STRUCT>(BufferMessages::CurrentProcedureStatus) ;
                         // Actually send the message
                         proc_status_ptr -> set(&proc_status); }
                                                               break;


                     case 9 :
                        runproc_true_one = 1;

                        while (runproc_true_one == 1)
                        {

                           printf("++++++++++ enter the number for specific runproc message\n"
                                  "++++++++++ 1  for start time message\n"
                                  "++++++++++ 2  for current time message\n"
                                  "++++++++++ 3  for time remaining in procedure message\n"
                                  "++++++++++ 4  for current access pressure message\n"
                                  "++++++++++ 5  for current platelets message\n"
                                  "++++++++++ 6  for current plasma message\n"
                                  "++++++++++ 7  for current rbc message\n"
                                  "++++++++++ 8  for estimated platelets message\n"
                                  "++++++++++ 9  for estimated plasma message\n"
                                  "++++++++++ 10 for estimated rbc message\n"
                                  "++++++++++ 11 for current max pressure message\n"
                                  "++++++++++ 12 for current min pressure message\n"
                                  "++++++++++ 13 for use startup pressure meter (1=use,0=don't use)\n"
                                  "++++++++++ 14 to  clear all entries\n"
                                  "++++++++++ 15 to  EXIT\n"
                                  "++++++++++ enter selection -> "
                                  );

                           switch (get_switch_value())
                           {
                              case 1 :
                                 printf("+++++++++++++ enter start time -> ");
                                 get_input(&runprocstarttime);
                                 proc_status.procedure_start_time = runprocstarttime;
                                 break;

                              case 2 :
                                 printf("+++++++++++++ enter current time -> ");
                                 get_input(&runproccurrenttime);
                                 proc_status.current_procedure_time = runproccurrenttime;
                                 break;

                              case 3 :
                                 printf("+++++++++++++ enter time remaining -> ");
                                 get_input(&runproctimeremaining);
                                 proc_status.time_remaining_procedure = runproctimeremaining;
                                 break;

                              case 4 :
                                 printf("+++++++++++++ enter current access pressure -> ");
                                 get_input(&runprocaccesspressure);
                                 proc_status.current_inlet_pressure = runprocaccesspressure;
                                 break;

                              case 5 :
                                 printf("+++++++++++++ enter current platelets -> ");
                                 get_input(&runproccurrentplatelets);
                                 proc_status.current_platelets_collected = runproccurrentplatelets;
                                 break;

                              case 6 :
                                 printf("+++++++++++++ enter current plasma -> ");
                                 get_input(&runproccurrentplasma);
                                 proc_status.current_plasma_collected = runproccurrentplasma;
                                 break;

                              case 7 :
                                 printf("+++++++++++++ enter current rbcs -> ");
                                 get_input(&runproccurrentrbcs);
                                 proc_status.current_rbc_collected = runproccurrentrbcs;
                                 break;

                              case 8 :
                                 printf("+++++++++++++ enter estimated platelets -> ");
                                 get_input(&runprocestimatedplatelets);
                                 proc_status.estimated_platelets = runprocestimatedplatelets;
                                 break;

                              case 9 :
                                 printf("+++++++++++++ enter estimated plasma -> ");
                                 get_input(&runprocestimatedplasma);
                                 proc_status.estimated_plasma = runprocestimatedplasma;
                                 break;

                              case 10 :
                                 printf("+++++++++++++ enter estimated rbcs -> ");
                                 get_input(&runprocestimatedrbcs);
                                 proc_status.estimated_rbcs = runprocestimatedrbcs;
                                 break;

                              case 11 :
                                 printf("+++++++++++++ enter current max pressure -> ");
                                 get_input(&runproccurrentmaxpressure);
                                 proc_status.current_inlet_pressure_max = runproccurrentmaxpressure;
                                 break;

                              case 12 :
                                 printf("+++++++++++++ enter current min pressure -> ");
                                 get_input(&runproccurrentminpressure);
                                 proc_status.current_inlet_pressure_min = runproccurrentminpressure;
                                 break;

                              case 13 :
                                 printf("+++++++++++++ enter 1 for 'stick' meter, 0 for regular -> ");
                                 get_input(&runproccurrentminpressure);
                                 proc_status.use_startup_pressure_meter = int(runproccurrentminpressure);
                                 break;


                              case 14 :
                                 memset(&proc_status, 0, sizeof(proc_status));
                                 break;

                              case 15 :
                                 runproc_true_one = 0;
                                 break;

                              default :
                                 printf("\n +**** UNKNOWN SPECIFIC SCREEN MESSAGE - start over ****+\n");
                                 runproc_true_one = 0;

                           }


                           proc_status_ptr = new focusBufferMsg<CURRENT_PROCEDURE_STATUS_STRUCT>(BufferMessages::CurrentProcedureStatus) ;

                           // Actually send the message
                           proc_status_ptr -> set(&proc_status);


                        }             // End of while statement
                        break;


                     case 10 :
                        break;


                     default :
                        printf("\n +**** UNKNOWN RUNPROC SCREEN MESSAGE - start over ****+\n");

                  }
                  break;

               case 5 :

                  printf("+++++++ enter the number for the adjust message type\n"
                         "+++++++ 1  for  preset adjust screen message\n"
                         "+++++++   ap   apc    rp   rpc   ac   acc    cl   clc sstat\n"
                         "+++++++  0.0   0.0   0.0   0.0  0.0   0.0   0.0   0.0 SU_R1\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 2  for  preset adjust screen message\n"
                         "+++++++  ap   apc    rp   rpc   ac   acc    cl   clc  sstat\n"
                         "+++++++ 35.5  71.0 100.0 200.0 0.75   0.9 7.925  9.85 SU_R4\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 3  for  preset adjust screen message\n"
                         "+++++++  ap   apc    rp   rpc   ac   acc    cl   clc  sstat\n"
                         "+++++++ 91.0 123.5 330.0 370.0  1.1  1.15 10.85 12.78   RUN\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 4  for  preset adjust screen message\n"
                         "+++++++  ap   apc    rp   rpc   ac   acc    cl   clc  sstat\n"
                         "+++++++142.0 142.0 400.0 400.0  1.2   1.2  13.7  13.7 RBC\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 5  for  preset adjust screen message\n"
                         "+++++++  ap   apc    rp   rpc   ac   acc    cl   clc  sstat\n"
                         "+++++++142.0 142.0 400.0 400.0  1.2   1.2  13.7  13.7 WBCS1\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 6  for a custom adjust screen message\n"
                         "+++++++ 7  to  EXIT\n"
                         "+++++++ enter selection -> "
                         );

                  switch (get_switch_value())
                  {

                     case 1 :

                        proc_adjust.access_pressure_adjust.maximum_scale = 142.0;
                        proc_adjust.access_pressure_adjust.minimum_scale = 0.0;
                        proc_adjust.access_pressure_adjust.current_cap   = 142.0;
                        proc_adjust.access_pressure_adjust.current_value = 0.0;

                        proc_adjust.return_pressure_adjust.maximum_scale = 400.0;
                        proc_adjust.return_pressure_adjust.minimum_scale = 0.0;
                        proc_adjust.return_pressure_adjust.current_cap   = 400.0;
                        proc_adjust.return_pressure_adjust.current_value = 0.0;

                        proc_adjust.tingling_adjust.maximum_scale        = 1.2;
                        proc_adjust.tingling_adjust.minimum_scale        = 0.6;
                        proc_adjust.tingling_adjust.current_cap          = 1.2;
                        proc_adjust.tingling_adjust.current_value        = 0.0;

                        proc_adjust.clumping_adjust.maximum_scale        = 13.7;
                        proc_adjust.clumping_adjust.minimum_scale        = 6.0;
                        proc_adjust.clumping_adjust.current_cap          = 13.7;
                        proc_adjust.clumping_adjust.current_value        = 0.0;


                        proc_adjust_ptr = new focusBufferMsg<ADJUST_SCREEN_CONFIG>(BufferMessages::ProcedureToGUIAdjust);

                        // Actually send the message
                        proc_adjust_ptr -> set(&proc_adjust);


                        confirm_msg();
                        break;

                     case 2 :

                        proc_adjust.access_pressure_adjust.maximum_scale = 142.0;
                        proc_adjust.access_pressure_adjust.minimum_scale = 0.0;
                        proc_adjust.access_pressure_adjust.current_cap   = 71.0;
                        proc_adjust.access_pressure_adjust.current_value = 35.5;

                        proc_adjust.return_pressure_adjust.maximum_scale = 400.0;
                        proc_adjust.return_pressure_adjust.minimum_scale = 0.0;
                        proc_adjust.return_pressure_adjust.current_cap   = 200.0;
                        proc_adjust.return_pressure_adjust.current_value = 100.0;

                        proc_adjust.tingling_adjust.maximum_scale        = 1.2;
                        proc_adjust.tingling_adjust.minimum_scale        = 0.6;
                        proc_adjust.tingling_adjust.current_cap          = 0.9;
                        proc_adjust.tingling_adjust.current_value        = 0.75;

                        proc_adjust.clumping_adjust.maximum_scale        = 13.7;
                        proc_adjust.clumping_adjust.minimum_scale        = 6.0;
                        proc_adjust.clumping_adjust.current_cap          = 9.85;
                        proc_adjust.clumping_adjust.current_value        = 7.925;


                        proc_adjust_ptr = new focusBufferMsg<ADJUST_SCREEN_CONFIG>(BufferMessages::ProcedureToGUIAdjust);

                        // Actually send the message
                        proc_adjust_ptr -> set(&proc_adjust);


                        confirm_msg();
                        break;

                     case 3 :

                        proc_adjust.access_pressure_adjust.maximum_scale = 142.0;
                        proc_adjust.access_pressure_adjust.minimum_scale = 0.0;
                        proc_adjust.access_pressure_adjust.current_cap   = 123.5;
                        proc_adjust.access_pressure_adjust.current_value = 91.0;

                        proc_adjust.return_pressure_adjust.maximum_scale = 400.0;
                        proc_adjust.return_pressure_adjust.minimum_scale = 0.0;
                        proc_adjust.return_pressure_adjust.current_cap   = 370.0;
                        proc_adjust.return_pressure_adjust.current_value = 330.0;

                        proc_adjust.tingling_adjust.maximum_scale        = 1.2;
                        proc_adjust.tingling_adjust.minimum_scale        = 0.6;
                        proc_adjust.tingling_adjust.current_cap          = 1.15;
                        proc_adjust.tingling_adjust.current_value        = 1.12;

                        proc_adjust.clumping_adjust.maximum_scale        = 13.7;
                        proc_adjust.clumping_adjust.minimum_scale        = 6.0;
                        proc_adjust.clumping_adjust.current_cap          = 12.775;
                        proc_adjust.clumping_adjust.current_value        = 10.85;


                        proc_adjust_ptr = new focusBufferMsg<ADJUST_SCREEN_CONFIG>(BufferMessages::ProcedureToGUIAdjust) ;
                        // Actually send the message
                        proc_adjust_ptr -> set(&proc_adjust);


                        confirm_msg();
                        break;

                     case 4 :

                        proc_adjust.access_pressure_adjust.maximum_scale = 142.0;
                        proc_adjust.access_pressure_adjust.minimum_scale = 0.0;
                        proc_adjust.access_pressure_adjust.current_cap   = 142.0;
                        proc_adjust.access_pressure_adjust.current_value = 142.0;

                        proc_adjust.return_pressure_adjust.maximum_scale = 400.0;
                        proc_adjust.return_pressure_adjust.minimum_scale = 0.0;
                        proc_adjust.return_pressure_adjust.current_cap   = 400.0;
                        proc_adjust.return_pressure_adjust.current_value = 400.0;

                        proc_adjust.tingling_adjust.maximum_scale        = 1.2;
                        proc_adjust.tingling_adjust.minimum_scale        = 0.6;
                        proc_adjust.tingling_adjust.current_cap          = 1.2;
                        proc_adjust.tingling_adjust.current_value        = 1.2;

                        proc_adjust.clumping_adjust.maximum_scale        = 13.7;
                        proc_adjust.clumping_adjust.minimum_scale        = 6.0;
                        proc_adjust.clumping_adjust.current_cap          = 13.7;
                        proc_adjust.clumping_adjust.current_value        = 13.7;


                        proc_adjust_ptr = new focusBufferMsg<ADJUST_SCREEN_CONFIG>(BufferMessages::ProcedureToGUIAdjust) ;
                        // Actually send the message
                        proc_adjust_ptr -> set(&proc_adjust);


                        confirm_msg();
                        break;

                     case 5 :

                        proc_adjust.access_pressure_adjust.maximum_scale = 142.0;
                        proc_adjust.access_pressure_adjust.minimum_scale = 0.0;
                        proc_adjust.access_pressure_adjust.current_cap   = 142.0;
                        proc_adjust.access_pressure_adjust.current_value = 142.0;

                        proc_adjust.return_pressure_adjust.maximum_scale = 400.0;
                        proc_adjust.return_pressure_adjust.minimum_scale = 0.0;
                        proc_adjust.return_pressure_adjust.current_cap   = 400.0;
                        proc_adjust.return_pressure_adjust.current_value = 400.0;

                        proc_adjust.tingling_adjust.maximum_scale        = 1.2;
                        proc_adjust.tingling_adjust.minimum_scale        = 0.6;
                        proc_adjust.tingling_adjust.current_cap          = 1.2;
                        proc_adjust.tingling_adjust.current_value        = 1.2;

                        proc_adjust.clumping_adjust.maximum_scale        = 13.7;
                        proc_adjust.clumping_adjust.minimum_scale        = 6.0;
                        proc_adjust.clumping_adjust.current_cap          = 13.7;
                        proc_adjust.clumping_adjust.current_value        = 13.7;


                        proc_adjust_ptr = new focusBufferMsg<ADJUST_SCREEN_CONFIG>(BufferMessages::ProcedureToGUIAdjust) ;
                        // Actually send the message
                        proc_adjust_ptr -> set(&proc_adjust);


                        confirm_msg();
                        break;

                     case 6 :
                        trouble_true_one = 1;

                        while (trouble_true_one == 1)
                        {

                           printf("++++++++++ enter the number for specific adjust message\n"
                                  "++++++++++ 1  for draw maximum message\n"
                                  "++++++++++ 2  for draw cap message\n"
                                  "++++++++++ 3  for draw value message\n"
                                  "++++++++++ 4  for return maximum message\n"
                                  "++++++++++ 5  for return cap message\n"
                                  "++++++++++ 6  for return value message\n"
                                  "++++++++++ 7  for ac reaction maximum message\n"
                                  "++++++++++ 8  for ac reaction cap message\n"
                                  "++++++++++ 9  for ac reaction value message\n"
                                  "++++++++++ 10 for clumping maximum message\n"
                                  "++++++++++ 11 for clumping cap message\n"
                                  "++++++++++ 12 for clumping value message\n"
                                  "++++++++++ 13 for \n"
                                  "++++++++++ 14 for enable/disable spillover button message\n"
                                  "++++++++++ 15 for enable/disable air block button message\n"
                                  "++++++++++ 16 for enable/disable saline bolus button message\n"
                                  "++++++++++ 17 to  clear all entries\n"
                                  "++++++++++ 18 to  EXIT\n"
                                  "++++++++++ enter selection -> "
                                  );

                           switch (get_switch_value())
                           {
                              case 1 :
                                 printf("+++++++++++++ enter draw maximum -> ");
                                 get_input(&troubledrawmax);
                                 break;

                              case 2 :
                                 printf("+++++++++++++ enter draw cap -> ");
                                 get_input(&troubledrawcap);
                                 proc_adjust.access_pressure_adjust.current_cap = troubledrawcap;
                                 break;

                              case 3 :
                                 printf("+++++++++++++ enter draw value -> ");
                                 get_input(&troubledrawvalue);
                                 proc_adjust.access_pressure_adjust.current_value = troubledrawvalue;
                                 break;

                              case 4 :
                                 printf("+++++++++++++ enter return maximum -> ");
                                 get_input(&troublereturnmax);
                                 break;

                              case 5 :
                                 printf("+++++++++++++ enter return cap -> ");
                                 get_input(&troublereturncap);
                                 proc_adjust.return_pressure_adjust.current_cap = troublereturncap;
                                 break;

                              case 6 :
                                 printf("+++++++++++++ enter return value -> ");
                                 get_input(&troublereturnvalue);
                                 proc_adjust.return_pressure_adjust.current_value = troublereturnvalue;
                                 break;

                              case 7 :
                                 printf("+++++++++++++ enter ac reaction maximum -> ");
                                 get_input(&troubleacreactionmax);
                                 break;

                              case 8 :
                                 printf("+++++++++++++ enter ac reaction cap -> ");
                                 get_input(&troubleacreactioncap);
                                 proc_adjust.tingling_adjust.current_cap = troubleacreactioncap;
                                 break;

                              case 9 :
                                 printf("+++++++++++++ enter ac reaction value -> ");
                                 get_input(&troubleacreactionvalue);
                                 proc_adjust.tingling_adjust.current_value = troubleacreactionvalue;
                                 break;

                              case 10 :
                                 printf("+++++++++++++ enter clumping maximum -> ");
                                 get_input(&troubleclumpingmax);
                                 break;

                              case 11 :
                                 printf("+++++++++++++ enter clumping cap -> ");
                                 get_input(&troubleclumpingcap);
                                 proc_adjust.clumping_adjust.current_cap = troubleclumpingcap;
                                 break;

                              case 12 :
                                 printf("+++++++++++++ enter clumping value -> ");
                                 get_input(&troubleclumpingvalue);
                                 proc_adjust.clumping_adjust.current_value = troubleclumpingvalue;
                                 break;

                              case 13 :
                                 break;

                              case 14 :
                                 printf("+++++++++++++ enter spillover button status (0=invis, 1=grey, 2=vis.) -> ");
                                 get_input(&trouble_button_status);

                                 switch (trouble_button_status)
                                 {
                                    case 0 :
                                       proc_adjust.spillover_button_state = BUTTON_INVISIBLE ;
                                       break;

                                    case 1 :
                                       proc_adjust.spillover_button_state = BUTTON_GREY ;
                                       break;

                                    case 2 :
                                       proc_adjust.spillover_button_state = BUTTON_VISIBLE ;
                                       break;

                                    default :
                                       printf ("Bad button status \n") ;
                                 }                   // End of while statement


                                 break;

                              case 15 :
                                 printf("+++++++++++++ enter air block button status (0=invis, 1=grey, 2=vis.) -> ");
                                 get_input(&trouble_button_status);

                                 switch (trouble_button_status)
                                 {
                                    case 0 :
                                       proc_adjust.air_block_button_state = BUTTON_INVISIBLE ;
                                       break;

                                    case 1 :
                                       proc_adjust.air_block_button_state = BUTTON_GREY ;
                                       break;

                                    case 2 :
                                       proc_adjust.air_block_button_state = BUTTON_VISIBLE ;
                                       break;

                                    default :
                                       printf ("Bad button status \n") ;
                                 }                  // End of while statement


                                 break;

                              case 16 :
                                 printf("+++++++++++++ enter saline bolus button status (0=invis, 1=grey, 2=vis.) -> ");
                                 get_input(&trouble_button_status);

                                 switch (trouble_button_status)
                                 {
                                    case 0 :
                                       proc_adjust.saline_bolus_button_state = BUTTON_INVISIBLE ;
                                       break;

                                    case 1 :
                                       proc_adjust.saline_bolus_button_state = BUTTON_GREY ;
                                       break;

                                    case 2 :
                                       proc_adjust.saline_bolus_button_state = BUTTON_VISIBLE ;
                                       break;

                                    default :
                                       printf ("Bad button status \n") ;
                                 }                  // End of while statement


                                 break;

                              case 17 :
                                 memset(&proc_adjust, 0, sizeof(proc_adjust));
                                 break;

                              case 18 :
                                 trouble_true_one = 0;
                                 break;

                              default :
                                 printf("\n +**** UNKNOWN SPECIFIC SCREEN MESSAGE - start over ****+\n");
                                 trouble_true_one = 0;

                           }


                           proc_adjust_ptr = new focusBufferMsg<ADJUST_SCREEN_CONFIG>(BufferMessages::ProcedureToGUIAdjust) ;

                           // Actually send the message
                           proc_adjust_ptr -> set(&proc_adjust);


                        }             // End of while statement

                        break;

                     case 7 :
                        break;

                     default :
                        printf("\n +**** UNKNOWN ADJUST SCREEN MESSAGE - start over ****+\n");

                  }           // End of switch statement
                  break;

               case 6 :
                  printf("+++++++ enter the number for the runsumm1 message type\n"
                         "+++++++ 1  for  preset data set\n"
                         "+++++++  total ac used 550ml\n"
                         "+++++++  end of run time ???\n"
                         "+++++++  length of run 74min\n"
                         "+++++++  post platelet count 179x1000\n"
                         "+++++++  post hct 40pct\n"
                         "+++++++  actual ac to donor 387ml\n"
                         "+++++++  total tbv processed 3429ml\n"
                         "+++++++  total saline used 1200ml\n"
                         "+++++++  !!! all label and verification flags 'on'\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 2  for  random data set\n"
                         "+++++++  total ac used ???ml\n"
                         "+++++++  end of run time ???\n"
                         "+++++++  length of run ???min\n"
                         "+++++++  post platelet count ???x1000\n"
                         "+++++++  post hct ??pct\n"
                         "+++++++  actual ac to donor ???ml\n"
                         "+++++++  total tbv processed ????ml\n"
                         "+++++++  total saline used ????ml\n"
                         "+++++++  !!! all label and verification flags '?'\n"

                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 3  for  custom runsumm data set\n"
                         "+++++++ 4  to  EXIT\n"
                         "+++++++ enter selection -> "
                         );

                  switch (get_switch_value())
                  {
                     case 1 :

                        run_summ1.rs1_total_AC_used = 550.0  ;                        // Units: mL
                        time (&time_struct) ;
                        _localtime(&time_struct, &run_summ1.rs1_end_of_run_time);    // Units: QNX rep. of point in time
                        run_summ1.rs1_length_of_run             = 74.0 ;             // Units: minutes
                        run_summ1.rs1_post_platelet_count       = 179.0 ;            // Units: "n" where: n x 1000/uL
                        run_summ1.rs1_post_hct                  = 40.0 ;             // Units: %
                        run_summ1.rs1_actual_AC_to_donor        = 387.0 ;            // Units: mL
                        run_summ1.rs1_total_blood_vol_processed = 3429.0;            // Units: mL

                        run_summ1.replacement_fluid_volume      = 1200.0 ;        // Units: ml

                        // Set all label flags to "TRUE"
                        run_summ1.PQdata.label_platelets_leukoreduced = 1;
                        run_summ1.PQdata.label_plasma_leukoreduced    = 1;

                        // Set all Product Quality flagging reasons to "TRUE"
                        //  NOTE: It should be impossible in reality to have both the
                        //        "label" and "verify WBC" flags set.  But, this simulates
                        //        what the original code was doing, so...
                        run_summ1.PQdata.verify_platelet_wbc_reasons.Initialize(SPILL_DETECTION, SPILL_RECOVERY, STOPPED_CENTRIFUGE, RBC_DETECTOR_ERROR,
                                                                                HIGH_PLATELET_CONCENTRATION, LOW_CHAMBER_FLOW, HIGH_DELTA_HCT, AIR_BLOCK_RECOVERY,
                                                                                LONG_DRAW, HIGH_HCT, PLATELET_CONTAMINANT_DETECTION);

                        run_summ1.PQdata.verify_plasma_wbc_reasons.Initialize(SPILL_DETECTION, SPILL_RECOVERY, STOPPED_CENTRIFUGE, RBC_DETECTOR_ERROR, AIR_BLOCK_RECOVERY);

                        run_summ1.PQdata.verify_platelet_yield_volume_reasons.Initialize(SPILL_DETECTION, SPILL_RECOVERY, STOPPED_CENTRIFUGE, HIGH_PLATELET_CONCENTRATION,
                                                                                         LOW_CHAMBER_FLOW, AIR_BLOCK_RECOVERY, RUN_ENDED_EARLY,
                                                                                         LOW_DETECTED_PLATELET_CONCENTRATION, HIGH_PLATELET_YIELD_DEVIATION, LONG_DRAW,
                                                                                         SHORT_DRAW, PLATELET_VOLUME_ERROR);

                        run_summ1.PQdata.verify_plasma_volume_reasons.Initialize(STOPPED_CENTRIFUGE, AIR_BLOCK_RECOVERY, SHORT_DRAW, LONG_DRAW, PLASMA_VOLUME_ERROR, RUN_ENDED_EARLY);

                        run_summ1.PQdata.verify_rbc_volume_reasons.Initialize(STOPPED_CENTRIFUGE, AIR_BLOCK_RECOVERY, LONG_DRAW, RBC_VOLUME_ERROR, RUN_ENDED_EARLY);

                        run_summ1.PQdata.platelet_concentration_reasons.Initialize(PLATELET_CONCENTRATION_LT1000, PLATELET_CONCENTRATION_GT2100,
                                                                                   CONCENTRATION_TARGET_LT1000, CONCENTRATION_TARGET_GT2100);



                        runsumm_ptr = new focusBufferMsg<RUN_SUMMARY_STRUCT>(BufferMessages::EndRunStats) ;
                        // Actually send the message
                        runsumm_ptr -> set(&run_summ1);


                        confirm_msg();
                        break;

                     case 2 :

                        run_summ1.rs1_total_AC_used = (float)(rand() * 9999) / (float)RAND_MAX  ;                          // Units: mL
                        time (&time_struct) ;
                        _localtime(&time_struct, &run_summ1.rs1_end_of_run_time);                            // Units: QNX rep. of point in time
                        run_summ1.rs1_length_of_run             = (float)(rand() * 9999) / (float)RAND_MAX ; // Units: minutes
                        run_summ1.rs1_post_platelet_count       = (float)(rand() * 99) / (float)RAND_MAX ;   // Units: "n" where: n x 1000/uL
                        run_summ1.rs1_actual_AC_to_donor        = (float)(rand() * 999) / (float)RAND_MAX ;  // Units: mL
                        run_summ1.rs1_post_hct                  = (float)(rand() * 99) / (float)RAND_MAX ;   // Units: no units: (Qin / Qac)
                        run_summ1.rs1_total_blood_vol_processed = (float)(rand() * 9999) / (float)RAND_MAX ; // Units: mL

                        run_summ1.replacement_fluid_volume      = (float)(rand() * 9999) / (float)RAND_MAX   ;          // Units: ml

                        // Set all label flags to "TRUE"Random Values
                        run_summ1.PQdata.label_platelets_leukoreduced = ((rand() * 9999) > 5000.0) ? 1 : 0;
                        run_summ1.PQdata.label_plasma_leukoreduced    = ((rand() * 9999) > 5000.0) ? 1 : 0;

                        // Set all Product Quality flagging reasons to "TRUE"
                        //  NOTE: It should be impossible in reality to have both the
                        //        "label" and "verify WBC" flags set, or to have a reason
                        //        for one flag that is not set for another for which it
                        //        is valid.  But, this simulates what the original code was doing, so...
                        run_summ1.PQdata.verify_platelet_wbc_reasons.Initialize();
                        run_summ1.PQdata.verify_plasma_wbc_reasons.Initialize();
                        run_summ1.PQdata.verify_platelet_yield_volume_reasons.Initialize();
                        run_summ1.PQdata.verify_plasma_volume_reasons.Initialize();
                        run_summ1.PQdata.verify_rbc_volume_reasons.Initialize();
                        run_summ1.PQdata.platelet_concentration_reasons.Initialize();
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_wbc_reasons.Put(SPILL_DETECTION);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_wbc_reasons.Put(SPILL_RECOVERY);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_wbc_reasons.Put(STOPPED_CENTRIFUGE);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_wbc_reasons.Put(RBC_DETECTOR_ERROR);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_wbc_reasons.Put(HIGH_PLATELET_CONCENTRATION);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_wbc_reasons.Put(LOW_CHAMBER_FLOW);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_wbc_reasons.Put(HIGH_DELTA_HCT);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_wbc_reasons.Put(AIR_BLOCK_RECOVERY);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_wbc_reasons.Put(LONG_DRAW);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_wbc_reasons.Put(HIGH_HCT);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_wbc_reasons.Put(PLATELET_CONTAMINANT_DETECTION);

                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_plasma_wbc_reasons.Put(SPILL_DETECTION);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_plasma_wbc_reasons.Put(SPILL_RECOVERY);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_plasma_wbc_reasons.Put(STOPPED_CENTRIFUGE);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_plasma_wbc_reasons.Put(RBC_DETECTOR_ERROR);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_plasma_wbc_reasons.Put(AIR_BLOCK_RECOVERY);

                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(SPILL_DETECTION);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(SPILL_RECOVERY);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(STOPPED_CENTRIFUGE);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(HIGH_PLATELET_CONCENTRATION);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(LOW_CHAMBER_FLOW);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(AIR_BLOCK_RECOVERY);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(RUN_ENDED_EARLY);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(LOW_DETECTED_PLATELET_CONCENTRATION);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(HIGH_PLATELET_YIELD_DEVIATION);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(LONG_DRAW);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(SHORT_DRAW);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(PLATELET_VOLUME_ERROR);

                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_plasma_volume_reasons.Put(STOPPED_CENTRIFUGE);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_plasma_volume_reasons.Put(AIR_BLOCK_RECOVERY);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_plasma_volume_reasons.Put(SHORT_DRAW);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_plasma_volume_reasons.Put(LONG_DRAW);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_plasma_volume_reasons.Put(PLASMA_VOLUME_ERROR);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_plasma_volume_reasons.Put(RUN_ENDED_EARLY);

                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_rbc_volume_reasons.Put(STOPPED_CENTRIFUGE);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_rbc_volume_reasons.Put(AIR_BLOCK_RECOVERY);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_rbc_volume_reasons.Put(LONG_DRAW);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_rbc_volume_reasons.Put(RBC_VOLUME_ERROR);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.verify_rbc_volume_reasons.Put(RUN_ENDED_EARLY);

                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.platelet_concentration_reasons.Put(PLATELET_CONCENTRATION_LT1000);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.platelet_concentration_reasons.Put(PLATELET_CONCENTRATION_GT2100);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.platelet_concentration_reasons.Put(CONCENTRATION_TARGET_LT1000);
                        if (((rand() * 9999) > 5000.0) ? 1 : 0) run_summ1.PQdata.platelet_concentration_reasons.Put(CONCENTRATION_TARGET_GT2100);


                        runsumm_ptr = new focusBufferMsg<RUN_SUMMARY_STRUCT>(BufferMessages::EndRunStats) ;
                        // Actually send the message
                        runsumm_ptr -> set(&run_summ1);


                        confirm_msg();
                        break;

                     case 3 :
                        runsumm1_true_one = 1;

                        while (runsumm1_true_one == 1)
                        {

                           printf("++++++++++ enter the number for specific runsumm1 message\n"
                                  "++++++++++ 1   for total AC used message\n"
                                  "++++++++++ 2   for end of run time message\n"
                                  "++++++++++ 3   for length of run message\n"
                                  "++++++++++ 4   for post platelet count message\n"
                                  "++++++++++ 5   for post hct message\n"
                                  "++++++++++ 6   for ac to donor message\n"
                                  "++++++++++ 7   for total blood volume processed message\n"
                                  "++++++++++ 8   for total saline used\n"
                                  "++++++++++ 9   for label flag(s)\n"
                                  "++++++++++ 10  for verify WBC's in product flag(s)\n"
                                  "++++++++++ 11  for verify plt yield flag\n"
                                  "++++++++++ 12  for verify plt volume flag\n"
                                  "++++++++++ 13  for verify plt concentration flag\n"
                                  "++++++++++ 14  for verify pls volume flag\n"
                                  "++++++++++ 15  for verify rbc volume flag\n"
                                  "++++++++++ 16  to clear all entries\n"
                                  "++++++++++ 17  to  EXIT\n"
                                  "++++++++++ enter selection -> "
                                  );

                           switch (get_switch_value())
                           {
                              case 1 :
                                 printf("+++++++++++++ enter total ac used -> ");
                                 get_input(&runsummtotalacused);
                                 run_summ1.rs1_total_AC_used = runsummtotalacused;
                                 break;

                              case 2 :
                                 time (&time_struct) ;
                                 _localtime(&time_struct, &run_summ1.rs1_end_of_run_time);
                                 break;

                              case 3 :
                                 printf("+++++++++++++ enter length of time -> ");
                                 get_input(&runsummlengthoftime);
                                 run_summ1.rs1_length_of_run = runsummlengthoftime;
                                 break;

                              case 4 :
                                 printf("+++++++++++++ enter post platelet count -> ");
                                 get_input(&runsummpostplateletcount);
                                 run_summ1.rs1_post_platelet_count = runsummpostplateletcount;
                                 break;

                              case 5 :
                                 printf("+++++++++++++ enter post hct -> ");
                                 get_input(&runsummposthct);
                                 run_summ1.rs1_post_hct = runsummposthct;
                                 break;

                              case 6 :
                                 printf("+++++++++++++ enter ac to donor -> ");
                                 get_input(&runsummactodonor);
                                 run_summ1.rs1_actual_AC_to_donor = runsummactodonor;
                                 break;

                              case 7 :
                                 printf("+++++++++++++ enter total blood volume processed -> ");
                                 get_input(&runsummtotalbloodvolumeprocessed);
                                 run_summ1.rs1_total_blood_vol_processed = runsummtotalbloodvolumeprocessed;
                                 break;

                              case 8 :
                                 printf("+++++++++++++ enter total saline used -> ");
                                 get_input(&(run_summ1.replacement_fluid_volume));
                                 break;

                              case 9 :
                              {
                                 int j;
                                 printf("+++++++++++++ enter label_platelets_leukoreduced flag (1 or 0) -> ");
                                 get_input(&j);
                                 run_summ1.PQdata.label_platelets_leukoreduced = j;
                                 printf("+++++++++++++ enter label_plasma_leukoreduced flag (1 or 0) -> ");
                                 get_input(&j);
                                 run_summ1.PQdata.label_plasma_leukoreduced = j;
                                 break;
                              }

                              case 10 :
                              {
                                 int j;
                                 printf("+++++++++++++ select Platelet (0) or Plasma (1) +++++++++++++\n");
                                 get_input(&j);
                                 switch (j)
                                 {
                                    default :
                                       printf("\nINVALID!\n");
                                       break;
                                    case 0 :
                                    {
                                       run_summ1.PQdata.verify_platelet_wbc_reasons.Initialize();
                                       do
                                       {
                                          j = 0;
                                          printf("+++++++++++++ select verify_platelet_wbc_reasons +++++++++++++\n"
                                                 "++++++++++ 0   when done\n"
                                                 "++++++++++ 1   for SPILL_DETECTION\n"
                                                 "++++++++++ 2   for SPILL_RECOVERY\n"
                                                 "++++++++++ 3   for STOPPED_CENTRIFUGE\n"
                                                 "++++++++++ 4   for RBC_DETECTOR_ERROR\n"
                                                 "++++++++++ 5   for HIGH_PLATELET_CONCENTRATION\n"
                                                 "++++++++++ 6   for LOW_CHAMBER_FLOW\n"
                                                 "++++++++++ 7   for HIGH_DELTA_HCT\n"
                                                 "++++++++++ 8   for AIR_BLOCK_RECOVERY\n"
                                                 "++++++++++ 9   for LONG_DRAW\n"
                                                 "++++++++++ 10  for HIGH_HCT\n"
                                                 "++++++++++ 11  for LARGE_VOLUME_PROCESSED\n"
                                                 "++++++++++ 12  for PLATELET_CONTAMINANT_DETECTION\n"
                                                 "++++++++++ enter selection -> "
                                                 );
                                          get_input(&j);
                                          switch (j)
                                          {
                                             case 0 :
                                                break;
                                             case 1 :
                                                run_summ1.PQdata.verify_platelet_wbc_reasons.Put(SPILL_DETECTION);
                                                break;
                                             case 2 :
                                                run_summ1.PQdata.verify_platelet_wbc_reasons.Put(SPILL_RECOVERY);
                                                break;
                                             case 3 :
                                                run_summ1.PQdata.verify_platelet_wbc_reasons.Put(STOPPED_CENTRIFUGE);
                                                break;
                                             case 4 :
                                                run_summ1.PQdata.verify_platelet_wbc_reasons.Put(RBC_DETECTOR_ERROR);
                                                break;
                                             case 5 :
                                                run_summ1.PQdata.verify_platelet_wbc_reasons.Put(HIGH_PLATELET_CONCENTRATION);
                                                break;
                                             case 6 :
                                                run_summ1.PQdata.verify_platelet_wbc_reasons.Put(LOW_CHAMBER_FLOW);
                                                break;
                                             case 7 :
                                                run_summ1.PQdata.verify_platelet_wbc_reasons.Put(HIGH_DELTA_HCT);
                                                break;
                                             case 8 :
                                                run_summ1.PQdata.verify_platelet_wbc_reasons.Put(AIR_BLOCK_RECOVERY);
                                                break;
                                             case 9 :
                                                run_summ1.PQdata.verify_platelet_wbc_reasons.Put(LONG_DRAW);
                                                break;
                                             case 10 :
                                                run_summ1.PQdata.verify_platelet_wbc_reasons.Put(HIGH_HCT);
                                                break;
                                             case 12 :
                                                run_summ1.PQdata.verify_platelet_wbc_reasons.Put(PLATELET_CONTAMINANT_DETECTION);
                                                break;
                                             default :
                                                printf("\nINVALID!\n");
                                                break;
                                          }
                                       } while (j != 0);
                                       break;
                                    }
                                    case 1 :
                                    {
                                       run_summ1.PQdata.verify_plasma_wbc_reasons.Initialize();
                                       do
                                       {
                                          j = 0;
                                          printf("+++++++++++++ select verify_plasma_wbc_reasons +++++++++++++\n"
                                                 "++++++++++ 0   when done\n"
                                                 "++++++++++ 1   for SPILL_DETECTION\n"
                                                 "++++++++++ 2   for SPILL_RECOVERY\n"
                                                 "++++++++++ 3   for STOPPED_CENTRIFUGE\n"
                                                 "++++++++++ 4   for RBC_DETECTOR_ERROR\n"
                                                 "++++++++++ 5   for AIR_BLOCK_RECOVERY\n"
                                                 "++++++++++ enter selection -> "
                                                 );
                                          get_input(&j);
                                          switch (j)
                                          {
                                             case 0 :
                                                break;
                                             case 1 :
                                                run_summ1.PQdata.verify_plasma_wbc_reasons.Put(SPILL_DETECTION);
                                                break;
                                             case 2 :
                                                run_summ1.PQdata.verify_plasma_wbc_reasons.Put(SPILL_RECOVERY);
                                                break;
                                             case 3 :
                                                run_summ1.PQdata.verify_plasma_wbc_reasons.Put(STOPPED_CENTRIFUGE);
                                                break;
                                             case 4 :
                                                run_summ1.PQdata.verify_plasma_wbc_reasons.Put(RBC_DETECTOR_ERROR);
                                                break;
                                             case 5 :
                                                run_summ1.PQdata.verify_plasma_wbc_reasons.Put(AIR_BLOCK_RECOVERY);
                                                break;
                                             default :
                                                printf("\nINVALID!\n");
                                                break;
                                          }
                                       } while (j != 0);
                                       break;
                                    }
                                 }                    // End of switch on j for the Platelet/Plasma selection
                                 break;
                              }

                              case 11 :
                              case 12 :
                              {
                                 printf("    NOTE: Platelet Yield and Volume use the same flag.\n");
                                 run_summ1.PQdata.verify_platelet_yield_volume_reasons.Initialize();
                                 int j;
                                 do
                                 {
                                    j = 0;
                                    printf("+++++++++++++ select verify_platelet_yield_volume_reasons +++++++++++++\n"
                                           "++++++++++ 0   when done\n"
                                           "++++++++++ 1   for SPILL_DETECTION\n"
                                           "++++++++++ 2   for SPILL_RECOVERY\n"
                                           "++++++++++ 3   for STOPPED_CENTRIFUGE\n"
                                           "++++++++++ 4   for HIGH_PLATELET_CONCENTRATION\n"
                                           "++++++++++ 5   for LOW_CHAMBER_FLOW\n"
                                           "++++++++++ 6   for AIR_BLOCK_RECOVERY\n"
                                           "++++++++++ 7   for LARGE_VOLUME_PROCESSED\n"
                                           "++++++++++ 8   for RUN_ENDED_EARLY\n"
                                           "++++++++++ 9   for LOW_DETECTED_PLATELET_CONCENTRATION\n"
                                           "++++++++++ 10  for HIGH_PLATELET_YIELD_DEVIATION\n"
                                           "++++++++++ 11  for LONG_DRAW\n"
                                           "++++++++++ 12  for SHORT_DRAW\n"
                                           "++++++++++ 13  for PLATELET_VOLUME_ERROR\n"
                                           "++++++++++ enter selection -> "
                                           );
                                    get_input(&j);
                                    switch (j)
                                    {
                                       case 0 :
                                          break;
                                       case 1 :
                                          run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(SPILL_DETECTION);
                                          break;
                                       case 2 :
                                          run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(SPILL_RECOVERY);
                                          break;
                                       case 3 :
                                          run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(STOPPED_CENTRIFUGE);
                                          break;
                                       case 4 :
                                          run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(HIGH_PLATELET_CONCENTRATION);
                                          break;
                                       case 5 :
                                          run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(LOW_CHAMBER_FLOW);
                                          break;
                                       case 6 :
                                          run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(AIR_BLOCK_RECOVERY);
                                          break;
                                       case 8 :
                                          run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(RUN_ENDED_EARLY);
                                          break;
                                       case 9 :
                                          run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(LOW_DETECTED_PLATELET_CONCENTRATION);
                                          break;
                                       case 10 :
                                          run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(HIGH_PLATELET_YIELD_DEVIATION);
                                          break;
                                       case 11 :
                                          run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(LONG_DRAW);
                                          break;
                                       case 12 :
                                          run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(SHORT_DRAW);
                                          break;
                                       case 13 :
                                          run_summ1.PQdata.verify_platelet_yield_volume_reasons.Put(PLATELET_VOLUME_ERROR);
                                          break;
                                       default :
                                          printf("\nINVALID!\n");
                                    }
                                 } while (j != 0);
                                 break;
                              }

                              case 13 :
                              {
                                 int j;
                                 run_summ1.PQdata.platelet_concentration_reasons.Initialize();
                                 do
                                 {
                                    j = 0;
                                    printf("+++++++++++++ select platelet_concentration_reasons +++++++++++++\n"
                                           "++++++++++ 0   when done\n"
                                           "++++++++++ 1   for PLATELET_CONCENTRATION_LT1000\n"
                                           "++++++++++ 2   for PLATELET_CONCENTRATION_GT2100\n"
                                           "++++++++++ 3   for CONCENTRATION_TARGET_LT1000\n"
                                           "++++++++++ 4   for CONCENTRATION_TARGET_GT2100\n"
                                           "++++++++++ enter selection -> "
                                           );
                                    get_input(&j);
                                    switch (j)
                                    {
                                       case 0 :
                                          break;
                                       case 1 :
                                          run_summ1.PQdata.platelet_concentration_reasons.Put(PLATELET_CONCENTRATION_LT1000);
                                          break;
                                       case 2 :
                                          run_summ1.PQdata.platelet_concentration_reasons.Put(PLATELET_CONCENTRATION_GT2100);
                                          break;
                                       case 3 :
                                          run_summ1.PQdata.platelet_concentration_reasons.Put(CONCENTRATION_TARGET_LT1000);
                                          break;
                                       case 4 :
                                          run_summ1.PQdata.platelet_concentration_reasons.Put(CONCENTRATION_TARGET_GT2100);
                                          break;
                                       default :
                                          printf("\nINVALID!\n");
                                          break;
                                    }
                                 } while (j != 0);
                                 break;
                              }

                              case 14 :
                              {
                                 int j;
                                 run_summ1.PQdata.verify_plasma_volume_reasons.Initialize();
                                 do
                                 {
                                    j = 0;
                                    printf("+++++++++++++ select verify_plasma_volume_reasons +++++++++++++\n"
                                           "++++++++++ 0   when done\n"
                                           "++++++++++ 1   for STOPPED_CENTRIFUGE\n"
                                           "++++++++++ 2   for AIR_BLOCK_RECOVERY\n"
                                           "++++++++++ 3   for SHORT_DRAW\n"
                                           "++++++++++ 4   for LONG_DRAW\n"
                                           "++++++++++ 5   for PLASMA_VOLUME_ERROR\n"
                                           "++++++++++ 6   for RUN_ENDED_EARLY\n"
                                           "++++++++++ enter selection -> "
                                           );
                                    get_input(&j);
                                    switch (j)
                                    {
                                       case 0 :
                                          break;
                                       case 1 :
                                          run_summ1.PQdata.verify_plasma_volume_reasons.Put(STOPPED_CENTRIFUGE);
                                          break;
                                       case 2 :
                                          run_summ1.PQdata.verify_plasma_volume_reasons.Put(AIR_BLOCK_RECOVERY);
                                          break;
                                       case 3 :
                                          run_summ1.PQdata.verify_plasma_volume_reasons.Put(SHORT_DRAW);
                                          break;
                                       case 4 :
                                          run_summ1.PQdata.verify_plasma_volume_reasons.Put(LONG_DRAW);
                                          break;
                                       case 5 :
                                          run_summ1.PQdata.verify_plasma_volume_reasons.Put(PLASMA_VOLUME_ERROR);
                                          break;
                                       case 6 :
                                          run_summ1.PQdata.verify_plasma_volume_reasons.Put(RUN_ENDED_EARLY);
                                          break;
                                       default :
                                          printf("\nINVALID!\n");
                                          break;
                                    }
                                 } while (j != 0);
                                 break;
                              }

                              case 15 :
                              {
                                 int j;
                                 run_summ1.PQdata.verify_rbc_volume_reasons.Initialize();
                                 do
                                 {
                                    j = 0;
                                    printf("+++++++++++++ select verify_rbc_volume_reasons +++++++++++++\n"
                                           "++++++++++ 0   when done\n"
                                           "++++++++++ 1   for STOPPED_CENTRIFUGE\n"
                                           "++++++++++ 2   for AIR_BLOCK_RECOVERY\n"
                                           "++++++++++ 3   for LONG_DRAW\n"
                                           "++++++++++ 4   for RBC_VOLUME_ERROR\n"
                                           "++++++++++ 5   for RUN_ENDED_EARLY\n"
                                           "++++++++++ enter selection -> "
                                           );
                                    get_input(&j);
                                    switch (j)
                                    {
                                       case 0 :
                                          break;
                                       case 1 :
                                          run_summ1.PQdata.verify_rbc_volume_reasons.Put(STOPPED_CENTRIFUGE);
                                          break;
                                       case 2 :
                                          run_summ1.PQdata.verify_rbc_volume_reasons.Put(AIR_BLOCK_RECOVERY);
                                          break;
                                       case 3 :
                                          run_summ1.PQdata.verify_rbc_volume_reasons.Put(LONG_DRAW);
                                          break;
                                       case 4 :
                                          run_summ1.PQdata.verify_rbc_volume_reasons.Put(RBC_VOLUME_ERROR);
                                          break;
                                       case 5 :
                                          run_summ1.PQdata.verify_rbc_volume_reasons.Put(RUN_ENDED_EARLY);
                                          break;
                                       default :
                                          printf("\nINVALID!\n");
                                          break;
                                    }
                                 } while (j != 0);
                                 break;
                              }

                              case 16 :
                                 run_summ1.rs1_total_AC_used             = 0.0 ;                 // Units: mL
                                 run_summ1.rs1_length_of_run             = 0.0 ;                 // Units: minutes
                                 run_summ1.rs1_post_platelet_count       = 0.0 ;                 // Units: "n" where: n x 1000/uL
                                 run_summ1.rs1_post_hct                  = 0.0 ;                 // Units: %
                                 run_summ1.rs1_actual_AC_to_donor        = 0.0 ;                 // Units: mL
                                 run_summ1.rs1_total_blood_vol_processed = 0.0;                  // Units: mL
                                 run_summ1.replacement_fluid_volume      = 0.0 ;                 // Units: mL

                                 // Set all label flags to "FALSE"
                                 run_summ1.PQdata.label_platelets_leukoreduced = 0;
                                 run_summ1.PQdata.label_plasma_leukoreduced    = 0;

                                 // Set all Product Quality flagging reasons to "FALSE"
                                 run_summ1.PQdata.verify_platelet_wbc_reasons.Initialize();
                                 run_summ1.PQdata.verify_plasma_wbc_reasons.Initialize();
                                 run_summ1.PQdata.verify_platelet_yield_volume_reasons.Initialize();
                                 run_summ1.PQdata.verify_plasma_volume_reasons.Initialize();
                                 run_summ1.PQdata.verify_rbc_volume_reasons.Initialize();
                                 run_summ1.PQdata.platelet_concentration_reasons.Initialize();
                                 break;

                              case 17 :
                                 runsumm1_true_one = 0;
                                 break;

                              default :
                                 printf("\n +**** UNKNOWN SPECIFIC SCREEN MESSAGE - start over ****+\n");
                                 runsumm1_true_one = 0;

                           }


                           runsumm_ptr = new focusBufferMsg<RUN_SUMMARY_STRUCT>(BufferMessages::EndRunStats) ;

                           // Actually send the message
                           runsumm_ptr -> set(&run_summ1);

                           confirm_msg();

                        }             // End of while statement

                        break;

                     case 4 :
                        break;

                     default :
                        printf("\n +**** UNKNOWN RUNSUMM1 SCREEN MESSAGE - start over ****+\n");

                  }

                  break;

               case 7 :
                  printf("+++++++ enter the number for the runsumm2 message type\n"
                         "+++++++ 1  for  preset data set\n"
                         "+++++++  platelet volume 420ml\n"
                         "+++++++  yield of platelets 3.5x10^11\n"
                         "+++++++  volume of ac in platelets 63ml\n"
                         "+++++++  plasma volume 200ml\n"
                         "+++++++  volume of ac in plasma 62ml\n"
                         "+++++++  rbc volume 150ml\n"
                         "+++++++  volume of ac in rbc 38ml\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 2  for  random data set\n"
                         "+++++++  platelet volume ???ml\n"
                         "+++++++  yield of platelets ??x10^11\n"
                         "+++++++  volume of ac in platelets ??ml\n"
                         "+++++++  plasma volume ???ml\n"
                         "+++++++  volume of ac in plasma ??ml\n"
                         "+++++++  rbc volume ???ml\n"
                         "+++++++  volume of ac in rbc ??ml\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 3  for  custom runsumm data set\n"
                         "+++++++ 4  to  EXIT\n"
                         "+++++++ enter selection -> "
                         );

                  switch (get_switch_value())
                  {
                     case 1 :

                        run_summ2.rs2_platelet_volume     = 420.0 ;                // Units: mL
                        run_summ2.rs2_yield_of_platelets  = 3.5 ;                  // Units: "n" where: n x 10^11
                        run_summ2.rs2_vol_AC_in_platelets = 63.0 ;                 // Units: mL
                        run_summ2.rs2_plasma_volume       = 200.0 ;                // Units: mL
                        run_summ2.rs2_vol_AC_in_plasma    = 62.0 ;                 // Units: mL
                        run_summ2.rs2_RBC_volume          = 150.0 ;                // Units: mL
                        run_summ2.rs2_vol_AC_in_RBC       = 38.0 ;                 // Units: mL

                        runsumm_ptr = new focusBufferMsg<RUN_SUMMARY_STRUCT>(BufferMessages::EndRunStats) ;
                        // Actually send the message
                        runsumm_ptr -> set(&run_summ2);


                        confirm_msg();
                        break;

                     case 2 :

                        run_summ2.rs2_platelet_volume     = (float)(rand() * 9999) / (float)RAND_MAX  ;                  // Units: mL
                        run_summ2.rs2_yield_of_platelets  = (float)(rand() * 99) / (float)RAND_MAX  ;                    // Units: "n" where: n x 10^11
                        run_summ2.rs2_vol_AC_in_platelets = (float)(rand() * 99) / (float)RAND_MAX  ;                    // Units: mL
                        run_summ2.rs2_plasma_volume       = (float)(rand() * 999) / (float)RAND_MAX  ;                   // Units: mL
                        run_summ2.rs2_vol_AC_in_plasma    = (float)(rand() * 99) / (float)RAND_MAX  ;                    // Units: mL
                        run_summ2.rs2_RBC_volume          = (float)(rand() * 999) / (float)RAND_MAX  ;                   // Units: mL
                        run_summ2.rs2_vol_AC_in_RBC       = (float)(rand() * 99) / (float)RAND_MAX  ;                    // Units: mL

                        runsumm_ptr = new focusBufferMsg<RUN_SUMMARY_STRUCT>(BufferMessages::EndRunStats) ;
                        // Actually send the message
                        runsumm_ptr -> set(&run_summ2);


                        confirm_msg();
                        break;

                     case 3 :
                        runsumm2_true_one = 1;

                        while (runsumm2_true_one == 1)
                        {

                           printf("++++++++++ enter the number for specific runsumm2 message\n"
                                  "++++++++++ 1  for platelet volume message\n"
                                  "++++++++++ 2  for platelet yield message\n"
                                  "++++++++++ 3  for volume of ac in platelets message\n"
                                  "++++++++++ 4  for plasma volume message\n"
                                  "++++++++++ 5  for volume of ac in plasma message\n"
                                  "++++++++++ 6  for rbc volume message\n"
                                  "++++++++++ 7  for volume of ac in rbcs message\n"
                                  "++++++++++ 8  to  clear all entries\n"
                                  "++++++++++ 9  to  EXIT\n"
                                  "++++++++++ enter selection -> "
                                  );

                           switch (get_switch_value())
                           {
                              case 1 :
                                 printf("+++++++++++++ enter platelet volume -> ");
                                 get_input(&runsummplateletvolume);
                                 run_summ2.rs2_platelet_volume = runsummplateletvolume;
                                 break;

                              case 2 :
                                 printf("+++++++++++++ enter platelet yield -> ");
                                 get_input(&runsummplateletyield);
                                 run_summ2.rs2_yield_of_platelets = runsummplateletyield;
                                 break;

                              case 3 :
                                 printf("+++++++++++++ enter volume of ac in platelets -> ");
                                 get_input(&runsummvolumeofacinplatelets);
                                 run_summ2.rs2_vol_AC_in_platelets = runsummvolumeofacinplatelets;
                                 break;

                              case 4 :
                                 printf("+++++++++++++ enter plasma volume -> ");
                                 get_input(&runsummplasmavolume);
                                 run_summ2.rs2_plasma_volume = runsummplasmavolume;
                                 break;

                              case 5 :
                                 printf("+++++++++++++ enter volume of ac in plasma -> ");
                                 get_input(&runsummvolumeofacinplasma);
                                 run_summ2.rs2_vol_AC_in_plasma = runsummvolumeofacinplasma;
                                 break;

                              case 6 :
                                 printf("+++++++++++++ enter rbc volume -> ");
                                 get_input(&runsummrbcvolume);
                                 run_summ2.rs2_RBC_volume = runsummrbcvolume;
                                 break;

                              case 7 :
                                 printf("+++++++++++++ enter volume of ac in rbcs -> ");
                                 get_input(&runsummvolumeofacinrbcs);
                                 run_summ2.rs2_vol_AC_in_RBC = runsummvolumeofacinrbcs;
                                 break;

                              case 8 :
                                 run_summ2.rs2_platelet_volume     = 0.0 ;                       // Units: mL
                                 run_summ2.rs2_yield_of_platelets  = 0.0 ;                       // Units: "n" where: n x 10^11
                                 run_summ2.rs2_vol_AC_in_platelets = 0.0 ;                       // Units: mL
                                 run_summ2.rs2_plasma_volume       = 0.0 ;                       // Units: mL
                                 run_summ2.rs2_vol_AC_in_plasma    = 0.0 ;                       // Units: mL
                                 run_summ2.rs2_RBC_volume          = 0.0 ;                       // Units: mL
                                 run_summ2.rs2_vol_AC_in_RBC       = 0.0 ;                       // Units: mL
                                 break;

                              case 9 :
                                 runsumm2_true_one = 0;
                                 break;

                              default :
                                 printf("\n +**** UNKNOWN SPECIFIC SCREEN MESSAGE - start over ****+\n");
                                 runsumm2_true_one = 0;

                           }

                           runsumm_ptr = new focusBufferMsg<RUN_SUMMARY_STRUCT>(BufferMessages::EndRunStats) ;

                           // Actually send the message
                           runsumm_ptr -> set(&run_summ2);


                        }             // End of while statement

                        break;

                     case 4 :
                        break;

                     default :
                        printf("\n +**** UNKNOWN RUNSUMM2 SCREEN MESSAGE - start over ****+\n");

                  }

                  break;

               case 8 :          // 6-8
                  printf("+++++++ enter the number for the donvital message type\n"
                         "+++++++ 1  for  preset donordl screen message\n"
                         "+++++++   sex    height     weight      tbv       units\n"
                         "+++++++    M      6'3''     235lbs     6579ml     ft/lb\n"
                         "+++++++  btype  hct   platelets    DOB         name\n"
                         "+++++++    O+    48   244x10^11  1811/06/15  Bill Thackery \n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 2  for  preset donordl screen message\n"
                         "+++++++   sex    height     weight      tbv       units\n"
                         "+++++++    F      160cm      54kgs     3428ml     cm/kg\n"
                         "+++++++  btype  hct   platelets    DOB         name\n"
                         "+++++++   AB+    41   229x10^11  1887/12/01  Georgia O'Keefe\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 3   to  enter custom donor information\n"
                         "+++++++ 4   to  EXIT\n"
                         "+++++++ enter selection -> "
                         );

                  switch (get_switch_value())
                  {

                     case 1 :

                        donor_vitals.cDonorVitalsStatus   = DONOR_VITAL_OPERATOR_COMFIRMATION;
                        donor_vitals.cDonorInfoStatus     = DONOR_INFO_OPERATOR_COMFIRMATION;
                        donor_vitals.cDonorDownloadStatus = DONOR_DOWNLOAD_DEFAULT;
                        donor_vitals.cDonorSex            = DONOR_SEX_IS_MALE; //
                        donor_vitals.cDonorHeightUnits    = HEIGHT_UNITS_IN;   //
                        donor_vitals.cDonorWeightUnits    = WEIGHT_UNITS_LB;   //
                        donor_vitals.fDonorHeight         = 75.0;              //
                        donor_vitals.fDonorWeight         = 235.0;             //
                        donor_vitals.fSafetyDonorTBV      = 6579.0;            //
                        donor_vitals.fProcDonorTBV        = 6573.0;            //
                        donor_vitals.fDonorHct            = 48.0;              //
                        donor_vitals.fDonorPltPrecount    = 244.0;             //
                        donor_vitals.DonorBloodType       = BT_O_POS;
                        donor_vitals.DonorDataCRC         = 0X465B2637 ;
                        strcpy(donor_vitals.cDonorDateofBirth, "18110615");
                        strcpy(donor_vitals.cDonorName, "William Makepeace Thackery");

                        donor_vitals_ptr = new focusBufferMsg<SDonorVitals>(BufferMessages::SafetyToGuiDonorVitals);
                        // Actually send the message
                        donor_vitals_ptr -> set(&donor_vitals);

                        confirm_msg();
                        break;

                     case 2 :

                        donor_vitals.cDonorVitalsStatus   = DONOR_VITAL_OPERATOR_COMFIRMATION;
                        donor_vitals.cDonorInfoStatus     = DONOR_INFO_OPERATOR_COMFIRMATION;
                        donor_vitals.cDonorDownloadStatus = DONOR_DOWNLOAD_DEFAULT;
                        donor_vitals.cDonorSex            = DONOR_SEX_IS_FEMALE; //
                        donor_vitals.cDonorHeightUnits    = HEIGHT_UNITS_CM;     //
                        donor_vitals.cDonorWeightUnits    = WEIGHT_UNITS_KG;     //
                        donor_vitals.fDonorHeight         = 160.0;               //
                        donor_vitals.fDonorWeight         = 54.0;                //
                        donor_vitals.fSafetyDonorTBV      = 3428.0;              //
                        donor_vitals.fProcDonorTBV        = 3424.0;              //
                        donor_vitals.fDonorHct            = 41.0;                //
                        donor_vitals.fDonorPltPrecount    = 229.0;               //
                        donor_vitals.DonorBloodType       = BT_AB_POS;
                        donor_vitals.DonorDataCRC         = 0X0123def0 ;
                        strcpy(donor_vitals.cDonorName, "Georgia O'keefe");
                        strcpy(donor_vitals.cDonorDateofBirth, "18871201");


                        donor_vitals_ptr = new focusBufferMsg<SDonorVitals>(BufferMessages::SafetyToGuiDonorVitals) ;
                        // Actually send the message
                        donor_vitals_ptr -> set(&donor_vitals);


                        confirm_msg();
                        break;


                     case 3 :
                        donvital_true_one = 1;

                        while (donvital_true_one == 1)
                        {

                           printf("++++++++++ enter the number for specific donordl message\n"
                                  "++++++++++  1  for donor female sex message\n"
                                  "++++++++++  2  for donor male sex message\n"
                                  "++++++++++  3  for donor height message\n"
                                  "++++++++++  4  for donor weight message\n"
                                  "++++++++++  5  for english/metric units message\n"
                                  "++++++++++  6  for donor tbv message\n"
                                  "++++++++++  7  for donor blood type message\n"
                                  "++++++++++  8  for donor hematocrit message\n"
                                  "++++++++++  9  for donor platelet precount message\n"
                                  "++++++++++ 10  for DOB\n"
                                  "++++++++++ 11  for donor name\n"
                                  "++++++++++ 12   to clear all entries\n"
                                  "++++++++++ 13   to EXIT\n"
                                  "++++++++++ enter selection -> "
                                  );

                           switch (get_switch_value())
                           {
                              case 1 :
                                 donor_vitals.cDonorSex = DONOR_SEX_IS_FEMALE;
                                 break;


                              case 2 :
                                 donor_vitals.cDonorSex = DONOR_SEX_IS_MALE;
                                 break;


                              case 3 :
                                 printf("+++++++++++++ enter height in inches/cms -> ");

                                 if ((donor_vitals.cDonorHeightUnits == HEIGHT_UNITS_IN) || (donor_vitals.cDonorHeightUnits == HEIGHT_UNITS_CM))
                                 {
                                    get_input(&donvitalsheight);
                                    donor_vitals.fDonorHeight = donvitalsheight;
                                 }
                                 else
                                 {
                                    printf("\n+++++++++++++ enter height units first !\n");
                                 }

                                 break;


                              case 4 :
                                 printf("+++++++++++++ enter weight in lbs/kgs -> ");

                                 if ((donor_vitals.cDonorWeightUnits == WEIGHT_UNITS_LB) || (donor_vitals.cDonorWeightUnits == WEIGHT_UNITS_KG))
                                 {
                                    get_input(&donvitalsweight);
                                    donor_vitals.fDonorWeight = donvitalsweight;
                                 }
                                 else
                                 {
                                    printf("\n+++++++++++++ enter weight units first !\n");
                                 }

                                 break;


                              case 5 :
                                 printf("+++++++++++++ enter the number for the type of units\n"
                                        "+++++++++++++ 1  for english (ft/lb)\n"
                                        "+++++++++++++ 2  for metric (cm/kg)\n"
                                        "+++++++++++++ enter selection -> ");
                                 switch (get_switch_value())
                                 {
                                    case 1 :
                                       memset(&donor_vitals, 0, sizeof(donor_vitals));
                                       donor_vitals.cDonorHeightUnits = HEIGHT_UNITS_IN;
                                       donor_vitals.cDonorWeightUnits = WEIGHT_UNITS_LB;
                                       break;

                                    case 2 :
                                       memset(&donor_vitals, 0, sizeof(donor_vitals));
                                       donor_vitals.cDonorHeightUnits = HEIGHT_UNITS_CM;
                                       donor_vitals.cDonorWeightUnits = WEIGHT_UNITS_KG;
                                       break;

                                    default :
                                       printf("\n +**** UNKNOWN HEIGHT UNITS MESSAGE - start over ****+\n");
                                 }

                                 break;


                              case 6 :
                                 printf("+++++++++++++ enter tbv -> ");
                                 get_input(&donvitalstbv);
                                 donor_vitals.fSafetyDonorTBV = donvitalstbv;
                                 break;

                              case 7 :
                                 printf("+++++++++++++ enter the number for specific blood type\n"
                                        "+++++++++++++ 1  for donor blood type A+\n"
                                        "+++++++++++++ 2  for donor blood type A-\n"
                                        "+++++++++++++ 3  for donor blood type AB+\n"
                                        "+++++++++++++ 4  for donor blood type AB-\n"
                                        "+++++++++++++ 5  for donor blood type B+\n"
                                        "+++++++++++++ 6  for donor blood type B-\n"
                                        "+++++++++++++ 7  for donor blood type O+\n"
                                        "+++++++++++++ 8  for donor blood type O-\n"
                                        "+++++++++++++ 9  for no donor blood type\n"
                                        "+++++++++++++ 10 to  EXIT\n"
                                        "+++++++++++++ enter selection -> ");

                                 switch (get_switch_value())
                                 {
                                    case 1 :
                                       donor_vitals.DonorBloodType = BT_A_POS;
                                       break;

                                    case 2 :
                                       donor_vitals.DonorBloodType = BT_A_NEG;
                                       break;

                                    case 3 :
                                       donor_vitals.DonorBloodType = BT_AB_POS;
                                       break;

                                    case 4 :
                                       donor_vitals.DonorBloodType = BT_AB_NEG;
                                       break;

                                    case 5 :
                                       donor_vitals.DonorBloodType = BT_B_POS;
                                       break;

                                    case 6 :
                                       donor_vitals.DonorBloodType = BT_B_NEG;
                                       break;

                                    case 7 :
                                       donor_vitals.DonorBloodType = BT_O_POS;
                                       break;

                                    case 8 :
                                       donor_vitals.DonorBloodType = BT_O_NEG;
                                       break;

                                    case 9 :
                                       donor_vitals.DonorBloodType = BT_UNKNOWN;
                                       break;

                                    case 10 :
                                       break;

                                    default :
                                       printf("\n +**** UNKNOWN SPECIFIC SCREEN MESSAGE - start over ****+\n");

                                 }
                                 break;

                              case 8 :
                                 printf("+++++++++++++ enter hct -> ");
                                 get_input(&doninfohct);
                                 donor_vitals.fDonorHct = doninfohct;
                                 break;

                              case 9 :
                                 printf("+++++++++++++ enter platelet precount -> ");
                                 get_input(&doninfoplateletprecount);
                                 donor_vitals.fDonorPltPrecount = doninfoplateletprecount;
                                 break;

                              case 10 :
                                 printf("++++++++++ enter Donor DOB (yyyymmdd) -> ");
                                 get_input(donor_vitals.cDonorDateofBirth);
                                 if (strlen(donor_vitals.cDonorDateofBirth) != 8) strcpy(donor_vitals.cDonorDateofBirth, "18110615");
                                 break;


                              case 11 :
                                 printf("++++++++++ enter Donor Name (<=%d characters)\n-> ", sizeof(donor_vitals.cDonorName) - 1);
                                 get_input(donor_vitals.cDonorName);
                                 if (strlen(donor_vitals.cDonorName) < 1) strcpy(donor_vitals.cDonorName, "William Makepeace Thackery");
                                 break;


                              case 12 :
                                 memset(&donor_vitals, 0, sizeof(donor_vitals));
                                 break;

                              case 13 :
                                 donvital_true_one = 0;
                                 break;

                              default :
                                 printf("\n +**** UNKNOWN SPECIFIC SCREEN MESSAGE - start over ****+\n");
                                 donvital_true_one = 0;

                           }


                           donor_vitals_ptr = new focusBufferMsg<SDonorVitals>(BufferMessages::SafetyToGuiDonorVitals) ;
                           // Actually send the message
                           donor_vitals_ptr -> set(&donor_vitals);

                           confirm_msg();

                        }             // End of while statement

                        break;

                     default :
                        printf("\n +**** UNKNOWN DONORDL MESSAGE SELECTION - start over ****+\n");
                  }
                  break;

               case 9 :
                  printf("+++++++ enter the number for the donvital message type\n"
                         "+++++++ 1  for  preset Vista SYSDONOR screen message\n"
                         "+++++++   sex       name              DOB       Units\n"
                         "+++++++    M    Bill Thackery      1811/06/15  English\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 2  for  preset Vista SYSDONOR screen message\n"
                         "+++++++   sex       name              DOB       Units\n"
                         "+++++++    F    Georgia O'keefe    1887/12/01  Metric\n"
                         "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                         "+++++++ 3   to  enter custom donor information\n"
                         "+++++++ \n"
                         "+++++++ 4   to  EXIT\n"
                         "+++++++ enter selection -> "
                         );

                  switch (get_switch_value())
                  {

                     case 1 :

                        donor_vitals.cDonorSex = DONOR_SEX_IS_MALE;                        //
                        // donor_vitals.cDonorHeightUnits  = HEIGHT_UNITS_IN;     //
                        // donor_vitals.cDonorWeightUnits  = WEIGHT_UNITS_LB;     //
                        // donor_vitals.fDonorHeight       = 75.0;                //
                        // donor_vitals.fDonorWeight       = 235.0;               //
                        // donor_vitals.fSafetyDonorTBV    = 6579.0;              //
                        // donor_vitals.fProcDonorTBV      = 6573.0;              //
                        // donor_vitals.fDonorHct          = 48.0;    //
                        // donor_vitals.fDonorPltPrecount  = 244.0;   //
                        // donor_vitals.DonorBloodType     = BT_O_POS;
                        // donor_vitals.DonorDataCRC       = 0X465B2637 ;
                        // printf("++++++++++ enter Donor DOB (yyyymmdd) -> ");
                        // get_input(donor_vitals.cDonorDateofBirth);
                        // if (strlen(donor_vitals.cDonorDateofBirth) != 8) strcpy(donor_vitals.cDonorDateofBirth, "18110615");
                        // printf("++++++++++ enter Donor Name (<=%d characters)\n-> ", sizeof(donor_vitals.cDonorName)-1 );
                        // get_input(donor_vitals.cDonorName);
                        // if (strlen(donor_vitals.cDonorName) < 1) strcpy(donor_vitals.cDonorName, "William Makepeace Thackery");

                        // strcpy(donor_vitals.cDonorDateofBirth, "18110615");
                        strcpy(donor_vitals.cDonorName, "William Makepeace Thackery");
                        donor_vitals_ptr = new focusBufferMsg<SDonorVitals>(BufferMessages::VistaDonorMsg);
                        // Actually send the message
                        donor_vitals_ptr -> set(&donor_vitals);


                        confirm_msg();
                        break;


                     case 2 :

                        donor_vitals.cDonorSex = DONOR_SEX_IS_FEMALE;                        //
                        // donor_vitals.cDonorHeightUnits  = HEIGHT_UNITS_CM;     //
                        // donor_vitals.cDonorWeightUnits  = WEIGHT_UNITS_KG;     //
                        // donor_vitals.fDonorHeight       = 160.0;    //
                        // donor_vitals.fDonorWeight       = 54.0;   //
                        ////donor_vitals.fSafetyDonorTBV    = 3428.0;     //
                        // donor_vitals.fProcDonorTBV      = 3424.0;     //
                        // donor_vitals.fDonorHct          = 41.0;    //
                        // donor_vitals.fDonorPltPrecount  = 229.0;   //
                        // donor_vitals.DonorBloodType     = BT_AB_POS;
                        // donor_vitals.DonorDataCRC       = 0X0123def0 ;
                        strcpy(donor_vitals.cDonorName, "Georgia O'keefe");
                        // strcpy(donor_vitals.cDonorDateofBirth, "18871201");


                        donor_vitals_ptr = new focusBufferMsg<SDonorVitals>(BufferMessages::VistaDonorMsg) ;
                        // Actually send the message
                        donor_vitals_ptr -> set(&donor_vitals);

                        confirm_msg();
                        break;


                     case 3 :

                        printf("++++++++ enter the number for specific donordl message\n"
                               "++++++++ 1  for donor female name message\n"
                               "++++++++ 2  for donor male name message\n"
                               "++++++++ 3   to EXIT\n"
                               "++++++++ enter selection -> "
                               );

                        switch (get_switch_value())
                        {
                           case 1 :
                              donor_vitals.cDonorSex = DONOR_SEX_IS_MALE;
                              printf("++++++++++ enter Donor Name (<=%d characters)\n-> ", sizeof(donor_vitals.cDonorName) - 1);
                              get_input(donor_vitals.cDonorName);
                              if (strlen(donor_vitals.cDonorName) < 1) strcpy(donor_vitals.cDonorName, "William Makepeace Thackery");

                              donor_vitals_ptr = new focusBufferMsg<SDonorVitals>(BufferMessages::VistaDonorMsg) ;

                              // Actually send the message
                              donor_vitals_ptr -> set(&donor_vitals);

                              confirm_msg();
                              break;


                           case 2 :
                              donor_vitals.cDonorSex = DONOR_SEX_IS_FEMALE;
                              printf("++++++++++ enter Donor Name (<=%d characters)\n-> ", sizeof(donor_vitals.cDonorName) - 1);
                              get_input(donor_vitals.cDonorName);
                              if (strlen(donor_vitals.cDonorName) < 1) strcpy(donor_vitals.cDonorName, "Georgia O'Keefe");

                              donor_vitals_ptr = new focusBufferMsg<SDonorVitals>(BufferMessages::VistaDonorMsg) ;

                              // Actually send the message
                              donor_vitals_ptr -> set(&donor_vitals);
                              break;


                           case 3 :
                              break;



                           default :
                              printf("\n +**** UNKNOWN SPECIFIC SCREEN MESSAGE - start over ****+\n");
                              donvital_true_one = 0;

                        }


                        // donor_vitals_ptr = new focusBufferMsg<SDonorVitals>(BufferMessages::SafetyToGuiDonorVitals) ;
                        // Actually send the message
                        // donor_vitals_ptr -> set(&donor_vitals);

                        // confirm_msg();


                        break;


                     case 4 :
                        break;

                     default :
                        printf("\n +**** UNKNOWN SYSDONOR Vista MESSAGE SELECTION - start over ****+\n");
                  }
                  break;

               case 10 :
               {
                  service_true_one = 1;

                  while (service_true_one == 1)
                  {

                     printf("+++++++ enter the number for the Service Meter message type\n"
                            "+++++++ 1  for  preset Service Meter screen message\n"
                            "+++++++   Machine On Time -    50.0\n"
                            "+++++++   Centrifuge On Time - 35.0\n"
                            "+++++++   Number of Runs -     15.0\n"
                            "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                            "+++++++ 2  to set Machine On Time message\n"
                            "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                            "+++++++ 3  to set Centrifuge On Time message\n"
                            "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                            "+++++++ 4  to set Number of Runs message\n"
                            "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                            "+++++++ 5  to zero out entries\n"
                            "+++++++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                            "+++++++ 6  to EXIT\n"
                            "+++++++ enter selection -> "
                            );

                     switch (get_switch_value())
                     {

                        case 1 :

                           meter_service.l_trima_on_time         = 3000;
                           meter_service.l_centrifuge_on_time    = 2100;
                           meter_service.l_no_of_successful_runs = 15;
                           break;

                        case 2 :

                           printf("++++++++++ Enter as a factor of .016666\n"
                                  "++++++++++ enter Machine On Time  -> ");
                           get_input(&machinetime);
                           meter_service.l_trima_on_time = machinetime;
                           break;


                        case 3 :

                           printf("++++++++++ Enter as a factor of .016666\n"
                                  "++++++++++ enter Centrifuge On Time  -> ");
                           get_input(&centrifugetime);
                           meter_service.l_centrifuge_on_time = centrifugetime;
                           break;


                        case 4 :

                           printf("++++++++++ enter Number of Runs  -> ");
                           get_input(&numberRuns);
                           meter_service.l_no_of_successful_runs = numberRuns;


                        case 5 :

                           meter_service.l_trima_on_time         = 0;
                           meter_service.l_centrifuge_on_time    = 0;
                           meter_service.l_no_of_successful_runs = 0;
                           break;


                        case 6 :
                           service_true_one = 0;
                           break;


                        default :
                           printf("\n +**** UNKNOWN SERVICE MESSAGE SELECTION - start over ****+\n");
                           service_true_one = 0;

                     }                 // End switch statement

                     meter_service_ptr = new focusBufferMsg<Meter_Return_Struct>(BufferMessages::MeterDataMsg) ;
                     // Actually send the message
                     meter_service_ptr -> set(&meter_service);

                     confirm_msg();
                  }                // End while statement
               }
               break;

               case 11 :
                  break;


               default :
                  printf("\n +**** UNKNOWN SCREEN MESSAGE SELECTION - start over ****+\n");

            }         // End of switch statement
         }
         break;

         case 7 :
            printf("++++ enter the number for the height units\n"
                   "++++ 1  for output file comment\n"
                   "++++ 2  for system call\n"
                   "++++ 3  for \n"
                   "++++ 4  to  EXIT\n"
                   "++++ enter selection -> ");

            switch (get_switch_value())
            {
               case 1 :
                  // add an explicit write to file for commenting we need to put in the comment
                  // delim hence we cannot use the overloaded get_input() function.

                  if ((xcommmsg_output_file != NULL) && (xcommmsg_input_file == NULL))
                  {
                     printf("+++++++ enter a comment for script generator < 256 char\n"
                            "+++++++ enter comment -> "
                            );

                     char output_file_comment[MAX_CHAR_BUFFER_LENGTH];
                     char char_buffer[MAX_CHAR_BUFFER_LENGTH];

                     // initilize buffers
                     strcpy(output_file_comment, "\0");
                     strcpy(char_buffer, "\0");

                     // use the standard input manager to get the string
                     standard_input_manager (char_buffer);

                     // add comment delimiter to buffer
                     strcat(output_file_comment, COMMENT_DELIM_STRING);
                     strcat(output_file_comment, BLANK_STRING);
                     strcat(output_file_comment, char_buffer);
                     strcat(output_file_comment, BUTTON_CAR_RETURN);

                     fputs(output_file_comment, xcommmsg_output_file);
                  }
                  else
                  {
                     printf("+++++++ no file for output or input file open\n");
                  }
                  break;

               case 2 :
                  printf("+++++++ enter a system call command\n"
                         "+++++++ enter command -> "
                         );

                  // initilize buffer
                  strcpy(system_call_buffer, "\0");

                  // use the standard input manager to get the string
                  standard_input_manager (system_call_buffer);

                  // pass using the system call
                  system(system_call_buffer);

                  break;

               case 3 :      // Send AP2AgentInterface section change messages
                  break;

               case 4 :      // exit this case - noop
                  break;

               default :
                  printf("\n +**** UNKNOWN SELECTION - start over ****+\n");
            }
            break;


         case 8 :
            printf("++++ enter the driver delay in milliseconds\n"
                   "++++ enter delay in milliseconds -> "
                   );

            get_input (&milliseconddelay);
            set_delay (milliseconddelay);
            break;

         case 9 :
            printf("++++ enter the number for specific mouse event\n"
                   "++++ 1  for creating a mouse input event\n"
                   "++++ 2  for mouse autodetect\n"
                   "++++ enter selection -> "
                   );

            switch (get_switch_value())
            {
               case 1 :

                  printf("+++++++ enter x-coordinate for the mouse (1 for snapshot)\n"
                         "+++++++ enter selection -> ") ;

                  get_input(&x_mouse_location);

                  printf("+++++++ enter y-coordinate for the mouse (1 for snapshot)\n"
                         "+++++++ enter selection -> ") ;

                  get_input(&y_mouse_location);

                  // temporary and crude but effective means of generating snapshot delay
                  // NOTE : snapshot delay occurs on (1,1) mouse event emulation only and snapshot
                  // must be set in gtest script for now.  ultimately snapshot will be a component
                  // of xcommmsg.
                  if (((x_mouse_location == 1) && (y_mouse_location == 1)) && (snapshot_data_dir != NULL))
                  {
                     // create snapshot event
                     create_snapshot (x_mouse_location, y_mouse_location, NULL) ;
                  }
                  else
                  {
                     // standard mouse event
                     set_mouse_event (x_mouse_location, y_mouse_location) ;
                  }

                  break;

               case 2 :
                  printf("\n +**** MOUSE AUTODETECT MESSAGE RECEIVE ****+\n");
                  break;


               default :
                  printf("\n +**** UNKNOWN SPECIFIC MOUSE MESSAGE - start over ****+\n");

            }

            break;

         case 10 :
            // reset main control loop variable to exit
            true_one = 0;
            break;

         default :
            printf("\n +**** UNKNOWN SELECTION - select again ****+\n");

      }   // End of switch statement

      // add an explicit EOL and delay after every pass through the main loop when
      // the output is open but an input file is not,  i.e. we don't want to echo
      // the delay when it is already contained in the input file.
      if ((xcommmsg_output_file != NULL) && (xcommmsg_input_file == NULL))
      {
         // provide c-return and delays after text entries between main loop cycles
         // this code is temporary until the messaging is in place to support screen
         // refresh complete logic which will gate the next cycle
         fputs(MESSAGE_PROCESSING_ENTRY, xcommmsg_output_file);
         fputs(BUTTON_CAR_RETURN, xcommmsg_output_file);
         fputs(MESSAGE_PROCESSING_DELAY, xcommmsg_output_file);
         fputs(BUTTON_CAR_RETURN, xcommmsg_output_file);
      }

   }  // End of while statement

   // close all input and output fils
   close_all_files(XC_SUCCESS);

   // delete the dispatcher
   delete dispatch;

}  // End of start_xcommmsg



//
// This method extracts input parameters error checks then opens files for input

// error checking is not done here, screening is done at te script level (i.e. gtest)
// order of input variables is also critical and determined at the script level.
void GUI_message_driver::parse_xcommmsg_input_arguments (int argc, char* argv[])
{
/*
        // echo input parameters
        for(int i = 0; i < argc; i++)
        {
           printf("arg [%d] -> %s\n", i, argv[i]);
        }
*/
   switch (argc)
   {
      case 4 :
      {
         // currently a noop confirmation of standard xcommmsg invocation
      }

      break;

      case 6 :
      {
         if (strcmp(argv[4], INPUT_FILE_OPERATOR)  == 0)
         {
            if (strlen(argv[5]) < MAX_CHAR_BUFFER_LENGTH)
            {
               strcpy(input_file, argv[5]);
            }
            else
            {
               printf("error : could not start xcommmsg, input file name too long\n\n");
               break;
            }
            open_file_for_input (input_file);
         }
         else if (strcmp(argv[4], OUTPUT_FILE_OPERATOR) == 0)
         {
            if (strlen(argv[5]) < MAX_CHAR_BUFFER_LENGTH)
            {
               strcpy(output_file, argv[5]);
            }
            else
            {
               printf("error : could not start xcommmsg, output file name too long\n\n");
               break;
            }
            open_file_for_output (output_file);
         }
         else if (strcmp(argv[4], SNAPSHOT_OPERATOR) == 0)
         {
            if (strlen(argv[5]) < MAX_CHAR_BUFFER_LENGTH)
            {
               strcpy(snapshot_filename_dir, argv[5]);
            }
            else
            {
               printf("error : could not start xcommmsg, snapshot directory name too long\n\n");
               break;
            }
            open_dir_for_snapshot (snapshot_filename_dir);
         }

      }

      break;

      case 8 :
      {
         if (strcmp(argv[4], INPUT_FILE_OPERATOR)  == 0)
         {
            if (strlen(argv[5]) < MAX_CHAR_BUFFER_LENGTH)
            {
               strcpy(input_file, argv[5]);
            }
            else
            {
               printf("error : could not start xcommmsg, input file name too long\n\n");
               break;
            }
            open_file_for_input (input_file);
         }
         else if (strcmp(argv[4], OUTPUT_FILE_OPERATOR) == 0)
         {
            if (strlen(argv[5]) < MAX_CHAR_BUFFER_LENGTH)
            {
               strcpy(output_file, argv[5]);
            }
            else
            {
               printf("error : could not start xcommmsg, output file name too long\n\n");
               break;
            }
            open_file_for_output (output_file);
         }

         if (strcmp(argv[6], OUTPUT_FILE_OPERATOR) == 0)
         {
            if (strlen(argv[7]) < MAX_CHAR_BUFFER_LENGTH)
            {
               strcpy(output_file, argv[7]);
            }
            else
            {
               printf("error : could not start xcommmsg, output file name too long\n\n");
               break;
            }
            open_file_for_output (output_file);
         }
         else if (strcmp(argv[6], SNAPSHOT_OPERATOR) == 0)
         {
            if (strlen(argv[7]) < MAX_CHAR_BUFFER_LENGTH)
            {
               strcpy(snapshot_filename_dir, argv[7]);
            }
            else
            {
               printf("error : could not start xcommmsg, snapshot directory name too long\n\n");
               break;
            }
            open_dir_for_snapshot (snapshot_filename_dir);
         }

      }

      break;

      case 10 :
      {
         if (strcmp(argv[4], INPUT_FILE_OPERATOR)  == 0)
         {
            if (strlen(argv[5]) < MAX_CHAR_BUFFER_LENGTH)
            {
               strcpy(input_file, argv[5]);
            }
            else
            {
               printf("error : could not start xcommmsg, input file name too long\n\n");
               break;
            }
            open_file_for_input (input_file);
         }

         if (strcmp(argv[6], OUTPUT_FILE_OPERATOR) == 0)
         {
            if (strlen(argv[7]) < MAX_CHAR_BUFFER_LENGTH)
            {
               strcpy(output_file, argv[7]);
            }
            else
            {
               printf("error : could not start xcommmsg, output file name too long\n\n");
               break;
            }
            open_file_for_output (output_file);
         }

         if (strcmp(argv[8], SNAPSHOT_OPERATOR) == 0)
         {
            if (strlen(argv[9]) < MAX_CHAR_BUFFER_LENGTH)
            {
               strcpy(snapshot_filename_dir, argv[9]);
            }
            else
            {
               printf("error : could not start xcommmsg, snapshot directory name too long\n\n");
               break;
            }
            open_dir_for_snapshot (snapshot_filename_dir);
         }

      }

      break;

      default :
         printf("error : could not start xcommmsg, wrong number or type of parameters\n\n");
         close_all_files(XC_FAILURE);
         exit (1);
   }

}


//
// This method opens files for input
void GUI_message_driver::open_file_for_input (char* input_file)
{
   printf("\nselected xcommmsg input file : \n%s\n", input_file);

   xcommmsg_input_file = fopen(input_file, "r");

   if (xcommmsg_input_file == NULL)
   {
      printf("\nerror: unable to open xcommmsg input file : \n%s\n", input_file);
      close_all_files(XC_FAILURE);
      exit (1);
   }

   // allow for syncing of a slow gui
   set_delay (FILE_MAINTANENCE_TIMEOUT);

}


//
// This method opens files for output
void GUI_message_driver::open_file_for_output (char* output_file)
{
   printf("\nselected xcommmsg output file : \n%s\n", output_file);

   xcommmsg_output_file = fopen(output_file, "w");

   if (xcommmsg_output_file == NULL)
   {
      printf("\nerror: unable to open xcommmsg output file : \n%s\n", output_file);
      close_all_files(XC_FAILURE);
      exit (1);
   }

   // write output file header
   write_test_file_header ();

}


//
// This method opens files for output
void GUI_message_driver::open_dir_for_snapshot (char* snapshot_filename_dir)
{
   printf("\nselected xcommmsg snapshot direcory : \n%s\n", snapshot_filename_dir);

   // determine if a temp directory exists, open
   snapshot_temp_dir = opendir(SNAPSHOT_TEMP_PATH);

   if (snapshot_temp_dir == NULL)
   {
      // directory doesn't exist, create it
      if (mkdir(SNAPSHOT_TEMP_PATH, S_IRWXU | S_IRWXG | S_IRWXO))
      {
         printf("\nerror: unable to create temporary snapshot dir : \n%s\n", SNAPSHOT_TEMP_PATH);
         close_all_files(XC_FAILURE);
         exit (1);
      }
      // now directory exists, open
      snapshot_temp_dir = opendir(SNAPSHOT_TEMP_PATH);

   }

   // determine if a directory exists, open
   snapshot_data_dir = opendir(snapshot_filename_dir);

   if (snapshot_data_dir == NULL)
   {
      // directory doesn't exist, create it
      if (mkdir(snapshot_filename_dir, S_IRWXU | S_IRWXG | S_IRWXO))
      {
         printf("\nerror: unable to create snapshot dir : \n%s\n", snapshot_filename_dir);
         close_all_files(XC_FAILURE);
         exit (1);
      }

      // now directory exists, open
      snapshot_data_dir = opendir(snapshot_filename_dir);

   }

   strcpy(system_call_buffer, "\0") ;

   // create export system call
   strcpy(system_call_buffer, EXPORT_SYSTEM_CALL) ;
   strcat(system_call_buffer, SNAPSHOT_SYSTEM_CALL) ;
   strcat(system_call_buffer, SNAPSHOT_TEMP_PATH) ;

   // set export variable
   system(system_call_buffer);

   // append header only if output file exists
   if (xcommmsg_output_file != NULL)
   {
      // append snapshot file header
      write_test_file_snapshot_header ();
   }

}


// This method manages file input
void GUI_message_driver::file_input_manager  (char char_buffer[MAX_CHAR_BUFFER_LENGTH])
{
   // capture input from file
   fgets(char_buffer, MAX_CHAR_BUFFER_LENGTH, xcommmsg_input_file);

   // skip comments
   while (char_buffer[0] == COMMENT_DELIM_CHAR)
   {
      // capture input from file
      fgets(char_buffer, MAX_CHAR_BUFFER_LENGTH, xcommmsg_input_file);
   }

   // output to standard out (i.e. xcommmsg executible)
   fputs(char_buffer, stdout);

}


// This method manages file output
void GUI_message_driver::file_output_manager  (char char_buffer[MAX_CHAR_BUFFER_LENGTH])
{
   // capture input from user and post to file
   fgets(char_buffer, MAX_CHAR_BUFFER_LENGTH, stdin);

   // remove whitespace
   strip_whitespace(char_buffer) ;

   // output the modified buffer to file
   fputs(char_buffer, xcommmsg_output_file);
   fputs(BUTTON_CAR_RETURN, xcommmsg_output_file);
}

// This method manages standard input
void GUI_message_driver::standard_input_manager (char char_buffer[MAX_CHAR_BUFFER_LENGTH])
{
   // capture input from user and post to stdin
   fgets(char_buffer, MAX_CHAR_BUFFER_LENGTH, stdin);

   // remove whitespace
   strip_whitespace(char_buffer) ;

}


//
// This method opens files for input
void GUI_message_driver::write_test_file_header ()
{

   char output_file_header [STRING_BUFFER_1K];
   char file_date_buffer [STRING_BUFFER_1C];

   // initialize char buffers
   strcpy(output_file_header, "\0");
   strcpy(file_date_buffer, "\0");

   // reset time of run pointer as to access current
   time_of_run = time(NULL);

   // obtain file date to append to the machine name abbreviation
   strftime(file_date_buffer, STRING_BUFFER_1C, "%B %d, %Y time: %r", localtime(&time_of_run));

   if (xcommmsg_input_file != NULL)
   {
      // build and output the header
      strcat(output_file_header,  COMMENT_DELIM_STRING);
      strcat(output_file_header,  BUTTON_CAR_RETURN);
      strcat(output_file_header,  COMMENT_DELIM_STRING);
      strcat(output_file_header,  " input file name : ");
      strcat(output_file_header,  input_file);
      strcat(output_file_header,  BUTTON_CAR_RETURN);
      strcat(output_file_header,  COMMENT_DELIM_STRING);
      strcat(output_file_header,  BUTTON_CAR_RETURN);
   }

   // build and output the header
   strcat(output_file_header,  COMMENT_DELIM_STRING);
   strcat(output_file_header,  BUTTON_CAR_RETURN);
   strcat(output_file_header,  COMMENT_DELIM_STRING);
   strcat(output_file_header,  " output file name : ");
   strcat(output_file_header,  output_file);
   strcat(output_file_header,  BUTTON_CAR_RETURN);
   strcat(output_file_header,  COMMENT_DELIM_STRING);
   strcat(output_file_header,  " output file date : ");
   strcat(output_file_header,  file_date_buffer);
   strcat(output_file_header,  BUTTON_CAR_RETURN);
   strcat(output_file_header,  COMMENT_DELIM_STRING);
   strcat(output_file_header,  BUTTON_CAR_RETURN);

   fputs (output_file_header,  xcommmsg_output_file);

}


//
// This method opens files for input
void GUI_message_driver::write_test_file_snapshot_header ()
{

   char output_file_header [STRING_BUFFER_1K];

   // initialize char buffers
   strcpy(output_file_header, "\0");

   // build and output the header
   strcat(output_file_header,  COMMENT_DELIM_STRING);
   strcat(output_file_header,  BUTTON_CAR_RETURN);
   strcat(output_file_header,  COMMENT_DELIM_STRING);
   strcat(output_file_header,  " snapshot directory name : ");
   strcat(output_file_header,  snapshot_filename_dir);
   strcat(output_file_header,  BUTTON_CAR_RETURN);
   strcat(output_file_header,  COMMENT_DELIM_STRING);
   strcat(output_file_header,  BUTTON_CAR_RETURN);

   fputs (output_file_header,  xcommmsg_output_file);

}


//
// This method opens files for input
void GUI_message_driver::write_test_file_footer ()
{

   char output_file_footer [STRING_BUFFER_1K];
   char file_date_buffer [STRING_BUFFER_1C];

   // initialize char buffers
   strcpy(output_file_footer, "\0");
   strcpy(file_date_buffer, "\0");

   // reset time of run pointer as to access current
   time_of_run = time(NULL);

   // obtain file date to append to the machine name abbreviation
   strftime(file_date_buffer, STRING_BUFFER_1C, "%B %d, %Y time: %r", localtime(&time_of_run));

   if (xcommmsg_input_file != NULL)
   {
      // build and output the footer
      strcat(output_file_footer,  COMMENT_DELIM_STRING);
      strcat(output_file_footer,  BUTTON_CAR_RETURN);
      strcat(output_file_footer,  COMMENT_DELIM_STRING);
      strcat(output_file_footer,  " input file name : ");
      strcat(output_file_footer,  input_file);
      strcat(output_file_footer,  BUTTON_CAR_RETURN);
   }

   // build and output the footer
   strcat(output_file_footer,  COMMENT_DELIM_STRING);
   strcat(output_file_footer,  BUTTON_CAR_RETURN);
   strcat(output_file_footer,  COMMENT_DELIM_STRING);
   strcat(output_file_footer,  " output file name : ");
   strcat(output_file_footer,  output_file);
   strcat(output_file_footer,  BUTTON_CAR_RETURN);
   strcat(output_file_footer,  COMMENT_DELIM_STRING);
   strcat(output_file_footer,  " output file date : ");
   strcat(output_file_footer,  file_date_buffer);
   strcat(output_file_footer,  BUTTON_CAR_RETURN);
   strcat(output_file_footer,  COMMENT_DELIM_STRING);
   strcat(output_file_footer,  BUTTON_CAR_RETURN);

   if (xcommmsg_input_file != NULL)
   {
      strcat(output_file_footer,  COMMENT_DELIM_STRING);
      strcat(output_file_footer,  " ***** SUCCESSFUL COMPLETION OF TEST *****");
      strcat(output_file_footer,  BUTTON_CAR_RETURN);
      strcat(output_file_footer,  COMMENT_DELIM_STRING);
      strcat(output_file_footer,  BUTTON_CAR_RETURN);
   }
   else
   {
      strcat(output_file_footer,  COMMENT_DELIM_STRING);
      strcat(output_file_footer,  " ***** SUCCESSFUL COMPLETION OF AUTOSCRIPT *****");
      strcat(output_file_footer,  BUTTON_CAR_RETURN);
      strcat(output_file_footer,  COMMENT_DELIM_STRING);
      strcat(output_file_footer,  BUTTON_CAR_RETURN);
   }

   // output to file
   fputs (output_file_footer,  xcommmsg_output_file);

}


//
// This method opens files for input
void GUI_message_driver::write_test_file_failure ()
{

   char output_file_failure [STRING_BUFFER_1K];
   char file_date_buffer [STRING_BUFFER_1C];

   // initialize char buffers
   strcpy(output_file_failure, "\0");
   strcpy(file_date_buffer, "\0");

   // reset time of run pointer as to access current
   time_of_run = time(NULL);

   // obtain file date to append to the machine name abbreviation
   strftime(file_date_buffer, STRING_BUFFER_1C, "%B %d, %Y time: %r", localtime(&time_of_run));

   if (xcommmsg_input_file != NULL)
   {
      // build and output the footer
      strcat(output_file_failure,  COMMENT_DELIM_STRING);
      strcat(output_file_failure,  BUTTON_CAR_RETURN);
      strcat(output_file_failure,  COMMENT_DELIM_STRING);
      strcat(output_file_failure,  " input file name : ");
      strcat(output_file_failure,  input_file);
      strcat(output_file_failure,  BUTTON_CAR_RETURN);
   }

   // build and output the footer
   strcat(output_file_failure,  COMMENT_DELIM_STRING);
   strcat(output_file_failure,  BUTTON_CAR_RETURN);
   strcat(output_file_failure,  COMMENT_DELIM_STRING);
   strcat(output_file_failure,  " output file name : ");
   strcat(output_file_failure,  output_file);
   strcat(output_file_failure,  BUTTON_CAR_RETURN);
   strcat(output_file_failure,  COMMENT_DELIM_STRING);
   strcat(output_file_failure,  " output file date : ");
   strcat(output_file_failure,  file_date_buffer);
   strcat(output_file_failure,  BUTTON_CAR_RETURN);
   strcat(output_file_failure,  COMMENT_DELIM_STRING);
   strcat(output_file_failure,  BUTTON_CAR_RETURN);

   if (xcommmsg_input_file != NULL)
   {
      strcat(output_file_failure,  COMMENT_DELIM_STRING);
      strcat(output_file_failure,  " ***** FAILURE OF TEST *****");
      strcat(output_file_failure,  BUTTON_CAR_RETURN);
      strcat(output_file_failure,  COMMENT_DELIM_STRING);
      strcat(output_file_failure,  BUTTON_CAR_RETURN);
   }
   else
   {
      strcat(output_file_failure,  COMMENT_DELIM_STRING);
      strcat(output_file_failure,  " ***** FAILURE OF AUTOSCRIPT *****");
      strcat(output_file_failure,  BUTTON_CAR_RETURN);
      strcat(output_file_failure,  COMMENT_DELIM_STRING);
      strcat(output_file_failure,  BUTTON_CAR_RETURN);
   }

   // output to file
   fputs (output_file_failure,  xcommmsg_output_file);

}


// This method opens files for output
void GUI_message_driver::close_all_files (int exit_code)
{
   // close all files
   if (xcommmsg_input_file != NULL)
   {
      printf("\nclosing xcommmsg input file : \n%s\n", input_file);

      fclose(xcommmsg_input_file);

      // set delay prior to xcommmsg exit
      set_delay (FILE_MAINTANENCE_TIMEOUT);

   }
   if (xcommmsg_output_file != NULL)
   {
      printf("\nclosing xcommmsg output file : \n%s\n", output_file);

      // append success or failure
      if (exit_code == XC_SUCCESS)
      {
         // write output file footer
         write_test_file_footer ();
      }
      else
      {
         // write output file footer
         write_test_file_failure ();
      }

      fclose(xcommmsg_output_file);

      // set delay prior to xcommmsg exit
      set_delay (FILE_MAINTANENCE_TIMEOUT);

   }

   if (snapshot_data_dir != NULL)
   {
      printf("\nclosing xcommmsg snapshot directory : \n%s\n", snapshot_filename_dir);

      // close open directories
      closedir(snapshot_data_dir);
      closedir(snapshot_temp_dir);

      // set delay prior to xcommmsg exit
      set_delay (FILE_MAINTANENCE_TIMEOUT);

   }

}


// provide output script generation for subsequent input
int GUI_message_driver::get_switch_value ()

{

   char char_buffer[MAX_CHAR_BUFFER_LENGTH];

   // initilize buffer
   strcpy(char_buffer, "\0");

   // if file open for input capture, run autoscript
   if (xcommmsg_input_file != NULL)
   {
      file_input_manager (char_buffer);
   }

   if (( xcommmsg_output_file != NULL ) && ( xcommmsg_input_file != NULL ))
   {
      fputs(char_buffer, xcommmsg_output_file);
   }

   // if file open for output capture, generate autoscript
   if (( xcommmsg_output_file != NULL ) && ( xcommmsg_input_file == NULL ))
   {
      file_output_manager (char_buffer);
   }

   // standard input, no files open
   if (( xcommmsg_input_file == NULL ) && ( xcommmsg_output_file == NULL ))
   {
      standard_input_manager (char_buffer);
   }

   // translate results to script for processing
   return(atoi(char_buffer));

}

// provide output script generation for subsequent input
void GUI_message_driver::get_input (short* short_data)

{

   char char_buffer[MAX_CHAR_BUFFER_LENGTH];

   // initilize buffer
   strcpy(char_buffer, "\0");

   // if file open for input capture, run autoscript
   if (xcommmsg_input_file != NULL)
   {
      file_input_manager (char_buffer);
   }

   if (( xcommmsg_output_file != NULL ) && ( xcommmsg_input_file != NULL ))
   {
      fputs(char_buffer, xcommmsg_output_file);
   }

   if (( xcommmsg_output_file != NULL ) && ( xcommmsg_input_file == NULL ))
   {
      file_output_manager (char_buffer);
   }

   // standard input, no files open
   if (( xcommmsg_input_file == NULL ) && ( xcommmsg_output_file == NULL ))
   {
      standard_input_manager (char_buffer);
   }

   // translate results to script for processing
   *short_data = (short)(atoi(char_buffer));

}

// provide output script generation for subsequent input
void GUI_message_driver::get_input (int* int_data)

{

   char char_buffer[MAX_CHAR_BUFFER_LENGTH];

   // initilize buffer
   strcpy(char_buffer, "\0");

   // if file open for input capture, run autoscript
   if (xcommmsg_input_file != NULL)
   {
      file_input_manager (char_buffer);
   }

   if (( xcommmsg_output_file != NULL ) && ( xcommmsg_input_file != NULL ))
   {
      fputs(char_buffer, xcommmsg_output_file);
   }

   // if file open for output capture, generate autoscript
   if (( xcommmsg_output_file != NULL ) && ( xcommmsg_input_file == NULL ))
   {
      file_output_manager (char_buffer);
   }

   // standard input, no files open
   if (( xcommmsg_input_file == NULL ) && ( xcommmsg_output_file == NULL ))
   {
      standard_input_manager (char_buffer);
   }

   // translate results to script for processing
   *int_data = (atoi(char_buffer));

}

// provide output script generation for subsequent input
void GUI_message_driver::get_input (float* float_data)

{

   char char_buffer[MAX_CHAR_BUFFER_LENGTH];

   // initilize buffer
   strcpy(char_buffer, "\0");

   // if file open for input capture, run autoscript
   if (xcommmsg_input_file != NULL)
   {
      file_input_manager (char_buffer);
   }

   if (( xcommmsg_output_file != NULL ) && ( xcommmsg_input_file != NULL ))
   {
      fputs(char_buffer, xcommmsg_output_file);
   }

   // if file open for output capture, generate autoscript
   if (( xcommmsg_output_file != NULL ) && ( xcommmsg_input_file == NULL ))
   {
      file_output_manager (char_buffer);
   }

   // standard input, no files open
   if (( xcommmsg_input_file == NULL ) && ( xcommmsg_output_file == NULL ))
   {
      standard_input_manager (char_buffer);
   }

   // translate results to script for processing
   *float_data = (atof(char_buffer));

}

// provide output script generation for subsequent input
void GUI_message_driver::get_input (char char_buffer_pointer[MAX_CHAR_BUFFER_LENGTH])

{

   char char_buffer[MAX_CHAR_BUFFER_LENGTH];

   // initilize buffer
   strcpy(char_buffer, "\0");

   // if file open for input capture, run autoscript
   if (xcommmsg_input_file != NULL)
   {
      file_input_manager (char_buffer);
   }

   if (( xcommmsg_output_file != NULL ) && ( xcommmsg_input_file != NULL ))
   {
      fputs(char_buffer, xcommmsg_output_file);
   }

   // if file open for output capture, generate autoscript
   if (( xcommmsg_output_file != NULL ) && ( xcommmsg_input_file == NULL ))
   {
      file_output_manager (char_buffer);
   }

   // standard input, no files open
   if (( xcommmsg_input_file == NULL ) && ( xcommmsg_output_file == NULL ))
   {
      standard_input_manager (char_buffer);
   }

   strcpy (char_buffer_pointer, char_buffer) ;

}

// provide output script generation for subsequent input
void GUI_message_driver::strip_whitespace (char char_buffer_pointer[MAX_CHAR_BUFFER_LENGTH])

{

   char* char_ptr;

   // check and supplant EOL and C-returns in filename buffer for following strcats
   char_ptr = strchr(char_buffer_pointer, '\n');
   if (char_ptr)
   {
      *char_ptr = '\0';
   }

   char_ptr = strchr(char_buffer_pointer, '\r');
   if (char_ptr)
   {
      *char_ptr = '\0';
   }

}

// cause a mouse event using the appropriate input vector
void GUI_message_driver::set_mouse_event (short x_location, short y_location)
{

   short            region_offset = 1;
   short            x_offset      = 0;
   short            y_offset      = 0;

   PhEvent_t        mouse_event ;
   PhRect_t         rect ;
   PhPointerEvent_t mouse_pointer_event ;

   x_offset = (short)(x_location + region_offset);
   y_offset = (short)(y_location + region_offset);

   memset(&mouse_event, 0, sizeof(PhEvent_t));
   memset(&rect, 0, sizeof(PhRect_t));
   memset(&mouse_pointer_event, 0, sizeof(PhPointerEvent_t));

   if (NULL == PhAttach (NULL, NULL))
   {
      printf("error : could not attach a photon event\n\n") ;
      close_all_files(XC_FAILURE);
      exit (1) ;
   }

   mouse_event.type            = Ph_EV_BUT_PRESS ;
   mouse_event.subtype         = 0 ;
   mouse_event.flags           = 0 ;
   mouse_event.num_rects       = 1 ;
   mouse_event.emitter.rid     = Ph_DEV_RID ;
   mouse_event.input_group     = 1 ;
   mouse_event.data_len        = sizeof(mouse_pointer_event) ;

   rect.ul.x                   = x_location;
   rect.lr.x                   = x_offset;
   rect.ul.y                   = y_location;
   rect.lr.y                   = y_offset;

   mouse_pointer_event.flags   = 0 ;
   mouse_pointer_event.buttons = Ph_BUTTON_SELECT ;

   PhEventEmit (&mouse_event, &rect, &mouse_pointer_event) ;

   set_delay(BUTTON_PRESS_TIMEOUT) ;

   memset(&mouse_event, 0, sizeof(PhEvent_t));
   memset(&rect, 0, sizeof(PhRect_t));
   memset(&mouse_pointer_event, 0, sizeof(PhPointerEvent_t));

   if (NULL == PhAttach (NULL, NULL))
   {
      printf("error : could not attach a photon event\n\n");
      close_all_files(XC_FAILURE);
      exit (1);
   }

   mouse_event.type            = Ph_EV_BUT_RELEASE ;
   mouse_event.subtype         = 0 ;
   mouse_event.flags           = 0 ;
   mouse_event.num_rects       = 1 ;
   mouse_event.emitter.rid     = Ph_DEV_RID ;
   mouse_event.input_group     = 1 ;
   mouse_event.data_len        = sizeof(mouse_pointer_event) ;

   rect.ul.x                   = x_location;
   rect.lr.x                   = x_offset;
   rect.ul.y                   = y_location;
   rect.lr.y                   = y_offset;

   mouse_pointer_event.flags   = 0 ;
   mouse_pointer_event.buttons = Ph_BUTTON_SELECT ;

   PhEventEmit (&mouse_event, &rect, &mouse_pointer_event) ;

}


// set delay method for screen captures and refresh management
void GUI_message_driver::set_delay (int milliseconds)
{

   // execute requested delay
   delay(milliseconds);

}


// Create snapshot event
void GUI_message_driver::create_snapshot (short x_location, short y_location, char file_name [MAX_CHAR_BUFFER_LENGTH])
{

   // initialize buffer
   strcpy(snapshot_filename_path, "\0") ;

   // browse the directory and remove all files before creating snapshot
   if (snapshot_temp_dir != NULL)
   {

      struct dirent* directory_entry_ptr;

      directory_true_one = 1;

      while (directory_true_one == 1)
      {
         // obtain the entry name
         directory_entry_ptr = readdir (snapshot_temp_dir);

         if (directory_entry_ptr == NULL)
         {
            directory_true_one = 0;
         }
         else
         {
            // is the file a bitmap file? check for substring existence
            if ((strcmp((directory_entry_ptr -> d_name), BITMAP_FILE_EXTENSION)) == 1)
            {
               // initialize buffer
               strcpy(system_call_buffer, "\0") ;

               // we must clear the temp directory of all *.bmp files prior to executing mv
               // create the remove system call or new screen snapshots, first we create the
               // path to the misappropriated bitmap file
               strcpy(system_call_buffer, CHMOD_SYSTEM_CALL) ;
               strcat(system_call_buffer, SNAPSHOT_TEMP_PATH) ;
               strcat(system_call_buffer, FILE_DELIM) ;
               strcat(system_call_buffer, (directory_entry_ptr -> d_name)) ;

               // execute system call
               system(system_call_buffer) ;

               // reinitialize buffer
               strcpy(system_call_buffer, "\0") ;

               // we must clear the temp directory of all *.bmp files prior to executing mv
               // create the remove system call or new screen snapshots, first we create the
               // path to the misappropriated bitmap file
               strcpy(system_call_buffer, REMOVE_SYSTEM_CALL) ;
               strcat(system_call_buffer, SNAPSHOT_TEMP_PATH) ;
               strcat(system_call_buffer, FILE_DELIM) ;
               strcat(system_call_buffer, (directory_entry_ptr -> d_name)) ;

               // execute system call
               system(system_call_buffer) ;
            }

         }

      }

   }

   set_mouse_event (x_location, y_location) ;

   // set explicit delay to assure snapshot completion
   set_delay(SNAPSHOT_TIMEOUT);

   // if the method has been invoked from snapshot utility then use the file name
   // given as input from console, otherwise use the value that is provided.
   if (file_name == NULL)
   {

      printf("++++++++++ enter a filename for snapshot w/o file extension\n"
             "++++++++++ enter selection -> ") ;

      get_input(snapshot_filename) ;

   }
   else
   {
      strcpy (snapshot_filename, file_name) ;

      printf("\n SNAPSHOT FILE NAME: %s.bmp\n", file_name);

   }

   strip_whitespace(snapshot_filename) ;

   // create filename
   strcpy(snapshot_filename_path, snapshot_filename_dir) ;
   strcat(snapshot_filename_path, "/") ;
   strcat(snapshot_filename_path, snapshot_filename) ;
   strcat(snapshot_filename_path, BITMAP_FILE_EXTENSION) ;

   // initialize buffer
   strcpy(system_call_buffer, "\0") ;

   // we must chmod all the entries in the temp directory (*.bmp files)
   // prior to executing mv
   // create the chmod system call for new screen snapshots
   strcpy(system_call_buffer, CHMOD_SYSTEM_CALL) ;
   strcat(system_call_buffer, SNAPSHOT_TEMP_PATH) ;
   strcat(system_call_buffer, FILE_DELIM) ;
   strcat(system_call_buffer, FILE_WILDCARD) ;
   strcat(system_call_buffer, BITMAP_FILE_EXTENSION) ;

   // execute system call
   system(system_call_buffer) ;

   // move the *.bmp file from tempsnap to the requested directory
   // reinitialize call buffer
   strcpy(system_call_buffer, "\0") ;

   // create the mv system call
   strcpy(system_call_buffer, MOVE_SYSTEM_CALL) ;
   strcat(system_call_buffer, SNAPSHOT_TEMP_PATH) ;
   strcat(system_call_buffer, FILE_DELIM) ;
   strcat(system_call_buffer, FILE_WILDCARD) ;
   strcat(system_call_buffer, BITMAP_FILE_EXTENSION) ;
   strcat(system_call_buffer, BLANK_STRING) ;
   strcat(system_call_buffer, snapshot_filename_path) ;

   // execute system call
   system(system_call_buffer) ;

}  // End of CREATE_SNAPSHOT


// negotiate alarm message
void GUI_message_driver::select_alarm ()

{
   int              alarm_enum ;
   int              alarm_state ;
   int              next_alarm ;
   int              single_alarm_screen ;
   int              number_of_alarm_screens;
   int              alarm_sequence;
   char             alarm_response[100] ;

   GUI_Alarm_struct alarm_struct;

   printf("+++++++ press  1  to  send an R1 alarm\n"
          "+++++++ press  2  to  send an R2 alarm\n"
          "+++++++ press  3  to  send an A1 alarm\n"
          "+++++++ press  4  to  send an A2 alarm\n"
          "+++++++ press  5  to  send an OPERATOR ATTENTION\n"
          "+++++++ press  6  to  change the state of an R1 or R2 alarm\n"
          "+++++++ press  7  to  run through all alarms sequentially\n"
          "+++++++ press  8  to  create an alarm archive\n"
          "+++++++ press  9  to  EXIT\n"
          "+++++++ enter selection -> "
          );

   // Get the choice
   switch (get_switch_value())
   {

      case 1 :
         printf("++++++++++ enter the number of the R1 ALARM\n"
                "++++++++++ enter selection -> ");

         get_input (&alarm_enum);

         if (valid_alarm(alarm_enum))
         {
            alarm_struct.alarm_name  = (ALARM_VALUES)alarm_enum;
            alarm_struct.alarm_type  = R1;
            alarm_struct.alarm_state = ACTIVE;

            Msg_Ptr_Alarm_Set        = new Msg_Alarm_Set;
            Msg_Ptr_Alarm_Set -> set (&alarm_struct);
            confirm_msg();
         }
         else
         {
            printf("\n +**** UNKNOWN ALARM MESSAGE SELECTION - start over ****+\n");
         }

         break;

      case 2 :
         printf("++++++++++ enter the number of the R2 ALARM\n"
                "++++++++++ enter selection -> ");

         get_input (&alarm_enum);

         if (valid_alarm(alarm_enum))
         {
            alarm_struct.alarm_name  = (ALARM_VALUES)alarm_enum;
            alarm_struct.alarm_type  = R2;
            alarm_struct.alarm_state = LATCHED;

            Msg_Ptr_Alarm_Set        = new Msg_Alarm_Set;
            Msg_Ptr_Alarm_Set -> set (&alarm_struct);
            confirm_msg();
         }
         else
         {
            printf("\n +**** UNKNOWN ALARM MESSAGE SELECTION - start over ****+\n");
         }

         break;

      case 3 :
         printf("++++++++++ enter the number of the A1 ALARM\n"
                "++++++++++ enter selection -> ");

         get_input (&alarm_enum);

         if (valid_alarm(alarm_enum))
         {
            alarm_struct.alarm_name  = (ALARM_VALUES)alarm_enum;
            alarm_struct.alarm_type  = A1;
            alarm_struct.alarm_state = LATCHED;

            Msg_Ptr_Alarm_Set        = new Msg_Alarm_Set;
            Msg_Ptr_Alarm_Set -> set (&alarm_struct);
            confirm_msg();
         }
         else
         {
            printf("\n +**** UNKNOWN ALARM MESSAGE SELECTION - start over ****+\n");
         }

         break;

      case 4 :
         printf("++++++++++ enter the number of the A2 ALARM\n"
                "++++++++++ enter selection -> ");

         get_input (&alarm_enum);

         if (valid_alarm(alarm_enum))
         {
            alarm_struct.alarm_name  = (ALARM_VALUES)alarm_enum;
            alarm_struct.alarm_type  = A2;
            alarm_struct.alarm_state = LATCHED;

            Msg_Ptr_Alarm_Set        = new Msg_Alarm_Set;
            Msg_Ptr_Alarm_Set -> set (&alarm_struct);
            confirm_msg();
         }
         else
         {
            printf("\n +**** UNKNOWN ALARM MESSAGE SELECTION - start over ****+\n");
         }

         break;

      case 5 :
         printf("++++++++++ enter the number of the OPERATOR ATTENTION\n"
                "++++++++++ enter selection -> ");

         get_input (&alarm_enum);

         if (valid_alarm(alarm_enum))
         {
            alarm_struct.alarm_name  = (ALARM_VALUES)alarm_enum;
            alarm_struct.alarm_type  = OPERATOR_ATTENTION;
            alarm_struct.alarm_state = ACTIVE;

            Msg_Ptr_Alarm_Set        = new Msg_Alarm_Set;
            Msg_Ptr_Alarm_Set -> set (&alarm_struct);
            confirm_msg();
         }
         else
         {
            printf("\n +**** UNKNOWN ALARM MESSAGE SELECTION - start over ****+\n");
         }

         break;

      case 6 :
         printf("++++++++++ enter the alarm state\n"
                "++++++++++ 1  for  active\n"
                "++++++++++ 2  for  nonactive\n"
                "++++++++++ enter selection -> ");

         get_input (&alarm_state);
         alarm_state -= 1;

         send_int32_message (focusInt32Msg::AlarmStateChange, alarm_state);
         break;

      case 7 :
         // initialize "next" alarm to -1 because of zero indexing and subseuent increment
         // and initialize loop control
         next_alarm              = -1;
         alarm_true_one          = 1;
         number_of_alarm_screens = 1;
         alarm_sequence          = 0;
         single_alarm_screen     = TRUE;

         while (alarm_true_one == 1)
         {
            printf("\n++++++++++ enter the desired option for the alarm review\n"
                   "++++++++++ \"n\"  an <n> to review the <n>ext alarm screen\n"
                   "++++++++++ \"#\"  any number to review specific alarm\n"
                   "++++++++++ \"x\"  an <x> to e<x>it the alarm review \n"
                   "++++++++++ enter selection -> "
                   );

            get_input (alarm_response);

            strip_whitespace(alarm_response);

            if (strcmp(alarm_response, "x") == 0)
            {
               send_int32_message (focusInt32Msg::RemoveAllStatelessScreens, 0);
               break;
            }
            else if (strcmp(alarm_response, "n") == 0)
            {

               if (single_alarm_screen)
               {
                  next_alarm += 1;

                  while (!(valid_alarm(next_alarm)))
                  {
                     next_alarm += 1;
                     if (next_alarm >= NULL_ALARM) break;         // break from valid alarm while loop
                  }

                  // break from scroll through alarm while loop
                  if (next_alarm >= NULL_ALARM) break;

                  number_of_alarm_screens = number_of_alarm_entries (next_alarm);

                  if (number_of_alarm_screens > 1)
                  {
                     single_alarm_screen = FALSE;
                     alarm_sequence      = 0;
                  }

               }
               else if (number_of_alarm_screens > 1)
               {
                  // make the appropriate adjustments to the indecies
                  number_of_alarm_screens -= 1;        // deduct the alarm subscreen number

                  alarm_sequence          += 1;        // increment the alarm subscreen index

                  // standard mouse event to more info button to get to next alarm
                  set_mouse_event (320, 445) ;

               }

            }
            else if (valid_alarm(atoi(alarm_response)))
            {
               int alarm_index = 0;

               next_alarm = atoi(alarm_response);

/*
                    while ((int)(alarms[alarm_index].alarm_name) != next_alarm)
                    {
                        alarm_index += 1;
                    }
                    strcpy (alarm_name, alarms[alarm_index].alarm_name_literal);
*/

               number_of_alarm_screens = number_of_alarm_entries (next_alarm);

               if (number_of_alarm_screens > 1)
               {
                  single_alarm_screen = FALSE;
                  alarm_sequence      = 0;
               }

            }
            else
            {
               break;
            }

            if (next_alarm >= NULL_ALARM)
            {
               printf("\n +******** ALARM TABLE COMPLETE ********+\n\n");
               break;
            }

            alarm_struct.alarm_name  = (ALARM_VALUES)next_alarm;
            alarm_struct.alarm_type  = (ALARM_TYPES)get_alarm_type (next_alarm);
            alarm_struct.alarm_state = ACTIVE;

            printf("\n +******** ALARM INFORMATION ********+");
            printf("\n ALARM NUMBER : %d", (int)alarm_struct.alarm_name);
            printf("\n ALARM NAME : %s", alarm_name);
            printf("\n ALARM TYPE : %d", (int)alarm_struct.alarm_type);

            // if a multiple alarm add the alarm subscreen modifier
            if ((single_alarm_screen == FALSE) && (alarm_sequence > 0))
            {
               printf("\n ALARM SUBSCREEN NUMBER : %d\n", alarm_sequence);
            }

            Msg_Ptr_Alarm_Set = new Msg_Alarm_Set;
            Msg_Ptr_Alarm_Set -> set (&alarm_struct);

            // enable the rinseback button for alarm scroll feature
            set_delay(4000);
            send_int32_message (focusInt32Msg::RinsebackStatus, RINSEBACK_OK);

            // if we have reduced the number of alarm screens to one reset single alarm to true
            if (number_of_alarm_screens == 1)
            {
               single_alarm_screen = TRUE;
            }

         }    // End of "while (alarm_true_one == 1 ) ...

         break;

      case 8 :
         // start with alarm screen cardinality of one
         int first_alarm, last_alarm;
         number_of_alarm_screens = 1;
         single_alarm_screen     = TRUE;
         alarm_sequence          = 0;

         if (snapshot_data_dir == NULL)
         {
            printf("\n +**** SNAPSHOT UTILITY NOT INVOKED ****+\n\n");
            break;
         }

         printf("++++++++++ enter the desired option for the alarm snapshot\n"
                "++++++++++ 1  for  control alert suite snapshot\n"
                "++++++++++ 2  for  safety alarm suite snapshot\n"
                "++++++++++ enter selection -> "
                );

         switch (get_switch_value())
         {

            case 1 :
               first_alarm = _64V_TOO_HIGH;
               last_alarm  = DRBC_SPLIT_ALERT;
               break;

            case 2 :
               first_alarm = PWR_SWITCH_24V_FAILED;
               last_alarm  = HYPERVOLEMIA_DETECTED;
               break;

            default :
               printf("\n +**** UNKNOWN ALARM SUITE - start over ****+\n");

         }

         for (next_alarm = first_alarm; next_alarm <= last_alarm; next_alarm++)
         {
            if (single_alarm_screen)
            {

               while (!(valid_alarm(next_alarm)))
               {
                  next_alarm += 1;
                  if (next_alarm >= NULL_ALARM) break;        // break from valid alarm while loop
               }

               // break from scroll through alarm while loop
               if (next_alarm >= NULL_ALARM) break;

               number_of_alarm_screens = number_of_alarm_entries (next_alarm);

               if (number_of_alarm_screens > 1)
               {
                  single_alarm_screen = FALSE;
                  alarm_sequence      = 0;
               }

            }
            else if (number_of_alarm_screens > 1)
            {

               // make the appropriate adjustments to the indecies
               next_alarm              -= 1;       // remove increment of alarm since muliple screens

               number_of_alarm_screens -= 1;       // deduct the alarm subscreen number

               alarm_sequence          += 1;       // increment the name modifier

               // standard mouse event to more info button to get to next alarm
               set_mouse_event (320, 445) ;

            }

            if (next_alarm > last_alarm)
            {
               printf("\n +******** ALARM TABLE COMPLETE ********+\n\n");
               break;
            }

            // if a multiple alarm add the alarm subscreen modifier
            if ((single_alarm_screen == FALSE) && (alarm_sequence > 0))
            {
               strcpy(alarm_name_buffer, alarm_name);
               sprintf(alarm_sequence_buffer, "%02d", alarm_sequence);
               strncat(alarm_name_buffer, ALARM_NAME_DELIM_STRING, 1);
               strncat(alarm_name_buffer, alarm_sequence_buffer, 2);
            }
            else
            {
               strcpy(alarm_name_buffer, alarm_name);
            }

            alarm_struct.alarm_name  = (ALARM_VALUES)next_alarm;
            alarm_struct.alarm_type  = (ALARM_TYPES)get_alarm_type (next_alarm);
            alarm_struct.alarm_state = ACTIVE;

            printf("\n +******** ALARM INFORMATION ********+");
            printf("\n ALARM NUMBER : %d", (int)alarm_struct.alarm_name);
            printf("\n ALARM NAME : %s", alarm_name_buffer);
            printf("\n ALARM TYPE : %d", (int)alarm_struct.alarm_type);

            // if a multiple alarm add the alarm subscreen modifier
            if ((single_alarm_screen == FALSE) && (alarm_sequence > 0))
            {
               printf("\n ALARM SUBSCREEN NUMBER : %d\n", alarm_sequence);
            }

            Msg_Ptr_Alarm_Set = new Msg_Alarm_Set;
            Msg_Ptr_Alarm_Set -> set (&alarm_struct);

            // enable the rinseback button for alarm scroll feature
            set_delay(4000);
            send_int32_message (focusInt32Msg::RinsebackStatus, RINSEBACK_OK);
            set_delay(4000);

            // if snapshot directory exists, create snapshots explicitly so we don't lose our place
            // in the scroll utility
            if (snapshot_data_dir != NULL)
            {
               // set a delay to allow for screen allocation
               set_delay(10000);

               // create snaphsot event
               create_snapshot (1, 1, alarm_name_buffer);

               // set a delay for menu reset
               set_delay(4000);

            }

            // if we have reduced the number of alarm screens to one reset single alarm to true
            if (number_of_alarm_screens == 1)
            {
               single_alarm_screen = TRUE;
            }

         }

         break;

      case 9 :
         break;

      default :
         printf("\n +**** UNKNOWN ALARM MESSAGE SELECTION - start over ****+\n");

   }  // End of switch statement

}   // END  SELECT_ALARM


// Check for alarm validity
int GUI_message_driver::valid_alarm (int alarm_id)
{
   // alarm external range check
   if ((alarm_id < 0) || (alarm_id >= NULL_ALARM))
   {
      return(FALSE);
   }

   // alarm internal range check
   if ((alarm_id > (int)INVALID_ALARM_FAULT) && (alarm_id <= (int)FIRST_SAFETY_ALARM))
   {
      return(FALSE);
   }

   // alarms in the enumeration not currently used
   if ((alarm_id == (int)DEFAULT_HCT_IN_RBC) ||
       (alarm_id == (int)POSTCOUNT_TOO_LOW) ||
       (alarm_id == (int)POSTHCT_TOO_LOW) ||
       (alarm_id == (int)TOO_MUCH_REMOVE_VOLUME) ||
       (alarm_id == (int)VIBRATION_ALARM) ||
       (alarm_id == (int)APS_PAUSE) ||
       (alarm_id == (int)RETURN_CYCLE_TOO_BIG) ||
       (alarm_id == (int)AC_OFF_DURING_DRAW) ||
       (alarm_id == (int)PLASMA_VOLUME_ALERT) ||
       (alarm_id == (int)PLATELET_CONCENTRATION_ALERT) ||
       (alarm_id == (int)PLATELET_VOLUME_ALERT) ||
       (alarm_id == (int)COUNT_PRODUCT) ||
       (alarm_id == (int)LRS_PRODUCT_ALERT) ||
       (alarm_id == (int)PUMP_TOLERANCE_CURRENT) ||
       (alarm_id == (int)PUMP_TOLERANCE_PREVIOUS) ||
       (alarm_id == (int)PLATELET_YIELD_ALERT) ||
       (alarm_id == (int)APS_DURING_PAUSE))  // mapped to regular APS alarm
   {
      return(FALSE);
   }

   return(TRUE);

}   // END  VALID_ALARM



// Check for the number of screens for an individual alarm
int GUI_message_driver::number_of_alarm_entries (int alarm_id)
{
   GuiAlarmTable alarmTable;

   int           starting_alarm_entry;
   // int alarm_index = 0;
   int           alarm_entry_index = 0;

   // find first occurrence of alarm
   ALARM_TABLE* alarmEntry = alarmTable.GetEntry(alarm_id);
/*
    while ((int)(alarms[alarm_index].alarm_name) != alarm_id)
    {
        alarm_index += 1;
    }
*/

   starting_alarm_entry = (int)(alarmEntry->alarm_name);
   // strcpy (alarm_name, alarmEntry->alarm_name_literal);

   for (;; )
   {
      alarmEntry = alarmTable.GetNextEntry();

      if (alarmEntry->alarm_name == starting_alarm_entry)
      {
         alarm_entry_index += 1;
      }
      else
      {
         break;
      }
   }

/*
    while ((int)(alarms[alarm_index].alarm_name) == starting_alarm_entry)
    {
        alarm_entry_index += 1;
        alarm_index += 1;
    }
*/

   return(alarm_entry_index);

}   // END  NUMBER_OF_ALARM_ENTRIES



// Get alarm type
int GUI_message_driver::get_alarm_type (int alarm_id)
{

   int alarm_table_index = 0;

   // alarm external range check
   while (alarm_id != (int)(_AlarmsArray[alarm_table_index].getName()))
   {
      alarm_table_index += 1;
   }

   return((int)_AlarmsArray[alarm_table_index].getType());

}   // END  GET_ALARM_TYPE



// Send a one off goto screen message
void GUI_message_driver::goto_screen (GUI_SCREEN_ID screen_id, char* parameter)
{
   screen_choice.screen_id = screen_id ;
   strcpy(screen_choice.alloc_param, parameter) ;

   Msg_Ptr_Switch_to_Screen = new Msg_Switch_to_Screen;
   Msg_Ptr_Switch_to_Screen -> set (&screen_choice);
   confirm_msg();
}   // END  GOTO_SCREEN



// Send an int32 message
void GUI_message_driver::send_int32_message (focusInt32Msg::INT32_MESSAGES msg_type, long msg_value)
{
   focusInt32Msg* msg_ptr ;

   // Create an instance of the Int32 message
   msg_ptr = new focusInt32Msg (msg_type) ;

   // Send the message - along with its parameter data
   msg_ptr -> set (msg_value) ;

   confirm_msg();

}   // END  SEND_INT32_MESSAGE



// Confirm transmission of message
void GUI_message_driver::confirm_msg ()
{

   printf("\nmessage sent ...\n");

}  // End of Confirm transmission of message

/* FORMAT HASH 281a2b710aab0b19529a053b2e567402 */

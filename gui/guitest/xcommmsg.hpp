/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996, 1997, 1998 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb, Terry Wahl

*     Class name:  GUI_message_driver

*     File name:   xcommmsg.hpp

*     Contents:    Test code for the GUI class structures

*     Description: Class header file for xcommmsg.cpp

*     Documents:
          Requirements:    I:\....
          Select OMT:      I:\....
          Test:            I:\....

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: I:/BCT_Development/vxWorks/Trima/gui/guitest/rcs/xcommmsg.hpp 1.8 2002/02/01 18:46:13Z rm10919 Exp rm70006 $
$Log: xcommmsg.hpp $
Revision 1.8  2002/02/01 18:46:13Z  rm10919
Enhanced service, predict, sysdonor and donordl screen messages.
Revision 1.7  2001/10/01 23:29:23Z  rm10919
added vista sysdonor ability
Revision 1.6  2001/05/15 14:44:48Z  sb07663
Initial rev for V5 screen snapshot capability
Revision 1.5  2000/08/14 18:35:25  sb07663
IT4564: consolidation of config.dat parameter tables
Revision 1.4  2000/06/22 21:37:46  ms10234
Move the Run Summary structure to the trima common project
Revision 1.3  2000/06/06 19:35:42  ms10234
Messages were moved from the common project to the trima project
Revision 1.2  2000/04/06 23:57:25  MS10234
Changed include file names
Revision 1.1  2000/02/09 20:02:28  JR08014
Initial revision
Revision 1.16  1999/08/11 00:11:33  TW09453
Update GUI test driver for 3.3a
Revision 1.15  1999/05/21 18:20:39  TW09453
Add test messaging for new predict screen and adjust screen
functioning
Revision 1.14  1999/04/28 23:18:06  TW09453
Make structure, define, enum changes throughout the program to
keep in sync with Proc changes made in Adjust and Predict
messaging.
Revision 1.13  1998/08/12 19:59:50  tw09453
Add the ability to send "section updated" messages (AOT) via
main selection 7
Revision 1.12  1998/08/05 22:03:58  MC03015
addition of alarm type information for alarm scroll feature screen snapshots
Revision 1.11  1998/07/07 22:57:52  MC03015
updated to remove external tbv information
Revision 1.10  1998/04/20 19:35:33  MC03015
addition of alarm autosequencing for snapshots
Revision 1.9  1998/04/09 00:57:22  MC03015
change snap time out and file maintanance timeout
Revision 1.8  1998/04/01 04:00:27  MC03015
extend snapshot time for reliability, fix keypad autorun
Revision 1.7  1998/03/27 22:45:42  MC03015
addition of alarm table scroll feature
Revision 1.5  1998/02/06 04:12:26  MC03015
addition of 30 sec syncing delay for snapshots.
addition of memeet of donvitals struct prior to units implementation.
addition of clear all entries for the doninfo screen.
Revision 1.4  1998/01/28 23:40:26  MC03015
addition of procedure tim start logic and alarm number printout logic
Revision 1.3  1997/12/04 19:07:41  MC03015
modification of timeout values
Revision 1.2  1997/12/03 20:10:21  MC03015
changed tempsnap directory location
Revision 1.1  1997/12/03 19:16:46  MC03015
Initial revision
*/

#include <Pt.h>
#include <Ph.h>
#include <Ap.h>
#include <stdio.h>
#include <iostream.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// DIRECTORY BROWSE INCLUDES
#include <dirent.h>


// MESSAGE INCLUDES
#include "buffmsg.hpp"
#include "msg.hpp"


// GUI INCLUDES
#include "gui.h"
#include "bwidget.hpp"


// CONFIG INCLUDES
#include "cfg_ids.h"


// PROCEDURE STATE INCLUDES
#include "states.h"


// SAFETY LIMIT INCLUDES
#include "safproc.h"
#include "runsummary.h"


// CASSETTE INCLUDES
#include "cassette.h"


// ALARM INCLUDES
#include "alarms.h"
#include "alarmtb.h"


// KEYPAD INCLUDES
#include "keypad.h"

#include "barcode_repository.hpp"
#include "barcode_screen.hpp"


// TEMPORARY SCREEN INCLUDES
#include "guiproc.h"
#include "predict_msgs.h"

// Service Meter Numbers
#include "meter.hpp"

// DEFINE FOR XCOMMMSG SUCCESS
#define  XC_SUCCESS                    0
#define  XC_FAILURE                    1

// DEFINES FOR OPERATOR ARGUMENTS
#define  INPUT_FILE_OPERATOR       "-infile"
#define  OUTPUT_FILE_OPERATOR      "-outfile"
#define  SNAPSHOT_OPERATOR         "-snapshot"

// DEFINES FOR SNAPSHOT MANIPULATION
#define  SNAPSHOT_TEMP_PATH        "/home1/tempsnap"       // must match gtest and gdbtest entry
#define  FILE_DELIM                "/"
#define  FILE_WILDCARD             "*"
#define  BITMAP_FILE_EXTENSION     ".bmp"
#define  ALARM_NAME_DELIM_STRING   "_"

// DEFINES FOR MAX GENERIC LENGTH
#define  MAX_CHAR_BUFFER_LENGTH      256

// DEFINE FOR MAX LENGTH OUTPUT FILE HEADER
#define  STRING_BUFFER_1K           1000

// DEFINES FOR STANDARD TIMEOUTS IN MILLISECONDS
#define  BUTTON_PRESS_TIMEOUT        333
#define  FILE_MAINTANENCE_TIMEOUT   3000
#define  SNAPSHOT_TIMEOUT          30000

// DEFINE FOR C-RETURN
#define  BUTTON_CAR_RETURN         "\n"

// DEFINES FOR SYSTEM CALL CONSTRUCTION
#define  MOVE_SYSTEM_CALL          "mv "
#define  REMOVE_SYSTEM_CALL        "rm -f "
#define  CHMOD_SYSTEM_CALL         "chmod 777 "
#define  EXPORT_SYSTEM_CALL        "export "
#define  SNAPSHOT_SYSTEM_CALL      "GUI_SNAPSHOT="

// DEFINE FOR COMMENTING OUTPUT AND INPUT FILES
#define  COMMENT_DELIM_STRING      "#"
#define  COMMENT_DELIM_CHAR        '#'

// DEFINE FOR STANDARD GUI DELAY
#define  MESSAGE_PROCESSING_ENTRY  "8"
#define  MESSAGE_PROCESSING_DELAY  "4000"
#define  SNAPSHOT_PROCESSING_DELAY "30000"



// CLASS DECLARES

class GUI_message_driver

{

private:

   // ATTRIBUTES

   // while loop control variables
   int true_one;                      // eliminate compiler warning
   int donvital_true_one;             // eliminate compiler warning
   int doninfo_true_one;              // eliminate compiler warning
   int predict_true_one;              // eliminate compiler warning
   int runproc_true_one;              // eliminate compiler warning
   int trouble_true_one;              // eliminate compiler warning
   int runsumm1_true_one;             // eliminate compiler warning
   int runsumm2_true_one;             // eliminate compiler warning
   int alarm_true_one;                // eliminate compiler warning
   int directory_true_one;            // eliminate compiler warning
   int service_true_one;              // eliminate compiler warning

   // system specific parameters
   short x_mouse_location ;
   short y_mouse_location ;
   int   cassette_id ;
   int   screen_id ;
   float milliseconddelay ;


   // donvital screen numeric input values
   int   donvitalsheightunits ;
   int   donvitalsweightunits ;
   float donvitalsheight ;
   float donvitalsweight ;
   float donvitalstbv ;

   // doninfo screen numeric input values
   int   doninfobloodtype ;
   float doninfohct ;
   float doninfoplateletprecount ;
   float doninfotbv ;

   // predict screen numeric input values
   float predictprevplatelets;
   float predictprevplasma;
   float predictprevrbcs;
   float predictprevrepfluid ;
   float predictprevtime;
   float predictprevmagnify;
   float predictestplatelets;
   float predictestplasma;
   float predictestrbcs;
   float predictestrepfluid;
   float predictesttime;
   float predictestmagnify;
   float predictnextplatelets;
   float predictnextplasma;
   float predictnextrbcs;
   float predictnextrepfluid;
   float predictnexttime;
   float predictnextmagnify;


   // runproc screen numeric input values
   float runprocstarttime ;
   float runproccurrenttime ;
   float runproctimeremaining ;
   float runprocaccesspressure ;
   float runproccurrentplatelets ;
   float runproccurrentmaxpressure ;
   float runproccurrentminpressure ;
   float runproccurrentplasma ;
   float runproccurrentrbcs ;
   float runprocestimatedplatelets ;
   float runprocestimatedplasma ;
   float runprocestimatedrbcs ;

   // trouble screen numeric input values
   float                    troubledrawmax ;
   float                    troubledrawcap ;
   float                    troubledrawvalue ;
   float                    troublereturnmax ;
   float                    troublereturncap ;
   float                    troublereturnvalue ;
   float                    troubleacreactionmax ;
   float                    troubleacreactioncap ;
   float                    troubleacreactionvalue ;
   float                    troubleclumpingmax ;
   float                    troubleclumpingcap ;
   float                    troubleclumpingvalue ;
   int                      trouble_button_status ;
   BUTTON_VISIBILITY_STATUS trouble_btn_spillover ;
   BUTTON_VISIBILITY_STATUS trouble_btn_air_block ;
   BUTTON_VISIBILITY_STATUS trouble_btn_saline_bolus ;


   // runsumm screen numeric input values
   float runsummtotalacused ;
   float runsummendofruntime ;
   float runsummlengthoftime ;
   float runsummpostplateletcount ;
   float runsummposthct ;
   float runsummactodonor ;
   float runsummtotalbloodvolumeprocessed ;
   float runsummplateletvolume ;
   float runsummplateletyield ;
   float runsummvolumeofacinplatelets ;
   float runsummplasmavolume ;
   float runsummvolumeofacinplasma ;
   float runsummrbcvolume ;
   float runsummvolumeofacinrbcs ;

   // Service Mode numeric input values
   float machinetime;
   float centrifugetime;
   float numberRuns;

   // character buffers for conversions
   char predict_call_type[STRING_BUFFER_1C];            // conversion buffer for alloc
   char keypad_allocation_string[STRING_BUFFER_1C];     // conversion buffer for keypad
   char user_comment[MAX_CHAR_BUFFER_LENGTH];           // conversion buffer for user comment
   char input_file[MAX_CHAR_BUFFER_LENGTH];             // local storage for input file name
   char output_file[MAX_CHAR_BUFFER_LENGTH];            // local storage for output file name
   char snapshot_filename[MAX_CHAR_BUFFER_LENGTH];      // conversion buffer for filename
   char snapshot_filename_dir[MAX_CHAR_BUFFER_LENGTH];  // conversion buffer for directory
   char snapshot_filename_path[MAX_CHAR_BUFFER_LENGTH]; // conversion buffer for filename and directory
   char system_call_buffer[MAX_CHAR_BUFFER_LENGTH];     // conversion buffer for system call
   // char  alarm_name[MAX_CHAR_BUFFER_LENGTH];//alarm name
   char alarm_name_buffer[MAX_CHAR_BUFFER_LENGTH];     // conversion buffer for alarm name
   char alarm_sequence_buffer[MAX_CHAR_BUFFER_LENGTH]; // conversion buffer for alarm number

   // file pointers
   FILE* xcommmsg_input_file;
   FILE* xcommmsg_output_file;
   FILE* misplaced_bitmap_file;
   DIR*  snapshot_temp_dir;
   DIR*  snapshot_data_dir;

   // time element pointer
   time_t time_of_run;


   // MESSAGE DATA ATTRIBUTES

   // time structure for end of run capture
   time_t time_struct ;

   // necessary screen buffer messaging structures
   RUN_SUMMARY_STRUCT              run_summ1;
   RUN_SUMMARY_STRUCT              run_summ2;
   SDonorVitals                    donor_vitals;
   CURRENT_PROCEDURE_STATUS_STRUCT proc_status;
   ADJUST_SCREEN_CONFIG            proc_adjust;
   ProcedureInformationStruct      proc_predict;
   ProcedureInformationStruct      local_predict_status_buffer;
   SCREEN_INVOKE_STRUCT            screen_choice;
   BARCODE_STRUCT                  barcode_cat;
   Meter_Return_Struct             meter_service;

   // corresponding screen buffer messaging pointers
   focusBufferMsg<RUN_SUMMARY_STRUCT>*              runsumm_ptr;
   focusBufferMsg<SDonorVitals>*                    donor_vitals_ptr;
   focusBufferMsg<CURRENT_PROCEDURE_STATUS_STRUCT>* proc_status_ptr;
   focusBufferMsg<ADJUST_SCREEN_CONFIG>*            proc_adjust_ptr;
   focusBufferMsg<ProcedureInformationStruct>*      proc_predict_ptr;
   focusBufferMsg<BARCODE_STRUCT>*                  barcode_cat_ptr;
   focusBufferMsg<Meter_Return_Struct>*             meter_service_ptr;


   //
   // This method opens files for input
   void open_file_for_input (char* input) ;

   //
   // This method opens files for output
   void open_file_for_output (char* output) ;

   //
   // This method opens files for output
   void open_dir_for_snapshot (char* snapshot_filename_dir) ;

   //
   // This method manages file input
   void file_input_manager (char char_buffer[MAX_CHAR_BUFFER_LENGTH]) ;

   //
   // This method manages file output
   void file_output_manager (char char_buffer[MAX_CHAR_BUFFER_LENGTH]) ;

   //
   // This method manages standard input
   void standard_input_manager (char char_buffer[MAX_CHAR_BUFFER_LENGTH]) ;

   //
   // This method opens writes an output file header
   void write_test_file_header () ;

   //
   // This method opens writes an output file header
   void write_test_file_snapshot_header () ;

   //
   // This method opens writes an output file header
   void write_test_file_footer () ;

   //
   // This method opens writes an output file header
   void write_test_file_failure () ;

   //
   // This method closes all files
   void close_all_files (int exit_code) ;

   //
   // This method allows you to input switch values from different sources
   int get_switch_value () ;

   //
   // This method allows you to manage input from different sources
   void get_input (short* short_data) ;

   //
   // This method allows you to manage input from different sources
   void get_input (int* int_data) ;

   //
   // This method allows you to manage input from different sources
   void get_input (float* float_data) ;

   //
   // This method allows you to manage input from different sources
   void get_input (char char_buffer_pointer[MAX_CHAR_BUFFER_LENGTH]) ;

   //
   // This method allows you to manage input from different sources
   void strip_whitespace (char char_buffer_pointer[MAX_CHAR_BUFFER_LENGTH]) ;

   //
   // This method allows you to generate a mouse event
   void set_mouse_event (short x_location, short y_location) ;

   //
   // This method allows you to modify programatic delay
   void set_delay (int milliseconds) ;

   //
   // This method allows you to generate a snapshot event
   void create_snapshot (short x_location, short y_location, char file_name [MAX_CHAR_BUFFER_LENGTH]) ;

   //
   // This method allows you to manipulate alarms independently
   void select_alarm () ;

   //
   // This method allows you to check for valid alarms
   int valid_alarm (int alarm_id) ;

   //
   // This method allows you to check for the number of alarms in a sequence
   int number_of_alarm_entries (int alarm_id) ;

   //
   // This method retrieves alarm type
   int get_alarm_type (int alarm_id) ;

   //
   // This method handles all goto screen invocations
   void goto_screen (GUI_SCREEN_ID screen_id, char* parameter) ;

   //
   // This method manages all int32 message invocations
   void send_int32_message (focusInt32Msg::INT32_MESSAGES msg_type, long msg_value) ;

   //
   // This method generates a small message confirmation message
   void confirm_msg () ;

protected:



public:



   //
   // This method opens files for input
   void parse_xcommmsg_input_arguments (int argc, char* argv[]) ;

   //
   // This method is an affordance for remote execution
   void start_xcommmsg (int argc, char* argv[]) ;


   // CONSTRUCTOR
   GUI_message_driver();


   // DESTRUCTOR
   ~GUI_message_driver();


private:


/////////////// MESSAGING DECLARATIONS AND SUPPORTING POINTERS ////////////////


   // DECLARE A MESSAGE CLASS: MSG_DISPLAY_CHILD_SCREEN
   //  PURPOSE: This message will request the display of a child screen on top
   //  of another or a procedure screen.

   class Msg_Display_Child_Screen
      : public focusBufferMsg<SCREEN_INVOKE_STRUCT>
   {
   private:


   public:

      // CONSTRUCTOR
      Msg_Display_Child_Screen ()
         : focusBufferMsg<SCREEN_INVOKE_STRUCT> (ScreenInvokeChild)
      {}

      // DESTRUCTOR
      ~Msg_Display_Child_Screen()
      {}


   };  // End of Msg_Display_Child_Screen Class Declaration


   // DECLARE A MESSAGE CLASS: MSG_DISPLAY_STATELESS_SCREEN
   //  PURPOSE: This message will request the display of a stateless screen on top
   //  of another or a procedure screen.

   class Msg_Display_Stateless_Screen
      : public focusBufferMsg<SCREEN_INVOKE_STRUCT>
   {
   private:


   public:

      // CONSTRUCTOR
      Msg_Display_Stateless_Screen ()
         : focusBufferMsg<SCREEN_INVOKE_STRUCT> (ScreenInvokeStateless)

      {}

      // DESTRUCTOR
      ~Msg_Display_Stateless_Screen()
      {}


   };  // End of Msg_Display_Stateless_Screen Class Declaration


   // DECLARE A MESSAGE CLASS: MSG_SWITCH_TO_SCREEN
   //  PURPOSE: This message will request the switching from one procedure screen
   //  to another.

   class Msg_Switch_to_Screen
      : public focusBufferMsg<SCREEN_INVOKE_STRUCT>
   {
   private:


   public:

      // CONSTRUCTOR
      Msg_Switch_to_Screen ()
         : focusBufferMsg<SCREEN_INVOKE_STRUCT> (ScreenInvoke)
      {}

      // DESTRUCTOR
      ~Msg_Switch_to_Screen()
      {}


   };  // End of Msg_Switch_to_Screen Class Declaration


   // DECLARE A MESSAGE CLASS: MSG_SWAP_STATELESS_SCREEN
   //  PURPOSE: This message will request the swapping from one stateless screen
   //  to another.

   class Msg_Swap_Stateless_Screen
      : public focusBufferMsg<SCREEN_INVOKE_STRUCT>
   {
   private:


   public:

      // CONSTRUCTOR
      Msg_Swap_Stateless_Screen ()
         : focusBufferMsg<SCREEN_INVOKE_STRUCT> (ScreenSwapStateless)
      {}

      // DESTRUCTOR
      ~Msg_Swap_Stateless_Screen()
      {}


   };  // End of Msg_Swap_Stateless_Screen Class Declaration


   // DECLARE A MESSAGE CLASS: MSG_SWAP_STATELESS_STACK_SCREEN
   //  PURPOSE: This message will request the swapping of the stateless stack for
   //  a stateless screen.

   class Msg_Swap_Stateless_Stack_Screen
      : public focusBufferMsg<SCREEN_INVOKE_STRUCT>
   {
   private:


   public:

      // CONSTRUCTOR
      Msg_Swap_Stateless_Stack_Screen ()
         : focusBufferMsg<SCREEN_INVOKE_STRUCT> (ScreenSwapStackStateless)
      {}

      // DESTRUCTOR
      ~Msg_Swap_Stateless_Stack_Screen()
      {}


   };  // End of Msg_Swap_Stateless_Stack_Screen Class Declaration


   // DECLARE A MESSAGE CLASS: MSG_SWAP_CHILD_SCREEN
   //  PURPOSE: This message will request the swapping from one CHILD screen
   //  to another.

   class Msg_Swap_Child_Screen
      : public focusBufferMsg<SCREEN_INVOKE_STRUCT>
   {
   private:


   public:

      // CONSTRUCTOR
      Msg_Swap_Child_Screen ()
         : focusBufferMsg<SCREEN_INVOKE_STRUCT> (ScreenSwapChild)
      {}

      // DESTRUCTOR
      ~Msg_Swap_Child_Screen()
      {}


   };  // End of Msg_Swap_Child_Screen Class Declaration


   // DECLARE A MESSAGE CLASS: MSG_SWAP_CHILD_STACK_SCREEN
   //  PURPOSE: This message will request the swapping of the CHILD screen stack
   //  for a single child screen.

   class Msg_Swap_Child_Stack_Screen
      : public focusBufferMsg<SCREEN_INVOKE_STRUCT>
   {
   private:


   public:

      // CONSTRUCTOR
      Msg_Swap_Child_Stack_Screen ()
         : focusBufferMsg<SCREEN_INVOKE_STRUCT> (ScreenSwapStackChild)
      {}

      // DESTRUCTOR
      ~Msg_Swap_Child_Stack_Screen()
      {}


   };  // End of Msg_Swap_Child_Stack_Screen Class Declaration


   // DECLARE A MESSAGE CLASS: Msg_Donor_Vitals_Info
   //  PURPOSE: This message will request a message to be sent to the screens
   //  needing the current donor vitals

   class Msg_Donor_Vitals_Info
      : public focusBufferMsg<SDonorVitals>
   {
   private:


   public:

      // CONSTRUCTOR
      Msg_Donor_Vitals_Info ()
         : focusBufferMsg<SDonorVitals> (SafetyToGuiDonorVitals)
      {}

      // DESTRUCTOR
      ~Msg_Donor_Vitals_Info()
      {}


   };  // End of Msg_Donor_Vitals_Info Class Declaration


   // DECLARE A MESSAGE CLASS: Msg_Adjust_Status_Info
   //  PURPOSE: This message will request a message to be sent to the screens
   //  needing the current adjust status

   class Msg_Adjust_Status_Info
      : public focusBufferMsg<ADJUST_SCREEN_CONFIG>
   {
   private:


   public:

      // CONSTRUCTOR
      Msg_Adjust_Status_Info ()
         : focusBufferMsg<ADJUST_SCREEN_CONFIG> (ProcedureToGUIAdjust)
      {}

      // DESTRUCTOR
      ~Msg_Adjust_Status_Info()
      {}


   };  // End of Msg_Adjust_Status_Info Class Declaration


   // DECLARE A MESSAGE CLASS: Msg_Procedure_Predict_Info
   //  PURPOSE: This message will request a message to be sent to all screens
   //  needing the current predict status

   class Msg_Procedure_Predict_Info
      : public focusBufferMsg<ProcedureInformationStruct>
   {
   private:


   public:

      // CONSTRUCTOR
      Msg_Procedure_Predict_Info ()
         : focusBufferMsg<ProcedureInformationStruct> (GUIConfigToProcedure)
      {}

      // DESTRUCTOR
      ~Msg_Procedure_Predict_Info()
      {}


   };  // End of Msg_Procedure_Predict_Info Class Declaration


   // DECLARE A MESSAGE CLASS: Msg_Procedure_Status_Info
   //  PURPOSE: This message will request a message to be sent to all screens
   //  needing the current procedure status

   class Msg_Procedure_Status_Info
      : public focusBufferMsg<CURRENT_PROCEDURE_STATUS_STRUCT>
   {
   private:


   public:

      // CONSTRUCTOR
      Msg_Procedure_Status_Info ()
         : focusBufferMsg<CURRENT_PROCEDURE_STATUS_STRUCT> (CurrentProcedureStatus)
      {}

      // DESTRUCTOR
      ~Msg_Procedure_Status_Info()
      {}


   };  // End of Msg_Procedure_Status_Info Class Declaration


   // DECLARE A MESSAGE CLASS: Msg_Alarm_Set
   //  PURPOSE: This message will request a message to be sent that allocates the
   //  alarm child screen

   class Msg_Alarm_Set
      : public focusBufferMsg<GUI_Alarm_struct>
   {
   private:


   public:

      // CONSTRUCTOR
      Msg_Alarm_Set ()
         : focusBufferMsg<GUI_Alarm_struct> (AlarmDisplayRequest)
      {}

      // DESTRUCTOR
      ~Msg_Alarm_Set()
      {}


   };  // End of Msg_Alarm_Set Class Declaration





////////////POINTERS TO EACH ALLOCATED BUFFER "SEND" MESSAGE OBJECT//////////////


   Msg_Display_Child_Screen*        Msg_Ptr_Display_Child_Screen ;

   Msg_Switch_to_Screen*            Msg_Ptr_Switch_to_Screen ;

   Msg_Alarm_Set*                   Msg_Ptr_Alarm_Set ;

   Msg_Display_Stateless_Screen*    Msg_Ptr_Display_Stateless_Screen ;

   Msg_Swap_Stateless_Screen*       Msg_Ptr_Swap_Stateless_Screen ;

   Msg_Swap_Child_Screen*           Msg_Ptr_Swap_Child_Screen ;

   Msg_Swap_Stateless_Stack_Screen* Msg_Ptr_Swap_Stateless_Stack_Screen ;

   Msg_Swap_Child_Stack_Screen*     Msg_Ptr_Swap_Child_Stack_Screen ;


}; // END of GUI_message_driver Class Declaration

/* FORMAT HASH 68dd3570f123789006a1d4130240a67f */

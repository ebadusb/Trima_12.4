/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Screen_DONINFO

*     File name:   doninfo.hpp

*     Contents:    Definition of class methods and attributes

*     Description: Class header file for doninfo.cpp

*     Documents:
          Requirements:    I:\....
          Select OMT:      I:\....
          Test:            I:\....

*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.77  2008/08/04 17:03:01Z  dslausb
More GUI PFR robustication
Revision 1.76  2003/07/18 20:03:34Z  jl11312
- updated power response defines
- removed references to safproc.hpp
Revision 1.75  2003/02/12 18:34:59Z  jl11312
- corrected several problems with operation of run-related screens
- removed old GUI debug hooks
- added common code to support interface of run-related screens to proc
Revision 1.74  2003/01/06 18:31:09  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.73  2002/12/02 21:14:10  jl11312
- simplified handling of button text
Revision 1.72  2002/11/11 06:01:16  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.71  2002/10/03 20:06:31  sb07663
Dynamically create/delete message allocators at allocate/deallocate
Revision 1.70  2002/08/15 22:16:35  sb07663
Compiling version for Vx
Revision 1.69  2002/05/07 13:56:25  sb07663
First cut at VxWorks compatibility
Revision 1.68  2001/01/25 00:50:51  tw09453
Added flag in donor info to prevent messages from the keypad from
being processed once the doninfo screen has timed out or been
dismissed.
Revision 1.67  2000/08/14 18:34:46  sb07663
IT4564: consolidation of config.dat parameter tables
Revision 1.66  2000/03/14 16:08:05  MS10234
Changed include files to remove paths, and case sensitive file
names.  (All files will be specified in lower case)  The makefile was
updated with appropriate dependencies
Revision 1.65  2000/02/28 23:20:59  SB07663
IT4604: include directive changes for safproc.hpp
Revision 1.64  2000/02/28 21:56:37  SB07663
IT4604: changes to include directives for new MKS structure
Revision 1.63  1999/04/07 16:23:02  SB07663
Changed enums to be compatible with DRBC cfg_keys.h; added
logic for V3.3a optimizer messaging
Revision 1.62  1998/07/14 22:36:26  MC03015
update of donor data object messaging protocol
Revision 1.61  1998/07/02 22:04:52  MC03015
addition of donor data object data management
Revision 1.60  1998/05/21 00:17:30  MC03015
removal of external tbv massaging and graphics and status line for APII
Revision 1.59  1998/02/25 21:19:37  MC03015
addition of string id's for button face pad for translation
Revision 1.57  1998/01/29 01:14:56  MC03015
addition of logging for all SDonorVitals struct messages
Revision 1.56  1998/01/28 20:17:20  MC03015
removal of inlet_pressure_status structure element for access pressure too high and low warnings
Revision 1.55  1998/01/28 00:57:44  MC03015
Removal of INLET_PRESSURE_OK INLET_PRESSURE_NOT_OK metric for inlet pressure too low warning.  Removed all references to inlet pressure too high warning.
Revision 1.54  1998/01/17 01:37:14  MC03015
removal of unnecessary SDonorVitals struct copies.
Revision 1.53  1997/12/02 21:57:34  MC03015
dynamic rinseback confirm invocation (PPI 60) and confirm info on predict button press
Revision 1.51  1997/05/30 21:59:54  MC03015
Modification for accurate pressure warning display.
Revision 1.50  1997/05/22 15:43:04  MC03015
Addition of tbv gating value for refresh
Revision 1.49  1997/05/22 00:44:17  MC03015
Modification of string management, placement in frmtstr
Revision 1.48  1997/05/09 06:56:39  MC03015
Blood type keypad modifications
Revision 1.47  1997/03/29 00:45:09  MC03015
Addition of blood type transmission
Revision 1.46  1997/03/20 20:56:14  MC03015
Modification of flicker management
Revision 1.45  1997/03/17 22:30:14  MC03015
Revisions for beta 2.5F text and graphics
Revision 1.44  1997/03/15 02:27:18  MC03015
Addition of new keypad table and parsing logic
Revision 1.43  1997/02/27 08:55:54  MC03015
Addition of donor time - run time dependancies
Revision 1.42  1997/02/24 23:27:29  MC03015
Addition of new predict protocol for beta 2.5
Revision 1.41  1997/02/23 10:56:17  MC03015
Removal of product bars and addition of status line logic.
Revision 1.40  1997/01/29 16:54:55  MC03015
Addition of abort through rinseback logic for non-alarm instantiations
Revision 1.39  1996/12/20 09:03:23  mc03015
Modified range checking to exclude resubmission of similar values
Revision 1.38  1996/12/15 04:33:43  mc03015
APII messaging modifications
Revision 1.37  1996/11/04 19:51:43  MC03015
Removal of string compares for update flicker, not necessary anymore
Revision 1.36  1996/10/29 09:45:47  MC03015
Modifications for screen init and refresh
Revision 1.35  1996/10/27 14:47:46  MC03015
Addition of PREDICT screen messaging and bitmap logic
Revision 1.34  1996/10/26 21:02:28  MC03015
Additional debud, addition of new adjust screen stuff
Revision 1.33  1996/10/19 11:20:29  MC03015
Delta of messaging
Revision 1.32  1996/10/19 10:41:19  MC03015
Removed TEAL from status line
Revision 1.31  1996/10/17 09:11:21  MC03015
Removal of common header guitemp.h, addition of status bar, status line , and button allocation logic to DONINFO and PREDICT screens
Revision 1.30  1996/10/07 11:24:18  MC03015
Addition of PREDICT Screen
Revision 1.29  1996/09/30 23:34:30  MC03015
Changes for beta 2.3.
Revision 1.28  1996/09/24 22:17:49  MC03015
Changes for Beta 2.3, specifically text entry name modifications and additions.
Revision 1.22  1996/08/17 19:49:10  MC03015
hct must be entered before continuing implemented per cft review 8/16/96
Revision 1.21  1996/07/10 18:33:14  MC03015
addition of new header (log  and header info), maigic number removal, and string function review
*/

// Single include define guardian
#ifndef DONINFO_HPP
#define DONINFO_HPP

// GUI global element include
#include "guiglobs.hpp"

// GUI class includes
#include "aphwind.hpp"
#include "display_text.h"
#include "gui.h"
#include "scrnstat.hpp"
#include "trimamessages.h"
#include "timermessage.h"
#include "guistring.h"

// string definitions and methods
#include "frmtstr.hpp"

// state definitions
#include "states.h"

// keypad includes
#include "keypad.h"

// GUI <-> safety screen include
#include "safproc.h"

// sound include
#include "sounds.hpp"

// proc includes
#include "predict_msgs.h"


// CLASS DECLARES

class Screen_DONINFO
   : public Base_Apheresis_Window
{

private:

   // ATTRIBUTES
   const ProcedureStatus::ProcSnapshot& current_proc_data;

   // flags
   bool hct_set;                           // flag for donor hct value set

   bool tbv_set;                           // flag for donor tbv value set

   bool blood_type_set;                    // flag for blood type set since not
                                           // part of message get, yet.

   ///// Exit condition state variables
   bool exiting;                            // user-requested exit?
   bool exited;                             // true when we are exiting and no longer respond to inputs
                                            // (including messages in the queue)
   bool waiting_for_donor_data;             // true when waiting for a response to a sent DVitalsFromGui message
   bool last_update_sent;                   // true when we need to check current procedure validity
   bool send_is_valid;                      // we need to check to see if current procedure is valid
   bool wait_is_valid;                      // we're waiting on a response to the above.
   bool timeout_occurred;                   // for special timeout handling
   bool ignore_keypad_message ;
   // Ignore_keypad flag: This flag is reset on allocation and set when the process_continue_button
   // method is executed (either via a button press or timeout). This flag will gate the
   // processing of any keypad message coming back to the doninfo screen. If it is set, the
   // keypad message will be ignored. This is to resolve the problem described in I.T. 4626
   /////

   int  blood_type_btnface_last_proc;         // delta checking of proc entry

   int  donor_blood_type_text_id_to_keypad; // text id transmission to
                                            // keyad for initialization
   bool continue_allocated;                 // flag if continue button has been allocated


   float hematocrit_value_btnface_last_proc;               // delta checking of proc entry
   float platelet_precount_value_btnface_last_proc;        // delta checking of proc entry
   float hematocrit_value_btnface_last_keypad;             // delta checking of user entry
   float platelet_precount_value_btnface_last_keypad;      // delta checking of user entry
   float tbv;                                              // donor tbv


   guistring blood_type_allocation_string;     // keypad com
   guistring hematocrit_allocation_string;     // keypad com
   guistring plt_precount_allocation_string;   // keypad com

   guistring text_blood_type;        // local storage for btn text
   guistring text_hematocrit;        // local storage for btn text
   guistring text_platelet_precount; // local storage for btn text

   guistring units_hematocrit;        // local storage for hct units
   guistring units_platelet_precount; // local storage for plt precount units
   guistring units_tbv;               // local storage for tbv units

   guistring blood_type_string_btnface;
   guistring hematocrit_string_btnface;
   guistring platelet_precount_string_btnface;

   // delta checking of user entry
   guistring donor_blood_type_string_last;

   // resubmit to keypad last user entry for display
   guistring donor_blood_type_string_to_keypad;

   // character buffers for number to text conversion
   guistring tbv_string;
   guistring tbv_last;

   // character buffers for construction of display text
   guistring tbv_display;

   // character buffers for units
   // guistring predict_call_type;   // conversion buffer for alloc

   // the donor info bitmap buttons
   Button_Bitmap btn_donor_blood_type;
   Button_Bitmap btn_donor_hematocrit;
   Button_Bitmap btn_donor_platelet_precount;

   // the placard bitmap objects
   Bitmap bitmap_donation_time;

   // the display text objects
   Display_Text text_hematocrit_units;
   Display_Text text_platelet_precount_units;
   Display_Text text_tbv_units;
   Display_Text text_tbv_display;
   Display_Text text_tbv_label;
   Display_Text text_donor_blood_type_string_id;

   // access to the safety structure for parameter passing
   SDonorVitals gui_donor_vitals_struct;


   // the following structure defines all the necessary elements to convert all of
   // the string ids to blood types and visa versa
   struct  BLOOD_TYPE_TABLE
   {
      BLOODTYPE      blood_type;
      TEXT_STRING_ID blood_type_string_id;
   };

   // blood type table declaration
   BLOOD_TYPE_TABLE blood_type_strings [LAST_STRING_INDEX];

   // METHODS

   // DONOR_INFO_STRUCT_INITIALZATION
   // This method initializes donor vitals struct values as follows
   void donor_info_struct_initialization ();


   // RESET_DYNAMIC_BUTTON_PARAMETERS
   // This method initializes screen data
   void reset_dynamic_button_parameters ();


   // SCREEN_INITIALZATION
   // This method initializes screen components
   void screen_initialization ();

   // Attach and detach timers and callbacks...
   void enable_callbacks ();
   void disable_callbacks ();


   // GET_DYNAMIC_TEXT
   // This method obtains all dynamic text elements from the text database
   // for manipulation during screen allocation.
   void get_dynamic_text ();


   // FREEZE_SCREEN
   // This method disables all screen button objects.
   void freeze_screen ();


   // THAW_SCREEN
   // This method enables all screen button objects.
   void thaw_screen ();


   // SCREEN_INPUT_FORMAT
   // This method formats and outputs all data to be displayed on the screen
   void screen_input_format ();


   // DONOR_VITALS_SET_STRUCT_MANAGER
   // This method manages the donor vitals struct sent via messaging
   void donor_vitals_set_struct_manager ();


   // SET_PREDICT_REQUEST_STRUCT
   // This method manages the predict structs sent via messaging
   void set_predict_request_struct ();

   // Private Cleanup method
   void cleanup ();

   // shutdown; runs the shutdown statemachine, doesn't actually exit until
   // we're done waiting for events.
   void shutdown ();

protected:


   //
   //
   //   Messages received
   //
   Message<KEYPAD_RETURN_STRUCT> _msgKeypadCb;
   ACPrimeCompletedMsg           _msgAcPrimeComplete;

   // Screen Timer Handler pointers
   TimerMessage _timerMsgDonInfoTimeout;

   //
   //
   //   Callback handlers for received messages
   //
   void get_keypad_struct_manager ();
   void predict_complete_notify ();
   void ac_prime_complete_received ();
   void donor_vitals_get_struct_manager ();
   void screen_timer_handler ();

   void system_state_change_notify ();


public: // METHODS

   Screen_DONINFO();

   virtual ~Screen_DONINFO();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   virtual void refresh_resources ();


   // RESET_INTERNAL_SCREEN_STATE
   // This method will reset the DONINFO internal screen state
   virtual void reset_internal_screen_state ();


   // PROCESS_CANCEL_BUTTON
   // This method is called from the C callback stub and executes "exit"
   virtual void process_cancel_button ();


   // PROCESS_GOBACK_BUTTON
   // This method is called from the vitual C callback stub and executes
   // "go back"
   virtual void process_goback_button ();


   // PROCESS_PROCEDURE_TIME_BUTTON
   // This method is called from the C callback stub and executes
   // "blood type"
   void process_blood_type_button ();


   // PROCESS_HEMATOCRIT_BUTTON
   // This method is called from the C callback stub and executes
   // "hematocrit"
   void process_hematocrit_button ();


   // PROCESS_PLATELET_PRECOUNT_BUTTON
   // This method is called from the C callback stub and executes
   // "platelet precount"
   void process_platelet_precount_button ();


   // PROCESS_LOWLEFT_BUTTON
   // This method is called from the C callback stub and executes "adjust"
   virtual void process_lowleft_button ();


   // PROCESS_CONTINUE_BUTTON
   // This method is called from the virtual C callback stub and executes
   // "continue"
   virtual void process_continue_button ();

}; // END of Screen_DONINFO Class Declaration


#endif /* ifndef DONINFO_HPP */

/* FORMAT HASH c9dac18bff86ebcf231c64fedea19282 */

/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Sysdonor_Action

*     File name:   sdstate.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< TRIMA GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/sdaction.cpp 1.31 2008/07/10 20:30:46Z dslausb Exp $

*/

#include <vxworks.h>


#define TOO_BIG_ENUM 9999

// EXTERNAL REFERENCES


#include "sdaction.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR(S)

// For button bitmap changes
Sysdonor_Action::Sysdonor_Action(int p_action_id,
                                 int p_micro_action_seq_id,
                                 Screen_SYSDONOR* p_SD_instance,
                                 SYSDONOR_OBJECT p_GUI_object_name,
                                 SYSDONOR_OP p_GUI_object_operation,
                                 BITMAP_ID p_bitmap_ID)
   : // init base class data members
     GUI_Action(p_action_id, p_micro_action_seq_id),

     SD_instance(p_SD_instance),
     GUI_object_name(p_GUI_object_name),
     GUI_object_operation(p_GUI_object_operation),

     bitmap_ID(p_bitmap_ID),

// defaults
     sysdonor_is_visible(false),
     text_ID(textEmpty),
     text_color(GREY_DARK),
     statusbar_phase(STATUS_BAR_LOAD),
     delay_time(0),
     goto_screen(GUI_SCREEN_NULL_SCREEN),
     status_line_color(TEAL)
{}


// For button text string changes
Sysdonor_Action::Sysdonor_Action(int p_action_id,
                                 int p_micro_action_seq_id,
                                 Screen_SYSDONOR* p_SD_instance,
                                 SYSDONOR_OBJECT p_GUI_object_name,
                                 SYSDONOR_OP p_GUI_object_operation,
                                 TEXT_STRING_ID p_text_ID
                                 ) :

// init base class data members
   GUI_Action(p_action_id, p_micro_action_seq_id),

   SD_instance(p_SD_instance),
   GUI_object_name(p_GUI_object_name),
   GUI_object_operation(p_GUI_object_operation),

   text_ID(p_text_ID),

// defaults
   sysdonor_is_visible(false),
   bitmap_ID(BITMAP_NULL),
   text_color(GREY_DARK),
   statusbar_phase(STATUS_BAR_LOAD),
   delay_time(0),
   goto_screen(GUI_SCREEN_NULL_SCREEN),
   status_line_color(TEAL)

{}



// For button text color changes
Sysdonor_Action::Sysdonor_Action(int p_action_id,
                                 int p_micro_action_seq_id,
                                 Screen_SYSDONOR* p_SD_instance,
                                 SYSDONOR_OBJECT p_GUI_object_name,
                                 SYSDONOR_OP p_GUI_object_operation,
                                 TEXT_COLOR p_text_color
                                 )
   : // init base class data members
     GUI_Action(p_action_id, p_micro_action_seq_id),

     SD_instance(p_SD_instance),
     GUI_object_name(p_GUI_object_name),
     GUI_object_operation(p_GUI_object_operation),

     text_color(p_text_color),

// defaults
     sysdonor_is_visible(false),
     bitmap_ID(BITMAP_NULL),
     text_ID(textEmpty),
     statusbar_phase(STATUS_BAR_LOAD),
     delay_time(0),
     goto_screen(GUI_SCREEN_NULL_SCREEN),
     status_line_color(TEAL)

{}



// For Status Bar phase changes
Sysdonor_Action::Sysdonor_Action(int p_action_id,
                                 int p_micro_action_seq_id,
                                 Screen_SYSDONOR* p_SD_instance,
                                 SYSDONOR_OBJECT p_GUI_object_name,
                                 SYSDONOR_OP p_GUI_object_operation,
                                 STATUS_BAR_PHASE p_statusbar_phase
                                 )
   : // init base class data members
     GUI_Action(p_action_id, p_micro_action_seq_id),

     SD_instance(p_SD_instance),
     GUI_object_name(p_GUI_object_name),
     GUI_object_operation(p_GUI_object_operation),

     statusbar_phase(p_statusbar_phase),

// defaults
     sysdonor_is_visible(false),
     bitmap_ID(BITMAP_NULL),
     text_ID(textEmpty),
     text_color(GREY_DARK),
     delay_time(0),
     goto_screen(GUI_SCREEN_NULL_SCREEN),
     status_line_color(TEAL)

{}


// For time delays
Sysdonor_Action::Sysdonor_Action(int p_action_id,
                                 int p_micro_action_seq_id,
                                 Screen_SYSDONOR* p_SD_instance,
                                 SYSDONOR_OBJECT p_GUI_object_name,
                                 SYSDONOR_OP p_GUI_object_operation,
                                 int p_delay_time
                                 )
   : // init base class data members
     GUI_Action(p_action_id, p_micro_action_seq_id),

     SD_instance(p_SD_instance),
     GUI_object_name(p_GUI_object_name),
     GUI_object_operation(p_GUI_object_operation),

     delay_time(p_delay_time),

// defaults
     sysdonor_is_visible(false),
     bitmap_ID(BITMAP_NULL),
     text_ID(textEmpty),
     text_color(GREY_DARK),
     statusbar_phase(STATUS_BAR_LOAD),
     goto_screen(GUI_SCREEN_NULL_SCREEN),
     status_line_color(TEAL)

{}



// For going-to another screen
Sysdonor_Action::Sysdonor_Action(int p_action_id,
                                 int p_micro_action_seq_id,
                                 Screen_SYSDONOR* p_SD_instance,
                                 SYSDONOR_OBJECT p_GUI_object_name,
                                 SYSDONOR_OP p_GUI_object_operation,
                                 GUI_SCREEN_ID p_goto_screen
                                 )
   : // init base class data members
     GUI_Action(p_action_id, p_micro_action_seq_id),

     SD_instance(p_SD_instance),
     GUI_object_name(p_GUI_object_name),
     GUI_object_operation(p_GUI_object_operation),

     goto_screen(p_goto_screen),

// defaults
     sysdonor_is_visible(false),
     bitmap_ID(BITMAP_NULL),
     text_ID(textEmpty),
     text_color(GREY_DARK),
     statusbar_phase(STATUS_BAR_LOAD),
     delay_time(0),
     status_line_color(TEAL)

{}




// For object/op pairs with no parameters (e.g. visibility, activation)
Sysdonor_Action::Sysdonor_Action(int p_action_id,
                                 int p_micro_action_seq_id,
                                 Screen_SYSDONOR* p_SD_instance,
                                 SYSDONOR_OBJECT p_GUI_object_name,
                                 SYSDONOR_OP p_GUI_object_operation
                                 )
   : // init base class data members
     GUI_Action(p_action_id, p_micro_action_seq_id),

     SD_instance(p_SD_instance),
     GUI_object_name(p_GUI_object_name),
     GUI_object_operation(p_GUI_object_operation),

// defaults
     sysdonor_is_visible(false),
     bitmap_ID(BITMAP_NULL),
     text_ID(textEmpty),
     text_color(GREY_DARK),
     statusbar_phase(STATUS_BAR_LOAD),
     delay_time(0),
     goto_screen(GUI_SCREEN_NULL_SCREEN),
     status_line_color(TEAL)

{}



// For status line color changes
Sysdonor_Action::Sysdonor_Action(int p_action_id,
                                 int p_micro_action_seq_id,
                                 Screen_SYSDONOR* p_SD_instance,
                                 SYSDONOR_OBJECT p_GUI_object_name,
                                 SYSDONOR_OP p_GUI_object_operation,
                                 WINDOW_COLOR p_status_line_color
                                 )
   : // init base class data members
     GUI_Action(p_action_id, p_micro_action_seq_id),

     SD_instance(p_SD_instance),
     GUI_object_name(p_GUI_object_name),
     GUI_object_operation(p_GUI_object_operation),

     status_line_color(p_status_line_color),

// defaults
     sysdonor_is_visible(false),
     bitmap_ID(BITMAP_NULL),
     text_ID(textEmpty),
     text_color(GREY_DARK),
     statusbar_phase(STATUS_BAR_LOAD),
     delay_time(0),
     goto_screen(GUI_SCREEN_NULL_SCREEN)

{}


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR(S)
#if 0
Sysdonor_Action::Sysdonor_Action()
   : GUI_Action (0, 0) // this base initialization is to satisfy the compiler only
{}                     // END of Sysdonor_Action CONSTRUCTOR (invalid, private constructor)
#endif



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Sysdonor_Action::~Sysdonor_Action()
{}




////////////////////////////////////////////////////////////////////////////////
// COMPARE_ACTION_ID
int Sysdonor_Action::compare_action_id (int p_action_id, int p_micro_action_seq_id)
{
   if ( (p_action_id == get_action_id() ) &&
        (p_micro_action_seq_id == get_micro_action_seq_id() )
        )
   {
      return 0;
   }
   else
   {
      return -1;
   }
}



////////////////////////////////////////////////////////////////////////////////
// EXECUTE_MICRO_ACTION
void Sysdonor_Action::execute_micro_action ()
{
   static SYSDONOR_OP last_operation_donorinfo = (SYSDONOR_OP)TOO_BIG_ENUM;
   static SYSDONOR_OP last_operation_loadsys   = (SYSDONOR_OP)TOO_BIG_ENUM;
   static SYSDONOR_OP last_operation_config    = (SYSDONOR_OP)TOO_BIG_ENUM;
   static SYSDONOR_OP last_operation_beginrun  = (SYSDONOR_OP)TOO_BIG_ENUM;
   static SYSDONOR_OP last_operation_endrun    = (SYSDONOR_OP)TOO_BIG_ENUM;


   sysdonor_is_visible = false; // not visible by default

   if (SD_instance->allocated())
   {
      sysdonor_is_visible = true;
   }

   /* Excessive logging here is causing watchdog failures.  (See IT 8707)
   DataLog(log_level_gui_info) << "ActID=" << get_action_id()
      << ",MActID=" << get_micro_action_seq_id()
      << ",Obj=" << GUI_object_name
      << ",Act=" << GUI_object_operation
      << ",Bitmap=" << bitmap_ID
      << ",Text_id=" << text_ID
      << ",Textcolor=" << text_color
      << ",StatusBar=" << statusbar_phase
      << ",Delaytime=" << delay_time
      << ",Gotoscrn=" << goto_screen
      << ",StatlineCol=" << status_line_color
      << ",SYSDONOR_Visibility=" << sysdonor_is_visible
      << endmsg;

   DataLog(log_level_gui_info) << "Last Actions: DonInfo=" << last_operation_donorinfo
      << ",LoadSys=" << last_operation_loadsys
      << ",Config=" << last_operation_config
      << ",BeginRun=" << last_operation_beginrun
      << ",EndRun=" << last_operation_endrun
      << endmsg;
   */

   switch (GUI_object_name)
   {
      case SD_STATUSLINE :
         execute_statusline_action();
         break;

      case SD_STATUSBAR :
         execute_statusbar_action();
         break;

      case SD_ENDRUN_BTN :
         if ((last_operation_endrun != GUI_object_operation) || (sysdonor_is_visible == false))
         {
            last_operation_endrun = GUI_object_operation;
            execute_button_action(SD_instance->btn_cancel);
         }

         break;

      case SD_CONFIG_BTN :
         if ((last_operation_config != GUI_object_operation) || (sysdonor_is_visible == false))
         {
            last_operation_config = GUI_object_operation;
            execute_button_action(SD_instance->btn_lowleft);
         }

         break;

      case SD_CONTINUE_BTN :
         if ((last_operation_beginrun != GUI_object_operation) || (sysdonor_is_visible == false))
         {
            last_operation_beginrun = GUI_object_operation;
            execute_button_action(SD_instance->btn_continue);
         }

         break;

      case SD_LOADSYS_BTN :
         if ((last_operation_loadsys != GUI_object_operation) || (sysdonor_is_visible == false))
         {
            last_operation_loadsys = GUI_object_operation;
            execute_button_action(SD_instance->btn_system_setup);
         }

         break;

      case SD_DONORINFO_BTN :
         if ((last_operation_donorinfo != GUI_object_operation) || (sysdonor_is_visible == false))
         {
            last_operation_donorinfo = GUI_object_operation;
            execute_button_action(SD_instance->btn_donor_info);
         }

         break;

      case SD_GENERAL_FUNC :
         execute_general_action();
         break;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Unknown SYSDONOR micro action requested");

   }
}



////////////////////////////////////////////////////////////////////////////////
// EXECUTE_BUTTON_ACTION
void Sysdonor_Action::execute_button_action (Button_Bitmap& sysdonor_button)
{
   switch (GUI_object_operation)
   {
      case SDOP_BUTTON_SET_VISIBLE :
         // make sure its allocated
         if (!sysdonor_button.allocated())
         {
            sysdonor_button.allocate_resources(*SD_instance);

            // Link up the callbacks to the SYSDONOR callbacks
            switch (GUI_object_name)
            {
               case SD_ENDRUN_BTN :
                  sysdonor_button.set_callback (aph_callback_cancel, (void*)SD_instance);
                  break;

               case SD_CONFIG_BTN :
                  sysdonor_button.set_callback (aph_callback_lowleft, (void*)SD_instance);
                  break;

               case SD_CONTINUE_BTN :
                  sysdonor_button.set_callback (aph_callback_continue, (void*)SD_instance);
                  break;

               case SD_LOADSYS_BTN :
                  sysdonor_button.set_callback (sysdonor_cb_system_setup, (void*)SD_instance);
                  break;

               case SD_DONORINFO_BTN :
                  sysdonor_button.set_callback (sysdonor_cb_donor_info, (void*)SD_instance);
                  break;

               default :
                  _FATAL_ERROR (__FILE__, __LINE__, "Unknown SYSDONOR micro allocation for a bitmap button");
            }
         }

         sysdonor_button.disable_callbacks();
         break;

      case SDOP_BUTTON_SET_INVISIBLE :
         if (sysdonor_button.allocated())
         {
            sysdonor_button.deallocate_resources();
         }

         break;

      case SDOP_BUTTON_SET_ACTIVE :
         if (sysdonor_button.allocated())
         {
            sysdonor_button.enable_callbacks();
         }

         break;

      case SDOP_BUTTON_SET_INACTIVE :
         if (sysdonor_button.allocated())
         {
            sysdonor_button.disable_callbacks();
         }

         break;

      case SDOP_BUTTON_SET_UP_BITMAP :
         if (sysdonor_button.allocated())
         {
            sysdonor_button.set_up_bitmap (bitmap_ID);
         }

         break;

      case SDOP_BUTTON_SET_DN_BITMAP :
         if (sysdonor_button.allocated())
         {
            sysdonor_button.set_down_bitmap (bitmap_ID);
         }

         break;

      case SDOP_BUTTON_SET_TEXT :
         if (sysdonor_button.allocated())
         {
            sysdonor_button.set_string_id (text_ID);
         }

         break;

      case SDOP_BUTTON_SET_TEXT_COLOR :
         if (sysdonor_button.allocated())
         {
            sysdonor_button.set_button_text_color(text_color);
         }

         break;

      case SDOP_BUTTON_SYSLOAD_ALL_COL_UNCHK_ACT :
         // This makes the system setup button: color graphic; unchecked; active to touches; standard text - yellow

         // Make sure the system setup button is allocated and callback is in place
         if (!SD_instance->btn_system_setup.allocated())
         {
            SD_instance->btn_system_setup.allocate_resources(*SD_instance);

            SD_instance->btn_system_setup.set_callback (sysdonor_cb_system_setup, (void*)SD_instance);
         }

         // Set the System Setup button up bitmap to Color / UN-CHECKED
         SD_instance->btn_system_setup.set_up_bitmap (BITMAP_BUTTON_SYS_SETUP_UP);

         // Load the system setup text
         SD_instance->btn_system_setup.set_string_id (textButtonGiantLoadSystem);

         // Set the text color to yellow
         SD_instance->btn_system_setup.set_button_text_color(YELLOW);

         // Activate the System Setup button
         SD_instance->btn_system_setup.enable_callbacks();

         break;

      case SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT :
         // This makes the system setup button: grey graphic; checked; inactive to touches; standard text - light grey

         // Make sure the system setup button is allocated and callback is in place
         if (!SD_instance->btn_system_setup.allocated())
         {
            SD_instance->btn_system_setup.allocate_resources(*SD_instance);
            SD_instance->btn_system_setup.set_callback (sysdonor_cb_system_setup, (void*)SD_instance);
         }

         // Set the System Setup button up bitmap to Grey / Checked
         SD_instance->btn_system_setup.set_up_bitmap (BITMAP_BUTTON_SYS_SETUP_UP_CHKED);

         // Load the system setup text
         SD_instance->btn_system_setup.set_string_id (textButtonGiantLoadSystem);

         // Set the text color to grey
         SD_instance->btn_system_setup.set_button_text_color(GREY_LIGHT);

         // Deactivate the System Setup button
         SD_instance->btn_system_setup.disable_callbacks();
         break;

      case SDOP_BUTTON_SYSLOAD_INVISIBLE :
         // This makes the system setup button: non-existent

         // Make the system setup real invisible
         SD_instance->btn_system_setup.deallocate_resources();
         break;

      case SDOP_BUTTON_DONINFO_MAN_COL_UNCHK_ACT :
         // This makes the donor info button: manual color graphic; unchecked; active to touches; standard text - yellow

         // Make sure the donor info button is allocated and callback is in place
         if (!SD_instance->btn_donor_info.allocated())
         {
            SD_instance->btn_donor_info.allocate_resources(*SD_instance);

            SD_instance->btn_donor_info.set_callback (sysdonor_cb_donor_info, (void*)SD_instance);
         }

         // Set the donor info button up bitmap to Color / unchecked
         SD_instance->btn_donor_info.set_up_bitmap (BITMAP_BUTTON_DONOR_INFO_UP);

         // Load the donor info text
         SD_instance->btn_donor_info.set_string_id (textButtonDonorInfo);

         // Set the text color to yellow
         SD_instance->btn_donor_info.set_button_text_color(YELLOW);

         // Activate the donor info button
         SD_instance->btn_donor_info.enable_callbacks();
         break;

      case SDOP_BUTTON_DONINFO_MAN_COL_CHKED_ACT :
         // This makes the donor info button: manual color graphic; checked; active to touches; standard text - yellow

         // Make sure the donor info button is allocated and callback is in place
         if (!SD_instance->btn_donor_info.allocated())
         {
            SD_instance->btn_donor_info.allocate_resources(*SD_instance);

            SD_instance->btn_donor_info.set_callback (sysdonor_cb_donor_info, (void*)SD_instance);
         }

         // Set the donor info button up bitmap to Color / checked
         SD_instance->btn_donor_info.set_up_bitmap (BITMAP_BUTTON_DONOR_INFO_UP_CHKED);

         // Load the donor info text
         SD_instance->btn_donor_info.set_string_id (textButtonDonorInfo);

         // Set the text color to grey
         SD_instance->btn_donor_info.set_button_text_color(GREY_LIGHT);

         // Activate the donor info button
         SD_instance->btn_donor_info.enable_callbacks();
         break;

      case SDOP_BUTTON_DONINFO_AP2_GRY_UNCHK_INACT :
         // This makes the donor info button: AP2 grey graphic; UN-checked; inactive to touches; AP2 text - light grey

         // Remove any residual Donor Picture bitmap and borders
         if (SD_instance->bitmap_donor_picture != 0)
         {
            delete SD_instance->bitmap_donor_picture;
            SD_instance->bitmap_donor_picture = 0;
         }

         SD_instance->bitmap_donor_top.deallocate_resources();
         SD_instance->bitmap_donor_bottom.deallocate_resources();
         SD_instance->bitmap_donor_left.deallocate_resources();
         SD_instance->bitmap_donor_right.deallocate_resources();

         // Make sure the donor info button is allocated and callback is in place
         if (!SD_instance->btn_donor_info.allocated())
         {
            SD_instance->btn_donor_info.allocate_resources(*SD_instance);

            SD_instance->btn_donor_info.set_callback (sysdonor_cb_donor_info, (void*)SD_instance);
         }

         // Set the donor info button up bitmap to Color / unchecked
         SD_instance->btn_donor_info.set_up_bitmap (BITMAP_BUTTON_DONOR_INFO_AP2_UNCHKED_IACT);

         // Load the donor info text
         SD_instance->btn_donor_info.set_string_id (textButtonDonorInfo);

         // Set the text color to yellow
         SD_instance->btn_donor_info.set_button_text_color(GREY_LIGHT);

         // Deactivate the donor info button
         SD_instance->btn_donor_info.disable_callbacks();
         break;

      case SDOP_BUTTON_DONINFO_AP2_COL_UNCHK_ACT :
         // This makes the donor info button: AP2 color graphic; unchecked; active to touches; AP2 text - light grey
      {
         const BITMAP_ID donor_bitmap_id = guiglobs::gui_donor_data_repository.GetBitmapIdSd();

         // Remove any residual Donor Picture bitmap and borders
         if (SD_instance->bitmap_donor_picture != 0)
         {
            delete SD_instance->bitmap_donor_picture;
            SD_instance->bitmap_donor_picture = 0;
         }

         SD_instance->bitmap_donor_top.deallocate_resources();
         SD_instance->bitmap_donor_bottom.deallocate_resources();
         SD_instance->bitmap_donor_left.deallocate_resources();
         SD_instance->bitmap_donor_right.deallocate_resources();

         // Make sure the donor info button is allocated and callback is in place
         if (!SD_instance->btn_donor_info.allocated())
         {
            SD_instance->btn_donor_info.allocate_resources(*SD_instance);

            SD_instance->btn_donor_info.set_callback (sysdonor_cb_donor_info, (void*)SD_instance);
         }

         if (donor_bitmap_id == BITMAP_SYSDONOR_IMAGE_DEFAULT_MALE
             || donor_bitmap_id == BITMAP_SYSDONOR_IMAGE_DEFAULT_FEMALE)
         {
            // Set the donor info button up bitmap to the default donor info button.
            SD_instance->btn_donor_info.set_up_bitmap (donor_bitmap_id);
         }
         else
         {
            if (donor_bitmap_id == BITMAP_NULL)
            {
               _FATAL_ERROR (__FILE__, __LINE__, "Donor bitmap file not set");
            }

            // Set the donor info button up bitmap to background.  We'll overlay it with the right images.

            SD_instance->btn_donor_info.set_up_bitmap (BITMAP_BUTTON_BACKGROUND);

            // Overlay the button with the Donor Picture.

            if (SD_instance->bitmap_donor_picture == 0)
            {
               SD_instance->bitmap_donor_picture = new Bitmap (BITMAP_DONOR_PICTURE_X, BITMAP_DONOR_PICTURE_Y, donor_bitmap_id);
               SD_instance->bitmap_donor_picture->allocate_resources(SD_instance->btn_donor_info);

               // Make sure that the bitmap is realized.
               SD_instance->bitmap_donor_top.allocate_resources(SD_instance->btn_donor_info);
               SD_instance->bitmap_donor_bottom.allocate_resources(SD_instance->btn_donor_info);
               SD_instance->bitmap_donor_left.allocate_resources(SD_instance->btn_donor_info);
               SD_instance->bitmap_donor_right.allocate_resources(SD_instance->btn_donor_info);
            }
         }

         // Load the donor info text
//           SD_instance->btn_donor_info.set_string_id (textButtonDonorInfo);
         SD_instance->btn_donor_info.set_string_id (textEmpty);

         // Set the text color to yellow
         SD_instance->btn_donor_info.set_button_text_color(YELLOW);

         // Activate the donor info button
         SD_instance->btn_donor_info.enable_callbacks();
      }
      break;

      case SDOP_BUTTON_DONINFO_AP2_COL_CHKED_ACT :
         // This makes the donor info button: AP2 color graphic; checked; active to touches; AP2 text - light grey

         // Remove any residual Donor Picture bitmap and borders
         if (SD_instance->bitmap_donor_picture != 0)
         {
            delete SD_instance->bitmap_donor_picture;
            SD_instance->bitmap_donor_picture = 0;
         }

         SD_instance->bitmap_donor_top.deallocate_resources();
         SD_instance->bitmap_donor_bottom.deallocate_resources();
         SD_instance->bitmap_donor_left.deallocate_resources();
         SD_instance->bitmap_donor_right.deallocate_resources();

         // Make sure the donor info button is allocated and callback is in place
         if (!SD_instance->btn_donor_info.allocated())
         {
            SD_instance->btn_donor_info.allocate_resources(*SD_instance);

            SD_instance->btn_donor_info.set_callback (sysdonor_cb_donor_info, (void*)SD_instance);
         }

         // Set the donor info button up bitmap to grey / checked
         SD_instance->btn_donor_info.set_up_bitmap (BITMAP_BUTTON_DONOR_INFO_AP2_CHKED_ACT);

         // Load the donor info text
         SD_instance->btn_donor_info.set_string_id (textButtonDonorInfo);

         // Set the text color to grey
         SD_instance->btn_donor_info.set_button_text_color(YELLOW);

         // Activate the donor info button
         SD_instance->btn_donor_info.enable_callbacks();
         break;

      case SDOP_BUTTON_ENDRUN_ACTIVATE :
         // This makes the end run button visible and active

         // Make sure the end run button is allocated and callback is in place
         if (!sysdonor_button.allocated())
         {
            sysdonor_button.allocate_resources(*SD_instance);
            sysdonor_button.set_callback (aph_callback_cancel, (void*)SD_instance);
         }

         sysdonor_button.enable_callbacks();

         break;

      case SDOP_BUTTON_CONFIG_ACTIVATE :
         // This makes the config (lowleft) button visible and active

         // Make sure the config button is allocated and callback is in place
         if (!sysdonor_button.allocated())
         {
            sysdonor_button.allocate_resources(*SD_instance);
            sysdonor_button.set_callback (aph_callback_lowleft, (void*)SD_instance);
         }

         // Assign the "config" text literal
         sysdonor_button.set_string_id (textButtonConfig);

         // Activate the config button
         sysdonor_button.enable_callbacks();

         break;

      case SDOP_BUTTON_BEGINRUN_ACTIVATE :
         // This makes the Begin Run (continue) button visible and active

         // Make sure the continue button is allocated and callback is in place
         if (!sysdonor_button.allocated())
         {
            sysdonor_button.allocate_resources(*SD_instance);
            sysdonor_button.set_callback (aph_callback_continue, (void*)SD_instance);
         }

         // Assign the "Begin Run" text literal
         sysdonor_button.set_string_id (textButtonLongBeginDonorPrep);

         // Activate the continue button
         sysdonor_button.enable_callbacks();

         if (guiglobs::screen_status.get_current_system_state()!= AC_PRIME)
         {
            //
            //   Turn off the "don't connect" icon
            //
            guiglobs::display_dont_connect_donor_icon = 0;
            guiglobs::dont_connect_icon.set_visible(0);
         }
         break;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Unknown SYSDONOR micro action for a bitmap button");
   }  // end of switch statement
}    // END of Sysdonor_Action EXECUTE_BUTTON_ACTION



////////////////////////////////////////////////////////////////////////////////
// EXECUTE_STATUSLINE_ACTION
void Sysdonor_Action::execute_statusline_action ()
{
   static SYSDONOR_OP last_operation = (SYSDONOR_OP)TOO_BIG_ENUM;

   // Don't set the status line unless you own the screen.
   if (guiglobs::apheresis_status_line->get_owning_screen() != GUI_SCREEN_SYSDONOR)
   {
      return;
   }

   switch (GUI_object_operation)
   {
      case SDOP_STATUSLINE_RESET :
         // This will only be a partial reset: resetting the default and attention
         // modes. This will not reset the alert/alarm mode.
         if (last_operation != GUI_object_operation)
         {
            guiglobs::apheresis_status_line->deactivate_status_line_type(DEFAULT_STATUS_LINE);
            guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
            last_operation = GUI_object_operation;
         }

         break;

      case SDOP_STATUSLINE_DEACTIVATE_ATTENTION :
         if (last_operation != GUI_object_operation)
         {
            guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
            last_operation = GUI_object_operation;
         }

         break;

      case SDOP_STATUSLINE_SET_COLOR :
         if (guiglobs::apheresis_status_line->get_color () != status_line_color)
         {
            guiglobs::apheresis_status_line->set_color (status_line_color);
         }

         break;

      case SDOP_STATUSLINE_SET_TEXT :
         if (guiglobs::apheresis_status_line->get_current_text () != text_ID)
         {
            guiglobs::apheresis_status_line->set_current_text (text_ID);
         }

         break;

      case SDOP_STATUSLINE_LOAD_DONOR_NAME :
         if (guiglobs::apheresis_status_line->get_current_text () != text_ID)
         {
            SDonorVitals donorVitals;
            guiglobs::gui_donor_data_repository.get_donor_vitals_struct(&donorVitals);

            guiglobs::apheresis_status_line->set_current_text (textEmptyFormatDonDlDonorNameInit);
         }

         break;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Unknown SYSDONOR micro action for the status line");
   }  // end of the switch
}    // END of Sysdonor_Action EXECUTE_STATUSLINE_ACTION



////////////////////////////////////////////////////////////////////////////////
// EXECUTE_STATUSBAR_ACTION
void Sysdonor_Action::execute_statusbar_action ()
{
   static SYSDONOR_OP      last_operation = (SYSDONOR_OP)TOO_BIG_ENUM;
   static STATUS_BAR_PHASE last_operand   = (STATUS_BAR_PHASE)TOO_BIG_ENUM;

   // Don't set the status line unless you own the screen.
   if (guiglobs::apheresis_status_bar->get_owning_screen() != GUI_SCREEN_SYSDONOR)
   {
      return;
   }

   if ((last_operation != GUI_object_operation) || (last_operand != statusbar_phase))
   {
      last_operation = GUI_object_operation;
      last_operand   = statusbar_phase;
   }
   else
   {
      return;
   }


   switch (GUI_object_operation)
   {
      case SDOP_STATUSBAR_RESET_ALL_PHASES :
         guiglobs::apheresis_status_bar->reset_all_phases ();
         break;

      case SDOP_STATUSBAR_SET_PHASE_COMPLETE :
         guiglobs::apheresis_status_bar->set_phase_completed (statusbar_phase);
         break;

      case SDOP_STATUSBAR_SET_PHASE_ACTIVE :
         guiglobs::apheresis_status_bar->set_phase_active (statusbar_phase);
         break;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Unknown SYSDONOR micro action for the status bar");

   }  // end of the switch
}    // END of Sysdonor_Action EXECUTE_STATUSBAR_ACTION



////////////////////////////////////////////////////////////////////////////////
// EXECUTE_GENERAL_ACTION
void Sysdonor_Action::execute_general_action ()
{
   switch (GUI_object_operation)
   {
      case SDOP_GENERAL_GOTO_SCREEN :

         SD_instance->goto_screen (goto_screen, "MICRO ACTION SCREEN SWITCH");
         break;


      case SDOP_GENERAL_FREEZE_SCREEN :

         SD_instance->disable_standard_buttons();
         break;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Unknown SYSDONOR micro action for general operations");
   }  // end of the switch
}    // END of Sysdonor_Action EXECUTE_GENERAL_ACTION



//
// Stream Manipulators
//
DataLog_Stream& operator << (DataLog_Stream& os, const SYSDONOR_OBJECT& obj)
{
   switch (obj)
   {
      case SD_STATUSLINE :
         os << "STATUSLINE";
         break;

      case SD_STATUSBAR :
         os << "STATUSBAR";
         break;

      case SD_ENDRUN_BTN :
         os << "END RUN BUTTON";
         break;

      case SD_CONFIG_BTN :
         os << "CONFIG BUTTON";
         break;

      case SD_CONTINUE_BTN :
         os << "CONTINUE BUTTON";
         break;

      case SD_LOADSYS_BTN :
         os << "LOAD SYSTEM BUTTON";
         break;

      case SD_DONORINFO_BTN :
         os << "DONOR INFO BUTTON";
         break;

      case SD_GENERAL_FUNC :
         os << "GENERAL FUNCTION";
         break;

      default :
         os << "UNKNOWN OBJ";
         break;
   }

   os << "(" << (int)obj << ")";

   return os;
}



DataLog_Stream& operator << (DataLog_Stream& os, const SYSDONOR_OP& op)
{
   switch (op)
   {
      case SDOP_STATUSLINE_RESET :
         os << "SDOP_STATUSLINE_RESET";
         break;

      case SDOP_STATUSLINE_DEACTIVATE_ATTENTION :
         os << "SDOP_STATUSLINE_DEACTIVATE_ATTENTION";
         break;

      case SDOP_STATUSLINE_SET_COLOR :
         os << "SDOP_STATUSLINE_SET_COLOR";
         break;

      case SDOP_STATUSLINE_SET_TEXT :
         os << "SDOP_STATUSLINE_SET_TEXT";
         break;

      case SDOP_STATUSLINE_LOAD_DONOR_NAME :
         os << "SDOP_STATUSLINE_LOAD_DONOR_NAME";
         break;

      case SDOP_STATUSBAR_RESET_ALL_PHASES :
         os << "SDOP_STATUSBAR_RESET_ALL_PHASES";
         break;

      case SDOP_STATUSBAR_SET_PHASE_COMPLETE :
         os << "SDOP_STATUSBAR_SET_PHASE_COMPLETE";
         break;

      case SDOP_STATUSBAR_SET_PHASE_ACTIVE :
         os << "SDOP_STATUSBAR_SET_PHASE_ACTIVE";
         break;

      case SDOP_BUTTON_SET_VISIBLE :
         os << "SDOP_BUTTON_SET_VISIBLE";
         break;

      case SDOP_BUTTON_SET_INVISIBLE :
         os << "SDOP_BUTTON_SET_INVISIBLE";
         break;

      case SDOP_BUTTON_SET_ACTIVE :
         os << "SDOP_BUTTON_SET_ACTIVE";
         break;

      case SDOP_BUTTON_SET_INACTIVE :
         os << "SDOP_BUTTON_SET_INACTIVE";
         break;

      case SDOP_BUTTON_SET_UP_BITMAP :
         os << "SDOP_BUTTON_SET_UP_BITMAP";
         break;

      case SDOP_BUTTON_SET_DN_BITMAP :
         os << "SDOP_BUTTON_SET_DN_BITMAP";
         break;

      case SDOP_BUTTON_SET_TEXT :
         os << "SDOP_BUTTON_SET_TEXT";
         break;

      case SDOP_BUTTON_SET_TEXT_COLOR :
         os << "SDOP_BUTTON_SET_TEXT_COLOR";
         break;

      case SDOP_BUTTON_SYSLOAD_ALL_COL_UNCHK_ACT :
         os << "SDOP_BUTTON_SYSLOAD_ALL_COL_UNCHK_ACT";
         break;

      case SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT :
         os << "SDOP_BUTTON_SYSLOAD_ALL_GRY_CHKED_INACT";
         break;

      case SDOP_BUTTON_SYSLOAD_INVISIBLE :
         os << "SDOP_BUTTON_SYSLOAD_INVISIBLE";
         break;

      case SDOP_BUTTON_DONINFO_MAN_COL_UNCHK_ACT :
         os << "SDOP_BUTTON_DONINFO_MAN_COL_UNCHK_ACT";
         break;

      case SDOP_BUTTON_DONINFO_MAN_COL_CHKED_ACT :
         os << "SDOP_BUTTON_DONINFO_MAN_COL_CHKED_ACT";
         break;

      case SDOP_BUTTON_DONINFO_AP2_GRY_UNCHK_INACT :
         os << "SDOP_BUTTON_DONINFO_AP2_GRY_UNCHK_INACT";
         break;

      case SDOP_BUTTON_DONINFO_AP2_COL_UNCHK_ACT :
         os << "SDOP_BUTTON_DONINFO_AP2_COL_UNCHK_ACT";
         break;

      case SDOP_BUTTON_DONINFO_AP2_COL_CHKED_ACT :
         os << "SDOP_BUTTON_DONINFO_AP2_COL_CHKED_ACT";
         break;

      case SDOP_BUTTON_ENDRUN_ACTIVATE :
         os << "SDOP_BUTTON_ENDRUN_ACTIVATE";
         break;

      case SDOP_BUTTON_CONFIG_ACTIVATE :
         os << "SDOP_BUTTON_CONFIG_ACTIVATE";
         break;

      case SDOP_BUTTON_BEGINRUN_ACTIVATE :
         os << "SDOP_BUTTON_BEGINRUN_ACTIVATE";
         break;

      case SDOP_GENERAL_GOTO_SCREEN :
         os << "SDOP_GENERAL_GOTO_SCREEN";
         break;

      case SDOP_GENERAL_FREEZE_SCREEN :
         os << "SDOP_GENERAL_FREEZE_SCREEN";
         break;

      default :
         os << "UNKNOWN OP";
         break;
   }

   os << "(" << (int)op << ")";

   return os;
}

/* FORMAT HASH fcaceedfa974b229562157ef661180c9 */

/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Status_Line

*     File name:   statline.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************
$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/statline.cpp 1.42 2009/01/06 19:18:21Z dslausb Exp dslausb $
*/


#include <vxworks.h>
#include "guiglobs.hpp"
#include "statline.hpp"
#include "trima_string_info.h"
#include "stringError.h"
#include "string_data.h"

extern StringTable stringTable[];

const char*        status_line_type_names[] = {"Default", "Attention", "End Run", "Alert", "Alarm", "Null"};


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Status_Line::Status_Line (WINDOW_COLOR statusline_color)
   : OSWindow(*GuiMain::getDisplay(), OSRegion(STATLINE_X, STATLINE_Y, GuiMain::getDisplay()->width(), 24)),
     current_statusline_bitmap(0, 0, BITMAP_STATUSLINE_DEFAULT),
     current_statusline_color(statusline_color),
     current_status_line_type(STATUS_LINE_NULL),
     current_statusline_text(textEmpty),
     current_custom_text(),
     current_parent_screen(GUI_SCREEN_NULL_SCREEN),
     dingbat(1, 1, BITMAP_EXCLAMATION),
     _timerFlashIcon(1000, Callback<Status_Line>(this, &Status_Line::dingbat_timeout), TimerMessage::DISARMED),
     type_status_line_text_last(textEmpty),
     _callbackTxtChange()
{
   // set the default values for the different line types
   status_line_defaults[STATUS_LINE_DEFAULT].type_status_line_text         =  textEmpty;
   status_line_defaults[STATUS_LINE_DEFAULT].type_status_line_color        =  BITMAP_STATUSLINE_DEFAULT;
   status_line_defaults[STATUS_LINE_DEFAULT].type_status_line_font_color   =  WHITE;

   status_line_defaults[STATUS_LINE_ATTENTION].type_status_line_text       =  textEmpty;
   status_line_defaults[STATUS_LINE_ATTENTION].type_status_line_color      =  BITMAP_STATUSLINE_ATTENTION;
   status_line_defaults[STATUS_LINE_ATTENTION].type_status_line_font_color =  NAVY_BLUE;

   status_line_defaults[STATUS_LINE_END_RUN].type_status_line_text         =  textEmpty;
   status_line_defaults[STATUS_LINE_END_RUN].type_status_line_color        =  BITMAP_STATUSLINE_DEFAULT;
   status_line_defaults[STATUS_LINE_END_RUN].type_status_line_font_color   =  WHITE;

   status_line_defaults[STATUS_LINE_ALERT].type_status_line_text           =  textEmpty;
   status_line_defaults[STATUS_LINE_ALERT].type_status_line_color          =  BITMAP_STATUSLINE_ALARM;
   status_line_defaults[STATUS_LINE_ALERT].type_status_line_font_color     =  YELLOW;

   status_line_defaults[STATUS_LINE_ALARM].type_status_line_text           =  textEmpty;
   status_line_defaults[STATUS_LINE_ALARM].type_status_line_color          =  BITMAP_STATUSLINE_ALARM;
   status_line_defaults[STATUS_LINE_ALARM].type_status_line_font_color     =  YELLOW;

   for (int i = 0; i < STATUS_LINE_NULL; i++)
   {
      status_line_type[i].reinit(status_line_defaults[i]);
   }

}  // END of Status_Line CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Status_Line::~Status_Line ()
{

   /* This object is global. If destroyed, GUI is being destroyed. There is no
      recovery contingent. This change is prompted by seg. viols. encountered in
      Photon 1.12 when photon calls are made during destruction.
   */

}  // END of Status_Line DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE RESOURCES

void Status_Line::allocate_resources ()
{

   // Allocate the constituent bitmap and text objects and parent them to the container
   // Make the container color transparent and set the resize policy to fit to the graphics

   // Allocate bitmap
   current_statusline_bitmap.allocate_resources (*this) ;

   // Set the color to the constructor spec'ed color
   set_color (current_statusline_color) ;

   // Now make the text object the same size as the bitmap background
   forceTextDimensions();

   // Allocate the display text object
   current_statusline_text.allocate_resources (*this) ;

   dingbat.allocate_resources(*this);
   dingbat.set_visible(false);

   //
   // Set up the dingbat timer ...
   //
   _timerFlashIcon.armTimer(TimerMessage::ARMED);

} // END of Status_Line ALLOCATE RESOURCES


// Force the text dimensions to be centered in the bitmap
void Status_Line::forceTextDimensions ()
{
   OSRegion     bmpRegion  = current_statusline_bitmap.get_dimensions();

   unsigned int attributes =
      OSText::LEFT_TO_RIGHT |
      OSText::HJUSTIFY_CENTER |
      OSText::VJUSTIFY_CENTER;

   current_statusline_text.set_requested_region(bmpRegion);
   current_statusline_text.setAttributes(attributes);
}

////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE RESOURCES

void Status_Line::deallocate_resources ()
{
   _timerFlashIcon.armTimer(TimerMessage::DISARMED);

   // Set parenthood off of the current window
   unparent_status_line (get_owning_screen ()) ;

   // Deallocate all the constituent objects.
   current_statusline_bitmap.deallocate_resources () ;

   // Allocate the five text objects
   current_statusline_text.deallocate_resources () ;

   dingbat.deallocate_resources();

}  // END of Status_Line DEALLOCATE RESOURCES


////////////////////////////////////////////////////////////////////////////////
// SET_OWNING_SCREEN

void Status_Line::set_owning_screen (GUI_SCREEN_ID owning_screen)
{
   current_parent_screen = owning_screen ;

}  // END of Status_Line SET_OWNING_SCREEN



////////////////////////////////////////////////////////////////////////////////
// SET_CURRENT_STATUS_LINE_LOOKUP
Status_Line::STATUS_LINE_LOOKUP Status_Line::set_current_status_line_lookup (STATUS_LINE_TYPE user_status_line_request)
{

   // Convert the status line request
   switch (user_status_line_request)
   {
      case DEFAULT_STATUS_LINE :
         return(STATUS_LINE_DEFAULT) ;

      case ATTENTION_STATUS_LINE :
         return(STATUS_LINE_ATTENTION) ;

      case END_RUN_STATUS_LINE :
         return(STATUS_LINE_END_RUN) ;

      case ALERT_STATUS_LINE :
         return(STATUS_LINE_ALERT) ;

      case ALARM_STATUS_LINE :
         return(STATUS_LINE_ALARM) ;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Invalid status line specified");

   }  // End of Switch

   // Compiler warning
   return(STATUS_LINE_DEFAULT) ;


}  // END of Status_Line SET_CURRENT_STATUS_LINE_LOOKUP



////////////////////////////////////////////////////////////////////////////////
// GET_OWNING_SCREEN

GUI_SCREEN_ID Status_Line::get_owning_screen ()
{
   return (current_parent_screen);

}  // END of Status_Line GET_OWNING_SCREEN


////////////////////////////////////////////////////////////////////////////////
// REPARENT_STATUS_LINE

void Status_Line::reparent_status_line (Base_Window& parent)
{
   DataLog(log_level_gui_statline_info) << "status line reparented from: " << current_parent_screen
                                        << " to: " << parent.get_screen_id() << endmsg;

   detach();
   attach(&parent);

   // Establish the ownership of the status line
   set_owning_screen (parent.get_screen_id()) ;

}  // END of Status_Line REPARENT_STATUS_LINE




////////////////////////////////////////////////////////////////////////////////
// UNPARENT_STATUS_LINE

void Status_Line::unparent_status_line (GUI_SCREEN_ID unlinking_request_screen)
{

   // Only let the owning screen of the status line "unparent" the status line
   if (unlinking_request_screen == get_owning_screen ())
   {
      DataLog(log_level_gui_statline_info) << "status line unparented by: " << unlinking_request_screen << endmsg;

      detach();

      // Establish the ownership of the status line
      set_owning_screen (GUI_SCREEN_NULL_SCREEN);
   }

}  // END of Status_Line UNPARENT_STATUS_LINE



////////////////////////////////////////////////////////////////////////////////
// RESET_STATUS_LINE

void Status_Line::reset_status_line_real (const char* sourceFileName, int sourceFileLine)
{
   // turn this on to get excessive logging
//   DataLog(log_level_gui_statline_info) << "status line reset"
//                                        << " by " << sourceFileName << ":" << sourceFileLine << endmsg;

   int table_index;                // index to traverse status line state table

   // Reset all text elements to NULL effectively reinitializing all status line
   // types to their initial state.
   // Use a bottom up traversal of the table to locate the most severe currently
   // defined instance of the status line.
   for (table_index = 0; table_index < STATUS_LINE_NULL; table_index++)
   {
      status_line_type[table_index].reinit(status_line_defaults[table_index]);
   }

   // Reset the status line to the highest active state as a standard check
   activate_highest_priority_status_line() ;

}   // END of Status_Line RESET_STATUS_LINE



////////////////////////////////////////////////////////////////////////////////
// DEACTIVATE_STATUS_LINE_TYPE

void Status_Line::deactivate_status_line_type_real (STATUS_LINE_TYPE user_status_line_deactivate, const char* sourceFileName, int sourceFileLine)
{
   // turn this on to get excessive logging
//   DataLog(log_level_gui_statline_info) << "status line deactivate line by type, type " << user_status_line_deactivate
//                                        << " by " << sourceFileName << ":" << sourceFileLine << endmsg;

   // Local status line type for status line text adjustments
   STATUS_LINE_LOOKUP user_status_line_deactivate_lookup ;

   // Convert request to local status line type
   user_status_line_deactivate_lookup = set_current_status_line_lookup (user_status_line_deactivate) ;

   // remove the top line of this type
   status_line_type[user_status_line_deactivate_lookup].pop();

   // Reset the status line to the highest active state as a standard check
   activate_highest_priority_status_line() ;

}   // END of Status_Line DEACTIVATE_STATUS_LINE_TYPE



////////////////////////////////////////////////////////////////////////////////
// DEACTIVATE_STATUS_LINE_TEXT_TYPE

void Status_Line::deactivate_status_line_text_type_real (TEXT_STRING_ID user_status_line_text_request, STATUS_LINE_TYPE user_status_line_deactivate, const char* sourceFileName, int sourceFileLine)
{
   // turn this on to get excessive logging
//   DataLog(log_level_gui_statline_info) << "status line deactivate line by text/type, type " << user_status_line_deactivate
//                                        << " by " << sourceFileName << ":" << sourceFileLine << endmsg;

   // Local status line type for status line text adjustments
   STATUS_LINE_LOOKUP user_status_line_deactivate_lookup ;

   // Convert request to local status line type
   user_status_line_deactivate_lookup = set_current_status_line_lookup (user_status_line_deactivate) ;

   // remove the top line of this type
   status_line_type[user_status_line_deactivate_lookup].erase(user_status_line_text_request);

   // Reset the status line to the highest active state as a standard check
   activate_highest_priority_status_line() ;

}   // END of Status_Line DEACTIVATE_STATUS_LINE_TYPE



////////////////////////////////////////////////////////////////////////////////
// SET_STATUS_LINE_TYPE_INTERNAL

void Status_Line::set_status_line_type_internal (STATUS_LINE_LOOKUP internal_status_line_request)
{
   // Reset the type to the user requested type if not the same as the last
   // implementation.
   if (type_status_line_text_last != (status_line_type[internal_status_line_request].top().type_status_line_text))
   {
      // load current status line text
      if (status_line_type[internal_status_line_request].top().type_status_line_text == textEmptyFormatDonDlDonorNameInit)
      {
         //
         //
         //   !!!!!!!!  DO NOT LOG DONOR NAME OR DOB !!!!!!  This information must not be available
         //   in Trima-saved files in order to protect donor confidentiality!
         //
         //
         current_statusline_text.setText(guiglobs::gui_donor_data_repository.name() );
         DataLog(log_level_gui_statline_info) << "status line displaying donor name" << endmsg;
      }
      else
      {
         // current_statusline_text.set_auto_fit_text(false); // This doesn't do anything, as far as I can see.

         // this is a bit of a hack to handle custom status line text
         // if the type is TEXT_NULL the use the text in current_custom_text
         if (status_line_type[internal_status_line_request].top().type_status_line_text == TEXT_NULL)
         {
            current_statusline_text.setText(current_custom_text);
         }
         else
         {
            current_statusline_text.set_string_id (status_line_type[internal_status_line_request].top().type_status_line_text) ;
         }

         forceTextDimensions();

         DataLog(log_level_gui_statline_info) << "new status line displayed: " << status_line_type_names[internal_status_line_request]
                                              << " : " << convertStringIdToString(status_line_type[internal_status_line_request].top().type_status_line_text) << endmsg;

         dump_stacks();

      }

      // establish text only dimensions
      current_statusline_text.set_visible(false);

      // update colors
      current_statusline_bitmap.set_id(status_line_type[internal_status_line_request].top().type_status_line_color) ;
      current_statusline_text.set_color (status_line_type[internal_status_line_request].top().type_status_line_font_color) ;
      current_statusline_text.set_visible(true);
      _callbackTxtChange();
   }

   // Reset the last type to the user requested type
   type_status_line_text_last = (status_line_type[internal_status_line_request].top().type_status_line_text) ;

}  // END of Status_Line SET_STATUS_LINE_TYPE_INTERNAL


////////////////////////////////////////////////////////////////////////////////
// SET_CURRENT_STATUS_LINE_TEXT_BY_TYPE

void Status_Line::set_current_status_line_text_by_type_real (STATUS_LINE_TYPE user_status_line_request,
                                                             TEXT_STRING_ID user_status_line_text_request,
                                                             const char* sourceFileName, int sourceFileLine)
{
//   DataLog(log_level_gui_statline_info) << "status line set line by type, type " << user_status_line_request
//      << " line " << user_status_line_text_request
//		<< " by " << sourceFileName << ":" << sourceFileLine << endmsg;

   // Local status line type for status line text adjustments
   STATUS_LINE_LOOKUP text_status_line_type ;

   // Reset the type to the user requested type.
   text_status_line_type = set_current_status_line_lookup (user_status_line_request) ;

   // don't push blank lines
   if (user_status_line_text_request != textEmpty)
   {
      // don't keep pushing the same line
      if (user_status_line_text_request != status_line_type[text_status_line_type].top().type_status_line_text)
      {
         // push the new line
         status_line_type[text_status_line_type].push(user_status_line_text_request);

         // WARNING
         // This may be excessive logging
         DataLog(log_level_gui_statline_info) << "status line pushed into stack, type "
                                              << status_line_type_names[text_status_line_type]
                                              << " text " << convertStringIdToString(user_status_line_text_request)
                                              << " by " << sourceFileName << ":" << sourceFileLine
                                              << endmsg;
      }
      else
      {
         DataLog(log_level_gui_statline_info) << "status line duplicate push attempt, type " << status_line_type_names[text_status_line_type]
                                              << " text " << convertStringIdToString(user_status_line_text_request)
                                              << " by " << sourceFileName << ":" << sourceFileLine
                                              << endmsg;
      }
   }
   else
   {
      DataLog(log_level_gui_statline_info) << "status line attempt to push blank line of type " << status_line_type_names[text_status_line_type]
                                           << " by " << sourceFileName << ":" << sourceFileLine
                                           << endmsg;
   }

   // Reset the status line to the highest active state as a standard check
   activate_highest_priority_status_line() ;

}   // END of Status_Line SET_CURRENT_STATUS_LINE_TEXT_BY_TYPE



////////////////////////////////////////////////////////////////////////////////
// ACTIVATE_HIGHEST_PRIORITY_STATUS_LINE

void Status_Line::activate_highest_priority_status_line ()
{
   int table_index;                // index to traverse status line state table

   // initialize current_status_line_type to lowest level
   current_status_line_type = STATUS_LINE_DEFAULT;

   // Use a bottom up traversal of the table to locate the most severe currently
   // defined instance of the status line.
   for (table_index = 0; table_index < STATUS_LINE_NULL; table_index++)
   {
      if (status_line_type[table_index].top().type_status_line_text != textEmpty)
      {
         // This will set the current_status_line_type to the status lines
         // highest active state
         current_status_line_type = (STATUS_LINE_LOOKUP)table_index ;
      }
   }

   // Explicitly reset the status line to the status line state)
   set_status_line_type_internal (current_status_line_type) ;
}   // END of Status_Line ACTIVATE_HIGHEST_PRIORITY_STATUS_LINE


////////////////////////////////////////////////////////////////////////////////
// SET_COLOR

void Status_Line::set_color (WINDOW_COLOR statusline_color)
{
   BITMAP_ID   newColor    = BITMAP_STATUSLINE_DEFAULT;

   const char* color_ref[] = {"Teal", "Blue", "Magenta", "Grey" };

   DataLog(log_level_gui_statline_info) << "status line color changed from " << color_ref[current_statusline_color]
                                        << " to " << color_ref[statusline_color] << endmsg;

   // Set the status line to the proper bitmap color
   switch (statusline_color)
   {
      case TEAL :
         newColor = BITMAP_STATUSLINE_TEAL;
//         status_line_type[STATUS_LINE_DEFAULT].top().type_status_line_color = BITMAP_STATUSLINE_TEAL ;
//		 status_line_type[STATUS_LINE_END_RUN].top().type_status_line_color = BITMAP_STATUSLINE_TEAL ;
         break;

      case BLUE :
         newColor = BITMAP_STATUSLINE_BLUE;
//         status_line_type[STATUS_LINE_DEFAULT].top().type_status_line_color = BITMAP_STATUSLINE_BLUE ;
//		 status_line_type[STATUS_LINE_END_RUN].top().type_status_line_color = BITMAP_STATUSLINE_BLUE ;
         break;

      case MAGENTA :
         newColor = BITMAP_STATUSLINE_MAGENTA;
//         status_line_type[STATUS_LINE_DEFAULT].top().type_status_line_color = BITMAP_STATUSLINE_MAGENTA ;
//		 status_line_type[STATUS_LINE_END_RUN].top().type_status_line_color = BITMAP_STATUSLINE_MAGENTA ;
         break;

      default :
         newColor = BITMAP_STATUSLINE_DEFAULT;
//         status_line_type[STATUS_LINE_DEFAULT].top().type_status_line_color = BITMAP_STATUSLINE_DEFAULT ;
//		 status_line_type[STATUS_LINE_END_RUN].top().type_status_line_color = BITMAP_STATUSLINE_DEFAULT ;
         break;

   } // End of switch (statusline_color)...

   // Now set all of the lines in the default/EOR status line stacks to this color
   for (int i = 0; i < MAX_STATUS_LINE_ARRAY; i++)
   {
      status_line_type[STATUS_LINE_DEFAULT].myStack[i].type_status_line_color = newColor;
      status_line_type[STATUS_LINE_END_RUN].myStack[i].type_status_line_color = newColor;
   }

   if (current_statusline_color != statusline_color)
   {
      // If there is a true default color change, and the default bitmap is
      // allocated set the new bitmap object to that bitmap
      if (current_status_line_type == STATUS_LINE_DEFAULT || current_status_line_type == STATUS_LINE_END_RUN)
      {
         current_statusline_bitmap.set_id(status_line_type[current_status_line_type].top().type_status_line_color) ;
      }

      // Remember the logical default color
      current_statusline_color = statusline_color ;
   }


}  // END of Status_Line SET_COLOR



////////////////////////////////////////////////////////////////////////////////
// GET_COLOR

WINDOW_COLOR Status_Line::get_color ()
{

   return (current_statusline_color);

}   // END of Status_Line GET_COLOR



////////////////////////////////////////////////////////////////////////////////
// SET_CURRENT_TEXT

void Status_Line::set_current_text_real (TEXT_STRING_ID statusline_text, const char* sourceFileName, int sourceFileLine)
{
//	DataLog(log_level_gui_statline_info) << "status line set current text " << statusline_text
//		<< " by " << sourceFileName << ":" << sourceFileLine << endmsg;

   // Load in the status text info.
//   status_line_type[STATUS_LINE_DEFAULT].top().type_status_line_text =  statusline_text;

   set_current_status_line_text_by_type_real(DEFAULT_STATUS_LINE, statusline_text, sourceFileName, sourceFileLine) ;

   // Reset the status line to the highest active state as a standard check
//   activate_highest_priority_status_line() ;

}   // END of Status_Line SET_CURRENT_TEXT



////////////////////////////////////////////////////////////////////////////////
// SET_CURRENT_TEXT_DIRECT

void Status_Line::set_current_text_direct_real (const char* statusline_text, const char* sourceFileName, int sourceFileLine)
{
   DataLog(log_level_gui_statline_info) << "status line set current text direct "  << statusline_text
                                        << " by " << sourceFileName << ":" << sourceFileLine << endmsg;

   // set the text ID to null so we know it's custom text
   status_line_type[STATUS_LINE_DEFAULT].top().type_status_line_text = TEXT_NULL;

   // set the custom text
   current_custom_text = statusline_text;

   // Reset the status line to the highest active state as a standard check
   activate_highest_priority_status_line();

}   // END of Status_Line SET_CURRENT_TEXT




////////////////////////////////////////////////////////////////////////////////
// GET_CURRENT_TEXT

TEXT_STRING_ID Status_Line::get_current_text ()
{
   // Return the text ID of the currently displayed text in the status line
   return (status_line_type[STATUS_LINE_DEFAULT].top().type_status_line_text) ;

}   // END of Status_Line GET_CURRENT_TEXT




////////////////////////////////////////////////////////////////////////////////
// GET_CURRENT_STATUS_LINE_TEXT_BY_TYPE

TEXT_STRING_ID Status_Line::get_current_status_line_text_by_type (STATUS_LINE_TYPE user_status_line_request)
{

   // Local status line type for status line text adjustments
   STATUS_LINE_LOOKUP user_status_line_text_lookup ;

   // Convert request to local status line type
   user_status_line_text_lookup = set_current_status_line_lookup (user_status_line_request) ;

   // Return the text ID of the currently displayed text in the status line
   return (status_line_type[user_status_line_text_lookup].top().type_status_line_text) ;

}   // END of Status_Line GET_CURRENT_STATUS_LINE_TEXT_BY_TYPE


STATUS_LINE_TYPE Status_Line::get_current_status_line_type ()
{

   STATUS_LINE_TYPE retval = DEFAULT_STATUS_LINE;

   // Convert to status line type
   switch (current_status_line_type)
   {
      case STATUS_LINE_ALARM :
         retval = ALARM_STATUS_LINE;
         break;
      case STATUS_LINE_ALERT :
         retval = ALERT_STATUS_LINE;
         break;
      case STATUS_LINE_ATTENTION :
         retval = ATTENTION_STATUS_LINE;
         break;
      case STATUS_LINE_END_RUN :
         retval = END_RUN_STATUS_LINE;
         break;
      default :
         retval = DEFAULT_STATUS_LINE;
         break;
   }

   return(retval);
}


void Status_Line::dingbat_timeout ()
{
   static int dingbat_active = 0;
   int        flashing       = ((current_status_line_type == STATUS_LINE_ALERT) ||
                                (current_status_line_type == STATUS_LINE_ALARM));
   if (flashing && (dingbat_active==0))
   {
      dingbat_active = 1;
      dingbat.set_visible(true);
   }
   else if (dingbat_active)
   {
      dingbat_active = 0;
      dingbat.set_visible(false);
   }
}

void Status_Line::setTextChangeCallback (const CallbackBase& callback)
{
   _callbackTxtChange = callback;
}

void Status_Line::clearTextChangeCallback ()
{
   CallbackBase null_callback;
   _callbackTxtChange = null_callback;
}


void Status_Line::dump_stacks ()
{
   DataLog(log_level_gui_statline_info) << "status line stack" ;

   for (int i = 0; i < STATUS_LINE_NULL; i++)
   {
//      DataLog(log_level_gui_statline_info) << "status line stack " << status_line_type_names[i] << " (" << i << ") :";
//      DataLog(log_level_gui_statline_info) << "status line stack " << status_line_type_names[i] ;
      DataLog(log_level_gui_statline_info) << " #" << status_line_type_names[i] << "# ";
      status_line_type[i].dump();
   }
   DataLog(log_level_gui_statline_info) << endmsg;
}

/* FORMAT HASH 2a05a4b8db8062a7025275d164265e92 */

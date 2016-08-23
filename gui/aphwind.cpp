/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Base_Apheresis_Window

*     File name:   aphwind.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >***************************/

#include <vxworks.h>

// BASE CLASS INCLUDE

#include "aphwind.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "software_cds.h"

FeatureChanged* Base_Apheresis_Window::featureChangedMsg = NULL;
bool            Base_Apheresis_Window::screenshotEnabled = false;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Base_Apheresis_Window::Base_Apheresis_Window(WINDOW_COLOR wcolor       = TEAL,
                                             GUI_SCREEN_ID win_ID       = GUI_SCREEN_NULL_SCREEN,
                                             DisplayOrderType displayOrder = DisplayOrder_Normal,
                                             BITMAP_ID bgBmpId      = BITMAP_DEFAULT_BACKGROUND)
   : Base_Window(bgBmpId, displayOrder),

#if (CPU==SIMNT)
     _screenTraverseMessage(NULL),
#endif

     status_bar_is_linked(false),
     status_line_is_linked(false),
     init_status_line(false),

     btn_cancel(wcolor),
     btn_continue(wcolor),
     btn_goback(wcolor),
     btn_lowleft(wcolor),
     snapshotButton(GUI_BUTTON_SCREENSHOT, 10, 434, textEmpty, BITMAP_BTN_SCREENSHOT_UP, BITMAP_BTN_SCREENSHOT_DOWN)
{
   Base_Apheresis_Window::set_screen_id (win_ID);

   // do the first-pass initialization of snapshot feature
   if ( Base_Apheresis_Window::featureChangedMsg == NULL )
   {
      featureChangedMsg = new FeatureChanged(Callback<Base_Apheresis_Window>(&Base_Apheresis_Window::featureChangedMsgHandler));
      screenshotEnabled = Software_CDS::GetInstance().getFeature(EnableScreenshotButton);
   }

}   // END of Base_Apheresis_Window CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Base_Apheresis_Window::~Base_Apheresis_Window()
{
   // If we haven't already deallocated, do it now.
   Base_Apheresis_Window::deallocate_resources ();
}   // END of Base_Apheresis_Window DESTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES (allocate_paramter)
void Base_Apheresis_Window::allocate_resources (const char* allocate_parameter)
{
   if (_allocated)
   {
      _FATAL_ERROR(__FILE__, __LINE__, "Base_Window: invalid reallocation");
   }

   // Register for the special status line setting / clearing message
   _setStatusLineMsg.init(Callback<Base_Apheresis_Window>(this, &Base_Apheresis_Window::setStatusLineMsgHandler), MessageBase::SNDRCV_RECEIVE_ONLY);
   _clearStatusLineMsg.init(Callback<Base_Apheresis_Window>(this, &Base_Apheresis_Window::clearStatusLineMsgHandler), MessageBase::SNDRCV_RECEIVE_ONLY);

   // allocate parent's resources first
   Base_Window::allocate_resources (allocate_parameter);

   // set the status line to be initialized
   init_status_line = true;

   // allocate all the standard button resources and parent them onto the
   //  apheresis window.
   btn_cancel.allocate_resources    (*this);
   btn_continue.allocate_resources  (*this);
   btn_goback.allocate_resources    (*this);
   btn_lowleft.allocate_resources   (*this);

   if (screenshotEnabled)
   {
      snapshotButton.allocate_resources(*this);
      snapshotButton.set_callback(&(Base_Apheresis_Window::screenshotCB), NULL);
   }

   // link the buttons to their C callback stubs
   btn_cancel.set_callback    (aph_callback_cancel,   (void*)this);
   btn_continue.set_callback  (aph_callback_continue, (void*)this);
   btn_goback.set_callback    (aph_callback_goback,   (void*)this);
   btn_lowleft.set_callback   (aph_callback_lowleft,  (void*)this);

#if (CPU==SIMNT)
   _screenTraverseMessage = new SimntTraverseMsg(Callback<Base_Apheresis_Window>(this, &Base_Apheresis_Window::handleTraverseMessage), MessageBase::SNDRCV_RECEIVE_ONLY);
#endif
}  // END of Base_Apheresis_Window ALLOCATE_RESOURCES ()


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES (BITMAP_ID, allocate_parameter)
void Base_Apheresis_Window::allocate_resources (BITMAP_ID background_bitmap, const char* allocate_parameter)
{
   // set parent's background bitmap and then perform normal allocation.
   Base_Window::set_background_bitmap (background_bitmap);
   allocate_resources (allocate_parameter);

} // END of Base_Apheresis_Window ALLOCATE_RESOURCES (BITMAP_ID)



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Base_Apheresis_Window::deallocate_resources ()
{
   if (!_allocated)
      return;

   // Register for the special status line setting / clearing message
   _setStatusLineMsg.deregisterMsg();
   _clearStatusLineMsg.deregisterMsg();

#if CPU==SIMNT
   if (_screenTraverseMessage)
   {
      delete _screenTraverseMessage;
      _screenTraverseMessage = NULL;
   }
#endif /* if CPU==SIMNT */

   // deallocate all the has-a standard button resources first
   snapshotButton.deallocate_resources();

   btn_cancel.deallocate_resources   ();
   btn_continue.deallocate_resources ();
   btn_goback.deallocate_resources   ();
   btn_lowleft.deallocate_resources  ();


   // deallocate parent's resources
   Base_Window::deallocate_resources ();

}   // END of Base_Apheresis_Window DEALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// REFRESH_RESOURCES
void Base_Apheresis_Window::refresh_resources ()
{
   // At a minimum, this method will relink the status bar and
   //  status line to the current window (if they were linked in the
   //  past). Other activity may be added by individual screens by overriding the refresh_resources
   //  method.

   // Relink the status bar it was owned by this
   // screen in the past and is not currently linked to this screen.
   if (status_bar_is_linked)
   {
      // Link the status bar to this screen
      guiglobs::apheresis_status_bar->reparent_status_bar (*this);
   }

   // reparent the status line if the screen wants it
   if (status_line_is_linked)
   {
      guiglobs::apheresis_status_line->reparent_status_line (*this);

      // init the status line if we need to
      if (init_status_line)
      {
         init_status_line = false;

         initialize_status_line();
      }
   }

   // check for a change to the snapshot button
   if ( screenshotEnabled && !snapshotButton.allocated() )
   {
      // turn it on
      DataLog(log_level_gui_info) << "Enable screenshot button for Screen " << get_screen_id() << endmsg;
      snapshotButton.allocate_resources(*this);
      snapshotButton.set_callback(&(Base_Apheresis_Window::screenshotCB), NULL);
   }
   else if ( snapshotButton.allocated() && !screenshotEnabled )
   {
      // turn it off
      DataLog(log_level_gui_info) << "Disable screenshot button for Screen " << get_screen_id() << endmsg;
      snapshotButton.deallocate_resources();
      snapshotButton.remove_callbacks();
   }

   // Other operations may occur here in the future to re-constitute
   //  the screen after having been covered or super-imposed
   //  by a child screen.


}   // END of Base_Apheresis_Window REFRESH_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// LINK_STATUS_BAR_LINE
void Base_Apheresis_Window::link_status_bar_line ()
{
   // Parent the status bar onto this screen
   guiglobs::apheresis_status_bar->reparent_status_bar (*this);

   status_bar_is_linked = true;

   // set the flag for the status line so it gets reparented at refresh
   status_line_is_linked = true;

}  // END of Base_Apheresis_Window LINK_STATUS_BAR_LINE


////////////////////////////////////////////////////////////////////////////////
// LINK_STATUS_LINE_ONLY
void Base_Apheresis_Window::link_status_line_only ()
{
   // set the flag for the status line so it gets reparented at refresh
   status_line_is_linked = true;

   guiglobs::apheresis_status_line->reparent_status_line (*this);
}  // END of Base_Apheresis_Window LINK_STATUS_BAR_LINE


////////////////////////////////////////////////////////////////////////////////
// UNLINK_STATUS_BAR_LINE
void Base_Apheresis_Window::unlink_status_bar_line ()
{
   // Parent the status bar onto the UR window
   guiglobs::apheresis_status_bar->unparent_status_bar (get_screen_id());

   status_bar_is_linked = false;

   // Parent the status line onto the UR window
   guiglobs::apheresis_status_line->unparent_status_line (get_screen_id());

   status_line_is_linked = false;

   init_status_line      = true;
}



////////////////////////////////////////////////////////////////////////////////
// UNLINK_STATUS_LINE_ONLY
void Base_Apheresis_Window::unlink_status_line_only ()
{
   // Parent the status line onto the UR window
   guiglobs::apheresis_status_line->unparent_status_line (get_screen_id());

   status_line_is_linked = false;

   init_status_line      = true;
}



////////////////////////////////////////////////////////////////////////////////
// GOTO_SCREEN
void Base_Apheresis_Window::goto_screen (GUI_SCREEN_ID new_screen,
                                         const char* screen_param)
{
   send_screen_display_message (INVOKE_SCREEN, new_screen, screen_param);
}   // END of Base_Apheresis_Window GOTO_SCREEN



////////////////////////////////////////////////////////////////////////////////
// GOTO_CHILD_SCREEN
void Base_Apheresis_Window::goto_child_screen (GUI_SCREEN_ID new_screen,
                                               const char* screen_param)
{
   send_screen_display_message (DISPLAY_AS_CHILD, new_screen, screen_param);
}   // END of Base_Apheresis_Window GOTO_CHILD_SCREEN



////////////////////////////////////////////////////////////////////////////////
// GOTO_STATELESS_SCREEN
void Base_Apheresis_Window::goto_stateless_screen (GUI_SCREEN_ID new_screen,
                                                   const char* screen_param)
{
   send_screen_display_message (DISPLAY_AS_STATELESS, new_screen, screen_param);
}   // END of Base_Apheresis_Window GOTO_STATELESS_SCREEN



////////////////////////////////////////////////////////////////////////////////
// SWAP_CHILD_SCREEN
void Base_Apheresis_Window::swap_child_screen (GUI_SCREEN_ID new_screen,
                                               const char* screen_param)
{
   send_screen_display_message (SWAP_CHILD_SCREEN, new_screen, screen_param);

}   // END of Base_Apheresis_Window SWAP_CHILD_SCREEN



////////////////////////////////////////////////////////////////////////////////
// SWAP_STATELESS_SCREEN
void Base_Apheresis_Window::swap_stateless_screen (GUI_SCREEN_ID new_screen,
                                                   const char* screen_param)
{
   send_screen_display_message (SWAP_STATELESS_SCREEN, new_screen, screen_param);
}   // END of Base_Apheresis_Window SWAP_STATELESS_SCREEN



////////////////////////////////////////////////////////////////////////////////
// SWAP_STATELESS_STACK_FOR_SCREEN
void Base_Apheresis_Window::swap_stateless_stack_for_screen (GUI_SCREEN_ID new_screen,
                                                             const char* screen_param)
{
   send_screen_display_message (SWAP_STATELESS_WITH_SCREEN, new_screen, screen_param);
}   // END of Base_Apheresis_Window SWAP_STATELESS_STACK_FOR_SCREEN



////////////////////////////////////////////////////////////////////////////////
// SWAP_CHILD_STACK_FOR_SCREEN
void Base_Apheresis_Window::swap_child_stack_for_screen (GUI_SCREEN_ID new_screen,
                                                         const char* screen_param)
{
   send_screen_display_message (SWAP_CHILD_WITH_SCREEN, new_screen, screen_param);

}   // END of Base_Apheresis_Window SWAP_CHILD_STACK_FOR_SCREEN



////////////////////////////////////////////////////////////////////////////////
// SEND_SCREEN_DISPLAY_MESSAGE
void Base_Apheresis_Window::send_screen_display_message (invocation_method invocation,
                                                         GUI_SCREEN_ID new_screen,
                                                         const char* screen_param)
{
   SCREEN_INVOKE_STRUCT          disp_screen_struct;
   Message<SCREEN_INVOKE_STRUCT> scrnmsg(MessageBase::SEND_LOCAL);

   // Set up parameters to effect the display screen
   disp_screen_struct.screen_invocation = invocation;
   disp_screen_struct.screen_id         = new_screen;
   disp_screen_struct.alloc_param.initialize();

   // Copy in the parameter string (ensure no overrun and cauterize the end)
   if (strlen(screen_param) > 0)
   {
      strncpy (disp_screen_struct.alloc_param, screen_param, MAXSIZE_BWINDOW_PARAM - 1);
   }

   // Actually send the message
   scrnmsg.send(disp_screen_struct);
}   // END of Base_Apheresis_Window SEND_SCREEN_DISPLAY_MESSAGE


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON
void Base_Apheresis_Window::process_cancel_button ()
{
   char abort_request[50];

   // Build the abort request type message into the allocation parameter.
   strcpy (abort_request, ABORT_TYPE_USER);
   strcat (abort_request, "~"); // request type delimiter

   goto_child_screen (GUI_SCREEN_ABORTCNF, abort_request);
}   // END of Base_Apheresis_Window PROCESS_CANCEL_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Base_Apheresis_Window::process_continue_button ()
{
   // Do NOTHING, if not overriden by derived class

}   // END of Base_Apheresis_Window PROCESS_CONTINUE_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_GOBACK_BUTTON
void Base_Apheresis_Window::process_goback_button ()
{
   // Do NOTHING, if not overriden by derived class

}   // END of Base_Apheresis_Window PROCESS_GOBACK_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_LOWLEFT_BUTTON
void Base_Apheresis_Window::process_lowleft_button ()
{
   // Do NOTHING, if not overriden by derived class

}   // END of Base_Apheresis_Window PROCESS_LOWLEFT_BUTTON



////////////////////////////////////////////////////////////////////////////////
// DISABLE_STANDARD_BUTTONS
void Base_Apheresis_Window::disable_standard_buttons ()
{
   btn_cancel.disable_callbacks();
   btn_continue.disable_callbacks();
   btn_goback.disable_callbacks();
   btn_lowleft.disable_callbacks();
}   // END of Base_Apheresis_Window DISABLE_STANDARD_BUTTONS



////////////////////////////////////////////////////////////////////////////////
// ENABLE_STANDARD_BUTTONS
void Base_Apheresis_Window::enable_standard_buttons ()
{
   btn_cancel.enable_callbacks();
   btn_continue.enable_callbacks();
   btn_goback.enable_callbacks();
   btn_lowleft.enable_callbacks();
}   // END of Base_Apheresis_Window ENABLE_STANDARD_BUTTONS



void Base_Apheresis_Window::screenshotCB (void* data)
{
   guistring bmpFile;
   screenshot(bmpFile);
   DataLog(log_level_gui_info) << "Saved screenshot to: " << bmpFile << endmsg;
}



void Base_Apheresis_Window::setStatusLineMsgHandler ()
{
   // const TEXT_STRING_ID id = (TEXT_STRING_ID)_setStatusLineMsg.getData();

   const TEXT_STRING_ID id = textStatAlarmPreWatchdogAlarm;

   DataLog(log_level_gui_info) << "Setting status line for DISP_STATUS_LINE alarm.  Screen ID="
                               << get_screen_id() << ".  text string id=" << id << endmsg;

   guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, id);
}



void Base_Apheresis_Window::clearStatusLineMsgHandler ()
{
   // const TEXT_STRING_ID id = (TEXT_STRING_ID)_clearStatusLineMsg.getData();

   const TEXT_STRING_ID id = textStatAlarmPreWatchdogAlarm;

   DataLog(log_level_gui_info) << "Clearing status line for DISP_STATUS_LINE alarm.  Screen ID="
                               << get_screen_id() << ".  Id=" << id << endmsg;

   guiglobs::apheresis_status_line->deactivate_status_line_text_type(id, ATTENTION_STATUS_LINE);
}



void Base_Apheresis_Window::featureChangedMsgHandler ()
{
   // Only feature of concern here is the snapshot button, which is applied to all Apheresis Windows
   if ( Software_CDS::GetInstance().getFeature(EnableScreenshotButton) != screenshotEnabled )
   {
      // toggle the current setting
      screenshotEnabled = !screenshotEnabled;
      DataLog(log_level_gui_info) << "setting screenshotEnabled=" << screenshotEnabled << endmsg;
   }

   // refresh_resources() will handle any necessary changes for active windows
}


#if CPU==SIMNT
void Base_Apheresis_Window::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (guiglobs::screen_status.get_current_displayed_screen() == get_screen_id())
   {
      DataLog(log_level_sim_gui_side_error) << "Screen " << get_screen_id()
                                            << " received invalid SimntTraverseMsg. Current Screen="
                                            << (guiglobs::screen_status.get_current_displayed_screen()) << ", TRAVERSE_TYPE="
                                            << traverseInfo.type << endmsg;
   }
}
#endif /* if CPU==SIMNT */

/* FORMAT HASH 335afdb4984bf87e10f924135a356682 */

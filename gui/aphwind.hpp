/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Base_Apheresis_Window

*     File name:    aphwind.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class is the base class for all of the apheresis windows.  It
         includes instances for all of the standard buttons, and it has methods for
         attaching and detaching the status line and status bar.  It also has
         methods for button callback handling which would normally be overridden by
         the derived screen class.  It has methods for sending screen switching
         messages and standardized callback methods for receiving messages.

*************************< FOCUSSED GUI DOMAIN >***************************/



// Single include define guardian
#ifndef APHWIND_HPP
#define APHWIND_HPP

#include <vxworks.h> // Needed for CPU compiler directives.


#include "aphcalbk.hpp"
#include "btncanc.hpp"
#include "btncont.hpp"
#include "btngobk.hpp"
#include "btnlowl.hpp"
#include "bwindow.hpp"
#include "iguimsgs.h"
#include "trimamessages.h"


#if (CPU==SIMNT) // These guys are only needed if we're compiling for simulator.
#include "guiUnitTestMessageDefs.h"
#endif

class Base_Apheresis_Window
   : public Base_Window
{

private:

   // Memory of having linked the status bar and status line
   //  so that Refresh_Resources method only re-attaches the
   //  status entities if they were attached at allocate time.
   bool status_bar_is_linked;
   bool status_line_is_linked;

   // Flag to know if the status line has been initialized so it isn't initialized
   // every time the screen is refreshed
   bool init_status_line;


   // SEND_SCREEN_DISPLAY_MESSAGE
   // This method performs the actual message creation and transmission of
   // a screen-struct-type message. It is used by all the screen display/swap
   // methods.
   static void send_screen_display_message (invocation_method invocation, GUI_SCREEN_ID new_screen, const char* screen_param);

protected:

   Button_Cancel          btn_cancel;
   Button_Continue        btn_continue;
   Button_Goback          btn_goback;
   Button_LowLeft         btn_lowleft;
   Button_Bitmap          snapshotButton;

   SetStatusLineMsg       _setStatusLineMsg;     // Message from alarm handler to display a status line
   ClearStatusLineMsg     _clearStatusLineMsg;   // Message from alarm handler to clear a status line

   static FeatureChanged* featureChangedMsg;     // Handle feature change, such as screenshot button
   static bool            screenshotEnabled;

public:

   // CONSTRUCTOR
   Base_Apheresis_Window(WINDOW_COLOR wcolor = TEAL,
                         GUI_SCREEN_ID win_ID = GUI_SCREEN_NULL_SCREEN,
                         DisplayOrderType displayOrder = DisplayOrder_Normal,
                         BITMAP_ID bgBmpId = BITMAP_DEFAULT_BACKGROUND);


   // DESTRUCTOR
   // This is made virtual to cause ALL destructors in the inheritance chain
   // to fire. IT#3360
   virtual ~Base_Apheresis_Window();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   // Version 1 - allocation parameter string
   virtual void allocate_resources (const char* allocation_parameter);

   // Version 2 - background bitmap parameter and allocation parameter string
   virtual void allocate_resources (BITMAP_ID background_bitmap, const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // REFRESH_RESOURCES
   // This method will update the linkages to any shareable resources such as
   // the status bar and status line and then rerealize those resources. This
   // method checks that this screen is currently allocated before preforming the
   // refresh.
   virtual void refresh_resources ();


   // LINK_STATUS_BAR_LINE
   // This method makes the status bar and the status line children of this window
   void link_status_bar_line ();
   void link_status_line_only ();


   // UNLINK_STATUS_BAR_LINE
   // This method removes the status bar and the status line as children of this
   // window.
   void unlink_status_bar_line ();
   void unlink_status_line_only ();


   // GET_STATUS_BAR_IS_LINKED
   // This method returns the status of the parenting of the status bar to the derived
   // screen. If True, the status bar IS/WAS linked to this screen. If it WAS linked
   // that means that another screen had covered it and taken the status bar.
   inline bool get_status_bar_is_linked ()  { return status_bar_is_linked; }
   inline bool get_status_line_is_linked () { return status_line_is_linked; }


   // INITIALIZE_STATUS_LINE
   // This method can be overridden to initialize the status line so that it is
   // only initialized once the screen is displayed but not every time the
   // screen is refreshed.  This one does nothing but it isn't = 0 so child
   // classed don't have to override it.
   virtual void initialize_status_line () {}

   // DISABLE_STANDARD_BUTTONS
   // This method will deactivate the five (cancel, continue, goback, lowleft)
   // standard buttons, if they are allocated.
   virtual void disable_standard_buttons ();


   // ENABLE_STANDARD_BUTTONS
   // This method will activate the five (cancel, continue, goback, lowleft)
   // standard buttons, if they are allocated.
   virtual void enable_standard_buttons ();


   // PROCESS_CANCEL_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the cancel button.
   virtual void process_cancel_button ();


   // PROCESS_CONTINUE_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the continue button.
   virtual void process_continue_button ();


   // PROCESS_GOBACK_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the goback button.
   virtual void process_goback_button ();


   // PROCESS_LOWLEFT_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the lowleft button.
   virtual void process_lowleft_button ();


   // Message handlers for setting/clearing the status line
   virtual void setStatusLineMsgHandler ();
   virtual void clearStatusLineMsgHandler ();

   // Message handler for updating a changed feature
   static void featureChangedMsgHandler ();

   ////////////////// ROUTER MESSAGE ORIENTED METHODS //////////////////

   // GOTO_SCREEN
   // This method will send a router message to transfer control to another
   // "static" procedure screen. Two parameters are passed: the screen ID and a parameter
   // string.
   static void goto_screen (GUI_SCREEN_ID new_screen, const char* screen_param);


   // GOTO_CHILD_SCREEN
   // This method will send a router message to display a child screen over the
   // current screen. Two parameters are passed: the screen ID and a parameter
   // string.
   static void goto_child_screen (GUI_SCREEN_ID new_child_screen, const char* screen_param);


   // GOTO_STATELESS_SCREEN
   // This method will send a router message to display a stateless screen over the
   // current screen. Two parameters are passed: the screen ID and a parameter
   // string.
   static void goto_stateless_screen (GUI_SCREEN_ID new_stateless_screen, const char* screen_param);


   // SWAP_CHILD_SCREEN
   // This method will send a router message to swap the current top-of-child-stack child screen
   // with the one passed as a parameter. If a child screen doesn't exist, the one passed as
   // a parameter will be displayed similarly to GOTO_CHILD_SCREEN. Two parameters are passed:
   // the screen ID and a parameter string.
   static void swap_child_screen (GUI_SCREEN_ID new_child_screen, const char* screen_param);


   // SWAP_STATELESS_SCREEN
   // This method will send a router message to swap the current top-of-stateless-stack
   // stateless screen with the one passed as a parameter. If a stateless screen doesn't exist,
   // the one passed as a parameter will be displayed similarly to GOTO_STATELESS_SCREEN.
   // Two parameters are passed: the screen ID and a parameter string.
   static void swap_stateless_screen (GUI_SCREEN_ID new_stateless_screen, const char* screen_param);


   // SWAP_STATELESS_STACK_FOR_SCREEN
   // This method will send a router message to clear the stateless-stack and then display the
   // stateless screen passed as a parameter. If a stateless stack is already empty, the screen
   // passed as a parameter will be displayed similarly to GOTO_STATELESS_SCREEN.
   // Two parameters are passed: the screen ID and a parameter string.
   static void swap_stateless_stack_for_screen (GUI_SCREEN_ID new_stateless_screen, const char* screen_param);


   // SWAP_CHILD_STACK_FOR_SCREEN
   // This method will send a router message to clear the child-stack and then display the
   // child screen passed as a parameter. If the child stack is already empty, the screen
   // passed as a parameter will be displayed similarly to GOTO_CHILD_SCREEN.
   // Two parameters are passed: the screen ID and a parameter string.
   static void swap_child_stack_for_screen (GUI_SCREEN_ID new_child_screen, const char* screen_param);


   static void screenshotCB (void* data);

#if (CPU==SIMNT)
protected:
   SimntTraverseMsg* _screenTraverseMessage;
   // This function should be overridden by derived classes
   // if that class expects to receive a SimntTraverseMsg
   // and needs to handle it.
   // The SimntTraverseMsg will contain a structure that
   // has an enumerated type TRAVERSE_TYPE (guiUnitTestMessageDefs.h)
   // that determines what kind of TRAVERSE should occur.
   // If a derived class doesn't need multiple TRAVERSES,
   // just use the default UNIVERSAL enumeration.
   // In addition, the struct contains an array of ten longs;
   // use these however you see fit.
   virtual void handleTraverseMessage ();
#endif
}; // END of Base_Apheresis_Window Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH cbf73f8538876b8e3369919d506eff51 */

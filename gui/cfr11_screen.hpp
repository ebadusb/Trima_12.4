/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2003 Gambro BCT, Inc. All rights reserved

*     Class name:   Screen_CFR11

*     File name:    CFR11_screen.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the screen that is raised when the
         the operator id needs to be entered or operator actions reviewed.

*************************< FOCUSSED GUI DOMAIN >****************************
*/

// Single include define guardian
#ifndef CFR11SCR_HPP
#define CFR11SCR_HPP


// CLASS INCLUDE
#include "aphwind.hpp"
#include "bitmap.h"
#include "button_bitmap.h"
#include "discrete_button.h"
#include "display_text.h"
#include "bio_mgr.h"
#include "cfr_list.h"
#include "scan_structure.h"
#include "keypad.h"
#include "keypad.hpp"

// alarm include
#include "alarms.h"

#define MAX_NUMBER_OF_BUTTONS 1

// CLASS DECLARES
class Screen_CFR11
   : public Base_Apheresis_Window
{
private:
   // Bitmaps
   Bitmap operator_id_cartouche;
   Bitmap list_cartouche;

   // Text
   Display_Text operator_id_label;
   Display_Text operator_badge_id;
   Display_Text text_continue_button_disabled;
   Display_Text text_time_of_day;

   // Buttons for list elements
   Discrete_Button* current_selection;
   Discrete_Button* list_btn_1;
   Discrete_Button* list_btn_2;
   Discrete_Button* list_btn_3;
   Discrete_Button* list_btn_4;

   // Buttons
   Button_Bitmap enter_later_button;
   Button_Bitmap clear_badge_button;

   // scroll buttons for list
   Button_Bitmap* up_button;
   Bitmap*        grey_up_button;
   Button_Bitmap* down_button;
   Bitmap*        grey_down_button;

   Docked_Keypad  _dockedKeypad;

   // list iterators to track list elements
   CFR_LIST::iterator list_iter;
   CFR_LIST::iterator end_iter;

   CFR_LIST::iterator btn_one_iter;
   CFR_LIST::iterator btn_two_iter;
   CFR_LIST::iterator btn_three_iter;
   CFR_LIST::iterator btn_four_iter;

   string             _badge;

   TEXT_STRING_ID     _statusLine;
   STATUS_LINE_TYPE   _statusLineType;

   // Received messages
   Message<BADGE_BARCODE_MSG>       _badgeBarcodeMsg;
   Message<KEYPAD_RETURN_STRUCT>    _keypadMsg;
   Message<PinSignatureResponseMsg> _pinSignatureResponseMsg;

   AlarmStateChangeMsg              _alarmStateChangeMsg;


   void allocate_list_buttons ();
   void time_of_day_display ();

   // noncopyable, nonassignable
   Screen_CFR11(const Screen_CFR11&);              // not defined
   Screen_CFR11& operator = (const Screen_CFR11&); // not defined

public:
   // CONSTRUCTOR
   Screen_CFR11();


   // DESTRUCTOR
   virtual ~Screen_CFR11();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // PROCESS_CONTINUE_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the continue button.
   virtual void process_continue_button ();

   // REFRESH_RESOURCES
   // This method will update the linkages to any shareable resources such as
   // the status bar and status line and then rerealize those resources. This
   // method checks that this screen is currently allocated before preforming the
   // refresh.
   virtual void refresh_resources ();

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();


   // Button Actions
   void process_enter_later_button ();
   void process_clear_badge_button ();
   void process_up_button ();
   void process_down_button ();

   void process_list_btn_1 ();
   void process_list_btn_2 ();
   void process_list_btn_3 ();
   void process_list_btn_4 ();

private:

   // REFRESH_LIST
   // only display the elements in the list that have not been processed.
   void refresh_list ();

   bool IsEventSelected ();
   bool NoEventSelected ();
   bool IsPostRun ();

   void ClearSelections ();
   void UpdateAllSelectedItems (const char* badge, AuthorizationStatusType status, BiometricSignatureType deviceId, int sequenceNumber, const void* authData);
   void EraseAllSelectedItems ();
   void DisableButtons (bool action);

   // Message Handlers
   void HandleBadgeBarcodeMsg ();
   void HandleKeypadMsg ();
   void HandlePinSignatureResponseMsg ();
   void HandleAlarmStateChangeMsg ();

   void ExitScreen ();

}; // END of Screen_CFR11 Class Declaration

// End of single include define guardian
#endif

/* FORMAT HASH 8b4a5d4601425fc05ab01aa609fa4b8f */

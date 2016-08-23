/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_ABORTCNF

*     File name:    abortcnf.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

          This screen is based on the Base_Apheresis_Window class and is
          displayed when the operator first asks to cancel the apheresis
          run. It will either dismiss itself (being a child screen of the
          procedure screen) or will send an abort request message to the
          procedure domain to terminate the procedure.

*************************< FOCUSSED GUI DOMAIN >***************************/



// Single include define guardian
#ifndef ABORTCNF_HPP
#define ABORTCNF_HPP



// CLASS INCLUDE
#include <vxworks.h> // for SIMNT compiler directives
#include "aphwind.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "button_bitmap.h"
#include "trimamessages.h"
#include "guiproc.h"


// CLASS DECLARES
class Screen_ABORTCNF
   : public Base_Apheresis_Window
{
private:

   // The type of cancel that calls this routine
   enum CONFIRM_CALL_TYPE
   {
      ABORT_NORMAL,
      ABORT_RINSEBACK,
      ABORT_ALARM,
      ABORT_ALARM_RINSEBACK,
      ABORT_ADDITIVE_SOLUTIONS,        // via endRun
      ABORT_ADDITIVE_SOLUTIONS_ALARM,  // via an alarm
      CONFIRM_SPILLOVER,
      CONFIRM_AIR_IN_PLASMA
   };

   enum AAS_ABORT_ALARM_TYPE
   {
      AAS_ABORT_ALARM_NOT_ACTUALLY_AN_ALARM,
      AAS_ABORT_ALARM_NOT_AAS_RELATED,
      AAS_ABORT_ALARM_RAS_RELATED,
      AAS_ABORT_ALARM_PAS_RELATED
   };

   enum ENUMERATED_ABORT_BUTTONS
   {
      ABORT_BUTTON_NULL,
      ABORT_BUTTON_END_RBC,
      ABORT_BUTTON_END_PLT
   };

   // The exit explanation text object
   Display_Text text_abort_confirm;


   // The explanation cartouche bitmap
   Bitmap bitmap_cancel_text_panel;


   // The buttons
   Button_Bitmap
      _btnExit,              // exit
      _btnRinseback,         // um, rinseback.
      _btnDontExit,          // don't exit
      _btnAbortRas,          // abort RAS
      _btnAbortPas;          // abort PAS

   // The dismiss flag will be set when abortcnf is allocated and either of two
   // messages occur: RemoveMostRecentStatelessScreen or RemoveAllStatelessScreens.
   // If the abort/confirm screen is allocated; it's refresh method is called;
   // the dismiss_flag is set, then the abort/confirm screen will dismiss itself.
   // The intent of this flag is to detect when an alarm screen (stateless screen)
   // has been deallocated off of the abort/confirm screen, the abort/confirm
   // screen is no longer needed (or wanted, see I.T. 4621) and must be deallocated.
   // This flag will be set whenever one of the two above referenced messages is
   // received by this screen.
   int dismiss_flag;

   // declare message for notify RemoveMostRecentStatelessScreen message.
   RemoveMostRecentStatelessScreenMsg _msgRemoveMostRecentStatelessScreen;


   // declare message for notify RemoveAllStatelessScreens message.
   RemoveAllStatelessScreensMsg _msgRemoveAllStatelessScreens;


   // return_confirm_type
   // This method returns the type of cancel requested when this screen is
   // activated. Currently: Normal Cancel, Alarm Cancel, Alarm Rinseback
   // Cancel -- if the call type is an alarm or an alarm rinseback, the number of
   // the alarm will be set in the integer location pointed to by
   // alarm_number.
   CONFIRM_CALL_TYPE return_confirm_type (int* alarm_number);

   AAS_ABORT_ALARM_TYPE getAlarmAasCorrelation (int alarmId);

   void removeButtonCallbacks ();

   // Dismiss this screen, and optionally, the underlying alarm screen too
   void dismissScreen (bool allStatelessScreens = false);

   void composeTextAndGraphics (int alarmId, CONFIRM_CALL_TYPE abortType, bool isBothPasAndRas = false);

   //
   //
   //   Method to clear "pause before spillover confirmed" button
   //
   void clear_spillover_pause ();

   //
   //  message notification
   //
   void screen_notify_handler ();

#if (CPU==SIMNT)
   void cleanSlate ();
#endif // #if (CPU==SIMNT)

public:

   Screen_ABORTCNF();

   virtual ~Screen_ABORTCNF();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // PROCESS_EXIT_BUTTON
   // This method is called when the EXIT button is pressed via the "C"
   // photon event routine: abortcnf_cb_exit_btn
   void process_exit_button ();


   // PROCESS_RINSEBACK_BUTTON
   // This method is called when the RINSEBACK button is pressed via the "C"
   // photon event routine: abortcnf_cb_rinseback_btn
   void process_rinseback_button ();


   // PROCESS_DONOTEXIT_BUTTON
   // This method is called when the DO NOT EXIT button is pressed via the "C"
   // photon event routine: abortcnf_cb_donotexit_btn
   void process_donotexit_button ();

   void process_partial_abort (PARTIAL_END_RUN_TYPE type);


   // REFRESH_RESOURCES
   // This method will update the linkages to any shareable resources such as
   // the status bar and status line and then rerealize those resources. This
   // method checks that this screen is currently allocated before preforming the
   // refresh. This is an override of the aphwind base class.
   virtual void refresh_resources ();

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

#if (CPU==SIMNT)
protected:
   virtual void handleTraverseMessage ();
#endif
};

#endif // include guard

/* FORMAT HASH 13ed4c89535b520df7ea3e41a1dcc91d */

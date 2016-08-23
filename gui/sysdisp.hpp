/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_SYSDISP

*     File name:    sysdisp.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the screen that directs the operator to
         load the blood tubing set disposable. This class is derived from
         the base apheresis class.

*************************< FOCUSSED GUI DOMAIN >****************************
*/



// Single include define guardian
#ifndef SYSDISP_HPP
#define SYSDISP_HPP



#include <vxworks.h> // Needed for SIMNT compiler directives

// CLASS INCLUDE
#include "aphwind.hpp"
#include "bitmap.h"
#include "display_text.h"

// INCLUDES FOR CASSETTE MESSAGING
#include "guiproc.h"
#include "cassette.h"
#include "predict_msgs.h"
#include "trimamessages.h"
#include "timermessage.h"


// CLASS DECLARES



class Screen_SYSDISP
   : public Base_Apheresis_Window
{

private:

   // This screen has three possible states. The latter two depend on the
   // optional features (ARM or Japan Features) and disposable type.
   enum SCREEN_STATE
   {
      LOAD_SET,        // initial state
      CLAMP_DLINE,     // clamp donor line AND sample bag line
      CLAMP_PAS        // clamp PAS line
   };

   SCREEN_STATE _screenState;

   bool
      _showClampPasLine,
      _a2donorMitigation,
      _japanWF;

   // The disposable setup placard bitmap object
   Bitmap
      _bmpDispSetup,
      _bmpClampPasLine,
      _bmpDispClamp,
      _bmpDispClampJp;

   // The display text objects

   Display_Text
   // For Load Set State
      _txtHang,
      _txtInsert,
      _txtLoad,
      _txtClose,

   // For Clamp PAS State
      _txtClampPasLine,

   // For Japanese work flow, don't connect AC message.
      _txtDontConnectAc,

   // For Clamping Donor & Sample lines
      _txtCloseClamp;

   void chooseTextAndGraphic (SCREEN_STATE newScreenState,
                              bool jpDispSet = false);

protected:

   // Screen Event Handler pointers

   // The following two messages are mutually exclusive.
   DisposableLoweredMsg _msgDispLowered;
   void disposable_lowered_received ();

   UnsuccessfulCassetteLoweringMsg _msgDispLowerFailure;
   void disposable_lower_fail_received ();

   // This flag will be set whenever (the 2 mutually exclusive messages)
   //  a disposable lowered message OR a disposable lowered failed message
   //  is received. Once it is set, it will lock out the processing of the
   //  alternate message.
   bool _dispLowered;

   // Timer Event Handler pointers
   TimerMessage _dispLowerTimeout;
   void remove_lowered_msg ();

   CassettePositionStatusMsg _msgProcedureStatusUpdate;
   void cassette_status_message ();
   void displayCassetteStatus (CASSETTE_POSITION_STATUS status, bool airRemovalOn = true);

public:

   // CONSTRUCTOR
   Screen_SYSDISP();


   // DESTRUCTOR
   virtual ~Screen_SYSDISP();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


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

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

       #if CPU==SIMNT
   virtual void handleTraverseMessage ();
       #endif // #if CPU==SIMNT

};  // END of Screen_SYSDISP Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH d7a0c02dab9349d6974d87dbac1c59b4 */

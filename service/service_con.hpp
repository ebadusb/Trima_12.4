/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:

*     Class name:   Screen_SERVICE

*     File name:    service.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This screen is derived from the base apheresis class. It displays
         the initial service product screen at software initialization.

*************************< FOCUSSED GUI DOMAIN >****************************/


// Single include define guardian
#ifndef SERVICE_HPP
#define SERVICE_HPP

// CLASS INCLUDE

#include "bwindow.hpp"
#include "bitmap.h"
#include "button_bitmap.h"
#include "display_text.h"
#include "halstatus_cds.h"


// MESSAGE DECLARES
#include "timermessage.h"
#include "trimamessages.h"

// CLASS DECLARES


class Screen_SERVICE
   : public Base_Window
{
public:
   // CONSTRUCTOR
   Screen_SERVICE();

   // DESTRUCTOR
   virtual ~Screen_SERVICE();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // PROCESS_START_SERVICE_BUTTON
   // This method is called when the START SERVICE button is pressed via the "C"
   // photon event routine: cb_start_service
   void process_start_service ();


   // GOTO_TOUCHSCREEN CAL SCREEN
   // This method is called when the touchscreen cal message is recieved from
   // the TED interface.  This initiates the screen calibration process.
   void gotoTouchscreenCalScreen ();

private:
   // SCREEN_TIMER_HANDLER
   // This virtual method (derived from Base_Apheresis_Window) receives all the
   //  timer notifications and determines which method
   //  to call depending on the ID associated with the timer.
   virtual void handleScreenTimer ();


   // Message Handlers
   void handleSafetyServiceModeMsg ();
   void handleSetCursorMsg ();
   void handleGetCursorMsg ();


   void InitializeFeatureList (bool firstTime = false);
   void FeatureChangedNotification (void* data);
   bool _compPlsFeatureOn;
   bool _extraWBCMsgFeatureOn;


   // The full screen power-up graphic bitmap
   bool         _serviceModeActive;
   Display_Text _connectMsgText;
   Display_Text _titleText;

   Display_Text _hourMeterLabelText;
   Display_Text _trimaHoursText;

   Display_Text _centrifugeHoursLabelText;
   Display_Text _centrifugeHoursText;

   Display_Text _numberOfRunsLabelText;
   Display_Text _numberOfRunsText;

   Display_Text _ipAddressText;
   Display_Text _serialNumText;

   Display_Text _buildLabelText;
   Display_Text _buildNumberText;

   Display_Text _crcLabelText;
   Display_Text _crcValueText;

   // Screen Event Handlers
   SafetyServiceModeMsg  _safetyServiceModeMsg;

   MovePointerServiceMsg _setCursorMsg;
   GetPointerServiceMsg  _getCursorMsg;
   RequestScreenCal      _initScrnCalMsg;

   TimerMessage          _meterUpdateTimer;
   FeatureChanged        _featureChanged;


   // Allow touchscreen calibration to be initiated
   // with the pause button in case the screen is so
   // far out of calibration that touching the start
   // button is impossible.
   TimerMessage _pauseButtonPoll;
   void pollPauseBtn ();

   Button_Bitmap _btnGotoTouchscreenCal;

   HalStatus_CDS _halStatus;
}; // END of Screen_SERVICE Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 843378c2b11d36a61a0337f97736f53e */

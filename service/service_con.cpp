/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:

*     Class name:  Screen_SERVICE

*     File name:   service_con.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$

*/

// BASE CLASS INCLUDE

#include "service_con.hpp"
#include "serv_enum.h"
#include "guiglobs.hpp"
#include "cursor_msgs.h"
#include "units.h"
#include "trima_info.h"
#include "aphwind.hpp"
#include "xif_cds.h"
#include "hour_meter_dat.h"


#define POS_INC   1
#define NEG_INC  -1


#define TEN_SECONDS (10 * 1000) // msec
#define ONE_SECOND  (1000)      // msec
#define PAUSE_BUTTON_POLL_INTERVAL (ONE_SECOND / 2)

// Starting in 5.R, we no longer have a button to initiate start of service mode; this is done automatically
// upon receiving safety service boot-up message
// static void cb_start_service (void * data);


void handleGotoTouchscreenCal (void* data)
{
   ((Screen_SERVICE*)data)->gotoTouchscreenCalScreen();
}

//
// CONSTRUCTOR
//
Screen_SERVICE::Screen_SERVICE()
   : Base_Window(BITMAP_PLACARD_SERVICE),
     _titleText(textMiscServiceDontConnectDonor),
     _connectMsgText(textMiscServiceStarted),

     _hourMeterLabelText(textMiscServiceMachineOnTime),
     _trimaHoursText(textEmptyFormatServiceHourMeter),

     _centrifugeHoursLabelText(textMiscServiceCentrifugeOnTime),
     _centrifugeHoursText(textEmptyFormatServiceCentrifugeMeter),

     _numberOfRunsLabelText(textMiscServiceNumberOfRuns),
     _numberOfRunsText(textEmptyFormatServiceNumRunsMeter),

     _ipAddressText(textEmptyFormatServiceIpAddressInit),
     _serialNumText(textEmptyFormatServiceSerialNumInit),

     _buildLabelText(textMiscFormatServiceBuildNumber),
     _buildNumberText(textEmptyFormatServiceBuildNumber),

     _crcLabelText(textMiscFormatServiceCRCValue),
     _crcValueText(textEmptyFormatServiceCRCValue),

     _btnGotoTouchscreenCal(GUI_BUTTON_SERVICE_TOUCHSCREEN_CAL,
                            530, 60, textEmpty,
                            BITMAP_TOUCHSCREEN_CAL_BTN_UP,
                            BITMAP_TOUCHSCREEN_CAL_BTN_DOWN,
                            true),

     _featureChanged  (Callback<Screen_SERVICE>(this,
                                                &Screen_SERVICE::FeatureChangedNotification),
                       MessageBase::SNDRCV_LOCAL),
     _extraWBCMsgFeatureOn(false),
     _compPlsFeatureOn(false),

     _halStatus(ROLE_RO)
{
   set_screen_id (GUI_SCREEN_SERVICE);
   _display.displayCursor();

   _btnGotoTouchscreenCal.set_callback(handleGotoTouchscreenCal, (void*)this);
   _serviceModeActive = false;
}  // END of Screen_SERVICE CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_SERVICE::~Screen_SERVICE()
{
   // If we haven't already deallocated, do it now.
   Screen_SERVICE::deallocate_resources ();
}



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_SERVICE::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Window::allocate_resources (allocation_parameter);

   // then allocate constituent objects with this window as parent widget
   //  layer allocation from Z-back to Z-front.

   // display the banner
   _titleText.allocate_resources(*this);

   _hourMeterLabelText.allocate_resources(*this);
   _trimaHoursText.allocate_resources(*this);

   _centrifugeHoursLabelText.allocate_resources(*this);
   _centrifugeHoursText.allocate_resources(*this);

   _numberOfRunsLabelText.allocate_resources(*this);
   _numberOfRunsText.allocate_resources(*this);

   _ipAddressText.set_text_direct(strlen(TrimaInfo::ipAddress()), TrimaInfo::ipAddress());
   _ipAddressText.allocate_resources(*this);

   _serialNumText.set_text_direct(strlen(TrimaInfo::serialNumber()), TrimaInfo::serialNumber());
   _serialNumText.allocate_resources(*this);

   _buildLabelText.allocate_resources(*this);
   _buildNumberText.set_text_direct(strlen(TrimaInfo::buildNumber()), TrimaInfo::buildNumber());
   _buildNumberText.allocate_resources(*this);

   _crcLabelText.allocate_resources(*this);
   _crcValueText.set_text_direct(strlen(TrimaInfo::trimaCrc()), TrimaInfo::trimaCrc());
   _crcValueText.allocate_resources(*this);

   DataLog (log_level_service_info) << "Allocating touchscreen cal button." << endmsg;
   _btnGotoTouchscreenCal.allocate_resources(*this);

   // activate the notify object for incoming messages
   _safetyServiceModeMsg.init(Callback<Screen_SERVICE>(this, &Screen_SERVICE::handleSafetyServiceModeMsg), MessageBase::SNDRCV_GLOBAL);

   _setCursorMsg.init(Callback<Screen_SERVICE>(this, &Screen_SERVICE::handleSetCursorMsg), MessageBase::SNDRCV_RECEIVE_ONLY);

   guistring gString;
   gString.initialize();
   gString += "########.#";

   _trimaHoursText.set_text_direct(gString.length(), gString);
   _centrifugeHoursText.set_text_direct(gString.length(), gString);
   _numberOfRunsText.set_text_direct(gString.length(), gString);

   // Setup initial callback interval for one second to get initial data
   // to screen quickly.  Thereafter, update is at 10 second intervals.
   //
   _meterUpdateTimer.init(ONE_SECOND, Callback<Screen_SERVICE>(this, &Screen_SERVICE::handleScreenTimer), TimerMessage::ARMED);
   _pauseButtonPoll.init(PAUSE_BUTTON_POLL_INTERVAL, Callback<Screen_SERVICE>(this, &Screen_SERVICE::pollPauseBtn), TimerMessage::ARMED);

   _getCursorMsg.init(Callback<Screen_SERVICE>(this, &Screen_SERVICE::handleGetCursorMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
   _initScrnCalMsg.init(Callback<Screen_SERVICE>(this, &Screen_SERVICE::gotoTouchscreenCalScreen), MessageBase::SNDRCV_RECEIVE_ONLY);

} // END of Screen_SERVICE ALLOCATE_RESOURCES



//
////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_SERVICE::deallocate_resources ()
{
   // deallocate all constituent objects first
   _titleText.deallocate_resources();

   _hourMeterLabelText.deallocate_resources();
   _trimaHoursText.deallocate_resources();

   _centrifugeHoursLabelText.deallocate_resources();
   _centrifugeHoursText.deallocate_resources();

   _numberOfRunsLabelText.deallocate_resources();
   _numberOfRunsText.deallocate_resources();

   _ipAddressText.deallocate_resources();
   _serialNumText.deallocate_resources();

   _buildLabelText.deallocate_resources();
   _buildNumberText.deallocate_resources();

   _crcLabelText.deallocate_resources();
   _crcValueText.deallocate_resources();

   _meterUpdateTimer.armTimer(TimerMessage::DISARMED);
   _pauseButtonPoll.armTimer(TimerMessage::DISARMED);

   _safetyServiceModeMsg.deregisterMsg();

   _setCursorMsg.deregisterMsg();
   _getCursorMsg.deregisterMsg();
   _initScrnCalMsg.deregisterMsg();

   _btnGotoTouchscreenCal.deallocate_resources();

   // deallocate parent's resources
   Base_Window::deallocate_resources ();
}  // END of Screen_SERVICE DEALLOCATE_RESOURCES


//
////////////////////////////////////////////
//
void Screen_SERVICE::process_start_service ()
{
   SystemStateChangeMsg systemState(MessageBase::SEND_GLOBAL);
   systemState.send( (State_names)SERVICE_MODE);

   _connectMsgText.allocate_resources(*this);

   External_IF_CDS xifCds(ROLE_RW);
   xifCds.safetyStarted.Set(true);
}



//
///////////////////////////////////////////////////////////////////////////////
//
void Screen_SERVICE::handleSafetyServiceModeMsg ()
{
   if ( !_serviceModeActive )
   {
      DataLog(log_level_service_info) << "Safety service request received - displaying start service mode button" << endmsg;
      InitializeFeatureList(true);

      // Center the button on the screen
      OSRegion btn_dimensions;
      OSRegion scrn_dimensions;

      // Starting in 5.R, we no longer have a button to initiate start of service mode; this is done automatically
      // upon receiving safety service boot-up message
      scrn_dimensions  = background_bitmap.get_dimensions();
      btn_dimensions.x = (short)(scrn_dimensions.width / 2 - btn_dimensions.width / 2 );
      btn_dimensions.y = (short)(scrn_dimensions.height / 2 - btn_dimensions.height / 2 );

      process_start_service();
      _serviceModeActive = true;
   }

   if ( _safetyServiceModeMsg.getData() == 0 )
   {
      // let safety know we got the message
      DataLog(log_level_service_info) << "Responding to safety service request" << endmsg;
      _safetyServiceModeMsg.send(1);
   }
}



//
// Handles the Set Cursor Msg from TED
//
void Screen_SERVICE::handleSetCursorMsg ()
{
   int x, y;

   // Get Current Cursor position
   _display.getCursorPos(x, y);

   // Get Command
   CURSOR_CMDS cmd = (CURSOR_CMDS)_setCursorMsg.getData();

   switch (cmd)
   {
      case CURSOR_LEFT :
         x += NEG_INC;
         break;

      case CURSOR_RIGHT :
         x += POS_INC;
         break;

      case CURSOR_UP :
         y += NEG_INC;
         break;

      case CURSOR_DOWN :
         y += POS_INC;
         break;
   }

   // Set new cursor position
   _display.setCursorPos(x, y);
}



//
// This routine handles the Get Cursor Msg from TED.
//
void Screen_SERVICE::handleGetCursorMsg ()
{
   CURRENT_CURSOR_POSITION_MSG          cursorPos;
   Message<CURRENT_CURSOR_POSITION_MSG> msg(MessageBase::SEND_LOCAL);

   _display.getCursorPos(cursorPos.x, cursorPos.y);

   msg.send(cursorPos);
}



//
// handleScreenTimer
//
void Screen_SERVICE::handleScreenTimer ()
{
   char ipAddressStr[INET_ADDR_LEN];

   // data is in minutes.  Divide by 60 to get the actual hours
   const float trimaOnTime      = (float)(HourMeterDat::totalMachineTime()) / float(MINUTES_PER_HOUR);
   const float centrifugeOnTime = (float)(HourMeterDat::totalCentrifugeOnTime()) / float(MINUTES_PER_HOUR);
   const int   numberOfRuns     = HourMeterDat::totalNumberOfRuns();

   guistring   gString;
   gString.initialize();

   sprintf(gString, "%.1f", trimaOnTime);
   _trimaHoursText.deallocate_resources();
   _trimaHoursText.set_text_direct(gString.length(), gString);
   _trimaHoursText.allocate_resources(*this);

   sprintf(gString, "%.1f", centrifugeOnTime);
   _centrifugeHoursText.deallocate_resources();
   _centrifugeHoursText.set_text_direct(gString.length(), gString);
   _centrifugeHoursText.allocate_resources(*this);

   sprintf(gString, "%d", numberOfRuns);
   _numberOfRunsText.deallocate_resources();
   _numberOfRunsText.set_text_direct(gString.length(), gString);
   _numberOfRunsText.allocate_resources(*this);

   _meterUpdateTimer.interval(TEN_SECONDS);
}

//
// handleScreenTimer
//
void Screen_SERVICE::pollPauseBtn ()
{
   if ( _halStatus.PauseSwitch() )
   {
      DataLog(log_level_service_info) << "Pause button pressed" << endmsg;
      gotoTouchscreenCalScreen();
   }
}

//
// gotoTouchscreenCalScreen
//
void Screen_SERVICE::gotoTouchscreenCalScreen ()
{
   Base_Apheresis_Window::goto_screen(GUI_SCREEN_TOUCHSCREEN_CAL, "GUI_SCREEN_TOUCHSCREEN_CAL");
}

void Screen_SERVICE::InitializeFeatureList (bool firstTime)
{
   for (int i = 0; i < FEATURE_DEF_END; i++)
   {
      if (Software_CDS::GetInstance().getFeature((FEATURE_ID)i))
      {
         DataLog(log_level_service_info) << "Feature: " << i << " "
                                         << Software_CDS::GetInstance().getFeatureConfigFileLabel((FEATURE_ID)i) << " =on" << endmsg;

         if (firstTime)
         {
            if (i == ComplementaryPlasmaAvailable)
            {
               _compPlsFeatureOn = true;
            }
            else if (i == AllowExtraWbcMessages)
            {
               _extraWBCMsgFeatureOn = true;
            }
         }
         else
         {
            if (i == ComplementaryPlasmaAvailable)
            {
               if (!_compPlsFeatureOn)
               {
                  guiglobs::cdsData.rwConfig.predict.key_complementary_plasma = false;
                  guiglobs::cdsData.rwConfig.write();
                  DataLog(log_level_service_info) << "key_complementary_plasma is turned off " << endmsg;

                  _compPlsFeatureOn = true;
               }
            }
            else if (i == AllowExtraWbcMessages)
            {
               if (!_extraWBCMsgFeatureOn)
               {
                  guiglobs::cdsData.rwConfig.procedure.key_show_pre_aas_flags = true;
                  guiglobs::cdsData.rwConfig.write();
                  DataLog(log_level_service_info) << "AllowExtraWbcMessages is turned on " << endmsg;

                  _extraWBCMsgFeatureOn = true;
               }
            }
         }
      }
      else
      {
         if (i == ComplementaryPlasmaAvailable)
         {
            _compPlsFeatureOn = false;
         }
         else if (i == AllowExtraWbcMessages)
         {
            _extraWBCMsgFeatureOn = false;
         }
      }
   }
}

void Screen_SERVICE::FeatureChangedNotification (void* data)
{
   InitializeFeatureList();
}

/* FORMAT HASH b6f23368f160f411e8f127dbe22ad4e4 */

/*
 * $Header$
 *
 *  GUI screen used to encapsulate the calibration of the touch screen.
 *
 */

#include <vxWorks.h>
#include <usrLib.h>
#include <math.h>

#include "touchscreenCal.hpp"

#include "aphwind.hpp"
#include "guipalette.h"
#include "trima_datalog_levels.h"
#include "software_cds.h"

#define XOFFSET 20
#define YOFFSET 20
#define HAIR_LENGTH 15
#define HAIR_WIDTH 1
#define MIN_TS_EVENT_DISTANCE 100.0f
#define MAX_TS_REJECTIONS 3
#define WRITE_DELAY 1000

TouchscreenCal::TouchscreenCal()
   : Base_Window(BITMAP_PLACARD_SERVICE),
     _animationTimer(),
     _state(TSCAL_IDLE),
     _upperLeftX(0.0),
     _upperLeftY(0.0),
     _upperRightX(0.0),
     _upperRightY(0.0),
     _lowerLeftX(0.0),
     _lowerLeftY(0.0),
     _pointerDevId(NULL),
     _screenTitle(textMiscServiceTouchscreenCalTitle),
     _instructions(textMiscServiceTsCalInstruction1),
     _datfile(PNAME_TCHSCRNDAT),

     _vHair (OSRectangle(_display, OSRegion(XOFFSET - HAIR_WIDTH / 2, YOFFSET - HAIR_LENGTH / 2, HAIR_WIDTH, HAIR_LENGTH), guipalette::lookupColor(255, 255, 255))),
     _hHair (OSRectangle(_display, OSRegion(XOFFSET - HAIR_LENGTH / 2, YOFFSET - HAIR_WIDTH / 2, HAIR_LENGTH, HAIR_WIDTH), guipalette::lookupColor(255, 255, 255))),

     _rejectionCount(0)
{
   _newCalArray[0] = 16.0;
   _newCalArray[1] = 0.5;
   _newCalArray[2] = 0.0;
   _newCalArray[3] = 0.0;
   _newCalArray[4] = 0.0;
   _newCalArray[5] = 0.5;
   _newCalArray[6] = 0.0;

   _oldCalArray[0] = 0.0;
   _oldCalArray[1] = 0.0;
   _oldCalArray[2] = 0.0;
   _oldCalArray[3] = 0.0;
   _oldCalArray[4] = 0.0;
   _oldCalArray[5] = 0.0;
   _oldCalArray[6] = 0.0;

   Base_Window::set_screen_id (GUI_SCREEN_TOUCHSCREEN_CAL);

   if (_datfile.Error())
   {
      DataLog (log_level_service_error) << "Unable to open datfile " << PNAME_TCHSCRNDAT << ". Error=" << _datfile.Error() << endmsg;
      return;
   }

   // Get pointer to pointing device.
   const UGL_REG_DATA* pRegistryData = uglRegistryFind (UGL_PTR_TYPE, 0, 0, 0);

   if (pRegistryData == UGL_NULL)
   {
      DataLog (log_level_service_error) << "Error finding pointer device" << endmsg ;
   }
   else
   {
      _pointerDevId = (UGL_INPUT_DEV_ID)pRegistryData->id;
   }

   _animationTimer.init(WRITE_DELAY, Callback<TouchscreenCal>(this, &TouchscreenCal::commitNewCalibration), TimerMessage::DISARMED);
}



void TouchscreenCal::allocate_resources (const char* allocation_parameter)
{
   Base_Window::allocate_resources(allocation_parameter);

   _rejectionCount = 0;

   add_object_to_front(&_vHair);
   add_object_to_front(&_hHair);

   if (_datfile.Error())
   {
      DataLog (log_level_service_error) << "Unable to open datfile " << PNAME_TCHSCRNDAT << ". Error=" << _datfile.Error() << endmsg;
      return;
   }

   // Scale calibration to avoid clipping by touchscreen driver
   //
   _oldCalArray[0] = Software_CDS::GetInstance().getTouchscreenBoundDelay();
   _oldCalArray[1] = _datfile.get_float("TOUCHSCREEN", "a", -1000.0f, 1000.0f);
   _oldCalArray[2] = _datfile.get_float("TOUCHSCREEN", "b", -1000.0f, 1000.0f);
   _oldCalArray[3] = _datfile.get_float("TOUCHSCREEN", "c", -1000.0f, 1000.0f);
   _oldCalArray[4] = _datfile.get_float("TOUCHSCREEN", "d", -1000.0f, 1000.0f);
   _oldCalArray[5] = _datfile.get_float("TOUCHSCREEN", "e", -1000.0f, 1000.0f);
   _oldCalArray[6] = _datfile.get_float("TOUCHSCREEN", "f", -1000.0f, 1000.0f);

   _newCalArray[0] = 16.0;
   _newCalArray[1] = 0.5;
   _newCalArray[2] = 0.0;
   _newCalArray[3] = 0.0;
   _newCalArray[4] = 0.0;
   _newCalArray[5] = 0.5;
   _newCalArray[6] = 0.0;

   if (_pointerDevId)
      uglInputDevControl(_pointerDevId, ICR_CALIBRATE, (void*)_newCalArray);

   _state = TSCAL_UPPER_LEFT;   // start up the calibration state machine.

   attach();

   // This needs to be set here in case the screen is visited more than once.
   _instructions.set_string_id(textMiscServiceTsCalInstruction1);

   _screenTitle.allocate_resources(*this);
   _instructions.allocate_resources(*this);
}



//
//
//
TouchscreenCal::~TouchscreenCal()
{
   TouchscreenCal::deallocate_resources();
}



//
//
//
void TouchscreenCal::deallocate_resources ()
{
   killTimer();

   _screenTitle.deallocate_resources();
   _instructions.deallocate_resources();

   Base_Window::deallocate_resources();
}



// Need this for the same reason CGUIButton does.
void TouchscreenCal::attach ()
{
   WIN_ATTRIB winAttrib = WIN_ATTRIB_NO_INPUT | WIN_ATTRIB_VISIBLE;

   OSWindow::attach(getParentPointer(), winAttrib);

   UGL_STATUS_MONITOR(winCbAdd(_uglWinId, MSG_PTR_FIRST, MSG_PTR_LAST, &TouchscreenCal::respondToPointerEvent, UGL_NULL) );
}


// Set the array values, then check them for validity.  If they look OK,
// then set the 1 second timer to commit them to disk.
void TouchscreenCal::setCalibration ()
{
   // Algorithm lifted from gui_main.c.
   _newCalArray[0] = _oldCalArray[0]; // This should be 8.0 or whatever was read in from config file at allocate_resources
   _newCalArray[1] = (((double)_display.width()) / 2) / (_upperRightX - _upperLeftX);
   _newCalArray[2] = 0.0;
   _newCalArray[3] = -_newCalArray[1] * ( _upperLeftX + _lowerLeftX );
   _newCalArray[4] = 0.0;
   _newCalArray[5] = (((double)_display.height()) / 2) / (_lowerLeftY - _upperLeftY);
   _newCalArray[6] = -_newCalArray[5] * ( _upperLeftY + _upperRightY );

   if (_newCalArray[1] > 2 || _newCalArray[1] < 0 ||
       _newCalArray[3] > 0 || _newCalArray[3] < -200 ||
       _newCalArray[5] > 2 || _newCalArray[5] < 0 ||
       _newCalArray[6] > 0 || _newCalArray[6] < -200)
   {
      DataLog (log_level_service_info) << "Bad touchscreen calibration values: {"
                                       << _newCalArray[1] << ";" << _newCalArray[2] << ";"
                                       << _newCalArray[3] << ";" << _newCalArray[4] << ";"
                                       << _newCalArray[5] << ";" << _newCalArray[6] << "}" << endmsg ;

      abort();
      return;
   }
   else
   {
      // We do this with a timer to give the screen enough time to display a message
      // before the disk update.  Otherwise, the screen never gets a chance to update
      // before the heavy disk operations.
      _animationTimer.armTimer();
   }
}



// Actually commit the new calibration to disk
void TouchscreenCal::commitNewCalibration ()
{
   killTimer();

   DataLog (log_level_service_info) << "Setting touchscreen bounce delay to " << _newCalArray[0] << endmsg ;
   DataLog (log_level_service_info) << "Setting touchscreen calibration values to {" << _newCalArray[1] << ";" << _newCalArray[2] << ";" << _newCalArray[3] << ";" << _newCalArray[4] << ";" << _newCalArray[5] << ";" << _newCalArray[6] << "}" << endmsg ;

   if (_pointerDevId)
      uglInputDevControl(_pointerDevId, ICR_CALIBRATE, (void*)_newCalArray);

   if (_datfile.Error())
   {
      DataLog (log_level_service_error) << "Unable to open datfile " << PNAME_TCHSCRNDAT << ". Error=" << _datfile.Error() << endmsg;
      abort();
      return;
   }

   _datfile.SetFloat("TOUCHSCREEN", "a", _newCalArray[1]);
   _datfile.SetFloat("TOUCHSCREEN", "b", _newCalArray[2]);
   _datfile.SetFloat("TOUCHSCREEN", "c", _newCalArray[3]);
   _datfile.SetFloat("TOUCHSCREEN", "d", _newCalArray[4]);
   _datfile.SetFloat("TOUCHSCREEN", "e", _newCalArray[5]);
   _datfile.SetFloat("TOUCHSCREEN", "f", _newCalArray[6]);

   _datfile.Write(PNAME_TCHSCRNDAT);

   fixCrc();

   DataLog (log_level_service_info) << FILE_TCHSCRN_DAT << " file written." << endmsg;

   Base_Apheresis_Window::goto_screen(GUI_SCREEN_SERVICE, "GUI_SCREEN_SERVICE");
}



// POINTER EVENT
// virtual function in OSWindow class that is implemented here for this screen to intercept
// the events and and use them to obtain the touchscreen coordinates at which they occur.
UGL_STATUS TouchscreenCal::respondToPointerEvent (WIN_ID id, WIN_MSG* pMsg, void* pData, void* pParam)
{
   UGL_WINDOW_ID   windowId = id;
   TouchscreenCal* window   = *(TouchscreenCal**)pData;

   if (pMsg != 0)
   {
      DataLog(log_level_service_info) << "Pointer event: (x=" << pMsg->data.ptr.position.x
                                      << "; y=" << pMsg->data.ptr.position.y << "; windowId=" << windowId
                                      << "; buttonState=" << pMsg->data.ptr.buttonState << ")" << endmsg;
   }

   if ((windowId != UGL_NULL) && window && pMsg)
   {
      // We are only interested in left button press or release events
      if ((pMsg->data.ptr.buttonChange & 0x01) == 0)
         return UGL_STATUS_FINISHED;

      if ((pMsg->data.ptr.buttonState & 0x01) != 0)
      {
         // Left button is pressed.  Grab the pointer to ensure
         // we get the release event as well.
         winPointerGrab (windowId);

         window->workYourCrazyCrosshairMagic(pMsg->data.ptr.position.x, pMsg->data.ptr.position.y);
      }
   }
   else
   {
      winPointerUngrab (windowId);
   }

   return UGL_STATUS_FINISHED;
}

void TouchscreenCal::workYourCrazyCrosshairMagic (int touchX, int touchY)
{
   switch (_state)
   {
      case TSCAL_UPPER_LEFT :
         _upperLeftX = touchX - XOFFSET / 2;
         _upperLeftY = touchY - YOFFSET / 2;
         DataLog (log_level_service_info) << "Upper left point = {" << _upperLeftX << ";" << _upperLeftY << "}" << endmsg ;

         _vHair.set_region(OSRegion(_display.width() - XOFFSET - HAIR_WIDTH / 2, YOFFSET - HAIR_LENGTH / 2, HAIR_WIDTH, HAIR_LENGTH) );
         _hHair.set_region(OSRegion(_display.width() - XOFFSET - HAIR_LENGTH / 2, YOFFSET - HAIR_WIDTH / 2, HAIR_LENGTH, HAIR_WIDTH) );
         _instructions.set_string_id(textMiscServiceTsCalInstruction2);
         _state = TSCAL_UPPER_RIGHT;
         break;

      case TSCAL_UPPER_RIGHT :

         _upperRightX = touchX + XOFFSET / 2;
         _upperRightY = touchY - YOFFSET / 2;
         DataLog (log_level_service_info) << "Upper right point = {" << _upperRightX << ";" << _upperRightY << "}" << endmsg ;

         if (distance(_upperLeftX, _upperLeftY, _upperRightX, _upperRightY) < MIN_TS_EVENT_DISTANCE)
         {
            rejectScreenTouch();
            return;
         }

         _vHair.set_region(OSRegion(XOFFSET - HAIR_WIDTH / 2, _display.height() - YOFFSET - HAIR_LENGTH / 2, HAIR_WIDTH, HAIR_LENGTH) );
         _hHair.set_region(OSRegion(XOFFSET - HAIR_LENGTH / 2, _display.height() - YOFFSET - HAIR_WIDTH / 2, HAIR_LENGTH, HAIR_WIDTH) );
         _instructions.set_string_id(textMiscServiceTsCalInstruction3);
         _state = TSCAL_LOWER_LEFT;
         break;

      case TSCAL_LOWER_LEFT :
         _lowerLeftX = touchX - XOFFSET / 2;
         _lowerLeftY = touchY + YOFFSET / 2;
         DataLog (log_level_service_info) << "Lower left point = {" << _lowerLeftX << ";" << _lowerLeftY << "}" << endmsg ;

         if (distance(_upperRightX, _upperRightY, _lowerLeftX, _lowerLeftY) < MIN_TS_EVENT_DISTANCE ||
             distance(_upperLeftX, _upperLeftY, _lowerLeftX, _lowerLeftY) < MIN_TS_EVENT_DISTANCE)
         {
            rejectScreenTouch();
            return;
         }

         _instructions.set_string_id(textMiscServiceTsCalInstruction4);
         delete_object(&_vHair);
         delete_object(&_hHair);

         setCalibration();
         _state = TSCAL_IDLE;
         break;

      default :
         DataLog (log_level_service_error) << "Button press in idle mode." << endmsg;
         break;
   }
}

void TouchscreenCal::fixCrc ()
{
   DataLog (log_level_service_info) << "Updating CRC for touch_screen.dat" << endmsg;
   attrib(CALDAT_CRC_FILE, "-R");
   softcrc("-filelist " CALDAT_CRC_LIST_FILES "   -update " CALDAT_CRC_FILE);
   attrib(CALDAT_CRC_FILE, "+R");

   DataLog (log_level_service_info) << "Updating machine CRC" << endmsg;
   attrib(MACHINE_CRC_FILE, "-R");
   softcrc("-filelist " MACHINE_CRC_LIST_FILES " -update " MACHINE_CRC_FILE);
   attrib(MACHINE_CRC_FILE, "+R");
}

void TouchscreenCal::rejectScreenTouch ()
{
   DataLog (log_level_service_info) << "Rejecting most recent screen touch event." << endmsg;
   // Increment the counter, and if we've reached the
   // maximum rejections, then abort.
   if (++_rejectionCount >= MAX_TS_REJECTIONS)
      abort();
}

int TouchscreenCal::distance (int x1, int y1, int x2, int y2)
{
   int returnVal = (int)sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));

   DataLog (log_level_service_info) << "Distance from (" << x1 << "; " << y1 << ") to ("
                                    << x2 << "; " << y2 << ") is " << returnVal << endmsg;

   return returnVal;
}

void TouchscreenCal::abort ()
{
   killTimer();
   DataLog (log_level_service_error) << "Aborting touchscreen cal update." << endmsg;

   DataLog (log_level_service_error) << "Roll back config values." << endmsg;

   if (_pointerDevId)
      uglInputDevControl(_pointerDevId, ICR_CALIBRATE, (void*)_oldCalArray);

   DataLog (log_level_service_error) << "Exit screen." << endmsg;

   _state = TSCAL_IDLE;

   Base_Apheresis_Window::goto_screen(GUI_SCREEN_SERVICE, "GUI_SCREEN_SERVICE");
}

void TouchscreenCal::killTimer ()
{
   _animationTimer.armTimer(TimerMessage::DISARMED);
}

/* FORMAT HASH d9992eaaf4ad5a62b08948268cbdea3f */

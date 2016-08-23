/*
 * $Header$
 *
 *  Touchscreen calibration service screen.
 *
 */

#ifndef __TOUCHSCREEN_CAL_HPP__
#define __TOUCHSCREEN_CAL_HPP__

#include <vxworks.h>
#include <ugl/uglinput.h>

#include "timermessage.h"

#include "bwindow.hpp"
#include "bitmap.h"
#include "button_bitmap.h"
#include "display_text.h"
#include "halstatus_cds.h"
#include "datfile.h"

class TouchscreenCal
   : public Base_Window
{
public:

   TouchscreenCal();  // display context created in gui_main, provided to state machines which in
                      // turn provides it to the screens that run as part of the state machine.

   virtual ~TouchscreenCal();

   virtual void TouchscreenCal::allocate_resources (const char* allocation_parameter);

   // Callback.  Needs to be static
   static UGL_STATUS respondToPointerEvent (WIN_ID id, WIN_MSG* pMsg, void* pData, void* pParam);

   virtual void attach ();

   // delete objects.  Should be done in destructor eventually.
   virtual void deallocate_resources ();

   const double calArray (int which)
   {
      if (which >=0 && which < 7)
      {
         return _newCalArray[which];
      }
      else
      {
         return 0.0;
      }
   }

   static void fixCrc ();

private:
   TimerMessage _animationTimer;

   enum
   {
      TSCAL_UPPER_LEFT, TSCAL_UPPER_RIGHT, TSCAL_LOWER_LEFT, TSCAL_IDLE
   } _state;

   double _upperLeftX, _upperLeftY;
   double _upperRightX, _upperRightY;
   double _lowerLeftX, _lowerLeftY;

   // These are the values we're planning on
   // switching to.
   double _newCalArray[7];

   // Also store the original values in case we
   // abort and need to roll back.
   double _oldCalArray[7];

   // The UGL device ID for the touchscreen
   UGL_INPUT_DEV_ID _pointerDevId;

   OSRectangle      _vHair, _hHair;

   Display_Text     _screenTitle;
   Display_Text     _instructions;

   TouchscreenCal (const TouchscreenCal& obj);
   TouchscreenCal operator = (TouchscreenCal& obj);

   inline const double xToRaw (short x, short y);
   inline const double yToRaw (short x, short y);
   void                touchscreenCalCB (void);
   void                setCalibration (void);
   void                workYourCrazyCrosshairMagic (int touchX, int touchY);

   static int distance (int x1, int y1, int x2, int y2);
   void       abort ();
   void       commitNewCalibration ();
   void       rejectScreenTouch ();
   void       killTimer ();

   int            _rejectionCount;

   CDatFileReader _datfile;
};

#endif

/* FORMAT HASH 10350d9c6d22e7c3bcb3cb4931b6fd06 */

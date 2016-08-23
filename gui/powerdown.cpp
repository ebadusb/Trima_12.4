/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_POWERDOWN

*     File name:   powerdown.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$

*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "powerdown.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "guipalette.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_POWERDOWN::Screen_POWERDOWN()
   : Base_Window(BITMAP_NULL),
     _txtMsg(textMiscPowerdownMsg),
#if (CPU==SIMNT)
     _screenTraverseMessage(NULL),
#endif
     _background(_display, OSRegion(0, 0, _display.width(), _display.height()), guipalette::lookupColor(0, 0, 0))
{
   set_screen_id(GUI_SCREEN_POWERDOWN);
}   // END of Screen_POWERDOWN CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_POWERDOWN::~Screen_POWERDOWN()
{
   deallocate_resources ();
}   // END of Screen_POWERDOWN DESTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_POWERDOWN::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Window::allocate_resources (allocation_parameter);

   add_object_to_front(&_background);
   _txtMsg.allocate_resources(*this);

   guiglobs::guiMain->waitForWatchdog();
        #if (CPU==SIMNT)
   _screenTraverseMessage = new SimntTraverseMsg(Callback<Screen_POWERDOWN>(this, &Screen_POWERDOWN::handleTraverseMessage), MessageBase::SNDRCV_RECEIVE_ONLY);
        #endif

}  // END of Screen_POWERDOWN ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_POWERDOWN::deallocate_resources ()
{
   _txtMsg.deallocate_resources();
   delete_object(&_background);

        #if CPU==SIMNT
   if (_screenTraverseMessage)
   {
      delete _screenTraverseMessage;
      _screenTraverseMessage = NULL;
   }
          #endif /* if CPU==SIMNT */

   // deallocate parent's resources
   Base_Window::deallocate_resources ();
}   // END of Screen_POWERDOWN DEALLOCATE_RESOURCES


void Screen_POWERDOWN::watchdogFailure ()
{
   _txtMsg.set_string_id(textMiscPowerdownWatchdogFailure);
}

#if CPU==SIMNT
void Screen_POWERDOWN::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == UNIVERSAL)
   {
      POWERDOWN_DISPLAY_TYPE powerdownDisplayType = (POWERDOWN_DISPLAY_TYPE)traverseInfo.parameters[0];

      DataLog(log_level_sim_gui_side_info) << "Received our Power Down Traverse Message type " << powerdownDisplayType << endmsg;

      switch (powerdownDisplayType)
      {
         case POWERDOWN_POWERING_DOWN :
            _txtMsg.set_string_id(textMiscPowerdownMsg);
            break;
         case POWERDOWN_WATCHDOG_FAILURE :
            _txtMsg.set_string_id(textMiscPowerdownWatchdogFailure);
            break;
         default :
            DataLog (log_level_sim_gui_side_error) << "Malformed message received.  Unknown Powerdown Display Type: " << powerdownDisplayType << endmsg;
            return;
      }

      _display.flush();
      guiglobs::message_system.dispatchMessages();
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Unable to deal with traverse type " << traverseInfo.type << endmsg;
   }
}
#endif /* if CPU==SIMNT */

/* FORMAT HASH 861daf64565ada4d6ba795df97abd4d5 */

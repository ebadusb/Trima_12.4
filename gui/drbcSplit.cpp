/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_DRBC_SPLIT

*     File name:   drbcSplit.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "drbcSplit.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "sounds.hpp"
#include "generic_positions.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "CatalogChecker.h"

static void drbcSplitCbCancelBtn (void* data);
static void drbcSplitCbContinueBtn (void* data);
static void drbcSplitCbRinsebackBtn (void* data);

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
//

void Screen_DRBC_SPLIT::setUpOipInfo (const char* allocation_parameter)
{
   // allocation paramter ignored here.
   _alarmId        = DRBC_SPLIT_ALERT;
   _isBeepingAlarm = true;
   // override default sound settings for the alarm.
   _soundCode      = DRBC_SPLIT_SOUND;
   _soundMode      = SOUND_CONTINUOUS;
}

Screen_DRBC_SPLIT::Screen_DRBC_SPLIT()
   : OipAlarmBase(TEAL, GUI_SCREEN_DRBC_SPLIT),

     _bmpSplitDrbcsCollectNonfiltered (272, 142, BITMAP_DRBC_SPLIT),
     _bmpSplitDrbcsCollectFiltered (272, 142, BITMAP_DRBC_SPLIT_FILTERED),
     _bmpSplitDrbcsAddition (272, 142, BITMAP_DRBC_SPLIT_AAS),
     _bmpBackground (10, 115, BITMAP_ALARM_TEXT_AND_BITMAP_BACKGROUND),

     _txtSplitDuringCollectNonfiltered    (textAlarmBodyWithGraphicDrbcSplit),
     _txtSplitDuringCollectFiltered       (textAlarmBodyWithGraphicDrbcPtfSplit),
     _txtSplitDuringAddition              (textAlarmBodyWithGraphicDrbcAasSplit),

     _btnContinue   (GUI_BUTTON_DRBC_SPLIT_CONTINUE,
                     BITMAP_BUTTON_GENERIC_X,
                     BITMAP_BUTTON_CONTINUE_Y,
                     textButtonLongContinue,
                     BITMAP_BUTTON_CONTINUE_UP,
                     BITMAP_BUTTON_CONTINUE_DN),

     _btnCancel     (GUI_BUTTON_DRBC_SPLIT_CANCEL,
                     BITMAP_BUTTON_GENERIC_X,
                     BITMAP_BUTTON_EXIT_Y,
                     textButtonAlarmExit,
                     BITMAP_BUTTON_EXIT_UP,
                     BITMAP_BUTTON_EXIT_DN),

     _btnRinseback  (GUI_BUTTON_REPFLUID_RINSEBACK,
                     BITMAP_BUTTON_GENERIC_X,
                     BITMAP_BUTTON_RINSEBACK_Y,
                     textButtonAlarmRinseback,
                     BITMAP_BUTTON_RINSEBACK_UP,
                     BITMAP_BUTTON_RINSEBACK_DN),

     _currentDisplayMode(DISPLAY_MODE_COLLECT_FILTERED)
{}



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_DRBC_SPLIT::~Screen_DRBC_SPLIT()
{
   if (_allocated)
   {
      // If we haven't already deallocated, do it now.
      Screen_DRBC_SPLIT::deallocate_resources ();
   }
}



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_DRBC_SPLIT::allocate_resources (const char* allocation_parameter)
{
   OipAlarmBase::allocate_resources(allocation_parameter);
   _currentDisplayMode = getDisplayMode();

   // Allocate the bitmaps
   _bmpBackground.allocate_resources(*this);

   switch (_currentDisplayMode)
   {
      case DISPLAY_MODE_COLLECT_FILTERED :
         DataLog (log_level_gui_info) << "Allocating as DISPLAY_MODE_COLLECT_FILTERED" << endmsg;
         _bmpSplitDrbcsCollectFiltered.allocate_resources(*this);
         _txtSplitDuringCollectFiltered.allocate_resources(*this);
         break;

      case DISPLAY_MODE_COLLECT_NONFILTERED :
         DataLog (log_level_gui_info) << "Allocating as DISPLAY_MODE_COLLECT_NONFILTERED" << endmsg;
         _bmpSplitDrbcsCollectNonfiltered.allocate_resources(*this);
         _txtSplitDuringCollectNonfiltered.allocate_resources(*this);
         break;

      case DISPLAY_MODE_RAS_ADDITION :
         DataLog (log_level_gui_info) << "Allocating as DISPLAY_MODE_RAS_ADDITION" << endmsg;
         _bmpSplitDrbcsAddition.allocate_resources(*this);
         _txtSplitDuringAddition.allocate_resources(*this);
         break;
   }

   // Allocate continue button
   _btnContinue.allocate_resources(*this);

   // Allocate either rinseback or cancel button, as appropriate.
   // (Note that as long as this is the collect version, rinseback
   // will be an option.)
   if (guiglobs::proc_status.rinsebackIsAnOption())
   {
      _btnCancel.deallocate_resources();
      _btnRinseback.allocate_resources(*this);
   }
   else
   {
      _btnRinseback.deallocate_resources();
      _btnCancel.allocate_resources(*this);
   }

   // Link the buttons to their "C" callback stubs
   _btnContinue.set_callback (drbcSplitCbContinueBtn, (void*)this);
   _btnCancel.set_callback   (drbcSplitCbCancelBtn, (void*)this);
   _btnRinseback.set_callback(drbcSplitCbRinsebackBtn, (void*)this);

   // now deallocate "standard" buttons that are not needed
   btn_goback.deallocate_resources ();
   btn_continue.deallocate_resources ();
   btn_lowleft.set_up_bitmap (BITMAP_BUTTON_ACTIVATE_SOUND);
   // btn_lowleft.deallocate_resources ();

}  // END of Screen_DRBC_SPLIT ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_DRBC_SPLIT::deallocate_resources ()
{
   // Remove alarm status line color and message
   guiglobs::apheresis_status_line->deactivate_status_line_type(ALARM_STATUS_LINE);

   _bmpSplitDrbcsCollectNonfiltered.deallocate_resources();
   _bmpSplitDrbcsCollectFiltered.deallocate_resources();
   _bmpSplitDrbcsAddition.deallocate_resources();
   _bmpBackground.deallocate_resources();

   _txtSplitDuringCollectNonfiltered.deallocate_resources();
   _txtSplitDuringCollectFiltered.deallocate_resources();
   _txtSplitDuringAddition.deallocate_resources();

   _btnContinue.deallocate_resources();
   _btnCancel.deallocate_resources();

   // deallocate parent's resources
   OipAlarmBase::deallocate_resources ();
}   // END of Screen_DRBC_SPLIT DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// PROCESS_RF_CANCEL_BUTTON
void Screen_DRBC_SPLIT::processCancelBtn ()
{
   cancelOip();
}   // END of Screen_DRBC_SPLIT PROCESS_RF_CANCEL_BUTTON



////////////////////////////////////////////////////////////////////////////////
// PROCESS_RF_CONTINUE_BUTTON
void Screen_DRBC_SPLIT::processContinueBtn ()
{
   continueOip();
}   // END of Screen_DRBC_SPLIT PROCESS_RF_CONTINUE_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_RF_CONTINUE_BUTTON
void Screen_DRBC_SPLIT::processRinsebackBtn ()
{
   rinsebackOip();
}   // END of Screen_DRBC_SPLIT PROCESS_RF_CONTINUE_BUTTON

////////////////////////////////////////////////////////////////////////////////
// GET_DISPLAY_MODE

Screen_DRBC_SPLIT::DISPLAY_MODE Screen_DRBC_SPLIT::getDisplayMode ()
{
   if (guiglobs::screen_status.get_current_system_state() == METERED_STORAGE)
   {
      DataLog (log_level_gui_info) << "Returning DISPLAY_MODE_RAS_ADDITION." << endmsg;
      return DISPLAY_MODE_RAS_ADDITION;
   }
   else if (guiglobs::cassette_mgr->currentSetIsRas())
   {
      DataLog (log_level_gui_info) << "Returning DISPLAY_MODE_COLLECT_FILTERED." << endmsg;
      return DISPLAY_MODE_COLLECT_FILTERED;
   }
   else
   {
      DataLog (log_level_gui_info) << "Returning DISPLAY_MODE_COLLECT_NONFILTERED." << endmsg;
      return DISPLAY_MODE_COLLECT_NONFILTERED;
   }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CONTINUE BUTTON CALLBACK
void drbcSplitCbContinueBtn (void* data)
{
   // Actually call the continue method within REFLUID screen
   ((Screen_DRBC_SPLIT*)data) -> processContinueBtn ();

}   // END of CONTINUE BUTTON CALLBACK



////////////////////////////////////////////////////////////////////////////////
// CANCEL BUTTON CALLBACK
void drbcSplitCbCancelBtn (void* data)
{
   // Actually call the continue method within REFLUID screen
   ((Screen_DRBC_SPLIT*)data) -> processCancelBtn ();

}   // END of CANCEL BUTTON CALLBACK

// RINSEBACK BUTTON CALLBACK
void drbcSplitCbRinsebackBtn (void* data)
{
   // Actually call the continue method within REFLUID screen
   ((Screen_DRBC_SPLIT*)data) -> processRinsebackBtn ();

}   // END of RINSEBACK BUTTON CALLBACK

void Screen_DRBC_SPLIT::initialize_status_line ()
{
   guiglobs::apheresis_status_line->set_current_status_line_text_by_type (ALARM_STATUS_LINE, textStatDrbcSplitAlert);
}


#if CPU==SIMNT
void Screen_DRBC_SPLIT::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == UNIVERSAL)
   {
      DRBC_SPLIT_DISPLAY_TYPE drbcSplitDisplayType = (DRBC_SPLIT_DISPLAY_TYPE)traverseInfo.parameters[0];

      _bmpSplitDrbcsCollectFiltered.deallocate_resources();
      _txtSplitDuringCollectFiltered.deallocate_resources();
      _bmpSplitDrbcsCollectNonfiltered.deallocate_resources();
      _txtSplitDuringCollectNonfiltered.deallocate_resources();
      _bmpSplitDrbcsAddition.deallocate_resources();
      _txtSplitDuringAddition.deallocate_resources();

      DataLog (log_level_sim_gui_side_info) << "DRBC split screen got display type " << drbcSplitDisplayType << endmsg;

      switch (drbcSplitDisplayType)
      {
         case DRBC_SPLIT_COLLECT_FILTERED :
            _bmpSplitDrbcsCollectFiltered.allocate_resources(*this);
            _txtSplitDuringCollectFiltered.allocate_resources(*this);
            break;
         case DRBC_SPLIT_COLLECT_NONFILTERED :
            _bmpSplitDrbcsCollectNonfiltered.allocate_resources(*this);
            _txtSplitDuringCollectNonfiltered.allocate_resources(*this);
            break;
         case DRBC_SPLIT_RAS_ADDITION :
            _bmpSplitDrbcsAddition.allocate_resources(*this);
            _txtSplitDuringAddition.allocate_resources(*this);
            break;
         default :
            DataLog (log_level_sim_gui_side_error) << "ERROR: Unknown display type " << drbcSplitDisplayType << endmsg;
            return;
      }
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif /* if CPU==SIMNT */

/* FORMAT HASH c9d7fa06a8924b9243214e249a32c062 */

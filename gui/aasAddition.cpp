/*
* Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
*
* TITLE:      aasAddition.cpp
*
* $Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/aasAddition.cpp 1.15 2008/07/14 20:23:16Z dslausb Exp dslausb $
*
*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "aasAddition.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "runsumGlobs.hpp"


// EXTERNAL REFERENCES

#include "guipalette.h"
#include "guiglobs.hpp"
#include "math.h"
#include "procvolumes_cds.h"

///////////////////////////////////////
// DEFINES FOR METER PLACEMENT
//
//  -> common to both meters
#define METER_GRAPHIC_X             70
#define METER_BAR_X_DELTA           4
#define METER_BAR_X                 METER_GRAPHIC_X + METER_BAR_X_DELTA
#define METER_BAR_W                 466
#define METER_BAR_H                 33
// pas only
#define AAS_METER_TEXT_DELTA            20
#define AAS_METER_BAR_Y_DELTA           3

#define FIRST_METER_GRAPHIC_Y               230
#define FIRST_METER_BAR_Y               FIRST_METER_GRAPHIC_Y + AAS_METER_BAR_Y_DELTA
#define FIRST_METER_LABEL_Y             FIRST_METER_GRAPHIC_Y - AAS_METER_TEXT_DELTA

// ras only
#define SECOND_METER_GRAPHIC_Y              290
#define SECOND_METER_LABEL_Y                SECOND_METER_GRAPHIC_Y - AAS_METER_TEXT_DELTA

////////////////////////////////////////////////////////////////////////////////

Screen_AASADDITION::Screen_AASADDITION()
   : Base_Apheresis_Window(MAGENTA, GUI_SCREEN_AAS_ADDITION),
     _txtAasLabel(textMiscAas),
     _txtAasProg(textMiscPasProgress),
     _aasProgBar(NULL),

     _bmpAasProg(METER_GRAPHIC_X,
                 FIRST_METER_GRAPHIC_Y,
                 BITMAP_MSS_PROG_BAR),

     _meterUpdateLoggingCount(0)
{}


////////////////////////////////////////////////////////////////////////////////
Screen_AASADDITION::~Screen_AASADDITION()
{
   Screen_AASADDITION::deallocate_resources();
}


////////////////////////////////////////////////////////////////////////////////
void Screen_AASADDITION::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);
   guiglobs::aasRunInfo.attach(this);
   guiglobs::aasRunInfo.setAasAddedCallback(Callback<Screen_AASADDITION>(this, &Screen_AASADDITION::updateAasMeter));

   // now deallocate "standard" buttons that are not needed
   btn_continue.set_string_id (textButtonLongNextPage);
   btn_continue.allocate_resources(*this);

   btn_lowleft.deallocate_resources ();
   btn_goback.deallocate_resources();
   btn_cancel.set_string_id(textButtonEndAddition);

   // Now link the status bar/line to this window
   link_status_bar_line();

   // allocate the text holders  (NULL right now)
   _txtAasLabel.allocate_resources(*this);
   _txtAasProg.allocate_resources(*this);

   _bmpAasProg.allocate_resources(*this);
   buildAasMeter();
}

////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_AASADDITION::deallocate_resources ()
{
   guiglobs::aasRunInfo.detach(this);
   // deallocate all constituent objects first

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   btn_cancel.set_string_id(textButtonCancel);

   // clean up the text lines
   _txtAasLabel.deallocate_resources();
   _txtAasProg.deallocate_resources();
   _bmpAasProg.deallocate_resources();

   // deallocate the progress bar
   delete _aasProgBar;
   _aasProgBar = NULL;

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////

void Screen_AASADDITION::process_cancel_button ()
{
   goto_child_screen (GUI_SCREEN_ABORTCNF, ABORT_TYPE_ADDITIVE_SOLUTIONS);
}

////////////////////////////////////////////////////////////////////////////////

void Screen_AASADDITION::buildAasMeter ()
{
   _aasProgBar = constructProgMeter(guipalette::meter_yellow,
                                    TroubleMeter::Horizontal,
                                    METER_BAR_X,
                                    FIRST_METER_BAR_Y,
                                    METER_BAR_H,
                                    METER_BAR_W);

   updateAasMeter();
}

void Screen_AASADDITION::updateAasMeter ()
{
   if (!_aasProgBar)
   {
      DataLog (log_level_gui_info) << "AAS progress bar does not exist. Unable to update." << endmsg;
      return;
   }

   const float aasNeeded = guiglobs::aasRunInfo.getAasNeeded();
   const float aasAdded  = guiglobs::aasRunInfo.getAasAdded();

   if (aasNeeded <= 0 ||
       aasNeeded >= 5000 ||
       aasAdded < 0 ||
       aasAdded > aasNeeded)
   {
      DataLog (log_level_gui_debug) << "Unable to update AAS progress bar. (aasNeeded="
                                    << aasNeeded << "; aasAdded=" << aasAdded << ")" << endmsg;

      return;
   }

   _aasProgBar->Update(0, 1, (aasAdded / aasNeeded), 1, 0);

  #if (CPU!=SIMNT) // Only log every 10th message, unless this is the simulator
   if (++_meterUpdateLoggingCount % 10 == 0)
  #endif
   {
      DataLog (log_level_gui_info) << "AAS progress bar has been updated.  AAS Needed = "
                                   << aasNeeded << ", AAS Accumulated = " << aasAdded << "." << endmsg;
   }
}

TroubleMeter* Screen_AASADDITION::constructProgMeter (OSColor color, TroubleMeter::MeterType type, short x, short y, short height, short width)
{
   TroubleMeter* meter = new TroubleMeter;

   meter->Initialize(*this, x, y, height, width, color, TroubleMeter::CapDisabled, type);
   meter->Active(1);
   return (meter);

}  // End of BUILD_METER

void Screen_AASADDITION::stopListeningToProcUpdates ()
{
   guiglobs::aasRunInfo.detach(this);
}

void Screen_AASADDITION::process_continue_button ()
{
   // switch to run summary screen
   DataLog (log_level_gui_mss_info) << "Stop listening to updates from proc for a while." << endmsg;
   stopListeningToProcUpdates();

   DataLog (log_level_gui_mss_info) << "Going to Runsumm1 screen in parallel with metering" << endmsg;
   guiglobs::run_summary_display_active = true;

   RunsumGlobs::gotoNextRunSummaryScreen();
}

void Screen_AASADDITION::initialize_status_line ()
{
   guiglobs::apheresis_status_line->set_current_text (textStatAasActiveAddition);
}

/* FORMAT HASH 5d0ea3dcd4e0b245121d3231c4e4eb37 */

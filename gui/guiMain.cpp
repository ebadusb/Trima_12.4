/*
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <signal.h>
#include "guiMain.hpp"
#include "error.h"
#include "trima_datalog.h"
#include "guiglobs.hpp"
#include "guipalette.h"
#include "scrncomm.hpp"
#include "caldat.h"
#include "powerdown.hpp"
#include "aphwind.hpp"
#include "cgui_pool_allocator.hpp"
#include "osgraphics.h"
#include "timermessage.h"
#include "software_cds.h"

extern void guiinit ();

//
// Application specific initialization function
//

bool          GuiMain::_initialized               = false;
bool          GuiMain::_screenRefresh             = false;
bool          GuiMain::_systemReady               = false;

TimerMessage* GuiMain::_timerMsgPtr_sendRebootMsg = NULL;

OSDisplay*    GuiMain::_display                   = NULL;
GuiMain*      GuiMain::_instance                  = NULL;

GuiMain::GuiMain(void)
   : _alreadyCleanedUp(false)
{
   if (_instance)
   {
      _FATAL_ERROR(__FILE__, __LINE__, "Multiple creation of main GUI object");
   }

   DataLog (log_level_gui_info) << "initializing GuiMain." << endmsg;

   _instance = this;

   // GUI memory allocator is now initialized by TrimaVariablDatabase::read(),
   // which is loaded & called by language_loader.out
   // cguiPoolAllocatorInitialize();

   // create display reference
   _display = new OSDisplay (Callback<GuiMain>(this, &GuiMain::startCB),
                             Callback<GuiMain>(this, &GuiMain::wakeUpCB), 25);
   if (_display == NULL)
   {
      _FATAL_ERROR(__FILE__, __LINE__, "OSDisplay returned null");
      return; // redundant, but this helps static analysis tools
   }

   DataLog (log_level_gui_info) << "GuiMain is initialized." << endmsg;

   _initialized = true;
}

GuiMain::~GuiMain()
{
   _FATAL_ERROR(__FILE__, __LINE__, "Attempted deletion of main GUI procedure object");
}

void GuiMain::wakeUpCB ()
{
   // If we recieved the _alreadyCleanedUp signal and we haven't already processed the
   // the cleanup stuff, go ahead and do it.
   if (guiglobs::message_system.dispatcher().receivedSignal() && !_alreadyCleanedUp)
   {
      printf("GUI got the _alreadyCleanedUp signal.\n");
      cleanup();
   }
}


void GuiMain::startCB (void)
{
   DataLog (log_level_gui_startup_debug) << "WinApp startCB()" << endmsg;

   TASK_DESC taskDescriptor;
   taskInfoGet (taskIdSelf(), &taskDescriptor);

   DataLog(log_level_gui_info) << "winApp current stack margin is " << taskDescriptor.td_stackMargin
                               << " high water mark is " << taskDescriptor.td_stackHigh << " size is " << taskDescriptor.td_stackSize << endmsg;

   log_level_gui_info.consoleOutput(DataLog_ConsoleEnabled);
   log_level_gui_error.consoleOutput(DataLog_ConsoleEnabled);

#if CPU==SIMNT
   log_level_gui_startup_debug.consoleOutput(DataLog_ConsoleEnabled);
#endif

   DataLog (log_level_gui_startup_debug) << "Initialize font data structure." << endmsg;
   if (_display)
   {
      _display->initFonts();

      DataLog (log_level_gui_startup_debug) << "Set color entries." << endmsg;

      if (guiglobs::guiMain)
      {
         for (int i = 0; i<OSPaletteSize; i++)
         {
            _display->set_color_entry(i, guipalette::palette[i]);
         }
      }
      else
      {
         _FATAL_ERROR(__FILE__, __LINE__, "Null GUI pointer");
      }
   }
   else
   {
      _FATAL_ERROR(__FILE__, __LINE__, "Null _display pointer");
      return; // redundant, but helps static analysis
   }

   DataLog (log_level_gui_startup_debug) << "Look up default colors." << endmsg;
   guipalette::meter_green  = guipalette::lookupColor(165, 214, 165);
   guipalette::meter_yellow = guipalette::lookupColor(255, 255, 0);
   guipalette::meter_red    = guipalette::lookupColor(255, 0, 0);

   DataLog (log_level_gui_startup_debug) << "Show the splash screen." << endmsg;

   showSplash();

   // ***********************************
   // Initialize touchscreen
   // ***********************************
   DataLog (log_level_gui_startup_debug) << "Initialize touchscreen." << endmsg;

   const float bounceSetting = Software_CDS::GetInstance().getTouchscreenBoundDelay();

   double      touchScreenCalData[7];
   touchScreenCalData[0] = (double)((bounceSetting) ? bounceSetting : 8.0f);
   touchScreenCalData[1] = (double)CalDat::data().touchScreenCalA;
   touchScreenCalData[2] = (double)CalDat::data().touchScreenCalB;
   touchScreenCalData[3] = (double)CalDat::data().touchScreenCalC;
   touchScreenCalData[4] = (double)CalDat::data().touchScreenCalD;
   touchScreenCalData[5] = (double)CalDat::data().touchScreenCalE;
   touchScreenCalData[6] = (double)CalDat::data().touchScreenCalF;

   DataLog (log_level_gui_startup_debug) << "Copied touchscreen data to local GUI structure: "
                                         << "(a=" << touchScreenCalData[1] << ";b=" << touchScreenCalData[2] << ";c=" << touchScreenCalData[3]
                                         << ";d=" << touchScreenCalData[4] << ";e=" << touchScreenCalData[5] << ";f=" << touchScreenCalData[6]
                                         << ";bounce=" << touchScreenCalData[0] << ")"   << endmsg;

   DataLog (log_level_gui_startup_debug) << "Set up UGL registry." << endmsg;

   UGL_REG_DATA* pRegistryData = uglRegistryFind (UGL_PTR_TYPE, 0, 0, 0);

   if (pRegistryData != UGL_NULL)
   {
      UGL_INPUT_DEV_ID pointerDevId = (UGL_INPUT_DEV_ID)pRegistryData->id;
      uglInputDevControl(pointerDevId, ICR_CALIBRATE, (void*)touchScreenCalData);
   }

   // End of touchscreen calibration. *******************************
   // Allocate the GUI Commutator.
   DataLog(log_level_gui_startup_debug) << "Creating and starting the screen commutator." << endmsg;
   guiglobs::scrnComm = new Screen_Commutator;
   guiglobs::scrnComm->start_commutator();

   // Call the initialization routine of the Alarm object.
   // This is declared in GUIGLOBS for addressability by
   // alarm.cpp.
   DataLog (log_level_gui_startup_debug) << "Starting the alarm manager." << endmsg;
   guiglobs::gui_alarm_manager.start_alarm_manager ();

   guiinit(); // specific for service or normal mode

   DataLog (log_level_gui_info) << "GUI start routine complete." << endmsg;
}



void GuiMain::showSplash ()
{
   // Display startup bitmap just to avoid having a blank screen for several seconds
   OSWindow* startupWindow = new OSWindow(*_display, OSRegion(0, 0, _display->width(), _display->height()));

   OSBitmap* startupBitmap = new OSBitmap(*_display, BITMAP_PLACARD_POWERUP);

   startupWindow->add_object_to_front(startupBitmap);

   startupWindow->attach(NULL);

   _display->flush();

   delete startupWindow;
   delete startupBitmap;
}

void GuiMain::waitForWatchdog ()
{
   printf("Entering waitForWatchdog().\n");
   _timerMsgPtr_sendRebootMsg = new TimerMessage(1000, CallbackBase(&GuiMain::watchdogWaitRoutine), TimerMessage::ARMED);
   printf("Leaving waitForWatchdog().\n");
}

void GuiMain::watchdogWaitRoutine ()
{
   printf("Entering watchdogWaitRoutine().\n");

   if (_timerMsgPtr_sendRebootMsg)
   {
      printf("Disarming watchdog wait timer message.\n");
      _timerMsgPtr_sendRebootMsg->armTimer(TimerMessage::DISARMED);

      printf("Deleting watchdog wait timer message.\n");
      delete _timerMsgPtr_sendRebootMsg;
      _timerMsgPtr_sendRebootMsg = NULL;
   }

   printf("Sending the FinishedViewingStatsMsg message.\n");

   // Send a message declaring that the information viewing is completed
   FinishedViewingStatsMsg finished(MessageBase::SEND_LOCAL);
   finished.send(0);

   printf("Waiting for watchdog....\n");

   // Wait for watchdog to reset machine
   taskDelay(15 * sysClkRateGet());

   printf("The watchdog didn't do its job.\n");
   // If we got here the watchdog failed

   if (guiglobs::scrnComm && _display)
   {
      Screen_POWERDOWN* powerdownScreen = (Screen_POWERDOWN*)guiglobs::scrnComm->getScreenInstancePointer(GUI_SCREEN_POWERDOWN);
      powerdownScreen->watchdogFailure();
      _display->flush();
      guiglobs::message_system.dispatchMessages(); // Make sure GUI's message system gets dispatched here.
   }
   else
   {
      printf("Graphical stuff wasn't set up.\n");
   }

   printf("Leaving watchdogWaitRoutine().\n");
}


void GuiMain::cleanup (void)
{
   _alreadyCleanedUp = true;
   Base_Apheresis_Window::goto_screen(GUI_SCREEN_POWERDOWN, "GUI_SCREEN_POWERDOWN");
   waitForWatchdog();
}


OSDisplay* GuiMain::getDisplay ()
{
   if (!_display)
   {
      _FATAL_ERROR(__FILE__, __LINE__, "Requested display is NULL.");
   }

   return _display;
}

/* FORMAT HASH e4d4a945836f5b273dc33608649fc274 */

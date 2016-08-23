/*
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#ifndef __GUI_MAIN_HPP__
#define __GUI_MAIN_HPP__

class TimerMessage;
class OSDisplay;

class GuiMain
{
private:
   bool            _alreadyCleanedUp;
   static GuiMain* _instance;
   static bool     _initialized;
   void showSplash ();

   // As soon as GUI sends out the message saying it's
   // reboot time, it sets off a chain reaction that
   // basicly starves out the GUI task.  So, handle this
   // in a timing message to allow GUI to TCOB beforehand.
   static TimerMessage* _timerMsgPtr_sendRebootMsg;

   static OSDisplay*    _display;

public:
   GuiMain(void);
   virtual ~GuiMain();

   void wakeUpCB (void);
   void startCB (void);
   void cleanup (void);

   static bool        _systemReady;
   static bool        _screenRefresh;
   static const char* _defaultFontName;

   static GuiMain*   instance (void) { return _instance; }
   static bool       initialized (void) { return _initialized; }
   static OSDisplay* getDisplay (void);

   static void waitForWatchdog ();
   static void watchdogWaitRoutine ();
};

#endif /* ifndef __GUI_MAIN_HPP__ */

/* FORMAT HASH 5cc645988964885aa33c3da49eac33e4 */

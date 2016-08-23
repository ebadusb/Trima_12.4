/*
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 * $Log$
 *
 * Display raw A/D values to a text screen
 */

#include <curses.h>
#include <stdlib.h>

#include "buffmsg.hpp"
#include "chw.hpp"
#include "dispatch.hpp"

WINDOW * stdscr;

class statusMessage : public focusBufferMsg<CHwStates>
{
   public:
      statusMessage();
      ~statusMessage();
      void notify();

   private:
      CHwStates _statusData;
};

statusMessage::statusMessage() :
   focusBufferMsg<CHwStates>(ControlHardwareStatus)
{
}

statusMessage::~statusMessage()
{
}

void
statusMessage::notify()
{
   static int  updateCount = 3; 
   get(&_statusData);

   if ( ++updateCount > 3 )
   {
      updateCount = 0;
      move(1,17);
      printw("%4d", _statusData.accessPressure);
      move(2,17);
      printw("%4d", _statusData.centrifugeCurrent);
      move(3,17);
      printw("%4d", _statusData.centrifugePressure);
      move(4,17);
      printw("%4d", _statusData.fiveVoltSupply);
      move(5,17);
      printw("%4d", _statusData.leakDetector);
      move(6,17);
      printw("%4d", _statusData.minusTwelvePS);
      move(7,17);
      printw("%4d", _statusData.sixtyFourVoltCurrent);
      move(8,17);
      printw("%4d", _statusData.sixtyFourVoltSupply);
      move(9,17);
      printw("%4d", _statusData.sixtyFourVoltSwitched);
      move(10,17);
      printw("%4d", _statusData.twelveVoltSupply);
      move(11,17);
      printw("%4d", _statusData.twentyFourVoltCurrent);
      move(12,17);
      printw("%4d", _statusData.twentyFourVoltSupply);
      move(13,17);
      printw("%4d", _statusData.twentyFourVoltSwitched);
      refresh();
   }
}

void
main(int argc, char *argv[])
{
   int d_argc = 4;
   char* d_argv[5];

   argc = 0;
   d_argv[0] = argv[0];
   d_argv[1] = getenv("CQUE");
   if(d_argv[1] == 0)
   {
       d_argv[1] = "ctest";
   }

   d_argv[2] = getenv("SNODE");
   if(d_argv[2] == 0)
   {
       d_argv[2] = "2";
   }

   d_argv[3] = getenv("SQUE");
   if(d_argv[3] == 0)
   {
       d_argv[3] = "stest";
   }

   d_argv[4] = 0;
   dispatch = new dispatcher(d_argc, d_argv);

   stdscr = initscr();
   cbreak();
   noecho();
   erase();

   move(1,1);
   printw("access pressure");
   move(2,1);
   printw("cent current");
   move(3,1);
   printw("cent pressure");
   move(4,1);
   printw("5V supply");
   move(5,1);
   printw("leak detector");
   move(6,1);
   printw("-12V supply");
   move(7,1);
   printw("64V current");
   move(8,1);
   printw("64V supply");
   move(9,1);
   printw("64V switched");
   move(10,1);
   printw("12V supply");
   move(11,1);
   printw("24V current");
   move(12,1);
   printw("24V supply");
   move(13,1);
   printw("24V switched");
   refresh();

   statusMessage  * msg = new statusMessage();
   dispatch->dispatchLoop();

   endwin();
   delete dispatch;
   exit(0);
}

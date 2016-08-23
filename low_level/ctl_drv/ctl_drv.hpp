/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      Focussed System Control Hardware Driver
 *
 * ABSTRACT:   Interface to the control hardware
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 */

#ifndef _CTL_DRV_INCLUDE
#define _CTL_DRV_INCLUDE

#include <time.h>

#include "chw.hpp"
#include "valve.hpp"
#include "cent.hpp"
#include "c_cent2.h"
#include "c_door.hpp"
#include "c_rbc.hpp"
#include "c_pump.hpp"
#include "c_temp.hpp"
#include "c_sound.hpp"
#include "c_valve.h"
#include "fastdata.h"
#include "fastpack.hpp"
#include "cmsginfo.hpp"
#include "ostime.hpp"



// forward refs
void processAnalog (unsigned char& base16, unsigned char& powerFailFlag,
                    bool& blockPumps, bool& blockMessages);

void processAnalogProd (bool& blockPumps, bool& blockMessages);

// classes

// SPECIFICATION:    derived message class, used to send fast filtered
//                   access pressure data.
//
// ERROR HANDLING:   none.

// SPECIFICATION:    debounce pause and stop switches, update() will only
//                   change states when a number readings in a row match.
//
// ERROR HANDLING:   none.

class debounce
{
public:
   debounce(HWSwitch select, HW_SWITCH_STATES& sw, HW_SWITCH_STATES& swLatch);
   virtual ~debounce();

   void update (void);

private:
   HWSwitch          _select;             // switch to be monitored
   HW_SWITCH_STATES& _debouncedValue;     // debounced status
   HW_SWITCH_STATES& _latchedValue;       // latched status

   unsigned short    _count;              // debounce counter
   unsigned long     _latchCount;         // counter to maintain the latch value
};

// SPECIFICATION:    used to send immediate response pressure
//                   data.
//
// ERROR HANDLING:   none.

class fastfilter
{
public:
   fastfilter(const char* sharename);
   virtual ~fastfilter();
   void dataPoint (short data);

private:
   FASTFIL* _FPtr;
};



//
// Create the control version of the centrifugeRPM class
//
class ControlCentRpm
   : public centrifugeRpm
{
protected:
   virtual void clearCommStatus ()
   {
      chw_centClearCommutation();
   }
};



// SPECIFICATION:    derived message class, used to get order info
//
// ERROR HANDLING:   none.

class commands
{
public:
   commands(centrifugeControl& centrifuge);
   virtual ~commands();

   bool getNewOrder ();                   // get new orders from shmem task
   void doNewOrder ();                    // do the new orders
   unsigned char redDrive;                // rbc red drive
   unsigned char greenDrive;              // rbc green drive

   static const CHO* getCHO () { return _CHOPtr; }

private:
   commands();                            // default ctr disabled
   commands(commands const&);
   commands& operator = (commands const&);

   static CHO*        _CHOPtr;

   centrifugeControl& _centControl;        // centrifuge control class
   unsigned int       _commandCounter;
   unsigned           _rbcCounter;         // used to force an update
                                           // to rbc drive values
};


// SPECIFICATION:    timer class, this class runs at a periodic rate,
//                   set by main(), and controls all hardware by calling
//                   various class members
//
// ERROR HANDLING:   none.

class updateTimer
{
public:
   updateTimer(unsigned long dt,                  // timer rate, ms
               commands& command,                 // command class
               centrifugeControl& centrifuge,     // centrifuge class
               unsigned char& powerFailFlag);     // power fail flag (set by interrupt handler)

   virtual ~updateTimer();

   void timerCallback (void);                       // process periodic timer messages
   void getValves ();                               // update valve states
   void newOrders ();                               // process new orders
   void petTheDog ();                               // service the hardware watchdog
   void checkTheDog (void);                         // check the software watchdog timer

   HWWatchDogStatus controlWDStatus (void);                    // Get the status of the Hardware WatchDog
   void             updateTimer::controlSealSafeStatus (void); // Get Status of the Seal Safe

private:
   unsigned long _lastPetCounter;                 // software watchdog counter
   rawTime       _lastPetTime;                    // last time of petting software watchdog
   rawTime       _timeInSafeState;                // time spent in safe state waiting for Proc Orders
   bool          _preSoftWatchdogPausePumps;

private:
   updateTimer();                         // default ctr disabled
   updateTimer(updateTimer const&);
   updateTimer& operator = (updateTimer const&);

   void commandUpdate ();                  // checks for new commands in shmem
   void dataToGo ();                       // new status data to shmem
   void monitorTickTiming (void);          // check for proper operation of OS tick timer

private:
   commands&            _commands;

   CHS*                 _CHSPtr;

   pump                 _acPump;        // pumps
   pump                 _inletPump;
   pump                 _plateletPump;
   pump                 _plasmaPump;
   returnPump           _returnPump;

   readValve            _plasmaValve;  // valves
   valveRunLogic        _plasmaRunValve;
   readValve            _plateletValve;
   valveRunLogic        _plateletRunValve;
   readValve            _rbcValve;
   valveRunLogic        _rbcRunValve;
   readValve            _cassette;

   cassetteRunLogic     _cassetteRun;

   ControlCentRpm       _centrifuge;

   rbcDetector          _rbcDetector;    // interface to the rbcPort task

   debounce             _stop;         // pushbuttons
   debounce             _pause;

   long                 _dt;             // update interval
   unsigned             _count;          // counter
   unsigned             _ccount;         // centrifuge control counter
   bool                 _newMxInstalled; // New MX Technology or Legacy FPGAs Installed
   rawTime              _powerOnTime;    // Time since the 24V power was turned on.
   bool                 _powerOnFlag;    // TRUE if 24V power is stable

   HW_SWITCH_STATES     _oldPauseSwitch;  // old states
   HW_SWITCH_STATES     _oldStopSwitch;

   CHW_RESERVOIR_STATES _oldReservoir;


   HWWatchDogStatus     _newWdStatus;     // WatchDog Status new/old
   HWWatchDogStatus     _oldWdStatus;

   HWSealSafeStatus     _oldSealSafeStatus; // Seal Safe Status ( ON/Off )
   HWSealSafeStatus     _newSealSafeStatus;

   rawTime              _lastTimeSealSafeSwitchedOn;
   bool                 _sealSafeLatch;

   HWSealSafeOTW        _oldSealSafeOTW;  // Seal Safe Over Temp Warning
   HWSealSafeOTW        _newSealSafeOTW;  // ( Warning/OK )

   HWOvpTestResult      _oldOVPTestResult; // Power Supply Over Voltage
   HWOvpTestResult      _newOVPTestResult; // Protect (OVP) Test Result (Pass/Fail)

   HWSupplyOutputStatus _oldSupplyOutputStatus; // Power Supply Outputs 12.7,
   HWSupplyOutputStatus _newSupplyOutputStatus; // 24, 64 (Enabled/Disabled)

   centrifugeControl&   _centControl;        // centrifuge control

   doorControlStatus    _door;           // door control and status
   tempSense            _temp;           // temperature sensor
   soundLevelInterface  _soundInterface; // sound control and status

   rawTime              _fluidsSeenTimer; // timer of fluid at low-level sensor
   bool                 _logStopReturn;   // log flags controlling information about
   bool                 _logStartReturn;  //  handling of reservoir empty
   HW_ORDERS            _fluid_seen_once; // flag if seen at least once

   unsigned char&       _powerFailFlag;
   rawTime              _dogLastTime;       // last time of petting watchdog
   rawTime              _doorOverrideTimer; // override the centrifuge command during door solenoid retry
   bool                 _blockPumps;        // TRUE = stop pumps for APS violation
   bool                 _blockMessages;     // TRUE = don't send any more APS violation messages

   bool                 _softWatchdogActive; // TRUE = ignore all commands from Proc
   rawTime              _acAirToFluidTimer;  // debounce timer for AC air to fluid transitions

   fastpack             _usAir;
   fastpack             _usFluid;
   fastpack             _lsAir;
   fastpack             _lsFluid;


   //
   // see monitorTickTiming for usage of these data memebers
   //
   unsigned long _mtt_startupDelay;
   unsigned long _mtt_oldLowerAccTotal;
   unsigned long _mtt_oldUpperAccTotal;
   unsigned long _mtt_accInterval;
   rawTime       _mtt_lastCheckTime;
   bool          _mtt_skipTimeCheck;

   //
   // keep history of last few scans to help debug timing errors
   //
   enum { TimingHistorySize = 3 };
   rawTime _lastStartTime[TimingHistorySize];
   rawTime _lastEndTime[TimingHistorySize];
   int     _lastTimeIndex;

   //
   // counter to insure ctl_msg acknowledges a/d errors in a timely manner
   //
   unsigned long _atodErrorAckCounter;
   bool          _loggedPs; // Have we logged the power supply yet?
};

#endif /* ifndef _CTL_DRV_INCLUDE */

/* FORMAT HASH 78cba425cc191e4187ba835815f18c15 */

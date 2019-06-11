/*
 * Copyright (c) 1995, 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      safe_drv.hpp, Focussed System control hardware driver
 *
 * ABSTRACT:   Interface to the control hardware
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS8.DOC
 * Test:             I:\ieee1498\STD8.DOC
 *
 * CLASSES:
 *
 * class debounce - debounce stop and pause switches
 *    debounce() - constructor
 *    ~debounce() - destructor
 *    update() - update debounce data
 *
 * class locks - read and debounce door locks
 *    locks() - constructor
 *    ~locks() - destructor
 *    update() - update status
 *    state() - get current state
 *
 * class commands - message to get order data
 *    commands() - constructor
 *    ~commands() - destructor
 *    notify() - notify function
 *
 * class pumpHall - read pump hall sensors
 *    pumpHall() - constructor
 *    ~pumpHall() - destructor
 *    actuals() - get actuals
 *    update() - update status
 *
 * class status - send safety status
 *    status() - constructor
 *    ~status() - destructor
 *
 * class updateTimer - 10 ms timer message
 *    updateTimer() - constructor
 *    ~updateTimer() - destructor
 *    notify() - notify
 *    sendStatus() - send status message
 */
#ifndef SAFE_DRV_HPP
#define SAFE_DRV_HPP


#include "cent.hpp"
#include "s_ultrasonics.hpp"
#include "valve.hpp"
#include "shw.hpp"
#include "basindata.hpp"
#include "auxclock.h"
#include "ostime.hpp"
#include "fastpack.hpp"
#include "safety_warm_ram.h"


// #define DRIVER_TESTING

// Note: Safety's FAST_DATA is not required for production builds.
// FAST_DATA=0 disables collection of Hall sensor time data for all pumps.
// If enabled, then the tasks for TTID_SafeFastMsgLoad and TTID_SafeFastMsg must
// also be enabled in trima_tasks.h in order startup the tasks that publish the
// collected data.
#define FAST_DATA 0

#ifdef DRIVER_TESTING
enum DRV_TEST_STATE
{
   NOT_TESTED  = 0,
   IN_PROGRESS = 1,
   TESTED      = 2
};
#endif

// classes

// SPECIFICATION:    debounce pause and stop switches, update() will only
//                   change states when a number of readings in a row match.
//
// ERROR HANDLING:   none.

class debounce
{
public:
   debounce(HWSwitch select, HW_SWITCH_STATES& sw);
   virtual ~debounce();

   void update (void);                          // update

private:
   HWSwitch          _select;                // switch to be monitored
   HW_SWITCH_STATES& _debouncedValue;        // debounced status
   unsigned short    _count;                 // debounce counter
};

// SPECIFICATION:    debounce door locks
//
// ERROR HANDLING:   none.

class locks
{
public:
   locks(void);
   virtual ~locks();

   void           update ();                 // update status
   HW_DOOR_STATES state ();                  // get current state

private:
   HW_DOOR_STATES _now;                      // current state
   short          _count;                    // timer
};

// SPECIFICATION:    RequestSafetyDriverStatus message
//
// ERROR HANDLING:   none.

class requestDriverStatus
{
public:
   requestDriverStatus();
   virtual ~requestDriverStatus();
   bool newRequest ();

private:
   RS* _RSPtr;
   int _requestCounter;
};
// SPECIFICATION:    derived message class, used to get order info
//
// ERROR HANDLING:   none.

class commands
{
public:
   commands(unsigned char& powerFailFlag);
   virtual ~commands();

   void update ();                         // update function
   void commandMode (unsigned char& mode,  // allows override of air2donor
                     HW_ORDERS& donorMode,
                     HW_ORDERS& serviceMode);
private:
   bool getNewOrder ();
   SHO*           _SHOPtr;
   HW_ORDERS      _serviceMode;           // have received service mode enable
   HW_ORDERS      _donorMode;             // have received donor connect mode enable
   unsigned long  _myNID;
   unsigned long  _sendingNID;
   unsigned char& _powerFailFlag;
   readValve      _cassette;
};

// SPECIFICATION:    pump hall class, this class is used to read hall
//                   sensors and computes rpm and revolutions
//
// ERROR HANDLING:   none.

class pumpHall
{
public:
   pumpHall(HWPump select);
   virtual ~pumpHall();

   void actuals (long& rpm, long& revs);        // get actuals
   void actualRPM (long& rpm);                  // get RPM and don't reset revs
   void lastRPM (long& rpm);                    // get lastRPM (valid for return pump only)
   void update (void);                          // update status

protected:
   HWPump        _select;                       // pump to be monitored
   long          _rpm;                          // actual rpm
   long          _revs;                         // actual revolutions
   unsigned char _lastHall;                     // last hall sensor state
   long          _lastRPM;                      // calculated rpm from last sample
   int           _timeout;                      // timeout value
   rawTime       _lastHallTime;                 // last hall time
   fastpacklong* _hallTime;
};

#ifdef DRIVER_TESTING
class AirDonorTestCase
{
public:
   AirDonorTestCase(ultrasonics* const sensor);
   ~AirDonorTestCase();

   DRV_TEST_STATE start (bool isReturning);
   DRV_TEST_STATE check (bool isReturning);

   void           reset () { status = NOT_TESTED; }
   DRV_TEST_STATE getStatus () { return status; }

private:
   ultrasonics*   low_level;
   rawTime        testTime;
   DRV_TEST_STATE status;
};
#endif

//
// Create the control version of the centrifugeRPM class
//
class SafetyCentRpm
   : public centrifugeRpm
{
protected:
   virtual void clearCommStatus ()
   {
      shw_centClearFault();
   }
};



// SPECIFICATION:    timer class, this class runs at a periodic rate,
//                   set by main(), and controls all hardware by calling
//                   various class members
//
// ERROR HANDLING:   none.

class updateTimer
{
public:
   updateTimer(unsigned long dt,
               commands* c,
               requestDriverStatus* r,
               unsigned char& powerFailFlag);
   virtual ~updateTimer();

   void             notify ();                     // notify
   void             sendStatus (SHW_EVENTS event); // send status message
   void             petTheHardwareDog (void);      // service the hardware watchdog
   HWWatchDogStatus safetyWDStatus (void);         // get the status of the safety FPGA WatchDog
   void             checkTheSoftwareDog (void);    // check the software watchdog timer
   unsigned long _lastPetCounter;                  // software watchdog counter
   rawTime       _lastPetTime;                     // last time of petting software watchdog

private:
   updateTimer();             // default ctr disabled
   updateTimer(updateTimer const&);
   updateTimer& operator = (updateTimer const&);
   void                  requestUpdate ();     // check for status requests
   void                  monitorTiming (void); // check for proper timing information

   SHS* _SHSPtr;

   pumpHall _acPump;                        // pumps
   pumpHall _inletPump;
   pumpHall _plateletPump;
   pumpHall _plasmaPump;
   pumpHall _returnPump;

   unsigned _dt;                             // update interval

   debounce _stop;                          // pushbuttons
   debounce _pause;

   readValve _plasmaValve;                  // valves
   readValve _plateletValve;
   readValve _rbcValve;
   readValve _cassette;                     // cassette

   SafetyCentRpm _centrifuge;               // centrifuge
   ultrasonics   _lowLevel;                 // ultrasonics
   locks         _locks;                    // door lock and latch

   commands*            _commands;           // link to command data
   requestDriverStatus* _request;            // link to driver status request

   getbyte   _tempCapture;                   // captures single-bit line temp data from basin
   basinData _basinData;                     // basin temperature data

   bool    _airToDonorEnable;                // flag enabling air to donor monitoring
   bool    _airToDonor;                      // one-shot indicates air2donor occured
   bool    _returnPumpRunaway;               // one-shot indicates return pump runaway occured
   bool    _acPumpRunaway;                   // one-shot indicates ac pump runaway occured
   rawTime _lastFluidTime;                   // last fluid time
   rawTime _acPrimeTime;                     // used to enable air2donor
   rawTime _dogLastTime;                     // last time of petting hardware watchdog
   rawTime _a2dWaitTime;                     // this does the 20s wait to see if microair settles out (restarts 24v)
   bool    _softDogBite;                     // Temporary flag

   bool _newMxInstalled;                     // New MX Technology or Legacy FPGAs Installed

   HWWatchDogStatus _newWdStatus;            // Safety WatchDog Status New/Old
   HWWatchDogStatus _oldWdStatus;

   rawTime _ShutdownTime;                    // Time when the fatal error was called.  The driver
                                             //  will exit 2 seconds after this time.
   unsigned char& _powerFailFlag;

   //
   // see monitorTiming for usage of these data members
   //
   unsigned long _mt_startupDelay;
   unsigned long _mt_updateCount;
   rawTime       _mt_lastCheckTime;

   //
   // keep history of last few scans to help debug timing errors
   //
   enum { TimingHistorySize = 3 };
   rawTime _lastStartTime[TimingHistorySize];
   rawTime _lastEndTime[TimingHistorySize];
   int     _lastTimeIndex;

   safetyRamSafetyDriverData* _driverData;

   bool _inA2Dwait;
   int  _a2dRestartsInDDC;                       // attempted restarts so we dont jerk the system around

#ifdef DRIVER_TESTING
   AirDonorTestCase  mytestCase;
   AirDonorTestCase* testCase;
#endif
};


#endif /* ifndef SAFE_DRV_HPP */

/* FORMAT HASH a635fdce90796670d1db47c7eed55e23 */

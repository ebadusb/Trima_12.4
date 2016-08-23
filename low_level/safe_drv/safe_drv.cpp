/*
 * Copyright (c) 1995, 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: H:/BCT_Development/Trima5.R/Trima/low_level/safe_drv/rcs/safe_drv.cpp 1.147 2008/08/26 18:46:45Z spriusb Exp spriusb $
 *
 * TITLE:      safe_drv.cpp, Focussed System safety hardware driver
 *
 * ABSTRACT:   Interface to the safety hardware.  This program reads the Valves, Pumps and
 *             other sensors.  It also turns on and off the power supplies.
 *             The driver accepts commands and issues status messages upon request.
 *
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS8.DOC
 * Test:             I:\ieee1498\STD8.DOC
 *
 * PUBLIC FUNCTIONS:
 * safe_drv() - main entry point
 */

// #includes
#include <vxWorks.h>
#include <tickLib.h>
#include <signal.h>


#include "safe_drv.hpp"
#include "sh_mem.h"
#include "safe_hw_private.h"
#include "trima_assert.h"
#include "trima_datalog.h"
#include "error.h"
#include "crcgen.h"
#include "pfw_int.h"
#include "optionparser.h"
#include "failure_debug.h"
#include "states.h"

// public data


// forward reference

static void valveLedTest ();      // valve led test

// local constants

static const short DEBOUNCE_COUNT = 10;               // debounce count
#define MS 1000
#define US 1000000
#define PRIORITY 27
static const int RETURN_SWITCH_TIME = 250;                        // return pump switch time, ms
static const int RPM_CONV           = 60 * 1000;
#define STR_LEN 80
#define VALVE_TEST_COUNT (60 * 2)                       // once a minute
#define VALVE_MASK1 (0xff)
#define VALVE_MASK2 (0x07)
#define ONE_TO_ONE_TOLERANCE 5.0                      // if two pumps are within this
                                                      // tolerance of each other we
                                                      // will call them equal
#define FORCE_MONITOR_MODE 1
#define HARDWARE_MODE 3
#define FATAL_MODE 4

#define RETURN_PUMP_SPEED_LIMIT 165                  // RPM
#define AC_PUMP_SPEED_LIMIT 120                      // RPM

// local data

static SHwOrders        _orderData;                                      // order data
static SHwStates        _statusData;                                     // status data
static HW_SWITCH_STATES oldPauseSwitch             = HW_SWITCH_DISABLED; // switch state
static HW_SWITCH_STATES oldStopSwitch              = HW_SWITCH_DISABLED; // switch state
static SHW_DIRECTION    oldReturnDirection         = SHW_RETURN;         // old return pump direction

static bool             wasAir                     = true; // last us reading
static const int        AC_PRIME_IN_PROGRESS_TIME  = 3000; // ms for which AC prime must persist before
                                                           // triggering air-to-donor monitoring
static const int        MAX_DOG_LIMIT              = 50;   // if 10 ms loop exceeds 50 ms, log it
static const int        MIN_DOG_LIMIT              =  5;   // if 10 ms loop is less than 5 ms, ignore it

static const int        SOFT_WATCHDOG_WARNING_TIME = 1000;

static const int        MAX_SOFT_WATCHDOG_TIME     = 8000; // soft watchdog timer expires in 8 seconds

static bool             shutdownInProgress         = false;
static bool             safeState                  = false;

static rawTime          shutdownTime               = {0, 0};

#ifdef DRIVER_TESTING
static DRV_TEST_STATE testPoint           = NOT_TESTED;

static const long     EMPTY_RES_TEST_TIME = 30000;
#endif

inline SHW_DIRECTION returnDraw ()
{
   SHW_DIRECTION result;

   switch (shw_pumpDirection(hw_returnPump))
   {
      case shw_pumpDraw :
         result = SHW_DRAW;
         break;

      default :
         result = SHW_RETURN;
         break;
   }

   return result;
}

//
// SPECIFICATION:    FATAL_ERROR_HANDLER - perform safety-driver-specific processing of a FATAL ERROR condition
//
static void FATAL_ERROR_HANDLER (void* arg)
{
   if (!safeState)
   {

      // go to safe state
      hw_safeState();

      safeState = true;
   }

   // start a timer as a backup, in case the system is too unstable for the normal
   // fatal error handling to shut us down.
   shutdownInProgress = true;
   osTime::snapshotRawTime(shutdownTime);
}

// SPECIFICATION:    switch debounce - constructor
//
// ERROR HANDLING:   none.

debounce::debounce(HWSwitch select, HW_SWITCH_STATES& sw)
   : _select(select),
     _debouncedValue(sw),
     _count(0)
{
   _debouncedValue = HW_SWITCH_DISABLED;
}

// SPECIFICATION:    switch debounce - destructor
//
// ERROR HANDLING:   none.

debounce::~debounce()
{}

// SPECIFICATION:    switch debounce - update, process data, when a number in a row
//                   match, change states and transmit status message.
//
// ERROR HANDLING:   none.

void debounce::update (void)
{
   HW_SWITCH_STATES newValue = ( hw_switchGetStatus(_select) & hw_switchPressed) ?
                               HW_SWITCH_ENABLED : HW_SWITCH_DISABLED;

   if (_debouncedValue != newValue)
   {
      _count++;                                 // debounce count
      if (_count >= DEBOUNCE_COUNT)             // enough counts?
      {
         _debouncedValue = newValue;            // change state
         _count          = 0;                   // reset count
      }
   }
   else
   {
      _count = 0;                               // clear count
   }
}


// SPECIFICATION:    Safety hardware commands, constructor
//
// ERROR HANDLING:   none.

commands::commands(unsigned char& powerFailFlag)
   : _powerFailFlag(powerFailFlag),
     _serviceMode(HW_DISABLE),
     _donorMode(HW_DISABLE)
{
   // create shared memory for communication to the safety message task
   _SHOPtr = (SHO*)shmem_create("safetyOrder", sizeof(SHO));
   if (!_SHOPtr)
      _FATAL_ERROR(__FILE__, __LINE__, "safetyOrder shmem_create failed");

   // initialize the order area
   memset(_SHOPtr, 0, sizeof(SHO));

   // initialize internals
   _myNID      = 0UL; // New message subsystem returns this for messages originated on the same node.
   _sendingNID = 0UL; // Initially

}

// SPECIFICATION:    Safety hardware commands, destructor
//
// ERROR HANDLING:   none.

commands::~commands()
{}


bool commands::getNewOrder ()
{
   bool result = false;

   if (_SHOPtr->NextOut != _SHOPtr->NextIn)
   {
      // new orders are in
      // check the crc
      unsigned long checksum = 0;
      if (crcgen32(&checksum, (const unsigned char*)&_SHOPtr->msg[_SHOPtr->NextOut].orders,
                   (sizeof(SHwOrders) + sizeof(unsigned long) + sizeof(unsigned long) + sizeof(struct timespec))) != 0)
         _FATAL_ERROR(__FILE__, __LINE__, "CRC generation error");
      else
      {
         if (checksum == _SHOPtr->msg[_SHOPtr->NextOut].crc)
         {
            memcpy(&_orderData, &_SHOPtr->msg[_SHOPtr->NextOut], sizeof(SHwOrders));
            _sendingNID = _SHOPtr->msg[_SHOPtr->NextOut].nid;
            result      = true;
         }
         else
         {
            DataLog(log_level_critical) << "Order CRC mismatch Expected: 0x"
                                        << hex << checksum << dec << ", Received: 0x" << hex
                                        << _SHOPtr->msg[_SHOPtr->NextOut].crc << dec << endmsg;

            _FATAL_ERROR(__FILE__, __LINE__, "Order CRC mismatch");
         }
      }

      if (_SHOPtr->NextOut == NUM_SHWORDERS - 1)
         _SHOPtr->NextOut = 0;
      else
         _SHOPtr->NextOut++;
   }
   return ( result );
}


//
// This function looks for new orders from Safe_Exec.  Note that Safety works differently than Control.
// Safety does not send periodic updates, only on events so no periodic monitoring of Safety is necessary.
//
void commands::update ()
{
   static bool solenoidPowerOn       = false;
   static bool twentyFourVoltPowerOn = false;

   if (getNewOrder())
   {
      unsigned int powerMask = 0;

      // enable 24 volts
      if (_orderData.pumpPower == HW_ENABLE)
      {
         if (!twentyFourVoltPowerOn)
         {
            DataLog_Default << "24V power enabled" << endmsg;
            twentyFourVoltPowerOn = true;
         }

         powerMask |= shw_pumpPower;
      }
      else
      {
         if (twentyFourVoltPowerOn)
         {
            DataLog_Default << "24V power disabled" << endmsg;
            twentyFourVoltPowerOn = false;
         }
      }

      // enable 64 volts
      if (_orderData.centrifugePower == HW_ENABLE)
      {
         powerMask |= shw_centPower;
      }

      // enable led power
      if (_orderData.valveLED == HW_ENABLE)
      {
         powerMask |= shw_ledPower;
      }

      // enable door solenoid
      if (_orderData.doorSolenoidPower == HW_ENABLE)
      {
         powerMask |= shw_doorPower;

         if (!solenoidPowerOn)
         {
            DataLog_Default << "Solenoid Power enabled" << endmsg;
            solenoidPowerOn = true;
         }
      }
      else
      {
         if (solenoidPowerOn)
         {
            DataLog_Default << "Solenoid Power disabled" << endmsg;
            solenoidPowerOn = false;
         }
      }

      shw_powerSet(powerMask);

      // alarm light
      hw_alarmSetCommand( (_orderData.alarmLight == HW_ENABLE) ? hw_alarmOn : hw_alarmOff);

      // are we in service mode?
      if (_orderData.serviceEnableMode == HW_ENABLE)
      {
         // check for the sender.  If it came from any one running on the safety
         // computer, we will fatal out under the assumption that the safety
         // executive (or some other devious program) is trying to trick us.

         if (_myNID == _sendingNID)
            _FATAL_ERROR(__FILE__, __LINE__, "Service request received from safety node");
         else
            _serviceMode = HW_ENABLE;
      }
      else
         _serviceMode = HW_DISABLE;

      // are we in donor connected mode?
      if (_orderData.donorConnectMode == HW_ENABLE)
         _donorMode = HW_ENABLE;
      else
         _donorMode = HW_DISABLE;

   }
}

// SPECIFICATION:    Reveals service mode and donor mode
//
// ERROR HANDLING:   none.

void commands::commandMode (unsigned char& mode,
                            HW_ORDERS& donorMode,
                            HW_ORDERS& serviceMode)
{
   if ( (_serviceMode == HW_DISABLE) &&
        (_donorMode == HW_ENABLE) )
   {
      mode = FORCE_MONITOR_MODE;
   }
   else if (_donorMode == HW_DISABLE)
   {
      mode = HARDWARE_MODE;
   }
   else
   {
      mode = FATAL_MODE;
   }

   donorMode   = _donorMode;
   serviceMode = _serviceMode;
}


// SPECIFICATION:    door lock debounce constructor
//
// ERROR HANDLING:   none.

locks::locks(void)
   : _now(HW_DOOR_OPEN),
     _count(0)
{}

// SPECIFICATION:    door lock debounce destructor
//
// ERROR HANDLING:   none.

locks::~locks()
{}

// SPECIFICATION:    door lock debounce update, debounces switches
//
// ERROR HANDLING:   none.

void locks::update ()
{
   unsigned short doorStatus = hw_doorGetStatus();
   HW_DOOR_STATES doorLocked = (doorStatus & hw_doorLocked) ? HW_DOOR_LOCKED_STATE : HW_DOOR_OPEN;
   HW_DOOR_STATES doorClosed = (doorStatus & hw_doorClosed) ? HW_DOOR_CLOSED_STATE : HW_DOOR_OPEN;

   // test door states
   HW_DOOR_STATES data = HW_DOOR_OPEN;
   if ((doorLocked == HW_DOOR_LOCKED_STATE)
       &&(doorClosed == HW_DOOR_CLOSED_STATE))
      data = HW_DOOR_CLOSED_AND_LOCKED;
   else if (doorLocked == HW_DOOR_LOCKED_STATE)
      data = doorLocked;
   else if (doorClosed == HW_DOOR_CLOSED_STATE)
      data = doorClosed;

   if (_now != data)                      // different?
   {
      _count++;                           // debounce count
      if (_count >= DEBOUNCE_COUNT)       // enough counts?
      {
         _now   = data;                   // change state
         _count = 0;                      // reset count
      }
   }
   else
   {
      _count = 0;                         // clear count
   }
}

// SPECIFICATION:    door lock debounce report status
//
// ERROR HANDLING:   none.

HW_DOOR_STATES locks::state ()
{
   return _now;
}

// SPECIFICATION:    RequestStatus, constructor
//
// ERROR HANDLING:   none.

requestDriverStatus::requestDriverStatus()
{
   // create shared memory for communication to the safety message task
   _RSPtr = (RS*)shmem_create("requestStatus", sizeof(RS));

   if (_RSPtr == (RS*)NULL)
      _FATAL_ERROR(__FILE__, __LINE__, "requestStatus shmem_create failed");

   // initialize shared memory
   memset(_RSPtr, 0, sizeof(RS));

   // initialize counter
   _requestCounter = 0;
}

// SPECIFICATION:    RequestSStatus, destructor
//
// ERROR HANDLING:   none.

requestDriverStatus::~requestDriverStatus()
{}

// SPECIFICATION:    RequestStatus message, check for new request
//
// ERROR HANDLING:   none.

bool requestDriverStatus::newRequest ()
{
   bool result = false;

   if (_RSPtr->NextOut != _RSPtr->NextIn)
   {
      if (_RSPtr->counter[_RSPtr->NextOut] != ++_requestCounter)
      {
         DataLog(log_level_critical) << "requestDriverStatus count mismatch Expected:"
                                     << _requestCounter << " Received " << _RSPtr->counter[_RSPtr->NextOut] << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "requestDriverStatus count mismatch");
      }
      else
      {
         // reset to avoid wrap
         if (_requestCounter == 0xfffffff)
            _requestCounter = 0;
         result = true;
      }

      if (_RSPtr->NextOut == NUM_SREQUEST - 1)
         _RSPtr->NextOut = 0;
      else
         _RSPtr->NextOut++;
   }

   return(result);
}



static void signal_handler (int sig)
{
   shutdownInProgress = true;
   osTime::snapshotRawTime(shutdownTime);
}

static void readback_failed (const char* file,       // source file name where error was detected
                             int line,               // source file line where error was detected
                             const char* type,       // type of readback (e.g. "byte", "word")
                             unsigned short port,    // I/O port address
                             unsigned short value,   // value read back from I/O port
                             unsigned short expected // value expected to be read back
                             )
{

   DataLog(log_level_critical) << "I/O " << type << " readback failed: port "
                               << hex << port << " value " << value << " expected " << expected << dec << endmsg;
}

// SPECIFICATION:    main entry point
//                   parameter 1 = local queue name
//                   2 = remote node number
//                   3 = remote queue name
//
// ERROR HANDLING:   none.

void safe_drv (const char* options)
{
   int           current;
   unsigned char powerFailFlag = 0x00;

   //
   // Make sure the driver specific fatal error handler hook is in place
   // before any other code is executed.
   //
   _CHAIN_FATAL_ERROR_HANDLER(FATAL_ERROR_HANDLER, NULL);

   //
   // Use SIGQUIT to initiate shutdown of the safety driver
   //
   signal(SIGQUIT, signal_handler);


   // Initialize default log level
   datalog_SetDefaultLevel(log_handle_safe_drv_info);

   // Initialize the hardware driver
   hw_init();
   hw_setReadbackFailedFunc(&readback_failed);

   // parse the command line - format is: "[-In] [-R]"
   //    -In specifies that interrupt vector n be used for the power fail interrupt
   //    -R specifies that power fail ride-thru should be enabled
   //
   // by default, interrupt vector 6 is used, and ride-thru is disabled
   //
   const char* separators  = " \t";
   int         optIndex    = strspn(options, separators);

   bool        pfwRidethru = false;

   DataLog_Default << "safe_drv started with command line: \"" << options << "\"" << endmsg;
   while (optIndex < strlen(options))
   {
      if (options[optIndex] == '-')
      {
         // parse command line option
         if (strncmp(&options[optIndex], "-R", 2) == 0)
         {
            pfwRidethru = true;
         }
         else
         {
            _FATAL_ERROR(__FILE__, __LINE__, "illegal command line option");
         }
      }

      // go to beginning of next command line argument
      optIndex += strcspn(&options[optIndex], separators);
      optIndex += strspn(&options[optIndex], separators);
   }

   if (pfwRidethru)
   {
      hw_powerFailEnableRideThru();
   }
   else
   {
      hw_powerFailDisableRideThru();
   }

   pfw_attach(&powerFailFlag);

   // clear old values
   memset(&_orderData, 0, sizeof( _orderData));
   memset(&_statusData, 0, sizeof( _statusData));
   _statusData.valveLedTest = 1;          // start as OK

   commands            c(powerFailFlag);

   requestDriverStatus driverStatusMsg;

   hw_safeState();

   updateTimer t(10ul, &c, &driverStatusMsg, powerFailFlag);

   // Only perform this check if the New MX Safety FPGA is installed
   if (hw_mxFpgaInstalled())
   {
      t.safetyWDStatus();
   }

   // setup timer notifications and handle periodic update
   SEM_ID tmrSem = auxClockSemaphoreAttach(10000, AuxClockCountingSemaphore);
   if (!tmrSem)
   {
      _FATAL_ERROR(__FILE__, __LINE__, "auxClockSemaphoreAttach failed");
   }

   while (!shutdownInProgress)
   {
      semTake(tmrSem, WAIT_FOREVER);
      t.notify();
   }

   // If Trima is not in the safe state, set it
   if (!safeState)
   {
      // go to safe state
      hw_safeState();

      safeState = true;
   }

   DataLog(log_level_critical) << "Safety: shutdown started - hardware in safe state" << endmsg;

   // If the New MX Version of the Saftey FPGA is installed, wait for
   // the WatchDog to Reset the Processor in 5 Seconds
   if (hw_mxFpgaInstalled())
   {
      int count = 5 * sysClkRateGet();

      // Wait for the WatchDog Circuit to Reset the Safety Processor in
      // 5 Seconds
      while (count--)
      {
         t.safetyWDStatus();

         taskDelay(1);
      }
   }
   else
   {  // Otherwise, use the Legacy Method

#ifndef DEBUG_EXTEND_SHUTDOWN

      // hold here for five seconds to allow shutdown (especially disk
      // synchronization) to complete
      int count = 5 * sysClkRateGet();

#else /* ifndef DEBUG_EXTEND_SHUTDOWN */

      int count = DEBUG_EXTEND_SHUTDOWN * sysClkRateGet();

#endif /* ifndef DEBUG_EXTEND_SHUTDOWN */

      while (count--)
      {
         t.petTheHardwareDog();

         taskDelay(1);
      }
   }
}

HWWatchDogStatus updateTimer::safetyWDStatus (void)
{
   datalog_SetDefaultLevel(log_handle_safe_drv_info);

   _newWdStatus = hw_watchDogStatus();

   if (_newWdStatus != _oldWdStatus)
   {
      switch (_newWdStatus)
      {
         case hw_noFailure :
            DataLog_Default << "Safety WatchDog Status: No Failures\n" << endmsg;
            break;

         case hw_wrongA5Byte :
            DataLog_Default << "Safety WatchDog Status: Reset Due To Wrong A5 Byte Written\n" << endmsg;
            break;

         case hw_wrongF0Byte :
            DataLog_Default << "Safety WatchDog Status: Reset Due To Wrong F0 Byte Written\n" << endmsg;
            break;

         case hw_a5WriteWaitingF0 :
            DataLog_Default << "Safety WatchDog Status: Reset Due To A5 Byte Written While Waiting For F0 Byte\n" << endmsg;
            break;

         case hw_f0WriteWaitingA5 :
            DataLog_Default << "Safety WatchDog Status: Reset Due To F0 Byte Written While Waiting For A5 Byte\n" << endmsg;
            break;

         case hw_wdTimeOut :
            DataLog_Default << "Safety WatchDog Status: Reset Due To WatchDog Time-Out\n" << endmsg;
            break;

         case hw_powerUp :
            DataLog_Default << "Safety WatchDog Status: Normal After a Power-Up Or Power-Fail\n" << endmsg;
            break;

         default :
            DataLog_Default << "Safety WatchDog Status:  UNKNOWN\n" << endmsg;
            break;
      }

      _oldWdStatus = _newWdStatus;
   }

   return _newWdStatus;
}


// SPECIFICATION:    pump constructor
//
// ERROR HANDLING:   none.

pumpHall::pumpHall(HWPump select)
   : _select(select),
     _rpm(0),
     _revs(0),
     _lastRPM(0),
     _hallTime(0),
     _timeout(0) // ,
{
   _lastHall = shw_hallGetStatus(_select) & shw_hallActive;
   osTime::snapshotRawTime(_lastHallTime);

#if FAST_DATA
   switch (_select)
   {
      case hw_inletPump :
         _hallTime = new fastpacklong(INLET_PUMP_HALL_TIME, "inletHallTime");
         break;

      case hw_acPump :
         _hallTime = new fastpacklong(AC_PUMP_HALL_TIME, "acHallTime");
         break;

      case hw_plasmaPump :
         _hallTime = new fastpacklong(PLASMA_PUMP_HALL_TIME, "plasmaHallTime");
         break;

      case hw_plateletPump :
         _hallTime = new fastpacklong(PLATELET_PUMP_HALL_TIME, "plateletHallTime");
         break;

      case hw_returnPump :
         _hallTime = new fastpacklong(RETURN_PUMP_HALL_TIME, "returnHallTime");
         break;

      default :
         DataLog(log_level_critical) << "Illegal choice for pumpHall " << (int)_select << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "illegal pumpHall creation");
         break;
   }
#endif
}

// SPECIFICATION:    pump destructor
//
// ERROR HANDLING:   none.

pumpHall::~pumpHall()
{}

// SPECIFICATION:    pump control get actual values
//
// ERROR HANDLING:   none.

void pumpHall::actuals (long& rpm, long& revs)
{
   rpm   = _rpm;
   revs  = _revs;
   _revs = 0;
}

// SPECIFICATION:    pump control get actual values of RPM
//                   and don't reset
//
// ERROR HANDLING:   none.

void pumpHall::actualRPM (long& rpm)
{
   rpm = _rpm;
}

// SPECIFICATION:    pump control get previous values of RPM
//                   and don't reset
//
// ERROR HANDLING:   none.

void pumpHall::lastRPM (long& rpm)
{
   rpm = _lastRPM;
}


// SPECIFICATION:    pump hall sensor update
//
// ERROR HANDLING:   none.

void pumpHall::update ()
{
   const int dt = osTime::howLongMilliSec(_lastHallTime);

#if FAST_DATA
   _hallTime->dataPoint(dt);            // send data
#endif

   unsigned char state = shw_hallGetStatus(_select);

   // check if different
   if (state != _lastHall)
   {
      // check for reverse
      if (_select == hw_returnPump &&
          returnDraw())
      {
         _revs--;             // backup
      }
      else
      {
         _revs++;             // forward
      }

      // compute rpm if dt non-zero and keep last
      if (dt > 0)
      {
         _lastRPM = _rpm;
         _rpm     = RPM_CONV / dt;
      }

      // compute expected pulse time plus 3 seconds, scale to milliseconds
      if (_rpm != 0)
         _timeout = (60 / _rpm + 3) * MS;
      else
         _timeout = 63 * MS;

      // if reverse, switch rpm sign
      if (_select == hw_returnPump &&
          returnDraw())
      {
         _rpm *= -1;
      }

      // save previous values
      osTime::snapshotRawTime(_lastHallTime);
      _lastHall = state;
   }
   else if (dt > _timeout)             // time out processing
   {
      _rpm     = 0;                    // set speed to 0
      _lastRPM = _rpm;
   }
}

#ifdef DRIVER_TESTING
AirDonorTestCase::AirDonorTestCase(ultrasonics* const sensor)
   : low_level(sensor),
     status(NOT_TESTED)
{
   testTime.nanosec = 0;
   testTime.sec     = 0;
}

DRV_TEST_STATE AirDonorTestCase::start (bool isReturning)
{
   if ((status == NOT_TESTED) && isReturning)
   {
      osTime::snapshotRawTime(testTime);
      status = IN_PROGRESS;
   }

   return status;
}

DRV_TEST_STATE AirDonorTestCase::check (bool isReturning)
{
   if (status == IN_PROGRESS)
   {
      if ((osTime::howLongMilliSec(testTime) < EMPTY_RES_TEST_TIME) &&
          isReturning)
      {
         low_level->setSpoofingEmpty(true);
      }
      else
      {
         low_level->setSpoofingEmpty(false);
         testTime.nanosec = 0;
         testTime.sec     = 0;
         status           = TESTED;
      }
   }

   return status;
}
#endif



// SPECIFICATION:    timer constructor, creates hardware objects
//
// ERROR HANDLING:   none.

updateTimer::updateTimer(unsigned long dt,
                         commands* c,
                         requestDriverStatus* r,
                         unsigned char& powerFailFlag)
   : _dt(dt),
     _commands(c),
     _request(r),
     _airToDonorEnable(false),
     _airToDonor(false),
     _returnPumpRunaway(false),
     _acPumpRunaway(false),
     _powerFailFlag(powerFailFlag),
     _mt_startupDelay(1000),
     _mt_updateCount(0),
     _newWdStatus(hw_unknown),
     _oldWdStatus(hw_unknown),
     _lastTimeIndex(0),
     _acPump(hw_acPump),
     _inletPump(hw_inletPump),
     _plateletPump(hw_plateletPump),
     _plasmaPump(hw_plasmaPump),
     _returnPump(hw_returnPump),
     _pause(hw_pauseSwitch, _statusData.pauseSwitch),
     _stop(hw_stopSwitch, _statusData.stopSwitch),
     _plasmaValve(hw_plasmaValve),
     _plateletValve(hw_plateletValve),
     _rbcValve(hw_rbcValve),
     _cassette(hw_cassette),
     _lowLevel(2),
     _centrifuge(),
     _tempCapture(shw_basinDataSource, 65.535),
     _basinData(&_tempCapture),
     _driverData(NULL)
{
   trima_assert(_commands);

   _driverData = safetyRamDriverData();

   // Initialize the sentinel value
   _driverData->sentinel = 0xBABECAFE;

   _softDogBite          = false;

   // Are the updated MX FPGAs Installed?
   if (hw_mxFpgaInstalled())
   {
      _newMxInstalled = true;
   }
   else
   {
      _newMxInstalled = false;
   }

   // create shared memory for communication to the safety message task
   _SHSPtr = (SHS*)shmem_create("safetyStatus", sizeof(SHS));
   if (!_SHSPtr)
      _FATAL_ERROR(__FILE__, __LINE__, "safetyStatus shmem_create failed");

   // initialize the status area
   memset(_SHSPtr, 0, sizeof(SHS));

   // initialize software watchdog timer
   _lastPetCounter = 0;
   osTime::snapshotRawTime(_lastPetTime);

   // initialize for hardware watchdog timer
   osTime::snapshotRawTime(_dogLastTime);

   _mt_lastCheckTime.sec     = 0;
   _mt_lastCheckTime.nanosec = 0;

   _lastFluidTime.sec        = 0;
   _lastFluidTime.nanosec    = 0;

   // initialize timer to look for AC Prime for air2donor monitor
   _acPrimeTime.sec     = 0;
   _acPrimeTime.nanosec = 0;

   // Initialize the time keepers.
   memset(&_lastStartTime, 0, sizeof(_lastStartTime));
   memset(&_lastEndTime, 0, sizeof(_lastEndTime));

#ifdef DRIVER_TESTING
   testCase = &mytestCase;
#endif
}


// SPECIFICATION:    timer destructor, deletes hardware objects
//
// ERROR HANDLING:   none.

updateTimer::~updateTimer()
{}


// SPECIFICATION:    timer notify(), processes hardware
//
// ERROR HANDLING:   none.

void updateTimer::notify ()
{
   long          acRPM;
   long          aclastRPM;
   long          inletRPM;
   long          plasmaRPM;
   long          plateletRPM;
   long          returnRPM;
   long          returnlastRPM;

   unsigned char mode;
   bool          acPrime = false;

   osTime::snapshotRawTime(_lastStartTime[_lastTimeIndex]);

   _driverData->here = 1;   // breadcrumb

   monitorTiming();

   _driverData->here = 2;   // breadcrumb

   //
   // Check for shutdown ...
   //
   if (shutdownInProgress &&
       osTime::howLongMilliSec(shutdownTime) > 5000 /*msecs*/)
   {
      abort();
   }

   pfw_timer();

   _driverData->here = 3;   // breadcrumb

   // check for new orders
   _commands->update();

   _driverData->here = 4;   // breadcrumb

   // pump updates
   _acPump.update();
   _inletPump.update();
   _plateletPump.update();
   _plasmaPump.update();
   _returnPump.update();

   _driverData->here = 5;   // breadcrumb

   // switches
   _stop.update();
   _pause.update();
   _locks.update();

   _driverData->here = 6;   // breadcrumb

   // centrifuge
   _centrifuge.update();

   _driverData->here = 7;   // breadcrumb

   // ultrasonics
   _lowLevel.update();

   _driverData->here = 8;   // breadcrumb

// air to donor processing,
// if return pump is pumping towards donor and
// low level sensor indicating air, for
// a time limit, then safe state machine

// enable air to donor processing after cassette down and:
//    AC prime is started
// or fluid is seen at the lower sensor
// or safety executive tells us to do it

#ifdef DRIVER_TESTING
   // BEGIN TEST CODE
   if ((_orderData.test1 >= (long)METERED_STORAGE) &&
       (_orderData.donorConnectMode == HW_DISABLE) &&      // we're disconnected
       (testPoint < TESTED))                               // test not yet run
   {
      if (testPoint == NOT_TESTED)
      {
         if ((testPoint = testCase->start((returnDraw() == SHW_RETURN))) == IN_PROGRESS)
            DataLog_Default << "Safety driver air-to-donor test IN PROGRESS" << endmsg;
      }
      else if (testPoint == IN_PROGRESS)
      {
         if ((testPoint = testCase->check((returnDraw() == SHW_RETURN))) == TESTED)
            DataLog_Default << "Safety driver air-to-donor test COMPLETED" << endmsg;
      }
   }
   // END TEST CODE
#endif

   if (_orderData.donorConnectMode == HW_DISABLE)
   {
      if (_airToDonorEnable)
         DataLog_Default << "DONOR CONNECT MODE turned off" << endmsg;

      _airToDonorEnable = false;
      _airToDonor       = false;
      osTime::snapshotRawTime(_lastFluidTime);

      _driverData->here = 9;   // breadcrumb
   }

   //
   //  Note: this code no longer looks at cassette position to determine if the donor is
   //  connected (see IT8266).  It is acceptable to use donor connect mode to turn on
   //  air-to-donor monitoring per that safety code review.  If control stops sending state
   //  information then the system will alarm when fluid is first seen at the low level sensor, as
   //  safety ensures that condition can only happen during blood prime.
   //
   //  This block of code does however log Safety state information used by QA.
   //
   else
   {
      // get the donor and service mode from the commands
      _commands->commandMode(mode,
                             _statusData.donorConnectMode,
                             _statusData.serviceEnableMode);

      _driverData->here = 10;   // breadcrumb

      // check for pumps indicating AC Prime
      if (!_airToDonorEnable)
      {
         // what's the current speed?
         _acPump.actualRPM(acRPM);
         _inletPump.actualRPM(inletRPM);
         _plasmaPump.actualRPM(plasmaRPM);
         _plateletPump.actualRPM(plateletRPM);
         _returnPump.actualRPM(returnRPM);

         _driverData->here = 11;   // breadcrumb

         if ((plasmaRPM == 0)                              // not running
             &&(plateletRPM == 0)                          // & not running
             &&(returnRPM == 0)                            // & not running
             &&(inletRPM >= 40 )                           // & running
             &&(acRPM >= 40))                              // & running
         {
            _driverData->here = 12;   // breadcrumb

            if ((_acPrimeTime.nanosec == 0)
                &&(_acPrimeTime.sec == 0))
            {
               // start timing to see if this is really AC Prime
               osTime::snapshotRawTime(_acPrimeTime);
               DataLog_Default << "Look for AC Prime." << endmsg;
            }
            else
            {
               const int dt = osTime::howLongMilliSec(_acPrimeTime);

               if (dt < 0)
               {
                  osTime::snapshotRawTime(_acPrimeTime);
                  DataLog_Default << "Look for AC Prime." << endmsg;
               }
               // it sure looks like AC Prime
               if (dt > AC_PRIME_IN_PROGRESS_TIME)
               {
                  acPrime = true;
                  DataLog_Default << dt << " AC Prime detected." << endmsg;
               }
            }
         }
         else
         {
            _acPrimeTime.sec     = 0;                          // not AC Prime
            _acPrimeTime.nanosec = 0;

            _driverData->here    = 13; // breadcrumb
         }
      }

      // check for disposable test running
      if (!_airToDonorEnable &&
          ( acPrime || _lowLevel.isFluid() ) )                   // & in AC Prime or fluid at lower
      {
         DataLog_Default << "DONOR CONNECT MODE turned on" << endmsg;

         _airToDonorEnable = true;
         DataLog(log_level_qa_external) << "Begin Air monitor" << endmsg;
      }

      _driverData->here = 14;   // breadcrumb

      // evaluate donor mode to decide if air2donor should be active
      if (!_airToDonorEnable && (mode == FORCE_MONITOR_MODE))
      {
         DataLog_Default << "DONOR CONNECT MODE turned on" << endmsg;

         _airToDonorEnable = true;
         DataLog(log_level_qa_external) << "Safety exec forces Air monitor" << endmsg;
      }
      else if (mode == FATAL_MODE)
      {
         sendStatus(SHW_AIR_EVENT);                 // send event
         _FATAL_ERROR(__FILE__, __LINE__, "Shutdown for Illegal service mode");
      }
   }

   _driverData->here = 15;   // breadcrumb

   // Log sensor history upon state change
   if ( (_lowLevel.isFluid() && wasAir) ||
        (!_lowLevel.isFluid() && !wasAir) )
   {
      _lowLevel.logHistory();
   }

   _driverData->here = 16;   // breadcrumb

   // reset flags for next cycle

   if (_lowLevel.isFluid() && wasAir)
   {
      const int  dt = osTime::howLongMilliSec(_lastFluidTime);
      ultraStats stats;

      _driverData->here = 17;   // breadcrumb

      DataLog_Default << "Air timer = " << dt << " mS" << endmsg;
      _lowLevel.getStats(&stats);

      _driverData->here = 18;   // breadcrumb

      DataLog_Default << "Ultrasonic statistics: totalMin " << stats.totalMin
                      << ", totalMax " << stats.totalMax << ", totalAve " << stats.averageTotal << endmsg;

      _driverData->here = 19;   // breadcrumb
   }

   _driverData->here = 20;   // breadcrumb

   wasAir            = !_lowLevel.isFluid();

   _driverData->here = 21;   // breadcrumb

   if (_lowLevel.isFluid() ||                    // low level found
       returnDraw()        ||                    // pump in draw
       (!(shw_powerStatus() & shw_pumpPower)) || // pump not powered
       !_airToDonorEnable)                       // not enabled
   {
      osTime::snapshotRawTime(_lastFluidTime);
      _airToDonor = false;
      _statusData.returnPumpDirChgTime = 0;

      _driverData->here                = 22; // breadcrumb
   }

   // air to donor checks

   if ( _airToDonorEnable &&                        // is enabled
        !returnDraw()     &&                        // return &&
        (shw_powerStatus() & shw_pumpPower) &&      // power enabled
        !_lowLevel.isFluid()                        // no fluid
        )
   {
      const int dt = osTime::howLongMilliSec(_lastFluidTime);

      _driverData->here = 23;   // breadcrumb

      // time test, this will put machine in safe state

      if (dt > RETURN_SWITCH_TIME)
      {
         shw_powerSet(shw_ledPower);

         _driverData->here = 24;   // breadcrumb

         if (!_airToDonor)
         {
            _airToDonor = true;
            sendStatus(SHW_AIR_EVENT);                    // send event

            DataLog_Default << "Shutdown Air Timeout = " << dt << " ms" << endmsg;
            _lowLevel.logHistory();

            _driverData->here = 25;   // breadcrumb
         }
      }
   }

   // test ac and return pump speeds when the donor is enabled
   // which is the same time period as when air2donor is enabled
   _returnPump.actualRPM(returnRPM);
   _returnPump.lastRPM(returnlastRPM);

   _driverData->here = 26;   // breadcrumb

   if ( _airToDonorEnable &&
        ( returnRPM > RETURN_PUMP_SPEED_LIMIT) &&
        ( returnlastRPM > RETURN_PUMP_SPEED_LIMIT)
        )
   {
      shw_powerSet(shw_ledPower);

      _driverData->here = 27;   // breadcrumb

      if (!_returnPumpRunaway)
      {
         _returnPumpRunaway = true;
         sendStatus(SHW_RETURN_PUMP_TOO_FAST_EVENT);   // send event
         DataLog(log_level_safety_alarm_detail) << "Shutdown for Return Pump Too Fast = " << returnRPM << endmsg;
      }
   }

   _driverData->here = 28;   // breadcrumb

   _acPump.actualRPM(acRPM);
   _acPump.lastRPM(aclastRPM);

   _driverData->here = 29;   // breadcrumb

   if ( _airToDonorEnable &&
        ( acRPM > AC_PUMP_SPEED_LIMIT) &&
        ( aclastRPM > AC_PUMP_SPEED_LIMIT)
        )
   {
      _driverData->here = 30;   // breadcrumb

      shw_powerSet(shw_ledPower);

      if (!_acPumpRunaway)
      {
         _acPumpRunaway = true;
         sendStatus(SHW_AC_PUMP_TOO_FAST_EVENT);   // send event
         DataLog(log_level_safety_alarm_detail) << "Shutdown for AC Pump Too Fast = " << acRPM << endmsg;

         _driverData->here = 31;   // breadcrumb
      }
   }

   _driverData->here = 32;   // breadcrumb

   // service the hardware watchdog
   petTheHardwareDog();

   // Perform the following status check only if the New MX Safety FPGA is installed
   if (_newMxInstalled)
   {
      // Get the Status of the WatchDog
      const HWWatchDogStatus wdStatus = safetyWDStatus();

      _driverData->here = 33;   // breadcrumb

      // Call _FATAL_ERROR if the WatchDog is Going to Cause a Processor Reset
      if ( (wdStatus != hw_noFailure) &&
           (wdStatus != hw_powerUp) )
      {
         _FATAL_ERROR(__FILE__, __LINE__, "Safety: WatchDog Will Reset The Processor In Five Seconds");
      }
   }

   _driverData->here = 34;   // breadcrumb

   // check the software watchdog
   checkTheSoftwareDog();

   _driverData->here = 35;   // breadcrumb

   // event checks
   if (_statusData.pauseSwitch != oldPauseSwitch)
   {
      _driverData->here = 36;   // breadcrumb

      oldPauseSwitch    = _statusData.pauseSwitch;
      if (_statusData.pauseSwitch == HW_SWITCH_ENABLED)
         sendStatus(SHW_PAUSE_EVENT);
   }

   _driverData->here = 37;   // breadcrumb

   if (_statusData.stopSwitch != oldStopSwitch)
   {
      _driverData->here = 38;   // breadcrumb

      oldStopSwitch     = _statusData.stopSwitch;
      if (_statusData.stopSwitch == HW_SWITCH_ENABLED)
         sendStatus(SHW_STOP_EVENT);
   }

   _driverData->here = 39;   // breadcrumb

   if (_statusData.doorLocks != _locks.state())
   {
      _driverData->here = 40;   // breadcrumb

      sendStatus(SHW_DOOR_EVENT);
   }

   _driverData->here = 41;   // breadcrumb

   const SHW_RESERVOIR_STATES lowLevelSensorState = (_lowLevel.isFluid() ? SHW_RESERVOIR_LOW : SHW_RESERVOIR_EMPTY);

   if ( ( _statusData.reservoir != lowLevelSensorState) &&
        (_statusData.event != SHW_RESERVOIR_EVENT)
        )
   {
      _driverData->here = 42;   // breadcrumb

      sendStatus(SHW_RESERVOIR_EVENT);
   }

   const SHW_DIRECTION dir = returnDraw();

   _driverData->here = 43;   // breadcrumb

   if (oldReturnDirection != dir)
   {
      _driverData->here = 44;   // breadcrumb

      sendStatus(SHW_RETURN_EVENT);
      oldReturnDirection = dir;
   }

   _tempCapture.update();

   _driverData->here = 45;   // breadcrumb

   _basinData.update();

   _driverData->here = 46;   // breadcrumb

   // check for new status request
   requestUpdate();

   _driverData->here = 47;   // breadcrumb

   osTime::snapshotRawTime(_lastEndTime[_lastTimeIndex]);
   _lastTimeIndex = ++_lastTimeIndex % TimingHistorySize;

   // Save off the start and end cycle times
   memcpy(_driverData->lastStartTime, _lastStartTime, sizeof(_lastStartTime));
   memcpy(_driverData->lastEndTime, _lastEndTime, sizeof(_lastEndTime));
   _driverData->lastTimeIndex = _lastTimeIndex;

   _driverData->counter++;
}

// SPECIFICATION:    check for status request
//
// ERROR HANDLING:   none.

void updateTimer::requestUpdate ()
{
   if (_request->newRequest())
   {
      sendStatus(SHW_NORMAL_UPDATE);

      // update the software watchdog
      _lastPetCounter++;
      osTime::snapshotRawTime(_lastPetTime);
   }
}



// SPECIFICATION:    send status message
//
// ERROR HANDLING:   none.
void updateTimer::sendStatus (SHW_EVENTS event)
{
   // pumps
   if (!event)
   {
      _acPump.actuals(_statusData.acRPM, _statusData.acRevs);
      _statusData.acAccumRevs += _statusData.acRevs;

      _inletPump.actuals(_statusData.inletRPM, _statusData.inletRevs);
      _statusData.inletAccumRevs += _statusData.inletRevs;

      _plateletPump.actuals(_statusData.plateletRPM, _statusData.plateletRevs);
      _statusData.plateletAccumRevs += _statusData.plateletRevs;

      _plasmaPump.actuals(_statusData.plasmaRPM, _statusData.plasmaRevs);
      _statusData.plasmaAccumRevs += _statusData.plasmaRevs;

      _returnPump.actuals(_statusData.returnRPM, _statusData.returnRevs);
      _statusData.returnAccumRevs += _statusData.returnRevs;
   }
   else
   {
      _statusData.acRevs       = 0;
      _statusData.inletRevs    = 0;
      _statusData.plateletRevs = 0;
      _statusData.plasmaRevs   = 0;
      _statusData.returnRevs   = 0;
   }

   // valves
   _statusData.plasmaValve      = _plasmaValve.update();
   _statusData.plateletValve    = _plateletValve.update();
   _statusData.rbcValve         = _rbcValve.update();
   _statusData.cassettePosition = _cassette.updateCassette();

   if (!_powerFailFlag)
   {
      valveLedTest();
   }

   // digital
   _statusData.doorLocks = _locks.state();

   const bool powerOK = ((shw_powerStatus() & shw_centPower) && (_statusData.centrifugeRPM > 100)) ? true : false;

   _statusData.centrifugeRPM = _centrifuge.rpm();
   _centrifuge.errorStatus(powerOK, _statusData.centrifugeError);

   // reservoir
   _statusData.reservoir = (_lowLevel.isFluid()) ? SHW_RESERVOIR_LOW : SHW_RESERVOIR_EMPTY;

   // return pump direction
   _statusData.returnDirection = returnDraw();

   // send message
   _statusData.event = event;
   _statusData.msgCount++;                   // bump message count

   // copy the data into shmem
   memcpy(&_SHSPtr->msg[_SHSPtr->NextIn].status, &_statusData, sizeof(SHwStates));

   // generate a crc
   _SHSPtr->msg[_SHSPtr->NextIn].crc = 0;

   if (crcgen32(&_SHSPtr->msg[_SHSPtr->NextIn].crc, (const unsigned char*)&_SHSPtr->msg[_SHSPtr->NextIn].status, sizeof(SHwStates)) != 0)
      _FATAL_ERROR(__FILE__, __LINE__, "CRC generation error");
   else
   {
      // test for lapping NextOut
      if (_SHSPtr->NextIn + 1 == _SHSPtr->NextOut)
         _FATAL_ERROR(__FILE__, __LINE__, "safetyStatus NextIn lapping NextOut!");

      // increment NextIn index
      if (_SHSPtr->NextIn == NUM_SHWSTATUS - 1)
         _SHSPtr->NextIn = 0;
      else
         _SHSPtr->NextIn++;
   }
}

// SPECIFICATION:   petTheHardwareDog -- hardware watchdog service routine
//                  If not serviced within 420 ms, the hardware will
//                  automatically reset.
//
// ERROR HANDLING:   none.

void updateTimer::petTheHardwareDog (void)
{
   int dt = osTime::howLongMilliSec(_dogLastTime);
   if (dt > MIN_DOG_LIMIT)
   {
      osTime::snapshotRawTime(_dogLastTime);
      hw_watchdogUpdate();
   }

   if (dt > MAX_DOG_LIMIT)
   {
      DataLog_Stream& outStream = log_level_critical(__FILE__, __LINE__);

      outStream << "Long cycle: " << dt << " " << _dogLastTime.sec << " "
                << _dogLastTime.nanosec << _lastStartTime[_lastTimeIndex].sec << " "
                << _lastStartTime[_lastTimeIndex].nanosec;

      int index = _lastTimeIndex - 1; // Use last start stop time.  Current one hasn't finished yet.
      index = (index < 0) ? TimingHistorySize - 1 : index;

      for (int i = 0; i < TimingHistorySize - 1; i++)
      {
         outStream << "(" << index << ") " << _lastEndTime[index].sec << "|" << _lastEndTime[index].nanosec
                   << "|" << _lastStartTime[index].sec << "|" << _lastStartTime[index].nanosec;

         index = (--index < 0) ? (TimingHistorySize - 1) : index;
      }

      outStream << endmsg;

      DBG_DumpData();
   }
}

// SPECIFICATION:    timer message,
//                   check the software watchdog timer
//
// ERROR HANDLING:   none.

void updateTimer::checkTheSoftwareDog ()
{
   static unsigned int noOrderCounter = 0;

   int                 dt;

   // if we haven't got one message yet, don't evaluate
   if (_lastPetCounter >= 1)
   {
      dt = osTime::howLongMilliSec(_lastPetTime);

      if (dt >= MAX_SOFT_WATCHDOG_TIME)
      {
         if (!_softDogBite)
         {
            datalog_Print(log_handle_critical, __FILE__, __LINE__, "Timing slip for soft watchdog of %d ms; last pet %lu.%lu sec.  Counter is %d", dt, (unsigned long)_lastPetTime.sec, (unsigned long)_lastPetTime.nanosec / 1000000, _lastPetCounter);
            _FATAL_ERROR(__FILE__, __LINE__, "Soft watchdog hit - Safety did not hear from Control");
            _softDogBite = true;
         }
      }
      else if (dt > SOFT_WATCHDOG_WARNING_TIME)
      {
         if (++noOrderCounter % 50 == 0) // Should log every 1/2 second
         {
            // DBG_DumpData();
            DataLog(log_level_safe_drv_error) << "WARNING: Soft Watchdog time last pet "
                                              << dt << " ms ago.  Counter is " << _lastPetCounter << endmsg;
         }
      }
      else
      {
         noOrderCounter = 0;
      }

      // if it ever goes backward something is very wrong
      if (dt < 0)
         _FATAL_ERROR(__FILE__, __LINE__, "Raw time went backward!");
   }
}




// SPECIFICATION:    Valve Led Test, turn off Valve LED power
//                   and check to see that all valve opto
//                   sensors turn off.
//
//                   Also, test the door lock in the same way.
//
// ERROR HANDLING:   none.

static
void valveLedTest ()
{
   static short valveTestCount = VALVE_TEST_COUNT;

   valveTestCount++;
   if (valveTestCount > VALVE_TEST_COUNT)
   {
      valveTestCount = 0;
      if (shw_powerStatus() & shw_ledPower)
      {
         unsigned short rawStatus;

         shw_powerDisable(shw_ledPower);
         osTime::delayMilliSec(4);
         if (shw_ledTest(&rawStatus))
         {
            _statusData.valveLedTest = 1;
         }
         else
         {
            static char errStr[13][50] = {" Plasma Valve Counterclockwise;", " Plasma Valve Clockwise;", \
                                          " Plasma Valve Center;", " RBC Valve Counterclockwise;", " RBC Valve Clockwise;", \
                                          " RBC Valve Center;", " Cassette Unload;", " Cassette Load;", " Platelet Valve Counterclockwise;", \
                                          " Platelet Valve Clockwise;", " Platelet Valve Center;", " Door Lock;", " Door Close"};

            int         bit = 1;

            DataLog(log_level_safety_alarm_detail) << "blink test failed: status=0x" << hex << rawStatus << dec << " Reason(s):";
            _statusData.valveLedTest = 0;

            // Go over each bit (bit 0 to bit 12) to interpret the errors
            for (int i = 0; i <= 12; i++)
            {
               bit = (rawStatus >> i) & 0x01;

               if (bit == 0)
               {
                  DataLog(log_level_safety_alarm_detail) << errStr[i];
               }
            }
            DataLog(log_level_safety_alarm_detail) << endmsg;
         }
         shw_powerEnable(shw_ledPower);
      }
   }
}

void updateTimer::monitorTiming (void)
{
   static ULONG previous;
   if (_mt_startupDelay > 0)
   {
      _mt_startupDelay -= 1;
      if (_mt_startupDelay <= 0)
      {
         _mt_updateCount = 0;
         osTime::snapshotRawTime(_mt_lastCheckTime);
         previous        = tickGet();
      }
   }
   else if (!shutdownInProgress)
   {
      _mt_updateCount += 1;
      if (_mt_updateCount >= 150)
      {
         ULONG deltaTicks = tickGet() - previous;
         previous += deltaTicks;

         //
         // If a timingError was detected on the last scan, it better
         // have been acknowledged by safe_msg by now.  If not, shutdown
         // the safety driver with a fatal error.
         //
         if (_SHSPtr->timingError)
         {
            DataLog_Critical critical;

            DataLog(critical) << "Unacknowledged timing error deltaMSec(ticks)=" << deltaTicks << endmsg;
            _FATAL_ERROR(__FILE__, __LINE__, "Unacknowledged timing error");
            _SHSPtr->timingError = false;
         }

         //
         // Check for correct ultrasonics timing
         //
         if (_lowLevel.checkTimingError())
         {
            _SHSPtr->timingError = true;
         }

         unsigned long expectedMSec   = _mt_updateCount * 10;
         unsigned long timerMSec      = osTime::howLongMilliSecAndUpdate(_mt_lastCheckTime);

         unsigned long minAllowedMSec = (expectedMSec * 99) / 100;
         unsigned long maxAllowedMSec = (expectedMSec * 101) / 100;

         if (timerMSec < minAllowedMSec || timerMSec > maxAllowedMSec)
         {
            _SHSPtr->timingError = true;
            DataLog(log_level_safety_alarm_detail)
               << "timing error: expectedMSec=" << expectedMSec << " minAllowedMSec=" << minAllowedMSec
               << " maxAllowedMSec=" << maxAllowedMSec << " timerMSec=" << timerMSec
               << " deltaMSec(ticks)=" << ((ULONG)sysClkRateGet()) * (deltaTicks) << endmsg;
         }

         if (_SHSPtr->timingError)
         {
            if (!safeState)
            {
               // go to safe state
               hw_safeState();

               safeState = true;
            }
         }

         _mt_updateCount = 0;
      }
   }
}

/* FORMAT HASH 9e39524e0e6f86a38b29cca34f01bb04 */

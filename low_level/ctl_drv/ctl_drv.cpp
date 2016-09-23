/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      ctl_drv.cpp, Focussed System control hardware driver
 *
 * ABSTRACT:   Interface to the control hardware.  This driver responds to a
 *             command message and transmits a status message every 500 ms.
 *             This driver controls all pumps, valves, centrifuge, and other
 *             devices and reports status on all devices.  For message description
 *             see chw.hpp.
 *
 * DOCUMENTS
 *
 */

#include <vxWorks.h>
#include <sigLib.h>
#include <sysLib.h>

/* Common */
#include "hw_intf.h"    /* hwGetPortRegister() */

#include "ctl_drv.hpp"
#include "crc_util.h"
#include "c_ultra.h"
#include "failure_debug.h"
#include "fastpack.hpp"
#include "pfw_int.h"
#include "sh_mem.h"
#include "trima_assert.h"
#include "trima_datalog.h"
#include "trima_tasks.h"

// constants

static const float    MAX_PUMP_SPEED_ORDER = 250.0f; // max pump speed, rpm
static const unsigned DEBOUNCE_COUNT       = 5;      // debounce count

// set above 3000 as absolute max limit
static const float MAX_CENT_SPEED_ORDER = 3100.0f;

static const int      MUX_SETTLING_TIME =    5;    // mux settling time, ms
static const unsigned FLUIDS_SEEN_TIME  = 1400;    // fluid at low sensor, 1.4 sec
static const unsigned CENTRIFUGE_TIMER  =   95;    // centrifuge order, ms
static const unsigned STATUS_RATE       =  500;    // status msg rate, ms
static const unsigned RBC_RESEND_COUNT  =   10;    // rbc count
static const long     MIN_CENT_POWER    =   50;    // min centrifuge power, volts
static const long     MIN_PUMP_POWER    =   18;    // min pump power, volts

#define APS_MUX  8                           // access presssure sen
#define LOW_AGC_MUX 19                       // low AGC sensor
#define HIGH_AGC_MUX 20                      // high AGC sensor

static const int MAX_DOG_LIMIT = 50;         // if 10 ms loop exceeds 50 ms, log it
static const int MIN_DOG_LIMIT =  5;         // if 10 ms loop is less than 5 ms, ignore it

static const int MAX_SOFT_WATCHDOG_FATAL_ERROR_TIME = 8000;      // soft watchdog timer expires in 9 seconds
static const int MAX_SOFT_WATCHDOG_TIME             = 2000;      // soft watchdog timer expires in 2 seconds
static const int SOFT_WATCHDOG_WARNING_TIME         = 1000;      // soft watchdog timer expires in 1 seconds

static const int HOLD_CENTRIFUGE_DURING_DOOR_RETRY_TIME = 3000;  // hold centrifuge for 3 seconds
                                                                 // while retrying the door solenoid

// local data

static CHwOrders  _orderData;                              // order data
static CHwStates  _statusData;                             // status data
static HW_ORDERS  _forceAirToDonorMonitor;                 // HW_ENABLE if control driver should enable air to donor monitoring during PFR
static fastpack   _aps(CONTROL_ACCESS_PRESSURE, "aps");    // access pressure data
static fastfilter _apsFilter("apsFilter");                 // access pressure data

static SHwOrders* _safetyOrders = NULL;

static short _oldSixtyFourVoltSwitched = 0;               // from previous read
static bool  _oldSixtyFourVoltsOk      = false;           // from previous read

static bool _shutdownInProgress = false;
static bool _safeState          = false;

static rawTime shutdownTime = {0, 0};

static unsigned char atodCycleCount = 0;

#define COUNT_OF(a) (sizeof(a) / sizeof(a[0]))



// SPECIFICATION:    Fatal error handling specific to control driver task
//
static void FATAL_ERROR_HANDLER (void* arg)
{
   if (!_safeState)
   {
      // go to safe state
      hw_safeState();

      _safeState = true;
   }

   // start a timer as a backup, in case the system is too unstable for the normal
   // fatal error handling to shut us down.
   _shutdownInProgress = true;

   osTime::snapshotRawTime(shutdownTime);
}



// SPECIFICATION:   switch debounce - constructor
//
// ERROR HANDLING:   none.
debounce::debounce(HWSwitch select, HW_SWITCH_STATES& sw, HW_SWITCH_STATES& swLatch)
   : _select(select),
     _debouncedValue(sw),
     _latchedValue(swLatch),
     _count(0),
     _latchCount(0)
{
   _debouncedValue = HW_SWITCH_DISABLED;
   _latchedValue   = HW_SWITCH_DISABLED;
}



// SPECIFICATION:    switch debounce - destructor
//
// ERROR HANDLING:   none.
debounce::~debounce()
{}



// SPECIFICATION:    switch debounce - update, process data, when
//                   DEBOUNCE_COUNT in a row match, change states
//                   and transmit status message.
//
// ERROR HANDLING:   none.
void debounce::update (void)
{
   HW_SWITCH_STATES newValue = (hw_switchGetStatus(_select) & hw_switchPressed) ?
                               HW_SWITCH_ENABLED : HW_SWITCH_DISABLED;

   if (_debouncedValue != newValue)
   {
      // debounce switch data to avoid multiple events from a single switch press.
      // Note that debounce is quicker on press than release.  We do this to limit
      // the number of switch events that the system must deal with while still
      // maintaining a fast response to a switch press.
      _count++;

      if ( (_count >= DEBOUNCE_COUNT && newValue == HW_SWITCH_ENABLED) ||
           (_count >= 10 * DEBOUNCE_COUNT)
           )
      {
         if (newValue == HW_SWITCH_ENABLED)
         {
            _latchedValue = HW_SWITCH_ENABLED;
            _latchCount   = 0;
         }

         _debouncedValue = newValue;
         _count          = 0;
      }
   }
   else
   {
      _count = 0;                            // clear count
   }

   if (_latchedValue == HW_SWITCH_ENABLED)
   {
      if (++_latchCount >= 150)
      {
         _latchedValue = HW_SWITCH_DISABLED;
         _latchCount   = 0;
      }
   }
}



// SPECIFICATION:    used to send immediate response pressure
//                   data.
//                   fastfilter constructor opens shared memory
//                   that was created by the ctl_msg task
//
// ERROR HANDLING:   none.
fastfilter::fastfilter(const char* sharename)
{
   TRIMA_ASSERT(sharename);

   // create shared memory for communication to the driver
   _FPtr = (FASTFIL*)shmem_create(sharename, sizeof(FASTFIL));

   if (!_FPtr)
   {
      DataLog(log_level_critical) << sharename << " shmem_create failed" << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "shmem_create");
   }
   else
   {
      memset((void*)_FPtr, 0, sizeof(FASTFIL));
   }
}



// SPECIFICATION:    fastfilter destructor
//
//
// ERROR HANDLING:   none.
fastfilter::~fastfilter()
{}



// SPECIFICATION:    fastfilter data collector to
//                   shared memory
//
//
// ERROR HANDLING:   none.
void fastfilter::dataPoint (short data)
{
   _FPtr->msg[_FPtr->NextIn].data.raw[_FPtr->count] = data;
   _FPtr->count++;

   if (_FPtr->count >= FASTFILTERLENGTH)
   {
      _FPtr->msg[_FPtr->NextIn].crc = 0;

      if (CRCGEN32_STRUCT(&_FPtr->msg[_FPtr->NextIn].crc, &_FPtr->msg[_FPtr->NextIn]) != 0)
      {
         _FATAL_ERROR(__FILE__, __LINE__, "crcgen32");
      }
      else
      {
         // test for lapping NextOut
         // this is optional data so only test the lap
         // if someone is retrieving the data
         if (_FPtr->NextIn + 1 == _FPtr->NextOut)
            _FATAL_ERROR(__FILE__, __LINE__, "Fastpack NextIn lapping NextOut!");

         // increment NextIn index
         if (_FPtr->NextIn == NUM_FASTPACK - 1)
            _FPtr->NextIn = 0;
         else
            _FPtr->NextIn++;

         _FPtr->count = 0;
      }
   }
}



// Static variables
CHO* commands::_CHOPtr = NULL;



// SPECIFICATION:    Control hardware commands, constructor
//
// ERROR HANDLING:   none.
commands::commands(centrifugeControl& centrifuge)
   : redDrive(10),
     greenDrive(10),
     _centControl(centrifuge),
     _rbcCounter(0),
     _commandCounter(0)
{
   // open shared memory for communication to the control message task
   _CHOPtr = (CHO*)shmem_create("controlOrder", sizeof(CHO));

   if (!_CHOPtr)
   {
      _FATAL_ERROR(__FILE__, __LINE__, "controlOrder shmem_create failed");
   }

   //
   // initialize the control orders related data
   //
   memset((void*)_CHOPtr, 0, sizeof(CHO));

   _CHOPtr->forceAirToDonorMonitor = HW_DISABLE;
}



// SPECIFICATION:    Control hardware commands, destructor
//
// ERROR HANDLING:   none.
commands::~commands()
{}



// SPECIFICATION:    Control hardware commands, get new order data
//                   from shared memory
//
// ERROR HANDLING:   none.
bool commands::getNewOrder ()
{
   bool result = false;

   _forceAirToDonorMonitor = _CHOPtr->forceAirToDonorMonitor;

   if (_CHOPtr->NextOut != _CHOPtr->NextIn)
   {
      _commandCounter += 1;

      if (_CHOPtr->msg[_CHOPtr->NextOut].count != _commandCounter)
      {
         DataLog(log_level_critical) << "controlOrder count mismatch: expected=" << _commandCounter
                                     << " received=" << _CHOPtr->msg[_CHOPtr->NextOut].count << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "controlOrder count");
      }

      // check the crc
      unsigned long checksum = 0;

      if (CRCGEN32_STRUCT(&checksum, &_CHOPtr->msg[_CHOPtr->NextOut]) != 0)
      {
         _FATAL_ERROR(__FILE__, __LINE__, "crcgen32");
      }
      else
      {
         if (checksum == _CHOPtr->msg[_CHOPtr->NextOut].crc)
         {
            // new orders are in
            memcpy(&_orderData, &_CHOPtr->msg[_CHOPtr->NextOut], sizeof(CHwOrders));

            // Snapshot time message was dequeued for soft watchdog analysis
            osTime::snapshotRawTime(_CHOPtr->msg[_CHOPtr->NextOut].timeOrdersDequeued);

            result = true;
         }
         else
         {
            DataLog(log_level_critical) << "controlOrder CRC mismatch: expected=" << checksum
                                        << " received=" << _CHOPtr->msg[_CHOPtr->NextOut].crc << endmsg;

            _FATAL_ERROR(__FILE__, __LINE__, "controlOrder CRC");
         }
      }

      if (_CHOPtr->NextOut == NUM_CHWORDERS - 1)
         _CHOPtr->NextOut = 0;
      else
         _CHOPtr->NextOut++;
   }

   return result;
}



// SPECIFICATION:    Control hardware commands, do new centrifuge
//                   and alarm orders
//
// ERROR HANDLING:   none.
void commands::doNewOrder ()
{
   // limit centrifuge order if necessary
   float centOrder = _orderData.centrifugeRPM;

   if (centOrder > MAX_CENT_SPEED_ORDER)
   {
      centOrder = MAX_CENT_SPEED_ORDER;   // range check to testing range

      DataLog(log_level_critical) << "Centrifuge Speed orderd to " << _orderData.centrifugeRPM
                                  << " max command is " << MAX_CENT_SPEED_ORDER << endmsg;

      _FATAL_ERROR(__FILE__, __LINE__, "Centrifuge Speed Order Limited");
   }
   else if (centOrder < 0)
   {
      chw_centDisable();
      centOrder = 0;
   }

   _centControl.newOrder(centOrder, _orderData.centrifugeRampUpRate, _orderData.centrifugeRampDownRate);

   hw_alarmSetCommand((_orderData.alarmLight == HW_ENABLE) ? hw_alarmOn : hw_alarmOff);

   chw_leakDetectorSetCommand(chw_leakDetectorOn);
}



static void signal_handler (int sig)
{
   _shutdownInProgress = true;
   osTime::snapshotRawTime(shutdownTime);
}



static void readback_failed (const char* file,      // source file name where error was detected
                             int line,              // source file line where error was detected
                             const char* type,      // type of readback (e.g. "byte", "word")
                             unsigned long port,    // I/O port address
                             unsigned long value,   // value read back from I/O port
                             unsigned long expected // value expected to be read back
                             )
{
   DataLog(log_level_critical) << "I/O " << type << " readback failed: portId " << hex << port
                               << " portReg " << hwGetPortRegister(port)
                               << " value " << value << " expected " << expected << dec
                               << " @ " << file << ":" << line
                               << endmsg;

   _FATAL_ERROR(file, line, "I/O readback failed");

}


#define TEN_MILLISECONDS 10000

extern "C" void ctl_drv (const char* options)
{
   //
   // Make sure the driver specific fatal error handler hook is in place
   // before any other code is executed.
   //
   _CHAIN_FATAL_ERROR_HANDLER(FATAL_ERROR_HANDLER, NULL);

   //
   // Use SIGQUIT to initiate shutdown of the control driver
   //
   signal(SIGQUIT, signal_handler);

   //
   // ctl_drv_info is used as the default log level, errors should be sent
   // explicitly to ctl_drv_error.
   //
   datalog_SetDefaultLevel(log_handle_ctl_drv_info);

   unsigned char powerFailFlag = 0;

   // Initialize the hardware driver
   hw_init();
   hw_setReadbackFailedFunc(&readback_failed);

   // parse the command line - format is: "[-In] [-R]"
   //    -In specifies that interrupt vector n be used for the power fail interrupt
   //    -R specifies that power fail ride-thru should be enabled
   //
   // by default, interrupt vector 6 is used, and ride-thru is disabled
   //
   const char* separators = " \t";
   int         optIndex   = strspn(options, separators);

   bool pfwRidethru = false;

   DataLog_Default << "ctl_drv started with command line: \"" << options << "\"" << endmsg;

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
   memset(&_orderData, 0, sizeof(_orderData));
   memset(&_statusData, 0, sizeof(_statusData));

   // initialize hardware state
   hw_safeState();

   // setup timer notifications and handle periodic update
   SEM_ID tmrSem = auxClockSemaphoreAttach (TEN_MILLISECONDS, AuxClockCountingSemaphore);

   if (!tmrSem)
   {
      _FATAL_ERROR(__FILE__, __LINE__, "auxClockSemaphoreAttach failed");
   }

   // create the 10ms timer and command processing objects
   centrifugeControl centControl;
   commands          cmds(centControl);
   updateTimer       timer(10, cmds, centControl, powerFailFlag);

   // Only perform this check if the New MX Control1&2 FPGAs are installed
   if (hw_mxFpgaInstalled())
   {
      timer.controlWDStatus();
   }

   // set valves and cassette to current state
   // if coming back from an abnormal shutdown, the positions may be invalid
   timer.getValves();

   _orderData.cassettePosition = (CHW_CASSETTE_ORDERS)_statusData.cassettePosition;
   _orderData.rbcValve         = (CHW_VALVE_ORDERS)_statusData.rbcValve;
   _orderData.plasmaValve      = (CHW_VALVE_ORDERS)_statusData.plasmaValve;
   _orderData.plateletValve    = (CHW_VALVE_ORDERS)_statusData.plateletValve;

   chw_pumpEnable();

   //
   // Main loop for control driver.  Do this until a system shutdown has been started
   while (!_shutdownInProgress)
   {
      semTake(tmrSem, WAIT_FOREVER);
      timer.timerCallback();
   }

   //
   // SHUTDOWN SEQUENCE
   //

   // If Trima is not in the safe state, set it
   if (!_safeState)
   {
      // go to safe state
      hw_safeState();

      _safeState = true;
   }

   DataLog_Default << "Control: shutdown started - hardware in safe state" << endmsg;

   // If the New MX Versions of the Control1&2 FPGAs are installed, wait for
   // the WatchDog to Reset the Processor in 5 Seconds
   if (hw_mxFpgaInstalled())
   {
      int count = 5 * sysClkRateGet();

      // Wait for the WatchDog Circuit to Reset the Control Processor in
      // 5 Seconds
      while (count--)
      {
         timer.controlWDStatus();

         taskDelay(1);
      }
   }
   else
   {  // Otherwise, use the Legacy Method

#ifndef DEBUG_EXTEND_SHUTDOWN
      // hold here for five seconds to allow shutdown (especially disk
      // synchronization) to complete
      int count = 5 * sysClkRateGet();
#else
      int count = DEBUG_EXTEND_SHUTDOWN * sysClkRateGet();
#endif

      while (count--)
      {
         timer.petTheDog();

         taskDelay(1);
      }
   }
}



// SPECIFICATION:    process analog channels, this function takes a single
//                   reading, places the value in the status structure,
//                   changes the mux address, and starts the A/D.
//
// ERROR HANDLING:   none.
void processAnalog (bool& blockPumps, bool& blockMessages, bool& atodError)
{
   typedef struct
   {
      unsigned char mux;
      short*        data;
   } MUX;

   static MUX muxTable[] =
   {
      8,    &_statusData.accessPressure,           // access pressure sensor
      21,   &_statusData.emiTemperature,           // EMI enclosure temperature
      8,    &_statusData.accessPressure,           // access pressure sensor
      18,   &_statusData.centrifugeCurrent,        // centrifuge current
      8,    &_statusData.accessPressure,           // access pressure sensor
      9,    &_statusData.centrifugePressure,       // centrifuge pressure sensor
      8,    &_statusData.accessPressure,           // access pressure sensor
      12,   &_statusData.fiveVoltSupply,           // +5V supply
      8,    &_statusData.accessPressure,           // access pressure sensor
      17,   &_statusData.leakDetector,             // leak
      8,    &_statusData.accessPressure,           // access pressure sensor
      16,   &_statusData.minusTwelvePS,            // -12 volts
      8,    &_statusData.accessPressure,           // access pressure sensor
      14,   &_statusData.sixtyFourVoltSupply,      // +64 volt (centrifuge)
      8,    &_statusData.accessPressure,           // access pressure sensor
      15,   &_statusData.sixtyFourVoltSwitched,    // +64 volt after switch
      8,    &_statusData.accessPressure,           // access pressure sensor
      13,   &_statusData.twelveVoltSupply,         // +12V supply
      8,    &_statusData.accessPressure,           // access pressure sensor
      23,   &_statusData.twentyFourVoltCurrent,    // +24V current
      8,    &_statusData.accessPressure,           // access pressure sensor
      10,   &_statusData.twentyFourVoltSupply,     // +24V supply
      8,    &_statusData.accessPressure,           // access pressure sensor
      11,   &_statusData.twentyFourVoltSwitched,   // +24V switched
      8,    &_statusData.accessPressure,           // access pressure sensor
      19,   &_statusData.lowAGC,                   // low ultrasonics AGC voltage
      8,    &_statusData.accessPressure,           // access pressure sensor
      20,   &_statusData.highAGC,                  // high ultrasonics AGC voltage
      8,    &_statusData.accessPressure,           // access pressure sensor
      22,   &_statusData.sixtyFourVoltCurrent,     // +64V current
   };

   static rawTime startTime             = {0, 0};
   static bool    errorRetryDelayActive = false;

   int           dt;                           // delta time, ms
   int           filterSize = 5;
   unsigned char muxAddress = 0;

   static unsigned short tableCounter = 0xFF;           // table counter
   if (tableCounter == 0xFF)
   {
      // Set the initial mux address before starting the ADC
      tableCounter = 0;
      muxAddress   = muxTable[tableCounter].mux;
      chw_adcSetMux(muxAddress);
   }

   // check time to insure mux settles
   dt = osTime::howLongMilliSec(startTime);

   if (errorRetryDelayActive)
   {
      //
      // Wait 10 seconds before retrying after an A/D error detected.
      // This is intended to limit the alarm reporting rate to something
      // manageble by the system, but still repeat it until the alarm
      // handler is up to receive it.
      //
      if (dt < 10000)
      {
         return;
      }

      errorRetryDelayActive = false;
   }

   if ((dt < MUX_SETTLING_TIME) && (startTime.sec != 0))
   {
      return;
   }

   // start the a/d conversion
   chw_adcStart();
   osTime::snapshotRawTime(startTime);

   // wait for conversion complete
   while (chw_adcGetStatus() & chw_adcBusy)
   {
      dt = osTime::howLongMilliSec(startTime);

      if (dt > MUX_SETTLING_TIME)
      {
         atodError             = true;
         errorRetryDelayActive = true;
         osTime::snapshotRawTime(startTime);
         return;
      }
   }

   // get and save data
   short data = chw_adcGetValue();

   if (tableCounter > COUNT_OF(muxTable))
   {
      _FATAL_ERROR(__FILE__, __LINE__, "tableCounter out of range");
   }

   // save and send high speed access pressure sensor data (aps)

   if (muxTable[tableCounter].mux == APS_MUX)   // access?
   {
      _aps.dataPoint(data);                    // send data
      *(muxTable[tableCounter].data) = (short) // filter value
                                       ((data + (filterSize - 1) * (*(muxTable[tableCounter].data))) / filterSize);
      // test filtered value against high pressure level
      // and send message if it exceeds the limit.
      // We'll stop pump elsewhere
      if (_orderData.highAPSLimit != 0)
      {
         // if this is the first time that we see it over the
         // upper limit, send a message.
         if ((*(muxTable[tableCounter].data) >= _orderData.highAPSLimit)
             &&(blockMessages == false)
             &&(blockPumps == false))
         {
            _apsFilter.dataPoint (*(muxTable[tableCounter].data));         // send data
            blockPumps    = true;
            blockMessages = true;
         }

      }

      // test filtered value against low pressure level
      // and send message if it exceeds the limit.
      // We'll stop pump elsewhere
      if (_orderData.lowAPSLimit != 0)
      {
         // if this is the first time that we see it under the
         // lower limit, send a message.
         if ( (*(muxTable[tableCounter].data) <= _orderData.lowAPSLimit) &&
              (blockMessages == false) &&
              (blockPumps == false)
              )
         {
            _apsFilter.dataPoint (*(muxTable[tableCounter].data));         // send data
            blockPumps    = true;
            blockMessages = true;
         }
      }
   }
   else if  // divide by 3 for electronics change in 2.6 AGC HW
   ((muxTable[tableCounter].mux == LOW_AGC_MUX) ||
    (muxTable[tableCounter].mux == HIGH_AGC_MUX))
   {
      *(muxTable[tableCounter].data) = (short)(data / 3);
   }
   else
   {
      *(muxTable[tableCounter].data) = data;    // save value
   }

   // get next mux address and send to hardware
   tableCounter++;                              // bump table pointer

   if (tableCounter >= COUNT_OF(muxTable))      // wrap check
   {
      tableCounter = 0;                         // start over

      if (atodCycleCount < 255)
         atodCycleCount += 1;
   }

   muxAddress = muxTable[tableCounter].mux;
   chw_adcSetMux(muxAddress);
}



// SPECIFICATION:    Check if 24 volt switched supply ok
//                   Tests to see if voltage above 18 volts
//
// ERROR HANDLING:   none.
unsigned short twentyFourVoltsOk ()
{
   return((_statusData.twentyFourVoltSwitched / 91 > MIN_PUMP_POWER) ? 1 : 0);
}



// SPECIFICATION:    Check if 64 volt switched supply ok
//                   Tests to see if voltage above 50 volts
//
// ERROR HANDLING:   none.
bool sixtyFourVoltsOk ()
{
   return ( (_statusData.sixtyFourVoltSwitched / 19 > MIN_CENT_POWER) ? true : false);
}



// SPECIFICATION:    timer constructor, creates hardware objects
//
// ERROR HANDLING:   none.
updateTimer::updateTimer(unsigned long dt,
                         commands& command,
                         centrifugeControl& centrifuge,
                         unsigned char& powerFailFlag)
   : _dt(dt),
     _commands(command),
     _centControl(centrifuge),
     _powerFailFlag(powerFailFlag),

     _count(0),
     _ccount(0),
     _oldPauseSwitch(HW_SWITCH_DISABLED),
     _oldStopSwitch(HW_SWITCH_DISABLED),
     _oldReservoir(CHW_RESERVOIR_ERROR),
     _softWatchdogActive(false),
     _blockPumps(false),
     _blockMessages(false),
     _powerOnFlag(false),
     _mtt_startupDelay(1000),
     _mtt_oldLowerAccTotal(0),
     _mtt_oldUpperAccTotal(0),
     _mtt_accInterval(0),
     _mtt_skipTimeCheck(false),
     _atodErrorAckCounter(0),
     _logStopReturn(true),
     _logStartReturn(false),
     _newWdStatus(hw_unknown),
     _oldWdStatus(hw_unknown),
     _newSealSafeStatus(hw_sealSafeOn),
     _oldSealSafeStatus(hw_sealSafeOn),
     _newSealSafeOTW(hw_sealSafeOTW),
     _oldSealSafeOTW(hw_sealSafeOTW),
     _newOVPTestResult(hw_ovpTestFail),
     _oldOVPTestResult(hw_ovpTestFail),
     _newSupplyOutputStatus(hw_supplyOutputsDisabled),
     _oldSupplyOutputStatus(hw_supplyOutputsDisabled),
     _loggedPs(0),
     _sealSafeLatch(false),
     _lastTimeIndex(0),
     _preSoftWatchdogPausePumps(false),

// Pumps
     _acPump(hw_acPump, dt, _orderData, _statusData),
     _inletPump(hw_inletPump, dt, _orderData, _statusData),
     _plateletPump(hw_plateletPump, dt, _orderData, _statusData),
     _plasmaPump(hw_plasmaPump, dt, _orderData, _statusData),
     _returnPump(hw_returnPump, dt, _orderData, _statusData),

// valves
     _plasmaValve(hw_plasmaValve),
     _plasmaRunValve(hw_plasmaValve, _orderData.plasmaValve, _statusData.plasmaValve),
     _plateletValve(hw_plateletValve),
     _plateletRunValve(hw_plateletValve, _orderData.plateletValve, _statusData.plateletValve),
     _rbcValve(hw_rbcValve),
     _rbcRunValve(hw_rbcValve, _orderData.rbcValve, _statusData.rbcValve),
     _cassette(hw_cassette),
     _cassetteRun(_orderData.cassettePosition, _statusData.cassettePosition, _statusData.cassetteError),

// centrifuge monitor
     _centrifuge(),

// interface to rbcPort task
     _rbcDetector (_statusData.red, _statusData.green, _statusData.newReflectanceCounts, _statusData.newDriveCounts),

// create switch objects
     _pause(hw_pauseSwitch, _statusData.pauseSwitch, _statusData.pauseSwitchLatch),
     _stop(hw_stopSwitch, _statusData.stopSwitch, _statusData.stopSwitchLatch),

// door locks
     _door(_orderData, _statusData),

// create temperature monitor
     _temp (_statusData.tempError, _statusData.emiTemperature, _statusData.fanSense0, _statusData.fanSense1, _statusData.fanSense2),

// sound control and status
     _soundInterface(_orderData.soundLevel),

// create ultrasonic data log objects
     _usAir(CONTROL_UPPER_US_AIR, "controlUSAir"),
     _usFluid (CONTROL_UPPER_US_FLUID, "controlUSFluid"),
     _lsAir (CONTROL_LOWER_US_AIR, "controlLSAir"),
     _lsFluid (CONTROL_LOWER_US_FLUID, "controlLSFluid")

{
   //
   // Create the safety orders shared memory area (maintained by ctl_msg.cpp).
   // This is made available to the subsystems that need information about Safety
   // Orders (e.g. pump monitor needs to determine if Safety disabled the switched
   // 24V supply).
   //
   _safetyOrders = (SHwOrders*)shmem_create("safetyOrders", sizeof(SHwOrders));

   if (!_safetyOrders)
      _FATAL_ERROR(__FILE__, __LINE__, "safetyOrders shmem_create failed");

   //
   // initialize the safety orders related data
   //
   memset((void*)_safetyOrders, 0, sizeof(SHwOrders));

   // Add a link to the pump objects
   pump::setSHwOrders(_safetyOrders);
   _door.setSHwOrders(_safetyOrders);

   _fluid_seen_once = HW_DISABLE;

   // Are the updated MX FPGAs Installed?
   if (hw_mxFpgaInstalled())
   {
      _newMxInstalled = true;
   }
   else
   {
      _newMxInstalled = false;
   }

   // create shared memory for communication to the control message task
   _CHSPtr = (CHS*)shmem_create("controlStatus", sizeof(CHS));

   if (!_CHSPtr)
   {
      _FATAL_ERROR(__FILE__, __LINE__, "controlStatus shmem_create failed");
   }

   //
   // initialize the control status related data
   //
   memset((void*)_CHSPtr, 0, sizeof(CHS));
   _CHSPtr->timingError                = false;
   _CHSPtr->atodError                  = false;
   _CHSPtr->controlOrdersPastThreshold = false;


   // create ultrasonics
   ctl_interruptAttach(&_statusData.lowAGC, &_statusData.highAGC);

   //
   // Initialize timer for AC air to fluid debounce
   //
   osTime::snapshotRawTime(_acAirToFluidTimer);

   // initialize timer for fluids seen
   osTime::snapshotRawTime(_fluidsSeenTimer);

   // initialize timer for Seal Safe status
   osTime::snapshotRawTime(_lastTimeSealSafeSwitchedOn);

   //
   // Initialize the power on time
   //
   osTime::snapshotRawTime(_powerOnTime);

   // initialize software watchdog timer
   _lastPetCounter = 0;
   osTime::snapshotRawTime(_lastPetTime);

   // Initiialize the hardware watchdog timer
   memset(&_dogLastTime, 0, sizeof(_dogLastTime));

   // Initialize the time keepers.
   memset(&_lastStartTime, 0, sizeof(_lastStartTime));
   memset(&_lastEndTime, 0, sizeof(_lastEndTime));
}



// SPECIFICATION:    timer destructor, deletes hardware objects
//
// ERROR HANDLING:   none.
updateTimer::~updateTimer()
{
   ctl_interruptDeattach();
}



// SPECIFICATION:    Control WatchDog Status. Logs Change in Status of the
//                   WatchDog Circuit.
//
// ERROR HANDLING:   none.
//
// RETURN:           Enum Representing Good Status or the Reason for a
//                   WatchDog Reset.
//
HWWatchDogStatus updateTimer::controlWDStatus (void)
{

   datalog_SetDefaultLevel(log_handle_ctl_drv_info);

   _newWdStatus = hw_watchDogStatus();

   if (_newWdStatus != _oldWdStatus)
   {
      switch (_newWdStatus)
      {
         case hw_noFailure :
            DataLog_Default << "Control WatchDog Status: No Failures" << endmsg;
            break;

         case hw_wrongA5Byte :
            DataLog_Default << "Control WatchDog Status: Reset Due To Wrong A5 Byte Written" << endmsg;
            break;

         case hw_wrongF0Byte :
            DataLog_Default << "Control WatchDog Status: Reset Due To Wrong F0 Byte Written" << endmsg;
            break;

         case hw_a5WriteWaitingF0 :
            DataLog_Default << "Control WatchDog Status: Reset Due To A5 Byte Written While Waiting For F0 Byte" << endmsg;
            break;

         case hw_f0WriteWaitingA5 :
            DataLog_Default << "Control WatchDog Status: Reset Due To F0 Byte Written While Waiting For A5 Byte" << endmsg;
            break;

         case hw_wdTimeOut :
            DataLog_Default << "Control WatchDog Status: Reset Due To WatchDog Time-Out" << endmsg;
            break;

         case hw_powerUp :
            DataLog_Default << "Control WatchDog Status: Normal After a Power-Up Or Power-Fail" << endmsg;
            break;

         default :
            DataLog_Default << "Control WatchDog Status:  UNKNOWN" << endmsg;
            break;
      }

      _oldWdStatus = _newWdStatus;
   }

   return _newWdStatus;
}



void updateTimer::controlSealSafeStatus (void)
{
   datalog_SetDefaultLevel(log_handle_ctl_drv_info);

   if (hw_hasArtesynPs())
   {
      if (!_loggedPs)
      {
         _loggedPs = true;
         DataLog_Default << "Control: Power Supply is Artesyn." << endmsg;
      }

      _newSealSafeOTW        = hw_sealSafeOTWC();
      _newOVPTestResult      = hw_OVPTestResult();
      _newSupplyOutputStatus = hw_SupplyOutputStatus();

      HWSealSafeStatus currSealSafeStatus = hw_sealSafeStatus();

      if (currSealSafeStatus != _oldSealSafeStatus && !_sealSafeLatch)
      {
         _newSealSafeStatus = currSealSafeStatus;

         // Only latch for "on" status.
         if (_newSealSafeStatus == hw_sealSafeOn)
         {
            osTime::snapshotRawTime(_lastTimeSealSafeSwitchedOn);
            _sealSafeLatch = true;
         }
         else
            DataLog_Default << "seal safe status: active=" << currSealSafeStatus << "; latched=" << _sealSafeLatch << endmsg;
      }
      else
         _newSealSafeStatus = _oldSealSafeStatus;

      if (osTime::howLongMilliSec(_lastTimeSealSafeSwitchedOn) > osTime::howLongMilliSec(_CHSPtr->statusSentTimestamp))
         _sealSafeLatch = false;

      if (_newSealSafeStatus != _oldSealSafeStatus)
      {
         switch (_newSealSafeStatus)
         {
            case hw_sealSafeOn :
               _statusData.sealSafeOperating = CHW_SEAL_SAFE_ON;
               DataLog_Default << "Control: Seal Safe Is On" << endmsg;
               break;

            case hw_sealSafeOff :
               _statusData.sealSafeOperating = CHW_SEAL_SAFE_OFF;
               DataLog_Default << "Control: Seal Safe Is Off" << endmsg;
               break;

            default :
               _statusData.sealSafeOperating = CHW_SEAL_SAFE_OPERATION_NOT_AVAILABLE;
               DataLog_Default << "Control: Seal Safe Status Unknown" << endmsg;
               break;
         }

         _oldSealSafeStatus = _newSealSafeStatus;
      }

      if (_newSealSafeOTW != _oldSealSafeOTW)
      {
         switch (_newSealSafeOTW)
         {
            case hw_sealSafeOTW :
               _statusData.sealSafeOvertemp = CHW_SEAL_SAFE_OTW;
               DataLog_Default << "Control: Seal Safe Over Temp Warning" << endmsg;
               break;

            case hw_sealSafeTempOk :
               _statusData.sealSafeOvertemp = CHW_SEAL_SAFE_TEMP_OK;
               DataLog_Default << "Control: Seal Safe Temp Is OK" << endmsg;
               break;

            default :
               _statusData.sealSafeOvertemp = CHW_SEAL_SAFE_OTW_NOT_AVAILABLE;
               DataLog_Default << "Control: Seal Safe Temp UNKNOWN" << endmsg;
               break;
         }

         _oldSealSafeOTW = _newSealSafeOTW;
      }

      if (_newOVPTestResult != _oldOVPTestResult)
      {
         switch (_newOVPTestResult)
         {
            case hw_ovpTestPass :
               _statusData.ovpTestResult = CHW_OVP_TEST_RESULT_PASS;
               DataLog_Default << "Control: Power Supply OVP Test PASSED" << endmsg;
               break;

            case hw_ovpTestFail :
               _statusData.ovpTestResult = CHW_OVP_TEST_RESULT_FAIL;
               DataLog_Default << "Control: Power Supply OVP Test FAILED" << endmsg;
               break;

            default :
               _statusData.ovpTestResult = CHW_OVP_TEST_RESULT_NOT_AVAILABLE;
               DataLog_Default << "Control: Power Supply OVP Test Result UNKNOWN" << endmsg;
               break;
         }

         _oldOVPTestResult = _newOVPTestResult;
      }


      if (_newSupplyOutputStatus != _oldSupplyOutputStatus)
      {
         switch (_newSupplyOutputStatus)
         {
            case hw_supplyOutputsEnabled :
               DataLog_Default << "Control: Power Supply Outputs (12.7v, 24v, and 64v) Enabled" << endmsg;
               _statusData.powerSupplyEnabled = HW_PS_OUTPUT_STATUS_ENABLED;
               break;

            case hw_supplyOutputsDisabled :
               DataLog_Default << "Control: Power Supply Outputs (12.7v, 24v, and 64v) Disabled" << endmsg;
               _statusData.powerSupplyEnabled = HW_PS_OUTPUT_STATUS_DISABLED;
               break;

            default :
               _statusData.powerSupplyEnabled = HW_PS_OUTPUT_STATUS_UNKNOWN;
               DataLog_Default << "Control: Power Supply Outputs Status UNKNOWN" << endmsg;
               break;
         }

         _oldSupplyOutputStatus = _newSupplyOutputStatus;
      }
   }
   else
   {
      if (!_loggedPs)
      {
         _loggedPs = true;
         DataLog_Default << "Control: Power Supply is NOT Artesyn." << endmsg;
      }

      _statusData.sealSafeOperating  = CHW_SEAL_SAFE_OPERATION_NOT_AVAILABLE;
      _statusData.sealSafeOvertemp   = CHW_SEAL_SAFE_OTW_NOT_AVAILABLE;
      _statusData.ovpTestResult      = CHW_OVP_TEST_RESULT_NOT_AVAILABLE;
      _statusData.powerSupplyEnabled = HW_PS_OUTPUT_STATUS_UNKNOWN;
   }
}

// SPECIFICATION:   petTheDog -- hardware watchdog service routine
//                  If not serviced within 420 ms, the hardware will
//                  automatically reset.
//
// ERROR HANDLING:   none.
void updateTimer::petTheDog ()
{
   const int dt            = osTime::howLongMilliSec(_dogLastTime);
   bool      firstTimeThru = false;

   if (dt > MIN_DOG_LIMIT)
   {
      if (_dogLastTime.sec == 0 && _dogLastTime.nanosec == 0)
      {
         firstTimeThru = true;
      }

      osTime::snapshotRawTime(_dogLastTime);
      hw_watchdogUpdate();
   }

   if (!firstTimeThru && (dt > MAX_DOG_LIMIT) )
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



// SPECIFICATION:    get valve states
//
// ERROR HANDLING:   none.
void updateTimer::getValves ()
{
   _statusData.plasmaValve      = _plasmaValve.update();
   _statusData.plateletValve    = _plateletValve.update();
   _statusData.rbcValve         = _rbcValve.update();
   _statusData.cassettePosition = _cassette.updateCassette();
}



//
// This function looks for new commands.  It is called from the main timer callback function and
// is executed at the 10 ms rate.
//
void updateTimer::commandUpdate ()
{
   static unsigned int noOrderCounter = 0;

   // If the soft watchdog alarm is active, don't bother processing or logging anything
   if (_softWatchdogActive)
   {
      return;
   }

   if (_commands.getNewOrder())
   {
      _commands.doNewOrder();

      // rbc detector
      if ( (_orderData.cassettePosition == CHW_CASSETTE_DOWN) ||
           (_statusData.cassettePosition == HW_CASSETTE_DOWN)
           )
      {
         if ( (_orderData.redDrive != _commands.redDrive) ||
              (_orderData.greenDrive != _commands.greenDrive)
              )
         {
            _commands.redDrive   = _orderData.redDrive;
            _commands.greenDrive = _orderData.greenDrive;

            _rbcDetector.newDrive(_commands.redDrive, _commands.greenDrive);
         }
      }
      else
      {
         if ( (_commands.redDrive != 0) || (_commands.greenDrive != 0) )
         {
            _commands.redDrive   = 0;
            _commands.greenDrive = 0;

            _rbcDetector.newDrive(_commands.redDrive, _commands.greenDrive);
         }
      }

      // process pump orders
      newOrders();

      // update the software watchdog
      _lastPetCounter++;
      osTime::snapshotRawTime(_lastPetTime);

      noOrderCounter = 0;
   }
   else  // Issue warnings when we have waited too long.
   {
      const unsigned int dt = osTime::howLongMilliSec(_lastPetTime);

      if ( (dt >= SOFT_WATCHDOG_WARNING_TIME) && (_lastPetCounter > 0) )
      {
         if (noOrderCounter++ % 50 == 0) // Should log every 1/2 second
         {
            // DBG_DumpData();
            DataLog(log_level_ctl_drv_error) << "WARNING: no CHWOrders for " << dt
                                             << " ms.  WD counter is " << _lastPetCounter << endmsg;
         }
      }
   }
}



// SPECIFICATION:    timer newOrder(), process new pump orders
//
// ERROR HANDLING:   none.
void updateTimer::newOrders ()
{
   float rpm;
   int   dtAir;

   //
   // Handle forcing air to donor monitoring at power fail recovery
   //
   if (_forceAirToDonorMonitor == HW_ENABLE &&
       _fluid_seen_once != HW_ENABLE)
   {
      _fluid_seen_once = HW_ENABLE;
      DataLog_Default << "forced air-to-donor monitoring" << endmsg;
   }

   // if the cassette comes up or is ordered up, let pumps run
   // and APS messages flow
   if ( (_orderData.cassettePosition == CHW_CASSETTE_UP) ||
        (_statusData.cassettePosition != HW_CASSETTE_DOWN) )
   {
      _blockPumps    = false;
      _blockMessages = false;
   }

   // if we were previously in a stop-for-APS-violation state
   // and the pump were previously order to 0 and now have been ordered
   // to run, allow the message to be sent if the APS violation
   // re-occurs
   else if ((_blockPumps == false)
            && _blockMessages
            &&( (_orderData.inletRPM != 0.0)
                ||(_orderData.returnRPM != 0.0) ))
   {
      _blockMessages = false;
   }
   // if we are in a stop-for-APS-violations state and
   // the pumps have been ordered to 0, allow the pumps
   // but don't allow any messages until they do run
   else if (_blockPumps
            &&(_orderData.inletRPM == 0.0)
            &&(_orderData.returnRPM == 0.0))
   {
      _blockPumps    = false;
      _blockMessages = true;
      DataLog_Default << "Unblocking pumps from APS event" << endmsg;
   }

   // set AC pump order
   rpm = _orderData.acRPM;

   if (_blockPumps)                                      // if we are in APS trouble
   {
      _acPump.order(0.0f);
   }
   else if ((rpm >= 0.0f) && (rpm <=MAX_PUMP_SPEED_ORDER))
   {
      _acPump.order(rpm);
   }
   else
   {
      _acPump.order(0.0f);
      DataLog(log_level_ctl_drv_error) << "AC Pump Speed Order Out-of-Range: rpm=" << rpm << endmsg;
   }

   // set inlet pump order
   // we override the inlet pump order if the access pressure
   // is below the low APS limit.
   // gate with the same flag we use for air2donor
   rpm = _orderData.inletRPM;

   if ( (_orderData.lowAPSLimit != 0) &&
        (_fluid_seen_once == HW_ENABLE) &&
        ( (_statusData.accessPressure <= _orderData.lowAPSLimit) || // exceeds low pressure limit
          (_blockPumps) )
        )                             // or pumps have not been commanded 0
   {
      _inletPump.order(0.0f);
   }
   else if ( (rpm >= 0.0f) && (rpm <=MAX_PUMP_SPEED_ORDER) )
   {
      _inletPump.order(rpm);
   }
   else
   {
      _inletPump.order(0.0f);
      DataLog(log_level_ctl_drv_error) << "Inlet Pump Speed Order Out-of-Range: rpm=" << rpm << endmsg;
   }

   // set platelet pump order
   rpm = _orderData.plateletRPM;

   if ( (rpm >= 0.0f) && (rpm <=MAX_PUMP_SPEED_ORDER) )
   {
      _plateletPump.order(rpm);
   }
   else
   {
      _plateletPump.order(0.0f);
      DataLog(log_level_ctl_drv_error) << "Platelet Pump Speed Order Out-of-Range: rpm=" << rpm << endmsg;
   }

   // set plasma pump order
   rpm = _orderData.plasmaRPM;                                  // if we are in APS trouble

   if ( (rpm >= 0.0f) && (rpm <=MAX_PUMP_SPEED_ORDER) )
   {
      _plasmaPump.order(rpm);
   }
   else
   {
      _plasmaPump.order(0.0f);
      DataLog(log_level_ctl_drv_error) << "Plasma Pump Speed Order Out-of-Range: rpm=" << rpm << endmsg;
   }

   // set return pump order
   // we override the return pump order if there is air at the lower sensor.
   // we also override the return pump order if there is excess high pressure.
   // the first clause of the if below takes care of air2donor.
   // the second clause of the if below takes care of the high pressure.
   rpm = _orderData.returnRPM;

   // how long has it been since we saw air?
   dtAir = osTime::howLongMilliSec(_fluidsSeenTimer);

   if (( (_fluid_seen_once == HW_ENABLE)                            // seen in this procedure
         &&(dtAir < FLUIDS_SEEN_TIME )                              // seen for a while
         &&( (_statusData.cassettePosition == HW_CASSETTE_DOWN)     // cassette down OR
             ||(_orderData.cassettePosition == CHW_CASSETTE_DOWN) ) // cassette ordered down
         &&(rpm > 0.0) )                                            // ordered to run toward donor
       ||( (_orderData.highAPSLimit != 0)
           &&(_fluid_seen_once == HW_ENABLE)                           // gate with air2donor flag
           &&( (_statusData.cassettePosition == HW_CASSETTE_DOWN)      // cassette down OR
               ||(_orderData.cassettePosition == CHW_CASSETTE_DOWN) )  // cassette ordered down
           &&(rpm > 0.0)                                               // ordered to run toward donor
           &&( (_statusData.accessPressure >= _orderData.highAPSLimit) // exceeds high pressure limit
               ||(_blockPumps ) ) ))                                   // or not yet commanded to 0
   {
      _returnPump.order(0.0f);

      if (_logStopReturn)
      {
         _logStopReturn  = false;
         _logStartReturn = true;
         DataLog_Default << "Return stopped(newOrders)" << endmsg;
      }
   }
   else if ( (rpm >= -MAX_PUMP_SPEED_ORDER) && (rpm <=MAX_PUMP_SPEED_ORDER) )  // Valid range
   {
      _returnPump.order(rpm);

      if ( (rpm > 0) && _logStartReturn)
      {
         _logStartReturn = false;
         _logStopReturn  = true;
         DataLog_Default << "Return started dtAir=" << dtAir << " rpm=" << rpm << endmsg;
      }
   }
   else
   {
      _returnPump.order(0.0f);

      DataLog(log_level_ctl_drv_error) << "Return Pump Speed Order Out-of-Range: rpm=" << rpm << endmsg;
   }

   _soundInterface.changeLevel(_orderData.soundLevel);
}



// SPECIFICATION:    timer callback, processes hardware
//
// ERROR HANDLING:   none.
void updateTimer::timerCallback (void)
{
   CHW_EVENTS       event            = CHW_NORMAL_UPDATE;
   unsigned char    lowerSensorError = 0;
   unsigned char    upperSensorError = 0;
   bool             doorOverride     = false;
   bool             centOverride     = false;
   int              dt, dtAir;
   HWWatchDogStatus wdStatus;

   osTime::snapshotRawTime(_lastStartTime[_lastTimeIndex]);

   //
   // Check for shutdown ...
   //
   if (_shutdownInProgress &&
       osTime::howLongMilliSec(shutdownTime) > 5000 /*msecs*/)
   {
      abort();
   }

   //
   //  If the 24 Volts is not powered up yet ...
   //
   static bool logPowerOn     = false;
   static bool logPowerStable = false;

   if (!twentyFourVoltsOk() || _powerFailFlag)
   {
      if (_powerOnFlag)
      {
         DataLog_Default << ((_powerFailFlag) ? "Pumps responding to power fail" : "24 V out of range") << endmsg;
      }

      //
      // Grab the last time instant in time the power was not good
      //
      osTime::snapshotRawTime(_powerOnTime);
      _powerOnFlag   = false;
      logPowerOn     = false;
      logPowerStable = false;

   }
   else if (_powerOnFlag != true)
   {
      if (logPowerOn == false)
      {
         DataLog_Default << "Twenty-Four Volt power OK" << endmsg;
         logPowerOn = true;
      }

      //
      // if it has been powered on for at least 100 milliseconds ...
      //
      if (osTime::howLongMilliSec(_powerOnTime) > 100 /*msecs*/)
      {
         //
         // The power can now be considered on
         //
         _powerOnFlag = true;
         DataLog_Default << "Twenty-Four Volt power stable for 100ms" << endmsg;
      }
   }

   // service the hardware watchdog
   petTheDog();

   // Perform the following status checks only if the New MX Cntrol1&2 FPGAs are installed
   if (_newMxInstalled)
   {
      // Get the Status of the WatchDog
      wdStatus = controlWDStatus();

      // Call _FATAL_ERROR if the WatchDog is Going to Cause a Processor Reset
      if (wdStatus != hw_noFailure &&
          wdStatus != hw_powerUp)
      {

         _FATAL_ERROR(__FILE__, __LINE__, "Control: WatchDog Will Reset The Processor In Five Seconds");
      }

      // Check Status of the Seal Safe
      controlSealSafeStatus();
   }

   // check for new orders
   commandUpdate();

   pfw_timer();

   _acPump.update(_powerOnFlag);
   _inletPump.update(_powerOnFlag);
   _plateletPump.update(_powerOnFlag);
   _plasmaPump.update(_powerOnFlag);
   _returnPump.update(_powerOnFlag);

   // analog
   if (!_CHSPtr->atodError)
   {
      _atodErrorAckCounter = 0;
      processAnalog(_blockPumps, _blockMessages, _CHSPtr->atodError);
   }
   else
   {
      _atodErrorAckCounter += 1;

      if (_atodErrorAckCounter > 50)
      {
         _FATAL_ERROR(__FILE__, __LINE__, "a/d busy - error not acknowledged");
      }
   }

   _temp.update();
   _soundInterface.update();

   // valves
   getValves();

   if (twentyFourVoltsOk())
   {
      processValves(_rbcRunValve, _plasmaRunValve, _plateletRunValve, _cassetteRun, _orderData, _statusData);
      _soundInterface.actual(_statusData.soundLevel);
   }

   if (twentyFourVoltsOk())
      _statusData.valveFault = (chw_valveGetStatus(hw_allValves) & chw_valveFault) ? HW_FAULT : HW_NO_FAULT;
   else
      _statusData.valveFault = HW_NO_FAULT;

   // switches
   _stop.update();
   _pause.update();

   //
   // Fix for IT8632: centrifuge error bits in FPGA are susceptible to spurious state
   // when 64V switched power is turned on.  With each 10ms loop iteration, we first
   // reset error bits if switched power was just recently turned on.
   if (sixtyFourVoltsOk() && (_oldSixtyFourVoltSwitched < MIN_CENT_POWER ) )
   {
      chw_centClearCommutation();
      datalog_PrintToDefault(__FILE__, __LINE__, "centrifuge error bits reset at 64V power-up");
   }
   // Fix for IT 12063.  Same as above but cover the power down as well
   else if (!sixtyFourVoltsOk() && _oldSixtyFourVoltsOk)
   {
      chw_centClearCommutation();
      datalog_PrintToDefault(__FILE__, __LINE__, "centrifuge error bits reset at 64V power-down");
   }

   _oldSixtyFourVoltSwitched = _statusData.sixtyFourVoltSwitched;
   _oldSixtyFourVoltsOk      = sixtyFourVoltsOk();

   // door locks
   // if the stop button is pushed while the centrifuge is already
   // stopped, force a retry of the door lock
   if ( (_centrifuge.rpm() <= 60) &&
        (_statusData.stopSwitch == HW_SWITCH_ENABLED) )
   {
      // set to force a retry of the door solenoid
      doorOverride = true;

      // start timing to block the centrifuge command
      if ( (_doorOverrideTimer.sec == 0) &&
           (_doorOverrideTimer.nanosec == 0) )
         osTime::snapshotRawTime(_doorOverrideTimer);
   }

   _door.update(doorOverride);

   // if we are forcing a retry of the door solenoid, block
   // commands to the centrifuge for at least 3 seconds after
   // the start of the forced retry
   if ( (_doorOverrideTimer.sec != 0) ||
        (_doorOverrideTimer.nanosec != 0) )
   {
      // how much time has gone by?
      dt = osTime::howLongMilliSec(_doorOverrideTimer);

      // if it has been less than 3 seconds, override the centrifuge command
      if ((dt > 0) && (dt < HOLD_CENTRIFUGE_DURING_DOOR_RETRY_TIME))
         centOverride = true;
      else if (dt > HOLD_CENTRIFUGE_DURING_DOOR_RETRY_TIME)
      {
         // time is up, allow the centrifuge command
         centOverride               = false;
         _doorOverrideTimer.sec     = 0;
         _doorOverrideTimer.nanosec = 0;
      }
   }

   // centrifuge
   _centrifuge.update();
   _ccount += _dt;

   if (_ccount > CENTRIFUGE_TIMER)
   {
      _ccount = 0;
      _centControl.update(_centrifuge.encoderRate(),
                          (_orderData.openLoop & 0x01),
                          (sixtyFourVoltsOk() && !centOverride),
                          _statusData.centrifugeError);
   }

   // process rbc detector update from rbcPort task
   _rbcDetector.newDriveResult();
   _rbcDetector.read(_statusData.rbcCommError);

   // process reservoir indication
   if (ctl_interruptLowerSensor(&lowerSensorError))
   {
      if (ctl_interruptUpperSensor(&upperSensorError))
      {
         _statusData.reservoir = CHW_RESERVOIR_HIGH;   // high
      }
      else
      {
         _statusData.reservoir = CHW_RESERVOIR_MIDDLE; // middle
      }
   }
   else
   {
      if (ctl_interruptUpperSensor(&upperSensorError))
      {
         _statusData.reservoir = CHW_RESERVOIR_ERROR;  // error
      }
      else
      {
         _statusData.reservoir = CHW_RESERVOIR_EMPTY;  // empty
      }
   }

   _lsAir.dataPoint((short)ctl_interruptLowerSensorAir());
   _lsFluid.dataPoint((short)ctl_interruptLowerSensorFluid());
   _usAir.dataPoint((short)ctl_interruptUpperSensorAir());
   _usFluid.dataPoint((short)ctl_interruptUpperSensorFluid());

   if (lowerSensorError != 0 && !_shutdownInProgress)
      _FATAL_ERROR(__FILE__, __LINE__, "Lower Ultrasonics Register Failure");
   if (upperSensorError != 0 && !_shutdownInProgress)
      _FATAL_ERROR(__FILE__, __LINE__, "Upper Ultrasonics Register Failure");

   // air to donor processing
   if (ctl_interruptLowerSensor(&lowerSensorError))
   {
      if (_fluid_seen_once == HW_DISABLE)
         DataLog_Default << "air-to-donor monitoring enabled" << endmsg;

      _fluid_seen_once = HW_ENABLE;
   }
   else
      // if there is air at the lower sensor, take a time stamp
      osTime::snapshotRawTime(_fluidsSeenTimer);

   if (lowerSensorError != 0)
      _FATAL_ERROR(__FILE__, __LINE__, "Lower Ultrasonics Register Failure");

   // stop return pump for air to donor
   if (_fluid_seen_once == HW_ENABLE)                     // fluid seen once
   {
      // how long has it been since we saw air?
      dtAir = osTime::howLongMilliSec(_fluidsSeenTimer);
      if ((dtAir < FLUIDS_SEEN_TIME ) &&                 // for a short time
          ( _orderData.returnRPM > 0.0) &&               // ordered returning
          ( (_orderData.cassettePosition == CHW_CASSETTE_DOWN)||
            (_statusData.cassettePosition == HW_CASSETTE_DOWN) ))
      {
         _returnPump.order(0.0f);

         if (_logStopReturn)
         {
            _logStopReturn  = false;
            _logStartReturn = true;
            DataLog_Default << "Return stopped(timerCallback)" << endmsg;
         }
      }
   }

   // stop pumps for excessive high access pressure
   if ( (_orderData.highAPSLimit != 0)  &&
        (_fluid_seen_once == HW_ENABLE) &&
        _blockPumps)
   {
      _returnPump.order(0.0f);
      _inletPump.order(0.0f);
      _acPump.order(0.0f);
   }

   // stop pumps for excessive low access pressure
   if ( (_orderData.lowAPSLimit != 0) &&
        (_fluid_seen_once == HW_ENABLE) &&
        _blockPumps)
   {
      _inletPump.order(0.0f);
      _returnPump.order(0.0f);
      _acPump.order(0.0f);
   }

   // check that OS tick timing is functioning correctly
   monitorTickTiming();

   // check the software watchdog
   checkTheDog();

   // event processing
   CHW_AC_DETECTOR_STATES acValue = (chw_acDetectorGetStatus() & chw_acDetectorFluid) ?
                                    CHW_AC_DETECTOR_FLUID : CHW_AC_DETECTOR_AIR;
   if (chw_acDetectorGetStatus() & chw_acDetectorFluid)
   {
      if (_statusData.acDetector != CHW_AC_DETECTOR_FLUID)
      {
         //
         // Must see fluid continously for 200 msec to report fluid
         // at AC detector.
         //
         if (osTime::howLongMilliSec(_acAirToFluidTimer) >= 200)
         {
            _statusData.acDetector = CHW_AC_DETECTOR_FLUID;
            event                  = CHW_AC_EVENT;
         }
      }
   }
   else
   {
      osTime::snapshotRawTime(_acAirToFluidTimer);
      if (_statusData.acDetector != CHW_AC_DETECTOR_AIR)
      {
         _statusData.acDetector = CHW_AC_DETECTOR_AIR;
         event                  = CHW_AC_EVENT;
      }
   }

   if ((_oldPauseSwitch != _statusData.pauseSwitch)
       &&(_statusData.event != CHW_PAUSE_EVENT))
   {
      _oldPauseSwitch = _statusData.pauseSwitch;
      if (_statusData.pauseSwitch == HW_SWITCH_ENABLED)
         event = CHW_PAUSE_EVENT;
   }

   if ((_oldStopSwitch != _statusData.stopSwitch)
       &&(_statusData.event != CHW_STOP_EVENT))
   {
      _oldStopSwitch = _statusData.stopSwitch;
      if (_statusData.stopSwitch == HW_SWITCH_ENABLED)
         event = CHW_STOP_EVENT;
   }

   // don't report if already reported
   if ((_oldReservoir != _statusData.reservoir) &&
       (_statusData.event != CHW_RESERVOIR_EVENT))
   {
      _oldReservoir = _statusData.reservoir;
      event         = CHW_RESERVOIR_EVENT;
   }

   // report actuals every 500 ms
   _count += _dt;


   ///////////////////////////////////////////////////////////////////////////////////////
   //
   // This is the 500 ms update section.  Everything inside is only run every half second.
   //
   ///////////////////////////////////////////////////////////////////////////////////////
   if ( atodCycleCount >= 2 &&
        ( ( (_count > STATUS_RATE) && (event == CHW_NORMAL_UPDATE) ) ||
          (event != CHW_NORMAL_UPDATE)
        )
        )
   {
      // update pump values for normal updates and reservoir events
      if ( (event == CHW_NORMAL_UPDATE) || (event == CHW_RESERVOIR_EVENT) )
      {
         _acPump.actuals(&_statusData.acPump);
         _inletPump.actuals(&_statusData.inletPump);
         _plateletPump.actuals(&_statusData.plateletPump);
         _plasmaPump.actuals(&_statusData.plasmaPump);
         _returnPump.actuals(&_statusData.returnPump);
      }
      else                    // don't report revs twice
      {
         _statusData.acPump.Revs       = 0.0f;
         _statusData.inletPump.Revs    = 0.0f;
         _statusData.plateletPump.Revs = 0.0f;
         _statusData.plasmaPump.Revs   = 0.0f;
         _statusData.returnPump.Revs   = 0.0f;
      }

      // process centrifuge
      _statusData.centrifugeRPM = _centrifuge.rpm();

      _centrifuge.errorStatus(sixtyFourVoltsOk(), _statusData.centrifugeError);

      // report pump motor controller fault only when 24V is on
      if ( (chw_pumpGetStatus(hw_allPumps) & chw_pumpFault) && twentyFourVoltsOk())
      {
         _statusData.motorFault = HW_FAULT;
      }
      else
      {
         _statusData.motorFault = HW_NO_FAULT;
      }

      // send status
      static struct timespec sendTime; // time msg sent
      clock_gettime(CLOCK_REALTIME, &sendTime);

      _statusData.event     = event;
      _statusData.timestamp = sendTime;
      _statusData.msgCount++;

      // Queue up the Status data to the ctl_msg task in the Control status shared memory block
      dataToGo();

      if (!event)
      {
         // sent for periodic update (rather than asynchronous event), so reset
         // count for next periodic update
         _count = 0;
      }
   }

   osTime::snapshotRawTime(_lastEndTime[_lastTimeIndex]);
   _lastTimeIndex = ++_lastTimeIndex % TimingHistorySize;
}



//
// This function sends status data to the ctl_msg task
//
void updateTimer::dataToGo ()
{
   static rawTime lastEnqueueTime;
   static bool    firstTime = true;

   if (firstTime)
   {
      // Init the last Rx time
      osTime::snapshotRawTime(lastEnqueueTime);
      firstTime = false;
   }

   // copy the data into shared memory
   memcpy(&_CHSPtr->msg[_CHSPtr->NextIn].status, &_statusData, sizeof(CHwStates));

   // generate a crc
   _CHSPtr->msg[_CHSPtr->NextIn].crc = 0;

   if (CRCGEN32_STRUCT(&_CHSPtr->msg[_CHSPtr->NextIn].crc, &_CHSPtr->msg[_CHSPtr->NextIn]) != 0)
   {
      _FATAL_ERROR(__FILE__, __LINE__, "crcgen32");
   }
   else
   {
      // test for lapping NextOut
      if (_CHSPtr->NextIn + 1 == _CHSPtr->NextOut)
         _FATAL_ERROR(__FILE__, __LINE__, "controlStatus NextIn lapping NextOut!");

      // increment NextIn index
      if (_CHSPtr->NextIn == NUM_CHWSTATUS - 1)
         _CHSPtr->NextIn = 0;
      else
         _CHSPtr->NextIn++;
   }

   const int dt = osTime::howLongMilliSecAndUpdate(lastEnqueueTime);

   // This enqueue should occur at a 500 ms frequency.  Since it's timing is based on the 10 ms
   // timer, don't tolerate much slop
   if (dt > 600)
   {
      DataLog(log_level_ctl_drv_error) << "WARNING: SHWOrders enqueue delay of "
                                       << dt << " ms." << endmsg;
   }
}



// SPECIFICATION:    timer message,
//                   check the software watchdog timer
//
// ERROR HANDLING:   none.
void updateTimer::checkTheDog ()
{
   int         dt;
   static bool powerHasBeenEnabled = false;
   static int  lastIn, lastOut = 0;


   if (!powerHasBeenEnabled           &&
       pump::checkSafetyPumpEnabled() &&
       twentyFourVoltsOk())
   {
      datalog_PrintToDefault(__FILE__, __LINE__, "soft watchdog monitor enabled");
      powerHasBeenEnabled = true;
   }

   // don't start monitoring until we've seen 24V power switch on
   // at least once
   if (powerHasBeenEnabled && !_shutdownInProgress)
   {
      dt = osTime::howLongMilliSec(_lastPetTime);

      // Basic Logic of checking for the Control Side soft watchdog
      //
      // If the watchdog dt is beyond the MAX time then FATAL ERROR
      // If the watchdog dt is beyond the 2 second boundary then pause the pumps and wait for commands
      // If the watchdog dt is beyond the 1 second boundary then log the command timestamp history
      // If the watchdog dt is negative, then FATAL ERROR
      // If the watchdog dt is short AND we are in the pause state, then go back to normal

      // If the time since the last pet exceeds the max time, game over!
      if ( (dt >= MAX_SOFT_WATCHDOG_FATAL_ERROR_TIME) && !_CHSPtr->controlOrdersPastMaxThreshold)
      {
         DataLog(log_level_critical) << "Timing slip for soft watchdog of " << dt << " ms" << endmsg;
         DataLog(log_level_critical) << "soft watchdog hit - Control command daisy chain failed." << endmsg;

         // Tell ctl_msg to generate the alarm.
         _CHSPtr->controlOrdersPastMaxThreshold = true;

         _softWatchdogActive = true;                    // Block new Orders

         // Pumps should all be off but re-issue order just in case
         _acPump.order(0.0);
         _inletPump.order(0.0);
         _plasmaPump.order(0.0);
         _plateletPump.order(0.0);
         _returnPump.order(0.0);
         _centControl.newOrder(0.0, 20.0, 100.0);  // Up and down taken from cobe_config standard rates
      }
      else if ( (dt >= MAX_SOFT_WATCHDOG_TIME) && !_CHSPtr->controlOrdersPastThreshold)
      {
         // No commands from Proc within timeout period.
         // Pause pumps and wait for proc to come back alive.
         DataLog_Default << "Timing slip for soft watchdog of " << dt
                         << " ms.  Pausing pumps until communication is re-established" << endmsg;

         // Tell ctl_msg to generate the alarm.
         _CHSPtr->controlOrdersPastThreshold = true;

         _acPump.order(0.0);
         _inletPump.order(0.0);
         _plasmaPump.order(0.0);
         _plateletPump.order(0.0);
         _returnPump.order(0.0);

         osTime::snapshotRawTime(_timeInSafeState); // Track for logging purposes
      }
      else if (!_softWatchdogActive && (dt >= SOFT_WATCHDOG_WARNING_TIME) )
      {
         const CHO* cho = commands::getCHO();

         // Don't repeatedly log the same condition over and over again
         if ( (lastIn == cho->NextIn) && (lastOut == cho->NextOut) )
         {
            return;
         }

         lastIn  = cho->NextIn;
         lastOut = cho->NextOut;

         // Extra logging to troubleshoot Control Side soft watchdog failure
         datalog_Print(log_handle_ctl_drv_error, __FILE__, __LINE__, "Timing slip for soft watchdog of %d ms.  Orders in/out %d/%d\n", dt, cho->NextIn, cho->NextOut);

         int j = ( (cho->NextIn - 1) == -1) ? NUM_CHWORDERS - 1 : (cho->NextIn - 1);

         for (int i = 0; i < NUM_CHWORDERS; i++)
         {
            const CHOEntry& entry    = const_cast<CHOEntry&>(cho->msg[j]);
            const rawTime&  created  = entry.orders.timeOrdersCreated;
            const rawTime&  enqueued = entry.timeOrdersEnqueued;
            const rawTime&  dequeued = entry.timeOrdersDequeued;

            const int delta1 = ( (enqueued.sec - created.sec) * 1000) + ( (enqueued.nanosec - created.nanosec) / 1000000);
            const int delta2 = ( (dequeued.sec - enqueued.sec) * 1000) + ( (dequeued.nanosec - enqueued.nanosec) / 1000000);

            DataLog_Default << "CHO entry " << j << " count " << entry.count << " CtoE " << delta1 << " ms EtoD " << delta2 << " ms " << endmsg;

            j = ( (--j == -1) ? NUM_CHWORDERS - 1 : j);
         }
      }
      else if (dt < 0)
      {
         datalog_Print(log_handle_critical, __FILE__, __LINE__, "raw time %d %lu %lu", dt, _lastPetTime.sec, (unsigned long)_lastPetTime.nanosec);
         _FATAL_ERROR(__FILE__, __LINE__, "raw time went backward");
      }
      else if ( (dt < SOFT_WATCHDOG_WARNING_TIME) && _CHSPtr->controlOrdersPastThreshold)
      {
         dt = osTime::howLongMilliSec(_timeInSafeState);
         DataLog_Default << "Temporary Safe State canceled.  DT=" << dt << "ms.  Canceling pumps paused state." << endmsg;
         _CHSPtr->controlOrdersPastThreshold = false;
      }
   }
}



void updateTimer::monitorTickTiming (void)
{
   //
   // Check timing accuracy between OS timers and internal ultrasonics timing.
   // A failure indicates that either OS is using the wrong time base, or that
   // the ultrasonics are not working correctly.  In either of these cases,
   // we need to generate a safety alarm.
   //
   if (_mtt_startupDelay > 0)
   {
      //
      // Delay startup to allow internal timing to stabilize
      //
      _mtt_startupDelay -= 1;
      if (_mtt_startupDelay == 0)
      {
         _mtt_accInterval      = 0;
         _mtt_oldLowerAccTotal = ctl_interruptLowerSensorAccTotal();
         _mtt_oldUpperAccTotal = ctl_interruptUpperSensorAccTotal();
         osTime::snapshotRawTime(_mtt_lastCheckTime);
      }
   }
   else if (!_shutdownInProgress)
   {
      _mtt_accInterval += 1;
      if (_mtt_accInterval >= 1000)
      {
         //
         // If a timingError was detected on the last scan, it better
         // have been acknowledged by ctl_msg by now.  If not, shutdown
         // the control driver with a fatal error.
         //
         if (_CHSPtr->timingError)
         {
            _FATAL_ERROR(__FILE__, __LINE__, "Unacknowledged timing error");
            _CHSPtr->timingError = false;
         }

         unsigned long newLowerAccTotal = ctl_interruptLowerSensorAccTotal();
         unsigned long newUpperAccTotal = ctl_interruptUpperSensorAccTotal();
         unsigned long lowerDiff        = newLowerAccTotal - _mtt_oldLowerAccTotal;
         unsigned long upperDiff        = newUpperAccTotal - _mtt_oldUpperAccTotal;
         int           elapsedRawTime   = osTime::howLongMicroSecAndUpdate(_mtt_lastCheckTime);

         //
         // Check for the accuracy of the interval timing by comparing the
         // expected number of ultrasonic register counts with the raw tick time
         // reported by _timeTeller.  The ultrasonics registers should get
         // 1 count every 256 uSec.  We allow a +/- 1% error in timing (based
         // on the expected pump error of up to 6%, this should be sufficient).
         //
         unsigned long expectedAcc   = elapsedRawTime / 256;
         unsigned long minAllowedAcc = (expectedAcc * 99) / 100;
         unsigned long maxAllowedAcc = (expectedAcc * 101) / 100;

         if (lowerDiff < minAllowedAcc || lowerDiff > maxAllowedAcc)
         {
            _CHSPtr->timingError = true;

            DataLog(log_level_control_alarm_detail)
               << "lower timing error: elapsedRawTime=" << elapsedRawTime
               << " expectedAcc=" << expectedAcc
               << " minAllowedAcc=" << minAllowedAcc
               << " maxAllowedAcc=" << maxAllowedAcc
               << " oldLower=" << _mtt_oldLowerAccTotal
               << " newLower=" << newLowerAccTotal
               << " diffLower=" << lowerDiff << endmsg;
         }

         if (upperDiff < minAllowedAcc || upperDiff > maxAllowedAcc)
         {
            _CHSPtr->timingError = true;

            DataLog(log_level_control_alarm_detail)
               << "upper timing error: elapsedRawTime=" << elapsedRawTime
               << " expectedAcc=" << expectedAcc
               << " minAllowedAcc=" << minAllowedAcc
               << " maxAllowedAcc=" << maxAllowedAcc
               << " oldUpper=" << _mtt_oldUpperAccTotal
               << " newUpper=" << newUpperAccTotal
               << " diffUpper=" << upperDiff << endmsg;
         }

         _mtt_accInterval      = 0;
         _mtt_oldLowerAccTotal = newLowerAccTotal;
         _mtt_oldUpperAccTotal = newUpperAccTotal;
      }
   }
}

/* FORMAT HASH 83179b07a89013d2cf97f98a4d00fe61 */

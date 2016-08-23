/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      ctl_msg.cpp Trima task to manage urgent and high-priority
 *             messages used by the control driver
 *
 */

#include <vxWorks.h>
#include <sysLib.h>

#include "an_alarm.h"
#include "clearablealarm.h"
#include "ostime.hpp"
#include "chw.hpp"
#include "cmsginfo.hpp"
#include "crc_util.h"
#include "messagesystem.h"
#include "sh_mem.h"
#include "shw.hpp"
#include "timermessage.h"
#include "trima_datalog.h"
#include "trimamessages.h"
#include "trima_tasks.h"


class ctlMsgHandler
{
public:
   ctlMsgHandler(void);

   virtual ~ctlMsgHandler();

   void periodicUpdate (void);

protected:
   void newControlOrders (void);
   void newSafetyOrders (void);
   void forceAirToDonorMonitor (void);
   void enqueueNewOrder (const CHwOrders& order);

private:
   void updateControlStatus (void);
   void updateAPSFilter (void);
   void updateControlStatusTimestamp (void);

private:
   CHO*                         _CHOPtr;
   unsigned int                 _controlOrdersCount;
   Message<CHwOrders>           _controlOrdersMessage;

   SHwOrders*                   _safetyOrders;
   Message<SHwOrders>           _safetyOrdersMessage;

   CHS*                         _CHSPtr;
   CHwStates                    _currentControlStatus;
   Message<CHwStates>           _controlStatusMessage;

   FASTFIL*                     _APSPtr;
   Message<FASTFILTER>          _apsFilterMessage;

   PFRForceAirToDonorMonitorMsg _forceAirToDonorMonitor;

   SetCtlStatusSentTimestamp    _ctlStatusTimestampMsg; // Used by TED in Service Mode

   ClearableAlarm               _preWatchdogPause;

   bool                         _preWatchdogPauseAlarmRaised;
   bool                         _ControlSoftWatchdogAlarmRaised;
};



ctlMsgHandler::ctlMsgHandler(void)
   : _controlOrdersMessage(Callback<ctlMsgHandler>(this, &ctlMsgHandler::newControlOrders), MessageBase::SNDRCV_RECEIVE_ONLY),
     _safetyOrdersMessage(Callback<ctlMsgHandler>(this, &ctlMsgHandler::newSafetyOrders), MessageBase::SNDRCV_RECEIVE_ONLY),
     _forceAirToDonorMonitor(Callback<ctlMsgHandler>(this, &ctlMsgHandler::forceAirToDonorMonitor), MessageBase::SNDRCV_RECEIVE_ONLY),
     _ctlStatusTimestampMsg(Callback<ctlMsgHandler>(this, &ctlMsgHandler::updateControlStatusTimestamp), MessageBase::SNDRCV_RECEIVE_ONLY),
     _controlStatusMessage(MessageBase::SEND_GLOBAL),
     _apsFilterMessage(MessageBase::SEND_LOCAL),
     _preWatchdogPause(CONTROL_PRE_WATCHDOG_PAUSE),
     _preWatchdogPauseAlarmRaised(false),
     _ControlSoftWatchdogAlarmRaised(false)
{
   //
   // Link to the shared memory area for control orders
   //
   _CHOPtr = (CHO*)shmem_open("controlOrder");

   if (!_CHOPtr)
      _FATAL_ERROR(__FILE__, __LINE__, "controlOrder shmem_open failed");

   _controlOrdersCount = 0;

   //
   // Link to the shared memory area for safety orders used by c_pump
   //
   _safetyOrders = (SHwOrders*)shmem_open("safetyOrders");

   if (!_safetyOrders)
      _FATAL_ERROR(__FILE__, __LINE__, "safetyOrders shmem_open failed");

   //
   // Link to the shared memory area for control status
   //
   _CHSPtr = (CHS*)shmem_open("controlStatus");

   if (!_CHSPtr)
      _FATAL_ERROR(__FILE__, __LINE__, "controlStatus shmem_open failed");

   //
   // Link to shared memory area for fast APS data
   //
   _APSPtr = (FASTFIL*)shmem_open("apsFilter");

   if (!_APSPtr)
      _FATAL_ERROR(__FILE__, __LINE__, "apsFilter shmem_open failed");
}



ctlMsgHandler::~ctlMsgHandler(void)
{
   //
   // Close the shared memory area for control orders
   //
   shmem_close("controlOrder");

   //
   // Close the shared memory area for safety orders
   //
   shmem_close("safetyOrders");

   //
   // Close the shared memory area for control status
   //
   shmem_close("controlStatus");

   //
   // Close the shared memory area for fast APS data
   //
   shmem_close("apsFilter");
}



//
// This function is called from the CHWOrders message handler and effectively copies the message to
// the ctl_drv shared memory message area.
//
void ctlMsgHandler::enqueueNewOrder (const CHwOrders& order)
{
   static rawTime lastEnqueueTime, enqueueTime;
   static bool    firstTime = true;

   if (firstTime)
   {
      // Init the last enqueue time
      osTime::snapshotRawTime(lastEnqueueTime);
      enqueueTime = lastEnqueueTime;
      firstTime   = false;
   }
   else
   {
      // Snapshot the enqueue time to debug soft watchdog errors
      osTime::snapshotRawTime(enqueueTime);
   }

   //
   // store new orders for control driver
   //
   _controlOrdersCount                += 1;
   _CHOPtr->msg[_CHOPtr->NextIn].count = _controlOrdersCount;
   memcpy(&_CHOPtr->msg[_CHOPtr->NextIn].orders, &order, sizeof(CHwOrders));

   // Snapshot the enqueue time to debug soft watchdog errors
   _CHOPtr->msg[_CHOPtr->NextIn].timeOrdersEnqueued = enqueueTime;

   // generate a crc
   _CHOPtr->msg[_CHOPtr->NextIn].crc = 0;

   if (CRCGEN32_STRUCT(&_CHOPtr->msg[_CHOPtr->NextIn].crc, &_CHOPtr->msg[_CHOPtr->NextIn]) != 0)
   {
      _FATAL_ERROR(__FILE__, __LINE__, "controlOrders crc generation failure");
   }
   else if (!_ControlSoftWatchdogAlarmRaised)  // Don't perform this test if the driver has shut down
   {
      // test for lapping NextOut
      if (_CHOPtr->NextIn + 1 == _CHOPtr->NextOut)
         _FATAL_ERROR(__FILE__, __LINE__, "controlOrders overrun");

      // increment NextIn index
      if (_CHOPtr->NextIn >= NUM_CHWORDERS - 1)
         _CHOPtr->NextIn = 0;
      else
         _CHOPtr->NextIn++;
   }

   const int dt = osTime::howLongMilliSecAndUpdate(lastEnqueueTime);

   // This message should occur at a 500 ms frequency.  800 seems like too much.
   if (dt > 800)
   {
      Dispatcher& dispatcher = MessageSystem::MsgSystem()->dispatcher();

      DataLog(log_level_ctl_msg_error) << "WARNING: CHWOrders message enqueue delay of "
                                       << dt << " ms.  " << dispatcher.messageRemaining() << " messages remaining." << endmsg;
   }
}



//
// CHwOrder message handler
//
void ctlMsgHandler::newControlOrders (void)
{
   //
   // get data and header information
   //
   const CHwOrders newOrders = _controlOrdersMessage.getData();

   //
   // store new orders for control driver
   //
   enqueueNewOrder(newOrders);
}



//
// SHwOrder message handler
//
void ctlMsgHandler::newSafetyOrders (void)
{
   static rawTime lastRxTime;
   static bool    firstTime            = true;
   static int     lastRxSequenceNumber = 1; // Start off a the first number exected to reduce logging.


   if (firstTime)
   {
      // Init the last Rx time
      osTime::snapshotRawTime(lastRxTime);
      firstTime = false;
   }

   //
   // get data and header information
   //
   // NOTE:  This is kind of stupid.  All c_pump needs is the power order but we are copying the
   // entire struct.  If we ever did more than this, we would want a semaphore to protect the data.
   // Seems best to rewrite this to only share out what is needed and this may not be the best way to do
   // it.
   //
   const int dt = osTime::howLongMilliSecAndUpdate(lastRxTime);

   *_safetyOrders = _safetyOrdersMessage.getData();

   // Check for out of order packets
   if (_safetyOrders->sequenceNumber != lastRxSequenceNumber + 1)
   {
      DataLog(log_level_ctl_msg_error) << "SHWOrders message received out of sequence.  Expected "
                                       << lastRxSequenceNumber + 1 << ".  Received " << _safetyOrders->sequenceNumber << endmsg;
   }
   // This message should occur at a 500 ms frequency.  Since it comes over the network from Safety
   // Allow a bigger delay due to the non-critical nature of the data
   else if (dt > 1000)
   {
      Dispatcher& dispatcher = MessageSystem::MsgSystem()->dispatcher();

      DataLog(log_level_ctl_msg_error) << "WARNING: SHWOrders message receive delay of "
                                       << dt << " ms.  " << dispatcher.messageRemaining() << " messages remaining." << endmsg;
   }

   lastRxSequenceNumber = _safetyOrders->sequenceNumber;
}


//
// This function runs at a 10 ms nominal rate
//
void ctlMsgHandler::updateControlStatus (void)
{
   const int      PRE_WATCHDOG_ALARM_SET_TIME = 2000; // 2 seconds

   static rawTime _preSoftWatchDogAlarmSetTime;   // time Pre Soft Watchdog alarm was raised

   //
   // Check the fault conditions at 10 ms rate
   //
   if (_CHSPtr->timingError)
   {
      anAlarmMsg timingAlarm(INTERNAL_TIMING_ERROR);
      timingAlarm.setAlarm("Internal timing error alarm");

      _CHSPtr->timingError = false;
   }

   if (_CHSPtr->atodError)
   {
      anAlarmMsg atodAlarm(INTERNAL_ATOD_ERROR);
      atodAlarm.setAlarm("Internal A/D error alarm");

      _CHSPtr->atodError = false;
   }

   if (!_preWatchdogPauseAlarmRaised && _CHSPtr->controlOrdersPastThreshold)
   {
      _preWatchdogPause.setAlarm("Pre watchdog pause");
      _preWatchdogPauseAlarmRaised = true;

      DataLog_Default << "Setting PreWatchdog alarm" << endmsg;

      DataLog_Default << "Node Router Queue size is " << trimaNodeRouterQueueSize() << endmsg;
      trimaReportTaskQueueSizes();

      // osTime::snapshotRawTime(_preSoftWatchDogAlarmSetTime);  // capture time alarm was raised
   }
   else if (_preWatchdogPauseAlarmRaised && !_CHSPtr->controlOrdersPastThreshold)
   {
      DataLog_Default << "Pre watchdog pause alarm cleared" << endmsg;
      _preWatchdogPause.clearAlarm();
      _preWatchdogPauseAlarmRaised = false;
   }
#if 0   // Really need a way to determine if Alarm was really raised
   else if ( _preWatchdogPauseAlarmRaised &&
             (osTime::howLongMilliSec(_preSoftWatchDogAlarmSetTime) > PRE_WATCHDOG_ALARM_SET_TIME)
             )
   {
      DataLog(log_level_critical) << "Pre Soft Watchdog alarm not raised within " << PRE_WATCHDOG_ALARM_SET_TIME << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "Pre Soft Watchdog alarm not set");
   }
#endif

   if (!_ControlSoftWatchdogAlarmRaised && _CHSPtr->controlOrdersPastMaxThreshold)
   {
      _ControlSoftWatchdogAlarmRaised = true;

      anAlarmMsg controlSoftWatchdogAlarm(CONTROL_SOFT_WATCHDOG);
      controlSoftWatchdogAlarm.setAlarm("Control soft watchdog alarm");
   }

   //
   // Check to see if there is a new status message in the queue
   //
   if (_CHSPtr->NextIn != _CHSPtr->NextOut)
   {
      unsigned long checksum = 0;

      CRCGEN32_STRUCT(&checksum, &_CHSPtr->msg[_CHSPtr->NextOut]);

      if (checksum == _CHSPtr->msg[_CHSPtr->NextOut].crc)
      {
         // Calculate time deltas between ctl_drv enqueing and now (ctl_msg dequeing)
         struct timespec sendTime; // time msg sent
         clock_gettime(CLOCK_REALTIME, &sendTime);

         const struct timespec timeStamp               = _CHSPtr->msg[_CHSPtr->NextOut].status.timestamp;

         const long            CtlDrvToCtlMsgDeltaTime = ( (sendTime.tv_sec - timeStamp.tv_sec) * 1000) +     // sec to ms
                                                         ( (sendTime.tv_nsec - timeStamp.tv_nsec) / 1000000); // nsec to ms

         // copy the message and send it off to message consumers.
         _currentControlStatus = _CHSPtr->msg[_CHSPtr->NextOut].status;
         _controlStatusMessage.send(_currentControlStatus);

         //
         // Check to see if there is a back-up from the message being generated by the control driver
         // to being sent by the ctl_msg task
         //
         // If the delay exceed 100 ms, log it.
         //
         if ( (CtlDrvToCtlMsgDeltaTime > 100) || (CtlDrvToCtlMsgDeltaTime < 0) )
         {
            DataLog(log_level_ctl_msg_error) << "WARNING: CHWStates messages are backing up. deltaTime="
                                             << CtlDrvToCtlMsgDeltaTime << " ms.  MsgCount=" << _currentControlStatus.msgCount << endmsg;
         }
      }
      else
      {
         DataLog(log_level_critical) << hex << "CRC check error: generated=0x" << checksum
                                     << " expected=0x" << _CHSPtr->msg[_CHSPtr->NextOut].crc << dec << endmsg;

         _FATAL_ERROR(__FILE__, __LINE__, "crc");
      }

      if (_CHSPtr->NextOut >= NUM_CHWSTATUS - 1)
         _CHSPtr->NextOut = 0;
      else
         _CHSPtr->NextOut++;
   }
}



void ctlMsgHandler::updateAPSFilter (void)
{
   if (_APSPtr->NextIn != _APSPtr->NextOut)
   {
      unsigned long checksum = 0;

      CRCGEN32_STRUCT(&checksum, &_APSPtr->msg[_APSPtr->NextOut]);

      if (checksum == _APSPtr->msg[_APSPtr->NextOut].crc)
      {
         _apsFilterMessage.send(_APSPtr->msg[_APSPtr->NextOut].data);
      }
      else
      {
         DataLog(log_level_critical) << hex << "CRC check error: generated=0x" << checksum
                                     << " expected=0x" << _APSPtr->msg[_APSPtr->NextOut].crc << dec << endmsg;

         _FATAL_ERROR(__FILE__, __LINE__, "crc");
      }

      if (_APSPtr->NextOut >= NUM_FASTFILT - 1)
         _APSPtr->NextOut = 0;
      else
         _APSPtr->NextOut++;
   }
}



void ctlMsgHandler::periodicUpdate (void)
{
   static rawTime _lastUpdateTime;   // last time timer loop ran
   static bool    firstTime = true;

   if (firstTime)
   {
      osTime::snapshotRawTime(_lastUpdateTime);
      firstTime = false;
   }

   const int deltaTime = osTime::howLongMilliSecAndUpdate(_lastUpdateTime);

   // Time chosen arbitrarily.  Nominal time is 10 ms.  50 seems too long for a high frequency timer.
   if (deltaTime > 50)
   {
      DataLog(log_level_ctl_msg_error) << "WARNING: ctl_msg periodic timer too long.  Delta time is " << deltaTime
                                       << " ms." << endmsg;
   }

   updateControlStatus();
   updateAPSFilter();
}



void ctlMsgHandler::forceAirToDonorMonitor (void)
{
   _CHOPtr->forceAirToDonorMonitor = HW_ENABLE;
}



void ctlMsgHandler::updateControlStatusTimestamp (void)
{
   osTime::snapshotRawTime(_CHSPtr->statusSentTimestamp);
}



#define TEN_MILLISECONDS 10000


//
// Main entry point.  Demangle the name for easy calling in Trima Tasks
//
extern "C" void ctl_msg (void)
{
   datalog_SetDefaultLevel(log_handle_ctl_msg_info);

   //
   // Create message system
   //
   MessageSystem msgSystem;
   msgSystem.initNonBlock("ctl_msg");
   trimaSetRouterQ(taskIdSelf(), msgSystem.MsgSystem());
   ctlMsgHandler msgHandler;

   //
   // Attach to aux clock periodic update at a rate of 10 ms
   //
   SEM_ID tmrSem = auxClockSemaphoreAttach(TEN_MILLISECONDS, AuxClockBinarySemaphore);

   if (!tmrSem)
   {
      _FATAL_ERROR(__FILE__, __LINE__, "auxClockSemaphoreAttach failed");
   }

   while (1)
   {
      semTake(tmrSem, WAIT_FOREVER);

      msgHandler.periodicUpdate();
      msgSystem.dispatchMessages();
   }
}

/* FORMAT HASH 7e0091fe7a948d3db7743a2d72711e85 */

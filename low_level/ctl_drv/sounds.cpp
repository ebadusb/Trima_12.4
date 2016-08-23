/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * ABSTRACT:   Interface to the pc speaker hardware.  The software interface
 * is described in sounds.hpp.   This module reads a sound data
 * file containing a list of frequencies and durations for each type
 * of alarm, alert or other operator indication.  This module then manages
 * requests (using the focus messaging system) and queues these requests.
 * A ten millisecond timer is used to run through the request
 * table and the corresponding series of tones is played.  When completed,
 * the next requested sound is played.
 *
 */

#include <vxWorks.h>
#include <intLib.h>
#include <sysLib.h>

#include "message.h"
#include "messagesystem.h"
#include "ostime.hpp"
#include "sounds.hpp"
#include "soundsdat.h"
#include "alarms.h"
#include "an_alarm.h"
#include "trima_datalog.h"
#include "filenames.h"
#include "boot_mode.h"

//
// Main class for controlling sound generation
//
class soundHandler
{
public:
   soundHandler(void);
   virtual ~soundHandler();

   void orderCallback (void);
   void timerCallback (void);

private:
   enum { DIAG_CTRL = 0x61, PIT_BASE_ADDR = 0x40 };

   void startTone (short freq);
   void stopTone (void);
   void processImmediate (void);

private:
   //
   // Define message for incoming sound commands
   //
   Message<SoundOrders> _orderMsg;

   //
   // Information for sound generation
   //
   unsigned  _orderIndex;            // order being handled currently
   ToneData* _toneList;              // tone list for current order
   int       _toneIndex;             // current index into _toneList
   short     _duration;              // remaining duration for current tone list entry
   bool      _toneEnabled;
   bool      _soundOn;

   //
   // Data and current command mode for each sound type
   //
   SOUND_MODE orderData[MAX_SOUNDS];
};

soundHandler::soundHandler(void)
   : _orderMsg(Callback<soundHandler>(this, &soundHandler::orderCallback), MessageBase::SNDRCV_RECEIVE_ONLY),
     _orderIndex(0),
     _toneList(NULL),
     _toneIndex(0),
     _duration(0),
     _toneEnabled(false),
     _soundOn(true)
{
   for (int k = 0; k < MAX_SOUNDS; k++)
   {
      orderData[k] = SOUND_CLEAR;
   }

   // Turn off sound if file /config/no_sound exists
   FILE* fp = fopen(SOUND_OFF, "r");

   if (fp)
   {
      _soundOn = false;
      fclose(fp);
   }

   if (bootKeyboardAttached())
   {
      _soundOn = false;
   }

   DataLog(log_level_sounds_info) << "Sound On: " << _soundOn << endmsg;
}



soundHandler::~soundHandler()
{}



void soundHandler::orderCallback (void)
{
   if (!_soundOn)
   {
      return;
   }

   // Get message
   const SoundOrders order = _orderMsg.getData();

   switch (order.soundCode)
   {
      // Don't play sounds from this list
      case BUTTON_CLICK_SOUND :
         break;
      default :
         DataLog(log_level_gui_info) << "Playing sound. sound code: "
                                     << SoundCodeStrings[order.soundCode] << " sound mode: "
                                     << SoundModeStrings[order.mode] << endmsg;
   }

   // If this is the clear all message
   if ( (order.soundCode == NO_SOUND) &&
        (order.mode      == SOUND_CLEAR_ALL)
        )
   {
      for (int code = 0; code < MAX_SOUNDS; code++ )
      {
         orderData[code] = SOUND_CLEAR;     // put value in queue
      }
   }
   // else range check the one sound message
   else if ( (order.soundCode < MAX_SOUNDS) &&
             (order.mode      < SOUND_MAX_MODES)
             )
   {
      orderData[order.soundCode] = order.mode;              // put value in queue

      if (order.mode == SOUND_IMMEDIATE)
      {
         processImmediate();               // start sound now
      }
   }
}



void soundHandler::timerCallback (void)
{
   if (!_soundOn)
   {
      return;
   }

   //  decrement timer, if expired, check next command
   _duration -= 10;

   if ( _duration < 0 )
   {
      short freq = 0;                          // sound frequency
      _duration = 0;

      if ( _toneList )                          // is there more audio data?
      {
         _duration   = _toneList[_toneIndex].dur;       // get new duration
         freq        = _toneList[_toneIndex].freq;      // and freqency
         _toneIndex += 1;

         if ( _duration == 0 )
         {
            _toneList = NULL;
         }
      }
      else
      {
         _orderIndex += 1;                   // check next command
         if ( _orderIndex >= MAX_SOUNDS)     // wrap
         {
            _orderIndex = 0;
         }

         // if not CLEAR, then must be active entry
         if ( orderData[_orderIndex] != SOUND_CLEAR)
         {
            _toneList  = SoundsDat::data().toneData[_orderIndex];
            _toneIndex = 0;
         }

         // set one time entries to clear
         if ( orderData[_orderIndex] == SOUND_IMMEDIATE ||
              orderData[_orderIndex] == SOUND_ONCE )
         {
            orderData[_orderIndex] = SOUND_CLEAR;
         }

         // fetch next tone and process
         if ( _toneList )
         {
            _duration   = _toneList[_toneIndex].dur;
            freq        = _toneList[_toneIndex].freq;
            _toneIndex += 1;

            if ( _duration == 0 )
            {
               _toneList = NULL;
            }
         }
      }

      // output new data to hardware
      if ( (_duration > 0) && (freq > 0) )
      {
         startTone(freq);
      }
      else
      {
         stopTone();
      }
   }
}

void soundHandler::startTone (short freq)
{
   unsigned int counterReload = 1192000 / freq;

   /* set command for counter 2, 2 byte write */
   int oldlevel = intLock();
   sysOutByte(PIT_BASE_ADDR + 3, 0xb6);
   sysOutByte(PIT_BASE_ADDR + 2, (char)(counterReload & 0xff));
   sysOutByte(PIT_BASE_ADDR + 2, (char)((counterReload >> 8) & 0xff));

   /* enable counter 2 */
   sysOutByte(DIAG_CTRL, sysInByte (DIAG_CTRL) | 0x03);
   intUnlock(oldlevel);

   _toneEnabled = true;
}

void soundHandler::stopTone (void)
{
   if ( _toneEnabled )
   {
      /* disable counter 2 */
      int oldlevel = intLock();
      sysOutByte(DIAG_CTRL, sysInByte(DIAG_CTRL) & ~0x03);
      intUnlock(oldlevel);
      _toneEnabled = false;
   }

   return;
}



void soundHandler::processImmediate (void)
{
   // scan for immediate entry
   for (_orderIndex = 0; _orderIndex<MAX_SOUNDS; _orderIndex += 1)
   {
      // check for immediate
      if ( orderData[_orderIndex] == SOUND_IMMEDIATE )
      {
         // clear entry
         orderData[_orderIndex] = SOUND_CLEAR;

         // set local values
         _toneList   = SoundsDat::data().toneData[_orderIndex];
         _toneIndex  = 0;

         _duration   = _toneList[_toneIndex].dur;
         short freq = _toneList[_toneIndex].freq;
         _toneIndex += 1;

         if ( _duration == 0 )
         {
            _toneList = NULL;
         }

         // if duration and freq are none zero, start sound
         if ( (_duration > 0) && (freq > 0) )
         {
            startTone(freq);
         }
         else
         {
            stopTone();
         }
      }
   }
}

extern "C" void sounds (void)
{
   // create message system
   MessageSystem msgSystem;
   msgSystem.initNonBlock("sounds");

   // load sounds
   ALARM_VALUES datStatus = SoundsDat::initialize(1);
   if (datStatus != NULL_ALARM)
   {
      DataLog(log_level_sounds_error) << "Sounds file initialization failed" << endmsg;
      anAlarmMsg msg (datStatus);
      msg.setAlarm();
   }
   else
   {
      DataLog(log_level_sounds_info) << "Initialized the sounds.dat file" << endmsg;
   }

   // create handler for sound orders
   soundHandler handler;

   // setup timer notifications and handle periodic update
   SEM_ID tmrSem = auxClockSemaphoreAttach(10000, AuxClockBinarySemaphore);
   if ( !tmrSem )
   {
      _FATAL_ERROR(__FILE__, __LINE__, "auxClockSemaphoreAttach failed");
   }

   while ( 1 )
   {
      semTake(tmrSem, WAIT_FOREVER);
      handler.timerCallback();
      msgSystem.dispatchMessages();
   }
}

/* FORMAT HASH 5493f8e75b01302b271841627c6823b6 */

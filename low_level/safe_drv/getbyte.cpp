/*
 * Copyright (c) 1995-1999 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/low_level/safe_drv/rcs/getbyte.cpp 1.12 2003/06/19 19:44:59Z jl11312 Exp dslausb $
 * $Log: getbyte.cpp $
 * Revision 1.12  2003/06/19 19:44:59Z  jl11312
 * - moved hardware interface to separate modules so that it is available earlier in the boot process
 * Revision 1.11  2003/03/28 19:48:12Z  jl11312
 * - modified to use new data log levels
 * Revision 1.10  2002/12/04 15:53:43Z  pn02526
 * Checkins for first operating safety driver under vxWorks
 * Revision 1.9  2002/10/31 18:28:26Z  pn02526
 * Massive checkin for integration testing.  Changes BOOL to bool, and forces build to be for 486, only.
 * Revision 1.8  2002/08/13 13:43:50  pn02526
 * Remove extraneous #includes of numerous system files.
 * Change ASSERT() macro calls to trima_assert().
 * Substitute vxWorks sysInWord() calls for QNX inpw() calls.
 * Revise logging to use the DaaLog_Level class through the DataLog_Default macro.
 * Revision 1.7  2000/05/05 15:38:26  BS04481
 * Change log_error function
 * Revision 1.6  1999/10/28 22:52:39  BS04481
 * Do a 16b read on the basin temp sensor in order to work around
 * a memory mapping problem in revision 4 of the FPGA.  The basin
 * temp registers map into the ultrasonics registers.  Hardware will
 * be fixed in the future.
 * Revision 1.5  1999/10/28 20:29:14  BS04481
 * Code review change.  Previous design disabled soft watchdogs
 * while the clock was being set.  This was unacceptable.  This code
 * changes the soft watchdogs to run off of an element of the
 * kernel's ticks space which is not sensitive to changes in the
 * real-time clock.    All code with disables the soft watchdogs is
 * disabled.  The soft watchdog will go off if not petted within 2
 * seconds under all conditions.  The machine will also safe-state
 * if the soft watchdogs ever go backward.   In addition, time set
 * changes are rejected if received while air-to-donor monitoring is
 * in effect.  This is done because the time jumps in the real-time
 * clocks effect the control loops.
 *
 * Revision 1.4  1999/08/13 14:24:43  BS04481
 * Remove logging
 * Revision 1.3  1999/08/03 18:45:35  BS04481
 * Increase idle time between packets to at least 500ms.  Remove
 * idle time between bytes.   Force resync at start and when data
 * errors are detected.  Enable error detection for bad start bit, bad
 * stop bit and bad CRC on the packet.
 * Revision 1.2  1999/07/30 17:01:44  BS04481
 * Fixed problem where if communication stopped in an error state,
 * the watchdogs for "no data" did not set the message.
 * Revision 1.1  1999/07/23 01:42:08  BS04481
 * Initial revision
 *
 * TITLE:      getbyte.cpp, part of Focussed System basin temperature monitoring
 *
 * ABSTRACT:   Interface single bit data stream from the basin
 *             temperature microprocessor.
 *             This object is not terribly flexible as it assumes a
 *             data format (1 start, 8 data, no parity, 1 stop) and
 *             polarity (start=high, data1=low, data0=high, stop=low)
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 */

// #includes
#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trima_assert.h"
#include "trima_datalog.h"
#include "error.h"
#include "getbyte.hpp"

// SPECIFICATION:   Single bit data stream.
//                  Available for 2.6 hardware only
// ERROR HANDLING:  None

getbyte::getbyte(SafeHWGetByteSource select, float period)
   : _select(select), _period(period)
{
   _numStart     = 1;
   _numData      = 8;
   _numParity    = 0;
   _numStop      = 1;
   _totalBits    = _numStart + _numData
                   + _numParity + _numStop;
   _totalSamples = (int)((float)_totalBits * period / 10.0) - 1;

   if ( _totalSamples > MAX_SAMPLES)
      trima_assert(0);     // too many samples for storage space

   _sampleCount    = 0;
   _sampleWord     = 0;
   _syncCounter    = 0;
   _notSyncCounter = 0;
   _state          = WAIT_FOR_IDLE;
   _forceIdle      = true;
   _dataValid      = NOT_DETERMINED;
   for (int i = 0; i<(MAX_SAMPLES / 32); _sampleSpace[i] = 0, i++) ;

}

getbyte::~getbyte()
{}


void getbyte::update ()
{
   unsigned char sample;

   sample = shw_getByteSourceData(_select);
   switch (_state)
   {
      case WAIT_FOR_START :
         // waiting for the start bit (1)
         if (sample)
         {
            _sampleSpace[0] |= sample;
            _sampleCount++;
            _state           = SAMPLE;
            _forceIdle       = false;
            _dataValid       = NOT_DETERMINED;
         }
         break;
      case SAMPLE :
         // accumulating samples every 10ms
         if ( (_sampleCount % 32) == 0)
            _sampleWord++;
         _sampleSpace[_sampleWord] |= (sample << (_sampleCount - (32 * _sampleWord)));
         if (++_sampleCount >= _totalSamples)
            _state = STOP;
         break;
      case STOP :
         // validate the data format
         _dataValid   = validate();
         _syncCounter = 0;
         _state       = WAIT_FOR_IDLE;
         break;
      case WAIT_FOR_IDLE :
         if (sample == 0)
         {
            if (_syncCounter++ >= IDLE_COUNT)
            {
               _sampleCount    = 0;
               _sampleWord     = 0;
               for (int i = 0; i<(MAX_SAMPLES / 32); _sampleSpace[i] = 0, i++) ;
               _state          = WAIT_FOR_START;
               _notSyncCounter = 0;
            }
         }
         else
         {
            // we got ones so reset the sync counter because we need to see
            // consecutive 0s before we are in sync
            _syncCounter = 0;

            if (_forceIdle /* == true */)
            {
               // we're here absorbing 1s while waiting for the sync period
               if (_notSyncCounter++ == IDLE_COUNT )
                  _dataValid = NOT_DETERMINED;
            }
            else
            {
               // if we're not here waiting for a resync, assume we are starting a new
               // acquisition.

               // clean up from last time
               _sampleCount    = 0;
               _sampleWord     = 0;
               for (int i = 0; i<(MAX_SAMPLES / 32); _sampleSpace[i] = 0, i++) ;
               _notSyncCounter = 0;
               _forceIdle      = false;

               // keep the first sample and go look for more
               _sampleSpace[0] |= sample;
               _sampleCount++;
               _state           = SAMPLE;
               _dataValid       = NOT_DETERMINED;
            }
         }
         break;
      default :
         // should not be here
         _dataValid = DATA_ERR;
         DataLog(log_level_safe_drv_error) << (int)_state << " Acquire should not be here" << endmsg;
         break;
   }

}


GETBYTE_RESULT getbyte::validate ()
{
   GETBYTE_RESULT result = DATA_OK;
   int            mask, sb1, sb2, sb3, position;
   int            last   = 0;
   int            data   = 0;

   // verify first three samples are high (start bit)
   if ( (_sampleSpace[0] & 0x7) != 0x7)
   {
      result = DATA_ERR;
      DataLog(log_level_safe_drv_error) << "Bad start bit 0x" << hex << _sampleSpace[0] << dec << endmsg;
   }
   else
   {
      last = _sampleSpace[0];
      // verify last three samples are low (stop bit)
      sb1  = 0x1 << ((_totalSamples) % 32);
      sb2  = 0x1 << ((_totalSamples - 1) % 32);
      sb3  = 0x1 << ((_totalSamples - 2) % 32);
      if ( ( (_sampleSpace[(_totalSamples) / 32] & sb1) != 0)
           ||( (_sampleSpace[(_totalSamples - 1) / 32] & sb2) != 0)
           ||( (_sampleSpace[(_totalSamples - 2) / 32] & sb3) != 0) )
      {
         result = DATA_ERR;
         DataLog(log_level_safe_drv_error) << "Bad stop bit 0x"
                                           << hex << _sampleSpace[(_totalSamples) / 32] << dec
                                           <<  "0x"
                                           << hex << _sampleSpace[(_totalSamples - 2) / 32] << dec
                                           << endmsg;
      }
      else
      {
         // verify any data transitions that occur are in
         // expected positions
         for (int i = 0; i<_sampleWord; i++)
         {
            for (int j = 0; j<31; j++)
            {
               mask = (0x1 << j);
               data = (_sampleSpace[i] & mask) >> j;
               if (data != last)
               {
                  // transition occurred
                  position = (i * 32) + j;
                  if ( ( ( (int)((float)position * 10.0) % (int)_period) > 10 )
                       &&( ( (int)((float)(position - 1) * 10.0) % (int)_period) > 10)
                       &&( ( (int)((float)(position + 1) * 10.0) % (int)_period) > 10) )
                  {
                     result = DATA_ERR;
                     DataLog(log_level_safe_drv_error) << position << " Bad transition position" << endmsg;
                     break;
                  }
               }
               last = data;

            }
            if (result == DATA_ERR)
               break;
         }
      }
   }
   return(result);
}


char getbyte::assembleByte ()
{
   int   start, stop, bit, mask;
   char  data = 0;
   int   position, positionT;
   float time;

   // pick off start bit
   time     = _period / 2.0;
   position = (int)(time / 10.0);
   mask     = 0x1 << (position - 1);
   bit      = (_sampleSpace[(position / 32)] & mask) >> (position - 1);
   start    = bit;

   // pick off data bits
   for (int i = 0; i<_numData; i++)
   {
      time     += _period;
      positionT = (int)(time / 10.0);
      if (time - ((float)positionT * 10.0) > 5.0)
         positionT++;
      position = (positionT % 32);
      mask     = 0x1 << (position - 1);
      bit      = (~(_sampleSpace[(positionT / 32)]) & mask) >> (position - 1);
      data    |= (char)(bit << i);
   }

   // pick off stop bit
   time     += _period;
   positionT = (int)(time / 10.0);
   if (time - ((float)positionT * 10.0) > 5.0)
      positionT++;
   position = (positionT % 32);
   mask     = 0x1 << (position - 1);
   bit      = (_sampleSpace[(positionT / 32)] & mask) >> (position - 1);
   stop     = bit;

   if (start != 1)
      DataLog(log_level_safe_drv_error) << start << " Assemble: Bad start bit" << endmsg;

   if (stop != 0)
      DataLog(log_level_safe_drv_error) << stop << " Assemble: Bad stop bit" << endmsg;

   return(data);
}

void getbyte::setForceIdle (bool idle)
{
   _notSyncCounter = 0;
   _syncCounter    = 0;
   _forceIdle      = idle;
}

/* FORMAT HASH ae5398281744d4f2049599469e923735 */

/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.22.1.2  2009/10/05 16:03:34Z  dslausb
 * Shawn's safety code
 * Revision 1.1  2009/10/02 19:12:35Z  dslausb
 * Initial revision
 * Revision 1.22  2008/04/23 00:30:39Z  spriusb
 * Fix for 8286: safety valve check should be conditional on accumulated volume, not instantaneous change.
 * Revision 1.21  2007/10/17 22:38:47Z  spriusb
 * Rolling back dRBC auto-split changes
 * Revision 1.20  2007/09/20 20:36:21Z  spriusb
 * Forgot to take out extra logging.
 * Revision 1.19  2007/09/19 18:00:10Z  spriusb
 * Fix for IT8210: Various refactors to Container classes in safety model
 * Revision 1.18  2003/07/21 21:53:03Z  td07711
 * IT6157 - no need to shutdown in ResetCycleVol()
 * Revision 1.17  2003/07/01 22:34:08Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.16  2003/04/03 23:13:46Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.15  2002/10/07 18:13:28Z  pn02526
 * Correct cut/paste error that was causing saf_exec to crash immediately upon being started.
 * Revision 1.14  2002/08/14 12:40:23  pn02526
 * Porting V5.0's revision 1.10  change from 2002/08/08 15:03:46  ms10234:
 * 5738 - Initialized deltaVolume to 0.
 * Revision 1.13  2002/08/08 14:51:16  pn02526
 * Porting V5.0's revision 1.9  change from 2002/07/31 08:55:21  ms10234:
 * 5738 - Changed safety's bag volume accrual code to only allow fluid to flow out of the bag if the bag actually contains fluid.
 *
 * Revision 1.12  2002/08/07 11:55:12  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * Revision 1.11  2002/07/23 16:52:24  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.10  2002/04/15 09:55:53  pn02526
 * Change log(LOG_ALARM_DETAIL) to alarm_detail(LOG_ALARM_DETAIL)
 * Revision 1.9  2002/04/09 19:03:18Z  pn02526
 * "Revise logging to use DataLogLevel class.  Revise FATAL_ERROR calls to use the new one."
 * Revision 1.8  2000/07/21 18:32:56  bs04481
 * Remove compile flag and support of dual stage sets
 * Revision 1.7  2000/07/07 23:22:52  bs04481
 * Enable code to check for maximum volumes in the RBC, Plasma
 * and Platelet bags.
 * Revision 1.6  2000/06/30 21:28:46  bs04481
 * Re-did valve monitoring to not monitor by draw and return cycles
 * but to alarm anytime there is a valve out of position for 20
 * consecutive seconds while the associated pump is running.
 * Revision 1.5  2000/06/22 16:46:28  bs04481
 * Plumbing changes for single-stage.  1) Change to RBC flow.
 * 2) Add valve position monitoring during Blood Run and Blood
 * Rinseback. 3)  Add compile flag to support dual-stage
 * Revision 1.4  2000/05/26 17:11:48  BS04481
 * More maintanence changes:  This set contains changes for
 * 1. Moving valve object from container to bag
 * 2. Removing SafetyTimer->SafetySoftTimer and letting the
 *      switch and disconnect tests have their own timers.
 * 3.  All message types are changed to NO_BOUNCE to cut down
 *      the on unnecessary stuff in the message que.
 * Revision 1.3  1999/10/03 22:00:49  BS04481
 * Non-critical 3.3a safety code review change.
 * Revision 1.2  1999/04/06 18:32:33  BS04481
 * Remove default of set type to white when unknown.  If set type is
 * not known in states from Donor Connect to Rinseback, inclusive,
 * and any pumps run, alarm.  Default to white if unknown in Donor
 * Disconnect
 * Revision 1.1  1998/05/12 19:49:44  bs04481
 * Initial revision
 *
 *
 * TITLE:      s_bag.cpp, a bag
 *
 * ABSTRACT:   Safety's view of a thing that holds fluid.
 *             Flow into a bag is positive and flow out of a bag is negative
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "trima_datalog.h"
#include "s_bag.hpp"
#include "s_donor.hpp"
#include "s_exec.hpp"
#include "state_objects.hpp"

// SPECIFICATION:    Safety baseBag constructor
//
// ERROR HANDLING:   none.

baseBag::baseBag()
   : _flowIO(FLOW_UNKNOWN),
     _valveIO(HW_VALVE_UNKNOWN),
     _initialVolume(0.0f),
     _maximumVolume(0.0f),
     _minimumVolume(0.0f),
     _flowTypeRequired(true), // until proven otherwise
     _initialVolumeKnown(false),
     _maximumVolumeKnown(false),
     _minimumVolumeKnown(false),
     _accumulatedVolume(0.0f),
     _deltaVolume(0.0f),
     _drawCycleVolume(0.0f),
     _returnCycleVolume(0.0f),
     _drawCycleVolumeInfusion(0.0f),
     _returnCycleVolumeInfusion(0.0f)
{}



// SPECIFICATION:    Safety baseBag destructor
//
// ERROR HANDLING:   none.

baseBag::~baseBag()
{}

// SPECIFICATION:    Safety baseBag method to set general flow
//                   direction for the bag.  This direction
//                   can be modified by the sign of the pump
//                   This function is called after the set
//                   type is determined.
//
//
// ERROR HANDLING:   none.

void baseBag::SetFlowIO (FLOW_DIRECTION dir)
{
   _flowIO = dir;
}

// SPECIFICATION:    Safety baseBag method to define the valve
//                   position which allows flow in or out of the bag
//
// ERROR HANDLING:   none.

void baseBag::SetValveIO (HW_VALVE_STATES dir)
{
   _valveIO = dir;
}

// SPECIFICATION:    Safety baseBag method to set up for the new
//                   system state
//
// ERROR HANDLING:   none.

void baseBag::InitializeForState (long newState)
{

   switch (newState)
   {
      case AC_PRIME :
         // reset the accumulated and cycle volumes
         // because any volumes accumulated prior to
         // this were accumulated during load and
         // disposable test and do not reflect fluids
         logPumpState();

         _accumulatedVolume         = 0.0f;
         _deltaVolume               = 0.0f;
         _drawCycleVolume           = 0.0f;
         _returnCycleVolume         = 0.0f;
         _drawCycleVolumeInfusion   = 0.0f;
         _returnCycleVolumeInfusion = 0.0f;
         _flowTypeRequired          = false;
         break;

      case DONOR_CONNECTED :
      case BLOOD_PRIME :
      case BLOOD_RUN :
      case BLOOD_RINSEBACK :
         _flowTypeRequired = true;
         break;

      default :
         _flowTypeRequired = false;
         break;
   }
}

// SPECIFICATION:    Safety baseBag method to set the initial
//                   volume of the bag if it is known.
//
// ERROR HANDLING:   none.

void baseBag::SetInitialVol (float vol)
{
   _initialVolume      = vol;
   _initialVolumeKnown = true;
}

// SPECIFICATION:    Safety baseBag method to get the initial
//                   volume of the bag if it is known.
//
// RETURNS:          TRUE if initial volume is known.
//
// ERROR HANDLING:   none.

bool baseBag::GetInitialVol (float& vol)
{
   vol = _initialVolume;
   return(_initialVolumeKnown);
}

// SPECIFICATION:    Safety baseBag method to set the maximum
//                   volume of the bag if it is known.
//
// ERROR HANDLING:   none.

void baseBag::SetMaxVol (float vol)
{
   _maximumVolume      = vol;
   _maximumVolumeKnown = true;
}

// SPECIFICATION:    Safety baseBag method to get the maximum
//                   volume of the bag if it is known.
//
// RETURNS:          TRUE if maximum volume is known.
//
// ERROR HANDLING:   none.

bool baseBag::GetMaxVol (float& vol)
{
   vol = _maximumVolume;
   return(_maximumVolumeKnown);
}

// SPECIFICATION:    Safety baseBag method to set the minimum
//                   volume of the bag if it is known.
//
// ERROR HANDLING:   none.

void baseBag::SetMinVol (float vol)
{
   _minimumVolume      = vol;
   _minimumVolumeKnown = true;
}

// SPECIFICATION:    Safety baseBag method to get the minimum
//                   volume of the bag if it is known.
//
// RETURNS:          TRUE if minimum volume is known.
//
// ERROR HANDLING:   none.

bool baseBag::GetMinVol (float& vol)
{
   vol = _minimumVolume;
   return(_minimumVolumeKnown);
}

// SPECIFICATION:    Safety baseBag method to reset the
//                   cycle volumes at the start of the draw
//                   or return cycle
// ERROR HANDLING:   none.

void baseBag::ResetCycleVol (char cCycle)
{
   if (cCycle == DRAW_CYCLE)
   {
      _drawCycleVolume         = 0.0f;
      _drawCycleVolumeInfusion = 0.0f;
   }
   else if (cCycle == RETURN_CYCLE)
   {
      _returnCycleVolume         = 0.0f;
      _returnCycleVolumeInfusion = 0.0f;
   }
}

// SPECIFICATION:    Safety constructor for a bag which has a
//                   single pump and no valve associated with it.
//
// ERROR HANDLING:   none.

bag::bag(SafetyPumps pumps)
   :  baseBag()
{

   pumpId = pumps;

   // there is no valve but let's pretend
   // by making it a pseudovalve
   _myValve.MakeItFake();

   DataLog(log_level_safe_exec_debug) << _myValve._pszValveName
                                      << " control: " << _myValve._positionC
                                      << ", safety: " << _myValve._positionS << endmsg;

   DataLog(log_level_safe_exec_debug) << pumps << endmsg;
}

// SPECIFICATION:    Derived bag class destructor
//
// ERROR HANDLING:   none.

bag::~bag()
{}

// SPECIFICATION:    Logs the volume-related metrics
//                   associated with the pump model for
//                   this instance.  This function implements
//                   the pure abstract interface from baseBag
//                   class.
//
// ERROR HANDLING:   none.

void bag::logPumpState () const
{
   DataLog(log_level_safe_exec_info) << "Resetting "
                                     << pumpId
                                     << " volumes: Accum: "
                                     << _accumulatedVolume
                                     << ", Delta: "
                                     << _deltaVolume
                                     << ", Draw: "
                                     << _drawCycleVolume
                                     << ", Return: "
                                     << _returnCycleVolume
                                     << ", DrawI: "
                                     << _drawCycleVolumeInfusion
                                     << ", ReturnI: "
                                     << _returnCycleVolumeInfusion
                                     << endmsg;
}

// SPECIFICATION:    Safety baseBag method to update the
//                   delta volume and accumulated volume
//                   of the bag.
//
// RETURNS:          VOLUME_OK if the accumulated volume is
//                   within limits (if defined) or if the limits
//                   are not known.
//                   VOLUME_MIN_EXCEEDED if the accumulated
//                   volume has fallen below the minimum allowed.
//                   VOLUME_MAX_EXCEEDED if the accumulated
//                   volume has risen above the maximum allowed.
//
// ERROR HANDLING:   FATAL_ERROR if we can't set the flowIO
//                   based on the set type.

VOLUME_VALID bag::UpdateVol (char cCycle)
{
   // first get the appropriate delta volume for
   // the associated pump for this sample period
   float volume = SafetyExec::instance()->data().derived.volumes[pumpId];

   // get the current valve position
   HW_VALVE_STATES valve = _myValve.GetControlValve();

   // if the valve is in the position to allow flow for this set
   // and the pump is running, accumulate the volume
   // depending on the normal flow direction
   if ( ( (valve == HW_VALVE_OPEN) || (valve == _valveIO) ) && (volume != 0.0f) )
   {
      if (_flowIO == FLOW_INTO_SET)
      {
         // normal flow is into the set so the volume is
         // added
         _deltaVolume = volume;
      }
      else if ( (_flowIO == FLOW_INTO_BAG) || (_flowIO == FLOW_INTO_BAG_RC) )
      {
         if (
            //
            // If the volume is coming going into the bag (
            //  the volume variable will contain a negative
            //  value if fluid is flowing out ),
            //
            volume > 0
            //
            // or the bag contains fluid ( bag volumes are
            //  stored as negative; if the bag volume is
            //  0 or a positive number, the bag is empty )
            //
            || _accumulatedVolume < 0
            )
         {
            // normal flow is into the bag so the volume is
            // subtracted
            _deltaVolume = -volume;
         }
         else
         {
            _deltaVolume = 0.0f;
         }
      }
      else if (_flowIO == FLOW_UNKNOWN)
      {
         // if we don't know the flowIO yet, we don't know the set type
         // and we can't accumulate any volume because the set is not down.
         // this can happen during load, but if this is during a run,
         // cause an alarm.
         _deltaVolume = 0.0f;

         if ( _flowTypeRequired && (_accumulatedVolume > VOL_TOLERANCE_FOR_UNKNOWN_FLOW))
         {
            return VOLUME_FLOW_ERROR;
         }
      }
      else if (_flowIO == FLOW_ERROR)
      {
         _accumulatedVolume += _deltaVolume;
         // we don't know the set type and so cannot reliably
         // accumulate volume.  We don't know what we have
         // here so fatal out to safe state if we pump more than
         // is reasonable.
         _deltaVolume = 0.0f;

         if (_flowTypeRequired && (_accumulatedVolume > VOL_TOLERANCE_FOR_UNKNOWN_FLOW))
            return VOLUME_FLOW_ERROR;

      }

      // add to the accumulators for the bag

      //
      //
      //  Initial volume accounting: This code corrects safety accumulation for cases where
      //  we want to ignore initial (prime) volumes.  Initial volumes are set in the container
      //  where they are applicable; e.g. the saline prime volume is set in the platelet pump
      //  container.  (IT9853)
      //
      if (_flowTypeRequired && _initialVolumeKnown && (_initialVolume != 0.0f)) // regardless of draw or return
      {
         // do not update draw or return volume

         //
         //
         // Ensure we are actually accruing volume in the correct direction (to diminish
         // the initial volume)
         //
         if (((_deltaVolume < 0.0f) && (_initialVolume < 0.0f)) ||
             ((_deltaVolume > 0.0f) && (_initialVolume > 0.0f)))
         {
            paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SW_FAULT);
            DataLog(log_level_safety_alarm_detail) << "Invalid volume accrual" << endmsg;
         }

         //
         // Accrue delta volume until initial volume is satisfied.
         //
         if (fabs(_deltaVolume) < fabs(_initialVolume))
         {
            _initialVolume += _deltaVolume;
            DataLog(log_level_safe_exec_info) << "Ignoring " << _deltaVolume << " volume for pump " << pumpId  << " towards " << _initialVolume << " remaining" << endmsg;
            _deltaVolume    = 0.0f;
         }
         else  // done with initial volume accrual
         {
            DataLog(log_level_safe_exec_info) << "Ignoring " << _initialVolume << " volume for pump " << pumpId  << " towards " << _initialVolume << " remaining" << endmsg;
            _deltaVolume  += _initialVolume;  // already checked to ensure sign compatibility
            _initialVolume = 0.0f;
         }
      }



      _accumulatedVolume += _deltaVolume;
      if (cCycle == DRAW_CYCLE)
      {
         _drawCycleVolume += _deltaVolume;

         if (valve == _valveIO)
            _drawCycleVolumeInfusion += _deltaVolume;
      }
      else
      {
         _returnCycleVolume += _deltaVolume;

         if (valve == _valveIO)
            _returnCycleVolumeInfusion += _deltaVolume;
      }
   }
   else
      // the fluid didn't go to the bag.
      _deltaVolume = 0.0f;

   // FIX for IT 8286: check the valve for legal position if we have, at any
   // point during the procedure, collected product into the bag
   if (_accumulatedVolume < 0)
      _myValve.CheckValvePosition();

   // if this bag has maximum and/or minimum volume set, test them and return
   // an error if we have exceeded this limit(s)
   //
   // watch the signs here.  if flowIO is into the bag then the accumulated
   // volumes are negative and maximum and minimum limits are negative
   // so the greater and less signs change.
   if ( (_flowIO == FLOW_INTO_BAG) || (_flowIO == FLOW_INTO_BAG_RC) )
   {
      if ( _minimumVolumeKnown && (_accumulatedVolume > _minimumVolume) )
      {
         DataLog(log_level_safe_exec_error) << "Volume in: " << _accumulatedVolume << " - Minimum Volume Exceeded" << endmsg;

         return VOLUME_MIN_EXCEEDED;
      }
      else if ( _maximumVolumeKnown && (_accumulatedVolume < _maximumVolume) )
      {
         DataLog(log_level_safe_exec_error) << "Volume in: " << _accumulatedVolume << " - Maximum Volume Exceeded" << endmsg;

         return VOLUME_MAX_EXCEEDED;
      }
      else
         return VOLUME_OK;
   }
   else
   {
      if ( _minimumVolumeKnown && (_accumulatedVolume < _minimumVolume) )
      {
         DataLog(log_level_safe_exec_error) << "Volume out: " << _accumulatedVolume << " - Minimum Volume Exceeded" << endmsg;

         return VOLUME_MIN_EXCEEDED;
      }
      else if ( _maximumVolumeKnown && (_accumulatedVolume > _maximumVolume) )
      {
         DataLog(log_level_safe_exec_error) << "Volume out: " << _accumulatedVolume << " - Maximum Volume Exceeded" << endmsg;

         return VOLUME_MAX_EXCEEDED;
      }
      else
         return VOLUME_OK;
   }
}

// SPECIFICATION:    Safety constructor for an RBC bag which has a
//                   single valve but no pump associated with it.
//                   In this case the volumes are derived from other pumps.
//                   See the class for anRBCPseudoPump_Safety in s_pump.
//
// ERROR HANDLING:   none.

rbcBag::rbcBag()
   : baseBag(),
     _mode(rbcBag::CM_UNKNOWN)
{
   _myValve.MakeItReal("RBCValve", RBC_OUT_OF_POSITION);

   DataLog(log_level_safe_exec_debug) << _myValve._pszValveName
                                      << " control: " << _myValve._positionC
                                      << ", safety: " << _myValve._positionS << endmsg;

   DataLog(log_level_safe_exec_debug) << PSEUDO_RBC << endmsg;
}

rbcBag::~rbcBag()
{}

// SPECIFICATION:    Logs the volume-related metrics
//                   associated with the pseudo-pump model for
//                   this instance.  This function implements
//                   the pure abstract interface from baseBag
//                   class.
//
// ERROR HANDLING:   none.

void rbcBag::logPumpState () const
{

   DataLog(log_level_safe_exec_info) << "Resetting "
                                     << PSEUDO_RBC
                                     << " volumes: Accum: "
                                     << _accumulatedVolume
                                     << ", Delta: "
                                     << _deltaVolume
                                     << ", Draw: "
                                     << _drawCycleVolume
                                     << ", Return: "
                                     << _returnCycleVolume
                                     << ", DrawI: "
                                     << _drawCycleVolumeInfusion
                                     << ", ReturnI: "
                                     << _returnCycleVolumeInfusion
                                     << endmsg;
}

// SPECIFICATION:    Utility method to set the
//                   Collection mode state member by
//                   way of conversion given
//                   positions for the RBC and Platelet
//                   valves.
//
// ERROR HANDLING:   none.

void rbcBag::setCollectMode (HW_VALVE_STATES RBCValveControl,
                             HW_VALVE_STATES PlateletValveControl)
{
   // Determine the "Collect Mode" for the RBC Container
   switch ( RBCValveControl )
   {
      case HW_VALVE_COLLECT :
         this->setCollectMode(rbcBag::CM_COLLECT);
         break;

      case HW_VALVE_OPEN :
         this->setCollectMode(rbcBag::CM_OPEN);
         break;

      case HW_VALVE_RETURN :
         this->setCollectMode(rbcBag::CM_RETURN);
         break;

      case HW_VALVE_UNKNOWN :
      default :
         this->setCollectMode(rbcBag::CM_UNKNOWN);
   }
}

// SPECIFICATION:    Update volume behavior overridden by
//                   RBC Bag class.  Includes specialization
//                   in the case of auto-split cassettes.
//
// RETURNS:          VOLUME_OK if the accumulated volume is
//                   within limits (if defined) or if the limits
//                   are not known.
//                   VOLUME_MIN_EXCEEDED if the accumulated
//                   volume has fallen below the minimum allowed.
//                   VOLUME_MAX_EXCEEDED if the accumulated
//                   volume has risen above the maximum allowed.
//
// ERROR HANDLING:   FATAL_ERROR if we can't set the flowIO
//                   based on the set type.

VOLUME_VALID rbcBag::UpdateVol (char cCycle)
{
   // first get the appropriate delta volume for
   // the associated pump for this sample period
   float volume = SafetyExec::instance()->data().derived.volumes[PSEUDO_RBC];


   // get the current valve position
   rbcBag::CollectMode mode = this->getCollectMode();

   // if the valve is in the position to allow flow for this set
   // and the pump is running, accumulate the volume
   // depending on the normal flow direction
   if ( ( (mode == rbcBag::CM_OPEN)
          ||(mode == rbcBag::CM_COLLECT) )
        &&(volume != 0.0f) )
   {

      if (_flowIO == FLOW_INTO_SET)
      {
         // normal flow is into the set so the volume is
         // added
         _deltaVolume = volume;
      }
      else if ( (_flowIO == FLOW_INTO_BAG)
                ||(_flowIO == FLOW_INTO_BAG_RC) )
      {
         if (
            //
            // If the volume is coming going into the bag (
            //  the volume variable will contain a negative
            //  value if fluid is flowing out ),
            //
            volume > 0
            //
            // or the bag contains fluid ( bag volumes are
            //  stored as negative; if the bag volume is
            //  0 or a positive number, the bag is empty )
            //
            || _accumulatedVolume < 0
            )
         {
            // normal flow is into the bag so the volume is
            // subtracted
            _deltaVolume = -volume;
         }
         else
         {
            _deltaVolume = 0.0f;
         }
      }
      else if (_flowIO == FLOW_UNKNOWN)
      {
         // if we don't know the flowIO yet, we don't know the set type
         // and we can't accumulate any volume because the set is not down.
         // this can happen during load, but if this is during a run,
         // cause an alarm.
         _deltaVolume = 0.0f;

         if ( _flowTypeRequired && (_accumulatedVolume > VOL_TOLERANCE_FOR_UNKNOWN_FLOW) )
         {
            return VOLUME_FLOW_ERROR;
         }
      }
      else if (_flowIO == FLOW_ERROR)
      {
         _accumulatedVolume += _deltaVolume;
         // we don't know the set type and so cannot reliably
         // accumulate volume.  We don't know what we have
         // here so fatal out to safe state if we pump more than
         // is reasonable.
         _deltaVolume = 0.0f;

         if ( _flowTypeRequired && (_accumulatedVolume > VOL_TOLERANCE_FOR_UNKNOWN_FLOW) )
         {
            return VOLUME_FLOW_ERROR;
         }

      }

      // add to the accumulators for the bag
      _accumulatedVolume += _deltaVolume;
      if (cCycle == DRAW_CYCLE)
      {
         _drawCycleVolume += _deltaVolume;

         if (mode == rbcBag::CM_COLLECT)
            _drawCycleVolumeInfusion += _deltaVolume;
      }
      else
      {
         _returnCycleVolume += _deltaVolume;

         if (mode == rbcBag::CM_COLLECT)
            _returnCycleVolumeInfusion += _deltaVolume;
      }
   }
   else
      // the fluid didn't go to the bag.
      _deltaVolume = 0.0f;

   // check the valve for legal position
   if (volume != 0.0f)
      _myValve.CheckValvePosition();

   // if this bag has maximum and/or minimum volume set, test them and return
   // an error if we have exceeded this limit(s)
   //
   // watch the signs here.  if flowIO is into the bag then the accumulated
   // volumes are negative and maximum and minimum limits are negative
   // so the greater and less signs change.
   if ( (_flowIO == FLOW_INTO_BAG) ||
        (_flowIO == FLOW_INTO_BAG_RC) )
   {
      if ( _minimumVolumeKnown && (_accumulatedVolume > _minimumVolume) )
      {
         DataLog(log_level_safe_exec_error) << "Volume in: " << _accumulatedVolume << " - Minimum Volume Exceeded" << endmsg;

         return VOLUME_MIN_EXCEEDED;
      }
      else if ( _maximumVolumeKnown && (_accumulatedVolume < _maximumVolume) )
      {
         DataLog(log_level_safe_exec_error) << "Volume in: " << _accumulatedVolume << " - Maximum Volume Exceeded" << endmsg;

         return VOLUME_MAX_EXCEEDED;
      }
      else
         return VOLUME_OK;
   }
   else
   {
      if ( _minimumVolumeKnown && (_accumulatedVolume < _minimumVolume) )
      {
         DataLog(log_level_safe_exec_error) << "Volume out: " << _accumulatedVolume << " - Minimum Volume Exceeded" << endmsg;

         return VOLUME_MIN_EXCEEDED;
      }
      else if ( _maximumVolumeKnown && (_accumulatedVolume > _maximumVolume) )
      {
         DataLog(log_level_safe_exec_error) << "Volume out: " << _accumulatedVolume << " - Maximum Volume Exceeded" << endmsg;

         return VOLUME_MAX_EXCEEDED;
      }
      else
         return VOLUME_OK;
   }
}

/* FORMAT HASH ee4d5e5bdbb402cd9e1696280f102999 */

/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      s_bag.hpp, a bag
 *
 * ABSTRACT:   Safety's view of a thing that holds fluid.
 *
 */

#ifndef S_BAG_HPP  //  Prevents multiple compilations.
#define S_BAG_HPP

#include <stdio.h>
#include <stdlib.h>

#include "s_valve.hpp"
#include "s_set.hpp"
#include "s_names.hpp"

#define VOL_TOLERANCE_FOR_UNKNOWN_FLOW 5.0 // ml allowed for a pump when we don't know set type
#define MAX_BAG_MULTIPLIER             1.3 // limit bag volume to 130% of procedure limit

enum VOLUME_VALID
{
   VOLUME_OK           = 0, // volume is within limits (if known) or volume limits are unknown
   VOLUME_MIN_EXCEEDED = 1, // volume has fallen below the minimum allowed
   VOLUME_MAX_EXCEEDED = 2, // volume has risten above the maximum allowed
   VOLUME_FLOW_ERROR   = 3  // volume cannot be accrued due to insufficient info about the set
};

/*******************************************************************************
*
*  baseBag CLASS DEFINITION
*
******************************************************************************/
class baseBag
{
public:
   virtual ~baseBag();                               // base destructor

   virtual VOLUME_VALID UpdateVol (char cCycle) = 0;     // update the volume in the bag based on pump(s) and valve

   void SetFlowIO (FLOW_DIRECTION dir);      // set normal flow which is determined from set type
   void SetValveIO (HW_VALVE_STATES valve);  // set valve position to allow flow (determined from set type)
   void SetInitialVol (float vol);           // set initial volume allowed, if known
   void SetMaxVol (float vol);               // set maximum volume allowed, if known
   void SetMinVol (float vol);               // set minimum volume allowed, if known
   void ResetCycleVol (char cCycle);         // reset cycle volume for a new draw/return cycle
   void InitializeForState (long newState);  // set variables for new state
   bool GetInitialVol (float& vol);          // get initial volume allowed, if known
   bool GetMaxVol (float& vol);              // get maximum volume allowed, if known
   bool GetMinVol (float& vol);              // get minimum volume allowed, if known

   valve           _myValve;                // valve that stops/starts flow

   float           _initialVolume;             // starting volume of bag, if known
   float           _maximumVolume;             // maximum fluid allowed in this bag, if known
   float           _minimumVolume;             // minimum fluid allowed in this bag, if known
   bool            _initialVolumeKnown;        // TRUE if the initial volume is known
   bool            _maximumVolumeKnown;        // TRUE if the maximum volume is known
   bool            _minimumVolumeKnown;        // TRUE if the minimum volume is known
   FLOW_DIRECTION  _flowIO;                    // normal flow direction for this bag
   HW_VALVE_STATES _valveIO;                   // normal valve position that allows flow for this bag
   bool            _flowTypeRequired;          // TRUE requires flow direction (i.e. set type) is known
   float           _accumulatedVolume;         // all fluid accumulated in bag, can be negative
   float           _deltaVolume;               // fluid accumulated in this sample
   float           _drawCycleVolume;           // fluid accumulated in current draw cycle while valve !return
   float           _returnCycleVolume;         // fluid accumulated in current return cycle while valve !return
   float           _drawCycleVolumeInfusion;   // fluid accumulated in current draw cycle while bag is collecting
   float           _returnCycleVolumeInfusion; // fluid accumulated in current return cycle while bag is collecting

protected:
   baseBag();                                    // base constructor

   virtual void logPumpState () const = 0;         // set variables for new state
};

/*******************************************************************************
*
*  bag CLASS DEFINITION
*
******************************************************************************/
class bag
   : public baseBag
{
public:
   // constructor for a bag with pump identification and no valve
   bag(SafetyPumps pump);

   // destructor for a bag
   virtual ~bag();

   virtual VOLUME_VALID UpdateVol (char cCycle);

protected:
   SafetyPumps pumpId;                        // pump that causes normal flow

   virtual void logPumpState () const;         // set variables for new state
};

/*******************************************************************************
*
*  rbcBag CLASS DEFINITION
*
*  A simple specialization of the baseBag model class used by RBC containers.
*  This class is differs from the bag class in that it represents the special
*  case in Trima of a modeled bag for which no real pump exists, thus requiring
*  the use of a "Pseudo-pump".  We also incorporate special behavior to deal
*  with the new requirement for disposables capable of DRBC Automated Split.
*
******************************************************************************/
class rbcBag
   : public baseBag
{
public:

   //
   // In the case of an automated split cassette, collection mode/state based
   // on valve position becomes more complex.  We have to rely on other information
   // not available within the RBC Bag (or even RBC Container) object to know whether
   // we are in collect or return; specifically, this judgement is based on the
   // platelet valve position when RBC valve is in RETURN.  To capture this during
   // the run, we need a "Collect Mode" variable that indicates whether or not we're
   // in collect -- either Product 1 or 2 for doubles.
   //
   enum CollectMode
   {
      CM_UNKNOWN,
      CM_OPEN,
      CM_COLLECT,
      CM_RETURN
   };

   rbcBag();
   virtual ~rbcBag();

   // Collection Mode accessors
   void        setCollectMode (HW_VALVE_STATES RBCValveControl, HW_VALVE_STATES PlateletValveControl);
   void        setCollectMode (CollectMode cm) { _mode = cm; }
   CollectMode getCollectMode () { return _mode; }

   virtual VOLUME_VALID UpdateVol (char cCycle);

protected:
   CollectMode _mode;

   virtual void logPumpState () const;         // set variables for new state
};
#endif // S_BAG_HPP

/* FORMAT HASH 6f3cb51ab408967ccd39cbfd31ac2ae5 */

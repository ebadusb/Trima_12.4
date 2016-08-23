/*
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      rbcdetectoryield.hpp
 *
 * ABSTRACT:  This class calculates the integrated platelet collection yield
 *            using the RBC detector and determines if the run has to be flagged
 *            as having a poor yield.
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 */


#ifndef _RBCDETECTORYIELD_H_
#define _RBCDETECTORYIELD_H_

// Include files

#include "an_alarm.h"
#include "rbcdat.h"
#include "timermessage.h"

//
// Yield Integration interval
//
#define YIELD_INTEGRATION_INTERVAL 6000
// 6000 msec = 6 seconds
//


class RBCDetectorYield
{
public:

   // Default constructor
   RBCDetectorYield();

   // Destructor
   virtual ~RBCDetectorYield();

   // Initialize the class
   void Initialize ();

   // Called as the callback function for the
   //  Hal Status data message
   void ProcessHalStatus ();

   // Compute final yield, cleanup all class dynamic heap memory,
   // and stop the integration timer
   void Finalize ();

private:

   // Return the current RED value, within min/max limits
   float FilteredRED ();

   // Start the Yield Integration
   void StartYieldIntegration ();

   void ComputeCr ();

   // Called as the callback function for the
   //  YIELD_INTEGRATION_INTERVAL timer.
   // Also called to compute the final yield.
   void ComputeYield ();

private:

   float _previousVolume;

   float _VCuvette;          // Total volume thru cuvette since the beginning of Yield Integration.

   float _Cwset;

   float _Yr;

   // Class' state variable
   enum { RBC_DETECTOR_BASELINE_STATE,
          WAITING_FOR_COLLECT_VALVE_STATE,
          INTEGRATING_YIELD_STATE,
          EVALUATING_INTEGRATED_YIELD} _state;

   //  YIELD_INTEGRATION_INTERVAL timer
   TimerMessage _timer;

};


#endif

/* FORMAT HASH 833187f5138bbef155a6a8bdadca07c1 */

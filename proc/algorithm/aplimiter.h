/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      aplimiter.h
 *
 *       Object used to set the APS Pressure Limits dynamically while
 *        the donor is connected based on the system state, and donor
 *        statistics.
 *
 */

#ifndef _APLIMITER_H_
#define _APLIMITER_H_

#include <string>

#include "procstate.h"
#include "states.h"
#include "timekeeper.h"
#include "trimamessages.h"

class APLimiter
{
public:
   //
   // Constructor
   APLimiter();


   //
   // Destructor
   virtual ~APLimiter();

   //
   // Functions used to initialize the object with all
   //  the information it needs
   //
   //  Returns 0 -- The data has not been passed in at this point
   //  Returns 1 -- The data has been initialized properly
   int init (float lowLimit, float highLimit);

   //
   // Function called to set the limits, with the currentState() of the
   // state machine from which this is invoked.
   //
   void setLimits (const string& currentState);

private:
   //
   // Checks for a state change (substate or recovery) and resets the
   // state change time keeper.
   //
   bool checkStateChange (const string& currentState);

   //
   // Proc state information
   //
   string     _LastState;
   string     _LastRecovery;
   TimeKeeper _StateChangeTime;

   //
   // Configured High and Low pressure limits
   //
   float _LowConfigLimit;
   float _HighConfigLimit;

   //
   // Last APS limits ...
   //
   float _LastLowLimit;
   float _LastHighLimit;

   bool  _FirstTimeInDRBCCollectReturn;
   bool  _FirstReturnLimiterActive;

   bool  _SpecialBlackStampNeedle;
};

#endif

/* FORMAT HASH c692a99fe43971043809275c97121b65 */

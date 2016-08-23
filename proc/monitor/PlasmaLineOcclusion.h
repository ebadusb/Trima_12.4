/*
 * Copyright (c) 2011 by CaridianBCT, Inc.  All rights reserved.
 *
 *
 */


#ifndef _PLASMALINEOCCLUSION_H_
#define _PLASMALINEOCCLUSION_H_

// Include files
#include <vxworks.h>
#include "aPQMonitor.h"
#include "timekeeper.h"
#include "pfrmsg.h"
#include "trimamessages.h"


class PlasmaLineOcclusion_Monitor
   : public aPQMonitor
{
   DECLARE_OBJ(PlasmaLineOcclusion_Monitor);

public:
   // Default constructor
   PlasmaLineOcclusion_Monitor();

   // Destructor
   virtual ~PlasmaLineOcclusion_Monitor();

   // Initialize the class
   virtual void Initialize ();

   virtual void enable ();

   virtual void disable ();

   //
   // Function called every status update ...
   virtual void Monitor ();

protected:
   void Process6SecondData ();
   void HandleSubstateChangeMsg ();
   void HandleMidrunDelayTimerMsg ();
   void HandlePfrStatusMsg ();

   void Deactivate ();
   void CalculateBaseline (float currentVc);

private:
   static ProcData*      _pd;

   unsigned long         _numberOfSamples;       // number of samples

   unsigned long         _redReflectanceRaw;     // accumulation of raw samples
   float                 _redReflectanceAverage; // average value
   float                 _redReflectanceTrigger; // trigger value

   unsigned long         _greenReflectanceRaw;     // accumulation of raw samples
   float                 _greenReflectanceAverage; // average value
   float                 _greenReflectanceTrigger; // trigger value

   float                 _epcaCollectVolume;     // volume collected during EPCA

   ProcSubstateChangeMsg _substateChangeMsg;     // Substate change message
   Message<PFR_Status>   _pfrStatusMsg;          // PFR status message

   TimeKeeper            _dataCollectorTimer;    // Timer for calculating values
   float                 _midrunStartPltVol;     // Starting platelet volume

   bool                  _redCriteriaTriggered;   // Criteria for red trigger
   bool                  _greenCriteriaTriggered; // Criteria for green trigger

   bool                  _flagTriggered;
   bool                  _baselineEstablished;

private:
   bool _featureEnabled;
};


#endif

/* FORMAT HASH 5d5b21f7a84c0e96078f44a895702382 */

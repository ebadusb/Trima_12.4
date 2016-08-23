/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      collect.h
 *
 * ABSTRACT:   This state controls the trima actions during the predict control
 *             phases of the run.
 *
 */


#ifndef COLLECT_H
#define COLLECT_H

#include "recoveryman.h"
#include "stateabs.h"
#include "states.h"
#include "vtargetchecker.h"
#include "timekeeper.h"
#include "an_alarm.h"

class Collect
   : public StateAbs
{
   DECLARE_STATE(Collect);
public:

   // Default constructor
   Collect();

   // Default destructor
   virtual ~Collect();

   //
   // Return the current state ...
   //
   virtual const char* currentState () const;

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   // virtual StateAbs *clone() const { return ( new Collect( *this )); };

protected:

   //
   // Functions to do this state's processing ...
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // Function called upon first entrance to this state
   //
   virtual int preEnter ();

   //
   // Function called upon transition from this state
   //
   virtual int postExit ();

   //
   // Reset the state upon termination
   //
   virtual void reset ();

   //
   //  Function to determine the next substate to process
   //
   State_names findPlace ();

   // protect against trying to execute substates that dont make sense/
   bool BogusSubState (const State_names state);

   //
   // Function which commands the pumps using the
   //  message data
   //
   void  commandPumps (const State_names substate);
   float rampInlet (const int drawCycle, const State_names substate);
   float rampReturn (const int drawCycle, const State_names substate, const float qin);
   float rampCollect (const int drawCycle, const State_names substate);

   // only effects Qp in PTF DRBC runs during the second setup
   float derampPlasmaForDRBC (const int drawCycle, const State_names substate);


   void commandCentrifuge (const State_names substate);

   //
   // Function used to limit the return flow at the beginning of a procedure
   void limitQrp (float& qrp);

   //
   // Function used to calculate and save the infusion rate
   void calculateIR ();

   //
   // Log the data from predict for debugging purposes...
   void logPredictData ();

protected:

   //
   // Manager for the recovery states
   //
   RecoveryMan _RecoveryMan;

   //
   // Flag to signify we are done with collection
   //
   int _CollectDone;

   //
   // Flag used to limit the return speed of the initial recoveries
   //
   int _LimitReturnSpeed;

   //
   // Qch Ramping
   //
   TimeKeeper _CommandTimer;

   //
   // Vin from the beginning of the current substate
   float _StartVin;

   //
   // Target checker
   VTargetChecker _TargetChecker;

   //
   // Previous qcollect value needed for incremental ramping
   //  of the collect pump speed
   float _QcollectPrev;

   //
   // Log the flows if they change ...
   //
   float      _PrevQac;
   float      _PrevQin;
   float      _PrevQrp;
   float      _PrevQpls;
   float      _PrevQplt;
   float      _PrevRpm;

   anAlarmMsg _DRBCSplitAlarm;
   bool       _doneSplitYet;
   bool       _prePirRamp;

   bool       _LimitedMidrunReturn;
   int        _FirstMidrunReturnNum;

};


#endif

/* FORMAT HASH ae028ef422d17e4d972a216eb1c46d41 */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      statecontrol.h
 *
 */


#ifndef STATECONTROL_H  //  Prevents multiple inclusions.
#define STATECONTROL_H

// Include files
#include <vxWorks.h>

#include "override_alr_msg.h"
#include "rbcdetectorinit.h"
#include "stateabs.h"
#include "timermessage.h"

#include "timekeeper.h"

// Class Declaration
class StateControl
   : public StateAbs
{
public:
   // Default constructor
   StateControl();

   // Default destructor
   virtual ~StateControl();

   //
   // Initialize the main controller state
   //
   virtual int init ();

   //
   // Callback for the run targets message
   //
   void newTargets ();

protected:

   //
   // Pure virtual functions which should be overridden in the
   //  base derived class
   //
   //   returns NO_TRANSITION -- Proper processing
   //   returns NOT_INITIALIZED -- Initialization error
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // Function to override current commands due to an
   //  alarm condition ...
   //
   void alarmOverrides ();

   //
   // Function to properly set the door lock or unlock command
   //  based on the speed of the centrifuge ...
   //
   void doDoorCommands ();
   void powerRequestCb ();

   //
   // Function which performs an AKO if requested ...
   //
   void doAKO ();

   //
   // Function to handle centrifuge speed commanding ...
   //
   void doCentrifugeCommands ();

   //
   // Function to check for calibration of the RBC detector
   //
   void calibrateRBCDetector ();

   //
   // Callback for the AKO request message
   //
   void akoRequested ();

   //
   // Callback for the RBC Uncalibrated message
   //
   void rbcDetectorUncalibrated ();

   //
   // Function to check the commanded vs. actual of the
   //  system
   //
   void checkCommands ();

   //
   // Callback for the hardware out of control timer
   //
   void systemOutOfControl ();

   //
   // Callback for alarm overrides
   //
   void receivedOverride ();

   //
   // Callback for saline bolus request message
   //
   void salineBolusRequest ();

   //
   // Predict communication hooks
   //
   virtual void predictCommunication ();

   // Copy constructor ( not implemented )
   StateControl(const StateControl& s);

   //
   // Assignment operator
   //
   StateControl& operator = (const StateControl&);

   //
   // Equality operator
   //
   int operator == (const StateControl&) const;
   int operator == (const char*) const;
   //
   // Comparison operator
   //
   int operator < (const StateControl&) const;
   int operator < (const char*) const;

   //
   // Compare function
   //
   int compare (const StateControl&) const;

   // Set/Get for parent state
   void            parentState (const StateAbs*) {}
   const StateAbs* parentState () const { return StateAbs::parentState(); }

   void predictTimeout ();

protected:

   //
   // Alarm override message
   //
   override_alr_msg _AlarmOverrideMsg;
   override_alr     _PreviousAlarmOverrides;

   //
   // AKO request message
   //
   AKOMsg _AKORequestMsg;

   //
   // RBC Uncalibrated message
   RBCDetectorUncalibratedMsg _RBCUncalibratedMsg;

   //
   // Stop and Pause alarms
   //
   anAlarmMsg _PauseAlarm;
   anAlarmMsg _StopAlarm;

   //
   // Hardware control timers ...
   //
   TimerMessage _CentStopTimer;
   bool         _CentStopTimerStarted;
   TimerMessage _PumpsStopTimer;
   bool         _PumpsStopTimerStarted;

   //
   // Flag to signify we are waiting to unlock the door ...
   //
   bool _UnlockDoor;

   //
   // RBC Detector Initializer object ...
   //
   RBCDetectorInit _RBCCalibrator;

   //
   // Flags to signify an AKO was requested and completed ...
   //
   bool _AKORequested;
   bool _AKOCompleted;

   //
   // Volume pumped during the AKO
   //
   float _AKOVolume;

   //
   // Last non-paused centrifuge speed
   //
   float _LastSeenCentSpeed;

   //
   //  Predict timeout timer and associated callback
   //
   TimerMessage _PredictTimer;

   //
   // Monitor power request messages to handle door locking and
   //  unlocking ...
   //
   RequestSafetyPowerMsg _PowerRequestMsg;

   //
   // Flag to save the fact that we are allowing a saline bolus during
   //  the stop or pause alarms ...
   //
   bool           _PerformSalineBolus;
   SalineBolusMsg _SalineBolusRequestMsg;

   // how log predict took in its calcs....
   TimeKeeper _predictTime;

   bool       _inHalfSpeedAPS;
   bool       _overrideChanged;

   float      _lastQin, _lastQac;

   bool       _PPRcheck;

};

#endif

/* FORMAT HASH cdb4699972adc06680e2a4d4ac810efc */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      donordisconnect.h
 *
 */


#ifndef DONORDISCONNECT_H  //  Prevents multiple inclusions.
#define DONORDISCONNECT_H

#include "an_alarm.h"
#include "trimamessages.h"
#include "stateabs.h"
#include "fastdata.h"
#include "timermessage.h"

class DonorDisconnect
   : public StateAbs
{
   DECLARE_STATE(DonorDisconnect);
public:

   DonorDisconnect();

   virtual ~DonorDisconnect();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback for the notification of failure timer
   //
   void testFailed ();

   //
   // Callback for the request permission message
   //
   void shouldStartTest ();

   //
   // Callback for the test timer
   //
   void testTooSlow ();

   //
   // Callback for the stabilization timer
   //
   void pressureStabilized ();

   //
   // Callback for the pressure hold timer
   //
   void pressureCheck ();

   //
   // Callback for the power request response
   //
   void powerRequestResponse ();

   //
   // Callback for the APS message
   //
   void reachedPressure ();

   // Callback for Proc's SetRunType message
   void setDisconMssStatus ();

   // Callback for Safety's disconnect status
   void safetyStatus ();

   // Callback for GUI message that says we're ready to transition.
   void readyToTransitionNotify ();

protected:

   //
   // Functions to do this states processing ...
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
   // Functions used to perform the test
   //
   void performTest ();
   void setUpTest ();
   void pressurize ();

   //
   // Function to ask for 24V Power
   //
   void request24VPower ();

   //
   // Stop the pumps
   //
   void stopPumps ();

   //
   // Close the valves
   //
   void closeValves ();

   //
   // Stop the timers
   //
   void stopTimers ();

   //
   // Resets the state upon termination
   //
   virtual void reset ();

   //
   //
   //  Determine hypovolemia constraints
   //
   virtual int Hypovolemic ();

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

   //
   // Message that allows the test to start
   //
   DonorDisconnectedMsg _PermissionGrantedMsg;

   //
   // Message that receives response from GUI that
   // we're ready to transition from donor disconnect.
   //
   TellProcToTransitionFromDisconnect _ReadyToTransitionMsg;

   //
   // Message that receives the response from the
   //  safety power request
   //
   SafetyTestDoneMsg _PowerRequestResponseMsg;

   //
   // Message from the control driver signifying the
   //  pressure has violated the limit
   //
   Message<FASTFILTER> _APSMsg;

   // Message for SetRunType info
   TriggerSetRunTypeMsg _SetRunType;

   //
   // Timer for the test
   //
   TimerMessage _TestTimer;

   //
   // Timer for a delay in notification for system synchronization
   //
   TimerMessage _NotificationTimer;

   //
   // Timer for the pressure stabilization
   //
   TimerMessage _StabilizationTimer;

   //
   // Timer for the pressure hold test
   //
   TimerMessage _PressureHoldTimer;

   //
   // Alarm that the cassette is out of position
   //
   anAlarmMsg _BadCassettePositionAlarm;

   //
   // Alarms if when the test fails
   anAlarmMsg _DisconnectTestAlert;
   anAlarmMsg _DisconnectTestAlarm;
   anAlarmMsg _DisconnectTestMssAlert;
   anAlarmMsg _DisconnectTestMssAlarm;

   //
   // Flag used to signify the test can be started
   //
   int _PerformTest;

   //
   // Flag to signify the power was turned on
   //
   int _PowerOn;

   //
   // Flag to signify when the test is completed
   //
   int _TestPassed;

   //
   // Flag to signify the test failed
   //
   int _TestFailed;

   //
   // Flag to signify the test has failed and we are waiting to
   //  notify the operator
   //
   int _FailPaused;

   //
   // Flag to signify the cassette position caused the test to fail
   //
   int _CassettePositionError;

   //
   // Number of times the test has failed
   //
   int _AlarmDisplayCount;

   //
   // Set if the test has failed at least once.
   //
   bool _TestHasFailed;

   //
   // Set if GUI has signaled that we're ready for transition.
   //
   bool _ReadyForTransition;

   //
   // Current functional test step
   //
   CallbackBase _TestStep;

   //
   // Target pressure
   //
   float _TargetPressure;

   //
   // Maximum pressure
   //
   float _MaximumPressure;

   //
   // Current APS reading after pressure stabilized
   //
   float _CurrentAPS;

   //
   // Flag to say signify the pressure goal was reached
   //
   int _AchievedPressureGoal;

   //
   //
   //  Initial distance to hypovolemia
   //
   float _HypovolemiaDistance;
   float _VinAtStart;

   //
   // Flag to save whether or not we registered for the power response yet ...
   bool _PowerRespRegistered;

   // Message from safety indicating its status of disconnect.
   SafetyDisconnectStatMsg _SafetyDiscoStatus;
   bool                    _safetyConfirmed;

private:
   // Prevent copying
   DonorDisconnect(const DonorDisconnect&);
   DonorDisconnect& operator = (const DonorDisconnect&);
};


#endif

/* FORMAT HASH c64a39816f50ebc06ee2a514ebcec1d6 */

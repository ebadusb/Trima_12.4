/*
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pfrecover.h
 *
 * ABSTRACT:   This header file defines the global data and classes for the
 *                  power fail recovery selection state.
 *
 * Revision 1.10  2003/06/19 18:43:37Z  ms10234
 * 5829 - changes for PFR
 * Revision 1.9  2003/04/30 14:09:52Z  jl11312
 * - start of power fail recovery implementation
 * Revision 1.8  2002/10/03 23:34:02Z  ms10234
 * Changes to make proc execute
 * Revision 1.7  2002/06/05 14:30:02  sb07663
 * Modifications for compatibility with new Vx messaging system
 * Revision 1.6  2001/12/11 20:16:53  jl11312
 * - added handling for Vista PFR status message
 * Revision 1.5  2001/06/21 21:36:29  jl11312
 * - corrected handling of alarms during PFR (IT 4922)
 * Revision 1.4  2001/06/14 18:00:10  jl11312
 * - disabled PFR notifications to predict unless run is resuming (IT 4922)
 * - reorganized function calls to improve readability
 * Revision 1.3  2001/06/13 22:17:40  jl11312
 * - added restoration of alarms during power fail recovery (IT 4922)
 * Revision 1.2  2001/06/05 20:09:52  jl11312
 * - added handling of power-fail recovery to rinseback
 * - changes required to move power test after continue from power-interrupted alarm
 * Revision 1.1  2001/06/01 20:15:38  jl11312
 * Initial revision
 */


#ifndef PFRECOVER_H
#define PFRECOVER_H

#include <stateabs.h>
#include "pfrmsg.h"
#include "trimamessages.h"
#include "timermessage.h"

class PFRecover
   : public StateAbs
{
   DECLARE_STATE(PFRecover);
public:

   // Default Constructor
   PFRecover();

   // Copy Constructor
   PFRecover(const PFRecover& state);

   // Destructor
   virtual ~PFRecover();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new PFRecover(*this)); }

protected:
   //
   // Functions to do this state's processing ...
   //
   virtual int preProcess  ();
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
   // Delete any dynamic memory here
   //
   virtual void cleanup ();

   //
   // Handle power-fail data recovery and alarm annunciation
   //
   void checkRecoveryByAlarmState (bool& continueAllowed, bool& rinsebackAllowed);
   void checkRecoveryByCassetteState (bool& continueAllowed, bool& rinsebackAllowed);
   void setPFAlarm ();
   void sendModifyPredictMessage (void);

   //
   // Handle notification of other tasks of power fail recovery status
   //
   void handleProcToSafetyDonorMessage ();
   void handlePowerFailRecoverNotification ();
   void handlePredictNotification ();

   //
   // Handle rinseback interaction with GUI
   //
   void handleRinseback  ();
   void rinsebackRequest ();
   void rinsebackStatus  ();


protected:

   TimerMessage _modifyMessageTimer;

   //
   // State specific data members
   //
   bool         _pfAlarmHasBeenSet;          // non-zero if power fail alarm has been set
   ALARM_VALUES _pfAlarmType;                // type of power fail alarm (one of POWER_FAIL_CONTINUE,
                                             // POWER_FAIL_RINSEBACK, or POWER_FAIL_ABORT)

   enum { TransitionToPFPowerTests = 1 };    // determines if state branches to PFPowerTest or StartupTests
   enum { TransitionToStartupTests = 2 };

   bool                _rinsebackAllowed;    // non-zero if rinseback is allowed from current state
   RinsebackRequestMsg _rinsebackRequestMsg; // message sent when operator presses rinseback button
   RinsebackStatusMsg  _rinsebackStatusMsg;  // message from GUI requesting of rinseback is allowed
};

#endif // ifndef PFRECOVER_H

/* FORMAT HASH ac524d0511bf01ff1c4f9231bfc4d25f */

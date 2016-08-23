/*
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pfbranch.h
 *
 * ABSTRACT:   This header file defines the global data and classes for the
 *                  power fail branch state, which determines the state at
 *                  which the run should be restarted.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/pfbranch.h 1.12 2007/05/30 20:54:32Z dslausb Exp dslausb $
 * $Log: pfbranch.h $
 * Revision 1.12  2007/05/30 20:54:32Z  dslausb
 * IT 8050 - Fixed PFR problems
 * Revision 1.11  2006/03/18 00:06:07Z  jheiusb
 * revised for PFR in MSS
 * Revision 1.10  2003/11/05 22:13:56Z  ms10234
 * 5959 - compensate for time drift between control and safety.
 * Revision 1.9  2002/10/28 23:31:24Z  ms10234
 * Changed proc files to use interface class - ProcData
 * Revision 1.8  2002/10/03 23:33:59  ms10234
 * Changes to make proc execute
 * Revision 1.7  2002/08/06 18:56:13  ms10234
 * Changed all int message names to end with "Msg"
 * Revision 1.6  2002/06/21 21:25:07  sb07663
 * Mods to remove dynamic data and implement CDS
 * Revision 1.5  2002/06/05 14:30:01  sb07663
 * Modifications for compatibility with new Vx messaging system
 * Revision 1.4  2001/08/31 20:18:34  jl11312
 * - changed pfbranch to close valves before resuming procedure
 * Revision 1.3  2001/06/21 21:36:26  jl11312
 * - corrected handling of alarms during PFR (IT 4922)
 * Revision 1.2  2001/06/13 22:16:03  jl11312
 * - added handling of transition to start draw screen from power fail
 * - added restoration of alarms during power fail recovery (IT 4922)
 * Revision 1.1  2001/06/05 21:20:01  jl11312
 * Initial revision
 */


#ifndef PFBRANCH_H  //  Prevents multiple compilations.
#define PFBRANCH_H

#include "stateabs.h"
#include "an_alarm.h"
#include "timermessage.h"
#include "trimamessages.h"

class PFBranch
   : public StateAbs
{
   DECLARE_STATE(PFBranch);
public:

   // Default Constructor
   PFBranch();

   // Copy Constructor
   PFBranch(const PFBranch& state);

   // Destructor
   virtual ~PFBranch();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new PFBranch(*this)); }

   //
   // Callback function for valve timer ...
   //
   virtual void valvesTimedOut ();

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
   // Delete any dynamic memory here
   //
   virtual void cleanup ();

   //
   // Determine transition type and handle notifying other system tasks
   //
   void handleTransition (void);

   //
   // Handle rinseback interaction with GUI
   //
   void rinsebackStatus (void);

   //
   // Command all valves
   //
   void commandValves (void);

protected:
   //
   // State specific data members
   //
   enum
   {
      TransitionToACPrime         = 1,
      TransitionToConnectDonor    = 2,
      TransitionToDonorConnected  = 3,
      TransitionToMSS             = 4,
      TransitionToMSSDisconnect   = 5,
      TransitionToDonorDisconnect = 6
   };

   ALARM_VALUES       _currentAlarm;               // current alarm type
   struct timespec    _currentAlarmMsgTime;        // time at which current alarm was set
   int                _currentRinsebackStatusSet;  // non-zero if got rinseback status request from GUI for current alarm
   int                _currentAlarmSet;            // non-zero if current alarm has been received in proc alarm queue
   int                _alarmsRestored;             // non-zero if alarm restoration is complete

   RinsebackStatusMsg _rinsebackStatusMsg;          // message from GUI requesting of rinseback is allowed

   //
   // Timer to ensure the valves move within a specified time
   //
   TimerMessage _ValveTimer;

   //
   // Valve Alarms
   //
   anAlarmMsg _RBCAlarm;
   anAlarmMsg _PlasmaAlarm;
   anAlarmMsg _CollectAlarm;

};

#endif // ifndef PFBRANCH_H

/* FORMAT HASH 5c4ae93528c9f44b3a37232a29d8cb67 */

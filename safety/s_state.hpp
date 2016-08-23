/*******************************************************************************
 *
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      s_state.hpp
 *             Safety state management class definitions.
 *
 *
 * ABSTRACT:   This file contains the class definitions for safety
 *             state management classes.
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 ******************************************************************************/
#ifndef S_STATE_HPP
#define S_STATE_HPP

#include "trima_assert.h"

#include "s_exec.hpp"
#include "s_pmsg.hpp"
#include "s_glob.hpp"
#include "s_res.hpp"
#include "s_donor.hpp"
#include "s_pfr.hpp"
#include "s_disc.hpp"
#include "s_door.hpp"

#include "safproc.h"  // hct and precount ranges.

// 07/29/97 msm
// modify to use procedure's state table to
// prevent maintenance probs if proc needs to define a
// new state
#include "states.h"    // proc file

// forward references
class aRequestSafetyPowerMsg_rcvr;
class aSafetyStatusMsg_rcvr;
class aControlCommandsMsg_rcvr;

/*++++++++++++++++++++++++++++++++++++++++++++++
        Safety System States Defines : Start
++++++++++++++++++++++++++++++++++++++++++++++++*/


// permission values used in the state transition table
enum Permission
{
   FORBID=0,   // Never allowed transition
   NORMAL,     // Normal transition
   N_OR_P,     // Normal or PFR transition
   PFR_OK,     // Allowed only in PFR
   NOT_AP,     // Not applicable because not a safety state
   IGN_PF      // Allow transition during PFR but do not carry out any actions
};

// state transition defines
#define TRANSITION_UNVERIFIED                        0    // system state allowed flag for not yet tested for valid trans
#define VALID_STATE_TRANSITION                       1    // requested state transition is allowable
#define INVALID_STATE_TRANSITION                     2    // requested state transition is forbidden
#define PFR_STATE_TRANSITION                         3    // requested state transition is allowed if PFR
#define UNDEFINED_SAFETY_STATE                       4    // requested state is not known to safety
#define IGNORED_TRANSITION_DURING_PFR                5    // requested state transition not performed during PFR

// Defines for gateways which have to be crossed to allow the transition
#define NO_GATE         0x0000     // no gate required
#define DISC_RESTORE    0x0001     // checkToRestoreDisconnectData();
#define PFR_WHATCYCLE   0x0008     // whatCycleAreWeIn();
#define CRC_GATE        0x0010     // checkCRCDisconnect();
#define ACCON_T1_GATE   0x0020     // checkACConnectT1();
#define DCON_T1_GATE    0x0040     // checkDonorConnectT1();
#define FIRST_DRAW_GATE 0x0080     // checkFirstDrawComplete();
#define DVIT_GATE       0x0100     // checkDonorVitals();
#define BRUN_UPDATE     0x0200     // updateBloodPrimeToBloodRun();
#define BRUN_T1_GATE    0x0400     // checkBloodRunT1();
#define DISC_GATE       0x0800     // checkDisconnectTest();
#define UNDEFINED_GATE  0x1000     // Safety has no gate defined for the transition
#define LAST_GATE       0x2000     // end of list


// 07/27/96
// init state processing to limited state trans until machine
// state examined for faults
#define FORCE_STATE_CASS_INIT_DOWN      0     // cassette/machine state limit state processing forcing donor dc
#define NOMINAL_STATE_PROCESSING        1     // nominal state transition matix in force

/*++++++++++++++++++++++++++++++++++++++++++++++
        Safety System States Defines : End
++++++++++++++++++++++++++++++++++++++++++++++++*/

/*******************************************************************************
*
*  aSafetySystemStateChangeMsg_rcvr CLASS DEFINITION
*
******************************************************************************/
// SPECIFICATION:    "receiver" for SystemStateChange command from proc.
//
//
// ERROR HANDLING:   _FATAL_ERROR .

class aSafetySystemStateChangeMsg_rcvr
{
private:

   int   _iLastSystemStateTransitionType;
   short _sGateDone;                        // gateways successfully completed

   TransitionValidationMsg*     _paSafetyTransitionValidationMsg_sndr;
   SystemStateChangeMsg         _theSystemStateChangeMsg;

   aCassette_Safety*            _paCassette;
   aReservoir_Safety*           _paReservoir;
   aDoor_Safety*                _paDoor;
   aDonorDisconnectTest*        _paDCTest;
   aCentrifuge_Safety*          _paCentrifuge;
   aCRC_Timer*                  _paCRC;
   aDonor*                      _paDonor;

   needleMonitor*               _needle;
   aBlinkTest_Safety*           _blink;

   aRequestSafetyPowerMsg_rcvr* _paRequestSafetyPowerMsg_rcvr;
   aMidCycleSwitchMsg_rcvr*     _paMidCycleSwitchMsg_rcvr;

   void       initializeForState (long newState);
   void       checkToRestoreDisconnectData ();
   bool       checkCRCDisconnect ();
   bool       checkACConnectT1 ();
   bool       checkDonorConnectT1 ();
   bool       checkDonorVitals ();
   void       updateBloodPrimeToBloodRun ();
   bool       checkBloodRunT1 ();
   bool       checkFirstDrawComplete ();
   bool       checkDisconnectTest ();
   void       resetRinsebackDeltas ();
   void       whatCycleAreWeIn ();
   Permission getStateTransPermission (State_names oldState, State_names newState);
   short      getStateTransGate (State_names newState, long transitionType);
   bool       doStateTransGate (short oldGate, short newGate);
   int        FindStateNameIndex (State_names lState);

   void notify ();

   aSafetySystemStateChangeMsg_rcvr();                                                     // not implemented
   aSafetySystemStateChangeMsg_rcvr(aSafetySystemStateChangeMsg_rcvr const&);              // not implemented
   aSafetySystemStateChangeMsg_rcvr& operator = (aSafetySystemStateChangeMsg_rcvr const&); // not implemented

public:
   aSafetySystemStateChangeMsg_rcvr(aCassette_Safety* paCassette
                                    , aReservoir_Safety* paReservoir
                                    , aDoor_Safety* paDoor
                                    , aDonorDisconnectTest* paDCTest
                                    , aCentrifuge_Safety* paCentrifuge
                                    , aCRC_Timer* paCRC
                                    , aRequestSafetyPowerMsg_rcvr* paRequestSafetyPowerMsg_rcvr
                                    , aMidCycleSwitchMsg_rcvr* paMidCycleSwitchMsg_rcvr
                                    , aDonor* paDonor
                                    , needleMonitor* pNeedle
                                    , aBlinkTest_Safety* pBlink)
      : _theSystemStateChangeMsg(Callback<aSafetySystemStateChangeMsg_rcvr>(this, &aSafetySystemStateChangeMsg_rcvr::notify),
                                 MessageBase::SNDRCV_RECEIVE_ONLY)
   {
      trima_assert(paCassette);
      trima_assert(paReservoir);
      trima_assert(paDoor);
      trima_assert(paDCTest);
      trima_assert(paCentrifuge);
      trima_assert(paCRC);
      trima_assert(paRequestSafetyPowerMsg_rcvr);
      trima_assert(paMidCycleSwitchMsg_rcvr);
      trima_assert(paDonor);
      trima_assert(pNeedle);
      trima_assert(pBlink);
      _iLastSystemStateTransitionType       = TRANSITION_UNVERIFIED;

      _paSafetyTransitionValidationMsg_sndr = new TransitionValidationMsg(MessageBase::SEND_GLOBAL);

      _sGateDone                            = NO_GATE;

      _paCassette                           = paCassette;
      _paReservoir                          = paReservoir;
      _paDoor                               = paDoor;
      _paDCTest                             = paDCTest;
      _paCentrifuge                         = paCentrifuge;
      _paCRC                                = paCRC;
      _paDonor                              = paDonor;

      _needle                               = pNeedle;
      _blink                                = pBlink;

      _paRequestSafetyPowerMsg_rcvr         = paRequestSafetyPowerMsg_rcvr;
      _paMidCycleSwitchMsg_rcvr             = paMidCycleSwitchMsg_rcvr;

   }


   ~aSafetySystemStateChangeMsg_rcvr()
   {
      delete(_paSafetyTransitionValidationMsg_sndr);
      _paSafetyTransitionValidationMsg_sndr = NULL;

      _paCassette                           = NULL;
      _paReservoir                          = NULL;
      _paDoor                               = NULL;
      _paDCTest                             = NULL;
      _paCentrifuge                         = NULL;
      _paDonor                              = NULL;

      _needle                               = NULL;
      _blink                                = NULL;

      _paRequestSafetyPowerMsg_rcvr         = NULL;
      _paMidCycleSwitchMsg_rcvr             = NULL;
   }

   void ifPFRThenRestore ();

};  // END aSafetySystemStateChangeMsg_rcvr class defintion

#endif /* S_STATE_HPP */

/* FORMAT HASH f77c9c95777e886f255e0ad419368c48 */

/*******************************************************************************
 *
 * Copyright (c) 1996-1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.9  2002/08/08 17:54:36Z  pn02526
 * Add #includes of new message system files.
 * Use the new buffered message base class definition to send the SafetyTestDoneMsg, rather than deriving class aSafetyTestDoneMsg_sndr from an INT32 message base class.
 * Add SafetyPowerControlTimer ::_myTimer to use the new timer message class definition, rather than deriving from a timer message class.
 * Add aRequestSafetyPowerMsg_rcvr :: _theRequestSafetyPowerMsg to use the new buffered message class definition, rather than deriving from an INT32 message base class.
 * Revision 1.8  2000/07/26 16:07:15  bs04481
 * Relocate code to control solenoid power from the power contol
 * timer to the door object because the door object normally
 * controls the solenoid power based on centrifuge speed.  Also,
 * delay 1 second before sending a response for solenoid power
 * disable.
 * Revision 1.7  2000/07/06 01:20:38  bs04481
 * Add handling for solenoid power to RequestSafetyPower message.
 * Respond to all RequestSafetyPower messages, not just test
 * requests.
 * Revision 1.6  2000/06/08 21:54:02  bs04481
 * Respond to power requests along with test requests
 * Revision 1.5  2000/05/26 17:12:21  BS04481
 * More maintanence changes:  This set contains changes for
 * 1. Moving valve object from container to bag
 * 2. Removing SafetyTimer->SafetySoftTimer and letting the
 *      switch and disconnect tests have their own timers.
 * 3.  All message types are changed to NO_BOUNCE to cut down
 *      the on unnecessary stuff in the message que.
 * Revision 1.4  2000/03/15 18:23:11  BS04481
 * Move power test and power test timer objects from this file
 * to pwr_test
 * Revision 1.3  1999/08/01 20:53:20  BS04481
 * Non-critical code review changes identified at the 3.2 release.
 * Revision 1.2  1998/11/05 16:23:05  bs04481
 * Timeouts should be in sec not in msec
 * Revision 1.1  1998/09/28 21:50:42  bs04481
 * Initial revision
 *
 *
 */


#ifndef S_POWER_HPP  //  Prevents multiple compilations.
#define S_POWER_HPP

#include "timermessage.h"
#include "callback.h"
#include "trimamessages.h"

#include "error.h"       // 07/26/96 _FATAL_ERROR, _LOG_ERROR
#include "hw.h"
#include "s_glob.hpp"
#include "s_disc.hpp"
#include "s_door.hpp"
#include "pwr_test.hpp"

#define MAX_SOLENOID_OFF_TIME 15    // control based on command for a max of 15 seconds.
                                    // after that, control it based on centrifuge speed.
#define RESPOND_SOLENOID_OFF_TIME 1 // respond to solenoid off request after 1 seconds.

/*******************************************************************************
*
*  aSafetyTestDoneMsg_sndr CLASS DEFINITION
*
******************************************************************************/
// SPECIFICATION:    derived message class, "sender" for SafetyTestDone response to procedure
//                   following successful completion of power tests.
//
// NOTES :
//
// class aSafetyTestDoneMsg_sndr
// {
//   public:
//      aSafetyTestDoneMsg_sndr() : safetyTestDoneMsg( MessageBase::SEND_GLOBAL ) {};
//      ~aSafetyTestDoneMsg_sndr() {};
//   private:
//      SafetyTestDone safetyTestDoneMsg;
//
// }; // END aSafetyTestDoneMsg_sndr class definition


/*******************************************************************************
*
*  SafetyPowerControlTimer CLASS DEFINITION
*
******************************************************************************/
// SPECIFICATION:    Used to time turn on and turn off times
//
// NOTES :
//
class SafetyPowerControlTimer
{
private:
   aDoor_Safety*         _paDoor;
   aCentrifuge_Safety*   _paCentrifuge;
   aSafety24vPowerTest*  _p24vPowerTest;
   aSafety64vPowerTest*  _p64vPowerTest;
   aDonorDisconnectTest* _paDonorDCTest;
   bool                  _bPowerTestInProgress;
   SafetyTestDoneMsg*    _paSafetyTestDoneMsg_sndr;
   bool                  _bTimeSolenoid;
   bool                  _bSolenoidResponseSent;
   TimerMessage          _myTimer;
   struct timespec       _solenoidTimeout;
   struct timespec       _lastTime;

   void notify ();
   SafetyPowerControlTimer();                                            // not implemented
   SafetyPowerControlTimer(SafetyPowerControlTimer const&);              // not implemented
   SafetyPowerControlTimer& operator = (SafetyPowerControlTimer const&); // not implemented

public:
   SafetyPowerControlTimer(aDoor_Safety* paDoor
                           , aCentrifuge_Safety* paCentrifuge
                           , aSafety24vPowerTest* paSafety24vPowerTest
                           , aSafety64vPowerTest* paSafety64vPowerTest
                           , aDonorDisconnectTest* paDonorDCTest);
   ~SafetyPowerControlTimer();
   long PowerControl24v (HW_ORDERS order, bool blockResponse, bool forceTest);
   long PowerControl64v (HW_ORDERS order, bool blockResponse, bool forceTest);
   void PowerControlDoor (HW_ORDERS order);
   void PowerControlBoth (HW_ORDERS order, bool forceTest);
   void initializeForState (long newState)
   {}
};

/*******************************************************************************
*
*  aRequestSafetyPowerMsg_rcvr CLASS DEFINITION
*
******************************************************************************/
// SPECIFICATION:    derived message class, "receiver" for
//                   RequestSafetyPower message sent to safety by proc
//                   to request power ON/OFF.
//
// NOTES :
//

class aRequestSafetyPowerMsg_rcvr
{
private:

   aSafety24vPowerTest*     _pa24vPowerTest;
   aSafety64vPowerTest*     _pa64vPowerTest;
   SafetyPowerControlTimer* _SafetyPowerControlTimer;
   RequestSafetyPowerMsg    _theRequestSafetyPowerMsg;

   aRequestSafetyPowerMsg_rcvr();                                                // not implemented
   aRequestSafetyPowerMsg_rcvr(aRequestSafetyPowerMsg_rcvr const&);              // not implemented
   aRequestSafetyPowerMsg_rcvr& operator = (aRequestSafetyPowerMsg_rcvr const&); // not implemented

public:

   aRequestSafetyPowerMsg_rcvr(aDoor_Safety* paDoor
                               , aCentrifuge_Safety* paCentrifuge
                               , aSafety24vPowerTest* pa24vPowerTest
                               , aSafety64vPowerTest* pa64vPowerTest
                               , aDonorDisconnectTest* paDonorDCTest);
   ~aRequestSafetyPowerMsg_rcvr();
   void notify ();
   void SetPowerTestInProgress (char cPowerTestInProgress);
   bool PowerT1OK (void);

   void initializeForState (long newState)
   {
      _SafetyPowerControlTimer->initializeForState(newState);
   }


};

#endif /* S_POWER_HPP */

/* FORMAT HASH f1b58e98c94efd3a5c64c178fb8069de */

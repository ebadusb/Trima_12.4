/*******************************************************************************
 *
 * Copyright (c) 1996-1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.10  2007/04/12 20:21:43Z  spriusb
 * New request method to decouple start/restart test behavior from test state setter.
 * Revision 1.9  2003/08/18 18:19:46Z  jl11312
 * - corrected problem with donor disconnect (see IT 5969)
 * Revision 1.8  2003/01/07 20:33:20Z  pn02526
 * Runnable safety executive with CRC checking of the code disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.
 * Revision 1.7  2002/08/07 12:15:31  pn02526
 * Remove extraneous #includes of old message system files, buffmsg.hpp and msg.hpp.
 * Revision 1.6  2000/08/14 14:40:59  pn02526
 * Added GetDonorDisconnectTestState so that s_power's PowerControl24v can detect whether any given power request that occurs during Donor Disconnect Test is expected or extraneous.
 * --- Added comments ---  pn02526 [2001/09/07 15:28:17Z]
 * IT #4648
 * Revision 1.5  2000/07/20 19:31:07  bs04481
 * Change pump down timeout to 35 seconds instead of 30.
 * Revision 1.4  2000/06/30 21:27:14  bs04481
 * Fixed test counting.
 * Revision 1.3  2000/05/26 17:12:04  BS04481
 * More maintanence changes:  This set contains changes for
 * 1. Moving valve object from container to bag
 * 2. Removing SafetyTimer->SafetySoftTimer and letting the
 *      switch and disconnect tests have their own timers.
 * 3.  All message types are changed to NO_BOUNCE to cut down
 *      the on unnecessary stuff in the message que.
 * Revision 1.2  2000/03/15 18:14:38  BS04481
 * 3.3 non-essential code review issue.  Asserts.  Use cassette object for position
 * knowledge; eliminated unneeded reference to control status
 * Revision 1.1  1998/09/28 21:50:37  bs04481
 * Initial revision
 *
 */

#ifndef S_DISC_HPP  //  Prevents multiple compilations.
#define S_DISC_HPP

#include "error.h"       // 07/26/96 _FATAL_ERROR, _LOG_ERROR

#include "chw.hpp"
#include "shw.hpp"
#include "s_valve.hpp"
#include "s_dev.hpp"
#include "s_donor.hpp"
#include "s_res.hpp"

// possible values for cDonorDisconnectTest
//
#define DISCONNECT_TEST_NOT_DONE           0       // state transition to donor d/c test init value, i.e. test not yet tried
#define DISCONNECT_TEST_NOMINAL            1       // donor disconnect test passed
#define DISCONNECT_TEST_IN_PROGRESS        3       // tell safety status notify to snoop on pressure
#define DISCONNECT_TEST_FAILED_HOLDING_LOW 5

// possible values for cDonorDisconnectTestState i.e. the states of the donor disconnect test
// state set to WAITING_FOR_POWER_REQUEST following state trans to DONOR_DISCONNECT after
// power has been disabled
//
#define WAITING_FOR_POWER_REQUEST          1
#define GOING_LOW                          2
#define HOLDING_LOW                        3

enum DisconnectStates
{
   VALID = 0,
   PUMPDOWN,
   STABILIZE,
   READP1,
   HOLD,
   READP2,
   FINAL,
   PASS_DISC,
   FAIL_DISC,
   NONE_DISC
};
#define DISCONNECT_STATES 10   // Array size for the state transition table in notify()

enum DisconnectIndex
{
   CURRENT = 0,
   TRANSITION,
   WAIT
};

#define PRESSURE_LIMIT1          -90.0f      // stabilization pressure limit
#define OFFSET1                   -5.0f      // retry attempt offset
#define STARTING_PRESSURE_LIMIT -350.0f      // don't start if below -350mmHg
#define LEAKAGE_ALLOWANCE         22.0f      // allow 22mmHg of decay during hold
#define GOING_LOW_TIMEOUT        350         // must attain pressure in 35 sec
#define GOING_LOW_STOPPED_TIMEOUT 50         // maximum 5 seconds after pump stops to attain pressure
#define PUMPS_STOPPED_TOLERANCE   40         // call it stopped if less than 40 tics change
#define STABILIZE_TIMEOUT         30         // let pressure settle for 3 sec
#define HOLD_TIMEOUT              50         // must hold pressure within leakage for 5 sec
#define THREE_STRIKES_YER_OUT      3         // donor d/c test failed 3X allow unload

// SPECIFICATION:    Donor disconnect test class.
//
// NOTES :
//
// ERROR HANDLING:   none.

class aDonorDisconnectTest
   : public aSoftTimer_Safety
{
private:
   char               _cDonorDisconnectTest;
   char               _cDonorDisconnectTestState;
   char               _cDonorDisconnectTestCount;
   DisconnectStates   _current;

   float              _fDonorDisconnectPressure;
   float              _fStabilizeThresholdLimit;
   float              _fFinalThresholdLimit;

   bool               _pumpHasBeenStarted;
   long               _lInletAccum;
   long               _lastInletAccum;
   long               _lDisconnectTestMessageCount;
   long               _lastDisconnectTestMessageCount;

   aDonor*            _paDonor;
   aCassette_Safety*  _paCassette;

   aReservoir_Safety* _paReservoir;

   void SetDonorDisconnectTestState (char cDonorDisconnectTestState);
   void SetDisconnectTestPassed ();
   void AreWeFinished ();
   /// lint fixup 07/27/96
   aDonorDisconnectTest();                                         // not implemented
   aDonorDisconnectTest(aDonorDisconnectTest const&);              // not implemented
   aDonorDisconnectTest& operator = (aDonorDisconnectTest const&); // not implemented

public:
   aDonorDisconnectTest(short tickLimit
                        , aDonor* paDonor
                        , aCassette_Safety* paCassette
                        , aReservoir_Safety*  paReservoir);

   ~aDonorDisconnectTest();
   void notify ();
   void initializeForState (long newState);
   void request (void);
   bool ensureNewData (void);

   char GetDonorDisconnectTest (void)
   {
      return( _cDonorDisconnectTest ) ;
   }

   char GetDonorDisconnectTestState (void)
   {
      return( _cDonorDisconnectTestState ) ;
   }

   void Update (float pressure, long accum, long count)
   {
      _fDonorDisconnectPressure    = pressure;
      _lInletAccum                 = accum;
      _lDisconnectTestMessageCount = count;
   }

}; // END aDonorDisconnectTest class definition

#endif /* S_DISC_HPP */

/* FORMAT HASH 0fc82cbb7a119594902c9c5151800a9b */

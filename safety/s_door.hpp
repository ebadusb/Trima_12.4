/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.6  2004/12/03 17:35:46Z  rm70006
 * IT 7025.  Fix bad log statement for door status.
 * Revision 1.5  2002/04/23 21:09:33Z  pn02526
 * Change type of T1Status to clear up compile warnings.
 * Revision 1.4  2001/12/14 07:26:32  jl11312
 * - modified centrifuge spinning with door open/unlocked alarm to occur at 1 second instead of 2.5 during startup tests
 * Revision 1.3  2000/07/26 22:07:04  bs04481
 * Relocate code to control solenoid power from the power contol
 * timer to the door object because the door object normally
 * controls the solenoid power based on centrifuge speed.  Also,
 * delay 1 second before sending a response for solenoid power
 * disable.
 * Revision 1.2  2000/05/26 17:12:08  BS04481
 * More maintanence changes:  This set contains changes for
 * 1. Moving valve object from container to bag
 * 2. Removing SafetyTimer->SafetySoftTimer and letting the
 *      switch and disconnect tests have their own timers.
 * 3.  All message types are changed to NO_BOUNCE to cut down
 *      the on unnecessary stuff in the message que.
 * Revision 1.1  2000/03/15 18:28:49  BS04481
 * Initial revision
 *
 *
 * TITLE:      s_door.hpp, a door
 *
 * ABSTRACT:   Safety's view of the basin door.
 *
 */

 #ifndef S_DOOR
 #define S_DOOR

#include "hw.h"
#include "error.h"

// local includes
#include "s_type.h"
#include "s_dev.hpp"
#include "pwr_test.hpp"

/*******************************************************************************
*
*  aDoor_Safety CLASS DEFINITION
*
******************************************************************************/
class aDoor_Safety
{
private:
   T1STATUS             _cT1Status;
   char                 _cDeviceConstraint;
   HW_DOOR_STATES       _doorLocks;
   bool                 _bForceDisable;
   bool                 _bForceDisableDone;
   aCentrifuge_Safety*  _centrifuge;
   aSafety24vPowerTest* _Safety24vPowerTest;

   // 10/01/96 allow door fault tolerance of 4 consecutive samples
   // to prevent shutdown for door problems with door not locked and closed
   // AND centrifuge RPM > 60 AND centrifuge power enabled.
   int _DoorFaultTolerantCounter;

   aDoor_Safety(aDoor_Safety const&);                 // not implemented
   aDoor_Safety& operator = (aDoor_Safety const&);    // not implemented

public:
   aDoor_Safety(aCentrifuge_Safety* centrifuge
                , aSafety24vPowerTest* powerTest);
   ~aDoor_Safety() {}
   void initializeForState (long newState);

   void           Update (HW_DOOR_STATES locks);
   void           UpdateAndTestDoorFault ();
   bool           DoorT1OK (void) ;
   HW_DOOR_STATES GetDoor (void)
   {
      return(_doorLocks);
   }
   void SetForceDisable (bool disable)
   {
      _bForceDisable = disable;
      // clear the response also if appropriate
      if (_bForceDisable == FALSE)
         _bForceDisableDone = FALSE;
   }
   bool IsSolenoidDisabled (void)
   {
      return(_bForceDisableDone);
   }


}; // END aDoor_Safety class definition

#endif // S_DOOR

/* FORMAT HASH 9ca9779770fa87bd8f52f30f4ab9e8e2 */

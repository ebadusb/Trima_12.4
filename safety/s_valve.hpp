/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.10  2007/10/17 22:39:59Z  spriusb
 * Rolling back dRBC auto-split changes
 * Revision 1.9  2007/09/19 18:04:01Z  spriusb
 * Revision 1.8  2007/08/30 20:07:00Z  spriusb
 * Enhancements to support dRBC auto-split cassettes
 * Revision 1.7  2002/04/23 21:09:34Z  pn02526
 * Change type of T1Status to clear up compile warnings.
 * Revision 1.6  2000/07/20 13:33:31  bs04481
 * Remove unused declaration
 * Revision 1.5  2000/06/30 21:29:30  bs04481
 * Re-did valve monitoring to not monitor by draw and return cycles
 * but to alarm anytime there is a valve out of position for 20
 * consecutive seconds while the associated pump is running.
 * Revision 1.4  2000/06/22 16:46:50  bs04481
 * Plumbing changes for single-stage.  1) Change to RBC flow.
 * 2) Add valve position monitoring during Blood Run and Blood
 * Rinseback. 3)  Add compile flag to support dual-stage
 * Revision 1.3  2000/05/26 17:12:43  BS04481
 * More maintanence changes:  This set contains changes for
 * 1. Moving valve object from container to bag
 * 2. Removing SafetyTimer->SafetySoftTimer and letting the
 *      switch and disconnect tests have their own timers.
 * 3.  All message types are changed to NO_BOUNCE to cut down
 *      the on unnecessary stuff in the message que.
 * Revision 1.2  2000/03/15 18:27:56  BS04481
 * Move cassette object here from s_dev.hpp
 * Revision 1.1  1998/05/12 19:49:58  bs04481
 * Initial revision
 *
 *
 * TITLE:      valve.hpp, a valve
 *
 * ABSTRACT:   Safety's view of the valve.
 *
 */

#ifndef S_VALVE_HPP  //  Prevents multiple compilations.
#define S_VALVE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hw.h"
#include "error.h"

// local includes
#include "s_type.h"
#include "s_donor.hpp"

// safety cassette position test particulars
#define DOWN_POSITION_VERIFIED               ( 1 << 1 )                    // cassette down position verified
#define UP_POSITION_VERIFIED                 ( 1 << 2     )                // cassette up position verified
#define UKNOWN_POSITION_VERIFIED             ( 1 << 0     )

#define ALL_CASS_POSITIONS_VERIFIED              ( UKNOWN_POSITION_VERIFIED + UP_POSITION_VERIFIED + DOWN_POSITION_VERIFIED )

// safety valve positon test particulars
#define NO_POSITION_VERIFIED                 0               // no valve positions have ben verified ( init value )

#define UNKNOWN_POSITION_VERIFIED            ( 1 << HW_VALVE_UNKNOWN )
#define COLLECT_POSITION_VERIFIED            ( 1 << HW_VALVE_COLLECT )     // collect position seen correctly for given valve
#define OPEN_POSITION_VERIFIED               ( 1 << HW_VALVE_OPEN )        // open position seen correctly for given valve
#define RETURN_POSITION_VERIFIED             ( 1 << HW_VALVE_RETURN )      // return position seen correctly for given valve

// all verified except unknown = 14 ( may not see unknown , function of status message timing.
#define ALL_VALVE_POSITIONS_VERIFIED_XCEPT_UNKNOWN ( RETURN_POSITION_VERIFIED | OPEN_POSITION_VERIFIED | COLLECT_POSITION_VERIFIED )
#define ALL_VALVE_POSITIONS_VERIFIED (  UNKNOWN_POSITION_VERIFIED | ALL_VALVE_POSITIONS_VERIFIED_XCEPT_UNKNOWN )


// during blood prime and blood run, we will insist that the valve position be
// either in collect or return within a resonable time.  this ensures that we actually know
// where the fluids are going.
#define MAX_SAMPLES_BAD_VALVE     40    // about 20 seconds
/*******************************************************************************
*
*  aCassette_Safety CLASS DEFINITION
*
******************************************************************************/
class aCassette_Safety
{
private:
   T1STATUS           _cT1Status;
   int                _deviceConstraint;
   HW_CASSETTE_STATES _position;
   bool               _forceInitialize;
   aDonor*            _paDonor;

   aCassette_Safety();                                         // not implemented
   aCassette_Safety(aCassette_Safety const&);                  // not implemented
   aCassette_Safety& operator = (aCassette_Safety const&);     // not implemented

public:
   aCassette_Safety(aDonor* paDonor);
   ~aCassette_Safety() {}

   void               UpdateT1Status (char cCassettePosition);
   T1STATUS           GetT1Status (void);
   void               SetDeviceConstraint (int deviceConstraint);
   inline int         GetDeviceConstraint (void) const { return _deviceConstraint; }
   HW_CASSETTE_STATES GetCassettePosition (void);
   void               Update (HW_CASSETTE_STATES position);
   void               initializeForState (long newState);
   bool               CassetteT1OK (void);
   bool               isFirstSampleIn ();
   void               saveToPFR ();
   void               restoreFromPFR ();

}; // END aCassette_Safety class definition

/*******************************************************************************
*
*  valve CLASS DEFINITION
*
******************************************************************************/
class valve
{
protected:
   ALARM_VALUES _alarm;

   void UpdateT1Status (HW_VALVE_STATES positionS);
   void reportOutOfPosition ();

public:
   char            _pszValveName[MAX_CHAR];
   HW_VALVE_STATES _positionC;         // position from control driver
   HW_VALVE_STATES _positionS;         // position from safety driver
   int             _iT1Status;
   int             _iBadValvePositionCount;
   bool            _bValvePositionMonitorEnabled;

   valve();                            // constructor for an unknown valve
   valve(valve const&);
   // valve& operator= ( valve const& );
   virtual ~valve();
   void            MakeItReal (const char* pszValveName, ALARM_VALUES alarm);
   void            MakeItFake ();
   void            UpdateControlValve (HW_VALVE_STATES positionC);
   void            UpdateSafetyValve (HW_VALVE_STATES positionS);
   virtual void    CheckValvePosition ();
   HW_VALVE_STATES GetControlValve (void);
   HW_VALVE_STATES GetSafetyValve (void);
   bool            ValveT1OK (void);
   void            InitializeForState (long newState);

   ALARM_VALUES GetAlarm () const { return _alarm; }

};  // End valve class definition

#endif // S_VALVE_HPP

/* FORMAT HASH 56d87842c779ac87d5bebee45964dc56 */

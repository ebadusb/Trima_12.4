/*******************************************************************************
 *
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.73.1.2  2009/10/05 16:04:32Z  dslausb
 * Shawn's safety code
 * Revision 1.1  2009/10/02 19:12:24Z  dslausb
 * Initial revision
 * Revision 1.73  2006/03/18 00:07:27Z  jheiusb
 * revised for PFR in MSS
 * Revision 1.72  2003/09/09 22:45:26Z  ms10234
 * 6406 - fix pfr code
 * Revision 1.71  2002/12/30 22:58:09Z  pn02526
 * Runnable safety executive with CRC checking of the code disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.
 * Revision 1.70  2002/04/10 13:49:07  pn02526
 * "Change ASSERT() macro calls to trima_assert()."
 * Revision 1.69  2002/03/01 22:37:24Z  ms10234
 * 5613 - Increased the robustness of the AC pump monitor by enabling
 * the monitor whenever the donor is connected, regardless of state.
 * The exceptions are for rinseback and donor disconnect states where
 * rinseback monitoring is in effect.
 * Revision 1.68  2000/07/26 21:09:05  bs04481
 * Enable pause monitoring after .5ml of inlet is pumped in Blood
 * Prime
 * Revision 1.67  2000/07/20 19:32:50  bs04481
 * Change function name to be more representative.
 * Revision 1.66  2000/07/11 23:48:27  bs04481
 * Extend pause monitoring back to Blood Prime
 * Revision 1.65  2000/06/08 21:54:32  bs04481
 * Access valves via the containers
 * Revision 1.64  2000/05/26 17:12:31  BS04481
 * More maintanence changes:  This set contains changes for
 * 1. Moving valve object from container to bag
 * 2. Removing SafetyTimer->SafetySoftTimer and letting the
 *      switch and disconnect tests have their own timers.
 * 3.  All message types are changed to NO_BOUNCE to cut down
 *      the on unnecessary stuff in the message que.
 * Revision 1.63  2000/03/15 18:25:54  BS04481
 * Push work back into containters; use needle monitor for pump
 * pauses and stops; watch for lower reservoir error during FIRST,
 * SELF_TEST, STARTUP, DISPOSABLE if the cassette is up
 * at the beginning (formerly only during disposable);
 * add logging comparison of safety and control delta times on
 * status messages; 3.3 non-essential code review issues
 * Revision 1.62  1999/08/01 20:53:29  BS04481
 * Non-critical code review changes identified at the 3.2 release.
 * Revision 1.61  1999/06/06 17:37:14  BS04481
 * Make decision on recoverability after a PF that happens during
 * first phase of rinseback dependent on time only.  In an AKO, only
 * disable the pump power timer if 3 ml (measured by encoders) has
 * been returned.
 * Revision 1.60  1999/05/13 15:43:53  BS04481
 * Change calculation of return pump speed to use the timestamp from
 * the control side message while calculating delta time.  Add logging
 * of the speed as it is calculated using delta time from the safety
 * side to attempt to detect messaging problems.
 * Revision 1.59  1999/04/06 18:32:58  BS04481
 * Remove default of set type to white when unknown.  If set type is
 * not known in states from Donor Connect to Rinseback, inclusive,
 * and any pumps run, alarm.  Default to white if unknown in Donor
 * Disconnect
 * Revision 1.58  1999/03/31 23:20:39  BS04481
 * During RBC/Plasma procedures, calculate an adjusted donor HCT
 * based on the amount of product collected and use this adjusted
 * value to calculate the infusion rate.  See IT 3679.
 * Revision 1.57  1998/11/12 18:29:25  bs04481
 * Call the update to evaluate pump volumes during Rinseback and
 * Donor Disconnect outside of the gateway of having a valid
 * average AC ratio as there are cases when we do not have a
 * valid ratio but still need to monitor pumps (i.e. pump runaway
 * during donor disconnect).
 * Also add a function to restore the necessary information to resume
 * hypovolemia monitoring if we are entering donor disconnect from
 * a state where we do not have full donor information.  This may
 * happen if we powered up with the cassette down but do not
 * have a recoverable procedure.  If the PFR data was not corrupted
 * we can still use it for disconnect test.
 * Revision 1.56  1998/05/12 19:47:06  bs04481
 * Change reservoir monitoring, which includes ac ratio, ac infusion
 * and hypovolemia montiroing, to an object oriented design.
 * Previously, the volumes were accumulated in the s_res code.
 * With this change, the machine is modeled as six containers, each
 * consisting of some combination of bags, valves and pumps.  The
 * safety monitors now query the containers and calculate the
 * safety variables.  See the safety executive design document for
 * version 3.2 software.
 * Revision 1.55  1998/01/22 18:01:24  bs04481
 * Fix potential problem with incorrectly deciding the anti-coagulation
 * of the needle if we paused or power failed in Rinseback before
 * we had pumped 3 ml back to the donor.
 * Revision 1.54  1997/12/17 21:51:01  bs04481
 * Don't start Access Keep Open monitoring unless we are at least
 * 2 seconds into a draw cycle.  Done to prevent false errors which
 * occur around the cycle switch.
 * Also, move the accumulated rev offset to only be read on the first
 * sample from the driver.
 * Revision 1.53  1997/12/12 17:11:15  bs04481
 * Implement new pump power timer to be used to ensure that pumps
 * run within the required timer period after a pause or stop is pushed.
 * Revision 1.52  1997/12/11 16:46:11  bs04481
 * Fixes for Net Ac monitoring to ignore AC accumulated in the
 * first two samples following the start of a return cycle.
 * Also, change to keep the return line prime volume following a power
 * fail.
 * Revision 1.51  1997/12/10 18:25:32  bs04481
 * Change to monitor WBP only when collect valve is open and to
 * add an offset to account for startup volume.
 * Changes for net AC to donor monitoring.
 * Fix for Too many AKO problem which I introduced when I fixed the
 * ign change problem on the return pump in first draw.
 * Changes to use the reservoir sensor to see what cycle we should
 * be in after a power fail.
 * Revision 1.50  1997/12/05 21:12:23  bs04481
 * Changes to: 1) Allow multiple Access Keep Open operations in
 * the same draw cycle by monitoring infusion rate instead of limiting
 * return pump volume during a draw cycle. 2) Monitor net AC to the
 * donor over the complete procedure. 3) Block Blood Rinseback
 * if attempting to rinseback during first draw.
 * These were all AC infusion issues.
 * Revision 1.49  1997/12/02 15:59:38  bs04481
 * Changes to implement power fail recovery
 * Revision 1.48  1997/08/01 18:57:59  SM02805
 * Modifications to correct alarms related t aborted first draw.
 * Modification 1 : do not call cal ac to bag ( R may be zero ) if
 * the donor is not connected. This fixes the case where a run
 * was aborted during first draw and as the cassette raises the
 * low level sensor goes from fluid to air causes a call to
 * switch_to_draw_cycle which calls calc ac to bag.
 * Modification 2 : If abort from first draw and desire rb, added
 * code to calculate a ratio ( using first draw eqn ) if ratio is 0
 * and if the calc ratio is > 5.0, rb can occur.
 * Modification 3 : If the system is shutdown for a ratio alarm, the
 * value of the _favgACRatio is set to MIN_ALLOWABLE_AC_RATIO
 * to prevent divide by zero problems elsewhare in the code which
 * result in FATAL ERROR.  The system has been safed by the alarm,
 * this mod prevents a FATAL ERROR which causes screen to go
 * to black.
 *
 * Revision 1.47  1997/07/31 18:58:16  TM02109
 * Modified to use the Draw cycle volume of AC processed for the
 * ACPrimeVolume.
 * Revision 1.46  1997/07/31 00:02:30  TM02109
 * Corrected first ratio for the draw cycle and the correct setting of
 * last know good ratio.
 * Revision 1.45  1997/07/29 16:20:24  SM02805
 * Numerous modifications to switch_to_return_cycle and
 * switch_to_draw_cycle methods to implement ratio robustness
 * as detailed in PPQA memo " Ratio Robustness and Small
 * Cycles".  Changes required new methods in the reservoir class
 * ( calls are in switch_to_return_cycle and switch_to_draw_cycle )
 * , some new private member data to accumulate values for
 * ratio robustness, and some new defines for ratio robustness
 * thresholds.
 * Revision 1.44  1997/06/20 16:41:41  SM02805
 * 1. Modified implementation of ac ratio low alarm generation for cases
 *  where ac is delivered during draw AND ratio < 1.0 ( see PPQA
 *  Resolutions from 06/19/97 Safety Meeting for details ).
 *  2. MOdify implementation of RB speed limit for return pump to
 *  use a 250 ml/min single-shot alarm versus a 100 ml.min filterd
 *  alarm criteria (  see PPQA Resolutions from 06/19/97
 *  Safety Meeting for details ).
 * Revision 1.43  1997/06/04 12:56:31  SM02805
 * Modification to ratio robustness small cycle allowance.  Changed vbl type from
 * char to int for smaples in a draw cycle.  Think problem is char wrap
 * on rbc draw cycles where draw time is on order of 120+s and char
 * vbl not large enough to count without overflow problem.
 * Revision 1.42  1997/05/14 20:08:04  SM02805
 * Modifcations for ratio robustness pursuant to PPQA memo "Improved Ratio Robustness
 * Based on 042997 Safety Meeting ..."
 * Revision 1.41  1997/03/03 15:22:49  bs04481
 * Revision 1.41  1997/02/28 23:07:04  bs04481
 * Merge in ratio monitoring code which was included in 2.3 but did
 * not get transferred to 2.5.
 * Revision 1.40  1997/02/11 18:00:45  bs04481
 * Changes due to enumerated types in the hardware structures
 * Revision 1.39.1.3  1997/02/17 21:05:09  TM02109
 * Made changes to allow for new LOW_RATIO filter.
 * Revision 1.39.1.2  1997/02/15 22:37:01  TM02109
 * Modification to rinseback monitoring.
 * Revision 1.39.1.1  1997/02/15 01:12:15  TM02109
 * Modified implementation of ac infusion monitoring for return pump
 * runaway and ac pump runaway during draw cycles.
 * Revision 1.39  1997/01/31 16:11:43  TM02109
 * Changed the MAX_ALLOWABLE_RATIO to 15.0, this is because
 * the control side is controled to 13.7 and 14.0 average ratio is
 * close enough to this to cause a problem.  Larry Dumont thinks
 * that 15.0 is acceptable.
 * Revision 1.38  1997/01/27 23:52:37  TM02109
 * Modification to implement new monitoring for AC delivery to set :
 * - Remove AC Off During Draw Alarm ( encompass with AC Ratio TOO High )
 * - AC Ratio Too High filter from 10 to 30 per McAteer, P F-H, Bainbridge analysis
 * - count occurances of AC delivered < 0.005 ml against ratio too high filter
 * ( gated with AC pump commanded on ) catches case where ac commanded
 * ON , but not delivering ac to set.
 * - add logging for ratio limits exceeded prior to alarm level
 * - range test average ac ratio calculated at top of draw prior
 * to return.
 * - generate AC ratio too high if ac delivered to set for entire draw is
 * 0.0 ml ( catches case where ac pump not commanded on for entire draw )..
 * - zero ratio too high and low filters at start of each draw.
 * Revision 1.37  1997/01/17 14:55:45  TM02109
 * Added a cycle transition variable to the reservoir class to allow
 * sync for logging of return pump rev error data when cycle switches
 * from reutrn to draw.  s_exec updates rev error monitoring after
 * the cycle is switched. Not syncing caused the rev error logged
 * to disagree with the revs. Encoders had been updated, halls
 * had not when retport is done in reservoir code that switcheds cycle.
 * Revision 1.36  1997/01/07 16:12:41  TM02109
 * Mdofied filter for ratio to 10 samples for ratio too low and ratio too
 * high ( 5 second filter ) .
 * Revision 1.35  1997/01/03 16:58:30  TM02109
 * Change _fdeltaACVolumeSet test for ratio calculation from
 * == 0.0f to > 0.0005f
 * Revision 1.34  1996/12/19 15:50:41  TM02109
 * Increase rb speed limit filter to 5 ( from 4 ) to account for invalid
 * time for first calculation ( first calc has a shorter sample time due to
 * async state change to rb from run ).
 * Revision 1.33  1996/12/18 18:57:52  SM02805
 * Increased low ratio instantaneous filter from 5 to 10 samples for
 * AC_RATIO_TOO_LOW alarm.
 * Revision 1.32  1996/12/09 18:42:18  TM02109
 * Added a filter for rinseback speed limit - 4 occurances of measured
 * speed in xs of 100 ml/min allowed per RB before alram gen.  If the
 * machine is transitioned to RB during a return cycle, it takes three
 * samples to get return pump speed indicating < 100 ml/min.
 * Revision 1.31  1996/11/28 00:07:37  SM02805
 * 1. Modified implmentation of reservoir monitoring to accrue all fill
 * volumes against hypovolemia until a maximum allowable fill volume
 * of 142 ml ( old limit 72 ml ) .
 * 2. Remove "return too big " monitoring and alarm ( not necessary any
 * longer, procedure still has a monitor for this condition, and hypo
 * accrual is maintained throughout until a return cycle is completed, volume
 * of drain not monitored.
 * 3.Remove reservoir alarm blinding following centrifuge shutdown/restart.
 * 4. Implement new infusion monitoring consisting of :
 * single cycle max 1.6 ml/min - l TBV
 * two cycle average 1.45 ( current cycle and previous )
 * three cycle average 1.3 ( current, t-1, t-2 cycle average ).
 * 5. Remove all infusion blinding folloiwng centrifuge shutdown/restart.
 * 6. Implement return pump speed limit monitoring for rinseback :
 * 1`00 ml/min maximum allowable return pump speed in rb.
 * 3.
 * Revision 1.30  1996/09/22 18:12:44  SM02805
 * 1. Respond to 09/19/96 code review action items.
 * 2. Fix bug in brace location for AC off during draw
 * filter countdown.
 * 3. U/S robustness for cycle timers (UpdateCycelTime method)
 *  - do not redundantly switch cycle.
 * Revision 1.29  1996/09/18 17:07:30  SM02805
 * Update document in code body for return to return infusion
 * monitoring.
 * Revision 1.28  1996/09/17 22:44:50  SM02805
 * 09/17/96 marlene resolution to infusion calculation use return
 * to return cycle for infusion calc.
 * Revision 1.27  1996/09/16 20:33:26  SM02805
 * marlene bainbridge resolution 09/16/96
 * Implement filter for infusion rate monitoring during rb for
 * 10 samples of consecutive infusion > 1.32 ml/min - l TBV
 * limiting maximal ac infused to 6 ml at R = 5  in face of return pump
 * runaway.
 * Revision 1.26  1996/09/16 17:17:22  SM02805
 * 09/16/96 xfunc team resolution :
 * 1. Implement first draw cycle infusion monitoring for rb using
 * accrued inlet draw and ac draw for avg ratio calculation.
 * 2.Remove high frequency monitoring for RB infusion.
 * Revision 1.25  1996/09/16 17:32:30  SM02805
 * 09/16/96 xfunc resolution :
 * Implement ratio calculation based on currently accrued inlet and
 * AC during first draw for RB during first draw monitoring.
 * Revision 1.24  1996/09/16 17:32:30  SM02805
 * Significantly reduce print instrumentation in source files.
 * Implement _LOG_ERROR processing for cases where
 * data is important for post-run safety analysis.
 * Revision 1.23  1996/09/16 17:32:30  SM02805
 * 09/12/96 xfuc resolutions :
 * 1.Implement infusion monitoring for RINSEBACK (RB).
 * 2.Implement valves must be in return monitoring for RB.
 * 3. Monitor for inappropriate movement of the AC pump
 * during RB and donor disconnect test.
 *
 * Revision 1.22  1996/09/16 17:32:30  SM02805
 * Mods resulting from 09/06/96 review and xfunc team resolutions :
 * 1. Implement U/S robustness do not redundantly switch cycle to
 * draw || return if already in draw || return cycle.
 * 2. Move run cycle fill limit from 67.0 ml to 72.0 ml.
 * 3. Implement hypovolemia monitoring as per xfunc
 * resolution resulting in three types of hypovolemia
 * calculations : first cycle, subsequent run cycles,
 * and rinseback/donor disconnect.
 * Revision 1.20  1996/08/28 16:34:33  tm02109
 * Modified the 175 lower volume limit to 210 lower first cycle limit.
 * Revision 1.19  1996/08/22 18:14:19  tm02109
 * Changed the max allowed first draw cycle volume to 300 mls.
 * Revision 1.18  1996/08/21 22:39:13  SM02805
 * Increase filter from two to three for the number of "half cycles"
 * safety does not monitor for reservoir or infusion following startup
 * from centrifuge shutdown.  Tests have indicated that for two
 * half cycles, the reservoir volumes can generate a reservoir error
 * ( draw too big has been seen ) on the first monitored half cycle.
 * The specific case has been cent shutdown during draw, expressed
 * volume does not get to high level sensor.  Startup draw is very short
 * so next draw can still be too large due to channel filling.
 * Revision 1.17  1996/08/20 21:15:43  tm02109
 * Changed first draw cycle volume limit to 250 mls.
 * Changed return drain volume limit to 72 mls to match procedure.
 * Revision 1.16  1996/08/15 01:04:02  SM02805
 * Modified first draw cycle too big limit from 200.0 to 235.0 :
 * procedure does not count return pump volume pumped
 * away from donor against reservoir, safety does therefore
 * the limit needed to be increased to prevent false alarms.
 *
 * Defined constant for first draw cycle too small = 210.0 ml.
 * Revision 1.15  1996/08/14 14:02:54  SM02805
 * Add new cycle monitoring state : SN_MON_FIRST_DRAW_COMPLETE
 * for first cycle infusion calc lockout.
 * Revision 1.14  1996/08/13 18:56:11  SM02805
 * Mods for reservoir and infusion monitoring lockout following centrifuge
 * shutdown during run. Infusion and reservoir monitoring lockout for
 * two "half cycles" for all cases of draw/return when centrifuge
 * shutdown occurs ( not rqd for shutdown during draw where
 * return not induced, however due to long cycle time when
 * restart, infusion will always be low ).
 *
 * FIx hypovolemia accrual bug : was counting portion of inlet
 * twice by including aggregate collect in calculation.
 *
 * First cycle monitoring fix firsat cycle ALWAYS ends upon
 * first return to donor irrespective of draw volume at trans.
 *
 * Implement alarm for first draw cycles < 210 ml
 * ( including AC prime volume) with shutdown.
 * Revision 1.13  1996/07/31 12:58:14  SM02805
 * Revision 1.12  1996/07/31 12:58:14  SM02805
 * Modifications for infusion during RBC : limit hct for rbc to
 * 0.99 - 0.01 .
 * Revision 1.11  1996/07/30 23:52:04  SM02805
 * Changes in reservoir object : separate values for platelet and plasma
 * collect volume accumulators dor DRAW and RETURN.
 * Revision 1.10  1996/07/30 21:18:12  SM02805
 * Working on infusion calculations and fill volumes for RBC collection
 * - the current version has not been tested, checked in for build 1.16.
 * Revision 1.9  1996/07/30 02:19:06  SM02805
 * Lint fixes per 07/23/96, 07/26/96 reviews.
 * Revision 1.8  1996/07/24 17:14:26  SM02805
 * Init _cCycleMon to SN_MON_INACTIVE in reservoir constructor.
 * Revision 1.7  1996/07/24 13:57:09  SM02805
 * Mods to remove absolute paths for include files, includes source
 * code modifications and makefile ( safety.mak) modifications.
 *
 * safety.mak mods use CPPFLAGS , now getting warnings level 3.
 *
 * Sorce code mods to implement donor disconnect test, only partially tested
 * - works as expected when test is passed first time, ie access line
 * properly clamped on first test try...
 *
 * TITLE:      s_res.hpp
 *             Safety reservoir class definitions.
 *
 *
 * ABSTRACT:   This file contains the class definitions for the
 *             safety reservoir.
 *
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 ******************************************************************************/

#ifndef S_RES_HPP  //  Prevents multiple compilations.
#define S_RES_HPP

#include <time.h>

#include "trima_assert.h"
// datalog_delete#include "error.h"       // 07/26/96 _FATAL_ERROR, _LOG_ERROR
#include "chw.hpp"
#include "shw.hpp"

#include "s_glob.hpp"
#include "s_donor.hpp"
#include "s_cont.hpp"
#include "needle.hpp"

#include "acratio.hpp"
#include "acinfus.hpp"
#include "hypovol.hpp"

/*++++++++++++++++++++++++++++++++++++++++++++++
   Reservoir Monitoring Defines : Start
+++++++++++++++++++++++++++++++++++++++++++++*/

// 12/06/96 defines for return cycle switch holdoff where reservoir fills secondary to
// channel fluid expression.
enum RETURN_CYCLE_SWITCH_HOLD
{
   NO_HOLD_SWITCH_TO_RETURN_CYCLE,                // nominal : when reservoir indicates full, switch to return
   FORCE_SWITCH_TO_RETURN_CYCLE_ON_RESTART,       // FS has re-started, reservoir filled due to channel expression, switch cycle
};

// 12/09/96 RB Excessive Speed Threshold
// RETURN_FLOW_RATE_LIMIT_RB = 100 ml/min (s_defs.hpp)
// If RB return pump spped exceeds 100 ml/min RB_EXCESSIVE_SPEED_THRESHOLD * per
// RB, shutdown alarm in s_res.cpp
//
// 12/19/96 mod from 4 - 5 rationale : first entry into rb infusion calc has
// an inaccurate delta time for calc of "instantaneous" infusion
//
//
// #define RB_EXCESSIVE_SPEED_THRESHOLD      5    // allow five speed hits per RB b4 alarm

// 01/17/97 use flag to sync return rev error reporting at start of draw.
// possible values for _cCycleTransition
//
#define INIT_CYCLE_UNK     0
#define SWITCHED_TO_DRAW   1
#define SWITCHED_TO_RETURN 2
#define SWITCH_CYCLE_ACK   4



/*++++++++++++++++++++++++++++++++++++++++++++++
   Reservoir Monitoring Defines : End
+++++++++++++++++++++++++++++++++++++++++++++*/



/*******************************************************************************
*
*  aReservoir_Safety CLASS DEFINITION
*
******************************************************************************/
#define LOW_LEVEL_LOG_SAMPLES 6
class aReservoir_Safety
{
private:

   bool                 _bLookingForFirstFluid;
   bool                 _bWetSetNotAllowed;
   bool                 _bPauseMonitoringActive;
   MON_TYPE             _cCycleMon;
   char                 _cSNCycle;
   SHW_RESERVOIR_STATES _lowLevel;
   char                 _lowLevelLog[LOW_LEVEL_LOG_SAMPLES];
   char                 _lowLevelLastLogged;
   int                  _lowLevelLogIndex;
   CHW_RESERVOIR_STATES _resLevel;
   float                _pumpTimerInletVolumeSnap;
   float                _startOfBloodPrimeInletVolumeSnap;
   bool                 _bHallOffsetComplete;
   bool                 _inMeteredStorage;

   // some calculations need to depend on the time from the control side
   timespec           _tLastStatusTime;
   long               _ldeltaControlStatusTime;
   timespec           _tSafetyEquivTime;

   aDonor*            _paDonor;
   aCassette_Safety*  _paCassette;

   InletContainer*    _Inlet;
   ReturnContainer*   _Return;
   ACContainer*       _AC;
   PlasmaContainer*   _Plasma;
   PlateletContainer* _Platelet;
   RBCContainer*      _RBC;

   // physiological monitors
   ACRatioAverage*       _ACRatioAverageMonitor;
   ACRatioPumpVolume*    _ACRatioPumpVolumeMonitor;
   ACRatioInstantaneous* _ACRatioInstantaneousMonitor;
   ACInfusion*           _ACInfusionMonitor;
   ACInfusionRinseback*  _ACInfusionRinsebackMonitor;
   ACInfusionAKO*        _ACInfusionAKOMonitor;
   HypovolemiaCycleSize* _HypovolemiaCycleSizeMonitor;
   HypovolemiaDonor*     _HypovolemiaDonorMonitor;
   ComponentDepletion*   _ComponentDepletionMonitor;

   // 11Dec97 a timer to make sure that if pumps do not move within
   // 10 minutes, they never move.
   needleMonitor* _needle;

   aReservoir_Safety();                                      // not implemented
   aReservoir_Safety(aReservoir_Safety const&);              // not implemented
   aReservoir_Safety& operator = (aReservoir_Safety const&); // not implemented

   void updateToPFR ();
   bool arePumpsStopped ();
   void checkForWetSet ();
   void ResetReturnCycleData (void);
   void LogReturnCycle (void);
   void ResetDrawCycleData (void);
   void LogDrawCycle (void);

public:
   aReservoir_Safety(InletContainer* pInletContainer
                     , ReturnContainer* pReturnContainer
                     , ACContainer* pACContainer
                     , PlasmaContainer* pPlasmaContainer
                     , PlateletContainer* pPlateletContainer
                     , RBCContainer* pRBCContainer
                     , needleMonitor* pNeedle
                     , aDonor* paDonor
                     , aCassette_Safety* paCassette)
   {
      trima_assert(pInletContainer);
      trima_assert(pReturnContainer);
      trima_assert(pACContainer);
      trima_assert(pPlasmaContainer);
      trima_assert(pPlateletContainer);
      trima_assert(pRBCContainer);
      trima_assert(pNeedle);
      trima_assert(paDonor);
      trima_assert(paCassette);
      _Inlet    = pInletContainer;
      _Return   = pReturnContainer;
      _AC       = pACContainer;
      _Plasma   = pPlasmaContainer;
      _Platelet = pPlateletContainer;
      _RBC      = pRBCContainer;

      // timer to monitor coagulation in the needle
      _needle     = pNeedle;
      _paCassette = paCassette;
      _paDonor    = paDonor;

      _bLookingForFirstFluid            = FALSE;
      _bPauseMonitoringActive           = FALSE;
      _cCycleMon                        = MON_INACTIVE;
      _bHallOffsetComplete              = FALSE;
      _cSNCycle                         = DRAW_CYCLE;
      _lowLevel                         = (SHW_RESERVOIR_STATES)0; // or maybe SHW_RESERVOIR_EMPTY?
      _lowLevelLastLogged               = 'U';                     // force logging first time through
      _lowLevelLogIndex                 = 0;
      _resLevel                         = CHW_RESERVOIR_ERROR;
      _pumpTimerInletVolumeSnap         = 0.0f;
      _startOfBloodPrimeInletVolumeSnap = 0.0f;
      _bWetSetNotAllowed                = TRUE;

      // ac ratio monitoring objects
      _ACRatioAverageMonitor       = new ACRatioAverage(_Inlet, _Return, _AC);
      _ACRatioPumpVolumeMonitor    = new ACRatioPumpVolume(_Inlet, _Return, _AC, _paDonor);
      _ACRatioInstantaneousMonitor = new ACRatioInstantaneous(_Inlet, _Return, _AC);

      // hypovolemia monitoring objects
      _HypovolemiaCycleSizeMonitor = new HypovolemiaCycleSize(_Inlet, _Return, _Platelet, _Plasma, _RBC);
      _HypovolemiaDonorMonitor     = new HypovolemiaDonor(_Inlet, _Return, _AC, _Platelet, _Plasma, _RBC, _paDonor);
      _ComponentDepletionMonitor   = new ComponentDepletion(_Inlet, _AC, _Platelet, _RBC, _paDonor);

      // ac infusion rate monitoring object
      // AC infusion for RBC/Plasma needs the extracorporeal volume from the hypovolemia monitor
      _ACInfusionMonitor          = new ACInfusion(_Inlet, _Return, _AC, _Platelet, _Plasma, _RBC, _paDonor,
                                                   _HypovolemiaDonorMonitor);
      _ACInfusionRinsebackMonitor = new ACInfusionRinseback(_Inlet, _Return, _AC, _paDonor);
      _ACInfusionAKOMonitor       = new ACInfusionAKO(_Inlet, _Return, _paDonor);

      // initialize timers for delta message times
      clock_gettime(CLOCK_REALTIME, &_tLastStatusTime);
      clock_gettime(CLOCK_REALTIME, &_tSafetyEquivTime);
      _ldeltaControlStatusTime = 1000000;      // start with a really long delta time

      updateToPFR();

   }

   /// lint fix 07/27/96
   ~aReservoir_Safety()
   {
      _Inlet    = NULL;
      _Return   = NULL;
      _AC       = NULL;
      _Plasma   = NULL;
      _Platelet = NULL;
      _RBC      = NULL;

      delete _ACRatioAverageMonitor;
      _ACRatioAverageMonitor       = NULL;
      delete _ACRatioPumpVolumeMonitor;
      _ACRatioPumpVolumeMonitor    = NULL;
      delete _ACRatioInstantaneousMonitor;
      _ACRatioInstantaneousMonitor = NULL;
      delete _ACInfusionMonitor;
      _ACInfusionMonitor           = NULL;
      delete _ACInfusionRinsebackMonitor;
      _ACInfusionRinsebackMonitor  = NULL;
      delete _ACInfusionAKOMonitor;
      _ACInfusionAKOMonitor        = NULL;
      delete _HypovolemiaCycleSizeMonitor;
      _HypovolemiaCycleSizeMonitor = NULL;
      delete _HypovolemiaDonorMonitor;
      _HypovolemiaDonorMonitor     = NULL;
      delete _ComponentDepletionMonitor;
      _ComponentDepletionMonitor   = NULL;

      _paCassette                  = NULL;
      _paDonor                     = NULL;
      _needle = NULL;

   }

   void restoreFromPFR ();
   void restoreReservoirMonitoringForDisconnect ();
   void initializeForState (long newState);
   bool FirstDrawComplete ();
   void SetReturnPrimeVolume ();
   bool shouldWeWorry ();
   void SetDeltaControlStatusTime (timespec newTime);

   void update_res_mon (void);
   void switch_to_draw_cycle (void);
   void switch_to_return_cycle (void);
   void UpdateControl (const CHwStates& controlStatus);
   void switchCycleForPFR ();
   void SetSetType (SET_TYPE disposableID);
   bool CheckSetType (void);
   bool CheckValveT1 ();
   void firstFluid ();
   void LogProcedureTotals (void);
   void UpdateSafety (SHwStates& safetyStatus);
   void SetCycleMon (MON_TYPE cCycleMon);
   void UpdateReservoirSafety (SHW_RESERVOIR_STATES reservoir);

   void UpdateInfusionTimers (CHW_RESERVOIR_STATES newReservoir)
   {
      _ACInfusionMonitor->UpdateTimers(newReservoir);
   }

}; // end aReservoir_Safety class definition

#endif /* S_RES_HPP */

/* FORMAT HASH 707968fc581f6278cb0459df7f4df64b */

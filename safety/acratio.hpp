/*
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.13  2003/04/03 21:49:57Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.12  2003/04/01 16:28:56  pn02526
 * Changes per IT 5950
 * Revision 1.11  2002/03/05 01:02:15Z  ms10234
 * 5613 -  Initialized containers to enable monitoring when the set type is unknown.  Enabled monitoring for AC pump always when the donor is connected.
 * Revision 1.10  2002/03/01 22:37:18  ms10234
 * 5613 - Increased the robustness of the AC pump monitor by enabling
 * the monitor whenever the donor is connected, regardless of state.
 * The exceptions are for rinseback and donor disconnect states where
 * rinseback monitoring is in effect.
 * Revision 1.9  2001/04/23 19:58:01  ms10234
 * 5080 -  Moved the running average ratio calculation to a point after the
 * volume accumulation/ratio alarm checks have been done.
 * Revision 1.8  2001/04/11 15:05:11  ms10234
 * IT5058 - Changed max infusion rate to 15.0, from 100.0
 * Revision 1.7  2000/10/27 14:19:09  bs04481
 * Correct error in accumulator for inlet volume when AC ratio exceeds
 * 15.  Because we now call this function every time we pause, we
 * were accumulating the volume more than once per cycle.  Also,
 * change the name of the variable that controls allowing AC ratios to
 * fall below 5 to make the code more readable.
 * Revision 1.6  2000/05/26 17:11:38  BS04481
 * More maintanence changes:  This set contains changes for
 * 1. Moving valve object from container to bag
 * 2. Removing SafetyTimer->SafetySoftTimer and letting the
 *      switch and disconnect tests have their own timers.
 * 3.  All message types are changed to NO_BOUNCE to cut down
 *      the on unnecessary stuff in the message que.
 * Revision 1.5  2000/03/15 18:00:42  BS04481
 * Remove unnecessary reference to control orders
 * Revision 1.4  1999/08/01 20:52:54  BS04481
 * Non-critical code review changes identified at the 3.2 release.
 * Revision 1.3  1999/06/25 17:09:48  BS04481
 * Corrections for AC ratio and AC ratio use during Blood Prime and
 * First cycle of Blood Run.  1) Correct formula used to calculate
 * AC ratio during first cycle by removing 5 ml bolus from the
 * numerator.  2) For purpose of determining if there is anti-coagulated
 * blood in the needle, allow ratio to go below 5.  3) For AKO during
 * Blood Prime, enforce 20ml limit on return volume because we don't
 * really know the AC ratio during Blood Prime so just enforce the
 * bolus.
 * Revision 1.2  1998/07/30 01:32:19  bs04481
 * Save FPR data after changing monitor type.
 * Revision 1.1  1998/05/12 19:49:30  bs04481
 * Initial revision
 *
 *
 * TITLE:      acratio.hpp, the AC Ratio monitoring objects
 *
 * ABSTRACT:   Safety's monitoring for AC ratio.   This includes:
 *                Monitoring for average AC ratio limits
 *                Monitoring for AC ratio related to pump volumes
 *                Monitoring for instantaneous AC ratio
 *
 *
 */

 #include "s_cont.hpp"
 #include "alarms.h"
 #include "s_donor.hpp"

#ifndef ACRATIO_HPP  //  Prevents multiple compilations.
#define ACRATIO_HPP

#define MAX_ALLOWABLE_AC_RATIO                          15.0f
#define MIN_ALLOWABLE_AC_RATIO                           5.0f

// 07/30/97 WTM
// Don't use at least 5.0 mls of fluid that you consider AC prime volume since
// it is not really fluid until it hits the manifold and control doesn't count
// the fluid until it hits the sensor.
// 24June99 BSS
// leave this quantity at 5 but change the explanation.  This 5ml represents
// the volume of AC in the inlet and return lines after AC prime
#define AC_PRIME_OFFSET                   5.0f

// 07/28/97 msm
// maximum allowable high ratio calculated is limited to MAX_USEABLE_HIGH_RATIO
// The average ratio value is limited to this value for any average R calculated
// > MAX_USEABLE_HIGH_RATIO, including infinite ( ac delivered during draw == 0.0 )
//
#define MAX_USEABLE_HIGH_RATIO             15.0f

// 07/28/97 msm
// cycles with inlet volumes >= INLET_SMALL_CYCLE_VOLUME_LIMIT are "valid"
// cycles with inlet volumes < INLET_SMALL_CYCLE_VOLUME_LIMIT are "invalid"
// average ratio calculation is different for valid and invalid cycles
// for small cycle robustness
//
#define INLET_SMALL_CYCLE_VOLUME_LIMIT      12.0f   // mls, inlet small cycle definition limit

// 07/28/97 msm
// a total of HIGH_RATIO_ACCUM_INLET_TOLERANCE mls are allowed per procedure
// where the average R > 15.0.
// This accumulator represent the inlet volume pumped during draw cycles where the
// average R for that cycle > 15.0.
//
#define HIGH_RATIO_ACCUM_INLET_TOLERANCE   300.0f   // mls, accum inlet for a procedure at average R > 15.

// 07/28/97 msm
// total accum ac pumped during invalid cycles with R <= 15.0.
// this accumulator is based on the "additional ac " delivered to the set
// during the invalid cycle i.e. that amount of ac that differs from what would be expected :
// inlet / last known good average R
//
#define LOW_RATIO_ACCUM_AC_TOLERANCE         2.0f   // mls, accum invalid cycles ac for a procedure at average R <= 15.0,

// 01/29/97
// redifine high ratio filter from 10 to 30 to address "AC off during draw" concerns
// 01/07/97 mod to address ratio alarm for ac bag rpl during run
//
#define HIGH_RATIO_FILTER                               30

// allowable  samples of low/high instantaneous ratio before alarm.
#define LOW_RATIO_FILTER                                10


/*******************************************************************************
 *
 *  ACRatioAverage CLASS DEFINITION
 *    Calculates and monitors average AC ratio
 *
 ******************************************************************************/

class ACRatioAverage
{
private:
   float            _fAccumInletVolumeAtHighRatio;
   float            _fAccumACVolumeAtLowRatio;
   bool             _bUseableACRatio;
   bool             _bACRatioMonitoringActive;
   float            _fAvgACRatio;
   float            _fLastKnownGoodRatio;
   MON_TYPE         _monType;
   bool             _dumpFlag;
   InletContainer*  _Inlet;
   ReturnContainer* _Return;
   ACContainer*     _AC;
   bool TestACHighRatioVolume (float favgRatio, bool accumulateVolumeAtHighRatio);
   bool TestACLowRatioVolume (float favgRatio, bool testLowRatio);
public:
   ACRatioAverage(InletContainer* InletCont, ReturnContainer* ReturnCont,
                  ACContainer* ACCont);
   ~ACRatioAverage();
   void  UpdateAverageRatio (bool testLowRatio, bool accumulateVolumeAtHighRatio);
   float GetAverageACRatio (bool& valid);
   void  InitializeForState (long newState);
   void  SetMonitoringType (MON_TYPE type){_monType = type; SaveToPFR(); }
   void  ResetCycleVol (char cCycle);
   float ComputeAvgACRatio (const float r);
   void  Dump (DataLog_Level& log_level);
   void  SaveToPFR ();
   void  RestoreFromPFR ();

};

/*******************************************************************************
 *
 *  ACRatioPumpVolume CLASS DEFINITION
 *    Monitors for AC ratio alarm conditions which are associated with
 *    unusual pump volumes.  These conditions are:
 *       1. AC pump running faster than (inlet pump - return pump)
 *       2. AC pump not running during draw cycle
 *       3. AC pump running during return cycle
 *
 ******************************************************************************/

class ACRatioPumpVolume
{
private:
   float            _fAccumACTooFastVolume;           // used for IsACTooFast
   int              _iHighRatioPersistsCounter;       // used for IsACOffDuringDraw
   bool             _bACRatioMonitoringActive;
   bool             _dumpFlag;
   InletContainer*  _Inlet;
   ReturnContainer* _Return;
   ACContainer*     _AC;
   aDonor*          _paDonor;
   void IsACTooFast ();
   void IsACOffDuringDraw (char cCycle, float commandRPM);
public:
   ACRatioPumpVolume(InletContainer* InletCont, ReturnContainer* ReturnCont,
                     ACContainer* ACCont, aDonor* paDonor);
   ~ACRatioPumpVolume();
   void Update (char cCycle, float commandRPM);
   void InitializeForState (long newState);
   void ResetCycleVol (char cCycle);
   void SetMonitoringType (MON_TYPE){}     // Monitoring type not used
   void Dump (DataLog_Level log_level);
   void SaveToPFR ();
   void RestoreFromPFR ();
};


/*******************************************************************************
 *
 *  ACRatioInstantaneous CLASS DEFINITION
 *    Calculates the AC ratio for each sample based on Inlet, Return and AC
 *    volumes.  Monitors for AC ratio outside of acceptable limits for
 *    an acceptable time period.
 *
 ******************************************************************************/

class ACRatioInstantaneous
{
private:
   int              _iHighRatioCounter;
   int              _iLowRatioCounter;
   MON_TYPE         _monType;
   bool             _bACRatioMonitoringActive;
   bool             _dumpFlag;
   InletContainer*  _Inlet;
   ReturnContainer* _Return;
   ACContainer*     _AC;
public:
   ACRatioInstantaneous(InletContainer* InletCont, ReturnContainer* ReturnCont,
                        ACContainer* ACCont);
   ~ACRatioInstantaneous();
   void InitializeForState (long newState);
   void Update (char cCycle);
   void ResetCycleVol (char cCycle);
   void SetMonitoringType (MON_TYPE type){_monType = type; SaveToPFR(); }
   void Dump (DataLog_Level log_level);
   void SaveToPFR ();
   void RestoreFromPFR ();
};



#endif // end of ACRATIO_HPP

/* FORMAT HASH aea0830e78bcf173b737b79d00e0881a */

/*
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.14  2003/12/04 20:17:42Z  ms10234
 * 6026 6104 - Add PFR values for ac infusion monitoring during plasma rinseback.
 * Revision 1.13  2003/09/09 22:44:31Z  ms10234
 * 6406 - fix pfr code
 * Revision 1.12  2003/04/04 16:26:51Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.11  2002/04/15 16:43:13Z  pn02526
 *
 * --- Added comments ---  pn02526 [2002/07/30 15:03:45Z]
 * 5621 - Compensated for the recirculation volume from the manifold for calculating rbc line hematocrit
 * Revision 1.11  2002/03/18 09:44:40  ms10234
 * 5621 - Compensated for the recirculation volume from the manifold for calculating rbc line hematocrit
 * Revision 1.10  2000/03/15 17:57:59  BS04481
 * Get cassette position from cassette object
 * Revision 1.9  1999/08/20 00:22:33  BS04481
 * Need to subtract off the return line prime while testing for the AC
 * bolus limit in first draw.  While I was in there, changed the label
 * of the accumulator to make operation clearer
 * Revision 1.8  1999/08/01 20:52:50  BS04481
 * Non-critical code review changes identified at the 3.2 release.
 * Revision 1.7  1999/06/25 17:09:41  BS04481
 * Corrections for AC ratio and AC ratio use during Blood Prime and
 * First cycle of Blood Run.  1) Correct formula used to calculate
 * AC ratio during first cycle by removing 5 ml bolus from the
 * numerator.  2) For purpose of determining if there is anti-coagulated
 * blood in the needle, allow ratio to go below 5.  3) For AKO during
 * Blood Prime, enforce 20ml limit on return volume because we don't
 * really know the AC ratio during Blood Prime so just enforce the
 * bolus.
 * Revision 1.6  1999/06/04 22:58:01  BS04481
 * Change calculation for delta time using control status header
 * information to only happen if there is valid pump data in the
 * control status message.
 * Don't evaluate return pump speed or AC infusion during rinseback
 * if the delta time between message is less than 20ms (avoid
 * reliability problem if messages stack up)
 * Finally, use control status message delta time when evaluating
 * AC infusion during Rinseback.
 * Revision 1.5  1999/05/13 15:43:32  BS04481
 * Change calculation of return pump speed to use the timestamp from
 * the control side message while calculating delta time.  Add logging
 * of the speed as it is calculated using delta time from the safety
 * side to attempt to detect messaging problems.
 * Revision 1.4  1999/04/01 19:47:09  BS04481
 * Change adjusted donor HCB formula to accumulate the collected
 * RBCs directly instead of accumulating the Input volume during
 * RBC collect and averaging RBCs collected.
 * Revision 1.3  1999/03/31 23:20:26  BS04481
 * During RBC/Plasma procedures, calculate an adjusted donor HCT
 * based on the amount of product collected and use this adjusted
 * value to calculate the infusion rate.  See IT 3679.
 * Revision 1.2  1998/11/12 18:22:11  bs04481
 * New function to check for pump volume violations during Donor
 * Disconnect.  Formerly part of the update function but needed
 * to be split out to allow pump volume to be checked even if we
 * did not have a valid average AC ratio.
 * Revision 1.1  1998/05/12 19:49:24  bs04481
 * Initial revision
 *
 *
 * TITLE:      acinfus.hpp, the AC infusion rate monitoring objects
 *
 * ABSTRACT:   Safety's monitoring for AC infusion rate.  This includes:
 *                AC Infusion rate calculated at the end of a draw cycle
 *                   covering the draw and previous return
 *                AC Infusion rate during Rinseback
 *                AC Infusion rate during Access Keep Open operations
 *
 */

#include "s_cont.hpp"
#include "alarms.h"

#include "s_donor.hpp"
#include "hypovol.hpp"  // needed to get EC volume for adjusted donor HCT calculation

#ifndef ACINFUS_HPP  //  Prevents multiple compilations.
#define ACINFUS_HPP

#define MAX_AC_ALLOWED_DURING_RB_OR_DISC  1.0f // mls
#define RB_INFUSION_THRESHOLD_COUNTER     10   // infusion > 1.3 consecutive counter during RB

// if we see this volume pumped by the return pump during a draw
// cycle, assume we are starting an Access Keep Open operation
#define  AKO_TRIGGER_VOLUME                              0.5f  // ml

// if we see this much time go by with the return pump running during
// a draw cycle, we will assume we are in an Access Keep Open operation
#define  AKO_MIN_CYCLE_TIME                              2.0f   // sec

// during AKO operations during Blood Prime, limit the return
// volume to the AC bolus spec under the assumption that the
// set is full of AC.
#define AC_BOLUS_SPEC                                    20.0f // ml

// 11/26/96
// new infusion monitoring will be based on :
// a one cycle max of 1.6 ml/min - l TBV
// a two cycle average of 1.45
// and a three cycle average of 1.3
//
#define MAX_ALLOWABLE_AC_INFUSION_SINGLE_CYCLE          1.60f
#define MAX_ALLOWABLE_AC_INFUSION_TWO_CYCLE_AVG         1.45f
#define MAX_ALLOWABLE_AC_INFUSION_THREE_CYCLE_AVG       1.30f


// 11/26/96
// return pump speed limit during rinseback
// speed limit 100 ml/min
// 06/19/97 move limit to 250 ml/min
// make alarm threshold one-shot
//
#define RETURN_FLOW_RATE_LIMIT_RB                       250.0f   // ml/min

//
// 03/14/02
// to accurately model the RBC Hematocrit, the recirculation volume
// from the needle tip through the manifold must be taken into account.
//
const float MANIFOLD_VOLUME = 3.0f;

// enumeration to get private data from infusion objects
enum INF_PDATA_TYPE
{
   INFPD_PLATELET = 1,          // ACInfusion->_faccumACToPlateletBag
   INFPD_PLASMA   = 2,          // ACInfusion->_faccumACToPlasmaBag
   INFPD_RBC      = 3,          // ACInfusion->_faccumACToRBCBag
   INFPD_INF      = 4           // ACInfusion->_fACInfusionPerTBV
};

//
// The plasma rinseback protocol allow the plasma valve to move and returns
//  plasma collected in the plasma bag to be used to flush the set.  To be safe
//  the plasma valve is monitored to ensure that the valve is not open for more
//  than 55 seconds before and infusion rate assuming all plasma mixed with AC.
//
#define MAX_PLASMA_VALVE_OPEN_TIME 55 // seconds

/*******************************************************************************
 *
 *  ACInfusion CLASS DEFINITION
 *    Calculates and monitors AC infusion rate at the end of every draw
 *    cycle for the draw and the preceeding return cycle.
 *
 ******************************************************************************/
class ACInfusion
{
private:
   float              _faccumDrawCycleInletRBCCollect;
   float              _faccumReturnCycleInletRBCCollect;
   float              _faccumCollectedRBCs;
   float              _faccumACToPlateletBag;
   float              _faccumACToPlasmaBag;
   float              _faccumACToRBCBag;
   float              _faccumSetVolume;
   float              _fadjustedDonorHCT;
   struct timespec    _drawCycleTimer;
   struct timespec    _returnCycleTimer;
   float              _fACInfusionPerTBV;
   float              _fACInfusionPerTBV_2;
   float              _fACInfusionPerTBV_3;
   bool               _bACInfusionMonitoringActive;
   MON_TYPE           _monType;
   bool               _dumpFlag;
   InletContainer*    _Inlet;
   ReturnContainer*   _Return;
   ACContainer*       _AC;
   PlateletContainer* _Platelet;
   PlasmaContainer*   _Plasma;
   RBCContainer*      _RBC;
   aDonor*            _Donor;
   HypovolemiaDonor*  _HypovolemiaDonorMonitor;
   void  CalculateDonorHCT (float avgACRatio);
   float CalculateACToPlateletBag (char cCycle, float fraction);
   float CalculateACToPlasmaBag (char cCycle, float fraction);
   float CalculateACToRBCBag (char cCycle, float fraction, float avgACRatio);
public:
   float _fdrawCycleTime;
   float _freturnCycleTime;
   ACInfusion(InletContainer* InletCont, ReturnContainer* ReturnCont,
              ACContainer* ACCont, PlateletContainer* PlateletCont,
              PlasmaContainer* PlasmaCont, RBCContainer* RBCCont,
              aDonor* Donor, HypovolemiaDonor* HypoDonorMon);
   ~ACInfusion();
   void  UpdateAccumulators (char cCycle);
   void  UpdateTimers (CHW_RESERVOIR_STATES newReservoir);
   void  UpdateInfusionRate (float avgACRatio);
   float GetPrivateFloatData (INF_PDATA_TYPE type);
   void  InitializeForState (long newState);
   void  SetSetType (SET_TYPE setType);
   void  SetMonitoringType (MON_TYPE type){_monType = type; SaveToPFR(); }
   void  ResetCycleVol (char cCycle);
   void  Dump (DataLog_Level& log_level);
   void  SaveToPFR ();
   void  RestoreFromPFR ();

};

/*******************************************************************************
 *
 *  ACInfusionRinseback CLASS DEFINITION
 *    Calculates and monitors AC infusion rate during Rinseback.
 *
 ******************************************************************************/

class ACInfusionRinseback
{
private:
   float            _faccumACDuringRBAndDisconnect;
   float            _fadjustedDonorHCT;
   float            _avgACRatio;
   float            _RBInfusionRate;
   float            _deltaACToDonor;
   struct timespec  _returnPumpTimer;
   struct timespec  _rinsebackTimer;
   struct timespec  _valveOpenTimer;
   unsigned long    _valveOpenAccumulatedTime; /*usecs*/
   float            _deltaControlMessageTime;  /*seconds*/
   int              _iRBHighInfusionCounter;
   bool             _bACInfusionMonitoringActive;
   bool             _bPlasmaValveOpenTooLong;
   MON_TYPE         _monType;
   bool             _dumpFlag;
   InletContainer*  _Inlet;
   ReturnContainer* _Return;
   ACContainer*     _AC;
   aDonor*          _Donor;
   TimerMessage     _LogInfoTimer;
   void  IsACRunningDuringRBorDisc (HW_CASSETTE_STATES position);
   void  IsReturnTooFastDuringRB (long deltaControlMessageTime);
   float CalculateACFlow (float avgACRatio, float flowToDonor);
   void  logInfo ();
public:
   ACInfusionRinseback(InletContainer* InletCont, ReturnContainer* ReturnCont,
                       ACContainer* ACCont, aDonor* Donor);
   ~ACInfusionRinseback();
   void Update (float avgACRatio, long deltaControlMessageTime, HW_VALVE_STATES plasmaValveState);
   void UpdatePumpChecks (HW_CASSETTE_STATES position, long deltaControlMessageTime);
   void InitializeForState (long newState);
   void SetMonitoringType (MON_TYPE type){_monType = type; SaveToPFR(); }
   void ResetCycleVol (char cCycle);
   void Dump (DataLog_Level& log_level);
   void SaveToPFR ();
   void RestoreFromPFR ();
};

/*******************************************************************************
*
*  ACInfusionAKO CLASS DEFINITION
*    Calculates and monitors AC infusion rate during Access Keep Open operations
*
******************************************************************************/

class ACInfusionAKO
{
private:
   float            _faccumAKOVolToDonor;
   float            _flastReturnToDonorDuringDraw;
   struct timespec  _AKODrawCycleTimer;
   bool             _bACInfusionMonitoringActive;
   bool             _blimitReturnVolumeOnly;
   MON_TYPE         _monType;
   bool             _dumpFlag;
   InletContainer*  _Inlet;
   ReturnContainer* _Return;
   aDonor*          _Donor;
public:
   ACInfusionAKO(InletContainer* InletCont,
                 ReturnContainer* ReturnCont,
                 aDonor* Donor);
   ~ACInfusionAKO();
   void Update (char cCycle, float avgACRatio, float returnLinePrimeVolume);
   void InitializeForState (long newState);
   void SetMonitoringType (MON_TYPE type){_monType = type; SaveToPFR(); }
   void ResetCycleVol (char cCycle);
   void Dump (DataLog_Level& log_level);
   void SaveToPFR ();
   void RestoreFromPFR ();
};


#endif      // end ACINFUS_HPP

/* FORMAT HASH 5667262cb045efd5862ca67f7f1bcf26 */

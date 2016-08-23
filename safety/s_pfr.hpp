/*******************************************************************************
 *
 * Copyright (c) 1997 by Cobe BCT, Inc.  All rights reserved.
 *
 *
 * s_pfr.hpp
 *
 * $Header$
 * Revision 1.37.1.2  2009/10/05 16:04:21Z  dslausb
 * Shawn's safety code
 * Revision 1.1  2009/10/02 19:12:17Z  dslausb
 * Initial revision
 * Revision 1.37  2006/03/18 00:07:17Z  jheiusb
 * revised for PFR in MSS
 * Revision 1.36  2004/05/07 22:51:21Z  ms10234
 * 6966 - save the ac prime volume at the entrance to donor connected state and subtract that number from the cycle accumulator volume to check for draw too small.
 * Revision 1.35  2004/05/07 16:24:29Z  ms10234
 * 6966 - Change first cycle volume accumulator to not use AC_PRIMEvolume
 * Revision 1.34  2003/12/04 20:13:24Z  ms10234
 * 6026 6104 - Add PFR values for ac infusion monitoring during plasma rinseback.
 * 6604 - Add forceInitialize flag to PFR data for cassette position monitoring.
 * Revision 1.33  2003/11/05 22:12:15Z  ms10234
 * 5959 - compensate for possible time drift between processors in PF conditions.
 * Revision 1.32  2003/09/09 22:43:41Z  ms10234
 * 6099 - change platelet depletion algorithm
 * Revision 1.31  2003/07/18 15:36:46Z  jl11312
 * - modified power fail save to use raw disk writes
 * Revision 1.30  2002/12/27 16:22:49Z  pn02526
 * Runnable safety executive with CRC checking of the code disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.
 * Revision 1.29  2002/10/10 14:25:36  pn02526
 * Massive checkin of fixed modules for integration checking
 * Revision 1.28  2002/08/08 11:23:40  pn02526
 * To keep GNU compiler happy for production builds
 * + Change type of PFR_MAX_DURATION from unsigned long to long.
 * + Declare all untyped functions as void.
 * Revision 1.27  2001/07/18 15:57:43  jl11312
 * - modified to have safety notify proc of safety PFR file status
 * Revision 1.26  2001/06/21 21:27:08  ms10234
 * 4922 - PFR implementation
 * Revision 1.25  2000/07/11 23:48:21  bs04481
 * Extend pause monitoring back to Blood Prime
 * Revision 1.24  2000/07/10 15:20:24  bs04481
 * Disable hypervolemia monitoring during the disconnect test.
 * Revision 1.23  2000/06/30 21:29:19  bs04481
 * Re-did valve monitoring to not monitor by draw and return cycles
 * but to alarm anytime there is a valve out of position for 20
 * consecutive seconds while the associated pump is running.
 * Revision 1.22  2000/06/22 16:46:43  bs04481
 * Plumbing changes for single-stage.  1) Change to RBC flow.
 * 2) Add valve position monitoring during Blood Run and Blood
 * Rinseback. 3)  Add compile flag to support dual-stage
 * Revision 1.21  2000/06/08 21:37:03  bs04481
 * Add donorVitals to the saved PFR data.
 * Revision 1.20  2000/05/26 17:12:16  BS04481
 * More maintanence changes:  This set contains changes for
 * 1. Moving valve object from container to bag
 * 2. Removing SafetyTimer->SafetySoftTimer and letting the
 *      switch and disconnect tests have their own timers.
 * 3.  All message types are changed to NO_BOUNCE to cut down
 *      the on unnecessary stuff in the message que.
 * Revision 1.19  2000/03/15 18:22:03  BS04481
 * Remove PFR timer in favor of generalized pump timer in the
 * needle monitor; add saved data for needle monitor in order to
 * time pauses across power fails.
 * Revision 1.18  2000/02/28 21:49:47  SB07663
 * renamed "..\router\hw.h" to "hw.h" (now in low_level directory)
 * Revision 1.17  1999/08/20 00:22:40  BS04481
 * Need to subtract off the return line prime while testing for the AC
 * bolus limit in first draw.  While I was in there, changed the label
 * of the accumulator to make operation clearer
 * Revision 1.16  1999/08/01 20:53:16  BS04481
 * Non-critical code review changes identified at the 3.2 release.
 * Revision 1.15  1999/06/25 17:09:53  BS04481
 * Corrections for AC ratio and AC ratio use during Blood Prime and
 * First cycle of Blood Run.  1) Correct formula used to calculate
 * AC ratio during first cycle by removing 5 ml bolus from the
 * numerator.  2) For purpose of determining if there is anti-coagulated
 * blood in the needle, allow ratio to go below 5.  3) For AKO during
 * Blood Prime, enforce 20ml limit on return volume because we don't
 * really know the AC ratio during Blood Prime so just enforce the
 * bolus.
 * Revision 1.14  1999/06/06 17:31:41  BS04481
 * Make decision on recoverability after a PF that happens during
 * first phase of rinseback dependent on time only.  In an AKO, only
 * disable the pump power timer if 3 ml (measured by encoders) has
 * been returned.
 * Revision 1.13  1999/04/01 19:47:20  BS04481
 * Change adjusted donor HCB formula to accumulate the collected
 * RBCs directly instead of accumulating the Input volume during
 * RBC collect and averaging RBCs collected.
 * Revision 1.12  1999/03/31 23:21:53  BS04481
 * Save PFR data for adjusted donor HCT in the infusion monitor
 * and for adjusted first cycle volumes in the hypovolemia monitor.
 * Revision 1.11  1998/10/20 20:40:39  bs04481
 * Change to power fail recovery to restore PF data and resume all
 * active monitoring immediately after Safety has determined that
 * the procedure is recoverable.
 * Added all donor data to the PFR data instead of just TBV.
 * Perform an exact comparison of TBV from donor vital data which
 * is received  after a PFR with TBV from restored data even if
 * TBVLockOut is in place.
 * Revision 1.10  1998/07/20 14:51:16  bs04481
 * Added Halls vs encoders limits to power fail data.
 * Revision 1.9  1998/06/24 15:19:09  bs04481
 * Changes to make pump stroke volumes dependent on set type.
 * Change necessary for DRBC.
 * Revision 1.8  1998/06/09 18:06:36  bs04481
 * Increase PFR time from 5 minutes to 7 minutes.
 * Revision 1.7  1998/05/12 19:47:14  bs04481
 * Change reservoir monitoring, which includes ac ratio, ac infusion
 * and hypovolemia montiroing, to an object oriented design.
 * Previously, the volumes were accumulated in the s_res code.
 * With this change, the machine is modeled as six containers, each
 * consisting of some combination of bags, valves and pumps.  The
 * safety monitors now query the containers and calculate the
 * safety variables.  See the safety executive design document for
 * version 3.2 software.
 * Revision 1.6  1998/02/05 19:51:38  bs04481
 * Add save and restore of previous cycle hall vs encoder error
 * amount during power fail and recovery
 * Revision 1.5  1998/01/22 18:01:25  bs04481
 * Fix potential problem with incorrectly deciding the anti-coagulation
 * of the needle if we paused or power failed in Rinseback before
 * we had pumped 3 ml back to the donor.
 * Revision 1.4  1997/12/12 17:12:16  bs04481
 * Make the pump power timer object which is used during PFR
 * usable for the pump power timer which is used after a stop
 * or pause button push.
 * Revision 1.3  1997/12/11 16:46:34  bs04481
 * Add CRC of Safety's power fail data
 * Revision 1.2  1997/12/05 21:10:07  bs04481
 * Add saved data for Access Keep Open operations and to monitor
 * net AC to the Donor over the complete procedure.
 * Revision 1.1  1997/12/02 16:01:37  bs04481
 * Initial revision
 */
#ifndef S_PFR_H
#define S_PFR_H

#include "s_type.h"
#include "hw.h"
#include "s_glob.hpp"
#include "alarms.h"
#include "needle.hpp"
#include "filenames.h"
#include "state_objects.hpp"
#include "s_donor.hpp"
#include "s_res.hpp"

#define PFR_AC_IN_NEEDLE_VOL 3.0f                  // amount pumped before we agree there
                                                   // is AC in the needle
const long          PFR_MAX_DURATION  = (10 * 60); // maximum recoverable duration of
                                                   // power outage in seconds
const unsigned long PUMP_MAX_DURATION = (10 * 60); // maximum recoverable duration of
                                                   // a paused pump in seconds

// Safety Power Fail Recovery Data
typedef struct
{
   unsigned long PFTimeOfFailure;                     // Timestamp (secs) when PF occurred
   bool          PFRSafetyOneShot;                    // TRUE=Safety in PFR
   bool          PFRSafetyAlarmInProgress;            // TRUE=Safety alarm at time of PF

   HW_ORDERS     PFRdonorConnectMode;                 // DonorConnect state at time of PF

   // Data from executive
   SET_TYPE    PFR_SetType;
   State_names PFR_SystemState;

   long        PFRanInlet_lAccumRevs;         // pump data is also saved/restored by the executive
   float       PFRanInlet_fDeltaVolSet;
   float       PFRanInlet_fSV;
   long        PFRanInlet_lAccumHalls;

   long        PFRaReturn_lAccumRevs;
   float       PFRaReturn_fDeltaVolSet;
   float       PFRaReturn_fSV;
   long        PFRaReturn_lAccumHalls;

   long        PFRanAC_lAccumRevs;
   float       PFRanAC_fDeltaVolSet;
   float       PFRanAC_fSV;
   long        PFRanAC_lAccumHalls;

   long        PFRaPlatelet_lAccumRevs;
   float       PFRaPlatelet_fDeltaVolSet;
   float       PFRaPlatelet_fSV;
   long        PFRaPlatelet_lAccumHalls;

   long        PFRaPlasma_lAccumRevs;
   float       PFRaPlasma_fDeltaVolSet;
   float       PFRaPlasma_fSV;
   long        PFRaPlasma_lAccumHalls;

   // Data from s_donor
   float        PFR_fSafetyDonorTBV_liters;           // Last Donor TBV in liters
   float        PFR_fECLimit_ml;                      // Last donor extracorporeal limie in ml
   float        PFR_fDonorHct_decimal;                // Last donor hematocrit
   float        PFR_fDonorPltPrecount;                // Last donor platelet precount
   float        PFR_fDonorPlasmaFraction;             // Last donor plasma fraction
   char         PFR_cDonorConnectedState;
   char         PFR_cDonorVitalStatus;
   bool         PFR_bTBVLockOut;
   SDonorVitals PFR_DonorVitals;

   // Data from s_res
   bool     PFR_bLookingForFirstFluid;
   bool     PFR_bPauseMonitoringActive;
   MON_TYPE PFR_cCycleMon;
   char     PFR_cSNCycle;
   float    PFR_pumpTimerInletVolumeSnap;
   bool     PFR_inMeteredStorage;

   // data from containers
   bool     PFRInlet_pumpErrorLogFlag;
   MON_TYPE PFRInlet_monType;

   bool     PFRReturn_pumpErrorLogFlag;
   MON_TYPE PFRReturn_monType;
   float    PFRReturn_fReturnVolumeAwayFromDonor;
   float    PFRReturn_fReturnPrimeVolume;

   bool     PFRAC_pumpErrorLogFlag;
   MON_TYPE PFRAC_monType;
   float    PFRAC_fACPrimeVolume;

   bool     PFRPlatelet_pumpErrorLogFlag;
   MON_TYPE PFRPlatelet_monType;

   bool     PFRPlasma_pumpErrorLogFlag;
   MON_TYPE PFRPlasma_monType;

   bool     PFRRBC_pumpErrorLogFlag;
   MON_TYPE PFRRBC_monType;

   // data from bags
   float           PFRInlet_initialVolume;
   float           PFRInlet_maximumVolume;
   float           PFRInlet_minimumVolume;
   bool            PFRInlet_initialVolumeKnown;
   bool            PFRInlet_maximumVolumeKnown;
   bool            PFRInlet_minimumVolumeKnown;
   FLOW_DIRECTION  PFRInlet_bagflowIO;
   HW_VALVE_STATES PFRInlet_bagvalveIO;
   float           PFRInlet_accumulatedVolume;
   float           PFRInlet_deltaVolume;
   float           PFRInlet_drawCycleVolume;
   float           PFRInlet_returnCycleVolume;
   float           PFRInlet_drawCycleVolumeInfusion;
   float           PFRInlet_returnCycleVolumeInfusion;

   float           PFRReturn_initialVolume;
   float           PFRReturn_maximumVolume;
   float           PFRReturn_minimumVolume;
   bool            PFRReturn_initialVolumeKnown;
   bool            PFRReturn_maximumVolumeKnown;
   bool            PFRReturn_minimumVolumeKnown;
   FLOW_DIRECTION  PFRReturn_bagflowIO;
   HW_VALVE_STATES PFRReturn_bagvalveIO;
   float           PFRReturn_accumulatedVolume;
   float           PFRReturn_deltaVolume;
   float           PFRReturn_drawCycleVolume;
   float           PFRReturn_returnCycleVolume;
   float           PFRReturn_drawCycleVolumeInfusion;
   float           PFRReturn_returnCycleVolumeInfusion;

   float           PFRAC_initialVolume;
   float           PFRAC_maximumVolume;
   float           PFRAC_minimumVolume;
   bool            PFRAC_initialVolumeKnown;
   bool            PFRAC_maximumVolumeKnown;
   bool            PFRAC_minimumVolumeKnown;
   FLOW_DIRECTION  PFRAC_bagflowIO;
   HW_VALVE_STATES PFRAC_bagvalveIO;
   float           PFRAC_accumulatedVolume;
   float           PFRAC_deltaVolume;
   float           PFRAC_drawCycleVolume;
   float           PFRAC_returnCycleVolume;
   float           PFRAC_drawCycleVolumeInfusion;
   float           PFRAC_returnCycleVolumeInfusion;

   float           PFRPlasma_initialVolume;
   float           PFRPlasma_maximumVolume;
   float           PFRPlasma_minimumVolume;
   bool            PFRPlasma_initialVolumeKnown;
   bool            PFRPlasma_maximumVolumeKnown;
   bool            PFRPlasma_minimumVolumeKnown;
   FLOW_DIRECTION  PFRPlasma_bagflowIO;
   HW_VALVE_STATES PFRPlasma_bagvalveIO;
   float           PFRPlasma_accumulatedVolume;
   float           PFRPlasma_deltaVolume;
   float           PFRPlasma_drawCycleVolume;
   float           PFRPlasma_returnCycleVolume;
   float           PFRPlasma_drawCycleVolumeInfusion;
   float           PFRPlasma_returnCycleVolumeInfusion;

   float           PFRPlatelet_initialVolume;
   float           PFRPlatelet_maximumVolume;
   float           PFRPlatelet_minimumVolume;
   bool            PFRPlatelet_initialVolumeKnown;
   bool            PFRPlatelet_maximumVolumeKnown;
   bool            PFRPlatelet_minimumVolumeKnown;
   FLOW_DIRECTION  PFRPlatelet_bagflowIO;
   HW_VALVE_STATES PFRPlatelet_bagvalveIO;
   float           PFRPlatelet_accumulatedVolume;
   float           PFRPlatelet_deltaVolume;
   float           PFRPlatelet_drawCycleVolume;
   float           PFRPlatelet_returnCycleVolume;
   float           PFRPlatelet_drawCycleVolumeInfusion;
   float           PFRPlatelet_returnCycleVolumeInfusion;

   float           PFRRBC_initialVolume;
   float           PFRRBC_maximumVolume;
   float           PFRRBC_minimumVolume;
   bool            PFRRBC_initialVolumeKnown;
   bool            PFRRBC_maximumVolumeKnown;
   bool            PFRRBC_minimumVolumeKnown;
   FLOW_DIRECTION  PFRRBC_bagflowIO;
   HW_VALVE_STATES PFRRBC_bagvalveIO;
   float           PFRRBC_accumulatedVolume;
   float           PFRRBC_deltaVolume;
   float           PFRRBC_drawCycleVolume;
   float           PFRRBC_returnCycleVolume;
   float           PFRRBC_drawCycleVolumeInfusion;
   float           PFRRBC_returnCycleVolumeInfusion;

   // data from valves
   HW_VALVE_STATES PFRPlasma_positionC;
   HW_VALVE_STATES PFRPlasma_positionS;
   int             PFRPlasma_iT1Status;
   bool            PFRPlasma_bValvePositionMonitorEnabled;

   HW_VALVE_STATES PFRPlatelet_positionC;
   HW_VALVE_STATES PFRPlatelet_positionS;
   int             PFRPlatelet_iT1Status;
   bool            PFRPlatelet_bValvePositionMonitorEnabled;

   HW_VALVE_STATES PFRRBC_positionC;
   HW_VALVE_STATES PFRRBC_positionS;
   int             PFRRBC_iT1Status;
   bool            PFRRBC_bValvePositionMonitorEnabled;

   // data from cassette
   HW_CASSETTE_STATES PFRCassette_position;
   unsigned char      PFRCassette_cDeviceConstraint;
   bool               PFRCassette_forceInitialize;


   // Data from needle coagulation monitor
   MON_TYPE PFRNeedle_monType;
   long     PFRNeedle_startTime;
   long     PFRNeedle_duration;
   bool     PFRNeedle_timerHasFired;
   float    PFRNeedle_pumpTimerInletAccumulator;
   float    PFRNeedle_pumpTimerReturnAccumulator;


   // Data from average ac ratio monitoring
   float    PFR_fAccumInletVolumeAtHighRatio;
   float    PFR_fAccumACVolumeAtLowRatio;
   bool     PFR_bUseableACRatio;
   float    PFR_fAvgACRatio;
   float    PFR_fLastKnownGoodRatio;
   bool     PFR_acRatioA_bACRatioMonitoringActive;
   MON_TYPE PFR_acRatioA_monType;

   // Data from ac ratio pump volume monitoring
   float    PFR_fAccumACTooFastVolume;
   int      PFR_iHighRatioPersistsCounter;
   bool     PFR_acRatioP_bACRatioMonitoringActive;
   MON_TYPE PFR_acRatioP_monType;

   // Data from instantaneous ac ratio monitoring
   int      PFR_iHighRatioCounter;
   int      PFR_iLowRatioCounter;
   MON_TYPE PFR_acRatioI_monType;
   bool     PFR_acRatioI_bACRatioMonitoringActive;

   // Data from cycle-based AC Infusion rate monitoring
   float           PFR_faccumDrawCycleInletRBCCollect;
   float           PFR_faccumReturnCycleInletRBCCollect;
   float           PFR_faccumCollectedRBCs;
   float           PFR_faccumACToPlateletBag;
   float           PFR_faccumACToPlasmaBag;
   float           PFR_faccumACToRBCBag;
   float           PFR_faccumSetVolume;
   float           PFR_fadjustedDonorHCT;
   struct timespec PFR_drawCycleTimer;
   struct timespec PFR_returnCycleTimer;
   float           PFR_fdrawCycleTime;
   float           PFR_freturnCycleTime;
   float           PFR_fACInfusionPerTBV;
   float           PFR_fACInfusionPerTBV_2;
   float           PFR_fACInfusionPerTBV_3;
   bool            PFR_acInfusC_bACInfusionMonitoringActive;
   MON_TYPE        PFR_acInfusC_monType;

   // Data from AC Infusion rate during Rinseback monitoring
   float           PFR_faccumACDuringRBAndDisconnect;
   struct timespec PFR_returnPumpTimer;
   struct timespec PFR_rinsebackTimer;
   struct timespec PFR_valveOpenTimer;
   unsigned long   PFR_valveOpenAccumulatedTime;
   int             PFR_iRBHighInfusionCounter;
   bool            PFR_acInfusR_bACInfusionMonitoringActive;
   bool            PFR_acInfusR_bPlasmaValveOpenTooLong;
   MON_TYPE        PFR_acInfusR_monType;

   // Data from AC infusion rate during Access Keep Open monitoring
   float           PFR_faccumAKOVolToDonor;
   float           PFR_flastReturnToDonorDuringDraw;
   struct timespec PFR_AKODrawCycleTimer;
   bool            PFR_acInfusA_bACInfusionMonitoringActive;
   bool            PFR_acInfusA_blimitReturnVolumeOnly;
   MON_TYPE        PFR_acInfusA_monType;

   // Data from hypovolemia cycle-size monitoring
   float    PFR_faccumVolume;
   float    PFR_facPrimeVolume;
   float    PFR_firstCycleMinVolumeAllowed;
   float    PFR_firstCycleMaxVolumeAllowed;
   bool     PFR_hypovolC_bHypovolemiaMonitoringActive;
   MON_TYPE PFR_hypovolC_monType;
   MON_TYPE PFR_hypovolC_PrevMonType;

   // Data from donor hypovolemia monitoring
   float    PFR_faccumECVolume;
   int      PFR_iRBValveBadPositionCounter;
   bool     PFR_hypovolD_bHypovolemiaMonitoringActive;
   bool     PFR_hypovolD_bHypervolemiaMonitoringActive;
   MON_TYPE PFR_hypovolD_monType;

   // Data from component depletion monitoring
   float         PFR_fWBProcessedDuringRBCCollect;
   float         PFR_fWBProcessedNotDuringRBCCollect;
   bool          PFR_compdepl_bDepletionMonitoringActive;
   bool          PFR_compdepl_bPlateletCollectionStarted;
   MON_TYPE      PFR_compdepl_monType;

   unsigned long PFRDataCRC;
} SPFRecovery;


/*******************************************************************************
*
*  a Safety PFRecovery CLASS DEFINITION
*
******************************************************************************/

class aPFR_Safety
{
public:
   aPFR_Safety();                                  // constructor
   virtual ~aPFR_Safety();                         // destructor
   virtual void init (aDonor* paDonor, aReservoir_Safety* paReservoir);
   void         saveFile ();                       // saves the PFR Data

   void ifPFRTimerNeeded (needleMonitor* needle);
   void SendPFRStatusMsg (void);                    // send PFR file status message to proc

private:
   aPFR_Safety(aPFR_Safety const&);                // not implemented
   aPFR_Safety& operator = (aPFR_Safety const&);   // not implemented
   void                  initializePFRFile ();     // create PF file if none exists
   void                  setPFChecksum ();         // determine if PFR is viable for data integrity
   void                  setPFDuration ();         // determine if PFR is viable for time
   void                  setPFState ();            // determine if PFR is viable for state
   void                  setPFNeedle ();           // determine if PFR is viable for AC in needle
   void                  setPFAlarm ();            // determine if PFR is viable for previous alarm

   long        _dt;

   SPFRecovery _SafetyPFRData;                     // local copy of Safety PFR Data
   bool        _pfSaveEnabled;

   bool        _PFRChecksumOK;                     // TRUE=checksum matches
   bool        _PFRDurationOK;                     // TRUE=time limit not exceeded
   bool        _PFRStateOK;                        // TRUE=state is possible for recovery
   bool        _PFRNeedleOK;                       // TRUE=Needle has AC
   bool        _PFRAlarmOK;                        // TRUE=Not in Safety alarm state
   bool        _PFRTimerNeeded;                    // TRUE=Need to time the pumps
};
#endif // S_PFR_H

/* FORMAT HASH 0eb6c5a008faa8b3c1fa006d5880a11c */

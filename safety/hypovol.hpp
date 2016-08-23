/*
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.31  2007/12/11 21:05:30Z  spriusb
 * Fix for IT8322 & 8329: added variable to synch WBP recirc adjustment to once per draw cycle.
 * Revision 1.30  2004/05/13 20:09:29Z  rm70006
 * IT 6966.  Changed first cycle draw volume to add more margin with control.
 * Revision 1.29  2004/05/07 22:51:12Z  ms10234
 * 6966 - save the ac prime volume at the entrance to donor connected state and subtract that number from the cycle accumulator volume to check for draw too small.
 * Revision 1.28  2004/05/07 22:21:58Z  ms10234
 * 6966 - change first cycle too small to 164.17
 * Revision 1.27  2004/05/07 16:24:20Z  ms10234
 * 6966 - Change first cycle volume accumulator to not use AC_PRIMEvolume
 * Revision 1.26  2004/02/03 21:47:22Z  ms10234
 * 6791 - changes to first cycle volumes
 * Revision 1.25  2004/01/20 23:04:53Z  ms10234
 * 6750 - removed startup volume from the whole blood processed volume.
 * Revision 1.24  2003/09/09 23:10:21Z  ms10234
 * 6406 - fixed pfr code
 * Revision 1.23  2003/09/09 22:43:33Z  ms10234
 * 6099 - change platelet depletion algorithm
 * Revision 1.22  2003/04/04 16:29:07Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.21  2002/07/30 15:56:18Z  pn02526
 * 5708 - First cycle volume changes
 * Synchronize with changes in revision 1.21 V5.0.
 * Revision 1.20  2002/04/15 10:46:53  pn02526
 * Bring in latest from V5.0
 * Revision 1.20  2002/04/09 15:01:53  ms10234
 * 5643 - Change volume limits due to filler changes
 * Revision 1.19  2002/04/02 14:56:51  ms10234
 * 5643 - Changed first cycle volumes per IT5643
 * Revision 1.18  2002/03/22 17:53:35  ms10234
 * 5631 - Change first cycle limits for the mini-undercut filler
 * Revision 1.17  2002/02/21 16:05:02  pn02526
 * Apply changes to constants per latest Response in IT 4844
 * Revision 1.16  2002/01/17 14:00:01  MS10234
 * 4844 - change first cycle limits per IT
 * Revision 1.15  2001/10/23 13:21:16  jl11312
 * - modified first cycle limits (IT 4844)
 * Revision 1.14  2001/09/25 15:38:31  ms10234
 * 5322 - Added ACPrime volume to the first cycle limits.
 * Revision 1.13  2001/08/27 21:37:22  ms10234
 * 5271 -  Remove references to stamp color
 * Revision 1.12  2001/06/26 14:49:38  ms10234
 * 5171 - Added a prev cycle state variable to hold the cycle we were in
 * before rinseback.
 * Revision 1.11  2001/03/05 19:12:14  ms10234
 * IT4844 - Changed set limits for SS.
 * Revision 1.10  2000/07/20 19:31:47  bs04481
 * Move some variables and methods from public to private
 * Revision 1.9  2000/07/11 14:33:16  bs04481
 * Temporarily lower the minimum first cycle draw limit in order to
 * experimentally determine what it should be
 * Revision 1.8  2000/07/10 15:20:13  bs04481
 * Disable hypervolemia monitoring during the disconnect test.
 * Revision 1.7  1999/04/01 22:43:13  BS04481
 * Add hypervolemia monitoring to limit the added fluid to the donor
 * to 1L.
 * Revision 1.6  1999/03/31 23:24:40  BS04481
 * Change method of calculating Vbp for RBC collect from a straight
 * 18.5 % of TBV to a method which depends on entered donor HCT.
 * See IT 3875, 3442.
 * Revision 1.5  1998/09/08 20:06:23  bs04481
 * Duplicate of rev 1.3 because 1.4 is for DRBC clinical build only
 * Revision 1.3  1998/06/16 21:22:51  bs04481
 * Change first cycle volume offset due to the lack of an LRS
 * chamber in the DRBC set from 15 to 11.1
 * Revision 1.2  1998/05/18 17:48:41  bs04481
 * Add set type to hypovolemia cycle volume monitoring
 * in order to adjust the first cycle volume limits due to the lack of
 * an LRS chamber.
 * Revision 1.1  1998/05/12 19:49:39  bs04481
 * Initial revision
 *
 *
 * TITLE:      hypovol.hpp, the hypovolemia monitoring objects
 *
 * ABSTRACT:   Safety's monitoring for donor hypovolemia.  This includes:
 *                Monitoring associated with draw cycle size
 *                Donor hypovolemia over the course of the procedure
 *                Monitoring for valve position during Rinseback
 *                Component depletion
 *
 */

#include "s_cont.hpp"
#include "alarms.h"
#include "s_donor.hpp"

#ifndef HYPOVOL_HPP
#define HYPOVOL_HPP

// 03/23/96 pump monitoring additions.
// 08/20/96 increase first cycle volume to prevent false alarms on first draw cycle
// per marlene : procedure volume could be as high as 240.0 ml calculated as safety does
//
// 02/21/02 change per latest IT 4844 Response
// 03/22/02 Change for the mini-undercut filler (IT 5631)
// 04/02/02 Change back to old filler (IT5643)
// 04/09/02 Final filler change (IT5643)
// 07/30/02 Change after review of clinical data (IT5708)
#define MAX_ALLOWABLE_DRAW_CYCLE_VOLUME_FIRST_CYCLE      255.67f

// 08/14/96 remove magic number for the minimum allowable
// inlet volume before a return cycle may be allowed.
//
// 02/21/02 change per latest IT 4844 Response
// 03/22/02 Change for the mini-undercut filler (IT 5631)
// 04/02/02 Change back to old filler (IT5643)
// 04/09/02 Final filler change (IT5643)
// 07/30/02 Change after review of clinical data (IT5708)
#define MIN_ALLOWABLE_DRAW_CYCLE_VOLUME_FIRST_CYCLE      161.77f

// 15May98 The rbc/plasma set does not have an LRS chamber so subtract
// some volume from the min and max first cycle limits
//
// 03/05/01 change for SS set type per IT4844
// 07/30/02 Change after review of clinical data (IT5708)
#define RBCPLS_SET_FIRST_DRAW_CYCLE_ADJUSTMENT           17.76f

// 07/29/96 rbc whole blood processed limit per
// Marlene Bainbridge 07/29/96 Whole blood processed during RBC collect
// SHALL be <= TBV * 0.182
//
// 1April99
// this limit is disabled because it is no longer used. Post-HCT
// monitoring is now done by calculating a Vbp limit based on
// TBV*(1-.28/HCTdonor) instead of using a percent of TBV.  See
// paragraph below.
// #define MAX_ALLOWABLE_TBV_FRACTION_RBC_COLL  0.182f

// 27Aug98
// We are calculating a moving volume of blood processed
// during RBC collection based on the donor's hematocrit.  The Vbp
// will be recalculated every time the hematocrit changes based on
// a minimum final hematocrit of 28.
#define MIN_ALLOWABLE_POST_CRIT 0.28f

// 05/23/97
// limit the whole blood processed to 2.2 times
// TBV when the RBC valve is not in collect and
// the return pump is not running
#define WBP_MULTIPLIER_LIMIT                            2.20f

// 1April99
// Limit the amount of extracorporeal volume added to the donor
// to 1L.  This is necessary because we are now using replacement
// fluid during RBC/Plasma procedures.  This value is negative
// because EC accumulates in a positive direction when volume
// is being removed from the donor and in the negative direction
// when volume is added to the donor.
#define MAX_ALLOWABLE_EC_CHANGE  (-1000.0f)

// IT6099
// Minimum Vin at which platelet collection could start.  This
// volume is used to start to postcount volume accumulation limiting
// on the platelet valve position.  Before this number has been reached,
// all Vin will accumulate toward the limit.  After this volume has
// been reached, the volume will accumulate toward this limit, after the
// platelet collection has started, only when the platelet valve is
// collecting.
#define MIN_PLATELET_COLLECTION_START 400.0f


// enumeration to get private data from hypovolemia objects
enum HYP_PDATA_TYPE
{
   HYPPD_EC         = 1,            // HypovolemiaDonor->_faccumECVolume
   HYPPD_WBRBC      = 2,            // ComponentDepletion->_fWBProcessedDuringRBCCollect
   HYPPD_WBNOTRBC   = 3,            // ComponentDepletion->_fWBProcessedNotDuringRBCCollect
   HYPPD_ACCUMCYCLE = 4             // HypovolemiaCycleSize->_faccumVolume
};


/*******************************************************************************
*
*  HypovolemiaCycleSize CLASS DEFINITION
*    Monitors the size of the draw cycle for several limits that could catch
*    a previously used set or a set with a leak.  The conditions are:
*       Draw cycle too big
*       First Draw too big
*       First Draw too small
*
******************************************************************************/

class HypovolemiaCycleSize
{
private:
   float              _faccumVolume;
   float              _facPrimeVolume;
   bool               _bHypovolemiaMonitoringActive;
   float              _firstCycleMaxVolumeAllowed;
   float              _firstCycleMinVolumeAllowed;
   MON_TYPE           _monType;
   MON_TYPE           _PrevMonType;
   bool               _dumpFlag;
   InletContainer*    _Inlet;
   ReturnContainer*   _Return;
   PlateletContainer* _Platelet;
   PlasmaContainer*   _Plasma;
   RBCContainer*      _RBC;
public:
   HypovolemiaCycleSize(InletContainer* InletCont,
                        ReturnContainer*  ReturnCont,
                        PlateletContainer* PlateletCont,
                        PlasmaContainer* PlasmaCont,
                        RBCContainer* RBCCont);
   ~HypovolemiaCycleSize();
   void  Update (char cCycle);
   float GetPrivateFloatData (HYP_PDATA_TYPE type);
   void  TestFillTooSmallFirstCycle ();
   void  InitializeForState (long newState);
   void  SetSetType (SET_TYPE setType);
   void  SetMonitoringType (MON_TYPE type)
   {
      if ( type == MON_RINSEBACK &&
           type != _monType )
         _PrevMonType = _monType;
      _monType = type;
      SaveToPFR();
   }
   void ResetCycleVol (char cCycle);
   void Dump (DataLog_Level log_level);
   void SaveToPFR ();
   void RestoreFromPFR ();
};

/*******************************************************************************
*
*  HypovolemiaDonor CLASS DEFINITION
*    Monitors for donor hypovolemia during Blood Run and Blood Rinseback.
*    The conditions are:
*       Volume removed exceeds donor limit
*       Valves not in return during rinseback
*
******************************************************************************/

class HypovolemiaDonor
{
private:
   float              _faccumECVolume;
   int                _iRBValveBadPositionCounter;
   bool               _bHypovolemiaMonitoringActive;
   bool               _bHypervolemiaMonitoringActive;
   bool               _bHypoOneShot;
   MON_TYPE           _monType;
   bool               _dumpFlag;
   InletContainer*    _Inlet;
   ReturnContainer*   _Return;
   ACContainer*       _AC;
   PlateletContainer* _Platelet;
   PlasmaContainer*   _Plasma;
   RBCContainer*      _RBC;
   aDonor*            _Donor;
public:
   HypovolemiaDonor(InletContainer* InletCont,
                    ReturnContainer* ReturnCont,
                    ACContainer* ACCont,
                    PlateletContainer* PlateletCont,
                    PlasmaContainer* PlasmaCont,
                    RBCContainer* RBCCont,
                    aDonor* Donor);
   ~HypovolemiaDonor();
   void  Update (char cCycle);
   float GetPrivateFloatData (HYP_PDATA_TYPE type);
   void  InitializeForState (long newState);
   void  SetMonitoringType (MON_TYPE type){_monType = type; SaveToPFR(); }
   void  ResetCycleVol (char cCycle);
   void  Dump (DataLog_Level log_level);
   void  SaveToPFR ();
   void  RestoreFromPFR ();
};


/*******************************************************************************
*
*  ComponentDepletion CLASS DEFINITION
*    Monitors for component depletion by monitoring the amount of whole
*    blood processed.  The conditions are:
*       Volume of whole blood processed during RBC collect greater than allowed
*       Volume of whole blood processed not during RBC collect greater than allowed
*
******************************************************************************/

class ComponentDepletion
{
private:
   float              _fWBProcessedDuringRBCCollect;
   float              _fWBProcessedNotDuringRBCCollect;
   bool               _bDepletionMonitoringActive;
   bool               _bPlateletCollectionStarted;
   bool               _bDrawCycleBarrier;
   MON_TYPE           _monType;
   bool               _dumpFlag;
   bool               _initWBProcessedDuringRBCCollectAfterPFR;
   bool               _initWBProcessedNotDuringRBCCollectAfterPFR;
   InletContainer*    _Inlet;
   ACContainer*       _AC;
   PlateletContainer* _Platelet;
   RBCContainer*      _RBC;
   aDonor*            _Donor;
public:
   ComponentDepletion(InletContainer* InletCont,
                      ACContainer* ACCont,
                      PlateletContainer* PlateletCont,
                      RBCContainer* RBCCont,
                      aDonor* Donor);
   ~ComponentDepletion();
   void  Update (char cCycle);
   float GetPrivateFloatData (HYP_PDATA_TYPE type);
   void  InitializeForState (long newState);
   void  SetMonitoringType (MON_TYPE type){_monType = type; SaveToPFR(); }
   void  ResetCycleVol (char cCycle);
   void  Dump (DataLog_Level log_level);
   void  SaveToPFR ();
   void  RestoreFromPFR ();
};








#endif   // HYPOVOL_HPP

/* FORMAT HASH a38f92a91716bc337d87319821080f35 */

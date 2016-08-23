/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.49  2003/07/15 21:13:07Z  rm70006
 * IT 6144.  Move enums only used by safety to safety code out of safeproc.
 * Revision 1.48  2002/10/08 14:56:35Z  pn02526
 * Move Donor Vitals message classes to safproc.h and change their definition to not use CDonor class.
 * Revision 1.47  2002/08/07 12:50:03  pn02526
 * Reinstate #include for s_glob.hpp.
 * Substitute $include message.h for the obsolete message system and dispatcher files
 * Use the new buffered message base class definition to send SafetyToGuiDonorVitalsMsg, rather than deriving class aSafetyToGuiDonorVitalsMsg_sndr from it.
 * Add aProcToSafetyDonorVitalsMsg_rcvr :: _theProcToSafetyDonorVitalsMsg to use the new buffered message class definition, rather than deriving from a buffered message class.
 *
 * Revision 1.46  2002/04/15 11:00:37  pn02526
 * Change ASSERT() macro calls to trima_assert().
 * Revision 1.45  2002/03/26 10:34:26  ms10234
 * 5638 - changed HYPO_ADJUST from 68 to 87.
 * Revision 1.44  2000/06/08 21:35:14  bs04481
 * Simplify.  Also, change processing during TBV lockout to restore
 * safety's donor data to the message before sending it to GUI
 * Revision 1.43  2000/05/26 17:12:06  BS04481
 * More maintanence changes:  This set contains changes for
 * 1. Moving valve object from container to bag
 * 2. Removing SafetyTimer->SafetySoftTimer and letting the
 *      switch and disconnect tests have their own timers.
 * 3.  All message types are changed to NO_BOUNCE to cut down
 *      the on unnecessary stuff in the message que.
 * Revision 1.42  2000/03/15 18:16:20  BS04481
 * Move s_donor methods from s_donor.hpp to s_donor.cpp.
 * Add default alarm for unknown problem in donor vital data
 * 3.3 non-essential code review issues.
 * Create SafetyToGuiDonorVital message where used instead
 * of in s_main
 * Revision 1.41  1998/11/12 18:33:17  bs04481
 * Default the EC limit to 85.  When donor data is entered, this will
 * be overwritten but if we never get donor data, the 85 will allow
 * us to catch a runaway inlet pump during donor disconnect.
 * Revision 1.40  1998/11/05 21:47:14  bs04481
 * Explicitly call TBCLockOut for Donor Disconnect and remove
 * default of FALSE
 * Revision 1.39  1998/10/20 20:43:49  bs04481
 * Change power fail recovery to restore PF data and result all
 * active monitoring immediately after Safety determines the
 * procedure is recoverable.
 * Add all donor data to PFR data set instead of just TBV
 * Do an exact TBV comparison of donor vital data received during
 * PFR with previously restored PFR data even if TBVLockOut
 * is in place
 * Revision 1.38  1998/10/09 05:19:29  bs04481
 * Re-activate TBVLockOut which was inadvertently left out in
 * the recent safety re-architecture.
 * Revision 1.37  1998/05/12 19:47:21  bs04481
 * Change reservoir monitoring, which includes ac ratio, ac infusion
 * and hypovolemia montiroing, to an object oriented design.
 * Previously, the volumes were accumulated in the s_res code.
 * With this change, the machine is modeled as six containers, each
 * consisting of some combination of bags, valves and pumps.  The
 * safety monitors now query the containers and calculate the
 * safety variables.  See the safety executive design document for
 * version 3.2 software.
 * Revision 1.36  1998/03/13 22:36:31  bs04481
 * Deleted old kludge code that I had put in during developement.  It
 * was causing a false donor hypovolmeia alarm
 * Revision 1.35  1997/12/12 17:09:36  bs04481
 * Code Cleanup and comments
 * Revision 1.34  1997/12/10 18:26:08  bs04481
 * Change to monitor WBP only when collect valve is open and to
 * use an offset to account for startup volume.
 * Revision 1.33  1997/12/05 21:06:34  bs04481
 * Removed kludge for procedure sending a percentage HCT instead
 * of a whole number HCT after power fail recovery.
 * Revision 1.32  1997/12/02 15:59:55  bs04481
 * Changes to implement power fail recovery
 * Revision 1.31  1997/07/30 18:05:44  bs04481
 * Fix limitation of whole blood processed when not in RBC to
 * ml to ml instead of ml to L.
 * Revision 1.30  1997/06/20 16:36:40  SM02805
 * Modified to implement continuous hct range testing
 *  ( see PPQA Resolutions from 06/19/97 Safety Meeting
 * for details )
 * Revision 1.29  1997/05/24 05:33:55  bs04481
 * Change to limit whole blood processed to 2.2 times TBV.  Calculated when the RBC valve is not in collect and the return pump is not running.
 * Revision 1.28  1997/02/11 18:00:34  bs04481
 * Changes due to enumerated types in the hardware structures
 * Revision 1.27  1996/12/03 19:08:41  TM02109
 * Fix bug in average infusion calculation.
 * Revision 1.26  1996/11/28 00:08:03  SM02805
 * Revision 1.25  1996/10/25 21:25:17  SM02805
 * Modifications to TBV calculations to address mixed units and
 * independant calc of TBV by safety and control.
 * Increase TBV neighborhood to 25.0 ml from 10.0 ml to accomodate
 * differences in calcs between proc and safety by using
 * two different equations taken from Spectra manual for version
 * 4 pgs 1-47.
 * Revision 1.24  1996/10/24 23:09:48  SM02805
 * 1. Modified method for TBV calculation :
 * only use one routine for calculation of TBV using metric
 * units for height and weight.  The routine converts height in
 * inches to cm and weight in lbs to kg if required before calculating
 * TBV.
 * 2. The TBV_COMPARISON _ERROR code was modified to
 * generate the proper alarm if this condition ever occurs ( would
 * have generated a TBV range error if the condition had been detected
 * previous to this change. )
 * Revision 1.23  1996/09/23 17:37:19  SM02805
 * Modify implementation of WB processed during RBC to allow
 * cassette unload when no donor viitals have been entered.
 * Revision 1.22  1996/09/22 18:08:07  SM02805
 * Respond to 09/19/96 code review action items.
 * Revision 1.21  1996/09/17 21:25:08  SM02805
 * 09/17/96 marlene resolution to infusion calculation use return
 * to return cycle for infusion calc.
 * Revision 1.20  1996/09/16 20:22:02  SM02805
 * Revision 1.19  1996/09/16 17:10:26  SM02805
 * Re-activate infusion alarm removed for temporary testing of
 * RB infusion monitoring.
 * Revision 1.18  1996/09/16 16:42:44  SM02805
 * Significantly reduce print instrumentation in source files.
 * Implement _LOG_ERROR processing for cases where
 * data is important for post-run safety analysis.
 * Revision 1.17  1996/09/16 16:42:44  SM02805
 * Pursuant to 09/06/96 review and xfunc resolution :
 * 1. Implement hypovolemia limit for all cases as
 * 15% of TBV - 68.0 ml to account for possible pump
 * tolerance errors during RBC collect ( fill volume
 * may be understated by as much as 17.0 for each
 * fill cycle during RBC coll assume four fill volumes
 * for rbc collect resulting in 68.0 ml worst-case
 * underreporting ).
 * Revision 1.15  1996/08/13 22:47:14  SM02805
 * Commented out some high frequency logging related to hypo testing.
 * Revision 1.14  1996/08/05 15:38:58  tm02109
 * Debug checks added.
 * Revision 1.13  1996/08/02 15:35:08  tm02109
 * Make the infusion alarm go off only if you see more then one high
 * infusion rate.
 * Revision 1.12  1996/08/02 14:35:44  SS03309
 * Disabled AC Infusion Rate - line 271
 * Revision 1.11  1996/07/30 19:46:34  SM02805
 * Revision 1.10  1996/07/30 19:46:34  SM02805
 * Revision 1.9  1996/07/30 03:04:06  SM02805
 * Lint fixes per 07/23/96, 07/26/96 reviews.
 * Revision 1.8  1996/07/24 13:56:55  SM02805
 * Mods to remove absolute paths for include files, includes source
 * code modifications and makefile ( safety.mak) modifications.
 *
 * safety.mak mods use CPPFLAGS , now getting warnings level 3.
 *
 * Sorce code mods to implement donor disconnect test, only partially tested
 * - works as expected when test is passed first time, ie access line
 * properly clamped on first test try...
 * Revision 1.7  1996/07/22 17:13:43  SM02805
 * Update documentation for code review.
 *
 * Corrections to pump monitoring : but for platelet pump due to two magnet rotor.
 *
 * stest.mak target changed to be  saf_exec.
 *
 * TITLE:      s_donor.hpp
 *
 * ABSTRACT:   Interface/Include file for FS Safety System Donor Vital Processing
 *
 *
 * DOCUMENTS
 *
 *
 * Cross Functional Issues :
 *
 * History
 * 06/24/96 create
 *
 */

#ifndef S_DONOR_VITAL_HPP
#define S_DONOR_VITAL_HPP

#include "message.h"

#include "alarms.h"
#include "trima_assert.h"

#include "safproc.h"    // required for exposure of safety/proc common definitons.

#include "s_defs.hpp"
#include "s_glob.hpp"
#include "states.h"    // proc file


/*+++++++++++++++++++++++++++++++++++++++++++++
   Donor Vitals defines : Start
+++++++++++++++++++++++++++++++++++++++++++++*/

// SPECIFICATION:    structure associated with
//                   Message<STBV>
//
// ERROR HANDLING:   none.

// Safety internal values for TBV processing


class aDonor
{
public:
   aDonor();
   ~aDonor() {}

   void SetTBV_liters (float TBV_ml);
   void SetHCT_decimal (float HCT_pcnt);
   void SetECLimit_ml (float ECLimit_ml);
   void SetPltPrecount (float PltPre);
   void SetDonorConnectedState (char cDonorConnectedState);
   void SetDonorVitalStatus (char cDonorVitalStatus);
   void SetDonorVitalData (SDonorVitals donorVital);
   void GetDonorVitalData (SDonorVitals& donorVital);
   void SetTBVLockOut (bool bTBVLockOut);
   void initializeForState (long newState);
   void updateToPFR ();
   void restoreDonorDataFromPFR ();
   bool DonorVitalsOK ();

   float GetTBV_liters (void) const { return( _fSafetyDonorTBV_liters ); }
   float GetHCT_decimal (void) const { return( _fDonorHct_decimal ); }
   float GetECLimit_ml (void) const { return( _fECLimit_ml ); }
   char  GetDonorConnectedState (void) const { return( _cDonorConnectedState ); }
   char  GetDonorVitalStatus (void) const { return( _cDonorVitalStatus ); }
   bool  GetTBVLockOut (void) const { return( _bTBVLockOut ); }

private:
   aDonor(aDonor const&);                   // not implemented
   aDonor& operator = (aDonor const&);      // not implemented

private:
   SDonorVitals _structSDonorVitals;

   char         _cDonorVitalStatus;
   char         _cDonorConnectedState;
   bool         _bTBVLockOut;

   float        _fSafetyDonorTBV_liters;
   float        _fECLimit_ml;
   float        _fDonorHct_decimal;
   float        _fDonorPltPrecount;
   float        _fDonorPlasmaFraction;

}; // end aDonor class definition





// SPECIFICATION:    "rcvr" for proc to safety Donor Vital messages.
//
// NOTES :
//
// ERROR HANDLING:   none.

class aProcToSafetyDonorVitalsMsg_rcvr
{
public:
   aProcToSafetyDonorVitalsMsg_rcvr(aDonor* paDonor_init);
   ~aProcToSafetyDonorVitalsMsg_rcvr();

   void  notify ();
   void  validate ();
   void  validateWeight ();
   void  validateHeight ();
   void  validateGenderAndTBV ();
   void  compareTBV ();
   void  checkAndAlarm ();
   float calcTBV_CM_KG (void);
   void  initDonorVitals (SDonorVitals* pstructDonorVitals);

private:
   aProcToSafetyDonorVitalsMsg_rcvr();                                                       // not implemented
   aProcToSafetyDonorVitalsMsg_rcvr(aProcToSafetyDonorVitalsMsg_rcvr const&);                // not implemented
   aProcToSafetyDonorVitalsMsg_rcvr& operator = (aProcToSafetyDonorVitalsMsg_rcvr const&);   // not implemented

private:
   SDonorVitals      _structSDonorVitals;
   DVitalsFromSafety _safetyToGuiDvitalsMsg;
   DVitalsFromProc   _theProcToSafetyDonorVitalsMsg;
   aDonor*           _paDonor;

   char              _cWeightRangeValid;
   char              _cHeightRangeValid;
   char              _cSexValid;
   char              _cTBVRangeValid;
   char              _cWeightUnitsValid;
   char              _cHeightUnitsValid;
   float             _fTempDonorTBV;

}; // END aProcToSafetyDonorVitalsMsg_rcvr class definition


#endif // S_DONOR_VITAL_HPP

/* FORMAT HASH e1a385b6781a160b82b65f1ed57ce713 */
